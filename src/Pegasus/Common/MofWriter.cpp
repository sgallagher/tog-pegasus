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
// Author: Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
// Modified By: Carol Ann Krug Graves, Hewlett-Packard Company
//                  (carolann_graves@hp.com)
//              Dave Sudlik, IBM (dsudlik@us.ibm.com)
//              David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <cstdlib>
#include <cstdio>
#include "Constants.h"
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
#include "CIMQualifier.h"
#include "CIMQualifierRep.h"
#include "CIMQualifierDecl.h"
#include "CIMQualifierDeclRep.h"
#include "CIMObjectPath.h"
#include "CIMValue.h"
#include "CIMFlavor.h"
#include "CIMScope.h"
#include "XmlWriter.h"
#include "MofWriter.h"
#include "Tracer.h"

PEGASUS_NAMESPACE_BEGIN

//------------------------------------------------------------------------------
//
// Helper functions for appendValueElement()
//
//------------------------------------------------------------------------------

inline void _mofWriter_appendValue(Array<char>& out, Boolean x)
{
    XmlWriter::append(out, x);
}

inline void _mofWriter_appendValue(Array<char>& out, Uint8 x)
{
    XmlWriter::append(out, Uint32(x));
}

inline void _mofWriter_appendValue(Array<char>& out, Sint8 x)
{
    XmlWriter::append(out, Sint32(x));
}

inline void _mofWriter_appendValue(Array<char>& out, Uint16 x)
{
    XmlWriter::append(out, Uint32(x));
}

inline void _mofWriter_appendValue(Array<char>& out, Sint16 x)
{
    XmlWriter::append(out, Sint32(x));
}

inline void _mofWriter_appendValue(Array<char>& out, Uint32 x)
{
    XmlWriter::append(out, x);
}

inline void _mofWriter_appendValue(Array<char>& out, Sint32 x)
{
    XmlWriter::append(out, x);
}

inline void _mofWriter_appendValue(Array<char>& out, Uint64 x)
{
    XmlWriter::append(out, x);
}

inline void _mofWriter_appendValue(Array<char>& out, Sint64 x)
{
    XmlWriter::append(out, x);
}

inline void _mofWriter_appendValue(Array<char>& out, Real32 x)
{
    XmlWriter::append(out, Real64(x));
}

inline void _mofWriter_appendValue(Array<char>& out, Real64 x)
{
    XmlWriter::append(out, x);
}

inline void _mofWriter_appendValue(Array<char>& out, Char16 x)
{
    XmlWriter::appendSpecial(out, x);
}

/** Convert the string back to MOF format and output it.
    The conversions are:
    \b // \x0008: backspace BS
    \t // \x0009: horizontal tab HT
    \n // \x000A: linefeed LF
    \f // \x000C: form feed FF
    \r // \x000D: carriage return CR
    \" // \x0022: double quote "
    \’ // \x0027: single quote '
    \\ // \x005C: backslash \
    \x<hex> // where <hex> is one to four hex digits
    \X<hex> // where <hex> is one to four hex digits
*/
/* ATTN:KS - We need to account for characters greater than x'7f
*/
inline void _mofWriter_appendValue(Array<char>& out, const String& x)
{
    out << "\"";
    const Uint32 size = x.size();
    for (Uint32 i = 0; i < size; i++)
    {
        switch (x[i])
        {
        case '\\':
                out.append("\\\\",2);
                break;

            case '\b':
                out.append("\\b",2);
                break;

            case '\t':
                out.append("\\t",2);
                break;

            case '\n':
                out.append("\\n",2);
                break;

            case '\f':
                out.append("\\f",2);
                break;

            case '\r':
                out.append("\\r",2);
                break;

           /* case '\'':
                out.append("\\'", 2);
                break;*/

            case '"':
                out.append("\\\"", 2);
                break;

            default:
                out.append(Sint8(x[i]));
        }

    }
    out << "\"";
}

inline void _mofWriter_appendValue(Array<char>& out, const CIMDateTime& x)
{
    _mofWriter_appendValue(out, x.toString());
}

