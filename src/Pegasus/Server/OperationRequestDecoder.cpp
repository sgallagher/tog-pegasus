//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 The Open group, BMC Software, Tivoli Systems, IBM
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
//
//%/////////////////////////////////////////////////////////////////////////////

#include <cctype>
#include <cstdio>
#include <Pegasus/Common/XmlParser.h>
#include <Pegasus/Common/XmlReader.h>
#include <Pegasus/Common/Destroyer.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/Logger.h>
#include "OperationRequestDecoder.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

OperationRequestDecoder::OperationRequestDecoder(
    MessageQueue* outputQueue) : _outputQueue(outputQueue)
{

}

OperationRequestDecoder::~OperationRequestDecoder()
{

}

void OperationRequestDecoder::sendResponse(
    Uint32 returnQueueId, 
    Array<Sint8>& message)
{
    MessageQueue* queue = MessageQueue::lookup(returnQueueId);

    if (queue)
    {
	HTTPMessage* httpMessage = new HTTPMessage(message);
	queue->enqueue(httpMessage);
    }
}

void OperationRequestDecoder::sendError(
    Uint32 returnQueueId, 
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
    
    sendResponse(returnQueueId, message);
}

void OperationRequestDecoder::handleEnqueue()
{
    Message* message = dequeue();

    if (!message)
	return;

    if (getenv("PEGASUS_TRACE"))
	message->print(cout);

    switch (message->getType())
    {
	case HTTP_MESSAGE:
	    handleHTTPMessage((HTTPMessage*)message);
	    break;

	case CIM_GET_CLASS_RESPONSE_MESSAGE:
	    handleGetClassResponseMessage(
		(CIMGetClassResponseMessage*)message);
	    break;

	case CIM_GET_INSTANCE_RESPONSE_MESSAGE:
	    handleGetInstanceResponseMessage(
		(CIMGetInstanceResponseMessage*)message);
	    break;

	case CIM_DELETE_CLASS_RESPONSE_MESSAGE:
	    handleDeleteClassResponseMessage(
		(CIMDeleteClassResponseMessage*)message);
	    break;

	case CIM_DELETE_INSTANCE_RESPONSE_MESSAGE:
	    handleDeleteInstanceResponseMessage(
		(CIMDeleteInstanceResponseMessage*)message);
	    break;

	case CIM_CREATE_CLASS_RESPONSE_MESSAGE:
	    handleCreateClassResponseMessage(
		(CIMCreateClassResponseMessage*)message);
	    break;

	case CIM_CREATE_INSTANCE_RESPONSE_MESSAGE:
	    handleCreateInstanceResponseMessage(
		(CIMCreateInstanceResponseMessage*)message);
	    break;

	case CIM_MODIFY_CLASS_RESPONSE_MESSAGE:
	    handleModifyClassResponseMessage(
		(CIMModifyClassResponseMessage*)message);
	    break;

	case CIM_MODIFY_INSTANCE_RESPONSE_MESSAGE:
	    handleModifyInstanceResponseMessage(
		(CIMModifyInstanceResponseMessage*)message);
	    break;

	case CIM_ENUMERATE_CLASSES_RESPONSE_MESSAGE:
	    handleEnumerateClassesResponseMessage(
		(CIMEnumerateClassesResponseMessage*)message);
	    break;

	case CIM_ENUMERATE_CLASS_NAMES_RESPONSE_MESSAGE:
	    handleEnumerateClassNamesResponseMessage(
		(CIMEnumerateClassNamesResponseMessage*)message);
	    break;

	case CIM_ENUMERATE_INSTANCES_RESPONSE_MESSAGE:
	    handleEnumerateInstancesResponseMessage(
		(CIMEnumerateInstancesResponseMessage*)message);
	    break;

	case CIM_ENUMERATE_INSTANCE_NAMES_RESPONSE_MESSAGE:
	    handleEnumerateInstanceNamesResponseMessage(
		(CIMEnumerateInstanceNamesResponseMessage*)message);
	    break;

	// ATTN: implement this!
	case CIM_EXEC_QUERY_RESPONSE_MESSAGE:
	    break;

	case CIM_ASSOCIATORS_RESPONSE_MESSAGE:
	    handleAssociatorsResponseMessage(
		(CIMAssociatorsResponseMessage*)message);
	    break;

	case CIM_ASSOCIATOR_NAMES_RESPONSE_MESSAGE:
	    handleAssociatorNamesResponseMessage(
		(CIMAssociatorNamesResponseMessage*)message);
	    break;

	case CIM_REFERENCES_RESPONSE_MESSAGE:
	    handleReferencesResponseMessage(
		(CIMReferencesResponseMessage*)message);
	    break;

	case CIM_REFERENCE_NAMES_RESPONSE_MESSAGE:
	    handleReferenceNamesResponseMessage(
		(CIMReferenceNamesResponseMessage*)message);
	    break;

	// ATTN: implement this!
	case CIM_GET_PROPERTY_RESPONSE_MESSAGE:
	    break;

	// ATTN: implement this!
	case CIM_SET_PROPERTY_RESPONSE_MESSAGE:
	    break;

	case CIM_GET_QUALIFIER_RESPONSE_MESSAGE:
	    handleGetQualifierResponseMessage(
		(CIMGetQualifierResponseMessage*)message);
	    break;

	case CIM_SET_QUALIFIER_RESPONSE_MESSAGE:
	    handleSetQualifierResponseMessage(
		(CIMSetQualifierResponseMessage*)message);
	    break;

	case CIM_DELETE_QUALIFIER_RESPONSE_MESSAGE:
	    handleDeleteQualifierResponseMessage(
		(CIMDeleteQualifierResponseMessage*)message);
	    break;

	case CIM_ENUMERATE_QUALIFIERS_RESPONSE_MESSAGE:
	    handleEnumerateQualifiersResponseMessage(
		(CIMEnumerateQualifiersResponseMessage*)message);
	    break;

	// ATTN: implement this:
	case CIM_INVOKE_METHOD_RESPONSE_MESSAGE:
	    break;
    }

    delete message;
}

