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

#ifndef IPC_ZOS_include
#define IPC_ZOS_include

//#include <sched.h>
//#include <semaphore.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>
#include <sys/time.h>
#include <time.h>
//#include <sys/timex.h>
#include <unistd.h>

#define SEM_VALUE_MAX 0xffffffff


PEGASUS_NAMESPACE_BEGIN

//typedef sem_t PEGASUS_SEMAPHORE_TYPE;
//typedef pthread_t PEGASUS_THREAD_TYPE;

typedef PEGASUS_UINT64 PEGASUS_THREAD_TYPE;
typedef pthread_mutex_t PEGASUS_MUTEX_TYPE;

typedef struct {
    Uint32 waiters;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    PEGASUS_THREAD_TYPE owner;
} PEGASUS_SEM_HANDLE ;

typedef struct {
    pthread_mutex_t mut;
    pthread_mutexattr_t mutatt;
    PEGASUS_THREAD_TYPE owner;
} PEGASUS_MUTEX_HANDLE ;

typedef PEGASUS_MUTEX_HANDLE PEGASUS_CRIT_TYPE;


struct _pthread_cleanup_buffer {
    void (*__routine) (void *);             /* Function to call.  */
    void *__arg;                            /* Its argument.  */
    int __canceltype;                       /* Saved cancellation type. */
    struct _pthread_cleanup_buffer *__prev;  /* Chaining of cleanup functions.*/
};
typedef struct _pthread_cleanup_buffer  PEGASUS_CLEANUP_HANDLE;

//#define PEGASUS_THREAD_CDECL __cdecl
#define PEGASUS_THREAD_CDECL

typedef void *PEGASUS_THREAD_RETURN;

typedef struct {
    PEGASUS_THREAD_TYPE thid;
    pthread_attr_t thatt;
} PEGASUS_THREAD_HANDLE ;

//-----------------------------------------------------------------
/// Conditionals to support native or generic Conditional Semaphore
//-----------------------------------------------------------------

#define PEGASUS_CONDITIONAL_NATIVE = 1

typedef pthread_cond_t PEGASUS_COND_TYPE;

typedef struct {
    pthread_cond_t cond;
    PEGASUS_THREAD_TYPE owner;
} PEGASUS_COND_HANDLE;

//-----------------------------------------------------------------
/// Conditionals to support native or generic atomic variables
//-----------------------------------------------------------------


//-----------------------------------------------------------------
/// Conditionals to support native or generic read/write semaphores
//-----------------------------------------------------------------

#define PEGASUS_READWRITE_NATIVE = 1

typedef struct {
     pthread_rwlock_t rwlock;
     PEGASUS_THREAD_TYPE owner;
} PEGASUS_RWLOCK_HANDLE;


inline void pegasus_yield(void)
{
      pthread_yield(NULL);
}

// pthreads cancellation calls
inline void disable_cancel(void)
{
   pthread_setintr(PTHREAD_INTR_DISABLE);
}

inline void enable_cancel(void)
{
   pthread_setintr(PTHREAD_INTR_ENABLE);
}

inline void pegasus_sleep(int msec)
{
   struct timespec wait;
   pthread_mutex_t sleep_mut = PTHREAD_MUTEX_INITIALIZER;
   pthread_cond_t sleep_cond;
   pthread_cond_init (&sleep_cond, NULL);
   pthread_mutex_lock(&sleep_mut);
   wait.tv_sec = msec / 1000;
   msec -= wait.tv_sec * 1000;
   wait.tv_nsec =  (msec & 1000) * 1000000;
   pthread_cond_timedwait(&sleep_cond,&sleep_mut,&wait);
   pthread_mutex_unlock(&sleep_mut);
   pthread_cond_destroy(&sleep_cond);
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

inline static int pegasus_gettimeofday(struct timeval *tv)
{
   return gettimeofday(tv,NULL);
}

inline void exit_thread(PEGASUS_THREAD_RETURN rc)
{
  pthread_exit(rc);
}

inline PEGASUS_THREAD_TYPE pegasus_thread_self(void)
{
   pthread_t pt = pthread_self();
   return (* ((PEGASUS_THREAD_TYPE *) &pt));
}

inline void destroy_thread(PEGASUS_THREAD_TYPE th, PEGASUS_THREAD_RETURN rc)
{
   pthread_cancel(*(pthread_t *) &th);

}


PEGASUS_NAMESPACE_END

#endif // IPCzOSInclude
