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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By: Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
//              Nag Boranna, Hewlett-Packard Company (nagaraja_boranna@hp.com)
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                  (carolann_graves@hp.com)
//              Amit K Arora, IBM (amita@in.ibm.com) for PEP#101
//         Brian G. Campbell, EMC (campbell_brian@emc.com) - PEP140/phase1
//				 Willis White (whiwill@us.ibm.com) PEP 127 and 128
//         Brian G. Campbell, EMC (campbell_brian@emc.com) - PEP140/phase2
//              Dave Sudlik, IBM (dsudlik@us.ibm.com)
//              David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
//              Vijay Eli, vijayeli@in.ibm.com, fix for #2571
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
#include "CommonUTF.h"

PEGASUS_NAMESPACE_BEGIN

// This is a shortcut macro for outputing content length. This
// pads the output number to the max characters representing a Uint32 number
// so that it can be overwritten easily with a transfer encoding line later
// on in HTTPConnection if required. This is strictly for performance since
// messages can be very large. This overwriting shortcut allows us to NOT have
// to repackage a large message later.

#define OUTPUT_CONTENTLENGTH                                   \
{                                                              \
    char contentLengthP[11];                                   \
    sprintf(contentLengthP,"%.10u", contentLength);            \
    out << "content-length: " << contentLengthP << "\r\n";     \
}

Array<char>& operator<<(Array<char>& out, const char* x)
{
    XmlWriter::append(out, x);
    return out;
}

Array<char>& operator<<(Array<char>& out, char x)
{
    XmlWriter::append(out, x);
    return out;
}

Array<char>& operator<<(Array<char>& out, const Char16& x)
{
    XmlWriter::append(out, x);
    return out;
}

Array<char>& operator<<(Array<char>& out, const String& x)
{
    XmlWriter::append(out, x);
    return out;
}

Array<char>& operator<<(Array<char>& out, const Indentor& x)
{
    XmlWriter::append(out, x);
    return out;
}

Array<char>& operator<<(Array<char>& out, const Array<char>& x)
{
    out.appendArray(x);
    return out;
}

Array<char>& operator<<(Array<char>& out, Uint32 x)
{
    XmlWriter::append(out, x);
    return out;
}

Array<char>& operator<<(Array<char>& out, const CIMName& name)
{
    XmlWriter::append(out, name.getString ());
    return out;
}


// l10n
Array<char>& operator<<(Array<char>& out, const AcceptLanguages& al)
{
    XmlWriter::append(out, al.toString ());
    return out;
}

// l10n
Array<char>& operator<<(Array<char>& out, const ContentLanguages& cl)
{
    XmlWriter::append(out, cl.toString ());
    return out;
}


PEGASUS_STD(ostream)& operator<<(PEGASUS_STD(ostream)& os, const CIMDateTime& x)
{
    return os << x.toString();
}

PEGASUS_STD(ostream)& operator<<(PEGASUS_STD(ostream)& os, const CIMName& name)
{
    os << name.getString();
    return os;
}

PEGASUS_STD(ostream)& operator<<(PEGASUS_STD(ostream)& os,
    const CIMNamespaceName& name)
{
    os << name.getString();
    return os;
}

inline void _xmlWritter_appendChar(Array<char>& out, const Char16& c)
{
    // We need to convert the Char16 to UTF8 then append the UTF8
    // character into the array.
    // NOTE: The UTF8 character could be several bytes long.
    // WARNING: This function will put in replacement character for
    // all characters that have surogate pairs.
    char str[6];
    memset(str,0x00,sizeof(str));
    Uint8* charIN = (Uint8 *)&c;

    const Uint16 *strsrc = (Uint16 *)charIN;
    Uint16 *endsrc = (Uint16 *)&charIN[1];

    Uint8 *strtgt = (Uint8 *)str;
    Uint8 *endtgt = (Uint8 *)&str[5];

    UTF16toUTF8(&strsrc,
		endsrc,
		&strtgt,
		endtgt);

    out.append(str, UTF_8_COUNT_TRAIL_BYTES(str[0]) + 1);
}

inline void _xmlWritter_appendSpecialChar(Array<char>& out, const Char16& c)
{
    if ( ((c < Char16(0x20)) && (c >= Char16(0x00))) || (c == Char16(0x7f)) )
    {
        char charref[7];
        sprintf(charref, "&#%u;", (Uint16)c);
        out.append(charref, static_cast<Uint32>(strlen(charref)));
    }
    else
    {
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
		{
		    // We need to convert the Char16 to UTF8 then append the UTF8
		    // character into the array.
		    // NOTE: The UTF8 character could be several bytes long.
		    // WARNING: This function will put in replacement character for
		    // all characters that have surogate pairs.
            char str[6];
		    memset(str,0x00,sizeof(str));
		    const char* charIN = reinterpret_cast<const char *>(&c);

		    const Uint16 *strsrc = (const Uint16 *)charIN;
		    const Uint16 *endsrc = (const Uint16 *)&charIN[1];

		    Uint8 *strtgt = (Uint8 *)str;
		    Uint8 *endtgt = (Uint8 *)&str[5];

		    UTF16toUTF8(&strsrc,
				endsrc,
				&strtgt,
				endtgt);

		    Uint32 number1 = UTF_8_COUNT_TRAIL_BYTES(str[0]) + 1;

		    out.append(str,number1);
		}
        }
    }
}

inline void _xmlWritter_appendSpecialChar(Array<char>& out, char c)
{
	if ( ((c < 0x20) && (c >= 0)) || (c == 0x7f) )
    {
        char charref[7];
        sprintf(charref, "&#%u;", (Uint8)c);
        out.append(charref, static_cast<Uint32>(strlen(charref)));
    }
    else
    {
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
		out.append(static_cast<Sint8>(c));
        }
    }
}


inline void _xmlWritter_appendSpecialChar(PEGASUS_STD(ostream)& os, char c)
{
    if ( ((c < 0x20) && (c >= 0)) || (c == 0x7f) )
    {
        char charref[7];
        sprintf(charref, "&#%u;", static_cast<Uint8>(c));
        os << charref;
    }
    else
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
}

void _xmlWritter_appendSurrogatePair(Array<char>& out, Uint16 high, Uint16 low)
{
    char str[6];
    Uint8 charIN[5];
    memset(str,0x00,sizeof(str));
    memcpy(&charIN,&high,2);
    memcpy(&charIN[2],&low,2);
    const Uint16 *strsrc = (Uint16 *)charIN;
    Uint16 *endsrc = (Uint16 *)&charIN[3];

    Uint8 *strtgt = (Uint8 *)str;
    Uint8 *endtgt = (Uint8 *)&str[5];

    UTF16toUTF8(&strsrc,
		endsrc,
		&strtgt,
		endtgt);

    Uint32 number1 = UTF_8_COUNT_TRAIL_BYTES(str[0]) + 1;
    out.append(str,number1);
}

inline void _xmlWritter_appendSpecial(PEGASUS_STD(ostream)& os, const char* str)
{
    while (*str)
	_xmlWritter_appendSpecialChar(os, *str++);
}

void XmlWriter::append(Array<char>& out, const Char16& x)
{
    _xmlWritter_appendChar(out, x);
}