//------------------------------------------------------------------------------
//
// From the HTTP/1.1 Specification (RFC 2626):
//
// Both types of message consist of a start-line, zero or more header fields 
// (also known as "headers"), an empty line (i.e., a line with nothing 
// preceding the CRLF) indicating the end of the header fields, and possibly 
// a message-body.
//
// Example CIM request:
//
//     M-POST /cimom HTTP/1.1 
//     HOST: www.erewhon.com 
//     Content-Type: application/xml; charset="utf-8" 
//     Content-Length: xxxx 
//     Man: http://www.dmtf.org/cim/operation ; ns=73 
//     73-CIMOperation: MethodCall 
//     73-CIMMethod: EnumerateInstances 
//     73-CIMObject: root/cimv2 
// 
//------------------------------------------------------------------------------

void OperationRequestDecoder::handleHTTPMessage(HTTPMessage* httpMessage)
{
    // Save returnQueueId:

    Uint32 returnQueueId = httpMessage->returnQueueId;

    // Parse the HTTP message:

    String startLine;
    Array<HTTPHeader> headers;
    Sint8* content;
    Uint32 contentLength;

    httpMessage->parse(startLine, headers, content, contentLength);

    // Parse the request line:

    String methodName;
    String requestUri;
    String httpVersion;

    HTTPMessage::parseRequestLine(
        startLine, methodName, requestUri, httpVersion);

    // Process M_POST and POST messages:

    if (methodName == "M_POST" || methodName == "POST")
    {
	// Search for "CIMOperation" header:

	String cimOperation;

	if (!HTTPMessage::lookupHeader(
	    headers, "*CIMOperation", cimOperation, true))
	{
	    // ATTN: no error can be sent back to the client yet since
	    // errors require a message-id (which comes later).
	    return;
	}

	// Zero-terminate the message:

	httpMessage->message.append('\0');

	// If it is a method call, then dispatch it to be handled:

	if (String::equalNoCase(cimOperation, "MethodCall"))
	    handleMethodCall(returnQueueId, content);
    }
}


