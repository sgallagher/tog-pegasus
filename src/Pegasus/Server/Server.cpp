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
// $Log: Server.cpp,v $
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
#include <ace/SOCK_Acceptor.h>
#include <ace/Acceptor.h>
#include <ace/INET_Addr.h>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/XmlParser.h>
#include <Pegasus/Common/XmlReader.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Repository/Repository.h>
#include <Pegasus/Protocol/Handler.h>
#include <Pegasus/Server/Server.h>
#include <Pegasus/Server/Dispatcher.h>

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

    ServerHandler() : _dispatcher(0)
    {
    }

    virtual int handleMessage();

    int handleGetRequest();

    int handleMethodCall();

    void sendError(
	Uint32 messageId,
	const char* methodName,
	CimException::Code code,
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

    void setDispatcher(Dispatcher* dispatcher)
    {
	_dispatcher = dispatcher;
    }

private:

    Dispatcher* _dispatcher;
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
	    "Server: Pegasus 1.0\r\n"
	    "Content-Length: %d\r\n"
	    "Connection: close\r\n"
	    "Content-Type: text/html\r\n\r\n";

	const char CONTENT[] =
	    "<HTML>\n"
	    "  <HEAD>\n"
	    "    <TITLE>Pegasus 1.0 Server</TITLE>\n"
	    "  </HEAD>\n"
	    "  <BODY>\n"
	    "    <H1>You have reached the Pegasus 1.0 Server</H1>\n"
	    "  </BODY>\n"
	    "</HTML>\n";

	char header[sizeof(HEADER) + 20];
	sprintf(header, HEADER, strlen(CONTENT));

	peer().send_n(header, strlen(header));
	peer().send_n(CONTENT, strlen(CONTENT));
    }
    else
    {
	const char HEADER[] = 
	    "HTTP/1.1 404 Not Found\r\n"
	    "Server: Pegasus 1.0\r\n"
	    /* "Content-Length: %d\r\n" */
	    "Connection: close\r\n"
	    "Content-Type: text/html\r\n\r\n";

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

	peer().send_n(header, strlen(header));
	peer().send_n(CONTENT, strlen(CONTENT));

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
    CimException::Code code,
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

    ConstClassDecl classDecl;
    
    try
    {
	classDecl = _dispatcher->getClass(
	    nameSpace,
	    className,
	    localOnly,
	    includeQualifiers,
	    includeClassOrigin);
    }
    catch (CimException& e)
    {
	sendError(
	    messageId, "GetClass", e.getCode(), e.codeToString(e.getCode()));
	return;
    }
    catch (Exception&)
    {
	sendError(messageId, "GetClass", CimException::FAILED, 
	    CimException::codeToString(CimException::FAILED));
	return;
    }

    Array<Sint8> body;
    classDecl.toXml(body);

    Array<Sint8> message = XmlWriter::formatSimpleRspMessage(
	"GetClass", body);

    sendMessage(message);
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

    Reference instanceName;
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

    ConstInstanceDecl instanceDecl;
    
    try
    {
	instanceDecl = _dispatcher->getInstance(
	    nameSpace,
	    instanceName,
	    includeQualifiers,
	    includeClassOrigin);
    }
    catch (CimException& e)
    {
	sendError(
	    messageId, "GetInstance", e.getCode(), e.codeToString(e.getCode()));
	return;
    }
    catch (Exception& e)
    {
std::cout << e.getMessage() << std::endl;
	sendError(messageId, "GetInstance", CimException::FAILED, 
	    CimException::codeToString(CimException::FAILED));
	return;
    }

    Array<Sint8> body;
    instanceDecl.toXml(body);

    Array<Sint8> message = XmlWriter::formatSimpleRspMessage(
	"GetInstance", body);

    sendMessage(message);
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
    catch (CimException& e)
    {
	sendError(messageId, "EnumerateClassNames", 
	    e.getCode(), e.codeToString(e.getCode()));
	return;
    }
    catch (Exception&)
    {
	sendError(messageId, "EnumerateClassNames", CimException::FAILED, 
	    CimException::codeToString(CimException::FAILED));
	return;
    }

    Array<Sint8> body;

    for (Uint32 i = 0; i < classNames.getSize(); i++)
	XmlWriter::appendClassNameElement(body, classNames[i]);

    Array<Sint8> message = XmlWriter::formatSimpleRspMessage(
	"EnumerateClassNames", body);

    sendMessage(message);
}
//STUB}

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

    Array<Reference> instanceNames;
    
    try
    {
	instanceNames = _dispatcher->enumerateInstanceNames(
	    nameSpace,
	    className);
    }
    catch (CimException& e)
    {
	sendError(messageId, "EnumerateInstanceNames", 
	    e.getCode(), e.codeToString(e.getCode()));
	return;
    }
    catch (Exception&)
    {
	sendError(messageId, "EnumerateInstanceNames", CimException::FAILED, 
	    CimException::codeToString(CimException::FAILED));
	return;
    }

    Array<Sint8> body;

    for (Uint32 i = 0; i < instanceNames.getSize(); i++)
	XmlWriter::appendInstanceNameElement(body, instanceNames[i]);

    Array<Sint8> message = XmlWriter::formatSimpleRspMessage(
	"EnumerateInstanceNames", body);

    sendMessage(message);
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
    catch (CimException& e)
    {
	sendError(messageId, "DeleteQualifier", 
	    e.getCode(), e.codeToString(e.getCode()));
	return;
    }
    catch (Exception&)
    {
	sendError(messageId, "DeleteQualifier", CimException::FAILED, 
	    CimException::codeToString(CimException::FAILED));
	return;
    }

    Array<Sint8> body;

    Array<Sint8> message = XmlWriter::formatSimpleRspMessage(
	"DeleteQualifier", body);

    sendMessage(message);
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

    QualifierDecl qualifierDecl;
    
    try
    {
	qualifierDecl = _dispatcher->getQualifier(nameSpace, qualifierName);
    }
    catch (CimException& e)
    {
	sendError(messageId, "GetQualifier", 
	    e.getCode(), e.codeToString(e.getCode()));
	return;
    }
    catch (Exception&)
    {
	sendError(messageId, "GetQualifier", CimException::FAILED, 
	    CimException::codeToString(CimException::FAILED));
	return;
    }

    Array<Sint8> body;
    qualifierDecl.toXml(body);

    Array<Sint8> message = XmlWriter::formatSimpleRspMessage(
	"GetQualifier", body);

    sendMessage(message);
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
    QualifierDecl qualifierDecl;

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
    catch (CimException& e)
    {
	sendError(messageId, "SetQualifier", 
	    e.getCode(), e.codeToString(e.getCode()));
	return;
    }
    catch (Exception&)
    {
	sendError(messageId, "SetQualifier", CimException::FAILED, 
	    CimException::codeToString(CimException::FAILED));
	return;
    }

    Array<Sint8> body;

    Array<Sint8> message = XmlWriter::formatSimpleRspMessage(
	"SetQualifier", body);

    sendMessage(message);
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

    Array<QualifierDecl> qualifierDecls;
    
    try
    {
	qualifierDecls = _dispatcher->enumerateQualifiers(nameSpace);
    }
    catch (CimException& e)
    {
	sendError(messageId, "EnumerateQualifiers", 
	    e.getCode(), e.codeToString(e.getCode()));
	return;
    }
    catch (Exception&)
    {
	sendError(messageId, "EnumerateQualifiers", CimException::FAILED, 
	    CimException::codeToString(CimException::FAILED));
	return;
    }

    Array<Sint8> body;

    for (Uint32 i = 0; i < qualifierDecls.getSize(); i++)
	qualifierDecls[i].toXml(body);

    Array<Sint8> message = XmlWriter::formatSimpleRspMessage(
	"EnumerateQualifiers", body);

    sendMessage(message);
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

    Array<ClassDecl> classDecls;
    
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
    catch (CimException& e)
    {
	sendError(messageId, "EnumerateClasses", 
	    e.getCode(), e.codeToString(e.getCode()));
	return;
    }
    catch (Exception&)
    {
	sendError(messageId, "EnumerateClasses", CimException::FAILED, 
	    CimException::codeToString(CimException::FAILED));
	return;
    }

    Array<Sint8> body;

    for (Uint32 i = 0; i < classDecls.getSize(); i++)
	classDecls[i].toXml(body);

    Array<Sint8> message = XmlWriter::formatSimpleRspMessage(
	"EnumerateClasses", body);

    sendMessage(message);
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

    ClassDecl classDecl;

    for (const char* name; XmlReader::getIParamValueTag(parser, name);)
    {
	if (strcmp(name, "NewClass") == 0)
	    XmlReader::getClassElement(parser, classDecl);

	XmlReader::expectEndTag(parser, "IPARAMVALUE");
    }

    try
    {
	_dispatcher->createClass(nameSpace, classDecl);
    }
    catch (CimException& e)
    {
	sendError(messageId, "CreateClass", 
	    e.getCode(), e.codeToString(e.getCode()));
	return;
    }
    catch (Exception&)
    {
	sendError(messageId, "CreateClass", CimException::FAILED, 
	    CimException::codeToString(CimException::FAILED));
	return;
    }

    Array<Sint8> body;

    Array<Sint8> message = XmlWriter::formatSimpleRspMessage(
	"CreateClass", body);

    sendMessage(message);
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

    ClassDecl classDecl;

    for (const char* name; XmlReader::getIParamValueTag(parser, name);)
    {
	if (strcmp(name, "ModifiedClass") == 0)
	    XmlReader::getClassElement(parser, classDecl);

	XmlReader::expectEndTag(parser, "IPARAMVALUE");
    }

    try
    {
	_dispatcher->modifyClass(nameSpace, classDecl);
    }
    catch (CimException& e)
    {
	sendError(messageId, "ModifyClass", 
	    e.getCode(), e.codeToString(e.getCode()));
	return;
    }
    catch (Exception&)
    {
	sendError(messageId, "ModifyClass", CimException::FAILED, 
	    CimException::codeToString(CimException::FAILED));
	return;
    }

    Array<Sint8> body;

    Array<Sint8> message = XmlWriter::formatSimpleRspMessage(
	"ModifyClass", body);

    sendMessage(message);
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
    catch (CimException& e)
    {
	sendError(messageId, "DeleteClass", 
	    e.getCode(), e.codeToString(e.getCode()));
	return;
    }
    catch (Exception&)
    {
	sendError(messageId, "DeleteClass", CimException::FAILED, 
	    CimException::codeToString(CimException::FAILED));
	return;
    }

    Array<Sint8> body;

    Array<Sint8> message = XmlWriter::formatSimpleRspMessage(
	"DeleteClass", body);

    sendMessage(message);
}

