//%/////////////////////////////////////////////////////////////////////////////
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
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

PEGASUS_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////

Mutex::Mutex()
{
    pthread_mutexattr_init(&_mutex.mutatt);
    pthread_mutexattr_settype(&_mutex.mutatt,PTHREAD_MUTEX_ERRORCHECK);
    pthread_mutex_init(&_mutex.mut,&_mutex.mutatt);
    _mutex.owner = pthread_self();
}

Mutex::~Mutex()
{
    pthread_mutex_destroy(&_mutex.mut);
    pthread_mutexattr_destroy(&_mutex.mutatt);
}

// block until gaining the lock - throw a deadlock 
// exception if process already holds the lock 
void Mutex::lock()
{
    int errorcode;
    errorcode = pthread_mutex_lock(&_mutex.mut);
    if (errorcode == EDEADLK)
        cout << "DEADLOCK, thread " << pthread_self() << ", owner " <<
                _mutex.owner << endl;
}
  
// try to gain the lock - lock succeeds immediately if the 
// mutex is not already locked. throws an exception and returns
// immediately if the mutex is currently locked. 
Boolean Mutex::try_lock(void)
{
    int errorcode;
    errorcode = pthread_mutex_trylock(&_mutex.mut);
    pthread_mutex_trylock(&_mutex.mut);

    if (errorcode == EBUSY) return false;
    else return true;
}

// wait for milliseconds and throw an exception then return if the wait
// expires without gaining the lock. Otherwise return without throwing an
// exception. 
void Mutex::timed_lock( Uint32 milliseconds )
{
    struct timeval now;
    struct timespec timeout;
    int errorcode;

    gettimeofday(&now,NULL);
    timeout.tv_sec = now.tv_sec;
    timeout.tv_nsec += now.tv_usec * 1000 + milliseconds; 
    errorcode = pthread_mutex_timedlock(&_mutex.mut, &timeout);
    if (errorcode == ETIMEDOUT) cout << "Mutex timedlock exception\n";
}

// unlock the mutex
void Mutex::unlock()
{
    pthread_mutex_unlock(&_mutex.mut);
}

PEGASUS_MUTEX_TYPE * Mutex::getMutex()
{
    return &(_mutex.mut);
}

////////////////////////////////////////////////////////////////////////////

// ReadWriteSemaphore are best implemented through Unix 98 rwlocks

ReadWriteSem::ReadWriteSem(Uint32 mode = SEM_WRITE)
{
    pthread_rwlock_init(&_rwlock.rwlock, NULL);
    _rwlock.owner = pthread_self();
}
    
ReadWriteSem::~ReadWriteSem()
{
    pthread_rwlock_destroy(&_rwlock.rwlock);
}

void ReadWriteSem::wait(Uint32 mode)
{
    if (mode == SEM_READ)
    {
        pthread_rwlock_rdlock(&_rwlock.rwlock);
    }
    else if (mode == SEM_WRITE)
    {
        pthread_rwlock_wrlock(&_rwlock.rwlock);
    }
    else cout << "Exception - neither read nor write\n";
}

Boolean ReadWriteSem::try_wait(Uint32 mode)
{
    int errorcode = 0;
    if (mode == SEM_READ)
    {
        errorcode = pthread_rwlock_tryrdlock(&_rwlock.rwlock);
    }
    else if (mode == SEM_WRITE)
    {
        errorcode = pthread_rwlock_trywrlock(&_rwlock.rwlock);
    }
    else cout << "Exception - neither read nor write\n";

    if (errorcode == EBUSY) return false;
    else return true;
}