void XmlWriter::append(Array<char>& out, Boolean x)
{
    append(out, (x ? "TRUE" : "FALSE"));
}

void XmlWriter::append(Array<char>& out, Uint32 x)
{
    char buffer[32];
    sprintf(buffer, "%u", x);
    append(out, buffer);
}

void XmlWriter::append(Array<char>& out, Sint32 x)
{
    char buffer[32];
    sprintf(buffer, "%d", x);
    append(out, buffer);
}

void XmlWriter::append(Array<char>& out, Uint64 x)
{
    char buffer[32];  // Should need 21 chars max
    sprintf(buffer, "%" PEGASUS_64BIT_CONVERSION_WIDTH "u", x);
    append(out, buffer);
}

void XmlWriter::append(Array<char>& out, Sint64 x)
{
    char buffer[32];  // Should need 21 chars max
    sprintf(buffer, "%" PEGASUS_64BIT_CONVERSION_WIDTH "d", x);
    append(out, buffer);
}

void XmlWriter::append(Array<char>& out, Real32 x)
{
    char buffer[128];
    // %.7e gives '[-]m.ddddddde+/-xx', which seems compatible with the format
    // given in the CIM/XML spec, and the precision required by the CIM 2.2 spec
    // (4 byte IEEE floating point)
    sprintf(buffer, "%.7e", x);
    append(out, buffer);
}

void XmlWriter::append(Array<char>& out, Real64 x)
{
    char buffer[128];
    // %.16e gives '[-]m.dddddddddddddddde+/-xx', which seems compatible with the format
    // given in the CIM/XML spec, and the precision required by the CIM 2.2 spec
    // (8 byte IEEE floating point)
    sprintf(buffer, "%.16e", x);
    append(out, buffer);
}

void XmlWriter::append(Array<char>& out, const char* str)
{
    while (*str)
      XmlWriter::append(out, *str++);
}

void XmlWriter::append(Array<char>& out, const String& str)
{
    for (Uint32 i = 0; i < str.size(); i++)
    {
        Uint16 c = str[i];
	if(((c >= FIRST_HIGH_SURROGATE) && (c <= LAST_HIGH_SURROGATE)) ||
	   ((c >= FIRST_LOW_SURROGATE) && (c <= LAST_LOW_SURROGATE)))
	{
	    Char16 highSurrogate = str[i];
	    Char16 lowSurrogate = str[++i];

	    _xmlWritter_appendSurrogatePair(out, Uint16(highSurrogate),Uint16(lowSurrogate));
	}
	else
	{
	    _xmlWritter_appendChar(out, str[i]);
	}
    }
}

void XmlWriter::append(Array<char>& out, const Indentor& x)
{
    for (Uint32 i = 0; i < 4 * x.getLevel(); i++)
	out.append(' ');
}

void XmlWriter::appendSpecial(Array<char>& out, const Char16& x)
{
    _xmlWritter_appendSpecialChar(out, x);
}

void XmlWriter::appendSpecial(Array<char>& out, char x)
{
    _xmlWritter_appendSpecialChar(out, x);
}

void XmlWriter::appendSpecial(Array<char>& out, const char* str)
{
    while (*str)
	_xmlWritter_appendSpecialChar(out, *str++);
}

void XmlWriter::appendSpecial(Array<char>& out, const String& str)
{
    for (Uint32 i = 0; i < str.size(); i++)
    {
        Uint16 c = str[i];

	if(((c >= FIRST_HIGH_SURROGATE) && (c <= LAST_HIGH_SURROGATE)) ||
	   ((c >= FIRST_LOW_SURROGATE) && (c <= LAST_LOW_SURROGATE)))
	{
	    Char16 highSurrogate = str[i];
	    Char16 lowSurrogate = str[++i];

	    _xmlWritter_appendSurrogatePair(out, Uint16(highSurrogate),Uint16(lowSurrogate));
	}
	else
	{
	    _xmlWritter_appendSpecialChar(out, str[i]);
	}
    }
}

// See http://www.ietf.org/rfc/rfc2396.txt section 2
// Reserved characters = ';' '/' '?' ':' '@' '&' '=' '+' '$' ','
// Excluded characters:
//   Control characters = 0x00-0x1f, 0x7f
//   Space character = 0x20
//   Delimiters = '<' '>' '#' '%' '"'
//   Unwise = '{' '}' '|' '\\' '^' '[' ']' '`'
//

inline void _xmlWritter_encodeURIChar(String& outString, Sint8 char8)
{
    Uint8 c = (Uint8)char8;

#ifndef PEGASUS_DO_NOT_IMPLEMENT_URI_ENCODING
    if ( (c <= 0x20) ||                     // Control characters + space char
         ( (c >= 0x22) && (c <= 0x26) ) ||  // '"' '#' '$' '%' '&'
         (c == 0x2b) ||                     // '+'
         (c == 0x2c) ||                     // ','
         (c == 0x2f) ||                     // '/'
         ( (c >= 0x3a) && (c <= 0x40) ) ||  // ':' ';' '<' '=' '>' '?' '@'
         ( (c >= 0x5b) && (c <= 0x5e) ) ||  // '[' '\\' ']' '^'
         (c == 0x60) ||                     // '`'
         ( (c >= 0x7b) && (c <= 0x7d) ) ||  // '{' '|' '}'
         (c >= 0x7f) )                      // Control character or non US-ASCII (UTF-8)
    {
        char hexencoding[4];

        sprintf(hexencoding, "%%%X%X", c/16, c%16);
        outString.append(hexencoding);
    }
    else
#endif
    {
        outString.append((Uint16)c);
    }
}

String XmlWriter::encodeURICharacters(const Array<char>& uriString)
{
    String encodedString;

    for (Uint32 i=0; i<uriString.size(); i++)
    {
        _xmlWritter_encodeURIChar(encodedString, uriString[i]);
    }

    return encodedString;
}

String XmlWriter::encodeURICharacters(const String& uriString)
{
    String encodedString;

/* i18n remove - did not handle surrogate pairs
    for (Uint32 i=0; i<uriString.size(); i++)
    {
        _xmlWritter_encodeURIChar(encodedString, uriString[i]);
    }
*/

    // See the "CIM Operations over HTTP" spec, section 3.3.2 and
    // 3.3.3, for the treatment of non US-ASCII (UTF-8) chars

    // First, convert to UTF-8 (include handling of surrogate pairs)
    Array<char> utf8;
    for (Uint32 i = 0; i < uriString.size(); i++)
    {
        Uint16 c = uriString[i];

	if(((c >= FIRST_HIGH_SURROGATE) && (c <= LAST_HIGH_SURROGATE)) ||
	   ((c >= FIRST_LOW_SURROGATE) && (c <= LAST_LOW_SURROGATE)))
	{
	    Char16 highSurrogate = uriString[i];
	    Char16 lowSurrogate = uriString[++i];

	    _xmlWritter_appendSurrogatePair(utf8, Uint16(highSurrogate),Uint16(lowSurrogate));
	}
        else
        {
            _xmlWritter_appendChar(utf8, uriString[i]);
        }
    }

    // Second, escape the non HTTP-safe chars
    for (Uint32 i=0; i<utf8.size(); i++)
    {
        _xmlWritter_encodeURIChar(encodedString, utf8[i]);
    }

    return encodedString;
}

