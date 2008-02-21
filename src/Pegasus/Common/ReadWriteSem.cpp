//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
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
//%/////////////////////////////////////////////////////////////////////////////

#include "ReadWriteSem.h"
#include "Time.h"
#include "PegasusAssert.h"
#include "Threads.h"

PEGASUS_NAMESPACE_BEGIN

//==============================================================================
//
// PEGASUS_USE_POSIX_RWLOCK
//
//==============================================================================

#ifdef PEGASUS_USE_POSIX_RWLOCK

ReadWriteSem::ReadWriteSem():_readers(0), _writers(0)
{
    pthread_rwlock_init(&_rwlock.rwlock, NULL);
    Threads::clear(_rwlock.owner);
}

ReadWriteSem::~ReadWriteSem()
{
    int r = 0;
    while ((r=pthread_rwlock_destroy(&_rwlock.rwlock)) == EBUSY ||
           (r == -1 && errno == EBUSY))
    {
        Threads::yield();
    }
}

void ReadWriteSem::wait(Uint32 mode, ThreadType caller)
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
        throw(Permission(Threads::self()));

    if (errorcode == EDEADLK)
        throw(Deadlock(_rwlock.owner));
    else
        throw(WaitFailed(Threads::self()));
}

void ReadWriteSem::try_wait(Uint32 mode, ThreadType caller)
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
        throw(Permission(Threads::self()));
    if (errorcode == -1)
        errorcode = errno;
    if (errorcode == EBUSY)
        throw(AlreadyLocked(_rwlock.owner));
    else if (errorcode == EDEADLK)
        throw(Deadlock(_rwlock.owner));
    else
        throw(WaitFailed(Threads::self()));
}


// timedrdlock and timedwrlock are not supported on HPUX
// mdday Sun Aug  5 14:21:00 2001
void ReadWriteSem::timed_wait(Uint32 mode,
                              ThreadType caller, int milliseconds)
{
    int errorcode = 0, timeout = 0;
    struct timeval now, finish, remaining;
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
            if (errorcode == -1)
                errorcode = errno;
            gettimeofday(&now, NULL);
        }
        while (errorcode == EBUSY &&
               (0 == (timeout = Time::subtract(&remaining, &finish, &now))));
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
            if (errorcode == -1)
                errorcode = errno;
            gettimeofday(&now, NULL);
        }
        while (errorcode == EBUSY &&
               (0 == (timeout = Time::subtract(&remaining, &finish, &now))));

        if (0 == errorcode)
        {
            _writers++;
            _rwlock.owner = caller;
            return;
        }
    }
    else
        throw(Permission(Threads::self()));
    if (timeout != 0)
        throw(TimeOut(_rwlock.owner));
    else if (errorcode == EDEADLK)
        throw(Deadlock(_rwlock.owner));
    else
        throw(WaitFailed(Threads::self()));
}

void ReadWriteSem::unlock(Uint32 mode, ThreadType caller)
{
    ThreadType owner;

    if (mode == PEG_SEM_WRITE)
    {
        owner = _rwlock.owner;
        Threads::clear(_rwlock.owner);
    }
    if (0 != pthread_rwlock_unlock(&_rwlock.rwlock))
    {
        _rwlock.owner = owner;
        throw(Permission(Threads::self()));
    }
    if (mode == PEG_SEM_READ && _readers.get() != 0)
        _readers--;
    else if (_writers.get() != 0)
        _writers--;
}

int ReadWriteSem::read_count() const
{
    return _readers.get();
}

int ReadWriteSem::write_count() const
{
    return _writers.get();
}

#endif /* PEGASUS_USE_POSIX_RWLOCK */

//==============================================================================
//
// PEGASUS_USE_SEMAPHORE_RWLOCK
//
//==============================================================================

#if defined(PEGASUS_USE_SEMAPHORE_RWLOCK)

// // If i get cancelled, I MUST ensure:
// 1) I do not hold the internal mutex
// 2) I do not hold the write lock
// 3) I am not using a reader slot

#if 0
void extricate_read_write(void *parm)
{
    ReadWriteSem *rws = (ReadWriteSem *) parm;
    ThreadType myself = Threads::self();

    if (Threads::equal(rws->_rwlock._wlock.get_owner(), myself))
        rws->_rwlock._wlock.unlock();
    else if (rws->_readers.get() > 0)
        rws->_rwlock._rlock.signal();

    if (Threads::equal(rws->_rwlock._internal_lock.get_owner(), myself))
        rws->_rwlock._internal_lock.unlock();
}
#endif


