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
// Modified By: Carol Ann Krug Graves, Hewlett-Packard Company
//                  (carolann_graves@hp.com)
//              Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////
#include <Pegasus/Common/Config.h>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#if defined(PEGASUS_OS_TYPE_UNIX)
#include <errno.h>
#endif
#include "CIMName.h"
#include "XmlReader.h"
#include "XmlWriter.h"
#include "CIMQualifier.h"
#include "CIMQualifierDecl.h"
#include "CIMClass.h"
#include "CIMInstance.h"
#include "CIMObject.h"
#include "CIMParamValue.h"

//#define PEGASUS_SINT64_MIN (-PEGASUS_SINT64_LITERAL(9223372036854775808))
//#define PEGASUS_UINT64_MAX PEGASUS_UINT64_LITERAL(18446744073709551615)
#define PEGASUS_SINT64_MIN (-PEGASUS_SINT64_LITERAL((Sint64) 0x8000000000000000))
#define PEGASUS_UINT64_MAX PEGASUS_UINT64_LITERAL((Uint64) 0xFFFFFFFFFFFFFFFF)

PEGASUS_USING_STD;
PEGASUS_NAMESPACE_BEGIN

static const Uint32 MESSAGE_SIZE = 128;

//------------------------------------------------------------------------------
//
// getXmlDeclaration()
//
//     <?xml version="1.0" encoding="utf-8"?>
//
//------------------------------------------------------------------------------

void XmlReader::getXmlDeclaration(
    XmlParser& parser, 
    const char*& xmlVersion,
    const char*& xmlEncoding)
{
    XmlEntry entry;

    if (!parser.next(entry) ||
	entry.type != XmlEntry::XML_DECLARATION ||
	strcmp(entry.text, "xml") != 0)
    {
	throw XmlValidationError(parser.getLine(),
	    "Expected <?xml ... ?> style declaration");
    }

    if (!entry.getAttributeValue("version", xmlVersion))
	throw XmlValidationError(
	    parser.getLine(), "missing xml.version attribute");

    if (!entry.getAttributeValue("encoding", xmlEncoding))
    {
        // ATTN-RK-P3-20020403:  Is there a default encoding?
    }
}

//------------------------------------------------------------------------------
//
//  testXmlDeclaration ()
//
//------------------------------------------------------------------------------

Boolean XmlReader::testXmlDeclaration (
    XmlParser& parser,
    XmlEntry& entry)
{
    if (!parser.next (entry) ||
        entry.type != XmlEntry::XML_DECLARATION ||
        strcmp (entry.text, "xml") != 0)
    {
        parser.putBack (entry);
        return false;
    }

    return true;
}

//------------------------------------------------------------------------------
//
// expectStartTag()
//
//------------------------------------------------------------------------------

void XmlReader::expectStartTag(
    XmlParser& parser, 
    XmlEntry& entry,
    const char* tagName)
{
    if (!parser.next(entry) ||
	entry.type != XmlEntry::START_TAG ||
	strcmp(entry.text, tagName) != 0)
    {
	char message[MESSAGE_SIZE];
	sprintf(message, "Expected open of %s element", tagName);
	throw XmlValidationError(parser.getLine(), message);
    }
}

//------------------------------------------------------------------------------
//
// expectEndTag()
//
//------------------------------------------------------------------------------

void XmlReader::expectEndTag(XmlParser& parser, const char* tagName)
{
    XmlEntry entry;

    if (!parser.next(entry) ||
	entry.type != XmlEntry::END_TAG ||
	strcmp(entry.text, tagName) != 0)
    {
	char message[MESSAGE_SIZE];
	sprintf(message, "Expected close of %s element, got %s instead",
              tagName,entry.text);
	throw XmlValidationError(parser.getLine(), message);
    }
}

//------------------------------------------------------------------------------
//
// expectStartTagOrEmptyTag()
//
//------------------------------------------------------------------------------

void XmlReader::expectStartTagOrEmptyTag(
    XmlParser& parser, 
    XmlEntry& entry,
    const char* tagName)
{
    if (!parser.next(entry) ||
	(entry.type != XmlEntry::START_TAG &&
	entry.type != XmlEntry::EMPTY_TAG) ||
	strcmp(entry.text, tagName) != 0)
    {
	char message[MESSAGE_SIZE];
	sprintf(message, 
	    "Expected either open or open/close %s element", tagName);
	throw XmlValidationError(parser.getLine(), message);
    }
}

//------------------------------------------------------------------------------
//
// expectContentOrCData()
//
//------------------------------------------------------------------------------

Boolean XmlReader::expectContentOrCData(
    XmlParser& parser, 
    XmlEntry& entry)
{
    if (!parser.next(entry) ||
	(entry.type != XmlEntry::CONTENT &&
	entry.type != XmlEntry::CDATA))
    {
	throw XmlValidationError(parser.getLine(),
	    "Expected content of CDATA");
    }

    return true;
}

//------------------------------------------------------------------------------
//
// testStartTag()
//
//------------------------------------------------------------------------------

Boolean XmlReader::testStartTag(
    XmlParser& parser, 
    XmlEntry& entry,
    const char* tagName)
{
    if (!parser.next(entry) ||
	entry.type != XmlEntry::START_TAG ||
	strcmp(entry.text, tagName) != 0)
    {
	parser.putBack(entry);
	return false;
    }

    return true;
}

//------------------------------------------------------------------------------
//
// testEndTag()
//
//------------------------------------------------------------------------------

Boolean XmlReader::testEndTag(XmlParser& parser, const char* tagName)
{
    XmlEntry entry;

    if (!parser.next(entry) ||
	entry.type != XmlEntry::END_TAG ||
	strcmp(entry.text, tagName) != 0)
    {
	parser.putBack(entry);
	return false;
    }

    return true;
}

//------------------------------------------------------------------------------
//
// testStartTagOrEmptyTag()
//
//------------------------------------------------------------------------------

Boolean XmlReader::testStartTagOrEmptyTag(
    XmlParser& parser, 
    XmlEntry& entry,
    const char* tagName)
{
    if (!parser.next(entry) ||
	(entry.type != XmlEntry::START_TAG &&
	entry.type != XmlEntry::EMPTY_TAG) ||
	strcmp(entry.text, tagName) != 0)
    {
	parser.putBack(entry);
	return false;
    }

    return true;
}

//------------------------------------------------------------------------------
//
// testContentOrCData()
//
//------------------------------------------------------------------------------

Boolean XmlReader::testContentOrCData(
    XmlParser& parser, 
    XmlEntry& entry)
{
    if (!parser.next(entry) ||
	(entry.type != XmlEntry::CONTENT &&
	entry.type != XmlEntry::CDATA))
    {
	parser.putBack(entry);
	return false;
    }

    return true;
}

//------------------------------------------------------------------------------
//
// getCimStartTag()
//
//     <!ELEMENT CIM (MESSAGE|DECLARATION)>
//     <!ATTRLIST CIM 
//         CIMVERSION CDATA #REQUIRED
//         DTDVERSION CDATA #REQUIRED>
//
//------------------------------------------------------------------------------

void XmlReader::getCimStartTag(
    XmlParser& parser, 
    const char*& cimVersion,
    const char*& dtdVersion)
{
    XmlEntry entry;
    XmlReader::expectStartTag(parser, entry, "CIM");

    if (!entry.getAttributeValue("CIMVERSION", cimVersion))
	throw XmlValidationError(
	    parser.getLine(), "missing CIM.CIMVERSION attribute");

    if (!entry.getAttributeValue("DTDVERSION", dtdVersion))
	throw XmlValidationError(
	    parser.getLine(), "missing CIM.DTDVERSION attribute");
}

//------------------------------------------------------------------------------
//
// getCimNameAttribute()
//
//     <!ENTITY % CIMName "NAME CDATA #REQUIRED">
//
//------------------------------------------------------------------------------

String XmlReader::getCimNameAttribute(
    Uint32 lineNumber, 
    const XmlEntry& entry,
    const char* elementName,
    Boolean acceptNull)
{
    String name;

    if (!entry.getAttributeValue("NAME", name))
    {
	char buffer[MESSAGE_SIZE];
	sprintf(buffer, "missing %s.NAME attribute", elementName);
	throw XmlValidationError(lineNumber, buffer);
    }

    if (acceptNull && name.size() == 0)
	return name;

    if (!CIMName::legal(name))
    {
	char buffer[MESSAGE_SIZE];
	sprintf(buffer, "Illegal value for %s.NAME attribute", elementName);
	throw XmlSemanticError(lineNumber, buffer);
    }

    return name;
}

//------------------------------------------------------------------------------
//
// getClassNameAttribute()
//
//     <!ENTITY % CIMName "CLASSNAME CDATA #REQUIRED">
//
//------------------------------------------------------------------------------

String XmlReader::getClassNameAttribute(
    Uint32 lineNumber, 
    const XmlEntry& entry,
    const char* elementName)
{
    String name;

    if (!entry.getAttributeValue("CLASSNAME", name))
    {
	char buffer[MESSAGE_SIZE];
	sprintf(buffer, "missing %s.CLASSNAME attribute", elementName);
	throw XmlValidationError(lineNumber, buffer);
    }

    if (!CIMName::legal(name))
    {
	char buffer[MESSAGE_SIZE];
	sprintf(buffer, 
	    "Illegal value for %s.CLASSNAME attribute", elementName);
	throw XmlSemanticError(lineNumber, buffer);
    }

    return name;
}

//------------------------------------------------------------------------------
//
// getClassOriginAttribute()
//
//     <!ENTITY % ClassOrigin "CLASSORIGIN CDATA #IMPLIED">
//
//------------------------------------------------------------------------------

CIMName XmlReader::getClassOriginAttribute(
    Uint32 lineNumber, 
    const XmlEntry& entry,
    const char* tagName)
{
    String name;

    if (!entry.getAttributeValue("CLASSORIGIN", name))
	return CIMName();

    if (!CIMName::legal(name))
    {
	char buffer[MESSAGE_SIZE];
	sprintf(buffer, 
	    "Illegal value for %s.CLASSORIGIN attribute", tagName);
	throw XmlSemanticError(lineNumber, buffer);
    }

    return name;
}

//------------------------------------------------------------------------------
//
// getReferenceClassAttribute()
//
//     <!ENTITY % ReferenceClass "REFERENCECLASS CDATA #IMPLIED">
//
//------------------------------------------------------------------------------

CIMName XmlReader::getReferenceClassAttribute(
    Uint32 lineNumber, 
    const XmlEntry& entry,
    const char* elementName)
{
    String name;

    if (!entry.getAttributeValue("REFERENCECLASS", name))
	return CIMName();

    if (!CIMName::legal(name))
    {
	char buffer[MESSAGE_SIZE];
	sprintf(buffer, 
	    "Illegal value for %s.REFERENCECLASS attribute", elementName);
	throw XmlSemanticError(lineNumber, buffer);
    }

    return name;
}

//------------------------------------------------------------------------------
//
// getSuperClassAttribute()
//
//     <!ENTITY % SuperClass "SUPERCLASS CDATA #IMPLIED">
//
//------------------------------------------------------------------------------

CIMName XmlReader::getSuperClassAttribute(
    Uint32 lineNumber, 
    const XmlEntry& entry,
    const char* tagName)
{
    String superClass;

    if (!entry.getAttributeValue("SUPERCLASS", superClass))
	return CIMName();

    if (!CIMName::legal(superClass))
    {
	char buffer[MESSAGE_SIZE];
	sprintf(
	    buffer, "Illegal value for %s.SUPERCLASS attribute", tagName);
	throw XmlSemanticError(lineNumber, buffer);
    }

    return superClass;
}

//------------------------------------------------------------------------------
//
// getCimTypeAttribute()
//
// This method can be used to get a TYPE attribute or a PARAMTYPE attribute.
// The only significant difference is that PARAMTYPE may specify a value of
// "reference" type.  This method recognizes these attributes by name, and
// does not allow a "TYPE" attribute to be of "reference" type.
//
//     <!ENTITY % CIMType "TYPE (boolean|string|char16|uint8|sint8|uint16
//         |sint16|uint32|sint32|uint64|sint64|datetime|real32|real64)">
//
//     <!ENTITY % ParamType "PARAMTYPE (boolean|string|char16|uint8|sint8
//         |uint16|sint16|uint32|sint32|uint64|sint64|datetime|real32|real64
//         |reference)">
//
//------------------------------------------------------------------------------

