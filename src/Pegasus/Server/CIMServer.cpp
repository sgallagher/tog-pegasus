//BEGIN_LICENSE
//
// Copyright (c) 2000 The Open Group, BMC Software
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
//END_LICENSE
//BEGIN_HISTORY
//
// Author: Mike Brasher
//
// $Log: CIMServer.cpp,v $
// Revision 1.9  2001/04/12 09:57:40  mike
// Post Channel Port to Linux
//
// Revision 1.5  2001/02/20 07:25:57  mike
// Added basic create-instance in repository and in client.
//
// Revision 1.4  2001/02/19 01:47:17  mike
// Renamed names of the form CIMConst to CIMConst.
//
// Revision 1.3  2001/02/18 19:02:18  mike
// Fixed CIM debacle
//
// Revision 1.2  2001/02/18 03:56:01  mike
// Changed more class names (e.g., ConstClassDecl -> CIMConstClass)
//
// Revision 1.1  2001/02/16 02:08:26  mike
// Renamed several classes
//
// Revision 1.4  2001/01/31 08:20:51  mike
// Added dispatcher framework.
// Added enumerateInstanceNames.
//
// Revision 1.3  2001/01/29 07:03:48  mike
// reworked build environment variables
//
// Revision 1.2  2001/01/29 02:19:18  mike
// added primitive provider dispatching
//
// Revision 1.1.1.1  2001/01/14 19:54:04  mike
// Pegasus import
//
//
//END_HISTORY

#include <cassert>
#include <cstdio>
#include <cctype>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/XmlParser.h>
#include <Pegasus/Common/XmlReader.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/TCPChannel.h>
#include <Pegasus/Repository/CIMRepository.h>
#include <Pegasus/Protocol/Handler.h>
#include <Pegasus/Server/CIMServer.h>
#include <Pegasus/Server/Dispatcher.h>

using namespace std;

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
	return Handler::handleOpen(channel);
    }

    virtual int handleMessage();

    int handleGetRequest();

    int handleMethodCall();

    void sendError(
	Uint32 messageId,
	const char* methodName,
	CIMException::Code code,
	const char* description);

    void handleGetClass(
	XmlParser& parser, 
	Uint32 messageId,
	const String& nameSpace);

    void handleGetInstance(
	XmlParser& parser, 
	Uint32 messageId,
	const String& nameSpace);

    //STUB{
    void handleEnumerateClassNames(
	XmlParser& parser, 
	Uint32 messageId,
	const String& nameSpace);
    //STUB}

    void handleCreateInstance(
	XmlParser& parser, 
	Uint32 messageId,
	const String& nameSpace);

    void handleEnumerateInstanceNames(
	XmlParser& parser, 
	Uint32 messageId,
	const String& nameSpace);

    void handleDeleteQualifier(
	XmlParser& parser, 
	Uint32 messageId,
	const String& nameSpace);

    void handleGetQualifier(
	XmlParser& parser, 
	Uint32 messageId,
	const String& nameSpace);

    void handleSetQualifier(
	XmlParser& parser, 
	Uint32 messageId,
	const String& nameSpace);

    void handleEnumerateQualifiers(
	XmlParser& parser, 
	Uint32 messageId,
	const String& nameSpace);

    void handleEnumerateClasses(
	XmlParser& parser, 
	Uint32 messageId,
	const String& nameSpace);

    void handleCreateClass(
	XmlParser& parser, 
	Uint32 messageId,
	const String& nameSpace);

    void handleModifyClass(
	XmlParser& parser, 
	Uint32 messageId,
	const String& nameSpace);

    void handleDeleteClass(
	XmlParser& parser, 
	Uint32 messageId,
	const String& nameSpace);

private:

    Dispatcher* _dispatcher;
    Channel* _channel;
};

//------------------------------------------------------------------------------
//
// ServerHandler::handleMessage()
//
//------------------------------------------------------------------------------

