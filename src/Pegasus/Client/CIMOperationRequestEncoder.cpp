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
// Modified By: Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
//              Nag Boranna, Hewlett-Packard Company (nagaraja_boranna@hp.com)
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                (carolann_graves@hp.com)
//				Seema Gupta (gseema@in.ibm.com) for PEP135
//              David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
//              Willis White, IBM (whiwill@us.ibm.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <iostream>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/HTTPMessage.h>
#include "CIMOperationRequestEncoder.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN


CIMOperationRequestEncoder::CIMOperationRequestEncoder(
    MessageQueue* outputQueue,
    const String& hostName,
    ClientAuthenticator* authenticator,
    Uint32 showOutput)
    :
    MessageQueue(PEGASUS_QUEUENAME_OPREQENCODER),
    _outputQueue(outputQueue),
    _hostName(hostName.getCString()),
    _authenticator(authenticator),
    _showOutput(showOutput)
{
}

CIMOperationRequestEncoder::~CIMOperationRequestEncoder()
{
}

void CIMOperationRequestEncoder::handleEnqueue()
{
    Message* message = dequeue();

    if (!message)
	return;

    _authenticator->setRequestMessage(message);

    switch (message->getType())
    {
	case CIM_CREATE_CLASS_REQUEST_MESSAGE:
	    _encodeCreateClassRequest(
		(CIMCreateClassRequestMessage*)message);
	    break;

	case CIM_GET_CLASS_REQUEST_MESSAGE:
	    _encodeGetClassRequest((CIMGetClassRequestMessage*)message);
	    break;

	case CIM_MODIFY_CLASS_REQUEST_MESSAGE:
	    _encodeModifyClassRequest(
		(CIMModifyClassRequestMessage*)message);
	    break;

	case CIM_ENUMERATE_CLASS_NAMES_REQUEST_MESSAGE:
	    _encodeEnumerateClassNamesRequest(
		(CIMEnumerateClassNamesRequestMessage*)message);
	    break;

	case CIM_ENUMERATE_CLASSES_REQUEST_MESSAGE:
	    _encodeEnumerateClassesRequest(
		(CIMEnumerateClassesRequestMessage*)message);
	    break;

	case CIM_DELETE_CLASS_REQUEST_MESSAGE:
	    _encodeDeleteClassRequest(
		(CIMDeleteClassRequestMessage*)message);
	    break;

	case CIM_CREATE_INSTANCE_REQUEST_MESSAGE:
	    _encodeCreateInstanceRequest(
		(CIMCreateInstanceRequestMessage*)message);
	    break;

	case CIM_GET_INSTANCE_REQUEST_MESSAGE:
	    _encodeGetInstanceRequest((CIMGetInstanceRequestMessage*)message);
	    break;

	case CIM_MODIFY_INSTANCE_REQUEST_MESSAGE:
	    _encodeModifyInstanceRequest(
		(CIMModifyInstanceRequestMessage*)message);
	    break;

	case CIM_ENUMERATE_INSTANCE_NAMES_REQUEST_MESSAGE:
	    _encodeEnumerateInstanceNamesRequest(
		(CIMEnumerateInstanceNamesRequestMessage*)message);
	    break;

	case CIM_ENUMERATE_INSTANCES_REQUEST_MESSAGE:
	    _encodeEnumerateInstancesRequest(
		(CIMEnumerateInstancesRequestMessage*)message);
	    break;

	case CIM_DELETE_INSTANCE_REQUEST_MESSAGE:
	    _encodeDeleteInstanceRequest(
		(CIMDeleteInstanceRequestMessage*)message);
	    break;

	case CIM_SET_QUALIFIER_REQUEST_MESSAGE:
	    _encodeSetQualifierRequest(
		(CIMSetQualifierRequestMessage*)message);
	    break;

	case CIM_GET_QUALIFIER_REQUEST_MESSAGE:
	    _encodeGetQualifierRequest(
		(CIMGetQualifierRequestMessage*)message);
	    break;

	case CIM_ENUMERATE_QUALIFIERS_REQUEST_MESSAGE:
	    _encodeEnumerateQualifiersRequest(
		(CIMEnumerateQualifiersRequestMessage*)message);
	    break;

	case CIM_DELETE_QUALIFIER_REQUEST_MESSAGE:
	    _encodeDeleteQualifierRequest(
		(CIMDeleteQualifierRequestMessage*)message);
	    break;

	case CIM_REFERENCE_NAMES_REQUEST_MESSAGE:
	    _encodeReferenceNamesRequest(
		(CIMReferenceNamesRequestMessage*)message);
	    break;

	case CIM_REFERENCES_REQUEST_MESSAGE:
	    _encodeReferencesRequest(
		(CIMReferencesRequestMessage*)message);
	    break;

	case CIM_ASSOCIATOR_NAMES_REQUEST_MESSAGE:
	    _encodeAssociatorNamesRequest(
		(CIMAssociatorNamesRequestMessage*)message);
	    break;

	case CIM_ASSOCIATORS_REQUEST_MESSAGE:
	    _encodeAssociatorsRequest(
		(CIMAssociatorsRequestMessage*)message);
	    break;

	case CIM_EXEC_QUERY_REQUEST_MESSAGE:
	    _encodeExecQueryRequest(
		(CIMExecQueryRequestMessage*)message);
	    break;

	case CIM_GET_PROPERTY_REQUEST_MESSAGE:
	    _encodeGetPropertyRequest((CIMGetPropertyRequestMessage*)message);
	    break;

	case CIM_SET_PROPERTY_REQUEST_MESSAGE:
	    _encodeSetPropertyRequest((CIMSetPropertyRequestMessage*)message);
	    break;

	case CIM_INVOKE_METHOD_REQUEST_MESSAGE:
	    _encodeInvokeMethodRequest(
		(CIMInvokeMethodRequestMessage*)message);
	    break;
    }

    //ATTN: Do not delete the message here.
    //
    // ClientAuthenticator needs this message for resending the request on
    // authentication challenge from the server. The message is deleted in
    // the decoder after receiving the valid response from thr server.
    //
    //delete message;
}

