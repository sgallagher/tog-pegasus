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

#ifndef IPC_UNIX_include
#define IPC_UNIX_include

// there is a bug in the headers on at least one Red Hat 7.1 release
// that undefines some important environment variables for multi-threading

/* POSIX spinlock data type.  */
#ifndef pthread_spinlock_t
typedef volatile int pthread_spinlock_t;
#endif

/* POSIX barrier. */
#ifndef pthread_barrier_t
typedef struct {
  struct _pthread_fastlock __ba_lock; 
  int __ba_required; 
  int __ba_present;
  _pthread_descr __ba_waiting;
} pthread_barrier_t;
#endif

/* barrier attribute */
#ifndef pthread_barrierattr_t
typedef struct {
  int __pshared;
} pthread_barrierattr_t;
#endif 

#include <pthread.h>

#ifndef pthread_yield
extern int pthread_yield(void)
   __THROW;
#endif

#ifndef pthread_spin_init
extern int pthread_spin_init (pthread_spinlock_t *__lock, int __pshared)
     __THROW;
#endif

#ifndef pthread_spin_destroy
extern int pthread_spin_destroy (pthread_spinlock_t *__lock) 
   __THROW;
#endif

#ifndef pthread_spin_lock
extern int pthread_spin_lock (pthread_spinlock_t *__lock) 
   __THROW;
#endif

#ifndef pthread_spin_trylock
extern int pthread_spin_trylock (pthread_spinlock_t *__lock)
   __THROW;
#endif

#ifndef pthread_spin_unlock
extern int pthread_spin_unlock (pthread_spinlock_t *__lock) 
   __THROW;
#endif

#ifndef pthread_mutexattr_settype
extern int pthread_mutexattr_settype (pthread_mutexattr_t *__attr, int __kind)
     __THROW;
#endif

#ifndef pthread_mutexattr_gettype
extern int pthread_mutexattr_gettype (__const pthread_mutexattr_t *__restrict
				      __attr, int *__restrict __kind) 
   __THROW;
#endif

#ifndef _pthread_cleanup_push_defer
extern void _pthread_cleanup_push_defer (struct _pthread_cleanup_buffer *__buffer,
					 void (*__routine) (void *),
					 void *__arg) __THROW;
#endif 

#ifndef _pthread_cleanup_pop_restore
extern void _pthread_cleanup_pop_restore (struct _pthread_cleanup_buffer *__buffer,
					  int __execute) __THROW;
#endif


#ifndef pthread_cleanup_push_defer_np
# define pthread_cleanup_push_defer_np(routine,arg) \
  { struct _pthread_cleanup_buffer _buffer;				      \
    _pthread_cleanup_push_defer (&_buffer, (routine), (arg));
#endif


#ifndef pthread_cleanup_pop_restore_np
# define pthread_cleanup_pop_restore_np(execute) \
  _pthread_cleanup_pop_restore (&_buffer, (execute)); }
#endif

#include <sched.h>
#include <semaphore.h>
#include <signal.h>
#include <errno.h>
#include <sys/time.h>
#include <time.h>
#include <sys/timex.h>
#include <unistd.h>

#if defined(PEGASUS_PLATFORM_LINUX_IX86_GNU)
#include <asm/atomic.h>
#endif

typedef pthread_spinlock_t PEGASUS_CRIT_TYPE;
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

typedef struct _pthread_cleanup_buffer  PEGASUS_CLEANUP_HANDLE ;


typedef void *PEGASUS_THREAD_RETURN;

#define PEGASUS_THREAD_CDECL

typedef struct {
    pthread_t thid;
    pthread_attr_t thatt;
} PEGASUS_THREAD_HANDLE ;

//-----------------------------------------------------------------
/// Conditionals to support native or generic Conditional Semaphore
//-----------------------------------------------------------------

#if defined(PEGASUS_PLATFORM_LINUX_IX86_GNU)
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

#if defined(PEGASUS_PLATFORM_LINUX_IX86_GNU)
#define PEGASUS_ATOMIC_INT_NATIVE = 1

typedef atomic_t PEGASUS_ATOMIC_TYPE ;

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

PEGASUS_NAMESPACE_BEGIN

inline void pegasus_yield(void)
{

      pthread_yield();
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

inline void pegasus_sleep(int msec)
{
   struct timespec wait;
   wait.tv_sec = msec / 1000;
   msec -= wait.tv_sec * 1000;
   wait.tv_nsec =  (msec & 1000) * 1000000;
   nanosleep(&wait, NULL);
}

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
