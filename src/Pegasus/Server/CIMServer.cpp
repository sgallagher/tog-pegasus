//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000 The Open Group, BMC Software, Tivoli Systems, IBM
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
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
#include <Pegasus/Common/XmlParser.h>
#include <Pegasus/Common/XmlReader.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/TCPChannel.h>
#include <Pegasus/Common/Logger.h>
#include <Pegasus/Repository/CIMRepository.h>
#include <Pegasus/Protocol/Handler.h>
#include <Pegasus/Server/CIMServer.h>
#include <Pegasus/Server/Dispatcher.h>
#include <Pegasus/Common/lslp-perl-lib.h>


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
static LSLP_LIB_DAADVERT *slp_das;


////////////////////////////////////////////////////////////////////////////////
//
// ServerHandler
//
//	This is a one-per-connection class.
//
////////////////////////////////////////////////////////////////////////////////

class ServerHandler : public Handler
{
public:

    ServerHandler(Dispatcher* dispatcher) : _dispatcher(dispatcher), _channel(0)
    {

    }

    virtual Boolean handleOpen(Channel* channel)
    {
	_channel = channel;
	DDD(cout << "Handle Open" << endl;)
	return Handler::handleOpen(channel);
    }

    virtual int handleMessage();

    int handleGetRequest();

    int handleMethodCall();

    void sendError(
	const String& messageId,
	const char* methodName,
	CIMException::Code code,
	const char* description);

