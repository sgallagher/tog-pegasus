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

#ifndef PEGASUS_PLATFORM_HPUX_PARISC_ACC
#include <sys/timex.h>
#endif

PEGASUS_NAMESPACE_BEGIN

inline void pegasus_yield(void)
{
#ifdef PEGASUS_PLATFORM_HPUX_PARISC_ACC
      sched_yield();
#else
      pthread_yield();
#endif
}

// pthreads cancellation calls 
inline void disable_cancel(void)
{
   pthread_setcanceltype(PTHREAD_CANCEL_DISABLE, NULL);
}

inline void enable_cancel(void)
{
   pthread_setcanceltype(PTHREAD_CANCEL_DISABLE, NULL);
}


// the next two routines are macros that MUST SHARE the same stack frame
// they are implemented as macros by glibc. 
// native_cleanup_push( void (*func)(void *) ) ;
// these ALSO SET CANCEL STATE TO DEFER
#define native_cleanup_push( func, arg ) \
   pthread_cleanup_push_defer_np((func), arg)

// native cleanup_pop(Boolean execute) ; 
#define native_cleanup_pop(execute) \
   pthread_cleanup_pop_restore_np(execute)

void sleep(int msec)
{
  struct timespec timeout;
  timeout.tv_sec = msec / 1000;
  timeout.tv_nsec = (msec & 1000) * 1000;
  nanosleep(&timeout,NULL);
}

#ifndef PEGASUS_PLATFORM_HPUX_PARISC_ACC
// ATTN: RK - Need to determine HP-UX equivalents

inline void init_crit(PEGASUS_CRIT_TYPE *crit)
{
   pthread_spin_init(crit, 0);
}

inline void enter_crit(PEGASUS_CRIT_TYPE *crit)
{
   pthread_spin_lock(crit);
}

inline void try_crit(PEGASUS_CRIT_TYPE *crit)
{
   pthread_spin_trylock(crit);
}

inline void exit_crit(PEGASUS_CRIT_TYPE *crit)
{
   pthread_spin_unlock(crit);
}