Boolean XmlReader::getCimTypeAttribute(
    Uint32 lineNumber, 
    const XmlEntry& entry, 
    CIMType& cimType,  // Output parameter
    const char* tagName,
    const char* attributeName,
    Boolean required)
{
    const char* typeName;

    if (!entry.getAttributeValue(attributeName, typeName))
    {
        if (required)
	{
	    char message[MESSAGE_SIZE];
	    sprintf(message, "missing %s.%s attribute", tagName, attributeName);
	    throw XmlValidationError(lineNumber, message);
	}
	else
	{
	    return false;
	}
    }

    CIMType type = CIMTYPE_BOOLEAN;
    Boolean unrecognizedType = false;

    if (strcmp(typeName, "boolean") == 0)
	type = CIMTYPE_BOOLEAN;
    else if (strcmp(typeName, "string") == 0)
	type = CIMTYPE_STRING;
    else if (strcmp(typeName, "char16") == 0)
	type = CIMTYPE_CHAR16;
    else if (strcmp(typeName, "uint8") == 0)
	type = CIMTYPE_UINT8;
    else if (strcmp(typeName, "sint8") == 0)
	type = CIMTYPE_SINT8;
    else if (strcmp(typeName, "uint16") == 0)
	type = CIMTYPE_UINT16;
    else if (strcmp(typeName, "sint16") == 0)
	type = CIMTYPE_SINT16;
    else if (strcmp(typeName, "uint32") == 0)
	type = CIMTYPE_UINT32;
    else if (strcmp(typeName, "sint32") == 0)
	type = CIMTYPE_SINT32;
    else if (strcmp(typeName, "uint64") == 0)
	type = CIMTYPE_UINT64;
    else if (strcmp(typeName, "sint64") == 0)
	type = CIMTYPE_SINT64;
    else if (strcmp(typeName, "datetime") == 0)
	type = CIMTYPE_DATETIME;
    else if (strcmp(typeName, "real32") == 0)
	type = CIMTYPE_REAL32;
    else if (strcmp(typeName, "real64") == 0)
	type = CIMTYPE_REAL64;
    else if (strcmp(typeName, "reference") == 0)
	type = CIMTYPE_REFERENCE;
    else unrecognizedType = true;

    if (unrecognizedType ||
        ((type == CIMTYPE_REFERENCE) &&
         (strcmp(attributeName, "PARAMTYPE") != 0)))
    {
	char message[MESSAGE_SIZE];
	sprintf(message, "Illegal value for %s.%s attribute", tagName,
	        attributeName);
	throw XmlSemanticError(lineNumber, message);
    }

    cimType = type;
    return true;
}

//------------------------------------------------------------------------------
//
// getCimBooleanAttribute()
//
//------------------------------------------------------------------------------

Boolean XmlReader::getCimBooleanAttribute(
    Uint32 lineNumber,
    const XmlEntry& entry,
    const char* tagName,
    const char* attributeName,
    Boolean defaultValue,
    Boolean required)
{
    const char* tmp;

    if (!entry.getAttributeValue(attributeName, tmp))
    {
	if (!required)
	    return defaultValue;

	char buffer[62];
	sprintf(buffer, "missing required %s.%s attribute", 
	    attributeName, tagName);

	throw XmlValidationError(lineNumber, buffer);
    }

    if (strcmp(tmp, "true") == 0)
	return true;
    else if (strcmp(tmp, "false") == 0)
	return false;

    char buffer[62];
    sprintf(buffer, "Invalid %s.%s attribute value", attributeName, tagName);
    throw XmlSemanticError(lineNumber, buffer);
    return false;
}

//------------------------------------------------------------------------------
//
// SringToReal()
//
//	[ "+" | "-" ] *decimalDigit "." 1*decimalDigit 
//	    [ ( "e" | "E" ) [ "+" | "-" ] 1*decimalDigit ]
//
//------------------------------------------------------------------------------

Boolean XmlReader::stringToReal(const char* stringValue, Real64& x)
{
    //
    // Check the string against the DMTF-defined grammar
    //
    const char* p = stringValue;

    if (!*p)
	return false;

    // Skip optional sign:

    if (*p == '+' || *p  == '-')
	p++;

    // Skip optional first set of digits:

    while (isdigit(*p))
	p++;

    // Test required dot:

    if (*p++ != '.')
	return false;

    // One or more digits required:

    if (!isdigit(*p++))
	return false;

    while (isdigit(*p))
	p++;

    // If there is an exponent now:

    if (*p)
    {
	// Test exponent:

	if (*p != 'e' && *p != 'E')
	    return false;

	p++;

	// Skip optional sign:

	if (*p == '+' || *p  == '-')
	    p++;

	// One or more digits required:

	if (!isdigit(*p++))
	    return false;

	while (isdigit(*p))
	    p++;
    }

    if (*p)
	return false;

    //
    // Do the conversion
    //
    char* end;
    errno = 0;
    x = strtod(stringValue, &end);
    if (*end || (errno == ERANGE))
    {
        return false;
    }

    return true;
}

inline Uint8 _hexCharToNumeric(const char c)
{
    Uint8 n;

    if (isdigit(c))
        n = (c - '0');
    else if (isupper(c))
        n = (c - 'A' + 10);
    else // if (islower(c))
        n = (c - 'a' + 10);

    return n;
}

//------------------------------------------------------------------------------
//
// stringToSignedInteger
//
//	[ "+" | "-" ] ( positiveDecimalDigit *decimalDigit | "0" )
//    or
//      [ "+" | "-" ] ( "0x" | "0X" ) 1*hexDigit
//
//------------------------------------------------------------------------------

Boolean XmlReader::stringToSignedInteger(
    const char* stringValue, 
    Sint64& x)
{
    x = 0;
    const char* p = stringValue;

    if (!p || !*p)
	return false;

    // Skip optional sign:

    Boolean negative = *p == '-';

    if (negative || *p == '+')
	p++;

    if (*p == '0')
    {
        if ( (p[1] == 'x') || (p[1] == 'X') )
        {
            // Convert a hexadecimal string

            // Skip over the "0x"
            p+=2;

            // At least one hexadecimal digit is required
            if (!isxdigit(*p))
                return false;

            // Build the Sint64 as a negative number, regardless of the
            // eventual sign (negative numbers can be bigger than positive ones)

            // Add on each digit, checking for overflow errors
            while (isxdigit(*p))
            {
                // Make sure we won't overflow when we multiply by 16
                if (x < PEGASUS_SINT64_MIN/16)
                {
                    return false;
                }
                x = x << 4;

                // Make sure we don't overflow when we add the next digit
                Sint64 newDigit = Sint64(_hexCharToNumeric(*p++));
                if (PEGASUS_SINT64_MIN - x > -newDigit)
                {
                    return false;
                }
                x = x - newDigit;
            }

            // If we found a non-hexadecimal digit, report an error
            if (*p)
                return false;

            // Return the integer to positive, if necessary, checking for an
            // overflow error
            if (!negative)
            {
                if (x == PEGASUS_SINT64_MIN)
                {
                    return false;
                }
                x = -x;
            }
            return true;
        }
        else
        {
            // A decimal string that starts with '0' must be exactly "0".
	    return p[1] == '\0';
        }
    }

    // Expect a positive decimal digit:

    // At least one decimal digit is required
    if (!isdigit(*p))
        return false;

    // Build the Sint64 as a negative number, regardless of the
    // eventual sign (negative numbers can be bigger than positive ones)

    // Add on each digit, checking for overflow errors
    while (isdigit(*p))
    {
        // Make sure we won't overflow when we multiply by 10
        if (x < PEGASUS_SINT64_MIN/10)
        {
            return false;
        }
        x = 10 * x;

        // Make sure we won't overflow when we add the next digit
        Sint64 newDigit = (*p++ - '0');
        if (PEGASUS_SINT64_MIN - x > -newDigit)
        {
            return false;
        }
        x = x - newDigit;
    }

    // If we found a non-decimal digit, report an error
    if (*p)
	return false;

    // Return the integer to positive, if necessary, checking for an
    // overflow error
    if (!negative)
    {
        if (x == PEGASUS_SINT64_MIN)
        {
            return false;
        }
        x = -x;
    }
    return true;
}

//------------------------------------------------------------------------------
//
// stringToUnsignedInteger
//
//	( positiveDecimalDigit *decimalDigit | "0" )
//    or
//      ( "0x" | "0X" ) 1*hexDigit
//
//------------------------------------------------------------------------------

Boolean XmlReader::stringToUnsignedInteger(
    const char* stringValue, 
    Uint64& x)
{
    x = 0;
    const char* p = stringValue;

    if (!p || !*p)
	return false;

    if (*p == '0')
    {
        if ( (p[1] == 'x') || (p[1] == 'X') )
        {
            // Convert a hexadecimal string

            // Skip over the "0x"
            p+=2;

            // At least one hexadecimal digit is required
            if (!*p)
                return false;

            // Add on each digit, checking for overflow errors
            while (isxdigit(*p))
            {
                // Make sure we won't overflow when we multiply by 16
                if (x > PEGASUS_UINT64_MAX/16)
                {
                    return false;
                }
                x = x << 4;

                // We can't overflow when we add the next digit
                Uint64 newDigit = Uint64(_hexCharToNumeric(*p++));
                if (PEGASUS_UINT64_MAX - x < newDigit)
                {
                    return false;
                }
                x = x + newDigit;
            }

            // If we found a non-hexadecimal digit, report an error
            if (*p)
                return false;

            return true;
        }
        else
        {
            // A decimal string that starts with '0' must be exactly "0".
	    return p[1] == '\0';
        }
    }

    // Expect a positive decimal digit:

    // Add on each digit, checking for overflow errors
    while (isdigit(*p))
    {
        // Make sure we won't overflow when we multiply by 10
        if (x > PEGASUS_UINT64_MAX/10)
        {
            return false;
        }
        x = 10 * x;

        // Make sure we won't overflow when we add the next digit
        Uint64 newDigit = (*p++ - '0');
        if (PEGASUS_UINT64_MAX - x < newDigit)
        {
            return false;
        }
        x = x + newDigit;
    }

    // If we found a non-decimal digit, report an error
    if (*p)
	return false;

    return true;
}

//------------------------------------------------------------------------------
//
// stringToValue()
//
// Return: CIMValue. If the string input is zero length creates a CIMValue
//         with value defined by the type.  Else the value is inserted.
//         
//         Note that this does not set the CIMValue Null if the string is empty.
//
//------------------------------------------------------------------------------

