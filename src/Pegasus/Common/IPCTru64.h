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
// Author: Brian Bobryk (Brian.Bobryk@compaq.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef IPC_TRU64_include
#define IPC_TRU64_include

#include <sched.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <errno.h>
#include <sys/time.h>
#include <time.h>

PEGASUS_NAMESPACE_BEGIN

typedef sem_t PEGASUS_SEMAPHORE_TYPE;
typedef pthread_t PEGASUS_THREAD_TYPE;
typedef pthread_mutex_t PEGASUS_MUTEX_TYPE;

typedef struct {
	PEGASUS_SEMAPHORE_TYPE sem;
	PEGASUS_THREAD_TYPE owner;
} PEGASUS_SEM_HANDLE ;

/*
typedef struct {
    Uint32 waiters;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    PEGASUS_THREAD_TYPE owner;
} PEGASUS_SEM_HANDLE ;
*/

typedef struct {
    pthread_mutex_t mut;
    pthread_mutexattr_t mutatt;
    pthread_t owner;
} PEGASUS_MUTEX_HANDLE ;

typedef PEGASUS_MUTEX_HANDLE PEGASUS_CRIT_TYPE;

typedef void *PEGASUS_CLEANUP_HANDLE ;
typedef void *PEGASUS_THREAD_RETURN;

#define PEGASUS_THREAD_CDECL

typedef struct {
    pthread_t thid;
    pthread_attr_t thatt;
} PEGASUS_THREAD_HANDLE ;

//-----------------------------------------------------------------
/// Conditionals to support native or generic Conditional Semaphore
//-----------------------------------------------------------------

#define PEGASUS_CONDITIONAL_NATIVE = 1

typedef pthread_cond_t PEGASUS_COND_TYPE;

typedef struct {
    pthread_cond_t cond;
    pthread_t owner;
} PEGASUS_COND_HANDLE;


//-----------------------------------------------------------------
/// Conditionals to support native or generic atomic variables
//-----------------------------------------------------------------

// linux offers a built-in integer type for atomic access
// other unix platforms HPUX, AIX, may have different types
// implementors should use the native type for faster operations

// ATTN: RK - sig_atomic_t is defined on HP-UX, but the atomic_read()
// and atomic_write() methods are not defined.  Use the non-native
// implementation for now.

// #define PEGASUS_ATOMIC_INT_NATIVE = 1

// typedef sig_atomic_t PEGASUS_ATOMIC_TYPE ;


//-----------------------------------------------------------------
/// Conditionals to support native or generic read/write semaphores
//-----------------------------------------------------------------

#define PEGASUS_READWRITE_NATIVE = 1

typedef struct {
    pthread_rwlock_t rwlock;
    pthread_t owner;
} PEGASUS_RWLOCK_HANDLE;

inline void pegasus_yield(void)
{
      sched_yield();
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
    msec -= wait.tv_sec * 1000;  // What is this line for?
    wait.tv_nsec = (msec & 1000) * 1000000;
    nanosleep(&wait, NULL);
}

inline void init_crit(PEGASUS_CRIT_TYPE *crit)
{
   pthread_mutex_init(&(crit->mut), NULL);
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
   pthread_mutexattr_destroy(&(crit->mutatt));
}

static inline int pegasus_gettimeofday(struct timeval *tv) { return(gettimeofday(tv, NULL)); }

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

#endif // IPCTRU64Include
