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
// Modified By: 
//         Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
//         Nag Boranna, Hewlett-Packard Company (nagaraja_boranna@hp.com)
//         Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//         Carol Ann Krug Graves, Hewlett-Packard Company
//             (carolann_graves@hp.com)
//         Brian G. Campbell, EMC (campbell_brian@emc.com) - PEP140/phase1
//		   Willis White (whiwill@us.ibm.com) PEP 127 and 128
//         Brian G. Campbell, EMC (campbell_brian@emc.com) - PEP140/phase2
//         David Dillard, VERITAS Software Corp.  (david.dillard@veritas.com)
//              Vijay Eli, vijayeli@in.ibm.com, fix for #2571
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
#include <Pegasus/Common/ContentLanguages.h>  // l10n
#include <Pegasus/Common/AcceptLanguages.h>   // l10n

PEGASUS_NAMESPACE_BEGIN

// l10n - added accept language and content language support below

class PEGASUS_COMMON_LINKAGE XmlWriter
{
public:

    static void append(Array<char>& out, const Char16& x);

    static void append(Array<char>& out, char x)
    {
      out.append(x);
    }

    static void append(Array<char>& out, Boolean x);

    static void append(Array<char>& out, Uint32 x);

    static void append(Array<char>& out, Sint32 x);

    static void append(Array<char>& out, Uint64 x);

    static void append(Array<char>& out, Sint64 x);

    static void append(Array<char>& out, Real32 x);

    static void append(Array<char>& out, Real64 x);

    static void append(Array<char>& out, const char* str);

    static void append(Array<char>& out, const String& str);

    static void append(Array<char>& out, const Indentor& x);

    static void appendSpecial(Array<char>& out, const Char16& x);

    static void appendSpecial(Array<char>& out, char x);

    static void appendSpecial(Array<char>& out, const char* str);

    static void appendSpecial(Array<char>& out, const String& str);

    static String encodeURICharacters(const Array<char>& uriString);
    static String encodeURICharacters(const String& uriString);

    static void appendLocalNameSpacePathElement(
	Array<char>& out, 
	const CIMNamespaceName& nameSpace);

    static void appendNameSpacePathElement(
	Array<char>& out, 
	const String& host,
	const CIMNamespaceName& nameSpace);

    static void appendClassNameElement(
	Array<char>& out,
	const CIMName& className);

    static void appendInstanceNameElement(
	Array<char>& out,
	const CIMObjectPath& instanceName);

    static void appendClassPathElement(
	Array<char>& out,
	const CIMObjectPath& classPath);

    static void appendInstancePathElement(
	Array<char>& out,
	const CIMObjectPath& instancePath);

    static void appendLocalClassPathElement(
	Array<char>& out,
	const CIMObjectPath& classPath);

    static void appendLocalInstancePathElement(
	Array<char>& out,
	const CIMObjectPath& instancePath);

    static void appendLocalObjectPathElement(
	Array<char>& out,
	const CIMObjectPath& objectPath);

    static void appendValueElement(
        Array<char>& out,
        const CIMValue& value);

    static void printValueElement(
        const CIMValue& value,
        PEGASUS_STD(ostream)& os=PEGASUS_STD(cout));

    static void appendValueObjectWithPathElement(
        Array<char>& out,
        const CIMObject& objectWithPath);

    static void appendValueReferenceElement(
        Array<char>& out,
        const CIMObjectPath& reference,
        Boolean putValueWrapper);

    static void printValueReferenceElement(
        const CIMObjectPath& reference,
        PEGASUS_STD(ostream)& os=PEGASUS_STD(cout));

    static void appendValueNamedInstanceElement(
        Array<char>& out,
        const CIMInstance& namedInstance);

    static void appendClassElement(
        Array<char>& out,
        const CIMConstClass& cimclass);

    static void printClassElement(
        const CIMConstClass& cimclass,
        PEGASUS_STD(ostream)& os=PEGASUS_STD(cout));

    static void appendInstanceElement(
        Array<char>& out,
        const CIMConstInstance& instance);

    static void printInstanceElement(
        const CIMConstInstance& instance,
        PEGASUS_STD(ostream)& os=PEGASUS_STD(cout));

    static void appendObjectElement(
        Array<char>& out,
        const CIMConstObject& object);

    static void appendPropertyElement(
        Array<char>& out,
        const CIMConstProperty& property);

    static void printPropertyElement(
        const CIMConstProperty& property,
        PEGASUS_STD(ostream)& os=PEGASUS_STD(cout));

    static void appendMethodElement(
        Array<char>& out,
        const CIMConstMethod& method);

    static void printMethodElement(
        const CIMConstMethod& method,
        PEGASUS_STD(ostream)& os=PEGASUS_STD(cout));

    static void appendParameterElement(
        Array<char>& out,
        const CIMConstParameter& parameter);

    static void printParameterElement(
        const CIMConstParameter& parameter,
        PEGASUS_STD(ostream)& os=PEGASUS_STD(cout));

    static void appendParamValueElement(
        Array<char>& out,
        const CIMParamValue& paramValue);

