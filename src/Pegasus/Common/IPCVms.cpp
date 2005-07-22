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
// Modified By: Mike Day (mdday@us.ibm.com) -- added native implementation
// of AtomicInt class, exceptions
//          Ramnath Ravindran (Ramnath.Ravindran@compaq.com)
//          David Eger (dteger@us.ibm.com)
//          Amit K Arora, IBM (amita@in.ibm.com) for PEP#101
//          David Dillard, VERITAS Software Corp.
//              (david.dillard@veritas.com)
//          Sean Keenan, Hewlett-Packard Company (sean.keenan@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

PEGASUS_NAMESPACE_BEGIN

#define SEM_VALUE_MAX 0x0000ffff

Mutex::Mutex()
{
  pthread_mutexattr_init(&_mutex.mutatt);
  pthread_mutex_init(&_mutex.mut, NULL);
  _mutex.owner = 0;
}

Mutex::Mutex(int mutex_type)
{
  pthread_mutexattr_init(&_mutex.mutatt);
  pthread_mutex_init(&_mutex.mut, &_mutex.mutatt);
  _mutex.owner = 0;
}

// to be able share the mutex between different condition variables
Mutex::Mutex(const Mutex & mutex)
{
  // only copy the handle, not the entire object.
  // avoid calling the destructor twice.
  _mutex.mut = mutex._mutex.mut;
  _mutex.owner = 0;
}

Mutex::~Mutex()
{
  if (0 == pthread_mutex_destroy(&_mutex.mut))
    pthread_mutexattr_destroy(&_mutex.mutatt);
}

// block until gaining the lock - throw a deadlock
// exception if process already holds the lock

void Mutex::lock(PEGASUS_THREAD_TYPE caller)
{
  int errorcode;
  if (0 == (errorcode = pthread_mutex_lock(&(_mutex.mut))))
  {
    _mutex.owner = caller;
    return;
  }
  if (errorcode == EDEADLK)
  {
    throw(Deadlock(_mutex.owner));
  }
  else
  {
    throw(WaitFailed(_mutex.owner));
  }
}

// try to gain the lock - lock succeeds immediately if the
// mutex is not already locked. throws an exception and returns
// immediately if the mutex is currently locked.

void Mutex::try_lock(PEGASUS_THREAD_TYPE caller)
{
  int errorcode;
  if (0 == (errorcode = pthread_mutex_trylock(&_mutex.mut)))
  {
    _mutex.owner = caller;
    return;
  }
  else if (errorcode == EBUSY)
  {
    throw(AlreadyLocked(_mutex.owner));
  }
  else if (errorcode == EDEADLK)
  {
    throw(Deadlock(_mutex.owner));
  }
  else
  {
    throw(WaitFailed(_mutex.owner));
  }
}

// wait for milliseconds and throw an exception then return if the wait
// expires without gaining the lock. Otherwise return without throwing an
// exception.

// Note: I was unable to get the expected behavior using pthread_mutex_timedlock.
// I don't know excactly why, but the locks were never timing out. Reimplemting
// using pthread_mutex_trylock works reliably. The documentation says that
// pthread_mutex_timedlock works with error checking mutexes but works
// just like pthread_mutex_lock (i.e., it never times out) with other
// kinds of mutexes. I couldn't determine whether or not it actually
// works with any type of mutex other than PTHREAD_MUTEX_TIMED_NP.
// However, we want the mutexes to be error checking whenever possible
// mdday Sun Aug  5 13:08:43 2001

// pthread_mutex_timedlock is not supported on HUPX
// mdday Sun Aug  5 14:12:22 2001