CIMValue XmlReader::stringToValue(
    Uint32 lineNumber, 
    const char* valueString, 
    CIMType type)
{
    // ATTN-B: accepting only UTF-8 for now! (affects string and char16):

    // Create value per type
    switch (type)
    {
	case CIMTYPE_BOOLEAN:
	{
	    if (CompareNoCase(valueString, "TRUE") == 0)
		return CIMValue(true);
	    else if (CompareNoCase(valueString, "FALSE") == 0)
		return CIMValue(false);
	    else
		throw XmlSemanticError(
		    lineNumber, "Invalid boolean value");
	}

	case CIMTYPE_STRING:
	{
	    return CIMValue(String(valueString));
	}

	case CIMTYPE_CHAR16:
	{
	    if (strlen(valueString) != 1)
		throw XmlSemanticError(lineNumber, "Invalid char16 value");

	    return CIMValue(Char16(valueString[0]));
	}

	case CIMTYPE_UINT8:
	case CIMTYPE_UINT16:
	case CIMTYPE_UINT32:
	case CIMTYPE_UINT64:
	{
	    Uint64 x;

	    if (!stringToUnsignedInteger(valueString, x))
	    {
		throw XmlSemanticError(
		    lineNumber, "Invalid unsigned integer value");
	    }

	    switch (type)
	    {
		case CIMTYPE_UINT8:
                {
                    if (x >= (Uint64(1)<<8))
		    {
			throw XmlSemanticError(
			    lineNumber, "Uint8 value out of range");
		    }
		    return CIMValue(Uint8(x));
                }
		case CIMTYPE_UINT16:
                {
                    if (x >= (Uint64(1)<<16))
		    {
			throw XmlSemanticError(
			    lineNumber, "Uint16 value out of range");
		    }
		    return CIMValue(Uint16(x));
                }
		case CIMTYPE_UINT32:
                {
                    if (x >= (Uint64(1)<<32))
		    {
			throw XmlSemanticError(
			    lineNumber, "Uint32 value out of range");
		    }
		    return CIMValue(Uint32(x));
                }
		case CIMTYPE_UINT64: return CIMValue(Uint64(x));
		default: break;
	    }
	}

	case CIMTYPE_SINT8:
	case CIMTYPE_SINT16:
	case CIMTYPE_SINT32:
	case CIMTYPE_SINT64:
	{
	    Sint64 x;

	    if (!stringToSignedInteger(valueString, x))
	    {
		throw XmlSemanticError(
		    lineNumber, "Invalid signed integer value");
	    }

	    switch (type)
	    {
		case CIMTYPE_SINT8:
                {
                    if(  (x >= (Sint64(1)<<7)) || (x < (-(Sint64(1)<<7))) )
		    {
			throw XmlSemanticError(
			    lineNumber, "Sint8 value out of range");
		    }
		    return CIMValue(Sint8(x));
                }
		case CIMTYPE_SINT16:
                {
                    if(  (x >= (Sint64(1)<<15)) || (x < (-(Sint64(1)<<15))) )
		    {
			throw XmlSemanticError(
			    lineNumber, "Sint16 value out of range");
		    }
		    return CIMValue(Sint16(x));
                }
		case CIMTYPE_SINT32:
                {
                    if(  (x >= (Sint64(1)<<31)) || (x < (-(Sint64(1)<<31))) )
		    {
			throw XmlSemanticError(
			    lineNumber, "Sint32 value out of range");
		    }
		    return CIMValue(Sint32(x));
                }
		case CIMTYPE_SINT64: return CIMValue(Sint64(x));
		default: break;
	    }
	}

	case CIMTYPE_DATETIME:
	{
	    CIMDateTime tmp;

	    try
	    {
		tmp.set(valueString);
	    }
	    catch (InvalidDateTimeFormatException&)
	    {
		throw XmlSemanticError(lineNumber, "Invalid datetime value");
	    }

	    return CIMValue(tmp);
	}

	case CIMTYPE_REAL32:
	{
	    Real64 x;

	    if (!stringToReal(valueString, x))
		throw XmlSemanticError(lineNumber, "Invalid real number value");

	    return CIMValue(Real32(x));
	}

	case CIMTYPE_REAL64:
	{
	    Real64 x;

	    if (!stringToReal(valueString, x))
		throw XmlSemanticError(lineNumber, "Invalid real number value");

	    return CIMValue(x);
	}

	default:
	    break;
    }

    throw XmlSemanticError(lineNumber, "malformed XML");
    return false;
}

//------------------------------------------------------------------------------
//
// getValueElement()
//
//     <!ELEMENT VALUE (#PCDATA)>
//
// Return: false if no value element.
//
//------------------------------------------------------------------------------

Boolean XmlReader::getValueElement(
    XmlParser& parser, 
    CIMType type, 
    CIMValue& value)
{
    // Get VALUE start tag: Return false if no VALUE start Tag

    XmlEntry entry;
    if (!testStartTagOrEmptyTag(parser, entry, "VALUE"))
	return false;

    Boolean empty = entry.type == XmlEntry::EMPTY_TAG;

    const char* valueString = "";

    if (!empty)
    {
	if (testContentOrCData(parser, entry))
	    valueString = entry.text;

	expectEndTag(parser, "VALUE");
    }

    value = stringToValue(parser.getLine(), valueString,type);
    
    return true;
}

//------------------------------------------------------------------------------
//
// getStringValueElement()
//
//     <!ELEMENT VALUE (#PCDATA)>
//
//------------------------------------------------------------------------------

Boolean XmlReader::getStringValueElement(
    XmlParser& parser, 
    String& str,
    Boolean required)
{
    XmlEntry entry;

    if (!testStartTagOrEmptyTag(parser, entry, "VALUE"))
    {
	if (required)
	    throw XmlValidationError(parser.getLine(),"Expected VALUE element");
	return false;
    }

    Boolean empty = entry.type == XmlEntry::EMPTY_TAG;

    const char* valueString = "";

    if (!empty)
    {
	if (testContentOrCData(parser, entry))
	    valueString = entry.text;

	expectEndTag(parser, "VALUE");
    }

    str = valueString;
    return true;
}

//----------------------------------------------------------------------------
//
// getPropertyValue
//     Use: Decode property value from SetProperty request and
//     GetProperty response.
//
//     PropertyValue is one of:
//
//
//	<!ELEMENT VALUE.ARRAY (VALUE*)>
//
//	<!ELEMENT VALUE.REFERENCE (CLASSPATH|LOCALCLASSPATH|CLASSNAME|
//         <!ELEMENT VALUE.ARRAY (VALUE*)>
//
//         <!ELEMENT VALUE.REFERENCE (CLASSPATH|LOCALCLASSPATH|CLASSNAME|
//                           INSTANCEPATH|LOCALINSTANCEPATH|INSTANCENAME)>
//
//         <!ELEMENT VALUE.REFARRAY (VALUE.REFERENCE*)>
//
//----------------------------------------------------------------------------
Boolean XmlReader::getPropertyValue(
    XmlParser& parser, 
    CIMValue& cimValue)
{
    // Can not test for value type, so assume String
    const CIMType type = CIMTYPE_STRING;

    // Test for VALUE element
    if (XmlReader::getValueElement(parser, type, cimValue))
    {
	return true;
    }

    // Test for VALUE.ARRAY element
    if (XmlReader::getValueArrayElement(parser, type, cimValue))
    {
       return true;
    }

    // Test for VALUE.REFERENCE element
    CIMObjectPath reference;
    if (XmlReader::getValueReferenceElement(parser, reference))
    {
        cimValue.set(reference);
        return true;
    }

    // Test for VALUE.REFARRAY element
    if (XmlReader::getValueReferenceArrayElement(parser, cimValue))
    {
       return true;
    }

    return false;
}

//------------------------------------------------------------------------------
//
// stringArrayToValue()
//
//------------------------------------------------------------------------------

template<class T>
CIMValue StringArrayToValueAux(
    Uint32 lineNumber, 
    const Array<const char*>& stringArray,
    CIMType type,
    T*)
{
    Array<T> array;

    for (Uint32 i = 0, n = stringArray.size(); i < n; i++)
    {
	CIMValue value = XmlReader::stringToValue(
	    lineNumber, stringArray[i], type);

	T x;
	value.get(x);
	array.append(x);
    }

    return CIMValue(array);
}

CIMValue XmlReader::stringArrayToValue(
    Uint32 lineNumber, 
    const Array<const char*>& array, 
    CIMType type)
{
    switch (type)
    {
	case CIMTYPE_BOOLEAN: 
	    return StringArrayToValueAux(lineNumber, array, type, (Boolean*)0);

	case CIMTYPE_STRING:
	    return StringArrayToValueAux(lineNumber, array, type, (String*)0);

	case CIMTYPE_CHAR16:
	    return StringArrayToValueAux(lineNumber, array, type, (Char16*)0);

	case CIMTYPE_UINT8:
	    return StringArrayToValueAux(lineNumber, array, type, (Uint8*)0);

	case CIMTYPE_UINT16:
	    return StringArrayToValueAux(lineNumber, array, type, (Uint16*)0);

	case CIMTYPE_UINT32:
	    return StringArrayToValueAux(lineNumber, array, type, (Uint32*)0);

	case CIMTYPE_UINT64:
	    return StringArrayToValueAux(lineNumber, array, type, (Uint64*)0);

	case CIMTYPE_SINT8:
	    return StringArrayToValueAux(lineNumber, array, type, (Sint8*)0);

	case CIMTYPE_SINT16:
	    return StringArrayToValueAux(lineNumber, array, type, (Sint16*)0);

	case CIMTYPE_SINT32:
	    return StringArrayToValueAux(lineNumber, array, type, (Sint32*)0);

	case CIMTYPE_SINT64:
	    return StringArrayToValueAux(lineNumber, array, type, (Sint64*)0);

	case CIMTYPE_DATETIME:
	    return StringArrayToValueAux(lineNumber, array, type, (CIMDateTime*)0);

	case CIMTYPE_REAL32:
	    return StringArrayToValueAux(lineNumber, array, type, (Real32*)0);

	case CIMTYPE_REAL64:
	    return StringArrayToValueAux(lineNumber, array, type, (Real64*)0);

	default:
	    break;
    }

    // Unreachable:
    return CIMValue();
}

//------------------------------------------------------------------------------
//
// getValueArrayElement()
//
//     <!ELEMENT VALUE.ARRAY (VALUE*)>
//
//  Return: Boolean. Returns false if there is no VALUE.ARRAY start element
//
//------------------------------------------------------------------------------

Boolean XmlReader::getValueArrayElement(
    XmlParser& parser, 
    CIMType type, 
    CIMValue& value)
{
    // Clears any values from the Array. Assumes this is array CIMValue
    value.clear();

    // Get VALUE.ARRAY open tag:

    XmlEntry entry;
    Array<const char*> stringArray;

    // If no VALUE.ARRAY start tag, return false
    if (!testStartTagOrEmptyTag(parser, entry, "VALUE.ARRAY"))
	return false;

    if (entry.type != XmlEntry::EMPTY_TAG)
    {
        // For each VALUE element:

        while (testStartTagOrEmptyTag(parser, entry, "VALUE"))
        {
	    if (entry.type == XmlEntry::EMPTY_TAG)
	    {
	        stringArray.append("");
	        continue;
	    }

	    if (testContentOrCData(parser, entry))
	        stringArray.append(entry.text);
	    else
	        stringArray.append("");

	    expectEndTag(parser, "VALUE");
        }

        expectEndTag(parser, "VALUE.ARRAY");
    }

    value = stringArrayToValue(parser.getLine(), stringArray, type);
    return true;
}

//------------------------------------------------------------------------------
//
// getFlavor()
//
//     <!ENTITY % QualifierFlavor 
//         "OVERRIDABLE (true|false) 'true'
//         TOSUBCLASS (true|false) 'true'
//         TOINSTANCE (true|false)  'false'
//         TRANSLATABLE (true|false)  'false'">
//
//------------------------------------------------------------------------------

CIMFlavor XmlReader::getFlavor(
    XmlEntry& entry, 
    Uint32 lineNumber, 
    const char* tagName)
{
    // Get QUALIFIER.OVERRIDABLE

    Boolean overridable = getCimBooleanAttribute(
	lineNumber, entry, tagName, "OVERRIDABLE", true, false);

    // Get QUALIFIER.TOSUBCLASS

    Boolean toSubClass = getCimBooleanAttribute(
	lineNumber, entry, tagName, "TOSUBCLASS", true, false);

    // Get QUALIFIER.TOINSTANCE

    Boolean toInstance = getCimBooleanAttribute(
	lineNumber, entry, tagName, "TOINSTANCE", false, false);

    // Get QUALIFIER.TRANSLATABLE

    Boolean translatable = getCimBooleanAttribute(
	lineNumber, entry, tagName, "TRANSLATABLE", false, false);

    // Start with CIMFlavor::NONE.  Defaults are specified in the
    // getCimBooleanAttribute() calls above.
    CIMFlavor flavor = CIMFlavor (CIMFlavor::NONE);

    if (overridable)
        flavor.addFlavor (CIMFlavor::OVERRIDABLE);
    else
        flavor.addFlavor (CIMFlavor::DISABLEOVERRIDE);

    if (toSubClass)
        flavor.addFlavor (CIMFlavor::TOSUBCLASS);
    else
        flavor.addFlavor (CIMFlavor::RESTRICTED);

    if (toInstance)
        flavor.addFlavor (CIMFlavor::TOINSTANCE);

    if (translatable)
        flavor.addFlavor (CIMFlavor::TRANSLATABLE);

    return flavor;
}

//------------------------------------------------------------------------------
//
// getOptionalScope()
//
//     DTD:
//         <!ELEMENT SCOPE EMPTY>
//         <!ATTLIST SCOPE 
//              CLASS (true|false) 'false'
//              ASSOCIATION (true|false) 'false'
//              REFERENCE (true|false) 'false'
//              PROPERTY (true|false) 'false'
//              METHOD (true|false) 'false'
//              PARAMETER (true|false) 'false'
//              INDICATION (true|false) 'false'>
//
//------------------------------------------------------------------------------

