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
#include "ClientConnection.h"
#include <Pegasus/Common/XmlParser.h>
#include <Pegasus/Common/XmlReader.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/Logger.h>

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

#define DDD(X) // X

static const char _GET[] = "GET";

static const Uint32 _GET_LEN = sizeof(_GET) - 1;

static const char _M_POST[] = "M-POST";

static const Uint32 _M_POST_LEN = sizeof(_M_POST) - 1;

static const char _POST[] = "POST";

static const Uint32 _POST_LEN = sizeof(_POST) - 1;


ClientConnection::ClientConnection(MessageQueue* outputQueue) 
    : _outputQueue(outputQueue), _channel(0)
{

}

ClientConnection::~ClientConnection()
{

}


Boolean ClientConnection::handleOpen(Channel* channel)
{
    _channel = channel;
    DDD(cout << "Handle Open" << endl;)
    return Handler::handleOpen(channel);
}

/** sendMessage added simply to consolidate all of the
    message output to one function so we could control
    output and trace display better.
    Note that this function outputs but does not format
    the message. 
    ATTN: Today there is no choice of output for trace.
    It goes to the console, is accompanied by long header,
    and you cannot chose input or output.
    @Author: KS
*/
void ClientConnection::sendMessage (Array<Sint8>& message)
{
    if (_messageTrace)
    {
	cout << "\n========== SENT ==========" << endl;
	
	message.append('\0');
	cout << message.getData() << endl;
	message.remove(message.size() - 1);

    }

    if (_messageLogTrace)
    {
	message.append('\0');
	Logger::put(Logger::TRACE_LOG, "Handler",Logger::INFORMATION,
	       "SENT================\n $0", message.getData());
	message.remove(message.size() - 1);

       // Handler:logMessage(Logger::TRACE_LOG, "Handler",Logger::INFORMATION,
       //        "SENT================\n $0", message);
        
    }

    _channel->writeN(message.getData(), message.size());
}

int ClientConnection::handleMessage()
{
    Handler::handleMessage();
    DDD(cout << "HandleMessage " << endl;)
    const char* m = _message.getData();

    if (strncmp(m, _GET, _GET_LEN) == 0 && isspace(m[_GET_LEN]))
    {
	Handler::print();

	return handleGetRequest();
    }
    else if ((strncmp(m, _M_POST, _M_POST_LEN) == 0 && isspace(m[_M_POST_LEN]))
         || (strncmp(m, _POST, _POST_LEN) == 0 && isspace(m[_POST_LEN])))
    {
	const char* cimOperation = getFieldValueSubString("CIMOperation:");

	if (cimOperation && CompareNoCase(cimOperation, "MethodCall") == 0)
	{
	    try
	    {
		return handleMethodCall();
	    }

	    // Common point to catch XML parsing errors that generate
	    // exceptions. Puts out the error message to the console,
	    // to the Logger and also puts the message to the Logger
	    catch (Exception& e)
	    {
		Logger::put(Logger::ERROR_LOG,
			    "XMLParser", 
			    Logger::WARNING, 
			    "Error: $0\n======\n$1\n========\n",
			    e.getMessage(),
			    m);

		// ATTN-KS - At some point remove this console outpt
		PEGASUS_STD(cerr) << 
		    "Error: " << e.getMessage() << PEGASUS_STD(endl);
	    }
	    return 0;
	}
    }

    // cout << "Error: unknown message: " << m << endl;
    // ATTN: Is this a valid point to log?
    return -1;
}

