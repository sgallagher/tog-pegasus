//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software, Hewlett-Packard Company, IBM,
// The Open Group, Tivoli Systems
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
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Sharable_h
#define Pegasus_Sharable_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Linkage.h>

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

    Uint32 getRef() const { return _ref; }

    friend void Inc(const Sharable* sharable);

    friend void Dec(const Sharable* sharable);

private:

    Uint32 _ref;
};

inline void Inc(const Sharable* x)
{
    if (x)
	((Sharable*)x)->_ref++;
}

inline void Dec(const Sharable* x)
{
    if (x && --((Sharable*)x)->_ref == 0)
	delete (Sharable*)x;
}

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Sharable_h */