void ReadWriteSem::timed_wait(Uint32 mode, int milliseconds)
{
    struct timeval now;
    struct timespec timeout;
    int errorcode = 0;

    gettimeofday(&now,NULL);
    timeout.tv_sec = now.tv_sec;
    timeout.tv_nsec += now.tv_usec * 1000 + milliseconds; 

    if (mode == SEM_READ)
    {
        errorcode = pthread_rwlock_timedrdlock(&_rwlock.rwlock,
                                               &timeout);
    }
    else if (mode == SEM_WRITE)
    {
        errorcode = pthread_rwlock_timedwrlock(&_rwlock.rwlock,
                                               &timeout);
    }
    else cout << "Exception - neither read nor write\n";
    if (errorcode == ETIMEDOUT) cout << "rwlock timedlock exception\n";
}

void ReadWriteSem::unlock(Uint32 mode)
{
    pthread_rwlock_unlock(&_rwlock.rwlock);
}

int ReadWriteSem::read_count()
{
    return -1;
}

int ReadWriteSem::write_count()
{
    return -1;
}

////////////////////////////////////////////////////////////////////////////

/* Conditions are implemented as process-wide condition variables */
Condition::Condition() : _cond_mutex()
{
    pthread_cond_init(&_condition.cond,NULL);
    _condition.owner = pthread_self();
}

Condition::~Condition()
{
    pthread_cond_destroy(&_condition.cond);
    //~_cond_mutex;
}

// block until this semaphore is in a signalled state 
void Condition::wait()
{
    _cond_mutex.lock();
    pthread_cond_wait(&_condition.cond, _cond_mutex.getMutex());
    _cond_mutex.unlock();
}

void Condition::signal()
{
    _cond_mutex.lock();
    pthread_cond_broadcast(&_condition.cond);
    _cond_mutex.unlock();
}

// wait for milliseconds and throw an exception
// if wait times out without gaining the semaphore
void Condition::time_wait( Uint32 milliseconds )
{
    struct timeval now;
    struct timespec timeout;
    int retcode;

    _cond_mutex.lock();
    gettimeofday(&now,NULL);
    timeout.tv_sec = now.tv_sec;
    timeout.tv_nsec += now.tv_usec * 1000 + milliseconds; 
    retcode = 0;
    pthread_cond_timedwait(&_condition.cond, _cond_mutex.getMutex(), &timeout);
    _cond_mutex.unlock();
}

Mutex * Condition::getMutex()
{
    return &(_cond_mutex);
}

// block until this semaphore is in a signalled state 
void Condition::unlocked_wait()
{
    pthread_cond_wait(&_condition.cond, _cond_mutex.getMutex());
}

// block until this semaphore is in a signalled state 
void Condition::unlocked_timed_wait(int milliseconds)
{
    struct timeval now;
    struct timespec timeout;
    int retcode;

    gettimeofday(&now,NULL);
    timeout.tv_sec = now.tv_sec;
    timeout.tv_nsec += now.tv_usec * 1000 + milliseconds; 
    pthread_cond_timedwait(&_condition.cond, _cond_mutex.getMutex(), &timeout);
}

void Condition::unlocked_signal()
{
    pthread_cond_broadcast(&_condition.cond);
}

////////////////////////////////////////////////////////////////////////////

/* Semaphores are implemented as process-wide condition variables */
Semaphore::Semaphore(Uint32 initial)
{
    sem_init(&_semaphore.sem,0,initial);
}

Semaphore::~Semaphore()
{
    sem_destroy(&_semaphore.sem);
}

// block until this semaphore is in a signalled state
void Semaphore::wait()
{
    sem_wait(&_semaphore.sem);
}

// wait succeeds immediately if semaphore has a non-zero count, 
// return immediately and throw and exception if the 
// count is zero. 
Boolean Semaphore::try_wait()
{
    if (sem_trywait(&_semaphore.sem)) return false;
    else return true;
}

// wait for milliseconds and throw an exception
// if wait times out without gaining the semaphore
void Semaphore::time_wait( Uint32 milliseconds )
{
    struct timeval now;
    struct timespec timeout;
    int retcode;

    gettimeofday(&now,NULL);
    timeout.tv_sec = now.tv_sec;
    timeout.tv_nsec += now.tv_usec * 1000 + milliseconds; 
    retcode = 0;
    if (sem_timedwait(&_semaphore.sem, &timeout))
        cout << "Semaphore::Exception\n";
}

