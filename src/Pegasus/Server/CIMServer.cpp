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
//         Mike Day (mdday@us.ibm.com)s
//
//%/////////////////////////////////////////////////////////////////////////////

#include <cassert>
#include <cstdio>
#include <cctype>
#include <time.h>

#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/XmlParser.h>
#include <Pegasus/Common/XmlReader.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/TCPChannel.h>
#include <Pegasus/Common/Logger.h>
#include <Pegasus/Repository/CIMRepository.h>
#include <Pegasus/Protocol/Handler.h>
#include <Pegasus/Server/CIMServer.h>
#include <Pegasus/Server/Dispatcher.h>
#include <Pegasus/Common/MessageQueue.h>
#include <Pegasus/Common/CIMMessage.h>

//debugging

#define DDD(X) // X

#include <iostream>

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
//
// Local routines:
//
////////////////////////////////////////////////////////////////////////////////

static const char _GET[] = "GET";
static const Uint32 _GET_LEN = sizeof(_GET) - 1;

static const char _M_POST[] = "M-POST";
static const Uint32 _M_POST_LEN = sizeof(_M_POST) - 1;

// some degree of leniency toward the SNIA Browser, that contrary to
// the norm sends out a POST instead of an M_POST
static const char _POST[] = "POST";
static const Uint32 _POST_LEN = sizeof(_POST) - 1;

////////////////////////////////////////////////////////////////////////////////
//
// ServerHandler
//
//	This is a one-per-connection class.
//
////////////////////////////////////////////////////////////////////////////////

class ServerHandler : public Handler, public MessageQueue
{
public:

    ServerHandler(Dispatcher* dispatcher) : _dispatcher(dispatcher), _channel(0)
    {
    }

    ~ServerHandler();

    virtual Boolean handleOpen(Channel* channel)
    {
	_channel = channel;
	DDD(cout << "Handle Open" << endl;)
	return Handler::handleOpen(channel);
    }

    virtual int handleMessage();

    int handleGetRequest();

    int handleMethodCall();

    /** This method is called when a response is enqueued on this queue.
    */
    virtual void handleEnqueue();

    void sendError(
	const String& messageId,
	const char* methodName,
	CIMStatusCode code,
	const char* description);

    void sendError(
	CIMResponseMessage* message,
	const char* methodName);