inline void _mofWriter_appendValue(Array<char>& out, const CIMObjectPath& x)
{
    _mofWriter_appendValue(out, x.toString());
}

inline void _mofWriter_appendValue(Array<char>& out, const CIMObject& x)
{
    _mofWriter_appendValue(out, x.toString());
}

/** Array -
    arrayInitializer = "{" constantValue*( "," constantValue)"}"
*/
template<class T>

void _mofWriter_appendValueArrayMof(Array<char>& out, const T* p, Uint32 size)
{
    Boolean isFirstEntry = true;
    // if there are any entries in the array output them
    if (size)
    {
        out << "{";
        while (size--)
        {
            // Put comma on all but first entry.
            if (!isFirstEntry)
            {
                out << ", ";
            }
            isFirstEntry = false;
            _mofWriter_appendValue(out, *p++);
        }
        out << "}";
    }
}

//------------------------------------------------------------------------------
//
// appendValueElement()
//
//------------------------------------------------------------------------------

void MofWriter::appendValueElement(
    Array<char>& out,
    const CIMValue& value)
{
    // if the CIMValue is Null we return the Null indicator.
    if (value.isNull())
    {
        out << "null";
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
                _mofWriter_appendValueArrayMof(out, a.getData(), a.size());
                break;
            }

            case CIMTYPE_UINT8:
            {
                Array<Uint8> a;
                value.get(a);
                _mofWriter_appendValueArrayMof(out, a.getData(), a.size());
                break;
            }

            case CIMTYPE_SINT8:
            {
                Array<Sint8> a;
                value.get(a);
                _mofWriter_appendValueArrayMof(out, a.getData(), a.size());
                break;
            }

            case CIMTYPE_UINT16:
            {
                Array<Uint16> a;
                value.get(a);
                _mofWriter_appendValueArrayMof(out, a.getData(), a.size());
                break;
            }

            case CIMTYPE_SINT16:
            {
                Array<Sint16> a;
                value.get(a);
                _mofWriter_appendValueArrayMof(out, a.getData(), a.size());
                break;
            }

            case CIMTYPE_UINT32:
            {
                Array<Uint32> a;
                value.get(a);
                _mofWriter_appendValueArrayMof(out, a.getData(), a.size());
                break;
            }

            case CIMTYPE_SINT32:
            {
                Array<Sint32> a;
                value.get(a);
                _mofWriter_appendValueArrayMof(out, a.getData(), a.size());
                break;
            }

            case CIMTYPE_UINT64:
            {
                Array<Uint64> a;
                value.get(a);
                _mofWriter_appendValueArrayMof(out, a.getData(), a.size());
                break;
            }

            case CIMTYPE_SINT64:
            {
                Array<Sint64> a;
                value.get(a);
                _mofWriter_appendValueArrayMof(out, a.getData(), a.size());
                break;
            }

            case CIMTYPE_REAL32:
            {
                Array<Real32> a;
                value.get(a);
                _mofWriter_appendValueArrayMof(out, a.getData(), a.size());
                break;
            }

            case CIMTYPE_REAL64:
            {
                Array<Real64> a;
                value.get(a);
                _mofWriter_appendValueArrayMof(out, a.getData(), a.size());
                break;
            }

            case CIMTYPE_CHAR16:
            {
                Array<Char16> a;
                value.get(a);
                _mofWriter_appendValueArrayMof(out, a.getData(), a.size());
                break;
            }

            case CIMTYPE_STRING:
            {
                Array<String> a;
                value.get(a);
                _mofWriter_appendValueArrayMof(out, a.getData(), a.size());
                break;
            }

            case CIMTYPE_DATETIME:
            {
                Array<CIMDateTime> a;
                value.get(a);
                _mofWriter_appendValueArrayMof(out, a.getData(), a.size());
                break;
            }

            case CIMTYPE_REFERENCE:
            {
                Array<CIMObjectPath> a;
                value.get(a);
                _mofWriter_appendValueArrayMof(out, a.getData(), a.size());
                break;
            }

            case CIMTYPE_OBJECT:
            {
                Array<CIMObject> a;
                value.get(a);
                _mofWriter_appendValueArrayMof(out, a.getData(), a.size());
                break;
            }

            default:
                PEGASUS_ASSERT(false);
        }
    }
    else
    {
        switch (value.getType())
        {
            case CIMTYPE_BOOLEAN:
            {
                Boolean v;
                value.get(v);
                _mofWriter_appendValue(out, v);
                break;
            }

            case CIMTYPE_UINT8:
            {
                Uint8 v;
                value.get(v);
                _mofWriter_appendValue(out, v);
                break;
            }

            case CIMTYPE_SINT8:
            {
                Sint8 v;
                value.get(v);
                _mofWriter_appendValue(out, v);
                break;
            }

            case CIMTYPE_UINT16:
            {
                Uint16 v;
                value.get(v);
                _mofWriter_appendValue(out, v);
                break;
            }

            case CIMTYPE_SINT16:
            {
                Sint16 v;
                value.get(v);
                _mofWriter_appendValue(out, v);
                break;
            }

            case CIMTYPE_UINT32:
            {
                Uint32 v;
                value.get(v);
                _mofWriter_appendValue(out, v);
                break;
            }

            case CIMTYPE_SINT32:
            {
                Sint32 v;
                value.get(v);
                _mofWriter_appendValue(out, v);
                break;
            }

            case CIMTYPE_UINT64:
            {
                Uint64 v;
                value.get(v);
                _mofWriter_appendValue(out, v);
                break;
            }

            case CIMTYPE_SINT64:
            {
                Sint64 v;
                value.get(v);
                _mofWriter_appendValue(out, v);
                break;
            }

            case CIMTYPE_REAL32:
            {
                Real32 v;
                value.get(v);
                _mofWriter_appendValue(out, v);
                break;
            }

            case CIMTYPE_REAL64:
            {
                Real64 v;
                value.get(v);
                _mofWriter_appendValue(out, v);
                break;
            }

            case CIMTYPE_CHAR16:
            {
                Char16 v;
                value.get(v);
                _mofWriter_appendValue(out, v);
                break;
            }

            case CIMTYPE_STRING:
            {
                String v;
                value.get(v);
                _mofWriter_appendValue(out, v);
                break;
            }

            case CIMTYPE_DATETIME:
            {
                CIMDateTime v;
                value.get(v);
                _mofWriter_appendValue(out, v);
                break;
            }

            case CIMTYPE_REFERENCE:
            {
                CIMObjectPath v;
                value.get(v);
                _mofWriter_appendValue(out, v);
                break;
            }

            case CIMTYPE_OBJECT:
            {
                CIMObject v;
                value.get(v);
                _mofWriter_appendValue(out, v);
                break;
            }

            default:
                PEGASUS_ASSERT(false);
        }
    }
}

