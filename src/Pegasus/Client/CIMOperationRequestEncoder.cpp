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
// Modified By: Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
//              Nag Boranna, Hewlett-Packard Company (nagaraja_boranna@hp.com)
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/HTTPMessage.h>
#include "CIMOperationRequestEncoder.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

CIMOperationRequestEncoder::CIMOperationRequestEncoder(
    MessageQueue* outputQueue, ClientAuthenticator* authenticator)
    :
    _outputQueue(outputQueue),
    _authenticator(authenticator)
{
    String tmpHostName = System::getHostName();
    _hostName = tmpHostName.allocateCString();
}

CIMOperationRequestEncoder::~CIMOperationRequestEncoder()
{
    delete [] _hostName;
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

	// ATTN: implement this!
	case CIM_EXEC_QUERY_REQUEST_MESSAGE:
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

const char* CIMOperationRequestEncoder::getQueueName() const
{
    return "CIMOperationRequestEncoder";
}

void CIMOperationRequestEncoder::_encodeCreateClassRequest(
    CIMCreateClassRequestMessage* message)
{
    Array<Sint8> params;
    XmlWriter::appendClassParameter(params, "NewClass", message->newClass);
	
    Array<Sint8> buffer = XmlWriter::formatSimpleIMethodReqMessage(_hostName, 
        message->nameSpace, "CreateClass", message->messageId,
        _authenticator->buildRequestAuthHeader(), params);

    _outputQueue->enqueue(new HTTPMessage(buffer));
}

void CIMOperationRequestEncoder::_encodeGetClassRequest(
    CIMGetClassRequestMessage* message)
{
    Array<Sint8> params;

    XmlWriter::appendClassNameParameter(
	params, "ClassName", message->className);
	
    if (message->localOnly != true)
	XmlWriter::appendBooleanParameter(params, "LocalOnly", false);

    if (message->includeQualifiers != true)
	XmlWriter::appendBooleanParameter(params, "IncludeQualifiers", false);

    if (message->includeClassOrigin != false)
	XmlWriter::appendBooleanParameter(params, "IncludeClassOrigin", true);

    if (!message->propertyList.isNull())
	XmlWriter::appendPropertyListParameter(
	    params, message->propertyList);

    Array<Sint8> buffer = XmlWriter::formatSimpleIMethodReqMessage(
        _hostName, message->nameSpace, "GetClass", message->messageId,
        _authenticator->buildRequestAuthHeader(), params);

    _outputQueue->enqueue(new HTTPMessage(buffer));
}

void CIMOperationRequestEncoder::_encodeModifyClassRequest(
    CIMModifyClassRequestMessage* message)
{
    Array<Sint8> params;

    XmlWriter::appendClassParameter(
	params, "ModifiedClass", message->modifiedClass);
	
    Array<Sint8> buffer = XmlWriter::formatSimpleIMethodReqMessage(_hostName,
        message->nameSpace, "ModifyClass", message->messageId,
        _authenticator->buildRequestAuthHeader(), params);


    _outputQueue->enqueue(new HTTPMessage(buffer));
}

void CIMOperationRequestEncoder::_encodeEnumerateClassNamesRequest(
    CIMEnumerateClassNamesRequestMessage* message)
{
    Array<Sint8> params;

    if (message->className.size())
	XmlWriter::appendClassNameParameter(
	    params, "ClassName", message->className);
	
    if (message->deepInheritance != false)
	XmlWriter::appendBooleanParameter(params, "DeepInheritance", true);

    Array<Sint8> buffer = XmlWriter::formatSimpleIMethodReqMessage(_hostName,
        message->nameSpace, "EnumerateClassNames", message->messageId,
        _authenticator->buildRequestAuthHeader(), params);

    _outputQueue->enqueue(new HTTPMessage(buffer));
}

void CIMOperationRequestEncoder::_encodeEnumerateClassesRequest(
    CIMEnumerateClassesRequestMessage* message)
{
    Array<Sint8> params;

    if (message->className.size())
	XmlWriter::appendClassNameParameter(
	    params, "ClassName", message->className);
	
    if (message->deepInheritance != false)
	XmlWriter::appendBooleanParameter(params, "DeepInheritance", true);

    if (message->localOnly != true)
	XmlWriter::appendBooleanParameter(params, "LocalOnly", false);

    if (message->includeQualifiers != true)
	XmlWriter::appendBooleanParameter(
	    params, "IncludeQualifiers", false);

    if (message->includeClassOrigin != false)
	XmlWriter::appendBooleanParameter(
	    params, "IncludeClassOrigin", true);

    Array<Sint8> buffer = XmlWriter::formatSimpleIMethodReqMessage(_hostName,
        message->nameSpace, "EnumerateClasses", message->messageId,
        _authenticator->buildRequestAuthHeader(), params);

    _outputQueue->enqueue(new HTTPMessage(buffer));
}

void CIMOperationRequestEncoder::_encodeDeleteClassRequest(
    CIMDeleteClassRequestMessage* message)
{
    Array<Sint8> params;

    if (message->className.size())
	XmlWriter::appendClassNameParameter(
	    params, "ClassName", message->className);

    Array<Sint8> buffer = XmlWriter::formatSimpleIMethodReqMessage(_hostName,
        message->nameSpace, "DeleteClass", message->messageId,
        _authenticator->buildRequestAuthHeader(), params);

    _outputQueue->enqueue(new HTTPMessage(buffer));
}

void CIMOperationRequestEncoder::_encodeCreateInstanceRequest(
    CIMCreateInstanceRequestMessage* message)
{
    Array<Sint8> params;

    XmlWriter::appendInstanceParameter(
	params, "NewInstance", message->newInstance);
	
    Array<Sint8> buffer = XmlWriter::formatSimpleIMethodReqMessage(_hostName,
        message->nameSpace, "CreateInstance", message->messageId,
        _authenticator->buildRequestAuthHeader(), params);

    _outputQueue->enqueue(new HTTPMessage(buffer));
}

void CIMOperationRequestEncoder::_encodeGetInstanceRequest(
    CIMGetInstanceRequestMessage* message)
{
    Array<Sint8> params;

    XmlWriter::appendInstanceNameParameter(
	params, "InstanceName", message->instanceName);
	
    if (message->localOnly != true)
	XmlWriter::appendBooleanParameter(
	    params, "LocalOnly", false);

    if (message->includeQualifiers != false)
	XmlWriter::appendBooleanParameter(
	    params, "IncludeQualifiers", true);

    if (message->includeClassOrigin != false)
	XmlWriter::appendBooleanParameter(
	    params, "IncludeClassOrigin", true);

    if (!message->propertyList.isNull())
	XmlWriter::appendPropertyListParameter(
	    params, message->propertyList);

    Array<Sint8> buffer = XmlWriter::formatSimpleIMethodReqMessage(_hostName, 
        message->nameSpace, "GetInstance", message->messageId,
        _authenticator->buildRequestAuthHeader(), params);

    _outputQueue->enqueue(new HTTPMessage(buffer));
}

void CIMOperationRequestEncoder::_encodeModifyInstanceRequest(
    CIMModifyInstanceRequestMessage* message)
{
    Array<Sint8> params;
    XmlWriter::appendNamedInstanceParameter(
	params, "ModifiedInstance", message->modifiedInstance);
	
    if (message->includeQualifiers != true)
	XmlWriter::appendBooleanParameter(
	    params, "IncludeQualifiers", false);

    if (!message->propertyList.isNull())
	XmlWriter::appendPropertyListParameter(
	    params, message->propertyList);

    Array<Sint8> buffer = XmlWriter::formatSimpleIMethodReqMessage(_hostName, 
        message->nameSpace, "ModifyInstance", message->messageId,
        _authenticator->buildRequestAuthHeader(), params);

    _outputQueue->enqueue(new HTTPMessage(buffer));
}

void CIMOperationRequestEncoder::_encodeEnumerateInstanceNamesRequest(
    CIMEnumerateInstanceNamesRequestMessage* message)
{
    Array<Sint8> params;

    XmlWriter::appendClassNameParameter(
	params, "ClassName", message->className);
	
    Array<Sint8> buffer = XmlWriter::formatSimpleIMethodReqMessage(_hostName,
        message->nameSpace, "EnumerateInstanceNames", message->messageId,
        _authenticator->buildRequestAuthHeader(), params);

    _outputQueue->enqueue(new HTTPMessage(buffer));
}

void CIMOperationRequestEncoder::_encodeEnumerateInstancesRequest(
    CIMEnumerateInstancesRequestMessage* message)
{
    Array<Sint8> params;

    XmlWriter::appendClassNameParameter(
        params, "ClassName", message->className);
	
    if (message->localOnly != true)
	XmlWriter::appendBooleanParameter(params, "LocalOnly", false);

    if (message->deepInheritance != true)
	XmlWriter::appendBooleanParameter(params, "DeepInheritance", false);

    if (message->includeQualifiers != false)
	XmlWriter::appendBooleanParameter(
	    params, "IncludeQualifiers", true);

    if (message->includeClassOrigin != false)
	XmlWriter::appendBooleanParameter(
	    params, "IncludeClassOrigin", true);

    if (!message->propertyList.isNull())
	XmlWriter::appendPropertyListParameter(
	    params, message->propertyList);

    Array<Sint8> buffer = XmlWriter::formatSimpleIMethodReqMessage(_hostName,
        message->nameSpace, "EnumerateInstances", message->messageId,
        _authenticator->buildRequestAuthHeader(), params);

    _outputQueue->enqueue(new HTTPMessage(buffer));
}

void CIMOperationRequestEncoder::_encodeDeleteInstanceRequest(
    CIMDeleteInstanceRequestMessage* message)
{
    Array<Sint8> params;

    XmlWriter::appendInstanceNameParameter(
	params, "InstanceName", message->instanceName);

    Array<Sint8> buffer = XmlWriter::formatSimpleIMethodReqMessage(_hostName, 
        message->nameSpace, "DeleteInstance", message->messageId,
        _authenticator->buildRequestAuthHeader(), params);

    _outputQueue->enqueue(new HTTPMessage(buffer));
}

void CIMOperationRequestEncoder::_encodeGetPropertyRequest(
    CIMGetPropertyRequestMessage* message)
{
    Array<Sint8> params;

    XmlWriter::appendInstanceNameParameter(
	params, "InstanceName", message->instanceName);
	
    XmlWriter::appendPropertyNameParameter(
	params, message->propertyName);

    Array<Sint8> buffer = XmlWriter::formatSimpleIMethodReqMessage(_hostName, 
        message->nameSpace, "GetProperty", message->messageId,
        _authenticator->buildRequestAuthHeader(), params);

    _outputQueue->enqueue(new HTTPMessage(buffer));
}

void CIMOperationRequestEncoder::_encodeSetPropertyRequest(
    CIMSetPropertyRequestMessage* message)
{
    Array<Sint8> params;

    XmlWriter::appendInstanceNameParameter(
	params, "InstanceName", message->instanceName);
	
    XmlWriter::appendPropertyNameParameter(
	params, message->propertyName);

    if (!message->newValue.isNull())
        XmlWriter::appendPropertyValueParameter(
	    params, "NewValue", message->newValue);

    Array<Sint8> buffer = XmlWriter::formatSimpleIMethodReqMessage(_hostName, 
        message->nameSpace, "SetProperty", message->messageId,
        _authenticator->buildRequestAuthHeader(), params);

    _outputQueue->enqueue(new HTTPMessage(buffer));
}

void CIMOperationRequestEncoder::_encodeSetQualifierRequest(
    CIMSetQualifierRequestMessage* message)
{
    Array<Sint8> params;
    XmlWriter::appendQualifierDeclarationParameter(
	params, "QualifierDeclaration", message->qualifierDeclaration);

    Array<Sint8> buffer = XmlWriter::formatSimpleIMethodReqMessage(_hostName,
        message->nameSpace, "SetQualifier", message->messageId,
        _authenticator->buildRequestAuthHeader(), params);

    _outputQueue->enqueue(new HTTPMessage(buffer));
}

void CIMOperationRequestEncoder::_encodeGetQualifierRequest(
    CIMGetQualifierRequestMessage* message)
{
    Array<Sint8> params;

    if (message->qualifierName.size())
	XmlWriter::appendClassNameParameter(
	    params, "QualifierName", message->qualifierName);
	
    Array<Sint8> buffer = XmlWriter::formatSimpleIMethodReqMessage(_hostName,
        message->nameSpace, "GetQualifier", message->messageId,
        _authenticator->buildRequestAuthHeader(), params);

    _outputQueue->enqueue(new HTTPMessage(buffer));
}

void CIMOperationRequestEncoder::_encodeEnumerateQualifiersRequest(
    CIMEnumerateQualifiersRequestMessage* message)
{
    Array<Sint8> params;

    Array<Sint8> buffer = XmlWriter::formatSimpleIMethodReqMessage(_hostName, 
        message->nameSpace, "EnumerateQualifiers", message->messageId,
        _authenticator->buildRequestAuthHeader(), params);

    _outputQueue->enqueue(new HTTPMessage(buffer));
}

void CIMOperationRequestEncoder::_encodeDeleteQualifierRequest(
    CIMDeleteQualifierRequestMessage* message)
{
    Array<Sint8> params;

    if (message->qualifierName.size())
	XmlWriter::appendClassNameParameter(
	    params, "QualifierName", message->qualifierName);
	
    Array<Sint8> buffer = XmlWriter::formatSimpleIMethodReqMessage(_hostName,
        message->nameSpace, "DeleteQualifier", message->messageId,
        _authenticator->buildRequestAuthHeader(), params);

    _outputQueue->enqueue(new HTTPMessage(buffer));
}

void CIMOperationRequestEncoder::_encodeReferenceNamesRequest(
    CIMReferenceNamesRequestMessage* message)
{
    Array<Sint8> params;

    XmlWriter::appendObjectNameParameter(
	params, "ObjectName", message->objectName);

    XmlWriter::appendClassNameParameter(
	params, "ResultClass", message->resultClass);

    XmlWriter::appendStringIParameter(
	params, "Role", message->role);

    Array<Sint8> buffer = XmlWriter::formatSimpleIMethodReqMessage(_hostName,
        message->nameSpace, "ReferenceNames", message->messageId,
        _authenticator->buildRequestAuthHeader(), params);

    _outputQueue->enqueue(new HTTPMessage(buffer));
}

void CIMOperationRequestEncoder::_encodeReferencesRequest(
    CIMReferencesRequestMessage* message)
{
    Array<Sint8> params;

    XmlWriter::appendObjectNameParameter(
	params, "ObjectName", message->objectName);

    XmlWriter::appendClassNameParameter(
	params, "ResultClass", message->resultClass);

    XmlWriter::appendStringIParameter(
	params, "Role", message->role);

    if (message->includeQualifiers != false)
	XmlWriter::appendBooleanParameter(params, "IncludeQualifiers", true);

    if (message->includeClassOrigin != false)
	XmlWriter::appendBooleanParameter(params, "IncludeClassOrigin", true);

    if (!message->propertyList.isNull())
	XmlWriter::appendPropertyListParameter(
	    params, message->propertyList);

    Array<Sint8> buffer = XmlWriter::formatSimpleIMethodReqMessage(_hostName,
        message->nameSpace, "References", message->messageId,
        _authenticator->buildRequestAuthHeader(), params);

    _outputQueue->enqueue(new HTTPMessage(buffer));
}

void CIMOperationRequestEncoder::_encodeAssociatorNamesRequest(
    CIMAssociatorNamesRequestMessage* message)
{
    Array<Sint8> params;

    XmlWriter::appendObjectNameParameter(
	params, "ObjectName", message->objectName);

    XmlWriter::appendClassNameParameter(
	params, "AssocClass", message->assocClass);

    XmlWriter::appendClassNameParameter(
	params, "ResultClass", message->resultClass);

    XmlWriter::appendStringIParameter(
	params, "Role", message->role);

    XmlWriter::appendStringIParameter(
	params, "ResultRole", message->resultRole);

    Array<Sint8> buffer = XmlWriter::formatSimpleIMethodReqMessage(_hostName,
        message->nameSpace, "AssociatorNames", message->messageId,
        _authenticator->buildRequestAuthHeader(), params);

    _outputQueue->enqueue(new HTTPMessage(buffer));
}

void CIMOperationRequestEncoder::_encodeAssociatorsRequest(
    CIMAssociatorsRequestMessage* message)
{
    Array<Sint8> params;

    XmlWriter::appendObjectNameParameter(
	params, "ObjectName", message->objectName);

    XmlWriter::appendClassNameParameter(
	params, "AssocClass", message->assocClass);

    XmlWriter::appendClassNameParameter(
	params, "ResultClass", message->resultClass);

    XmlWriter::appendStringIParameter(
	params, "Role", message->role);

    XmlWriter::appendStringIParameter(
	params, "ResultRole", message->resultRole);

    if (message->includeQualifiers != false)
	XmlWriter::appendBooleanParameter(params, "IncludeQualifiers", true);

    if (message->includeClassOrigin != false)
	XmlWriter::appendBooleanParameter(params, "IncludeClassOrigin", true);

    if (!message->propertyList.isNull())
	XmlWriter::appendPropertyListParameter(
	    params, message->propertyList);

    Array<Sint8> buffer = XmlWriter::formatSimpleIMethodReqMessage(_hostName,
        message->nameSpace, "Associators", message->messageId,
        _authenticator->buildRequestAuthHeader(), params);

    _outputQueue->enqueue(new HTTPMessage(buffer));
}

void CIMOperationRequestEncoder::_encodeInvokeMethodRequest(
    CIMInvokeMethodRequestMessage* message)
{
    Array<Sint8> params;

    message->instanceName.localObjectPathtoXml(params);

    for (Uint32 i=0; i < message->inParameters.size(); i++)
    {
        // ATTN: Need to support non-String parameter values
	XmlWriter::appendStringParameter(
	    params, 
	    _CString(message->inParameters[i].getParameter().getName()),
	    message->inParameters[i].getValue().toString());
    }

    Array<Sint8> buffer = XmlWriter::formatSimpleMethodReqMessage(_hostName,
	message->nameSpace, _CString(message->methodName),
	message->messageId, _authenticator->buildRequestAuthHeader(), params);
    
    _outputQueue->enqueue(new HTTPMessage(buffer));
}

PEGASUS_NAMESPACE_END
