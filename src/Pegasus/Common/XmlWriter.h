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
//
//         Nag Boranna, Hewlett-Packard Company(nagaraja_boranna@hp.com)
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
        const String& authenticationHeader,
	const Array<Sint8>& content);

    static Array<Sint8> formatMethodResponseHeader(
	const Array<Sint8>& content);

    static Array<Sint8> formatMessageElement(
	const String& messageId,
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
	CIMStatusCode code,
	const char* description);

    static Array<Sint8>& appendBooleanParameter(
	Array<Sint8>& out,
	const char* parameterName,
	Boolean flag);

    static Array<Sint8>& appendStringIParameter(
	Array<Sint8>& out,
	const char* parameterName,
	const String& str);

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
	const CIMConstClass& cimClass);

    static Array<Sint8>& appendInstanceNameParameter(
	Array<Sint8>& out,
	const char* parameterName,
	const CIMReference& instanceName);

    static Array<Sint8>& appendInstanceParameter(
	Array<Sint8>& out,
	const char* parameterName,
	const CIMConstInstance& instance);

    static Array<Sint8>& appendQualifierDeclarationParameter(
	Array<Sint8>& out,
	const char* parameterName,
	const CIMConstQualifierDecl& qualifierDecl);

    static Array<Sint8>& appendClassNameElement(
	Array<Sint8>& out,
	const String& className);

    static Array<Sint8>& appendInstanceNameElement(
	Array<Sint8>& out,
	const CIMReference& instanceName);

    static Array<Sint8>& appendPropertyNameParameter(
	Array<Sint8>& out,
	const String& propertyName);

    static Array<Sint8>& appendPropertyValueParameter(
	Array<Sint8>& out,
	const char* parameterName,
	const CIMValue& value);

    static Array<Sint8>& appendPropertyListParameter(
	Array<Sint8>& out,
	const Array<String>& propertyList);

    static Array<Sint8>& appendObjectNameParameter(
	Array<Sint8>& out,
	const char* name,
	const CIMReference& objectName);

    static void indentedPrint(
	PEGASUS_STD(ostream)& os,
	const char* text, 
	Uint32 indentChars = 2);

    static Array<Sint8> formatSimpleIMethodReqMessage(
	const char* host,
	const String& nameSpace,
	const char* iMethodName,
	const String& messageId,
        const String& authenticationHeader,
	const Array<Sint8>& body);

    static Array<Sint8> formatSimpleRspMessage(
	const char* iMethodName,
        const String& messageId,
	const Array<Sint8>& body);

    static String getNextMessageId();

    static Array<Sint8> formatSimpleIndicationReqMessage(
	const char* host,
    	const char* iMethodName,
	const String& messageId,
        const String& authenticationHeader,
    	const Array<Sint8>& body);

    static Array<Sint8> formatEMethodCallElement(
    	const char* name,
    	const Array<Sint8>& iParamValues);

    static Array<Sint8> formatMPostIndicationHeader(
    	const char* host,
    	const char* cimOperation,
    	const char* cimMethod,
        const String& authenticationHeader,
    	const Array<Sint8>& content);

    static Array<Sint8> formatSimpleExportReqElement(
    	const Array<Sint8>& body);

    static Array<Sint8> formatSimpleExportRspElement(
	const Array<Sint8>& body);

    static Array<Sint8> formatSimpleIndicationRspMessage(
	const char* iMethodName,
        const String& messageId,
	const Array<Sint8>& body);

    static Array<Sint8> formatEMethodResponseElement(
	const char* name,
	const Array<Sint8>& iParamValues);

    static Array<Sint8> formatEMethodResponseHeader(
	const Array<Sint8>& content);

    static Array<Sint8> formatSimpleMethodReqMessage(
	const char* host,
	const String& nameSpace,
	const char* iMethodName,
	const String& messageId,
        const String& authenticationHeader,
	const Array<Sint8>& body);

    static Array<Sint8> formatMethodCallElement(
	const char* name,
	const String& nameSpace,
	const Array<Sint8>& iParamValues);

    static Array<Sint8> formatSimpleMethodRspMessage(
	const char* iMethodName,
        const String& messageId,
	const Array<Sint8>& body);

    static Array<Sint8> formatMethodResponseElement(
	const char* name,
	const Array<Sint8>& iParamValues);

    static Array<Sint8>& appendStringParameter(
	Array<Sint8>& out,
	const char* parameterName,
	const String& str);

    static Array<Sint8>& formatParamValueElement(
	Array<Sint8>& out,
	const char* name,
	const Array<Sint8>& body);
    
    static Array<Sint8> formatReturnValueElement(
	const Array<Sint8>& body);

    static Array<Sint8> formatUnauthorizedResponseHeader(
        const String& content);

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
