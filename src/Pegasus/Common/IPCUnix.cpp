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

#include <sys/timex.h>

PEGASUS_NAMESPACE_BEGIN

inline void pegasus_yield(void)
{
#ifdef PEGASUS_PLATFORM_HPUX_PARISC_ACC
      sched_yield();
#else
      pthread_yield();
#endif
}

//-----------------------------------------------------------------
/// accurate version of gettimeofday for unix systems
//  posix glibc implementation does not return microseconds.
//  mdday Wed Aug  1 16:05:26 2001
//-----------------------------------------------------------------
static int pegasus_gettimeofday(struct timeval *tv)
{
   struct ntptimeval ntp;
   int err;
   if(tv == NULL)
      return(EINVAL);
   err = ntp_gettime(&ntp);
   tv->tv_sec = ntp.time.tv_sec;
   tv->tv_usec = ntp.time.tv_usec;
   return(err);
}

// If  pegasus_gettimeofday() does not compile, comment it 
// out and uncomment the following line
//static inline int pegasus_gettimeofday(struct timeval *tv) { return(gettimeofday(tv, NULL)); }


   


PEGASUS_THREAD_TYPE pegasus_thread_self(void) 
{ 
   return(pthread_self());
}

Mutex::Mutex()
{
   pthread_mutexattr_init(&_mutex.mutatt);
   pthread_mutexattr_settype(&_mutex.mutatt,PTHREAD_MUTEX_ERRORCHECK);
   pthread_mutex_init(&_mutex.mut,&_mutex.mutatt);
   _mutex.owner = 0;
}