int ServerHandler::handleMessage()
{
    // std::cout << "ServerHandler::handleMessage()" << std::endl;

    Handler::handleMessage();

    const char* m = _message.getData();

    if (strncmp(m, _GET, _GET_LEN) == 0 && isspace(m[_GET_LEN]))
    {
	print();
	return handleGetRequest();
    }
    else if (strncmp(m, _M_POST, _M_POST_LEN) == 0 && isspace(m[_M_POST_LEN]))
    {
	const char* cimOperation = getFieldValueSubString("CIMOperation:");

	if (cimOperation && strcmp(cimOperation, "MethodCall") == 0)
	{
	    try
	    {
		return handleMethodCall();
	    }
	    catch (Exception& e)
	    {
		std::cerr << "Error: " << e.getMessage() << std::endl;
	    }
	    return 0;
	}
    }

    cout << "Error: unknown message: " << m << endl;
    return -1;
}

//------------------------------------------------------------------------------
//
// ServerHandler::handleGetRequest()
//
//------------------------------------------------------------------------------

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
//           <NAMESPACE NAME="cimv20"/>
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

#if 0
cout << "CONTENT[" << (char*)getContent() << "]" << endl;
#endif

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

    Uint32 messageId = 0;
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

    if (strcmp(iMethodCallName, "GetClass") == 0)
	handleGetClass(parser, messageId, nameSpace);
    else if (strcmp(iMethodCallName, "GetInstance") == 0)
	handleGetInstance(parser, messageId, nameSpace);
    //STUB{
    else if (strcmp(iMethodCallName, "EnumerateClassNames") == 0)
	handleEnumerateClassNames(parser, messageId, nameSpace);
    //STUB}
    else if (strcmp(iMethodCallName, "CreateInstance") == 0)
	handleCreateInstance(parser, messageId, nameSpace);
    else if (strcmp(iMethodCallName, "EnumerateInstanceNames") == 0)
	handleEnumerateInstanceNames(parser, messageId, nameSpace);
    else if (strcmp(iMethodCallName, "DeleteQualifier") == 0)
	handleDeleteQualifier(parser, messageId, nameSpace);
    else if (strcmp(iMethodCallName, "GetQualifier") == 0)
	handleGetQualifier(parser, messageId, nameSpace);
    else if (strcmp(iMethodCallName, "SetQualifier") == 0)
	handleSetQualifier(parser, messageId, nameSpace);
    else if (strcmp(iMethodCallName, "EnumerateQualifiers") == 0)
	handleEnumerateQualifiers(parser, messageId, nameSpace);
    else if (strcmp(iMethodCallName, "EnumerateClasses") == 0)
	handleEnumerateClasses(parser, messageId, nameSpace);
    else if (strcmp(iMethodCallName, "CreateClass") == 0)
	handleCreateClass(parser, messageId, nameSpace);
    else if (strcmp(iMethodCallName, "ModifyClass") == 0)
	handleModifyClass(parser, messageId, nameSpace);
    else if (strcmp(iMethodCallName, "DeleteClass") == 0)
	handleDeleteClass(parser, messageId, nameSpace);

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
    Uint32 messageId,
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

    _channel->writeN(message.getData(), message.getSize());
}

//------------------------------------------------------------------------------
//
// ServerHandler::handleGetClass()
//
//------------------------------------------------------------------------------

void ServerHandler::handleGetClass(
    XmlParser& parser, 
    Uint32 messageId,
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
	if (strcmp(name, "ClassName") == 0)
	    XmlReader::getClassNameElement(parser, className, true);
	else if (strcmp(name, "LocalOnly") == 0)
	    XmlReader::getBooleanValueElement(parser, localOnly, true);
	else if (strcmp(name, "IncludeQualifiers") == 0)
	    XmlReader::getBooleanValueElement(parser, includeQualifiers, true);
	else if (strcmp(name, "IncludeClassOrigin") == 0)
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
	"GetClass", body);

    _channel->writeN(message.getData(), message.getSize());
}

//------------------------------------------------------------------------------
//
// ServerHandler::handleGetInstance()
//
//------------------------------------------------------------------------------

