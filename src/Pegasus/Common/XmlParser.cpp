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

////////////////////////////////////////////////////////////////////////////////
//
// XmlParser
//
//      This file contains a simple non-validating XML parser. Here are
//      serveral rules for well-formed XML:
//
//          1.  Documents must begin with an XML declaration:
//
//              <?xml version="1.0" standalone="yes"?>
//
//          2.  Comments have the form:
//
//              <!-- blah blah blah -->
//
//          3. The following entity references are supported:
//
//              &amp - ampersand
//              &lt - less-than
//              &gt - greater-than
//              &quot - full quote
//              &apos - apostrophe
//
//             as well as character (numeric) references:
//
//              &#49; - decimal reference for character '1'
//              &#x31; - hexadecimal reference for character '1'
//
//          4. Element names and attribute names take the following form:
//
//              [A-Za-z_][A-Za-z_0-9-.:]
//
//          5.  Arbitrary data (CDATA) can be enclosed like this:
//
//                  <![CDATA[
//                  ...
//                  ]]>
//
//          6.  Element names and attributes names are case-sensitive.
//
//          7.  XmlAttribute values must be delimited by full or half quotes.
//              XmlAttribute values must be delimited.
//
//          8.  <!DOCTYPE...>
//
// TODO:
//
//      ATTN: KS P1 4 Mar 2002. Review the following TODOs to see if there is
//      work. Handle <!DOCTYPE...> sections which are complicated (containing
//        rules rather than references to files).
//
//      Remove newlines from string literals:
//
//          Example: <xyz x="hello
//              world">
//
////////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "XmlParser.h"
#include "Logger.h"
#include "ExceptionRep.h"
#include "CharSet.h"

PEGASUS_NAMESPACE_BEGIN

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

// ATTN: Add support for more entity references
static EntityReference _references[] =
{
    { "&amp;", 5, '&' },
    { "&lt;", 4, '<' },
    { "&gt;", 4, '>' },
    { "&quot;", 6, '"' },
    { "&apos;", 6, '\'' }
};


// Implements a check for a whitespace character, without calling
// isspace( ).  The isspace( ) function is locale-sensitive,
// and incorrectly flags some chars above 0x7f as whitespace.  This
// causes the XmlParser to incorrectly parse UTF-8 data.
//
// Section 2.3 of XML 1.0 Standard (http://www.w3.org/TR/REC-xml)
// defines white space as:
// S    ::=    (#x20 | #x9 | #xD | #xA)+
static inline int _isspace(char c)
{
    return CharSet::isXmlWhiteSpace((Uint8)c);
}

static Uint32 _REFERENCES_SIZE = (sizeof(_references) / sizeof(_references[0]));

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

static const char* _xmlKeys[] =
{
    "Common.XmlParser.BAD_START_TAG",
    "Common.XmlParser.BAD_END_TAG",
    "Common.XmlParser.BAD_ATTRIBUTE_NAME",
    "Common.XmlParser.EXPECTED_EQUAL_SIGN",
    "Common.XmlParser.BAD_ATTRIBUTE_VALUE",
    "Common.XmlParser.MINUS_MINUS_IN_COMMENT",
    "Common.XmlParser.UNTERMINATED_COMMENT",
    "Common.XmlParser.UNTERMINATED_CDATA",
    "Common.XmlParser.UNTERMINATED_DOCTYPE",
    "Common.XmlParser.TOO_MANY_ATTRIBUTES",
    "Common.XmlParser.MALFORMED_REFERENCE",
    "Common.XmlParser.EXPECTED_COMMENT_OR_CDATA",
    "Common.XmlParser.START_END_MISMATCH",
    "Common.XmlParser.UNCLOSED_TAGS",
    "Common.XmlParser.MULTIPLE_ROOTS",
    "Common.XmlParser.VALIDATION_ERROR",
    "Common.XmlParser.SEMANTIC_ERROR"
};

// l10n replace _formMessage (comment out the old one)
/*
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
*/

static MessageLoaderParms _formMessage(
    Uint32 code,
    Uint32 line,
    const String& message)
{
    String dftMsg = _xmlMessages[Uint32(code) - 1];
    String key = _xmlKeys[Uint32(code) - 1];
        String msg = message;

    dftMsg.append(": on line $0");
    if (message.size())
    {
        msg = ": " + msg;
        dftMsg.append("$1");
    }

    return MessageLoaderParms(key, dftMsg, line ,msg);
}

static MessageLoaderParms _formPartialMessage(Uint32 code, Uint32 line)
{
    String dftMsg = _xmlMessages[Uint32(code) - 1];
    String key = _xmlKeys[Uint32(code) - 1];

    dftMsg.append(": on line $0");

    return MessageLoaderParms(key, dftMsg, line);
}


