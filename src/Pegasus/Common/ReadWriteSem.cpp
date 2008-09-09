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

ReadWriteSem::ReadWriteSem()
{
    pthread_rwlock_init(&_rwlock.rwlock, NULL);
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

void ReadWriteSem::waitRead()
{
    if (pthread_rwlock_rdlock(&_rwlock.rwlock) != 0)
    {
        throw WaitFailed(Threads::self());
    }
}

void ReadWriteSem::waitWrite()
{
    if (pthread_rwlock_wrlock(&_rwlock.rwlock) != 0)
    {
        throw WaitFailed(Threads::self());
    }
}

void ReadWriteSem::unlockRead()
{
    int rc = pthread_rwlock_unlock(&_rwlock.rwlock);
    // All documented error codes represent coding errors.
    PEGASUS_ASSERT(rc == 0);
}

void ReadWriteSem::unlockWrite()
{
    int rc = pthread_rwlock_unlock(&_rwlock.rwlock);
    // All documented error codes represent coding errors.
    PEGASUS_ASSERT(rc == 0);
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

ReadWriteSem::ReadWriteSem() : _rwlock()
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
    while (_rwlock._readers.get() > 0 || _rwlock._writers.get() > 0)
    {
        Threads::yield();
    }
    _rwlock._internal_lock.unlock();
}

void ReadWriteSem::waitRead()
{
    // Lock the internal mutex to ensure only one waiter is processed at a time.
    AutoMutex lock(_rwlock._internal_lock);

    // Wait for the existing writer (if any) to clear.
    while (_rwlock._writers.get() > 0)
    {
        Threads::yield();
    }

    // Wait for a reader slot to open up.
    _rwlock._rlock.wait();

    // Increment the number of readers.
    _rwlock._readers++;
}

void ReadWriteSem::waitWrite()
{
    // Lock the internal mutex to ensure only one waiter is processed at a time.
    AutoMutex lock(_rwlock._internal_lock);

    // Allow all the readers to exit.
    while (_rwlock._readers.get() > 0)
    {
        Threads::yield();
    }

    // Obtain the write mutex.
    _rwlock._wlock.lock();

    // Set the writer count to one.
    _rwlock._writers = 1;
}

void ReadWriteSem::unlockRead()
{
    PEGASUS_ASSERT(_rwlock._readers.get() > 0);
    _rwlock._readers--;
    _rwlock._rlock.signal();
}

void ReadWriteSem::unlockWrite()
{
    PEGASUS_ASSERT(_rwlock._writers.get() == 1);
    _rwlock._writers = 0;
    _rwlock._wlock.unlock();
}

#endif /* !PEGASUS_USE_SEMAPHORE_RWLOCK */

PEGASUS_NAMESPACE_END
