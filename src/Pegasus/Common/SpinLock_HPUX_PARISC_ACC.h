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
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_SpinLock_HPUX_PARISC_ACC_h
#define Pegasus_SpinLock_HPUX_PARISC_ACC_h

#include <Pegasus/Common/Config.h>
#include <pthread.h>
#include <sched.h>
#include <errno.h>

#define PEGASUS_SPINLOCK_USE_PTHREADS

PEGASUS_NAMESPACE_BEGIN

// This type implements a spinlock. It is deliberately not a class since we 
// wish to avoid automatic construction/destruction.
struct SpinLock
{
    pthread_mutexattr_t attr;
    pthread_mutex_t mutex;
    Uint32 initialized;
};

inline void SpinLockCreate(SpinLock& x)
{
    pthread_mutexattr_init(&x.attr);
    pthread_mutexattr_setspin_np(&x.attr, PTHREAD_MUTEX_SPINONLY_NP);
    pthread_mutex_init(&x.mutex, &x.attr);
    x.initialized = 1;
}

inline void SpinLockDestroy(SpinLock& x)
{
    while (pthread_mutex_destroy(&x.mutex) == EBUSY)
        sched_yield();

    pthread_mutexattr_destroy(&x.attr);
}

inline void SpinLockLock(SpinLock& x)
{
    pthread_mutex_lock(&x.mutex);
}

inline void SpinLockUnlock(SpinLock& x)
{
    pthread_mutex_unlock(&x.mutex);
}

PEGASUS_NAMESPACE_END

#endif /* Pegasus_SpinLock_HPUX_PARISC_ACC_h */