Mutex::Mutex(int mutex_type)
{
   pthread_mutexattr_init(&_mutex.mutatt);
   pthread_mutexattr_settype(&_mutex.mutatt, mutex_type);
   pthread_mutex_init(&_mutex.mut,&_mutex.mutatt);
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

// block until gaining the lock - throw a deadlock 
// exception if process already holds the lock 
void Mutex::lock(PEGASUS_THREAD_TYPE caller) throw(Deadlock, WaitFailed)
{
   int errorcode;
   if( 0 == (errorcode = pthread_mutex_lock(&(_mutex.mut)))) 
   {
      _mutex.owner = caller;
      return;
   }
   else if (errorcode == EDEADLK) 
      throw( Deadlock( _mutex.owner ) );
   else 
      throw( WaitFailed( _mutex.owner) );
}
  
// try to gain the lock - lock succeeds immediately if the 
// mutex is not already locked. throws an exception and returns
// immediately if the mutex is currently locked. 
void Mutex::try_lock(PEGASUS_THREAD_TYPE caller) throw(Deadlock, AlreadyLocked, WaitFailed)
{
   int errorcode ;
   if(0 == (errorcode = pthread_mutex_trylock(&_mutex.mut))) 
   {
      _mutex.owner = caller;
      return;
   }
   else if (errorcode == EBUSY) 
      throw(AlreadyLocked(_mutex.owner));
   else if (errorcode == EDEADLK) 
      throw(Deadlock(_mutex.owner));
   else
      throw(WaitFailed(_mutex.owner));
}

// wait for milliseconds and throw an exception then return if the wait
// expires without gaining the lock. Otherwise return without throwing an
// exception. 
void Mutex::timed_lock( Uint32 milliseconds , PEGASUS_THREAD_TYPE caller) throw(Deadlock, TimeOut, WaitFailed)
{

   struct timeval now;
   struct timespec timeout;
   int errorcode;
  
//   gettimeofday(&now,NULL);
   pegasus_gettimeofday(&now);
   
   timeout.tv_sec = now.tv_sec;
   timeout.tv_nsec += now.tv_usec * 1000 + milliseconds; 
   if(0 == (errorcode = pthread_mutex_timedlock(&_mutex.mut, &timeout)))
   {
      _mutex.owner = caller;
      return;
   }
   else if (errorcode == ETIMEDOUT)
      throw(TimeOut(_mutex.owner));
   else if (errorcode == EDEADLK)
      throw(Deadlock(_mutex.owner));
   else
      throw(WaitFailed(_mutex.owner));
}

// unlock the mutex
void Mutex::unlock() throw(Permission)
{
   if(0 != pthread_mutex_unlock(&_mutex.mut))
      throw(Permission(_mutex.owner));
}


#ifdef PEGASUS_READWRITE_NATIVE 
//-----------------------------------------------------------------
/// Native Implementation of Read/Write semaphore
//-----------------------------------------------------------------

// ReadWriteSemaphore are best implemented through Unix 98 rwlocks

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

void ReadWriteSem::timed_wait(Uint32 mode, PEGASUS_THREAD_TYPE caller, int milliseconds) 
   throw(TimeOut, Deadlock, Permission, WaitFailed, TooManyReaders)
{
   struct timeval now;
   struct timespec timeout;
   int errorcode = 0;

//   gettimeofday(&now,NULL);
   pegasus_gettimeofday(&now);
   timeout.tv_sec = now.tv_sec;
   timeout.tv_nsec += now.tv_usec * 1000 + milliseconds; 
   
   if (mode == PEG_SEM_READ)
   {
      if(0 == (errorcode = pthread_rwlock_timedrdlock(&_rwlock.rwlock,
						      &timeout)))
      {
	 _readers++;
	 return;
      }
   }
   else if (mode == PEG_SEM_WRITE)
   {
      if(0 == (errorcode = pthread_rwlock_timedwrlock(&_rwlock.rwlock,
						      &timeout)))
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
   if(0 != pthread_rwlock_unlock(&_rwlock.rwlock))
      throw(Permission(pthread_self()));
   if(mode == PEG_SEM_READ)
      _readers--;
   else if (mode == PEG_SEM_WRITE)
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



/// Conditions are implemented as process-wide condition variables
Condition::Condition() 
{
#ifdef PEGASUS_PLATFORM_HPUX_PARISC_ACC
   // ATTN - HP-UX pthread library does not define PTHREAD_MUTEX_TIMED_NP
   _cond_mutex = Mutex();
#else
   _cond_mutex = Mutex(PTHREAD_MUTEX_TIMED_NP);
#endif
#ifdef PEGASUS_PLATFORM_HPUX_PARISC_ACC
   // ATTN - HP-UX can not deal with the non-static structure assignment
   // Also, the (PEGASUS_COND_TYPE) cast seems to break the HP-UX compile
   PEGASUS_COND_TYPE tmpCond = PTHREAD_COND_INITIALIZER;
   memcpy(&_condition, &tmpCond, sizeof(PEGASUS_COND_TYPE));
#else
   _condition = (PEGASUS_COND_TYPE) PTHREAD_COND_INITIALIZER;
#endif
   _owner = 0;
}

Condition::~Condition()
{
   while(EBUSY == pthread_cond_destroy(&_condition))
   {
      pegasus_yield();
   }
   
   _cond_mutex.~Mutex();
}

// block until this semaphore is in a signalled state 
void Condition::wait(PEGASUS_THREAD_TYPE caller) throw(Deadlock, WaitFailed)
{
   try { _cond_mutex.lock(caller); }
   catch(...) { throw; }

   pthread_cond_wait(&_condition, _cond_mutex.getMutex());
   _cond_mutex.unlock();
}

void Condition::signal(PEGASUS_THREAD_TYPE caller) throw(Deadlock, WaitFailed)
{
   try { _cond_mutex.lock(caller); }
   catch(...) { throw; }
   pthread_cond_broadcast(&_condition);
   _cond_mutex.unlock();
}

// wait for milliseconds and throw an exception
// if wait times out without gaining the semaphore
void Condition::time_wait( Uint32 milliseconds, PEGASUS_THREAD_TYPE caller ) throw(TimeOut, Deadlock, WaitFailed)
{
   struct timeval now;
   struct timespec timeout;
   int retcode;

   try { _cond_mutex.lock(caller); }
   catch(...) { throw; }
   do 
   { 
//      gettimeofday(&now,NULL);
      pegasus_gettimeofday(&now);
      timeout.tv_sec = now.tv_sec;
      timeout.tv_nsec += now.tv_usec * 1000 + milliseconds; 
      retcode = pthread_cond_timedwait(&_condition, _cond_mutex.getMutex(), &timeout);
   } while( retcode == EINTR ) ;
    
   _cond_mutex.unlock();
   if( retcode)
      throw(TimeOut(caller)) ;
}

// block until this semaphore is in a signalled state 
void Condition::unlocked_wait(PEGASUS_THREAD_TYPE caller) 
{
   pthread_cond_wait(&_condition, _cond_mutex.getMutex());
}

// block until this semaphore is in a signalled state 
void Condition::unlocked_timed_wait(int milliseconds, PEGASUS_THREAD_TYPE caller) throw(TimeOut)
{
   struct timeval now;
   struct timespec timeout;
   int retcode;
  
   do
   {
//      gettimeofday(&now,NULL);
      pegasus_gettimeofday(&now);
      timeout.tv_sec = now.tv_sec;
      timeout.tv_nsec += now.tv_usec * 1000 + milliseconds; 
      retcode = pthread_cond_timedwait(&_condition, _cond_mutex.getMutex(), &timeout) ;
   } while ( retcode == EINTR ) ;
   if(retcode)
      throw(TimeOut(caller));
}

void Condition::unlocked_signal(PEGASUS_THREAD_TYPE caller) 
{
   // force the caller to own the conditional mutex 
   pthread_cond_broadcast(&_condition);
}

//-----------------------------------------------------------------//

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

// block until this semaphore is in a signalled state
void Semaphore::wait(void) 
{
   sem_wait(&_semaphore.sem);
}

// wait succeeds immediately if semaphore has a non-zero count, 
// return immediately and throw and exception if the 
// count is zero. 
void Semaphore::try_wait(void) throw(WaitFailed)
{
   if (sem_trywait(&_semaphore.sem)) 
      throw(WaitFailed(_semaphore.owner));
}

// wait for milliseconds and throw an exception
// if wait times out without gaining the semaphore
void Semaphore::time_wait( Uint32 milliseconds ) throw(TimeOut)
{
   struct timeval now;
   struct timespec timeout;
   int retcode;
  
   do
   {
//      gettimeofday(&now,NULL);
      pegasus_gettimeofday(&now);
      timeout.tv_sec = now.tv_sec;
      timeout.tv_nsec += now.tv_usec * 1000 + milliseconds; 
      retcode = sem_timedwait(&_semaphore.sem, &timeout);
   } while (retcode != EINTR) ;
   if(retcode)
      throw(TimeOut(_semaphore.owner));
}

// increment the count of the semaphore 
void Semaphore::signal()
{
   sem_post(&_semaphore.sem);
}

// return the count of the semaphore
int Semaphore::count() 
{
   sem_getvalue(&_semaphore.sem,&_count);
   return _count;
}



//-----------------------------------------------------------------
/// Native Unix implementation of AtomicInt class
//-----------------------------------------------------------------
#if defined(PEGASUS_ATOMIC_INT_NATIVE)

AtomicInt::AtomicInt(): _rep(0) { }

AtomicInt::AtomicInt( Uint32 initial) : _rep(initial) {}

AtomicInt::~AtomicInt() {}

AtomicInt::AtomicInt(const AtomicInt& original) 
{
   _rep = original._rep;
}

AtomicInt& AtomicInt::operator=( const AtomicInt& original)
{
   // don't worry about self-assignment in this implementation
   if(this != &original)
      _rep = original._rep;
   return *this;
}


Uint32 AtomicInt::value(void)
{
   return((Uint32)_rep);
}

void AtomicInt::operator++(void) { _rep++; }
void AtomicInt::operator--(void) { _rep--; }

Uint32 AtomicInt::operator+(const AtomicInt& val) { return((Uint32)(_rep + val._rep));}
Uint32 AtomicInt::operator+(Uint32 val) { return( (Uint32)(_rep + val)); }

Uint32 AtomicInt::operator-(const AtomicInt& val) { return((Uint32)(_rep - val._rep));}
Uint32 AtomicInt::operator-(Uint32 val) { return((Uint32)(_rep - val)); }

AtomicInt& AtomicInt::operator+=(const AtomicInt& val) { _rep += val._rep; return *this; }
AtomicInt& AtomicInt::operator+=(Uint32 val) { _rep += val; return *this; }
AtomicInt& AtomicInt::operator-=(const AtomicInt& val) { _rep -= val._rep; return *this; }
AtomicInt& AtomicInt::operator-=(Uint32 val) { _rep -= val; return *this; }

#endif // Native Atomic Type 

PEGASUS_NAMESPACE_END