    void handleGetClass(
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    void handleGetInstance(
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    //STUB{
    void handleEnumerateClassNames(
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);
    //STUB}

    void handleCreateInstance(
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    void handleEnumerateInstanceNames(
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    void handleDeleteQualifier(
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    void handleGetQualifier(
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    void handleSetQualifier(
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    void handleEnumerateQualifiers(
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    void handleEnumerateClasses(
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    void handleCreateClass(
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    void handleModifyClass(
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    void handleDeleteClass(
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    void handleGetProperty(
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    void handleSetProperty(
	XmlParser& parser, 
	const String& messageId,
	const String& nameSpace);

    /** outputN sends the message and optionally
        generates a trace output.
	@param message - The message as an array<sint8>
    */     
    void outputN(Array<Sint8>& message);



private:

    Dispatcher* _dispatcher;
    Channel* _channel;
};

/** outputN added simply to consolidate all of the
    message output to one function so we could control
    output and trace display better.
    Note that this function outputs but does not format
    the message. 
    ATTN: Today there is no choice of output for trace.
    It goes to the console, is accompanied by long header,
    and you cannot chose input or output.
    @Author: KS
*/
void ServerHandler::outputN (Array<Sint8>& message)
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
	print();

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
	handleGetClass(parser, messageId, nameSpace);
    else if (CompareNoCase(iMethodCallName, "GetInstance") == 0)
	handleGetInstance(parser, messageId, nameSpace);
    //STUB{
    else if (CompareNoCase(iMethodCallName, "EnumerateClassNames") == 0)
	handleEnumerateClassNames(parser, messageId, nameSpace);
    //STUB}
    else if (CompareNoCase(iMethodCallName, "CreateInstance") == 0)
	handleCreateInstance(parser, messageId, nameSpace);
    else if (CompareNoCase(iMethodCallName, "EnumerateInstanceNames") == 0)
	handleEnumerateInstanceNames(parser, messageId, nameSpace);
    else if (CompareNoCase(iMethodCallName, "DeleteQualifier") == 0)
	handleDeleteQualifier(parser, messageId, nameSpace);
    else if (CompareNoCase(iMethodCallName, "GetQualifier") == 0)
	handleGetQualifier(parser, messageId, nameSpace);
    else if (CompareNoCase(iMethodCallName, "SetQualifier") == 0)
	handleSetQualifier(parser, messageId, nameSpace);
    else if (CompareNoCase(iMethodCallName, "EnumerateQualifiers") == 0)
	handleEnumerateQualifiers(parser, messageId, nameSpace);
    else if (CompareNoCase(iMethodCallName, "EnumerateClasses") == 0)
	handleEnumerateClasses(parser, messageId, nameSpace);
    else if (CompareNoCase(iMethodCallName, "CreateClass") == 0)
	handleCreateClass(parser, messageId, nameSpace);
    else if (CompareNoCase(iMethodCallName, "ModifyClass") == 0)
	handleModifyClass(parser, messageId, nameSpace);
    else if (CompareNoCase(iMethodCallName, "DeleteClass") == 0)
	handleDeleteClass(parser, messageId, nameSpace);
    else if (CompareNoCase(iMethodCallName, "GetProperty") == 0)
	handleGetProperty(parser, messageId, nameSpace);
    else if (CompareNoCase(iMethodCallName, "SetProperty") == 0)
	handleSetProperty(parser, messageId, nameSpace);



    //--------------------------------------------------------------------------
    // Handle end tags:
    //--------------------------------------------------------------------------

    XmlReader::expectEndTag(parser, "IMETHODCALL");
    XmlReader::expectEndTag(parser, "SIMPLEREQ");
    XmlReader::expectEndTag(parser, "MESSAGE");
    XmlReader::expectEndTag(parser, "CIM");

    return 0;
}

//------------------------------------------------------------------------------
//
// ServerHandler::sendError()
//
//------------------------------------------------------------------------------

void ServerHandler::sendError(
    const String& messageId,
    const char* methodName,
    CIMException::Code code,
    const char* description) 
{
    Array<Sint8> message = XmlWriter::formatMethodResponseHeader(
	XmlWriter::formatMessageElement(
	    messageId,
	    XmlWriter::formatSimpleRspElement(
		XmlWriter::formatIMethodResponseElement(
		    methodName,
		    XmlWriter::formatErrorElement(code, description)))));
    
    outputN(message);
}

//------------------------------------------------------------------------------
//
// ServerHandler::handleGetClass()
//
//------------------------------------------------------------------------------

void ServerHandler::handleGetClass(
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

    CIMConstClass cimClass;
    
    try
    {
	cimClass = _dispatcher->getClass(
	    nameSpace,
	    className,
	    localOnly,
	    includeQualifiers,
	    includeClassOrigin);
    }
    catch (CIMException& e)
    {
	sendError(
	    messageId, "GetClass", e.getCode(), e.codeToString(e.getCode()));
	return;
    }
    catch (Exception&)
    {
	sendError(messageId, "GetClass", CIMException::FAILED, 
	    CIMException::codeToString(CIMException::FAILED));
	return;
    }

    Array<Sint8> body;
    cimClass.toXml(body);

    Array<Sint8> message = XmlWriter::formatSimpleRspMessage(
	"GetClass", messageId, body);

    outputN(message);
}
//------------------------------------------------------------------------------
//
// ServerHandler::handleGetInstance()
//
//------------------------------------------------------------------------------

void ServerHandler::handleGetInstance(
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

    CIMReference instanceName;
    Boolean localOnly = true;
    Boolean includeQualifiers = false;
    Boolean includeClassOrigin = false;

    for (const char* name; XmlReader::getIParamValueTag(parser, name);)
    {
	if (CompareNoCase(name, "InstanceName") == 0)
	{
	    String className;
	    Array<KeyBinding> keyBindings;
	    XmlReader::getInstanceNameElement(parser, className, keyBindings);

	    // ATTN: do we need the namespace here?

	    instanceName.set(String(), String(), className, keyBindings);
	}
	else if (CompareNoCase(name, "LocalOnly") == 0)
	    XmlReader::getBooleanValueElement(parser, localOnly, true);
	else if (CompareNoCase(name, "IncludeQualifiers") == 0)
	    XmlReader::getBooleanValueElement(parser, includeQualifiers, true);
	else if (CompareNoCase(name, "IncludeClassOrigin") == 0)
	    XmlReader::getBooleanValueElement(parser, includeClassOrigin, true);

	XmlReader::expectEndTag(parser, "IPARAMVALUE");
    }

    CIMConstInstance cimInstance;
    
    try
    {
	cimInstance = _dispatcher->getInstance(
	    nameSpace,
	    instanceName,
	    includeQualifiers,
	    includeClassOrigin);
    }
    catch (CIMException& e)
    {
	sendError(
	    messageId, "GetInstance", e.getCode(), e.codeToString(e.getCode()));
	return;
    }
    catch (Exception&)
    {
	sendError(messageId, "GetInstance", CIMException::FAILED, 
	    CIMException::codeToString(CIMException::FAILED));
	return;
    }

    Array<Sint8> body;
    cimInstance.toXml(body);

    Array<Sint8> message = XmlWriter::formatSimpleRspMessage(
	"GetInstance", messageId, body);

    outputN(message);
}

//STUB{
//------------------------------------------------------------------------------
//
// ServerHandler::handleEnumerateClassNames()
//
//------------------------------------------------------------------------------

void ServerHandler::handleEnumerateClassNames(
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
	    e.getCode(), e.codeToString(e.getCode()));
	return;
    }
    catch (Exception&)
    {
	sendError(messageId, "EnumerateClassNames", CIMException::FAILED, 
	    CIMException::codeToString(CIMException::FAILED));
	return;
    }

    Array<Sint8> body;

    for (Uint32 i = 0; i < classNames.size(); i++)
	XmlWriter::appendClassNameElement(body, classNames[i]);

    Array<Sint8> message = XmlWriter::formatSimpleRspMessage(
	"EnumerateClassNames", messageId, body);

    outputN(message);
}
//STUB}

//------------------------------------------------------------------------------
//
// ServerHandler::handleCreateInstance()
//
//------------------------------------------------------------------------------

void ServerHandler::handleCreateInstance(
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
	    e.getCode(), e.codeToString(e.getCode()));
	return;
    }
    catch (Exception& e)
    {
	char* tmp = e.getMessage().allocateCString();

	sendError(messageId, "CreateInstance", CIMException::FAILED, tmp);
	    // CIMException::codeToString(CIMException::FAILED));

	delete [] tmp;
	return;
    }

    Array<Sint8> body;

    Array<Sint8> message = XmlWriter::formatSimpleRspMessage(
	"CreateInstance", messageId, body);

    outputN(message);
}

//------------------------------------------------------------------------------
//
// ServerHandler::handleEnumerateInstanceNames()
//
//------------------------------------------------------------------------------

void ServerHandler::handleEnumerateInstanceNames(
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
	    e.getCode(), e.codeToString(e.getCode()));
	return;
    }
    catch (Exception&)
    {
	sendError(messageId, "EnumerateInstanceNames", CIMException::FAILED, 
	    CIMException::codeToString(CIMException::FAILED));
	return;
    }

    Array<Sint8> body;

    for (Uint32 i = 0; i < instanceNames.size(); i++)
	XmlWriter::appendInstanceNameElement(body, instanceNames[i]);

    Array<Sint8> message = XmlWriter::formatSimpleRspMessage(
	"EnumerateInstanceNames", messageId, body);

    outputN(message);
}

//------------------------------------------------------------------------------
//
// ServerHandler::handleDeleteQualifier()
//
//------------------------------------------------------------------------------

void ServerHandler::handleDeleteQualifier(
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
	    e.getCode(), e.codeToString(e.getCode()));
	return;
    }
    catch (Exception&)
    {
	sendError(messageId, "DeleteQualifier", CIMException::FAILED, 
	    CIMException::codeToString(CIMException::FAILED));
	return;
    }

    Array<Sint8> body;

    Array<Sint8> message = XmlWriter::formatSimpleRspMessage(
	"DeleteQualifier", messageId, body);

    outputN(message);
}

//------------------------------------------------------------------------------
//
// ServerHandler::handleGetQualifier()
//
//------------------------------------------------------------------------------

void ServerHandler::handleGetQualifier(
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
	    e.getCode(), e.codeToString(e.getCode()));
	return;
    }
    catch (Exception&)
    {
	sendError(messageId, "GetQualifier", CIMException::FAILED, 
	    CIMException::codeToString(CIMException::FAILED));
	return;
    }

    Array<Sint8> body;
    qualifierDecl.toXml(body);

    Array<Sint8> message = XmlWriter::formatSimpleRspMessage(
	"GetQualifier", messageId, body);

    outputN(message);
}

//------------------------------------------------------------------------------
//
// ServerHandler::handleSetQualifier()
//
//------------------------------------------------------------------------------

void ServerHandler::handleSetQualifier(
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
	    e.getCode(), e.codeToString(e.getCode()));
	return;
    }
    catch (Exception&)
    {
	sendError(messageId, "SetQualifier", CIMException::FAILED, 
	    CIMException::codeToString(CIMException::FAILED));
	return;
    }

