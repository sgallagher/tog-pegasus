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

#include "Mutex.h"
#include "Time.h"
#include "PegasusAssert.h"
#include "Once.h"

PEGASUS_NAMESPACE_BEGIN

//==============================================================================
//
// Mutex implementation with PTHREAD_MUTEX_RECURSIVE feature.
//
//==============================================================================

#if defined(PEGASUS_HAVE_PTHREADS) && defined(PEGASUS_HAVE_RECURSIVE_MUTEXES)

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
#if defined(PEGASUS_DEBUG)
    _rep.count = 0;
#endif
}

Mutex::Mutex(RecursiveTag)
{
    once(&_once, _init_attr);
    pthread_mutex_init(&_rep.mutex, &_attr);
#if defined(PEGASUS_DEBUG)
    _rep.count = 0;
#endif
}

Mutex::Mutex(NonRecursiveTag)
{
    pthread_mutex_init(&_rep.mutex, NULL);
#if defined(PEGASUS_DEBUG)
    _rep.count = 0;
#endif
}

Mutex::~Mutex()
{
    PEGASUS_DEBUG_ASSERT(_magic);
    pthread_mutex_destroy(&_rep.mutex);
}

void Mutex::lock()
{
    PEGASUS_DEBUG_ASSERT(_magic);

    switch (pthread_mutex_lock(&_rep.mutex))
    {
        case 0:
#if defined(PEGASUS_DEBUG)
            _rep.count++;
#endif
            break;

        default:
            throw WaitFailed(Threads::self());
    }
}

void Mutex::try_lock()
{
    PEGASUS_DEBUG_ASSERT(_magic);

    int r = pthread_mutex_trylock(&_rep.mutex);
    if (r == -1)
        r=errno;
    switch (r)
    {
        case 0:
#if defined(PEGASUS_DEBUG)
            _rep.count++;
#endif
            break;

        case EBUSY:
            throw AlreadyLocked(Threads::self());

        default:
            throw WaitFailed(Threads::self());
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
        int r=pthread_mutex_trylock(&_rep.mutex);
        if (r == -1)
            r = errno;
        switch (r)
        {
            case 0:
#if defined(PEGASUS_DEBUG)
                _rep.count++;
#endif
                return;

            case EBUSY:
            {
                gettimeofday(&now, NULL);

                if (Time::subtract(&remaining, &finish, &now))
                    throw TimeOut(Threads::self());

                Threads::yield();
                break;
            }

            default:
                throw WaitFailed(Threads::self());
        }
    }
}

void Mutex::unlock()
{
    PEGASUS_DEBUG_ASSERT(_magic);
    PEGASUS_DEBUG_ASSERT(_rep.count > 0);

#if defined(PEGASUS_DEBUG)
    _rep.count--;
#endif

    if (pthread_mutex_unlock(&_rep.mutex) != 0)
        throw Permission(Threads::self());
}

#if defined(PEGASUS_OS_LINUX)
void Mutex::reinitialize()
{
    pthread_mutex_init(&_rep.mutex, &_attr);
#if defined(PEGASUS_DEBUG)
    _rep.count = 0;
#endif
}
#endif

#endif /* PEGASUS_HAVE_PTHREADS && PEGASUS_HAVE_RECURSIVE_MUTEXES */

//==============================================================================
//
// Mutex implementation without PTHREAD_MUTEX_RECURSIVE feature.
//
//==============================================================================

#if defined(PEGASUS_HAVE_PTHREADS) && !defined(PEGASUS_HAVE_RECURSIVE_MUTEXES)

Mutex::Mutex()
{
    pthread_mutex_init(&_rep.mutex, NULL);
    pthread_cond_init(&_rep.cond, NULL);
    _rep.owner = 0;
    _rep.count = 0;
}

Mutex::Mutex(RecursiveTag)
{
    _rep.recursive = 1;
    pthread_mutex_init(&_rep.mutex, NULL);
    pthread_cond_init(&_rep.cond, NULL);
    _rep.owner = 0;
    _rep.count = 0;
}

Mutex::Mutex(NonRecursiveTag)
{
    _rep.recursive = 0;
    pthread_mutex_init(&_rep.mutex, NULL);
}

Mutex::~Mutex()
{
    PEGASUS_DEBUG_ASSERT(_magic);
    pthread_mutex_destroy(&_rep.mutex);

    if (_rep.recursive)
        pthread_cond_destroy(&_rep.cond);
}

void Mutex::lock()
{
    PEGASUS_DEBUG_ASSERT(_magic);

    if (_rep.recursive)
    {
        pthread_t self = pthread_self();

        pthread_mutex_lock(&_rep.mutex);
        {
            if (_rep.count == 0)
            {
                _rep.owner = self;
            }
            else if (!pthread_equal(_rep.owner, self))
            {
                while (_rep.count > 0)
                    pthread_cond_wait(&_rep.cond, &_rep.mutex);

                _rep.owner = self;
            }

            _rep.count++;
        }
        pthread_mutex_unlock(&_rep.mutex);
    }
    else
    {
        if (pthread_mutex_lock(&_rep.mutex) != 0)
        {
            throw WaitFailed(Threads::self());
        }
    }
}

