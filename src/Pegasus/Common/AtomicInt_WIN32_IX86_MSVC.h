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

#ifndef _Pegasus_Common_AtomicInt_WIN32_IX86_MSVC_h
#define _Pegasus_Common_AtomicInt_WIN32_IX86_MSVC_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Network.h>
#include <windows.h>

PEGASUS_NAMESPACE_BEGIN

typedef LONG AtomicType;

PEGASUS_TEMPLATE_SPECIALIZATION
inline AtomicIntTemplate<AtomicType>::AtomicIntTemplate(Uint32 n)
{
    _rep = LONG(n);
}

PEGASUS_TEMPLATE_SPECIALIZATION
inline AtomicIntTemplate<AtomicType>::~AtomicIntTemplate()
{
}

PEGASUS_TEMPLATE_SPECIALIZATION
inline Uint32 AtomicIntTemplate<AtomicType>::get() const
{
    return Uint32(_rep);
}

PEGASUS_TEMPLATE_SPECIALIZATION
inline void AtomicIntTemplate<AtomicType>::set(Uint32 n)
{
    _rep = LONG(n);
}

PEGASUS_TEMPLATE_SPECIALIZATION
inline void AtomicIntTemplate<AtomicType>::inc()
{
    InterlockedIncrement(&_rep);
}

PEGASUS_TEMPLATE_SPECIALIZATION
inline void AtomicIntTemplate<AtomicType>::dec()
{
    InterlockedDecrement(&_rep);
}

PEGASUS_TEMPLATE_SPECIALIZATION
inline bool AtomicIntTemplate<AtomicType>::decAndTestIfZero()
{
    return InterlockedDecrement(&_rep) == 0;
}

PEGASUS_NAMESPACE_END

#endif /* _Pegasus_Common_AtomicInt_WIN32_IX86_MSVC_h */