//------------------------------------------------------------------------------
//
// appendValueReferenceElement()
//
//------------------------------------------------------------------------------

void MofWriter::appendValueReferenceElement(
    Array<char>& out,
    const CIMObjectPath& reference)
{
    _mofWriter_appendValue(out, reference);
}

//------------------------------------------------------------------------------
//
// appendClassElement()
//
//------------------------------------------------------------------------------

void MofWriter::appendClassElement(
    Array<char>& out,
    const CIMConstClass& cimclass)
{
    cimclass._checkRep();
    cimclass._rep->toMof(out);
}

void MofWriter::printClassElement(
    const CIMConstClass& cimclass,
    PEGASUS_STD(ostream)& os)
{
    Array<char> tmp;
    appendClassElement(tmp, cimclass);
    tmp.append('\0');
    os << tmp.getData() << PEGASUS_STD(endl);
}

//------------------------------------------------------------------------------
//
// appendInstanceElement()
//
//------------------------------------------------------------------------------

void MofWriter::appendInstanceElement(
    Array<char>& out,
    const CIMConstInstance& instance)
{
    instance._checkRep();
    instance._rep->toMof(out);
}

//------------------------------------------------------------------------------
//
// appendPropertyElement()
//
//------------------------------------------------------------------------------

void MofWriter::appendPropertyElement(
    Array<char>& out,
    const CIMConstProperty& property)
{
    property._checkRep();
    property._rep->toMof(out);
}

