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
// Author: Marek Szermutzky (MSzermutzky@de.ibm.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef _Pegasus_Common_AtomicInt_ZOS_ZSERIES_IBM_h
#define _Pegasus_Common_AtomicInt_ZOS_ZSERIES_IBM_h

#include <Pegasus/Common/Config.h>

PEGASUS_NAMESPACE_BEGIN

struct AtomicType
{
    cs_t n;
};

PEGASUS_TEMPLATE_SPECIALIZATION
inline AtomicIntTemplate<AtomicType>::AtomicIntTemplate(Uint32 n)
{
    _rep.n = (cs_t)n;
}

PEGASUS_TEMPLATE_SPECIALIZATION
inline AtomicIntTemplate<AtomicType>::~AtomicIntTemplate()
{
}

PEGASUS_TEMPLATE_SPECIALIZATION
inline Uint32 AtomicIntTemplate<AtomicType>::get() const
{
    return (Uint32)_rep.n;
}

PEGASUS_TEMPLATE_SPECIALIZATION
inline void AtomicIntTemplate<AtomicType>::set(Uint32 n)
{
    _rep.n = (cs_t)n;
}

PEGASUS_TEMPLATE_SPECIALIZATION
inline void AtomicIntTemplate<AtomicType>::inc()
{
    Uint32 x = (Uint32)_rep.n;
    Uint32 old = x;
    x++;
    while ( cs( (cs_t*)&old, &(_rep.n), (cs_t)x) )
    {
       x = (Uint32)_rep.n;
       old = x;
       x++;
    }
}

PEGASUS_TEMPLATE_SPECIALIZATION
inline void AtomicIntTemplate<AtomicType>::dec()
{
    Uint32 x = (Uint32)_rep.n;
    Uint32 old = x;
    x--;
    while ( cs( (cs_t*)&old, &(_rep.n), (cs_t) x) )
    {
       x = (Uint32) _rep.n;
       old = x;
       x--;
    }
}

PEGASUS_TEMPLATE_SPECIALIZATION
inline bool AtomicIntTemplate<AtomicType>::decAndTestIfZero()
{
    Uint32 x = (Uint32)_rep.n;
    Uint32 old = x;
    x--;
    while ( cs( (cs_t*)&old, &(_rep.n), (cs_t) x) )
    {
       x = (Uint32) _rep.n;
       old = x;
       x--;
    }
    return x==0;
}

PEGASUS_NAMESPACE_END

#endif /* _Pegasus_Common_AtomicInt_ZOS_ZSERIES_IBM_h */
