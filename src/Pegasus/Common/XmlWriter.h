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
// Modified By: 
//         Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
//         Nag Boranna, Hewlett-Packard Company (nagaraja_boranna@hp.com)
//         Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_XmlWriter_h
#define Pegasus_XmlWriter_h

#include <iostream>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Indentor.h>
#include <Pegasus/Common/CIMReference.h>
#include <Pegasus/Common/CIMPropertyList.h>
#include <Pegasus/Common/CIMNamedInstance.h>

PEGASUS_NAMESPACE_BEGIN

class CIMConstQualifierDecl;
class CIMConstClass;
class CIMConstInstance;

class PEGASUS_COMMON_LINKAGE XmlWriter
{
public:

    static void append(Array<Sint8>& out, Char16 x);

    static void append(Array<Sint8>& out, char x)
    {
	append(out, Char16(x));
    }

    static void append(Array<Sint8>& out, Uint32 x);

    static void append(Array<Sint8>& out, const char* str);

    static void append(Array<Sint8>& out, const String& str);

    static void append(Array<Sint8>& out, const Indentor& x);

    static void appendSpecial(Array<Sint8>& out, Char16 x);

    static void appendSpecial(Array<Sint8>& out, char x);

    static void appendSpecial(Array<Sint8>& out, const char* str);

    static void appendSpecial(Array<Sint8>& out, const String& str);

    static void appendLocalNameSpaceElement(
	Array<Sint8>& out, 
	const String& nameSpace);

    static void appendMethodCallHeader(
	Array<Sint8>& out,
	const char* host,
	const char* cimMethod,
	const String& cimObject,
        const String& authenticationHeader,
	Uint32 contentLength);

    static void appendMethodResponseHeader(
	Array<Sint8>& out,
	Uint32 contentLength);

    static void appendUnauthorizedResponseHeader(
	Array<Sint8>& out,
        const String& content);

    static void appendMessageElementBegin(
	Array<Sint8>& out,
	const String& messageId);
    static void appendMessageElementEnd(
	Array<Sint8>& out);

    static void appendSimpleReqElementBegin(Array<Sint8>& out);
    static void appendSimpleReqElementEnd(Array<Sint8>& out);

    static void appendMethodCallElementBegin(
	Array<Sint8>& out,
	const char* name);
    static void appendMethodCallElementEnd(
	Array<Sint8>& out);

    static void appendIMethodCallElementBegin(
	Array<Sint8>& out,
	const char* name);
    static void appendIMethodCallElementEnd(
	Array<Sint8>& out);

    static void appendIParamValueElementBegin(
	Array<Sint8>& out,
	const char* name);
    static void appendIParamValueElementEnd(
	Array<Sint8>& out);

    static void appendSimpleRspElementBegin(Array<Sint8>& out);
    static void appendSimpleRspElementEnd(Array<Sint8>& out);

    static void appendMethodResponseElementBegin(
	Array<Sint8>& out,
	const char* name);
    static void appendMethodResponseElementEnd(
	Array<Sint8>& out);

    static void appendIMethodResponseElementBegin(
	Array<Sint8>& out,
	const char* name);
    static void appendIMethodResponseElementEnd(
	Array<Sint8>& out);

    static void appendErrorElement(
	Array<Sint8>& out,
	CIMStatusCode code,
	const char* description);

    static void appendReturnValueElement(
	Array<Sint8>& out,
	const CIMValue& value);

    static void appendIReturnValueElementBegin(Array<Sint8>& out);
    static void appendIReturnValueElementEnd(Array<Sint8>& out);

    static void appendBooleanIParameter(
	Array<Sint8>& out,
	const char* name,
	Boolean flag);

    static void appendStringIParameter(
	Array<Sint8>& out,
	const char* name,
	const String& str);

    static void appendQualifierNameIParameter(
	Array<Sint8>& out,
	const char* name,
	const String& qualifierName);

    static void appendClassNameIParameter(
	Array<Sint8>& out,
	const char* name,
	const String& className);

    static void appendInstanceNameIParameter(
	Array<Sint8>& out,
	const char* name,
	const CIMReference& instanceName);

    static void appendObjectNameIParameter(
	Array<Sint8>& out,
	const char* name,
	const CIMReference& objectName);

    static void appendClassIParameter(
	Array<Sint8>& out,
	const char* name,
	const CIMConstClass& cimClass);

    static void appendInstanceIParameter(
	Array<Sint8>& out,
	const char* name,
	const CIMConstInstance& instance);