////////////////////////////////////////////////////////////////////////////////
//
// Acceptor
//
//	This is a one-per-process class.
//
////////////////////////////////////////////////////////////////////////////////

typedef ACE_Acceptor<ServerHandler, ACE_SOCK_ACCEPTOR> AcceptorBase;

class Acceptor : public AcceptorBase
{
public:

    Acceptor(const ACE_INET_Addr& addr, const String& repositoryRoot) :
	AcceptorBase(addr, ACE_Reactor::instance()),
	_repository(repositoryRoot), _dispatcher(0)
    {

    }

    virtual ~Acceptor()
    {
	if (_dispatcher)
	    delete _dispatcher;
    }

    virtual int make_svc_handler(ServerHandler*& handler)
    {
	handler = new ServerHandler;

	_dispatcher = new Dispatcher(&_repository);
	handler->setDispatcher(_dispatcher);

	if (reactor())
	    handler->reactor(reactor());

	return 0;
    }

    Repository _repository;
    Dispatcher* _dispatcher;
};

////////////////////////////////////////////////////////////////////////////////
//
// ServerRep
//
////////////////////////////////////////////////////////////////////////////////

struct ServerRep
{
#ifdef PEGASUS_OS_TYPE_WINDOWS
    ACE_OS_Object_Manager ace_os_object_manager;
    ACE_Object_Manager ace_object_manager;
#endif
    Acceptor* _acceptor;
};

