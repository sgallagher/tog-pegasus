//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 The Open group, BMC Software, Tivoli Systems, IBM
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to 
// deal in the Software without restriction, including without limitation the 
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or 
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// THE ABOVE COPYRIGHT NOTICE AND THIS PERMISSION NOTICE SHALL BE INCLUDED IN 
// ALL COPIES OR SUBSTANTIAL PORTIONS OF THE SOFTWARE. THE SOFTWARE IS PROVIDED
// "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
// LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
// PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN 
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//==============================================================================
//
// Author: Markus Mueller (sedgewick_de@yahoo.de)
//
// Modified By: Mike Day (mdday@us.ibm.com) -- added native implementation
// of AtomicInt class, exceptions
//
//%/////////////////////////////////////////////////////////////////////////////



PEGASUS_NAMESPACE_BEGIN



Mutex::Mutex()
{
   pthread_mutex_init(&_mutex.mut, NULL);
   _mutex.owner = 0;
}

Mutex::Mutex(int mutex_type) 
{
   pthread_mutexattr_init(&_mutex.mutatt);
   pthread_mutexattr_settype(&_mutex.mutatt, mutex_type);
   pthread_mutex_init(&_mutex.mut,&_mutex.mutatt);
   _mutex.owner = 0;
}

// to be able share the mutex between different condition variables
Mutex::Mutex(const Mutex& mutex)
{
   // only copy the handle, not the entire object. 
   // avoid calling the destructor twice. 
   _mutex.mut  = mutex._mutex.mut;
   _mutex.owner = 0;
}

Mutex::~Mutex()
{
   while( EBUSY == pthread_mutex_destroy(&_mutex.mut))
   {
      pegasus_yield();
   }
   pthread_mutexattr_destroy(&_mutex.mutatt);
}



#ifdef PEGASUS_READWRITE_NATIVE 
//-----------------------------------------------------------------
/// Native Implementation of Read/Write semaphore
//-----------------------------------------------------------------


ReadWriteSem::ReadWriteSem(void) :  _readers(0), _writers(0) 
{
   pthread_rwlock_init(&_rwlock.rwlock, NULL);
   _rwlock.owner = 0;
}
    
ReadWriteSem::~ReadWriteSem(void)
{

   while( EBUSY == pthread_rwlock_destroy(&_rwlock.rwlock))
   {
      pegasus_yield();
   }
}


void ReadWriteSem::wait(Uint32 mode, PEGASUS_THREAD_TYPE caller) 
   throw(Deadlock, Permission, WaitFailed, TooManyReaders)

{
   int errorcode;
   if (mode == PEG_SEM_READ) 
   {
      if(0 == (errorcode = pthread_rwlock_rdlock(&_rwlock.rwlock)))
      {
	 _readers++;
	 return;
      }
   }
   else if (mode == PEG_SEM_WRITE)
   {
      if( 0 == (errorcode = pthread_rwlock_wrlock(&_rwlock.rwlock)))
      {
	 _rwlock.owner = caller;
	 _writers++;
	 return;
      }
   }
   else 
      throw(Permission(pthread_self()));
    
   if (errorcode == EDEADLK)
      throw(Deadlock(_rwlock.owner));
   else
      throw(WaitFailed(pthread_self()));
}

void ReadWriteSem::try_wait(Uint32 mode, PEGASUS_THREAD_TYPE caller) 
   throw(Deadlock, Permission, WaitFailed, TooManyReaders)
{
   int errorcode = 0;
   if (mode == PEG_SEM_READ) 
   {
      if( 0 == (errorcode = pthread_rwlock_tryrdlock(&_rwlock.rwlock)))
      {
	 _readers++;
	 return;
      }
   }
   else if (mode == PEG_SEM_WRITE) 
   {
      if(0 == (errorcode = pthread_rwlock_trywrlock(&_rwlock.rwlock)))
      {
	 _writers++;
	 _rwlock.owner = caller;
	 return;
      }
   }
   else 
      throw(Permission(pthread_self()));

   if (errorcode == EBUSY)
      throw(AlreadyLocked(_rwlock.owner));
   else if (errorcode == EDEADLK)
      throw(Deadlock(_rwlock.owner));
   else
      throw(WaitFailed(pthread_self()));
}


