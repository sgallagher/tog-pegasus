//%///////-*-c++-*-/////////////////////////////////////////////////////////////
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
// Author: Mike Day (mdday@us.ibm.com)
//
// Modified By: 
//
//%/////////////////////////////////////////////////////////////////////////////


//%// ---- inline implmentations of Unix/Linux IPC routines ---- 

#ifndef IPCUnix_inline_h
#define IPCUnix_inline_h

// the next two routines are macros that MUST SHARE the same stack frame
// they are implemented as macros by glibc. 
// native_cleanup_push( void (*func)(void *) ) ;
// these ALSO SET CANCEL STATE TO DEFER
#define native_cleanup_push( func, arg ) \
   pthread_cleanup_push_defer_np((func), arg)

// native cleanup_pop(Boolean execute) ; 
#define native_cleanup_pop(execute) \
   pthread_cleanup_pop_restore_np(execute)


// block until gaining the lock - throw a deadlock 
// exception if process already holds the lock 
inline void Mutex::lock(PEGASUS_THREAD_TYPE caller) throw(Deadlock, WaitFailed)
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
inline void Mutex::try_lock(PEGASUS_THREAD_TYPE caller) throw(Deadlock, AlreadyLocked, WaitFailed)
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

// Note: I was unable to get the expected behavior using pthread_mutex_timedlock. 
// I don't know excactly why, but the locks were never timing out. Reimplemting
// using pthread_mutex_trylock works reliably. The documentation says that
// pthread_mutex_timedlock works with error checking mutexes but works
// just like pthread_mutex_lock (i.e., it never times out) with other
// kinds of mutexes. I couldn't determine whether or not it actually
// works with any type of mutex other than PTHREAD_MUTEX_TIMED_NP.
// However, we want the mutexes to be error checking whenever possible
// mdday Sun Aug  5 13:08:43 2001

// pthread_mutex_timedlock is not supported on HUPX
// mdday Sun Aug  5 14:12:22 2001

inline void Mutex::timed_lock( Uint32 milliseconds , PEGASUS_THREAD_TYPE caller) 
   throw(Deadlock, TimeOut, WaitFailed)
{

   struct timeval start, now;
   int errorcode;
  
   gettimeofday(&start,NULL);
   start.tv_usec += (milliseconds * 1000);
   
   do 
   {
      errorcode = pthread_mutex_trylock(&_mutex.mut);
      gettimeofday(&now, NULL);
   } while (errorcode == EBUSY && 
	    ((now.tv_usec < start.tv_usec) || (now.tv_sec <= start.tv_sec ))) ;

   if (errorcode)
   {
      throw(TimeOut(_mutex.owner));
   }
}

// unlock the mutex
inline void Mutex::unlock() throw(Permission)
{
   PEGASUS_THREAD_TYPE m_owner = _mutex.owner;
   _mutex.owner = 0;
   if(0 != pthread_mutex_unlock(&_mutex.mut)) 
   {
      _mutex.owner = m_owner;
      throw(Permission(_mutex.owner));
   }
}


//-----------------------------------------------------------------
// Native implementation of Conditional semaphore object
//-----------------------------------------------------------------
 
#ifdef PEGASUS_CONDITIONAL_NATIVE

inline void Condition::signal(PEGASUS_THREAD_TYPE caller) 
   throw(IPCException) 
{ 
   if(_disallow.value() > 0) 
      return;
   _cond_mutex->lock(caller); 
   pthread_cond_broadcast(&_condition);
   _cond_mutex->unlock(); 
}


inline void Condition::unlocked_signal(PEGASUS_THREAD_TYPE caller) 
   throw(IPCException)
{
   if(_cond_mutex->get_owner() != caller)
      throw Permission(_cond_mutex->get_owner());
   pthread_cond_broadcast(&_condition);
   _cond_mutex->_set_owner(caller);
}


inline void Condition::lock_object(PEGASUS_THREAD_TYPE caller)
   throw(IPCException)
{
   if(_disallow.value() > 0) 
      throw ListClosed();
   _cond_mutex->lock(caller);
}

inline void Condition::try_lock_object(PEGASUS_THREAD_TYPE caller)
   throw(IPCException)
{
   if(_disallow.value() > 0) 
      throw ListClosed();
   _cond_mutex->try_lock(caller);
}

inline void Condition::wait_lock_object(PEGASUS_THREAD_TYPE caller, int milliseconds)
   throw(IPCException)
{
   if(_disallow.value() > 0) 
      throw ListClosed();
   _cond_mutex->timed_lock(milliseconds, caller);
}

inline void Condition::unlock_object(void)
{
   _cond_mutex->unlock();
}


// block until this semaphore is in a signalled state 
inline void Condition::unlocked_wait(PEGASUS_THREAD_TYPE caller) 
   throw(IPCException)
{
   if(_disallow.value() > 0) 
   {
      _cond_mutex->unlock();
      throw ListClosed();
   }
   pthread_cond_wait(&_condition, &_cond_mutex->_mutex.mut);
   _cond_mutex->_set_owner(caller);
}

