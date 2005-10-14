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
// Author: Mike Brasher (mbrasher@austin.rr.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef _Pegasus_Common_AtomicInt_Generic_h
#define _Pegasus_Common_AtomicInt_Generic_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/IPC.h>

PEGASUS_NAMESPACE_BEGIN

struct AtomicType
{
    Mutex mutex;
    Uint32 n; 
};

inline AtomicIntTemplate<AtomicType>::AtomicIntTemplate(Uint32 n)
{
    _rep.n = n;
}

inline AtomicIntTemplate<AtomicType>::~AtomicIntTemplate()
{
}

inline Uint32 AtomicIntTemplate<AtomicType>::get() const
{
    ((This*)this)->_rep.mutex.lock(pegasus_thread_self());
    Uint32 tmp = _rep.n;
    ((This*)this)->_rep.mutex.unlock();

    return tmp;
}

inline void AtomicIntTemplate<AtomicType>::set(Uint32 n)
{
    _rep.mutex.lock(pegasus_thread_self());
    _rep.n = n;
    _rep.mutex.unlock();
}

inline void AtomicIntTemplate<AtomicType>::inc()
{
    _rep.mutex.lock(pegasus_thread_self());
    _rep.n++;
    _rep.mutex.unlock();
}

inline void AtomicIntTemplate<AtomicType>::dec()
{
    _rep.mutex.lock(pegasus_thread_self());
    _rep.n--;
    _rep.mutex.unlock();
}

inline bool AtomicIntTemplate<AtomicType>::dec_and_test()
{
    _rep.mutex.lock(pegasus_thread_self());
    Uint32 tmp = --_rep.n;
    _rep.mutex.unlock();

    return tmp == 0;
}

PEGASUS_NAMESPACE_END

#endif /* _Pegasus_Common_AtomicInt_Generic_h */