// l10n Added accept language and content language support starting here

void CIMOperationRequestEncoder::_encodeCreateClassRequest(
    CIMCreateClassRequestMessage* message)
{
    Array<char> params;
    XmlWriter::appendClassIParameter(params, "NewClass", message->newClass);
	
	Array<char> buffer = XmlWriter::formatSimpleIMethodReqMessage(_hostName, 
        message->nameSpace, CIMName ("CreateClass"), message->messageId,
        message->getHttpMethod(), 
        _authenticator->buildRequestAuthHeader(), 
		((AcceptLanguageListContainer)message->operationContext.get(AcceptLanguageListContainer::NAME)).getLanguages(),
		((ContentLanguageListContainer)message->operationContext.get(ContentLanguageListContainer::NAME)).getLanguages(),
        params);
    _sendRequest(buffer);
}

void CIMOperationRequestEncoder::_encodeGetClassRequest(
    CIMGetClassRequestMessage* message)
{
    Array<char> params;

    XmlWriter::appendClassNameIParameter(
	params, "ClassName", message->className);
	
    if (message->localOnly != true)
	XmlWriter::appendBooleanIParameter(params, "LocalOnly", false);

    if (message->includeQualifiers != true)
	XmlWriter::appendBooleanIParameter(params, "IncludeQualifiers", false);

    if (message->includeClassOrigin != false)
	XmlWriter::appendBooleanIParameter(params, "IncludeClassOrigin", true);

    if (!message->propertyList.isNull())
	XmlWriter::appendPropertyListIParameter(
	    params, message->propertyList);

    Array<char> buffer = XmlWriter::formatSimpleIMethodReqMessage(
        _hostName, message->nameSpace, CIMName ("GetClass"), message->messageId,
        message->getHttpMethod(),
        _authenticator->buildRequestAuthHeader(),
		((AcceptLanguageListContainer)message->operationContext.get(AcceptLanguageListContainer::NAME)).getLanguages(),
		((ContentLanguageListContainer)message->operationContext.get(ContentLanguageListContainer::NAME)).getLanguages(),
         params);

    _sendRequest(buffer);
}