void Mutex::try_lock()
{
    PEGASUS_DEBUG_ASSERT(_magic);

    if (_rep.recursive)
    {
        pthread_t self = pthread_self();

        pthread_mutex_lock(&_rep.mutex);
        {
            if (_rep.count == 0)
            {
                _rep.owner = self;
                _rep.count = 1;
            }
            else if (pthread_equal(_rep.owner, self))
            {
                _rep.count++;
            }
            else
            {
                pthread_mutex_unlock(&_rep.mutex);
                throw AlreadyLocked(Threads::self());
            }
        }
        pthread_mutex_unlock(&_rep.mutex);
    }
    else
    {
        int r = pthread_mutex_trylock(&_rep.mutex);

        if (r == -1)
            r = errno;

        switch (r)
        {
            case 0:
                break;

            case EBUSY:
                throw AlreadyLocked(Threads::self());

            default:
                throw WaitFailed(Threads::self());
        }
    }
}

void Mutex::timed_lock(Uint32 milliseconds)
{
    PEGASUS_DEBUG_ASSERT(_magic);

    if (_rep.recursive)
    {
        pthread_t self = pthread_self();

        pthread_mutex_lock(&_rep.mutex);
        {
            if (_rep.count == 0)
            {
                _rep.owner = self;
            }
            else if (!pthread_equal(_rep.owner, self))
            {
                timespec ts;
                clock_gettime(CLOCK_REALTIME, &ts);

                ts.tv_sec += milliseconds / 1000;
                ts.tv_nsec += (milliseconds % 1000) * 1000000;

                while (_rep.count > 0)
                {
                    if (pthread_cond_timedwait(
                        &_rep.cond, &_rep.mutex, &ts) != 0)
                    {
                        pthread_mutex_unlock(&_rep.mutex);
                        throw TimeOut(Threads::self());
                    }
                }

                _rep.owner = self;
            }

            _rep.count++;
        }
        pthread_mutex_unlock(&_rep.mutex);
    }
    else
    {
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
            int r = pthread_mutex_trylock(&_rep.mutex);

            if (r == -1)
                r = errno;

            switch (r)
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

                default:
                    throw WaitFailed(Threads::self());
            }
        }
    }
}

void Mutex::unlock()
{
    PEGASUS_DEBUG_ASSERT(_magic);

    if (_rep.recursive)
    {
        pthread_t self = pthread_self();

        pthread_mutex_lock(&_rep.mutex);
        {
            // If not locked or if calling thread is not the locker.

            if (_rep.count == 0 || !pthread_equal(_rep.owner, self))
                PEGASUS_DEBUG_ASSERT(0);

            _rep.count--;
            _rep.owner = 0;
            pthread_cond_signal(&_rep.cond);
        }
        pthread_mutex_unlock(&_rep.mutex);
    }
    else
    {
        if (pthread_mutex_unlock(&_rep.mutex) != 0)
            throw Permission(Threads::self());
    }
}

#endif /* PEGASUS_HAVE_PTHREADS && !PEGASUS_HAVE_RECURSIVE_MUTEXES */

//==============================================================================
//
// PEGASUS_HAVE_WINDOWS_THREADS
//
//==============================================================================

#if defined(PEGASUS_HAVE_WINDOWS_THREADS)

static inline void _initialize(MutexRep& rep)
{
    rep.handle = CreateMutex(NULL, FALSE, NULL);
    rep.count = 0;
}

Mutex::Mutex()
{
    _initialize(_rep);
}

Mutex::Mutex(RecursiveTag)
{
    _initialize(_rep);
}

Mutex::Mutex(NonRecursiveTag)
{
    _initialize(_rep);
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
        throw WaitFailed(Threads::self());

    _rep.count++;
}

void Mutex::try_lock()
{
    PEGASUS_DEBUG_ASSERT(_magic);

    DWORD rc = WaitForSingleObject(_rep.handle, 0);

    if (rc == WAIT_TIMEOUT)
        throw AlreadyLocked(Threads::self());

    if (rc == WAIT_FAILED)
        throw WaitFailed(Threads::self());

    _rep.count++;
}

void Mutex::timed_lock(Uint32 milliseconds)
{
    PEGASUS_DEBUG_ASSERT(_magic);

    DWORD rc = WaitForSingleObject(_rep.handle, milliseconds);

    if (rc == WAIT_TIMEOUT)
        throw TimeOut(Threads::self());

    if (rc == WAIT_FAILED)
        throw WaitFailed(Threads::self());

    _rep.count++;
}

void Mutex::unlock()
{
    PEGASUS_DEBUG_ASSERT(_magic);
    PEGASUS_DEBUG_ASSERT(_rep.count > 0);

    _rep.count--;
    ReleaseMutex(_rep.handle);
}

#endif /* PEGASUS_HAVE_WINDOWS_THREADS */

PEGASUS_NAMESPACE_END
