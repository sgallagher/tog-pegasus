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
// Modified By: Yi Zhou (yi_zhou@hp.com)
//              Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <cctype>
#include <cstdio>
#include <Pegasus/Common/XmlParser.h>
#include <Pegasus/Common/XmlReader.h>
#include <Pegasus/Common/Destroyer.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/HTTPMessage.h>
#include <Pegasus/Common/Logger.h>
#include <Pegasus/Common/Tracer.h>
#include "CIMOperationResponseEncoder.h"

#ifdef PEGASUS_CCOVER
# include <ccover.h>
#endif

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

CIMOperationResponseEncoder::CIMOperationResponseEncoder()
{

}

CIMOperationResponseEncoder::~CIMOperationResponseEncoder()
{

}

void CIMOperationResponseEncoder::sendResponse(
    Uint32 queueId, 
    Array<Sint8>& message)
{
    PEG_FUNC_ENTER(TRC_DISPATCHER,"CIMOperationResponseEncoder::sendResponse()");
    MessageQueue* queue = MessageQueue::lookup(queueId);

    if (queue)

    {
      	HTTPMessage* httpMessage = new HTTPMessage(message);
      	Tracer::traceBuffer(TRC_XML_IO, Tracer::LEVEL2, 
		 httpMessage->message.getData(), httpMessage->message.size());

	queue->enqueue(httpMessage);



#ifdef PEGASUS_CCOVER
        cov_write();
#endif
    }
    PEG_FUNC_EXIT(TRC_DISPATCHER,"CIMOperationResponseEncoder::sendResponse()");
}

void CIMOperationResponseEncoder::sendError(
    Uint32 queueId, 
    const String& messageId,
    const String& cimMethodName,
    CIMStatusCode code,
    const String& description) 
{
    ArrayDestroyer<char> tmp1(cimMethodName.allocateCString());
    ArrayDestroyer<char> tmp2(description.allocateCString());

    Array<Sint8> message = XmlWriter::formatMethodResponseHeader(
	XmlWriter::formatMessageElement(
	    messageId,
	    XmlWriter::formatSimpleRspElement(
		XmlWriter::formatIMethodResponseElement(
		    tmp1.getPointer(),
		    XmlWriter::formatErrorElement(code, tmp2.getPointer())))));
    
    sendResponse(queueId, message);
}

void CIMOperationResponseEncoder::sendError(
    CIMResponseMessage* response,
    const String& cimMethodName)
{
    Uint32 queueId = response->queueIds.top();
    response->queueIds.pop();

    sendError(
	queueId,
	response->messageId, 
	cimMethodName, 
	response->errorCode, 
	response->errorDescription);
}

