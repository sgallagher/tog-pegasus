//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 BMC Software, Hewlett-Packard Company, IBM,
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
// Author: Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
// Modified By:
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
#include "CIMReference.h"
#include "CIMValue.h"
#include "XmlWriter.h"
#include "MofWriter.h"
#include "Tracer.h"

PEGASUS_NAMESPACE_BEGIN

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

inline void _appendValue(Array<Sint8>& out, Char16 x)
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
inline void _appendValue(Array<Sint8>& out, const String& x)
{
    out << "\"";
    const Char16* tmp = x.getData();
    char c;
    while ((c = *tmp++))
    {
        switch (c)
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
                out.append(Sint8(c));
        }

    }
    out << "\"";
}

inline void _appendValue(Array<Sint8>& out, const CIMDateTime& x)
{
    out << x.getString();  //ATTN: append() method?
}

inline void _appendValue(Array<Sint8>& out, const CIMReference& x)
{
    XmlWriter::appendValueReferenceElement(out, x, true);
}

/** Array -
    arrayInitializer = "{" constantValue*( "," constantValue)"}"
*/
template<class T>
void _appendValueArrayMof(Array<Sint8>& out, const T* p, Uint32 size)
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
            _appendValue(out, *p++);
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
    Array<Sint8>& out,
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
            case CIMType::BOOLEAN:
            {
                Array<Boolean> a;
                value.get(a);
                _appendValueArrayMof(out, a.getData(), a.size());
                break;
            }

            case CIMType::UINT8:
            {
                Array<Uint8> a;
                value.get(a);
                _appendValueArrayMof(out, a.getData(), a.size());
                break;
            }

            case CIMType::SINT8:
            {
                Array<Sint8> a;
                value.get(a);
                _appendValueArrayMof(out, a.getData(), a.size());
                break;
            }

            case CIMType::UINT16:
            {
                Array<Uint16> a;
                value.get(a);
                _appendValueArrayMof(out, a.getData(), a.size());
                break;
            }

            case CIMType::SINT16:
            {
                Array<Sint16> a;
                value.get(a);
                _appendValueArrayMof(out, a.getData(), a.size());
                break;
            }

            case CIMType::UINT32:
            {
                Array<Uint32> a;
                value.get(a);
                _appendValueArrayMof(out, a.getData(), a.size());
                break;
            }

            case CIMType::SINT32:
            {
                Array<Sint32> a;
                value.get(a);
                _appendValueArrayMof(out, a.getData(), a.size());
                break;
            }

            case CIMType::UINT64:
            {
                Array<Uint64> a;
                value.get(a);
                _appendValueArrayMof(out, a.getData(), a.size());
                break;
            }

            case CIMType::SINT64:
            {
                Array<Sint64> a;
                value.get(a);
                _appendValueArrayMof(out, a.getData(), a.size());
                break;
            }

            case CIMType::REAL32:
            {
                Array<Real32> a;
                value.get(a);
                _appendValueArrayMof(out, a.getData(), a.size());
                break;
            }

            case CIMType::REAL64:
            {
                Array<Real64> a;
                value.get(a);
                _appendValueArrayMof(out, a.getData(), a.size());
                break;
            }

            case CIMType::CHAR16:
            {
                Array<Char16> a;
                value.get(a);
                _appendValueArrayMof(out, a.getData(), a.size());
                break;
            }

            case CIMType::STRING:
            {
                Array<String> a;
                value.get(a);
                _appendValueArrayMof(out, a.getData(), a.size());
                break;
            }

            case CIMType::DATETIME:
            {
                Array<CIMDateTime> a;
                value.get(a);
                _appendValueArrayMof(out, a.getData(), a.size());
                break;
            }

            case CIMType::REFERENCE:
            {
                Array<CIMReference> a;
                value.get(a);
                _appendValueArrayMof(out, a.getData(), a.size());
                break;
            }

            default:
                throw CIMValueInvalidType();
        }
    }
    else
    {
        switch (value.getType())
        {
            case CIMType::BOOLEAN:
            {
                Boolean v;
                value.get(v);
                _appendValue(out, v);
                break;
            }

            case CIMType::UINT8:
            {
                Uint8 v;
                value.get(v);
                _appendValue(out, v);
                break;
            }

            case CIMType::SINT8:
            {
                Sint8 v;
                value.get(v);
                _appendValue(out, v);
                break;
            }

            case CIMType::UINT16:
            {
                Uint16 v;
                value.get(v);
                _appendValue(out, v);
                break;
            }

            case CIMType::SINT16:
            {
                Sint16 v;
                value.get(v);
                _appendValue(out, v);
                break;
            }

            case CIMType::UINT32:
            {
                Uint32 v;
                value.get(v);
                _appendValue(out, v);
                break;
            }

            case CIMType::SINT32:
            {
                Sint32 v;
                value.get(v);
                _appendValue(out, v);
                break;
            }

            case CIMType::UINT64:
            {
                Uint64 v;
                value.get(v);
                _appendValue(out, v);
                break;
            }

            case CIMType::SINT64:
            {
                Sint64 v;
                value.get(v);
                _appendValue(out, v);
                break;
            }

            case CIMType::REAL32:
            {
                Real32 v;
                value.get(v);
                _appendValue(out, v);
                break;
            }

            case CIMType::REAL64:
            {
                Real64 v;
                value.get(v);
                _appendValue(out, v);
                break;
            }

            case CIMType::CHAR16:
            {
                Char16 v;
                value.get(v);
                _appendValue(out, v);
                break;
            }

            case CIMType::STRING:
            {
                String v;
                value.get(v);
                _appendValue(out, v);
                break;
            }

            case CIMType::DATETIME:
            {
                CIMDateTime v;
                value.get(v);
                _appendValue(out, v);
                break;
            }

            case CIMType::REFERENCE:
            {
                CIMReference v;
                value.get(v);
                _appendValue(out, v);
                break;
            }

            default:
                throw CIMValueInvalidType();
        }
    }
}

