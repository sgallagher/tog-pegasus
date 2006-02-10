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

#include <Pegasus/Common/Config.h>
#include <machine/sys/inline.h>

PEGASUS_NAMESPACE_BEGIN

struct SpinLock
{
    // Available when zero.
    volatile Uint32 lock;
};

inline void SpinLockCreate(SpinLock& x)
{
    x.lock = 0;
}

inline void SpinLockDestroy(SpinLock& x)
{
}

inline void SpinLockLock(SpinLock& x)
{
    // Spin until we are able to obtain a lock.

    for (;;)
    {
	// Wait until the lock is zero (unlocked) and then try to grab it
	// before some other thread gets it.

	if (x.lock == 0)
	{
	    // Try to grab the lock (by performing an increment and test
	    // operation).

	    uint32 tmp = _Asm_fetchadd(
		(_Asm_fasz)_FASZ_W,
		(_Asm_sem)_SEM_ACQ,
		(volatile uint32*)&x.lock,
		(int)-1,
		(_Asm_ldhint)LDHINT_NONE);

	    // If we got the lock then the old value was zero (available).
	    // The lock is now one or greater (other threads may have 
	    // incremented as well).

	    if (tmp == 0)
		return;

	    // Some other thread must have gotten it before we did (tmp will
	    // be greater than one since some other thread incremented it too).
	    // So decrement the lock and keep trying.

	    _Asm_fetchadd(
		(_Asm_fasz)_FASZ_W,
		(_Asm_sem)_SEM_ACQ,
		(volatile uint32*)&x.lock,
		(int)-1,
		(_Asm_ldhint)LDHINT_NONE);
	}
    }
}

inline void SpinLockUnlock(SpinLock& x)
{
    _Asm_fetchadd(
	(_Asm_fasz)_FASZ_W,
	(_Asm_sem)_SEM_ACQ,
	(volatile uint32*)&x.lock,
	(int)-1,
	(_Asm_ldhint)LDHINT_NONE);
}

PEGASUS_NAMESPACE_END

#endif /* Pegasus_SpinLock_LINUX_IX86_GNU_h */