void OperationRequestDecoder::handleMethodCall(
    Uint32 returnQueueId,
    Sint8* content)
{
    // Create a parser:

    XmlParser parser(content);
    XmlEntry entry;
    String messageId;
    const char* cimMethodName = "";

    try
    {
	// Expect <?xml ...>

	XmlReader::expectXmlDeclaration(parser, entry);

	// Expect <CIM ...>

	XmlReader::testCimStartTag(parser);

	// Expect <MESSAGE ...>

	const char* protocolVersion = 0;

	if (!XmlReader::getMessageStartTag(
	    parser, messageId, protocolVersion))
	{
	    throw XmlValidationError(
		parser.getLine(), "expected MESSAGE element");
	}

	if (strcmp(protocolVersion, "1.0") != 0)
	{
	    throw XmlSemanticError(parser.getLine(), 
		"Expected MESSAGE.PROTOCOLVERSION to be \"1.0\"");
	}
    }
    catch (Exception&)
    {
	// ATTN: no error can be sent back to the client yet since
	// errors require a message-id (which was in the process of
	// being obtained above).
	return;
    }

    try
    {
	// Expect <SIMPLEREQ ...>

	XmlReader::expectStartTag(parser, entry, "SIMPLEREQ");

	// Expect <IMETHODCALL ...>

	if (!XmlReader::getIMethodCallStartTag(parser, cimMethodName))
	{
	    throw XmlValidationError(parser.getLine(), 
		"expected IMETHODCALL element");
	}

	// Expect <LOCALNAMESPACEPATH ...>

	String nameSpace;

	if (!XmlReader::getLocalNameSpacePathElement(parser, nameSpace))
	{
	    throw XmlValidationError(parser.getLine(), 
		"expected LOCALNAMESPACEPATH element");
	}

	// Delegate to appropriate method to handle:

	if (CompareNoCase(cimMethodName, "GetClass") == 0)
	    handleGetClassRequest(parser, messageId, nameSpace);
	else if (CompareNoCase(cimMethodName, "GetInstance") == 0)
	    handleGetInstanceRequest(parser, messageId, nameSpace);
	else if (CompareNoCase(cimMethodName, "EnumerateClassNames") == 0)
	    handleEnumerateClassNamesRequest(parser, messageId, nameSpace);
	else if (CompareNoCase(cimMethodName, "References") == 0)
	    handleReferencesRequest(parser, messageId, nameSpace);
	else if (CompareNoCase(cimMethodName, "ReferenceNames") == 0)
	    handleReferenceNamesRequest(parser, messageId, nameSpace);
	else if (CompareNoCase(cimMethodName, "AssociatorNames") == 0)
	    handleAssociatorNamesRequest(parser, messageId, nameSpace);
	else if (CompareNoCase(cimMethodName, "Associators") == 0)
	    handleAssociatorsRequest(parser, messageId, nameSpace);
	else if (CompareNoCase(cimMethodName, "CreateInstance") == 0)
	    handleCreateInstanceRequest(parser, messageId, nameSpace);
	else if (CompareNoCase(cimMethodName, "EnumerateInstanceNames") == 0)
	    handleEnumerateInstanceNamesRequest(parser, messageId, nameSpace);
	else if (CompareNoCase(cimMethodName, "DeleteQualifier") == 0)
	    handleDeleteQualifierRequest(parser, messageId, nameSpace);
	else if (CompareNoCase(cimMethodName, "GetQualifier") == 0)
	    handleGetQualifierRequest(parser, messageId, nameSpace);
	else if (CompareNoCase(cimMethodName, "SetQualifier") == 0)
	    handleSetQualifierRequest(parser, messageId, nameSpace);
	else if (CompareNoCase(cimMethodName, "EnumerateQualifiers") == 0)
	    handleEnumerateQualifiersRequest(parser, messageId, nameSpace);
	else if (CompareNoCase(cimMethodName, "EnumerateClasses") == 0)
	    handleEnumerateClassesRequest(parser, messageId, nameSpace);
	else if (CompareNoCase(cimMethodName, "EnumerateClassNames") == 0)
	    handleEnumerateClassNamesRequest(parser, messageId, nameSpace);
	else if (CompareNoCase(cimMethodName, "EnumerateInstances") == 0)
	    handleEnumerateInstancesRequest(parser, messageId, nameSpace);
	else if (CompareNoCase(cimMethodName, "CreateClass") == 0)
	    handleCreateClassRequest(parser, messageId, nameSpace);
	else if (CompareNoCase(cimMethodName, "ModifyClass") == 0)
	    handleModifyClassRequest(parser, messageId, nameSpace);
	else if (CompareNoCase(cimMethodName, "ModifyInstance") == 0)
	    handleModifyInstanceRequest(parser, messageId, nameSpace);
	else if (CompareNoCase(cimMethodName, "DeleteClass") == 0)
	    handleDeleteClassRequest(parser, messageId, nameSpace);
	else if (CompareNoCase(cimMethodName, "DeleteInstance") == 0)
	    handleDeleteInstanceRequest(parser, messageId, nameSpace);
	else if (CompareNoCase(cimMethodName, "GetProperty") == 0)
	    handleGetPropertyRequest(parser, messageId, nameSpace);
	else if (CompareNoCase(cimMethodName, "SetProperty") == 0)
	    handleSetPropertyRequest(parser, messageId, nameSpace);
	else
	{
	    String description = "Unknown intrinsic method: ";
	    description += cimMethodName;

	    sendError(
		returnQueueId, 
		messageId,
		cimMethodName,
		CIM_ERR_FAILED,
		description);

	    return;
	}

	// Expect </IMETHODCALL>

	XmlReader::expectEndTag(parser, "IMETHODCALL");

	// Expect </SIMPLEREQ>

	XmlReader::expectEndTag(parser, "SIMPLEREQ");

	// Expect </MESSAGE>

	XmlReader::expectEndTag(parser, "MESSAGE");

	// Expect </CIM>

	XmlReader::expectEndTag(parser, "CIM");
    }
    catch (Exception& e)
    {
	sendError(
	    returnQueueId, 
	    messageId,
	    cimMethodName,
	    CIM_ERR_FAILED,
	    e.getMessage());
    }
}

#if 0