// timedrdlock and timedwrlock are not supported on HPUX
// mdday Sun Aug  5 14:21:00 2001
void ReadWriteSem::timed_wait(Uint32 mode, PEGASUS_THREAD_TYPE caller, int milliseconds) 
   throw(TimeOut, Deadlock, Permission, WaitFailed, TooManyReaders)
{
   struct timeval start, now;
   int errorcode = 0;

   gettimeofday(&start, NULL);
   start.tv_usec += (milliseconds * 1000);
   
   if (mode == PEG_SEM_READ)
   {
      do
      {
	 errorcode = pthread_rwlock_tryrdlock(&_rwlock.rwlock);
	 gettimeofday(&now, NULL);
      }
      while (errorcode == EBUSY && 
	     ((now.tv_usec < start.tv_usec) || (now.tv_sec <= start.tv_sec)));
      if(0 == errorcode)
      {
	 _readers++;
	 return;
      }
   }
   else if (mode == PEG_SEM_WRITE)
   {
      do
      {
	 errorcode = pthread_rwlock_trywrlock(&_rwlock.rwlock);
	 gettimeofday(&now, NULL);
      }
      while(errorcode == EBUSY && 
	    ((now.tv_usec < start.tv_usec) || (now.tv_sec <= start.tv_sec )));
      // ATTN: RK - Added ');' to the end of the preceding line.  Is this
      // "while" condition correct?

      if(0 == errorcode)
      {
	 _writers++;
	 _rwlock.owner = caller;
	 return;
      }
   }
   else
      throw(Permission(pthread_self()));
   if (errorcode == ETIMEDOUT)
      throw(TimeOut(_rwlock.owner));
   else if (errorcode == EDEADLK)
      throw(Deadlock(_rwlock.owner));
   else
      throw(WaitFailed(pthread_self()));
}

void ReadWriteSem::unlock(Uint32 mode, PEGASUS_THREAD_TYPE caller) throw(Permission)
{
   PEGASUS_THREAD_TYPE owner;

   if (mode == PEG_SEM_WRITE)
   {
      owner = _rwlock.owner;
      _rwlock.owner = 0;
   }
   if(0 != pthread_rwlock_unlock(&_rwlock.rwlock))
   {
      _rwlock.owner = owner;
      throw(Permission(pthread_self()));
   }
   if(mode == PEG_SEM_READ)
      _readers--;
   else 
      _writers--;
}

int ReadWriteSem::read_count()

{
#if defined(PEGASUS_PLATFORM_LINUX_IX86_GNU)
   PEGASUS_ASSERT(_readers ==  _rwlock.rwlock.__rw_readers);
#endif
   return( _readers );
}

int ReadWriteSem::write_count()
{
#if defined(PEGASUS_PLATFORM_LINUX_IX86_GNU)    
   if(_rwlock.rwlock.__rw_writer != NULL) 
   {
      PEGASUS_ASSERT(_writers  == 1); 
   }
#endif
   return( _writers );
}

#endif // PEGASUS_READWRITE_NATIVE 
//-----------------------------------------------------------------
// END of native read/write implementation for unix
//-----------------------------------------------------------------



//-----------------------------------------------------------------
// Native implementation of Conditional semaphore object
//-----------------------------------------------------------------

#ifdef PEGASUS_CONDITIONAL_NATIVE

// Note: I felt uncomfortable exposing the condition mutex outside
// of the class so I defined method calls to lock and unlock the 
// mutex object. This protects the (hidden) conditional mutex from
// being called outside of the control of the object.

// Further, the use model of conditions seems to require locking the object, 
// examining the state of the condition variable while that variable is 
// protected from other threads, and then determining whether to signal or
//    wait on the condition variable. Then afterwards explicitly unlocking
// the condition object. 

// So I commented out the method calls that do all three operations 
// without examining the state of the condition variable. 
// i.e., lock, signal, unlock or lock, wait, unlock. 

//    The method calls I commented out are: wait, signal, time_wait.
// mdday Sun Aug  5 13:19:30 2001

/// Conditions are implemented as process-wide condition variables
Condition::Condition() : _disallow(0)
{
   _cond_mutex = new Mutex();
   _destroy_mut = true;
   pthread_cond_init((PEGASUS_COND_TYPE *)&_condition, 0);

// #ifdef PEGASUS_PLATFORM_HPUX_PARISC_ACC
//    // ATTN - HP-UX can not deal with the non-static structure assignment
//    // Also, the (PEGASUS_COND_TYPE) cast seems to break the HP-UX compile
//    PEGASUS_COND_TYPE tmpCond = PTHREAD_COND_INITIALIZER;
//    memcpy(&_condition, &tmpCond, sizeof(PEGASUS_COND_TYPE));
// #else
//    _condition = (PEGASUS_COND_TYPE) PTHREAD_COND_INITIALIZER;
// #endif
}

