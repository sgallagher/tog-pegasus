//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software, Hewlett-Packard Company, IBM,
// The Open Group, Tivoli Systems
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

#ifndef IPC_UNIX_include
#define IPC_UNIX_include

#include <sched.h>
#include <semaphore.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>
#include <sys/time.h>
#include <time.h>
#include <sys/timex.h>
//#include <unistd.h>

#if defined(PEGASUS_PLATFORM_LINUX_GENERIC_GNU)

#if defined(PEGASUS_PLATFORM_LINUX_IA64_GNU)
#include <asm/atomic.h>
#endif 

#endif

PEGASUS_NAMESPACE_BEGIN

int timeval_subtract (struct timeval *result, 
		      struct timeval *x, 
		      struct timeval *y);


#ifdef PEGASUS_NEED_CRITICAL_TYPE
typedef pthread_spinlock_t PEGASUS_CRIT_TYPE;
#endif

typedef sem_t PEGASUS_SEMAPHORE_TYPE;
typedef pthread_t PEGASUS_THREAD_TYPE;
typedef pthread_mutex_t PEGASUS_MUTEX_TYPE;

#ifndef PEGASUS_OS_ZOS_ZSERIES_IBM
typedef struct {
    sem_t sem;
    pthread_t owner;
} PEGASUS_SEM_HANDLE ;
#else
typedef struct {
    UInt32 waiters;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    pthread_t owner;
} PEGASUS_SEM_HANDLE ;
#endif

typedef struct {
    pthread_mutex_t mut;
    pthread_mutexattr_t mutatt;
    pthread_t owner;
} PEGASUS_MUTEX_HANDLE ;

#ifdef PEGASUS_PLATFORM_SOLARIS_SPARC_GNU
typedef _cleanup_t PEGASUS_CLEANUP_HANDLE;
#else
typedef struct _pthread_cleanup_buffer  PEGASUS_CLEANUP_HANDLE;
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

#if defined(PEGASUS_PLATFORM_LINUX_GENERIC_GNU)
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

#if defined(PEGASUS_PLATFORM_LINUX_GENERIC_GNU)
#define PEGASUS_ATOMIC_INT_NATIVE = 1
#if defined(PEGASUS_PLATFORM_LINUX_IA64_GNU)
  #define PEGASUS_OLD_ATOMIC_INT = 1
  typedef atomic_t PEGASUS_ATOMIC_TYPE ;

#else
  typedef pthread_spinlock_t PEGASUS_ATOMIC_TYPE ;
#endif

#endif // linux platform atomic type

//-----------------------------------------------------------------
/// Conditionals to support native or generic read/write semaphores
//-----------------------------------------------------------------


//-----------------------------------------------------------------
// NOTE: Tue Oct  9 13:36:53 2001 mdday
//
//  I put some read/write counting into the Thread test program to see
//  how native r/w performance compares to generic r/w on linux. 
//  For RH linux 7.1, the generic r/w lock in IPC.cpp performs 
//  much better than the pthreads native implementation.
//  
//  Here are the data (remember that there are 4 readers for every one
//  writer):
// 
//  Generic  read operations  6538     <- 4:1 ratio of reads to writes
//           write operations 1422
//
//  Native   read operations   2060    <- 1:1 even though there are 
//           write operations  2033       4 readers for every writer
//
//
//  Comments -
// 
// The native implementation prefers writers, which slows the entire
// test down because write operations take longer than read operations.
// Moreover, as soon as one writer gains the lock, the next owner will 
// always be a writer until there are no further writers. Readers are 
// blocked out until all writers are sleeping. 
//
// In the test there are 4 readers for every writer. However, the 
// native r/w lock severely skews resources toward writers.
// 
// The generic implementation has no preference among readers and writers. 
// Therefore whichever thread is ready to read or write will gain the lock
// with no dependence on whether the predecessor is a reader or writer. 
// This results in higher throughput in the test program for linux.
//
// I would encourage all the platform maintainers to run their own tests
// so we can decide which implementation to use for production builds. 
//
//-----------------------------------------------------------------


//#if defined(PEGASUS_PLATFORM_LINUX_IX86_GNU) 
// #define PEGASUS_READWRITE_NATIVE = 1

// typedef struct {
//     pthread_rwlock_t rwlock;
//     pthread_t owner;
// } PEGASUS_RWLOCK_HANDLE;

//#endif // linux platform read/write type

inline void pegasus_yield(void)
{
#ifdef PEGASUS_NEED_CRITICAL_TYPE
      pthread_yield();
#else
      sched_yield();
#endif
}

// pthreads cancellation calls 
inline void disable_cancel(void)
{
   pthread_setcanceltype(PTHREAD_CANCEL_DISABLE, NULL);
}

inline void enable_cancel(void)
{
   pthread_setcanceltype(PTHREAD_CANCEL_ENABLE, NULL);
}

inline void pegasus_sleep(int msec)
{
   struct timespec wait;
   wait.tv_sec = msec / 1000;
   wait.tv_nsec = (msec % 1000) * 1000000;
   nanosleep(&wait, NULL);
}

#ifdef PEGASUS_NEED_CRITICAL_TYPE

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

#endif /* PEGASUS_NEED_CRITICAL_TYPE */

//-----------------------------------------------------------------
/// accurate version of gettimeofday for unix systems
//  posix glibc implementation does not return microseconds.
//  mdday Wed Aug  1 16:05:26 2001
//-----------------------------------------------------------------
inline static int pegasus_gettimeofday(struct timeval *tv)
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
   
inline void exit_thread(PEGASUS_THREAD_RETURN rc)
{
  pthread_exit(rc);
}

inline PEGASUS_THREAD_TYPE pegasus_thread_self(void) 
{ 
   return(pthread_self());
}

inline void destroy_thread(PEGASUS_THREAD_TYPE th, PEGASUS_THREAD_RETURN rc)
{
   pthread_cancel(th);

}

PEGASUS_NAMESPACE_END

#endif // IPCUNIXInclude