    void handleGetClassRequest(
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    void handleResponse(
	CIMGetClassResponseMessage* response);

    void handleGetInstanceRequest(
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    void handleResponse(
	CIMGetInstanceResponseMessage* response);

    //STUB{
    void handleEnumerateClassNamesRequest(
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);
    //STUB}

    void handleReferenceNamesRequest(
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    void handleReferencesRequest(
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    void handleAssociatorNamesRequest(
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    void handleAssociatorsRequest(
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    void handleCreateInstanceRequest(
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    void handleEnumerateInstanceNamesRequest(
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    void handleDeleteQualifierRequest(
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    void handleGetQualifierRequest(
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    void handleSetQualifierRequest(
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    void handleEnumerateQualifiersRequest(
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    void handleEnumerateClassesRequest(
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    void handleEnumerateInstancesRequest(
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    void handleCreateClassRequest(
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    void handleModifyClassRequest(
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    void handleDeleteClassRequest(
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    void handleResponse(
	CIMDeleteClassResponseMessage* response);

    void handleDeleteInstanceRequest(
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    void handleResponse(
	CIMDeleteInstanceResponseMessage* response);

    void handleGetPropertyRequest(
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    void handleSetPropertyRequest(
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    /** sendMessage sends the message and optionally
        generates a trace output.
	@param message - The message as an array<sint8>
    */     
    void sendMessage(Array<Sint8>& message);

private:

    Dispatcher* _dispatcher;
    Channel* _channel;
};

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
void ServerHandler::sendMessage (Array<Sint8>& message)
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

ServerHandler::~ServerHandler()
{

}

//------------------------------------------------------------------------------
//
// ServerHandler::handleMessage()
//
//------------------------------------------------------------------------------

int ServerHandler::handleMessage()
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

//------------------------------------------------------------------------------
//
// ServerHandler::handleGetRequest()
//
//------------------------------------------------------------------------------

// ATTN-KS: The handleGetRequest is where we can begin to move in a WEB 
//	server.
// This is also where an external module can be really effective.  If the 
// external module exists, it would do the WEB stuff.

int ServerHandler::handleGetRequest()
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

//------------------------------------------------------------------------------
//
// ServerHandler::handleMethodCall()
//
// <?xml version="1.0" encoding="utf-8"?>
// <CIM CIMVERSION="2.0" DTDVERSION="2.0">
//   <MESSAGE ID="1" PROTOCOLVERSION="1.0">
//     <SIMPLEREQ>
//       <IMETHODCALL NAME="GetClass">
//         <LOCALNAMESPACEPATH>
//           <NAMESPACE NAME="root"/>
//           <NAMESPACE NAME="cimv2"/>
//         </LOCALNAMESPACEPATH>
//         <IPARAMVALUE NAME="ClassName">
//           <CLASSNAME NAME="MyClass"/>
//         </IPARAMVALUE>
//         <IPARAMVALUE NAME="LocalOnly">
//           <VALUE>
//             FALSE
//           </VALUE>
//         </IPARAMVALUE>
//       </IMETHODCALL>
//     </SIMPLEREQ>
//   </MESSAGE>
// </CIM>
//
//------------------------------------------------------------------------------

int ServerHandler::handleMethodCall()
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
    //STUB{
    else if (CompareNoCase(iMethodCallName, "EnumerateClassNames") == 0)
	handleEnumerateClassNamesRequest(parser, messageId, nameSpace);
    //STUB}
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
    else if (CompareNoCase(iMethodCallName, "EnumerateInstances") == 0)
	handleEnumerateInstancesRequest(parser, messageId, nameSpace);
    else if (CompareNoCase(iMethodCallName, "CreateClass") == 0)
	handleCreateClassRequest(parser, messageId, nameSpace);
    else if (CompareNoCase(iMethodCallName, "ModifyClass") == 0)
	handleModifyClassRequest(parser, messageId, nameSpace);
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

void ServerHandler::handleEnqueue()
{
    Message* response = dequeue();

    if (!response)
	return;

    // response->print(cout);

    switch (response->getType())
    {
	case CIM_GET_CLASS_RESPONSE_MESSAGE:
	    handleResponse((CIMGetClassResponseMessage*)response);
	    break;

	case CIM_GET_INSTANCE_RESPONSE_MESSAGE:
	    handleResponse((CIMGetInstanceResponseMessage*)response);
	    break;

	case CIM_DELETE_CLASS_RESPONSE_MESSAGE:
	    handleResponse((CIMDeleteClassResponseMessage*)response);
	    break;

	case CIM_DELETE_INSTANCE_RESPONSE_MESSAGE:
	    handleResponse((CIMDeleteInstanceResponseMessage*)response);
	    break;

	case CIM_CREATE_CLASS_RESPONSE_MESSAGE:
	case CIM_CREATE_INSTANCE_RESPONSE_MESSAGE:
	case CIM_MODIFY_CLASS_RESPONSE_MESSAGE:
	case CIM_MODIFY_INSTANCE_RESPONSE_MESSAGE:
	case CIM_ENUMERATE_CLASSES_RESPONSE_MESSAGE:
	case CIM_ENUMERATE_CLASS_NAMES_RESPONSE_MESSAGE:
	case CIM_ENUMERATE_INSTANCES_RESPONSE_MESSAGE:
	case CIM_ENUMERATE_INSTANCE_NAMES_RESPONSE_MESSAGE:
	case CIM_EXEC_QUERY_RESPONSE_MESSAGE:
	case CIM_ASSOCIATORS_RESPONSE_MESSAGE:
	case CIM_ASSOCIATOR_NAMES_RESPONSE_MESSAGE:
	case CIM_REFERENCES_RESPONSE_MESSAGE:
	case CIM_REFERENCE_NAMES_RESPONSE_MESSAGE:
	case CIM_GET_PROPERTY_RESPONSE_MESSAGE:
	case CIM_SET_PROPERTY_RESPONSE_MESSAGE:
	case CIM_GET_QUALIFIER_RESPONSE_MESSAGE:
	case CIM_SET_QUALIFIER_RESPONSE_MESSAGE:
	case CIM_DELETE_QUALIFIER_RESPONSE_MESSAGE:
	case CIM_ENUMERATE_QUALIFIERS_RESPONSE_MESSAGE:
	case CIM_INVOKE_METHOD_RESPONSE_MESSAGE:
	    break;
    }
}

//------------------------------------------------------------------------------
//
// ServerHandler::sendError()
//
//------------------------------------------------------------------------------

void ServerHandler::sendError(
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

void ServerHandler::sendError(
    CIMResponseMessage* m,
    const char* methodName)
{
    char* tmp = m->errorDescription.allocateCString();
    sendError(m->messageId, methodName, m->errorCode, tmp);
    delete [] tmp;
}

//STUB{
//------------------------------------------------------------------------------
//
// ServerHandler::handleEnumerateClassNamesRequest()
//
//------------------------------------------------------------------------------

void ServerHandler::handleEnumerateClassNamesRequest(
    XmlParser& parser, 
    const String& messageId,
    const String& nameSpace)
{
    //--------------------------------------------------------------------------
    // <!ELEMENT IPARAMVALUE (VALUE|VALUE.ARRAY|VALUE.REFERENCE
    //     |INSTANCENAME|CLASSNAME|QUALIFIER.DECLARATION
    //     |CLASS|INSTANCE|VALUE.NAMEDINSTANCE)?>
    // <!ATTLIST IPARAMVALUE %CIMName;>
    //--------------------------------------------------------------------------

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

    Array<String> classNames;
    
    try
    {
	classNames = _dispatcher->enumerateClassNames(
	    nameSpace,
	    className,
	    deepInheritance);
    }
    catch (CIMException& e)
    {
	sendError(messageId, "EnumerateClassNames", 
	    e.getCode(), CIMStatusCodeToString(e.getCode()));
	return;
    }
    catch (Exception&)
    {
	sendError(messageId, "EnumerateClassNames", CIM_ERR_FAILED, 
	    CIMStatusCodeToString(CIM_ERR_FAILED));
	return;
    }

    Array<Sint8> body;

    for (Uint32 i = 0; i < classNames.size(); i++)
	XmlWriter::appendClassNameElement(body, classNames[i]);

    Array<Sint8> message = XmlWriter::formatSimpleRspMessage(
	"EnumerateClassNames", messageId, body);

    sendMessage(message);
}
//STUB}

//------------------------------------------------------------------------------
//
// ServerHandler::handleReferencesRequest()
//
//------------------------------------------------------------------------------

void ServerHandler::handleReferencesRequest(
    XmlParser& parser, 
    const String& messageId,
    const String& nameSpace)
{
    // -- Extract the parameters:

    CIMReference objectName;
    String resultClass;
    String role;
    Boolean includeQualifiers = false;
    Boolean includeClassOrigin = false;
    Array<String> propertyList;

    // ATTN-B: handle the property list!

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

    Array<CIMObjectWithPath> objectWithPathArray;
    
    try
    {
	objectWithPathArray = _dispatcher->references(
	    nameSpace,
	    objectName,
	    resultClass,
	    role,
	    includeQualifiers,
	    includeClassOrigin,
	    propertyList);
    }
    catch (CIMException& e)
    {
	sendError(messageId, "References", 
	    e.getCode(), CIMStatusCodeToString(e.getCode()));
	return;
    }
    catch (Exception&)
    {
	sendError(messageId, "References", CIM_ERR_FAILED, 
	    CIMStatusCodeToString(CIM_ERR_FAILED));
	return;
    }

    Array<Sint8> body;

    for (Uint32 i = 0; i < objectWithPathArray.size(); i++)
	objectWithPathArray[i].toXml(body);

    Array<Sint8> message = XmlWriter::formatSimpleRspMessage(
	"References", messageId, body);

    sendMessage(message);
}

//------------------------------------------------------------------------------
//
// ServerHandler::handleCreateInstanceRequest()
//
//------------------------------------------------------------------------------

void ServerHandler::handleCreateInstanceRequest(
    XmlParser& parser, 
    const String& messageId,
    const String& nameSpace)
{
    //--------------------------------------------------------------------------
    // <!ELEMENT IPARAMVALUE (VALUE|VALUE.ARRAY|VALUE.REFERENCE
    //     |INSTANCENAME|CLASSNAME|QUALIFIER.DECLARATION
    //     |CLASS|INSTANCE|VALUE.NAMEDINSTANCE)?>
    // <!ATTLIST IPARAMVALUE %CIMName;>
    //--------------------------------------------------------------------------

    CIMInstance cimInstance;

    for (const char* name; XmlReader::getIParamValueTag(parser, name);)
    {
	if (CompareNoCase(name, "NewInstance") == 0)
	    XmlReader::getInstanceElement(parser, cimInstance);

	XmlReader::expectEndTag(parser, "IPARAMVALUE");
    }

    try
    {
	_dispatcher->createInstance(nameSpace, cimInstance);
    }
    catch (CIMException& e)
    {
	sendError(messageId, "CreateInstance", 
	    e.getCode(), CIMStatusCodeToString(e.getCode()));
	return;
    }
    catch (Exception& e)
    {
	char* tmp = e.getMessage().allocateCString();

	sendError(messageId, "CreateInstance", CIM_ERR_FAILED, tmp);
	    // CIMStatusCodeToString(CIM_ERR_FAILED));

	delete [] tmp;
	return;
    }

    Array<Sint8> body;

    Array<Sint8> message = XmlWriter::formatSimpleRspMessage(
	"CreateInstance", messageId, body);

    sendMessage(message);
}

//------------------------------------------------------------------------------
//
// ServerHandler::handleEnumerateInstanceNamesRequest()
//
//------------------------------------------------------------------------------

void ServerHandler::handleEnumerateInstanceNamesRequest(
    XmlParser& parser, 
    const String& messageId,
    const String& nameSpace)
{
    //--------------------------------------------------------------------------
    // <!ELEMENT IPARAMVALUE (VALUE|VALUE.ARRAY|VALUE.REFERENCE
    //     |INSTANCENAME|CLASSNAME|QUALIFIER.DECLARATION
    //     |CLASS|INSTANCE|VALUE.NAMEDINSTANCE)?>
    // <!ATTLIST IPARAMVALUE %CIMName;>
    //--------------------------------------------------------------------------

    String className;

    for (const char* name; XmlReader::getIParamValueTag(parser, name);)
    {
	if (CompareNoCase(name, "ClassName") == 0)
	    XmlReader::getClassNameElement(parser, className, true);

	XmlReader::expectEndTag(parser, "IPARAMVALUE");
    }

    Array<CIMReference> instanceNames;
    
    try
    {
	instanceNames = _dispatcher->enumerateInstanceNames(
	    nameSpace,
	    className);
    }
    catch (CIMException& e)
    {
	sendError(messageId, "EnumerateInstanceNames", 
	    e.getCode(), CIMStatusCodeToString(e.getCode()));
	return;
    }
    catch (Exception&)
    {
	sendError(messageId, "EnumerateInstanceNames", CIM_ERR_FAILED, 
	    CIMStatusCodeToString(CIM_ERR_FAILED));
	return;
    }

    Array<Sint8> body;

    for (Uint32 i = 0; i < instanceNames.size(); i++)
	XmlWriter::appendInstanceNameElement(body, instanceNames[i]);

    Array<Sint8> message = XmlWriter::formatSimpleRspMessage(
	"EnumerateInstanceNames", messageId, body);

    sendMessage(message);
}

//------------------------------------------------------------------------------
//
// ServerHandler::handleDeleteQualifierRequest()
//
//------------------------------------------------------------------------------

void ServerHandler::handleDeleteQualifierRequest(
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

    try
    {
	_dispatcher->deleteQualifier(nameSpace, qualifierName);
    }
    catch (CIMException& e)
    {
	sendError(messageId, "DeleteQualifier", 
	    e.getCode(), CIMStatusCodeToString(e.getCode()));
	return;
    }
    catch (Exception&)
    {
	sendError(messageId, "DeleteQualifier", CIM_ERR_FAILED, 
	    CIMStatusCodeToString(CIM_ERR_FAILED));
	return;
    }

    Array<Sint8> body;

    Array<Sint8> message = XmlWriter::formatSimpleRspMessage(
	"DeleteQualifier", messageId, body);

    sendMessage(message);
}

//------------------------------------------------------------------------------
//
// ServerHandler::handleGetQualifierRequest()
//
//------------------------------------------------------------------------------

void ServerHandler::handleGetQualifierRequest(
    XmlParser& parser, 
    const String& messageId,
    const String& nameSpace)
{
    String qualifierName;
    Boolean deepInheritance = false;

    for (const char* name; XmlReader::getIParamValueTag(parser, name);)
    {
	if (CompareNoCase(name, "QualifierName") == 0)
	    XmlReader::getClassNameElement(parser, qualifierName, true);

	XmlReader::expectEndTag(parser, "IPARAMVALUE");
    }

    CIMQualifierDecl qualifierDecl;
    
    try
    {
	qualifierDecl = _dispatcher->getQualifier(nameSpace, qualifierName);
    }
    catch (CIMException& e)
    {
	sendError(messageId, "GetQualifier", 
	    e.getCode(), CIMStatusCodeToString(e.getCode()));
	return;
    }
    catch (Exception&)
    {
	sendError(messageId, "GetQualifier", CIM_ERR_FAILED, 
	    CIMStatusCodeToString(CIM_ERR_FAILED));
	return;
    }

    Array<Sint8> body;
    qualifierDecl.toXml(body);

    Array<Sint8> message = XmlWriter::formatSimpleRspMessage(
	"GetQualifier", messageId, body);

    sendMessage(message);
}

//------------------------------------------------------------------------------
//
// ServerHandler::handleSetQualifierRequest()
//
//------------------------------------------------------------------------------

void ServerHandler::handleSetQualifierRequest(
    XmlParser& parser, 
    const String& messageId,
    const String& nameSpace)
{
    CIMQualifierDecl qualifierDecl;

    for (const char* name; XmlReader::getIParamValueTag(parser, name);)
    {
	if (CompareNoCase(name, "QualifierDeclaration") == 0)
	    XmlReader::getQualifierDeclElement(parser, qualifierDecl);

	XmlReader::expectEndTag(parser, "IPARAMVALUE");
    }

    Array<String> classNames;
    
    try
    {
	_dispatcher->setQualifier(nameSpace, qualifierDecl);
    }
    catch (CIMException& e)
    {
	sendError(messageId, "SetQualifier", 
	    e.getCode(), CIMStatusCodeToString(e.getCode()));
	return;
    }
    catch (Exception&)
    {
	sendError(messageId, "SetQualifier", CIM_ERR_FAILED, 
	    CIMStatusCodeToString(CIM_ERR_FAILED));
	return;
    }

    Array<Sint8> body;

    Array<Sint8> message = XmlWriter::formatSimpleRspMessage(
	"SetQualifier", messageId, body);

    sendMessage(message);
}

//------------------------------------------------------------------------------
//
// ServerHandler::handleEnumerateQualifiersRequest()
//
//------------------------------------------------------------------------------

void ServerHandler::handleEnumerateQualifiersRequest(
    XmlParser& parser, 
    const String& messageId,
    const String& nameSpace)
{
    for (const char* name; XmlReader::getIParamValueTag(parser, name);)
	XmlReader::expectEndTag(parser, "IPARAMVALUE");

    Array<CIMQualifierDecl> qualifierDecls;
    
    try
    {
	qualifierDecls = _dispatcher->enumerateQualifiers(nameSpace);
    }
    catch (CIMException& e)
    {
	sendError(messageId, "EnumerateQualifiers", 
	    e.getCode(), CIMStatusCodeToString(e.getCode()));
	return;
    }
    catch (Exception&)
    {
	sendError(messageId, "EnumerateQualifiers", CIM_ERR_FAILED, 
	    CIMStatusCodeToString(CIM_ERR_FAILED));
	return;
    }

    Array<Sint8> body;

    for (Uint32 i = 0; i < qualifierDecls.size(); i++)
	qualifierDecls[i].toXml(body);

    Array<Sint8> message = XmlWriter::formatSimpleRspMessage(
	"EnumerateQualifiers", messageId, body);

    sendMessage(message);
}

//------------------------------------------------------------------------------
//
// ServerHandler::handleEnumerateClassesRequest()
//
//------------------------------------------------------------------------------

void ServerHandler::handleEnumerateClassesRequest(
    XmlParser& parser, 
    const String& messageId,
    const String& nameSpace)
{
    //--------------------------------------------------------------------------
    // <!ELEMENT IPARAMVALUE (VALUE|VALUE.ARRAY|VALUE.REFERENCE
    //     |INSTANCENAME|CLASSNAME|QUALIFIER.DECLARATION
    //     |CLASS|INSTANCE|VALUE.NAMEDINSTANCE)?>
    // <!ATTLIST IPARAMVALUE %CIMName;>
    //--------------------------------------------------------------------------

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

    Array<CIMClass> classDecls;
    
    try
    {
	classDecls = _dispatcher->enumerateClasses(
	    nameSpace,
	    className,
	    deepInheritance,
	    localOnly,
	    includeQualifiers,
	    includeClassOrigin);
    }
    catch (CIMException& e)
    {
	sendError(messageId, "EnumerateClasses", 
	    e.getCode(), CIMStatusCodeToString(e.getCode()));
	return;
    }
    catch (Exception&)
    {
	sendError(messageId, "EnumerateClasses", CIM_ERR_FAILED, 
	    CIMStatusCodeToString(CIM_ERR_FAILED));
	return;
    }

    Array<Sint8> body;

    for (Uint32 i = 0; i < classDecls.size(); i++)
	classDecls[i].toXml(body);

    Array<Sint8> message = XmlWriter::formatSimpleRspMessage(
	"EnumerateClasses", messageId, body);

    sendMessage(message);
}

//------------------------------------------------------------------------------
//
// ServerHandler::handleEnumerateClassesRequest()
//
//------------------------------------------------------------------------------

void ServerHandler::handleEnumerateInstancesRequest(
    XmlParser& parser, 
    const String& messageId,
    const String& nameSpace)
{
    //--------------------------------------------------------------------------
    // <!ELEMENT IPARAMVALUE (VALUE|VALUE.ARRAY|VALUE.REFERENCE
    //     |INSTANCENAME|CLASSNAME|QUALIFIER.DECLARATION
    //     |CLASS|INSTANCE|VALUE.NAMEDINSTANCE)?>
    // <!ATTLIST IPARAMVALUE %CIMName;>
    //--------------------------------------------------------------------------

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

    Array<CIMInstance> instances;
    
    try
    {
	instances = _dispatcher->enumerateInstances(
	    nameSpace,
	    className,
	    deepInheritance,
	    localOnly,
	    includeQualifiers,
	    includeClassOrigin,
	    propertyList);
    }
    catch (CIMException& e)
    {
	sendError(messageId, "EnumerateInstances", 
	    e.getCode(), CIMStatusCodeToString(e.getCode()));
	return;
    }
    catch (Exception&)
    {
	sendError(messageId, "EnumerateInstances", CIM_ERR_FAILED, 
	    CIMStatusCodeToString(CIM_ERR_FAILED));
	return;
    }

    Array<Sint8> body;

    for (Uint32 i = 0; i < instances.size(); i++)
	instances[i].toXml(body);

    Array<Sint8> message = XmlWriter::formatSimpleRspMessage(
	"EnumerateInstances", messageId, body);

    sendMessage(message);
}


//------------------------------------------------------------------------------
//
// ServerHandler::handleCreateClassRequest()
//
//------------------------------------------------------------------------------

void ServerHandler::handleCreateClassRequest(
    XmlParser& parser, 
    const String& messageId,
    const String& nameSpace)
{
    //--------------------------------------------------------------------------
    // <!ELEMENT IPARAMVALUE (VALUE|VALUE.ARRAY|VALUE.REFERENCE
    //     |INSTANCENAME|CLASSNAME|QUALIFIER.DECLARATION
    //     |CLASS|INSTANCE|VALUE.NAMEDINSTANCE)?>
    // <!ATTLIST IPARAMVALUE %CIMName;>
    //--------------------------------------------------------------------------

    CIMClass cimClass;

    for (const char* name; XmlReader::getIParamValueTag(parser, name);)
    {
	if (CompareNoCase(name, "NewClass") == 0)
	    XmlReader::getClassElement(parser, cimClass);

	XmlReader::expectEndTag(parser, "IPARAMVALUE");
    }

    try
    {
	_dispatcher->createClass(nameSpace, cimClass);
    }
    catch (CIMException& e)
    {
	sendError(messageId, "CreateClass", 
	    e.getCode(), CIMStatusCodeToString(e.getCode()));
	return;
    }
    catch (Exception&)
    {
	sendError(messageId, "CreateClass", CIM_ERR_FAILED, 
	    CIMStatusCodeToString(CIM_ERR_FAILED));
	return;
    }

    Array<Sint8> body;

    Array<Sint8> message = XmlWriter::formatSimpleRspMessage(
	"CreateClass", messageId, body);

    sendMessage(message);
}

//------------------------------------------------------------------------------
//
// ServerHandler::handleModifyClassRequest()
//
//------------------------------------------------------------------------------

void ServerHandler::handleModifyClassRequest(
    XmlParser& parser, 
    const String& messageId,
    const String& nameSpace)
{
    //--------------------------------------------------------------------------
    // <!ELEMENT IPARAMVALUE (VALUE|VALUE.ARRAY|VALUE.REFERENCE
    //     |INSTANCENAME|CLASSNAME|QUALIFIER.DECLARATION
    //     |CLASS|INSTANCE|VALUE.NAMEDINSTANCE)?>
    // <!ATTLIST IPARAMVALUE %CIMName;>
    //--------------------------------------------------------------------------

    CIMClass cimClass;

    for (const char* name; XmlReader::getIParamValueTag(parser, name);)
    {
	if (CompareNoCase(name, "ModifiedClass") == 0)
	    XmlReader::getClassElement(parser, cimClass);

	XmlReader::expectEndTag(parser, "IPARAMVALUE");
    }

    try
    {
	_dispatcher->modifyClass(nameSpace, cimClass);
    }
    catch (CIMException& e)
    {
	sendError(messageId, "ModifyClass", 
	    e.getCode(), CIMStatusCodeToString(e.getCode()));
	return;
    }
    catch (Exception&)
    {
	sendError(messageId, "ModifyClass", CIM_ERR_FAILED, 
	    CIMStatusCodeToString(CIM_ERR_FAILED));
	return;
    }

    Array<Sint8> body;

    Array<Sint8> message = XmlWriter::formatSimpleRspMessage(
	"ModifyClass", messageId, body);

    sendMessage(message);
}

//------------------------------------------------------------------------------
//
// ServerHandler::handleGetProoperty()
//
// <!ELEMENT IPARAMVALUE (VALUE|VALUE.ARRAY|VALUE.REFERENCE
//     |INSTANCENAME|CLASSNAME|QUALIFIER.DECLARATION
//     |CLASS|INSTANCE|VALUE.NAMEDINSTANCE)?>
// <!ATTLIST IPARAMVALUE %CIMName;>
//------------------------------------------------------------------------------

void ServerHandler::handleGetPropertyRequest(
    XmlParser& parser, 
    const String& messageId,
    const String& nameSpace)
{

    /// This is a really sloppy way to get the instance anme
    CIMReference instanceName;
    String propertyName;
    CIMValue cimValueRtn;

    for (const char* name; XmlReader::getIParamValueTag(parser, name);)
    {
	CIMValue cimValue; 
	if (CompareNoCase(name, "InstanceName") == 0)
       {
	   String className;
	   Array<KeyBinding> keyBindings;
	   XmlReader::getInstanceNameElement(parser, className, keyBindings);

	   // ATTN: do we need the namespace here?

	   instanceName.set(String(), String(), className, keyBindings);
       }

       // Get the Property Name field
       // How do we know if we got everything??
       else if (strcmp(name, "PropertyName") == 0)
	   XmlReader::getValueElement(parser, CIMType::STRING, cimValue);

       XmlReader::expectEndTag(parser, "IPARAMVALUE");

       //propertyName = cimValue.data;
       propertyName = "pid";
    }

    
    try
    {
	cimValueRtn = _dispatcher->getProperty(
	    nameSpace,
	    instanceName,
	    propertyName);
    }
    catch (CIMException& e)
    {
	sendError(messageId, "GetProperty", 
	    e.getCode(), CIMStatusCodeToString(e.getCode()));
	return;
    }
    catch (Exception&)
    {
	sendError(messageId, "GetProperty", CIM_ERR_FAILED, 
	    CIMStatusCodeToString(CIM_ERR_FAILED));
	return;
    }

    // Here is where we format the parm part of the response.
    // neet to format the CIMValue here in the response
    //for (Uint32 i = 0; i < classNames.size(); i++)
    //    XmlWriter::appendClassNameElement(body, classNames[i]);
    // We need to put propertyValue on this thing.  Note
    // Lets assume that at this point we have cimvalue with
    // everything defined.  It will only be on the move back
    // through XML that we lose the typing.
//<?xml version="1.0" encoding="utf-8" ?>
//  <CIM CIMVERSION="2.0" DTDVERSION="2.0">
//   <MESSAGE ID="87872" PROTOCOLVERSION="1.0">
//    <SIMPLERSP>
//     <IMETHODRESPONSE NAME="GetProperty">
//      <IRETURNVALUE>
//       <VALUE>6752332</VALUE>
//      </IRETURNVALUE>
//     </IMETHODRESPONSE>
//    </SIMPLERSP>
//   </MESSAGE>
// </CIM>

    Array<Sint8> body;

    cimValueRtn.toXml(body);

	// body contains <value>value</value> to return
    Array<Sint8> message = XmlWriter::formatSimpleRspMessage(
	"GetProperty", messageId, body);

    // cout << "DEBUG CIMServer:IhandleGetPropertyRequest " <<
    //	cimValueRtn.toString() << endl;

    sendMessage(message);
}


//------------------------------------------------------------------------------
//
// ServerHandler::handleSetPropertyRequest()
//
//------------------------------------------------------------------------------

void ServerHandler::handleSetPropertyRequest(
    XmlParser& parser, 
    const String& messageId,
    const String& nameSpace)
{
    //--------------------------------------------------------------------------
    // <!ELEMENT IPARAMVALUE (VALUE|VALUE.ARRAY|VALUE.REFERENCE
    //     |INSTANCENAME|CLASSNAME|QUALIFIER.DECLARATION
    //     |CLASS|INSTANCE|VALUE.NAMEDINSTANCE)?>
    // <!ATTLIST IPARAMVALUE %CIMName;>
    //--------------------------------------------------------------------------

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

    Array<String> classNames;
    
    try
    {
	classNames = _dispatcher->enumerateClassNames(
	    nameSpace,
	    className,
	    deepInheritance);
    }
    catch (CIMException& e)
    {
	sendError(messageId, "EnumerateClassNames", 
	    e.getCode(), CIMStatusCodeToString(e.getCode()));
	return;
    }
    catch (Exception&)
    {
	sendError(messageId, "EnumerateClassNames", CIM_ERR_FAILED, 
	    CIMStatusCodeToString(CIM_ERR_FAILED));
	return;
    }

    Array<Sint8> body;

    for (Uint32 i = 0; i < classNames.size(); i++)
	XmlWriter::appendClassNameElement(body, classNames[i]);

    Array<Sint8> message = XmlWriter::formatSimpleRspMessage(
	"EnumerateClassNames", messageId, body);

    sendMessage(message);
}

//------------------------------------------------------------------------------
//
// ServerHandler::handleAssociatorsRequest()
//
//------------------------------------------------------------------------------

void ServerHandler::handleAssociatorsRequest(
    XmlParser& parser, 
    const String& messageId,
    const String& nameSpace)
{
    // -- Extract the parameters:

    CIMReference objectName;
    String assocClass;
    String resultClass;
    String role;
    String resultRole;
    Boolean includeQualifiers = false;
    Boolean includeClassOrigin = false;
    Array<String> propertyList;

    // ATTN-B: handle the property list!

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

    Array<CIMObjectWithPath> objectWithPathArray;

    try
    {
	objectWithPathArray = _dispatcher->associators(
	    nameSpace,
	    objectName,
	    assocClass,
	    resultClass,
	    role,
	    resultRole,
	    includeQualifiers,
	    includeClassOrigin,
	    propertyList);
    }
    catch (CIMException& e)
    {
	sendError(messageId, "Associators", 
	    e.getCode(), CIMStatusCodeToString(e.getCode()));
	return;
    }
    catch (Exception&)
    {
	sendError(messageId, "Associators", CIM_ERR_FAILED, 
	    CIMStatusCodeToString(CIM_ERR_FAILED));
	return;
    }

    Array<Sint8> body;

    for (Uint32 i = 0; i < objectWithPathArray.size(); i++)
	objectWithPathArray[i].toXml(body);

    Array<Sint8> message = XmlWriter::formatSimpleRspMessage(
	"Associators", messageId, body);

    sendMessage(message);
}

//------------------------------------------------------------------------------
//
// ServerHandler::handleAssociatorNamesRequest()
//
//------------------------------------------------------------------------------

void ServerHandler::handleAssociatorNamesRequest(
    XmlParser& parser, 
    const String& messageId,
    const String& nameSpace)
{
    // -- Extract the parameters:

    CIMReference objectName;
    String assocClass;
    String resultClass;
    String role;
    String resultRole;

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

	XmlReader::expectEndTag(parser, "IPARAMVALUE");
    }

    Array<CIMReference> objectPaths;
    
    try
    {
	objectPaths = _dispatcher->associatorNames(
	    nameSpace,
	    objectName,
	    assocClass,
	    resultClass,
	    role,
	    resultRole);
    }
    catch (CIMException& e)
    {
	sendError(messageId, "AssociatorNames", 
	    e.getCode(), CIMStatusCodeToString(e.getCode()));
	return;
    }
    catch (Exception&)
    {
	sendError(messageId, "AssociatorNames", CIM_ERR_FAILED, 
	    CIMStatusCodeToString(CIM_ERR_FAILED));
	return;
    }

    Array<Sint8> body;

    for (Uint32 i = 0; i < objectPaths.size(); i++)
    {
	body << "<OBJECTPATH>\n";
	objectPaths[i].toXml(body, false);
	body << "</OBJECTPATH>\n";
    }

    Array<Sint8> message = XmlWriter::formatSimpleRspMessage(
	"AssociatorNames", messageId, body);

    sendMessage(message);
}

//------------------------------------------------------------------------------
//
// ServerHandler::handleReferenceNamesRequest()
//
//------------------------------------------------------------------------------

void ServerHandler::handleReferenceNamesRequest(
    XmlParser& parser, 
    const String& messageId,
    const String& nameSpace)
{
    // -- Extract the parameters:

    CIMReference objectName;
    String assocClass;
    String resultClass;
    String role;
    String resultRole;

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

    Array<CIMReference> objectPaths;
    
    try
    {
	objectPaths = _dispatcher->referenceNames(
	    nameSpace,
	    objectName,
	    resultClass,
	    role);
    }
    catch (CIMException& e)
    {
	sendError(messageId, "ReferenceNames", 
	    e.getCode(), CIMStatusCodeToString(e.getCode()));
	return;
    }
    catch (Exception&)
    {
	sendError(messageId, "ReferenceNames", CIM_ERR_FAILED, 
	    CIMStatusCodeToString(CIM_ERR_FAILED));
	return;
    }

    Array<Sint8> body;

    for (Uint32 i = 0; i < objectPaths.size(); i++)
    {
	body << "<OBJECTPATH>\n";
	objectPaths[i].toXml(body, false);
	body << "</OBJECTPATH>\n";
    }

    Array<Sint8> message = XmlWriter::formatSimpleRspMessage(
	"ReferenceNames", messageId, body);

    sendMessage(message);
}

////////////////////////////////////////////////////////////////////////////////
//
// ServerHandlerFactory
//
////////////////////////////////////////////////////////////////////////////////

class ServerHandlerFactory : public ChannelHandlerFactory
{
public:

    ServerHandlerFactory(Dispatcher* dispatcher) : _dispatcher(dispatcher) { }

    virtual ~ServerHandlerFactory() { }

    virtual ChannelHandler* create() { return new ServerHandler(_dispatcher); }

private:

    Dispatcher* _dispatcher;
};

////////////////////////////////////////////////////////////////////////////////
//
// CIMServer
//
////////////////////////////////////////////////////////////////////////////////

const char REPOSITORY[] = "/repository";

//------------------------------------------------------------------------------
//
// CIMServer::CIMServer()
//
//------------------------------------------------------------------------------

CIMServer::CIMServer(
    Selector* selector,
    const String& rootPath) 
    : _rootPath(rootPath), _dieNow(false)
{
    // -- Save the selector or create a new one:

    _selector = selector;

    // -- Create a repository:

    if (!FileSystem::isDirectory(_rootPath))
	throw NoSuchDirectory(_rootPath);

    _repositoryRootPath = rootPath;
    _repositoryRootPath.append(REPOSITORY);

    if (!FileSystem::isDirectory(_repositoryRootPath))
	throw NoSuchDirectory(_repositoryRootPath);

    CIMRepository* repository = new CIMRepository(rootPath + "/repository");

    // -- Create a dispatcher object:

    Dispatcher* dispatcher = new Dispatcher(repository);

    // -- Create a channel factory:

    ServerHandlerFactory* factory = new ServerHandlerFactory(dispatcher);

    // Create an acceptor:

    _acceptor = new TCPChannelAcceptor(factory, _selector);
}

//------------------------------------------------------------------------------
//
// CIMServer::~CIMServer()
//
//------------------------------------------------------------------------------

CIMServer::~CIMServer()
{
    // Note: do not delete the acceptor because it belongs to the Selector
    // which takes care of disposing of it.
}

//------------------------------------------------------------------------------
//
// CIMServer::bind()
//
//------------------------------------------------------------------------------

void CIMServer::bind(const char* address)
{

  // not the best place to build the service url, but it works for now
  // because the address string is accessible  mdday

  if (!_acceptor->bind(address))
    throw CannotBindToAddress(address);
  
}

//------------------------------------------------------------------------------
//
// CIMServer::runForever()
//
//------------------------------------------------------------------------------

void CIMServer::runForever()
{
    if(!_dieNow)
	_selector->select(100);
}

////////////////////////////////////////////////////////////////////////////////

void ServerHandler::handleGetClassRequest(
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

    _dispatcher->enqueue(request);
}

void ServerHandler::handleResponse(CIMGetClassResponseMessage* response)
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

void ServerHandler::handleGetInstanceRequest(
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

    _dispatcher->enqueue(request);
}

void ServerHandler::handleResponse(CIMGetInstanceResponseMessage* response)
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

void ServerHandler::handleDeleteClassRequest(
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

    _dispatcher->enqueue(request);
}

void ServerHandler::handleResponse(CIMDeleteClassResponseMessage* response)
{
    if (response->errorCode != CIM_ERR_SUCCESS)
    {
	sendError(response, "GetClass");
	return;
    }

    Array<Sint8> body;

    Array<Sint8> message = XmlWriter::formatSimpleRspMessage(
	"DeleteClass", response->messageId, body);

    sendMessage(message);
}


void ServerHandler::handleDeleteInstanceRequest(
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

    _dispatcher->enqueue(request);
}

void ServerHandler::handleResponse(CIMDeleteInstanceResponseMessage* response)
{
    if (response->errorCode != CIM_ERR_SUCCESS)
    {
	sendError(response, "GetInstance");
	return;
    }

    Array<Sint8> body;

    Array<Sint8> message = XmlWriter::formatSimpleRspMessage(
	"DeleteInstance", response->messageId, body);

    sendMessage(message);
}

PEGASUS_NAMESPACE_END