//------------------------------------------------------------------------------
//
// appendMethodElement()
//
//------------------------------------------------------------------------------

void MofWriter::appendMethodElement(
    Array<char>& out,
    const CIMConstMethod& method)
{
    method._checkRep();
    method._rep->toMof(out);
}

//------------------------------------------------------------------------------
//
// appendParameterElement()
//
//------------------------------------------------------------------------------

void MofWriter::appendParameterElement(
    Array<char>& out,
    const CIMConstParameter& parameter)
{
    parameter._checkRep();
    parameter._rep->toMof(out);
}

//------------------------------------------------------------------------------
//
// appendQualifierElement()
//
//------------------------------------------------------------------------------

void MofWriter::appendQualifierElement(
    Array<char>& out,
    const CIMConstQualifier& qualifier)
{
    qualifier._checkRep();
    qualifier._rep->toMof(out);
}

//------------------------------------------------------------------------------
//
// appendQualifierDeclElement()
//
//------------------------------------------------------------------------------

void MofWriter::appendQualifierDeclElement(
    Array<char>& out,
    const CIMConstQualifierDecl& qualifierDecl)
{
    qualifierDecl._checkRep();
    qualifierDecl._rep->toMof(out);
}

//------------------------------------------------------------------------------
//
// getQualifierFlavor()
//
// Convert the Qualifier flavors to a string of MOF flavor keywords.
//
//   <pre>
//   Keyword            Function                             Default
//     EnableOverride  Qualifier is overridable.               yes
//     DisableOverride Qualifier cannot be overridden.         no
//     ToSubclass      Qualifier is inherited by any subclass. yes
//     Restricted      Qualifier applies only to the class     no
//                     in which it is declared
//     Translatable    Indicates the value of the qualifier
//                     can be specified in multiple languages  no
//     NOTE: There is an open issue with the keyword toinstance.
//
//     flavor            = ENABLEOVERRIDE | DISABLEOVERRIDE | RESTRICTED |
//                         TOSUBCLASS | TRANSLATABLE
//     DISABLEOVERRIDE   = "disableoverride"
//
//     ENABLEOVERRIDE    = "enableoverride"
//
//     RESTRICTED        = "restricted"
//
//     TOSUBCLASS        = "tosubclass"
//
//     TRANSLATABLE      = "translatable"
//    </pre>
//
//    The keyword toinstance is not in the CIM specification. For the moment 
//    we are assuming that it is the same as the toSubclass. We had a choice
//    of using one entity for both or separating them and letting the
//    compiler set both.
//
//------------------------------------------------------------------------------

String MofWriter::getQualifierFlavor(const CIMFlavor & flavor)
{
    String tmp;

    if (!(flavor.hasFlavor (CIMFlavor::OVERRIDABLE)))
        tmp.append("DisableOverride, ");

    if (!(flavor.hasFlavor (CIMFlavor::TOSUBCLASS)))
        tmp.append("Restricted, ");

    // ATTN-RK-P3-20020515: FUTURE: Need to check toInstance flavor?
    //if (!(flavor.hasFlavor (CIMFlavor::TOINSTANCE)))
    //    tmp.append("Restricted, ");

    if (flavor.hasFlavor (CIMFlavor::TRANSLATABLE))
        tmp.append("Translatable, ");

    if (tmp.size())
        tmp.remove(tmp.size() - 2);

    return tmp;
}

//------------------------------------------------------------------------------
//
// getQualifierScope()
//
//------------------------------------------------------------------------------

String MofWriter::getQualifierScope (const CIMScope & scope)
{
    if (scope.equal (CIMScope::ANY))
    {
        return "any";
    }
    else
    {
        String tmp = scope.toString ();
        tmp.toLower();
        return tmp;
    }
}

PEGASUS_NAMESPACE_END
