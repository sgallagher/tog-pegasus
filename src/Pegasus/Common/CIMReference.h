//%/////////////////////////////////////////////////////////////////////////////
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
//==============================================================================
//
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Reference_h
#define Pegasus_Reference_h

#include <iostream>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMName.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/Exception.h>

PEGASUS_NAMESPACE_BEGIN

class CIMReference;

/** The KeyBinding class associates a key name, value, and type.
    It is used by the reference class to represent key bindings.
    See the CIMReference class to see how they are used.
*/
class PEGASUS_COMMON_LINKAGE KeyBinding
{
public:

    enum Type { BOOLEAN, STRING, NUMERIC };

    /** Default constructor */
    KeyBinding();

    /** Copy constructor */
    KeyBinding(const KeyBinding& x);

    /** Construct a KeyBinding with a name, value, and type */
    KeyBinding(const String& name, const String& value, Type type);

    /** Destructor */
    ~KeyBinding();

    /** Assignment operator */
    KeyBinding& operator=(const KeyBinding& x);

    /** Accessor */
    const String& getName() const
    {
	return _name;
    }

    /** Modifier */
    void setName(const String& name)
    {
	_name = name;
    }

    /** Accessor */
    const String& getValue() const
    {
	return _value;
    }

    /** Modifier */
    void setValue(const String& value)
    {
	_value = value;
    }

    /** Accessor */
    Type getType() const
    {
	return _type;
    }

    /** Modifier */
    void setType(Type type)
    {
	_type = type;
    }

    /** Converts the given type to one of the following:
	"boolean", "string", or "numeric"
    */
    static const char* typeToString(Type type);

private:

    String _name;
    String _value;
    Type _type;

    friend Boolean operator==(const KeyBinding& x, const KeyBinding& y);
    friend class CIMReference;
};

inline Boolean operator==(const KeyBinding& x, const KeyBinding& y)
{
    return
	CIMName::equal(x._name, y._name) &&
	String::equal(x._value, y._value) &&
	x._type == y._type;
}

#define PEGASUS_ARRAY_T KeyBinding
# include "ArrayInter.h"
#undef PEGASUS_ARRAY_T

typedef Array<KeyBinding> KeyBindingArray;

class XmlWriter;