    Array<Sint8> body;

    Array<Sint8> message = XmlWriter::formatSimpleRspMessage(
	"SetQualifier", messageId, body);

    outputN(message);
}

//------------------------------------------------------------------------------
//
// ServerHandler::handleEnumerateQualifiers()
//
//------------------------------------------------------------------------------

void ServerHandler::handleEnumerateQualifiers(
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
	    e.getCode(), e.codeToString(e.getCode()));
	return;
    }
    catch (Exception&)
    {
	sendError(messageId, "EnumerateQualifiers", CIMException::FAILED, 
	    CIMException::codeToString(CIMException::FAILED));
	return;
    }

    Array<Sint8> body;

    for (Uint32 i = 0; i < qualifierDecls.size(); i++)
	qualifierDecls[i].toXml(body);

    Array<Sint8> message = XmlWriter::formatSimpleRspMessage(
	"EnumerateQualifiers", messageId, body);

    outputN(message);
}

//------------------------------------------------------------------------------
//
// ServerHandler::handleEnumerateClasses()
//
//------------------------------------------------------------------------------

void ServerHandler::handleEnumerateClasses(
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
	    e.getCode(), e.codeToString(e.getCode()));
	return;
    }
    catch (Exception&)
    {
	sendError(messageId, "EnumerateClasses", CIMException::FAILED, 
	    CIMException::codeToString(CIMException::FAILED));
	return;
    }

    Array<Sint8> body;

    for (Uint32 i = 0; i < classDecls.size(); i++)
	classDecls[i].toXml(body);

    Array<Sint8> message = XmlWriter::formatSimpleRspMessage(
	"EnumerateClasses", messageId, body);

    outputN(message);
}

