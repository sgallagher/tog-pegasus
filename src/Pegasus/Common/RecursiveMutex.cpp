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

#include <new>
#include "RecursiveMutex.h"

//==============================================================================
//
// Pick an implementation by defining one of the following.
//
//     PEGASUS_RMUTEX_PTHREADS
//     PEGASUS_RMUTEX_WINDOWS
//     PEGASUS_RMUTEX_GENERIC
//
//==============================================================================

#if defined(PEGASUS_PLATFORM_LINUX_GENERIC_GNU) || \
    defined(PEGASUS_OS_HPUX) || defined(PEGASUS_PLATFORM_ZOS_ZSERIES_IBM)
# define PEGASUS_RMUTEX_PTHREADS
# include <pthread.h>
#elif defined(PEGASUS_OS_TYPE_WINDOWS)
# define PEGASUS_RMUTEX_WINDOWS
# include <windows.h>
#else
# define PEGASUS_RMUTEX_GENERIC
# include <Pegasus/Common/Mutex.h>
# include <Pegasus/Common/IPC.h>
#endif

PEGASUS_NAMESPACE_BEGIN

//==============================================================================
//
// Implementation for PEGASUS_RMUTEX_PTHREADS
//
//==============================================================================

#if defined(PEGASUS_RMUTEX_PTHREADS)

struct RecursiveMutexRep
{
    pthread_mutex_t mutex;
};

RecursiveMutex::RecursiveMutex()
{
    if (sizeof(RecursiveMutexRep) <= sizeof(_opaque))
	_rep = (RecursiveMutexRep*)_opaque;
    else
	_rep = new RecursiveMutexRep;

    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);

#if defined(PEGASUS_OS_HPUX) || defined(PEGASUS_PLATFORM_ZOS_ZSERIES_IBM)
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
#else
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE_NP);
#endif

    pthread_mutex_init(&_rep->mutex, &attr);
    pthread_mutexattr_destroy(&attr);
}

RecursiveMutex::~RecursiveMutex()
{
    pthread_mutex_destroy(&_rep->mutex);

    if ((void*)_rep != (void*)_opaque)
	delete _rep;
}

void RecursiveMutex::lock()
{
    // Note: no error can occur since this is a recursive mutex.
    pthread_mutex_lock(&_rep->mutex);
}

bool RecursiveMutex::try_lock()
{
    // Note: no error can occur since this is a recursive mutex.
    return pthread_mutex_trylock(&_rep->mutex) == 0;
}

void RecursiveMutex::unlock()
{
    // Note: no error can occur since this is a recursive mutex.
    pthread_mutex_unlock(&_rep->mutex);
}

#endif /* PEGASUS_RMUTEX_PTHREADS */

//==============================================================================
//
// Implementation for PEGASUS_RMUTEX_GENERIC
//
//==============================================================================

#ifdef PEGASUS_RMUTEX_GENERIC

struct RecursiveMutexRep
{
    Mutex mutex;
    Condition cond;
    int count;
    ThreadType owner;
    RecursiveMutexRep() : cond(mutex), count(0), owner(0) { }
};

RecursiveMutex::RecursiveMutex()
{
    if (sizeof(RecursiveMutexRep) <= sizeof(_opaque))
	_rep = new((RecursiveMutexRep*)_opaque) RecursiveMutexRep;
    else
	_rep = new RecursiveMutexRep;
}

RecursiveMutex::~RecursiveMutex()
{
    if ((void*)_rep != (void*)_opaque)
	delete _rep;
    else
	((RecursiveMutexRep*)_opaque)->~RecursiveMutexRep();
}

void RecursiveMutex::lock()
{
    _rep->mutex.lock();
    {
	// assert(_rep->count >= 0);

	while (_rep->count > 0 && _rep->owner != Threads::self())
	    _rep->cond.unlocked_wait(Threads::self());

	_rep->count++;
	_rep->owner = Threads::self();
    }
    _rep->mutex.unlock();
}

bool RecursiveMutex::try_lock()
{
    _rep->mutex.try_lock();
    {
	if (_rep->count > 0 && _rep->owner != Threads::self())
	    return false;

	_rep->count++;
	_rep->owner = Threads::self();
    }
    _rep->mutex.unlock();

    return true;
}

void RecursiveMutex::unlock()
{
    _rep->mutex.lock();
    {
	// assert(_rep->owner == Threads::self());

	_rep->count--;

	if (_rep->count == 0)
	{
	    _rep->owner = 0;
	    _rep->cond.unlocked_signal(Threads::self());
	}
    }
    _rep->mutex.unlock();
}

#endif /* PEGASUS_RMUTEX_GENERIC */

//==============================================================================
//
// Implementation for PEGASUS_RMUTEX_WINDOWS
//
//==============================================================================

#ifdef PEGASUS_RMUTEX_WINDOWS

struct RecursiveMutexRep
{
    HANDLE handle;
};

RecursiveMutex::RecursiveMutex()
{
    RecursiveMutexRep* rep = (RecursiveMutexRep*)_opaque;
    rep->handle = CreateMutex(NULL, FALSE, NULL);
}

RecursiveMutex::~RecursiveMutex()
{
    RecursiveMutexRep* rep = (RecursiveMutexRep*)_opaque;
    CloseHandle(rep->handle);
}

void RecursiveMutex::lock()
{
    RecursiveMutexRep* rep = (RecursiveMutexRep*)_opaque;
    // Can return WAIT_FAILED but won't since this class guard for errors.
    WaitForSingleObject(rep->handle, INFINITE);
}

bool RecursiveMutex::try_lock()
{
    RecursiveMutexRep* rep = (RecursiveMutexRep*)_opaque;
    return WaitForSingleObject(rep->handle, 0) == 0;
}

void RecursiveMutex::unlock()
{
    RecursiveMutexRep* rep = (RecursiveMutexRep*)_opaque;
    ReleaseMutex(rep->handle);
}

#endif /* PEGASUS_RMUTEX_WINDOWS */

PEGASUS_NAMESPACE_END
