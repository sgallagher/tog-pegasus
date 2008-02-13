//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
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

#include <Pegasus/Common/Config.h>
#include <cstring>
#include <iostream>
#include "HashTable.h"
#include "CIMObjectPath.h"
#include "Indentor.h"
#include "CIMName.h"
#include "XmlWriter.h"
#include "XmlReader.h"
#include "ArrayInternal.h"
#include "HostLocator.h"

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
            case '\\':
                result.append("\\\\");
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

    //
    //  If the key is a reference, the keys in the reference must also be
    //  sorted
    //
    for (Uint32 k = 0; k < n ; k++)
        if (x[k].getType () == CIMKeyBinding::REFERENCE)
        {
            CIMObjectPath tmp (x[k].getValue ());
            Array <CIMKeyBinding> keyBindings = tmp.getKeyBindings ();
            _BubbleSort (keyBindings);
            tmp.setKeyBindings (keyBindings);
            x[k].setValue (tmp.toString ());
        }

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

CIMKeyBinding::CIMKeyBinding(
    const CIMName& name,
    const String& value,
    Type type)
{
    _rep = new CIMKeyBindingRep(name, value, type);
}

CIMKeyBinding::CIMKeyBinding(const CIMName& name, const CIMValue& value)
{
    if (value.isArray())
    {
        throw TypeMismatchException();
    }

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
//  case CIMTYPE_REAL32:
//  case CIMTYPE_REAL64:
    case CIMTYPE_OBJECT:
#ifdef PEGASUS_EMBEDDED_INSTANCE_SUPPORT
    case CIMTYPE_INSTANCE:
#endif // PEGASUS_EMBEDDED_INSTANCE_SUPPORT
        // From PEP 194: EmbeddedObjects cannot be keys.
        throw TypeMismatchException();
        break;
    default:
        kbType = NUMERIC;
        break;
    }

    _rep = new CIMKeyBindingRep(name, kbValue, kbType);
}

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

Boolean CIMKeyBinding::equal(CIMValue value)
{
    if (value.isArray())
    {
        return false;
    }

    CIMValue kbValue;

    try
    {
        switch (value.getType())
        {
        case CIMTYPE_CHAR16:
            if (getType() != STRING) return false;
            kbValue.set(getValue()[0]);
            break;
        case CIMTYPE_DATETIME:
            if (getType() != STRING) return false;
            kbValue.set(CIMDateTime(getValue()));
            break;
        case CIMTYPE_STRING:
            if (getType() != STRING) return false;
            kbValue.set(getValue());
            break;
        case CIMTYPE_REFERENCE:
            if (getType() != REFERENCE) return false;
            kbValue.set(CIMObjectPath(getValue()));
            break;
        case CIMTYPE_BOOLEAN:
            if (getType() != BOOLEAN) return false;
            kbValue = XmlReader::stringToValue(0, getValue().getCString(),
                                               value.getType());
            break;
//      case CIMTYPE_REAL32:
//      case CIMTYPE_REAL64:
        case CIMTYPE_OBJECT:
#ifdef PEGASUS_EMBEDDED_INSTANCE_SUPPORT
        case CIMTYPE_INSTANCE:
#endif // PEGASUS_EMBEDDED_INSTANCE_SUPPORT
            // From PEP 194: EmbeddedObjects cannot be keys.
            return false;
            break;
        default:  // Numerics
            if (getType() != NUMERIC) return false;
            kbValue = XmlReader::stringToValue(0, getValue().getCString(),
                                               value.getType());
            break;
        }
    }
    catch (Exception&)
    {
        return false;
    }

    return value.equal(kbValue);
}