void CIMOperationRequestEncoder::_encodeModifyClassRequest(
    CIMModifyClassRequestMessage* message)
{
    Array<char> params;

    XmlWriter::appendClassIParameter(
	params, "ModifiedClass", message->modifiedClass);
	
    Array<char> buffer = XmlWriter::formatSimpleIMethodReqMessage(_hostName,
        message->nameSpace, CIMName ("ModifyClass"), message->messageId,
        message->getHttpMethod(),
        _authenticator->buildRequestAuthHeader(),
		((AcceptLanguageListContainer)message->operationContext.get(AcceptLanguageListContainer::NAME)).getLanguages(),
		((ContentLanguageListContainer)message->operationContext.get(ContentLanguageListContainer::NAME)).getLanguages(),
         params);

    _sendRequest(buffer);
}

void CIMOperationRequestEncoder::_encodeEnumerateClassNamesRequest(
    CIMEnumerateClassNamesRequestMessage* message)
{
    Array<char> params;

    if (!message->className.isNull())
	XmlWriter::appendClassNameIParameter(
	    params, "ClassName", message->className);
	
    if (message->deepInheritance != false)
	XmlWriter::appendBooleanIParameter(params, "DeepInheritance", true);

    Array<char> buffer = XmlWriter::formatSimpleIMethodReqMessage(_hostName,
        message->nameSpace, CIMName ("EnumerateClassNames"), message->messageId,
        message->getHttpMethod(),
        _authenticator->buildRequestAuthHeader(),
		((AcceptLanguageListContainer)message->operationContext.get(AcceptLanguageListContainer::NAME)).getLanguages(),
		((ContentLanguageListContainer)message->operationContext.get(ContentLanguageListContainer::NAME)).getLanguages(),
         params);

    _sendRequest(buffer);
}

void CIMOperationRequestEncoder::_encodeEnumerateClassesRequest(
    CIMEnumerateClassesRequestMessage* message)
{
    Array<char> params;

    if (!message->className.isNull())
	XmlWriter::appendClassNameIParameter(
	    params, "ClassName", message->className);
	
    if (message->deepInheritance != false)
	XmlWriter::appendBooleanIParameter(params, "DeepInheritance", true);

    if (message->localOnly != true)
	XmlWriter::appendBooleanIParameter(params, "LocalOnly", false);

    if (message->includeQualifiers != true)
	XmlWriter::appendBooleanIParameter(
	    params, "IncludeQualifiers", false);

    if (message->includeClassOrigin != false)
	XmlWriter::appendBooleanIParameter(
	    params, "IncludeClassOrigin", true);

    Array<char> buffer = XmlWriter::formatSimpleIMethodReqMessage(_hostName,
        message->nameSpace, CIMName ("EnumerateClasses"), message->messageId,
        message->getHttpMethod(),
        _authenticator->buildRequestAuthHeader(),
		((AcceptLanguageListContainer)message->operationContext.get(AcceptLanguageListContainer::NAME)).getLanguages(),
		((ContentLanguageListContainer)message->operationContext.get(ContentLanguageListContainer::NAME)).getLanguages(),
         params);

    _sendRequest(buffer);
}

void CIMOperationRequestEncoder::_encodeDeleteClassRequest(
    CIMDeleteClassRequestMessage* message)
{
    Array<char> params;

    if (!message->className.isNull())
	XmlWriter::appendClassNameIParameter(
	    params, "ClassName", message->className);

    Array<char> buffer = XmlWriter::formatSimpleIMethodReqMessage(_hostName,
        message->nameSpace, CIMName ("DeleteClass"), message->messageId,
        message->getHttpMethod(),
        _authenticator->buildRequestAuthHeader(),
		((AcceptLanguageListContainer)message->operationContext.get(AcceptLanguageListContainer::NAME)).getLanguages(),
		((ContentLanguageListContainer)message->operationContext.get(ContentLanguageListContainer::NAME)).getLanguages(),
         params);

    _sendRequest(buffer);
}