CIMScope XmlReader::getOptionalScope(XmlParser& parser)
{
    XmlEntry entry;
    CIMScope scope;

    if (!parser.next(entry))
	return scope;    // No SCOPE element found; return the empty scope

    Boolean isEmptyTag = entry.type == XmlEntry::EMPTY_TAG;

    if ((!isEmptyTag && 
	entry.type != XmlEntry::START_TAG) ||
	strcmp(entry.text, "SCOPE") != 0)
    {
	// No SCOPE element found; return the empty scope
	parser.putBack(entry);
	return scope;
    }

    Uint32 line = parser.getLine();

    if (getCimBooleanAttribute(line, entry, "SCOPE", "CLASS", false, false))
	scope.addScope (CIMScope::CLASS);

    if (getCimBooleanAttribute(
	line, entry, "SCOPE", "ASSOCIATION", false, false))
	scope.addScope (CIMScope::ASSOCIATION);

    if (getCimBooleanAttribute(
	line, entry, "SCOPE", "REFERENCE", false, false))
	scope.addScope (CIMScope::REFERENCE);

    if (getCimBooleanAttribute(line, entry, "SCOPE", "PROPERTY", false, false))
	scope.addScope (CIMScope::PROPERTY);

    if (getCimBooleanAttribute(line, entry, "SCOPE", "METHOD", false, false))
	scope.addScope (CIMScope::METHOD);

    if (getCimBooleanAttribute(line, entry, "SCOPE", "PARAMETER", false, false))
	scope.addScope (CIMScope::PARAMETER);

    if (getCimBooleanAttribute(line, entry, "SCOPE", "INDICATION",false, false))
	scope.addScope (CIMScope::INDICATION);

    if (!isEmptyTag)
	expectEndTag(parser, "SCOPE");

    return scope;
}

//------------------------------------------------------------------------------
//
// getQualifierElement()
//
//     <!ELEMENT QUALIFIER (VALUE|VALUE.ARRAY)?>
//     <!ATTLIST QUALIFIER
//         %CIMName;
//         %CIMType; #REQUIRED
//         %Propagated;
//         %QualifierFlavor;>
//
//------------------------------------------------------------------------------

Boolean XmlReader::getQualifierElement(
    XmlParser& parser, 
    CIMQualifier& qualifier)
{
    // Get QUALIFIER element:

    XmlEntry entry;
    if (!testStartTag(parser, entry, "QUALIFIER"))
	return false;

    // Get QUALIFIER.NAME attribute:

    String name = getCimNameAttribute(parser.getLine(), entry, "QUALIFIER");

    // Get QUALIFIER.TYPE attribute:

    CIMType type;
    getCimTypeAttribute(parser.getLine(), entry, type, "QUALIFIER");

    // Get QUALIFIER.PROPAGATED

    Boolean propagated = getCimBooleanAttribute(
	parser.getLine(), entry, "QUALIFIER", "PROPAGATED", false, false);

    // Get flavor oriented attributes:

    CIMFlavor flavor = getFlavor(entry, parser.getLine(), "QUALIFIER");

    // Get VALUE or VALUE.ARRAY element:

    CIMValue value;

    if (!getValueElement(parser, type, value) &&
	!getValueArrayElement(parser, type, value))
    {
        value.setNullValue(type, false);
    }

    // Expect </QUALIFIER>:

    expectEndTag(parser, "QUALIFIER");

    // Build qualifier:

    qualifier = CIMQualifier(name, value, flavor, propagated);
    return true;
}

//------------------------------------------------------------------------------
//
// getQualifierElements()
//
//------------------------------------------------------------------------------

template<class CONTAINER>
void getQualifierElements(XmlParser& parser, CONTAINER& container)
{
    CIMQualifier qualifier;

    while (XmlReader::getQualifierElement(parser, qualifier))
    {
	try
	{
	    container.addQualifier(qualifier);
	}
	catch (AlreadyExistsException&)
	{
	    throw XmlSemanticError(parser.getLine(), "duplicate qualifier");
	}
    }
}

//------------------------------------------------------------------------------
//
// getPropertyElement()
//
//     <!ELEMENT PROPERTY (QUALIFIER*,VALUE?)>
//     <!ATTLIST PROPERTY
//         %CIMName;
//         %ClassOrigin;
//         %Propagated;
//         %CIMType; #REQUIRED>
//
//------------------------------------------------------------------------------

Boolean XmlReader::getPropertyElement(XmlParser& parser, CIMProperty& property)
{
    XmlEntry entry;

    if (!testStartTagOrEmptyTag(parser, entry, "PROPERTY"))
	return false;

    Boolean empty = entry.type == XmlEntry::EMPTY_TAG;

    // Get PROPERTY.NAME attribute:

    String name = getCimNameAttribute(parser.getLine(), entry, "PROPERTY");

    // Get PROPERTY.CLASSORIGIN attribute:

    CIMName classOrigin = 
	getClassOriginAttribute(parser.getLine(), entry, "PROPERTY");

    // Get PROPERTY.PROPAGATED

    Boolean propagated = getCimBooleanAttribute(
	parser.getLine(), entry, "PROPERTY", "PROPAGATED", false, false);

    // Get PROPERTY.TYPE attribute:

    CIMType type;
    getCimTypeAttribute(parser.getLine(), entry, type, "PROPERTY");

    // Create property: Sets type and !isarray

    CIMValue value(type, false);
    property = CIMProperty(name, value, 0, CIMName(), classOrigin, propagated);

    if (!empty)
    {
	// Get qualifiers:

	getQualifierElements(parser, property);

	// Get value:  Insert value if getValueElement exists (returns True)

	if (getValueElement(parser, type, value))
	    property.setValue(value);

	expectEndTag(parser, "PROPERTY");
    }

    return true;
}

//------------------------------------------------------------------------------
//
// getArraySizeAttribute()
//
//     Returns true if able to get array-size. Note that array size will
//     always be a positive integer.
//
//     <!ENTITY % ArraySize "ARRAYSIZE CDATA #IMPLIED">
//
//------------------------------------------------------------------------------

Boolean XmlReader::getArraySizeAttribute(
    Uint32 lineNumber,
    const XmlEntry& entry,
    const char* tagName,
    Uint32& value)
{
    const char* tmp;

    if (!entry.getAttributeValue("ARRAYSIZE", tmp))
	return false;

    Uint64 arraySize;

    if (!stringToUnsignedInteger(tmp, arraySize) || arraySize == 0)
    {
	char message[128];
	sprintf(message, "Illegal value for %s.%s", tagName, "ARRAYSIZE");
	throw XmlSemanticError(lineNumber, message);
    }

    value = Uint32(arraySize);
    return true;
}

//------------------------------------------------------------------------------
//
// getPropertyArrayElement()
//
//     <!ELEMENT PROPERTY.ARRAY (QUALIFIER*,VALUE.ARRAY?)>
//     <!ATTLIST PROPERTY.ARRAY
//             %CIMName;
//             %CIMType; #REQUIRED
//             %ArraySize;
//             %ClassOrigin;
//             %Propagated;>
//
//------------------------------------------------------------------------------

Boolean XmlReader::getPropertyArrayElement(
    XmlParser& parser, 
    CIMProperty& property)
{
    // Get PROPERTY element:

    XmlEntry entry;

    if (!testStartTagOrEmptyTag(parser, entry, "PROPERTY.ARRAY"))
	return false;

    Boolean empty = entry.type == XmlEntry::EMPTY_TAG;

    // Get PROPERTY.NAME attribute:

    String name = 
	getCimNameAttribute(parser.getLine(), entry, "PROPERTY.ARRAY");

    // Get PROPERTY.TYPE attribute:

    CIMType type;
    getCimTypeAttribute(parser.getLine(), entry, type, "PROPERTY.ARRAY");

    // Get PROPERTY.ARRAYSIZE attribute:

    Uint32 arraySize = 0;
    getArraySizeAttribute(parser.getLine(), entry, "PROPERTY.ARRAY", arraySize);

    // Get PROPERTY.CLASSORIGIN attribute:

    CIMName classOrigin 
	= getClassOriginAttribute(parser.getLine(), entry, "PROPERTY.ARRAY");

    // Get PROPERTY.ARRAY.PROPAGATED

    Boolean propagated = getCimBooleanAttribute(
	parser.getLine(), entry, "PROPERTY.ARRAY", "PROPAGATED", false, false);

    // Create property:

    CIMValue value(type, true, arraySize);
    property = CIMProperty(
	name, value, arraySize, CIMName(), classOrigin, propagated);

    if (!empty)
    {
	// Get qualifiers:

	getQualifierElements(parser, property);

	// Get value:

	if (getValueArrayElement(parser, type, value))
	{
	    if (arraySize && arraySize != value.getArraySize())
	    {
		throw XmlSemanticError(parser.getLine(),
		    "ARRAYSIZE attribute and value-array size are different");
	    }

	    property.setValue(value);
	}

	expectEndTag(parser, "PROPERTY.ARRAY");
    }

    return true;
}

//------------------------------------------------------------------------------
//
// getHostElement()
//
//     <!ELEMENT HOST (#PCDATA)>
//
//------------------------------------------------------------------------------

Boolean XmlReader::getHostElement(
    XmlParser& parser,
    String& host)
{
    XmlEntry entry;

    if (!testStartTag(parser, entry, "HOST"))
	return false;

    if (!parser.next(entry) || entry.type != XmlEntry::CONTENT)
    {
	throw XmlValidationError(parser.getLine(),
	    "expected content of HOST element");
    }

    host = entry.text;

    expectEndTag(parser, "HOST");
    return true;
}

//------------------------------------------------------------------------------
//
// getNameSpaceElement()
//     
//     <!ELEMENT NAMESPACE EMPTY>
//     <!ATTLIST NAMESPACE %CIMName;>
//
//------------------------------------------------------------------------------

Boolean XmlReader::getNameSpaceElement(
    XmlParser& parser,
    String& nameSpaceComponent)
{
    XmlEntry entry;

    if (!testStartTagOrEmptyTag(parser, entry, "NAMESPACE"))
	return false;

    Boolean empty = entry.type == XmlEntry::EMPTY_TAG;

    nameSpaceComponent = getCimNameAttribute(
	parser.getLine(), entry, "NAMESPACE");

    if (!empty)
	expectEndTag(parser, "NAMESPACE");

    return true;
}

//------------------------------------------------------------------------------
//
// getLocalNameSpacePathElement()
//     
//     <!ELEMENT LOCALNAMESPACEPATH (NAMESPACE+)>
//
//------------------------------------------------------------------------------

Boolean XmlReader::getLocalNameSpacePathElement(
    XmlParser& parser,
    String& nameSpace)
{
    XmlEntry entry;

    if (!testStartTag(parser, entry, "LOCALNAMESPACEPATH"))
	return false;

    String nameSpaceComponent;

    while (getNameSpaceElement(parser, nameSpaceComponent))
    {
	if (nameSpace.size())
	    nameSpace.append('/');

	nameSpace.append(nameSpaceComponent);
    }

    if (!nameSpace.size())
    {
	throw XmlValidationError(parser.getLine(),
	    "Expected one or more NAMESPACE elements within "
	    "LOCALNAMESPACEPATH element");
    }

    expectEndTag(parser, "LOCALNAMESPACEPATH");
    return true;
}

//------------------------------------------------------------------------------
//
// getNameSpacePathElement()
//
//     <!ELEMENT NAMESPACEPATH (HOST,LOCALNAMESPACEPATH)>
//
//------------------------------------------------------------------------------

Boolean XmlReader::getNameSpacePathElement(
    XmlParser& parser,
    String& host,
    String& nameSpace)
{
    host.clear();
    nameSpace.clear();

    XmlEntry entry;

    if (!testStartTag(parser, entry, "NAMESPACEPATH"))
	return false;

    if (!getHostElement(parser, host))
	throw XmlValidationError(parser.getLine(), "expected HOST element");

    if (!getLocalNameSpacePathElement(parser, nameSpace))
    {
	throw XmlValidationError(parser.getLine(), 
	    "expected LOCALNAMESPACEPATH element");
    }

    expectEndTag(parser, "NAMESPACEPATH");

    return true;
}

