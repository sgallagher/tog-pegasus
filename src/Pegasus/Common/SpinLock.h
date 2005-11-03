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

#ifndef Pegasus_SpinLock_h
#define Pegasus_SpinLock_h

#include <Pegasus/Common/Config.h>

#if defined (XPEGASUS_PLATFORM_HPUX_PARISC_ACC)
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

// This array defines spin locks shared across the system. Note that these
// are initialized in a lazy fashion. Before using one, check the initialized
// member. If zero, then call SpinLockConditionalCreate().
extern SpinLock sharedSpinLocks[PEGASUS_NUM_SHARED_SPIN_LOCKS];

// Calls SpinLockCreate() on the given lock if lock.initialized is zero. The
// function uses a mutex so it is safe to call this function from multiple
// threads on the very same lock (only one of the calls will succeed.
void SpinLockConditionalCreate(SpinLock& lock);

// Maps an address into the sharedSpinLocks[] array defined above. This is used
// to assign objects (by their addresses) to a shared lock. Collisions are 
// okay.
inline size_t SpinLockIndex(const void* x)
{
    // Throw away the lower two bits since they are almost always zero
    // anyway due to alignment properties.
    return ((unsigned long)x >> 2) % PEGASUS_NUM_SHARED_SPIN_LOCKS;
}

PEGASUS_NAMESPACE_END

#endif /* Pegasus_SpinLock_h */
