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
      long int tv_sec;      long int tv_usec;
};

struct timezone
{
      int tz_minuteswest;
      int tz_dsttime;
};


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
	
static int gettimeofday(struct timeval *tv, struct timezone *tz)
{
   return(pegasus_gettimeofday(tv));
}

inline void pegasus_yield(void)
{
  Sleep(0);
}

// pthreads cancellation calls 
inline void disable_cancel(void)
{
  ;
}

inline void enable_cancel(void)
{
  ;
}


// Windows does not have equivalent functionality with Unix-like
// operating systems. Be careful using these next two 
// macros. There is no pop routine in windows. Further, windows
// does not allow passing parameters to exit functions. !!
#define native_cleanup_push( func, arg ) 

#define native_cleanup_pop(execute) 

inline void PEGASUS_EXPORT init_crit(PEGASUS_CRIT_TYPE *crit)
{
   InitializeCriticalSection(crit);
}

inline void PEGASUS_EXPORT enter_crit(PEGASUS_CRIT_TYPE *crit)
{
   EnterCriticalSection(crit);
}

inline void PEGASUS_EXPORT try_crit(PEGASUS_CRIT_TYPE *crit)
{
  EnterCriticalSection(crit); 
}

inline void PEGASUS_EXPORT exit_crit(PEGASUS_CRIT_TYPE *crit)
{
   LeaveCriticalSection(crit);
}

inline void PEGASUS_EXPORT destroy_crit(PEGASUS_CRIT_TYPE *crit)
{
   DeleteCriticalSection(crit);
}
   
PEGASUS_THREAD_TYPE PEGASUS_EXPORT pegasus_thread_self(void) 
{ 
   return((PEGASUS_THREAD_TYPE)GetCurrentThreadId());
}

inline void PEGASUS_EXPORT exit_thread(PEGASUS_THREAD_RETURN rc)
{
  _endthreadex(rc);
}

inline void PEGASUS_EXPORT sleep(int ms)
{
  Sleep(ms);
}

Mutex::Mutex()
{
   _mutex.mut = CreateMutex(NULL, false, NULL);
   _mutex.owner = (PEGASUS_THREAD_TYPE)GetCurrentThreadId();
}

Mutex::Mutex(int mutex_type)
{
   _mutex.mut = CreateMutex(NULL, false, NULL);
   _mutex.owner = (PEGASUS_THREAD_TYPE)GetCurrentThreadId();
}

Mutex::~Mutex()
{
   WaitForSingleObject(_mutex.mut, INFINITE);
   CloseHandle(_mutex.mut);
}

// block until gaining the lock - throw a deadlock 
// exception if process already holds the lock 
void Mutex::lock(PEGASUS_THREAD_TYPE caller) throw(Deadlock, WaitFailed)
{
   if(_mutex.owner == caller)
      throw( Deadlock( _mutex.owner ) );

   DWORD errorcode = WaitForSingleObject(_mutex.mut, INFINITE);
   if(errorcode == WAIT_FAILED)
      throw( WaitFailed( _mutex.owner) );
   _mutex.owner = caller;
}
  
// try to gain the lock - lock succeeds immediately if the 
// mutex is not already locked. throws an exception and returns
// immediately if the mutex is currently locked. 
void Mutex::try_lock(PEGASUS_THREAD_TYPE caller) throw(Deadlock, AlreadyLocked, WaitFailed)
{
   if(_mutex.owner == caller)
      throw( Deadlock( _mutex.owner ) );

   DWORD errorcode = WaitForSingleObject(_mutex.mut, 0);
   if (errorcode == WAIT_TIMEOUT) 
      throw(AlreadyLocked(_mutex.owner));
   if(errorcode == WAIT_FAILED)
      throw(WaitFailed(_mutex.owner));
   _mutex.owner = caller;
}

// wait for milliseconds and throw an exception then return if the wait
// expires without gaining the lock. Otherwise return without throwing an
// exception.

void Mutex::timed_lock( Uint32 milliseconds , PEGASUS_THREAD_TYPE caller) 
   throw(Deadlock, TimeOut, WaitFailed)
{
   if(_mutex.owner == caller)
      throw( Deadlock( _mutex.owner ) );

   DWORD errorcode = WaitForSingleObject(_mutex.mut, milliseconds);
   if (errorcode == WAIT_TIMEOUT) 
      throw(TimeOut(_mutex.owner));
   if(errorcode == WAIT_FAILED)
      throw(WaitFailed(_mutex.owner));
   _mutex.owner = caller;
}

// unlock the mutex
void Mutex::unlock() throw(Permission)
{
   PEGASUS_THREAD_TYPE m_owner = _mutex.owner;
   _mutex.owner = 0;
   ReleaseMutex(_mutex.mut);
}

