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
// Modified By: Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                (carolann_graves@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <cctype>
#include <cstring>
#include "HashTable.h"
#include "CIMObjectPath.h"
#include "Indentor.h"
#include "CIMName.h"
#include "Destroyer.h"
#include "XmlWriter.h"
#include "XmlReader.h"
#include "ArrayInternal.h"
#include "CIMOMPort.h"

PEGASUS_NAMESPACE_BEGIN

#define PEGASUS_ARRAY_T CIMKeyBinding
# include "ArrayImpl.h"
#undef PEGASUS_ARRAY_T

#define PEGASUS_ARRAY_T CIMObjectPath
# include "ArrayImpl.h"
#undef PEGASUS_ARRAY_T

// ATTN: KS May 2002 P0 Add resolve method to CIMObjectPath.
// Add a resolve method to this class to verify that the
// reference is correct (that the class name corresponds to a real
// class and that the property names are really keys and that all keys
// of the class or used. Also be sure that there is a valid conversion
// between the string value and the value of that property).
//
// ATTN: also check to see that the reference refers to a class that is the
// same or derived from the _className member.

////////////////////////////////////////////////////////////////////////////////
//
// Local routines:
//
////////////////////////////////////////////////////////////////////////////////

static String _escapeSpecialCharacters(const String& str)
{
    String result;

    for (Uint32 i = 0, n = str.size(); i < n; i++)
    {
        switch (str[i])
        {
            case '\n':
                result.append("\\n");
                break;

            case '\r':
                result.append("\\r");
                break;

            case '\t':
                result.append("\\t");
                break;

            case '"':
                result.append("\\\"");
                break;

            default:
                result.append(str[i]);
        }
    }

    return result;
}