//------------------------------------------------------------------------------
//
// appendLocalNameSpacePathElement()
//
//     <!ELEMENT LOCALNAMESPACEPATH (NAMESPACE+)>
//
//------------------------------------------------------------------------------

void XmlWriter::appendLocalNameSpacePathElement(
    Array<char>& out,
    const CIMNamespaceName& nameSpace)
{
    out << "<LOCALNAMESPACEPATH>\n";

    char* nameSpaceCopy = strdup(nameSpace.getString().getCString());

#if defined(PEGASUS_PLATFORM_SOLARIS_SPARC_CC) || \
    defined(PEGASUS_OS_HPUX) || \
    defined(PEGASUS_OS_LINUX)
    char *last;
    for (const char* p = strtok_r(nameSpaceCopy, "/", &last); p;
         p = strtok_r(NULL, "/", &last))
#else
    for (const char* p = strtok(nameSpaceCopy, "/"); p; p = strtok(NULL, "/"))
#endif
    {
	out << "<NAMESPACE NAME=\"" << p << "\"/>\n";
    }
    free(nameSpaceCopy);

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
    Array<char>& out,
    const String& host,
    const CIMNamespaceName& nameSpace)
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
    Array<char>& out,
    const CIMName& className)
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
    Array<char>& out,
    const CIMObjectPath& instanceName)
{
    out << "<INSTANCENAME CLASSNAME=\"" << instanceName.getClassName() << "\">\n";

    Array<CIMKeyBinding> keyBindings = instanceName.getKeyBindings();
    for (Uint32 i = 0, n = keyBindings.size(); i < n; i++)
    {
        out << "<KEYBINDING NAME=\"" << keyBindings[i].getName() << "\">\n";

        if (keyBindings[i].getType() == CIMKeyBinding::REFERENCE)
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
    Array<char>& out,
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
    Array<char>& out,
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
    Array<char>& out,
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
    Array<char>& out,
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
    Array<char>& out,
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

inline void _xmlWritter_appendValue(Array<char>& out, Boolean x)
{
    XmlWriter::append(out, x);
}

inline void _xmlWritter_appendValue(Array<char>& out, Uint8 x)
{
    XmlWriter::append(out, Uint32(x));
}

inline void _xmlWritter_appendValue(Array<char>& out, Sint8 x)
{
    XmlWriter::append(out, Sint32(x));
}

inline void _xmlWritter_appendValue(Array<char>& out, Uint16 x)
{
    XmlWriter::append(out, Uint32(x));
}

inline void _xmlWritter_appendValue(Array<char>& out, Sint16 x)
{
    XmlWriter::append(out, Sint32(x));
}

inline void _xmlWritter_appendValue(Array<char>& out, Uint32 x)
{
    XmlWriter::append(out, x);
}

inline void _xmlWritter_appendValue(Array<char>& out, Sint32 x)
{
    XmlWriter::append(out, x);
}

inline void _xmlWritter_appendValue(Array<char>& out, Uint64 x)
{
    XmlWriter::append(out, x);
}

inline void _xmlWritter_appendValue(Array<char>& out, Sint64 x)
{
    XmlWriter::append(out, x);
}

inline void _xmlWritter_appendValue(Array<char>& out, Real32 x)
{
    XmlWriter::append(out, x);
}

inline void _xmlWritter_appendValue(Array<char>& out, Real64 x)
{
    XmlWriter::append(out, x);
}

inline void _xmlWritter_appendValue(Array<char>& out, const Char16& x)
{
    XmlWriter::appendSpecial(out, x);
}

inline void _xmlWritter_appendValue(Array<char>& out, const String& x)
{
    XmlWriter::appendSpecial(out, x);
}

inline void _xmlWritter_appendValue(Array<char>& out, const CIMDateTime& x)
{
    out << x.toString();  //ATTN: append() method?
}

inline void _xmlWritter_appendValue(Array<char>& out, const CIMObjectPath& x)
{
    XmlWriter::appendValueReferenceElement(out, x, true);
}

inline void _xmlWritter_appendValue(Array<char>& out, const CIMObject& x)
{
    String myStr = x.toString();
    _xmlWritter_appendValue(out, myStr);
}

void _xmlWritter_appendValueArray(Array<char>& out, const CIMObjectPath* p, Uint32 size)
{
    out << "<VALUE.REFARRAY>\n";
    while (size--)
    {
        _xmlWritter_appendValue(out, *p++);
    }
    out << "</VALUE.REFARRAY>\n";
}

template<class T>
void _xmlWritter_appendValueArray(Array<char>& out, const T* p, Uint32 size)
{
    out << "<VALUE.ARRAY>\n";

    while (size--)
    {
        out << "<VALUE>";
        _xmlWritter_appendValue(out, *p++);
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
    Array<char>& out,
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
                _xmlWritter_appendValueArray(out, a.getData(), a.size());
                break;
            }

            case CIMTYPE_UINT8:
            {
                Array<Uint8> a;
                value.get(a);
                _xmlWritter_appendValueArray(out, a.getData(), a.size());
                break;
            }

            case CIMTYPE_SINT8:
            {
                Array<Sint8> a;
                value.get(a);
                _xmlWritter_appendValueArray(out, a.getData(), a.size());
                break;
            }

            case CIMTYPE_UINT16:
            {
                Array<Uint16> a;
                value.get(a);
                _xmlWritter_appendValueArray(out, a.getData(), a.size());
                break;
            }

            case CIMTYPE_SINT16:
            {
                Array<Sint16> a;
                value.get(a);
                _xmlWritter_appendValueArray(out, a.getData(), a.size());
                break;
            }

            case CIMTYPE_UINT32:
            {
                Array<Uint32> a;
                value.get(a);
                _xmlWritter_appendValueArray(out, a.getData(), a.size());
                break;
            }

            case CIMTYPE_SINT32:
            {
                Array<Sint32> a;
                value.get(a);
                _xmlWritter_appendValueArray(out, a.getData(), a.size());
                break;
            }

            case CIMTYPE_UINT64:
            {
                Array<Uint64> a;
                value.get(a);
                _xmlWritter_appendValueArray(out, a.getData(), a.size());
                break;
            }

            case CIMTYPE_SINT64:
            {
                Array<Sint64> a;
                value.get(a);
                _xmlWritter_appendValueArray(out, a.getData(), a.size());
                break;
            }

            case CIMTYPE_REAL32:
            {
                Array<Real32> a;
                value.get(a);
                _xmlWritter_appendValueArray(out, a.getData(), a.size());
                break;
            }

            case CIMTYPE_REAL64:
            {
                Array<Real64> a;
                value.get(a);
                _xmlWritter_appendValueArray(out, a.getData(), a.size());
                break;
            }

            case CIMTYPE_CHAR16:
            {
                Array<Char16> a;
                value.get(a);
                _xmlWritter_appendValueArray(out, a.getData(), a.size());
                break;
            }

            case CIMTYPE_STRING:
            {
                Array<String> a;
                value.get(a);
                _xmlWritter_appendValueArray(out, a.getData(), a.size());
                break;
            }

            case CIMTYPE_DATETIME:
            {
                Array<CIMDateTime> a;
                value.get(a);
                _xmlWritter_appendValueArray(out, a.getData(), a.size());
                break;
            }

            case CIMTYPE_REFERENCE:
            {
                Array<CIMObjectPath> a;
                value.get(a);
                _xmlWritter_appendValueArray(out, a.getData(), a.size());
                break;
            }

            case CIMTYPE_OBJECT:
            {
                Array<CIMObject> a;
                value.get(a);
                _xmlWritter_appendValueArray(out, a.getData(), a.size());
                break;
            }

            default:
                PEGASUS_ASSERT(false);
        }
    }
    else if (value.getType() == CIMTYPE_REFERENCE)
    {
        // Has to be separate because it uses VALUE.REFERENCE tag
        CIMObjectPath v;
        value.get(v);
        _xmlWritter_appendValue(out, v);
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
                _xmlWritter_appendValue(out, v);
                break;
            }

            case CIMTYPE_UINT8:
            {
                Uint8 v;
                value.get(v);
                _xmlWritter_appendValue(out, v);
                break;
            }

            case CIMTYPE_SINT8:
            {
                Sint8 v;
                value.get(v);
                _xmlWritter_appendValue(out, v);
                break;
            }

            case CIMTYPE_UINT16:
            {
                Uint16 v;
                value.get(v);
                _xmlWritter_appendValue(out, v);
                break;
            }

            case CIMTYPE_SINT16:
            {
                Sint16 v;
                value.get(v);
                _xmlWritter_appendValue(out, v);
                break;
            }

            case CIMTYPE_UINT32:
            {
                Uint32 v;
                value.get(v);
                _xmlWritter_appendValue(out, v);
                break;
            }

            case CIMTYPE_SINT32:
            {
                Sint32 v;
                value.get(v);
                _xmlWritter_appendValue(out, v);
                break;
            }

            case CIMTYPE_UINT64:
            {
                Uint64 v;
                value.get(v);
                _xmlWritter_appendValue(out, v);
                break;
            }

            case CIMTYPE_SINT64:
            {
                Sint64 v;
                value.get(v);
                _xmlWritter_appendValue(out, v);
                break;
            }

            case CIMTYPE_REAL32:
            {
                Real32 v;
                value.get(v);
                _xmlWritter_appendValue(out, v);
                break;
            }

            case CIMTYPE_REAL64:
            {
                Real64 v;
                value.get(v);
                _xmlWritter_appendValue(out, v);
                break;
            }

            case CIMTYPE_CHAR16:
            {
                Char16 v;
                value.get(v);
                _xmlWritter_appendValue(out, v);
                break;
            }

            case CIMTYPE_STRING:
            {
                String v;
                value.get(v);
                _xmlWritter_appendValue(out, v);
                break;
            }

            case CIMTYPE_DATETIME:
            {
                CIMDateTime v;
                value.get(v);
                _xmlWritter_appendValue(out, v);
                break;
            }

            case CIMTYPE_OBJECT:
            {
                CIMObject v;
                value.get(v);
                _xmlWritter_appendValue(out, v);
                break;
            }

            default:
                PEGASUS_ASSERT(false);
        }

        out << "</VALUE>\n";
    }
}

