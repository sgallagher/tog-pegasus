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
// Modified By: Mike Day (mdday@us.ibm.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include "IPC.h"

#if defined(PEGASUS_OS_TYPE_WINDOWS)
# include "IPCWindows.cpp"
#elif defined(PEGASUS_OS_TYPE_UNIX)
# include "IPCUnix.cpp"
#else
# error "Unsupported platform"
#endif

PEGASUS_NAMESPACE_BEGIN

const Uint32 PEG_SEM_READ = 1 ;
const Uint32 PEG_SEM_WRITE = 2 ;

//-----------------------------------------------------------------
/// Generic Implementation of read/write semaphore class
//-----------------------------------------------------------------
#ifndef PEGASUS_READWRITE_NATIVE 

ReadWriteSem::ReadWriteSem(void) : _readers(0), _writers(0)
{
   _rwlock._internal_lock = Mutex();
    _rwlock._wlock = Mutex();
   _rwlock._rlock= Semaphore(10); // allow 10 concurrent readers 
   _rwlock._owner = pegasus_thread_self();
}

ReadWriteSem::~ReadWriteSem(void)
{
   
   // lock everyone out of this object
   try 
   {
      _rwlock._internal_lock.lock(pegasus_thread_self());
   }
   catch(Deadlock& d) 
   {
      // no problem - we own the lock, which is what we want
   }
   catch(IPCException& e) 
   {
      // oops - this is really bad
      PEGASUS_ASSERT(0);
   }
   while(_readers > 0 || _writers > 0) 
   {
      pegasus_yield();
   }
   _rwlock._wlock.~Mutex();
   _rwlock._rlock.~Semaphore();
   _rwlock._internal_lock.unlock();
   _rwlock._internal_lock.~Mutex();
}

//-----------------------------------------------------------------
// if milliseconds == -1, wait indefinately
// if milliseconds == 0, fast wait 
//-----------------------------------------------------------------
void ReadWriteSem::timed_wait(Uint32 mode, PEGASUS_THREAD_TYPE caller, int milliseconds) 
   throw(TimeOut, Deadlock, Permission, WaitFailed, TooManyReaders)
{

//-----------------------------------------------------------------
// Lock this object to maintain integrity while we decide 
// exactly what to do next.
//-----------------------------------------------------------------
   if(milliseconds == 0)
      _rwlock._internal_lock.lock(pegasus_thread_self()); 
   else if(milliseconds == -1)
      _rwlock._internal_lock.try_lock(pegasus_thread_self());
   else
      _rwlock._internal_lock.timed_lock(milliseconds, pegasus_thread_self());

   if(mode == PEG_SEM_WRITE)
   {
//-----------------------------------------------------------------
// Write Lock Step 1: lock the object and allow all the readers to exit
//-----------------------------------------------------------------

      if(milliseconds == 0) // fast wait
      {
	 if(_rwlock._rlock.count() > 0)
	 {
	    _rwlock._internal_lock.unlock();
	    throw(WaitFailed(pegasus_thread_self()));
	 }
      }
      else if(milliseconds == -1) // infinite wait
      {
	 while(_rwlock._rlock.count() > 0 )
	    pegasus_yield();
      }
      else // timed wait 
      {
	 struct timeval start, now;
	 pegasus_gettimeofday(&start);
	 now.tv_sec = start.tv_sec;
	 now.tv_usec = start.tv_usec;
	 
	 while(_rwlock._rlock.count() > 0)
	 {
	    if((now.tv_usec - start.tv_usec) > (1000 * milliseconds))
	    {
	       _rwlock._internal_lock.unlock();
	       throw(TimeOut(pegasus_thread_self()));
	    }
	    pegasus_yield();
	 }
      }
//-----------------------------------------------------------------
// Write Lock Step 2: Obtain the Write Mutex
//  Although there are no readers, there may be a writer
//-----------------------------------------------------------------
      if(milliseconds == 0) // fast wait
      {
	 try 
	 {
	    _rwlock._wlock.try_lock(pegasus_thread_self());
	 }
	 catch(IPCException e) 
	 {
	    _rwlock._internal_lock.unlock();
	    throw;
	 }
      }
      else if(milliseconds == -1) // infinite wait
      {
	 try 
	 {
	    _rwlock._wlock.lock(pegasus_thread_self());
	 }
	 catch (IPCException& e) 
	 {
	    _rwlock._internal_lock.unlock();
	    throw;
	 }
      }

      else // timed wait
      {
	 try
	 {
	    _rwlock._wlock.timed_lock(milliseconds, pegasus_thread_self());
	 }
	 catch(IPCException& e)
	 {
	    _rwlock._internal_lock.unlock();
	 }
      }
      
//-----------------------------------------------------------------      
// Write Lock Step 3: set the writer count to one, unlock the object
//   There are no readers and we are the only writer !
//-----------------------------------------------------------------      
      _writers = 1;
      // set the owner
      _rwlock._owner = pegasus_thread_self();
      // unlock the object
      _rwlock._internal_lock.unlock();
   } // PEG_SEM_WRITE
   else
   {
//-----------------------------------------------------------------
// Read Lock Step 1: Wait for the existing writer (if any) to clear
//-----------------------------------------------------------------
      if(milliseconds == 0) // fast wait
      {
	 if(_writers > 0)
	 {
	    _rwlock._internal_lock.unlock();
	    throw(WaitFailed(pegasus_thread_self()));
	 }
      }
      else if(milliseconds == -1) // infinite wait
      {
	 while(_writers > 0)
	    pegasus_yield(); 
      }
      else // timed wait
      {
	 struct timeval start, now;
	 pegasus_gettimeofday(&start);
	 now.tv_sec = start.tv_sec;
	 now.tv_usec = start.tv_usec;
	 
	 while(_writers > 0)
	 {
	    if((now.tv_usec - start.tv_usec) > (1000 * milliseconds))
	    {
	       _rwlock._internal_lock.unlock();
	       throw(TimeOut(pegasus_thread_self()));
	    }
	    pegasus_yield();
	 }
      }
      
//-----------------------------------------------------------------
// Read Lock Step 2: wait for a reader slot to open up, then return
//  At this point there are no writers, but there may be too many
//  readers.
//-----------------------------------------------------------------
      if(milliseconds == 0) // fast wait
      {
	 try 
	 {
	    _rwlock._rlock.try_wait();  
	 }
	 catch(IPCException& e) 
	 {
	    // the wait failed, there must be too many readers already. 
	    // unlock the object
	    _rwlock._internal_lock.unlock();
	    throw(TooManyReaders(pegasus_thread_self()));
	 }
      }
      else if(milliseconds == -1) // infinite wait
      {
	 _rwlock._rlock.wait(); // does not throw an exception
      }
      else // timed wait
      {
	 try 
	 {
	    _rwlock._rlock.time_wait(milliseconds);
	 }
	 catch(IPCException& e)
	 {
	    _rwlock._internal_lock.unlock();
	    throw;
	 }
      }
      
//-----------------------------------------------------------------      
// Read Lock Step 3: increment the number of readers, unlock the object, 
// return
//-----------------------------------------------------------------
      _readers++;
      _rwlock._internal_lock.unlock();
   }
   return;
}

