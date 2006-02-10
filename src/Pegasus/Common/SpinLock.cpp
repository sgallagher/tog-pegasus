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
// Author: Mike Brasher, Inova Europe (mike-brasher@austin.rr.com)
//
// Modified By: David Dillard, Symantec Corp. (david_dillard@symantec.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#if defined(PEGASUS_OS_SOLARIS)
# include <sys/types.h>
# include <unistd.h>
#endif

#include "SpinLock.h"

#if  !defined(PEGASUS_OS_VMS) &&  !defined(PEGASUS_OS_TYPE_WINDOWS)
# define PEGASUS_SPINLOCK_USE_PTHREADS
#endif

#ifdef PEGASUS_SPINLOCK_USE_PTHREADS
# include <pthread.h>
#else
# include "IPC.h"
#endif

PEGASUS_NAMESPACE_BEGIN

SpinLock spinLockPool[PEGASUS_NUM_SHARED_SPIN_LOCKS];
int spinLockPoolInitialized;

#ifdef PEGASUS_SPINLOCK_USE_PTHREADS
pthread_mutex_t _spinLockInitMutex = PTHREAD_MUTEX_INITIALIZER;
#else
static Mutex _spinLockInitMutex;
#endif

void SpinLockCreatePool()
{
    // There's no need to check spinLockPoolInitialized before locking the
    // mutex, because the caller can check the flag before calling this
    // function.

#ifdef PEGASUS_SPINLOCK_USE_PTHREADS
    pthread_mutex_lock(&_spinLockInitMutex);
#else
    _spinLockInitMutex.lock(pegasus_thread_self());
#endif

    if (spinLockPoolInitialized == 0)
    {
        for (size_t i = 0; i < PEGASUS_NUM_SHARED_SPIN_LOCKS; i++)
            SpinLockCreate(spinLockPool[i]);

        spinLockPoolInitialized = 1;
    }

#ifdef PEGASUS_SPINLOCK_USE_PTHREADS
    pthread_mutex_unlock(&_spinLockInitMutex);
#else
    _spinLockInitMutex.unlock();
#endif
}

#if defined(PEGASUS_SPINLOCK_USE_PTHREADS)

// This function is called prior to forking.  We must obtain a lock
// on every mutex that the child will inherit.  These will remain locked
// until they are unlocked (by _unlockSpinLockPool()).  This prevents a
// child process from waiting indefinitely on a mutex that was locked by
// another thread in the parent process during the fork.

extern "C" void _lockSpinLockPool()
{
    // Initialize the spinlock pool if not already done.

    if (spinLockPoolInitialized == 0)
        SpinLockCreatePool();

    pthread_mutex_lock(&_spinLockInitMutex);

    for (size_t i = 0; i < PEGASUS_NUM_SHARED_SPIN_LOCKS; i++)
        SpinLockLock(spinLockPool[i]);
}

// This function is called after forking.  It unlocks the mutexes that
// were locked by _lockSpinLockPool() before the fork.

extern "C" void _unlockSpinLockPool()
{
    pthread_mutex_unlock(&_spinLockInitMutex);

    for (size_t i = 0; i < PEGASUS_NUM_SHARED_SPIN_LOCKS; i++)
        SpinLockUnlock(spinLockPool[i]);
}

class SpinLockInitializer
{
public:
    SpinLockInitializer()
    {
// ATTN: Temporary workaround for Bug 4559
#if defined(PEGASUS_OS_HPUX) || defined(PEGASUS_OS_SOLARIS)
        pthread_atfork(
            _lockSpinLockPool,
            _unlockSpinLockPool,
            _unlockSpinLockPool);
#endif
    }
};

static SpinLockInitializer spinLockInitializer;

#endif /* PEGASUS_SPINLOCK_USE_PTHREADS */

PEGASUS_NAMESPACE_END