// increment the count of the semaphore 
void Semaphore::signal()
{
    sem_post(&_semaphore.sem);
}

// return the count of the semaphore
int Semaphore::count() 
{
    sem_getvalue(&_semaphore.sem,&_count);
    return _count;
}

////////////////////////////////////////////////////////////////////////////

cleanup_handler::cleanup_handler( void (*routine)(void *), void *arg  )
{
    _pthread_cleanup_push( &_cleanup_buffer, routine, arg);
}

cleanup_handler::~cleanup_handler()
{
    _pthread_cleanup_pop(&_cleanup_buffer,0);
}

////////////////////////////////////////////////////////////////////////////

SimpleThread::SimpleThread(
    void * (*start) (void *), void * parameter, Boolean detached) :
    _suspend()
{
    _start = start;
    _thread_parm = parameter;
    _is_detached = detached;
    pthread_attr_init(&_handle.thatt);
}

SimpleThread::~SimpleThread()
{
    if (!_is_detached)
        pthread_join(_handle.thid,NULL);
}

void SimpleThread::run()
{
    if (_is_detached)
        pthread_attr_setdetachstate(&_handle.thatt, PTHREAD_CREATE_DETACHED);
    pthread_create(&_handle.thid, &_handle.thatt, _start, _thread_parm);
}

Uint32 SimpleThread::threadId()
{
    return (Uint32) _handle.thid;
}

void * SimpleThread::get_parm(void)
{
    return _thread_parm;
}

void SimpleThread::kill(int signum)
{
    pthread_kill(_handle.thid, signum);
}

void SimpleThread::cancel()
{
    pthread_cancel(_handle.thid);
}

void SimpleThread::join(PEGASUS_THREAD_RETURN * ret_val)
{
    pthread_join(_handle.thid,ret_val);
}

void SimpleThread::test_cancel()
{
    pthread_testcancel();
}

void SimpleThread::thread_switch()
{
    pthread_yield();
}

//implemented later on using SIGUSR1
void SimpleThread::suspend()
{
    //_suspend.wait();
    pthread_kill(_handle.thid,SIGSTOP);
}

//implemented later on using SIGUSR2
void SimpleThread::resume()
{
    //_suspend.signal();
    pthread_kill(_handle.thid,SIGCONT);
}

void SimpleThread::sleep(Uint32 msec)
{
    struct timespec timeout;
    timeout.tv_sec = msec / 1000;
    timeout.tv_nsec = (msec & 1000) * 1000;
    nanosleep(&timeout,NULL);
}

#if 0
Thread::Thread(void * (*start) (void *), void * parameter, Boolean detached)
{
    _start = start;
    _thread_parm = parameter;
    _is_detached = detached;
    pthread_attr_init(&_handle.thatt);
}

Thread::~Thread()
{
    if (!_is_detached)
        pthread_join(_handle.thid,NULL);
}

void Thread::run()
{
    if (_is_detached)
        pthread_attr_setdetachstate(&_handle.thatt, PTHREAD_CREATE_DETACHED);
    pthread_create(&_handle.thid, &_handle.thatt, _start, _thread_parm);
}

void * Thread::get_parm(void)
{
    return _thread_parm;
}

void Thread::kill(int signum)
{
    pthread_kill(_handle.thid, signum);
}

void Thread::cancel()
{
    pthread_cancel(_handle.thid);
}

void Thread::test_cancel()
{
    pthread_testcancel();
}

void Thread::thread_switch()
{
    pthread_yield();
}

void Thread::suspend()
{
}

void Thread::resume()
{
}

void Thread::sleep(Uint32 msec)
{
    struct timespec timeout;
    timeout.tv_sec = msec / 1000;
    timeout.tv_nsec = (msec & 1000) * 1000;
    nanosleep(&timeout,NULL);
}
#endif

PEGASUS_NAMESPACE_END
