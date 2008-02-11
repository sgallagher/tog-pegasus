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
#include <Pegasus/Common/ContentLanguageList.h>
#include <Pegasus/Common/AcceptLanguageList.h>
#include <Pegasus/Common/Buffer.h>
#include <Pegasus/Common/StrLit.h>

PEGASUS_NAMESPACE_BEGIN

// l10n - added accept language and content language support below

class PEGASUS_COMMON_LINKAGE XmlWriter
{
public:

    static void append(Buffer& out, const Char16& x);

    static void append(Buffer& out, char x)
    {
      out.append(x);
    }

    static void append(Buffer& out, Boolean x);

    static void append(Buffer& out, Uint32 x);

    static void append(Buffer& out, Sint32 x);

    static void append(Buffer& out, Uint64 x);

    static void append(Buffer& out, Sint64 x);

    static void append(Buffer& out, Real32 x);

    static void append(Buffer& out, Real64 x);

    static void append(Buffer& out, const char* str);

    static void append(Buffer& out, const String& str);

    static void append(Buffer& out, const Indentor& x);

    static void appendSpecial(Buffer& out, const Char16& x);

    static void appendSpecial(Buffer& out, char x);

    static void appendSpecial(Buffer& out, const char* str);

    static void appendSpecial(Buffer& out, const String& str);

    static String encodeURICharacters(const Buffer& uriString);
    static String encodeURICharacters(const String& uriString);

    static void appendLocalNameSpacePathElement(
        Buffer& out,
        const CIMNamespaceName& nameSpace);

    static void appendNameSpacePathElement(
        Buffer& out,
        const String& host,
        const CIMNamespaceName& nameSpace);

    static void appendClassNameElement(
        Buffer& out,
        const CIMName& className);

    static void appendInstanceNameElement(
        Buffer& out,
        const CIMObjectPath& instanceName);

    static void appendClassPathElement(
        Buffer& out,
        const CIMObjectPath& classPath);

    static void appendInstancePathElement(
        Buffer& out,
        const CIMObjectPath& instancePath);

    static void appendLocalClassPathElement(
        Buffer& out,
        const CIMObjectPath& classPath);

    static void appendLocalInstancePathElement(
        Buffer& out,
        const CIMObjectPath& instancePath);

    static void appendLocalObjectPathElement(
        Buffer& out,
        const CIMObjectPath& objectPath);

    static void appendValueElement(
        Buffer& out,
        const CIMValue& value);

    static void printValueElement(
        const CIMValue& value,
        PEGASUS_STD(ostream)& os=PEGASUS_STD(cout));

    static void appendValueObjectWithPathElement(
        Buffer& out,
        const CIMObject& objectWithPath);

    static void appendValueReferenceElement(
        Buffer& out,
        const CIMObjectPath& reference,
        Boolean putValueWrapper);

    static void printValueReferenceElement(
        const CIMObjectPath& reference,
        PEGASUS_STD(ostream)& os=PEGASUS_STD(cout));

    static void appendValueNamedInstanceElement(
        Buffer& out,
        const CIMInstance& namedInstance);

    static void appendClassElement(
        Buffer& out,
        const CIMConstClass& cimclass);

    static void printClassElement(
        const CIMConstClass& cimclass,
        PEGASUS_STD(ostream)& os=PEGASUS_STD(cout));

    static void appendInstanceElement(
        Buffer& out,
        const CIMConstInstance& instance);

    static void printInstanceElement(
        const CIMConstInstance& instance,
        PEGASUS_STD(ostream)& os=PEGASUS_STD(cout));

    static void appendObjectElement(
        Buffer& out,
        const CIMConstObject& object);

    static void appendPropertyElement(
        Buffer& out,
        const CIMConstProperty& property);

    static void printPropertyElement(
        const CIMConstProperty& property,
        PEGASUS_STD(ostream)& os=PEGASUS_STD(cout));

    static void appendMethodElement(
        Buffer& out,
        const CIMConstMethod& method);

    static void printMethodElement(
        const CIMConstMethod& method,
        PEGASUS_STD(ostream)& os=PEGASUS_STD(cout));

    static void appendParameterElement(
        Buffer& out,
        const CIMConstParameter& parameter);

    static void printParameterElement(
        const CIMConstParameter& parameter,
        PEGASUS_STD(ostream)& os=PEGASUS_STD(cout));

    static void appendParamValueElement(
        Buffer& out,
        const CIMParamValue& paramValue);

    static void printParamValueElement(
        const CIMParamValue& paramValue,
        PEGASUS_STD(ostream)& os=PEGASUS_STD(cout));

    static void appendQualifierElement(
        Buffer& out,
        const CIMConstQualifier& qualifier);

    static void printQualifierElement(
        const CIMConstQualifier& qualifier,
        PEGASUS_STD(ostream)& os=PEGASUS_STD(cout));

