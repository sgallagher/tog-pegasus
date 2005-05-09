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
        [A-Za-z_][A-Za-z_0-9]*
    </PRE>
    <B>Examples:</B>
    <ul>
    <li>name - legal name
    <li>Type - legal name
    <li>3types - Illegal CIM name
    </ul>
    The CIMName class includes the attribute Null which is required
    by the DMTF operations definitions.  Note that this and the regular
    expression limits on CIMName are what separate this from the String
    class. This allows the names in CIM operations, such as getClass, to 
    provide pattern matching tests for the classname parameter as well as
    separate the concept of empty from Null.
    
*/
class PEGASUS_COMMON_LINKAGE CIMName
{
public:

    /**
        Constructs an object with no name.  A call to isNull() immediately
        after constructing the object will return true.

        @exception bad_alloc Thrown if there is insufficient memory.
    */
    CIMName();

    /**
        Constructor creates a new CIMName object from <TT>name</TT>.
        <TT>name</TT> must be a legal CIM name.

        @param name The name to use for the object.
        @exception InvalidNameException if <TT>name</TT> is not
                   a legal CIMName
        @exception bad_alloc Thrown if there is insufficient memory.
    */
    CIMName(const String& name);

    /**
        Constructor creates a new CIMName object from string
        provided as input. The string must be a legal name.

        @param name The name to use for the object.
        @exception InvalidNameException if <TT>name</TT> is not
                   a legal CIMName
        @exception bad_alloc Thrown if there is insufficient memory.
    */
    CIMName(const char* name);

    /**
        Assigns one CIMName to another.

        @param name CIMName object to copy.
        @exception bad_alloc Thrown if there is insufficient memory.
    */
    CIMName& operator=(const CIMName& name);

    /**
        Sets the name of the associated object to <TT>name</TT>.

        @param name The new name to use for the object.
        @exception InvalidNameException if <TT>name</TT> is not
                   a legal CIMName
        @exception bad_alloc Thrown if there is insufficient memory.

        <pre>
        CIMName n;
        String type = "type";
        n = type;
        </pre>
    */
    CIMName& operator=(const String& name);

    /**
        Gets a reference a String containing the name from the
        associated object.

        @return Reference to a String containing the name.

        <pre>
        CIMName n("name");
        String s = n.getString();
        </pre>
    */
    const String& getString() const;

    /**
        Tests if name is Null, i.e. not set.

        @return true if name is Null, false if not.

        <pre>
        CIMName n;
        assert(n.isNull());
        n = "name";
        assert(!n.isNull());
        </pre>
    */
    Boolean isNull() const;

    /**
        Clears the CIMName.

        <pre>
        CIMMame n("name");
        n.clear();
        assert(n.isNull());
        </pre>
    */
    void clear();

    /**
        Compares the CIMName object against another CIMName object
        for equality.

        @param name CIMName to compare with the associated object.
        @return true if the name passed is equal to the name in this
        class. CIM names are case insensitive and so is this method.
        <pre>
        CIMName n1 = "name";
        CIMName n2 = "InstanceID";
        if( n1.equal(n2) )
                ...                     // Should never get here
        else
                ...
        </pre>
    */
    Boolean equal(const CIMName& name) const;

    /**
        Determines if the name string input is legal as
        defined in the CIMName class definition. This is a static
        method used to test String values to determine if they are
        legal names.

        @param name String to test for legality.
        @return Returns true if the name is legal, otherwise false.

        <pre>
        assert(CIMName::legal("name"));
        assert(!CIMName::legal("3types"));
        </pre>
    */
    static Boolean legal(const String& name);

private:
    String cimName;
};

/**
    Compares two CIMNames to determine if they are equal.

    @param name1 One name to compare.
    @param name2 Another name to compare
    @return Returns true if the names are equal, false if they are not.

    CIMNames are not case sensitive, therefore the output of the
    following example is "Equal".

        <pre>
        CIMName lowerCaseName("this_is_a_name");
        CIMName upperCaseName("THIS_IS_A_NAME");

        if (lowerCaseName == upperCaseName)
        {
            puts("Equal");
        }
        else
        {
            puts("Not equal");
        }
        </pre>
 */
PEGASUS_COMMON_LINKAGE Boolean operator==(
    const CIMName& name1,
    const CIMName& name2);

/**
    Compares two CIMNames to determine if they are not equal.

    @param name1 One name to compare.
    @param name2 Another name to compare
    @return Returns true if the names are not equal, false if they are.

    The output of the following example is "Not equal".

        <pre>
        CIMName name1("this_is_a_name");
        CIMName name2("this is another_name");

        if (name1 != name2)
        {
            puts("Not equal");
        }
        else
        {
            puts("Equal");
        }
        </pre>
 */