int ClientConnection::handleGetRequest()
{
    const char* m = _message.getData();
    const char* p = m + _GET_LEN + 1;
    skipWhitespace(p);

    if (*p == '/' && isspace(p[1]))
    {
	const char HEADER[] = 
	    "HTTP/1.1 200 OK\r\n"
	    "CIMServer: Pegasus 1.0\r\n"
	    "Content-Length: %d\r\n"
	    "Connection: close\r\n"
	    "Content-CIMType: text/html\r\n\r\n";

	const char CONTENT[] =
	    "<HTML>\n"
	    "  <HEAD>\n"
	    "    <TITLE>Pegasus 1.0 CIMServer</TITLE>\n"
	    "  </HEAD>\n"
	    "  <BODY>\n"
	    "    <H1>You have reached the Pegasus 1.0 CIMServer</H1>\n"
	    "  </BODY>\n"
	    "</HTML>\n";

	char header[sizeof(HEADER) + 20];
	sprintf(header, HEADER, strlen(CONTENT));

	_channel->writeN(header, strlen(header));
	_channel->writeN(CONTENT, strlen(CONTENT));
    }
    else
    {
	// ATTN: We can expand this to a web server here
	const char HEADER[] = 
	    "HTTP/1.1 404 Not Found\r\n"
	    "CIMServer: Pegasus 1.0\r\n"
	    /* "Content-Length: %d\r\n" */
	    "Connection: close\r\n"
	    "Content-CIMType: text/html\r\n\r\n";

	const char CONTENT[] =
	    "<HTML>\n"
	    "  <HEAD>\n"
	    "    <TITLE>404 Not Found</TITLE>\n"
	    "  </HEAD>\n"
	    "  <BODY>\n"
	    "    <H1>Not Found</H1>\n"
	    "    The requested URL was not found on this server.<P>\n"
	    "  </BODY>\n"
	    "</HTML>\n";

	char header[sizeof(HEADER) + 20];
	sprintf(header, HEADER, strlen(CONTENT));

	_channel->writeN(header, strlen(header));
	_channel->writeN(CONTENT, strlen(CONTENT));

	return -1;
    }

    return 0;
}