ReadWriteSem::ReadWriteSem():_readers(0), _writers(0), _rwlock()
{
}

ReadWriteSem::~ReadWriteSem()
{
    // lock everyone out of this object
    try
    {
        _rwlock._internal_lock.lock();
    }
    catch (Deadlock &)
    {
        // no problem - we own the lock, which is what we want
    }
    catch (IPCException &)
    {
        PEGASUS_ASSERT(0);
    }
    while (_readers.get() > 0 || _writers.get() > 0)
    {
        Threads::yield();
    }
    _rwlock._internal_lock.unlock();
}






//-----------------------------------------------------------------
// if milliseconds == -1, wait indefinately
// if milliseconds == 0, fast wait
//-----------------------------------------------------------------
void ReadWriteSem::timed_wait(Uint32 mode, ThreadType caller,
                              int milliseconds)
{
//-----------------------------------------------------------------
// Lock this object to maintain integrity while we decide
// exactly what to do next.
//-----------------------------------------------------------------
    // AutoPtr<IPCException> caught;
    // IPCException caught((ThreadType)0);
    // WaitFailed caughtWaitFailed((ThreadType)0);
    // TimeOut caughtTimeOut((ThreadType)0);
    // TooManyReaders caughtTooManyReaders((ThreadType)0);

    ThreadType zero;
    IPCException caught(zero);
    WaitFailed caughtWaitFailed(zero);
    TimeOut caughtTimeOut(zero);
    TooManyReaders caughtTooManyReaders(zero);

    // cleanup stack frame
    {
        // Threads::cleanup_push(extricate_read_write, this);

        try
        {
            if (milliseconds == 0)
                _rwlock._internal_lock.try_lock();
            else if (milliseconds == -1)
                _rwlock._internal_lock.lock();
            else
                _rwlock._internal_lock.timed_lock(milliseconds);
        }
        catch (const IPCException & e)
        {
            caught = e;
            goto throw_from_here;
        }

        if (mode == PEG_SEM_WRITE)
        {
//-----------------------------------------------------------------
// Write Lock Step 1: lock the object and allow all the readers to exit
//-----------------------------------------------------------------


            if (milliseconds == 0)      // fast wait
            {
                if (_readers.get() > 0)
                {
                    _rwlock._internal_lock.unlock();
                    // caught.reset(new WaitFailed(Threads::self()));
                    caughtWaitFailed = WaitFailed(Threads::self());
                    goto throw_from_here;
                }
            }
            else if (milliseconds == -1)        // infinite wait
            {
                while (_readers.get() > 0)
                    Threads::yield();
            }
            else                // timed wait
            {
                struct timeval start, now;
                Time::gettimeofday(&start);
                start.tv_usec += (1000 * milliseconds);
                while (_readers.get() > 0)
                {
                    Time::gettimeofday(&now);
                    if ((now.tv_usec > start.tv_usec) ||
                        now.tv_sec > start.tv_sec)
                    {
                        _rwlock._internal_lock.unlock();
                        // caught.reset(new TimeOut(Threads::self()));
                        caughtTimeOut = TimeOut(Threads::self());
                        goto throw_from_here;
                    }
                    Threads::yield();
                }
            }
//-----------------------------------------------------------------
// Write Lock Step 2: Obtain the Write Mutex
//  Although there are no readers, there may be a writer
//-----------------------------------------------------------------
            if (milliseconds == 0)      // fast wait
            {
                try
                {
                    _rwlock._wlock.try_lock();
                }
                catch (IPCException & e)
                {
                    _rwlock._internal_lock.unlock();
                    caught = e;
                    goto throw_from_here;
                }
            }
            else if (milliseconds == -1)        // infinite wait
            {
                try
                {
                    _rwlock._wlock.lock();
                }
                catch (const IPCException & e)
                {
                    _rwlock._internal_lock.unlock();
                    caught = e;
                    goto throw_from_here;
                }
            }
            else                // timed wait
            {
                try
                {
                    _rwlock._wlock.timed_lock(milliseconds);
                }
                catch (const IPCException & e)
                {
                    _rwlock._internal_lock.unlock();
                    caught = e;
                    goto throw_from_here;
                }
            }

//-----------------------------------------------------------------
// Write Lock Step 3: set the writer count to one, unlock the object
//   There are no readers and we are the only writer !
//-----------------------------------------------------------------
            _writers = 1;
            // set the owner
            _rwlock._owner = Threads::self();
            // unlock the object
            _rwlock._internal_lock.unlock();
        }                       // PEG_SEM_WRITE
        else
        {
//-----------------------------------------------------------------
// Read Lock Step 1: Wait for the existing writer (if any) to clear
//-----------------------------------------------------------------
            if (milliseconds == 0)      // fast wait
            {
                if (_writers.get() > 0)
                {
                    _rwlock._internal_lock.unlock();
                    // caught.reset(new WaitFailed(Threads::self()));
                    caughtWaitFailed = WaitFailed(Threads::self());
                    goto throw_from_here;
                }
            }
            else if (milliseconds == -1)        // infinite wait
            {
                while (_writers.get() > 0)
                    Threads::yield();
            }
            else                // timed wait
            {
                struct timeval start, now;
                Time::gettimeofday(&start);
                start.tv_usec += (milliseconds * 1000);

                while (_writers.get() > 0)
                {
                    Time::gettimeofday(&now);
                    if ((now.tv_usec > start.tv_usec) ||
                        (now.tv_sec > start.tv_sec))
                    {
                        _rwlock._internal_lock.unlock();
                        // caught.reset(new TimeOut(Threads::self()));
                        caughtTimeOut = TimeOut(Threads::self());
                        goto throw_from_here;
                    }
                    Threads::yield();
                    Time::gettimeofday(&now);
                }
            }

//-----------------------------------------------------------------
// Read Lock Step 2: wait for a reader slot to open up, then return
//  At this point there are no writers, but there may be too many
//  readers.
//-----------------------------------------------------------------
            if (milliseconds == 0)      // fast wait
            {
                try
                {
                    _rwlock._rlock.try_wait();
                }
                catch (const IPCException &)
                {
                    // the wait failed, there must be too many readers
                    // already.
                    // unlock the object
                    caughtTooManyReaders = TooManyReaders(Threads::self());
                    _rwlock._internal_lock.unlock();
                    // caught.reset(new TooManyReaders(Threads::self()));
                }
            }
            else if (milliseconds == -1)        // infinite wait
            {
                try
                {
                    _rwlock._rlock.wait();
                }
                catch (const IPCException & e)
                {
                    _rwlock._internal_lock.unlock();
                    caught = e;
                    goto throw_from_here;
                }
            }
            else                // timed wait
            {
                try
                {
                    _rwlock._rlock.time_wait(milliseconds);
                }
                catch (const IPCException & e)
                {
                    _rwlock._internal_lock.unlock();
                    caught = e;
                    goto throw_from_here;
                }
            }

//-----------------------------------------------------------------
// Read Lock Step 3: increment the number of readers, unlock the object,
// return
//-----------------------------------------------------------------
            _readers++;
            _rwlock._internal_lock.unlock();
        }
      throw_from_here:
        // ATTN:
        Threads::cleanup_pop(0);
    }

    if (!Threads::null(caught.get_owner()))
        throw caught;
    if (!Threads::null(caughtWaitFailed.get_owner()))
        throw caughtWaitFailed;
    if (!Threads::null(caughtTimeOut.get_owner()))
        throw caughtTimeOut;
    if (!Threads::null(caughtTooManyReaders.get_owner()))

        throw caughtTooManyReaders;
    return;
}

//---------------------------------------------------------------------
void ReadWriteSem::wait(Uint32 mode, ThreadType caller)
{
    timed_wait(mode, caller, -1);
}

void ReadWriteSem::try_wait(Uint32 mode, ThreadType caller)
{
    timed_wait(mode, caller, 0);
}


void ReadWriteSem::unlock(Uint32 mode, ThreadType caller)
{
    if (mode == PEG_SEM_WRITE && _writers.get() != 0)
    {
        _writers = 0;
        _rwlock._wlock.unlock();
    }
    else if (_readers.get() != 0)
    {
        _readers--;
        _rwlock._rlock.signal();
    }
}

int ReadWriteSem::read_count() const
{
    return _readers.get();
}

int ReadWriteSem::write_count() const
{
    return _writers.get();
}

#endif /* !PEGASUS_USE_SEMAPHORE_RWLOCK */

PEGASUS_NAMESPACE_END