// block until this semaphore is in a signalled state 
inline void Condition::unlocked_timed_wait(int milliseconds, PEGASUS_THREAD_TYPE caller) 
   throw(IPCException)
{
   if(_disallow.value() > 0) 
   {
      _cond_mutex->unlock();
      throw ListClosed();
   }
   struct timeval now;
   int retcode;
   gettimeofday(&now, NULL);
   now.tv_usec += (milliseconds * 1000);
   do
   {
      retcode = pthread_cond_timedwait(&_condition, &_cond_mutex->_mutex.mut, (struct timespec *)&now) ;
   } while ( retcode == EINTR ) ;
   if(retcode)
      throw(TimeOut(caller));
   _cond_mutex->_set_owner(caller);
}

#endif // native conditional 


//-----------------------------------------------------------------
// Native implementation of semaphore object
//-----------------------------------------------------------------

// block until this semaphore is in a signalled state
inline void Semaphore::wait(void) 
{
   sem_wait(&_semaphore.sem);
}

// wait succeeds immediately if semaphore has a non-zero count, 
// return immediately and throw and exception if the 
// count is zero. 
inline void Semaphore::try_wait(void) throw(WaitFailed)
{
   if (sem_trywait(&_semaphore.sem)) 
      throw(WaitFailed(_semaphore.owner));
}


// Note: I could not get sem_timed_wait to work reliably. 
// See my comments above on mut timed_wait. 
// I reimplemented using try_wait, which works reliably. 
// mdd Sun Aug  5 13:25:31 2001

// wait for milliseconds and throw an exception
// if wait times out without gaining the semaphore
inline void Semaphore::time_wait( Uint32 milliseconds ) throw(TimeOut)
{
   struct timeval start, now;
   int retcode;
   gettimeofday(&start,NULL);
   start.tv_usec += (milliseconds * 1000);
   
   do 
   {
      retcode = sem_trywait(&_semaphore.sem);
      gettimeofday(&now, NULL);
   } while (retcode == -1 && 
	    errno == EAGAIN &&
	    (now.tv_usec < start.tv_usec) || ( now.tv_sec <= start.tv_sec)) ;
   
   if(retcode)
      throw(TimeOut(_semaphore.owner));
}

// increment the count of the semaphore 
inline void Semaphore::signal()
{
   sem_post(&_semaphore.sem);
}

// return the count of the semaphore
inline int Semaphore::count() 
{
   sem_getvalue(&_semaphore.sem,&_count);
   return _count;
}


//-----------------------------------------------------------------
/// Native Unix implementation of AtomicInt class
//-----------------------------------------------------------------

#if defined(PEGASUS_ATOMIC_INT_NATIVE)

inline  AtomicInt& AtomicInt::operator=(Uint32 i) 
{
   atomic_set(&_rep, i );
   return *this;
}

inline  AtomicInt& AtomicInt::operator=( const AtomicInt& original)
{
   if(this != &original)
      atomic_set(&_rep,atomic_read(&original._rep));
   return *this;
}

inline Uint32 AtomicInt::value(void)
{
   return((Uint32)atomic_read(&_rep));
}

inline void AtomicInt::operator++(void) { atomic_inc(&_rep); }
inline void AtomicInt::operator--(void) { atomic_dec(&_rep); }
inline void AtomicInt::operator++(int) { atomic_inc(&_rep); }
inline void AtomicInt::operator--(int) { atomic_dec(&_rep); }


inline Uint32 AtomicInt::operator+(const AtomicInt& val)
{
     return((Uint32)(atomic_read(&_rep) + atomic_read(&val._rep) ));
}

inline Uint32 AtomicInt::operator+(Uint32 val)
{ 
    return( (Uint32)(atomic_read(&_rep) + val));
}

inline Uint32 AtomicInt::operator-(const AtomicInt& val)
{
     return((Uint32)(atomic_read(&_rep) - atomic_read(&val._rep) ));
}

inline Uint32 AtomicInt::operator-(Uint32 val)
{ 
    return( (Uint32)(atomic_read(&_rep) - val));
}

inline AtomicInt& AtomicInt::operator+=(const AtomicInt& val)
{
    atomic_add(atomic_read(&val._rep),&_rep);
    return *this;
}

inline AtomicInt& AtomicInt::operator+=(Uint32 val)
{
    atomic_add(val,&_rep);
    return *this;
}

inline AtomicInt& AtomicInt::operator-=(const AtomicInt& val)
{
    atomic_sub(atomic_read(&val._rep),&_rep);
    return *this;
}

inline AtomicInt& AtomicInt::operator-=(Uint32 val)
{
    atomic_sub(val,&_rep);
    return *this;
}

#endif // native atomic int

#endif // IPCUnix_inline_h