void Mutex::timed_lock(Uint32 milliseconds, PEGASUS_THREAD_TYPE caller)
{

  struct timeval now,
    finish,
    remaining;
  int errorcode;

  Uint32 usec;

  gettimeofday(&finish, NULL);
  finish.tv_sec += (milliseconds / 1000);
  milliseconds %= 1000;
  usec = finish.tv_usec + (milliseconds * 1000);
  finish.tv_sec += (usec / 1000000);
  finish.tv_usec = usec % 1000000;

  while (1)
  {
    errorcode = pthread_mutex_trylock(&_mutex.mut);
    if (errorcode == 0)
    {
      break;
    }

    if (errorcode == EBUSY)
    {
      gettimeofday(&now, NULL);
      if (timeval_subtract(&remaining, &finish, &now))
      {
	throw TimeOut(pegasus_thread_self());
      }
      pegasus_yield();
      continue;
    }
    if (errorcode == EDEADLK)
    {
      throw Deadlock(pegasus_thread_self());
    }
    throw WaitFailed(pegasus_thread_self());
  }
}

// unlock the mutex

void Mutex::unlock()
{
  PEGASUS_THREAD_TYPE m_owner = _mutex.owner;
  _mutex.owner = 0;
  if (0 != pthread_mutex_unlock(&_mutex.mut))
  {
    _mutex.owner = m_owner;
    throw(Permission(_mutex.owner));
  }
}

#ifdef PEGASUS_READWRITE_NATIVE

//-----------------------------------------------------------------
/// Native Implementation of Read/Write semaphore
//-----------------------------------------------------------------

ReadWriteSem:: ReadWriteSem():_readers(0), _writers(0)
{
  pthread_rwlock_init(&_rwlock.rwlock, NULL);
  _rwlock.owner = 0;
}

ReadWriteSem::~ReadWriteSem()
{

  while (EBUSY == pthread_rwlock_destroy(&_rwlock.rwlock))
  {
    pegasus_yield();
  }
}

void ReadWriteSem::wait(Uint32 mode, PEGASUS_THREAD_TYPE caller)
{
  int errorcode;
  if (mode == PEG_SEM_READ)
  {
    if (0 == (errorcode = pthread_rwlock_rdlock(&_rwlock.rwlock)))
    {
      _readers++;
      return;
    }
  }
  else if (mode == PEG_SEM_WRITE)
  {
    if (0 == (errorcode = pthread_rwlock_wrlock(&_rwlock.rwlock)))
    {
      _rwlock.owner = caller;
      _writers++;
      return;
    }
  }
  else
    throw(Permission(pegasus_thread_self()));

  if (errorcode == EDEADLK)
  {
    throw(Deadlock(_rwlock.owner));
  }
  else
  {
    throw(WaitFailed(pegasus_thread_self()));
  }
}

void ReadWriteSem::try_wait(Uint32 mode, PEGASUS_THREAD_TYPE caller)
{
  int errorcode = 0;
  if (mode == PEG_SEM_READ)
  {
    if (0 == (errorcode = pthread_rwlock_tryrdlock(&_rwlock.rwlock)))
    {
      _readers++;
      return;
    }
  }
  else if (mode == PEG_SEM_WRITE)
  {
    if (0 == (errorcode = pthread_rwlock_trywrlock(&_rwlock.rwlock)))
    {
      _writers++;
      _rwlock.owner = caller;
      return;
    }
  }
  else
  {
    throw(Permission(pegasus_thread_self()));
  }

  if (errorcode == EBUSY)
  {
    throw(AlreadyLocked(_rwlock.owner));
  }
  else if (errorcode == EDEADLK)
  {
    throw(Deadlock(_rwlock.owner));
  }
  else
  {
    throw(WaitFailed(pegasus_thread_self()));
  }
}

