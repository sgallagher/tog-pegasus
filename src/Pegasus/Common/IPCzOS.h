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
// Author: Markus Mueller (sedgewick_de@yahoo.de)
//
// Modified By: Mike Day (mdday@us.ibm.com)
//              Robert Kieninger, IBM (kieningr@de.ibm.com)
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
#include <stdio.h>

#define SEM_VALUE_MAX 0xffffffff

#define UNLOCKED_VALUE 0
#define LOCKED_VALUE 1

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

// !!!!!! needs to be changed to support spin_lock like behaviour
typedef PEGASUS_MUTEX_HANDLE PEGASUS_CRIT_TYPE;
//typedef cs_t PEGASUS_CRIT_TYPE;
#define PEGASUS_ATOMIC_INT_NATIVE
// define the _rep type for AtomicInt
typedef cs_t PEGASUS_ATOMIC_TYPE;

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

#define PEGASUS_CONDITIONAL_NATIVE

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

#define PEGASUS_READWRITE_NATIVE

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
	int seconds;
	int microsecs = msec * 1000; /* convert from milliseconds to microseconds */

	if (microsecs < 1000000)
	{
		usleep(microsecs/2);
        pthread_testintr();
		usleep(microsecs/2);

	}
	else
	{
		// sleep for loop seconds
		seconds = microsecs / 1000000;
		sleep(seconds);
		// Usleep the remaining time
		if ((seconds*1000000) < microsecs)
		usleep(microsecs - (seconds*1000000));
	}
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

// l10n start
typedef pthread_key_t PEGASUS_THREAD_KEY_TYPE;

inline Uint32 pegasus_key_create(PEGASUS_THREAD_KEY_TYPE * key)
{
	// Note: a destructor is not supported
	// (because not supported on Windows (?))
	return pthread_key_create(key, NULL);
}

inline Uint32 pegasus_key_delete(PEGASUS_THREAD_KEY_TYPE key)
{
	return 0;
}

inline void * pegasus_get_thread_specific(PEGASUS_THREAD_KEY_TYPE key)
{
	return pthread_getspecific_d8_np(key);
}

inline Uint32 pegasus_set_thread_specific(PEGASUS_THREAD_KEY_TYPE key,
										 void * value)
{
	return pthread_setspecific(key, value);
}
// l10n end

inline void destroy_thread(PEGASUS_THREAD_TYPE th, PEGASUS_THREAD_RETURN rc)
{
   pthread_cancel(*(pthread_t *) &th);

}

PEGASUS_NAMESPACE_END

#endif // IPCzOSInclude
