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
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
// XmlParser
//
//	This file contains a simple non-validating XML parser. Here are 
//	serveral rules for well-formed XML:
//
//	    1.	Documents must begin with an XML declaration:
//
//		<?xml version="1.0" standalone="yes"?>
//
//	    2.	Comments have the form:
//
//		<!-- blah blah blah -->
//
//	    3. The following entity references are supported:
//
//		&amp - ampersand
//	 	&lt - less-than
//		&gt - greater-than
//		&quot - full quote
//		&apos - apostrophe
//
//	    4. Element names and attribute names take the following form:
//
//		[A-Za-z_][A-Za-z_0-9-.:]
//
//	    5.	Arbitrary data (CDATA) can be enclosed like this:
//
//		    <![CDATA[
//		    ...
//		    ]]>
//
//	    6.	Element names and attributes names are case-sensitive.
//
//	    7.	XmlAttribute values must be delimited by full or half quotes.
//		XmlAttribute values must be delimited.
//
//	    8.  <!DOCTYPE...>
//
// TODO:
//
//      ATTN: KS P1 4 Mar 2002. Review the following TODOs to see if there is work.
//	Handle <!DOCTYPE...> sections which are complicated (containing
//        rules rather than references to files).
//
//	Handle reference of this form: "&#913;"
//
//	Remove newlines from string literals:
//
//          Example: <xyz x="hello
//		world">
//
////////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <cctype>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "XmlParser.h"
#include "Logger.h"

PEGASUS_NAMESPACE_BEGIN

#define PEGASUS_ARRAY_T XmlEntry
# include "ArrayImpl.h"
#undef PEGASUS_ARRAY_T


////////////////////////////////////////////////////////////////////////////////
//
// Static helper functions
//
////////////////////////////////////////////////////////////////////////////////

static void _printValue(const char* p)
{
    for (; *p; p++)
    {
	if (*p == '\n')
	    PEGASUS_STD(cout) << "\\n";
	else if (*p == '\r')
	    PEGASUS_STD(cout) << "\\r";
	else if (*p == '\t')
	    PEGASUS_STD(cout) << "\\t";
	else
	    PEGASUS_STD(cout) << *p;
    }
}

struct EntityReference
{
    const char* match;
    Uint32 length;
    char replacement;
};

static EntityReference _references[] =
{
    { "&amp;", 5, '&' },
    { "&lt;", 4, '<' },
    { "&gt;", 4, '>' },
    { "&quot;", 6, '"' },
    { "&apos;", 6, '\'' }
};

static Uint32 _REFERENCES_SIZE = (sizeof(_references) / sizeof(_references[0]));

// Remove all redundant spaces from the given string:

static void _normalize(char* text)
{
    Uint32 length = strlen(text);
    char* p = text;
    char* end = p + length;

    // Remove leading spaces:

    while (isspace(*p))
	p++;

    if (p != text)
	memmove(text, p, end - p + 1);

    p = text;

    // Look for sequences of more than one space and remove all but one.

    for (;;)
    {
	// Advance to the next space:

	while (*p && !isspace(*p))
	    p++;

	if (!*p)
	    break;

	// Advance to the next non-space:

	char* q = p++;

	while (isspace(*p))
	    p++;

	// Discard trailing spaces (if we are at the end):

	if (!*p)
	{
	    *q = '\0';
	    break;
	}

	// Remove the redundant spaces:

	Uint32 n = p - q;

	if (n > 1)
	{
	    *q++ = ' ';
	    memmove(q, p, end - p + 1);
	    p = q;
	}
    }
}

////////////////////////////////////////////////////////////////////////////////
//
// XmlException
//
////////////////////////////////////////////////////////////////////////////////

static const char* _xmlMessages[] =
{
    "Bad opening element",
    "Bad closing element",
    "Bad attribute name",
    "Exepected equal sign",
    "Bad attribute value",
    "A \"--\" sequence found within comment",
    "Unterminated comment",
    "Unterminated CDATA block",
    "Unterminated DOCTYPE",
    "Too many attributes: parser only handles 10",
    "Malformed reference",
    "Expected a comment or CDATA following \"<!\" sequence",
    "Closing element does not match opening element",
    "One or more tags are still open",
    "More than one root element was encountered",
    "Validation error",
    "Semantic error"
};

