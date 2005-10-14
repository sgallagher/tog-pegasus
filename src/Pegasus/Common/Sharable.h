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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By: Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//              David Eger (dteger@us.ibm.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Sharable_h
#define Pegasus_Sharable_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Linkage.h>
#include <Pegasus/Common/IPC.h>
#include <assert.h>

PEGASUS_NAMESPACE_BEGIN

/** 
    The Sharable class implements a simple reference counting scheme. 
    The static Inc() method increments the reference count member. 
    The static Dec() method decrements the reference count and deletes
    the object when the reference count becomes zero.

    Other classes may inherit this reference counting behavior simply
    by deriving from this class.
*/
class PEGASUS_COMMON_LINKAGE Sharable
{
public:

    Sharable() : _ref(1) { }

    virtual ~Sharable();
    Uint32 getRef() const { return _ref.value(); }

    friend void Inc(Sharable* sharable); 

    friend void Dec(Sharable* sharable);

private:
    Sharable(const Sharable &s) : _ref(1) {assert(0);} 
    // we should never copy a counter, so we make this private - dte
    AtomicInt _ref;
};

inline void Inc(Sharable* x)
{
  if (x)
    {
      // A sharable object should never be incremented from zero.
      // If so, there is a double delete being cause by impropoer use
      // of sharable assignment or copy constructors somewhere
      // << Wed Nov  6 12:46:52 2002 mdd >>
      assert(((Sharable*)x)->_ref.value());
      x->_ref++;
    }
  
}


inline void Dec(Sharable* x)
{
  if (x && x->_ref.DecAndTestIfZero())
    delete x;
}

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Sharable_h */
