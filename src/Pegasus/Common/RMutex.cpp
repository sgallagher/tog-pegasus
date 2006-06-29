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

#include <new>
#include "RMutex.h"

//==============================================================================
//
// Pick an implementation by defining one of the following.
//
//     PEGASUS_RMUTEX_PTHREADS
//     PEGASUS_RMUTEX_WINDOWS
//     PEGASUS_RMUTEX_GENERIC
//
//==============================================================================

#if defined(XPEGASUS_PLATFORM_LINUX_GENERIC_GNU)
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

struct RMutexRep
{
    pthread_mutex_t mutex;
};

RMutex::RMutex()
{
    if (sizeof(RMutexRep) <= sizeof(_opaque))
	_rep = (RMutexRep*)_opaque;
    else
	_rep = new RMutexRep;

    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE_NP);
    pthread_mutex_init(&_rep->mutex, &attr);
    pthread_mutexattr_destroy(&attr);
}

RMutex::~RMutex()
{
    pthread_mutex_destroy(&_rep->mutex);

    if ((void*)_rep != (void*)_opaque)
	delete _rep;
}

void RMutex::lock()
{
    // Note: no error can occur since this is a recursive mutex.
    pthread_mutex_lock(&_rep->mutex);
}

bool RMutex::try_lock()
{
    // Note: no error can occur since this is a recursive mutex.
    return pthread_mutex_trylock(&_rep->mutex) == 0;
}

void RMutex::unlock()
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

struct RMutexRep
{
    Mutex mutex;
    Condition cond;
    int count;
    PEGASUS_THREAD_TYPE owner;
    RMutexRep() : cond(mutex), count(0), owner(0) { }
};

RMutex::RMutex()
{
    if (sizeof(RMutexRep) <= sizeof(_opaque))
	_rep = new((RMutexRep*)_opaque) RMutexRep;
    else
	_rep = new RMutexRep;
}

RMutex::~RMutex()
{
    if ((void*)_rep != (void*)_opaque)
	delete _rep;
    else
	((RMutexRep*)_opaque)->~RMutexRep();
}

void RMutex::lock()
{
    _rep->mutex.lock();
    {
	// assert(_rep->count >= 0);

	while (_rep->count > 0 && _rep->owner != pegasus_thread_self())
	    _rep->cond.unlocked_wait(pegasus_thread_self());

	_rep->count++;
	_rep->owner = pegasus_thread_self();
    }
    _rep->mutex.unlock();
}

bool RMutex::try_lock()
{
    _rep->mutex.try_lock();
    {
	if (_rep->count > 0 && _rep->owner != pegasus_thread_self())
	    return false;

	_rep->count++;
	_rep->owner = pegasus_thread_self();
    }
    _rep->mutex.unlock();

    return true;
}

void RMutex::unlock()
{
    _rep->mutex.lock();
    {
	// assert(_rep->owner == pegasus_thread_self());

	_rep->count--;

	if (_rep->count == 0)
	{
	    _rep->owner = 0;
	    _rep->cond.unlocked_signal(pegasus_thread_self());
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

struct RMutexRep
{
    Handle handle;
};

RMutex::RMutex()
{
    RMutex* rep = ((RMutexRep*)_opaque);
    rep->handle = CreateMutex(NULL, FALSE, NULL);
}

RMutex::~RMutex()
{
    RMutex* rep = ((RMutexRep*)_opaque);
    CloseHandle(rep->handle);
}

void RMutex::lock()
{
    RMutex* rep = ((RMutexRep*)_opaque);
    // Can return WAIT_FAILED but won't since this class guard for errors.
    WaitForSingleObject(rep->handle, INFINITE);
}

bool RMutex::try_lock()
{
    RMutex* rep = ((RMutexRep*)_opaque);
    return WaitForSingleObject(rep->handle, 0) == 0;
}

void RMutex::unlock()
{
    RMutex* rep = ((RMutexRep*)_opaque);
    ReleaseMutex(rep->handle);
}

#endif /* PEGASUS_RMUTEX_WINDOWS */

PEGASUS_NAMESPACE_END