void CIMOperationResponseEncoder::handleEnqueue()
{
    Message* message = dequeue();

    if (!message)
	return;

    switch (message->getType())
    {
	case CIM_GET_CLASS_RESPONSE_MESSAGE:
	    encodeGetClassResponse(
		(CIMGetClassResponseMessage*)message);
	    break;

	case CIM_GET_INSTANCE_RESPONSE_MESSAGE:
	    encodeGetInstanceResponse(
		(CIMGetInstanceResponseMessage*)message);
	    break;

	case CIM_DELETE_CLASS_RESPONSE_MESSAGE:
	    encodeDeleteClassResponse(
		(CIMDeleteClassResponseMessage*)message);
	    break;

	case CIM_DELETE_INSTANCE_RESPONSE_MESSAGE:
	    encodeDeleteInstanceResponse(
		(CIMDeleteInstanceResponseMessage*)message);
	    break;

	case CIM_CREATE_CLASS_RESPONSE_MESSAGE:
	    encodeCreateClassResponse(
		(CIMCreateClassResponseMessage*)message);
	    break;

	case CIM_CREATE_INSTANCE_RESPONSE_MESSAGE:
	    encodeCreateInstanceResponse(
		(CIMCreateInstanceResponseMessage*)message);
	    break;

	case CIM_MODIFY_CLASS_RESPONSE_MESSAGE:
	    encodeModifyClassResponse(
		(CIMModifyClassResponseMessage*)message);
	    break;

	case CIM_MODIFY_INSTANCE_RESPONSE_MESSAGE:
	    encodeModifyInstanceResponse(
		(CIMModifyInstanceResponseMessage*)message);
	    break;

	case CIM_ENUMERATE_CLASSES_RESPONSE_MESSAGE:
	    encodeEnumerateClassesResponse(
		(CIMEnumerateClassesResponseMessage*)message);
	    break;

	case CIM_ENUMERATE_CLASS_NAMES_RESPONSE_MESSAGE:
	    encodeEnumerateClassNamesResponse(
		(CIMEnumerateClassNamesResponseMessage*)message);
	    break;

	case CIM_ENUMERATE_INSTANCES_RESPONSE_MESSAGE:
	    encodeEnumerateInstancesResponse(
		(CIMEnumerateInstancesResponseMessage*)message);
	    break;

	case CIM_ENUMERATE_INSTANCE_NAMES_RESPONSE_MESSAGE:
	    encodeEnumerateInstanceNamesResponse(
		(CIMEnumerateInstanceNamesResponseMessage*)message);
	    break;

	// ATTN: implement this!
	case CIM_EXEC_QUERY_RESPONSE_MESSAGE:
	    break;

	case CIM_ASSOCIATORS_RESPONSE_MESSAGE:
	    encodeAssociatorsResponse(
		(CIMAssociatorsResponseMessage*)message);
	    break;

	case CIM_ASSOCIATOR_NAMES_RESPONSE_MESSAGE:
	    encodeAssociatorNamesResponse(
		(CIMAssociatorNamesResponseMessage*)message);
	    break;

	case CIM_REFERENCES_RESPONSE_MESSAGE:
	    encodeReferencesResponse(
		(CIMReferencesResponseMessage*)message);
	    break;

	case CIM_REFERENCE_NAMES_RESPONSE_MESSAGE:
	    encodeReferenceNamesResponse(
		(CIMReferenceNamesResponseMessage*)message);
	    break;

	case CIM_GET_PROPERTY_RESPONSE_MESSAGE:
	    encodeGetPropertyResponse(
		(CIMGetPropertyResponseMessage*)message);
	    break;

	case CIM_SET_PROPERTY_RESPONSE_MESSAGE:
	    encodeSetPropertyResponse(
		(CIMSetPropertyResponseMessage*)message);
	    break;

	case CIM_GET_QUALIFIER_RESPONSE_MESSAGE:
	    encodeGetQualifierResponse(
		(CIMGetQualifierResponseMessage*)message);
	    break;

	case CIM_SET_QUALIFIER_RESPONSE_MESSAGE:
	    encodeSetQualifierResponse(
		(CIMSetQualifierResponseMessage*)message);
	    break;

	case CIM_DELETE_QUALIFIER_RESPONSE_MESSAGE:
	    encodeDeleteQualifierResponse(
		(CIMDeleteQualifierResponseMessage*)message);
	    break;

	case CIM_ENUMERATE_QUALIFIERS_RESPONSE_MESSAGE:
	    encodeEnumerateQualifiersResponse(
		(CIMEnumerateQualifiersResponseMessage*)message);
	    break;

	case CIM_INVOKE_METHOD_RESPONSE_MESSAGE:
	    encodeInvokeMethodResponse(
		(CIMInvokeMethodResponseMessage*)message); 
	    break;
    }

    delete message;
}

const char* CIMOperationResponseEncoder::getQueueName() const
{
    return "CIMOperationResponseEncoder";
}

void CIMOperationResponseEncoder::encodeCreateClassResponse(
    CIMCreateClassResponseMessage* response)
{
    if (response->errorCode != CIM_ERR_SUCCESS)
    {
	sendError(response, "CreateClass");
	return;
    }

    Array<Sint8> body;

    Array<Sint8> message = XmlWriter::formatSimpleRspMessage(
	"CreateClass", response->messageId, body);

    sendResponse(response->queueIds.top(), message);
}

void CIMOperationResponseEncoder::encodeGetClassResponse(
    CIMGetClassResponseMessage* response)
{
    if (response->errorCode != CIM_ERR_SUCCESS)
    {
	sendError(response, "GetClass");
	return;
    }

    Array<Sint8> body;
    response->cimClass.toXml(body);

    Array<Sint8> message = XmlWriter::formatSimpleRspMessage(
	"GetClass", response->messageId, body);

    sendResponse(response->queueIds.top(), message);
}

