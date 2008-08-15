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
    while ((r = pthread_rwlock_destroy(&_rwlock.rwlock)) == EBUSY ||
           (r == -1 && errno == EBUSY))
    {
        Threads::yield();
    }
}

void ReadWriteSem::_wait(Boolean writeLock, ThreadType caller)
{
    if (writeLock)
    {
        if (0 == pthread_rwlock_wrlock(&_rwlock.rwlock))
        {
            _rwlock.owner = caller;
            _writers++;
            return;
        }
    }
    else
    {
        if (0 == pthread_rwlock_rdlock(&_rwlock.rwlock))
        {
            _readers++;
            return;
        }
    }

    throw WaitFailed(Threads::self());
}

void ReadWriteSem::_unlock(Boolean writeLock, ThreadType caller)
{
    ThreadType owner;

    if (writeLock)
    {
        owner = _rwlock.owner;
        Threads::clear(_rwlock.owner);
    }
    if (0 != pthread_rwlock_unlock(&_rwlock.rwlock))
    {
        _rwlock.owner = owner;
        throw(Permission(Threads::self()));
    }
    if (!writeLock && _readers.get() != 0)
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

//---------------------------------------------------------------------
void ReadWriteSem::_wait(Boolean writeLock, ThreadType caller)
{
//-----------------------------------------------------------------
// Lock this object to maintain integrity while we decide
// exactly what to do next.
//-----------------------------------------------------------------
    _rwlock._internal_lock.lock();

    if (writeLock)
    {
//-----------------------------------------------------------------
// Write Lock Step 1: lock the object and allow all the readers to exit
//-----------------------------------------------------------------
        while (_readers.get() > 0)
            Threads::yield();
//-----------------------------------------------------------------
// Write Lock Step 2: Obtain the Write Mutex
//  Although there are no readers, there may be a writer
//-----------------------------------------------------------------
        try
        {
            _rwlock._wlock.lock();
        }
        catch (const IPCException&)
        {
            _rwlock._internal_lock.unlock();
            throw;
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
    }
    else
    {
//-----------------------------------------------------------------
// Read Lock Step 1: Wait for the existing writer (if any) to clear
//-----------------------------------------------------------------
        while (_writers.get() > 0)
            Threads::yield();
//-----------------------------------------------------------------
// Read Lock Step 2: wait for a reader slot to open up, then return
//  At this point there are no writers, but there may be too many
//  readers.
//-----------------------------------------------------------------
        try
        {
            _rwlock._rlock.wait();
        }
        catch (const IPCException&)
        {
            _rwlock._internal_lock.unlock();
            throw;
        }
//-----------------------------------------------------------------
// Read Lock Step 3: increment the number of readers, unlock the object,
// return
//-----------------------------------------------------------------
        _readers++;
        _rwlock._internal_lock.unlock();
    }
}

void ReadWriteSem::_unlock(Boolean writeLock, ThreadType caller)
{
    if (writeLock && _writers.get() != 0)
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
