//BEGIN_LICENSE
//
// Copyright (c) 2000 The Open Group, BMC Software, Tivoli Systems, IBM
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
//END_LICENSE
//BEGIN_HISTORY
//
// Author:
//
// $Log: Indentor.h,v $
// Revision 1.1  2001/01/14 19:52:37  mike
// Initial revision
//
//
//END_HISTORY

////////////////////////////////////////////////////////////////////////////////
//
// Indentor.h
//
//	This class provides a convenient way of inserting indentation for all
//	the print routines in this library. Consider this example:
//
//	    cout << Indentor(3) << "name=" << name << endl;
//
//	This indents three levels (each level indents four spaces).
//
////////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Indentor_h
#define Pegasus_Indentor_h

#include <Pegasus/Common/Config.h>
#include <iostream>

PEGASUS_NAMESPACE_BEGIN

// ATTN: MSVC 5.0 Hack!
class Indentor;
PEGASUS_COMMON_LINKAGE std::ostream& operator<<(std::ostream& os, const Indentor& x);

class PEGASUS_COMMON_LINKAGE Indentor
{
public:

    Indentor(Uint32 level) : _level(level) { }

    PEGASUS_COMMON_LINKAGE friend std::ostream& operator<<(
	std::ostream& os, const Indentor& x);

    Uint32 getLevel() const { return _level; }

private:

    Uint32 _level;
};

PEGASUS_COMMON_LINKAGE std::ostream& operator<<(std::ostream& os, const Indentor& x);

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Indentor_h */