inline void destroy_crit(PEGASUS_CRIT_TYPE *crit)
{
   pthread_spin_destroy(crit);
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

#else

inline void init_crit(PEGASUS_CRIT_TYPE *crit)
{
   pthread_mutexattr_init(&(crit->mutatt));
   pthread_mutexattr_setspin_np(&(crit->mutatt), PTHREAD_MUTEX_SPINONLY_NP);
   pthread_mutex_init(&(crit->mut), &(crit->mutatt));
   crit->owner = 0;
}

inline void enter_crit(PEGASUS_CRIT_TYPE *crit)
{
   pthread_mutex_lock(&(crit->mut));
}

inline void try_crit(PEGASUS_CRIT_TYPE *crit)
{
   pthread_mutex_trylock(&(crit->mut));
}

inline void exit_crit(PEGASUS_CRIT_TYPE *crit)
{
   pthread_mutex_unlock(&(crit->mut));
}

inline void destroy_crit(PEGASUS_CRIT_TYPE *crit)
{
   while( EBUSY == pthread_mutex_destroy(&(crit->mut)))
   {
      pegasus_yield();
   }
   pthread_mutexattr_destroy(&(crit->mutatt));
}

static inline int pegasus_gettimeofday(struct timeval *tv) { return(gettimeofday(tv, NULL)); }

#endif
   
inline void exit_thread(PEGASUS_THREAD_RETURN rc)
{
  pthread_exit(rc);
}

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

void Mutex::timed_lock( Uint32 milliseconds , PEGASUS_THREAD_TYPE caller) 
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
void Mutex::unlock() throw(Permission)
{
   PEGASUS_THREAD_TYPE m_owner = _mutex.owner;
   _mutex.owner = 0;
   if(0 != pthread_mutex_unlock(&_mutex.mut)) 
   {
      _mutex.owner = m_owner;
      throw(Permission(_mutex.owner));
   }
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
}

Condition::~Condition()
{
   while(EBUSY == pthread_cond_destroy(&_condition))
   {
      unlocked_signal(pegasus_thread_self());
      pegasus_yield();
   }
   
   _cond_mutex.~Mutex();
}

// block until this semaphore is in a signalled state 
// void Condition::wait(PEGASUS_THREAD_TYPE caller) throw(Deadlock, WaitFailed)
// {
//    try { _cond_mutex.lock(caller); }
//    catch(...) { throw; }

//    pthread_cond_wait(&_condition, &_cond_mutex._mutex.mut);
//    _cond_mutex.unlock();
// }

void Condition::signal(PEGASUS_THREAD_TYPE caller) 
   throw(Deadlock, WaitFailed, Permission) 
{ 
   try 
   { 
      _cond_mutex.lock(caller); 
   }
   catch(...) 
   { throw; 
   } 
   pthread_cond_broadcast(&_condition);
   _cond_mutex.unlock(); 
}

// wait for milliseconds and throw an exception
// if wait times out without gaining the semaphore
// void Condition::time_wait( Uint32 milliseconds, PEGASUS_THREAD_TYPE caller ) throw(TimeOut, Deadlock, WaitFailed)
// {
//    struct timeval now;
//    struct timespec timeout;
//    int retcode;

//    try { _cond_mutex.lock(caller); }
//    catch(...) { throw; }
//    do 
//    { 
// //      gettimeofday(&now,NULL);
//       pegasus_gettimeofday(&now);
//       timeout.tv_sec = now.tv_sec;
//       timeout.tv_nsec += now.tv_usec * 1000 + milliseconds; 
//       retcode = pthread_cond_timedwait(&_condition, &_cond_mutex._mutex.mut, &timeout);
//    } while( retcode == EINTR ) ;
    
//    _cond_mutex.unlock();
//    if( retcode)
//       throw(TimeOut(caller)) ;
// }

void Condition::unlocked_signal(PEGASUS_THREAD_TYPE caller) 
   throw(Permission)
{
   pthread_cond_broadcast(&_condition);
}


void Condition::lock_object(PEGASUS_THREAD_TYPE caller)
   throw(Deadlock, WaitFailed)
{
   _cond_mutex.lock(caller);
}

void Condition::try_lock_object(PEGASUS_THREAD_TYPE caller)
   throw(Deadlock, AlreadyLocked, WaitFailed)
{
   _cond_mutex.try_lock(caller);
}

void Condition::wait_lock_object(PEGASUS_THREAD_TYPE caller, int milliseconds)
   throw(Deadlock, TimeOut, WaitFailed)
{
   _cond_mutex.timed_lock(milliseconds, caller);
}

void Condition::unlock_object(void)
{
   _cond_mutex.unlock();
}


// block until this semaphore is in a signalled state 
void Condition::unlocked_wait(PEGASUS_THREAD_TYPE caller) 
   throw(Permission)
{
   pthread_cond_wait(&_condition, &_cond_mutex._mutex.mut);
}

// block until this semaphore is in a signalled state 
void Condition::unlocked_timed_wait(int milliseconds, PEGASUS_THREAD_TYPE caller) 
   throw(TimeOut, Permission)
{
   struct timeval now;
   int retcode;
   gettimeofday(&now, NULL);
   now.tv_usec += (milliseconds * 1000);
   do
   {
      retcode = pthread_cond_timedwait(&_condition, &_cond_mutex._mutex.mut, (struct timespec *)&now) ;
   } while ( retcode == EINTR ) ;
   if(retcode)
      throw(TimeOut(caller));
}

#endif
//-----------------------------------------------------------------
// END of native conditional semaphore implementation
//-----------------------------------------------------------------

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


// Note: I could not get sem_timed_wait to work reliably. 
// See my comments above on mut timed_wait. 
// I reimplemented using try_wait, which works reliably. 
// mdd Sun Aug  5 13:25:31 2001

// wait for milliseconds and throw an exception
// if wait times out without gaining the semaphore
void Semaphore::time_wait( Uint32 milliseconds ) throw(TimeOut)
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

AtomicInt& AtomicInt::operator=( const AtomicInt& original)
{
   // don't worry about self-assignment in this implementation
   if(this != &original)
      atomic_set(&_rep,atomic_read(&original._rep));
   return *this;
}


Uint32 AtomicInt::value(void)
{
   return((Uint32)atomic_read(&_rep));
}

void AtomicInt::operator++(void) { atomic_inc(&_rep); }
void AtomicInt::operator--(void) { atomic_dec(&_rep); }

Uint32 AtomicInt::operator+(const AtomicInt& val)
{
     return((Uint32)(atomic_read(&_rep) + atomic_read(&val._rep) ));
}
Uint32 AtomicInt::operator+(Uint32 val)
{ 
    return( (Uint32)(atomic_read(&_rep) + val));
}
Uint32 AtomicInt::operator-(const AtomicInt& val)
{
     return((Uint32)(atomic_read(&_rep) - atomic_read(&val._rep) ));
}
Uint32 AtomicInt::operator-(Uint32 val)
{ 
    return( (Uint32)(atomic_read(&_rep) - val));
}

AtomicInt& AtomicInt::operator+=(const AtomicInt& val)
{
    atomic_add(atomic_read(&val._rep),&_rep);
    return *this;
}
AtomicInt& AtomicInt::operator+=(Uint32 val)
{
    atomic_add(val,&_rep);
    return *this;
}
AtomicInt& AtomicInt::operator-=(const AtomicInt& val)
{
    atomic_sub(atomic_read(&val._rep),&_rep);
    return *this;
}
AtomicInt& AtomicInt::operator-=(Uint32 val)
{
    atomic_sub(val,&_rep);
    return *this;
}
// still missing are atomic test like "subtract and test if zero"
#endif // Native Atomic Type 

PEGASUS_NAMESPACE_END
