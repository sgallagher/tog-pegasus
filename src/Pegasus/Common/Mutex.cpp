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
// Author: Mike Day (mdday@us.ibm.com)
//
// Modified By: Markus Mueller
//              Ramnath Ravindran (Ramnath.Ravindran@compaq.com)
//              David Eger (dteger@us.ibm.com)
//              Amit K Arora, IBM (amita@in.ibm.com) for PEP#101
//              Sean Keenan, Hewlett-Packard Company (sean.keenan@hp.com)
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//              David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
//              Aruran, IBM (ashanmug@in.ibm.com) for BUG# 3518
//
// Reworked By: Mike Brasher (m.brasher@inovadevelopment.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include "Mutex.h"
#include "Time.h"
#include "PegasusAssert.h"

PEGASUS_NAMESPACE_BEGIN

//==============================================================================
//
// PEGASUS_HAVE_PTHREADS
//
//==============================================================================

#if defined(PEGASUS_HAVE_PTHREADS)

Mutex::Mutex()
{
    pthread_mutexattr_init(&_rep.attr);

    // If your platform does not define PTHREAD_MUTEX_RECURSIVE, try
    // PTHREAD_MUTEX_RECURSIVE_NP.
    pthread_mutexattr_settype(&_rep.attr, PTHREAD_MUTEX_RECURSIVE);

    pthread_mutex_init(&_rep.mutex, &_rep.attr);
    _rep.owner.clear();
}

Mutex::~Mutex()
{
   PEGASUS_DEBUG_ASSERT(_magic);

    if (pthread_mutex_destroy(&_rep.mutex) == 0)
        pthread_mutexattr_destroy(&_rep.attr);
}

void Mutex::lock(ThreadType caller)
{
    PEGASUS_DEBUG_ASSERT(_magic);

    switch (pthread_mutex_lock(&_rep.mutex))
    {
        case 0:
            _rep.owner = caller;
            break;

        case EDEADLK:
            throw Deadlock(_rep.owner);

        default:
            throw WaitFailed(_rep.owner);
    }
}

void Mutex::try_lock(ThreadType caller)
{
    PEGASUS_DEBUG_ASSERT(_magic);

    switch (pthread_mutex_trylock(&_rep.mutex))
    {
        case 0:
            _rep.owner = caller;
            break;

        case EBUSY:
            throw AlreadyLocked(_rep.owner);

        case EDEADLK:
            throw Deadlock(_rep.owner);

        default:
            throw WaitFailed(_rep.owner);
    }
}

void Mutex::timed_lock(Uint32 milliseconds, ThreadType caller)
{
    PEGASUS_DEBUG_ASSERT(_magic);

    struct timeval now;
    struct timeval finish;
    struct timeval remaining;
    {
        Uint32 usec;
        gettimeofday(&finish, NULL);
        finish.tv_sec += (milliseconds / 1000 );
        milliseconds %= 1000;
        usec = finish.tv_usec + ( milliseconds * 1000 );
        finish.tv_sec += (usec / 1000000);
        finish.tv_usec = usec % 1000000;
    }

    for (;;)
    {
        switch (pthread_mutex_trylock(&_rep.mutex))
        {
            case 0:
                return;

            case EBUSY:
            {
                gettimeofday(&now, NULL);

                if (Time::subtract(&remaining, &finish, &now))
                    throw TimeOut(Threads::self());

                Threads::yield();
                break;
            }

            case EDEADLK:
                throw Deadlock(Threads::self());

            default:
                throw WaitFailed(Threads::self());
        }
    }
}

void Mutex::unlock()
{
    PEGASUS_DEBUG_ASSERT(_magic);

    ThreadType owner = _rep.owner;
    _rep.owner.clear();

    if (pthread_mutex_unlock(&_rep.mutex) != 0)
    {
        _rep.owner = owner;
        throw Permission(_rep.owner);
    }
}

#endif /* PEGASUS_HAVE_PTHREADS */

//==============================================================================
//
// PEGASUS_HAVE_WINDOWS_THREADS
//
//==============================================================================

#if defined(PEGASUS_HAVE_WINDOWS_THREADS)

Mutex::Mutex()
{
    _rep.handle = CreateMutex(NULL, FALSE, NULL);
    _rep.owner = ThreadType(0);
    _rep.count.clear();
}

Mutex::~Mutex()
{
    PEGASUS_DEBUG_ASSERT(_magic);

    WaitForSingleObject(_rep.handle, INFINITE);
    CloseHandle(_rep.handle);
}

void Mutex::lock(ThreadType caller)
{
    PEGASUS_DEBUG_ASSERT(_magic);

    DWORD rc = WaitForSingleObject(_rep.handle, INFINITE);

    if (rc == WAIT_FAILED)
        throw WaitFailed(_rep.owner);

    _rep.owner = caller;
    _rep.count++;
}

void Mutex::try_lock(ThreadType caller)
{
    PEGASUS_DEBUG_ASSERT(_magic);

    DWORD rc = WaitForSingleObject(_rep.handle, 0);

    if (rc == WAIT_TIMEOUT)
        throw AlreadyLocked(_rep.owner);

    if (rc == WAIT_FAILED)
        throw WaitFailed(_rep.owner);

    _rep.owner = caller;
    _rep.count++;
}

void Mutex::timed_lock(Uint32 milliseconds , ThreadType caller)
{
    PEGASUS_DEBUG_ASSERT(_magic);

    DWORD rc = WaitForSingleObject(_rep.handle, milliseconds);

    if (rc == WAIT_TIMEOUT)
        throw TimeOut(_rep.owner);

    if (rc == WAIT_FAILED)
        throw WaitFailed(_rep.owner);

    _rep.owner = caller;
    _rep.count++;
}

void Mutex::unlock()
{
    PEGASUS_DEBUG_ASSERT(_magic);

    _rep.owner.clear();
    _rep.count--;
    ReleaseMutex(_rep.handle);
}

#endif /* PEGASUS_HAVE_WINDOWS_THREADS */

PEGASUS_NAMESPACE_END