void CIMOperationResponseEncoder::encodeModifyClassResponse(
    CIMModifyClassResponseMessage* response)
{
    if (response->errorCode != CIM_ERR_SUCCESS)
    {
	sendError(response, "ModifyClass");
	return;
    }

    Array<Sint8> body;

    Array<Sint8> message = XmlWriter::formatSimpleRspMessage(
	"ModifyClass", response->messageId, body);

    sendResponse(response->queueIds.top(), message);
}

void CIMOperationResponseEncoder::encodeEnumerateClassNamesResponse(
    CIMEnumerateClassNamesResponseMessage* response)
{
    if (response->errorCode != CIM_ERR_SUCCESS)
    {
	sendError(response, "EnumerateClassNames");
	return;
    }

    Array<Sint8> body;

    for (Uint32 i = 0; i < response->classNames.size(); i++)
	XmlWriter::appendClassNameElement(body, response->classNames[i]);

    Array<Sint8> message = XmlWriter::formatSimpleRspMessage(
	"EnumerateClassNames", response->messageId, body);

    sendResponse(response->queueIds.top(), message);
}

void CIMOperationResponseEncoder::encodeEnumerateClassesResponse(
    CIMEnumerateClassesResponseMessage* response)
{
    if (response->errorCode != CIM_ERR_SUCCESS)
    {
	sendError(response, "EnumerateClasses");
	return;
    }

    Array<Sint8> body;

    for (Uint32 i = 0; i < response->cimClasses.size(); i++)
	response->cimClasses[i].toXml(body);

    Array<Sint8> message = XmlWriter::formatSimpleRspMessage(
	"EnumerateClasses", response->messageId, body);

    sendResponse(response->queueIds.top(), message);
}

void CIMOperationResponseEncoder::encodeDeleteClassResponse(
    CIMDeleteClassResponseMessage* response)
{
    if (response->errorCode != CIM_ERR_SUCCESS)
    {
	sendError(response, "DeleteClass");
	return;
    }

    Array<Sint8> body;

    Array<Sint8> message = XmlWriter::formatSimpleRspMessage(
	"DeleteClass", response->messageId, body);

    sendResponse(response->queueIds.top(), message);
}

void CIMOperationResponseEncoder::encodeCreateInstanceResponse(
    CIMCreateInstanceResponseMessage* response)
{
    if (response->errorCode != CIM_ERR_SUCCESS)
    {
	sendError(response, "CreateInstance");
	return;
    }

    Array<Sint8> body;

    XmlWriter::appendInstanceNameElement(body, response->instanceName);

    Array<Sint8> message = XmlWriter::formatSimpleRspMessage(
	"CreateInstance", response->messageId, body);

    sendResponse(response->queueIds.top(), message);
}

void CIMOperationResponseEncoder::encodeGetInstanceResponse(
    CIMGetInstanceResponseMessage* response)
{
    if (response->errorCode != CIM_ERR_SUCCESS)
    {
	sendError(response, "GetInstance");
	return;
    }

    Array<Sint8> body;
    response->cimInstance.toXml(body);

    Array<Sint8> message = XmlWriter::formatSimpleRspMessage(
	"GetInstance", response->messageId, body);

    sendResponse(response->queueIds.top(), message);
}

void CIMOperationResponseEncoder::encodeModifyInstanceResponse(
    CIMModifyInstanceResponseMessage* response)
{
    if (response->errorCode != CIM_ERR_SUCCESS)
    {
	sendError(response, "ModifyInstance");
	return;
    }

    Array<Sint8> body;

    Array<Sint8> message = XmlWriter::formatSimpleRspMessage(
	"ModifyInstance", response->messageId, body);

    sendResponse(response->queueIds.top(), message);
}

void CIMOperationResponseEncoder::encodeEnumerateInstancesResponse(
    CIMEnumerateInstancesResponseMessage* response)
{
    if (response->errorCode != CIM_ERR_SUCCESS)
    {
	sendError(response, "EnumerateInstances");
	return;
    }

    Array<Sint8> body;

    for (Uint32 i = 0; i < response->cimNamedInstances.size(); i++)
	response->cimNamedInstances[i].toXml(body);

    Array<Sint8> message = XmlWriter::formatSimpleRspMessage(
	"EnumerateInstances", response->messageId, body);

    sendResponse(response->queueIds.top(), message);
}