// timedrdlock and timedwrlock are not supported on HPUX
// mdday Sun Aug  5 14:21:00 2001
void ReadWriteSem::timed_wait(Uint32 mode, PEGASUS_THREAD_TYPE caller, int milliseconds)
{
  int errorcode = 0,
    timeout;
  struct timeval now,
    finish,
    remaining;
  Uint32 usec;

  gettimeofday(&finish, NULL);
  finish.tv_sec += (milliseconds / 1000);
  milliseconds %= 1000;
  usec = finish.tv_usec + (milliseconds * 1000);
  finish.tv_sec += (usec / 1000000);
  finish.tv_usec = usec % 1000000;

  if (mode == PEG_SEM_READ)
  {
    do
    {
      errorcode = pthread_rwlock_tryrdlock(&_rwlock.rwlock);
      gettimeofday(&now, NULL);
    }
    while (errorcode == EBUSY &&
	 (0 == (timeout = timeval_subtract(&remaining, &finish, &now))));
    if (0 == errorcode)
    {
      _readers++;
      return;
    }
  }
  else if (mode == PEG_SEM_WRITE)
  {
    do
    {
      errorcode = pthread_rwlock_trywrlock(&_rwlock.rwlock);
      gettimeofday(&now, NULL);
    }
    while (errorcode == EBUSY &&
	 (0 == (timeout = timeval_subtract(&remaining, &finish, &now))));

    if (0 == errorcode)
    {
      _writers++;
      _rwlock.owner = caller;
      return;
    }
  }
  else
  {
    throw(Permission(pegasus_thread_self()));
  }
  if (timeout != 0)
  {
    throw(TimeOut(_rwlock.owner));
  }
  else if (errorcode == EDEADLK)
  {
    throw(Deadlock(_rwlock.owner));
  }
  else
  {
    throw(WaitFailed(pegasus_thread_self()));
  }
}

void ReadWriteSem::unlock(Uint32 mode, PEGASUS_THREAD_TYPE caller)
{
  PEGASUS_THREAD_TYPE owner;

  if (mode == PEG_SEM_WRITE)
  {
    owner = _rwlock.owner;
    _rwlock.owner = 0;
  }
  if (0 != pthread_rwlock_unlock(&_rwlock.rwlock))
  {
    _rwlock.owner = owner;
    throw(Permission(pegasus_thread_self()));
  }
  if (mode == PEG_SEM_READ && _readers.value() != 0)
  {
    _readers--;
  }
  else if (_writers.value() != 0)
  {
    _writers--;
  }
}

int ReadWriteSem::read_count() const
{
  return (_readers.value());
}

int ReadWriteSem::write_count() const
{
  return (_writers.value());
}

#endif				// PEGASUS_READWRITE_NATIVE
//-----------------------------------------------------------------
// END of native read/write implementation for unix
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Native implementation of Conditional semaphore object
//-----------------------------------------------------------------

#ifdef PEGASUS_CONDITIONAL_NATIVE

// Note: I felt uncomfortable exposing the condition mutex outside
// of the class so I defined method calls to lock and unlock the
// mutex object. This protects the (hidden) conditional mutex from
// being called outside of the control of the object.

// Further, the use model of conditions seems to require locking the object,
// examining the state of the condition variable while that variable is
// protected from other threads, and then determining whether to signal or
//    wait on the condition variable. Then afterwards explicitly unlocking
// the condition object.

// So I commented out the method calls that do all three operations
// without examining the state of the condition variable.
// i.e., lock, signal, unlock or lock, wait, unlock.

//    The method calls I commented out are: wait, signal, time_wait.
// mdday Sun Aug  5 13:19:30 2001

/// Conditions are implemented as process-wide condition variables

Condition::Condition():_disallow(0)
{
  _cond_mutex.reset(new Mutex());
  _destroy_mut = true;
  pthread_cond_init((PEGASUS_COND_TYPE *) & _condition, 0);
}

Condition:: Condition(Mutex & mutex):_disallow(0)
{
  _cond_mutex.reset(&mutex);
  _destroy_mut = false;
  pthread_cond_init((PEGASUS_COND_TYPE *) & _condition, 0);
}