//------------------------------------------------------------------------------
//
// getClassNameElement()
//
//     <!ELEMENT CLASSNAME EMPTY>
//     <!ATTLIST CLASSNAME %CIMName;>
//
//------------------------------------------------------------------------------

Boolean XmlReader::getClassNameElement(
    XmlParser& parser,
    String& className,
    Boolean required)
{
    XmlEntry entry;

    if (!testStartTagOrEmptyTag(parser, entry, "CLASSNAME"))
    {
	if (required)
	{
	    throw XmlValidationError(parser.getLine(),
		"expected CLASSNAME element");
	}
	else
	    return false;
    }

    Boolean empty = entry.type == XmlEntry::EMPTY_TAG;

    className = getCimNameAttribute(
	parser.getLine(), entry, "CLASSNAME", true);

    if (!empty)
	expectEndTag(parser, "CLASSNAME");

    return true;
}

//------------------------------------------------------------------------------
//
// getValueTypeAttribute()
//
//     VALUETYPE (string|boolean|numeric) 'string'
//
//------------------------------------------------------------------------------

KeyBinding::Type XmlReader::getValueTypeAttribute(
    Uint32 lineNumber, 
    const XmlEntry& entry,
    const char* elementName)
{
    String tmp;

    if (!entry.getAttributeValue("VALUETYPE", tmp))
	return KeyBinding::STRING;

    if (String::equal(tmp, "string"))
	return KeyBinding::STRING;
    else if (String::equal(tmp, "boolean"))
	return KeyBinding::BOOLEAN;
    else if (String::equal(tmp, "numeric"))
	return KeyBinding::NUMERIC;

    char buffer[MESSAGE_SIZE];

    sprintf(buffer, 
	"Illegal value for %s.VALUETYPE attribute; "
	"CIMValue must be one of \"string\", \"boolean\", or \"numeric\"",
	elementName);

    throw XmlSemanticError(lineNumber, buffer);
    return KeyBinding::BOOLEAN;
}

//------------------------------------------------------------------------------
//
// getKeyValueElement()
//
//     <!ELEMENT KEYVALUE (#PCDATA)>
//     <!ATTLIST KEYVALUE
//         VALUETYPE (string|boolean|numeric)  'string'>
//
//------------------------------------------------------------------------------

Boolean XmlReader::getKeyValueElement(
    XmlParser& parser,
    KeyBinding::Type& type,
    String& value)
{
    XmlEntry entry;

    if (!testStartTagOrEmptyTag(parser, entry, "KEYVALUE"))
	return false;

    Boolean empty = entry.type == XmlEntry::EMPTY_TAG;

    type = getValueTypeAttribute(parser.getLine(), entry, "KEYVALUE");

    value.clear();

    if (!empty)
    {
	if (!parser.next(entry))
	    throw XmlException(XmlException::UNCLOSED_TAGS, parser.getLine());

	if (entry.type == XmlEntry::CONTENT)
	    value = entry.text;
	else
	    parser.putBack(entry);

	expectEndTag(parser, "KEYVALUE");
    }

    return true;
}

//------------------------------------------------------------------------------
//
// getKeyBindingElement()
//
//     <!ELEMENT KEYBINDING (KEYVALUE|VALUE.REFERENCE)>
//     <!ATTLIST KEYBINDING
//         %CIMName;>
//
//------------------------------------------------------------------------------

Boolean XmlReader::getKeyBindingElement(
    XmlParser& parser,
    String& name,
    String& value,
    KeyBinding::Type& type)
{
    XmlEntry entry;

    if (!testStartTag(parser, entry, "KEYBINDING"))
	return false;

    name = getCimNameAttribute(parser.getLine(), entry, "KEYBINDING");

    if (!getKeyValueElement(parser, type, value))
    {
        CIMObjectPath reference;

        if (!getValueReferenceElement(parser, reference))
        {
	    throw XmlValidationError(parser.getLine(),
                      "Expected KEYVALUE or VALUE.REFERENCE element");
        }

        type = KeyBinding::REFERENCE;
        value = reference.toString();
    }

    expectEndTag(parser, "KEYBINDING");
    return true;
}

//------------------------------------------------------------------------------
//
// getInstanceNameElement()
//
//     <!ELEMENT INSTANCENAME (KEYBINDING*|KEYVALUE?|VALUE.REFERENCE?)>
//     <!ATTLIST INSTANCENAME
//         %ClassName;>
//
// Note: An empty key name is used in the keyBinding when the INSTANCENAME is
// specified using a KEYVALUE or a VALUE.REFERENCE.
//
//------------------------------------------------------------------------------

Boolean XmlReader::getInstanceNameElement(
    XmlParser& parser,
    String& className,
    Array<KeyBinding>& keyBindings)
{
    className.clear();
    keyBindings.clear();

    XmlEntry entry;

    if (!testStartTagOrEmptyTag(parser, entry, "INSTANCENAME"))
	return false;

    Boolean empty = entry.type == XmlEntry::EMPTY_TAG;

    className = getClassNameAttribute(parser.getLine(), entry, "INSTANCENAME");

    if (empty)
    {
        return true;
    }

    String name;
    KeyBinding::Type type;
    String value;
    CIMObjectPath reference;

    if (getKeyValueElement(parser, type, value))
    {
        // Use empty key name because none was specified
        keyBindings.append(KeyBinding(name, value, type));
    }
    else if (getValueReferenceElement(parser, reference))
    {
        // Use empty key name because none was specified
        type = KeyBinding::REFERENCE;
        value = reference.toString();
        keyBindings.append(KeyBinding(name, value, type));
    }
    else
    {
	while (getKeyBindingElement(parser, name, value, type))
	    keyBindings.append(KeyBinding(name, value, type));
    }

    expectEndTag(parser, "INSTANCENAME");

    return true;
}

Boolean XmlReader::getInstanceNameElement(
    XmlParser& parser,
    CIMObjectPath& instanceName)
{
    String className;
    Array<KeyBinding> keyBindings;

    if (!XmlReader::getInstanceNameElement(parser, className, keyBindings))
	return false;

    instanceName.set(String(), CIMNamespaceName(), className, keyBindings);
    return true;
}

//------------------------------------------------------------------------------
//
// getInstancePathElement()
//
//     <!ELEMENT INSTANCEPATH (NAMESPACEPATH,INSTANCENAME)>
//
//------------------------------------------------------------------------------

Boolean XmlReader::getInstancePathElement(
    XmlParser& parser,
    CIMObjectPath& reference)
{
    XmlEntry entry;

    if (!testStartTag(parser, entry, "INSTANCEPATH"))
	return false;

    String host;
    String nameSpace;

    if (!getNameSpacePathElement(parser, host, nameSpace))
    {
	throw XmlValidationError(parser.getLine(),
	    "expected NAMESPACEPATH element");
    }

    String className;
    Array<KeyBinding> keyBindings;

    if (!getInstanceNameElement(parser, className, keyBindings))
    {
	throw XmlValidationError(parser.getLine(), 
	    "expected INSTANCENAME element");
    }

    reference.set(host, nameSpace, className, keyBindings);

    expectEndTag(parser, "INSTANCEPATH");
    return true;
}

//------------------------------------------------------------------------------
//
// getLocalInstancePathElement()
//
//     <!ELEMENT LOCALINSTANCEPATH (NAMESPACEPATH,INSTANCENAME)>
//
//------------------------------------------------------------------------------

Boolean XmlReader::getLocalInstancePathElement(
    XmlParser& parser,
    CIMObjectPath& reference)
{
    XmlEntry entry;

    if (!testStartTag(parser, entry, "LOCALINSTANCEPATH"))
	return false;

    String nameSpace;

    if (!getLocalNameSpacePathElement(parser, nameSpace))
    {
	throw XmlValidationError(parser.getLine(),
	    "expected LOCALNAMESPACEPATH element");
    }

    String className;
    Array<KeyBinding> keyBindings;

    if (!getInstanceNameElement(parser, className, keyBindings))
    {
	throw XmlValidationError(parser.getLine(), 
	    "expected INSTANCENAME element");
    }

    reference.set(String(), nameSpace, className, keyBindings);

    expectEndTag(parser, "LOCALINSTANCEPATH");
    return true;
}

//------------------------------------------------------------------------------
//
// getClassPathElement()
//
//     <!ELEMENT CLASSPATH (NAMESPACEPATH,CLASSNAME)>
//
//------------------------------------------------------------------------------

Boolean XmlReader::getClassPathElement(
    XmlParser& parser,
    CIMObjectPath& reference)
{
    XmlEntry entry;

    if (!testStartTag(parser, entry, "CLASSPATH"))
	return false;

    String host;
    String nameSpace;

    if (!getNameSpacePathElement(parser, host, nameSpace))
    {
	throw XmlValidationError(parser.getLine(),
	    "expected NAMESPACEPATH element");
    }

    String className;

    if (!getClassNameElement(parser, className))
    {
	throw XmlValidationError(parser.getLine(), 
	    "expected CLASSNAME element");
    }

    reference.set(host, nameSpace, className);

    expectEndTag(parser, "CLASSPATH");
    return true;
}

//------------------------------------------------------------------------------
//
// getLocalClassPathElement()
//
//     <!ELEMENT LOCALCLASSPATH (LOCALNAMESPACEPATH,CLASSNAME)>
//
//------------------------------------------------------------------------------

Boolean XmlReader::getLocalClassPathElement(
    XmlParser& parser,
    CIMObjectPath& reference)
{
    XmlEntry entry;

    if (!testStartTag(parser, entry, "LOCALCLASSPATH"))
	return false;

    String nameSpace;

    if (!getLocalNameSpacePathElement(parser, nameSpace))
    {
	throw XmlValidationError(parser.getLine(),
	    "expected LOCALNAMESPACEPATH element");
    }

    String className;

    if (!getClassNameElement(parser, className))
    {
	throw XmlValidationError(parser.getLine(), 
	    "expected CLASSNAME element");
    }

    reference.set(String(), nameSpace, className);

    expectEndTag(parser, "LOCALCLASSPATH");

    return true;
}

//------------------------------------------------------------------------------
//
// getValueReferenceElement()
//
//     <!ELEMENT VALUE.REFERENCE (CLASSPATH|LOCALCLASSPATH|CLASSNAME|
//         INSTANCEPATH|LOCALINSTANCEPATH|INSTANCENAME)>
//
//
//------------------------------------------------------------------------------

Boolean XmlReader::getValueReferenceElement(
    XmlParser& parser,
    CIMObjectPath& reference)
{
    XmlEntry entry;

    if (!testStartTag(parser, entry, "VALUE.REFERENCE"))
	return false;

    if (!parser.next(entry))
	throw XmlException(XmlException::UNCLOSED_TAGS, parser.getLine());

    if (entry.type != XmlEntry::START_TAG && 
	entry.type != XmlEntry::EMPTY_TAG)
    {
	throw XmlValidationError(parser.getLine(), 
	    "Expected one of the following start tags: "
	    "CLASSPATH, LOCALCLASSPATH, CLASSNAME, INSTANCEPATH, "
	    "LOCALINSTANCEPATH, INSTANCENAME");
    }

    if (strcmp(entry.text, "CLASSPATH") == 0)
    {
	parser.putBack(entry);
	getClassPathElement(parser, reference);
    }
    else if (strcmp(entry.text, "LOCALCLASSPATH") == 0)
    {
	parser.putBack(entry);
	getLocalClassPathElement(parser, reference);
    }
    else if (strcmp(entry.text, "CLASSNAME") == 0)
    {
	parser.putBack(entry);
	String className;
	getClassNameElement(parser, className);
	reference.set(String(), CIMNamespaceName(), className);
    }
    else if (strcmp(entry.text, "INSTANCEPATH") == 0)
    {
	parser.putBack(entry);
	getInstancePathElement(parser, reference);
    }
    else if (strcmp(entry.text, "LOCALINSTANCEPATH") == 0)
    {
	parser.putBack(entry);
	getLocalInstancePathElement(parser, reference);
    }
    else if (strcmp(entry.text, "INSTANCENAME") == 0)
    {
	parser.putBack(entry);
	String className;
	Array<KeyBinding> keyBindings;
	getInstanceNameElement(parser, className, keyBindings);
	reference.set(String(), CIMNamespaceName(), className, keyBindings);
    }

    expectEndTag(parser, "VALUE.REFERENCE");
    return true;
}