static void _BubbleSort(Array<CIMKeyBinding>& x)
{
    Uint32 n = x.size();

    if (n < 2)
        return;

    for (Uint32 i = 0; i < n - 1; i++)
    {
        for (Uint32 j = 0; j < n - 1; j++)
        {
            if (String::compareNoCase(x[j].getName().getString(), 
                                      x[j+1].getName().getString()) > 0)
            {
                CIMKeyBinding t = x[j];
                x[j] = x[j+1];
                x[j+1] = t;
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
//
// CIMKeyBinding
//
////////////////////////////////////////////////////////////////////////////////

class CIMKeyBindingRep
{
public:
    CIMKeyBindingRep()
    {
    }

    CIMKeyBindingRep(const CIMKeyBindingRep& x)
        : _name(x._name), _value(x._value), _type(x._type)
    {
    }

    CIMKeyBindingRep(
        const CIMName& name,
        const String& value,
        CIMKeyBinding::Type type)
        : _name(name), _value(value), _type(type)
    {
    }

    ~CIMKeyBindingRep()
    {
    }

    CIMKeyBindingRep& operator=(const CIMKeyBindingRep& x)
    {
        if (&x != this)
        {
            _name = x._name;
            _value = x._value;
            _type = x._type;
        }
        return *this;
    }

    CIMName _name;
    String _value;
    CIMKeyBinding::Type _type;
};


CIMKeyBinding::CIMKeyBinding()
{
    _rep = new CIMKeyBindingRep();
}

CIMKeyBinding::CIMKeyBinding(const CIMKeyBinding& x)
{
    _rep = new CIMKeyBindingRep(*x._rep);
}

CIMKeyBinding::CIMKeyBinding(const CIMName& name, const String& value, Type type)
{
    _rep = new CIMKeyBindingRep(name, value, type);
}

#ifdef PEGASUS_FUTURE
CIMKeyBinding::CIMKeyBinding(const CIMName& name, const CIMValue& value)
{
    String kbValue = value.toString();
    Type kbType;

    switch (value.getType())
    {
    case CIMTYPE_BOOLEAN:
        kbType = BOOLEAN;
        break;
    case CIMTYPE_CHAR16:
    case CIMTYPE_STRING:
    case CIMTYPE_DATETIME:
        kbType = STRING;
        break;
    case CIMTYPE_REFERENCE:
        kbType = REFERENCE;
        break;
    default:
        kbType = NUMERIC;
        break;
    }

    _rep = new CIMKeyBindingRep(name, kbValue, kbType);
}
#endif

CIMKeyBinding::~CIMKeyBinding()
{
    delete _rep;
}

CIMKeyBinding& CIMKeyBinding::operator=(const CIMKeyBinding& x)
{
    *_rep = *x._rep;
    return *this;
}

const CIMName& CIMKeyBinding::getName() const
{
    return _rep->_name;
}

void CIMKeyBinding::setName(const CIMName& name)
{
    _rep->_name = name;
}

const String& CIMKeyBinding::getValue() const
{
    return _rep->_value;
}

void CIMKeyBinding::setValue(const String& value)
{
    _rep->_value = value;
}

CIMKeyBinding::Type CIMKeyBinding::getType() const
{
    return _rep->_type;
}

void CIMKeyBinding::setType(CIMKeyBinding::Type type)
{
    _rep->_type = type;
}

#ifdef PEGASUS_FUTURE
Boolean CIMKeyBinding::equal(CIMValue value)
{
    CIMValue kbValue;

    try
    {
        switch (value.getType())
        {
        case CIMTYPE_CHAR16:
            kbValue.set(getValue()[0]);
            break;
        case CIMTYPE_DATETIME:
            kbValue.set(CIMDateTime(getValue()));
            break;
        case CIMTYPE_STRING:
            kbValue.set(getValue());
            break;
        case CIMTYPE_REFERENCE:
            kbValue.set(CIMObjectPath(getValue()));
            break;
        default:  // Boolean and numerics
            kbValue = XmlReader::stringToValue(0, getValue().getCString(),
                                               value.getType());
            break;
        }
    }
    catch (...)  // Just catch Exception?
    {
        return false;
    }

    return value.equal(kbValue);
}
#endif

Boolean operator==(const CIMKeyBinding& x, const CIMKeyBinding& y)
{
    return
        x.getName().equal(y.getName()) &&
        String::equal(x.getValue(), y.getValue()) &&
        x.getType() == y.getType();
}


////////////////////////////////////////////////////////////////////////////////
//
// CIMObjectPath
//
////////////////////////////////////////////////////////////////////////////////

class CIMObjectPathRep
{
public:
    CIMObjectPathRep()
    {
    }

    CIMObjectPathRep(const CIMObjectPathRep& x)
        : _host(x._host), _nameSpace(x._nameSpace),
        _className(x._className), _keyBindings(x._keyBindings)
    {
    }

    CIMObjectPathRep(
        const String& host,
        const CIMNamespaceName& nameSpace,
        const CIMName& className,
        const Array<CIMKeyBinding>& keyBindings)
        : _host(host), _nameSpace(nameSpace),
        _className(className), _keyBindings(keyBindings)
    {
    }

    ~CIMObjectPathRep()
    {
    }

    CIMObjectPathRep& operator=(const CIMObjectPathRep& x)
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

    //
    // Contains port as well (e.g., myhost:1234).
    //
    String _host;

    CIMNamespaceName _nameSpace;
    CIMName _className;
    Array<CIMKeyBinding> _keyBindings;
};


CIMObjectPath::CIMObjectPath()
{
    _rep = new CIMObjectPathRep();
}

CIMObjectPath::CIMObjectPath(const CIMObjectPath& x)
{
    _rep = new CIMObjectPathRep(*x._rep);
}

CIMObjectPath::CIMObjectPath(const String& objectName)
{
    // Test the objectName out to see if we get an exception
    CIMObjectPath tmpRef;
    tmpRef.set(objectName);

    _rep = new CIMObjectPathRep(*tmpRef._rep);
}

CIMObjectPath::CIMObjectPath(
    const String& host,
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    const Array<CIMKeyBinding>& keyBindings)
{
    // Test the objectName out to see if we get an exception
    CIMObjectPath tmpRef;
    tmpRef.set(host, nameSpace, className, keyBindings);

    _rep = new CIMObjectPathRep(*tmpRef._rep);
}

CIMObjectPath::~CIMObjectPath()
{
    delete _rep;
}

CIMObjectPath& CIMObjectPath::operator=(const CIMObjectPath& x)
{
    *_rep = *x._rep;
    return *this;
}

void CIMObjectPath::clear()
{
    _rep->_host.clear();
    _rep->_nameSpace.clear();
    _rep->_className.clear();
    _rep->_keyBindings.clear();
}

void CIMObjectPath::set(
    const String& host,
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    const Array<CIMKeyBinding>& keyBindings)
{
   setHost(host);
   setNameSpace(nameSpace);
   setClassName(className);
   setKeyBindings(keyBindings);
}

Boolean CIMObjectPath::_parseHostElement(
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
        throw MalformedObjectNameException(objectName);

    q++;

    while (isalnum(*q) || *q == '-')
        q++;

    // We now expect a port (or default the port).

    if (*q == ':')
    {
        q++;
        // Check for a port number:

        if (!isdigit(*q))
            throw MalformedObjectNameException(objectName);
        
        while (isdigit(*q))
            q++;

        // Finally, assign the host name:

        host.assign(p, q - p);
    }
    else
    {
        host.assign(p, q - p);

        // Assign the default port number:

        host.append(":");
        host.append(PEGASUS_CIMOM_DEFAULT_PORT_STRING);
    }

    // Check for slash terminating the entire sequence:

    if (*q != '/')
    {
        host.clear();
        throw MalformedObjectNameException(objectName);
    }

    p = ++q;

    return true;
}

Boolean CIMObjectPath::_parseNamespaceElement(
    const String& objectName,
    char*& p,
    CIMNamespaceName& nameSpace)
{
    // If we don't find a valid namespace name followed by a ':', we
    // assume we're not looking at a namespace name.

    char* colon = strchr(p, ':');
    if (!colon)
    {
        return false;
    }

    //----------------------------------------------------------------------
    // Validate the namespace path.  Namespaces must match the following
    // regular expression: "[A-Za-z_]+(/[A-Za-z_]+)*"
    //----------------------------------------------------------------------

    String namespaceName = String(p, colon - p);
    if (!CIMNamespaceName::legal(namespaceName))
    {
        throw MalformedObjectNameException(objectName);
    }
    nameSpace = namespaceName;

    p = colon+1;
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
    required that a CIMObjectPath object be able to distinguish
    between a key of String type and a key of reference type.

    I've modified the String format of CIMObjectPaths slightly to
    allow efficient processing of references whose keys are also
    of reference type.  The "official" form uses the same
    encoding for key values of String type and of reference type,
    and so it would be necessary to retrieve the class definition
    and look up the types of the key properties to determine how
    to treat the key values.  This is clearly too inefficient for
    internal transformations between CIMObjectPaths and String
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
    new CIMKeyBinding type (REFERENCE) has been defined to denote
    keys in a CIMObjectPath that are of reference type.  This
    CIMKeyBinding type must be used appropriately for
    CIMObjectPath::toString() to behave correctly.

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
void CIMObjectPath::_parseKeyBindingPairs(
    const String& objectName,
    char*& p,
    Array<CIMKeyBinding>& keyBindings)  
{
    // Get the key-value pairs:

    while (*p)
    {
        // Get key part:

        char* equalsign = strchr(p, '=');
        if (!equalsign)
        {
            throw MalformedObjectNameException(objectName);
        }

        *equalsign = 0;

        if (!CIMName::legal(p))
            throw MalformedObjectNameException(objectName);

        CIMName keyName (p);

        // Get the value part:

        String valueString;
        p = equalsign + 1;
        CIMKeyBinding::Type type;

        if (*p == 'R')
        {
            p++;

            type = CIMKeyBinding::REFERENCE;

            if (*p++ != '"')
                throw MalformedObjectNameException(objectName);

            while (*p && *p != '"')
            {
                // ATTN: need to handle special characters here:

                if (*p == '\\')
                    *p++;

                valueString.append(*p++);
            }

            if (*p++ != '"')
                throw MalformedObjectNameException(objectName);
        }
        else if (*p == '"')
        {
            p++;

            type = CIMKeyBinding::STRING;

            while (*p && *p != '"')
            {
                // ATTN: need to handle special characters here:

                if (*p == '\\')
                    *p++;

                valueString.append(*p++);
            }

            if (*p++ != '"')
                throw MalformedObjectNameException(objectName);
        }
        else if (toupper(*p) == 'T' || toupper(*p) == 'F')
        {
            type = CIMKeyBinding::BOOLEAN;

            char* r = p;
            Uint32 n = 0;

            while (*r && *r != ',')
            {
                *r = toupper(*r);
                r++;
                n++;
            }

            if (!(((strncmp(p, "TRUE", n) == 0) && n == 4) ||
                  ((strncmp(p, "FALSE", n) == 0) && n == 5)))
                throw MalformedObjectNameException(objectName);

            valueString.assign(p, n);

            p = p + n;
        }
        else
        {
            type = CIMKeyBinding::NUMERIC;

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
                throw MalformedObjectNameException(objectName);

            valueString.assign(p, n);

            if (isComma)
            {
                *r = ',';
            }

            p = p + n;
        }

        keyBindings.append(CIMKeyBinding(keyName.getString (), valueString, 
            type));

        if (*p)
        {
            if (*p++ != ',')
            {
                throw MalformedObjectNameException(objectName);
            }
        }
    }

    _BubbleSort(keyBindings);
}

void CIMObjectPath::set(const String& objectName)  
{
    clear();

    //--------------------------------------------------------------------------
    // We will extract components from an object name. Here is an sample
    // object name:
    //
    //     //atp:9999/root/cimv25:TennisPlayer.first="Patrick",last="Rafter"
    //--------------------------------------------------------------------------

    // Convert to a C String first:

    char* p = strdup(objectName.getCString());
    Destroyer<char> destroyer(p);
    Boolean gotHost;
    Boolean gotNamespace;

    gotHost = _parseHostElement(objectName, p, _rep->_host);
    gotNamespace = _parseNamespaceElement(objectName, p, _rep->_nameSpace);

    if (gotHost && !gotNamespace)
    {
        throw MalformedObjectNameException(objectName);
    }

    // Extract the class name:

    char* dot = strchr(p, '.');

    if (!dot)
    {
        if (!CIMName::legal(p))
        {
            throw MalformedObjectNameException(objectName);
        }

        // ATTN: remove this later: a reference should only be able to hold
        // an instance name.

        _rep->_className = CIMName (p);
        return;
    }

    String className = String(p, dot - p);
    if (!CIMName::legal(className))
    {
        throw MalformedObjectNameException(objectName);
    }
    _rep->_className = className;

    // Advance past dot:

    p = dot + 1;

    _parseKeyBindingPairs(objectName, p, _rep->_keyBindings);
}

CIMObjectPath& CIMObjectPath::operator=(const String& objectName)
{
    set(objectName);
    return *this;
}

const String& CIMObjectPath::getHost() const
{
    return _rep->_host;
}

void CIMObjectPath::setHost(const String& host)
{
    _rep->_host = host;
}

const CIMNamespaceName& CIMObjectPath::getNameSpace() const
{
    return _rep->_nameSpace;
}

void CIMObjectPath::setNameSpace(const CIMNamespaceName& nameSpace)
{
   _rep->_nameSpace = nameSpace;
}

const CIMName& CIMObjectPath::getClassName() const
{
    return _rep->_className;
}

void CIMObjectPath::setClassName(const CIMName& className)
{
    _rep->_className = className;
}

const Array<CIMKeyBinding>& CIMObjectPath::getKeyBindings() const
{
    return _rep->_keyBindings;
}

void CIMObjectPath::setKeyBindings(const Array<CIMKeyBinding>& keyBindings)
{
    _rep->_keyBindings = keyBindings;
    _BubbleSort(_rep->_keyBindings);
}

String CIMObjectPath::toString() const
{
    String objectName;

    // Get the host:

    if (_rep->_host.size())
    {
        objectName = "//";
        objectName.append(_rep->_host);
        objectName.append("/");
    }

    // Get the namespace (if we have a host name, we must write namespace):

    if (!_rep->_nameSpace.isNull() || _rep->_host.size())
    {
        objectName.append(_rep->_nameSpace.getString ());
        objectName.append(":");
    }

    // Get the class name:

    objectName.append(getClassName().getString ());

    //
    //  ATTN-CAKG-P2-20020726:  The following condition does not correctly
    //  distinguish instanceNames from classNames in every case
    //  The instanceName of a singleton instance of a keyless class has no
    //  key bindings
    //
    if (_rep->_keyBindings.size () != 0)
    {
        objectName.append('.');

        // Append each key-value pair:

        const Array<CIMKeyBinding>& keyBindings = getKeyBindings();

        for (Uint32 i = 0, n = keyBindings.size(); i < n; i++)
        {
            objectName.append(keyBindings[i].getName().getString ());
            objectName.append('=');

            const String& value = _escapeSpecialCharacters(
                keyBindings[i].getValue());

            CIMKeyBinding::Type type = keyBindings[i].getType();
        
            if (type == CIMKeyBinding::REFERENCE)
                objectName.append('R');

            if (type == CIMKeyBinding::STRING || type == CIMKeyBinding::REFERENCE)
                objectName.append('"');

            objectName.append(value);

            if (type == CIMKeyBinding::STRING || type == CIMKeyBinding::REFERENCE)
                objectName.append('"');

            if (i + 1 != n)
                objectName.append(',');
        }
    }

    return objectName;
}

String CIMObjectPath::_toStringCanonical() const
{
    CIMObjectPath ref = *this;

    // ATTN-RK-P2-20020510: Need to make hostname and namespace lower case?

    String classNameLower = ref._rep->_className.getString ();
    classNameLower.toLower();
    ref._rep->_className = classNameLower;

    for (Uint32 i = 0, n = ref._rep->_keyBindings.size(); i < n; i++)
    {
        String keyBindingNameLower = 
            ref._rep->_keyBindings[i]._rep->_name.getString ();
        keyBindingNameLower.toLower();
        ref._rep->_keyBindings[i]._rep->_name = keyBindingNameLower;
    }

    // ATTN-RK-20020826: Need to sort keys?

    return ref.toString();
}

Boolean CIMObjectPath::identical(const CIMObjectPath& x) const
{
    return
        String::equal(_rep->_host, x._rep->_host) &&
        _rep->_nameSpace.equal(x._rep->_nameSpace) &&
        _rep->_className.equal(x._rep->_className) &&
        _rep->_keyBindings == x._rep->_keyBindings;
}

Uint32 CIMObjectPath::makeHashCode() const
{
    return HashFunc<String>::hash(_toStringCanonical());
}

Boolean operator==(const CIMObjectPath& x, const CIMObjectPath& y)
{
    return x.identical(y);
}

Boolean operator!=(const CIMObjectPath& x, const CIMObjectPath& y)
{
    return !operator==(x, y);
}

PEGASUS_STD(ostream)& operator<<(
    PEGASUS_STD(ostream)& os,
    const CIMObjectPath& x)
{
    return os << x.toString();
}

PEGASUS_NAMESPACE_END