//------------------------------------------------------------------------------
//
// appendValueReferenceElement()
//
//------------------------------------------------------------------------------

void MofWriter::appendValueReferenceElement(
    Array<Sint8>& out,
    const CIMReference& reference)
{
    out << "MOF not implemented:\n";
    XmlWriter::appendValueReferenceElement(out, reference, true);
}

//------------------------------------------------------------------------------
//
// appendClassElement()
//
//------------------------------------------------------------------------------

void MofWriter::appendClassElement(
    Array<Sint8>& out,
    const CIMConstClass& cimclass)
{
    cimclass._checkRep();
    cimclass._rep->toMof(out);
}

void MofWriter::printClassElement(
    const CIMConstClass& cimclass,
    PEGASUS_STD(ostream)& os)
{
    Array<Sint8> tmp;
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
    Array<Sint8>& out,
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
    Array<Sint8>& out,
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
    Array<Sint8>& out,
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
    Array<Sint8>& out,
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
    Array<Sint8>& out,
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
    Array<Sint8>& out,
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
//     DisableOverride Qualifier cannot be overridden.          no
//     ToSubclass      Qualifier is inherited by any subclass. yes
//     Restricted      Qualifier applies only to the class     no
//                     in which it is declared
//     Translatable    Indicates the value of the qualifier
//                     can be specified inmultiple languages   no
//     NOTE: There is an open issue with the keyword toinstance.
//
//     flavor            = ENABLEOVERRIDE | DISABLEOVERRIDE | RESTRICTED |
//                         TOSUBCLASS | TRANSLATABLE
//     DISABLEOVERRIDE   = "disableOverride"
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

String MofWriter::getQualifierFlavor(Uint32 flavor)
{
    String tmp = "";

    if (!(flavor & CIMFlavor::OVERRIDABLE))
        tmp += "DisableOverride, ";

    if (!(flavor & CIMFlavor::TOSUBCLASS))
        tmp += "Restricted, ";

    // ATTN-RK-P3-20020515: FUTURE: Need to check toInstance flavor?
    //if (!(flavor & CIMFlavor::TOINSTANCE))
    //    tmp += "Restricted, ";

    if (flavor & CIMFlavor::TRANSLATABLE)
        tmp += "Translatable, ";

    if (tmp.size())
        tmp.remove(tmp.size() - 2);

    return tmp;
}

PEGASUS_NAMESPACE_END