    static void appendQualifierDeclElement(
        Buffer& out,
        const CIMConstQualifierDecl& qualifierDecl);

    static void printQualifierDeclElement(
        const CIMConstQualifierDecl& qualifierDecl,
        PEGASUS_STD(ostream)& os=PEGASUS_STD(cout));

    static void appendQualifierFlavorEntity(
        Buffer& out,
        const CIMFlavor & flavor);

    static void appendScopeElement(
        Buffer& out,
        const CIMScope & scope);

    static void appendMethodCallHeader(
        Buffer& out,
        const char* host,
        const CIMName& cimMethod,
        const String& cimObject,
        const String& authenticationHeader,
        HttpMethod httpMethod,
        const AcceptLanguageList& acceptLanguages,
        const ContentLanguageList& contentLanguages,
        Uint32 contentLength);

    // added to accommodate sending WBEMServerResponseTime PEP #128
    static void appendMethodResponseHeader(
        Buffer& out,
        HttpMethod httpMethod,
        const ContentLanguageList& contentLanguages,
        Uint32 contentLength,
        Uint64 serverResponseTime = 0);

    static void appendHttpErrorResponseHeader(
        Buffer& out,
        const String& status,
        const String& cimError = String::EMPTY,
        const String& errorDetail = String::EMPTY);

    static void appendUnauthorizedResponseHeader(
        Buffer& out,
        const String& content);

#ifdef PEGASUS_KERBEROS_AUTHENTICATION
    static void appendOKResponseHeader(
        Buffer& out,
        const String& content);
#endif
    static void appendParamTypeAndEmbeddedObjAttrib(
        Buffer& out,
        const CIMType& type);

    static void appendReturnValueElement(
        Buffer& out,
        const CIMValue& value);

    static void appendBooleanIParameter(
        Buffer& out,
        const char* name,
        Boolean flag);

    static void appendStringIParameter(
        Buffer& out,
        const char* name,
        const String& str);

    static void appendClassNameIParameter(
        Buffer& out,
        const char* name,
        const CIMName& className);

    static void appendInstanceNameIParameter(
        Buffer& out,
        const char* name,
        const CIMObjectPath& instanceName);

    static void appendObjectNameIParameter(
        Buffer& out,
        const char* name,
        const CIMObjectPath& objectName);

    static void appendClassIParameter(
        Buffer& out,
        const char* name,
        const CIMConstClass& cimClass);

    static void appendInstanceIParameter(
        Buffer& out,
        const char* name,
        const CIMConstInstance& instance);

    static void appendNamedInstanceIParameter(
        Buffer& out,
        const char* name,
        const CIMInstance& namedInstance) ;

    static void appendPropertyNameIParameter(
        Buffer& out,
        const CIMName& propertyName);

    static void appendPropertyValueIParameter(
        Buffer& out,
        const char* name,
        const CIMValue& value);

    static void appendPropertyListIParameter(
        Buffer& out,
        const CIMPropertyList& propertyList);

    static void appendQualifierDeclarationIParameter(
        Buffer& out,
        const char* name,
        const CIMConstQualifierDecl& qualifierDecl);

    static Buffer formatHttpErrorRspMessage(
        const String& status,
        const String& cimError = String::EMPTY,
        const String& errorDetail = String::EMPTY);

    static Buffer formatSimpleMethodReqMessage(
        const char* host,
        const CIMNamespaceName& nameSpace,
        const CIMObjectPath& path,
        const CIMName& methodName,
        const Array<CIMParamValue>& parameters,
        const String& messageId,
        HttpMethod httpMethod,
        const String& authenticationHeader,
        const AcceptLanguageList& httpAcceptLanguages,
        const ContentLanguageList& httpContentLanguages);

    // PEP 128 - sending serverResponseTime (WBEMServerResponseTime) in
    // response header
    static Buffer formatSimpleMethodRspMessage(
        const CIMName& methodName,
        const String& messageId,
        HttpMethod httpMethod,
        const ContentLanguageList& httpContentLanguages,
        const Buffer& body,
        Uint64 serverResponseTime,
        Boolean isFirst = true,
        Boolean isLast = true);

    static Buffer formatSimpleMethodErrorRspMessage(
        const CIMName& methodName,
        const String& messageId,
        HttpMethod httpMethod,
        const CIMException& cimException);

    static Buffer formatSimpleIMethodReqMessage(
        const char* host,
        const CIMNamespaceName& nameSpace,
        const CIMName& iMethodName,
        const String& messageId,
        HttpMethod httpMethod,
        const String& authenticationHeader,
        const AcceptLanguageList& httpAcceptLanguages,
        const ContentLanguageList& httpContentLanguages,
        const Buffer& body);

    // PEP 128 - sending serverResponseTime (WBEMServerResponseTime) in
    // response header
    static Buffer formatSimpleIMethodRspMessage(
        const CIMName& iMethodName,
        const String& messageId,
        HttpMethod httpMethod,
        const ContentLanguageList& httpContentLanguages,
        const Buffer& body,
        Uint64 serverResponseTime,
        Boolean isFirst = true,
        Boolean isLast = true);

