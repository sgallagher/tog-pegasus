//%////////////-*-c++-*-///////////////////////////////////////////////////////////
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

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0400
#endif 
#include <process.h>  

#include <windows.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/timeb.h> 
#include <errno.h>
#include <Pegasus/Common/Config.h>


typedef CRITICAL_SECTION  PEGASUS_CRIT_TYPE;
typedef HANDLE  PEGASUS_SEMAPHORE_TYPE;
typedef HANDLE  PEGASUS_THREAD_TYPE;
typedef HANDLE  PEGASUS_MUTEX_TYPE;

typedef struct {
      PEGASUS_SEMAPHORE_TYPE  sem;
      PEGASUS_THREAD_TYPE  owner;
} PEGASUS_SEM_HANDLE ;

typedef struct {
      HANDLE  mut;
      PEGASUS_THREAD_TYPE owner;
} PEGASUS_MUTEX_HANDLE ;


typedef void *PEGASUS_CLEANUP_HANDLE;

typedef DWORD PEGASUS_THREAD_RETURN;

#define PTHREAD_MUTEX_TIMED_NP

#define PEGASUS_THREAD_CDECL __stdcall

typedef struct {
      PEGASUS_THREAD_TYPE thid;
      void * thatt;
} PEGASUS_THREAD_HANDLE ;


//-----------------------------------------------------------------
/// Conditionals to support native conditional semaphore object
//-----------------------------------------------------------------

#if defined(PEGASUS_PLATFORM_WIN32_IX86_MSVC)

// windows uses the generic conditional semaphore defined in
// IPC.ccp

#endif // platform conditional  type

//-----------------------------------------------------------------
/// Conditionals to support native or generic atomic variables
//-----------------------------------------------------------------

#if defined(PEGASUS_PLATFORM_WIN32_IX86_MSVC)
#define PEGASUS_ATOMIC_INT_NATIVE = 1

typedef LONG  PEGASUS_ATOMIC_TYPE ;

#endif // platform atomic type

//-----------------------------------------------------------------
/// Conditionals to support native or generic read/write semaphores
//-----------------------------------------------------------------

#if defined(PEGASUS_PLATFORM_WIN32_IX86_MSVC)

// no native rw lock for windows -- use the generic one 

#endif // platform read/write type


//struct timeval 
//{
//      long int tv_sec;      long int tv_usec;
//};

struct timezone
{
      int tz_minuteswest;
      int tz_dsttime;
};


inline int pegasus_gettimeofday(struct timeval *tv)
{
	struct _timeb timebuffer;   
	if (tv == NULL)
		return(-1);
	_ftime( &timebuffer );
	tv->tv_sec = timebuffer.time;
	tv->tv_usec = ( timebuffer.millitm * 1000 );
	return(0);
}
	
inline int PEGASUS_EXPORT gettimeofday(struct timeval *tv, struct timezone *tz)
{
  return(pegasus_gettimeofday(tv));
}

PEGASUS_NAMESPACE_BEGIN

inline PEGASUS_EXPORT void pegasus_yield(void)
{
  Sleep(0);
}

// pthreads cancellation calls 
inline  PEGASUS_EXPORT void disable_cancel(void)
{
  ;
}

inline  PEGASUS_EXPORT void enable_cancel(void)
{
  ;
}


// Windows does not have equivalent functionality with Unix-like
// operating systems. Be careful using these next two 
// macros. There is no pop routine in windows. Further, windows
// does not allow passing parameters to exit functions. !!
inline PEGASUS_EXPORT void native_cleanup_push( void (*)(void *), void *) { ; }

inline PEGASUS_EXPORT void native_cleanup_pop(Boolean) { ; }

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

inline PEGASUS_THREAD_TYPE PEGASUS_EXPORT pegasus_thread_self(void) 
{ 
   return((PEGASUS_THREAD_TYPE)GetCurrentThreadId());
}

inline void PEGASUS_EXPORT exit_thread(PEGASUS_THREAD_RETURN rc)
{
  _endthreadex(rc);
}

inline void PEGASUS_EXPORT pegasus_sleep(int ms)
{
   if(ms == 0)
   {
      Sleep(0);
      return;
   }
   
   struct _timeb end, now;
   _ftime( &end );
   end.time += (ms / 1000);
   ms -= (ms / 1000);
   end.millitm += ms;
	
   do 
   {
      Sleep(0);
      _ftime(&now);
      
   } while( end.millitm > now.millitm && end.time >= now.time);
}


inline void PEGASUS_EXPORT destroy_thread(PEGASUS_THREAD_TYPE th, PEGASUS_THREAD_RETURN rc)
{
   TerminateThread(th, rc);
}


PEGASUS_NAMESPACE_END