Condition::~Condition()
{
  _disallow++;
  while (EBUSY == pthread_cond_destroy(&_condition))
  {
    pthread_cond_broadcast(&_condition);
    pegasus_yield();
  }
  if (_destroy_mut == true)
  {
    _cond_mutex.reset();
  }
  else
  {
    _cond_mutex.release();
  }
}

void Condition::signal(PEGASUS_THREAD_TYPE caller)
{
  _cond_mutex->lock(caller);
  pthread_cond_broadcast(&_condition);
  _cond_mutex->unlock();
}

void Condition::unlocked_signal(PEGASUS_THREAD_TYPE caller)
{
  if (_cond_mutex->get_owner() != caller)
  {
    throw Permission(_cond_mutex->get_owner());
  }
  pthread_cond_broadcast(&_condition);
}

void Condition::lock_object(PEGASUS_THREAD_TYPE caller)
{
  if (_disallow.value() > 0)
  {
    throw ListClosed();
  }
  _cond_mutex->lock(caller);
}

void Condition::try_lock_object(PEGASUS_THREAD_TYPE caller)
{
  if (_disallow.value() > 0)
  {
    throw ListClosed();
  }
  _cond_mutex->try_lock(caller);
}

void Condition::wait_lock_object(PEGASUS_THREAD_TYPE caller, int milliseconds)
{
  if (_disallow.value() > 0)
  {
    throw ListClosed();
  }
  _cond_mutex->timed_lock(milliseconds, caller);
  if (_disallow.value() > 0)
  {
    _cond_mutex->unlock();
    throw ListClosed();
  }
}

void Condition::unlock_object()
{
  _cond_mutex->unlock();
}

// block until this semaphore is in a signalled state

void Condition::unlocked_wait(PEGASUS_THREAD_TYPE caller)
{
  // The caller must own the Mutex in order to wait on the Condition
  if (_cond_mutex->get_owner() != caller)
  {
    throw Permission(_cond_mutex->get_owner());
  }

  if (_disallow.value() > 0)
  {
    _cond_mutex->unlock();
    throw ListClosed();
  }

  // pthread_cond_timedwait will release the Mutex
  _cond_mutex->_set_owner(0);

  pthread_cond_wait(&_condition, &_cond_mutex->_mutex.mut);

  // The caller holds the Mutex again when pthread_cond_timedwait returns
  _cond_mutex->_set_owner(caller);
}

// block until this semaphore is in a signalled state

void Condition::unlocked_timed_wait(
				     int milliseconds,
				     PEGASUS_THREAD_TYPE caller)
{
  // The caller must own the Mutex in order to wait on the Condition
  if (_cond_mutex->get_owner() != caller)
  {
    throw Permission(_cond_mutex->get_owner());
  }

  if (_disallow.value() > 0)
  {
    _cond_mutex->unlock();
    throw ListClosed();
  }
  struct timeval now;
  struct timespec waittime;
  gettimeofday(&now, NULL);
  waittime.tv_sec = now.tv_sec;
  waittime.tv_nsec = now.tv_usec + (milliseconds * 1000);	// microseconds
  waittime.tv_sec += (waittime.tv_nsec / 1000000);	// roll overflow into
  waittime.tv_nsec = (waittime.tv_nsec % 1000000);	// the "seconds" part
  waittime.tv_nsec = waittime.tv_nsec * 1000;	// convert to nanoseconds

  // pthread_cond_timedwait will release the Mutex
  _cond_mutex->_set_owner(0);

  int retcode = pthread_cond_timedwait(
		       &_condition, &_cond_mutex->_mutex.mut, &waittime);

  // The caller holds the Mutex again when pthread_cond_timedwait returns
  _cond_mutex->_set_owner(caller);

  if (retcode == ETIMEDOUT)
  {
    throw TimeOut(caller);
  }
  else if (retcode != EINTR)
  {
    throw WaitFailed(caller);
  }
}
#endif // native conditional semaphore

