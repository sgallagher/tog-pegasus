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
#include <pthread.h>
#include <semaphore.h>
// Signal blocking
#include <signal.h>

#ifdef PEGASUS_PLATFORM_HPUX_PARISC_ACC
# include <sched.h>
  extern int pthread_mutex_timedlock(
        pthread_mutex_t *mutex,
        const struct timespec *abstime);

  extern int pthread_rwlock_timedrdlock(
        pthread_rwlock_t *rwlock,
        const struct timespec *abstime);

  extern int pthread_rwlock_timedwrlock(
        pthread_rwlock_t *rwlock,
        const struct timespec *abstime);

  extern int sem_timedwait(
        sem_t *sem,
        const struct timespec *abstime);
#endif

#include <signal.h>
#include <errno.h>
#include <sys/time.h>
#include <time.h>

typedef pthread_t PEGASUS_THREAD_TYPE;
typedef pthread_mutex_t PEGASUS_MUTEX_TYPE;
typedef pthread_cond_t PEGASUS_COND_TYPE;

typedef struct {
  pthread_cond_t cond;
  pthread_t owner;
} PEGASUS_COND_HANDLE;

typedef struct {
  sem_t sem;
  pthread_t owner;
} PEGASUS_SEM_HANDLE ;

typedef struct {
  pthread_mutex_t mut;
  pthread_mutexattr_t mutatt;
  pthread_t owner;
} PEGASUS_MUTEX_HANDLE ;

typedef struct {
  pthread_rwlock_t rwlock;
  pthread_t owner;
} PEGASUS_RWLOCK_HANDLE;


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


// linux offers a built-in integer type for atomic access
// other unix platforms HPUX, AIX, may have different types
// implementors should use the native type for faster operations
#if defined(PEGASUS_PLATFORM_LINUX_IX86_GNU)
#define PEGASUS_ATOMIC_INT_NATIVE = 1
typedef sig_atomic_t PEGASUS_ATOMIC_TYPE ;
#endif



