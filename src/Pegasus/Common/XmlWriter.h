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
// Modified By: 
//         Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
//         Nag Boranna, Hewlett-Packard Company (nagaraja_boranna@hp.com)
//         Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//         Carol Ann Krug Graves, Hewlett-Packard Company
//             (carolann_graves@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_XmlWriter_h
#define Pegasus_XmlWriter_h

#include <iostream>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/InternalException.h>
#include <Pegasus/Common/ArrayInternal.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Indentor.h>
#include <Pegasus/Common/CIMObject.h>
#include <Pegasus/Common/CIMClass.h>
#include <Pegasus/Common/CIMInstance.h>
#include <Pegasus/Common/CIMProperty.h>
#include <Pegasus/Common/CIMMethod.h>
#include <Pegasus/Common/CIMParameter.h>
#include <Pegasus/Common/CIMQualifier.h>
#include <Pegasus/Common/CIMQualifierDecl.h>
#include <Pegasus/Common/CIMValue.h>
#include <Pegasus/Common/CIMObjectPath.h>
#include <Pegasus/Common/CIMPropertyList.h>
#include <Pegasus/Common/CIMParamValue.h>
#include <Pegasus/Common/Message.h>
#include <Pegasus/Common/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

class PEGASUS_COMMON_LINKAGE XmlWriter
{
public:

    static void append(Array<Sint8>& out, const Char16& x);

    static void append(Array<Sint8>& out, char x)
    {
	append(out, Char16(x));
    }

    static void append(Array<Sint8>& out, Boolean x);

    static void append(Array<Sint8>& out, Uint32 x);

    static void append(Array<Sint8>& out, Sint32 x);

    static void append(Array<Sint8>& out, Uint64 x);

    static void append(Array<Sint8>& out, Sint64 x);

    static void append(Array<Sint8>& out, Real64 x);

    static void append(Array<Sint8>& out, const char* str);

    static void append(Array<Sint8>& out, const String& str);

    static void append(Array<Sint8>& out, const Indentor& x);

    static void appendSpecial(Array<Sint8>& out, const Char16& x);

    static void appendSpecial(Array<Sint8>& out, char x);

    static void appendSpecial(Array<Sint8>& out, const char* str);

    static void appendSpecial(Array<Sint8>& out, const String& str);

    static String encodeURICharacters(Array<Sint8> uriString);
    static String encodeURICharacters(String uriString);

    static void appendLocalNameSpacePathElement(
	Array<Sint8>& out, 
	const CIMNamespaceName& nameSpace);

    static void appendNameSpacePathElement(
	Array<Sint8>& out, 
	const String& host,
	const CIMNamespaceName& nameSpace);

    static void appendClassNameElement(
	Array<Sint8>& out,
	const CIMName& className);

    static void appendInstanceNameElement(
	Array<Sint8>& out,
	const CIMObjectPath& instanceName);

    static void appendClassPathElement(
	Array<Sint8>& out,
	const CIMObjectPath& classPath);

    static void appendInstancePathElement(
	Array<Sint8>& out,
	const CIMObjectPath& instancePath);

    static void appendLocalClassPathElement(
	Array<Sint8>& out,
	const CIMObjectPath& classPath);

    static void appendLocalInstancePathElement(
	Array<Sint8>& out,
	const CIMObjectPath& instancePath);

    static void appendLocalObjectPathElement(
	Array<Sint8>& out,
	const CIMObjectPath& objectPath);

    static void appendValueElement(
        Array<Sint8>& out,
        const CIMValue& value);

    static void printValueElement(
        const CIMValue& value,
        PEGASUS_STD(ostream)& os=PEGASUS_STD(cout));

    static void appendValueObjectWithPathElement(
        Array<Sint8>& out,
        const CIMObject& objectWithPath);

    static void appendValueReferenceElement(
        Array<Sint8>& out,
        const CIMObjectPath& reference,
        Boolean putValueWrapper);

    static void printValueReferenceElement(
        const CIMObjectPath& reference,
        PEGASUS_STD(ostream)& os=PEGASUS_STD(cout));

    static void appendValueNamedInstanceElement(
        Array<Sint8>& out,
        const CIMInstance& namedInstance);

    static void appendClassElement(
        Array<Sint8>& out,
        const CIMConstClass& cimclass);

    static void printClassElement(
        const CIMConstClass& cimclass,
        PEGASUS_STD(ostream)& os=PEGASUS_STD(cout));

    static void appendInstanceElement(
        Array<Sint8>& out,
        const CIMConstInstance& instance);

    static void printInstanceElement(
        const CIMConstInstance& instance,
        PEGASUS_STD(ostream)& os=PEGASUS_STD(cout));

    static void appendObjectElement(
        Array<Sint8>& out,
        const CIMConstObject& object);