    static void appendNamedInstanceIParameter(
	Array<Sint8>& out,
	const char* name,
	const CIMNamedInstance& namedInstance);

    static void appendPropertyNameIParameter(
	Array<Sint8>& out,
	const String& propertyName);

    static void appendPropertyValueIParameter(
	Array<Sint8>& out,
	const char* name,
	const CIMValue& value);

    static void appendPropertyListIParameter(
	Array<Sint8>& out,
	const CIMPropertyList& propertyList);

    static void appendQualifierDeclarationIParameter(
	Array<Sint8>& out,
	const char* name,
	const CIMConstQualifierDecl& qualifierDecl);

    static void appendClassNameElement(
	Array<Sint8>& out,
	const String& className);

    static void appendInstanceNameElement(
	Array<Sint8>& out,
	const CIMReference& instanceName);

    static Array<Sint8> formatSimpleMethodReqMessage(
	const char* host,
	const String& nameSpace,
	const char* methodName,
	const String& messageId,
        const String& authenticationHeader,
	const Array<Sint8>& body);

    static Array<Sint8> formatSimpleMethodRspMessage(
	const char* methodName,
        const String& messageId,
	const Array<Sint8>& body);

    static Array<Sint8> formatSimpleIMethodReqMessage(
	const char* host,
	const String& nameSpace,
	const char* iMethodName,
	const String& messageId,
        const String& authenticationHeader,
	const Array<Sint8>& body);

    static Array<Sint8> formatSimpleIMethodRspMessage(
	const char* iMethodName,
        const String& messageId,
	const Array<Sint8>& body);

    static Array<Sint8> formatSimpleIndicationReqMessage(
	const char* host,
    	const char* iMethodName,
	const String& messageId,
        const String& authenticationHeader,
    	const Array<Sint8>& body);

    static void appendEMethodRequestHeader(
    	Array<Sint8>& out,
    	const char* host,
    	const char* cimMethod,
        const String& authenticationHeader,
	Uint32 contentLength);

    static void appendEMethodResponseHeader(
	Array<Sint8>& out,
	Uint32 contentLength);

    static void appendSimpleExportReqElementBegin(Array<Sint8>& out);
    static void appendSimpleExportReqElementEnd(Array<Sint8>& out);

    static void appendEMethodCallElementBegin(
    	Array<Sint8>& out,
    	const char* name);
    static void appendEMethodCallElementEnd(
    	Array<Sint8>& out);

    static void appendSimpleExportRspElementBegin(Array<Sint8>& out);
    static void appendSimpleExportRspElementEnd(Array<Sint8>& out);

    static void appendEMethodResponseElementBegin(
    	Array<Sint8>& out,
    	const char* name);
    static void appendEMethodResponseElementEnd(
    	Array<Sint8>& out);

    static Array<Sint8> formatSimpleEMethodReqMessage(
	const char* host,
	const char* eMethodName,
	const String& messageId,
	const String& authenticationHeader,
	const Array<Sint8>& body);

    static Array<Sint8> formatSimpleEMethodRspMessage(
	const char* eMethodName,
        const String& messageId,
	const Array<Sint8>& body);

    static void indentedPrint(
	PEGASUS_STD(ostream)& os,
	const char* text, 
	Uint32 indentChars = 2);

    static String getNextMessageId();

private:

    XmlWriter() { }
};

PEGASUS_COMMON_LINKAGE Array<Sint8>& operator<<(
    Array<Sint8>& out, 
    const char* x);

PEGASUS_COMMON_LINKAGE Array<Sint8>& operator<<(Array<Sint8>& out, char x);

PEGASUS_COMMON_LINKAGE Array<Sint8>& operator<<(Array<Sint8>& out, Char16 x);

PEGASUS_COMMON_LINKAGE Array<Sint8>& operator<<(
    Array<Sint8>& out, 
    const String& x);

PEGASUS_COMMON_LINKAGE Array<Sint8>& operator<<(
    Array<Sint8>& out, 
    const Indentor& x);

PEGASUS_COMMON_LINKAGE Array<Sint8>& operator<<(
    Array<Sint8>& out, 
    const Array<Sint8>& x);

PEGASUS_COMMON_LINKAGE Array<Sint8>& operator<<(
    Array<Sint8>& out, 
    Uint32 x);

PEGASUS_NAMESPACE_END

#endif /* Pegasus_XmlWriter_h */
