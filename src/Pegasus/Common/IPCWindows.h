//%2005////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
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
// Modified By: Arthur Pichlkostner
//             (checked in: Markus Mueller sedgewick_de@yahoo.de)
//              Mary Hinton (m.hinton@verizon.net)
//              Steve Hills (steve.hills@ncr.com)
//%/////////////////////////////////////////////////////////////////////////////


#define _WIN32_WINNT 0x0400

#include <process.h>  

//
// PLEASE DO NOT REMOVE THE DEFINTION OF FD_SETSIZE!
//

#ifndef FD_SETSIZE
# define FD_SETSIZE 1024
#endif

#include <windows.h>
#ifndef _WINSOCKAPI_
#include <winsock2.h>
#endif
#include <winbase.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/timeb.h> 
#include <errno.h>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Linkage.h>


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

typedef unsigned PEGASUS_THREAD_RETURN;

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

#define PEGASUS_CONDITIONAL_NATIVE
typedef HANDLE PEGASUS_COND_TYPE;

typedef struct {
      PEGASUS_COND_TYPE cond;
      PEGASUS_THREAD_TYPE owner;
} PEGASUS_COND_HANDLE;

#endif // platform conditional  type

//-----------------------------------------------------------------
/// Conditionals to support native or generic atomic variables
//-----------------------------------------------------------------

#if defined(PEGASUS_PLATFORM_WIN32_IX86_MSVC)
#define PEGASUS_ATOMIC_INT_NATIVE

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


// excluded

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

// Markus: new implementation with higher resolution
// that is needed for performance statistics
/*



THIS ROUTINE IS BROKEN << Thu Mar 20 10:24:14 2003 mdd >> 
inline int pegasus_gettimeofday(struct timeval *tv)
{
   if (tv == NULL){
                   return(-1);
   }
   LARGE_INTEGER frequency;
   if (!QueryPerformanceFrequency(&frequency)){
      struct _timeb timebuffer;
           _ftime( &timebuffer );
           tv->tv_sec = timebuffer.time;
           tv->tv_usec = ( timebuffer.millitm * 1000 );
           return(0);
   } else {
      LARGE_INTEGER counter;
      QueryPerformanceCounter(&counter);
      tv->tv_sec = (__int64)((counter.QuadPart)/(frequency.QuadPart));
      tv->tv_usec = ((__int64)
          ((counter.QuadPart)*1000000/(frequency.QuadPart)))%1000000;
      return(0);
   }
}
	
*/
inline int PEGASUS_COMMON_LINKAGE gettimeofday(struct timeval *tv, struct timezone *tz)
{
  return(pegasus_gettimeofday(tv));
}

PEGASUS_NAMESPACE_BEGIN

inline PEGASUS_COMMON_LINKAGE void pegasus_yield(void)
{
  Sleep(0);
}

// pthreads cancellation calls 
inline  PEGASUS_COMMON_LINKAGE void disable_cancel(void)
{
  ;
}

inline  PEGASUS_COMMON_LINKAGE void enable_cancel(void)
{
  ;
}


// Windows does not have equivalent functionality with Unix-like
// operating systems. Be careful using these next two 
// macros. There is no pop routine in windows. Further, windows
// does not allow passing parameters to exit functions. !!
inline PEGASUS_COMMON_LINKAGE void native_cleanup_push( void (*)(void *), void *) { ; }

inline PEGASUS_COMMON_LINKAGE void native_cleanup_pop(Boolean) { ; }

inline void PEGASUS_COMMON_LINKAGE init_crit(PEGASUS_CRIT_TYPE *crit)
{
   InitializeCriticalSection(crit);
}

inline void PEGASUS_COMMON_LINKAGE enter_crit(PEGASUS_CRIT_TYPE *crit)
{
   EnterCriticalSection(crit);
}

inline void PEGASUS_COMMON_LINKAGE try_crit(PEGASUS_CRIT_TYPE *crit)
{
  EnterCriticalSection(crit); 
}

inline void PEGASUS_COMMON_LINKAGE exit_crit(PEGASUS_CRIT_TYPE *crit)
{
   LeaveCriticalSection(crit);
}

inline void PEGASUS_COMMON_LINKAGE destroy_crit(PEGASUS_CRIT_TYPE *crit)
{
   DeleteCriticalSection(crit);
}

inline PEGASUS_THREAD_TYPE PEGASUS_COMMON_LINKAGE pegasus_thread_self(void) 
{ 
   return((PEGASUS_THREAD_TYPE)GetCurrentThreadId());
}

// l10n start
typedef DWORD PEGASUS_THREAD_KEY_TYPE;

inline Uint32 pegasus_key_create(PEGASUS_THREAD_KEY_TYPE * key)
{
	// Note: destructor is not supported
	*key = TlsAlloc();
	if (*key == -1)
		return 1;
	return 0;	
} 

inline Uint32 pegasus_key_delete(PEGASUS_THREAD_KEY_TYPE key)
{
	if (TlsFree(key))
		return 0;
	return 1;			
} 

inline void * pegasus_get_thread_specific(PEGASUS_THREAD_KEY_TYPE key)
{
	return TlsGetValue(key);
} 

inline Uint32 pegasus_set_thread_specific(PEGASUS_THREAD_KEY_TYPE key,
										 void * value)
{
	if (TlsSetValue(key, value))
		return 0;
	return 1;
} 
// l10n end

inline void PEGASUS_COMMON_LINKAGE exit_thread(PEGASUS_THREAD_RETURN rc)
{
  _endthreadex(rc);
}

inline void PEGASUS_COMMON_LINKAGE pegasus_sleep(int ms)
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


inline void PEGASUS_COMMON_LINKAGE destroy_thread(PEGASUS_THREAD_TYPE th, PEGASUS_THREAD_RETURN rc)
{
   TerminateThread(th, rc);
}


PEGASUS_NAMESPACE_END



