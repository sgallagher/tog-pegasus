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
// $Log: CIMName.h,v $
// Revision 1.2  2001/04/04 20:02:27  karl
// documentation update
//
// Revision 1.1  2001/02/16 02:07:06  mike
// Renamed many classes and headers (using new CIM prefixes).
//
// Revision 1.4  2001/01/30 08:00:43  karl
// DOC++ Documentation update for header files
//
// Revision 1.3  2001/01/28 04:11:03  mike
// fixed qualifier resolution
//
// Revision 1.2  2001/01/24 17:17:05  karl
// add Doc++ comments
//
// Revision 1.1.1.1  2001/01/14 19:52:59  mike
// Pegasus import
//
//
//END_HISTORY

/*
 CIMName.h	- This header file defines the name class
*/

#ifndef Pegasus_Name_h
#define Pegasus_Name_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Exception.h>

PEGASUS_NAMESPACE_BEGIN

/**
    The name class defines static methods for handling CIM names.
    The names of classes, properties, qualifiers, and methods are all
    CIM names. A CIM name must match the following regular
    expression:
    <PRE>

	[A-Z-a-z_][A-Za-z_0-9]*
    </PRE>

    Notice that the definition of a name is the same as C, C++,
    and Java.

    This class may not be instantiated (since its only constructor is
    private).
*/
class PEGASUS_COMMON_LINKAGE CIMName
{
public:

    /** CIMMethod legal - Determine if the name string input is legal as
	defnined in the CIMName class definition
	ATTN: Define what is legal
	@param - String to test
	@return Returns true if the given name is legal. Throws
	NullPointer exception if name argument is null.
    */

    static Boolean legal(const Char16* name);

    /** CIMMethod legal - Determine if the name string input is legal as
	defnined in the CIMName class definition
	@param - String to test
	@return Returns true if the given name is legal. Throws
	NullPointer exception if name argument is null.
    */

    static Boolean legal(const String& name)
    {
	return legal(name.getData());
    }

    /** CIMMethod equal - Compares two names.
	@return Return true if the two names are equal. CIM names are
	case insensitive and so it this method.
    */

    static Boolean equal(const String& name1, const String& name2);

private:

    CIMName() { }
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Name_h */
