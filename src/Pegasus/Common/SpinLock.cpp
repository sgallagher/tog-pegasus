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
// Author: Mike Brasher, Inova Europe (mike-brasher@austin.rr.com)
//
// Modified By: David Dillard, Symantec Corp. (david_dillard@symantec.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include "SpinLock.h"

#if !defined(PEGASUS_OS_TYPE_WINDOWS)
# define PEGASUS_SPINLOCK_USE_PTHREADS
#endif

#ifdef PEGASUS_SPINLOCK_USE_PTHREADS
# include <pthread.h>
#else
# include "IPC.h"
#endif

PEGASUS_NAMESPACE_BEGIN

SpinLock sharedSpinLocks[PEGASUS_NUM_SHARED_SPIN_LOCKS];

#ifdef PEGASUS_SPINLOCK_USE_PTHREADS
pthread_mutex_t _spinLockInitMutex = PTHREAD_MUTEX_INITIALIZER;
#else
static Mutex _spinLockInitMutex;
#endif

void SpinLockConditionalCreate(SpinLock& lock)
{
    // Use double-checked locking pattern to avoid mutex lock when possible.

    if (lock.initialized == 0)
    {
#ifdef PEGASUS_SPINLOCK_USE_PTHREADS
	pthread_mutex_lock(&_spinLockInitMutex);
#else
	_spinLockInitMutex.lock(pegasus_thread_self());
#endif

	if (lock.initialized == 0)
	    SpinLockCreate(lock);

#ifdef PEGASUS_SPINLOCK_USE_PTHREADS
	pthread_mutex_unlock(&_spinLockInitMutex);
#else
	_spinLockInitMutex.unlock();
#endif
    }
}

PEGASUS_NAMESPACE_END
