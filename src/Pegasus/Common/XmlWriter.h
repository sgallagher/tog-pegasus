//BEGIN_LICENSE
//
// Copyright (c) 2000 The Open Group, BMC Software, Tivoli Systems, IBM
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
//END_LICENSE
//BEGIN_HISTORY
//
// Author:
//
// $Log: XmlWriter.h,v $
// Revision 1.1  2001/01/14 19:53:36  mike
// Initial revision
//
//
//END_HISTORY

////////////////////////////////////////////////////////////////////////////////
//
// XmlWriter.h
//
////////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_XmlWriter_h
#define Pegasus_XmlWriter_h

#include <iostream>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Indentor.h>

PEGASUS_NAMESPACE_BEGIN

class ConstQualifierDecl;
class ConstClassDecl;

class PEGASUS_COMMON_LINKAGE XmlWriter
{
public:

    static void append(Array<Sint8>& out, Char16 x);

    static void append(Array<Sint8>& out, char x)
    {
	append(out, Char16(x));
    }

    static void append(Array<Sint8>& out, const char* x);

    static void append(Array<Sint8>& out, const String& x);

    static void append(Array<Sint8>& out, Uint32 x);

    static void appendSpecial(Array<Sint8>& out, Char16 x);

    static void appendSpecial(Array<Sint8>& out, char x);

    static void appendSpecial(Array<Sint8>& out, const char* x);

    static void appendSpecial(Array<Sint8>& out, const String& x);

    static void append(Array<Sint8>& out, const Indentor& x);

    static void appendLocalNameSpaceElement(
	Array<Sint8>& out, 
	const String& nameSpace);

    static void appendNameSpaceElement(
	Array<Sint8>& out, 
	const String& nameSpace);

    static Array<Sint8> formatGetHeader(
	const char* documentPath);

    static Array<Sint8> formatMPostHeader(
	const char* host,
	const char* cimOperation,
	const char* cimMethod,
	const String& cimObject,
	const Array<Sint8>& content);

    static Array<Sint8> formatMethodResponseHeader(
	const Array<Sint8>& content);

    static Array<Sint8> formatMessageElement(
	Uint32 messageId,
	const Array<Sint8>& body);

    static Array<Sint8> formatSimpleReqElement(
	const Array<Sint8>& body);

    static Array<Sint8> formatSimpleRspElement(
	const Array<Sint8>& body);

    static Array<Sint8> formatIMethodCallElement(
	const char* name,
	const String& nameSpace,
	const Array<Sint8>& iParamValues);

    static Array<Sint8> formatIReturnValueElement(
	const Array<Sint8>& body);

    static Array<Sint8> formatIMethodResponseElement(
	const char* name,
	const Array<Sint8>& iParamValues);

    static Array<Sint8>& formatIParamValueElement(
	Array<Sint8>& out,
	const char* name,
	const Array<Sint8>& body);

    static Array<Sint8> formatErrorElement(
	CimException::Code code,
	const char* description);

    static Array<Sint8>& appendBooleanParameter(
	Array<Sint8>& out,
	const char* parameterName,
	Boolean flag);

    static Array<Sint8>& appendClassNameParameter(
	Array<Sint8>& out,
	const char* parameterName,
	const String& className);

    static Array<Sint8>& appendQualifierNameParameter(
	Array<Sint8>& out,
	const char* parameterName,
	const String& qualifierName);

    static Array<Sint8>& appendClassParameter(
	Array<Sint8>& out,
	const char* parameterName,
	const ConstClassDecl& classDecl);

    static Array<Sint8>& appendQualifierDeclarationParameter(
	Array<Sint8>& out,
	const char* parameterName,
	const ConstQualifierDecl& qualifierDecl);

    static Array<Sint8>& appendClassNameElement(
	Array<Sint8>& out,
	const String& className);

    static void indentedPrint(
	std::ostream& os,
	const char* text, 
	Uint32 indentChars = 2);

    static Array<Sint8> formatSimpleReqMessage(
	const char* host,
	const String& nameSpace,
	const char* iMethodName,
	const Array<Sint8>& body);

    static Array<Sint8> formatSimpleRspMessage(
	const char* iMethodName,
	const Array<Sint8>& body);

    static Uint32 getNextMessageId();

private:

    XmlWriter() { }
};

inline Array<Sint8>& operator<<(Array<Sint8>& out, const char* x)
{
    XmlWriter::append(out, x);
    return out;
}

inline Array<Sint8>& operator<<(Array<Sint8>& out, char x)
{
    XmlWriter::append(out, x);
    return out;
}

inline Array<Sint8>& operator<<(Array<Sint8>& out, Char16 x)
{
    XmlWriter::append(out, x);
    return out;
}

inline Array<Sint8>& operator<<(Array<Sint8>& out, const String& x)
{
    XmlWriter::append(out, x);
    return out;
}

inline Array<Sint8>& operator<<(Array<Sint8>& out, const Indentor& x)
{
    XmlWriter::append(out, x);
    return out;
}

inline Array<Sint8>& operator<<(Array<Sint8>& out, const Array<Sint8>& x)
{
    out.appendArray(x);
    return out;
}

inline Array<Sint8>& operator<<(Array<Sint8>& out, Uint32 x)
{
    XmlWriter::append(out, x);
    return out;
}

PEGASUS_NAMESPACE_END

#endif /* Pegasus_XmlWriter_h */
