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
// $Log: Name.h,v $
// Revision 1.2  2001/01/24 17:17:05  karl
// add Doc++ comments
//
// Revision 1.1.1.1  2001/01/14 19:52:59  mike
// Pegasus import
//
//
//END_HISTORY

/*
 Name.h	- This header file defines the name class
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
@return ATTN:
*/
class PEGASUS_COMMON_LINKAGE Name
{
public:
    /** Method legal - Determine if the name string input is legal as
    defnined in the Name class definition
    @param - String to test
    @return Returns true if the given name is legal. Throws
    NullPointer exception if name argument is null.
    */
    static Boolean legal(const Char16* name);
    /** Method legal - Determine if the name string input is legal as
    defnined in the Name class definition
    @param - String to test
    @return Returns true if the given name is legal. Throws
    NullPointer exception if name argument is null.
    */
    static Boolean legal(const String& name)
    {
	return legal(name.getData());
    }
    /** Method equal - Compares two names.
    @return Return true if the two names are equal. CIM names are
    case insensitive and so it this method.
    */
    static Boolean equal(const String& name1, const String& name2);

private:

    Name() { }
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Name_h */