/** The CIMReference class represents the value of a reference. A reference
    is one of property types which an association may contain. Consider the
    following MOF for example:

    <pre>
    [Association]
    class MyAssociations
    {
	MyClass ref from;
	MyClass ref to;
    };
    </pre>

    The value of the from and to properties are internally represented using
    the CIMReference class.

    CIM references are used to uniquely identify a CIM class or CIM instance
    objects. CIMReference objects contain the following parts:

    <ul>
    <li>Host - name of host whose repository contains the object</li>
    <li>NameSpace - the namespace which contains the object</li>
    <li>ClassName - name of objects class</li>
    <li>KeyBindings key/value pairs which uniquely identify an instance</li>
    </ul>

    CIM references may also be expressed as simple strings (as opposed to
    being represented by the CIMReference class). This string is known as
    the "Object Name". An object name has the following form:

    <pre>
    &lt;namespace-path&gt;:&lt;model-path&gt;
    </pre>

    The namespace-path is implementation dependent and has the following form
    in Pegasus:

    <pre>
    //&lt;hostname&gt;>/&ltnamespace&gt;>
    </pre>

    For example, suppose there is a host named "atp" with a CIM Server
    listening on port 9999 which has a CIM repository with a namespace
    called "root/cimv25". Then the namespace-path is given as:

    <pre>
    //atp:9999/root/cimv25
    </pre>

    As for the model-path mentioned above, its form is defined by the CIM
    Standard (more is defined by the "XML Mapping Specification v2.0.0"
    specification) as follows:

    <pre>

    &lt;Qualifyingclass&gt;.&lt;key-1&gt;=&lt;value-1&gt;[,&lt;key-n&gt;=
    &lt;value-n&gt;]*
    </pre>

    For example:

    <pre>
    TennisPlayer.first="Patrick",last="Rafter"
    </pre>

    This of course presupposes the existence of a class called "TennisPlayer"
    that has key properties named "first" and "last". For example, here is what
    the MOF might look like:

    <pre>
    class TennisPlayer : Person
    {
	[key] string first;
	[key] string last;
    };
    </pre>

    All keys must be present in the model path.

    Now the namespace-type and model-path are combined in the following
    string object name.

    //atp:9999/root/cimv25:TennisPlayer.first="Patrick",last="Rafter"

    Now suppose we wish to create a CIMReference from this above string. There
    are two constructors provided: one which takes the above string and the
    other that takes the constituent elements. Here are the signature of the
    two constructors:

    <pre>
    CIMReference(const String& objectName);

    CIMReference(
	const String& host,
	const String& nameSpace,
	const String& className,
	const KeyBindingArray& keyBindings);
    </pre>

    Following our example, the above object name may be used to initialize
    a CIMReference like this:

	<pre>
	CIMReference ref =
	    "//atp:9999/root/cimv25:TennisPlayer.first="Patrick",last="Rafter";
	</pre>

    A CIMReference may also be initialized using the constituent elements
    of the object name (sometimes the object name is not available as a string:
    this is the case with CIM XML encodings). The arguments shown in that
    constructor above correspond elements of the object name in the following
    way:

    <ul>
    <li>host = "atp:9999"</li>
    <li>nameSpace = "root/cimv25"</li>
    <li>className = "TennisPlayer"</li>
    <li>keyBindings = "first=\"Patrick\",last=\"Rafter\""</li>
    </ul>

    Note that the host and nameSpace argument may be empty since object names
    need not necessarily include a namespace path according to the standard.

    The key bindings must be built up by appending KeyBinding objects
    to a KeyBindingArray like this:

    <pre>
    KeyBindingArray keyBindings;
    keyBindings.append(KeyBinding("first", "Patrick", KeyBinding::STRING));
    keyBindings.append(KeyBinding("last", "Rafter", KeyBinding::STRING));
    </pre>

    The only key values that are supported are:

    <ul>
    <li>KeyBinding::BOOLEAN</li>
    <li>KeyBinding::STRING</li>
    <li>KeyBinding::NUMERIC</li>
    </ul>

    This limitation is imposed by the "XML Mapping Specification v2.0.0"
    specification. The CIM types are encoded as one of these three in the
    following way:

    <pre>
    boolean - BOOLEAN (the value must be "true" or "false")
    uint8 - NUMERIC
    sint8 - NUMERIC
    uint16 - NUMERIC
    sint16 - NUMERIC
    uint32 - NUMERIC
    sint32 - NUMERIC
    uint64 - NUMERIC
    sint64 - NUMERIC
    char16 - NUMERIC
    string - STRING
    datetime - STRING
    </pre>

    Notice that real32 and real64 are missing. Properties of these types
    cannot be used as keys.

    Notice that the keys in the object name may appear in any order.
    That is the following object names refer to the same object:

    <pre>
    TennisPlayer.first="Patrick",last="Rafter"
    TennisPlayer.last="Rafter",first="Patrick"
    </pre>

    And since CIM is not case sensitive, the following refer to the same
    object:

    <pre>
    TennisPlayer.first="Patrick",last="Rafter"
    tennisplayer.FIRST="Patrick",Last="Rafter"
    </pre>

    Therefore, the CIMReferences::operator==() would return true for the last
    two examples.

    The CIM standard leaves it an open question whether model paths may have
    spaces around delimiters (like '.', '=', and ','). We assume they cannot.
    So the following is an invalid model path:

    <pre>
    TennisPlayer . first = "Patrick", last="Rafter"
    </pre>

    We require that the '.', '=', and ',' have no spaces around them.

    For reasons of efficiency, the key bindings are internally sorted
    during initialization. This allows the key bindings to be compared
    more easily. This means that when the string is converted back to
    string (by calling toString()) that the keys may have been rearranged.

    There are two forms an object name can take:

    <pre>
    &lt;namespace-path&gt;:&lt;model-path&gt;
    &lt;model-path&gt;
    </pre>

    In other words, the namespace-path is optional. Here is an example of
    each:

    <pre>
    //atp:9999/root/cimv25:TennisPlayer.first="Patrick",last="Rafter"
    TennisPlayer.first="Patrick",last="Rafter"
    </pre>

    If it begins with "//" then we assume the namespace-path is present and
    process it that way.

    It should also be noted that an object name may refer to an instance or
    a class. Here is an example of each:

    <pre>
    TennisPlayer.first="Patrick",last="Rafter"
    TennisPlayer
    </pre>

    In the second case--when it refers to a class--the key bindings are
    omitted.
*/
class PEGASUS_COMMON_LINKAGE CIMReference
{
public:

    /** Default constructor. */
    CIMReference();

    /** Copy constructor. */
    CIMReference(const CIMReference& x);

    /** Initializes a CIMReference object from a CIM object name.
    	@param objectName String representing the object name.
    	@return Returns the initialized CIMReference
    	@exception Throws "IllformedObjectName" exception if the name
    	not parsable.
    	<PRE>
    	    CIMReference r1 = "MyClass.z=true,y=1234,x=\"Hello World\"";

    	</PRE>
    */
    CIMReference(const String& objectName);

    /** Initializes this object from a CIM object name (char* version).
    	@param objectName char* representing the objectName
    	@return
    	@exception Throws "IllformedObjectName" if objectName parameter not
    	parsable as a CIMReference.
    */
    CIMReference(const char* objectName);

    /** Workaround to MSVC++ bug. */
    static KeyBindingArray getKeyBindingArray();

    /** Constructs a CIMReference from constituent elements.
	@param host Name of host (e.g., "nemesis-8888").
	@param nameSpace Namespace (e.g., "root/cimv20").
	@param className Name of a class (e.g., "MyClass").
	@param keyBindings An array of KeyBinding objects.
	@return Returns the constructed CIMReference
    */
    CIMReference(
	const String& host,
	const String& nameSpace,
	const String& className,
	const KeyBindingArray& keyBindings = getKeyBindingArray());