//------------------------------------------------------------------------------
//
// ServerHandler::handleCreateClass()
//
//------------------------------------------------------------------------------

void ServerHandler::handleCreateClass(
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
	    e.getCode(), e.codeToString(e.getCode()));
	return;
    }
    catch (Exception&)
    {
	sendError(messageId, "CreateClass", CIMException::FAILED, 
	    CIMException::codeToString(CIMException::FAILED));
	return;
    }

    Array<Sint8> body;

    Array<Sint8> message = XmlWriter::formatSimpleRspMessage(
	"CreateClass", messageId, body);

    outputN(message);
}

//------------------------------------------------------------------------------
//
// ServerHandler::handleModifyClass()
//
//------------------------------------------------------------------------------

void ServerHandler::handleModifyClass(
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
	    e.getCode(), e.codeToString(e.getCode()));
	return;
    }
    catch (Exception&)
    {
	sendError(messageId, "ModifyClass", CIMException::FAILED, 
	    CIMException::codeToString(CIMException::FAILED));
	return;
    }

    Array<Sint8> body;

    Array<Sint8> message = XmlWriter::formatSimpleRspMessage(
	"ModifyClass", messageId, body);

    outputN(message);
}

//------------------------------------------------------------------------------
//
// ServerHandler::handleDeleteClass()
//
//------------------------------------------------------------------------------

void ServerHandler::handleDeleteClass(
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

    try
    {
	_dispatcher->deleteClass(nameSpace, className);
    }
    catch (CIMException& e)
    {
	sendError(messageId, "DeleteClass", 
	    e.getCode(), e.codeToString(e.getCode()));
	return;
    }
    catch (Exception&)
    {
	sendError(messageId, "DeleteClass", CIMException::FAILED, 
	    CIMException::codeToString(CIMException::FAILED));
	return;
    }

    Array<Sint8> body;

    Array<Sint8> message = XmlWriter::formatSimpleRspMessage(
	"DeleteClass", messageId, body);

    outputN(message);
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

void ServerHandler::handleGetProperty(
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
	    e.getCode(), e.codeToString(e.getCode()));
	return;
    }
    catch (Exception&)
    {
	sendError(messageId, "GetProperty", CIMException::FAILED, 
	    CIMException::codeToString(CIMException::FAILED));
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

    // cout << "DEBUG CIMServer:IhandleGetProperty " <<
    //	cimValueRtn.toString() << endl;

    outputN(message);
}


//------------------------------------------------------------------------------
//
// ServerHandler::handleSetProperty()
//
//------------------------------------------------------------------------------

void ServerHandler::handleSetProperty(
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
	    e.getCode(), e.codeToString(e.getCode()));
	return;
    }
    catch (Exception&)
    {
	sendError(messageId, "EnumerateClassNames", CIMException::FAILED, 
	    CIMException::codeToString(CIMException::FAILED));
	return;
    }

    Array<Sint8> body;

    for (Uint32 i = 0; i < classNames.size(); i++)
	XmlWriter::appendClassNameElement(body, classNames[i]);

    Array<Sint8> message = XmlWriter::formatSimpleRspMessage(
	"EnumerateClassNames", messageId, body);

    outputN(message);
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
  : _rootPath(rootPath), _dieNow(0), _useSLP(1)

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

  if(_useSLP == true) {
    char *host_name = lslp_lib_get_host_name();
    _serviceURL.assign("service:cim.pegasus://");
    _serviceURL += host_name;
    _serviceURL += ":";
    _serviceURL += address;
  }

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

  time_t last = 0;
  char *url;
  if(! (_serviceURL.size() ) || (NULL ==  (url = _serviceURL.allocateCString())))
    _useSLP = false;
  
  while( ! _dieNow ) 
    {
      _selector->select(5000);
      if(_useSLP  ) 
	{
	  int success, failure;
	  if(  (time(NULL) - last ) > 60 ) 
	    {
	      lslp_lib_srv_reg_all("pegasus_cim_server", 
				   url, 
				   "(namespace=root/cimv20)", 
				   "service:cim.pegasus", 
				   "DEFAULT", 
				   70, 
				   &success, 
				   &failure);
	      time(&last);
	    }
	}
    }
}

PEGASUS_NAMESPACE_END