    static void appendPropertyElement(
        Array<Sint8>& out,
        const CIMConstProperty& property);

    static void printPropertyElement(
        const CIMConstProperty& property,
        PEGASUS_STD(ostream)& os=PEGASUS_STD(cout));

    static void appendMethodElement(
        Array<Sint8>& out,
        const CIMConstMethod& method);

    static void printMethodElement(
        const CIMConstMethod& method,
        PEGASUS_STD(ostream)& os=PEGASUS_STD(cout));

    static void appendParameterElement(
        Array<Sint8>& out,
        const CIMConstParameter& parameter);

    static void printParameterElement(
        const CIMConstParameter& parameter,
        PEGASUS_STD(ostream)& os=PEGASUS_STD(cout));

    static void appendParamValueElement(
        Array<Sint8>& out,
        const CIMParamValue& paramValue);

    static void printParamValueElement(
        const CIMParamValue& paramValue,
        PEGASUS_STD(ostream)& os=PEGASUS_STD(cout));

    static void appendQualifierElement(
        Array<Sint8>& out,
        const CIMConstQualifier& qualifier);

    static void printQualifierElement(
        const CIMConstQualifier& qualifier,
        PEGASUS_STD(ostream)& os=PEGASUS_STD(cout));

    static void appendQualifierDeclElement(
        Array<Sint8>& out,
        const CIMConstQualifierDecl& qualifierDecl);

    static void printQualifierDeclElement(
        const CIMConstQualifierDecl& qualifierDecl,
        PEGASUS_STD(ostream)& os=PEGASUS_STD(cout));

    static void appendQualifierFlavorEntity(
        Array<Sint8>& out,
        const CIMFlavor & flavor);

    static void appendScopeElement(
        Array<Sint8>& out,
        const CIMScope & scope);

    static void appendMethodCallHeader(
	Array<Sint8>& out,
	const char* host,
	const CIMName& cimMethod,
	const String& cimObject,
	const String& authenticationHeader,
        HttpMethod httpMethod,
	Uint32 contentLength);

    static void appendMethodResponseHeader(
	Array<Sint8>& out,
        HttpMethod httpMethod,
	Uint32 contentLength);

    static void appendHttpErrorResponseHeader(
	Array<Sint8>& out,
	const String& status,
	const String& cimError = String::EMPTY,
	const String& errorDetail = String::EMPTY);

    static void appendUnauthorizedResponseHeader(
	Array<Sint8>& out,
        const String& content);

    static void appendReturnValueElement(
	Array<Sint8>& out,
	const CIMValue& value);

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
	const CIMName& className);

    static void appendInstanceNameIParameter(
	Array<Sint8>& out,
	const char* name,
	const CIMObjectPath& instanceName);

    static void appendObjectNameIParameter(
	Array<Sint8>& out,
	const char* name,
	const CIMObjectPath& objectName);

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
	const CIMInstance& namedInstance) ;

    static void appendPropertyNameIParameter(
	Array<Sint8>& out,
	const CIMName& propertyName);

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

    static Array<Sint8> formatHttpErrorRspMessage(
	const String& status,
	const String& cimError = String::EMPTY,
	const String& errorDetail = String::EMPTY);

    static Array<Sint8> formatSimpleMethodReqMessage(
	const char* host,
	const CIMNamespaceName& nameSpace,
	const CIMObjectPath& path,
	const CIMName& methodName,
	const Array<CIMParamValue>& parameters,
	const String& messageId,
        HttpMethod httpMethod,
        const String& authenticationHeader);

    static Array<Sint8> formatSimpleMethodRspMessage(
	const CIMName& methodName,
        const String& messageId,
        HttpMethod httpMethod,
	const Array<Sint8>& body);

    static Array<Sint8> formatSimpleMethodErrorRspMessage(
	const CIMName& methodName,
	const String& messageId,
        HttpMethod httpMethod,
	const CIMException& cimException);

    static Array<Sint8> formatSimpleIMethodReqMessage(
	const char* host,
	const CIMNamespaceName& nameSpace,
	const CIMName& iMethodName,
	const String& messageId,
        HttpMethod httpMethod,
        const String& authenticationHeader,
	const Array<Sint8>& body);

    static Array<Sint8> formatSimpleIMethodRspMessage(
	const CIMName& iMethodName,
        const String& messageId,
        HttpMethod httpMethod,
	const Array<Sint8>& body);

    static Array<Sint8> formatSimpleIMethodErrorRspMessage(
	const CIMName& iMethodName,
	const String& messageId,
        HttpMethod httpMethod,
	const CIMException& cimException);

    static void appendEMethodRequestHeader(
    	Array<Sint8>& out,
        const char* requestUri,
    	const char* host,
    	const CIMName& cimMethod,
        HttpMethod httpMethod,
        const String& authenticationHeader,
	Uint32 contentLength);

    static void appendEMethodResponseHeader(
	Array<Sint8>& out,
        HttpMethod httpMethod,
	Uint32 contentLength);

    static Array<Sint8> formatSimpleEMethodReqMessage(
        const char* requestUri,
	const char* host,
	const CIMName& eMethodName,
	const String& messageId,
        HttpMethod httpMethod,
	const String& authenticationHeader,
	const Array<Sint8>& body);

    static Array<Sint8> formatSimpleEMethodRspMessage(
	const CIMName& eMethodName,
        const String& messageId,
        HttpMethod httpMethod,
	const Array<Sint8>& body);

    static Array<Sint8> formatSimpleEMethodErrorRspMessage(
	const CIMName& eMethodName,
	const String& messageId,
        HttpMethod httpMethod,
	const CIMException& cimException);

    static void indentedPrint(
	PEGASUS_STD(ostream)& os,
	const char* text, 
	Uint32 indentChars = 2);

    static String getNextMessageId();

    /** Converts the given CIMKeyBinding type to one of the following:
        "boolean", "string", or "numeric"
    */
    static const char* keyBindingTypeToString (CIMKeyBinding::Type type);