//#if defined(PEGASUS_PLATFORM_LINUX_IX86_GNU)
Condition::Condition(const Pegasus::Mutex& mutex)  : _disallow(0)
{
   _cond_mutex = const_cast<Mutex *>(&mutex);
   _destroy_mut = false;
   pthread_cond_init((PEGASUS_COND_TYPE *)&_condition, 0);
}
// #elif defined(PEGASUS_PLATFORM_HPUX_PARISC_ACC)
// Condition::Condition(const Mutex& mutex)  : _disallow(0)
// {
//    _cond_mutex = Mutex(mutex);
//    PEGASUS_COND_TYPE tmpCond = PTHREAD_COND_INITIALIZER;
//    memcpy(&_condition, &tmpCond, sizeof(PEGASUS_COND_TYPE));
// }
// #endif

Condition::~Condition()
{
   _disallow++;
   while(EBUSY == pthread_cond_destroy(&_condition))
   {
      pthread_cond_broadcast(&_condition);
      pegasus_yield();
   }
   if(_destroy_mut == true)
      delete _cond_mutex;
}

#endif // native conditional semaphore
//-----------------------------------------------------------------
// END of native conditional semaphore implementation
//-----------------------------------------------------------------

#ifndef PEGASUS_PLATFORM_ZOS_ZSERIES_IBM
Semaphore::Semaphore(Uint32 initial) 
{
   if(initial > SEM_VALUE_MAX)
      initial = SEM_VALUE_MAX - 1;
   sem_init(&_semaphore.sem,0,initial);
   _semaphore.owner = pthread_self();

}

Semaphore::~Semaphore()
{
   while( EBUSY == sem_destroy(&_semaphore.sem))
   {
      pegasus_yield();
   }
}


#else
//
// implementation as used in ACE derived from Mutex + Condition Variable
//
Semaphore::Semaphore(Uint32 initial) 
{
    pthread_mutex_init (&_semaphore.mutex);
    pthread_cond_init (&_semaphore.cond);
    if (initial > SEM_VALUE_MAX)
         _count = SEM_VALUE_MAX - 1;
    else
         _count = initial;
    _semaphore.owner = pthread_self();
    _semaphore.waiters = 0;
}

Semaphore::~Semaphore()
{
   pthread_mutex_lock(&_semaphore.mutex);
   while( EBUSY == pthread_cond_destroy(&_semaphore.cond))
   {
      pthread_mutex_unlock(&_semaphore.mutex);
      pegasus_yield();
   }
   pthread_mutex_unlock(&_semaphore.mutex);
   pthread_mutex_destroy(&_semaphore.mutex);
}

// block until this semaphore is in a signalled state
void Semaphore::wait(void) 
{
   // Acquire mutex to enter critical section.
   pthread_mutex_lock (&_semaphore.mutex);

   // Keep track of the number of waiters so that <sema_post> works correctly.
   _semaphore.waiters++;

   // Wait until the semaphore count is > 0, then atomically release
   // <lock_> and wait for <count_nonzero_> to be signaled. 
   while (count_ == 0)
      pthread_cond_wait (&_semaphore.count,&_semaphore.mutex);

   // <_semaphore.mutex> is now held.

   // Decrement the waiters count.
   _semaphore.waiters--;

   // Decrement the semaphore's count.
   _count--;

   // Release mutex to leave critical section.
   pthread_mutex_unlock (&_semaphore.mutex);
}

void Semaphore::try_wait(void) throw(WaitFailed)
{
// not implemented
}

void Semaphore::time_wait( Uint32 milliseconds ) throw(TimeOut)
{
// not implemented
}

// increment the count of the semaphore 
void Semaphore::signal()
{
   pthread_mutex_lock (&_semaphore.mutex);

   // Always allow one thread to continue if it is waiting.
   if (_semaphore.waiters > 0)
      pthread_cond_signal (&_semaphore.count);

   // Increment the semaphore's count.
   _count++;

   pthread_mutex_unlock (&_semaphore.mutex);
}

// return the count of the semaphore
int Semaphore::count() 
{
   return _count;
}
#endif



//-----------------------------------------------------------------
/// Native Unix implementation of AtomicInt class
//-----------------------------------------------------------------
//
// sig_atomic_t is identical to a simple integer on Linux with glibc,
// the atomicity refers to access atomicity, i.e. an integer is accessed
// loaded or stored atomically, but incrementing and decrementing is not
// done atomically.
//
//#ifdef PEGASUS_PLATFORM_LINUX_IX86_GNU

#if defined(PEGASUS_ATOMIC_INT_NATIVE)

AtomicInt::AtomicInt() { _rep.counter = 0;}

AtomicInt::AtomicInt( Uint32 initial) {_rep.counter = initial;}

AtomicInt::~AtomicInt() {}

AtomicInt::AtomicInt(const AtomicInt& original) 
{
   _rep.counter = atomic_read(&original._rep);
}



// still missing are atomic test like "subtract and test if zero"
#endif // Native Atomic Type 

PEGASUS_NAMESPACE_END