void XmlWriter::printValueElement(
    const CIMValue& value,
    PEGASUS_STD(ostream)& os)
{
    Array<char> tmp;
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
    Array<char>& out,
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
    Array<char>& out,
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

    Array<CIMKeyBinding> kbs = reference.getKeyBindings();

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
    Array<char> tmp;
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
    Array<char>& out,
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
    Array<char>& out,
    const CIMConstClass& cimclass)
{
    cimclass._checkRep();
    cimclass._rep->toXml(out);
}

void XmlWriter::printClassElement(
    const CIMConstClass& cimclass,
    PEGASUS_STD(ostream)& os)
{
    Array<char> tmp;
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
    Array<char>& out,
    const CIMConstInstance& instance)
{
    instance._checkRep();
    instance._rep->toXml(out);
}

void XmlWriter::printInstanceElement(
    const CIMConstInstance& instance,
    PEGASUS_STD(ostream)& os)
{
    Array<char> tmp;
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
    Array<char>& out,
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
    Array<char>& out,
    const CIMConstProperty& property)
{
    property._checkRep();
    property._rep->toXml(out);
}

void XmlWriter::printPropertyElement(
    const CIMConstProperty& property,
    PEGASUS_STD(ostream)& os)
{
    Array<char> tmp;
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
    Array<char>& out,
    const CIMConstMethod& method)
{
    method._checkRep();
    method._rep->toXml(out);
}

void XmlWriter::printMethodElement(
    const CIMConstMethod& method,
    PEGASUS_STD(ostream)& os)
{
    Array<char> tmp;
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
    Array<char>& out,
    const CIMConstParameter& parameter)
{
    parameter._checkRep();
    parameter._rep->toXml(out);
}

void XmlWriter::printParameterElement(
    const CIMConstParameter& parameter,
    PEGASUS_STD(ostream)& os)
{
    Array<char> tmp;
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
    Array<char>& out,
    const CIMParamValue& paramValue)
{
    paramValue._checkRep();
    paramValue._rep->toXml(out);
}

void XmlWriter::printParamValueElement(
    const CIMParamValue& paramValue,
    PEGASUS_STD(ostream)& os)
{
    Array<char> tmp;
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
    Array<char>& out,
    const CIMConstQualifier& qualifier)
{
    qualifier._checkRep();
    qualifier._rep->toXml(out);
}

void XmlWriter::printQualifierElement(
    const CIMConstQualifier& qualifier,
    PEGASUS_STD(ostream)& os)
{
    Array<char> tmp;
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
    Array<char>& out,
    const CIMConstQualifierDecl& qualifierDecl)
{
    qualifierDecl._checkRep();
    qualifierDecl._rep->toXml(out);
}