void CIMOperationRequestEncoder::_encodeCreateInstanceRequest(
    CIMCreateInstanceRequestMessage* message)
{
    Array<char> params;

    XmlWriter::appendInstanceIParameter(
	params, "NewInstance", message->newInstance);
	
    Array<char> buffer = XmlWriter::formatSimpleIMethodReqMessage(_hostName,
        message->nameSpace, CIMName ("CreateInstance"), message->messageId,
        message->getHttpMethod(),
        _authenticator->buildRequestAuthHeader(),
		((AcceptLanguageListContainer)message->operationContext.get(AcceptLanguageListContainer::NAME)).getLanguages(),
		((ContentLanguageListContainer)message->operationContext.get(ContentLanguageListContainer::NAME)).getLanguages(),
         params);

    _sendRequest(buffer);
}

void CIMOperationRequestEncoder::_encodeGetInstanceRequest(
    CIMGetInstanceRequestMessage* message)
{
    Array<char> params;

    XmlWriter::appendInstanceNameIParameter(
	params, "InstanceName", message->instanceName);
	
    if (message->localOnly != true)
	XmlWriter::appendBooleanIParameter(
	    params, "LocalOnly", false);

    if (message->includeQualifiers != false)
	XmlWriter::appendBooleanIParameter(
	    params, "IncludeQualifiers", true);

    if (message->includeClassOrigin != false)
	XmlWriter::appendBooleanIParameter(
	    params, "IncludeClassOrigin", true);

    if (!message->propertyList.isNull())
	XmlWriter::appendPropertyListIParameter(
	    params, message->propertyList);

	Array<char> buffer = XmlWriter::formatSimpleIMethodReqMessage(_hostName, 
        message->nameSpace, CIMName ("GetInstance"), message->messageId,
        message->getHttpMethod(),
        _authenticator->buildRequestAuthHeader(),
		((AcceptLanguageListContainer)message->operationContext.get(AcceptLanguageListContainer::NAME)).getLanguages(),
		((ContentLanguageListContainer)message->operationContext.get(ContentLanguageListContainer::NAME)).getLanguages(),
         params);

    _sendRequest(buffer);
}

void CIMOperationRequestEncoder::_encodeModifyInstanceRequest(
    CIMModifyInstanceRequestMessage* message)
{
    Array<char> params;
    XmlWriter::appendNamedInstanceIParameter(
	params, "ModifiedInstance", message->modifiedInstance);
	
    if (message->includeQualifiers != true)
	XmlWriter::appendBooleanIParameter(
	    params, "IncludeQualifiers", false);

    if (!message->propertyList.isNull())
	XmlWriter::appendPropertyListIParameter(
	    params, message->propertyList);

    Array<char> buffer = XmlWriter::formatSimpleIMethodReqMessage(_hostName, 
        message->nameSpace, CIMName ("ModifyInstance"), message->messageId,
        message->getHttpMethod(),
        _authenticator->buildRequestAuthHeader(),
		((AcceptLanguageListContainer)message->operationContext.get(AcceptLanguageListContainer::NAME)).getLanguages(),
		((ContentLanguageListContainer)message->operationContext.get(ContentLanguageListContainer::NAME)).getLanguages(),
         params);

    _sendRequest(buffer);
}

void CIMOperationRequestEncoder::_encodeEnumerateInstanceNamesRequest(
    CIMEnumerateInstanceNamesRequestMessage* message)
{
    Array<char> params;

    XmlWriter::appendClassNameIParameter(
	params, "ClassName", message->className);


    Array<char> buffer = XmlWriter::formatSimpleIMethodReqMessage(_hostName,
        message->nameSpace, CIMName ("EnumerateInstanceNames"), 
        message->messageId, message->getHttpMethod(),
        _authenticator->buildRequestAuthHeader(),
		((AcceptLanguageListContainer)message->operationContext.get(AcceptLanguageListContainer::NAME)).getLanguages(),
		((ContentLanguageListContainer)message->operationContext.get(ContentLanguageListContainer::NAME)).getLanguages(),
         params);

    _sendRequest(buffer);
}

