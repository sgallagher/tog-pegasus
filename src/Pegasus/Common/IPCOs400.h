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
// Author: Chuck Carmack (carmack@us.ibm.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef IPC_OS400_include
#define IPC_OS400_include

#define _MULTI_THREADED  // must be before pthread.h

#include <limits.h>      // has SEM_VALUE_MAX
#include <sched.h>
#include <semaphore.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>
#include <sys/time.h>
#include <time.h>
//#include <sys/timex.h>
#include <unistd.h>

//#define SEM_VALUE_MAX 0xffffffff - defined in limits.h

PEGASUS_NAMESPACE_BEGIN

//typedef sem_t PEGASUS_SEMAPHORE_TYPE;

//typedef pthread_t PEGASUS_THREAD_TYPE;

// Redefine PEGASUS_THREAD_TYPE as a class on OS/400.  On other platforms
// this is an int and Pegasus sets this to 0 to indicate
// that the thread is 'dead'.  Also, we cannot use == operator
// on pthread_t, but need to call pthread_equal( ), so the ==
// operation is overloaded.  Other operators are overloaded in this file.
typedef class __pegasus_os400_thread_type {
  public:
    pthread_t thid;           // An opaque struct on OS/400
    PEGASUS_UINT64 pegasusValue; // Needed because Pegasus sets PEGASUS_THREAD_TYPEs to 0
                              // to indicate the thread is dead, but
                              // pthread_t structs cannot be changed on OS/400.

   // Overloaded operators.
    __pegasus_os400_thread_type& operator=(const PEGASUS_UINT64 & rval);
    operator pthread_t*();
    operator pthread_t();
    operator Uint32 ();
} PEGASUS_THREAD_TYPE;

typedef pthread_mutex_t PEGASUS_MUTEX_TYPE;

typedef struct {
    sem_t sem;
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

/* Gerarda - commented out the following code */
/*
#define PEGASUS_READWRITE_NATIVE

typedef struct {
     pthread_rwlock_t rwlock;
     PEGASUS_THREAD_TYPE owner;
} PEGASUS_RWLOCK_HANDLE;
*/


inline void pegasus_yield(void)
{
      sched_yield();
}

// pthreads cancellation calls 
inline void disable_cancel(void)
{
   pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
}

inline void enable_cancel(void)
{
   pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
}


inline void pegasus_sleep(int msec)
{
   /* Gerarda - redid the sleep routine */ 
   int loop;
   int microsecs = msec * 1000; /* convert from milliseconds to microseconds */

   if (microsecs < 1000000)
       usleep(microsecs);
   else
   {
       loop = microsecs / 1000000;
       for(int i = 0; i < loop; i++)
	   usleep(1000000);
       if ((loop*1000000) < microsecs)
	   usleep(microsecs - (loop*1000000));
   }

   /* Turns out the following code does not induce sleep on OS400 */ 
   /*
   struct timespec wait;
   pthread_mutex_t sleep_mut = PTHREAD_MUTEX_INITIALIZER;
   pthread_cond_t sleep_cond;
   pthread_cond_init (&sleep_cond, NULL);
   pthread_mutex_lock(&sleep_mut);
   wait.tv_sec = msec / 1000;
   wait.tv_nsec = (msec % 1000) * 1000000;
   pthread_cond_timedwait(&sleep_cond,&sleep_mut,&wait);
   pthread_mutex_unlock(&sleep_mut);
   pthread_cond_destroy(&sleep_cond);
   */
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

// Overload pegasus_thread_self to construct our
// PEGASUS_THREAD_TYPE with all the variables set.
inline PEGASUS_THREAD_TYPE pegasus_thread_self(void)
{
    // Construct a PEGASUS_THREAD_TYPE with the 
    // pegasusValue initialized to 1 (ie. the thread
    // is 'alive')
    PEGASUS_THREAD_TYPE thrd = {pthread_self(),1};
    return thrd; 
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
	return pthread_key_delete(key);
} 

inline void * pegasus_get_thread_specific(PEGASUS_THREAD_KEY_TYPE key)
{
	return pthread_getspecific(key);
} 

inline Uint32 pegasus_set_thread_specific(PEGASUS_THREAD_KEY_TYPE key,
										 void * value)
{
	return pthread_setspecific(key, value);
} 
// l10n end

inline void destroy_thread(PEGASUS_THREAD_TYPE th, PEGASUS_THREAD_RETURN rc)
{
   pthread_cancel(th.thid);

}

// Need to overload some operators because Pegasus does compares,
// and assignments using the thread type, but pthread_t is a structure
// on OS/400
inline int operator==(const PEGASUS_THREAD_TYPE & lval,
                      const PEGASUS_THREAD_TYPE & rval)
{
    return (pthread_equal(lval.thid, rval.thid) &&
            (lval.pegasusValue == rval.pegasusValue));
}

inline int operator==(const PEGASUS_THREAD_TYPE & lval,
                      const PEGASUS_UINT64 & rval)
{
   return ( lval.pegasusValue == rval);
}


inline int operator!=(const PEGASUS_THREAD_TYPE & lval,
                      const PEGASUS_THREAD_TYPE & rval)
{
    return !operator==(lval,rval);
}  


inline int operator!=(const PEGASUS_THREAD_TYPE & lval,
                      const PEGASUS_UINT64 &  rval)
{
    return !operator==(lval,rval);
}

inline int operator!=(const PEGASUS_THREAD_TYPE & lval,
                      const int &  rval)
{
    return !operator==(lval,(PEGASUS_UINT64)rval);
}

inline int operator==(const PEGASUS_THREAD_TYPE & lval,
                      const int &  rval)
{
    return operator==(lval,(PEGASUS_UINT64)rval);
}
 
inline __pegasus_os400_thread_type& __pegasus_os400_thread_type::operator=(const PEGASUS_UINT64 & rval)
{
  pegasusValue = rval;
  return *this;
}

inline __pegasus_os400_thread_type::operator pthread_t *()
{
    return &thid;
}

inline __pegasus_os400_thread_type::operator pthread_t ()
{
    return thid;
}

inline __pegasus_os400_thread_type::operator Uint32 ()
{
  pthread_id_np_t   tid;
  pthread_getunique_np(&thid, &tid);
  return (Uint32)(tid.intId.lo);
}

PEGASUS_NAMESPACE_END

#endif // IPCOS400Include