//------------------------------------------------------------------------------
//
// getValueReferenceArrayElement()
//
//     <!ELEMENT VALUE.REFARRAY (VALUE.REFERENCE*)>
//
//------------------------------------------------------------------------------

Boolean XmlReader::getValueReferenceArrayElement(
    XmlParser& parser, 
    CIMValue& value)
{
    XmlEntry entry;
    Array<CIMObjectPath> referenceArray;
    CIMObjectPath reference;

    value.clear();

    // Get VALUE.REFARRAY open tag:

    if (!testStartTagOrEmptyTag(parser, entry, "VALUE.REFARRAY"))
	return false;

    if (entry.type != XmlEntry::EMPTY_TAG)
    {
        // For each VALUE.REFERENCE element:

        while (getValueReferenceElement(parser, reference))
        {
	    referenceArray.append(reference);
        }

        expectEndTag(parser, "VALUE.REFARRAY");
    }

    value.set(referenceArray);
    return true;
}

//------------------------------------------------------------------------------
//
// getPropertyReferenceElement()
//
//     <!ELEMENT PROPERTY.REFERENCE (QUALIFIER*,(VALUE.REFERENCE)?)>
//     <!ATTLIST PROPERTY.REFERENCE
//         %CIMName;
//         %ReferenceClass;
//         %ClassOrigin;
//         %Propagated;>
//
//------------------------------------------------------------------------------

Boolean XmlReader::getPropertyReferenceElement(
    XmlParser& parser, 
    CIMProperty& property)
{
    XmlEntry entry;

    if (!testStartTagOrEmptyTag(parser, entry, "PROPERTY.REFERENCE"))
	return false;

    Boolean empty = entry.type == XmlEntry::EMPTY_TAG;

    // Get PROPERTY.NAME attribute:

    String name = getCimNameAttribute(
	parser.getLine(), entry, "PROPERTY.REFERENCE");

    // Get PROPERTY.REFERENCECLASS attribute:

    CIMName referenceClass = getReferenceClassAttribute(
	parser.getLine(), entry, "PROPERTY.REFERENCE");

    // Get PROPERTY.CLASSORIGIN attribute:

    CIMName classOrigin = 
	getClassOriginAttribute(parser.getLine(), entry, "PROPERTY.REFERENCE");

    // Get PROPERTY.PROPAGATED

    Boolean propagated = getCimBooleanAttribute(parser.getLine(), entry, 
	"PROPERTY.REFERENCE", "PROPAGATED", false, false);

    // Create property:

    CIMValue value = CIMValue(CIMTYPE_REFERENCE, false, 0);
//    value.set(CIMObjectPath());
    property = CIMProperty(
	name, value, 0, referenceClass, classOrigin, propagated);

    if (!empty)
    {
	getQualifierElements(parser, property);

	CIMObjectPath reference;

	if (getValueReferenceElement(parser, reference))
	    property.setValue(reference);

	expectEndTag(parser, "PROPERTY.REFERENCE");
    }

    return true;
}

//------------------------------------------------------------------------------
//
// GetPropertyElements()
//
//------------------------------------------------------------------------------

template<class CONTAINER>
void GetPropertyElements(XmlParser& parser, CONTAINER& container)
{
    CIMProperty property;

    while (XmlReader::getPropertyElement(parser, property) ||
	XmlReader::getPropertyArrayElement(parser, property) ||
	XmlReader::getPropertyReferenceElement(parser, property))
    {
	try
	{
	    container.addProperty(property);
	}
	catch (AlreadyExistsException&)
	{
	    throw XmlSemanticError(parser.getLine(), "duplicate property");
	}
    }
}

//------------------------------------------------------------------------------
//
// getParameterElement()
//
//     <!ELEMENT PARAMETER (QUALIFIER*)>
//     <!ATTLIST PARAMETER
//         %CIMName;
//         %CIMType; #REQUIRED>
//
//------------------------------------------------------------------------------

Boolean XmlReader::getParameterElement(
    XmlParser& parser, 
    CIMParameter& parameter)
{
    XmlEntry entry;

    if (!testStartTagOrEmptyTag(parser, entry, "PARAMETER"))
	return false;

    Boolean empty = entry.type == XmlEntry::EMPTY_TAG;

    // Get PARAMETER.NAME attribute:

    String name = getCimNameAttribute(parser.getLine(), entry, "PARAMETER");

    // Get PARAMETER.TYPE attribute:

    CIMType type;
    getCimTypeAttribute(parser.getLine(), entry, type, "PARAMETER");

    // Create parameter:

    parameter = CIMParameter(name, type);

    if (!empty)
    {
	getQualifierElements(parser, parameter);

	expectEndTag(parser, "PARAMETER");
    }

    return true;
}

//------------------------------------------------------------------------------
//
// getParameterArrayElement()
//
//     <!ELEMENT PARAMETER.ARRAY (QUALIFIER*)>
//     <!ATTLIST PARAMETER.ARRAY
//         %CIMName;
//         %CIMType; #REQUIRED
//         %ArraySize;>
//
//------------------------------------------------------------------------------

Boolean XmlReader::getParameterArrayElement(
    XmlParser& parser, 
    CIMParameter& parameter)
{
    XmlEntry entry;

    if (!testStartTagOrEmptyTag(parser, entry, "PARAMETER.ARRAY"))
	return false;

    Boolean empty = entry.type == XmlEntry::EMPTY_TAG;

    // Get PARAMETER.ARRAY.NAME attribute:

    String name = getCimNameAttribute(
	parser.getLine(), entry, "PARAMETER.ARRAY");

    // Get PARAMETER.ARRAY.TYPE attribute:

    CIMType type;
    getCimTypeAttribute(parser.getLine(), entry, type, "PARAMETER.ARRAY");

    // Get PARAMETER.ARRAYSIZE attribute:

    Uint32 arraySize = 0;
    getArraySizeAttribute(parser.getLine(), entry, "PARAMETER.ARRAY",arraySize);

    // Create parameter:

    parameter = CIMParameter(name, type, true, arraySize);

    if (!empty)
    {
	getQualifierElements(parser, parameter);

	expectEndTag(parser, "PARAMETER.ARRAY");
    }

    return true;
}

//------------------------------------------------------------------------------
//
// getParameterReferenceElement()
//
//     <!ELEMENT PARAMETER.REFERENCE (QUALIFIER*)>
//     <!ATTLIST PARAMETER.REFERENCE
//         %CIMName;
//         %ReferenceClass;>
//
//------------------------------------------------------------------------------

Boolean XmlReader::getParameterReferenceElement(
    XmlParser& parser, 
    CIMParameter& parameter)
{
    XmlEntry entry;

    if (!testStartTagOrEmptyTag(parser, entry, "PARAMETER.REFERENCE"))
	return false;

    Boolean empty = entry.type == XmlEntry::EMPTY_TAG;

    // Get PARAMETER.NAME attribute:

    String name = getCimNameAttribute(
	parser.getLine(), entry, "PARAMETER.REFERENCE");

    // Get PARAMETER.REFERENCECLASS attribute:

    CIMName referenceClass = getReferenceClassAttribute(
	parser.getLine(), entry, "PARAMETER.REFERENCE");

    // Create parameter:

    parameter = CIMParameter(name, CIMTYPE_REFERENCE, false, 0, referenceClass);

    if (!empty)
    {
	getQualifierElements(parser, parameter);
	expectEndTag(parser, "PARAMETER.REFERENCE");
    }

    return true;
}

//------------------------------------------------------------------------------
//
// getParameterReferenceArrayElement()
//
//     <!ELEMENT PARAMETER.REFARRAY (QUALIFIER*)>
//     <!ATTLIST PARAMETER.REFARRAY
//         %CIMName;
//         %ReferenceClass;
//         %ArraySize;>
//
//------------------------------------------------------------------------------

Boolean XmlReader::getParameterReferenceArrayElement(
    XmlParser& parser, 
    CIMParameter& parameter)
{
    XmlEntry entry;

    if (!testStartTagOrEmptyTag(parser, entry, "PARAMETER.REFARRAY"))
	return false;

    Boolean empty = entry.type == XmlEntry::EMPTY_TAG;

    // Get PARAMETER.NAME attribute:

    String name = getCimNameAttribute(
	parser.getLine(), entry, "PARAMETER.REFARRAY");

    // Get PARAMETER.REFERENCECLASS attribute:

    CIMName referenceClass = getReferenceClassAttribute(
	parser.getLine(), entry, "PARAMETER.REFARRAY");

    // Get PARAMETER.ARRAYSIZE attribute:

    Uint32 arraySize = 0;
    getArraySizeAttribute(parser.getLine(), entry, "PARAMETER.REFARRAY",
			  arraySize);

    // Create parameter:

    parameter = CIMParameter(name, CIMTYPE_REFERENCE, true, arraySize,
			     referenceClass);

    if (!empty)
    {
	getQualifierElements(parser, parameter);
	expectEndTag(parser, "PARAMETER.REFARRAY");
    }

    return true;
}

//------------------------------------------------------------------------------
//
// GetParameterElements()
//
//------------------------------------------------------------------------------

template<class CONTAINER>
void GetParameterElements(XmlParser& parser, CONTAINER& container)
{
    CIMParameter parameter;

    while (XmlReader::getParameterElement(parser, parameter) ||
	XmlReader::getParameterArrayElement(parser, parameter) ||
	XmlReader::getParameterReferenceElement(parser, parameter) ||
	XmlReader::getParameterReferenceArrayElement(parser, parameter))
    {
	try
	{
	    container.addParameter(parameter);
	}
	catch (AlreadyExistsException&)
	{
	    throw XmlSemanticError(parser.getLine(), "duplicate parameter");
	}
    }
}

//------------------------------------------------------------------------------
//
// getQualifierDeclElement()
//
//     <!ELEMENT QUALIFIER.DECLARATION (SCOPE?,(VALUE|VALUE.ARRAY)?)>
//     <!ATTLIST QUALIFIER.DECLARATION 
//         %CIMName;               
//         %CIMType; #REQUIRED
//         ISARRAY (true|false) #IMPLIED
//         %ArraySize;
//         %QualifierFlavor;>
//         
//------------------------------------------------------------------------------

Boolean XmlReader::getQualifierDeclElement(
    XmlParser& parser, 
    CIMQualifierDecl& qualifierDecl)
{
    XmlEntry entry;

    if (!testStartTagOrEmptyTag(parser, entry, "QUALIFIER.DECLARATION"))
	return false;

    Boolean empty = entry.type == XmlEntry::EMPTY_TAG;

    // Get NAME attribute:

    String name = getCimNameAttribute(
	parser.getLine(), entry, "QUALIFIER.DECLARATION");

    // Get TYPE attribute:

    CIMType type;
    getCimTypeAttribute(parser.getLine(), entry, type, "QUALIFIER.DECLARATION");

    // Get ISARRAY attribute:

    Boolean isArray = getCimBooleanAttribute(
        parser.getLine(), entry, "QUALIFIER.DECLARATION", "ISARRAY",
        false, false); 

    // Get ARRAYSIZE attribute:

    Uint32 arraySize = 0;
    Boolean gotArraySize = getArraySizeAttribute(parser.getLine(),
	entry, "QUALIFIER.DECLARATION", arraySize);

    // Get flavor oriented attributes:

    CIMFlavor flavor = getFlavor (entry, parser.getLine (), 
        "QUALIFIER.DECLARATION");

    // No need to look for interior elements if empty tag:

    CIMScope scope = CIMScope ();
    CIMValue value;
    Boolean gotValue = false;

    if (!empty)
    {
	// Get the option SCOPE element:

	scope = getOptionalScope(parser);

	// Get VALUE or VALUE.ARRAY element:

	if (getValueArrayElement(parser, type, value))
	{
	    if (!isArray)
	    {
		throw XmlSemanticError(parser.getLine(),
		    "VALUE.ARRAY element encountered without "
		    "ISARRAY attribute");
	    }

	    if (arraySize && arraySize != value.getArraySize())
	    {
		throw XmlSemanticError(parser.getLine(),
		    "VALUE.ARRAY size is not the same as "
		    "ARRAYSIZE attribute");
	    }

            gotValue = true;
	}
	else if (getValueElement(parser, type, value))
	{
	    if (isArray)
	    {
		throw XmlSemanticError(parser.getLine(),
		    "ISARRAY attribute used but VALUE element encountered");
	    }

            gotValue = true;
	}

	// Now get the closing tag:

	expectEndTag(parser, "QUALIFIER.DECLARATION");
    }

    if (!gotValue)
    {
	if (isArray)
	    value.setNullValue(type, true, arraySize);
	else
	    value.setNullValue(type, false);
    }

    CIMQualifierDecl tmp(name, value, scope, flavor, arraySize);
    qualifierDecl = CIMQualifierDecl(name, value, scope, flavor, arraySize);
    return true;
}