int OperationRequestDecoder::handleMethodCall()
{
    //--------------------------------------------------------------------------
    // Dispatch the method:
    //--------------------------------------------------------------------------

    if (CompareNoCase(cimMethodName, "GetClass") == 0)
	handleGetClassRequest(parser, messageId, nameSpace);
    else if (CompareNoCase(cimMethodName, "GetInstance") == 0)
	handleGetInstanceRequest(parser, messageId, nameSpace);
    else if (CompareNoCase(cimMethodName, "EnumerateClassNames") == 0)
	handleEnumerateClassNamesRequest(parser, messageId, nameSpace);
    else if (CompareNoCase(cimMethodName, "References") == 0)
	handleReferencesRequest(parser, messageId, nameSpace);
    else if (CompareNoCase(cimMethodName, "ReferenceNames") == 0)
	handleReferenceNamesRequest(parser, messageId, nameSpace);
    else if (CompareNoCase(cimMethodName, "AssociatorNames") == 0)
	handleAssociatorNamesRequest(parser, messageId, nameSpace);
    else if (CompareNoCase(cimMethodName, "Associators") == 0)
	handleAssociatorsRequest(parser, messageId, nameSpace);
    else if (CompareNoCase(cimMethodName, "CreateInstance") == 0)
	handleCreateInstanceRequest(parser, messageId, nameSpace);
    else if (CompareNoCase(cimMethodName, "EnumerateInstanceNames") == 0)
	handleEnumerateInstanceNamesRequest(parser, messageId, nameSpace);
    else if (CompareNoCase(cimMethodName, "DeleteQualifier") == 0)
	handleDeleteQualifierRequest(parser, messageId, nameSpace);
    else if (CompareNoCase(cimMethodName, "GetQualifier") == 0)
	handleGetQualifierRequest(parser, messageId, nameSpace);
    else if (CompareNoCase(cimMethodName, "SetQualifier") == 0)
	handleSetQualifierRequest(parser, messageId, nameSpace);
    else if (CompareNoCase(cimMethodName, "EnumerateQualifiers") == 0)
	handleEnumerateQualifiersRequest(parser, messageId, nameSpace);
    else if (CompareNoCase(cimMethodName, "EnumerateClasses") == 0)
	handleEnumerateClassesRequest(parser, messageId, nameSpace);
    else if (CompareNoCase(cimMethodName, "EnumerateClassNames") == 0)
	handleEnumerateClassNamesRequest(parser, messageId, nameSpace);
    else if (CompareNoCase(cimMethodName, "EnumerateInstances") == 0)
	handleEnumerateInstancesRequest(parser, messageId, nameSpace);
    else if (CompareNoCase(cimMethodName, "CreateClass") == 0)
	handleCreateClassRequest(parser, messageId, nameSpace);
    else if (CompareNoCase(cimMethodName, "ModifyClass") == 0)
	handleModifyClassRequest(parser, messageId, nameSpace);
    else if (CompareNoCase(cimMethodName, "ModifyInstance") == 0)
	handleModifyInstanceRequest(parser, messageId, nameSpace);
    else if (CompareNoCase(cimMethodName, "DeleteClass") == 0)
	handleDeleteClassRequest(parser, messageId, nameSpace);
    else if (CompareNoCase(cimMethodName, "DeleteInstance") == 0)
	handleDeleteInstanceRequest(parser, messageId, nameSpace);
    else if (CompareNoCase(cimMethodName, "GetProperty") == 0)
	handleGetPropertyRequest(parser, messageId, nameSpace);
    else if (CompareNoCase(cimMethodName, "SetProperty") == 0)
	handleSetPropertyRequest(parser, messageId, nameSpace);
    else
    {
	String msg = CIMStatusCodeToString(CIM_ERR_FAILED);
	msg += ": unknown intrinsic method: ";
	msg += cimMethodName;
	char* tmp = msg.allocateCString();
	sendError(messageId, cimMethodName, CIM_ERR_FAILED, tmp);
	delete [] tmp;

	cerr << msg << endl;
	return 0;
    }

    //--------------------------------------------------------------------------
    // Handle end tags:
    //--------------------------------------------------------------------------

    XmlReader::expectEndTag(parser, "IMETHODCALL");
    XmlReader::expectEndTag(parser, "SIMPLEREQ");
    XmlReader::expectEndTag(parser, "MESSAGE");
    XmlReader::expectEndTag(parser, "CIM");

    return 0;
}

void OperationRequestDecoder::sendError(
    const String& messageId,
    const char* methodName,
    CIMStatusCode code,
    const char* description) 
{
    Array<Sint8> message = XmlWriter::formatMethodResponseHeader(
	XmlWriter::formatMessageElement(
	    messageId,
	    XmlWriter::formatSimpleRspElement(
		XmlWriter::formatIMethodResponseElement(
		    methodName,
		    XmlWriter::formatErrorElement(code, description)))));
    
    sendResponse(message);
}

void OperationRequestDecoder::sendError(
    CIMResponseMessage* m,
    const char* methodName)
{
    char* tmp = m->errorDescription.allocateCString();
    sendError(m->messageId, methodName, m->errorCode, tmp);
    delete [] tmp;
}

void OperationRequestDecoder::handleGetPropertyRequest(
    XmlParser& parser, 
    const String& messageId,
    const String& nameSpace)
{
    // ATTN: implement this!
    PEGASUS_ASSERT(0);
}

void OperationRequestDecoder::handleSetPropertyRequest(
    XmlParser& parser, 
    const String& messageId,
    const String& nameSpace)
{
    // ATTN: implement this!
    PEGASUS_ASSERT(0);
}

void OperationRequestDecoder::handleGetClassRequest(
    XmlParser& parser, 
    const String& messageId,
    const String& nameSpace)
{
    // ATTN: handle property lists!

    String className;
    Boolean localOnly = true;
    Boolean includeQualifiers = true;
    Boolean includeClassOrigin = false;

    for (const char* name; XmlReader::getIParamValueTag(parser, name);)
    {
	if (CompareNoCase(name, "ClassName") == 0)
	    XmlReader::getClassNameElement(parser, className, true);
	else if (CompareNoCase(name, "LocalOnly") == 0)
	    XmlReader::getBooleanValueElement(parser, localOnly, true);
	else if (CompareNoCase(name, "IncludeQualifiers") == 0)
	    XmlReader::getBooleanValueElement(parser, includeQualifiers, true);
	else if (CompareNoCase(name, "IncludeClassOrigin") == 0)
	    XmlReader::getBooleanValueElement(parser, includeClassOrigin, true);

	XmlReader::expectEndTag(parser, "IPARAMVALUE");
    }

    Message* request = new CIMGetClassRequestMessage(
	messageId,
	nameSpace,
	className,
	localOnly,
	includeQualifiers,
	includeClassOrigin,
	Array<String>(),
	getQueueId());

    _outputQueue->enqueue(request);
}

void OperationRequestDecoder::handleGetClassResponseMessage(
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

    delete response;

    sendResponse(message);
}

