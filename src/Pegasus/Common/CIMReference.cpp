//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 The Open group, BMC Software, Tivoli Systems, IBM
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
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <cctype>
#include <cstring>
#include "HashTable.h"
#include "CIMReference.h"
#include "Indentor.h"
#include "CIMName.h"
#include "Destroyer.h"
#include "XmlWriter.h"
#include "XmlReader.h"
#include "Array.h"
#include "CIMOMPort.h"

PEGASUS_NAMESPACE_BEGIN

#define PEGASUS_ARRAY_T CIMReference
# include "ArrayImpl.h"
#undef PEGASUS_ARRAY_T

#define PEGASUS_ARRAY_T KeyBinding
# include "ArrayImpl.h"
#undef PEGASUS_ARRAY_T

// ATTN: add a resolve method to this class to verify that the
// reference is correct (that the class name corresponds to a real
// class and that the property names are really keys and that all keys
// of the class or used. Also be sure that there is a valid conversion
// between the string value and the value of that property).
//
// ATTN: also check to see that the reference refers to a class that is the
// same or derived from the _className member.

static String _escapeSpecialCharacters(const String& str)
{
    String result;

    for (Uint32 i = 0, n = str.size(); i < n; i++)
    {
	switch (str[i])
	{
	    case '\n':
		result += "\\n";
		break;

	    case '\r':
		result += "\\r";
		break;

	    case '\t':
		result += "\\t";
		break;

	    case '"':
		result += "\\\"";
		break;

	    default:
		result += str[i];
	}
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////
//
// Local routines:
//
////////////////////////////////////////////////////////////////////////////////

int _Compare(const String& s1_, const String& s2_)
{
    const Char16* s1 = s1_.getData();
    const Char16* s2 = s2_.getData();

    while (*s1 && *s2)
    {
	char c1 = tolower(*s1++);
	char c2 = tolower(*s2++);
	int r = c1 - c2;

	if (r)
	    return r;
    }

    if (*s2)
	return -1;
    else if (*s1)
	return 1;

    return 0;
}

static void _BubbleSort(Array<KeyBinding>& x)
{
    Uint32 n = x.size();

    if (n < 2)
	return;

    for (Uint32 i = 0; i < n - 1; i++)
    {
	for (Uint32 j = 0; j < n - 1; j++)
	{
	    if (_Compare(x[j].getName(), x[j+1].getName()) > 0)
	    {
		KeyBinding t = x[j];
		x[j] = x[j+1];
		x[j+1] = t;
	    }
	}
    }
}

////////////////////////////////////////////////////////////////////////////////
//
// KeyBinding
//
////////////////////////////////////////////////////////////////////////////////

KeyBinding::KeyBinding() { }

KeyBinding::KeyBinding(const KeyBinding& x)
    : _name(x._name), _value(x._value), _type(x._type)
{

}

KeyBinding::KeyBinding(const String& name, const String& value, Type type)
    : _name(name), _value(value), _type(type)
{

}

KeyBinding::~KeyBinding()
{

}

KeyBinding& KeyBinding::operator=(const KeyBinding& x)
{
    _name = x._name;
    _value = x._value;
    _type = x._type;
    return *this;
}

////////////////////////////////////////////////////////////////////////////////
//
// CIMReference
//
////////////////////////////////////////////////////////////////////////////////

CIMReference::CIMReference()
{

}

CIMReference::CIMReference(const CIMReference& x)
    : _host(x._host), _nameSpace(x._nameSpace),
    _className(x._className), _keyBindings(x._keyBindings)
{
    _BubbleSort(_keyBindings);
}

CIMReference::CIMReference(const String& objectName)
{
    set(objectName);
}

CIMReference::CIMReference(const char* objectName)
{
    set(objectName);
}

CIMReference::CIMReference(
    const String& host,
    const String& nameSpace,
    const String& className,
    const Array<KeyBinding>& keyBindings)

{
   set(host, nameSpace, className, keyBindings);
}

CIMReference::~CIMReference()
{

}

CIMReference& CIMReference::operator=(const CIMReference& x)
{
    if (&x != this)
    {
	_host = x._host;
	_nameSpace = x._nameSpace;
	_className = x._className;
	_keyBindings = x._keyBindings;
    }
    return *this;
}

void CIMReference::clear()
{
    _host.clear();
    _nameSpace.clear();
    _className.clear();
    _keyBindings.clear();
}

void CIMReference::set(
    const String& host,
    const String& nameSpace,
    const String& className,
    const Array<KeyBinding>& keyBindings)
{
   setHost(host);
   setNameSpace(nameSpace);
   setClassName(className);
   setKeyBindings(keyBindings);
}

Boolean CIMReference::_parseHostElement(
    const String& objectName,
    char*& p,
    String& host)
{
    // See if there is a host name (true if it begins with "//"):
    // Host is of the from <hostname>-<port> and begins with "//"
    // and ends with "/":

    if (p[0] != '/' || p[1] != '/')
    {
        return false;
    }

    p += 2;

    //----------------------------------------------------------------------
    // Validate the hostname. Hostnames must match the following
    // regular expression: "[A-Za-z][A-Za-z0-9-]*"
    //----------------------------------------------------------------------

    char* q = p;

    if (!isalpha(*q))
        throw IllformedObjectName(objectName);

    q++;

    while (isalnum(*q) || *q == '-')
        q++;

    // We now expect a port (or default the port).

    if (*q == ':')
    {
        q++;
        // Check for a port number:

        if (!isdigit(*q))
            throw IllformedObjectName(objectName);
	
        while (isdigit(*q))
            q++;

        // Finally, assign the host name:

        _host.assign(p, q - p);
    }
    else
    {
        _host.assign(p, q - p);

        // Assign the default port number:

        _host.append(":");
        _host.append(PEGASUS_CIMOM_DEFAULT_PORT_STRING);
    }

    // Check for slash terminating the entire sequence:

    if (*q != '/')
    {
        host.clear();
        throw IllformedObjectName(objectName);
    }

    p = ++q;

    return true;
}

Boolean CIMReference::_parseNamespaceElement(
    const String& objectName,
    char*& p,
    String& nameSpace)
{
    // If we don't find a valid namespace name followed by a ':', we
    // assume we're not looking at a namespace name.

    //----------------------------------------------------------------------
    // Validate the namespace path.  Namespaces must match the following
    // regular expression: "[A-Za-z_]+(/[A-Za-z_]+)*"
    //----------------------------------------------------------------------

    char* q = p;

    for (;;)
    {
        // Pass the next token:

        if (!*q || (!isalpha(*q) && *q != '_'))
            return false;

        q++;

        while (isalnum(*q) || *q == '_')
            q++;

        if (!*q)
            return false;

        if (*q == ':')
            break;

        if (*q == '/')
        {
            q++;
            continue;
        }

        return false;
    }

    nameSpace.assign(p, q - p);
    p = ++q;
    return true;
}

/**
    ATTN: RK - Association classes have keys whose types are
    references.  These reference values must be treated specially
    in the XML encoding, using the VALUE.REFERENCE tag structure.

    Pegasus had been passing reference values simply as String
    values.  For example, EnumerateInstanceNames returned
    KEYVALUEs of string type rather than VALUE.REFERENCEs.

    I've modified the XmlReader::getKeyBindingElement() and
    XmlWriter::appendInstanceNameElement() methods to read and write
    the XML in the proper format.  However, making that change
    required that a CIMReference object be able to distinguish
    between a key of String type and a key of reference type.

    I've modified the String format of CIMReferences slightly to
    allow efficient processing of references whose keys are also
    of reference type.  The "official" form uses the same
    encoding for key values of String type and of reference type,
    and so it would be necessary to retrieve the class definition
    and look up the types of the key properties to determine how
    to treat the key values.  This is clearly too inefficient for
    internal transformations between CIMReferences and String
    values.

    The workaround is to encode a 'R' at the beginning of the
    value for a key of reference type (before the opening '"').
    This allows the parser to know a priori whether the key is of
    String or reference type.

    In this example:

        MyClass.Key1="StringValue",Key2=R"RefClass.KeyA="StringA",KeyB=10"

    Property Key1 of class MyClass is of String type, and so it
    gets the usual encoding.  Key2 is a reference property, so
    the extra 'R' is inserted before its encoded value.  Note
    that this algorithm is recursive, such that RefClass could
    include KeyC of reference type, which would also get encoded
    with the 'R' notation.

    The toString() method inserts the 'R' to provide symmetry.  A
    new KeyBinding type (REFERENCE) has been defined to denote
    keys in a CIMReference that are of reference type.  This
    KeyBinding type must be used appropriately for
    CIMReference::toString() to behave correctly.

    A result of this change is that instances names in the
    instance repository will include this extra 'R' character.
    Note that for user-facing uses of the String encoding of
    instance names (such as might appear in MOF for static
    association instances or in the CGI client), this solution
    is non-standard and therefore unacceptable.  It is likely
    that these points will need to process the more expensive
    operation of retrieving the class definition to determine
    the key property types.
*/
void CIMReference::_parseKeyBindingPairs(
    const String& objectName,
    char*& p,
    Array<KeyBinding>& keyBindings)
{
    // Get the key-value pairs:

    while (*p)
    {
        // Get key part:

        char* key = strtok(p, "=");

	if (!key)
        {
	    throw IllformedObjectName(objectName);
        }

	String keyString(key);

	if (!CIMName::legal(keyString))
	    throw IllformedObjectName(objectName);

	// Get the value part:

	String valueString;
        p = p + strlen(key) + 1;
	KeyBinding::Type type;

	if (*p == 'R')
	{
	    p++;

	    type = KeyBinding::REFERENCE;

	    if (*p++ != '"')
		throw IllformedObjectName(objectName);

	    while (*p && *p != '"')
	    {
		// ATTN: need to handle special characters here:

		if (*p == '\\')
		    *p++;

		valueString.append(*p++);
	    }

	    if (*p++ != '"')
		throw IllformedObjectName(objectName);
	}
	else if (*p == '"')
	{
	    p++;

	    type = KeyBinding::STRING;

	    while (*p && *p != '"')
	    {
		// ATTN: need to handle special characters here:

		if (*p == '\\')
		    *p++;

		valueString.append(*p++);
	    }

	    if (*p++ != '"')
		throw IllformedObjectName(objectName);
	}
	else if (toupper(*p) == 'T' || toupper(*p) == 'F')
	{
	    type = KeyBinding::BOOLEAN;

            char* r = p;
            Uint32 n = 0;

	    while (*r && *r != ',')
	    {
		*r = toupper(*r);
                r++;
                n++;
	    }

	    if (strncmp(p, "TRUE", n) != 0 && strncmp(p, "FALSE", n) != 0)
		throw IllformedObjectName(objectName);

	    valueString.assign(p, n);

            p = p + n;
	}
	else
	{
	    type = KeyBinding::NUMERIC;

            char* r = p;
            Uint32 n = 0;

	    while (*r && *r != ',')
	    {
                r++;
                n++;
	    }

            Boolean isComma = false;
            if (*r)
            {
                *r = '\0';
                isComma = true;
            }

	    Sint64 x;

	    if (!XmlReader::stringToSignedInteger(p, x))
		throw IllformedObjectName(objectName);

            valueString.assign(p, n);

            if (isComma)
            {
                *r = ',';
            }

            p = p + n;
	}

	keyBindings.append(KeyBinding(keyString, valueString, type));

        if (*p)
        {
            if (*p++ != ',')
            {
                throw IllformedObjectName(objectName);
            }
        }
    }

    _BubbleSort(keyBindings);
}

void CIMReference::set(const String& objectName)
{
    _host.clear();
    _nameSpace.clear();
    _className.clear();
    _keyBindings.clear();

    //--------------------------------------------------------------------------
    // We will extract components from an object name. Here is an sample
    // object name:
    //
    //     //atp:9999/root/cimv25:TennisPlayer.first="Patrick",last="Rafter"
    //--------------------------------------------------------------------------

    // Convert to a C String first:

    char* p = objectName.allocateCString(1);
    ArrayDestroyer<char> destroyer(p);
    Boolean gotHost;
    Boolean gotNamespace;

    // null terminate the C String
    // ATTN-RK-P3-20020301: Is the +1 correct?
    p[objectName.size() + 1]= '\0';

    gotHost = _parseHostElement(objectName, p, _host);
    gotNamespace = _parseNamespaceElement(objectName, p, _nameSpace);

    if (gotHost && !gotNamespace)
    {
        throw IllformedObjectName(objectName);
    }

    // Extract the class name:

    char* dot = strchr(p, '.');

    if (!dot)
    {
	if (!CIMName::legal(p))
        {
	    throw IllformedObjectName(objectName);
        }

	// ATTN: remove this later: a reference should only be able to hold
	// an instance name.

	_className.assign(p);
	return;
    }

    _className.assign(p, dot - p);

    // Advance past dot:

    p = dot + 1;

    _parseKeyBindingPairs(objectName, p, _keyBindings);
}

void CIMReference::setNameSpace(const String& nameSpace)
{
    String temp;

    // check each namespace segment (delimted by '/') for correctness

    for(Uint32 i = 0; i < nameSpace.size(); i += temp.size() + 1)
    {
        // isolate the segment beginning at i and ending at the first
	// ocurrance of '/' after i or eos

	temp = nameSpace.subString(i, nameSpace.subString(i).find('/'));

	// check segment for correctness

	if(!CIMName::legal(temp))
	{
	    throw IllegalName() ;
	}
    }

   _nameSpace = nameSpace;
}

void CIMReference::setClassName(const String& className)
{
    if (!CIMName::legal(className))
	throw IllegalName();

    _className = className;
}

void CIMReference::setKeyBindings(const Array<KeyBinding>& keyBindings)
{
    _keyBindings = keyBindings;
    _BubbleSort(_keyBindings);
}

String CIMReference::toString(Boolean includeHost) const
{
    String objectName;

    // Get the host:

    if (_host.size() && includeHost)
    {
	objectName = "//";
	objectName += _host;
	objectName += "/";
    }

    // Get the namespace (if we have a host name, we must write namespace):

    if (_nameSpace.size() || _host.size())
    {
	objectName += _nameSpace;
	objectName += ":";
    }

    // Get the class name:

    objectName.append(getClassName());

    if (isInstanceName())
    {
        objectName.append('.');

        // Append each key-value pair:

        const Array<KeyBinding>& keyBindings = getKeyBindings();

        for (Uint32 i = 0, n = keyBindings.size(); i < n; i++)
        {
	    objectName.append(keyBindings[i].getName());
	    objectName.append('=');

	    const String& value = _escapeSpecialCharacters(
	        keyBindings[i].getValue());

	    KeyBinding::Type type = keyBindings[i].getType();
	
	    if (type == KeyBinding::REFERENCE)
	        objectName.append('R');

	    if (type == KeyBinding::STRING || type == KeyBinding::REFERENCE)
	        objectName.append('"');

	    objectName.append(value);

	    if (type == KeyBinding::STRING || type == KeyBinding::REFERENCE)
	        objectName.append('"');

	    if (i + 1 != n)
	        objectName.append(',');
        }
    }

    return objectName;
}

String CIMReference::toStringCanonical(Boolean includeHost) const
{
    CIMReference ref = *this;

    ref._className.toLower();

    for (Uint32 i = 0, n = ref._keyBindings.size(); i < n; i++)
	ref._keyBindings[i]._name.toLower();

    return ref.toString(includeHost);
}

Boolean CIMReference::identical(const CIMReference& x) const
{
    return
	String::equal(_host, x._host) &&
	CIMName::equal(_nameSpace, x._nameSpace) &&
	CIMName::equal(_className, x._className) &&
	_keyBindings == x._keyBindings;
}

void CIMReference::toXml(Array<Sint8>& out, Boolean putValueWrapper) const
{
    if (putValueWrapper)
	out << "<VALUE.REFERENCE>\n";

    // See if it is a class or instance reference (instance references have
    // key-bindings; class references do not).

    if (_keyBindings.size())
    {
	if (_host.size())
	{
	    XmlWriter::appendInstancePathElement(out, *this);
	}
	else if (_nameSpace.size())
	{
	    XmlWriter::appendLocalInstancePathElement(out, *this);
	}
	else
	    XmlWriter::appendInstanceNameElement(out, *this);
    }
    else
    {
	if (_host.size())
	{
	    XmlWriter::appendClassPathElement(out, *this);
	}
	else if (_nameSpace.size())
	{
	    XmlWriter::appendLocalClassPathElement(out, *this);
	}
	else
	    XmlWriter::appendClassNameElement(out, _className);
    }

    if (putValueWrapper)
	out << "</VALUE.REFERENCE>\n";
}

//ATTNKS: At this point, I simply created the function
void CIMReference::toMof(Array<Sint8>& out, Boolean putValueWrapper) const
{
    if (putValueWrapper)
	out << "<VALUE.REFERENCE Mof>\n";

    // See if it is a class or instance reference (instance references have
    // key-bindings; class references do not).

    if (_keyBindings.size())
    {
	if (_host.size())
	{
	    XmlWriter::appendInstancePathElement(out, *this);
	}
	else if (_nameSpace.size())
	{
	    XmlWriter::appendLocalInstancePathElement(out, *this);
	}
	else
	    XmlWriter::appendInstanceNameElement(out, *this);
    }
    else
    {
	if (_host.size())
	{
	    XmlWriter::appendClassPathElement(out, *this);
	}
	else if (_nameSpace.size())
	{
	    XmlWriter::appendLocalClassPathElement(out, *this);
	}
	else
	    XmlWriter::appendClassNameElement(out, _className);
    }

    if (putValueWrapper)
	out << "</VALUE.REFERENCE Mof>\n";
}

void CIMReference::print(PEGASUS_STD(ostream)& os) const
{
    Array<Sint8> tmp;
    toXml(tmp);
    XmlWriter::indentedPrint(os, tmp.getData());
}

const char* KeyBinding::typeToString(Type type)
{
    switch (type)
    {
	case KeyBinding::BOOLEAN:
	    return "boolean";

	case KeyBinding::STRING:
	    return "string";

	case KeyBinding::NUMERIC:
	    return "numeric";

        case KeyBinding::REFERENCE:
        default:
            PEGASUS_ASSERT(false);
    }

    return "unknown";
}

Uint32 CIMReference::makeHashCode() const
{
    CIMReference ref = *this;

    ref._className.toLower();

    for (Uint32 i = 0, n = ref._keyBindings.size(); i < n; i++)
	ref._keyBindings[i]._name.toLower();

    return HashFunc<String>::hash(ref.toString());
}

KeyBindingArray CIMReference::getKeyBindingArray()
{
    return KeyBindingArray();
}

PEGASUS_NAMESPACE_END