void CIMOperationResponseEncoder::encodeEnumerateInstanceNamesResponse(
    CIMEnumerateInstanceNamesResponseMessage* response)
{
    if (response->errorCode != CIM_ERR_SUCCESS)
    {
	sendError(response, "EnumerateInstanceNames");
	return;
    }

    Array<Sint8> body;

    for (Uint32 i = 0; i < response->instanceNames.size(); i++)
	XmlWriter::appendInstanceNameElement(body, response->instanceNames[i]);

    Array<Sint8> message = XmlWriter::formatSimpleRspMessage(
	"EnumerateInstanceNames", response->messageId, body);

    sendResponse(response->queueIds.top(), message);
}

void CIMOperationResponseEncoder::encodeDeleteInstanceResponse(
    CIMDeleteInstanceResponseMessage* response)
{
    if (response->errorCode != CIM_ERR_SUCCESS)
    {
	sendError(response, "DeleteInstance");
	return;
    }

    Array<Sint8> body;

    Array<Sint8> message = XmlWriter::formatSimpleRspMessage(
	"DeleteInstance", response->messageId, body);

    sendResponse(response->queueIds.top(), message);
}

void CIMOperationResponseEncoder::encodeGetPropertyResponse(
    CIMGetPropertyResponseMessage* response)
{
    if (response->errorCode != CIM_ERR_SUCCESS)
    {
	sendError(response, "GetProperty");
	return;
    }

    Array<Sint8> body;
    response->value.toXml(body);

    Array<Sint8> message = XmlWriter::formatSimpleRspMessage(
	"GetProperty", response->messageId, body);

    sendResponse(response->queueIds.top(), message);
}

void CIMOperationResponseEncoder::encodeSetPropertyResponse(
    CIMSetPropertyResponseMessage* response)
{
    if (response->errorCode != CIM_ERR_SUCCESS)
    {
	sendError(response, "SetProperty");
	return;
    }

    Array<Sint8> body;

    Array<Sint8> message = XmlWriter::formatSimpleRspMessage(
	"SetProperty", response->messageId, body);

    sendResponse(response->queueIds.top(), message);
}

void CIMOperationResponseEncoder::encodeSetQualifierResponse(
    CIMSetQualifierResponseMessage* response)
{
    if (response->errorCode != CIM_ERR_SUCCESS)
    {
	sendError(response, "SetQualifier");
	return;
    }

    Array<Sint8> body;

    Array<Sint8> message = XmlWriter::formatSimpleRspMessage(
	"SetQualifier", response->messageId, body);

    sendResponse(response->queueIds.top(), message);
}

void CIMOperationResponseEncoder::encodeGetQualifierResponse(
    CIMGetQualifierResponseMessage* response)
{
    if (response->errorCode != CIM_ERR_SUCCESS)
    {
	sendError(response, "GetQualifier");
	return;
    }

    Array<Sint8> body;
    response->cimQualifierDecl.toXml(body);

    Array<Sint8> message = XmlWriter::formatSimpleRspMessage(
	"GetQualifier", response->messageId, body);

    sendResponse(response->queueIds.top(), message);
}

void CIMOperationResponseEncoder::encodeEnumerateQualifiersResponse(
    CIMEnumerateQualifiersResponseMessage* response)
{
    if (response->errorCode != CIM_ERR_SUCCESS)
    {
	sendError(response, "EnumerateQualifiers");
	return;
    }

    Array<Sint8> body;

    for (Uint32 i = 0; i < response->qualifierDeclarations.size(); i++)
	response->qualifierDeclarations[i].toXml(body);

    Array<Sint8> message = XmlWriter::formatSimpleRspMessage(
	"EnumerateQualifiers", response->messageId, body);

    sendResponse(response->queueIds.top(), message);
}

void CIMOperationResponseEncoder::encodeDeleteQualifierResponse(
    CIMDeleteQualifierResponseMessage* response)
{
    if (response->errorCode != CIM_ERR_SUCCESS)
    {
	sendError(response, "DeleteQualifier");
	return;
    }

    Array<Sint8> body;

    Array<Sint8> message = XmlWriter::formatSimpleRspMessage(
	"DeleteQualifier", response->messageId, body);

    sendResponse(response->queueIds.top(), message);
}

