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
// Modified By: Roger Kumpf, Hewlett Packard Company (roger_kumpf@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Name_h
#define Pegasus_Name_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Linkage.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/Exception.h>

PEGASUS_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
//
// CIMName
//
////////////////////////////////////////////////////////////////////////////////

/**
    The CIMName class defines methods for handling CIM names.
    <p>
    The names of classes, properties, qualifiers, and methods are all
    CIM names. A CIM name must match the following regular
    expression:
    <PRE>

	[A-Z-a-z_][A-Za-z_0-9]*
    </PRE>

    Notice that the definition of a name is the same as C, C++,
    and Java.
*/
class PEGASUS_COMMON_LINKAGE CIMName
{
public:

    CIMName();
    CIMName(const String& name);
    CIMName(const char* name);

    CIMName& operator=(const CIMName& name);
    CIMName& operator=(const String& name);
    CIMName& operator=(const char* name);

#if 0
    String toString() const;
#endif

    operator String() const;

    Boolean isNull() const;

    void clear();

    /** equal - Compares two names.
	@return Return true if the two names are equal. CIM names are
	case insensitive and so it this method.
    */
    Boolean equal(const CIMName& name) const;

    //  ATTN: Define what is legal
    /** legal - Determine if the name string input is legal as
	defnined in the CIMName class definition
	@param - String to test
	@return Returns true if the given name is legal.
    */
    static Boolean legal(const String& name) throw();

#if 0
    /** equal - Compares two names.
	@return Return true if the two names are equal. CIM names are
	case insensitive and so it this method.
    */
    static Boolean equal(const String& name1, const String& name2) throw();
#endif

private:
    String cimName;
};

#if 0
PEGASUS_COMMON_LINKAGE PEGASUS_STD(ostream)& operator<<(
    PEGASUS_STD(ostream)& os,
    const CIMName& name);
#endif

#define PEGASUS_ARRAY_T CIMName
# include "ArrayInter.h"
#undef PEGASUS_ARRAY_T


////////////////////////////////////////////////////////////////////////////////
//
// CIMNamespaceName
//
////////////////////////////////////////////////////////////////////////////////

/**
    The CIMNameSpace class defines methods for handling CIM namespace names.
    <p>
    A CIM namespace name must match the following expression:
    <PRE>
        <CIMName>[/<CIMName>]...
    </PRE>
*/
class PEGASUS_COMMON_LINKAGE CIMNamespaceName
{
public:

    CIMNamespaceName();
    CIMNamespaceName(const String& name);
    CIMNamespaceName(const char* name);

    CIMNamespaceName& operator=(const CIMNamespaceName& name);
    CIMNamespaceName& operator=(const String& name);
    CIMNamespaceName& operator=(const char* name);

#if 0
    String toString() const;
#endif

    operator String() const;

    Boolean isNull() const;

    void clear();

    /** equal - Compares two names.
	@return Return true if the two names are equal. CIM names are
	case insensitive and so it this method.
    */
    Boolean equal(const CIMNamespaceName& name) const;

    //  ATTN: Define what is legal
    /** legal - Determine if the name string input is legal as
	defnined in the CIMNamespaceName class definition
	@param - String to test
	@return Returns true if the given name is legal.
    */
    static Boolean legal(const String& name) throw();

#if 0
    /** equal - Compares two names.
	@return Return true if the two names are equal. CIM names are
	case insensitive and so it this method.
    */
    static Boolean equal(const String& name1, const String& name2) throw();
#endif

private:
    String cimNamespaceName;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Name_h */
