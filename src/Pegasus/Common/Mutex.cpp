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
// Author: Mike Brasher (m.brasher@inovadevelopment.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include "Mutex.h"
#include "Time.h"
#include "PegasusAssert.h"
#include "Once.h"

PEGASUS_NAMESPACE_BEGIN

//==============================================================================
//
// PEGASUS_HAVE_PTHREADS
//
//==============================================================================

#if defined(PEGASUS_HAVE_PTHREADS)

static Once _once = PEGASUS_ONCE_INITIALIZER;
static pthread_mutexattr_t _attr;

static void _init_attr()
{
    pthread_mutexattr_init(&_attr);
    pthread_mutexattr_settype(&_attr, PTHREAD_MUTEX_RECURSIVE);
}

Mutex::Mutex()
{
    once(&_once, _init_attr);
    pthread_mutex_init(&_rep.mutex, &_attr);
    _rep.count = 0;
}

Mutex::~Mutex()
{
    PEGASUS_DEBUG_ASSERT(_magic);
    pthread_mutex_destroy(&_rep.mutex);
    _rep.count = -1;
}

void Mutex::lock()
{
    PEGASUS_DEBUG_ASSERT(_magic);

    switch (pthread_mutex_lock(&_rep.mutex))
    {
        case 0:
            _rep.count++;
            break;

        case EDEADLK:
            throw Deadlock(ThreadType());

        default:
            throw WaitFailed(ThreadType());
    }
}

void Mutex::try_lock()
{
    PEGASUS_DEBUG_ASSERT(_magic);

    switch (pthread_mutex_trylock(&_rep.mutex))
    {
        case 0:
            _rep.count++;
            break;

        case EBUSY:
            throw AlreadyLocked(ThreadType());

        case EDEADLK:
            throw Deadlock(ThreadType());

        default:
            throw WaitFailed(ThreadType());
    }
}

void Mutex::timed_lock(Uint32 milliseconds)
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
                _rep.count++;
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
    PEGASUS_DEBUG_ASSERT(_rep.count > 0);

    if (pthread_mutex_unlock(&_rep.mutex) != 0)
        throw Permission(ThreadType());

    _rep.count--;
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
    Threads::clear(ThreadType());
    _rep.count = 0;
}

Mutex::~Mutex()
{
    PEGASUS_DEBUG_ASSERT(_magic);

    WaitForSingleObject(_rep.handle, INFINITE);
    CloseHandle(_rep.handle);
}

void Mutex::lock()
{
    PEGASUS_DEBUG_ASSERT(_magic);

    DWORD rc = WaitForSingleObject(_rep.handle, INFINITE);

    if (rc == WAIT_FAILED)
        throw WaitFailed(ThreadType());

    _rep.count++;
}

void Mutex::try_lock()
{
    PEGASUS_DEBUG_ASSERT(_magic);

    DWORD rc = WaitForSingleObject(_rep.handle, 0);

    if (rc == WAIT_TIMEOUT)
        throw AlreadyLocked(ThreadType());

    if (rc == WAIT_FAILED)
        throw WaitFailed(ThreadType());

    _rep.count++;
}

void Mutex::timed_lock(Uint32 milliseconds)
{
    PEGASUS_DEBUG_ASSERT(_magic);

    DWORD rc = WaitForSingleObject(_rep.handle, milliseconds);

    if (rc == WAIT_TIMEOUT)
        throw TimeOut(ThreadType());

    if (rc == WAIT_FAILED)
        throw WaitFailed(ThreadType());

    _rep.count++;
}

void Mutex::unlock()
{
    PEGASUS_DEBUG_ASSERT(_magic);

    Threads::clear(ThreadType());
    _rep.count--;
    ReleaseMutex(_rep.handle);
}

#endif /* PEGASUS_HAVE_WINDOWS_THREADS */

PEGASUS_NAMESPACE_END