void XmlWriter::printQualifierDeclElement(
    const CIMConstQualifierDecl& qualifierDecl,
    PEGASUS_STD(ostream)& os)
{
    Array<char> tmp;
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
    Array<char>& out,
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
    Array<char>& out,
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

// l10n - added content language and accept language support to
// the header methods below

//------------------------------------------------------------------------------
//
// appendMethodCallHeader()
//
//     Build HTTP method call request header.
//
//------------------------------------------------------------------------------

void XmlWriter::appendMethodCallHeader(
    Array<char>& out,
    const char* host,
    const CIMName& cimMethod,
    const String& cimObject,
    const String& authenticationHeader,
    HttpMethod httpMethod,
    const AcceptLanguages & acceptLanguages,
    const ContentLanguages & contentLanguages,
    Uint32 contentLength)
{
    char nn[] = { '0' + (rand() % 10), '0' + (rand() % 10), '\0' };

    // ATTN: KS 20020926 - Temporary change to issue only POST. This may
    // be changed in the DMTF CIM Operations standard in the future.
    // If we kept M-Post we would have to retry with Post. Does not
    // do that in client today. Permanent change is to retry until spec
    // updated. This change is temp to finish tests or until the retry
    // installed.  Required because of change to wbemservices cimom
    if (httpMethod == HTTP_METHOD_M_POST)
    {
        out << "M-POST /cimom HTTP/1.1\r\n";
    }
    else
    {
        out << "POST /cimom HTTP/1.1\r\n";
    }
    out << "HOST: " << host << "\r\n";
		out << "Content-Type: application/xml; charset=\"utf-8\"\r\n";
		OUTPUT_CONTENTLENGTH;
    if (acceptLanguages.size() > 0)
    {
    	out << "Accept-Language: " << acceptLanguages << "\r\n";
    }
    if (contentLanguages.size() > 0)
    {
    	out << "Content-Language: " << contentLanguages << "\r\n";
    }

#ifdef PEGASUS_DEBUG
		// backdoor environment variable to turn OFF client requesting transfer
		// encoding. The default is on. to turn off, set this variable to zero.
		// This should be removed when stable. This should only be turned off in
		// a debugging/testing environment.

		static const char *clientTransferEncodingOff =
			getenv("PEGASUS_HTTP_TRANSFER_ENCODING_REQUEST");
		if (!clientTransferEncodingOff || *clientTransferEncodingOff != '0')
#endif

			out << "TE: chunked, trailers" << "\r\n";

    if (httpMethod == HTTP_METHOD_M_POST)
    {
        out << "Man: http://www.dmtf.org/cim/mapping/http/v1.0; ns=";
        out << nn <<"\r\n";
        out << nn << "-CIMOperation: MethodCall\r\n";
        out << nn << "-CIMMethod: "
            << encodeURICharacters(cimMethod.getString()) << "\r\n";
        out << nn << "-CIMObject: " << encodeURICharacters(cimObject) << "\r\n";
    }
    else
    {
        out << "CIMOperation: MethodCall\r\n";
        out << "CIMMethod: " << encodeURICharacters(cimMethod.getString())
            << "\r\n";
        out << "CIMObject: " << encodeURICharacters(cimObject) << "\r\n";
    }

    if (authenticationHeader.size())
    {
        out << authenticationHeader << "\r\n";
    }

    out << "\r\n";
}


void XmlWriter::appendMethodResponseHeader(
     Array<char>& out,
     HttpMethod httpMethod,
     const ContentLanguages & contentLanguages,
     Uint32 contentLength,
     Uint32 serverResponseTime)
{
     char nn[] = { '0' + (rand() % 10), '0' + (rand() % 10), '\0' };
     out << "HTTP/1.1 " HTTP_STATUS_OK "\r\n";
     STAT_SERVERTIME
     out << "Content-Type: application/xml; charset=\"utf-8\"\r\n";
     OUTPUT_CONTENTLENGTH;

     if (contentLanguages.size() > 0)
     {
         out << "Content-Language: " << contentLanguages << "\r\n";
     }
     if (httpMethod == HTTP_METHOD_M_POST)
     {
         out << "Ext:\r\n";
         out << "Cache-Control: no-cache\r\n";
         out << "Man: http://www.dmtf.org/cim/mapping/http/v1.0; ns=";
         out << nn <<"\r\n";
         out << nn << "-CIMOperation: MethodResponse\r\n\r\n";
     }
     else
     {
         out << "CIMOperation: MethodResponse\r\n\r\n";
     }
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
    Array<char>& out,
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
        out << PEGASUS_HTTPHEADERTAG_ERRORDETAIL ": "
            << encodeURICharacters(errorDetail) << "\r\n";
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
    Array<char>& out,
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

#ifdef PEGASUS_KERBEROS_AUTHENTICATION
//------------------------------------------------------------------------------
//
// appendOKResponseHeader()
//
//     Build HTTP authentication response header for unauthorized requests.
//
//     Returns OK message in the following format:
//
//        HTTP/1.1 200 OK
//        Content-Length: 0
//        WWW-Authenticate: Negotiate "token"
//        <HTML><HEAD>
//        <TITLE>200 OK</TITLE>
//        </HEAD><BODY BGCOLOR="#99cc99">
//        <H2>TEST200 OK</H2>
//        <HR>
//        </BODY></HTML>
//
//------------------------------------------------------------------------------

void XmlWriter::appendOKResponseHeader(
    Array<char>& out,
    const String& content)
{
    out << "HTTP/1.1 " HTTP_STATUS_OK "\r\n";
    // Content-Length header needs to be added because 200 OK record
    // is usually intended to have content.  But, for Kerberos this
    // may not always be the case so we need to indicate that there
    // is no content
		Uint32 contentLength = 0;
		OUTPUT_CONTENTLENGTH;
    out << content << "\r\n";
    out << "\r\n";

//ATTN: We may need to include the following line, so that the browsers
//      can display the error message.
//    out << "<HTML><HEAD>\r\n";
//    out << "<TITLE>" << "200 OK" <<  "</TITLE>\r\n";
//    out << "</HEAD><BODY BGCOLOR=\"#99cc99\">\r\n";
//    out << "<H2>TEST" << "200 OK" << "</H2>\r\n";
//    out << "<HR>\r\n";
//    out << "</BODY></HTML>\r\n";
}
#endif

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
    Array<char>& out,
    const String& messageId)
{
    out << "<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n";
    out << "<CIM CIMVERSION=\"2.0\" DTDVERSION=\"2.0\">\n";
    out << "<MESSAGE ID=\"" << messageId << "\" PROTOCOLVERSION=\"1.0\">\n";
}

void XmlWriter::_appendMessageElementEnd(
    Array<char>& out)
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
    Array<char>& out)
{
    out << "<SIMPLEREQ>\n";
}

void XmlWriter::_appendSimpleReqElementEnd(
    Array<char>& out)
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
    Array<char>& out,
    const CIMName& name)
{
    out << "<METHODCALL NAME=\"" << name << "\">\n";
}

void XmlWriter::_appendMethodCallElementEnd(
    Array<char>& out)
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
    Array<char>& out,
    const CIMName& name)
{
    out << "<IMETHODCALL NAME=\"" << name << "\">\n";
}

void XmlWriter::_appendIMethodCallElementEnd(
    Array<char>& out)
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
    Array<char>& out,
    const char* name)
{
    out << "<IPARAMVALUE NAME=\"" << name << "\">\n";
}

void XmlWriter::_appendIParamValueElementEnd(
    Array<char>& out)
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
    Array<char>& out)
{
    out << "<SIMPLERSP>\n";
}

void XmlWriter::_appendSimpleRspElementEnd(
    Array<char>& out)
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
    Array<char>& out,
    const CIMName& name)
{
    out << "<METHODRESPONSE NAME=\"" << name << "\">\n";
}

void XmlWriter::_appendMethodResponseElementEnd(
    Array<char>& out)
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
    Array<char>& out,
    const CIMName& name)
{
    out << "<IMETHODRESPONSE NAME=\"" << name << "\">\n";
}

void XmlWriter::_appendIMethodResponseElementEnd(
    Array<char>& out)
{
    out << "</IMETHODRESPONSE>\n";
}

//------------------------------------------------------------------------------
//
// _appendErrorElement()
//
//------------------------------------------------------------------------------