void CIMOperationRequestEncoder::_encodeEnumerateInstancesRequest(
    CIMEnumerateInstancesRequestMessage* message)
{
    Array<char> params;

    XmlWriter::appendClassNameIParameter(
        params, "ClassName", message->className);
	
    if (message->localOnly != true)
	XmlWriter::appendBooleanIParameter(params, "LocalOnly", false);

    if (message->deepInheritance != true)
	XmlWriter::appendBooleanIParameter(params, "DeepInheritance", false);

    if (message->includeQualifiers != false)
	XmlWriter::appendBooleanIParameter(
	    params, "IncludeQualifiers", true);

    if (message->includeClassOrigin != false)
	XmlWriter::appendBooleanIParameter(
	    params, "IncludeClassOrigin", true);

    if (!message->propertyList.isNull())
	XmlWriter::appendPropertyListIParameter(
	    params, message->propertyList);

    Array<char> buffer = XmlWriter::formatSimpleIMethodReqMessage(_hostName,
        message->nameSpace, CIMName ("EnumerateInstances"), message->messageId,
        message->getHttpMethod(),
        _authenticator->buildRequestAuthHeader(),
		((AcceptLanguageListContainer)message->operationContext.get(AcceptLanguageListContainer::NAME)).getLanguages(),
		((ContentLanguageListContainer)message->operationContext.get(ContentLanguageListContainer::NAME)).getLanguages(),
         params);

    _sendRequest(buffer);
}

void CIMOperationRequestEncoder::_encodeDeleteInstanceRequest(
    CIMDeleteInstanceRequestMessage* message)
{
    Array<char> params;

    XmlWriter::appendInstanceNameIParameter(
	params, "InstanceName", message->instanceName);

    Array<char> buffer = XmlWriter::formatSimpleIMethodReqMessage(_hostName, 
        message->nameSpace, CIMName ("DeleteInstance"), message->messageId,
        message->getHttpMethod(),
        _authenticator->buildRequestAuthHeader(),
		((AcceptLanguageListContainer)message->operationContext.get(AcceptLanguageListContainer::NAME)).getLanguages(),
		((ContentLanguageListContainer)message->operationContext.get(ContentLanguageListContainer::NAME)).getLanguages(),
         params);

    _sendRequest(buffer);
}

void CIMOperationRequestEncoder::_encodeGetPropertyRequest(
    CIMGetPropertyRequestMessage* message)
{
    Array<char> params;

    XmlWriter::appendInstanceNameIParameter(
	params, "InstanceName", message->instanceName);
	
    XmlWriter::appendPropertyNameIParameter(
	params, message->propertyName);

    Array<char> buffer = XmlWriter::formatSimpleIMethodReqMessage(_hostName, 
        message->nameSpace, CIMName ("GetProperty"), message->messageId,
        message->getHttpMethod(),
        _authenticator->buildRequestAuthHeader(),
		((AcceptLanguageListContainer)message->operationContext.get(AcceptLanguageListContainer::NAME)).getLanguages(),
		((ContentLanguageListContainer)message->operationContext.get(ContentLanguageListContainer::NAME)).getLanguages(),
         params);

    _sendRequest(buffer);
}

void CIMOperationRequestEncoder::_encodeSetPropertyRequest(
    CIMSetPropertyRequestMessage* message)
{
    Array<char> params;

    XmlWriter::appendInstanceNameIParameter(
	params, "InstanceName", message->instanceName);
	
    XmlWriter::appendPropertyNameIParameter(
	params, message->propertyName);

    if (!message->newValue.isNull())
        XmlWriter::appendPropertyValueIParameter(
	    params, "NewValue", message->newValue);

    Array<char> buffer = XmlWriter::formatSimpleIMethodReqMessage(_hostName, 
        message->nameSpace, CIMName ("SetProperty"), message->messageId,
        message->getHttpMethod(),
        _authenticator->buildRequestAuthHeader(),
		((AcceptLanguageListContainer)message->operationContext.get(AcceptLanguageListContainer::NAME)).getLanguages(),
		((ContentLanguageListContainer)message->operationContext.get(ContentLanguageListContainer::NAME)).getLanguages(),
         params);

    _sendRequest(buffer);
}