void ReadWriteSem::wait(Uint32 mode, PEGASUS_THREAD_TYPE caller) 
   throw(Deadlock, Permission, WaitFailed, TooManyReaders)
{
   timed_wait(mode, caller, -1);
}

void ReadWriteSem::try_wait(Uint32 mode, PEGASUS_THREAD_TYPE caller) 
   throw(Deadlock, Permission, WaitFailed, TooManyReaders)
{
   timed_wait(mode, caller, 0);
}

void ReadWriteSem::unlock(Uint32 mode, PEGASUS_THREAD_TYPE caller) 
   throw(Permission)
{
   if(mode == PEG_SEM_WRITE)
   {
      _writers = 0;
      _rwlock._wlock.unlock();
   }
   else
   {
      _readers--;
      _rwlock._rlock.signal();
   }
   
}

#endif // ! PEGASUS_READWRITE_NATIVE
//-----------------------------------------------------------------
// END of generic read/write semaphore implementaion
//-----------------------------------------------------------------



//-----------------------------------------------------------------
/// Generic Implementation of Atomic Integer class
//-----------------------------------------------------------------

#ifndef PEGASUS_ATOMIC_INT_NATIVE

AtomicInt::AtomicInt() {_rep._value = 0; _rep._mutex = Mutex(); }

AtomicInt::AtomicInt(Uint32 initial) {_rep._value = initial;  _rep._mutex = Mutex() ; }

AtomicInt::~AtomicInt() { _rep._mutex.unlock(); }

AtomicInt::AtomicInt(const AtomicInt& original)
{
    _rep._mutex = Mutex();
    _rep._value = original._rep._value;
} 

AtomicInt& AtomicInt::operator=(const AtomicInt& original )
{
    // to avoid deadlocks, always be certain to only hold one mutex at a time. 
    // therefore, get the original value (which will lock and unlock the original's mutex)
    // and _then_ lock this mutex. This pattern is repeated throughout the class
  
    Uint32 temp = original._rep._value;
    _rep._mutex.lock(pegasus_thread_self());
    _rep._value = temp;
    _rep._mutex.unlock();
    return *this;
}

AtomicInt& AtomicInt::operator=(Uint32 val)
{
    _rep._mutex.lock(pegasus_thread_self());
    _rep._value = val;
    _rep._mutex.unlock();
    return *this;
}

Uint32 AtomicInt::value()
{
    _rep._mutex.lock(pegasus_thread_self());
    Uint32 retval = _rep._value;
    _rep._mutex.unlock();
    return retval;
}

void AtomicInt::operator++(void)
{
    _rep._mutex.lock(pegasus_thread_self());
    _rep._value++;
    _rep._mutex.unlock();
}