//-----------------------------------------------------------------
// END of native conditional semaphore implementation
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Native implementation of semaphore object
//-----------------------------------------------------------------

//
// implementation as used in ACE derived from Mutex + Condition Variable
//

Semaphore::Semaphore(Uint32 initial)
{
  pthread_mutex_init(&_semaphore.mutex, NULL);
  pthread_cond_init(&_semaphore.cond, NULL);
  if (initial > SEM_VALUE_MAX)
  {
    _count = SEM_VALUE_MAX - 1;
  }
  else
  {
    _count = initial;
  }
  _semaphore.owner = pegasus_thread_self();
  _semaphore.waiters = 0;
}

Semaphore::~Semaphore()
{
  pthread_mutex_lock(&_semaphore.mutex);
  while (EBUSY == pthread_cond_destroy(&_semaphore.cond))
  {
    pthread_mutex_unlock(&_semaphore.mutex);
    pegasus_yield();
    pthread_mutex_lock(&_semaphore.mutex);
  }
  pthread_mutex_unlock(&_semaphore.mutex);
  pthread_mutex_destroy(&_semaphore.mutex);
}

// block until this semaphore is in a signalled state or
// throw an exception if the wait fails

void Semaphore::wait(Boolean ignoreInterrupt)
{
  // Acquire mutex to enter critical section.

  pthread_mutex_lock(&_semaphore.mutex);

  // Keep track of the number of waiters so that <sema_post> works correctly.

  _semaphore.waiters++;

  // Wait until the semaphore count is > 0, then atomically release
  // <lock_> and wait for <count_nonzero_> to be signaled.

  while (_count == 0)
  {
    pthread_cond_wait(&_semaphore.cond, &_semaphore.mutex);
  }

  // <_semaphore.mutex> is now held.

  // Decrement the waiters count.

  _semaphore.waiters--;

  // Decrement the semaphore's count.

  _count--;

  // Release mutex to leave critical section.

  pthread_mutex_unlock(&_semaphore.mutex);
}

void Semaphore::try_wait()
{
// not implemented

  throw(WaitFailed(_semaphore.owner));
}

void Semaphore::time_wait(Uint32 milliseconds)
{
  // Acquire mutex to enter critical section.

  pthread_mutex_lock (&_semaphore.mutex);

  // Keep track of the number of waiters so that <sema_post> works correctly.

  _semaphore.waiters++;

  struct timeval now = {0,0};
  struct timespec waittime = {0,0};
  int retcode = 0;
  gettimeofday(&now, NULL);
  waittime.tv_sec = now.tv_sec;
  waittime.tv_nsec = now.tv_usec + (milliseconds * 1000);  // microseconds
  waittime.tv_sec += (waittime.tv_nsec / 1000000);  // roll overflow into
  waittime.tv_nsec = (waittime.tv_nsec % 1000000);  // the "seconds" part
  waittime.tv_nsec = waittime.tv_nsec * 1000;  // convert to nanoseconds

  // We are in a sense also sending a signal - as in the Semaphore is released
  // after the time has elapsed.

  int old_count =_count;

  retcode = pthread_cond_timedwait(&_semaphore.cond, &_semaphore.mutex, &waittime) ;

  if (_count != old_count)
  {
    _count=old_count;
  }

  // Decrement the waiters count.

  _semaphore.waiters--;

  pthread_mutex_unlock (&_semaphore.mutex);
}

// increment the count of the semaphore

void Semaphore::signal()
{
  pthread_mutex_lock(&_semaphore.mutex);

  // Always allow one thread to continue if it is waiting.

  if (_semaphore.waiters > 0)
  {
    pthread_cond_signal(&_semaphore.cond);
  }

  // Increment the semaphore's count.

  _count++;

  pthread_mutex_unlock(&_semaphore.mutex);
}

// return the count of the semaphore

int Semaphore::count() const
{
  return _count;
}

PEGASUS_NAMESPACE_END
