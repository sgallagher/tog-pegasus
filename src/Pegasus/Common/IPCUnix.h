// Ensure Unix 98
#ifdef PEGASUS_PLATFORM_LINUX_IX86_GNU
#define _GNU_SOURCE
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

#define PEGASUS_MUTEX_TYPE pthread_mutex_t

#define PEGASUS_COND_HANDLE struct \
{\
pthread_cond_t cond;\
pthread_t owner;\
}

#define PEGASUS_SEM_HANDLE struct \
{\
sem_t sem;\
pthread_t owner;\
}

#define PEGASUS_MUTEX_HANDLE struct \
{\
pthread_mutex_t mut;\
pthread_mutexattr_t mutatt;\
pthread_t owner;\
}

#define PEGASUS_RWLOCK_HANDLE struct \
{\
pthread_rwlock_t rwlock;\
pthread_t owner;\
}

#ifdef PEGASUS_PLATFORM_HPUX_PARISC_ACC
# define PEGASUS_CLEANUP_HANDLE void *
#else
# define PEGASUS_CLEANUP_HANDLE struct _pthread_cleanup_buffer
#endif

#define PEGASUS_THREAD_RETURN void *
#define PEGASUS_THREAD_CDECL
#define PEGASUS_THREAD_HANDLE struct {\
pthread_t thid;\
pthread_attr_t thatt;\
}