static const int SEM_VALUE_MAX = 0x0000ffff;
Semaphore::Semaphore(Uint32 initial) 
{
   if(initial > SEM_VALUE_MAX)
      initial = SEM_VALUE_MAX - 1;
   _count = initial;
   _semaphore.owner = (PEGASUS_THREAD_TYPE)GetCurrentThreadId();
   _semaphore.sem = CreateSemaphore(NULL, initial, SEM_VALUE_MAX, NULL);
}

Semaphore::~Semaphore()
{
   CloseHandle(_semaphore.sem);
}

// block until this semaphore is in a signalled state
void Semaphore::wait(void) 
{
   DWORD errorcode = WaitForSingleObject(_semaphore.sem, INFINITE);
   if(errorcode != WAIT_FAILED)
      _count--;
}

// wait succeeds immediately if semaphore has a non-zero count, 
// return immediately and throw and exception if the 
// count is zero. 
void Semaphore::try_wait(void) throw(WaitFailed)
{
   DWORD errorcode = WaitForSingleObject(_semaphore.sem, 0);
   if(errorcode == WAIT_TIMEOUT || errorcode == WAIT_FAILED)
      throw(WaitFailed((PEGASUS_THREAD_TYPE)GetCurrentThreadId()));
   _count--;
}


// wait for milliseconds and throw an exception
// if wait times out without gaining the semaphore
void Semaphore::time_wait( Uint32 milliseconds ) throw(TimeOut)
{
   DWORD errorcode = WaitForSingleObject(_semaphore.sem, milliseconds);
   if (errorcode == WAIT_TIMEOUT || errorcode == WAIT_FAILED)
      throw(TimeOut((PEGASUS_THREAD_TYPE)GetCurrentThreadId()));
   _count--;
}

// increment the count of the semaphore 
void Semaphore::signal()
{
   _count++;
   ReleaseSemaphore(_semaphore.sem, 1, NULL);
}

// return the count of the semaphore
int Semaphore::count() 
{
   return(_count);
}


//-----------------------------------------------------------------
/// Native Unix implementation of AtomicInt class
//-----------------------------------------------------------------
#if defined(PEGASUS_ATOMIC_INT_NATIVE)

AtomicInt::AtomicInt(): _rep(0) { init_crit(&_crit); }

AtomicInt::AtomicInt( Uint32 initial): _rep(initial) { init_crit(&_crit); }

AtomicInt::~AtomicInt() { destroy_crit(&_crit); }

AtomicInt::AtomicInt(const AtomicInt& original) 
{
   _rep = original._rep;
   init_crit(&_crit);
}

AtomicInt& AtomicInt::operator=( const AtomicInt& original)
{
   InterlockedExchange(&_rep, original._rep);
   return *this;
}


inline Uint32 AtomicInt::value(void)
{
   return (Uint32)_rep;
}

inline void AtomicInt::operator++(void) { InterlockedIncrement(&_rep); }
inline void AtomicInt::operator--(void) { InterlockedDecrement(&_rep); }

inline Uint32 AtomicInt::operator+(const AtomicInt& val) 
{
   InterlockedExchangeAdd(&_rep, val._rep);
   return _rep;
}
inline Uint32 AtomicInt::operator+(Uint32 val) 
{  
   InterlockedExchangeAdd(&_rep, val);
   return _rep;
}

inline Uint32 AtomicInt::operator-(const AtomicInt& val) 
{ 
   LONG temp_operand, temp_result;
   temp_operand = InterlockedExchangeAdd((long *)&(val._rep), 0);
   temp_result = InterlockedExchangeAdd(&_rep, 0);
   return(temp_result - temp_operand);
}

inline Uint32 AtomicInt::operator-(Uint32 val) 
{  
   LONG temp_operand, temp_result;
   temp_operand = InterlockedExchangeAdd( (long *)&val, 0);
   temp_result = InterlockedExchangeAdd(&_rep, 0);
   return(temp_result - temp_operand);
}

inline AtomicInt& AtomicInt::operator+=(const AtomicInt& val) 
{  
   InterlockedExchangeAdd(&_rep, val._rep);
   return *this;
}

inline AtomicInt& AtomicInt::operator+=(Uint32 val) 
{   
   InterlockedExchangeAdd(&_rep, val);
   return *this;
}
inline AtomicInt& AtomicInt::operator-=(const AtomicInt& val) 
{  
   LONG temp ;
   InterlockedExchange(&temp, val._rep);
   enter_crit(&_crit);
   _rep -= temp;
   exit_crit(&_crit);
      
   return *this;
}
inline AtomicInt& AtomicInt::operator-=(Uint32 val) 
{  
   LONG temp ;
   InterlockedExchange(&temp, val);
   enter_crit(&_crit);
   _rep -= temp;
   exit_crit(&_crit);
   
   return *this;
}

#endif // Native Atomic Type 

PEGASUS_NAMESPACE_END