static String _formMessage(Uint32 code, Uint32 line, const String& message)
{
    String result = _xmlMessages[Uint32(code) - 1];

    char buffer[32];
    sprintf(buffer, "%d", line);
    result.append(": on line ");
    result.append(buffer);

    if (message.size())
    {
	result.append(": ");
	result.append(message);
    }

    return result;
}

XmlException::XmlException(
    XmlException::Code code, 
    Uint32 lineNumber,
    const String& message) 
    : Exception(_formMessage(code, lineNumber, message))
{

}

////////////////////////////////////////////////////////////////////////////////
//
// XmlValidationError
//
////////////////////////////////////////////////////////////////////////////////

XmlValidationError::XmlValidationError(
    Uint32 lineNumber,
    const String& message)
    : XmlException(XmlException::VALIDATION_ERROR, lineNumber, message)
{

}

////////////////////////////////////////////////////////////////////////////////
//
// XmlSemanticError
//
////////////////////////////////////////////////////////////////////////////////

XmlSemanticError::XmlSemanticError(
    Uint32 lineNumber,
    const String& message)
    : XmlException(XmlException::SEMANTIC_ERROR, lineNumber, message)
{

}

////////////////////////////////////////////////////////////////////////////////
//
// XmlParser
//
////////////////////////////////////////////////////////////////////////////////

XmlParser::XmlParser(char* text) : _line(1), _text(text), _current(text), 
    _restoreChar('\0'), _foundRoot(false)
{

}

Boolean XmlParser::next(XmlEntry& entry)
{
    if (!_putBackStack.isEmpty())
    {
	entry = _putBackStack.top();
	_putBackStack.pop();
	return true;
    }

    // If a character was overwritten with a null-terminator the last
    // time this routine was called, then put back that character. Before
    // exiting of course, restore the null-terminator.

    char* nullTerminator = 0;

    if (_restoreChar && !*_current)
    {
	nullTerminator = _current;
	*_current = _restoreChar;
	_restoreChar = '\0';
    }

    // Skip over any whitespace:

    _skipWhitespace(_current);

    if (!*_current)
    {
	if (nullTerminator)
	    *nullTerminator = '\0';

	if (!_stack.isEmpty())
	    throw XmlException(XmlException::UNCLOSED_TAGS, _line);

	return false;
    }

    // Either a "<...>" or content begins next:

    if (*_current == '<')
    {
	_current++;
	_getElement(_current, entry);

	if (nullTerminator)
	    *nullTerminator = '\0';

	if (entry.type == XmlEntry::START_TAG)
	{
	    if (_stack.isEmpty() && _foundRoot)
		throw XmlException(XmlException::MULTIPLE_ROOTS, _line);

	    _foundRoot = true;
	    _stack.push((char*)entry.text);
	}
	else if (entry.type == XmlEntry::END_TAG)
	{
	    if (_stack.isEmpty())
		throw XmlException(XmlException::START_END_MISMATCH, _line);

	    if (strcmp(_stack.top(), entry.text) != 0)
		throw XmlException(XmlException::START_END_MISMATCH, _line);

	    _stack.pop();
	}

	return true;
    }
    else
    {
	entry.type = XmlEntry::CONTENT;
	entry.text = _current;
	_getContent(_current);
	_restoreChar = *_current;
	*_current = '\0';

	if (nullTerminator)
	    *nullTerminator = '\0';

	_substituteReferences((char*)entry.text);
	_normalize((char*)entry.text);

	return true;
    }
}

void XmlParser::putBack(XmlEntry& entry)
{
    _putBackStack.push(entry);
}

XmlParser::~XmlParser()
{
    // Nothing to do!
}

void XmlParser::_skipWhitespace(char*& p)
{
    while (*p && isspace(*p))
    {
	if (*p == '\n')
	    _line++;

	p++;
    }
}