void ServerHandler::handleGetInstance(
    XmlParser& parser, 
    Uint32 messageId,
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
	if (strcmp(name, "InstanceName") == 0)
	{
	    String className;
	    Array<KeyBinding> keyBindings;
	    XmlReader::getInstanceNameElement(parser, className, keyBindings);

	    // ATTN: do we need the namespace here?

	    instanceName.set(String(), String(), className, keyBindings);
	}
	else if (strcmp(name, "LocalOnly") == 0)
	    XmlReader::getBooleanValueElement(parser, localOnly, true);
	else if (strcmp(name, "IncludeQualifiers") == 0)
	    XmlReader::getBooleanValueElement(parser, includeQualifiers, true);
	else if (strcmp(name, "IncludeClassOrigin") == 0)
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
    catch (Exception& e)
    {
std::cout << e.getMessage() << std::endl;
	sendError(messageId, "GetInstance", CIMException::FAILED, 
	    CIMException::codeToString(CIMException::FAILED));
	return;
    }

    Array<Sint8> body;
    cimInstance.toXml(body);

    Array<Sint8> message = XmlWriter::formatSimpleRspMessage(
	"GetInstance", body);

    _channel->writeN(message.getData(), message.getSize());
}

//STUB{
//------------------------------------------------------------------------------
//
// ServerHandler::handleEnumerateClassNames()
//
//------------------------------------------------------------------------------

void ServerHandler::handleEnumerateClassNames(
    XmlParser& parser, 
    Uint32 messageId,
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
	if (strcmp(name, "ClassName") == 0)
	    XmlReader::getClassNameElement(parser, className, true);
	else if (strcmp(name, "DeepInheritance") == 0)
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

    for (Uint32 i = 0; i < classNames.getSize(); i++)
	XmlWriter::appendClassNameElement(body, classNames[i]);

    Array<Sint8> message = XmlWriter::formatSimpleRspMessage(
	"EnumerateClassNames", body);

    _channel->writeN(message.getData(), message.getSize());
}
//STUB}

//------------------------------------------------------------------------------
//
// ServerHandler::handleCreateInstance()
//
//------------------------------------------------------------------------------

void ServerHandler::handleCreateInstance(
    XmlParser& parser, 
    Uint32 messageId,
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
	if (strcmp(name, "NewInstance") == 0)
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
	"CreateInstance", body);

    _channel->writeN(message.getData(), message.getSize());
}

//------------------------------------------------------------------------------
//
// ServerHandler::handleEnumerateInstanceNames()
//
//------------------------------------------------------------------------------

void ServerHandler::handleEnumerateInstanceNames(
    XmlParser& parser, 
    Uint32 messageId,
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
	if (strcmp(name, "ClassName") == 0)
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

    for (Uint32 i = 0; i < instanceNames.getSize(); i++)
	XmlWriter::appendInstanceNameElement(body, instanceNames[i]);

    Array<Sint8> message = XmlWriter::formatSimpleRspMessage(
	"EnumerateInstanceNames", body);

    _channel->writeN(message.getData(), message.getSize());
}

//------------------------------------------------------------------------------
//
// ServerHandler::handleDeleteQualifier()
//
//------------------------------------------------------------------------------

void ServerHandler::handleDeleteQualifier(
    XmlParser& parser, 
    Uint32 messageId,
    const String& nameSpace)
{
    String qualifierName;

    for (const char* name; XmlReader::getIParamValueTag(parser, name);)
    {
	if (strcmp(name, "QualifierName") == 0)
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
	"DeleteQualifier", body);

    _channel->writeN(message.getData(), message.getSize());
}

//------------------------------------------------------------------------------
//
// ServerHandler::handleGetQualifier()
//
//------------------------------------------------------------------------------

void ServerHandler::handleGetQualifier(
    XmlParser& parser, 
    Uint32 messageId,
    const String& nameSpace)
{
    String qualifierName;
    Boolean deepInheritance = false;

    for (const char* name; XmlReader::getIParamValueTag(parser, name);)
    {
	if (strcmp(name, "QualifierName") == 0)
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
	"GetQualifier", body);

    _channel->writeN(message.getData(), message.getSize());
}