////////////////////////////////////////////////////////////////////////////////
//
// Server
//
////////////////////////////////////////////////////////////////////////////////

const char REPOSITORY[] = "/repository";

//------------------------------------------------------------------------------
//
// Server::Server()
//
//------------------------------------------------------------------------------

Server::Server(const String& rootPath) : _rootPath(rootPath)
{
    _rep = new ServerRep;
    _rep->_acceptor = 0;

    if (!FileSystem::isDirectory(_rootPath))
	throw NoSuchDirectory(_rootPath);

    _repositoryRootPath = rootPath;
    _repositoryRootPath.append(REPOSITORY);

    if (!FileSystem::isDirectory(_repositoryRootPath))
	throw NoSuchDirectory(_repositoryRootPath);
}

//------------------------------------------------------------------------------
//
// Server::~Server()
//
//------------------------------------------------------------------------------

Server::~Server()
{
    delete _rep->_acceptor;
    delete _rep;
}

//------------------------------------------------------------------------------
//
// Server::bind()
//
//------------------------------------------------------------------------------

void Server::bind(Protocol protocol, Uint32 port)
{
    ACE_INET_Addr addr(port);
    _rep->_acceptor = new Acceptor(addr, _rootPath);
}

//------------------------------------------------------------------------------
//
// Server::runForever()
//
//------------------------------------------------------------------------------

void Server::runForever()
{
    for (;;)
	ACE_Reactor::instance()->handle_events();
}

PEGASUS_NAMESPACE_END