XmlException::XmlException(
    XmlException::Code code,
    Uint32 lineNumber,
    const String& message)
    : Exception(_formMessage(code, lineNumber, message))
{

}


XmlException::XmlException(
    XmlException::Code code,
    Uint32 lineNumber,
    MessageLoaderParms& msgParms)
    : Exception(_formPartialMessage(code, lineNumber))
{
        if (msgParms.default_msg.size())
    {
        msgParms.default_msg = ": " + msgParms.default_msg;
    }
        _rep->message.append(MessageLoader::getMessage(msgParms));
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


XmlValidationError::XmlValidationError(
    Uint32 lineNumber,
    MessageLoaderParms& msgParms)
    : XmlException(XmlException::VALIDATION_ERROR, lineNumber, msgParms)
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


XmlSemanticError::XmlSemanticError(
    Uint32 lineNumber,
    MessageLoaderParms& msgParms)
    : XmlException(XmlException::SEMANTIC_ERROR, lineNumber, msgParms)
{
}


////////////////////////////////////////////////////////////////////////////////
//
// XmlParser
//
////////////////////////////////////////////////////////////////////////////////

XmlParser::XmlParser(char* text)
    : _line(1),
      _current(text),
      _restoreChar('\0'),
      _foundRoot(false)
{
}

inline void _skipWhitespace(Uint32& line, char*& p)
{
    while (*p && _isspace(*p))
    {
        if (*p == '\n')
            line++;

        p++;
    }
}

#if defined(PEGASUS_PLATFORM_WIN64_IA64_MSVC) || \
    defined(PEGASUS_PLATFORM_WIN64_X86_64_MSVC)
#pragma optimize( "", off )
#endif
static int _getEntityRef(char*& p)
{
    if ((p[0] == 'g') && (p[1] == 't') && (p[2] == ';'))
    {
        p += 3;
        return '>';
    }

    if ((p[0] == 'l') && (p[1] == 't') && (p[2] == ';'))
    {
        p += 3;
        return '<';
    }

    if ((p[0] == 'a') && (p[1] == 'p') && (p[2] == 'o') && (p[3] == 's') &&
        (p[4] == ';'))
    {
        p += 5;
        return '\'';
    }

    if ((p[0] == 'q') && (p[1] == 'u') && (p[2] == 'o') && (p[3] == 't') &&
        (p[4] == ';'))
    {
        p += 5;
        return '"';
    }

    if ((p[0] == 'a') && (p[1] == 'm') && (p[2] == 'p') && (p[3] == ';'))
    {
        p += 4;
        return '&';
    }

    return -1;
}
#if defined(PEGASUS_PLATFORM_WIN64_IA64_MSVC) || \
    defined(PEGASUS_PLATFORM_WIN64_X86_64_MSVC)
#pragma optimize( "", on ) 
#endif

static inline int _getCharRef(char*& p, bool hex)
{
    char* end;
    unsigned long ch;

    if (hex)
    {
        ch = strtoul(p, &end, 16);
    }
    else
    {
        ch = strtoul(p, &end, 10);
    }

    if ((end == p) || (*end != ';') || (ch > 255))
    {
        return -1;
    }

    if ((hex && (end - p > 4)) || (!hex && (end - p > 5)))
    {
        return -1;
    }

    p = end + 1;

    return ch;
}

static void _normalize(Uint32& line, char*& p, char end_char, char*& start)
{
    // Skip over leading whitespace:

    _skipWhitespace(line, p);
    start = p;

    // Process one character at a time:

    char* q = p;

    while (*p && (*p != end_char))
    {
        if (_isspace(*p))
        {
            // Compress sequences of whitespace characters to a single space
            // character. Update line number when newlines encountered.

            if (*p++ == '\n')
            {
                line++;
            }

            *q++ = ' ';

            _skipWhitespace(line, p);
        }
        else if (*p == '&')
        {
            // Process entity characters and entity references:

            p++;
            int ch;

            if (*p == '#')
            {
                *p++;

                if (*p == 'x')
                {
                    p++;
                    ch = _getCharRef(p, true);
                }
                else
                {
                    ch = _getCharRef(p, false);
                }
            }
            else
            {
                ch = _getEntityRef(p);
            }

            if (ch == -1)
            {
                throw XmlException(XmlException::MALFORMED_REFERENCE, line);
            }

            *q++ = ch;
        }
        else
        {
            *q++ = *p++;
        }
    }

    // Remove single trailing whitespace (consecutive whitespaces already
    // compressed above).  Since p >= q, we can tell if we need to strip a
    // trailing space from q by looking at the end of p.  We must not look at
    // the last character of p, though, if p is an empty string.
    Boolean adjust_q = (p != start) && _isspace(p[-1]);

    // We encountered a the end_char or a zero-terminator.

    *q = *p;

    if (adjust_q)
    {
        q--;
    }

    // If q got behind p, it is safe and necessary to null-terminate q

    if (q != p)
    {
        *q = '\0';
    }
}

Boolean XmlParser::next(XmlEntry& entry, Boolean includeComment)
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

    // Loop until we are done with comments if includeComment is false.
    do
    {
        // Skip over any whitespace:
        _skipWhitespace(_line, _current);

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
        }
        else
        {
            // Normalize the content:

            char* start;
            _normalize(_line, _current, '<', start);

            // Get the content:

            entry.type = XmlEntry::CONTENT;
            entry.text = start;

            // Overwrite '<' with a null character (temporarily).

            _restoreChar = *_current;
            *_current = '\0';

            if (nullTerminator)
                *nullTerminator = '\0';
        }
    }while (!includeComment && entry.type == XmlEntry::COMMENT);

    return true;
}

