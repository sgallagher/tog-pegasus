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
// Author: Mike Day (mdday@us.ibm.com)
//
// Modified By: 
//
//%/////////////////////////////////////////////////////////////////////////////


#include <sys/types.h>
#include <sys/timeb.h>

PEGASUS_NAMESPACE_BEGIN

struct timeval 
{
      long int tv_sec;
      long int tv_usec;
};

struct timezone
{
      int tz_minuteswest;
      int tz_dsttime;
};


static int gettimeofday(struct timeval *tv, struct timezone *tz)
{
   return(pegasus_gettimeofday(tv));
}

static int pegasus_gettimeofday(struct timeval *tv)
{
	struct _timeb timebuffer;   
	if (tv == NULL)
		return(-1);
	_ftime( &timebuffer );
	tv->tv_sec = timebuffer.time;
	tv->tv_usec = ( timebuffer.millitm / 1000 );
	return(0);
}	


inline void pegasus_yield(void)
{
   SwitchToThread();
}

// pthreads cancellation calls 
inline void disable_cancel(void)
{
   InterlockedExchange(&(LONG *)_cancel_enabled, (LONG)false);
}

inline void enable_cancel(void)
{
   InterlockedExchange(&(LONG *)_cancel_enabled, (LONG)true);
}



#define native_cleanup_push( func, arg ) \


// native cleanup_pop(Boolean execute) ; 
#define native_cleanup_pop(execute) \

inline void init_crit(PEGASUS_CRIT_TYPE *crit)
{

}

inline void enter_crit(PEGASUS_CRIT_TYPE *crit)
{

}

inline void try_crit(PEGASUS_CRIT_TYPE *crit)
{

}

inline void exit_crit(PEGASUS_CRIT_TYPE *crit)
{

}

inline void destroy_crit(PEGASUS_CRIT_TYPE *crit)
{

}
   

PEGASUS_THREAD_TYPE pegasus_thread_self(void) 
{ 
   return(GetCurrentThread());
}

Mutex::Mutex()
{

}

Mutex::Mutex(int mutex_type)
{

}

Mutex::~Mutex()
{
   while(  get the mutex )
   {
      pegasus_yield();
   }
   CloseHandle(_mutex.mut);
}

// block until gaining the lock - throw a deadlock 
// exception if process already holds the lock 
void Mutex::lock(PEGASUS_THREAD_TYPE caller) throw(Deadlock, WaitFailed)
{
   int errorcode;



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

void Mutex::timed_lock( Uint32 milliseconds , PEGASUS_THREAD_TYPE caller) 
   throw(Deadlock, TimeOut, WaitFailed)
{


}

// unlock the mutex
void Mutex::unlock() throw(Permission)
{
   PEGASUS_THREAD_TYPE m_owner = _mutex.owner;
   _mutex.owner = 0;

}


//-----------------------------------------------------------------
// Native implementation of Conditional semaphore object
//-----------------------------------------------------------------

#ifdef PEGASUS_CONDITIONAL_NATIVE


Condition::Condition() 
{

}

Condition::~Condition()
{

}

void Condition::signal(PEGASUS_THREAD_TYPE caller) 
   throw(Deadlock, WaitFailed, Permission) 
{ 


}


void Condition::unlocked_signal(PEGASUS_THREAD_TYPE caller) 
   throw(Permission)
{

}


void Condition::lock_object(PEGASUS_THREAD_TYPE caller)
   throw(Deadlock, WaitFailed)
{

}

void Condition::try_lock_object(PEGASUS_THREAD_TYPE caller)
   throw(Deadlock, AlreadyLocked, WaitFailed)
{

}

void Condition::wait_lock_object(PEGASUS_THREAD_TYPE caller, int milliseconds)
   throw(Deadlock, TimeOut, WaitFailed)
{

}

void Condition::unlock_object(void)
{

}


// block until this semaphore is in a signalled state 
void Condition::unlocked_wait(PEGASUS_THREAD_TYPE caller) 
   throw(Permission)
{

}

// block until this semaphore is in a signalled state 
void Condition::unlocked_timed_wait(int milliseconds, PEGASUS_THREAD_TYPE caller) 
   throw(TimeOut, Permission)
{

}

#endif
//-----------------------------------------------------------------
// END of native conditional semaphore implementation
//-----------------------------------------------------------------

Semaphore::Semaphore(Uint32 initial) 
{
   if(initial > SEM_VALUE_MAX)
      initial = SEM_VALUE_MAX - 1;

   _semaphore.owner = pthread_self();

}

Semaphore::~Semaphore()
{

}

// block until this semaphore is in a signalled state
void Semaphore::wait(void) 
{

}

// wait succeeds immediately if semaphore has a non-zero count, 
// return immediately and throw and exception if the 
// count is zero. 
void Semaphore::try_wait(void) throw(WaitFailed)
{

}


// wait for milliseconds and throw an exception
// if wait times out without gaining the semaphore
void Semaphore::time_wait( Uint32 milliseconds ) throw(TimeOut)
{

}

// increment the count of the semaphore 
void Semaphore::signal()
{

}

// return the count of the semaphore
int Semaphore::count() 
{


}



//-----------------------------------------------------------------
/// Native Unix implementation of AtomicInt class
//-----------------------------------------------------------------
#if defined(PEGASUS_ATOMIC_INT_NATIVE)

AtomicInt::AtomicInt()

AtomicInt::AtomicInt( Uint32 initial)

AtomicInt::~AtomicInt() {}

AtomicInt::AtomicInt(const AtomicInt& original) 
{

}

AtomicInt& AtomicInt::operator=( const AtomicInt& original)
{

}


Uint32 AtomicInt::value(void)
{
}

void AtomicInt::operator++(void) { }
void AtomicInt::operator--(void) { }

Uint32 AtomicInt::operator+(const AtomicInt& val) { }
Uint32 AtomicInt::operator+(Uint32 val) {  }

Uint32 AtomicInt::operator-(const AtomicInt& val) { }
Uint32 AtomicInt::operator-(Uint32 val) {  }

AtomicInt& AtomicInt::operator+=(const AtomicInt& val) {  }
AtomicInt& AtomicInt::operator+=(Uint32 val) { }
AtomicInt& AtomicInt::operator-=(const AtomicInt& val) {  }
AtomicInt& AtomicInt::operator-=(Uint32 val) {  }

#endif // Native Atomic Type 

PEGASUS_NAMESPACE_END