void XmlWriter::_appendErrorElement(
    Array<char>& out,
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
//     %EmbeddedObject; #IMPLIED
//     %ParamType;>
//
//------------------------------------------------------------------------------

void XmlWriter::appendReturnValueElement(
    Array<char>& out,
    const CIMValue& value)
{
    out << "<RETURNVALUE";

    CIMType type = value.getType();
    // If the property type is CIMObject, then
    //   encode the property in CIM-XML as a string with the EMBEDDEDOBJECT attribute
    //   (there is not currently a CIM-XML "object" datatype)
    // else
    //   output the real type
    if (type == CIMTYPE_OBJECT)
    {
        out << " PARAMTYPE=\"string\"";
        out << " EMBEDDEDOBJECT=\"object\"";
    }
    else
    {
        out << " PARAMTYPE=\"" << cimTypeToString (type) << "\"";
    }

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
    Array<char>& out)
{
    out << "<IRETURNVALUE>\n";
}

void XmlWriter::_appendIReturnValueElementEnd(
    Array<char>& out)
{
    out << "</IRETURNVALUE>\n";
}

//------------------------------------------------------------------------------
//
// appendBooleanIParameter()
//
//------------------------------------------------------------------------------

void XmlWriter::appendBooleanIParameter(
    Array<char>& out,
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
    Array<char>& out,
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
    Array<char>& out,
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
    Array<char>& out,
    const char* name,
    const CIMName& className)
{
    _appendIParamValueElementBegin(out, name);

    //
    //  A NULL (unassigned) value for a parameter is specified by an
    //  <IPARAMVALUE> element with no subelement
    //
    if (!className.isNull ())
    {
        appendClassNameElement(out, className);
    }

    _appendIParamValueElementEnd(out);
}

//------------------------------------------------------------------------------
//
// appendInstanceNameIParameter()
//
//------------------------------------------------------------------------------

void XmlWriter::appendInstanceNameIParameter(
    Array<char>& out,
    const char* name,
    const CIMObjectPath& instanceName)
{
    _appendIParamValueElementBegin(out, name);
    appendInstanceNameElement(out, instanceName);
    _appendIParamValueElementEnd(out);
}

void XmlWriter::appendObjectNameIParameter(
    Array<char>& out,
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
    Array<char>& out,
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
    Array<char>& out,
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
    Array<char>& out,
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
    Array<char>& out,
    const CIMName& propertyName)
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
    Array<char>& out,
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
    Array<char>& out,
    const CIMPropertyList& propertyList)
{
    _appendIParamValueElementBegin(out, "PropertyList");

    //
    //  A NULL (unassigned) value for a parameter is specified by an
    //  <IPARAMVALUE> element with no subelement
    //
    if (!propertyList.isNull ())
    {
        out << "<VALUE.ARRAY>\n";
        for (Uint32 i = 0; i < propertyList.size(); i++)
        {
            out << "<VALUE>" << propertyList[i] << "</VALUE>\n";
        }
        out << "</VALUE.ARRAY>\n";
    }

    _appendIParamValueElementEnd(out);
}

//------------------------------------------------------------------------------
//
// appendQualifierDeclarationIParameter()
//
//------------------------------------------------------------------------------

void XmlWriter::appendQualifierDeclarationIParameter(
    Array<char>& out,
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

Array<char> XmlWriter::formatHttpErrorRspMessage(
    const String& status,
    const String& cimError,
    const String& errorDetail)
{
    Array<char> out;

    appendHttpErrorResponseHeader(out, status, cimError, errorDetail);

    return out;
}

// l10n - add content language support to the format methods below

//------------------------------------------------------------------------------
//
// XmlWriter::formatSimpleMethodReqMessage()
//
//------------------------------------------------------------------------------

// ATTN-RK-P1-20020228: Need to complete copy elimination optimization
Array<char> XmlWriter::formatSimpleMethodReqMessage(
    const char* host,
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& path,
    const CIMName& methodName,
    const Array<CIMParamValue>& parameters,
    const String& messageId,
    HttpMethod httpMethod,
    const String& authenticationHeader,
    const AcceptLanguages& httpAcceptLanguages,
    const ContentLanguages& httpContentLanguages)
{
    Array<char> out;
    Array<char> tmp;
    CIMObjectPath localObjectPath = path;
    localObjectPath.setNameSpace(nameSpace.getString());
    localObjectPath.setHost(String::EMPTY);

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
	localObjectPath.toString(),
        authenticationHeader,
        httpMethod,
        httpAcceptLanguages,
        httpContentLanguages,
	out.size());
    tmp << out;

    return tmp;
}

//PEP 128 adding serverRsponseTime to header
Array<char> XmlWriter::formatSimpleMethodRspMessage(
    const CIMName& methodName,
    const String& messageId,
    HttpMethod httpMethod,
    const ContentLanguages & httpContentLanguages,
    const Array<char>& body,
		Uint32 serverResponseTime,
		Boolean isFirst,
		Boolean isLast)
{
	Array<char> out;

	if (isFirst == true)
	{
		// NOTE: temporarily put zero for content length. the http code
		// will later decide to fill in the length or remove it altogether
		appendMethodResponseHeader(out, httpMethod, httpContentLanguages, 0,
															 serverResponseTime);
		_appendMessageElementBegin(out, messageId);
		_appendSimpleRspElementBegin(out);
		_appendMethodResponseElementBegin(out, methodName);
	}

	if (body.size() != 0)
	{
		out << body;
	}

	if (isLast == true)
	{
		_appendMethodResponseElementEnd(out);
		_appendSimpleRspElementEnd(out);
		_appendMessageElementEnd(out);
	}

	return out;
}



//------------------------------------------------------------------------------
//
// XmlWriter::formatSimpleMethodErrorRspMessage()
//
//------------------------------------------------------------------------------

Array<char> XmlWriter::formatSimpleMethodErrorRspMessage(
    const CIMName& methodName,
    const String& messageId,
    HttpMethod httpMethod,
    const CIMException& cimException)
{
    Array<char> out;
    Array<char> tmp;

    _appendMessageElementBegin(out, messageId);
    _appendSimpleRspElementBegin(out);
    _appendMethodResponseElementBegin(out, methodName);
    _appendErrorElement(out, cimException);
    _appendMethodResponseElementEnd(out);
    _appendSimpleRspElementEnd(out);
    _appendMessageElementEnd(out);

// l10n
    appendMethodResponseHeader(tmp,
    	httpMethod,
    	cimException.getContentLanguages(),
    	out.size());
    tmp << out;

    return tmp;
}

//------------------------------------------------------------------------------
//
// XmlWriter::formatSimpleIMethodReqMessage()
//
//------------------------------------------------------------------------------

Array<char> XmlWriter::formatSimpleIMethodReqMessage(
    const char* host,
    const CIMNamespaceName& nameSpace,
    const CIMName& iMethodName,
    const String& messageId,
    HttpMethod httpMethod,
    const String& authenticationHeader,
    const AcceptLanguages& httpAcceptLanguages,
    const ContentLanguages& httpContentLanguages,
    const Array<char>& body)
{
    Array<char> out;
    Array<char> tmp;

    _appendMessageElementBegin(out, messageId);
    _appendSimpleReqElementBegin(out);
    _appendIMethodCallElementBegin(out, iMethodName);
    appendLocalNameSpacePathElement(out, nameSpace.getString());
    out << body;
    _appendIMethodCallElementEnd(out);
    _appendSimpleReqElementEnd(out);
    _appendMessageElementEnd(out);

    appendMethodCallHeader(
	tmp,
	host,
	iMethodName,
	nameSpace.getString(),
        authenticationHeader,
        httpMethod,
        httpAcceptLanguages,
        httpContentLanguages,
	out.size());
    tmp << out;

    return tmp;
}