void XmlParser::putBack(XmlEntry& entry)
{
    _putBackStack.push(entry);
}

XmlParser::~XmlParser()
{
    // Nothing to do!
}

// A-Za-z0-9_-:.
static unsigned char _isInnerElementChar[] =
{
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,1,1,0,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

Boolean XmlParser::_getElementName(char*& p)
{
    if (!CharSet::isAlNumUnder(Uint8(*p)))
        throw XmlException(XmlException::BAD_START_TAG, _line);

    p++;

    while (*p && _isInnerElementChar[Uint8(*p)])
        p++;

    // The next character must be a space:

    if (_isspace(*p))
    {
        *p++ = '\0';
        _skipWhitespace(_line, p);
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

    if (!CharSet::isAlNumUnder(Uint8(*p)))
        throw XmlException(XmlException::BAD_START_TAG, _line);

    p++;

    while (*p && _isInnerElementChar[Uint8(*p)])
        p++;

    // The next character must be a space:

    if (_isspace(*p))
    {
        *p++ = '\0';
        _skipWhitespace(_line, p);
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
    if (!CharSet::isAlNumUnder((Uint8)*p))
        throw XmlException(XmlException::BAD_ATTRIBUTE_NAME, _line);

    p++;

    while (*p && _isInnerElementChar[Uint8(*p)])
        p++;

    char* term = p;

    _skipWhitespace(_line, p);

    if (*p != '=')
        throw XmlException(XmlException::BAD_ATTRIBUTE_NAME, _line);

    p++;

    _skipWhitespace(_line, p);

    *term = '\0';
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
            entry.text = "";
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
    else if ((((*p >= 'A') && (*p <= 'Z')) ||
              ((*p >= 'a') && (*p <= 'z')) ||
              (*p == '_')))
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

        // Get the attribute value (e.g., "some value")
        {
            if ((*p != '"') && (*p != '\''))
            {
                throw XmlException(XmlException::BAD_ATTRIBUTE_VALUE, _line);
            }

            char quote = *p++;

            char* start;
            _normalize(_line, p, quote, start);
            attr.value = start;

            if (*p != quote)
            {
                throw XmlException(XmlException::BAD_ATTRIBUTE_VALUE, _line);
            }

            // Overwrite the closing quote with a null-terminator:

            *p++ = '\0';
        }

        if (entry.type == XmlEntry::XML_DECLARATION)
        {
            // The next thing must a space or a "?>":

            if (!(p[0] == '?' && p[1] == '>') && !_isspace(*p))
            {
                throw XmlException(
                    XmlException::BAD_ATTRIBUTE_VALUE, _line);
            }
        }
        else if (!(*p == '>' || (p[0] == '/' && p[1] == '>') || _isspace(*p)))
        {
            // The next thing must be a space or a '>':

            throw XmlException(XmlException::BAD_ATTRIBUTE_VALUE, _line);
        }

        _skipWhitespace(_line, p);

        if (entry.attributeCount == XmlEntry::MAX_ATTRIBUTES)
            throw XmlException(XmlException::TOO_MANY_ATTRIBUTES, _line);

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
//      "   87     "
//
// The first pointer would point to '8' and the last pointer woudl point one
// beyond '7'.

static void _findEnds(
    const char* str,
    const char*& first,
    const char*& last)
{
    first = str;

    while (_isspace(*first))
        first++;

    if (!*first)
    {
        last = first;
        return;
    }

    last = first + strlen(first);

    while (last != first && _isspace(last[-1]))
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

    value = static_cast<Real32>(tmp);
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

    value = String(tmp);
    return true;
}

void XmlAppendCString(Buffer& out, const char* str)
{
    out.append(str, static_cast<Uint32>(strlen(str)));
}

PEGASUS_NAMESPACE_END