void OperationRequestDecoder::handleGetInstanceRequest(
    XmlParser& parser, 
    const String& messageId,
    const String& nameSpace)
{
    // ATTN: handle property lists!

    CIMReference instanceName;
    Boolean localOnly = true;
    Boolean includeQualifiers = false;
    Boolean includeClassOrigin = false;

    for (const char* name; XmlReader::getIParamValueTag(parser, name);)
    {
	if (CompareNoCase(name, "InstanceName") == 0)
	    XmlReader::getInstanceNameElement(parser, instanceName);
	else if (CompareNoCase(name, "LocalOnly") == 0)
	    XmlReader::getBooleanValueElement(parser, localOnly, true);
	else if (CompareNoCase(name, "IncludeQualifiers") == 0)
	    XmlReader::getBooleanValueElement(parser, includeQualifiers, true);
	else if (CompareNoCase(name, "IncludeClassOrigin") == 0)
	    XmlReader::getBooleanValueElement(parser, includeClassOrigin, true);

	XmlReader::expectEndTag(parser, "IPARAMVALUE");
    }

    Message* request = new CIMGetInstanceRequestMessage(
	messageId,
	nameSpace,
	instanceName,
	localOnly,
	includeQualifiers,
	includeClassOrigin,
	Array<String>(),
	getQueueId());

    _outputQueue->enqueue(request);
}

void OperationRequestDecoder::handleGetInstanceResponseMessage(
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

    sendResponse(message);
}

void OperationRequestDecoder::handleDeleteClassRequest(
    XmlParser& parser, 
    const String& messageId,
    const String& nameSpace)
{
    String className;

    for (const char* name; XmlReader::getIParamValueTag(parser, name);)
    {
	if (CompareNoCase(name, "ClassName") == 0)
	    XmlReader::getClassNameElement(parser, className);

	XmlReader::expectEndTag(parser, "IPARAMVALUE");
    }

    Message* request = new CIMDeleteClassRequestMessage(
	messageId,
	nameSpace,
	className,
	getQueueId());

    _outputQueue->enqueue(request);
}

void OperationRequestDecoder::handleDeleteClassResponseMessage(
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

    sendResponse(message);
}


void OperationRequestDecoder::handleDeleteInstanceRequest(
    XmlParser& parser, 
    const String& messageId,
    const String& nameSpace)
{
    CIMReference instanceName;

    for (const char* name; XmlReader::getIParamValueTag(parser, name);)
    {
	if (CompareNoCase(name, "InstanceName") == 0)
	    XmlReader::getInstanceNameElement(parser, instanceName);

	XmlReader::expectEndTag(parser, "IPARAMVALUE");
    }

    Message* request = new CIMDeleteInstanceRequestMessage(
	messageId,
	nameSpace,
	instanceName,
	getQueueId());

    _outputQueue->enqueue(request);
}

void OperationRequestDecoder::handleDeleteInstanceResponseMessage(
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

    sendResponse(message);
}

void OperationRequestDecoder::handleCreateClassRequest(
    XmlParser& parser, 
    const String& messageId,
    const String& nameSpace)
{
    CIMClass newClass;

    for (const char* name; XmlReader::getIParamValueTag(parser, name);)
    {
	if (CompareNoCase(name, "NewClass") == 0)
	    XmlReader::getClassElement(parser, newClass);

	XmlReader::expectEndTag(parser, "IPARAMVALUE");
    }

    CIMCreateClassRequestMessage* request = new CIMCreateClassRequestMessage(
	messageId,
	nameSpace,
	newClass,
	getQueueId());

    _outputQueue->enqueue(request);
}

void OperationRequestDecoder::handleCreateClassResponseMessage(
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

    sendResponse(message);
}

void OperationRequestDecoder::handleCreateInstanceRequest(
    XmlParser& parser, 
    const String& messageId,
    const String& nameSpace)
{
    CIMInstance newInstance;

    for (const char* name; XmlReader::getIParamValueTag(parser, name);)
    {
	if (CompareNoCase(name, "NewInstance") == 0)
	    XmlReader::getInstanceElement(parser, newInstance);

	XmlReader::expectEndTag(parser, "IPARAMVALUE");
    }

    CIMCreateInstanceRequestMessage* request = 
	new CIMCreateInstanceRequestMessage(
	    messageId,
	    nameSpace,
	    newInstance,
	    getQueueId());

    _outputQueue->enqueue(request);
}

void OperationRequestDecoder::handleCreateInstanceResponseMessage(
    CIMCreateInstanceResponseMessage* response)
{
    if (response->errorCode != CIM_ERR_SUCCESS)
    {
	sendError(response, "CreateInstance");
	return;
    }

    Array<Sint8> body;

    Array<Sint8> message = XmlWriter::formatSimpleRspMessage(
	"CreateInstance", response->messageId, body);

    sendResponse(message);
}

void OperationRequestDecoder::handleModifyClassRequest(
    XmlParser& parser, 
    const String& messageId,
    const String& nameSpace)
{
    CIMClass modifiedClass;

    for (const char* name; XmlReader::getIParamValueTag(parser, name);)
    {
	if (CompareNoCase(name, "ModifiedClass") == 0)
	    XmlReader::getClassElement(parser, modifiedClass);

	XmlReader::expectEndTag(parser, "IPARAMVALUE");
    }

    CIMModifyClassRequestMessage* request = 
	new CIMModifyClassRequestMessage(
	    messageId,
	    nameSpace,
	    modifiedClass,
	    getQueueId());

    _outputQueue->enqueue(request);
}

void OperationRequestDecoder::handleModifyClassResponseMessage(
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

    sendResponse(message);
}

