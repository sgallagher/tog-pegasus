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
// Author: Markus Mueller (sedgewick_de@yahoo.de)
//
// Modified By: Mike Day (mdday@us.ibm.com)
//
//%/////////////////////////////////////////////////////////////////////////////


// Ensure Unix 98
#ifdef PEGASUS_PLATFORM_LINUX_IX86_GNU
   #ifndef _GNU_SOURCE
      #define _GNU_SOURCE
   #endif
#else
   #ifdef _XOPEN_SOURCE
      #undef _XOPEN_SOURCE
   #endif
   #define _XOPEN_SOURCE 600
#endif

#ifndef PEGASUS_PLATFORM_HPUX_PARISC_ACC
# include <features.h>
#endif

#ifdef PEGASUS_PLATFORM_HPUX_PARISC_ACC
# include <sched.h>
#endif

#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <errno.h>
#include <sys/time.h>
#include <time.h>


#ifdef PEGASUS_PLATFORM_HPUX_PARISC_ACC
// ATTN: RK - Need to determine HP-UX equivalent
typedef int PEGASUS_CRIT_TYPE;
#else
typedef pthread_spinlock_t PEGASUS_CRIT_TYPE;
#endif
typedef sem_t PEGASUS_SEMAPHORE_TYPE;
typedef pthread_t PEGASUS_THREAD_TYPE;
typedef pthread_mutex_t PEGASUS_MUTEX_TYPE;

typedef struct {
    sem_t sem;
    pthread_t owner;
} PEGASUS_SEM_HANDLE ;

typedef struct {
    pthread_mutex_t mut;
    pthread_mutexattr_t mutatt;
    pthread_t owner;
} PEGASUS_MUTEX_HANDLE ;

#ifdef PEGASUS_PLATFORM_HPUX_PARISC_ACC
   typedef void *PEGASUS_CLEANUP_HANDLE ;
#else
   typedef struct _pthread_cleanup_buffer  PEGASUS_CLEANUP_HANDLE ;
#endif

typedef void *PEGASUS_THREAD_RETURN;

#define PEGASUS_THREAD_CDECL

typedef struct {
    pthread_t thid;
    pthread_attr_t thatt;
} PEGASUS_THREAD_HANDLE ;

//-----------------------------------------------------------------
/// Conditionals to support native or generic Conditional Semaphore
//-----------------------------------------------------------------

#if defined(PEGASUS_PLATFORM_LINUX_IX86_GNU) || defined(PEGASUS_PLATFORM_HPUX_PARISC_ACC)
#define PEGASUS_CONDITIONAL_NATIVE = 1

typedef pthread_cond_t PEGASUS_COND_TYPE;

typedef struct {
    pthread_cond_t cond;
    pthread_t owner;
} PEGASUS_COND_HANDLE;

#endif // linux platform conditional type

//-----------------------------------------------------------------
/// Conditionals to support native or generic atomic variables
//-----------------------------------------------------------------

// linux offers a built-in integer type for atomic access
// other unix platforms HPUX, AIX, may have different types
// implementors should use the native type for faster operations

#if defined(PEGASUS_PLATFORM_LINUX_IX86_GNU) || defined(PEGASUS_PLATFORM_HPUX_PARISC_ACC)
#define PEGASUS_ATOMIC_INT_NATIVE = 1

typedef sig_atomic_t PEGASUS_ATOMIC_TYPE ;

#endif // linux platform atomic type

//-----------------------------------------------------------------
/// Conditionals to support native or generic read/write semaphores
//-----------------------------------------------------------------

#if defined(PEGASUS_PLATFORM_LINUX_IX86_GNU) || defined(PEGASUS_PLATFORM_HPUX_PARISC_ACC)
#ifdef PEGASUS_PLATFORM_HPUX_PARISC_ACC
// ATTN: RK - Need this definition to compile on HP-UX (or else
// PEGASUS_RWLOCK_HANDLE is undefined)
#define PEGASUS_READWRITE_NATIVE = 1
#else
// #define PEGASUS_READWRITE_NATIVE = 1
#endif

typedef struct {
    pthread_rwlock_t rwlock;
    pthread_t owner;
} PEGASUS_RWLOCK_HANDLE;

#endif // linux platform read/write type


