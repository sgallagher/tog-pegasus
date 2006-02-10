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
// Author: Mike Brasher (mike-brasher@austin.rr.com) - Inova Europe
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_AtomicInt_h
#define Pegasus_AtomicInt_h

#include <Pegasus/Common/Config.h>

//#ifndef PEGASUS_INTERNALONLY
//# error "ERROR: This header is for internal use only (AtomicInt.h)"
//#endif

PEGASUS_NAMESPACE_BEGIN

template<class ATOMIC_TYPE>
class AtomicIntTemplate
{
public:

    // Constructor.
    AtomicIntTemplate(Uint32 n = 0);

    // Destructor.
    ~AtomicIntTemplate();

    // Sets value.
    void set(Uint32 n);

    // Gets value.
    Uint32 get() const;

    // Increment.
    void inc();

    // Decrement.
    void dec();

    // Decrements and returns true if it is zero.
    bool decAndTestIfZero();

    // Assignment.
    AtomicIntTemplate& operator=(Uint32 n) { set(n); return* this; }

    // Post-increment.
    void operator++(int) { inc(); }

    // Post-decrement.
    void operator--(int) { dec(); }

private:

    // Note: These methods are intentionally hidden (and should not be called).
    // The implementation is much easier without having to implement these for
    // every platform.
    AtomicIntTemplate(const AtomicIntTemplate&) { }
    AtomicIntTemplate& operator=(const AtomicIntTemplate&) { return *this; }
    Boolean operator==(Uint32) const { return false; }
    void operator++() { }
    void operator--() { }

    typedef AtomicIntTemplate<ATOMIC_TYPE> This;

    ATOMIC_TYPE _rep;
};

PEGASUS_NAMESPACE_END

#if defined(PEGASUS_PLATFORM_LINUX_IX86_GNU)
# include <Pegasus/Common/AtomicInt_LINUX_IX86_GNU.h>
#elif defined(PEGASUS_PLATFORM_LINUX_X86_64_GNU)
# include <Pegasus/Common/AtomicInt_LINUX_X86_64_GNU.h>
#elif defined(PEGASUS_PLATFORM_LINUX_IA64_GNU)
# include <Pegasus/Common/AtomicInt_LINUX_IA64_GNU.h>
#elif defined(PEGASUS_PLATFORM_LINUX_PPC_GNU)
# include <Pegasus/Common/AtomicInt_LINUX_PPC_GNU.h>
#elif defined(PEGASUS_PLATFORM_WIN32_IX86_MSVC)
# include <Pegasus/Common/AtomicInt_WIN32_IX86_MSVC.h>
#elif defined (PEGASUS_PLATFORM_ZOS_ZSERIES_IBM)
# include <Pegasus/Common/AtomicInt_ZOS_ZSERIES_IBM.h>
#elif defined (PEGASUS_PLATFORM_HPUX_PARISC_ACC)
# include <Pegasus/Common/AtomicInt_Generic.h>
#elif defined (PEGASUS_PLATFORM_HPUX_IA64_ACC)
# include <Pegasus/Common/AtomicInt_HPUX_IA64_ACC.h>
#elif defined (PEGASUS_PLATFORM_LINUX_XSCALE_GNU)
# include <Pegasus/Common/AtomicInt_LINUX_XSCALE_GNU.h>
#else
# include <Pegasus/Common/AtomicInt_Generic.h>
#endif

PEGASUS_NAMESPACE_BEGIN

typedef AtomicIntTemplate<AtomicType> AtomicInt;

PEGASUS_NAMESPACE_END

#endif /* Pegasus_AtomicInt_h */