void OperationRequestDecoder::handleModifyInstanceRequest(
    XmlParser& parser, 
    const String& messageId,
    const String& nameSpace)
{
    CIMInstance modifiedInstance;

    for (const char* name; XmlReader::getIParamValueTag(parser, name);)
    {
	if (CompareNoCase(name, "ModifiedInstance") == 0)
	    XmlReader::getInstanceElement(parser, modifiedInstance);

	XmlReader::expectEndTag(parser, "IPARAMVALUE");
    }

    CIMModifyInstanceRequestMessage* request = 
	new CIMModifyInstanceRequestMessage(
	    messageId,
	    nameSpace,
	    modifiedInstance,
	    getQueueId());

    _outputQueue->enqueue(request);
}

void OperationRequestDecoder::handleModifyInstanceResponseMessage(
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

    sendResponse(message);
}

void OperationRequestDecoder::handleEnumerateClassesRequest(
    XmlParser& parser, 
    const String& messageId,
    const String& nameSpace)
{
    String className;
    Boolean deepInheritance = false;
    Boolean localOnly = true;
    Boolean includeQualifiers = true;
    Boolean includeClassOrigin = false;

    for (const char* name; XmlReader::getIParamValueTag(parser, name);)
    {
	if (CompareNoCase(name, "ClassName") == 0)
	    XmlReader::getClassNameElement(parser, className, true);
	else if (CompareNoCase(name, "DeepInheritance") == 0)
	    XmlReader::getBooleanValueElement(parser, deepInheritance, true);
	else if (CompareNoCase(name, "LocalOnly") == 0)
	    XmlReader::getBooleanValueElement(parser, localOnly, true);
	else if (CompareNoCase(name, "IncludeQualifiers") == 0)
	    XmlReader::getBooleanValueElement(parser, includeQualifiers, true);
	else if (CompareNoCase(name, "IncludeClassOrigin") == 0)
	    XmlReader::getBooleanValueElement(parser, includeClassOrigin, true);

	XmlReader::expectEndTag(parser, "IPARAMVALUE");
    }

    CIMEnumerateClassesRequestMessage* request = 
	new CIMEnumerateClassesRequestMessage(
	    messageId,
	    nameSpace,
	    className,
	    deepInheritance,
	    localOnly,
	    includeQualifiers,
	    includeClassOrigin,
	    getQueueId());

    _outputQueue->enqueue(request);
}

void OperationRequestDecoder::handleEnumerateClassesResponseMessage(
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

    sendResponse(message);
}

void OperationRequestDecoder::handleEnumerateClassNamesRequest(
    XmlParser& parser, 
    const String& messageId,
    const String& nameSpace)
{
    String className;
    Boolean deepInheritance = false;

    for (const char* name; XmlReader::getIParamValueTag(parser, name);)
    {
	if (CompareNoCase(name, "ClassName") == 0)
	    XmlReader::getClassNameElement(parser, className, true);
	else if (CompareNoCase(name, "DeepInheritance") == 0)
	    XmlReader::getBooleanValueElement(parser, deepInheritance, true);

	XmlReader::expectEndTag(parser, "IPARAMVALUE");
    }

    CIMEnumerateClassNamesRequestMessage* request = 
	new CIMEnumerateClassNamesRequestMessage(
	    messageId,
	    nameSpace,
	    className,
	    deepInheritance,
	    getQueueId());

    _outputQueue->enqueue(request);
}

void OperationRequestDecoder::handleEnumerateClassNamesResponseMessage(
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

    sendResponse(message);
}

void OperationRequestDecoder::handleEnumerateInstancesRequest(
    XmlParser& parser, 
    const String& messageId,
    const String& nameSpace)
{
    String className;
    Boolean deepInheritance = false;
    Boolean localOnly = true;
    Boolean includeQualifiers = true;
    Boolean includeClassOrigin = false;
    Array<String> propertyList = EmptyStringArray();

    for (const char* name; XmlReader::getIParamValueTag(parser, name);)
    {
	if (CompareNoCase(name, "ClassName") == 0)
	    XmlReader::getClassNameElement(parser, className, true);
	else if (CompareNoCase(name, "DeepInheritance") == 0)
	    XmlReader::getBooleanValueElement(parser, deepInheritance, true);
	else if (CompareNoCase(name, "LocalOnly") == 0)
	    XmlReader::getBooleanValueElement(parser, localOnly, true);
	else if (CompareNoCase(name, "IncludeQualifiers") == 0)
	    XmlReader::getBooleanValueElement(parser, includeQualifiers, true);
	else if (CompareNoCase(name, "IncludeClassOrigin") == 0)
	    XmlReader::getBooleanValueElement(parser, includeClassOrigin, true);
	//ATTN: Property List
	XmlReader::expectEndTag(parser, "IPARAMVALUE");
    }

    CIMEnumerateInstancesRequestMessage* request = 
	new CIMEnumerateInstancesRequestMessage(
	    messageId,
	    nameSpace,
	    className,
	    deepInheritance,
	    localOnly,
	    includeQualifiers,
	    includeClassOrigin,
	    Array<String>(),
	    getQueueId());

    _outputQueue->enqueue(request);
}