void CIMOperationRequestEncoder::_encodeSetQualifierRequest(
    CIMSetQualifierRequestMessage* message)
{
    Array<char> params;
    XmlWriter::appendQualifierDeclarationIParameter(
	params, "QualifierDeclaration", message->qualifierDeclaration);

    Array<char> buffer = XmlWriter::formatSimpleIMethodReqMessage(_hostName,
        message->nameSpace, CIMName ("SetQualifier"), message->messageId,
        message->getHttpMethod(),
        _authenticator->buildRequestAuthHeader(),
		((AcceptLanguageListContainer)message->operationContext.get(AcceptLanguageListContainer::NAME)).getLanguages(),
		((ContentLanguageListContainer)message->operationContext.get(ContentLanguageListContainer::NAME)).getLanguages(),
         params);

    _sendRequest(buffer);
}

void CIMOperationRequestEncoder::_encodeGetQualifierRequest(
    CIMGetQualifierRequestMessage* message)
{
    Array<char> params;

    if (!message->qualifierName.isNull())
	XmlWriter::appendStringIParameter(
	    params, "QualifierName", message->qualifierName.getString());
	
    Array<char> buffer = XmlWriter::formatSimpleIMethodReqMessage(_hostName,
        message->nameSpace, CIMName ("GetQualifier"), message->messageId,
        message->getHttpMethod(),
        _authenticator->buildRequestAuthHeader(),
		((AcceptLanguageListContainer)message->operationContext.get(AcceptLanguageListContainer::NAME)).getLanguages(),
		((ContentLanguageListContainer)message->operationContext.get(ContentLanguageListContainer::NAME)).getLanguages(),
         params);

    _sendRequest(buffer);
}

void CIMOperationRequestEncoder::_encodeEnumerateQualifiersRequest(
    CIMEnumerateQualifiersRequestMessage* message)
{
    Array<char> params;

    Array<char> buffer = XmlWriter::formatSimpleIMethodReqMessage(_hostName, 
        message->nameSpace, CIMName ("EnumerateQualifiers"), message->messageId,
        message->getHttpMethod(), 
        _authenticator->buildRequestAuthHeader(),
		((AcceptLanguageListContainer)message->operationContext.get(AcceptLanguageListContainer::NAME)).getLanguages(),
		((ContentLanguageListContainer)message->operationContext.get(ContentLanguageListContainer::NAME)).getLanguages(),
         params);

    _sendRequest(buffer);
}

void CIMOperationRequestEncoder::_encodeDeleteQualifierRequest(
    CIMDeleteQualifierRequestMessage* message)
{
    Array<char> params;

    if (!message->qualifierName.isNull())
	XmlWriter::appendStringIParameter(
	    params, "QualifierName", message->qualifierName.getString());
	
    Array<char> buffer = XmlWriter::formatSimpleIMethodReqMessage(_hostName,
        message->nameSpace, CIMName ("DeleteQualifier"), message->messageId,
        message->getHttpMethod(), 
        _authenticator->buildRequestAuthHeader(),
		((AcceptLanguageListContainer)message->operationContext.get(AcceptLanguageListContainer::NAME)).getLanguages(),
		((ContentLanguageListContainer)message->operationContext.get(ContentLanguageListContainer::NAME)).getLanguages(),
         params);

    _sendRequest(buffer);
}

void CIMOperationRequestEncoder::_encodeReferenceNamesRequest(
    CIMReferenceNamesRequestMessage* message)
{
    Array<char> params;

    XmlWriter::appendObjectNameIParameter(
	params, "ObjectName", message->objectName);

    XmlWriter::appendClassNameIParameter(
	params, "ResultClass", message->resultClass);

    //
    //  The Client API has no way to represent a NULL role;
    //  An empty string is interpreted as NULL
    //
    if (message->role != String::EMPTY)
    {
        XmlWriter::appendStringIParameter(
	    params, "Role", message->role);
    }

    Array<char> buffer = XmlWriter::formatSimpleIMethodReqMessage(_hostName,
        message->nameSpace, CIMName ("ReferenceNames"), message->messageId,
        message->getHttpMethod(),
        _authenticator->buildRequestAuthHeader(),
		((AcceptLanguageListContainer)message->operationContext.get(AcceptLanguageListContainer::NAME)).getLanguages(),
		((ContentLanguageListContainer)message->operationContext.get(ContentLanguageListContainer::NAME)).getLanguages(),
         params);

    _sendRequest(buffer);
}