Boolean XmlParser::_getElementName(char*& p)
{
    if (!isalpha(*p) && *p != '_')
	throw XmlException(XmlException::BAD_START_TAG, _line);

    while (*p && 
	(isalnum(*p) || *p == '_' || *p == '-' || *p == ':' || *p == '.'))
	p++;

    // The next character must be a space:

    if (isspace(*p))
    {
	*p++ = '\0';
	_skipWhitespace(p);
    }

    if (*p == '>')
    {
	*p++ = '\0';
	return true;
    }

    return false;
}

Boolean XmlParser::_getOpenElementName(char*& p, Boolean& openCloseElement)
{
    openCloseElement = false;

    if (!isalpha(*p) && *p != '_')
	throw XmlException(XmlException::BAD_START_TAG, _line);

    while (*p && 
	(isalnum(*p) || *p == '_' || *p == '-' || *p == ':' || *p == '.'))
	p++;

    // The next character must be a space:

    if (isspace(*p))
    {
	*p++ = '\0';
	_skipWhitespace(p);
    }

    if (*p == '>')
    {
	*p++ = '\0';
	return true;
    }

    if (p[0] == '/' && p[1] == '>')
    {
	openCloseElement = true;
	*p = '\0';
	p += 2;
	return true;
    }

    return false;
}

void XmlParser::_getAttributeNameAndEqual(char*& p)
{
    if (!isalpha(*p) && *p != '_')
	throw XmlException(XmlException::BAD_ATTRIBUTE_NAME, _line);

    while (*p && 
	(isalnum(*p) || *p == '_' || *p == '-' || *p == ':' || *p == '.'))
	p++;

    char* term = p;

    _skipWhitespace(p);

    if (*p != '=')
	throw XmlException(XmlException::BAD_ATTRIBUTE_NAME, _line);

    p++;

    _skipWhitespace(p);

    *term = '\0';
}

void XmlParser::_getAttributeValue(char*& p)
{
    // ATTN-B: handle values contained in semiquotes:

    if (*p != '"' && *p != '\'')
	throw XmlException(XmlException::BAD_ATTRIBUTE_VALUE, _line);

    char startChar = *p++;

    while (*p && *p != startChar)
	p++;

    if (*p != startChar)
	throw XmlException(XmlException::BAD_ATTRIBUTE_VALUE, _line);

    *p++ = '\0';
}

void XmlParser::_getComment(char*& p)
{
    // Now p points to first non-whitespace character beyond "<--" sequence:

    for (; *p; p++)
    {
	if (p[0] == '-' && p[1] == '-')
	{
	    if (p[2] != '>')
	    {
		throw XmlException(
		    XmlException::MINUS_MINUS_IN_COMMENT, _line);
	    }

	    // Find end of comment (excluding whitespace):

	    *p = '\0';
	    p += 3;
	    return;
	}
    }

    // If it got this far, then the comment is unterminated:

    throw XmlException(XmlException::UNTERMINATED_COMMENT, _line);
}

void XmlParser::_getCData(char*& p)
{
    // At this point p points one past "<![CDATA[" sequence:

    for (; *p; p++)
    {
	if (p[0] == ']' && p[1] == ']' && p[2] == '>')
	{
	    *p = '\0';
	    p += 3;
	    return;
	}
	else if (*p == '\n')
	    _line++;
    }

    // If it got this far, then the comment is unterminated:

    throw XmlException(XmlException::UNTERMINATED_CDATA, _line);
}

void XmlParser::_getDocType(char*& p)
{
    // Just ignore the DOCTYPE command for now:

    for (; *p && *p != '>'; p++)
    {
	if (*p == '\n')
	    _line++;
    }

    if (*p != '>')
	throw XmlException(XmlException::UNTERMINATED_DOCTYPE, _line);

    p++;
}

void XmlParser::_getContent(char*& p)
{
    while (*p && *p != '<')
    {
	if (*p == '\n')
	    _line++;

	p++;
    }
}