void OperationRequestDecoder::handleEnumerateInstancesResponseMessage(
    CIMEnumerateInstancesResponseMessage* response)
{
    if (response->errorCode != CIM_ERR_SUCCESS)
    {
	sendError(response, "EnumerateInstances");
	return;
    }

    Array<Sint8> body;

    for (Uint32 i = 0; i < response->cimInstances.size(); i++)
	response->cimInstances[i].toXml(body);

    Array<Sint8> message = XmlWriter::formatSimpleRspMessage(
	"EnumerateInstances", response->messageId, body);

    sendResponse(message);
}

void OperationRequestDecoder::handleEnumerateInstanceNamesRequest(
    XmlParser& parser, 
    const String& messageId,
    const String& nameSpace)
{
    String className;

    for (const char* name; XmlReader::getIParamValueTag(parser, name);)
    {
	if (CompareNoCase(name, "ClassName") == 0)
	    XmlReader::getClassNameElement(parser, className, true);

	XmlReader::expectEndTag(parser, "IPARAMVALUE");
    }

    CIMEnumerateInstanceNamesRequestMessage* request = 
	new CIMEnumerateInstanceNamesRequestMessage(
	    messageId,
	    nameSpace,
	    className,
	    getQueueId());

    _outputQueue->enqueue(request);
}

void OperationRequestDecoder::handleEnumerateInstanceNamesResponseMessage(
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

    sendResponse(message);
}

void OperationRequestDecoder::handleAssociatorsRequest(
    XmlParser& parser, 
    const String& messageId,
    const String& nameSpace)
{
    // ATTN: handle the property list!

    CIMReference objectName;
    String assocClass;
    String resultClass;
    String role;
    String resultRole;
    Boolean includeQualifiers = false;
    Boolean includeClassOrigin = false;
    Array<String> propertyList;

    for (const char* name; XmlReader::getIParamValueTag(parser, name);)
    {
	if (CompareNoCase(name, "ObjectName") == 0)
	{
	    XmlReader::getObjectNameElement(parser, objectName);
	}
	else if (CompareNoCase(name, "AssocClass") == 0)
	{
	    XmlReader::getClassNameElement(parser, assocClass, true);
	}
	else if (CompareNoCase(name, "ResultClass") == 0)
	{
	    XmlReader::getClassNameElement(parser, resultClass, true);
	}
	else if (CompareNoCase(name, "Role") == 0)
	{
	    XmlReader::getStringValueElement(parser, role, true);
	}
	else if (CompareNoCase(name, "ResultRole") == 0)
	{
	    XmlReader::getStringValueElement(parser, resultRole, true);
	}
	else if (CompareNoCase(name, "IncludeQualifiers") == 0)
	{
	    XmlReader::getBooleanValueElement(parser, includeQualifiers, true);
	}
	else if (CompareNoCase(name, "IncludeClassOrigin") == 0)
	{
	    XmlReader::getBooleanValueElement(parser, includeClassOrigin, true);
	}

	XmlReader::expectEndTag(parser, "IPARAMVALUE");
    }

    CIMAssociatorsRequestMessage* request = 
	new CIMAssociatorsRequestMessage(
	    messageId,
	    nameSpace,
	    objectName,
	    assocClass,
	    resultClass,
	    role,
	    resultRole,
	    includeQualifiers,
	    includeClassOrigin,
	    propertyList,
	    getQueueId());

    _outputQueue->enqueue(request);
}

void OperationRequestDecoder::handleAssociatorsResponseMessage(
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

    sendResponse(message);
}

void OperationRequestDecoder::handleAssociatorNamesRequest(
    XmlParser& parser, 
    const String& messageId,
    const String& nameSpace)
{
    CIMReference objectName;
    String assocClass;
    String resultClass;
    String role;
    String resultRole;

    for (const char* name; XmlReader::getIParamValueTag(parser, name);)
    {
	if (CompareNoCase(name, "ObjectName") == 0)
	    XmlReader::getObjectNameElement(parser, objectName);
	else if (CompareNoCase(name, "AssocClass") == 0)
	    XmlReader::getClassNameElement(parser, assocClass, true);
	else if (CompareNoCase(name, "ResultClass") == 0)
	    XmlReader::getClassNameElement(parser, resultClass, true);
	else if (CompareNoCase(name, "Role") == 0)
	    XmlReader::getStringValueElement(parser, role, true);
	else if (CompareNoCase(name, "ResultRole") == 0)
	    XmlReader::getStringValueElement(parser, resultRole, true);

	XmlReader::expectEndTag(parser, "IPARAMVALUE");
    }

    CIMAssociatorNamesRequestMessage* request = 
	new CIMAssociatorNamesRequestMessage(
	    messageId,
	    nameSpace,
	    objectName,
	    assocClass,
	    resultClass,
	    role,
	    resultRole,
	    getQueueId());

    _outputQueue->enqueue(request);
}

void OperationRequestDecoder::handleAssociatorNamesResponseMessage(
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

    sendResponse(message);
}