int ClientConnection::handleMethodCall()
{
    //--------------------------------------------------------------------------
    // Create a parser:
    //--------------------------------------------------------------------------

    _message.append('\0');
    XmlParser parser((char*)getContent());
    XmlEntry entry;

    //--------------------------------------------------------------------------
    // Expect "<?xml ...>":
    //--------------------------------------------------------------------------

    XmlReader::expectXmlDeclaration(parser, entry);

    //--------------------------------------------------------------------------
    // <!ELEMENT CIM (MESSAGE|DECLARATION)>
    // <!ATTRLIST CIM
    //     CIMVERSION CDATA #REQUIRED
    //     DTDVERSION CDATA #REQUIRED>
    //--------------------------------------------------------------------------

    XmlReader::testCimStartTag(parser);

    //--------------------------------------------------------------------------
    // <!ELEMENT MESSAGE (SIMPLEREQ|MULTIREQ|SIMPLERSP|MULTIRSP)>
    // <!ATTLIST MESSAGE
    //     ID CDATA #REQUIRED
    //     PROTOCOLVERSION CDATA #REQUIRED>
    //
    // ATTN: Handle MULTIREQ, SIMPLERSP, and MULTIRSP.
    //--------------------------------------------------------------------------

    String messageId;
    const char* protocolVersion = 0;

    if (!XmlReader::getMessageStartTag(parser, messageId, protocolVersion))
	throw XmlValidationError(parser.getLine(), "expected MESSAGE element");

    if (strcmp(protocolVersion, "1.0") != 0)
    {
	throw XmlSemanticError(parser.getLine(), 
	    "Expected MESSAGE.PROTOCOLVERSION to be \"1.0\"");
    }

    //--------------------------------------------------------------------------
    // <!ELEMENT SIMPLEREQ (IMETHODCALL|METHODCALL)>
    //--------------------------------------------------------------------------

    XmlReader::expectStartTag(parser, entry, "SIMPLEREQ");

    //--------------------------------------------------------------------------
    // <!ELEMENT IMETHODCALL (LOCALNAMESPACEPATH,IPARAMVALUE*)>
    // <!ATTLIST IMETHODCALL %CIMName;>
    //--------------------------------------------------------------------------
    DDD(cout <<"RCV CIMServer handleMethodCall" << __LINE__  << endl;)
    DDD(cout << _message.getData() << endl;)

    const char* iMethodCallName = 0;

    if (!XmlReader::getIMethodCallStartTag(parser, iMethodCallName))
	throw XmlValidationError(parser.getLine(), 
	    "expected IMETHODCALL element");

    //--------------------------------------------------------------------------
    // <!ELEMENT LOCALNAMESPACEPATH (NAMESPACE+)>
    //--------------------------------------------------------------------------

    String nameSpace;

    if (!XmlReader::getLocalNameSpacePathElement(parser, nameSpace))
	throw XmlValidationError(parser.getLine(), 
	    "expected LOCALNAMESPACEPATH element");

    //--------------------------------------------------------------------------
    // Dispatch the method:
    //--------------------------------------------------------------------------

    if (CompareNoCase(iMethodCallName, "GetClass") == 0)
	handleGetClassRequest(parser, messageId, nameSpace);
    else if (CompareNoCase(iMethodCallName, "GetInstance") == 0)
	handleGetInstanceRequest(parser, messageId, nameSpace);
    else if (CompareNoCase(iMethodCallName, "EnumerateClassNames") == 0)
	handleEnumerateClassNamesRequest(parser, messageId, nameSpace);
    else if (CompareNoCase(iMethodCallName, "References") == 0)
	handleReferencesRequest(parser, messageId, nameSpace);
    else if (CompareNoCase(iMethodCallName, "ReferenceNames") == 0)
	handleReferenceNamesRequest(parser, messageId, nameSpace);
    else if (CompareNoCase(iMethodCallName, "AssociatorNames") == 0)
	handleAssociatorNamesRequest(parser, messageId, nameSpace);
    else if (CompareNoCase(iMethodCallName, "Associators") == 0)
	handleAssociatorsRequest(parser, messageId, nameSpace);
    else if (CompareNoCase(iMethodCallName, "CreateInstance") == 0)
	handleCreateInstanceRequest(parser, messageId, nameSpace);
    else if (CompareNoCase(iMethodCallName, "EnumerateInstanceNames") == 0)
	handleEnumerateInstanceNamesRequest(parser, messageId, nameSpace);
    else if (CompareNoCase(iMethodCallName, "DeleteQualifier") == 0)
	handleDeleteQualifierRequest(parser, messageId, nameSpace);
    else if (CompareNoCase(iMethodCallName, "GetQualifier") == 0)
	handleGetQualifierRequest(parser, messageId, nameSpace);
    else if (CompareNoCase(iMethodCallName, "SetQualifier") == 0)
	handleSetQualifierRequest(parser, messageId, nameSpace);
    else if (CompareNoCase(iMethodCallName, "EnumerateQualifiers") == 0)
	handleEnumerateQualifiersRequest(parser, messageId, nameSpace);
    else if (CompareNoCase(iMethodCallName, "EnumerateClasses") == 0)
	handleEnumerateClassesRequest(parser, messageId, nameSpace);
    else if (CompareNoCase(iMethodCallName, "EnumerateClassNames") == 0)
	handleEnumerateClassNamesRequest(parser, messageId, nameSpace);
    else if (CompareNoCase(iMethodCallName, "EnumerateInstances") == 0)
	handleEnumerateInstancesRequest(parser, messageId, nameSpace);
    else if (CompareNoCase(iMethodCallName, "CreateClass") == 0)
	handleCreateClassRequest(parser, messageId, nameSpace);
    else if (CompareNoCase(iMethodCallName, "ModifyClass") == 0)
	handleModifyClassRequest(parser, messageId, nameSpace);
    else if (CompareNoCase(iMethodCallName, "ModifyInstance") == 0)
	handleModifyInstanceRequest(parser, messageId, nameSpace);
    else if (CompareNoCase(iMethodCallName, "DeleteClass") == 0)
	handleDeleteClassRequest(parser, messageId, nameSpace);
    else if (CompareNoCase(iMethodCallName, "DeleteInstance") == 0)
	handleDeleteInstanceRequest(parser, messageId, nameSpace);
    else if (CompareNoCase(iMethodCallName, "GetProperty") == 0)
	handleGetPropertyRequest(parser, messageId, nameSpace);
    else if (CompareNoCase(iMethodCallName, "SetProperty") == 0)
	handleSetPropertyRequest(parser, messageId, nameSpace);
    else
    {
	String msg = CIMStatusCodeToString(CIM_ERR_FAILED);
	msg += ": unknown intrinsic method: ";
	msg += iMethodCallName;
	char* tmp = msg.allocateCString();
	sendError(messageId, iMethodCallName, CIM_ERR_FAILED, tmp);
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

void ClientConnection::handleEnqueue()
{
    Message* response = dequeue();

    if (!response)
	return;

    if (getenv("PEGASUS_TRACE"))
	response->print(cout);

    switch (response->getType())
    {
	case CIM_GET_CLASS_RESPONSE_MESSAGE:
	    handleGetClassResponse((CIMGetClassResponseMessage*)response);
	    break;

	case CIM_GET_INSTANCE_RESPONSE_MESSAGE:
	    handleGetInstanceResponse((CIMGetInstanceResponseMessage*)response);
	    break;

	case CIM_DELETE_CLASS_RESPONSE_MESSAGE:
	    handleDeleteClassResponse((CIMDeleteClassResponseMessage*)response);
	    break;

	case CIM_DELETE_INSTANCE_RESPONSE_MESSAGE:
	    handleDeleteInstanceResponse(
		(CIMDeleteInstanceResponseMessage*)response);
	    break;

	case CIM_CREATE_CLASS_RESPONSE_MESSAGE:
	    handleCreateClassResponse((CIMCreateClassResponseMessage*)response);
	    break;

	case CIM_CREATE_INSTANCE_RESPONSE_MESSAGE:
	    handleCreateInstanceResponse(
		(CIMCreateInstanceResponseMessage*)response);
	    break;

	case CIM_MODIFY_CLASS_RESPONSE_MESSAGE:
	    handleModifyClassResponse(
		(CIMModifyClassResponseMessage*)response);
	    break;

	case CIM_MODIFY_INSTANCE_RESPONSE_MESSAGE:
	    handleModifyInstanceResponse(
		(CIMModifyInstanceResponseMessage*)response);
	    break;

	case CIM_ENUMERATE_CLASSES_RESPONSE_MESSAGE:
	    handleEnumerateClassesResponse(
		(CIMEnumerateClassesResponseMessage*)response);
	    break;

	case CIM_ENUMERATE_CLASS_NAMES_RESPONSE_MESSAGE:
	    handleEnumerateClassNamesResponse(
		(CIMEnumerateClassNamesResponseMessage*)response);
	    break;

	case CIM_ENUMERATE_INSTANCES_RESPONSE_MESSAGE:
	    handleEnumerateInstancesResponse(
		(CIMEnumerateInstancesResponseMessage*)response);
	    break;

	case CIM_ENUMERATE_INSTANCE_NAMES_RESPONSE_MESSAGE:
	    handleEnumerateInstanceNamesResponse(
		(CIMEnumerateInstanceNamesResponseMessage*)response);
	    break;

	// ATTN: implement this!
	case CIM_EXEC_QUERY_RESPONSE_MESSAGE:
	    break;

	case CIM_ASSOCIATORS_RESPONSE_MESSAGE:
	    handleAssociatorsResponse((CIMAssociatorsResponseMessage*)response);
	    break;

	case CIM_ASSOCIATOR_NAMES_RESPONSE_MESSAGE:
	    handleAssociatorNamesResponse(
		(CIMAssociatorNamesResponseMessage*)response);
	    break;

	case CIM_REFERENCES_RESPONSE_MESSAGE:
	    handleReferencesResponse((CIMReferencesResponseMessage*)response);
	    break;

	case CIM_REFERENCE_NAMES_RESPONSE_MESSAGE:
	    handleReferenceNamesResponse(
		(CIMReferenceNamesResponseMessage*)response);
	    break;

	// ATTN: implement this!
	case CIM_GET_PROPERTY_RESPONSE_MESSAGE:
	    break;

	// ATTN: implement this!
	case CIM_SET_PROPERTY_RESPONSE_MESSAGE:
	    break;

	case CIM_GET_QUALIFIER_RESPONSE_MESSAGE:
	    handleGetQualifierResponse(
		(CIMGetQualifierResponseMessage*)response);
	    break;

	case CIM_SET_QUALIFIER_RESPONSE_MESSAGE:
	    handleSetQualifierResponse(
		(CIMSetQualifierResponseMessage*)response);
	    break;

	case CIM_DELETE_QUALIFIER_RESPONSE_MESSAGE:
	    handleDeleteQualifierResponse(
		(CIMDeleteQualifierResponseMessage*)response);
	    break;

	case CIM_ENUMERATE_QUALIFIERS_RESPONSE_MESSAGE:
	    handleEnumerateQualifiersResponse(
		(CIMEnumerateQualifiersResponseMessage*)response);
	    break;

	// ATTN: implement this:
	case CIM_INVOKE_METHOD_RESPONSE_MESSAGE:
	    break;
    }
}

void ClientConnection::sendError(
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
    
    sendMessage(message);
}

void ClientConnection::sendError(
    CIMResponseMessage* m,
    const char* methodName)
{
    char* tmp = m->errorDescription.allocateCString();
    sendError(m->messageId, methodName, m->errorCode, tmp);
    delete [] tmp;
}

void ClientConnection::handleGetPropertyRequest(
    XmlParser& parser, 
    const String& messageId,
    const String& nameSpace)
{
    // ATTN: implement this!
    PEGASUS_ASSERT(0);
}

void ClientConnection::handleSetPropertyRequest(
    XmlParser& parser, 
    const String& messageId,
    const String& nameSpace)
{
    // ATTN: implement this!
    PEGASUS_ASSERT(0);
}

void ClientConnection::handleGetClassRequest(
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

void ClientConnection::handleGetClassResponse(
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

    sendMessage(message);
}

void ClientConnection::handleGetInstanceRequest(
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

void ClientConnection::handleGetInstanceResponse(
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

    sendMessage(message);
}

void ClientConnection::handleDeleteClassRequest(
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

void ClientConnection::handleDeleteClassResponse(
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

    sendMessage(message);
}


void ClientConnection::handleDeleteInstanceRequest(
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

void ClientConnection::handleDeleteInstanceResponse(
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

    sendMessage(message);
}

void ClientConnection::handleCreateClassRequest(
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

void ClientConnection::handleCreateClassResponse(
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

    sendMessage(message);
}

void ClientConnection::handleCreateInstanceRequest(
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

void ClientConnection::handleCreateInstanceResponse(
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

    sendMessage(message);
}

void ClientConnection::handleModifyClassRequest(
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

void ClientConnection::handleModifyClassResponse(
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

    sendMessage(message);
}

void ClientConnection::handleModifyInstanceRequest(
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

void ClientConnection::handleModifyInstanceResponse(
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

    sendMessage(message);
}

void ClientConnection::handleEnumerateClassesRequest(
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

void ClientConnection::handleEnumerateClassesResponse(
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

    sendMessage(message);
}

void ClientConnection::handleEnumerateClassNamesRequest(
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

void ClientConnection::handleEnumerateClassNamesResponse(
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

    sendMessage(message);
}

void ClientConnection::handleEnumerateInstancesRequest(
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

void ClientConnection::handleEnumerateInstancesResponse(
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

    sendMessage(message);
}

void ClientConnection::handleEnumerateInstanceNamesRequest(
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

void ClientConnection::handleEnumerateInstanceNamesResponse(
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

    sendMessage(message);
}

void ClientConnection::handleAssociatorsRequest(
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

void ClientConnection::handleAssociatorsResponse(
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

    sendMessage(message);
}

void ClientConnection::handleAssociatorNamesRequest(
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

void ClientConnection::handleAssociatorNamesResponse(
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

    sendMessage(message);
}

void ClientConnection::handleReferencesRequest(
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

void ClientConnection::handleReferencesResponse(
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

    sendMessage(message);
}

void ClientConnection::handleReferenceNamesRequest(
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

void ClientConnection::handleReferenceNamesResponse(
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

    sendMessage(message);
}

void ClientConnection::handleGetQualifierRequest(
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

void ClientConnection::handleGetQualifierResponse(
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

    sendMessage(message);
}

void ClientConnection::handleSetQualifierRequest(
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

void ClientConnection::handleSetQualifierResponse(
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

    sendMessage(message);
}

void ClientConnection::handleDeleteQualifierRequest(
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

void ClientConnection::handleDeleteQualifierResponse(
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

    sendMessage(message);
}

void ClientConnection::handleEnumerateQualifiersRequest(
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

void ClientConnection::handleEnumerateQualifiersResponse(
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

    sendMessage(message);
}

PEGASUS_NAMESPACE_END
