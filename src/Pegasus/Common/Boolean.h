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
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Boolean_h
#define Pegasus_Boolean_h

#include <Pegasus/Common/Config.h>

PEGASUS_NAMESPACE_BEGIN

/** This class provides an implementation of the boolean for compilers which
    do not support "bool" as a type.

    This class is not used by modern C++ compilers.
*/
class Boolean
{
public:

    Boolean() { }

    ~Boolean() { }

    Boolean(const Boolean& x) : _flag(x._flag) { }

    Boolean(int flag) : _flag(flag ? 1 : 0) { }

    Boolean& operator=(const Boolean& flag) { _flag = flag; return *this; }

    Boolean& operator=(int flag) { _flag = flag ? 1 : 0; return *this; }

    operator int() const { return _flag; }

    friend Boolean operator==(const Boolean& x, const Boolean& y);

    friend Boolean operator!=(const Boolean& x, const Boolean& y);

private:

    char _flag;
};

inline Boolean operator==(const Boolean& x, const Boolean& y)
{
    return x._flag == y._flag;
}

inline Boolean operator!=(const Boolean& x, const Boolean& y)
{
    return !operator==(x, y);
}

#define true Boolean(1)

#define false Boolean(0)

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Boolean_h */
