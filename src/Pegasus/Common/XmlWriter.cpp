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
#include "Buffer.h"
#include "StrLit.h"
#include "LanguageParser.h"
#include "IDFactory.h"
#include "StringConversion.h"

PEGASUS_NAMESPACE_BEGIN

// This is a shortcut macro for outputing content length. This
// pads the output number to the max characters representing a Uint32 number
// so that it can be overwritten easily with a transfer encoding line later
// on in HTTPConnection if required. This is strictly for performance since
// messages can be very large. This overwriting shortcut allows us to NOT have
// to repackage a large message later.

#define OUTPUT_CONTENTLENGTH                                               \
{                                                                          \
    char contentLengthP[11];                                               \
    int n = sprintf(contentLengthP,"%.10u", contentLength);                \
    out << STRLIT("content-length: ");                                     \
    out.append(contentLengthP, n);                                         \
    out << STRLIT("\r\n");                                                 \
}

////////////////////////////////////////////////////////////////////////////////
//
// SpecialChar and table.
//
////////////////////////////////////////////////////////////////////////////////

// Note: we cannot use StrLit here since it has a constructur (forbits
// structure initialization).

struct SpecialChar
{
    const char* str;
    Uint32 size;
};

// Defines encodings of special characters. Just use a 7-bit ASCII character
// as an index into this array to retrieve its string encoding and encoding
// length in bytes.
static const SpecialChar _specialChars[] =
{
    {STRLIT_ARGS("&#0;")},
    {STRLIT_ARGS("&#1;")},
    {STRLIT_ARGS("&#2;")},
    {STRLIT_ARGS("&#3;")},
    {STRLIT_ARGS("&#4;")},
    {STRLIT_ARGS("&#5;")},
    {STRLIT_ARGS("&#6;")},
    {STRLIT_ARGS("&#7;")},
    {STRLIT_ARGS("&#8;")},
    {STRLIT_ARGS("&#9;")},
    {STRLIT_ARGS("&#10;")},
    {STRLIT_ARGS("&#11;")},
    {STRLIT_ARGS("&#12;")},
    {STRLIT_ARGS("&#13;")},
    {STRLIT_ARGS("&#14;")},
    {STRLIT_ARGS("&#15;")},
    {STRLIT_ARGS("&#16;")},
    {STRLIT_ARGS("&#17;")},
    {STRLIT_ARGS("&#18;")},
    {STRLIT_ARGS("&#19;")},
    {STRLIT_ARGS("&#20;")},
    {STRLIT_ARGS("&#21;")},
    {STRLIT_ARGS("&#22;")},
    {STRLIT_ARGS("&#23;")},
    {STRLIT_ARGS("&#24;")},
    {STRLIT_ARGS("&#25;")},
    {STRLIT_ARGS("&#26;")},
    {STRLIT_ARGS("&#27;")},
    {STRLIT_ARGS("&#28;")},
    {STRLIT_ARGS("&#29;")},
    {STRLIT_ARGS("&#30;")},
    {STRLIT_ARGS("&#31;")},
    {STRLIT_ARGS(" ")},
    {STRLIT_ARGS("!")},
    {STRLIT_ARGS("&quot;")},
    {STRLIT_ARGS("#")},
    {STRLIT_ARGS("$")},
    {STRLIT_ARGS("%")},
    {STRLIT_ARGS("&amp;")},
    {STRLIT_ARGS("&apos;")},
    {STRLIT_ARGS("(")},
    {STRLIT_ARGS(")")},
    {STRLIT_ARGS("*")},
    {STRLIT_ARGS("+")},
    {STRLIT_ARGS(",")},
    {STRLIT_ARGS("-")},
    {STRLIT_ARGS(".")},
    {STRLIT_ARGS("/")},
    {STRLIT_ARGS("0")},
    {STRLIT_ARGS("1")},
    {STRLIT_ARGS("2")},
    {STRLIT_ARGS("3")},
    {STRLIT_ARGS("4")},
    {STRLIT_ARGS("5")},
    {STRLIT_ARGS("6")},
    {STRLIT_ARGS("7")},
    {STRLIT_ARGS("8")},
    {STRLIT_ARGS("9")},
    {STRLIT_ARGS(":")},
    {STRLIT_ARGS(";")},
    {STRLIT_ARGS("&lt;")},
    {STRLIT_ARGS("=")},
    {STRLIT_ARGS("&gt;")},
    {STRLIT_ARGS("?")},
    {STRLIT_ARGS("@")},
    {STRLIT_ARGS("A")},
    {STRLIT_ARGS("B")},
    {STRLIT_ARGS("C")},
    {STRLIT_ARGS("D")},
    {STRLIT_ARGS("E")},
    {STRLIT_ARGS("F")},
    {STRLIT_ARGS("G")},
    {STRLIT_ARGS("H")},
    {STRLIT_ARGS("I")},
    {STRLIT_ARGS("J")},
    {STRLIT_ARGS("K")},
    {STRLIT_ARGS("L")},
    {STRLIT_ARGS("M")},
    {STRLIT_ARGS("N")},
    {STRLIT_ARGS("O")},
    {STRLIT_ARGS("P")},
    {STRLIT_ARGS("Q")},
    {STRLIT_ARGS("R")},
    {STRLIT_ARGS("S")},
    {STRLIT_ARGS("T")},
    {STRLIT_ARGS("U")},
    {STRLIT_ARGS("V")},
    {STRLIT_ARGS("W")},
    {STRLIT_ARGS("X")},
    {STRLIT_ARGS("Y")},
    {STRLIT_ARGS("Z")},
    {STRLIT_ARGS("[")},
    {STRLIT_ARGS("\\")},
    {STRLIT_ARGS("]")},
    {STRLIT_ARGS("^")},
    {STRLIT_ARGS("_")},
    {STRLIT_ARGS("`")},
    {STRLIT_ARGS("a")},
    {STRLIT_ARGS("b")},
    {STRLIT_ARGS("c")},
    {STRLIT_ARGS("d")},
    {STRLIT_ARGS("e")},
    {STRLIT_ARGS("f")},
    {STRLIT_ARGS("g")},
    {STRLIT_ARGS("h")},
    {STRLIT_ARGS("i")},
    {STRLIT_ARGS("j")},
    {STRLIT_ARGS("k")},
    {STRLIT_ARGS("l")},
    {STRLIT_ARGS("m")},
    {STRLIT_ARGS("n")},
    {STRLIT_ARGS("o")},
    {STRLIT_ARGS("p")},
    {STRLIT_ARGS("q")},
    {STRLIT_ARGS("r")},
    {STRLIT_ARGS("s")},
    {STRLIT_ARGS("t")},
    {STRLIT_ARGS("u")},
    {STRLIT_ARGS("v")},
    {STRLIT_ARGS("w")},
    {STRLIT_ARGS("x")},
    {STRLIT_ARGS("y")},
    {STRLIT_ARGS("z")},
    {STRLIT_ARGS("{")},
    {STRLIT_ARGS("|")},
    {STRLIT_ARGS("}")},
    {STRLIT_ARGS("~")},
    {STRLIT_ARGS("&#127;")},
};

// If _isSpecialChar7[ch] is true, then ch is a special character, which must
// have a special encoding in XML. But only use 7-bit ASCII characters to
// index this array.
static const int _isSpecialChar7[] =
{
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,1,0,0,
    0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
};