//------------------------------------------------------------------------------
//
// ServerHandler::handleSetQualifier()
//
//------------------------------------------------------------------------------

void ServerHandler::handleSetQualifier(
    XmlParser& parser, 
    Uint32 messageId,
    const String& nameSpace)
{
    CIMQualifierDecl qualifierDecl;

    for (const char* name; XmlReader::getIParamValueTag(parser, name);)
    {
	if (strcmp(name, "QualifierDeclaration") == 0)
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
	"SetQualifier", body);

    _channel->writeN(message.getData(), message.getSize());
}

//------------------------------------------------------------------------------
//
// ServerHandler::handleEnumerateQualifiers()
//
//------------------------------------------------------------------------------

void ServerHandler::handleEnumerateQualifiers(
    XmlParser& parser, 
    Uint32 messageId,
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

    for (Uint32 i = 0; i < qualifierDecls.getSize(); i++)
	qualifierDecls[i].toXml(body);

    Array<Sint8> message = XmlWriter::formatSimpleRspMessage(
	"EnumerateQualifiers", body);

    _channel->writeN(message.getData(), message.getSize());
}

//------------------------------------------------------------------------------
//
// ServerHandler::handleEnumerateClasses()
//
//------------------------------------------------------------------------------

void ServerHandler::handleEnumerateClasses(
    XmlParser& parser, 
    Uint32 messageId,
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
	if (strcmp(name, "ClassName") == 0)
	    XmlReader::getClassNameElement(parser, className, true);
	else if (strcmp(name, "DeepInheritance") == 0)
	    XmlReader::getBooleanValueElement(parser, deepInheritance, true);
	else if (strcmp(name, "LocalOnly") == 0)
	    XmlReader::getBooleanValueElement(parser, localOnly, true);
	else if (strcmp(name, "IncludeQualifiers") == 0)
	    XmlReader::getBooleanValueElement(parser, includeQualifiers, true);
	else if (strcmp(name, "IncludeClassOrigin") == 0)
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

    for (Uint32 i = 0; i < classDecls.getSize(); i++)
	classDecls[i].toXml(body);

    Array<Sint8> message = XmlWriter::formatSimpleRspMessage(
	"EnumerateClasses", body);

    _channel->writeN(message.getData(), message.getSize());
}

//------------------------------------------------------------------------------
//
// ServerHandler::handleCreateClass()
//
//------------------------------------------------------------------------------

void ServerHandler::handleCreateClass(
    XmlParser& parser, 
    Uint32 messageId,
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
	if (strcmp(name, "NewClass") == 0)
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
	"CreateClass", body);

    _channel->writeN(message.getData(), message.getSize());
}

//------------------------------------------------------------------------------
//
// ServerHandler::handleModifyClass()
//
//------------------------------------------------------------------------------

void ServerHandler::handleModifyClass(
    XmlParser& parser, 
    Uint32 messageId,
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
	if (strcmp(name, "ModifiedClass") == 0)
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
	"ModifyClass", body);

    _channel->writeN(message.getData(), message.getSize());
}

//------------------------------------------------------------------------------
//
// ServerHandler::handleDeleteClass()
//
//------------------------------------------------------------------------------

void ServerHandler::handleDeleteClass(
    XmlParser& parser, 
    Uint32 messageId,
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
	if (strcmp(name, "ClassName") == 0)
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
	"DeleteClass", body);

    _channel->writeN(message.getData(), message.getSize());
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
    : _rootPath(rootPath)
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

    CIMRepository* repository = new CIMRepository(rootPath);

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
    _acceptor->bind(address);
}

//------------------------------------------------------------------------------
//
// CIMServer::runForever()
//
//------------------------------------------------------------------------------

void CIMServer::runForever()
{
    for (;;)
	_selector->select(5000);
}

PEGASUS_NAMESPACE_END