void OperationRequestDecoder::handleReferencesRequest(
    XmlParser& parser, 
    const String& messageId,
    const String& nameSpace)
{
    // ATTN: handle the property list!

    CIMReference objectName;
    String resultClass;
    String role;
    Boolean includeQualifiers = false;
    Boolean includeClassOrigin = false;
    Array<String> propertyList;

    for (const char* name; XmlReader::getIParamValueTag(parser, name);)
    {
	if (CompareNoCase(name, "ObjectName") == 0)
	{
	    XmlReader::getObjectNameElement(parser, objectName);
	}
	else if (CompareNoCase(name, "ResultClass") == 0)
	{
	    XmlReader::getClassNameElement(parser, resultClass, true);
	}
	else if (CompareNoCase(name, "Role") == 0)
	{
	    XmlReader::getStringValueElement(parser, role, true);
	}
	else if (CompareNoCase(name, "IncludeQualifiers") == 0)
	{
	    XmlReader::getBooleanValueElement(parser, includeQualifiers, true);
	}
	else if (CompareNoCase(name, "IncludeClassOrigin") == 0)
	{
	    XmlReader::getBooleanValueElement(parser, includeClassOrigin, true);
	}

	XmlReader::expectEndTag(parser, "IPARAMVALUE");
    }

    CIMReferencesRequestMessage* request = 
	new CIMReferencesRequestMessage(
	    messageId,
	    nameSpace,
	    objectName,
	    resultClass,
	    role,
	    includeQualifiers,
	    includeClassOrigin,
	    propertyList,
	    getQueueId());

    _outputQueue->enqueue(request);
}

void OperationRequestDecoder::handleReferencesResponseMessage(
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

    sendResponse(message);
}

void OperationRequestDecoder::handleReferenceNamesRequest(
    XmlParser& parser, 
    const String& messageId,
    const String& nameSpace)
{
    CIMReference objectName;
    String resultClass;
    String role;

    for (const char* name; XmlReader::getIParamValueTag(parser, name);)
    {
	if (CompareNoCase(name, "ObjectName") == 0)
	{
	    XmlReader::getObjectNameElement(parser, objectName);
	}
	else if (CompareNoCase(name, "ResultClass") == 0)
	{
	    XmlReader::getClassNameElement(parser, resultClass, true);
	}
	else if (CompareNoCase(name, "Role") == 0)
	{
	    XmlReader::getStringValueElement(parser, role, true);
	}

	XmlReader::expectEndTag(parser, "IPARAMVALUE");
    }

    CIMReferenceNamesRequestMessage* request = 
	new CIMReferenceNamesRequestMessage(
	    messageId,
	    nameSpace,
	    objectName,
	    resultClass,
	    role,
	    getQueueId());

    _outputQueue->enqueue(request);
}

void OperationRequestDecoder::handleReferenceNamesResponseMessage(
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

    sendResponse(message);
}

void OperationRequestDecoder::handleGetQualifierRequest(
    XmlParser& parser, 
    const String& messageId,
    const String& nameSpace)
{
    String qualifierName;

    for (const char* name; XmlReader::getIParamValueTag(parser, name);)
    {
	if (CompareNoCase(name, "QualifierName") == 0)
	    XmlReader::getClassNameElement(parser, qualifierName, true);

	XmlReader::expectEndTag(parser, "IPARAMVALUE");
    }

    CIMGetQualifierRequestMessage* request = 
	new CIMGetQualifierRequestMessage(
	    messageId,
	    nameSpace,
	    qualifierName,
	    getQueueId());

    _outputQueue->enqueue(request);
}

void OperationRequestDecoder::handleGetQualifierResponseMessage(
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

    sendResponse(message);
}

void OperationRequestDecoder::handleSetQualifierRequest(
    XmlParser& parser, 
    const String& messageId,
    const String& nameSpace)
{
    CIMQualifierDecl qualifierDeclaration;

    for (const char* name; XmlReader::getIParamValueTag(parser, name);)
    {
	if (CompareNoCase(name, "QualifierDeclaration") == 0)
	    XmlReader::getQualifierDeclElement(parser, qualifierDeclaration);

	XmlReader::expectEndTag(parser, "IPARAMVALUE");
    }

    CIMSetQualifierRequestMessage* request = 
	new CIMSetQualifierRequestMessage(
	    messageId,
	    nameSpace,
	    qualifierDeclaration,
	    getQueueId());

    _outputQueue->enqueue(request);
}

void OperationRequestDecoder::handleSetQualifierResponseMessage(
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

    sendResponse(message);
}

void OperationRequestDecoder::handleDeleteQualifierRequest(
    XmlParser& parser, 
    const String& messageId,
    const String& nameSpace)
{
    String qualifierName;

    for (const char* name; XmlReader::getIParamValueTag(parser, name);)
    {
	if (CompareNoCase(name, "QualifierName") == 0)
	    XmlReader::getClassNameElement(parser, qualifierName, true);

	XmlReader::expectEndTag(parser, "IPARAMVALUE");
    }

    CIMDeleteQualifierRequestMessage* request = 
	new CIMDeleteQualifierRequestMessage(
	    messageId,
	    nameSpace,
	    qualifierName,
	    getQueueId());

    _outputQueue->enqueue(request);
}

void OperationRequestDecoder::handleDeleteQualifierResponseMessage(
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

    sendResponse(message);
}

void OperationRequestDecoder::handleEnumerateQualifiersRequest(
    XmlParser& parser, 
    const String& messageId,
    const String& nameSpace)
{
    for (const char* name; XmlReader::getIParamValueTag(parser, name);)
	XmlReader::expectEndTag(parser, "IPARAMVALUE");

    CIMEnumerateQualifiersRequestMessage* request = 
	new CIMEnumerateQualifiersRequestMessage(
	    messageId,
	    nameSpace,
	    getQueueId());

    _outputQueue->enqueue(request);
}

void OperationRequestDecoder::handleEnumerateQualifiersResponseMessage(
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

    sendResponse(message);
}

#endif

PEGASUS_NAMESPACE_END