//------------------------------------------------------------------------------
// getMethodElement()
//
//     <!ELEMENT METHOD (QUALIFIER*,(PARAMETER|PARAMETER.REFERENCE|
//         PARAMETER.ARRAY|PARAMETER.REFARRAY)*)>
//     <!ATTLIST METHOD
//         %CIMName;
//         %CIMType; #IMPLIED
//         %ClassOrigin;
//         %Propagated;>
//
//------------------------------------------------------------------------------

Boolean XmlReader::getMethodElement(XmlParser& parser, CIMMethod& method)
{
    XmlEntry entry;

    if (!testStartTagOrEmptyTag(parser, entry, "METHOD"))
	return false;

    Boolean empty = entry.type == XmlEntry::EMPTY_TAG;

    String name = getCimNameAttribute(parser.getLine(), entry, "PROPERTY");

    CIMType type;
    getCimTypeAttribute(parser.getLine(), entry, type, "PROPERTY");

    CIMName classOrigin = 
	getClassOriginAttribute(parser.getLine(), entry, "PROPERTY");

    Boolean propagated = getCimBooleanAttribute(
	parser.getLine(), entry, "PROPERTY", "PROPAGATED", false, false);

    method = CIMMethod(name, type, classOrigin, propagated);

    if (!empty)
    {
        // ATTN-RK-P2-20020219: Decoding algorithm must not depend on the
        // ordering of qualifiers and parameters.
	getQualifierElements(parser, method);

	GetParameterElements(parser, method);

	expectEndTag(parser, "METHOD");
    }

    return true;
}

//------------------------------------------------------------------------------
// getClassElement()
//
//     <!ELEMENT CLASS (QUALIFIER*,
//         (PROPERTY|PROPERTY.ARRAY|PROPERTY.REFERENCE)*,METHOD*)>
//     <!ATTLIST CLASS %CIMName; %SuperClass;>
//
//------------------------------------------------------------------------------

Boolean XmlReader::getClassElement(XmlParser& parser, CIMClass& cimClass)
{
    XmlEntry entry;

    if (!testStartTag(parser, entry, "CLASS"))
	return false;

    String name = getCimNameAttribute(parser.getLine(), entry, "CLASS");

    CIMName superClass = getSuperClassAttribute(parser.getLine(), entry,"CLASS");

    cimClass = CIMClass(name, superClass);

    // Get QUALIFIER elements:

    getQualifierElements(parser, cimClass);

    // Get PROPERTY elements:

    GetPropertyElements(parser, cimClass);

    // Get METHOD elements:

    CIMMethod method;

    while (getMethodElement(parser, method))
	cimClass.addMethod(method);	

    // Get CLASS end tag:

    expectEndTag(parser, "CLASS");

    return true;
}

//------------------------------------------------------------------------------
// getInstanceElement()
//
//     <!ELEMENT INSTANCE (QUALIFIER*,
//         (PROPERTY|PROPERTY.ARRAY|PROPERTY.REFERENCE)*) >
//     <!ATTLIST INSTANCE
//         %ClassName;>
//
//------------------------------------------------------------------------------

Boolean XmlReader::getInstanceElement(
    XmlParser& parser, 
    CIMInstance& cimInstance)
{
    XmlEntry entry;

    if (!testStartTag(parser, entry, "INSTANCE"))
	return false;

    String className = getClassNameAttribute(
	parser.getLine(), entry, "INSTANCE");

    cimInstance = CIMInstance(className);

    // Get QUALIFIER elements:

    getQualifierElements(parser, cimInstance);

    // Get PROPERTY elements:

    GetPropertyElements(parser, cimInstance);

    // Get INSTANCE end tag:

    expectEndTag(parser, "INSTANCE");

    return true;
}

//------------------------------------------------------------------------------
// getNamedInstanceElement()
//
//     <!ELEMENT VALUE.NAMEDINSTANCE (INSTANCENAME,INSTANCE)>
//
//------------------------------------------------------------------------------

Boolean XmlReader::getNamedInstanceElement(
    XmlParser& parser, 
    CIMInstance& namedInstance)
{
    XmlEntry entry;

    if (!testStartTag(parser, entry, "VALUE.NAMEDINSTANCE"))
	return false;

    CIMObjectPath instanceName;

    // Get INSTANCENAME elements:

    if (!getInstanceNameElement(parser, instanceName))
    {
	throw XmlValidationError(parser.getLine(), 
	    "expected INSTANCENAME element");
    }

    // Get INSTANCE elements:

    if (!getInstanceElement(parser, namedInstance))
    {
	throw XmlValidationError(parser.getLine(),
	    "expected INSTANCE element");
    }

    // Get VALUE.NAMEDINSTANCE end tag:

    expectEndTag(parser, "VALUE.NAMEDINSTANCE");

    namedInstance.setPath (instanceName);

    return true;
}

//------------------------------------------------------------------------------
//
// getObject()
//
//------------------------------------------------------------------------------

void XmlReader::getObject(XmlParser& parser, CIMClass& x)
{
    if (!getClassElement(parser, x))
    {
	throw XmlValidationError(parser.getLine(),
	    "expected CLASS element");
    }
}

//------------------------------------------------------------------------------
//
// getObject()
//
//------------------------------------------------------------------------------

void XmlReader::getObject(XmlParser& parser, CIMInstance& x)
{
    if (!getInstanceElement(parser, x))
    {
	throw XmlValidationError(parser.getLine(),
	    "expected INSTANCE element");
    }
}

//------------------------------------------------------------------------------
//
// getObject()
//
//------------------------------------------------------------------------------

void XmlReader::getObject(XmlParser& parser, CIMQualifierDecl& x)
{
    if (!getQualifierDeclElement(parser, x))
    {
	throw XmlValidationError(parser.getLine(),
	    "expected QUALIFIER.DECLARATION element");
    }
}

//------------------------------------------------------------------------------
//
// getMessageStartTag()
//
//------------------------------------------------------------------------------

Boolean XmlReader::getMessageStartTag(
    XmlParser& parser, 
    String& id,
    String& protocolVersion)
{
    XmlEntry entry;

    if (!testStartTag(parser, entry, "MESSAGE"))
	return false;

    // Get MESSAGE.ID:

    if (!entry.getAttributeValue("ID", id))
	throw XmlValidationError(parser.getLine(), 
	    "Invalid or missing MESSAGE.ID attribute");

    // Get MESSAGE.PROTOCOLVERSION:

    if (!entry.getAttributeValue("PROTOCOLVERSION", protocolVersion))
	throw XmlValidationError(parser.getLine(),
	    "Invalid or missing MESSAGE.PROTOCOLVERSION attribute");

    return true;
}

//------------------------------------------------------------------------------
//
// getIMethodCallStartTag()
//
//------------------------------------------------------------------------------

Boolean XmlReader::getIMethodCallStartTag(
    XmlParser& parser, 
    const char*& name)
{
    XmlEntry entry;

    if (!testStartTag(parser, entry, "IMETHODCALL"))
	return false;

    // Get IMETHODCALL.NAME attribute:

    if (!entry.getAttributeValue("NAME", name))
	throw XmlValidationError(parser.getLine(),
	    "Missing IMETHODCALL.NAME attribute");

    return true;
}

//------------------------------------------------------------------------------
//
// getIMethodResponseStartTag()
//
//------------------------------------------------------------------------------

Boolean XmlReader::getIMethodResponseStartTag(
    XmlParser& parser, 
    const char*& name)
{
    XmlEntry entry;

    if (!testStartTag(parser, entry, "IMETHODRESPONSE"))
	return false;

    // Get IMETHODRESPONSE.NAME attribute:

    if (!entry.getAttributeValue("NAME", name))
	throw XmlValidationError(parser.getLine(),
	    "Missing IMETHODRESPONSE.NAME attribute");

    return true;
}

//------------------------------------------------------------------------------
//
// getIParamValueTag()
//
//------------------------------------------------------------------------------

Boolean XmlReader::getIParamValueTag(
    XmlParser& parser, 
    const char*& name)
{
    XmlEntry entry;

    if (!testStartTag(parser, entry, "IPARAMVALUE"))
	return false;

    // Get IPARAMVALUE.NAME attribute:

    if (!entry.getAttributeValue("NAME", name))
	throw XmlValidationError(parser.getLine(),
	    "Missing IPARAMVALUE.NAME attribute");

    return true;
}

//------------------------------------------------------------------------------
//
// getBooleanValueElement()
//
//     Get an elements like: "<VALUE>FALSE</VALUE>"
//
//------------------------------------------------------------------------------

Boolean XmlReader::getBooleanValueElement(
    XmlParser& parser, 
    Boolean& result,
    Boolean required)
{
    XmlEntry entry;

    if (!testStartTag(parser, entry, "VALUE"))
    {
	if (required)
	{
	    throw XmlValidationError(parser.getLine(),
		"Expected VALUE element");
	}
	return false;
    }

    expectContentOrCData(parser, entry);

    if (CompareNoCase(entry.text, "TRUE") == 0)
	result = true;
    else if (CompareNoCase(entry.text, "FALSE") == 0)
	result = false;
    else
	throw XmlSemanticError(parser.getLine(), 
	    "Invalid value for VALUE element: must be \"TRUE\" or \"FALSE\"");

    expectEndTag(parser, "VALUE");

    return true;
}

//------------------------------------------------------------------------------
//
// getErrorElement()
//
//     <!ELEMENT ERROR EMPTY>
//     <!ATTLIST ERROR 
//         CODE CDATA #REQUIRED
//         DESCRIPTION CDATA #IMPLIED>
//
//------------------------------------------------------------------------------

Boolean XmlReader::getErrorElement(
    XmlParser& parser, 
    CIMException& cimException,
    Boolean required)
{
    XmlEntry entry;

    if (!testStartTagOrEmptyTag(parser, entry, "ERROR"))
    {
	if (required)
	    throw XmlValidationError(parser.getLine(),"Expected ERROR element");
	return false;
    }

    Boolean empty = entry.type == XmlEntry::EMPTY_TAG;

    // Get ERROR.CODE

    Uint32 tmpCode;

    if (!entry.getAttributeValue("CODE", tmpCode))
	throw XmlValidationError(
	    parser.getLine(), "missing ERROR.CODE attribute");

    // Get ERROR.DESCRIPTION:

    String tmpDescription;

    entry.getAttributeValue("DESCRIPTION", tmpDescription);

    if (!empty)
	expectEndTag(parser, "ERROR");

    cimException = PEGASUS_CIM_EXCEPTION(CIMStatusCode(tmpCode), tmpDescription);
    return true;
}


//------------------------------------------------------------------------------
// getValueObjectElement()
//
// <!ELEMENT VALUE.OBJECT (CLASS|INSTANCE)>
//
//------------------------------------------------------------------------------

Boolean XmlReader::getValueObjectElement(
    XmlParser& parser, 
    CIMObject& object)
{
    XmlEntry entry;

    if (!testStartTag(parser, entry, "VALUE.OBJECT"))
	return false;

    CIMInstance cimInstance;
    CIMClass cimClass;

    if (XmlReader::getInstanceElement(parser, cimInstance))
    {
	object = CIMObject(cimInstance);
    }
    else if (!XmlReader::getClassElement(parser, cimClass))
    {
	object = CIMObject(cimClass);
    }
    else
    {
        throw XmlValidationError(parser.getLine(),
            "Expected INSTANCE or CLASS element");
    }

    expectEndTag(parser, "VALUE.OBJECT");

    return true;
}

//------------------------------------------------------------------------------
// getValueObjectWithPathElement()
//
// <!ELEMENT VALUE.OBJECTWITHPATH ((CLASSPATH,CLASS)|(INSTANCEPATH,INSTANCE))>
//
//------------------------------------------------------------------------------