//------------------------------------------------------------------------------
//
// XmlWriter::formatSimpleIMethodRspMessage()
//
//------------------------------------------------------------------------------

Array<char> XmlWriter::formatSimpleIMethodRspMessage(
    const CIMName& iMethodName,
    const String& messageId,
    HttpMethod httpMethod,
    const ContentLanguages & httpContentLanguages,
    const Array<char>& body,
    Uint32 serverResponseTime,
    Boolean isFirst,
    Boolean isLast)
{
    Array<char> out;

		if (isFirst == true)
		{
			// NOTE: temporarily put zero for content length. the http code
			// will later decide to fill in the length or remove it altogether
			appendMethodResponseHeader(out, httpMethod, httpContentLanguages, 0,
																 serverResponseTime);
			_appendMessageElementBegin(out, messageId);
			_appendSimpleRspElementBegin(out);
			_appendIMethodResponseElementBegin(out, iMethodName);

			// output the start of the return tag. Test if there is response data by:
			// 1. there is data on the first chunk OR
			// 2. there is no data on the first chunk but isLast is false implying
			//    there is more non-empty data to come. If all subsequent chunks
			//    are empty, then this generates and empty response.
			if (body.size() != 0 || isLast == false)
				_appendIReturnValueElementBegin(out);
		}

    if (body.size() != 0)
    {
			out << body;
    }

		if (isLast == true)
		{
			if (body.size() != 0 || isFirst == false)
				_appendIReturnValueElementEnd(out);
			_appendIMethodResponseElementEnd(out);
			_appendSimpleRspElementEnd(out);
			_appendMessageElementEnd(out);
		}

    return out;
}


//------------------------------------------------------------------------------
//
// XmlWriter::formatSimpleIMethodErrorRspMessage()
//
//------------------------------------------------------------------------------

Array<char> XmlWriter::formatSimpleIMethodErrorRspMessage(
    const CIMName& iMethodName,
    const String& messageId,
    HttpMethod httpMethod,
    const CIMException& cimException)
{
    Array<char> out;
    Array<char> tmp;

    _appendMessageElementBegin(out, messageId);
    _appendSimpleRspElementBegin(out);
    _appendIMethodResponseElementBegin(out, iMethodName);
    _appendErrorElement(out, cimException);
    _appendIMethodResponseElementEnd(out);
    _appendSimpleRspElementEnd(out);
    _appendMessageElementEnd(out);

// l10n
    appendMethodResponseHeader(tmp,
    	 httpMethod,
    	 cimException.getContentLanguages(),
    	 out.size());
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
    Array<char>& out,
    const char* requestUri,
    const char* host,
    const CIMName& cimMethod,
    HttpMethod httpMethod,
    const String& authenticationHeader,
    const AcceptLanguages& acceptLanguages,
    const ContentLanguages& contentLanguages,
    Uint32 contentLength)
{
    char nn[] = { '0' + (rand() % 10), '0' + (rand() % 10), '\0' };

    if (httpMethod == HTTP_METHOD_M_POST)
    {
      out << "M-POST " << requestUri << " HTTP/1.1\r\n";
    }
    else
    {
      out << "POST " << requestUri << " HTTP/1.1\r\n";
    }
    out << "HOST: " << host << "\r\n";
    out << "Content-Type: application/xml; charset=\"utf-8\"\r\n";
		OUTPUT_CONTENTLENGTH;

    if (acceptLanguages.size() > 0)
    {
    	out << "Accept-Language: " << acceptLanguages << "\r\n";
    }
    if (contentLanguages.size() > 0)
    {
    	out << "Content-Language: " << contentLanguages << "\r\n";
    }

#ifdef PEGASUS_DEBUG
		// backdoor environment variable to turn OFF client requesting transfer
		// encoding. The default is on. to turn off, set this variable to zero.
		// This should be removed when stable. This should only be turned off in
		// a debugging/testing environment.

		static const char *clientTransferEncodingOff =
			getenv("PEGASUS_HTTP_TRANSFER_ENCODING_REQUEST");
		if (!clientTransferEncodingOff || *clientTransferEncodingOff != '0')
#endif
			out << "TE: chunked, trailers" << "\r\n";

    if (httpMethod == HTTP_METHOD_M_POST)
    {
			  out << "Man: http://www.dmtf.org/cim/mapping/http/v1.0; ns=";
        out << nn <<"\r\n";
        out << nn << "-CIMExport: MethodRequest\r\n";
        out << nn << "-CIMExportMethod: " << cimMethod << "\r\n";
    }
    else
    {
        out << "CIMExport: MethodRequest\r\n";
        out << "CIMExportMethod: " << cimMethod << "\r\n";
    }

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
    Array<char>& out,
    HttpMethod httpMethod,
    const ContentLanguages& contentLanguages,
    Uint32 contentLength)
{
    char nn[] = { '0' + (rand() % 10), '0' + (rand() % 10), '\0' };

    out << "HTTP/1.1 " HTTP_STATUS_OK "\r\n";
    out << "Content-Type: application/xml; charset=\"utf-8\"\r\n";
		OUTPUT_CONTENTLENGTH;

    if (contentLanguages.size() > 0)
    {
    	out << "Content-Language: " << contentLanguages << "\r\n";
    }
    if (httpMethod == HTTP_METHOD_M_POST)
    {
        out << "Ext:\r\n";
        out << "Cache-Control: no-cache\r\n";
        out << "Man: http://www.dmtf.org/cim/mapping/http/v1.0; ns=";
        out << nn <<"\r\n";
        out << nn << "-CIMExport: MethodResponse\r\n\r\n";
    }
    else
    {
        out << "CIMExport: MethodResponse\r\n\r\n";
    }
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
    Array<char>& out)
{
    out << "<SIMPLEEXPREQ>\n";
}

void XmlWriter::_appendSimpleExportReqElementEnd(
    Array<char>& out)
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
    Array<char>& out,
    const CIMName& name)
{
    out << "<EXPMETHODCALL NAME=\"" << name << "\">\n";
}

void XmlWriter::_appendEMethodCallElementEnd(
    Array<char>& out)
{
    out << "</EXPMETHODCALL>\n";
}

//------------------------------------------------------------------------------
//
// _appendEParamValueElementBegin()
// _appendEParamValueElementEnd()
//
//     <!ELEMENT EXPPARAMVALUE (INSTANCE)>
//     <!ATTLIST EXPPARAMVALUE
//         %CIMName;>
//
//------------------------------------------------------------------------------

void XmlWriter::_appendEParamValueElementBegin(
    Array<char>& out,
    const char* name)
{
    out << "<EXPPARAMVALUE NAME=\"" << name << "\">\n";
}

void XmlWriter::_appendEParamValueElementEnd(
    Array<char>& out)
{
    out << "</EXPPARAMVALUE>\n";
}