void XmlParser::_substituteReferences(char* text)
{
    Uint32 rem = strlen(text);

    for (char* p = text; *p; p++, rem--)
    {
	if (*p == '&')
	{
	    // Look for predefined entity reference:

	    Boolean found = false;

	    for (Uint32 i = 0; i < _REFERENCES_SIZE; i++)
	    {
		Uint32 length = _references[i].length;
		const char* match = _references[i].match;

		if (strncmp(p, _references[i].match, length) == 0)
		{
		    found = true;
		    *p = _references[i].replacement;
		    char* q = p + length;
		    rem = rem - length + 1;
		    memmove(p + 1, q, rem);
		}
	    }

	    // If not found, then at least make sure it is well formed:

	    if (!found)
	    {
		char* start = p;
		p++;

		XmlException::Code code = XmlException::MALFORMED_REFERENCE;

		if (isalpha(*p) || *p == '_')
		{
		    for (p++; *p && *p != ';'; p++)
		    {
			if (!isalnum(*p) && *p != '_')
			    throw XmlException(code, _line);
		    }
		}
		else if (*p == '#')
		{
		    for (p++ ; *p && *p != ';'; p++)
		    {
			if (!isdigit(*p))
			    throw XmlException(code, _line);
		    } 
		}

		if (*p != ';')
		    throw XmlException(code, _line);

		rem -= p - start;
	    }
	}
    }
}

static const char _EMPTY_STRING[] = "";

void XmlParser::_getElement(char*& p, XmlEntry& entry)
{
    entry.attributeCount = 0;

    //--------------------------------------------------------------------------
    // Get the element name (expect one of these: '?', '!', [A-Za-z_])
    //--------------------------------------------------------------------------

    if (*p == '?')
    {
	entry.type = XmlEntry::XML_DECLARATION;
	entry.text = ++p;

	Boolean openCloseElement = false;

	if (_getElementName(p))
	    return;
    }
    else if (*p == '!')
    {
	p++;

	// Expect a comment or CDATA:

	if (p[0] == '-' && p[1] == '-')
	{
	    p += 2;
	    entry.type = XmlEntry::COMMENT;
	    entry.text = p;
	    _getComment(p);
	    return;
	}
	else if (memcmp(p, "[CDATA[", 7) == 0)
	{
	    p += 7;
	    entry.type = XmlEntry::CDATA;
	    entry.text = p;
	    _getCData(p);
	    return;
	}
	else if (memcmp(p, "DOCTYPE", 7) == 0)
	{
	    entry.type = XmlEntry::DOCTYPE;
	    entry.text = _EMPTY_STRING;
	    _getDocType(p);
	    return;
	}
	throw(XmlException(XmlException::EXPECTED_COMMENT_OR_CDATA, _line));
    }
    else if (*p == '/')
    {
	entry.type = XmlEntry::END_TAG;
	entry.text = ++p;

	if (!_getElementName(p))
	    throw(XmlException(XmlException::BAD_END_TAG, _line));

	return;
    }
    else if (isalpha(*p) || *p == '_')
    {
	entry.type = XmlEntry::START_TAG;
	entry.text = p;

	Boolean openCloseElement = false;

	if (_getOpenElementName(p, openCloseElement))
	{
	    if (openCloseElement)
		entry.type = XmlEntry::EMPTY_TAG;
	    return;
	}
    }
    else
	throw XmlException(XmlException::BAD_START_TAG, _line);

    //--------------------------------------------------------------------------
    // Grab all the attributes:
    //--------------------------------------------------------------------------

    for (;;)
    {
	if (entry.type == XmlEntry::XML_DECLARATION)
	{
	    if (p[0] == '?' && p[1] == '>')
	    {
		p += 2;
		return;
	    }
	}
	else if (entry.type == XmlEntry::START_TAG && p[0] == '/' && p[1] =='>')
	{
	    entry.type = XmlEntry::EMPTY_TAG;
	    p += 2;
	    return;
	}
	else if (*p == '>')
	{
	    p++;
	    return;
	}

	XmlAttribute attr;
	attr.name = p;
	_getAttributeNameAndEqual(p);

	if (*p != '"' && *p != '\'')
	    throw XmlException(XmlException::BAD_ATTRIBUTE_VALUE, _line);

	attr.value = p + 1;
	_getAttributeValue(p);

	if (entry.type == XmlEntry::XML_DECLARATION)
	{
	    // The next thing must a space or a "?>":

	    if (!(p[0] == '?' && p[1] == '>') && !isspace(*p))
	    {
		throw XmlException(
		    XmlException::BAD_ATTRIBUTE_VALUE, _line);
	    }
	}
	else if (!(*p == '>' || (p[0] == '/' && p[1] == '>') || isspace(*p)))
	{
	    // The next thing must be a space or a '>':

	    throw XmlException(XmlException::BAD_ATTRIBUTE_VALUE, _line);
	}

	_skipWhitespace(p);

	if (entry.attributeCount == XmlEntry::MAX_ATTRIBUTES)
	    throw XmlException(XmlException::TOO_MANY_ATTRIBUTES, _line);

	_substituteReferences((char*)attr.value);
	entry.attributes[entry.attributeCount++] = attr;
    }
}