void CIMOperationRequestEncoder::_encodeReferencesRequest(
    CIMReferencesRequestMessage* message)
{
    Array<char> params;

    XmlWriter::appendObjectNameIParameter(
	params, "ObjectName", message->objectName);

    XmlWriter::appendClassNameIParameter(
	params, "ResultClass", message->resultClass);

    //
    //  The Client API has no way to represent a NULL role;
    //  An empty string is interpreted as NULL
    //
    if (message->role != String::EMPTY)
    {
        XmlWriter::appendStringIParameter(
	    params, "Role", message->role);
    }

    if (message->includeQualifiers != false)
	XmlWriter::appendBooleanIParameter(params, "IncludeQualifiers", true);

    if (message->includeClassOrigin != false)
	XmlWriter::appendBooleanIParameter(params, "IncludeClassOrigin", true);

    if (!message->propertyList.isNull())
	XmlWriter::appendPropertyListIParameter(
	    params, message->propertyList);

    Array<char> buffer = XmlWriter::formatSimpleIMethodReqMessage(_hostName,
        message->nameSpace, CIMName ("References"), message->messageId,
        message->getHttpMethod(),
        _authenticator->buildRequestAuthHeader(),
		((AcceptLanguageListContainer)message->operationContext.get(AcceptLanguageListContainer::NAME)).getLanguages(),
		((ContentLanguageListContainer)message->operationContext.get(ContentLanguageListContainer::NAME)).getLanguages(),
         params);

    _sendRequest(buffer);
}

void CIMOperationRequestEncoder::_encodeAssociatorNamesRequest(
    CIMAssociatorNamesRequestMessage* message)
{
    Array<char> params;

    XmlWriter::appendObjectNameIParameter(
	params, "ObjectName", message->objectName);

    XmlWriter::appendClassNameIParameter(
	params, "AssocClass", message->assocClass);

    XmlWriter::appendClassNameIParameter(
	params, "ResultClass", message->resultClass);

    //
    //  The Client API has no way to represent a NULL role;
    //  An empty string is interpreted as NULL
    //
    if (message->role != String::EMPTY)
    {
        XmlWriter::appendStringIParameter(
	    params, "Role", message->role);
    }

    //
    //  The Client API has no way to represent a NULL resultRole;
    //  An empty string is interpreted as NULL
    //
    if (message->resultRole != String::EMPTY)
    {
        XmlWriter::appendStringIParameter(
	    params, "ResultRole", message->resultRole);
    }

    Array<char> buffer = XmlWriter::formatSimpleIMethodReqMessage(_hostName,
        message->nameSpace, CIMName ("AssociatorNames"), message->messageId,
        message->getHttpMethod(),
        _authenticator->buildRequestAuthHeader(),
		((AcceptLanguageListContainer)message->operationContext.get(AcceptLanguageListContainer::NAME)).getLanguages(),
		((ContentLanguageListContainer)message->operationContext.get(ContentLanguageListContainer::NAME)).getLanguages(),
         params);

    _sendRequest(buffer);
}

