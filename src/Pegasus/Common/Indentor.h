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

#ifndef Pegasus_Indentor_h
#define Pegasus_Indentor_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Linkage.h>
#include <iostream>

PEGASUS_NAMESPACE_BEGIN

/**
    This class provides a convenient way of inserting indentation for all
    the print routines in this library. Consider this example:

    <pre>
	cout << Indentor(3) << "name=" << name << endl;
    </pre>

    This indents three levels (each level indents four spaces).
*/
class PEGASUS_COMMON_LINKAGE Indentor
{
public:

    Indentor(Uint32 level) : _level(level) { }

    PEGASUS_COMMON_LINKAGE friend PEGASUS_STD(ostream)& operator<<(
	PEGASUS_STD(ostream)& os, const Indentor& x);

    Uint32 getLevel() const { return _level; }

private:

    Uint32 _level;
};

PEGASUS_COMMON_LINKAGE PEGASUS_STD(ostream)& operator<<(PEGASUS_STD(ostream)& os, const Indentor& x);

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Indentor_h */