static const char* _typeStrings[] =
{
    "XML_DECLARATION", 
    "START_TAG", 
    "EMPTY_TAG", 
    "END_TAG", 
    "COMMENT",
    "CDATA",
    "DOCTYPE",
    "CONTENT" 
};

void XmlEntry::print() const
{
    PEGASUS_STD(cout) << "=== " << _typeStrings[type] << " ";

    Boolean needQuotes = type == XmlEntry::CDATA || type == XmlEntry::CONTENT;

    if (needQuotes)
	PEGASUS_STD(cout) << "\"";
	
    _printValue(text);

    if (needQuotes)
	PEGASUS_STD(cout) << "\"";

    PEGASUS_STD(cout) << '\n';

    for (Uint32 i = 0; i < attributeCount; i++)
    {
	PEGASUS_STD(cout) << "    " << attributes[i].name << "=\"";
	_printValue(attributes[i].value);
	PEGASUS_STD(cout) << "\"" << PEGASUS_STD(endl);
    }
}

const XmlAttribute* XmlEntry::findAttribute(
    const char* name) const
{
    for (Uint32 i = 0; i < attributeCount; i++)
    {
	if (strcmp(attributes[i].name, name) == 0)
	    return &attributes[i];
    }

    return 0;
}

// Find first non-whitespace character (set first) and last non-whitespace
// character (set last one past this). For example, consider this string:
//
//	"   87     "
//
// The first pointer would point to '8' and the last pointer woudl point one
// beyond '7'.

static void _findEnds(
    const char* str, 
    const char*& first, 
    const char*& last)
{
    first = str;

    while (isspace(*first))
	first++;

    if (!*first)
    {
	last = first;
	return;
    }

    last = first + strlen(first);

    while (last != first && isspace(last[-1]))
	last--;
}

Boolean XmlEntry::getAttributeValue(
    const char* name, 
    Uint32& value) const
{
    const XmlAttribute* attr = findAttribute(name);

    if (!attr)
	return false;

    const char* first;
    const char* last;
    _findEnds(attr->value, first, last);

    char* end = 0;
    long tmp = strtol(first, &end, 10);

    if (!end || end != last)
	return false;

    value = Uint32(tmp);
    return true;
}

Boolean XmlEntry::getAttributeValue(
    const char* name, 
    Real32& value) const
{
    const XmlAttribute* attr = findAttribute(name);

    if (!attr)
	return false;

    const char* first;
    const char* last;
    _findEnds(attr->value, first, last);

    char* end = 0;
    double tmp = strtod(first, &end);

    if (!end || end != last)
	return false;

    value = Uint32(tmp);
    return true;
}

Boolean XmlEntry::getAttributeValue(
    const char* name, 
    const char*& value) const
{
    const XmlAttribute* attr = findAttribute(name);

    if (!attr)
	return false;

    value = attr->value;
    return true;
}

Boolean XmlEntry::getAttributeValue(const char* name, String& value) const
{
    const char* tmp;

    if (!getAttributeValue(name, tmp))
	return false;

    value = tmp;
    return true;
}

void XmlAppendCString(Array<Sint8>& out, const char* str)
{
    out.append(str, strlen(str));
}

PEGASUS_NAMESPACE_END