void AtomicInt::operator--(void)
{
    _rep._mutex.lock(pegasus_thread_self());
    _rep._value--;
    _rep._mutex.unlock();
}

Uint32 AtomicInt::operator+(const AtomicInt& val)
{
    // never acquire a mutex while holding a mutex 
    Uint32 retval = val._rep._value; 
    _rep._mutex.lock(pegasus_thread_self());
    retval += _rep._value ;
    _rep._mutex.unlock();
    return retval;
}

Uint32 AtomicInt::operator+(Uint32 val)
{
    _rep._mutex.lock(pegasus_thread_self());
    Uint32 retval = _rep._value + val;
    _rep._mutex.unlock();
    return retval;
}

Uint32 AtomicInt::operator-(const AtomicInt& val)
{
    // never acquire a mutex while holding a mutex
    Uint32 retval =  val._rep._value;
    _rep._mutex.lock(pegasus_thread_self());
    retval += _rep._value;
    _rep._mutex.unlock();
    return retval;
}

Uint32 AtomicInt::operator-(Uint32 val)
{
    _rep._mutex.lock(pegasus_thread_self());
    Uint32 retval = _rep._value - val;
    _rep._mutex.unlock();
    return retval;
}


AtomicInt& AtomicInt::operator+=(const AtomicInt& val)
{
    // never acquire a mutex while holding a mutex
    Uint32 temp = val._rep._value;
    _rep._mutex.lock(pegasus_thread_self());
    _rep._value += temp;
    _rep._mutex.unlock();
    return *this;
}
AtomicInt& AtomicInt::operator+=(Uint32 val)
{
    // never acquire a mutex while holding a mutex
    _rep._mutex.lock(pegasus_thread_self());
    _rep._value += val;
    _rep._mutex.unlock();
    return *this;
}

AtomicInt& AtomicInt::operator-=(const AtomicInt& val)
{
    // never acquire a mutex while holding a mutex
    Uint32 temp = val._rep._value;
    _rep._mutex.lock(pegasus_thread_self());
    _rep._value -= temp;
    _rep._mutex.unlock();
    return *this;
}

AtomicInt& AtomicInt::operator-=(Uint32 val)
{
    // never acquire a mutex while holding a mutex
    _rep._mutex.lock(pegasus_thread_self());
    _rep._value -= val;
    _rep._mutex.unlock();
    return *this;
}

#endif // ! PEGASUS_ATOMIC_INT_NATIVE
//-----------------------------------------------------------------
// END of generic atomic integer implementation
//-----------------------------------------------------------------



//-----------------------------------------------------------------
// Generic implementation of conditional semaphore object 
//-----------------------------------------------------------------
#ifndef PEGASUS_CONDITIONAL_NATIVE



#endif // PEGASUS_CONDITIONAL_NATIVE
//-----------------------------------------------------------------
// END of generic implementation of conditional semaphore object 
//-----------------------------------------------------------------

PEGASUS_NAMESPACE_END

// waiter: 
// get the mutex
// conditioned wait (releases the mutex)

// signaler:
// get the mutex (check for deadlock)
// signal the resource 
// release the mutex 


//   Consider two shared variables X and Y, protected by the mutex MUT,
// and a condition variable COND that is to be signaled whenever X becomes
// greater than Y.
//
//      int x,y;
//      pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
//      pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
//
//    Waiting until X is greater than Y is performed as follows:
//
//      pthread_mutex_lock(&mut);
//      while (x <= y) {
//              pthread_cond_wait(&cond, &mut);
//      }
//      /* operate on x and y */
//      pthread_mutex_unlock(&mut);
//
//    Modifications on X and Y that may cause X to become greater than Y
// should signal the condition if needed:
//
//      pthread_mutex_lock(&mut);
//      /* modify x and y */
//      if (x > y) pthread_cond_broadcast(&cond);
//      pthread_mutex_unlock(&mut);
//
//    If it can be proved that at most one waiting thread needs to be waken
// up (for instance, if there are only two threads communicating through X
// and Y), `pthread_cond_signal' can be used as a slightly more efficient
// alternative to `pthread_cond_broadcast'. In doubt, use
// `pthread_cond_broadcast'.
//
//    To wait for X to becomes greater than Y with a timeout of 5 seconds,
// do:
//
//      struct timeval now;
//      struct timespec timeout;
//      int retcode;
//     
//      pthread_mutex_lock(&mut);
//      gettimeofday(&now);
//      timeout.tv_sec = now.tv_sec + 5;
//      timeout.tv_nsec = now.tv_usec * 1000;
//      retcode = 0;
//      while (x <= y && retcode != ETIMEDOUT) {
//              retcode = pthread_cond_timedwait(&cond, &mut, &timeout);
//      }
//      if (retcode == ETIMEDOUT) {
//              /* timeout occurred */
//      } else {
//              /* operate on x and y */
//      }
//      pthread_mutex_unlock(&mut);
//
