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
// Modified By: Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
//              Nag Boranna, Hewlett-Packard Company (nagaraja_boranna@hp.com)
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                  (carolann_graves@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <cstdlib>
#include <cstdio>
#include "Constants.h"
#include "Destroyer.h"
#include "CIMClass.h"
#include "CIMClassRep.h"
#include "CIMInstance.h"
#include "CIMInstanceRep.h"
#include "CIMProperty.h"
#include "CIMPropertyRep.h"
#include "CIMMethod.h"
#include "CIMMethodRep.h"
#include "CIMParameter.h"
#include "CIMParameterRep.h"
#include "CIMParamValue.h"
#include "CIMParamValueRep.h"
#include "CIMQualifier.h"
#include "CIMQualifierRep.h"
#include "CIMQualifierDecl.h"
#include "CIMQualifierDeclRep.h"
#include "CIMValue.h"
#include "XmlWriter.h"
#include "XmlParser.h"
#include "Tracer.h"
#include <Pegasus/Common/StatisticalData.h>

PEGASUS_NAMESPACE_BEGIN

Array<Sint8>& operator<<(Array<Sint8>& out, const char* x)
{
    XmlWriter::append(out, x);
    return out;
}

Array<Sint8>& operator<<(Array<Sint8>& out, char x)
{
    XmlWriter::append(out, x);
    return out;
}

Array<Sint8>& operator<<(Array<Sint8>& out, const Char16& x)
{
    XmlWriter::append(out, x);
    return out;
}

Array<Sint8>& operator<<(Array<Sint8>& out, const String& x)
{
    XmlWriter::append(out, x);
    return out;
}

Array<Sint8>& operator<<(Array<Sint8>& out, const Indentor& x)
{
    XmlWriter::append(out, x);
    return out;
}

Array<Sint8>& operator<<(Array<Sint8>& out, const Array<Sint8>& x)
{
    out.appendArray(x);
    return out;
}

Array<Sint8>& operator<<(Array<Sint8>& out, Uint32 x)
{
    XmlWriter::append(out, x);
    return out;
}

PEGASUS_STD(ostream)& operator<<(PEGASUS_STD(ostream)& os, const CIMDateTime& x)
{
    return os << x.toString();
}

inline void _appendChar(Array<Sint8>& out, const Char16& c)
{
    out.append(Sint8(c));
}

inline void _appendSpecialChar(Array<Sint8>& out, const Char16& c)
{
    // ATTN-B: Only UTF-8 handled for now.

    switch (c)
    {
	case '&':
	    out.append("&amp;", 5);
	    break;

	case '<':
	    out.append("&lt;", 4);
	    break;

	case '>':
	    out.append("&gt;", 4);
	    break;

	case '"':
	    out.append("&quot;", 6);
	    break;

	case '\'':
	    out.append("&apos;", 6);
	    break;

	default:
	    out.append(Sint8(c));
    }
}

static inline void _appendSpecialChar(PEGASUS_STD(ostream)& os, char c)
{
    switch (c)
    {
	case '&':
	    os << "&amp;";
	    break;

	case '<':
	    os << "&lt;";
	    break;

	case '>':
	    os << "&gt;";
	    break;

	case '"':
	    os << "&quot;";
	    break;

	case '\'':
	    os << "&apos;";
	    break;

	default:
	    os << c;
    }
}

static inline void _appendSpecial(PEGASUS_STD(ostream)& os, const char* str)
{
    while (*str)
	_appendSpecialChar(os, *str++);
}

void XmlWriter::append(Array<Sint8>& out, const Char16& x)
{
    _appendChar(out, x);
}

void XmlWriter::append(Array<Sint8>& out, Boolean x)
{
    append(out, (x ? "TRUE" : "FALSE"));
}

void XmlWriter::append(Array<Sint8>& out, Uint32 x)
{
    char buffer[32];
    sprintf(buffer, "%u", x);
    append(out, buffer);
}

void XmlWriter::append(Array<Sint8>& out, Sint32 x)
{
    char buffer[32];
    sprintf(buffer, "%d", x);
    append(out, buffer);
}

void XmlWriter::append(Array<Sint8>& out, Uint64 x)
{
    char buffer[32];  // Should need 21 chars max
    // I know I shouldn't put platform flags here, but the other way is too hard
#if defined(PEGASUS_PLATFORM_WIN32_IX86_MSVC)
    sprintf(buffer, "%I64u", x);
#else
    sprintf(buffer, "%llu", x);
#endif
    append(out, buffer);
}

void XmlWriter::append(Array<Sint8>& out, Sint64 x)
{
    char buffer[32];  // Should need 21 chars max
    // I know I shouldn't put platform flags here, but the other way is too hard
#if defined(PEGASUS_PLATFORM_WIN32_IX86_MSVC)
    sprintf(buffer, "%I64d", x);
#else
    sprintf(buffer, "%lld", x);
#endif
    append(out, buffer);
}

void XmlWriter::append(Array<Sint8>& out, Real64 x)
{
    char buffer[128];
    // %e gives '[-]m.dddddde+/-xx', which seems compatible with CIM/XML spec
    sprintf(buffer, "%e", x);
    append(out, buffer);
}

void XmlWriter::append(Array<Sint8>& out, const char* str)
{
    while (*str)
	_appendChar(out, *str++);
}

void XmlWriter::append(Array<Sint8>& out, const String& str)
{
    for (Uint32 i = 0; i < str.size(); i++)
    {
        _appendChar(out, str[i]);
    }
}

void XmlWriter::append(Array<Sint8>& out, const Indentor& x)
{
    for (Uint32 i = 0; i < 4 * x.getLevel(); i++)
	out.append(' ');
}

void XmlWriter::appendSpecial(Array<Sint8>& out, const Char16& x)
{
    _appendSpecialChar(out, x);
}

void XmlWriter::appendSpecial(Array<Sint8>& out, char x)
{
    _appendSpecialChar(out, Char16(x));
}

void XmlWriter::appendSpecial(Array<Sint8>& out, const char* str)
{
    while (*str)
	_appendSpecialChar(out, *str++);
}

void XmlWriter::appendSpecial(Array<Sint8>& out, const String& str)
{
    for (Uint32 i = 0; i < str.size(); i++)
    {
        _appendSpecialChar(out, str[i]);
    }
}

//------------------------------------------------------------------------------
//
// appendLocalNameSpacePathElement()
//
//     <!ELEMENT LOCALNAMESPACEPATH (NAMESPACE+)>
//
//------------------------------------------------------------------------------

void XmlWriter::appendLocalNameSpacePathElement(
    Array<Sint8>& out,
    const String& nameSpace)
{
    out << "<LOCALNAMESPACEPATH>\n";

    char* nameSpaceCopy = strdup(nameSpace.getCString());
    for (const char* p = strtok(nameSpaceCopy, "/"); p; p = strtok(NULL, "/"))
    {
	out << "<NAMESPACE NAME=\"" << p << "\"/>\n";
    }
    delete nameSpaceCopy;

    out << "</LOCALNAMESPACEPATH>\n";
}

//------------------------------------------------------------------------------
//
// appendNameSpacePathElement()
//
//     <!ELEMENT NAMESPACEPATH (HOST,LOCALNAMESPACEPATH)>
//
//------------------------------------------------------------------------------

void XmlWriter::appendNameSpacePathElement(
    Array<Sint8>& out,
    const String& host,
    const String& nameSpace)
{
    out << "<NAMESPACEPATH>\n";
    out << "<HOST>" << host << "</HOST>\n";
    appendLocalNameSpacePathElement(out, nameSpace);
    out << "</NAMESPACEPATH>\n";
}

//------------------------------------------------------------------------------
//
// appendClassNameElement()
//
//     <!ELEMENT CLASSNAME EMPTY>
//     <!ATTLIST CLASSNAME
//              %CIMName;>
//
//------------------------------------------------------------------------------

void XmlWriter::appendClassNameElement(
    Array<Sint8>& out,
    const String& className)
{
    out << "<CLASSNAME NAME=\"" << className << "\"/>\n";
}

//------------------------------------------------------------------------------
//
// appendInstanceNameElement()
//
//    <!ELEMENT INSTANCENAME (KEYBINDING*|KEYVALUE?|VALUE.REFERENCE?)>
//    <!ATTLIST INSTANCENAME
//              %ClassName;>
//
//------------------------------------------------------------------------------

void XmlWriter::appendInstanceNameElement(
    Array<Sint8>& out,
    const CIMObjectPath& instanceName)
{
    out << "<INSTANCENAME CLASSNAME=\"" << instanceName.getClassName() << "\">\n";

    Array<KeyBinding> keyBindings = instanceName.getKeyBindings();
    for (Uint32 i = 0, n = keyBindings.size(); i < n; i++)
    {
        out << "<KEYBINDING NAME=\"" << keyBindings[i].getName() << "\">\n";

        if (keyBindings[i].getType() == KeyBinding::REFERENCE)
        {
            CIMObjectPath ref = keyBindings[i].getValue();
            appendValueReferenceElement(out, ref, true);
        }
        else {
            out << "<KEYVALUE VALUETYPE=\"";
            out << keyBindingTypeToString(keyBindings[i].getType());
            out << "\">";

            // fixed the special character problem - Markus

            appendSpecial(out, keyBindings[i].getValue());
            out << "</KEYVALUE>\n";
        }
        out << "</KEYBINDING>\n";
    }
    out << "</INSTANCENAME>\n";
}

//------------------------------------------------------------------------------
//
// appendClassPathElement()
//
//     <!ELEMENT CLASSPATH (NAMESPACEPATH,CLASSNAME)>
//
//------------------------------------------------------------------------------

void XmlWriter::appendClassPathElement(
    Array<Sint8>& out, 
    const CIMObjectPath& classPath)
{
    out << "<CLASSPATH>\n";
    appendNameSpacePathElement(out,
                               classPath.getHost(),
                               classPath.getNameSpace());
    appendClassNameElement(out, classPath.getClassName());
    out << "</CLASSPATH>\n";
}

//------------------------------------------------------------------------------
//
// appendInstancePathElement()
//
//     <!ELEMENT INSTANCEPATH (NAMESPACEPATH,INSTANCENAME)>
//
//------------------------------------------------------------------------------

void XmlWriter::appendInstancePathElement(
    Array<Sint8>& out, 
    const CIMObjectPath& instancePath)
{
    out << "<INSTANCEPATH>\n";
    appendNameSpacePathElement(out,
                               instancePath.getHost(),
                               instancePath.getNameSpace());
    appendInstanceNameElement(out, instancePath);
    out << "</INSTANCEPATH>\n";
}

//------------------------------------------------------------------------------
//
// appendLocalClassPathElement()
//
//     <!ELEMENT LOCALCLASSPATH (LOCALNAMESPACEPATH, CLASSNAME)>
//
//------------------------------------------------------------------------------

void XmlWriter::appendLocalClassPathElement(
    Array<Sint8>& out, 
    const CIMObjectPath& classPath)
{
    out << "<LOCALCLASSPATH>\n";
    appendLocalNameSpacePathElement(out, classPath.getNameSpace());
    appendClassNameElement(out, classPath.getClassName());
    out << "</LOCALCLASSPATH>\n";
}

//------------------------------------------------------------------------------
//
// appendLocalInstancePathElement()
//
//     <!ELEMENT LOCALINSTANCEPATH (LOCALNAMESPACEPATH, INSTANCENAME)>
//
//------------------------------------------------------------------------------

void XmlWriter::appendLocalInstancePathElement(
    Array<Sint8>& out, 
    const CIMObjectPath& instancePath)
{
    out << "<LOCALINSTANCEPATH>\n";
    appendLocalNameSpacePathElement(out, instancePath.getNameSpace());
    appendInstanceNameElement(out, instancePath);
    out << "</LOCALINSTANCEPATH>\n";
}

//------------------------------------------------------------------------------
//
// appendLocalObjectPathElement()
//
//     If the reference refers to an instance, write a LOCALINSTANCEPATH;
//     otherwise write a LOCALCLASSPATH.
//
//------------------------------------------------------------------------------

void XmlWriter::appendLocalObjectPathElement(
    Array<Sint8>& out, 
    const CIMObjectPath& objectPath)
{
    //
    //  ATTN-CAKG-P2-20020726:  The following condition does not correctly
    //  distinguish instanceNames from classNames in every case
    //  The instanceName of a singleton instance of a keyless class has no 
    //  key bindings
    //
    if (objectPath.getKeyBindings ().size () != 0)
    {
        appendLocalInstancePathElement(out, objectPath);
    }
    else
    {
        appendLocalClassPathElement(out, objectPath);
    }
}

//------------------------------------------------------------------------------
//
// Helper functions for appendValueElement()
//
//------------------------------------------------------------------------------

inline void _appendValue(Array<Sint8>& out, Boolean x)
{
    XmlWriter::append(out, x);
}

inline void _appendValue(Array<Sint8>& out, Uint8 x)
{
    XmlWriter::append(out, Uint32(x));
}

inline void _appendValue(Array<Sint8>& out, Sint8 x)
{
    XmlWriter::append(out, Sint32(x));
}

inline void _appendValue(Array<Sint8>& out, Uint16 x)
{
    XmlWriter::append(out, Uint32(x));
}

inline void _appendValue(Array<Sint8>& out, Sint16 x)
{
    XmlWriter::append(out, Sint32(x));
}

inline void _appendValue(Array<Sint8>& out, Uint32 x)
{
    XmlWriter::append(out, x);
}

inline void _appendValue(Array<Sint8>& out, Sint32 x)
{
    XmlWriter::append(out, x);
}

inline void _appendValue(Array<Sint8>& out, Uint64 x)
{
    XmlWriter::append(out, x);
}

inline void _appendValue(Array<Sint8>& out, Sint64 x)
{
    XmlWriter::append(out, x);
}

inline void _appendValue(Array<Sint8>& out, Real32 x)
{
    XmlWriter::append(out, Real64(x));
}

inline void _appendValue(Array<Sint8>& out, Real64 x)
{
    XmlWriter::append(out, x);
}

inline void _appendValue(Array<Sint8>& out, const Char16& x)
{
    XmlWriter::appendSpecial(out, x);
}

inline void _appendValue(Array<Sint8>& out, const String& x)
{
    XmlWriter::appendSpecial(out, x);
}

inline void _appendValue(Array<Sint8>& out, const CIMDateTime& x)
{
    out << x.toString();  //ATTN: append() method?
}

inline void _appendValue(Array<Sint8>& out, const CIMObjectPath& x)
{
    XmlWriter::appendValueReferenceElement(out, x, true);
}

void _appendValueArray(Array<Sint8>& out, const CIMObjectPath* p, Uint32 size)
{
    out << "<VALUE.REFARRAY>\n";
    while (size--)
    {
        _appendValue(out, *p++);
    }
    out << "</VALUE.REFARRAY>\n";
}

template<class T>
void _appendValueArray(Array<Sint8>& out, const T* p, Uint32 size)
{
    out << "<VALUE.ARRAY>\n";

    while (size--)
    {
        out << "<VALUE>";
        _appendValue(out, *p++);
        out << "</VALUE>\n";
    }

    out << "</VALUE.ARRAY>\n";
}

//------------------------------------------------------------------------------
//
// appendValueElement()
//
//    <!ELEMENT VALUE (#PCDATA)>
//    <!ELEMENT VALUE.ARRAY (VALUE*)>
//    <!ELEMENT VALUE.REFERENCE
//        (CLASSPATH|LOCALCLASSPATH|CLASSNAME|INSTANCEPATH|LOCALINSTANCEPATH|
//         INSTANCENAME)>
//    <!ELEMENT VALUE.REFARRAY (VALUE.REFERENCE*)>
//
//------------------------------------------------------------------------------

void XmlWriter::appendValueElement(
    Array<Sint8>& out,
    const CIMValue& value)
{
    if (value.isNull())
    {
        return;
    }
    if (value.isArray())
    {
        switch (value.getType())
        {
            case CIMTYPE_BOOLEAN:
            {
                Array<Boolean> a;
                value.get(a);
                _appendValueArray(out, a.getData(), a.size());
                break;
            }

            case CIMTYPE_UINT8:
            {
                Array<Uint8> a;
                value.get(a);
                _appendValueArray(out, a.getData(), a.size());
                break;
            }

            case CIMTYPE_SINT8:
            {
                Array<Sint8> a;
                value.get(a);
                _appendValueArray(out, a.getData(), a.size());
                break;
            }

            case CIMTYPE_UINT16:
            {
                Array<Uint16> a;
                value.get(a);
                _appendValueArray(out, a.getData(), a.size());
                break;
            }

            case CIMTYPE_SINT16:
            {
                Array<Sint16> a;
                value.get(a);
                _appendValueArray(out, a.getData(), a.size());
                break;
            }

            case CIMTYPE_UINT32:
            {
                Array<Uint32> a;
                value.get(a);
                _appendValueArray(out, a.getData(), a.size());
                break;
            }

            case CIMTYPE_SINT32:
            {
                Array<Sint32> a;
                value.get(a);
                _appendValueArray(out, a.getData(), a.size());
                break;
            }

            case CIMTYPE_UINT64:
            {
                Array<Uint64> a;
                value.get(a);
                _appendValueArray(out, a.getData(), a.size());
                break;
            }

            case CIMTYPE_SINT64:
            {
                Array<Sint64> a;
                value.get(a);
                _appendValueArray(out, a.getData(), a.size());
                break;
            }

            case CIMTYPE_REAL32:
            {
                Array<Real32> a;
                value.get(a);
                _appendValueArray(out, a.getData(), a.size());
                break;
            }

            case CIMTYPE_REAL64:
            {
                Array<Real64> a;
                value.get(a);
                _appendValueArray(out, a.getData(), a.size());
                break;
            }

            case CIMTYPE_CHAR16:
            {
                Array<Char16> a;
                value.get(a);
                _appendValueArray(out, a.getData(), a.size());
                break;
            }

            case CIMTYPE_STRING:
            {
                Array<String> a;
                value.get(a);
                _appendValueArray(out, a.getData(), a.size());
                break;
            }

            case CIMTYPE_DATETIME:
            {
                Array<CIMDateTime> a;
                value.get(a);
                _appendValueArray(out, a.getData(), a.size());
                break;
            }

            case CIMTYPE_REFERENCE:
            {
                Array<CIMObjectPath> a;
                value.get(a);
                _appendValueArray(out, a.getData(), a.size());
                break;
            }

            default:
                throw CIMValueInvalidTypeException();
        }
    }
    else if (value.getType() == CIMTYPE_REFERENCE)
    {
        // Has to be separate because it uses VALUE.REFERENCE tag
        CIMObjectPath v;
        value.get(v);
        _appendValue(out, v);
    }
    else
    {
        out << "<VALUE>";

        switch (value.getType())
        {
            case CIMTYPE_BOOLEAN:
            {
                Boolean v;
                value.get(v);
                _appendValue(out, v);
                break;
            }

            case CIMTYPE_UINT8:
            {
                Uint8 v;
                value.get(v);
                _appendValue(out, v);
                break;
            }

            case CIMTYPE_SINT8:
            {
                Sint8 v;
                value.get(v);
                _appendValue(out, v);
                break;
            }

            case CIMTYPE_UINT16:
            {
                Uint16 v;
                value.get(v);
                _appendValue(out, v);
                break;
            }

            case CIMTYPE_SINT16:
            {
                Sint16 v;
                value.get(v);
                _appendValue(out, v);
                break;
            }

            case CIMTYPE_UINT32:
            {
                Uint32 v;
                value.get(v);
                _appendValue(out, v);
                break;
            }

            case CIMTYPE_SINT32:
            {
                Sint32 v;
                value.get(v);
                _appendValue(out, v);
                break;
            }

            case CIMTYPE_UINT64:
            {
                Uint64 v;
                value.get(v);
                _appendValue(out, v);
                break;
            }

            case CIMTYPE_SINT64:
            {
                Sint64 v;
                value.get(v);
                _appendValue(out, v);
                break;
            }

            case CIMTYPE_REAL32:
            {
                Real32 v;
                value.get(v);
                _appendValue(out, v);
                break;
            }

            case CIMTYPE_REAL64:
            {
                Real64 v;
                value.get(v);
                _appendValue(out, v);
                break;
            }

            case CIMTYPE_CHAR16:
            {
                Char16 v;
                value.get(v);
                _appendValue(out, v);
                break;
            }

            case CIMTYPE_STRING:
            {
                String v;
                value.get(v);
                _appendValue(out, v);
                break;
            }

            case CIMTYPE_DATETIME:
            {
                CIMDateTime v;
                value.get(v);
                _appendValue(out, v);
                break;
            }

            default:
                throw CIMValueInvalidTypeException();
        }

        out << "</VALUE>\n";
    }
}

void XmlWriter::printValueElement(
    const CIMValue& value,
    PEGASUS_STD(ostream)& os)
{
    Array<Sint8> tmp;
    appendValueElement(tmp, value);
    tmp.append('\0');
    os << tmp.getData() << PEGASUS_STD(endl);
}

//------------------------------------------------------------------------------
//
// appendValueObjectWithPathElement()
//
//     <!ELEMENT VALUE.OBJECTWITHPATH
//         ((CLASSPATH,CLASS)|(INSTANCEPATH,INSTANCE))>
//
//------------------------------------------------------------------------------

void XmlWriter::appendValueObjectWithPathElement(
    Array<Sint8>& out,
    const CIMObject& objectWithPath)
{
    out << "<VALUE.OBJECTWITHPATH>\n";

    appendValueReferenceElement(out, objectWithPath.getPath (), false);
    appendObjectElement(out, objectWithPath);

    out << "</VALUE.OBJECTWITHPATH>\n";
}

//------------------------------------------------------------------------------
//
// appendValueReferenceElement()
//
//    <!ELEMENT VALUE.REFERENCE
//        (CLASSPATH|LOCALCLASSPATH|CLASSNAME|INSTANCEPATH|LOCALINSTANCEPATH|
//         INSTANCENAME)>
//
//------------------------------------------------------------------------------

void XmlWriter::appendValueReferenceElement(
    Array<Sint8>& out,
    const CIMObjectPath& reference,
    Boolean putValueWrapper)
{
    if (putValueWrapper)
        out << "<VALUE.REFERENCE>\n";

    // See if it is a class or instance reference (instance references have
    // key-bindings; class references do not).
    //
    //  ATTN-CAKG-P2-20020726:  The following condition does not correctly
    //  distinguish instanceNames from classNames in every case
    //  The instanceName of a singleton instance of a keyless class has no
    //  key bindings
    //

    KeyBindingArray kbs = reference.getKeyBindings();

    if (kbs.size())
    {
        if (reference.getHost().size())
        {
            appendInstancePathElement(out, reference);
        }
        else if (!reference.getNameSpace().isNull())
        {
            appendLocalInstancePathElement(out, reference);
        }
        else
        {
            appendInstanceNameElement(out, reference);
        }
    }
    else
    {
        if (reference.getHost().size())
        {
            appendClassPathElement(out, reference);
        }
        else if (!reference.getNameSpace().isNull())
        {
            appendLocalClassPathElement(out, reference);
        }
        else
        {
            appendClassNameElement(out, reference.getClassName());
        }
    }

    if (putValueWrapper)
        out << "</VALUE.REFERENCE>\n";
}

void XmlWriter::printValueReferenceElement(
    const CIMObjectPath& reference,
    PEGASUS_STD(ostream)& os)
{
    Array<Sint8> tmp;
    appendValueReferenceElement(tmp, reference, true);
    tmp.append('\0');
    indentedPrint(os, tmp.getData());
}

//------------------------------------------------------------------------------
//
// appendValueNamedInstanceElement()
//
//     <!ELEMENT VALUE.NAMEDINSTANCE (INSTANCENAME,INSTANCE)>
//
//------------------------------------------------------------------------------

void XmlWriter::appendValueNamedInstanceElement(
    Array<Sint8>& out,
    const CIMInstance& namedInstance)
{
    out << "<VALUE.NAMEDINSTANCE>\n";

    appendInstanceNameElement(out, namedInstance.getPath ());
    appendInstanceElement(out, namedInstance);

    out << "</VALUE.NAMEDINSTANCE>\n";
}

//------------------------------------------------------------------------------
//
// appendClassElement()
//
//     <!ELEMENT CLASS
//         (QUALIFIER*,(PROPERTY|PROPERTY.ARRAY|PROPERTY.REFERENCE)*,METHOD*)>
//     <!ATTLIST CLASS
//         %CIMName;
//         %SuperClass;>
//
//------------------------------------------------------------------------------

void XmlWriter::appendClassElement(
    Array<Sint8>& out,
    const CIMConstClass& cimclass)
{
    cimclass._checkRep();
    cimclass._rep->toXml(out);
}

void XmlWriter::printClassElement(
    const CIMConstClass& cimclass,
    PEGASUS_STD(ostream)& os)
{
    Array<Sint8> tmp;
    appendClassElement(tmp, cimclass);
    tmp.append('\0');
    indentedPrint(os, tmp.getData(), 4);
}

//------------------------------------------------------------------------------
//
// appendInstanceElement()
//
//     <!ELEMENT INSTANCE
//         (QUALIFIER*,(PROPERTY|PROPERTY.ARRAY|PROPERTY.REFERENCE)*)>
//     <!ATTLIST INSTANCE
//         %ClassName;>
//
//------------------------------------------------------------------------------

void XmlWriter::appendInstanceElement(
    Array<Sint8>& out,
    const CIMConstInstance& instance)
{
    instance._checkRep();
    instance._rep->toXml(out);
}

void XmlWriter::printInstanceElement(
    const CIMConstInstance& instance,
    PEGASUS_STD(ostream)& os)
{
    Array<Sint8> tmp;
    appendInstanceElement(tmp, instance);
    tmp.append('\0');
    os << tmp.getData() << PEGASUS_STD(endl);
}

//------------------------------------------------------------------------------
//
// appendObjectElement()
//
// May refer to a CLASS or an INSTANCE
//
//------------------------------------------------------------------------------

void XmlWriter::appendObjectElement(
    Array<Sint8>& out,
    const CIMConstObject& object)
{
    if (object.isClass())
    {
        CIMConstClass c(object);
        appendClassElement(out, c);
    }
    else if (object.isInstance())
    {
        CIMConstInstance i(object);
        appendInstanceElement(out, i);
    }
    // else PEGASUS_ASSERT(0);
}

//------------------------------------------------------------------------------
//
// appendPropertyElement()
//
//     <!ELEMENT PROPERTY (QUALIFIER*,VALUE?)>
//     <!ATTLIST PROPERTY
//              %CIMName;
//              %CIMType;           #REQUIRED
//              %ClassOrigin;
//              %Propagated;>
//
//     <!ELEMENT PROPERTY.ARRAY (QUALIFIER*,VALUE.ARRAY?)>
//     <!ATTLIST PROPERTY.ARRAY
//              %CIMName;
//              %CIMType;           #REQUIRED
//              %ArraySize;
//              %ClassOrigin;
//              %Propagated;>
//
//     <!ELEMENT PROPERTY.REFERENCE (QUALIFIER*,VALUE.REFERENCE?)>
//     <!ATTLIST PROPERTY.REFERENCE
//              %CIMName;
//              %ReferenceClass;
//              %ClassOrigin;
//              %Propagated;>
//
//------------------------------------------------------------------------------

void XmlWriter::appendPropertyElement(
    Array<Sint8>& out,
    const CIMConstProperty& property)
{
    property._checkRep();
    property._rep->toXml(out);
}

void XmlWriter::printPropertyElement(
    const CIMConstProperty& property,
    PEGASUS_STD(ostream)& os)
{
    Array<Sint8> tmp;
    appendPropertyElement(tmp, property);
    tmp.append('\0');
    os << tmp.getData() << PEGASUS_STD(endl);
}

//------------------------------------------------------------------------------
//
// appendMethodElement()
//
//     <!ELEMENT METHOD (QUALIFIER*,
//         (PARAMETER|PARAMETER.REFERENCE|PARAMETER.ARRAY|PARAMETER.REFARRAY)*)>
//     <!ATTLIST METHOD
//              %CIMName;
//              %CIMType;          #IMPLIED
//              %ClassOrigin;
//              %Propagated;>
//
//------------------------------------------------------------------------------

void XmlWriter::appendMethodElement(
    Array<Sint8>& out,
    const CIMConstMethod& method)
{
    method._checkRep();
    method._rep->toXml(out);
}

void XmlWriter::printMethodElement(
    const CIMConstMethod& method,
    PEGASUS_STD(ostream)& os)
{
    Array<Sint8> tmp;
    appendMethodElement(tmp, method);
    tmp.append('\0');
    os << tmp.getData() << PEGASUS_STD(endl);
}

//------------------------------------------------------------------------------
//
// appendParameterElement()
//
//     <!ELEMENT PARAMETER (QUALIFIER*)>
//     <!ATTLIST PARAMETER
//              %CIMName;
//              %CIMType;      #REQUIRED>
//
//     <!ELEMENT PARAMETER.REFERENCE (QUALIFIER*)>
//     <!ATTLIST PARAMETER.REFERENCE
//              %CIMName;
//              %ReferenceClass;>
//
//     <!ELEMENT PARAMETER.ARRAY (QUALIFIER*)>
//     <!ATTLIST PARAMETER.ARRAY
//              %CIMName;
//              %CIMType;           #REQUIRED
//              %ArraySize;>
//
//     <!ELEMENT PARAMETER.REFARRAY (QUALIFIER*)>
//     <!ATTLIST PARAMETER.REFARRAY
//              %CIMName;
//              %ReferenceClass;
//              %ArraySize;>
//
//------------------------------------------------------------------------------

void XmlWriter::appendParameterElement(
    Array<Sint8>& out,
    const CIMConstParameter& parameter)
{
    parameter._checkRep();
    parameter._rep->toXml(out);
}

void XmlWriter::printParameterElement(
    const CIMConstParameter& parameter,
    PEGASUS_STD(ostream)& os)
{
    Array<Sint8> tmp;
    appendParameterElement(tmp, parameter);
    tmp.append('\0');
    os << tmp.getData() << PEGASUS_STD(endl);
}

//------------------------------------------------------------------------------
//
// appendParamValueElement()
//
//     <!ELEMENT PARAMVALUE (VALUE|VALUE.REFERENCE|VALUE.ARRAY|VALUE.REFARRAY)?>
//     <!ATTLIST PARAMVALUE
//              %CIMName;>
//
//------------------------------------------------------------------------------

void XmlWriter::appendParamValueElement(
    Array<Sint8>& out,
    const CIMParamValue& paramValue)
{
    paramValue._checkRep();
    paramValue._rep->toXml(out);
}

void XmlWriter::printParamValueElement(
    const CIMParamValue& paramValue,
    PEGASUS_STD(ostream)& os)
{
    Array<Sint8> tmp;
    appendParamValueElement(tmp, paramValue);
    tmp.append('\0');
    os << tmp.getData() << PEGASUS_STD(endl);
}

//------------------------------------------------------------------------------
//
// appendQualifierElement()
//
//     <!ELEMENT QUALIFIER (VALUE|VALUE.ARRAY)>
//     <!ATTLIST QUALIFIER
//              %CIMName;
//              %CIMType;               #REQUIRED
//              %Propagated;
//              %QualifierFlavor;>
//
//------------------------------------------------------------------------------

void XmlWriter::appendQualifierElement(
    Array<Sint8>& out,
    const CIMConstQualifier& qualifier)
{
    qualifier._checkRep();
    qualifier._rep->toXml(out);
}

void XmlWriter::printQualifierElement(
    const CIMConstQualifier& qualifier,
    PEGASUS_STD(ostream)& os)
{
    Array<Sint8> tmp;
    appendQualifierElement(tmp, qualifier);
    tmp.append('\0');
    os << tmp.getData() << PEGASUS_STD(endl);
}

//------------------------------------------------------------------------------
//
// appendQualifierDeclElement()
//
//     <!ELEMENT QUALIFIER.DECLARATION (SCOPE?,(VALUE|VALUE.ARRAY)?)>
//     <!ATTLIST QUALIFIER.DECLARATION
//              %CIMName;
//              %CIMType;                       #REQUIRED
//              ISARRAY        (true|false)     #IMPLIED
//              %ArraySize;
//              %QualifierFlavor;>
//
//------------------------------------------------------------------------------

void XmlWriter::appendQualifierDeclElement(
    Array<Sint8>& out,
    const CIMConstQualifierDecl& qualifierDecl)
{
    qualifierDecl._checkRep();
    qualifierDecl._rep->toXml(out);
}

void XmlWriter::printQualifierDeclElement(
    const CIMConstQualifierDecl& qualifierDecl,
    PEGASUS_STD(ostream)& os)
{
    Array<Sint8> tmp;
    appendQualifierDeclElement(tmp, qualifierDecl);
    tmp.append('\0');
    os << tmp.getData() << PEGASUS_STD(endl);
}

//------------------------------------------------------------------------------
//
// appendQualifierFlavorEntity()
//
//     <!ENTITY % QualifierFlavor "OVERRIDABLE  (true|false)   'true'
//                                 TOSUBCLASS   (true|false)   'true'
//                                 TOINSTANCE   (true|false)   'false'
//                                 TRANSLATABLE (true|false)   'false'">
//
//------------------------------------------------------------------------------

void XmlWriter::appendQualifierFlavorEntity(
    Array<Sint8>& out,
    const CIMFlavor & flavor)
{
    if (!(flavor.hasFlavor (CIMFlavor::OVERRIDABLE)))
        out << " OVERRIDABLE=\"false\"";

    if (!(flavor.hasFlavor (CIMFlavor::TOSUBCLASS)))
        out << " TOSUBCLASS=\"false\"";

    if (flavor.hasFlavor (CIMFlavor::TOINSTANCE))
        out << " TOINSTANCE=\"true\"";

    if (flavor.hasFlavor (CIMFlavor::TRANSLATABLE))
        out << " TRANSLATABLE=\"true\"";
}

//------------------------------------------------------------------------------
//
// appendScopeElement()
//
//     <!ELEMENT SCOPE EMPTY>
//     <!ATTLIST SCOPE
//              CLASS        (true|false)      'false'
//              ASSOCIATION  (true|false)      'false'
//              REFERENCE    (true|false)      'false'
//              PROPERTY     (true|false)      'false'
//              METHOD       (true|false)      'false'
//              PARAMETER    (true|false)      'false'
//              INDICATION   (true|false)      'false'>
//
//------------------------------------------------------------------------------

void XmlWriter::appendScopeElement(
    Array<Sint8>& out,
    const CIMScope & scope)
{
    if (!(scope.equal (CIMScope ())))
    {
        out << "<SCOPE";

        if (scope.hasScope (CIMScope::CLASS))
            out << " CLASS=\"true\"";

        if (scope.hasScope (CIMScope::ASSOCIATION))
            out << " ASSOCIATION=\"true\"";

        if (scope.hasScope (CIMScope::REFERENCE))
            out << " REFERENCE=\"true\"";

        if (scope.hasScope (CIMScope::PROPERTY))
            out << " PROPERTY=\"true\"";

        if (scope.hasScope (CIMScope::METHOD))
            out << " METHOD=\"true\"";

        if (scope.hasScope (CIMScope::PARAMETER))
            out << " PARAMETER=\"true\"";

        if (scope.hasScope (CIMScope::INDICATION))
            out << " INDICATION=\"true\"";

        out << "/>";
    }
}

//------------------------------------------------------------------------------
//
// appendMethodCallHeader()
//
//     Build HTTP method call request header.
//
//------------------------------------------------------------------------------

void XmlWriter::appendMethodCallHeader(
    Array<Sint8>& out,
    const char* host,
    const char* cimMethod,
    const String& cimObject,
    const String& authenticationHeader,
    Uint32 contentLength)
{
    char nn[] = { '0' + (rand() % 10), '0' + (rand() % 10), '\0' };

    out << "M-POST /cimom HTTP/1.1\r\n";
    out << "HOST: " << host << "\r\n";
    out << "Content-Type: application/xml; charset=\"utf-8\"\r\n";
    out << "Content-Length: " << contentLength << "\r\n";
    out << "Man: http://www.dmtf.org/cim/mapping/http/v1.0; ns=";
    out << nn <<"\r\n";
    out << nn << "-CIMOperation: MethodCall\r\n";
    out << nn << "-CIMMethod: " << cimMethod << "\r\n";
    out << nn << "-CIMObject: " << cimObject << "\r\n";
    if (authenticationHeader.size())
    {
        out << authenticationHeader << "\r\n";
    }
    out << "\r\n";
}

//------------------------------------------------------------------------------
//
// appendMethodResponseHeader()
//
//     Build HTTP response header.
//
//------------------------------------------------------------------------------

void XmlWriter::appendMethodResponseHeader(
    Array<Sint8>& out,
    Uint32 contentLength)
{
    char nn[] = { '0' + (rand() % 10), '0' + (rand() % 10), '\0' };

    out << "HTTP/1.1 " HTTP_STATUS_OK "\r\n";
    STAT_SERVERTIME
    out << "Content-Type: application/xml; charset=\"utf-8\"\r\n";
    out << "Content-Length: " << contentLength << "\r\n";
    out << "Ext:\r\n";
    out << "Cache-Control: no-cache\r\n";
    out << "Man: http://www.dmtf.org/cim/mapping/http/v1.0; ns=";
    out << nn <<"\r\n";
    out << nn << "-CIMOperation: MethodResponse\r\n\r\n";
}

//------------------------------------------------------------------------------
//
// appendHttpErrorResponseHeader()
//
//     Build HTTP error response header.
//
//     Returns error response message in the following format:
//
//        HTTP/1.1 400 Bad Request       (using specified status code)
//        CIMError: <error type>         (if specified by caller)
//        PGErrorDetail: <error text>    (if specified by caller)
//
//------------------------------------------------------------------------------

void XmlWriter::appendHttpErrorResponseHeader(
    Array<Sint8>& out,
    const String& status,
    const String& cimError,
    const String& errorDetail)
{
    out << "HTTP/1.1 " << status << "\r\n";
    if (cimError != String::EMPTY)
    {
        out << "CIMError: " << cimError << "\r\n";
    }
    if (errorDetail != String::EMPTY)
    {
        // ATTN-RK-P3-20020404: It is critical that this text not contain '\n'
        // ATTN-RK-P3-20020404: Need to encode this value properly.  (See
        // CIM/HTTP Specification section 3.3.2
        out << PEGASUS_HTTPHEADERTAG_ERRORDETAIL ": " << errorDetail << "\r\n";
    }
    out << "\r\n";
}

//------------------------------------------------------------------------------
//
// appendUnauthorizedResponseHeader()
//
//     Build HTTP authentication response header for unauthorized requests.
//
//     Returns unauthorized message in the following format:
//
//        HTTP/1.1 401 Unauthorized
//        WWW-Authenticate: Basic "hostname:80"
//        <HTML><HEAD>
//        <TITLE>401 Unauthorized</TITLE>
//        </HEAD><BODY BGCOLOR="#99cc99">
//        <H2>TEST401 Unauthorized</H2>
//        <HR>
//        </BODY></HTML>
//
//------------------------------------------------------------------------------

void XmlWriter::appendUnauthorizedResponseHeader(
    Array<Sint8>& out,
    const String& content)
{
    out << "HTTP/1.1 " HTTP_STATUS_UNAUTHORIZED "\r\n";
    out << content << "\r\n";
    out << "\r\n";

//ATTN: We may need to include the following line, so that the browsers
//      can display the error message.
//    out << "<HTML><HEAD>\r\n";
//    out << "<TITLE>" << "401 Unauthorized" <<  "</TITLE>\r\n";
//    out << "</HEAD><BODY BGCOLOR=\"#99cc99\">\r\n";
//    out << "<H2>TEST" << "401 Unauthorized" << "</H2>\r\n";
//    out << "<HR>\r\n";
//    out << "</BODY></HTML>\r\n";
}

//------------------------------------------------------------------------------
//
// _appendMessageElementBegin()
// _appendMessageElementEnd()
//
//     <!ELEMENT MESSAGE (SIMPLEREQ|MULTIREQ|SIMPLERSP|MULTIRSP)>
//     <!ATTLIST MESSAGE
//         ID CDATA #REQUIRED
//         PROTOCOLVERSION CDATA #REQUIRED>
//
//------------------------------------------------------------------------------

void XmlWriter::_appendMessageElementBegin(
    Array<Sint8>& out,
    const String& messageId)
{
    out << "<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n";
    out << "<CIM CIMVERSION=\"2.0\" DTDVERSION=\"2.0\">\n";
    out << "<MESSAGE ID=\"" << messageId << "\" PROTOCOLVERSION=\"1.0\">\n";
}

void XmlWriter::_appendMessageElementEnd(
    Array<Sint8>& out)
{
    out << "</MESSAGE>\n";
    out << "</CIM>\n";
}

//------------------------------------------------------------------------------
//
// _appendSimpleReqElementBegin()
// _appendSimpleReqElementEnd()
//
//     <!ELEMENT SIMPLEREQ (IMETHODCALL|METHODCALL)>
//
//------------------------------------------------------------------------------

void XmlWriter::_appendSimpleReqElementBegin(
    Array<Sint8>& out)
{
    out << "<SIMPLEREQ>\n";
}

void XmlWriter::_appendSimpleReqElementEnd(
    Array<Sint8>& out)
{
    out << "</SIMPLEREQ>\n";
}

//------------------------------------------------------------------------------
//
// _appendMethodCallElementBegin()
// _appendMethodCallElementEnd()
//
//     <!ELEMENT METHODCALL ((LOCALCLASSPATH|LOCALINSTANCEPATH),PARAMVALUE*)>
//     <!ATTLIST METHODCALL %CIMName;>
//
//------------------------------------------------------------------------------

void XmlWriter::_appendMethodCallElementBegin(
    Array<Sint8>& out,
    const char* name)
{
    out << "<METHODCALL NAME=\"" << name << "\">\n";
}

void XmlWriter::_appendMethodCallElementEnd(
    Array<Sint8>& out)
{
    out << "</METHODCALL>\n";
}

//------------------------------------------------------------------------------
//
// _appendIMethodCallElementBegin()
// _appendIMethodCallElementEnd()
//
//     <!ELEMENT IMETHODCALL (LOCALNAMESPACEPATH,IPARAMVALUE*)>
//     <!ATTLIST IMETHODCALL %CIMName;>
//
//------------------------------------------------------------------------------

void XmlWriter::_appendIMethodCallElementBegin(
    Array<Sint8>& out,
    const char* name)
{
    out << "<IMETHODCALL NAME=\"" << name << "\">\n";
}

void XmlWriter::_appendIMethodCallElementEnd(
    Array<Sint8>& out)
{
    out << "</IMETHODCALL>\n";
}

//------------------------------------------------------------------------------
//
// _appendIParamValueElementBegin()
// _appendIParamValueElementEnd()
//
//     <!ELEMENT IPARAMVALUE (VALUE|VALUE.ARRAY|VALUE.REFERENCE
//         |INSTANCENAME|CLASSNAME|QUALIFIER.DECLARATION
//         |CLASS|INSTANCE|VALUE.NAMEDINSTANCE)?>
//     <!ATTLIST IPARAMVALUE %CIMName;>
//
//------------------------------------------------------------------------------

void XmlWriter::_appendIParamValueElementBegin(
    Array<Sint8>& out,
    const char* name)
{
    out << "<IPARAMVALUE NAME=\"" << name << "\">\n";
}

void XmlWriter::_appendIParamValueElementEnd(
    Array<Sint8>& out)
{
    out << "</IPARAMVALUE>\n";
}

//------------------------------------------------------------------------------
//
// _appendSimpleRspElementBegin()
// _appendSimpleRspElementEnd()
//
//     <!ELEMENT SIMPLERSP (METHODRESPONSE|IMETHODRESPONSE)>
//
//------------------------------------------------------------------------------

void XmlWriter::_appendSimpleRspElementBegin(
    Array<Sint8>& out)
{
    out << "<SIMPLERSP>\n";
}

void XmlWriter::_appendSimpleRspElementEnd(
    Array<Sint8>& out)
{
    out << "</SIMPLERSP>\n";
}

//------------------------------------------------------------------------------
//
// _appendMethodResponseElementBegin()
// _appendMethodResponseElementEnd()
//
//     <!ELEMENT METHODRESPONSE (ERROR|IRETURNVALUE?)>
//     <!ATTLIST METHODRESPONSE %CIMName;>
//
//------------------------------------------------------------------------------

void XmlWriter::_appendMethodResponseElementBegin(
    Array<Sint8>& out,
    const char* name)
{
    out << "<METHODRESPONSE NAME=\"" << name << "\">\n";
}

void XmlWriter::_appendMethodResponseElementEnd(
    Array<Sint8>& out)
{
    out << "</METHODRESPONSE>\n";
}

//------------------------------------------------------------------------------
//
// _appendIMethodResponseElementBegin()
// _appendIMethodResponseElementEnd()
//
//     <!ELEMENT IMETHODRESPONSE (ERROR|IRETURNVALUE?)>
//     <!ATTLIST IMETHODRESPONSE %CIMName;>
//
//------------------------------------------------------------------------------

void XmlWriter::_appendIMethodResponseElementBegin(
    Array<Sint8>& out,
    const char* name)
{
    out << "<IMETHODRESPONSE NAME=\"" << name << "\">\n";
}

void XmlWriter::_appendIMethodResponseElementEnd(
    Array<Sint8>& out)
{
    out << "</IMETHODRESPONSE>\n";
}

//------------------------------------------------------------------------------
//
// _appendErrorElement()
//
//------------------------------------------------------------------------------

void XmlWriter::_appendErrorElement(
    Array<Sint8>& out,
    const CIMException& cimException)
{
    Tracer::traceCIMException(TRC_XML_WRITER, Tracer::LEVEL2, cimException);

    out << "<ERROR";
    out << " CODE=\"" << Uint32(cimException.getCode()) << "\"";
    String description = TraceableCIMException(cimException).getDescription();
    if (description != String::EMPTY)
    {
        out << " DESCRIPTION=\"";
        appendSpecial(out, description);
        out << "\"";
    }
    out << "/>";
}

//------------------------------------------------------------------------------
//
// appendReturnValueElement()
//
// <!ELEMENT RETURNVALUE (VALUE|VALUE.REFERENCE)>
// <!ATTLIST RETURNVALUE
//     %ParamType;>
//
//------------------------------------------------------------------------------

void XmlWriter::appendReturnValueElement(
    Array<Sint8>& out,
    const CIMValue& value)
{
    out << "<RETURNVALUE";

    CIMType type = value.getType();
    out << " PARAMTYPE=\"" << cimTypeToString (type) << "\"";

    out << ">\n";

    // Add value.
    appendValueElement(out, value);
    out << "</RETURNVALUE>\n";
}

//------------------------------------------------------------------------------
//
// _appendIReturnValueElementBegin()
// _appendIReturnValueElementEnd()
//
//      <!ELEMENT IRETURNVALUE (CLASSNAME*|INSTANCENAME*|VALUE*|
//          VALUE.OBJECTWITHPATH*|VALUE.OBJECTWITHLOCALPATH*|VALUE.OBJECT*|
//          OBJECTPATH*|QUALIFIER.DECLARATION*|VALUE.ARRAY?|VALUE.REFERENCE?|
//          CLASS*|INSTANCE*|VALUE.NAMEDINSTANCE*)>
//
//------------------------------------------------------------------------------

void XmlWriter::_appendIReturnValueElementBegin(
    Array<Sint8>& out)
{
    out << "<IRETURNVALUE>\n";
}

void XmlWriter::_appendIReturnValueElementEnd(
    Array<Sint8>& out)
{
    out << "</IRETURNVALUE>\n";
}

//------------------------------------------------------------------------------
//
// appendBooleanIParameter()
//
//------------------------------------------------------------------------------

void XmlWriter::appendBooleanIParameter(
    Array<Sint8>& out,
    const char* name,
    Boolean flag)
{
    _appendIParamValueElementBegin(out, name);
    out << "<VALUE>";
    append(out, flag);
    out << "</VALUE>\n";
    _appendIParamValueElementEnd(out);
}

//------------------------------------------------------------------------------
//
// appendStringIParameter()
//
//------------------------------------------------------------------------------

void XmlWriter::appendStringIParameter(
    Array<Sint8>& out,
    const char* name,
    const String& str)
{
    _appendIParamValueElementBegin(out, name);
    out << "<VALUE>";
    appendSpecial(out, str);
    out << "</VALUE>\n";
    _appendIParamValueElementEnd(out);
}

//------------------------------------------------------------------------------
//
// appendQualifierNameIParameter()
//
//------------------------------------------------------------------------------

void XmlWriter::appendQualifierNameIParameter(
    Array<Sint8>& out,
    const char* name,
    const String& qualifierName)
{
    // <!ELEMENT IPARAMVALUE (VALUE|VALUE.ARRAY|VALUE.REFERENCE
    //     |INSTANCENAME|CLASSNAME|QUALIFIER.DECLARATION
    //     |CLASS|INSTANCE|VALUE.NAMEDINSTANCE)?>
    //
    // ATTN: notice that there is really no way to pass a qualifier name
    // as an IPARAMVALUE element according to the spec (look above). So we
    // just pass it as a class name. An answer must be obtained later.
    
    _appendIParamValueElementBegin(out, name);
    appendClassNameElement(out, qualifierName);
    _appendIParamValueElementEnd(out);
}

//------------------------------------------------------------------------------
//
// appendClassNameIParameter()
//
//------------------------------------------------------------------------------

void XmlWriter::appendClassNameIParameter(
    Array<Sint8>& out,
    const char* name,
    const String& className)
{
    _appendIParamValueElementBegin(out, name);
    appendClassNameElement(out, className);
    _appendIParamValueElementEnd(out);
}

//------------------------------------------------------------------------------
//
// appendInstanceNameIParameter()
//
//------------------------------------------------------------------------------

void XmlWriter::appendInstanceNameIParameter(
    Array<Sint8>& out,
    const char* name,
    const CIMObjectPath& instanceName)
{
    _appendIParamValueElementBegin(out, name);
    appendInstanceNameElement(out, instanceName);
    _appendIParamValueElementEnd(out);
}

void XmlWriter::appendObjectNameIParameter(
    Array<Sint8>& out,
    const char* name,
    const CIMObjectPath& objectName)
{
    //
    //  ATTN-CAKG-P2-20020726:  The following condition does not correctly
    //  distinguish instanceNames from classNames in every case
    //  The instanceName of a singleton instance of a keyless class also 
    //  has no key bindings
    //
    if (objectName.getKeyBindings ().size () == 0)
    {
	XmlWriter::appendClassNameIParameter(
	    out, name, objectName.getClassName());
    }
    else
    {
	XmlWriter::appendInstanceNameIParameter(
	    out, name, objectName);
    }
}

//------------------------------------------------------------------------------
//
// appendClassIParameter()
//
//------------------------------------------------------------------------------

void XmlWriter::appendClassIParameter(
    Array<Sint8>& out,
    const char* name,
    const CIMConstClass& cimClass)
{
    _appendIParamValueElementBegin(out, name);
    appendClassElement(out, cimClass);
    _appendIParamValueElementEnd(out);
}

//------------------------------------------------------------------------------
//
// appendInstanceIParameter()
//
//------------------------------------------------------------------------------

void XmlWriter::appendInstanceIParameter(
    Array<Sint8>& out,
    const char* name,
    const CIMConstInstance& instance)
{
    _appendIParamValueElementBegin(out, name);
    appendInstanceElement(out, instance);
    _appendIParamValueElementEnd(out);
}

//------------------------------------------------------------------------------
//
// appendNamedInstanceIParameter()
//
//------------------------------------------------------------------------------

void XmlWriter::appendNamedInstanceIParameter(
    Array<Sint8>& out,
    const char* name,
    const CIMInstance& namedInstance)
{
    _appendIParamValueElementBegin(out, name);
    appendValueNamedInstanceElement(out, namedInstance);
    _appendIParamValueElementEnd(out);
}

//----------------------------------------------------------
//
//  appendPropertyNameIParameter()
//   	
//     </IPARAMVALUE>
//     <IPARAMVALUE NAME="PropertyName"><VALUE>FreeSpace</VALUE></IPARAMVALUE>
//
//     USE: Create parameter for getProperty operation
//==========================================================
void XmlWriter::appendPropertyNameIParameter(
    Array<Sint8>& out,
    const String& propertyName)
{
    _appendIParamValueElementBegin(out, "PropertyName");
    out << "<VALUE>" << propertyName << "</VALUE>\n"; 
    _appendIParamValueElementEnd(out);
}

//------------------------------------------------------------------------------
//
// appendPropertyValueIParameter()
//
//------------------------------------------------------------------------------

void XmlWriter::appendPropertyValueIParameter(
    Array<Sint8>& out,
    const char* name,
    const CIMValue& value)
{
    _appendIParamValueElementBegin(out, name);
    appendValueElement(out, value);
    _appendIParamValueElementEnd(out);
}

//------------------------------------------------------------------------------
//
// appendPropertyListIParameter()
//
//------------------------------------------------------------------------------

void XmlWriter::appendPropertyListIParameter(
    Array<Sint8>& out,
    const CIMPropertyList& propertyList)
{
    // ATTN: P3 KS 4 Mar 2002 - As check shouldn't we check for null property list
    _appendIParamValueElementBegin(out, "PropertyList");

    out << "<VALUE.ARRAY>\n";
    for (Uint32 i = 0; i < propertyList.size(); i++)
    {
        out << "<VALUE>" << propertyList[i] << "</VALUE>\n"; 
    }
    out << "</VALUE.ARRAY>\n";

    _appendIParamValueElementEnd(out);
}

//------------------------------------------------------------------------------
//
// appendQualifierDeclarationIParameter()
//
//------------------------------------------------------------------------------

void XmlWriter::appendQualifierDeclarationIParameter(
    Array<Sint8>& out,
    const char* name,
    const CIMConstQualifierDecl& qualifierDecl)
{
    _appendIParamValueElementBegin(out, name);
    appendQualifierDeclElement(out, qualifierDecl);
    _appendIParamValueElementEnd(out);
}

//------------------------------------------------------------------------------
//
// XmlWriter::formatHttpErrorRspMessage()
//
//------------------------------------------------------------------------------

Array<Sint8> XmlWriter::formatHttpErrorRspMessage(
    const String& status,
    const String& cimError,
    const String& errorDetail)
{
    Array<Sint8> out;

    appendHttpErrorResponseHeader(out, status, cimError, errorDetail);

    return out;
}

//------------------------------------------------------------------------------
//
// XmlWriter::formatSimpleMethodReqMessage()
//
//------------------------------------------------------------------------------

// ATTN-RK-P1-20020228: Need to complete copy elimination optimization
Array<Sint8> XmlWriter::formatSimpleMethodReqMessage(
    const char* host,
    const String& nameSpace,
    const CIMObjectPath& path,
    const char* methodName,
    const Array<CIMParamValue>& parameters,
    const String& messageId,
    const String& authenticationHeader)
{
    Array<Sint8> out;
    Array<Sint8> tmp;
    CIMObjectPath localObjectPath = path;
    localObjectPath.setNameSpace(nameSpace);

    _appendMessageElementBegin(out, messageId);
    _appendSimpleReqElementBegin(out);
    _appendMethodCallElementBegin(out, methodName);
    appendLocalObjectPathElement(out, localObjectPath);
    for (Uint32 i=0; i < parameters.size(); i++)
    {
        appendParamValueElement(out, parameters[i]);
    }
    _appendMethodCallElementEnd(out);
    _appendSimpleReqElementEnd(out);
    _appendMessageElementEnd(out);

    appendMethodCallHeader(
	tmp,
	host,
	methodName,
	localObjectPath.toString(false),
        authenticationHeader,
	out.size());
    tmp << out;

    return tmp;
}

Array<Sint8> XmlWriter::formatSimpleMethodRspMessage(
    const char* methodName,
    const String& messageId,
    const Array<Sint8>& body)
{
    Array<Sint8> out;
    Array<Sint8> tmp;

    _appendMessageElementBegin(out, messageId);
    _appendSimpleRspElementBegin(out);
    _appendMethodResponseElementBegin(out, methodName);
    out << body;
    _appendMethodResponseElementEnd(out);
    _appendSimpleRspElementEnd(out);
    _appendMessageElementEnd(out);

    appendMethodResponseHeader(tmp, out.size());
    tmp << out;

    return tmp;
}

//------------------------------------------------------------------------------
//
// XmlWriter::formatSimpleMethodErrorRspMessage()
//
//------------------------------------------------------------------------------

Array<Sint8> XmlWriter::formatSimpleMethodErrorRspMessage(
    const String& methodName,
    const String& messageId,
    const CIMException& cimException)
{
    Array<Sint8> out;
    Array<Sint8> tmp;

    _appendMessageElementBegin(out, messageId);
    _appendSimpleRspElementBegin(out);
    _appendMethodResponseElementBegin(out, methodName.getCString());
    _appendErrorElement(out, cimException);
    _appendMethodResponseElementEnd(out);
    _appendSimpleRspElementEnd(out);
    _appendMessageElementEnd(out);

    appendMethodResponseHeader(tmp, out.size());
    tmp << out;

    return tmp;
}

//------------------------------------------------------------------------------
//
// XmlWriter::formatSimpleIMethodReqMessage()
//
//------------------------------------------------------------------------------

Array<Sint8> XmlWriter::formatSimpleIMethodReqMessage(
    const char* host,
    const String& nameSpace,
    const char* iMethodName,
    const String& messageId,
    const String& authenticationHeader,
    const Array<Sint8>& body)
{
    Array<Sint8> out;
    Array<Sint8> tmp;

    _appendMessageElementBegin(out, messageId);
    _appendSimpleReqElementBegin(out);
    _appendIMethodCallElementBegin(out, iMethodName);
    appendLocalNameSpacePathElement(out, nameSpace);
    out << body;
    _appendIMethodCallElementEnd(out);
    _appendSimpleReqElementEnd(out);
    _appendMessageElementEnd(out);

    appendMethodCallHeader(
	tmp,
	host,
	iMethodName,
	nameSpace,
        authenticationHeader,
	out.size());
    tmp << out;

    return tmp;
}

//------------------------------------------------------------------------------
//
// XmlWriter::formatSimpleIMethodRspMessage()
//
//------------------------------------------------------------------------------

Array<Sint8> XmlWriter::formatSimpleIMethodRspMessage(
    const char* iMethodName,
    const String& messageId,
    const Array<Sint8>& body)
{
    Array<Sint8> out;
    Array<Sint8> tmp;

    _appendMessageElementBegin(out, messageId);
    _appendSimpleRspElementBegin(out);
    _appendIMethodResponseElementBegin(out, iMethodName);
    if (body.size() != 0)
    {
        _appendIReturnValueElementBegin(out);
        out << body;
        _appendIReturnValueElementEnd(out);
    }
    _appendIMethodResponseElementEnd(out);
    _appendSimpleRspElementEnd(out);
    _appendMessageElementEnd(out);

    appendMethodResponseHeader(tmp, out.size());
    tmp << out;

    return tmp;
}

//------------------------------------------------------------------------------
//
// XmlWriter::formatSimpleIMethodErrorRspMessage()
//
//------------------------------------------------------------------------------

Array<Sint8> XmlWriter::formatSimpleIMethodErrorRspMessage(
    const String& iMethodName,
    const String& messageId,
    const CIMException& cimException)
{
    Array<Sint8> out;
    Array<Sint8> tmp;

    _appendMessageElementBegin(out, messageId);
    _appendSimpleRspElementBegin(out);
    _appendIMethodResponseElementBegin(out, iMethodName.getCString());
    _appendErrorElement(out, cimException);
    _appendIMethodResponseElementEnd(out);
    _appendSimpleRspElementEnd(out);
    _appendMessageElementEnd(out);

    appendMethodResponseHeader(tmp, out.size());
    tmp << out;

    return tmp;
}

//******************************************************************************
//
// Export Messages (used for indications)
//
//******************************************************************************

//------------------------------------------------------------------------------
//
// appendEMethodRequestHeader()
//
//     Build HTTP request header for export operation.
//
//------------------------------------------------------------------------------

void XmlWriter::appendEMethodRequestHeader(
    Array<Sint8>& out,
    const char* requestUri,
    const char* host,
    const char* cimMethod,
    const String& authenticationHeader,
    Uint32 contentLength)
{
    char nn[] = { '0' + (rand() % 10), '0' + (rand() % 10), '\0' };

    out << "M-POST " << requestUri << " HTTP/1.1\r\n";
    out << "HOST: " << host << "\r\n";
    out << "Content-Type: application/xml; charset=\"utf-8\"\r\n";
    out << "Content-Length: " << contentLength << "\r\n";
    out << "Man: http://www.hp.com; ns=";
    out << nn <<"\r\n";
    out << nn << "-CIMExport: MethodRequest\r\n";
    out << nn << "-CIMExportMethod: " << cimMethod << "\r\n";
    if (authenticationHeader.size())
    {
        out << authenticationHeader << "\r\n";
    }
    out << "\r\n";
}

//------------------------------------------------------------------------------
//
// appendEMethodResponseHeader()
//
//     Build HTTP response header for export operation.
//
//------------------------------------------------------------------------------

void XmlWriter::appendEMethodResponseHeader(
    Array<Sint8>& out,
    Uint32 contentLength)
{
    char nn[] = { '0' + (rand() % 10), '0' + (rand() % 10), '\0' };

    out << "HTTP/1.1 " HTTP_STATUS_OK "\r\n";
    out << "Content-Type: application/xml; charset=\"utf-8\"\r\n";
    out << "Content-Length: " << contentLength << "\r\n";
    out << "Ext:\r\n";
    out << "Cache-Control: no-cache\r\n";
    out << "Man: http://www.dmtf.org/cim/mapping/http/v1.0; ns=";
    out << nn <<"\r\n";
    out << nn << "-CIMExport: MethodResponse\r\n\r\n";
}

//------------------------------------------------------------------------------
//
// _appendSimpleExportReqElementBegin()
// _appendSimpleExportReqElementEnd()
//
//     <!ELEMENT SIMPLEEXPREQ (EXPMETHODCALL)>
//
//------------------------------------------------------------------------------

void XmlWriter::_appendSimpleExportReqElementBegin(
    Array<Sint8>& out)
{
    out << "<SIMPLEEXPREQ>\n";
}

void XmlWriter::_appendSimpleExportReqElementEnd(
    Array<Sint8>& out)
{
    out << "</SIMPLEEXPREQ>\n";
}

//------------------------------------------------------------------------------
//
// _appendEMethodCallElementBegin()
// _appendEMethodCallElementEnd()
//
//     <!ELEMENT EXPMETHODCALL (IPARAMVALUE*)>
//     <!ATTLIST EXPMETHODCALL %CIMName;>
//
//------------------------------------------------------------------------------

void XmlWriter::_appendEMethodCallElementBegin(
    Array<Sint8>& out,
    const char* name)
{
    out << "<EXPMETHODCALL NAME=\"" << name << "\">\n";
}

void XmlWriter::_appendEMethodCallElementEnd(
    Array<Sint8>& out)
{
    out << "</EXPMETHODCALL>\n";
}

//------------------------------------------------------------------------------
//
// _appendSimpleExportRspElementBegin()
// _appendSimpleExportRspElementEnd()
//
//     <!ELEMENT SIMPLEEXPRSP (EXPMETHODRESPONSE)>
//
//------------------------------------------------------------------------------

void XmlWriter::_appendSimpleExportRspElementBegin(
    Array<Sint8>& out)
{
    out << "<SIMPLEEXPRSP>\n";
}

void XmlWriter::_appendSimpleExportRspElementEnd(
    Array<Sint8>& out)
{
    out << "</SIMPLEEXPRSP>\n";
}

//------------------------------------------------------------------------------
//
// _appendEMethodResponseElementBegin()
// _appendEMethodResponseElementEnd()
//
//     <!ELEMENT EXPMETHODRESPONSE (ERROR|IRETURNVALUE?)>
//     <!ATTLIST EXPMETHODRESPONSE %CIMName;>
//
//------------------------------------------------------------------------------

void XmlWriter::_appendEMethodResponseElementBegin(
    Array<Sint8>& out,
    const char* name)
{
    out << "<EXPMETHODRESPONSE NAME=\"" << name << "\">\n";
}

void XmlWriter::_appendEMethodResponseElementEnd(
    Array<Sint8>& out)
{
    out << "</EXPMETHODRESPONSE>\n";
}

//------------------------------------------------------------------------------
//
// XmlWriter::formatSimpleEMethodReqMessage()
//
//------------------------------------------------------------------------------

Array<Sint8> XmlWriter::formatSimpleEMethodReqMessage(
    const char* requestUri,
    const char* host,
    const char* eMethodName,
    const String& messageId,
    const String& authenticationHeader,
    const Array<Sint8>& body)
{
    Array<Sint8> out;
    Array<Sint8> tmp;

    _appendMessageElementBegin(out, messageId);
    _appendSimpleExportReqElementBegin(out);
    _appendEMethodCallElementBegin(out, eMethodName);
    out << body;
    _appendEMethodCallElementEnd(out);
    _appendSimpleExportReqElementEnd(out);
    _appendMessageElementEnd(out);

    appendEMethodRequestHeader(
        tmp,
        requestUri,
        host,
        eMethodName,
        authenticationHeader,
	out.size());
    tmp << out;

    return tmp;
}

//------------------------------------------------------------------------------
//
// XmlWriter::formatSimpleEMethodRspMessage()
//
//------------------------------------------------------------------------------

Array<Sint8> XmlWriter::formatSimpleEMethodRspMessage(
    const char* eMethodName,
    const String& messageId,
    const Array<Sint8>& body)
{
    Array<Sint8> out;
    Array<Sint8> tmp;

    _appendMessageElementBegin(out, messageId);
    _appendSimpleExportRspElementBegin(out);
    _appendEMethodResponseElementBegin(out, eMethodName);
    out << body;
    _appendEMethodResponseElementEnd(out);
    _appendSimpleExportRspElementEnd(out);
    _appendMessageElementEnd(out);

    appendEMethodResponseHeader(tmp, out.size());
    tmp << out;

    return tmp;
}

//------------------------------------------------------------------------------
//
// XmlWriter::formatSimpleEMethodErrorRspMessage()
//
//------------------------------------------------------------------------------

Array<Sint8> XmlWriter::formatSimpleEMethodErrorRspMessage(
    const String& eMethodName,
    const String& messageId,
    const CIMException& cimException)
{
    Array<Sint8> out;
    Array<Sint8> tmp;

    _appendMessageElementBegin(out, messageId);
    _appendSimpleExportRspElementBegin(out);
    _appendEMethodResponseElementBegin(out, eMethodName.getCString());
    _appendErrorElement(out, cimException);
    _appendEMethodResponseElementEnd(out);
    _appendSimpleExportRspElementEnd(out);
    _appendMessageElementEnd(out);

    appendEMethodResponseHeader(tmp, out.size());
    tmp << out;

    return tmp;
}

//------------------------------------------------------------------------------
//
// _printAttributes()
//
//------------------------------------------------------------------------------

static void _printAttributes(
    PEGASUS_STD(ostream)& os,
    const XmlAttribute* attributes,
    Uint32 attributeCount)
{
    for (Uint32 i = 0; i < attributeCount; i++)
    {
	os << attributes[i].name << "=";

	os << '"';
	_appendSpecial(os, attributes[i].value);
	os << '"';

	if (i + 1 != attributeCount)
	    os << ' ';
    }
}

//------------------------------------------------------------------------------
//
// _indent()
//
//------------------------------------------------------------------------------

static void _indent(PEGASUS_STD(ostream)& os, Uint32 level, Uint32 indentChars)
{
    Uint32 n = level * indentChars;

    for (Uint32 i = 0; i < n; i++)
	os << ' ';
}

//------------------------------------------------------------------------------
//
// indentedPrint()
//
//------------------------------------------------------------------------------

void XmlWriter::indentedPrint(
    PEGASUS_STD(ostream)& os, 
    const char* text, 
    Uint32 indentChars)
{
    char* tmp = strcpy(new char[strlen(text) + 1], text);

    XmlParser parser(tmp);
    XmlEntry entry;
    Stack<const char*> stack;

    while (parser.next(entry))
    {
	switch (entry.type)
	{
	    case XmlEntry::XML_DECLARATION:
	    {
		_indent(os, stack.size(), indentChars);

		os << "<?" << entry.text << " ";
		_printAttributes(os, entry.attributes, entry.attributeCount);
		os << "?>";
		break;
	    }

	    case XmlEntry::START_TAG:
	    {
		_indent(os, stack.size(), indentChars);

		os << "<" << entry.text;

		if (entry.attributeCount)
		    os << ' ';

		_printAttributes(os, entry.attributes, entry.attributeCount);
		os << ">";
		stack.push(entry.text);
		break;
	    }

	    case XmlEntry::EMPTY_TAG:
	    {
		_indent(os, stack.size(), indentChars);

		os << "<" << entry.text << " ";
		_printAttributes(os, entry.attributes, entry.attributeCount);
		os << "/>";
		break;
	    }

	    case XmlEntry::END_TAG:
	    {
		if (!stack.isEmpty() && strcmp(stack.top(), entry.text) == 0)
		    stack.pop();

		_indent(os, stack.size(), indentChars);

		os << "</" << entry.text << ">";
		break;
	    }

	    case XmlEntry::COMMENT:
	    {

		_indent(os, stack.size(), indentChars);
		os << "<!--";
		_appendSpecial(os, entry.text);
		os << "-->";
		break;
	    }

	    case XmlEntry::CONTENT:
	    {
		_indent(os, stack.size(), indentChars);
		_appendSpecial(os, entry.text);
		break;
	    }

	    case XmlEntry::CDATA:
	    {
		_indent(os, stack.size(), indentChars);
		os << "<![CDATA[...]]>";
		break;
	    }

	    case XmlEntry::DOCTYPE:
	    {
		_indent(os, stack.size(), indentChars);
		os << "<!DOCTYPE...>";
		break;
	    }
	}

	os << PEGASUS_STD(endl);
    }

    delete [] tmp;
}

//------------------------------------------------------------------------------
//
// XmlWriter::getNextMessageId()
//
//------------------------------------------------------------------------------

String XmlWriter::getNextMessageId()
{
    // ATTN: make thread-safe:
    static Uint32 messageId = 1000;

    messageId++;

    if (messageId < 1000)
	messageId = 1001;

    char buffer[16];
    sprintf(buffer, "%d", messageId);
    return buffer;
}

//------------------------------------------------------------------------------
//
// XmlWriter::keyBindingTypeToString
//
//------------------------------------------------------------------------------
const char* XmlWriter::keyBindingTypeToString (KeyBinding::Type type)
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

PEGASUS_NAMESPACE_END