    static void printParamValueElement(
        const CIMParamValue& paramValue,
        PEGASUS_STD(ostream)& os=PEGASUS_STD(cout));

    static void appendQualifierElement(
        Array<char>& out,
        const CIMConstQualifier& qualifier);

    static void printQualifierElement(
        const CIMConstQualifier& qualifier,
        PEGASUS_STD(ostream)& os=PEGASUS_STD(cout));

    static void appendQualifierDeclElement(
        Array<char>& out,
        const CIMConstQualifierDecl& qualifierDecl);

    static void printQualifierDeclElement(
        const CIMConstQualifierDecl& qualifierDecl,
        PEGASUS_STD(ostream)& os=PEGASUS_STD(cout));

    static void appendQualifierFlavorEntity(
        Array<char>& out,
        const CIMFlavor & flavor);

    static void appendScopeElement(
        Array<char>& out,
        const CIMScope & scope);

    static void appendMethodCallHeader(
	Array<char>& out,
	const char* host,
	const CIMName& cimMethod,
	const String& cimObject,
	const String& authenticationHeader,
        HttpMethod httpMethod,
    const AcceptLanguages & acceptLanguages,    
    const ContentLanguages & contentLanguages,        
	Uint32 contentLength);

	// added to accommodate sending WBEMServerResponseTime PEP #128
    static void appendMethodResponseHeader(
        Array<char>& out,
        HttpMethod httpMethod,
    const ContentLanguages & contentLanguages,
        Uint32 contentLength,
	Uint32 serverResponseTime = 0);

    static void appendHttpErrorResponseHeader(
	Array<char>& out,
	const String& status,
	const String& cimError = String::EMPTY,
	const String& errorDetail = String::EMPTY);

    static void appendUnauthorizedResponseHeader(
	Array<char>& out,
        const String& content);

#ifdef PEGASUS_KERBEROS_AUTHENTICATION
    static void appendOKResponseHeader(
	Array<char>& out,
        const String& content);
#endif

    static void appendReturnValueElement(
	Array<char>& out,
	const CIMValue& value);

    static void appendBooleanIParameter(
	Array<char>& out,
	const char* name,
	Boolean flag);

    static void appendStringIParameter(
	Array<char>& out,
	const char* name,
	const String& str);

    static void appendQualifierNameIParameter(
	Array<char>& out,
	const char* name,
	const String& qualifierName);

    static void appendClassNameIParameter(
	Array<char>& out,
	const char* name,
	const CIMName& className);

    static void appendInstanceNameIParameter(
	Array<char>& out,
	const char* name,
	const CIMObjectPath& instanceName);

    static void appendObjectNameIParameter(
	Array<char>& out,
	const char* name,
	const CIMObjectPath& objectName);

    static void appendClassIParameter(
	Array<char>& out,
	const char* name,
	const CIMConstClass& cimClass);

    static void appendInstanceIParameter(
	Array<char>& out,
	const char* name,
	const CIMConstInstance& instance);

    static void appendNamedInstanceIParameter(
	Array<char>& out,
	const char* name,
	const CIMInstance& namedInstance) ;

    static void appendPropertyNameIParameter(
	Array<char>& out,
	const CIMName& propertyName);

    static void appendPropertyValueIParameter(
	Array<char>& out,
	const char* name,
	const CIMValue& value);

    static void appendPropertyListIParameter(
	Array<char>& out,
	const CIMPropertyList& propertyList);

    static void appendQualifierDeclarationIParameter(
	Array<char>& out,
	const char* name,
	const CIMConstQualifierDecl& qualifierDecl);

    static Array<char> formatHttpErrorRspMessage(
	const String& status,
	const String& cimError = String::EMPTY,
	const String& errorDetail = String::EMPTY);

    static Array<char> formatSimpleMethodReqMessage(
	const char* host,
	const CIMNamespaceName& nameSpace,
	const CIMObjectPath& path,
	const CIMName& methodName,
	const Array<CIMParamValue>& parameters,
	const String& messageId,
        HttpMethod httpMethod,
        const String& authenticationHeader,
    const AcceptLanguages& httpAcceptLanguages,
    const ContentLanguages& httpContentLanguages);

	//PEP 128 - sending serverResponseTime (WBEMServerResponseTime) in respons header
	static Array<char> formatSimpleMethodRspMessage(
	const CIMName& methodName,
        const String& messageId,
        HttpMethod httpMethod,
        const ContentLanguages & httpContentLanguages,          
	const Array<char>& body,
	Uint32 serverResponseTime,
	Boolean isFirst = true,
	Boolean isLast = true);

    static Array<char> formatSimpleMethodErrorRspMessage(
	const CIMName& methodName,
	const String& messageId,
        HttpMethod httpMethod,
	const CIMException& cimException);

    static Array<char> formatSimpleIMethodReqMessage(
	const char* host,
	const CIMNamespaceName& nameSpace,
	const CIMName& iMethodName,
	const String& messageId,
        HttpMethod httpMethod,
        const String& authenticationHeader,
    const AcceptLanguages& httpAcceptLanguages,
    const ContentLanguages& httpContentLanguages,        
	const Array<char>& body);

