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

#include <process.h>  
#include <windows.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/timeb.h> 
#include <errno.h>

int gettimeofday(struct timeval *tv, struct timezone *tz);

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

struct timeval 
{
      long int tv_sec;      long int tv_usec;
};

struct timezone
{
      int tz_minuteswest;
      int tz_dsttime;
};