void CIMOperationRequestEncoder::_encodeAssociatorsRequest(
    CIMAssociatorsRequestMessage* message)
{
    Array<char> params;

    XmlWriter::appendObjectNameIParameter(
	params, "ObjectName", message->objectName);

    XmlWriter::appendClassNameIParameter(
	params, "AssocClass", message->assocClass);

    XmlWriter::appendClassNameIParameter(
	params, "ResultClass", message->resultClass);

    //
    //  The Client API has no way to represent a NULL role;
    //  An empty string is interpreted as NULL
    //
    if (message->role != String::EMPTY)
    {
        XmlWriter::appendStringIParameter(
	    params, "Role", message->role);
    }

    //
    //  The Client API has no way to represent a NULL resultRole;
    //  An empty string is interpreted as NULL
    //
    if (message->resultRole != String::EMPTY)
    {
        XmlWriter::appendStringIParameter(
	    params, "ResultRole", message->resultRole);
    }

    if (message->includeQualifiers != false)
	XmlWriter::appendBooleanIParameter(params, "IncludeQualifiers", true);

    if (message->includeClassOrigin != false)
	XmlWriter::appendBooleanIParameter(params, "IncludeClassOrigin", true);

    if (!message->propertyList.isNull())
	XmlWriter::appendPropertyListIParameter(
	    params, message->propertyList);

    Array<char> buffer = XmlWriter::formatSimpleIMethodReqMessage(_hostName,
        message->nameSpace, CIMName ("Associators"), message->messageId,
        message->getHttpMethod(),
        _authenticator->buildRequestAuthHeader(),
		((AcceptLanguageListContainer)message->operationContext.get(AcceptLanguageListContainer::NAME)).getLanguages(),
		((ContentLanguageListContainer)message->operationContext.get(ContentLanguageListContainer::NAME)).getLanguages(),
         params);

    _sendRequest(buffer);
}

void CIMOperationRequestEncoder::_encodeExecQueryRequest(
    CIMExecQueryRequestMessage* message)
{
    Array<char> params;

    XmlWriter::appendStringIParameter(
	params, "QueryLanguage", message->queryLanguage);

    XmlWriter::appendStringIParameter(
	params, "Query", message->query);

    Array<char> buffer = XmlWriter::formatSimpleIMethodReqMessage(_hostName,
        message->nameSpace, CIMName ("ExecQuery"), message->messageId,
        message->getHttpMethod(),
        _authenticator->buildRequestAuthHeader(),
		((AcceptLanguageListContainer)message->operationContext.get(AcceptLanguageListContainer::NAME)).getLanguages(),
		((ContentLanguageListContainer)message->operationContext.get(ContentLanguageListContainer::NAME)).getLanguages(),
         params);

    _sendRequest(buffer);
}

void CIMOperationRequestEncoder::_encodeInvokeMethodRequest(
    CIMInvokeMethodRequestMessage* message)
{
    Array<char> buffer = XmlWriter::formatSimpleMethodReqMessage(_hostName,
	message->nameSpace, message->instanceName, message->methodName,
	message->inParameters, message->messageId, 
	message->getHttpMethod(), _authenticator->buildRequestAuthHeader(),
		((AcceptLanguageListContainer)message->operationContext.get(AcceptLanguageListContainer::NAME)).getLanguages(),
		((ContentLanguageListContainer)message->operationContext.get(ContentLanguageListContainer::NAME)).getLanguages());
        
    _sendRequest(buffer);
}

// Enqueue the buffer to the ouptut queue with a conditional display.
// This function is only enabled if the Pegasus Client trace is enabled.
// Uses parameter to determine whether to send to console to log.
void CIMOperationRequestEncoder::_sendRequest(Array<char>& buffer)
{
#ifdef PEGASUS_CLIENT_TRACE_ENABLE
    if (_showOutput & 1)
    {
        buffer.append('\0');
        XmlWriter::indentedPrint(cout, buffer.getData());
        cout << endl;
        buffer.remove(buffer.size() - 1);
    }
	if (_showOutput & 2)
    {
        buffer.append('\0');
        Logger::put(Logger::STANDARD_LOG,
					"CIMCLIENT",
					Logger::TRACE,
					"CIMOperationRequestEncoder::SendRequest, XML content: $1",
					buffer.getData());
        buffer.remove(buffer.size() - 1);
    }
#endif

    HTTPMessage * http_request = new HTTPMessage(buffer);

            /* collecting statistical information 
                - requestSize (contentLength) and network Start Time
            */
    ClientPerfDataStore* dataStore_prt = ClientPerfDataStore::Instance();

        //these variables are needed to call HTTPMessage::parse, all we need is contentLength
    String startLine;
    Array<HTTPHeader>  headers;
	Uint32  contentLength;

    http_request->parse(startLine, headers, contentLength);
    dataStore_prt->setRequestSize(contentLength);
    dataStore_prt->setStartNetworkTime();
    
    _outputQueue->enqueue(http_request);
}


PEGASUS_NAMESPACE_END