////////////////////////////////////////////////////////////////////////////////

Buffer& operator<<(Buffer& out, const Char16& x)
{
    XmlWriter::append(out, x);
    return out;
}

Buffer& operator<<(Buffer& out, const String& x)
{
    XmlWriter::append(out, x);
    return out;
}

Buffer& operator<<(Buffer& out, const Indentor& x)
{
    XmlWriter::append(out, x);
    return out;
}

Buffer& operator<<(Buffer& out, const Buffer& x)
{
    out.append(x.getData(), x.size());
    return out;
}

Buffer& operator<<(Buffer& out, Uint32 x)
{
    XmlWriter::append(out, x);
    return out;
}

Buffer& operator<<(Buffer& out, const CIMName& name)
{
    XmlWriter::append(out, name.getString ());
    return out;
}


Buffer& operator<<(Buffer& out, const AcceptLanguageList& al)
{
    XmlWriter::append(out, LanguageParser::buildAcceptLanguageHeader(al));
    return out;
}

Buffer& operator<<(Buffer& out, const ContentLanguageList& cl)
{
    XmlWriter::append(out, LanguageParser::buildContentLanguageHeader(cl));
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

static void _xmlWritter_appendChar(Buffer& out, const Char16& c)
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

    UTF16toUTF8(
        &strsrc,
        endsrc,
        &strtgt,
        endtgt);

    out.append(str, UTF_8_COUNT_TRAIL_BYTES(str[0]) + 1);
}

inline void _appendSpecialChar7(Buffer& out, char c)
{
    if (_isSpecialChar7[int(c)])
        out.append(_specialChars[int(c)].str, _specialChars[int(c)].size);
    else
        out.append(c);
}

inline void _xmlWritter_appendSpecialChar(Buffer& out, const Char16& c)
{
    if (c < 128)
        _appendSpecialChar7(out, char(c));
    else
        _xmlWritter_appendChar(out, c);
}

