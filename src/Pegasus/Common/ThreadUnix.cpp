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
// Modified By: Mike Day (mdday@us.ibm.com)
//
//%/////////////////////////////////////////////////////////////////////////////


PEGASUS_NAMESPACE_BEGIN


static sigset_t *block_signal_mask(sigset_t *sig)
{
    sigemptyset(sig);
    // should not be used for main()
    sigaddset(sig, SIGHUP);
    sigaddset(sig, SIGINT); 
    // maybe useless, since KILL can't be blocked according to POSIX
    sigaddset(sig, SIGKILL);

    sigaddset(sig, SIGABRT);
    sigaddset(sig, SIGALRM);
    sigaddset(sig, SIGPIPE);

// since SIGSTOP/CONT can handle suspend()/resume() on Linux
// block them
#if defined(PEGASUS_PLATFORM_LINUX_IX86_GNU)
    sigaddset(sig, SIGUSR1);
    sigaddset(sig, SIGUSR2);
#endif
    pthread_sigmask(SIG_BLOCK, sig, NULL);
    return sig;
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
    _handle.thid = 0;
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
    pthread_create(&_handle.thid, &_handle.thatt, _start, this);
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
#ifdef PEGASUS_PLATFORM_HPUX_PARISC_ACC
    sched_yield();
#else
    pthread_yield();
#endif
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


PEGASUS_THREAD_TYPE SimpleThread::self(void) { return(_handle.thid); }

#ifdef PEGASUS_PLATFORM_HPUX_PARISC_ACC

int pthread_mutex_timedlock(
        pthread_mutex_t *mutex,
        const struct timespec *abstime)
{
        return pthread_mutex_lock (mutex);
}

int pthread_rwlock_timedrdlock(
        pthread_rwlock_t *rwlock,
        const struct timespec *abstime)
{
        return pthread_rwlock_rdlock(rwlock);
}

int pthread_rwlock_timedwrlock(
        pthread_rwlock_t *rwlock,
        const struct timespec *abstime)
{
        return pthread_rwlock_wrlock(rwlock);
}

int sem_timedwait(sem_t *sem,
        const struct timespec *abstime)
{
        return sem_wait(sem);
}

#endif

Thread::Thread( PEGASUS_THREAD_RETURN (PEGASUS_THREAD_CDECL *start )(void *),
		void *parameter, Boolean detached ) : _is_detached(detached), 
	      _cancel_enabled(false), _cancelled(false),
	      _suspend_count(), _start(start), 
	      _tsd(), _thread_parm(parameter), _exit_code(0)
{

    pthread_attr_init(&_handle.thatt);

    if( _signals_blocked == false) 
    {
      sigset_t signals_to_block;
      block_signal_mask(&signals_to_block);
    } 
    _handle.thid = 0;
}

Thread::~Thread()
{
    if (!_is_detached && _handle.thid != 0 )
        pthread_join(_handle.thid,NULL);
}

void Thread::run()
{
    if (_is_detached)
        pthread_attr_setdetachstate(&_handle.thatt, PTHREAD_CREATE_DETACHED);
    pthread_create(&_handle.thid, &_handle.thatt, _start, this);
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

void Thread::sleep(Uint32 msec)
{
  struct timespec timeout;
  timeout.tv_sec = msec / 1000;
  timeout.tv_nsec = (msec & 1000) * 1000;
  nanosleep(&timeout,NULL);
}

void Thread::join(void) { pthread_join(_handle.thid, &_exit_code) ; }

void Thread::thread_init(void)
{
  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
  pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
  _cancel_enabled = true;
}

// *****----- native thread exit routine -----***** //

#if defined(PEGASUS_PLATFORM_LINUX_IX86_GNU)
#define PEGASUS_THREAD_EXIT_NATIVE 
inline void Thread::exit_self(void *return_code) { pthread_exit(return_code) ; }
#endif

// *****----- native cleanup routines -----***** //
#if defined(PEGASUS_PLATFORM_LINUX_IX86_GNU)
#define PEGASUS_THREAD_CLEANUP_NATIVE 
void Thread::cleanup_push( void (*routine)(void *), void *parm) throw(IPCException)
{
  cleanup_handler *cu = new cleanup_handler(routine, parm);
  try { _cleanup.insert_first(cu); } 
  catch(IPCException& e) { delete cu; throw; }
  _pthread_cleanup_push(&(cu->_cleanup_buffer), routine, parm);
  return;
}

void Thread::cleanup_pop(Boolean execute = true) throw(IPCException)
{
  cleanup_handler *cu ;
  try { cu = _cleanup.remove_first() ;}
  catch(IPCException& e) { PEGASUS_ASSERT(0); }
  _pthread_cleanup_pop(&(cu->_cleanup_buffer), execute);
  delete cu;
}


#endif 




PEGASUS_NAMESPACE_END