    /** Destructor */
    ~CIMReference();

    /** Assignment operator */
    CIMReference& operator=(const CIMReference& x);

    /** Clears out the internal fields of this object making it an empty
	(or unitialized reference). The effect is the same as if the object
	was initialized with the default constructor.
    */
    void clear();

    /** Sets this reference from constituent elements. The effect is same
	as if the object was initialized using the constructor above that
	has the same arguments.
    */
    void set(
	const String& host,
	const String& nameSpace,
	const String& className,
	const KeyBindingArray& keyBindings = getKeyBindingArray());

    /** Set the reference from an object name . */
    void set(const String& objectName);

    /** Same as set() above except that it is an assignment operator */
    CIMReference& operator=(const String& objectName)
    {
	set(objectName);
	return *this;
    }

    /** Same as set() above except that it is an assignment operator */
    CIMReference& operator=(const char* objectName)
    {
	set(objectName);
	return *this;
    }

    /** getHost - returns the hostname component of the
        CIMReference
        @return String contianing hostname.
        <pre>

        </pre>
    */
    const String& getHost() const
    {
	return _host;
    }

    /** setHost Sets the hostname component of the CIMReference
        object to the input parameter
        @param host String parameter with the hostname
        <PRE>
        CIMReference r1;
        r1.sethost("fred-8888");
        </PRE>
        Note that Pegasus does no checking on valid host names.
    */
    void setHost(const String& host)
    {
	_host = host;
    }

    /** getNameSpace - returns the namespace component of the
        CIMReference as a String.
    */
    const String& getNameSpace() const
    {
	return _nameSpace;
    }

    /** Sets the namespace component.
	@param String representing the Namespace. The functions tests for
	a legal name.
	@exception Throws IllegalName if form of the namespace is illegal.
	Pegasus does a limited check on name
    */
    void setNameSpace(const String& nameSpace);

    /** Accessor. */
    const String& getClassName() const
    {
	return _className;
    }

    /** Sets the classname component of the CIMReference object to the input
        parameter.
        @param className String containing the className.
	@exception Throws IllegalName if form of className is illegal.
	A CIM name must match the following regular expression:
	<PRE>
	   [A-Z-a-z_][A-Za-z_0-9]*
        </PRE>
    */
    void setClassName(const String& className);

    /** Accessor. */
    const Array<KeyBinding>& getKeyBindings() const
    {
	return _keyBindings;
    }

    /** Modifier. */
    void setKeyBindings(const Array<KeyBinding>& keyBindings);

    /** Returns the object name represented by this reference.
    The returned string is formed from the hostname, namespace, classname
    and keybindings defined for this CIMReference object.
    the form of the name is:
    <PRE>
    	"//" + hostname + "/" + namespace + ":" + classname +"." +
    		(keyname) + "=" (keyvalue) +"," ...
    </PRE>
    The building includes the escaping of special characters.
    ATTN: The form of the above string definition needs cleaning.
    */
    String toString() const;

    /** Returns true if this reference is identical to the one given
	by the x argument. Since CIMReferences are normalized when they
	are created, any differences in the ordering of keybindings is accounted
	for as are the case insensitivity characteristics defined by
	the specification
	@param CIMReference for comparison
	@return True if the objects are have identical components
	<PRE>
	</PRE>
    */
    Boolean identical(const CIMReference& x) const;

    /** Encodes this object as XML.
	@param out argument in which to place resutls
    */
    void toXml(Array<Sint8>& out) const;

    /** Prints the XML encoding of this object.
    */
    void print(PEGASUS_STD(ostream)& os = PEGASUS_STD(cout)) const;

    /** Generates hash code for the given reference. Two identical references
	generate the same hash code (despite any subtle differences such as
	the case of the classname and key names as well as the order of the
	keys).
    */
    Uint32 makeHashCode() const;

private:

    void nameSpaceToXml(Array<Sint8>& out) const;

    void localNameSpaceToXml(Array<Sint8>& out) const;

    void instanceNameToXml(Array<Sint8>& out) const;

    void classNameToXml(Array<Sint8>& out) const;

    String _host;
    String _nameSpace;
    String _className;
    Array<KeyBinding> _keyBindings;

    friend class XmlWriter;
};

inline Boolean operator==(const CIMReference& x, const CIMReference& y)
{
    return x.identical(y);
}

inline Boolean operator!=(const CIMReference& x, const CIMReference& y)
{
    return !operator==(x, y);
}

inline PEGASUS_STD(ostream)& operator<<(
    PEGASUS_STD(ostream)& os,
    const CIMReference& x)
{
    return os << x.toString();
}

#define PEGASUS_ARRAY_T CIMReference
# include "ArrayInter.h"
#undef PEGASUS_ARRAY_T

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Reference_h */