//------------------------------------------------------------------------------
//
// appendInstanceEParameter()
//
//------------------------------------------------------------------------------

void XmlWriter::appendInstanceEParameter(
    Array<char>& out,
    const char* name,
    const CIMInstance& instance)
{
    _appendEParamValueElementBegin(out, name);
    appendInstanceElement(out, instance);
    _appendEParamValueElementEnd(out);
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
    Array<char>& out)
{
    out << "<SIMPLEEXPRSP>\n";
}

void XmlWriter::_appendSimpleExportRspElementEnd(
    Array<char>& out)
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
    Array<char>& out,
    const CIMName& name)
{
    out << "<EXPMETHODRESPONSE NAME=\"" << name << "\">\n";
}

void XmlWriter::_appendEMethodResponseElementEnd(
    Array<char>& out)
{
    out << "</EXPMETHODRESPONSE>\n";
}

//------------------------------------------------------------------------------
//
// XmlWriter::formatSimpleEMethodReqMessage()
//
//------------------------------------------------------------------------------

Array<char> XmlWriter::formatSimpleEMethodReqMessage(
    const char* requestUri,
    const char* host,
    const CIMName& eMethodName,
    const String& messageId,
    HttpMethod httpMethod,
    const String& authenticationHeader,
    const AcceptLanguages& httpAcceptLanguages,
    const ContentLanguages& httpContentLanguages,
    const Array<char>& body)
{
    Array<char> out;
    Array<char> tmp;

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
        httpMethod,
        authenticationHeader,
        httpAcceptLanguages,
        httpContentLanguages,
	out.size());
    tmp << out;

    return tmp;
}

//------------------------------------------------------------------------------
//
// XmlWriter::formatSimpleEMethodRspMessage()
//
//------------------------------------------------------------------------------

Array<char> XmlWriter::formatSimpleEMethodRspMessage(
    const CIMName& eMethodName,
    const String& messageId,
    HttpMethod httpMethod,
    const ContentLanguages& httpContentLanguages,
    const Array<char>& body)
{
    Array<char> out;
    Array<char> tmp;

    _appendMessageElementBegin(out, messageId);
    _appendSimpleExportRspElementBegin(out);
    _appendEMethodResponseElementBegin(out, eMethodName);
    out << body;
    _appendEMethodResponseElementEnd(out);
    _appendSimpleExportRspElementEnd(out);
    _appendMessageElementEnd(out);

    appendEMethodResponseHeader(tmp,
    	httpMethod,
    	httpContentLanguages,
    	out.size());
    tmp << out;

    return tmp;
}

//------------------------------------------------------------------------------
//
// XmlWriter::formatSimpleEMethodErrorRspMessage()
//
//------------------------------------------------------------------------------

Array<char> XmlWriter::formatSimpleEMethodErrorRspMessage(
    const CIMName& eMethodName,
    const String& messageId,
    HttpMethod httpMethod,
    const CIMException& cimException)
{
    Array<char> out;
    Array<char> tmp;

    _appendMessageElementBegin(out, messageId);
    _appendSimpleExportRspElementBegin(out);
    _appendEMethodResponseElementBegin(out, eMethodName);
    _appendErrorElement(out, cimException);
    _appendEMethodResponseElementEnd(out);
    _appendSimpleExportRspElementEnd(out);
    _appendMessageElementEnd(out);

// l10n
    appendEMethodResponseHeader(tmp,
    	 httpMethod,
    	 cimException.getContentLanguages(),
	   	 out.size());
    tmp << out;

    return tmp;
}

//------------------------------------------------------------------------------
//
// _xmlWritter_printAttributes()
//
//------------------------------------------------------------------------------

void _xmlWritter_printAttributes(
    PEGASUS_STD(ostream)& os,
    const XmlAttribute* attributes,
    Uint32 attributeCount)
{
    for (Uint32 i = 0; i < attributeCount; i++)
    {
	os << attributes[i].name << "=";

	os << '"';
	_xmlWritter_appendSpecial(os, attributes[i].value);
	os << '"';

	if (i + 1 != attributeCount)
	    os << ' ';
    }
}

//------------------------------------------------------------------------------
//
// _xmlWritter_indent()
//
//------------------------------------------------------------------------------

void _xmlWritter_indent(PEGASUS_STD(ostream)& os, Uint32 level, Uint32 indentChars)
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
    AutoArrayPtr<char> tmp(strcpy(new char[strlen(text) + 1], text));

    XmlParser parser(tmp.get());
    XmlEntry entry;
    Stack<const char*> stack;

    while (parser.next(entry))
    {
	switch (entry.type)
	{
	    case XmlEntry::XML_DECLARATION:
	    {
		_xmlWritter_indent(os, stack.size(), indentChars);

		os << "<?" << entry.text << " ";
		_xmlWritter_printAttributes(os, entry.attributes, entry.attributeCount);
		os << "?>";
		break;
	    }

	    case XmlEntry::START_TAG:
	    {
		_xmlWritter_indent(os, stack.size(), indentChars);

		os << "<" << entry.text;

		if (entry.attributeCount)
		    os << ' ';

		_xmlWritter_printAttributes(os, entry.attributes, entry.attributeCount);
		os << ">";
		stack.push(entry.text);
		break;
	    }

	    case XmlEntry::EMPTY_TAG:
	    {
		_xmlWritter_indent(os, stack.size(), indentChars);

		os << "<" << entry.text << " ";
		_xmlWritter_printAttributes(os, entry.attributes, entry.attributeCount);
		os << "/>";
		break;
	    }

	    case XmlEntry::END_TAG:
	    {
		if (!stack.isEmpty() && strcmp(stack.top(), entry.text) == 0)
		    stack.pop();

		_xmlWritter_indent(os, stack.size(), indentChars);

		os << "</" << entry.text << ">";
		break;
	    }

	    case XmlEntry::COMMENT:
	    {

		_xmlWritter_indent(os, stack.size(), indentChars);
		os << "<!--";
		_xmlWritter_appendSpecial(os, entry.text);
		os << "-->";
		break;
	    }

	    case XmlEntry::CONTENT:
	    {
		_xmlWritter_indent(os, stack.size(), indentChars);
		_xmlWritter_appendSpecial(os, entry.text);
		break;
	    }

	    case XmlEntry::CDATA:
	    {
		_xmlWritter_indent(os, stack.size(), indentChars);
		os << "<![CDATA[...]]>";
		break;
	    }

	    case XmlEntry::DOCTYPE:
	    {
		_xmlWritter_indent(os, stack.size(), indentChars);
		os << "<!DOCTYPE...>";
		break;
	    }
	}

	os << PEGASUS_STD(endl);
    }

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
const char* XmlWriter::keyBindingTypeToString (CIMKeyBinding::Type type)
{
    switch (type)
    {
        case CIMKeyBinding::BOOLEAN:
            return "boolean";

        case CIMKeyBinding::STRING:
            return "string";

        case CIMKeyBinding::NUMERIC:
            return "numeric";

        case CIMKeyBinding::REFERENCE:
        default:
            PEGASUS_ASSERT(false);
    }

    return "unknown";
}

PEGASUS_NAMESPACE_END