static void _xmlWritter_appendSpecialChar(PEGASUS_STD(ostream)& os, char c)
{
    if ( ((c < 0x20) && (c >= 0)) || (c == 0x7f) )
    {
        char scratchBuffer[22];
        Uint32 outputLength;
        const char * output = Uint8ToString(scratchBuffer, 
                                            static_cast<Uint8>(c), 
                                            outputLength);
        os << "&#" << output << ";";
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

void _xmlWritter_appendSurrogatePair(Buffer& out, Uint16 high, Uint16 low)
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

    UTF16toUTF8(
        &strsrc,
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

// On windows sprintf outputs 3 digit precision exponent prepending
// zeros. Make it 2 digit precision if first digit is zero in the exponent.
#ifdef PEGASUS_OS_TYPE_WINDOWS
inline void _xmlWriter_normalizeRealValueString(char *str)
{
    // skip initial sign value...
    if (*str == '-' || *str == '+')
    {
        ++str;
    }
    while (*str && *str != '+' && *str != '-')
    {
        ++str;
    }
    if (*str && * ++str == '0')
    {
        *str = *(str+1);
        *(str+1) = *(str+2);
        *(str+2) = 0;
    }
}
#endif

void XmlWriter::append(Buffer& out, const Char16& x)
{
    _xmlWritter_appendChar(out, x);
}

void XmlWriter::append(Buffer& out, Boolean x)
{
    if (x)
        out.append(STRLIT_ARGS("TRUE"));
    else
        out.append(STRLIT_ARGS("FALSE"));
}

void XmlWriter::append(Buffer& out, Uint32 x)
{
    Uint32 outputLength=0;
    char buffer[22];
    const char * output = Uint32ToString(buffer, x, outputLength);
    out.append(output, outputLength);
}

void XmlWriter::append(Buffer& out, Sint32 x)
{
    Uint32 outputLength=0;
    char buffer[22];
    const char * output = Sint32ToString(buffer, x, outputLength);
    out.append(output, outputLength);
}

void XmlWriter::append(Buffer& out, Uint64 x)
{
    Uint32 outputLength=0;
    char buffer[22];
    const char * output = Uint64ToString(buffer, x, outputLength);
    out.append(output, outputLength);
}

void XmlWriter::append(Buffer& out, Sint64 x)
{
    Uint32 outputLength=0;
    char buffer[22];
    const char * output = Sint64ToString(buffer, x, outputLength);
    out.append(output, outputLength);
}

void XmlWriter::append(Buffer& out, Real32 x)
{
    char buffer[128];
    // %.7e gives '[-]m.ddddddde+/-xx', which seems compatible with the format
    // given in the CIM/XML spec, and the precision required by the CIM 2.2 spec
    // (4 byte IEEE floating point)
    sprintf(buffer, "%.7e", x);
#ifdef PEGASUS_OS_TYPE_WINDOWS
    _xmlWriter_normalizeRealValueString(buffer);
#endif
    append(out, buffer);
}

void XmlWriter::append(Buffer& out, Real64 x)
{
    char buffer[128];
    // %.16e gives '[-]m.dddddddddddddddde+/-xx', which seems compatible
    // with the format given in the CIM/XML spec, and the precision required
    // by the CIM 2.2 spec (8 byte IEEE floating point)
    sprintf(buffer, "%.16e", x);
#ifdef PEGASUS_OS_TYPE_WINDOWS
    _xmlWriter_normalizeRealValueString(buffer);
#endif
    append(out, buffer);
}

void XmlWriter::append(Buffer& out, const char* str)
{
    size_t n = strlen(str);
    out.append(str, n);
}

void XmlWriter::append(Buffer& out, const String& str)
{
    const Uint16* p = (const Uint16*)str.getChar16Data();
    size_t n = str.size();

    // Handle leading ASCII 7 characers in these next two loops (use unrolling).

    while (n >= 8 && ((p[0]|p[1]|p[2]|p[3]|p[4]|p[5]|p[6]|p[7]) & 0xFF80) == 0)
    {
        out.append(p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7]);
        p += 8;
        n -= 8;
    }

    while (n >= 4 && ((p[0]|p[1]|p[2]|p[3]) & 0xFF80) == 0)
    {
        out.append(p[0], p[1], p[2], p[3]);
        p += 4;
        n -= 4;
    }

    while (n--)
    {
        Uint16 c = *p++;

        // Special processing for UTF8 case:

        if (c < 128)
        {
            out.append(c);
            continue;
        }

        // Handle UTF8 case (if reached).

        if (((c >= FIRST_HIGH_SURROGATE) && (c <= LAST_HIGH_SURROGATE)) ||
            ((c >= FIRST_LOW_SURROGATE) && (c <= LAST_LOW_SURROGATE)))
        {
            Char16 highSurrogate = p[-1];
            Char16 lowSurrogate = p[0];
            p++;
            n--;

            _xmlWritter_appendSurrogatePair(
                out, Uint16(highSurrogate),Uint16(lowSurrogate));
        }
        else
        {
            _xmlWritter_appendChar(out, c);
        }
    }
}

void XmlWriter::append(Buffer& out, const Indentor& x)
{
    for (Uint32 i = 0; i < 4 * x.getLevel(); i++)
        out.append(' ');
}

void XmlWriter::appendSpecial(Buffer& out, const Char16& x)
{
    _xmlWritter_appendSpecialChar(out, x);
}

void XmlWriter::appendSpecial(Buffer& out, char x)
{
    _appendSpecialChar7(out, x);
}

void XmlWriter::appendSpecial(Buffer& out, const char* str)
{
    while (*str)
        _appendSpecialChar7(out, *str++);
}

void XmlWriter::appendSpecial(Buffer& out, const String& str)
{
    const Uint16* p = (const Uint16*)str.getChar16Data();
    // prevCharIsSpace is true when the last character written to the Buffer
    // is a space character (not a character reference).
    Boolean prevCharIsSpace = false;

    // If the first character is a space, use a character reference to avoid
    // space compression.
    if (*p == ' ')
    {
        out.append(STRLIT_ARGS("&#32;"));
        p++;
    }

    Uint16 c;
    while ((c = *p++) != 0)
    {
        if (c < 128)
        {
            if (_isSpecialChar7[c])
            {
                // Write the character reference for the special character
                out.append(
                    _specialChars[int(c)].str, _specialChars[int(c)].size);
                prevCharIsSpace = false;
            }
            else if (prevCharIsSpace && (c == ' '))
            {
                // Write the character reference for the space character, to
                // avoid compression
                out.append(STRLIT_ARGS("&#32;"));
                prevCharIsSpace = false;
            }
            else
            {
                out.append(c);
                prevCharIsSpace = (c == ' ');
            }
        }
        else
        {
            // Handle UTF8 case

            if ((((c >= FIRST_HIGH_SURROGATE) && (c <= LAST_HIGH_SURROGATE)) ||
                 ((c >= FIRST_LOW_SURROGATE) && (c <= LAST_LOW_SURROGATE))) &&
                *p)
            {
                _xmlWritter_appendSurrogatePair(out, c, *p++);
            }
            else
            {
                _xmlWritter_appendChar(out, c);
            }

            prevCharIsSpace = false;
        }
    }

    // If the last character is a space, use a character reference to avoid
    // space compression.
    if (prevCharIsSpace)
    {
        out.remove(out.size() - 1);
        out.append(STRLIT_ARGS("&#32;"));
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

static const char _is_uri[128] =
{
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,
    1,1,0,0,0,0,1,1,0,0,1,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,1,
};

// Perform the necessary URI encoding of characters in HTTP header values.
// This is required by the HTTP/1.1 specification and the CIM/HTTP
// Specification (section 3.3.2).
static void _xmlWritter_encodeURIChar(String& outString, Sint8 char8)
{
    Uint8 c = (Uint8)char8;

#ifndef PEGASUS_DO_NOT_IMPLEMENT_URI_ENCODING
    if (c > 127 || _is_uri[int(c)])
    {
        char hexencoding[4];
        int n = sprintf(hexencoding, "%%%X%X", c/16, c%16);
#ifdef PEGASUS_USE_STRING_EXTENSIONS
        outString.append(hexencoding, n);
#else /* PEGASUS_USE_STRING_EXTENSIONS */
        outString.append(hexencoding);
#endif /* PEGASUS_USE_STRING_EXTENSIONS */
    }
    else
#endif
    {
        outString.append((Uint16)c);
    }
}

String XmlWriter::encodeURICharacters(const Buffer& uriString)
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
    Buffer utf8;
    for (Uint32 i = 0; i < uriString.size(); i++)
    {
        Uint16 c = uriString[i];

        if (((c >= FIRST_HIGH_SURROGATE) && (c <= LAST_HIGH_SURROGATE)) ||
            ((c >= FIRST_LOW_SURROGATE) && (c <= LAST_LOW_SURROGATE)))
        {
            Char16 highSurrogate = uriString[i];
            Char16 lowSurrogate = uriString[++i];

            _xmlWritter_appendSurrogatePair(
                utf8, Uint16(highSurrogate),Uint16(lowSurrogate));
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
    Buffer& out,
    const CIMNamespaceName& nameSpace)
{
    out << STRLIT("<LOCALNAMESPACEPATH>\n");

    char* nameSpaceCopy = strdup(nameSpace.getString().getCString());

#if !defined(PEGASUS_COMPILER_MSVC) && !defined(PEGASUS_OS_ZOS)
    char *last;
    for (const char* p = strtok_r(nameSpaceCopy, "/", &last); p;
         p = strtok_r(NULL, "/", &last))
#else
    for (const char* p = strtok(nameSpaceCopy, "/"); p; p = strtok(NULL, "/"))
#endif
    {
        out << STRLIT("<NAMESPACE NAME=\"") << p << STRLIT("\"/>\n");
    }
    free(nameSpaceCopy);

    out << STRLIT("</LOCALNAMESPACEPATH>\n");
}

//------------------------------------------------------------------------------
//
// appendNameSpacePathElement()
//
//     <!ELEMENT NAMESPACEPATH (HOST,LOCALNAMESPACEPATH)>
//
//------------------------------------------------------------------------------

void XmlWriter::appendNameSpacePathElement(
    Buffer& out,
    const String& host,
    const CIMNamespaceName& nameSpace)
{
    out << STRLIT("<NAMESPACEPATH>\n");
    out << STRLIT("<HOST>") << host << STRLIT("</HOST>\n");
    appendLocalNameSpacePathElement(out, nameSpace);
    out << STRLIT("</NAMESPACEPATH>\n");
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
    Buffer& out,
    const CIMName& className)
{
    out << STRLIT("<CLASSNAME NAME=\"") << className << STRLIT("\"/>\n");
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
    Buffer& out,
    const CIMObjectPath& instanceName)
{
    out << STRLIT("<INSTANCENAME CLASSNAME=\"");
    out << instanceName.getClassName() << STRLIT("\">\n");

    const Array<CIMKeyBinding>& keyBindings = instanceName.getKeyBindings();
    for (Uint32 i = 0, n = keyBindings.size(); i < n; i++)
    {
        out << STRLIT("<KEYBINDING NAME=\"");
        out << keyBindings[i].getName() << STRLIT("\">\n");

        if (keyBindings[i].getType() == CIMKeyBinding::REFERENCE)
        {
            CIMObjectPath ref = keyBindings[i].getValue();
            appendValueReferenceElement(out, ref, true);
        }
        else
        {
            out << STRLIT("<KEYVALUE VALUETYPE=\"");
            out << keyBindingTypeToString(keyBindings[i].getType());
            out << STRLIT("\">");

            // fixed the special character problem - Markus

            appendSpecial(out, keyBindings[i].getValue());
            out << STRLIT("</KEYVALUE>\n");
        }
        out << STRLIT("</KEYBINDING>\n");
    }
    out << STRLIT("</INSTANCENAME>\n");
}

//------------------------------------------------------------------------------
//
// appendClassPathElement()
//
//     <!ELEMENT CLASSPATH (NAMESPACEPATH,CLASSNAME)>
//
//------------------------------------------------------------------------------

void XmlWriter::appendClassPathElement(
    Buffer& out,
    const CIMObjectPath& classPath)
{
    out << STRLIT("<CLASSPATH>\n");
    appendNameSpacePathElement(out,
                               classPath.getHost(),
                               classPath.getNameSpace());
    appendClassNameElement(out, classPath.getClassName());
    out << STRLIT("</CLASSPATH>\n");
}

//------------------------------------------------------------------------------
//
// appendInstancePathElement()
//
//     <!ELEMENT INSTANCEPATH (NAMESPACEPATH,INSTANCENAME)>
//
//------------------------------------------------------------------------------

void XmlWriter::appendInstancePathElement(
    Buffer& out,
    const CIMObjectPath& instancePath)
{
    out << STRLIT("<INSTANCEPATH>\n");
    appendNameSpacePathElement(out,
                               instancePath.getHost(),
                               instancePath.getNameSpace());
    appendInstanceNameElement(out, instancePath);
    out << STRLIT("</INSTANCEPATH>\n");
}

//------------------------------------------------------------------------------
//
// appendLocalClassPathElement()
//
//     <!ELEMENT LOCALCLASSPATH (LOCALNAMESPACEPATH, CLASSNAME)>
//
//------------------------------------------------------------------------------

void XmlWriter::appendLocalClassPathElement(
    Buffer& out,
    const CIMObjectPath& classPath)
{
    out << STRLIT("<LOCALCLASSPATH>\n");
    appendLocalNameSpacePathElement(out, classPath.getNameSpace());
    appendClassNameElement(out, classPath.getClassName());
    out << STRLIT("</LOCALCLASSPATH>\n");
}

//------------------------------------------------------------------------------
//
// appendLocalInstancePathElement()
//
//     <!ELEMENT LOCALINSTANCEPATH (LOCALNAMESPACEPATH, INSTANCENAME)>
//
//------------------------------------------------------------------------------

void XmlWriter::appendLocalInstancePathElement(
    Buffer& out,
    const CIMObjectPath& instancePath)
{
    out << STRLIT("<LOCALINSTANCEPATH>\n");
    appendLocalNameSpacePathElement(out, instancePath.getNameSpace());
    appendInstanceNameElement(out, instancePath);
    out << STRLIT("</LOCALINSTANCEPATH>\n");
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
    Buffer& out,
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

inline void _xmlWritter_appendValue(Buffer& out, Boolean x)
{
    XmlWriter::append(out, x);
}

inline void _xmlWritter_appendValue(Buffer& out, Uint8 x)
{
    XmlWriter::append(out, Uint32(x));
}

inline void _xmlWritter_appendValue(Buffer& out, Sint8 x)
{
    XmlWriter::append(out, Sint32(x));
}

inline void _xmlWritter_appendValue(Buffer& out, Uint16 x)
{
    XmlWriter::append(out, Uint32(x));
}

inline void _xmlWritter_appendValue(Buffer& out, Sint16 x)
{
    XmlWriter::append(out, Sint32(x));
}

inline void _xmlWritter_appendValue(Buffer& out, Uint32 x)
{
    XmlWriter::append(out, x);
}

inline void _xmlWritter_appendValue(Buffer& out, Sint32 x)
{
    XmlWriter::append(out, x);
}

inline void _xmlWritter_appendValue(Buffer& out, Uint64 x)
{
    XmlWriter::append(out, x);
}

inline void _xmlWritter_appendValue(Buffer& out, Sint64 x)
{
    XmlWriter::append(out, x);
}

inline void _xmlWritter_appendValue(Buffer& out, Real32 x)
{
    XmlWriter::append(out, x);
}

inline void _xmlWritter_appendValue(Buffer& out, Real64 x)
{
    XmlWriter::append(out, x);
}

inline void _xmlWritter_appendValue(Buffer& out, const Char16& x)
{
    XmlWriter::appendSpecial(out, x);
}

inline void _xmlWritter_appendValue(Buffer& out, const String& x)
{
    XmlWriter::appendSpecial(out, x);
}

inline void _xmlWritter_appendValue(Buffer& out, const CIMDateTime& x)
{
    // It is not necessary to use XmlWriter::appendSpecial(), because
    // CIMDateTime values do not contain special characters.
    out << x.toString();
}

inline void _xmlWritter_appendValue(Buffer& out, const CIMObjectPath& x)
{
    XmlWriter::appendValueReferenceElement(out, x, true);
}

inline void _xmlWritter_appendValue(Buffer& out, const CIMObject& x)
{
    String myStr = x.toString();
    _xmlWritter_appendValue(out, myStr);
}

void _xmlWritter_appendValueArray(
    Buffer& out, const CIMObjectPath* p, Uint32 size)
{
    out << STRLIT("<VALUE.REFARRAY>\n");
    while (size--)
    {
        _xmlWritter_appendValue(out, *p++);
    }
    out << STRLIT("</VALUE.REFARRAY>\n");
}

template<class T>
void _xmlWritter_appendValueArray(Buffer& out, const T* p, Uint32 size)
{
    out << STRLIT("<VALUE.ARRAY>\n");

    while (size--)
    {
        out << STRLIT("<VALUE>");
        _xmlWritter_appendValue(out, *p++);
        out << STRLIT("</VALUE>\n");
    }

    out << STRLIT("</VALUE.ARRAY>\n");
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
    Buffer& out,
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
                const String* data;
                Uint32 size;
                value._get(data, size);
                _xmlWritter_appendValueArray(out, data, size);
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
#ifdef PEGASUS_EMBEDDED_INSTANCE_SUPPORT
            case CIMTYPE_INSTANCE:
            {
                Array<CIMInstance> a;
                value.get(a);
                _xmlWritter_appendValueArray(out, a.getData(), a.size());
                break;
            }
#endif // PEGASUS_EMBEDDED_INSTANCE_SUPPORT
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
        out << STRLIT("<VALUE>");

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
#ifdef PEGASUS_EMBEDDED_INSTANCE_SUPPORT
            case CIMTYPE_INSTANCE:
            {
                CIMInstance v;
                value.get(v);
                _xmlWritter_appendValue(out, v);
                break;
            }
#endif // PEGASUS_EMBEDDED_INSTANCE_SUPPORT
            default:
                PEGASUS_ASSERT(false);
        }

        out << STRLIT("</VALUE>\n");
    }
}

void XmlWriter::printValueElement(
    const CIMValue& value,
    PEGASUS_STD(ostream)& os)
{
    Buffer tmp;
    appendValueElement(tmp, value);
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
    Buffer& out,
    const CIMObject& objectWithPath)
{
    out << STRLIT("<VALUE.OBJECTWITHPATH>\n");

    appendValueReferenceElement(out, objectWithPath.getPath (), false);
    appendObjectElement(out, objectWithPath);

    out << STRLIT("</VALUE.OBJECTWITHPATH>\n");
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
    Buffer& out,
    const CIMObjectPath& reference,
    Boolean putValueWrapper)
{
    if (putValueWrapper)
        out << STRLIT("<VALUE.REFERENCE>\n");

    // See if it is a class or instance reference (instance references have
    // key-bindings; class references do not).
    //
    //  ATTN-CAKG-P2-20020726:  The following condition does not correctly
    //  distinguish instanceNames from classNames in every case
    //  The instanceName of a singleton instance of a keyless class has no
    //  key bindings
    //

    const Array<CIMKeyBinding>& kbs = reference.getKeyBindings();

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
        out << STRLIT("</VALUE.REFERENCE>\n");
}

void XmlWriter::printValueReferenceElement(
    const CIMObjectPath& reference,
    PEGASUS_STD(ostream)& os)
{
    Buffer tmp;
    appendValueReferenceElement(tmp, reference, true);
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
    Buffer& out,
    const CIMInstance& namedInstance)
{
    out << STRLIT("<VALUE.NAMEDINSTANCE>\n");

    appendInstanceNameElement(out, namedInstance.getPath ());
    appendInstanceElement(out, namedInstance);

    out << STRLIT("</VALUE.NAMEDINSTANCE>\n");
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
    Buffer& out,
    const CIMConstClass& cimclass)
{
    CheckRep(cimclass._rep);
    cimclass._rep->toXml(out);
}

void XmlWriter::printClassElement(
    const CIMConstClass& cimclass,
    PEGASUS_STD(ostream)& os)
{
    Buffer tmp;
    appendClassElement(tmp, cimclass);
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
    Buffer& out,
    const CIMConstInstance& instance)
{
    CheckRep(instance._rep);
    instance._rep->toXml(out);
}

void XmlWriter::printInstanceElement(
    const CIMConstInstance& instance,
    PEGASUS_STD(ostream)& os)
{
    Buffer tmp;
    appendInstanceElement(tmp, instance);
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
    Buffer& out,
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
    Buffer& out,
    const CIMConstProperty& property)
{
    CheckRep(property._rep);
    property._rep->toXml(out);
}

void XmlWriter::printPropertyElement(
    const CIMConstProperty& property,
    PEGASUS_STD(ostream)& os)
{
    Buffer tmp;
    appendPropertyElement(tmp, property);
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
    Buffer& out,
    const CIMConstMethod& method)
{
    CheckRep(method._rep);
    method._rep->toXml(out);
}

void XmlWriter::printMethodElement(
    const CIMConstMethod& method,
    PEGASUS_STD(ostream)& os)
{
    Buffer tmp;
    appendMethodElement(tmp, method);
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
    Buffer& out,
    const CIMConstParameter& parameter)
{
    CheckRep(parameter._rep);
    parameter._rep->toXml(out);
}

void XmlWriter::printParameterElement(
    const CIMConstParameter& parameter,
    PEGASUS_STD(ostream)& os)
{
    Buffer tmp;
    appendParameterElement(tmp, parameter);
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
    Buffer& out,
    const CIMParamValue& paramValue)
{
    CheckRep(paramValue._rep);
    paramValue._rep->toXml(out);
}

void XmlWriter::printParamValueElement(
    const CIMParamValue& paramValue,
    PEGASUS_STD(ostream)& os)
{
    Buffer tmp;
    appendParamValueElement(tmp, paramValue);
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
    Buffer& out,
    const CIMConstQualifier& qualifier)
{
    CheckRep(qualifier._rep);
    qualifier._rep->toXml(out);
}

void XmlWriter::printQualifierElement(
    const CIMConstQualifier& qualifier,
    PEGASUS_STD(ostream)& os)
{
    Buffer tmp;
    appendQualifierElement(tmp, qualifier);
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
    Buffer& out,
    const CIMConstQualifierDecl& qualifierDecl)
{
    CheckRep(qualifierDecl._rep);
    qualifierDecl._rep->toXml(out);
}

void XmlWriter::printQualifierDeclElement(
    const CIMConstQualifierDecl& qualifierDecl,
    PEGASUS_STD(ostream)& os)
{
    Buffer tmp;
    appendQualifierDeclElement(tmp, qualifierDecl);
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
    Buffer& out,
    const CIMFlavor & flavor)
{
    if (!(flavor.hasFlavor (CIMFlavor::OVERRIDABLE)))
        out << STRLIT(" OVERRIDABLE=\"false\"");

    if (!(flavor.hasFlavor (CIMFlavor::TOSUBCLASS)))
        out << STRLIT(" TOSUBCLASS=\"false\"");

    if (flavor.hasFlavor (CIMFlavor::TOINSTANCE))
        out << STRLIT(" TOINSTANCE=\"true\"");

    if (flavor.hasFlavor (CIMFlavor::TRANSLATABLE))
        out << STRLIT(" TRANSLATABLE=\"true\"");
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
    Buffer& out,
    const CIMScope & scope)
{
    if (!(scope.equal (CIMScope ())))
    {
        out << STRLIT("<SCOPE");

        if (scope.hasScope (CIMScope::CLASS))
            out << STRLIT(" CLASS=\"true\"");

        if (scope.hasScope (CIMScope::ASSOCIATION))
            out << STRLIT(" ASSOCIATION=\"true\"");

        if (scope.hasScope (CIMScope::REFERENCE))
            out << STRLIT(" REFERENCE=\"true\"");

        if (scope.hasScope (CIMScope::PROPERTY))
            out << STRLIT(" PROPERTY=\"true\"");

        if (scope.hasScope (CIMScope::METHOD))
            out << STRLIT(" METHOD=\"true\"");

        if (scope.hasScope (CIMScope::PARAMETER))
            out << STRLIT(" PARAMETER=\"true\"");

        if (scope.hasScope (CIMScope::INDICATION))
            out << STRLIT(" INDICATION=\"true\"");

        out << STRLIT("/>");
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
    Buffer& out,
    const char* host,
    const CIMName& cimMethod,
    const String& cimObject,
    const String& authenticationHeader,
    HttpMethod httpMethod,
    const AcceptLanguageList& acceptLanguages,
    const ContentLanguageList& contentLanguages,
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
        out << STRLIT("M-POST /cimom HTTP/1.1\r\n");
    }
    else
    {
        out << STRLIT("POST /cimom HTTP/1.1\r\n");
    }
    out << STRLIT("HOST: ") << host << STRLIT("\r\n");
    out << STRLIT("Content-Type: application/xml; charset=\"utf-8\"\r\n");
    OUTPUT_CONTENTLENGTH;
    if (acceptLanguages.size() > 0)
    {
        out << STRLIT("Accept-Language: ") << acceptLanguages << STRLIT("\r\n");
    }
    if (contentLanguages.size() > 0)
    {
        out << STRLIT("Content-Language: ") << contentLanguages <<
            STRLIT("\r\n");
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

    out << STRLIT("TE: chunked, trailers\r\n");

    if (httpMethod == HTTP_METHOD_M_POST)
    {
        out << STRLIT("Man: http://www.dmtf.org/cim/mapping/http/v1.0; ns=");
        out << nn << STRLIT("\r\n");
        out << nn << STRLIT("-CIMOperation: MethodCall\r\n");
        out << nn << STRLIT("-CIMMethod: ")
            << encodeURICharacters(cimMethod.getString()) << STRLIT("\r\n");
        out << nn << STRLIT("-CIMObject: ") << encodeURICharacters(cimObject)
            << STRLIT("\r\n");
    }
    else
    {
        out << STRLIT("CIMOperation: MethodCall\r\n");
        out << STRLIT("CIMMethod: ")
            << encodeURICharacters(cimMethod.getString())
            << STRLIT("\r\n");
        out << STRLIT("CIMObject: ") << encodeURICharacters(cimObject)
            << STRLIT("\r\n");
    }

    if (authenticationHeader.size())
    {
        out << authenticationHeader << STRLIT("\r\n");
    }

    out << STRLIT("\r\n");
}


void XmlWriter::appendMethodResponseHeader(
     Buffer& out,
     HttpMethod httpMethod,
     const ContentLanguageList& contentLanguages,
     Uint32 contentLength,
     Uint64 serverResponseTime)
{
     char nn[] = { '0' + (rand() % 10), '0' + (rand() % 10), '\0' };
     out << STRLIT("HTTP/1.1 " HTTP_STATUS_OK "\r\n");

#ifndef PEGASUS_DISABLE_PERFINST
     if (StatisticalData::current()->copyGSD)
     {
         out << STRLIT("WBEMServerResponseTime: ") <<
             CIMValue(serverResponseTime).toString() << STRLIT("\r\n");
     }
#endif

     out << STRLIT("Content-Type: application/xml; charset=\"utf-8\"\r\n");
     OUTPUT_CONTENTLENGTH;

     if (contentLanguages.size() > 0)
     {
         out << STRLIT("Content-Language: ") << contentLanguages <<
             STRLIT("\r\n");
     }
     if (httpMethod == HTTP_METHOD_M_POST)
     {
         out << STRLIT("Ext:\r\n");
         out << STRLIT("Cache-Control: no-cache\r\n");
         out << STRLIT("Man: http://www.dmtf.org/cim/mapping/http/v1.0; ns=");
         out << nn << STRLIT("\r\n");
         out << nn << STRLIT("-CIMOperation: MethodResponse\r\n\r\n");
     }
     else
     {
         out << STRLIT("CIMOperation: MethodResponse\r\n\r\n");
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
    Buffer& out,
    const String& status,
    const String& cimError,
    const String& errorDetail)
{
    out << STRLIT("HTTP/1.1 ") << status << STRLIT("\r\n");
    if (cimError != String::EMPTY)
    {
        out << STRLIT("CIMError: ") << cimError << STRLIT("\r\n");
    }
    if (errorDetail != String::EMPTY)
    {
        out << STRLIT(PEGASUS_HTTPHEADERTAG_ERRORDETAIL ": ")
            << encodeURICharacters(errorDetail) << STRLIT("\r\n");
    }
    out << STRLIT("\r\n");
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
    Buffer& out,
    const String& content)
{
    out << STRLIT("HTTP/1.1 " HTTP_STATUS_UNAUTHORIZED "\r\n");
    Uint32 contentLength = 0;
    OUTPUT_CONTENTLENGTH;
    out << content << STRLIT("\r\n");
    out << STRLIT("\r\n");

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
    Buffer& out,
    const String& content)
{
    out << STRLIT("HTTP/1.1 " HTTP_STATUS_OK "\r\n");
    // Content-Length header needs to be added because 200 OK record
    // is usually intended to have content.  But, for Kerberos this
    // may not always be the case so we need to indicate that there
    // is no content
    Uint32 contentLength = 0;
    OUTPUT_CONTENTLENGTH;
    out << content << STRLIT("\r\n");
    out << STRLIT("\r\n");

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
    Buffer& out,
    const String& messageId)
{
    out << STRLIT("<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n");
    out << STRLIT("<CIM CIMVERSION=\"2.0\" DTDVERSION=\"2.0\">\n");
    out << STRLIT("<MESSAGE ID=\"") << messageId;
    out << STRLIT("\" PROTOCOLVERSION=\"1.0\">\n");
}

void XmlWriter::_appendMessageElementEnd(
    Buffer& out)
{
    out << STRLIT("</MESSAGE>\n");
    out << STRLIT("</CIM>\n");
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
    Buffer& out)
{
    out << STRLIT("<SIMPLEREQ>\n");
}

void XmlWriter::_appendSimpleReqElementEnd(
    Buffer& out)
{
    out << STRLIT("</SIMPLEREQ>\n");
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
    Buffer& out,
    const CIMName& name)
{
    out << STRLIT("<METHODCALL NAME=\"") << name << STRLIT("\">\n");
}

void XmlWriter::_appendMethodCallElementEnd(
    Buffer& out)
{
    out << STRLIT("</METHODCALL>\n");
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
    Buffer& out,
    const CIMName& name)
{
    out << STRLIT("<IMETHODCALL NAME=\"") << name << STRLIT("\">\n");
}

void XmlWriter::_appendIMethodCallElementEnd(
    Buffer& out)
{
    out << STRLIT("</IMETHODCALL>\n");
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
    Buffer& out,
    const char* name)
{
    out << STRLIT("<IPARAMVALUE NAME=\"") << name << STRLIT("\">\n");
}

void XmlWriter::_appendIParamValueElementEnd(
    Buffer& out)
{
    out << STRLIT("</IPARAMVALUE>\n");
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
    Buffer& out)
{
    out << STRLIT("<SIMPLERSP>\n");
}

void XmlWriter::_appendSimpleRspElementEnd(
    Buffer& out)
{
    out << STRLIT("</SIMPLERSP>\n");
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
    Buffer& out,
    const CIMName& name)
{
    out << STRLIT("<METHODRESPONSE NAME=\"") << name << STRLIT("\">\n");
}

void XmlWriter::_appendMethodResponseElementEnd(
    Buffer& out)
{
    out << STRLIT("</METHODRESPONSE>\n");
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
    Buffer& out,
    const CIMName& name)
{
    out << STRLIT("<IMETHODRESPONSE NAME=\"") << name << STRLIT("\">\n");
}

void XmlWriter::_appendIMethodResponseElementEnd(
    Buffer& out)
{
    out << STRLIT("</IMETHODRESPONSE>\n");
}

//------------------------------------------------------------------------------
//
// _appendErrorElement()
//
//------------------------------------------------------------------------------

void XmlWriter::_appendErrorElement(
    Buffer& out,
    const CIMException& cimException)
{
    Tracer::traceCIMException(TRC_XML_WRITER, Tracer::LEVEL2, cimException);

    out << STRLIT("<ERROR");
    out << STRLIT(" CODE=\"") << Uint32(cimException.getCode());
    out.append('"');

    String description = TraceableCIMException(cimException).getDescription();

    if (description != String::EMPTY)
    {
        out << STRLIT(" DESCRIPTION=\"");
        appendSpecial(out, description);
        out.append('"');
    }

    if (cimException.getErrorCount())
    {
        out << STRLIT(">");

        for (Uint32 i = 0, n = cimException.getErrorCount(); i < n; i++)
            appendInstanceElement(out, cimException.getError(i));

        out << STRLIT("</ERROR>");
    }
    else
        out << STRLIT("/>");
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
    Buffer& out,
    const CIMValue& value)
{
    out << STRLIT("<RETURNVALUE");

    CIMType type = value.getType();
    // If the property type is CIMObject, then
    //   encode the property in CIM-XML as a string with the EMBEDDEDOBJECT
    //   attribute (there is not currently a CIM-XML "object" datatype)
    // else
    //   output the real type
    if (type == CIMTYPE_OBJECT)
    {
        out << STRLIT(" PARAMTYPE=\"string\"");
        out << STRLIT(" EMBEDDEDOBJECT=\"object\"");
    }
#ifdef PEGASUS_EMBEDDED_INSTANCE_SUPPORT
    else if (type == CIMTYPE_INSTANCE)
    {
        out << STRLIT(" PARAMTYPE=\"string\"");
        out << STRLIT(" EMBEDDEDOBJECT=\"instance\"");
    }
#endif // PEGASUS_EMBEDDED_INSTANCE_SUPPORT
    else
    {
        out << STRLIT(" PARAMTYPE=\"") << cimTypeToString (type);
        out.append('"');
    }

    out << STRLIT(">\n");

    // Add value.
    appendValueElement(out, value);
    out << STRLIT("</RETURNVALUE>\n");
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
    Buffer& out)
{
    out << STRLIT("<IRETURNVALUE>\n");
}

void XmlWriter::_appendIReturnValueElementEnd(
    Buffer& out)
{
    out << STRLIT("</IRETURNVALUE>\n");
}

//------------------------------------------------------------------------------
//
// appendBooleanIParameter()
//
//------------------------------------------------------------------------------

void XmlWriter::appendBooleanIParameter(
    Buffer& out,
    const char* name,
    Boolean flag)
{
    _appendIParamValueElementBegin(out, name);
    out << STRLIT("<VALUE>");
    append(out, flag);
    out << STRLIT("</VALUE>\n");
    _appendIParamValueElementEnd(out);
}

//------------------------------------------------------------------------------
//
// appendStringIParameter()
//
//------------------------------------------------------------------------------

void XmlWriter::appendStringIParameter(
    Buffer& out,
    const char* name,
    const String& str)
{
    _appendIParamValueElementBegin(out, name);
    out << STRLIT("<VALUE>");
    appendSpecial(out, str);
    out << STRLIT("</VALUE>\n");
    _appendIParamValueElementEnd(out);
}

//------------------------------------------------------------------------------
//
// appendClassNameIParameter()
//
//------------------------------------------------------------------------------

void XmlWriter::appendClassNameIParameter(
    Buffer& out,
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
    Buffer& out,
    const char* name,
    const CIMObjectPath& instanceName)
{
    _appendIParamValueElementBegin(out, name);
    appendInstanceNameElement(out, instanceName);
    _appendIParamValueElementEnd(out);
}

void XmlWriter::appendObjectNameIParameter(
    Buffer& out,
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
    Buffer& out,
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
    Buffer& out,
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
    Buffer& out,
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
    Buffer& out,
    const CIMName& propertyName)
{
    _appendIParamValueElementBegin(out, "PropertyName");
    out << STRLIT("<VALUE>") << propertyName << STRLIT("</VALUE>\n");
    _appendIParamValueElementEnd(out);
}

//------------------------------------------------------------------------------
//
// appendPropertyValueIParameter()
//
//------------------------------------------------------------------------------

void XmlWriter::appendPropertyValueIParameter(
    Buffer& out,
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
    Buffer& out,
    const CIMPropertyList& propertyList)
{
    _appendIParamValueElementBegin(out, "PropertyList");

    //
    //  A NULL (unassigned) value for a parameter is specified by an
    //  <IPARAMVALUE> element with no subelement
    //
    if (!propertyList.isNull ())
    {
        out << STRLIT("<VALUE.ARRAY>\n");
        for (Uint32 i = 0; i < propertyList.size(); i++)
        {
            out << STRLIT("<VALUE>") << propertyList[i] << STRLIT("</VALUE>\n");
        }
        out << STRLIT("</VALUE.ARRAY>\n");
    }

    _appendIParamValueElementEnd(out);
}

//------------------------------------------------------------------------------
//
// appendQualifierDeclarationIParameter()
//
//------------------------------------------------------------------------------

void XmlWriter::appendQualifierDeclarationIParameter(
    Buffer& out,
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

Buffer XmlWriter::formatHttpErrorRspMessage(
    const String& status,
    const String& cimError,
    const String& errorDetail)
{
    Buffer out;

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
Buffer XmlWriter::formatSimpleMethodReqMessage(
    const char* host,
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& path,
    const CIMName& methodName,
    const Array<CIMParamValue>& parameters,
    const String& messageId,
    HttpMethod httpMethod,
    const String& authenticationHeader,
    const AcceptLanguageList& httpAcceptLanguages,
    const ContentLanguageList& httpContentLanguages)
{
    Buffer out;
    Buffer tmp;
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
Buffer XmlWriter::formatSimpleMethodRspMessage(
    const CIMName& methodName,
    const String& messageId,
    HttpMethod httpMethod,
    const ContentLanguageList& httpContentLanguages,
    const Buffer& body,
    Uint64 serverResponseTime,
    Boolean isFirst,
    Boolean isLast)
{
    Buffer out;

    if (isFirst == true)
    {
        // NOTE: temporarily put zero for content length. the http code
        // will later decide to fill in the length or remove it altogether
        appendMethodResponseHeader(
            out, httpMethod, httpContentLanguages, 0, serverResponseTime);
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

Buffer XmlWriter::formatSimpleMethodErrorRspMessage(
    const CIMName& methodName,
    const String& messageId,
    HttpMethod httpMethod,
    const CIMException& cimException)
{
    Buffer out;
    Buffer tmp;

    _appendMessageElementBegin(out, messageId);
    _appendSimpleRspElementBegin(out);
    _appendMethodResponseElementBegin(out, methodName);
    _appendErrorElement(out, cimException);
    _appendMethodResponseElementEnd(out);
    _appendSimpleRspElementEnd(out);
    _appendMessageElementEnd(out);

    appendMethodResponseHeader(
        tmp,
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

Buffer XmlWriter::formatSimpleIMethodReqMessage(
    const char* host,
    const CIMNamespaceName& nameSpace,
    const CIMName& iMethodName,
    const String& messageId,
    HttpMethod httpMethod,
    const String& authenticationHeader,
    const AcceptLanguageList& httpAcceptLanguages,
    const ContentLanguageList& httpContentLanguages,
    const Buffer& body)
{
    Buffer out;
    Buffer tmp;

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

Buffer XmlWriter::formatSimpleIMethodRspMessage(
    const CIMName& iMethodName,
    const String& messageId,
    HttpMethod httpMethod,
    const ContentLanguageList& httpContentLanguages,
    const Buffer& body,
    Uint64 serverResponseTime,
    Boolean isFirst,
    Boolean isLast)
{
    Buffer out;

    if (isFirst == true)
    {
        // NOTE: temporarily put zero for content length. the http code
        // will later decide to fill in the length or remove it altogether
        appendMethodResponseHeader(
            out, httpMethod, httpContentLanguages, 0, serverResponseTime);
        _appendMessageElementBegin(out, messageId);
        _appendSimpleRspElementBegin(out);
        _appendIMethodResponseElementBegin(out, iMethodName);

        // output the start of the return tag. Test if there is response data
        // by:
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

Buffer XmlWriter::formatSimpleIMethodErrorRspMessage(
    const CIMName& iMethodName,
    const String& messageId,
    HttpMethod httpMethod,
    const CIMException& cimException)
{
    Buffer out;
    Buffer tmp;

    _appendMessageElementBegin(out, messageId);
    _appendSimpleRspElementBegin(out);
    _appendIMethodResponseElementBegin(out, iMethodName);
    _appendErrorElement(out, cimException);
    _appendIMethodResponseElementEnd(out);
    _appendSimpleRspElementEnd(out);
    _appendMessageElementEnd(out);

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
    Buffer& out,
    const char* requestUri,
    const char* host,
    const CIMName& cimMethod,
    HttpMethod httpMethod,
    const String& authenticationHeader,
    const AcceptLanguageList& acceptLanguages,
    const ContentLanguageList& contentLanguages,
    Uint32 contentLength)
{
    char nn[] = { '0' + (rand() % 10), '0' + (rand() % 10), '\0' };

    if (httpMethod == HTTP_METHOD_M_POST)
    {
      out << STRLIT("M-POST ") << requestUri << STRLIT(" HTTP/1.1\r\n");
    }
    else
    {
      out << STRLIT("POST ") << requestUri << STRLIT(" HTTP/1.1\r\n");
    }
    out << STRLIT("HOST: ") << host << STRLIT("\r\n");
    out << STRLIT("Content-Type: application/xml; charset=\"utf-8\"\r\n");
    OUTPUT_CONTENTLENGTH;

    if (acceptLanguages.size() > 0)
    {
        out << STRLIT("Accept-Language: ") << acceptLanguages << STRLIT("\r\n");
    }
    if (contentLanguages.size() > 0)
    {
        out << STRLIT("Content-Language: ") << contentLanguages <<
            STRLIT("\r\n");
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
    out << STRLIT("TE: chunked, trailers\r\n");

    if (httpMethod == HTTP_METHOD_M_POST)
    {
        out << STRLIT("Man: http://www.dmtf.org/cim/mapping/http/v1.0; ns=");
        out << nn << STRLIT("\r\n");
        out << nn << STRLIT("-CIMExport: MethodRequest\r\n");
        out << nn << STRLIT("-CIMExportMethod: ") << cimMethod <<
            STRLIT("\r\n");
    }
    else
    {
        out << STRLIT("CIMExport: MethodRequest\r\n");
        out << STRLIT("CIMExportMethod: ") << cimMethod << STRLIT("\r\n");
    }

    if (authenticationHeader.size())
    {
        out << authenticationHeader << STRLIT("\r\n");
    }

    out << STRLIT("\r\n");
}

//------------------------------------------------------------------------------
//
// appendEMethodResponseHeader()
//
//     Build HTTP response header for export operation.
//
//------------------------------------------------------------------------------

void XmlWriter::appendEMethodResponseHeader(
    Buffer& out,
    HttpMethod httpMethod,
    const ContentLanguageList& contentLanguages,
    Uint32 contentLength)
{
    char nn[] = { '0' + (rand() % 10), '0' + (rand() % 10), '\0' };

    out << STRLIT("HTTP/1.1 " HTTP_STATUS_OK "\r\n");
    out << STRLIT("Content-Type: application/xml; charset=\"utf-8\"\r\n");
    OUTPUT_CONTENTLENGTH;

    if (contentLanguages.size() > 0)
    {
        out << STRLIT("Content-Language: ") << contentLanguages <<
            STRLIT("\r\n");
    }
    if (httpMethod == HTTP_METHOD_M_POST)
    {
        out << STRLIT("Ext:\r\n");
        out << STRLIT("Cache-Control: no-cache\r\n");
        out << STRLIT("Man: http://www.dmtf.org/cim/mapping/http/v1.0; ns=");
        out << nn << STRLIT("\r\n");
        out << nn << STRLIT("-CIMExport: MethodResponse\r\n\r\n");
    }
    else
    {
        out << STRLIT("CIMExport: MethodResponse\r\n\r\n");
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
    Buffer& out)
{
    out << STRLIT("<SIMPLEEXPREQ>\n");
}

void XmlWriter::_appendSimpleExportReqElementEnd(
    Buffer& out)
{
    out << STRLIT("</SIMPLEEXPREQ>\n");
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
    Buffer& out,
    const CIMName& name)
{
    out << STRLIT("<EXPMETHODCALL NAME=\"") << name << STRLIT("\">\n");
}

void XmlWriter::_appendEMethodCallElementEnd(
    Buffer& out)
{
    out << STRLIT("</EXPMETHODCALL>\n");
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
    Buffer& out,
    const char* name)
{
    out << STRLIT("<EXPPARAMVALUE NAME=\"") << name << STRLIT("\">\n");
}

void XmlWriter::_appendEParamValueElementEnd(
    Buffer& out)
{
    out << STRLIT("</EXPPARAMVALUE>\n");
}

//------------------------------------------------------------------------------
//
// appendInstanceEParameter()
//
//------------------------------------------------------------------------------

void XmlWriter::appendInstanceEParameter(
    Buffer& out,
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
    Buffer& out)
{
    out << STRLIT("<SIMPLEEXPRSP>\n");
}

void XmlWriter::_appendSimpleExportRspElementEnd(
    Buffer& out)
{
    out << STRLIT("</SIMPLEEXPRSP>\n");
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
    Buffer& out,
    const CIMName& name)
{
    out << STRLIT("<EXPMETHODRESPONSE NAME=\"") << name << STRLIT("\">\n");
}

void XmlWriter::_appendEMethodResponseElementEnd(
    Buffer& out)
{
    out << STRLIT("</EXPMETHODRESPONSE>\n");
}

//------------------------------------------------------------------------------
//
// XmlWriter::formatSimpleEMethodReqMessage()
//
//------------------------------------------------------------------------------

Buffer XmlWriter::formatSimpleEMethodReqMessage(
    const char* requestUri,
    const char* host,
    const CIMName& eMethodName,
    const String& messageId,
    HttpMethod httpMethod,
    const String& authenticationHeader,
    const AcceptLanguageList& httpAcceptLanguages,
    const ContentLanguageList& httpContentLanguages,
    const Buffer& body)
{
    Buffer out;
    Buffer tmp;

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

Buffer XmlWriter::formatSimpleEMethodRspMessage(
    const CIMName& eMethodName,
    const String& messageId,
    HttpMethod httpMethod,
    const ContentLanguageList& httpContentLanguages,
    const Buffer& body)
{
    Buffer out;
    Buffer tmp;

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

Buffer XmlWriter::formatSimpleEMethodErrorRspMessage(
    const CIMName& eMethodName,
    const String& messageId,
    HttpMethod httpMethod,
    const CIMException& cimException)
{
    Buffer out;
    Buffer tmp;

    _appendMessageElementBegin(out, messageId);
    _appendSimpleExportRspElementBegin(out);
    _appendEMethodResponseElementBegin(out, eMethodName);
    _appendErrorElement(out, cimException);
    _appendEMethodResponseElementEnd(out);
    _appendSimpleExportRspElementEnd(out);
    _appendMessageElementEnd(out);

    appendEMethodResponseHeader(
        tmp,
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

void _xmlWritter_indent(
    PEGASUS_STD(ostream)& os,
    Uint32 level,
    Uint32 indentChars)
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
                _xmlWritter_printAttributes(
                    os, entry.attributes, entry.attributeCount);
                os << "?>";
                break;
            }

            case XmlEntry::START_TAG:
            {
                _xmlWritter_indent(os, stack.size(), indentChars);

                os << "<" << entry.text;

                if (entry.attributeCount)
                    os << ' ';

                _xmlWritter_printAttributes(
                    os, entry.attributes, entry.attributeCount);
                os << ">";
                stack.push(entry.text);
                break;
            }

            case XmlEntry::EMPTY_TAG:
            {
                _xmlWritter_indent(os, stack.size(), indentChars);

                os << "<" << entry.text << " ";
                _xmlWritter_printAttributes(
                    os, entry.attributes, entry.attributeCount);
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
                os << "<![CDATA[" << entry.text << "]]>";
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

static IDFactory _messageIDFactory(1000);

String XmlWriter::getNextMessageId()
{
    char scratchBuffer[22];
    Uint32 n;
    const char * startP = Uint32ToString(scratchBuffer, 
                                         _messageIDFactory.getID(),
                                         n);
    return String(startP, n);
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
