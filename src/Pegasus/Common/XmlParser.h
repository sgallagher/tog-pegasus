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
// Modified By: David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_XmlParser_h
#define Pegasus_XmlParser_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/ArrayInternal.h>
#include <Pegasus/Common/InternalException.h>
#include <Pegasus/Common/Stack.h>
#include <Pegasus/Common/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

class PEGASUS_COMMON_LINKAGE XmlException : public Exception
{
public:

    enum Code
    {
	BAD_START_TAG = 1,
	BAD_END_TAG,
	BAD_ATTRIBUTE_NAME,
	EXPECTED_EQUAL_SIGN,
	BAD_ATTRIBUTE_VALUE,
	MINUS_MINUS_IN_COMMENT,
	UNTERMINATED_COMMENT,
	UNTERMINATED_CDATA,
	UNTERMINATED_DOCTYPE,
	TOO_MANY_ATTRIBUTES,
	MALFORMED_REFERENCE,
	EXPECTED_COMMENT_OR_CDATA,
	START_END_MISMATCH,
	UNCLOSED_TAGS,
	MULTIPLE_ROOTS,
	VALIDATION_ERROR,
	SEMANTIC_ERROR
    };


    XmlException(
	Code code, 
	Uint32 lineNumber,
	const String& message = String());
	
	
    XmlException(
	Code code, 
	Uint32 lineNumber,
	MessageLoaderParms& msgParms);


    XmlException::Code getCode() const { return _code; }

private:

    Code _code;
};

class PEGASUS_COMMON_LINKAGE XmlValidationError : public XmlException
{
public:

    XmlValidationError(Uint32 lineNumber, const String& message);
    XmlValidationError(Uint32 lineNumber, MessageLoaderParms& msgParms);     
};

class PEGASUS_COMMON_LINKAGE XmlSemanticError : public XmlException
{
public:

    XmlSemanticError(Uint32 lineNumber, const String& message);
    XmlSemanticError(Uint32 lineNumber, MessageLoaderParms& msgParms);    
};

struct XmlAttribute
{
    const char* name;
    const char* value;
};

struct PEGASUS_COMMON_LINKAGE XmlEntry
{
    enum CIMType
    {
	XML_DECLARATION,
	START_TAG, 
	EMPTY_TAG, 
	END_TAG, 
	COMMENT,
	CDATA,
	DOCTYPE,
	CONTENT
    };

    enum { MAX_ATTRIBUTES = 10 };

    CIMType type;
    const char* text;
    XmlAttribute attributes[MAX_ATTRIBUTES];
    Uint32 attributeCount;

    void print() const;

    const XmlAttribute* findAttribute(const char* name) const;

    Boolean getAttributeValue(const char* name, Uint32& value) const;

    Boolean getAttributeValue(const char* name, Real32& value) const;

    Boolean getAttributeValue(const char* name, const char*& value) const;

    Boolean getAttributeValue(const char* name, String& value) const;
};

inline int operator==(const XmlEntry&, const XmlEntry&)
{
    return 0;
}

#define PEGASUS_ARRAY_T XmlEntry
# include <Pegasus/Common/ArrayInter.h>
#undef PEGASUS_ARRAY_T

class PEGASUS_COMMON_LINKAGE XmlParser
{
public:

    // Warning: this constructor modifies the text.

    XmlParser(char* text);

    Boolean next(XmlEntry& entry);

    void putBack(XmlEntry& entry);

    ~XmlParser();

    Uint32 getStackSize() const { return _stack.size(); }

    Uint32 getLine() const { return _line; }

private:

    void _skipWhitespace(char*& p);

    Boolean _getElementName(char*& p);

    Boolean _getOpenElementName(char*& p, Boolean& openCloseElement);

    void _getAttributeNameAndEqual(char*& p);

    void _getAttributeValue(char*& p);

    void _getComment(char*& p);

    void _getCData(char*& p);

    void _getDocType(char*& p);

    void _getContent(char*& p);

    void _substituteReferences(char* text);

    void _getElement(char*& p, XmlEntry& entry);

    Uint32 _line;
    char* _text;
    char* _current;
    char _restoreChar;
    Stack<char*> _stack;
    Boolean _foundRoot;
    Stack<XmlEntry> _putBackStack;
};

PEGASUS_COMMON_LINKAGE void XmlAppendCString(
    Array<char>& out, 
    const char* str);

PEGASUS_NAMESPACE_END

#endif /* Pegasus_XmlParser_h */
