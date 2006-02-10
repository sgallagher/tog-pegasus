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
// Modified By: Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

// NOTE:  This spinlock implementation is based on the paper "Implementing
// Spinlocks on the Intel(R) Itanium(R) Architecture an PA-RISC" by Tor
// Ekqvist and David Graves.

// WARNING:  This implementation has subtle complexities.  Seemingly
// innocuous changes could have unexpected side effects.  Please use
// diligence when modifying the implementation.

#ifndef Pegasus_SpinLock_HPUX_PARISC_ACC_h
#define Pegasus_SpinLock_HPUX_PARISC_ACC_h

#include <Pegasus/Common/Config.h>

#include <sys/time.h>    // For select()

#define PEGASUS_SPINLOCK_USE_PTHREADS

extern "C" int LoadAndClearWord(volatile int* addr);
extern "C" void _flush_globals();

PEGASUS_NAMESPACE_BEGIN

// This type implements a spinlock. It is deliberately not a class since we 
// wish to avoid automatic construction/destruction.
struct SpinLock
{
    unsigned int initialized;

    /**
        Points to a 16-byte aligned lock word (which lies somewhere within
        the region). The lock word is zero when locked and 1 when unlocked.
     */
    volatile int* lock;

    /**
        Points to a lock region (which contains the lock).  The LDCWS
        instruction requires that the lock word be aligned on a 16-byte
        boundary.  So we allocate 32 bytes and adjust lock so that it falls
        on the first such boundary within this region. We make the region
        large to keep the spin locks from getting too close together, which
        would put them on the same cache line, creating contention.
     */
    char region[32];

    /**
        Extends the size of the struct to match the 64-byte cache line size.
        NOTE: This does not ensure that the struct will align with a cache
        line.  Doing so could benefit performance.
     */
    char unused[24];
};

inline void SpinLockCreate(SpinLock& x)
{
    // Set x.lock to first 16-byte boundary within region.
#ifdef __LP64__
    x.lock = (volatile int*)(((long)x.region + 15) & ~0xF);
#else
    x.lock = (volatile int*)(((int)x.region + 15) & ~0xF);
#endif

    // Set to unlocked
    *x.lock = 1;

    x.initialized = 1;
}

inline void SpinLockDestroy(SpinLock& x)
{
}

inline void SpinLockLock(SpinLock& x)
{
    // Spin until we obtain the lock.
    while (1)
    {
        for (Uint32 spins = 0; spins < 200; spins++)
        {
            if (*x.lock == 1)    // pre-check
            {
                if (LoadAndClearWord(x.lock) == 1)
                {
                    return;
                }
            }
        }

        // Didn't get the lock after 200 spins, so sleep for 5 ms
        struct timeval sleeptime = { 0, 5000 };
        select(0, 0, 0, 0, &sleeptime);
    }
}

inline void SpinLockUnlock(SpinLock& x)
{
    // Ensure that the compiler doesn't hold any externally-visible values in
    // registers across the lock release.
    _flush_globals();

    // Set to unlocked
    *x.lock = 1;
}

PEGASUS_NAMESPACE_END

#endif /* Pegasus_SpinLock_HPUX_PARISC_ACC_h */
