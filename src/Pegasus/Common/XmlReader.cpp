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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By: Carol Ann Krug Graves, Hewlett-Packard Company
//                  (carolann_graves@hp.com)
//              Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <cassert>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include "CIMName.h"
#include "XmlReader.h"
#include "XmlWriter.h"
#include "CIMQualifier.h"
#include "CIMQualifierDecl.h"
#include "CIMClass.h"
#include "CIMInstance.h"
#include "CIMObject.h"
#include "CIMNamedInstance.h"
#include "CIMParamValue.h"

PEGASUS_USING_STD;
PEGASUS_NAMESPACE_BEGIN

static const Uint32 MESSAGE_SIZE = 128;

//------------------------------------------------------------------------------
//
// expectXmlDeclaration()
//
//------------------------------------------------------------------------------

void XmlReader::expectXmlDeclaration(
    XmlParser& parser, 
    XmlEntry& entry)
{
    if (!parser.next(entry) ||
	entry.type != XmlEntry::XML_DECLARATION ||
	strcmp(entry.text, "xml") != 0)
    {
	throw XmlValidationError(parser.getLine(),
	    "Expected <?xml ... ?> style declaration");
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
// testEndTag>()
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
// testCimStartTag()
//
//     <!ELEMENT CIM (MESSAGE|DECLARATION)>
//     <!ATTRLIST CIM 
//         CIMVERSION CDATA #REQUIRED
//         DTDVERSION CDATA #REQUIRED>
//
//------------------------------------------------------------------------------

void XmlReader::testCimStartTag(XmlParser& parser)
{
    XmlEntry entry;
    XmlReader::expectStartTag(parser, entry, "CIM");

    const char* cimVersion;

    if (!entry.getAttributeValue("CIMVERSION", cimVersion))
	throw XmlValidationError(
	    parser.getLine(), "missing CIM.CIMVERSION attribute");

    if (strcmp(cimVersion, "2.0") != 0)
	throw XmlValidationError(parser.getLine(), 
	    "CIM.CIMVERSION attribute must be \"2.0\"");

    const char* dtdVersion;

    if (!entry.getAttributeValue("DTDVERSION", dtdVersion))
	throw XmlValidationError(
	    parser.getLine(), "missing CIM.DTDVERSION attribute");

    if (strcmp(dtdVersion, "2.0") != 0)
	throw XmlValidationError(parser.getLine(), 
	    "CIM.DTDVERSION attribute must be \"2.0\"");
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

String XmlReader::getClassOriginAttribute(
    Uint32 lineNumber, 
    const XmlEntry& entry,
    const char* tagName)
{
    String name;

    if (!entry.getAttributeValue("CLASSORIGIN", name))
	return String();

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

String XmlReader::getReferenceClassAttribute(
    Uint32 lineNumber, 
    const XmlEntry& entry,
    const char* elementName)
{
    String name;

    if (!entry.getAttributeValue("REFERENCECLASS", name))
	return String();

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

String XmlReader::getSuperClassAttribute(
    Uint32 lineNumber, 
    const XmlEntry& entry,
    const char* tagName)
{
    String superClass;

    if (!entry.getAttributeValue("SUPERCLASS", superClass))
	return String();

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

CIMType XmlReader::getCimTypeAttribute(
    Uint32 lineNumber, 
    const XmlEntry& entry, 
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
	    return CIMType::NONE;
	}
    }

    CIMType type = CIMType::NONE;

    if (strcmp(typeName, "boolean") == 0)
	type = CIMType::BOOLEAN;
    else if (strcmp(typeName, "string") == 0)
	type = CIMType::STRING;
    else if (strcmp(typeName, "char16") == 0)
	type = CIMType::CHAR16;
    else if (strcmp(typeName, "uint8") == 0)
	type = CIMType::UINT8;
    else if (strcmp(typeName, "sint8") == 0)
	type = CIMType::SINT8;
    else if (strcmp(typeName, "uint16") == 0)
	type = CIMType::UINT16;
    else if (strcmp(typeName, "sint16") == 0)
	type = CIMType::SINT16;
    else if (strcmp(typeName, "uint32") == 0)
	type = CIMType::UINT32;
    else if (strcmp(typeName, "sint32") == 0)
	type = CIMType::SINT32;
    else if (strcmp(typeName, "uint64") == 0)
	type = CIMType::UINT64;
    else if (strcmp(typeName, "sint64") == 0)
	type = CIMType::SINT64;
    else if (strcmp(typeName, "datetime") == 0)
	type = CIMType::DATETIME;
    else if (strcmp(typeName, "real32") == 0)
	type = CIMType::REAL32;
    else if (strcmp(typeName, "real64") == 0)
	type = CIMType::REAL64;
    else if (strcmp(typeName, "reference") == 0)
	type = CIMType::REFERENCE;

    if ((type == CIMType::NONE) ||
        ((type == CIMType::REFERENCE) &&
         (strcmp(attributeName, "PARAMTYPE") != 0)))
    {
	char message[MESSAGE_SIZE];
	sprintf(message, "Illegal value for %s.%s attribute", tagName,
	        attributeName);
	throw XmlSemanticError(lineNumber, message);
    }

    return type;
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
    sprintf(buffer, "Bad %s.%s attribute value", attributeName, tagName);
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

    char* end;
    x = strtod(stringValue, &end);
    return true;
}

//------------------------------------------------------------------------------
//
// stringToSignedInteger
//
//	[ "+" | "-" ] ( positiveDecimalDigit *decimalDigit | "0" )
//
// ATTN-B: handle conversion from hexadecimal.
//------------------------------------------------------------------------------

Boolean XmlReader::stringToSignedInteger(
    const char* stringValue, 
    Sint64& x)
{
    x = 0;
    const char* p = stringValue;

    if (!*p)
	return false;

    // Skip optional sign:

    Boolean negative = *p == '-';

    if (negative || *p == '+')
	p++;

    // If the next thing is a zero, then it must be the last:

    if (*p == '0')
	return p[1] == '\0';

    // Expect a positive decimal digit:

    const char* first = p;

    if (!isdigit(*p) || *p == '0')
	return false;

    p++;

    // Expect zero or more digits:

    while (isdigit(*p))
	p++;

    if (*p)
	return false;

    const char* last = p;

    while (first != last)
	x = 10 * x + (*first++ - '0');

    if (negative)
	x = -x;

    return true;
}

//------------------------------------------------------------------------------
//
// stringToUnsignedInteger
//
//	[ "+" | "-" ] ( positiveDecimalDigit *decimalDigit | "0" )
//
// ATTN-B: handle conversion from hexadecimal.
//------------------------------------------------------------------------------

Boolean XmlReader::stringToUnsignedInteger(
    const char* stringValue, 
    Uint64& x)
{
    x = 0;
    const char* p = stringValue;

    if (!*p)
	return false;

    // Skip optional sign:

    if (*p == '-')
	return false;

    if (*p == '+')
	p++;

    // If the next thing is a zero, then it must be the last:

    if (*p == '0')
	return p[1] == '\0';

    // Expect a positive decimal digit:

    const char* first = p;

    if (!isdigit(*p) || *p == '0')
	return false;

    p++;

    // Expect zero or more digits:

    while (isdigit(*p))
	p++;

    if (*p)
	return false;

    const char* last = p;

    while (first != last)
	x = 10 * x + (*first++ - '0');

    return true;
}

//------------------------------------------------------------------------------
//
// stringToValue()
//
// ATTN-C: note that integers are truncated without warning. What should be 
// done in this case? In C they are truncated without warning by design.
//
//------------------------------------------------------------------------------

CIMValue XmlReader::stringToValue(
    Uint32 lineNumber, 
    const char* valueString, 
    CIMType type)
{
    // ATTN-B: accepting only UTF-8 for now! (affects string and char16):

    if (strlen(valueString)==0) 
    {
        switch (type) 
	{
	    case CIMType::BOOLEAN: return CIMValue(false);
	    case CIMType::STRING: return CIMValue(valueString);
	    case CIMType::CHAR16: return CIMValue(Char16('\0'));
	    case CIMType::UINT8: return CIMValue(Uint8(0));
	    case CIMType::UINT16: return CIMValue(Uint16(0));
	    case CIMType::UINT32: return CIMValue(Uint32(0));
	    case CIMType::UINT64: return CIMValue(Uint64(0));
	    case CIMType::SINT8: return CIMValue(Sint8(0));
	    case CIMType::SINT16: return CIMValue(Sint16(0));
	    case CIMType::SINT32: return CIMValue(Sint32(0));
	    case CIMType::SINT64: return CIMValue(Sint64(0));
	    case CIMType::REAL32: return CIMValue(Real32(0));
	    case CIMType::REAL64: return CIMValue(Real64(0));
        }
    }

    switch (type)
    {
	case CIMType::BOOLEAN:
	{
	    if (CompareNoCase(valueString, "TRUE") == 0)
		return CIMValue(true);
	    else if (CompareNoCase(valueString, "FALSE") == 0)
		return CIMValue(false);
	    else
		throw XmlSemanticError(
		    lineNumber, "Bad boolean value");
	}

	case CIMType::STRING:
	{
	    return CIMValue(valueString);
	}

	case CIMType::CHAR16:
	{
	    if (strlen(valueString) != 1)
		throw XmlSemanticError(lineNumber, "Bad char16 value");

	    return CIMValue(Char16(valueString[0]));
	}

	case CIMType::UINT8:
	case CIMType::UINT16:
	case CIMType::UINT32:
	case CIMType::UINT64:
	{
	    Uint64 x;

	    if (!stringToUnsignedInteger(valueString, x))
	    {
		throw XmlSemanticError(
		    lineNumber, "Bad unsigned integer value");
	    }

	    switch (type)
	    {
		case CIMType::UINT8: return CIMValue(Uint8(x));
		case CIMType::UINT16: return CIMValue(Uint16(x));
		case CIMType::UINT32: return CIMValue(Uint32(x));
		case CIMType::UINT64: return CIMValue(Uint64(x));
		default: break;
	    }
	}

	case CIMType::SINT8:
	case CIMType::SINT16:
	case CIMType::SINT32:
	case CIMType::SINT64:
	{
	    Sint64 x;

	    if (!stringToSignedInteger(valueString, x))
	    {
		throw XmlSemanticError(
		    lineNumber, "Bad signed integer value");
	    }

	    switch (type)
	    {
		case CIMType::SINT8: return CIMValue(Sint8(x));
		case CIMType::SINT16: return CIMValue(Sint16(x));
		case CIMType::SINT32: return CIMValue(Sint32(x));
		case CIMType::SINT64: return CIMValue(Sint64(x));
		default: break;
	    }
	}

	case CIMType::DATETIME:
	{
	    CIMDateTime tmp;

	    try
	    {
		tmp.set(valueString);
	    }
	    catch (BadDateTimeFormat&)
	    {
		throw XmlSemanticError(lineNumber, "Bad datetime value");
	    }

	    return CIMValue(tmp);
	}

	case CIMType::REAL32:
	{
	    Real64 x;

	    if (!stringToReal(valueString, x))
		throw XmlSemanticError(lineNumber, "Bad real value");

	    return CIMValue(Real32(x));
	}

	case CIMType::REAL64:
	{
	    Real64 x;

	    if (!stringToReal(valueString, x))
		throw XmlSemanticError(lineNumber, "Bad real value");

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
//------------------------------------------------------------------------------

Boolean XmlReader::getValueElement(
    XmlParser& parser, 
    CIMType type, 
    CIMValue& value)
{
    // Get VALUE start tag:

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
//         <!ELEMENT VALUE (#PCDATA)>
//
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
    const CIMType type = CIMType::STRING;

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
    CIMReference reference;
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
	case CIMType::BOOLEAN: 
	    return StringArrayToValueAux(lineNumber, array, type, (Boolean*)0);

	case CIMType::STRING:
	    return StringArrayToValueAux(lineNumber, array, type, (String*)0);

	case CIMType::CHAR16:
	    return StringArrayToValueAux(lineNumber, array, type, (Char16*)0);

	case CIMType::UINT8:
	    return StringArrayToValueAux(lineNumber, array, type, (Uint8*)0);

	case CIMType::UINT16:
	    return StringArrayToValueAux(lineNumber, array, type, (Uint16*)0);

	case CIMType::UINT32:
	    return StringArrayToValueAux(lineNumber, array, type, (Uint32*)0);

	case CIMType::UINT64:
	    return StringArrayToValueAux(lineNumber, array, type, (Uint64*)0);

	case CIMType::SINT8:
	    return StringArrayToValueAux(lineNumber, array, type, (Sint8*)0);

	case CIMType::SINT16:
	    return StringArrayToValueAux(lineNumber, array, type, (Sint16*)0);

	case CIMType::SINT32:
	    return StringArrayToValueAux(lineNumber, array, type, (Sint32*)0);

	case CIMType::SINT64:
	    return StringArrayToValueAux(lineNumber, array, type, (Sint64*)0);

	case CIMType::DATETIME:
	    return StringArrayToValueAux(lineNumber, array, type, (CIMDateTime*)0);

	case CIMType::REAL32:
	    return StringArrayToValueAux(lineNumber, array, type, (Real32*)0);

	case CIMType::REAL64:
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
//------------------------------------------------------------------------------

Boolean XmlReader::getValueArrayElement(
    XmlParser& parser, 
    CIMType type, 
    CIMValue& value)
{
    value.clear();

    // Get VALUE.ARRAY open tag:

    XmlEntry entry;

    if (!testStartTagOrEmptyTag(parser, entry, "VALUE.ARRAY"))
	return false;

    if (entry.type == XmlEntry::EMPTY_TAG)
	return true;

    // For each VALUE element:

    Array<const char*> stringArray;

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

Uint32 XmlReader::getFlavor(
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

    Uint32 flavor = 0;

    if (overridable)
	flavor |= CIMFlavor::OVERRIDABLE;

    if (toSubClass)
	flavor |= CIMFlavor::TOSUBCLASS;

    if (toInstance)
	flavor |= CIMFlavor::TOINSTANCE;

    if (translatable)
	flavor |= CIMFlavor::TRANSLATABLE;

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

Uint32 XmlReader::getOptionalScope(XmlParser& parser)
{
    XmlEntry entry;

    if (!parser.next(entry))
	return false;

    Boolean isEmptyTag = entry.type == XmlEntry::EMPTY_TAG;

    if ((!isEmptyTag && 
	entry.type != XmlEntry::START_TAG) ||
	strcmp(entry.text, "SCOPE") != 0)
    {
	parser.putBack(entry);
	return 0;
    }

    Uint32 line = parser.getLine();
    Uint32 scope = 0;

    if (getCimBooleanAttribute(line, entry, "SCOPE", "CLASS", false, false))
	scope |= CIMScope::CLASS;

    if (getCimBooleanAttribute(
	line, entry, "SCOPE", "ASSOCIATION", false, false))
	scope |= CIMScope::ASSOCIATION;

    if (getCimBooleanAttribute(
	line, entry, "SCOPE", "REFERENCE", false, false))
	scope |= CIMScope::REFERENCE;

    if (getCimBooleanAttribute(line, entry, "SCOPE", "PROPERTY", false, false))
	scope |= CIMScope::PROPERTY;

    if (getCimBooleanAttribute(line, entry, "SCOPE", "METHOD", false, false))
	scope |= CIMScope::METHOD;

    if (getCimBooleanAttribute(line, entry, "SCOPE", "PARAMETER", false, false))
	scope |= CIMScope::PARAMETER;

    if (getCimBooleanAttribute(line, entry, "SCOPE", "INDICATION",false, false))
	scope |= CIMScope::INDICATION;

    if (!isEmptyTag)
	expectEndTag(parser, "SCOPE");

    return scope;
}

//------------------------------------------------------------------------------
//
// getQualifierElement()
//
//     <!ELEMENT QUALIFIER (VALUE|VALUE.ARRAY)>
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

    CIMType type = getCimTypeAttribute(parser.getLine(), entry, "QUALIFIER");

    // Get QUALIFIER.PROPAGATED

    Boolean propagated = getCimBooleanAttribute(
	parser.getLine(), entry, "QUALIFIER", "PROPAGATED", false, false);

    // Get flavor oriented attributes:

    Uint32 flavor = getFlavor(entry, parser.getLine(), "QUALIFIER");

    // Get VALUE or VALUE.ARRAY element:

    CIMValue value;

    if (!getValueElement(parser, type, value) &&
	!getValueArrayElement(parser, type, value))
    {
	throw XmlSemanticError(parser.getLine(),
	    "Expected VALUE or VALUE.ARRAY element");
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
	catch (AlreadyExists&)
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

    String classOrigin = 
	getClassOriginAttribute(parser.getLine(), entry, "PROPERTY");

    // Get PROPERTY.PROPAGATED

    Boolean propagated = getCimBooleanAttribute(
	parser.getLine(), entry, "PROPERTY", "PROPAGATED", false, false);

    // Get PROPERTY.TYPE attribute:

    CIMType type = getCimTypeAttribute(parser.getLine(), entry, "PROPERTY");

    // Create property:

    CIMValue value;
    value.setNullValue(type, false);
    property = CIMProperty(
	name, value, 0, String(), classOrigin, propagated);

    if (!empty)
    {
	// Get qualifiers:

	getQualifierElements(parser, property);

	// Get value:

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

    CIMType type = getCimTypeAttribute(parser.getLine(), entry, "PROPERTY.ARRAY");

    // Get PROPERTY.ARRAYSIZE attribute:

    Uint32 arraySize = 0;
    getArraySizeAttribute(parser.getLine(), entry, "PROPERTY.ARRAY", arraySize);

    // Get PROPERTY.CLASSORIGIN attribute:

    String classOrigin 
	= getClassOriginAttribute(parser.getLine(), entry, "PROPERTY.ARRAY");

    // Get PROPERTY.ARRAY.PROPAGATED

    Boolean propagated = getCimBooleanAttribute(
	parser.getLine(), entry, "PROPERTY.ARRAY", "PROPAGATED", false, false);

    // Create property:

    CIMValue nullValue;
    nullValue.setNullValue(type, true, arraySize);
    property = CIMProperty(
	name, nullValue, arraySize, String(), classOrigin, propagated);

    if (!empty)
    {
	// Get qualifiers:

	getQualifierElements(parser, property);

	// Get value:

	CIMValue value;

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
	    nameSpace += '/';

	nameSpace += nameSpaceComponent;
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
        CIMReference reference;

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
    CIMReference reference;

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
    CIMReference& instanceName)
{
    String className;
    Array<KeyBinding> keyBindings;

    if (!XmlReader::getInstanceNameElement(parser, className, keyBindings))
	return false;

    instanceName.set(String(), String(), className, keyBindings);
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
    CIMReference& reference)
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
    CIMReference& reference)
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
    CIMReference& reference)
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
    CIMReference& reference)
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
    CIMReference& reference)
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
	reference.set(String(), String(), className);
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
	reference.set(String(), String(), className, keyBindings);
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

    value.clear();

    // Get VALUE.REFARRAY open tag:

    if (!testStartTagOrEmptyTag(parser, entry, "VALUE.REFARRAY"))
	return false;

    if (entry.type == XmlEntry::EMPTY_TAG)
        // ATTN-RK-P3-20020220: Should the type and array size get set in
        // the value even though it is null?  (See also getValueArrayElement.)
	return true;

    // For each VALUE.REFERENCE element:

    Array<CIMReference> referenceArray;
    CIMReference reference;

    while (getValueReferenceElement(parser, reference))
    {
	referenceArray.append(reference);
    }

    expectEndTag(parser, "VALUE.REFARRAY");

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

    String referenceClass = getReferenceClassAttribute(
	parser.getLine(), entry, "PROPERTY.REFERENCE");

    // Get PROPERTY.CLASSORIGIN attribute:

    String classOrigin = 
	getClassOriginAttribute(parser.getLine(), entry, "PROPERTY.REFERENCE");

    // Get PROPERTY.PROPAGATED

    Boolean propagated = getCimBooleanAttribute(parser.getLine(), entry, 
	"PROPERTY.REFERENCE", "PROPAGATED", false, false);

    // Create property:

    CIMValue value;
    value.set(CIMReference());
    property = CIMProperty(
	name, value, 0, referenceClass, classOrigin, propagated);

    if (!empty)
    {
	getQualifierElements(parser, property);

	CIMReference reference;

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
	catch (AlreadyExists&)
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

    CIMType type = getCimTypeAttribute(parser.getLine(), entry, "PARAMETER");

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

    CIMType type = getCimTypeAttribute(parser.getLine(), entry, "PARAMETER.ARRAY");

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

    String referenceClass = getReferenceClassAttribute(
	parser.getLine(), entry, "PARAMETER.REFERENCE");

    // Create parameter:

    parameter = CIMParameter(name, CIMType::REFERENCE, false, 0, referenceClass);

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

    String referenceClass = getReferenceClassAttribute(
	parser.getLine(), entry, "PARAMETER.REFARRAY");

    // Get PARAMETER.ARRAYSIZE attribute:

    Uint32 arraySize = 0;
    getArraySizeAttribute(parser.getLine(), entry, "PARAMETER.REFARRAY",
			  arraySize);

    // Create parameter:

    parameter = CIMParameter(name, CIMType::REFERENCE, true, arraySize,
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
	catch (AlreadyExists&)
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

    CIMType type = getCimTypeAttribute(
	parser.getLine(), entry, "QUALIFIER.DECLARATION");

    // Get ISARRAY attribute:

    Boolean isArray = getCimBooleanAttribute(
        parser.getLine(), entry, "QUALIFIER.DECLARATION", "ISARRAY",
        false, false); 

    // Get ARRAYSIZE attribute:

    Uint32 arraySize = 0;
    Boolean gotArraySize = getArraySizeAttribute(parser.getLine(),
	entry, "QUALIFIER.DECLARATION", arraySize);

    // Get flavor oriented attributes:

    Uint32 flavor = getFlavor(entry,parser.getLine(), "QUALIFIER.DECLARATION");

    // No need to look for interior elements if empty tag:

    Uint32 scope = CIMScope::NONE;
    CIMValue value;

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
	}
	else if (getValueElement(parser, type, value))
	{
	    if (isArray)
	    {
		throw XmlSemanticError(parser.getLine(),
		    "ISARRAY attribute used but VALUE element encountered");
	    }
	}

	// Now get the closing tag:

	expectEndTag(parser, "QUALIFIER.DECLARATION");
    }

    if (value.getType() == CIMType::NONE)
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

    CIMType type = getCimTypeAttribute(parser.getLine(), entry, "PROPERTY");

    String classOrigin = 
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

    String superClass = getSuperClassAttribute(parser.getLine(), entry,"CLASS");

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
    CIMNamedInstance& namedInstance)
{
    XmlEntry entry;

    if (!testStartTag(parser, entry, "VALUE.NAMEDINSTANCE"))
	return false;

    CIMReference instanceName;

    // Get INSTANCENAME elements:

    if (!getInstanceNameElement(parser, instanceName))
    {
	throw XmlValidationError(parser.getLine(), 
	    "expected INSTANCENAME element");
    }

    CIMInstance instance;

    // Get INSTANCE elements:

    if (!getInstanceElement(parser, instance))
    {
	throw XmlValidationError(parser.getLine(),
	    "expected INSTANCE element");
    }

    // Get VALUE.NAMEDINSTANCE end tag:

    expectEndTag(parser, "VALUE.NAMEDINSTANCE");

    namedInstance.set(instanceName, instance);

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
    const char*& protocolVersion)
{
    XmlEntry entry;

    if (!testStartTag(parser, entry, "MESSAGE"))
	return false;

    // Get MESSAGE.ID:

    if (!entry.getAttributeValue("ID", id))
	throw XmlValidationError(parser.getLine(), 
	    "Bad or missing MESSAGE.ID attribute");

    // Get MESSAGE.PROTOCOLVERSION:

    if (!entry.getAttributeValue("PROTOCOLVERSION", protocolVersion))
	throw XmlValidationError(parser.getLine(),
	    "Bad or missing MESSAGE.PROTOCOLVERSION attribute");

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
	    "Bad value for VALUE element: must be \"TRUE\" or \"FALSE\"");

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
    CIMStatusCode& code,
    const char*& description,
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

    code = CIMStatusCode(tmpCode);

    // Get ERROR.DESCRIPTION:

    description = "";
    entry.getAttributeValue("DESCRIPTION", description);

    if (!empty)
	expectEndTag(parser, "ERROR");

    return true;
}


//------------------------------------------------------------------------------
// getObjectWithPath()
//
// <!ELEMENT VALUE.OBJECTWITHPATH ((CLASSPATH,CLASS)|(INSTANCEPATH,INSTANCE))>
//
//------------------------------------------------------------------------------

Boolean XmlReader::getObjectWithPath(
    XmlParser& parser, 
    CIMObjectWithPath& objectWithPath)
{
    XmlEntry entry;

    if (!testStartTag(parser, entry, "VALUE.OBJECTWITHPATH"))
	return false;

    CIMReference reference;
    Boolean isInstance = false;

    if (XmlReader::getInstancePathElement(parser, reference))
	isInstance = true;
    else if (!XmlReader::getClassPathElement(parser, reference))
    {
	throw XmlValidationError(parser.getLine(),
	    "Expected INSTANCE element");
    }

    if (isInstance)
    {
	CIMInstance cimInstance;

	if (!XmlReader::getInstanceElement(parser, cimInstance))
	{
	    throw XmlValidationError(parser.getLine(),
		"Expected INSTANCEPATH or CLASSPATH element");
	}
	objectWithPath.set(reference, CIMObject(cimInstance));
    }
    else
    {
	CIMClass cimClass;

	if (!XmlReader::getClassElement(parser, cimClass))
	{
	    throw XmlValidationError(parser.getLine(),
		"Expected CLASS element");
	}
	objectWithPath.set(reference, CIMObject(cimClass));
    }

    expectEndTag(parser, "VALUE.OBJECTWITHPATH");

    return true;
}

//------------------------------------------------------------------------------
//
// <objectName>: (CLASSNAME|INSTANCENAME)
//
//------------------------------------------------------------------------------

Boolean XmlReader::getObjectNameElement(
    XmlParser& parser, 
    CIMReference& objectName)
{
    String className;

    if (getClassNameElement(parser, className, false))
    {
	objectName.set(String(), String(), className);
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
    CIMReference& objectPath)
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

    type = getCimTypeAttribute(parser.getLine(), entry, "PARAMVALUE",
			       "PARAMTYPE", false);

    if (!empty)
    {
        // Parse VALUE.REFERENCE and VALUE.REFARRAY type
        if ( (type == CIMType::REFERENCE) || (type == CIMType::NONE) )
        {
	    CIMReference reference;
	    if (XmlReader::getValueReferenceElement(parser, reference))
	    {
	        value.set(reference);
	        type = CIMType::REFERENCE;
	    }
            else if (XmlReader::getValueReferenceArrayElement(parser, value))
	    {
	        type = CIMType::REFERENCE;
	    }
            // If type==reference but no VALUE.REFERENCE found, use null value
        }

        // Parse non-reference value
        if ( type != CIMType::REFERENCE )
        {
	    // If we don't know what type the value is, read it as a String
            CIMType effectiveType = type;
            if ( effectiveType == CIMType::NONE)
	    {
		effectiveType = CIMType::STRING;
	    }

            if ( !XmlReader::getValueArrayElement(parser, effectiveType, value) &&
	         !XmlReader::getValueElement(parser, effectiveType, value) )
	    {
	        value.clear();    // Isn't necessary; should already be cleared
	    }
        }

        expectEndTag(parser, "PARAMVALUE");
    }

    // ATTN-RK-P2-20020221: Any other properties to set in CIMParameter?
    // (referenceClassName)
    paramValue = CIMParamValue(CIMParameter(name, type, value.isArray(), value.getArraySize()), value);

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

    type = getCimTypeAttribute(parser.getLine(), entry, "RETURNVALUE",
			       "PARAMTYPE", false);

    // Parse VALUE.REFERENCE type
    if ( (type == CIMType::REFERENCE) || (type == CIMType::NONE) )
    {
        CIMReference reference;
        if (XmlReader::getValueReferenceElement(parser, reference))
        {
            returnValue.set(reference);
            type = CIMType::REFERENCE;
        }
        else if (type == CIMType::REFERENCE)
        {
            throw XmlValidationError(parser.getLine(),
                "expected VALUE.REFERENCE element");
        }
    }

    // Parse non-reference return value
    if ( type != CIMType::REFERENCE )
    {
        // If we don't know what type the value is, read it as a String
        if ( type == CIMType::NONE)
        {
            type = CIMType::STRING;
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
