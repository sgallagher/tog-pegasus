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

#ifndef Pegasus_SpinLock_h
#define Pegasus_SpinLock_h

#include <Pegasus/Common/Config.h>

#if defined (PEGASUS_PLATFORM_HPUX_PARISC_ACC)
# include <Pegasus/Common/SpinLock_HPUX_PARISC_ACC.h>
#elif defined (PEGASUS_PLATFORM_SOLARIS_SPARC_GNU)
# include <Pegasus/Common/SpinLock_SOLARIS_SPARC_GNU.h>
#elif defined (PEGASUS_PLATFORM_LINUX_IX86_GNU)
# include <Pegasus/Common/SpinLock_LINUX_IX86_GNU.h>
#elif defined (PEGASUS_PLATFORM_LINUX_IX86_64_GNU)
# include <Pegasus/Common/SpinLock_LINUX_IX86_GNU.h>
#else
# include <Pegasus/Common/SpinLock_Generic.h>
#endif

#define PEGASUS_NUM_SHARED_SPIN_LOCKS 64

PEGASUS_NAMESPACE_BEGIN

// This array defines spin locks shared across the system. These are 
// initialized by calling SpinLockCreatePool().
extern SpinLock spinLockPool[PEGASUS_NUM_SHARED_SPIN_LOCKS];

// This flag is 0 until SpinLockCreatePool() is called, which sets it
// to 1.
extern int spinLockPoolInitialized;

// Initializes the global pool of mutexes.
void SpinLockCreatePool();

// Maps an address into the spinLockPool[] array defined above. This is used
// to assign objects (by their addresses) to a shared lock. Collisions are 
// okay.
inline size_t SpinLockIndex(const void* x)
{
    // Throw away the lower two bits since they are almost always zero
    // anyway due to alignment properties.
    return ((unsigned long)x >> 2) % PEGASUS_NUM_SHARED_SPIN_LOCKS;
}

// Call this function before forking to unlock the spinlocks in the global
// spinlock pool (spinLockPool).
void SpinLockInit();

PEGASUS_NAMESPACE_END

#endif /* Pegasus_SpinLock_h */