Boolean XmlReader::getValueObjectWithPathElement(
    XmlParser& parser, 
    CIMObject& objectWithPath)
{
    XmlEntry entry;

    if (!testStartTag(parser, entry, "VALUE.OBJECTWITHPATH"))
	return false;

    CIMObjectPath reference;
    Boolean isInstance = false;

    if (XmlReader::getInstancePathElement(parser, reference))
	isInstance = true;
    else if (!XmlReader::getClassPathElement(parser, reference))
    {
        throw XmlValidationError(parser.getLine(),
            "Expected INSTANCEPATH or CLASSPATH element");
    }

    if (isInstance)
    {
	CIMInstance cimInstance;

	if (!XmlReader::getInstanceElement(parser, cimInstance))
	{
	    throw XmlValidationError(parser.getLine(),
	        "Expected INSTANCE element");
	}
	objectWithPath = CIMObject (cimInstance);
        objectWithPath.setPath (reference);
    }
    else
    {
	CIMClass cimClass;

	if (!XmlReader::getClassElement(parser, cimClass))
	{
	    throw XmlValidationError(parser.getLine(),
		"Expected CLASS element");
	}
	objectWithPath = CIMObject (cimClass);
        objectWithPath.setPath (reference);
    }

    expectEndTag(parser, "VALUE.OBJECTWITHPATH");

    return true;
}

//------------------------------------------------------------------------------
// getValueObjectWithLocalPathElement()
//
// <!ELEMENT VALUE.OBJECTWITHLOCALPATH
//     ((LOCALCLASSPATH,CLASS)|(LOCALINSTANCEPATH,INSTANCE))>
//
//------------------------------------------------------------------------------

Boolean XmlReader::getValueObjectWithLocalPathElement(
    XmlParser& parser, 
    CIMObject& objectWithPath)
{
    XmlEntry entry;

    if (!testStartTag(parser, entry, "VALUE.OBJECTWITHLOCALPATH"))
	return false;

    CIMObjectPath reference;
    Boolean isInstance = false;

    if (XmlReader::getLocalInstancePathElement(parser, reference))
	isInstance = true;
    else if (!XmlReader::getLocalClassPathElement(parser, reference))
    {
        throw XmlValidationError(parser.getLine(),
	    "Expected LOCALINSTANCEPATH or LOCALCLASSPATH element");
    }

    if (isInstance)
    {
	CIMInstance cimInstance;

	if (!XmlReader::getInstanceElement(parser, cimInstance))
	{
	    throw XmlValidationError(parser.getLine(),
	        "Expected INSTANCE element");
	}
	objectWithPath = CIMObject (cimInstance);
	objectWithPath.setPath (reference);
    }
    else
    {
	CIMClass cimClass;

	if (!XmlReader::getClassElement(parser, cimClass))
	{
	    throw XmlValidationError(parser.getLine(),
		"Expected CLASS element");
	}
	objectWithPath = CIMObject (cimClass);
	objectWithPath.setPath (reference);
    }

    expectEndTag(parser, "VALUE.OBJECTWITHLOCALPATH");

    return true;
}

//------------------------------------------------------------------------------
// getObjectArray()
//
// <object>
//     (VALUE.OBJECT|VALUE.OBJECTWITHLOCALPATH|VALUE.OBJECTWITHPATH)
//
//------------------------------------------------------------------------------

void XmlReader::getObjectArray(
    XmlParser& parser, 
    Array<CIMObject>& objectArray)
{
    CIMObject object;
    CIMObject objectWithPath;

    objectArray.clear();

    if (getValueObjectElement(parser, object))
    {
        objectArray.append(object);
        while (getValueObjectElement(parser, object))
            objectArray.append(object);
    }
    else if (getValueObjectWithPathElement(parser, objectWithPath))
    {
        objectArray.append(objectWithPath);
        while (getValueObjectWithPathElement(parser, objectWithPath))
            objectArray.append(objectWithPath);
    }
    else if (getValueObjectWithLocalPathElement(parser, objectWithPath))
    {
        objectArray.append(objectWithPath);
        while (getValueObjectWithLocalPathElement(parser, objectWithPath))
            objectArray.append(objectWithPath);
    }
}

//------------------------------------------------------------------------------
//
// <objectName>: (CLASSNAME|INSTANCENAME)
//
//------------------------------------------------------------------------------

Boolean XmlReader::getObjectNameElement(
    XmlParser& parser, 
    CIMObjectPath& objectName)
{
    String className;

    if (getClassNameElement(parser, className, false))
    {
	objectName.set(String(), CIMNamespaceName(), className);
	return true;
    }
    else if (getInstanceNameElement(parser, objectName))
	return true;
    else
    {
	throw XmlValidationError(parser.getLine(),
	    "expected CLASSNAME or INSTANCENAME element");
    }

    return false;
}

//------------------------------------------------------------------------------
//
// <!ELEMENT OBJECTPATH (INSTANCEPATH|CLASSPATH)>
//
//------------------------------------------------------------------------------

Boolean XmlReader::getObjectPathElement(
    XmlParser& parser, 
    CIMObjectPath& objectPath)
{
    XmlEntry entry;

    if (!testStartTag(parser, entry, "OBJECTPATH"))
	return false;

    if (getClassPathElement(parser, objectPath))
    {
	expectEndTag(parser, "OBJECTPATH");
	return true;
    }
    else if (getInstancePathElement(parser, objectPath))
    {
	expectEndTag(parser, "OBJECTPATH");
	return true;
    }
    else
    {
	throw XmlValidationError(parser.getLine(),
	    "expected INSTANCEPATH or CLASSPATH element");
    }

    PEGASUS_UNREACHABLE ( return false; )
}

//------------------------------------------------------------------------------
//
// getEMethodCallStartTag()
//
//------------------------------------------------------------------------------

Boolean XmlReader::getEMethodCallStartTag(
    XmlParser& parser, 
    const char*& name)
{
    XmlEntry entry;

    if (!testStartTag(parser, entry, "EXPMETHODCALL"))
	return false;

    // Get EXPMETHODCALL.NAME attribute:

    if (!entry.getAttributeValue("NAME", name))
	throw XmlValidationError(parser.getLine(),
	    "Missing EXPMETHODCALL.NAME attribute");

    return true;
}

//------------------------------------------------------------------------------
//
// getEMethodResponseStartTag()
//
//------------------------------------------------------------------------------

Boolean XmlReader::getEMethodResponseStartTag(
    XmlParser& parser, 
    const char*& name)
{
    XmlEntry entry;

    if (!testStartTag(parser, entry, "EXPMETHODRESPONSE"))
	return false;

    // Get EXPMETHODRESPONSE.NAME attribute:

    if (!entry.getAttributeValue("NAME", name))
	throw XmlValidationError(parser.getLine(),
	    "Missing EXPMETHODRESPONSE.NAME attribute");

    return true;
}

//------------------------------------------------------------------------------
//
// getMethodCallStartTag()
//
//------------------------------------------------------------------------------

Boolean XmlReader::getMethodCallStartTag(
    XmlParser& parser, 
    const char*& name)
{
    XmlEntry entry;

    if (!testStartTag(parser, entry, "METHODCALL"))
	return false;

    // Get METHODCALL.NAME attribute:

    if (!entry.getAttributeValue("NAME", name))
	throw XmlValidationError(parser.getLine(),
	    "Missing METHODCALL.NAME attribute");

    return true;
}

//------------------------------------------------------------------------------
//
// getMethodResponseStartTag()
//
//------------------------------------------------------------------------------

Boolean XmlReader::getMethodResponseStartTag(
    XmlParser& parser, 
    const char*& name)
{
    XmlEntry entry;

    if (!testStartTag(parser, entry, "METHODRESPONSE"))
	return false;

    // Get METHODRESPONSE.NAME attribute:

    if (!entry.getAttributeValue("NAME", name))
	throw XmlValidationError(parser.getLine(),
	    "Missing METHODRESPONSE.NAME attribute");

    return true;
}

//------------------------------------------------------------------------------
//
// getParamValueElement()
//
// <!ELEMENT PARAMVALUE (VALUE|VALUE.REFERENCE|VALUE.ARRAY|VALUE.REFARRAY)?>
// <!ATTLIST PARAMVALUE
//      %CIMName;
//      %ParamType;>
//
//------------------------------------------------------------------------------

Boolean XmlReader::getParamValueElement(
    XmlParser& parser, 
    CIMParamValue& paramValue)
{
    XmlEntry entry;
    const char* name;
    CIMType type;
    CIMValue value;

    if (!testStartTagOrEmptyTag(parser, entry, "PARAMVALUE"))
	return false;

    Boolean empty = entry.type == XmlEntry::EMPTY_TAG;

    // Get PARAMVALUE.NAME attribute:

    if (!entry.getAttributeValue("NAME", name))
	throw XmlValidationError(parser.getLine(),
	    "Missing PARAMVALUE.NAME attribute");

    // Get PARAMVALUE.PARAMTYPE attribute:

    Boolean gotType = getCimTypeAttribute(parser.getLine(), entry, type,
                                          "PARAMVALUE", "PARAMTYPE", false);

    if (!empty)
    {
        // Parse VALUE.REFERENCE and VALUE.REFARRAY type
        if ( (type == CIMTYPE_REFERENCE) || !gotType )
        {
	    CIMObjectPath reference;
	    if (XmlReader::getValueReferenceElement(parser, reference))
	    {
	        value.set(reference);
	        type = CIMTYPE_REFERENCE;
                gotType = true;
	    }
            else if (XmlReader::getValueReferenceArrayElement(parser, value))
	    {
	        type = CIMTYPE_REFERENCE;
                gotType = true;
	    }
            // If type==reference but no VALUE.REFERENCE found, use null value
        }

        // Parse non-reference value
        if ( type != CIMTYPE_REFERENCE )
        {
            CIMType effectiveType;
            if (!gotType)
	    {
	        // If we don't know what type the value is, read it as a String
		effectiveType = CIMTYPE_STRING;
	    }
            else
	    {
		effectiveType = type;
	    }

            if ( !XmlReader::getValueArrayElement(parser, effectiveType, value) &&
	         !XmlReader::getValueElement(parser, effectiveType, value) )
	    {
	        value.clear();    // Isn't necessary; should already be cleared
	    }
        }

        expectEndTag(parser, "PARAMVALUE");
    }

    paramValue = CIMParamValue(name, value, gotType);

    return true;
}

//------------------------------------------------------------------------------
//
// getReturnValueElement()
//
// <!ELEMENT RETURNVALUE (VALUE|VALUE.REFERENCE)>
// <!ATTLIST RETURNVALUE
//      %ParamType;>
//
//------------------------------------------------------------------------------

Boolean XmlReader::getReturnValueElement(
    XmlParser& parser, 
    CIMValue& returnValue)
{
    XmlEntry entry;
    CIMType type;
    CIMValue value;

    if (!testStartTag(parser, entry, "RETURNVALUE"))
	return false;

    // Get RETURNVALUE.PARAMTYPE attribute:
    // NOTE: Array type return values are not allowed (2/20/02)

    Boolean gotType = getCimTypeAttribute(parser.getLine(), entry, type,
                                          "RETURNVALUE", "PARAMTYPE", false);

    // Parse VALUE.REFERENCE type
    if ( (type == CIMTYPE_REFERENCE) || !gotType )
    {
        CIMObjectPath reference;
        if (XmlReader::getValueReferenceElement(parser, reference))
        {
            returnValue.set(reference);
            type = CIMTYPE_REFERENCE;
            gotType = true;
        }
        else if (type == CIMTYPE_REFERENCE)
        {
            throw XmlValidationError(parser.getLine(),
                "expected VALUE.REFERENCE element");
        }
    }

    // Parse non-reference return value
    if ( type != CIMTYPE_REFERENCE )
    {
        if (!gotType)
        {
            // If we don't know what type the value is, read it as a String
            type = CIMTYPE_STRING;
        }

        if ( !XmlReader::getValueElement(parser, type, returnValue) )
        {
            throw XmlValidationError(parser.getLine(),
                "expected VALUE element");
        }
    }

    expectEndTag(parser, "RETURNVALUE");

    return true;
}

PEGASUS_NAMESPACE_END