PEGASUS_COMMON_LINKAGE Boolean operator!=(
    const CIMName& name1,
    const CIMName& name2);

#define PEGASUS_ARRAY_T CIMName
# include "ArrayInter.h"
#undef PEGASUS_ARRAY_T


////////////////////////////////////////////////////////////////////////////////
//
// CIMNamespaceName
//
////////////////////////////////////////////////////////////////////////////////

/**
    The CIMNamespaceName class defines methods for handling CIM namespace names.
    <p>
    A CIM namespace name must match the following expression:
    <PRE>
        &lt;CIMName&gt;[ / &lt;CIMName&gt; ]*
    </PRE>
    </p>
    <B>Examples</B>
    <UL>
    <LI>root
    <LI>root/test
    </UL>
    NOTE: Pegasus uses namespaces starting with the top level name (ex. root).  It does
    not use the form /root/test with a leading slash.  The legal() test method in this class
    allows that form as a legal entity however.
*/
class PEGASUS_COMMON_LINKAGE CIMNamespaceName
{
public:

    /** Default constructor sets object Null. The Null state
            indicates that there is no name assigned to this object.
            The Null state can be tested with the isNull() method and
            set with the clear() method.
        */
    CIMNamespaceName();

    /** Constructor builds namespace from input String.
            The String input must be a legal namespace name.
            @param String from which the namespace object is built.
            This must be a legal namespace name.
            @exeception InvalidNamespaceName exception thrown if
            the namespace name input is illegal.
        */
    CIMNamespaceName(const String& name);

    /** Constructor builds namespace from input char*.
            The String input must be a legal namespace name.
            @param char* from which the namespace object is built.
            This must be a legal namespace name.
            @exeception InvalidNamespaceName exception thrown if
            the namespace name input parameter is illegal.
        */    
    CIMNamespaceName(const char* name);

    /** Assign one namespace object to another.
                @param CIMNamespaceName to assign to the object.
        */
    CIMNamespaceName& operator=(const CIMNamespaceName& name);

    /** Assign a String object to a CIMNamespaceName object.
                @param CIMNamespaceName to assign
                @exeception InvalidNamespaceName exception thrown if
                the namespace name input parameter is illegal.
            <pre>
                        String s = "root/test";
                        CIMNamespacename ns;
                        ns = s;
            </pre>
        */
    CIMNamespaceName& operator=(const String& name);

    /** Extracts the String value of the CIMNamespaceName
            from the object.
            @return String containing the name.
            <pre>
                        CIMNamespaceName ns("root/test");
                        String s = ns.getString();
            </pre>
        */
    const String& getString() const;

    /**	Tests the CIMNamespaceName for NULL attribute. Returns
            true if Null.  New objects without parameter and objects
            set with clear() are Null.  When a name is set into the
            object is is set to nonnull.  When the object is Null, it
            returns empty string.
            @return true if Null or false if not Null.
            <pre>
                        CIMName n;
                        assert(n.isNull());
                        n = "name";
                        assert(!n.isNull());
            </pre>
        */
    Boolean isNull() const;

    /**	Clears the CIMNamespaceName and sets it to Null. A Null
            object contains no name so that accessing it with getString
            should return an empty String
            <pre>
                        CIMMamespaceName ns("root/test");
                        ns.clear();
                        assert(ns.isNull());
            </pre>
        */
    void clear();

    /** Compares two CIMNamespace objects for equality.
                @return true if the name passed is equal to the name in this
        class. CIM names are case insensitive and so is this method.
                <pre>
                CIMMamespaceName ns("root/test");
                CIMMamespaceName ns1("root/test");
                assert( ns.equal(ns1);
            </pre>
    */
    Boolean equal(const CIMNamespaceName& name) const;

    /** Determines if the name string input is legal as
        defined in the CIMNamespaceName class definition.
        @param name String to test for legality.
        @return true if the given name is legal, false otherwise.
        <pre>
                assert(CIMNamespaceName::legal("root/test"));
        </pre>
    */
    static Boolean legal(const String& name);

private:
    String cimNamespaceName;
};

PEGASUS_COMMON_LINKAGE Boolean operator==(
    const CIMNamespaceName& name1,
    const CIMNamespaceName& name2);

PEGASUS_COMMON_LINKAGE Boolean operator!=(
    const CIMNamespaceName& name1,
    const CIMNamespaceName& name2);

#define PEGASUS_ARRAY_T CIMNamespaceName
# include "ArrayInter.h"
#undef PEGASUS_ARRAY_T

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Name_h */