private:

    static void _appendMessageElementBegin(
	Array<Sint8>& out,
	const String& messageId);
    static void _appendMessageElementEnd(
	Array<Sint8>& out);

    static void _appendSimpleReqElementBegin(Array<Sint8>& out);
    static void _appendSimpleReqElementEnd(Array<Sint8>& out);

    static void _appendMethodCallElementBegin(
	Array<Sint8>& out,
	const CIMName& name);
    static void _appendMethodCallElementEnd(
	Array<Sint8>& out);

    static void _appendIMethodCallElementBegin(
	Array<Sint8>& out,
	const CIMName& name);
    static void _appendIMethodCallElementEnd(
	Array<Sint8>& out);

    static void _appendIParamValueElementBegin(
	Array<Sint8>& out,
	const char* name);
    static void _appendIParamValueElementEnd(
	Array<Sint8>& out);

    static void _appendSimpleRspElementBegin(Array<Sint8>& out);
    static void _appendSimpleRspElementEnd(Array<Sint8>& out);

    static void _appendMethodResponseElementBegin(
	Array<Sint8>& out,
	const CIMName& name);
    static void _appendMethodResponseElementEnd(
	Array<Sint8>& out);

    static void _appendIMethodResponseElementBegin(
	Array<Sint8>& out,
	const CIMName& name);
    static void _appendIMethodResponseElementEnd(
	Array<Sint8>& out);

    static void _appendErrorElement(
	Array<Sint8>& out,
	const CIMException& cimException);

    static void _appendIReturnValueElementBegin(Array<Sint8>& out);
    static void _appendIReturnValueElementEnd(Array<Sint8>& out);

    static void _appendSimpleExportReqElementBegin(Array<Sint8>& out);
    static void _appendSimpleExportReqElementEnd(Array<Sint8>& out);

    static void _appendEMethodCallElementBegin(
    	Array<Sint8>& out,
    	const CIMName& name);
    static void _appendEMethodCallElementEnd(
    	Array<Sint8>& out);

    static void _appendSimpleExportRspElementBegin(Array<Sint8>& out);
    static void _appendSimpleExportRspElementEnd(Array<Sint8>& out);

    static void _appendEMethodResponseElementBegin(
    	Array<Sint8>& out,
    	const CIMName& name);
    static void _appendEMethodResponseElementEnd(
    	Array<Sint8>& out);

    XmlWriter() { }
};

PEGASUS_COMMON_LINKAGE Array<Sint8>& operator<<(
    Array<Sint8>& out, 
    const char* x);

PEGASUS_COMMON_LINKAGE Array<Sint8>& operator<<(Array<Sint8>& out, char x);

PEGASUS_COMMON_LINKAGE Array<Sint8>& operator<<(Array<Sint8>& out, const Char16& x);

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

PEGASUS_COMMON_LINKAGE Array<Sint8>& operator<<(
    Array<Sint8>& out, 
    const CIMName& name);

PEGASUS_COMMON_LINKAGE PEGASUS_STD(ostream)& operator<<(
    PEGASUS_STD(ostream)& os,
    const CIMDateTime& x);

PEGASUS_COMMON_LINKAGE PEGASUS_STD(ostream)& operator<<(
    PEGASUS_STD(ostream)& os,
    const CIMName& name);

PEGASUS_COMMON_LINKAGE PEGASUS_STD(ostream)& operator<<(
    PEGASUS_STD(ostream)& os,
    const CIMNamespaceName& name);

PEGASUS_NAMESPACE_END

#endif /* Pegasus_XmlWriter_h */