    static Buffer formatSimpleIMethodErrorRspMessage(
        const CIMName& iMethodName,
        const String& messageId,
        HttpMethod httpMethod,
        const CIMException& cimException);

    static void appendInstanceEParameter(
        Buffer& out,
        const char* name,
        const CIMInstance& instance);

    static void appendEMethodRequestHeader(
        Buffer& out,
        const char* requestUri,
        const char* host,
        const CIMName& cimMethod,
        HttpMethod httpMethod,
        const String& authenticationHeader,
        const AcceptLanguageList& acceptLanguages,
        const ContentLanguageList& contentLanguages,
        Uint32 contentLength);

    static void appendEMethodResponseHeader(
        Buffer& out,
        HttpMethod httpMethod,
        const ContentLanguageList& contentLanguages,
        Uint32 contentLength);

    static Buffer formatSimpleEMethodReqMessage(
        const char* requestUri,
        const char* host,
        const CIMName& eMethodName,
        const String& messageId,
        HttpMethod httpMethod,
        const String& authenticationHeader,
        const AcceptLanguageList& httpAcceptLanguages,
        const ContentLanguageList& httpContentLanguages,
        const Buffer& body);

    static Buffer formatSimpleEMethodRspMessage(
        const CIMName& eMethodName,
        const String& messageId,
        HttpMethod httpMethod,
        const ContentLanguageList& httpContentLanguages,
        const Buffer& body);

    static Buffer formatSimpleEMethodErrorRspMessage(
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
        Buffer& out,
        const String& messageId);
        static void _appendMessageElementEnd(
        Buffer& out);

    static void _appendSimpleReqElementBegin(Buffer& out);
    static void _appendSimpleReqElementEnd(Buffer& out);

    static void _appendMethodCallElementBegin(
        Buffer& out,
        const CIMName& name);

    static void _appendMethodCallElementEnd(
        Buffer& out);

    static void _appendIMethodCallElementBegin(
        Buffer& out,
        const CIMName& name);
        static void _appendIMethodCallElementEnd(
        Buffer& out);

    static void _appendIParamValueElementBegin(
        Buffer& out,
        const char* name);
        static void _appendIParamValueElementEnd(
        Buffer& out);

    static void _appendSimpleRspElementBegin(Buffer& out);
    static void _appendSimpleRspElementEnd(Buffer& out);

    static void _appendMethodResponseElementBegin(
        Buffer& out,
        const CIMName& name);
        static void _appendMethodResponseElementEnd(
        Buffer& out);

    static void _appendIMethodResponseElementBegin(
        Buffer& out,
        const CIMName& name);
        static void _appendIMethodResponseElementEnd(
        Buffer& out);

    static void _appendErrorElement(
        Buffer& out,
        const CIMException& cimException);

    static void _appendIReturnValueElementBegin(Buffer& out);
    static void _appendIReturnValueElementEnd(Buffer& out);

    static void _appendSimpleExportReqElementBegin(Buffer& out);
    static void _appendSimpleExportReqElementEnd(Buffer& out);

    static void _appendEMethodCallElementBegin(
        Buffer& out,
        const CIMName& name);

    static void _appendEMethodCallElementEnd(
        Buffer& out);

    static void _appendEParamValueElementBegin(
        Buffer& out,
        const char* name);
        static void _appendEParamValueElementEnd(
        Buffer& out);

    static void _appendSimpleExportRspElementBegin(Buffer& out);
    static void _appendSimpleExportRspElementEnd(Buffer& out);

    static void _appendEMethodResponseElementBegin(
        Buffer& out,
        const CIMName& name);

    static void _appendEMethodResponseElementEnd(
        Buffer& out);

    XmlWriter() { }
};

PEGASUS_COMMON_LINKAGE Buffer& operator<<(
    Buffer& out,
    const char* x);

inline Buffer& operator<<(Buffer& out, char x)
{
    out.append(x);
    return out;
}

inline Buffer& operator<<(Buffer& out, const char* s)
{
    out.append(s, strlen(s));
    return out;
}

PEGASUS_COMMON_LINKAGE Buffer& operator<<(Buffer& out, const Char16& x);

PEGASUS_COMMON_LINKAGE Buffer& operator<<(
    Buffer& out,
    const String& x);

PEGASUS_COMMON_LINKAGE Buffer& operator<<(
    Buffer& out,
    const Indentor& x);

PEGASUS_COMMON_LINKAGE Buffer& operator<<(
    Buffer& out,
    const Buffer& x);

PEGASUS_COMMON_LINKAGE Buffer& operator<<(
    Buffer& out,
    Uint32 x);

PEGASUS_COMMON_LINKAGE Buffer& operator<<(
    Buffer& out,
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