    //PEP 128 - sending serverResponseTime (WBEMServerResponseTime) in respons header
	static Array<char> formatSimpleIMethodRspMessage(
	const CIMName& iMethodName,
        const String& messageId,
        HttpMethod httpMethod,
        const ContentLanguages & httpContentLanguages,  
	const Array<char>& body,
	Uint32 serverResponseTime,
	Boolean isFirst = true,
	Boolean isLast = true);

    static Array<char> formatSimpleIMethodErrorRspMessage(
	const CIMName& iMethodName,
	const String& messageId,
        HttpMethod httpMethod,
	const CIMException& cimException);

    static void appendInstanceEParameter(
	Array<char>& out,
	const char* name,
	const CIMInstance& instance);

    static void appendEMethodRequestHeader(
    	Array<char>& out,
        const char* requestUri,
    	const char* host,
    	const CIMName& cimMethod,
        HttpMethod httpMethod,
        const String& authenticationHeader,
    const AcceptLanguages& acceptLanguages,      
    const ContentLanguages& contentLanguages,           
	Uint32 contentLength);

    static void appendEMethodResponseHeader(
	Array<char>& out,
        HttpMethod httpMethod,
    const ContentLanguages& contentLanguages,           
	Uint32 contentLength);

    static Array<char> formatSimpleEMethodReqMessage(
        const char* requestUri,
	const char* host,
	const CIMName& eMethodName,
	const String& messageId,
        HttpMethod httpMethod,
	const String& authenticationHeader,
    const AcceptLanguages& httpAcceptLanguages,
    const ContentLanguages& httpContentLanguages,  	
	const Array<char>& body);

    static Array<char> formatSimpleEMethodRspMessage(
	const CIMName& eMethodName,
        const String& messageId,
        HttpMethod httpMethod,
    const ContentLanguages& httpContentLanguages,        
	const Array<char>& body);

    static Array<char> formatSimpleEMethodErrorRspMessage(
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
	Array<char>& out,
	const String& messageId);
    static void _appendMessageElementEnd(
	Array<char>& out);

    static void _appendSimpleReqElementBegin(Array<char>& out);
    static void _appendSimpleReqElementEnd(Array<char>& out);

    static void _appendMethodCallElementBegin(
	Array<char>& out,
	const CIMName& name);
    static void _appendMethodCallElementEnd(
	Array<char>& out);

    static void _appendIMethodCallElementBegin(
	Array<char>& out,
	const CIMName& name);
    static void _appendIMethodCallElementEnd(
	Array<char>& out);

    static void _appendIParamValueElementBegin(
	Array<char>& out,
	const char* name);
    static void _appendIParamValueElementEnd(
	Array<char>& out);

    static void _appendSimpleRspElementBegin(Array<char>& out);
    static void _appendSimpleRspElementEnd(Array<char>& out);

    static void _appendMethodResponseElementBegin(
	Array<char>& out,
	const CIMName& name);
    static void _appendMethodResponseElementEnd(
	Array<char>& out);

    static void _appendIMethodResponseElementBegin(
	Array<char>& out,
	const CIMName& name);
    static void _appendIMethodResponseElementEnd(
	Array<char>& out);

    static void _appendErrorElement(
	Array<char>& out,
	const CIMException& cimException);

    static void _appendIReturnValueElementBegin(Array<char>& out);
    static void _appendIReturnValueElementEnd(Array<char>& out);

    static void _appendSimpleExportReqElementBegin(Array<char>& out);
    static void _appendSimpleExportReqElementEnd(Array<char>& out);

    static void _appendEMethodCallElementBegin(
    	Array<char>& out,
    	const CIMName& name);
    static void _appendEMethodCallElementEnd(
    	Array<char>& out);

    static void _appendEParamValueElementBegin(
	Array<char>& out,
	const char* name);
    static void _appendEParamValueElementEnd(
	Array<char>& out);

    static void _appendSimpleExportRspElementBegin(Array<char>& out);
    static void _appendSimpleExportRspElementEnd(Array<char>& out);

    static void _appendEMethodResponseElementBegin(
    	Array<char>& out,
    	const CIMName& name);
    static void _appendEMethodResponseElementEnd(
    	Array<char>& out);

    XmlWriter() { }
};

PEGASUS_COMMON_LINKAGE Array<char>& operator<<(
    Array<char>& out, 
    const char* x);

PEGASUS_COMMON_LINKAGE Array<char>& operator<<(Array<char>& out, char x);

PEGASUS_COMMON_LINKAGE Array<char>& operator<<(Array<char>& out, const Char16& x);

PEGASUS_COMMON_LINKAGE Array<char>& operator<<(
    Array<char>& out, 
    const String& x);

PEGASUS_COMMON_LINKAGE Array<char>& operator<<(
    Array<char>& out, 
    const Indentor& x);

PEGASUS_COMMON_LINKAGE Array<char>& operator<<(
    Array<char>& out, 
    const Array<char>& x);

PEGASUS_COMMON_LINKAGE Array<char>& operator<<(
    Array<char>& out, 
    Uint32 x);

PEGASUS_COMMON_LINKAGE Array<char>& operator<<(
    Array<char>& out, 
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
