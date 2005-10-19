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
// Author: Mike Brasher (mike-brasher@austin.rr.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef _Pegasus_Common_AtomicInt_HPUX_PARISC_ACC_h
#define _Pegasus_Common_AtomicInt_HPUX_PARISC_ACC_h

#include <Pegasus/Common/Config.h>

PEGASUS_NAMESPACE_BEGIN

#define ATOMIC_UNLOCKED_MASK 0x80000000

inline Uint32 load_and_clear(volatile Uint32* x) 
{
#if 1
    /* Warning: x must be aligned on a 16-byte boundary. */
    register Uint32 volatile* addr = (Uint32 volatile*)x;
    register Uint32 t = 0;
    ((volatile char*)(x + 1))[0] = 0;
    _asm("LDCWS,CO",0,0,addr,t);
    return t;
#else
    // Non-thread-safe version for testing with single thread.
    Uint32 t = *x;
    *x = 0;
    return t;
#endif
}

inline Uint32 obtain_spinlock(volatile Uint32* x)
{
    Uint32 t;

    // Spin here while not available.

    while (*x == 0 || (t = load_and_clear(x)) == 0)
	;

    return t;
}

struct AtomicType
{
    // This is the only data type that is aligned on a 16 byte boundary
    // (a requirement of the LDCWS instruction used in this module).
#if 1
    volatile __float128 tmpl;
#else
    int x;
#endif
};

inline volatile Uint32* AtomicCounter(AtomicType* a)
{
    return (volatile Uint32*)a;
}

inline AtomicIntTemplate<AtomicType>::AtomicIntTemplate(Uint32 n)
{
    *AtomicCounter(&_rep) = ATOMIC_UNLOCKED_MASK | n;
}

inline AtomicIntTemplate<AtomicType>::~AtomicIntTemplate()
{
    // Nothing to do!
}

inline Uint32 AtomicIntTemplate<AtomicType>::get() const
{
    Uint32 t;

    // Spin here until available.

    volatile Uint32* c = AtomicCounter((AtomicType*)&_rep);

    while ((t = *c) == 0)
	;

    return ~ATOMIC_UNLOCKED_MASK & t;
}

inline void AtomicIntTemplate<AtomicType>::set(Uint32 n)
{
    volatile Uint32* c = AtomicCounter(&_rep);
    obtain_spinlock(c);
    *c = ATOMIC_UNLOCKED_MASK | n;
}

inline void AtomicIntTemplate<AtomicType>::inc()
{
    volatile Uint32* c = AtomicCounter(&_rep);
    Uint32 t = obtain_spinlock(c);
    *c = t + 1;
}

inline void AtomicIntTemplate<AtomicType>::dec()
{
    volatile Uint32* c = AtomicCounter(&_rep);
    Uint32 t = obtain_spinlock(c);
    *c = t - 1;
}

inline bool AtomicIntTemplate<AtomicType>::decAndTestIfZero()
{
    volatile Uint32* c = AtomicCounter(&_rep);
    Uint32 t = obtain_spinlock(c);
    t -= 1;
    *c = t;
    return (~ATOMIC_UNLOCKED_MASK & t) == 0;
}

PEGASUS_NAMESPACE_END

#endif /* _Pegasus_Common_AtomicInt_HPUX_PARISC_ACC_h */