void CIMOperationResponseEncoder::encodeReferenceNamesResponse(
    CIMReferenceNamesResponseMessage* response)
{
    if (response->errorCode != CIM_ERR_SUCCESS)
    {
	sendError(response, "ReferenceNames");
	return;
    }

    Array<Sint8> body;

    for (Uint32 i = 0; i < response->objectNames.size(); i++)
    {
	body << "<OBJECTPATH>\n";
	response->objectNames[i].toXml(body, false);
	body << "</OBJECTPATH>\n";
    }

    Array<Sint8> message = XmlWriter::formatSimpleRspMessage(
	"ReferenceNames", response->messageId, body);

    sendResponse(response->queueIds.top(), message);
}

void CIMOperationResponseEncoder::encodeReferencesResponse(
    CIMReferencesResponseMessage* response)
{
    if (response->errorCode != CIM_ERR_SUCCESS)
    {
	sendError(response, "References");
	return;
    }

    Array<Sint8> body;

    for (Uint32 i = 0; i < response->cimObjects.size(); i++)
	response->cimObjects[i].toXml(body);

    Array<Sint8> message = XmlWriter::formatSimpleRspMessage(
	"References", response->messageId, body);

    sendResponse(response->queueIds.top(), message);
}

void CIMOperationResponseEncoder::encodeAssociatorNamesResponse(
    CIMAssociatorNamesResponseMessage* response)
{
    if (response->errorCode != CIM_ERR_SUCCESS)
    {
	sendError(response, "AssociatorNames");
	return;
    }

    Array<Sint8> body;

    for (Uint32 i = 0; i < response->objectNames.size(); i++)
    {
	body << "<OBJECTPATH>\n";
	response->objectNames[i].toXml(body, false);
	body << "</OBJECTPATH>\n";
    }

    Array<Sint8> message = XmlWriter::formatSimpleRspMessage(
	"AssociatorNames", response->messageId, body);

    sendResponse(response->queueIds.top(), message);
}

void CIMOperationResponseEncoder::encodeAssociatorsResponse(
    CIMAssociatorsResponseMessage* response)
{
    if (response->errorCode != CIM_ERR_SUCCESS)
    {
	sendError(response, "Associators");
	return;
    }

    Array<Sint8> body;

    for (Uint32 i = 0; i < response->cimObjects.size(); i++)
	response->cimObjects[i].toXml(body);

    Array<Sint8> message = XmlWriter::formatSimpleRspMessage(
	"Associators", response->messageId, body);

    sendResponse(response->queueIds.top(), message);
}

void CIMOperationResponseEncoder::encodeInvokeMethodResponse(
    CIMInvokeMethodResponseMessage* response)
{
    if (response->errorCode != CIM_ERR_SUCCESS)
    {
        sendMethodError(response, response->methodName);
	return;
    }

    Array<Sint8> body;
    response->retValue.toXml(body);

    body = XmlWriter::formatReturnValueElement(body);

    for (Uint8 i = 0; i < response->outParameters.size(); i++)
    {
        // ATTN: Need to support non-String parameter values
	XmlWriter::appendStringParameter(
	    body, 
	    response->outParameters[i].getParameter().getName().allocateCString(),
	    response->outParameters[i].getValue().toString());
    }

    Array<Sint8> message = XmlWriter::formatSimpleMethodRspMessage(
	response->methodName.allocateCString(), response->messageId, body);

    sendResponse(response->queueIds.top(), message);
}

void CIMOperationResponseEncoder::sendMethodError(
    Uint32 queueId, 
    const String& messageId,
    const String& cimMethodName,
    CIMStatusCode code,
    const String& description) 
{
    ArrayDestroyer<char> tmp1(cimMethodName.allocateCString());
    ArrayDestroyer<char> tmp2(description.allocateCString());

    Array<Sint8> message = XmlWriter::formatMethodResponseHeader(
	XmlWriter::formatMessageElement(
	    messageId,
	    XmlWriter::formatSimpleRspElement(
		XmlWriter::formatMethodResponseElement(
		    tmp1.getPointer(),
		    XmlWriter::formatErrorElement(code, tmp2.getPointer())))));
    
    sendResponse(queueId, message);
}

void CIMOperationResponseEncoder::sendMethodError(
    CIMResponseMessage* response,
    const String& cimMethodName)
{
    Uint32 queueId = response->queueIds.top();
    response->queueIds.pop();

    sendMethodError(
	queueId,
	response->messageId, 
	cimMethodName, 
	response->errorCode, 
	response->errorDescription);
}

PEGASUS_NAMESPACE_END