Boolean operator==(const CIMKeyBinding& x, const CIMKeyBinding& y)
{
    // Check that the names and types match
    if (!(x.getName().equal(y.getName())) ||
        !(x.getType() == y.getType()))
    {
        return false;
    }

    switch (x.getType())
    {
    case CIMKeyBinding::REFERENCE:
        try
        {
            // References should be compared as CIMObjectPaths
            return (CIMObjectPath(x.getValue()) == CIMObjectPath(y.getValue()));
        }
        catch (Exception&)
        {
            // If CIMObjectPath parsing fails, just compare strings
            return String::equal(x.getValue(), y.getValue());
        }
        break;
    case CIMKeyBinding::BOOLEAN:
        // Case-insensitive comparison is sufficient for booleans
        return String::equalNoCase(x.getValue(), y.getValue());
        break;
    case CIMKeyBinding::NUMERIC:
        // Note: This comparison assumes XML syntax for integers
        // First try comparing as unsigned integers
        {
            Uint64 xValue;
            Uint64 yValue;
            if (XmlReader::stringToUnsignedInteger(
                    x.getValue().getCString(), xValue) &&
                XmlReader::stringToUnsignedInteger(
                    y.getValue().getCString(), yValue))
            {
                return (xValue == yValue);
            }
        }
        // Next try comparing as signed integers
        {
            Sint64 xValue;
            Sint64 yValue;
            if (XmlReader::stringToSignedInteger(
                    x.getValue().getCString(), xValue) &&
                XmlReader::stringToSignedInteger(
                    y.getValue().getCString(), yValue))
            {
                return (xValue == yValue);
            }
        }
        // Note: Keys may not be real values, so don't try comparing as reals
        // We couldn't parse the numbers, so just compare the strings
        return String::equal(x.getValue(), y.getValue());
        break;
    default:  // CIMKeyBinding::STRING
        return String::equal(x.getValue(), y.getValue());
        break;
    }

    PEGASUS_UNREACHABLE(return false;)
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

    static Boolean isValidHostname(const String& hostname)
    {
        HostLocator addr(hostname);

        return addr.isValid();
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

Boolean _parseHostElement(
    const String& objectName,
    char*& p,
    String& host)
{
    // See if there is a host name (true if it begins with "//"):
    // Host is of the form <hostname>:<port> and begins with "//"
    // and ends with "/":

    if (p[0] != '/' || p[1] != '/')
    {
        return false;
    }

    p += 2;

    char* slash = strchr(p, '/');
    if (!slash)
    {
        throw MalformedObjectNameException(objectName);
    }

    String hostname = String(p, (Uint32)(slash - p));
    if (!CIMObjectPathRep::isValidHostname(hostname))
    {
        throw MalformedObjectNameException(objectName);
    }
    host = hostname;

    // Do not step past the '/'; it will be consumed by the namespace parser
    p = slash;

    return true;
}

Boolean _parseNamespaceElement(
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

    // A ':' as part of a keybinding value should not be interpreted as
    // a namespace delimiter.  Since keybinding pairs follow the first '.'
    // in the object path string, the ':' delimiter only counts if it
    // appears before the '.'.

    char* dot = strchr(p, '.');
    if (dot && (dot < colon))
    {
        return false;
    }

    //----------------------------------------------------------------------
    // Validate the namespace path.  Namespaces must match the following
    // regular expression: "[A-Za-z_]+(/[A-Za-z_]+)*"
    //----------------------------------------------------------------------

    String namespaceName = String(p, (Uint32)(colon - p));
    if (!CIMNamespaceName::legal(namespaceName))
    {
        throw MalformedObjectNameException(objectName);
    }
    nameSpace = namespaceName;

    p = colon+1;
    return true;
}

/**
    ATTN-RK: The DMTF specification for the string form of an
    object path makes it impossible for a parser to distinguish
    between a key values of String type and Reference type.

    Given the ambiguity, this implementation takes a guess at the
    type of a quoted key value.  If the value can be parsed into
    a CIMObjectPath with at least one key binding, the type is
    set to REFERENCE.  Otherwise, the type is set to STRING.
    Note: This algorithm appears to be in line with what the Sun
    WBEM Services implementation does.

    To be totally correct, it would be necessary to retrieve the
    class definition and look up the types of the key properties
    to determine how to interpret the key values.  This is clearly
    too inefficient for internal transformations between
    CIMObjectPaths and String values.
*/
void _parseKeyBindingPairs(
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

        if (*p == '"')
        {
            // Could be CIMKeyBinding::STRING or CIMKeyBinding::REFERENCE

            p++;

            Buffer keyValueUTF8(128);

            while (*p && *p != '"')
            {
                if (*p == '\\')
                {
                    p++;

                    if ((*p != '\\') && (*p != '"'))
                    {
                        throw MalformedObjectNameException(objectName);
                    }
                }

                keyValueUTF8.append(*p++);
            }

            if (*p++ != '"')
                throw MalformedObjectNameException(objectName);

            // Convert the UTF-8 value to a UTF-16 String

            valueString.assign(
                (const char*)keyValueUTF8.getData(),
                keyValueUTF8.size());

            /*
                Guess at the type of this quoted key value.  If the value
                can be parsed into a CIMObjectPath with at least one key
                binding, the type is assumed to be a REFERENCE.  Otherwise,
                the type is set to STRING.  (See method header for details.)
             */
            type = CIMKeyBinding::STRING;

            /* Performance shortcut will check for
               equal sign instead of doing the full
               CIMObjectPath creation and exception handling
            */
            if (strchr(keyValueUTF8.getData(), '='))
            {
                // found an equal sign, high probability for a reference
                try
                {
                    CIMObjectPath testForPath(valueString);
                    if (testForPath.getKeyBindings().size() > 0)
                    {
                        // We've found a reference value!
                        type = CIMKeyBinding::REFERENCE;
                    }
                }
                catch (const Exception &)
                {
                    // Not a reference value; leave type as STRING
                }
            }
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

            if (*p == '-')
            {
                Sint64 x;
                if (!XmlReader::stringToSignedInteger(p, x))
                    throw MalformedObjectNameException(objectName);
            }
            else
            {
                Uint64 x;
                if (!XmlReader::stringToUnsignedInteger(p, x))
                    throw MalformedObjectNameException(objectName);
            }

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

    CString pCString = objectName.getCString();
    char* p = const_cast<char*>((const char*) pCString);
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

    String className = String(p, (Uint32)(dot - p));
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
    if ((host != String::EMPTY) && !CIMObjectPathRep::isValidHostname(host))
    {
        throw MalformedObjectNameException(host);
    }

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

            if (type == CIMKeyBinding::STRING ||
                type == CIMKeyBinding::REFERENCE)
                objectName.append('"');

            objectName.append(value);

            if (type == CIMKeyBinding::STRING ||
                type == CIMKeyBinding::REFERENCE)
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

    // Normalize hostname by changing to lower case
    ref._rep->_host.toLower(); // ICU_TODO:

    // Normalize namespace by changing to lower case
    if (!ref._rep->_nameSpace.isNull())
    {
        String nameSpaceLower = ref._rep->_nameSpace.getString();
        nameSpaceLower.toLower(); // ICU_TODO:
        ref._rep->_nameSpace = nameSpaceLower;
    }

    // Normalize class name by changing to lower case
    if (!ref._rep->_className.isNull())
    {
        String classNameLower = ref._rep->_className.getString();
        classNameLower.toLower(); // ICU_TODO:
        ref._rep->_className = classNameLower;
    }

    for (Uint32 i = 0, n = ref._rep->_keyBindings.size(); i < n; i++)
    {
        // Normalize key binding name by changing to lower case
        if (!ref._rep->_keyBindings[i]._rep->_name.isNull())
        {
            String keyBindingNameLower =
                ref._rep->_keyBindings[i]._rep->_name.getString();
            keyBindingNameLower.toLower(); // ICU_TODO:
            ref._rep->_keyBindings[i]._rep->_name = keyBindingNameLower;
        }

        // Normalize the key value
        switch (ref._rep->_keyBindings[i]._rep->_type)
        {
        case CIMKeyBinding::REFERENCE:
            try
            {
                // Convert reference to CIMObjectPath and recurse
                ref._rep->_keyBindings[i]._rep->_value =
                    CIMObjectPath(ref._rep->_keyBindings[i]._rep->_value).
                        _toStringCanonical();
            }
            catch (Exception&)
            {
                // Leave value unchanged if the CIMObjectPath parsing fails
            }
            break;
        case CIMKeyBinding::BOOLEAN:
            // Normalize the boolean string by changing to lower case
            ref._rep->_keyBindings[i]._rep->_value.toLower(); // ICU_TODO:
            break;
        case CIMKeyBinding::NUMERIC:
            // Normalize the numeric string by converting to integer and back
            Uint64 uValue;
            Sint64 sValue;
            // First try converting to unsigned integer
            if (XmlReader::stringToUnsignedInteger(
                    ref._rep->_keyBindings[i]._rep->_value.getCString(),
                        uValue))
            {
                char buffer[32];  // Should need 21 chars max
                sprintf(buffer, "%" PEGASUS_64BIT_CONVERSION_WIDTH "u", uValue);
                ref._rep->_keyBindings[i]._rep->_value = String(buffer);
            }
            // Next try converting to signed integer
            else if (XmlReader::stringToSignedInteger(
                         ref._rep->_keyBindings[i]._rep->_value.getCString(),
                             sValue))
            {
                char buffer[32];  // Should need 21 chars max
                sprintf(buffer, "%" PEGASUS_64BIT_CONVERSION_WIDTH "d", sValue);
                ref._rep->_keyBindings[i]._rep->_value = String(buffer);
            }
            // Leave value unchanged if it cannot be converted to an integer
            break;
        default:  // CIMKeyBinding::STRING
            // No normalization required for STRING
            break;
        }
    }

    // Note: key bindings are sorted when set in the CIMObjectPath

    return ref.toString();
}

Boolean CIMObjectPath::identical(const CIMObjectPath& x) const
{
    return
        String::equalNoCase(_rep->_host, x._rep->_host) &&
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

PEGASUS_NAMESPACE_END
