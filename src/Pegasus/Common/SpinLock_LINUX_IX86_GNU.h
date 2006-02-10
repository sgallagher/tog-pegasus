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
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_SpinLock_LINUX_IX86_GNU_h
#define Pegasus_SpinLock_LINUX_IX86_GNU_h

// Note: this lock can be eliminated for single processor systems.
#define PEGASUS_ATOMIC_LOCK "lock ; "

#include <Pegasus/Common/Config.h>

PEGASUS_NAMESPACE_BEGIN

struct SpinLock
{
    // Available when zero.
    volatile int lock;
};

inline void SpinLockCreate(SpinLock& x)
{
    // Available when one.
    x.lock = 1;
}

inline void SpinLockDestroy(SpinLock& x)
{
}

inline void SpinLockLock(SpinLock& x)
{
    // Spin until we are able to obtain a lock.

    for (;;)
    {
	// Wait until the lock is available then try to grab it before another
	// thread gets it.

	if (x.lock == 1)
	{
	    unsigned char isZero;

	    asm volatile(
		PEGASUS_ATOMIC_LOCK "decl %0; sete %1"
		:"=m" (x.lock), "=qm" (isZero)
		:"m" (x.lock) : "memory");

	    // If it became zero, then we must have locked it.

	    if (isZero)
		return;

	    // Some other thread locked it. Increment and keep trying.

	    asm volatile(
		PEGASUS_ATOMIC_LOCK "incl %0"
		:"=m" (x.lock)
		:"m" (x.lock));
	}
    }
}

inline void SpinLockUnlock(SpinLock& x)
{
    asm volatile(
        PEGASUS_ATOMIC_LOCK "incl %0"
        :"=m" (x.lock)
        :"m" (x.lock));
}

PEGASUS_NAMESPACE_END

#endif /* Pegasus_SpinLock_LINUX_IX86_GNU_h */
