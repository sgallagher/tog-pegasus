//BEGIN_LICENSE
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
//END_LICENSE
//BEGIN_HISTORY
//
// Author:
//
// $Log: Client.cpp,v $
// Revision 1.4  2001/02/11 05:45:33  mike
// Added case insensitive logic for files in Repository
//
// Revision 1.3  2001/01/31 08:20:51  mike
// Added dispatcher framework.
// Added enumerateInstanceNames.
//
// Revision 1.2  2001/01/29 02:19:57  mike
// added getInstance stub
//
// Revision 1.1.1.1  2001/01/14 19:50:30  mike
// Pegasus import
//
//
//END_HISTORY

#include <ace/SOCK_Connector.h>
#include <ace/INET_Addr.h>
#include <ace/Reactor.h>
#include <ace/WFMO_Reactor.h>
#include <Pegasus/Protocol/Handler.h>
#include <Pegasus/Common/XmlParser.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/XmlReader.h>
#include "Client.h"

PEGASUS_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
//
// GetClassResult
//
////////////////////////////////////////////////////////////////////////////////

struct GetClassResult
{
    CimException::Code code;
    ClassDecl classDecl;
};

////////////////////////////////////////////////////////////////////////////////
//
// GetInstanceResult
//
////////////////////////////////////////////////////////////////////////////////

struct GetInstanceResult
{
    CimException::Code code;
    InstanceDecl instanceDecl;
};

//STUB{
////////////////////////////////////////////////////////////////////////////////
//
// EnumerateClassNamesResult
//
////////////////////////////////////////////////////////////////////////////////

struct EnumerateClassNamesResult
{
    CimException::Code code;
    Array<String> classNames;
};
//STUB}

////////////////////////////////////////////////////////////////////////////////
//
// EnumerateInstanceNamesResult
//
////////////////////////////////////////////////////////////////////////////////

struct EnumerateInstanceNamesResult
{
    CimException::Code code;
    Array<Reference> instanceNames;
};

////////////////////////////////////////////////////////////////////////////////
//
// DeleteQualifierResult
//
////////////////////////////////////////////////////////////////////////////////

struct DeleteQualifierResult
{
    CimException::Code code;
};

////////////////////////////////////////////////////////////////////////////////
//
// GetQualifierResult
//
////////////////////////////////////////////////////////////////////////////////

struct GetQualifierResult
{
    CimException::Code code;
    QualifierDecl qualifierDecl;
};

////////////////////////////////////////////////////////////////////////////////
//
// SetQualifierResult
//
////////////////////////////////////////////////////////////////////////////////

struct SetQualifierResult
{
    CimException::Code code;
};

////////////////////////////////////////////////////////////////////////////////
//
// EnumerateQualifiersResult
//
////////////////////////////////////////////////////////////////////////////////

struct EnumerateQualifiersResult
{
    CimException::Code code;
    Array<QualifierDecl> qualifierDecls;
};

////////////////////////////////////////////////////////////////////////////////
//
// EnumerateClassesResult
//
////////////////////////////////////////////////////////////////////////////////

struct EnumerateClassesResult
{
    CimException::Code code;
    Array<ClassDecl> classDecls;
};

////////////////////////////////////////////////////////////////////////////////
//
// CreateClassResult
//
////////////////////////////////////////////////////////////////////////////////

struct CreateClassResult
{
    CimException::Code code;
};

////////////////////////////////////////////////////////////////////////////////
//
// ModifyClassResult
//
////////////////////////////////////////////////////////////////////////////////

struct ModifyClassResult
{
    CimException::Code code;
};

////////////////////////////////////////////////////////////////////////////////
//
// DeleteClassResult
//
////////////////////////////////////////////////////////////////////////////////

struct DeleteClassResult
{
    CimException::Code code;
};

////////////////////////////////////////////////////////////////////////////////
//
// ClientHandler
//
////////////////////////////////////////////////////////////////////////////////

class ClientHandler : public Handler 
{
public:

    ClientHandler()
    {
	cerr << __FILE__ << "(" << __LINE__ << "): internal error" << endl;
	exit(1);
    }

    ClientHandler(const ACE_INET_Addr& addr_) 
	: addr(addr_), _getClassResult(0), _blocked(false)
    {

    }

    const char* getHostName() const;

    virtual int handleMessage();

    int handleMethodResponse();

    int handleGetClassResponse(XmlParser& parser, Uint32 messageId);

    int handleGetInstanceResponse(XmlParser& parser, Uint32 messageId);

    //STUB{
    int handleEnumerateClassNamesResponse(XmlParser& parser, Uint32 messageId);
    //STUB}

    int handleEnumerateInstanceNamesResponse(
	XmlParser& parser, 
	Uint32 messageId);

    int handleDeleteQualifierResponse(XmlParser& parser, Uint32 messageId);

    int handleGetQualifierResponse(XmlParser& parser, Uint32 messageId);

    int handleSetQualifierResponse(XmlParser& parser, Uint32 messageId);

    int handleEnumerateQualifiersResponse(XmlParser& parser, Uint32 messageId);

    int handleEnumerateClassesResponse(XmlParser& parser, Uint32 messageId);

    int handleCreateClassResponse(XmlParser& parser, Uint32 messageId);

    int handleModifyClassResponse(XmlParser& parser, Uint32 messageId);

    int handleDeleteClassResponse(XmlParser& parser, Uint32 messageId);


    Boolean waitForResponse(Uint32 timeOutMilliseconds);

    ACE_INET_Addr addr;

    union
    {
	GetClassResult* _getClassResult;
	GetInstanceResult* _getInstanceResult;
	//STUB{
	EnumerateClassNamesResult* _enumerateClassNamesResult;
	//STUB}
	EnumerateInstanceNamesResult* _enumerateInstanceNamesResult;
	DeleteQualifierResult* _deleteQualifierResult;
	GetQualifierResult* _getQualifierResult;
	SetQualifierResult* _setQualifierResult;
	EnumerateQualifiersResult* _enumerateQualifiersResult;
	EnumerateClassesResult* _enumerateClassesResult;
	CreateClassResult* _createClassResult;
	ModifyClassResult* _modifyClassResult;
	DeleteClassResult* _deleteClassResult;
    };

private:
    Boolean _blocked;
    char _hostNameTmp[256];
};

//------------------------------------------------------------------------------
//
// ClientHandler::ClientHandler()
//
//------------------------------------------------------------------------------

const char* ClientHandler::getHostName() const 
{
    ((char*)_hostNameTmp)[0] = '\0';
    addr.get_host_name((char*)_hostNameTmp, sizeof(_hostNameTmp)); 
    return _hostNameTmp;
}

//------------------------------------------------------------------------------
//
// ClientHandler::handleMessage()
//
//------------------------------------------------------------------------------

int ClientHandler::handleMessage()
{
    // cout << "ClientHandler::handleMessage()" << endl;

    Handler::handleMessage();

    const char* cimOperation = getFieldValueSubString("CIMOperation:");

    if (cimOperation && strcmp(cimOperation, "MethodResponse") == 0)
    {
	try
	{
	    handleMethodResponse();
	}
	catch (Exception& e)
	{
	    std::cerr << "Error: " << e.getMessage() << std::endl;
	}
    }

    return 0;
}

//------------------------------------------------------------------------------
//
// ClientHandler::handleMethodResponse()
//
//     <?xml version="1.0" encoding="utf-8" ?>
//     <CIM CIMVERSION="2.0" DTDVERSION="2.0">
//       <MESSAGE ID="87872" PROTOCOLVERSION="1.0">
//         <SIMPLERSP>
//           <IMETHODRESPONSE NAME="GetClass">
//           ...
//           </IMETHODRESPONSE>
//         </SIMPLERSP>
//       </MESSAGE>
//     </CIM>
//
//------------------------------------------------------------------------------

int ClientHandler::handleMethodResponse()
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
    // <!ELEMENT SIMPLERSP (IMETHODRESPONSE|METHODRESPONSE)>
    //--------------------------------------------------------------------------

    XmlReader::expectStartTag(parser, entry, "SIMPLERSP");

    //--------------------------------------------------------------------------
    // <!ELEMENT IMETHODRESPONSE (ERROR|IRETURNVALUE?)>
    // <!ATTLIST IMETHODRESPONSE %CIMName;>
    //--------------------------------------------------------------------------

    const char* iMethodResponseName = 0;

    if (!XmlReader::getIMethodResponseStartTag(parser, iMethodResponseName))
	throw XmlValidationError(parser.getLine(), 
	    "expected IMETHODRESPONSE element");

    //--------------------------------------------------------------------------
    // Dispatch the method:
    //--------------------------------------------------------------------------

    if (strcmp(iMethodResponseName, "GetClass") == 0)
	handleGetClassResponse(parser, messageId);
    else if (strcmp(iMethodResponseName, "GetInstance") == 0)
	handleGetInstanceResponse(parser, messageId);
    //STUB{
    else if (strcmp(iMethodResponseName, "EnumerateClassNames") == 0)
	handleEnumerateClassNamesResponse(parser, messageId);
    //STUB}
    else if (strcmp(iMethodResponseName, "EnumerateInstanceNames") == 0)
	handleEnumerateInstanceNamesResponse(parser, messageId);
    else if (strcmp(iMethodResponseName, "DeleteQualifier") == 0)
	handleDeleteQualifierResponse(parser, messageId);
    else if (strcmp(iMethodResponseName, "GetQualifier") == 0)
	handleGetQualifierResponse(parser, messageId);
    else if (strcmp(iMethodResponseName, "SetQualifier") == 0)
	handleSetQualifierResponse(parser, messageId);
    else if (strcmp(iMethodResponseName, "EnumerateQualifiers") == 0)
	handleEnumerateQualifiersResponse(parser, messageId);
    else if (strcmp(iMethodResponseName, "EnumerateClasses") == 0)
	handleEnumerateClassesResponse(parser, messageId);
    else if (strcmp(iMethodResponseName, "CreateClass") == 0)
	handleCreateClassResponse(parser, messageId);
    else if (strcmp(iMethodResponseName, "ModifyClass") == 0)
	handleModifyClassResponse(parser, messageId);
    else if (strcmp(iMethodResponseName, "DeleteClass") == 0)
	handleDeleteClassResponse(parser, messageId);

    //BOOKMARK

    //--------------------------------------------------------------------------
    // Handle end tags:
    //--------------------------------------------------------------------------

    XmlReader::expectEndTag(parser, "IMETHODRESPONSE");
    XmlReader::expectEndTag(parser, "SIMPLERSP");
    XmlReader::expectEndTag(parser, "MESSAGE");
    XmlReader::expectEndTag(parser, "CIM");

    return 0;
}

//------------------------------------------------------------------------------
//
// ClientHandler::handleGetClassResponse()
//
//     Expect (ERROR|IRETURNVALUE).
//
//------------------------------------------------------------------------------

int ClientHandler::handleGetClassResponse(XmlParser& parser, Uint32 messageId) 
{
    XmlEntry entry;
    CimException::Code code;
    const char* description = 0;

    if (XmlReader::getErrorElement(parser, code, description))
    {
	_getClassResult = new GetClassResult;
	_getClassResult->code = code;
	_blocked = false;
	return 0;
    }
    else if (XmlReader::testStartTag(parser, entry, "IRETURNVALUE"))
    {
	ClassDecl classDecl;

	if (!XmlReader::getClassElement(parser, classDecl))
	    throw XmlValidationError(parser.getLine(),"expected CLASS element");

	XmlReader::testEndTag(parser, "IRETURNVALUE");

	_getClassResult = new GetClassResult;
	_getClassResult->code = CimException::SUCCESS;
	_getClassResult->classDecl = classDecl;
	_blocked = false;
	return 0;
    }
    else
    {
	throw XmlValidationError(parser.getLine(),
	    "expected ERROR or IRETURNVALUE element");
    }

    return 0;
}

//------------------------------------------------------------------------------
//
// ClientHandler::handleGetInstanceResponse()
//
//     Expect (ERROR|IRETURNVALUE).
//
//------------------------------------------------------------------------------

int ClientHandler::handleGetInstanceResponse(
    XmlParser& parser, 
    Uint32 messageId) 
{
    XmlEntry entry;
    CimException::Code code;
    const char* description = 0;

    if (XmlReader::getErrorElement(parser, code, description))
    {
	_getInstanceResult = new GetInstanceResult;
	_getInstanceResult->code = code;
	_blocked = false;
	return 0;
    }
    else if (XmlReader::testStartTag(parser, entry, "IRETURNVALUE"))
    {
	InstanceDecl instanceDecl;

	if (!XmlReader::getInstanceElement(parser, instanceDecl))
	{
	    throw XmlValidationError(
		parser.getLine(), "expected INSTANCE element");
	}

	XmlReader::testEndTag(parser, "IRETURNVALUE");

	_getInstanceResult = new GetInstanceResult;
	_getInstanceResult->code = CimException::SUCCESS;
	_getInstanceResult->instanceDecl = instanceDecl;
	_blocked = false;
	return 0;
    }
    else
    {
	throw XmlValidationError(parser.getLine(),
	    "expected ERROR or IRETURNVALUE element");
    }

    return 0;
}

//STUB{
//------------------------------------------------------------------------------
//
// ClientHandler::handleEnumerateClassNamesResponse()
//
//     Expect (ERROR|IRETURNVALUE).
//
//------------------------------------------------------------------------------

int ClientHandler::handleEnumerateClassNamesResponse(
    XmlParser& parser, 
    Uint32 messageId) 
{
    XmlEntry entry;
    CimException::Code code;
    const char* description = 0;

    if (XmlReader::getErrorElement(parser, code, description))
    {
	_enumerateClassNamesResult = new EnumerateClassNamesResult;
	_enumerateClassNamesResult->code = code;
	_blocked = false;
	return 0;
    }
    else if (XmlReader::testStartTag(parser, entry, "IRETURNVALUE"))
    {
	Array<String> classNames;
	String className;

	while (XmlReader::getClassNameElement(parser, className, false))
	    classNames.append(className);

	XmlReader::testEndTag(parser, "IRETURNVALUE");

	_enumerateClassNamesResult = new EnumerateClassNamesResult;
	_enumerateClassNamesResult->code = CimException::SUCCESS;
	_enumerateClassNamesResult->classNames = classNames;
	_blocked = false;
	return 0;
    }
    else
    {
	throw XmlValidationError(parser.getLine(),
	    "expected ERROR or IRETURNVALUE element");
    }

    return 0;
}
//STUB}

//------------------------------------------------------------------------------
//
// ClientHandler::handleEnumerateInstanceNamesResponse()
//
//     Expect (ERROR|IRETURNVALUE).
//
//------------------------------------------------------------------------------

int ClientHandler::handleEnumerateInstanceNamesResponse(
    XmlParser& parser, 
    Uint32 messageId) 
{
    XmlEntry entry;
    CimException::Code code;
    const char* description = 0;

    if (XmlReader::getErrorElement(parser, code, description))
    {
	_enumerateInstanceNamesResult = new EnumerateInstanceNamesResult;
	_enumerateInstanceNamesResult->code = code;
	_blocked = false;
	return 0;
    }
    else if (XmlReader::testStartTag(parser, entry, "IRETURNVALUE"))
    {
	Array<Reference> instanceNames;
	String className;
	Array<KeyBinding> keyBindings;

	while (XmlReader::getInstanceNameElement(
	    parser, className, keyBindings))
	{
	    Reference r(
		String::EMPTY,
		String::EMPTY,
		className, 
		keyBindings);
	    instanceNames.append(r);
	}

	XmlReader::testEndTag(parser, "IRETURNVALUE");

	_enumerateInstanceNamesResult = new EnumerateInstanceNamesResult;
	_enumerateInstanceNamesResult->code = CimException::SUCCESS;
	_enumerateInstanceNamesResult->instanceNames = instanceNames;
	_blocked = false;
	return 0;
    }
    else
    {
	throw XmlValidationError(parser.getLine(),
	    "expected ERROR or IRETURNVALUE element");
    }

    return 0;
}

//------------------------------------------------------------------------------
//
// ClientHandler::handleDeleteQualifierResponse()
//
//     Expect (ERROR|IRETURNVALUE).
//
//------------------------------------------------------------------------------

int ClientHandler::handleDeleteQualifierResponse(
    XmlParser& parser, 
    Uint32 messageId) 
{
    XmlEntry entry;
    CimException::Code code;
    const char* description = 0;

    if (XmlReader::getErrorElement(parser, code, description))
    {
	_deleteQualifierResult = new DeleteQualifierResult;
	_deleteQualifierResult->code = code;
	_blocked = false;
	return 0;
    }
    else if (XmlReader::testStartTag(parser, entry, "IRETURNVALUE"))
    {
	XmlReader::testEndTag(parser, "IRETURNVALUE");

	_deleteQualifierResult = new DeleteQualifierResult;
	_deleteQualifierResult->code = CimException::SUCCESS;
	_blocked = false;
	return 0;
    }
    else
    {
	throw XmlValidationError(parser.getLine(),
	    "expected ERROR or IRETURNVALUE element");
    }

    return 0;
}

//------------------------------------------------------------------------------
//
// ClientHandler::handleGetQualifierResponse()
//
//     Expect (ERROR|IRETURNVALUE).
//
//------------------------------------------------------------------------------

int ClientHandler::handleGetQualifierResponse(
    XmlParser& parser, 
    Uint32 messageId) 
{
    XmlEntry entry;
    CimException::Code code;
    const char* description = 0;

    if (XmlReader::getErrorElement(parser, code, description))
    {
	_getQualifierResult = new GetQualifierResult;
	_getQualifierResult->code = code;
	_blocked = false;
	return 0;
    }
    else if (XmlReader::testStartTag(parser, entry, "IRETURNVALUE"))
    {
	QualifierDecl qualifierDecl;
	XmlReader::getQualifierDeclElement(parser, qualifierDecl);

	XmlReader::testEndTag(parser, "IRETURNVALUE");

	_getQualifierResult = new GetQualifierResult;
	_getQualifierResult->code = CimException::SUCCESS;
	_getQualifierResult->qualifierDecl = qualifierDecl;
	_blocked = false;
	return 0;
    }
    else
    {
	throw XmlValidationError(parser.getLine(),
	    "expected ERROR or IRETURNVALUE element");
    }

    return 0;
}

//------------------------------------------------------------------------------
//
// ClientHandler::handleSetQualifierResponse()
//
//     Expect (ERROR|IRETURNVALUE).
//
//------------------------------------------------------------------------------

int ClientHandler::handleSetQualifierResponse(
    XmlParser& parser, 
    Uint32 messageId) 
{
    XmlEntry entry;
    CimException::Code code;
    const char* description = 0;

    if (XmlReader::getErrorElement(parser, code, description))
    {
	_setQualifierResult = new SetQualifierResult;
	_setQualifierResult->code = code;
	_blocked = false;
	return 0;
    }
    else if (XmlReader::testStartTag(parser, entry, "IRETURNVALUE"))
    {
	XmlReader::testEndTag(parser, "IRETURNVALUE");

	_setQualifierResult = new SetQualifierResult;
	_setQualifierResult->code = CimException::SUCCESS;
	_blocked = false;
	return 0;
    }
    else
    {
	throw XmlValidationError(parser.getLine(),
	    "expected ERROR or IRETURNVALUE element");
    }

    return 0;
}

//------------------------------------------------------------------------------
//
// ClientHandler::handleEnumerateQualifiersResponse()
//
//     Expect (ERROR|IRETURNVALUE).
//
//------------------------------------------------------------------------------

int ClientHandler::handleEnumerateQualifiersResponse(
    XmlParser& parser, 
    Uint32 messageId) 
{
    XmlEntry entry;
    CimException::Code code;
    const char* description = 0;

    if (XmlReader::getErrorElement(parser, code, description))
    {
	_enumerateQualifiersResult = new EnumerateQualifiersResult;
	_enumerateQualifiersResult->code = code;
	_blocked = false;
	return 0;
    }
    else if (XmlReader::testStartTag(parser, entry, "IRETURNVALUE"))
    {
	Array<QualifierDecl> qualifierDecls;
	QualifierDecl qualifierDecl;

	while (XmlReader::getQualifierDeclElement(parser, qualifierDecl))
	    qualifierDecls.append(qualifierDecl);

	XmlReader::testEndTag(parser, "IRETURNVALUE");

	_enumerateQualifiersResult = new EnumerateQualifiersResult;
	_enumerateQualifiersResult->code = CimException::SUCCESS;
	_enumerateQualifiersResult->qualifierDecls = qualifierDecls;
	_blocked = false;
	return 0;
    }
    else
    {
	throw XmlValidationError(parser.getLine(),
	    "expected ERROR or IRETURNVALUE element");
    }

    return 0;
}

//------------------------------------------------------------------------------
//
// ClientHandler::handleEnumerateClassesResponse()
//
//     Expect (ERROR|IRETURNVALUE).
//
//------------------------------------------------------------------------------

int ClientHandler::handleEnumerateClassesResponse(
    XmlParser& parser, 
    Uint32 messageId) 
{
    XmlEntry entry;
    CimException::Code code;
    const char* description = 0;

    if (XmlReader::getErrorElement(parser, code, description))
    {
	_enumerateClassesResult = new EnumerateClassesResult;
	_enumerateClassesResult->code = code;
	_blocked = false;
	return 0;
    }
    else if (XmlReader::testStartTag(parser, entry, "IRETURNVALUE"))
    {
	Array<ClassDecl> classDecls;
	ClassDecl classDecl;

	while (XmlReader::getClassElement(parser, classDecl))
	    classDecls.append(classDecl);

	XmlReader::testEndTag(parser, "IRETURNVALUE");

	_enumerateClassesResult = new EnumerateClassesResult;
	_enumerateClassesResult->code = CimException::SUCCESS;
	_enumerateClassesResult->classDecls = classDecls;
	_blocked = false;
	return 0;
    }
    else
    {
	throw XmlValidationError(parser.getLine(),
	    "expected ERROR or IRETURNVALUE element");
    }

    return 0;
}

//------------------------------------------------------------------------------
//
// ClientHandler::handleCreateClassResponse()
//
//     Expect (ERROR|IRETURNVALUE).
//
//------------------------------------------------------------------------------

int ClientHandler::handleCreateClassResponse(
    XmlParser& parser, 
    Uint32 messageId) 
{
    XmlEntry entry;
    CimException::Code code;
    const char* description = 0;

    if (XmlReader::getErrorElement(parser, code, description))
    {
	_createClassResult = new CreateClassResult;
	_createClassResult->code = code;
	_blocked = false;
	return 0;
    }
    else if (XmlReader::testStartTag(parser, entry, "IRETURNVALUE"))
    {
	XmlReader::testEndTag(parser, "IRETURNVALUE");

	_createClassResult = new CreateClassResult;
	_createClassResult->code = CimException::SUCCESS;
	_blocked = false;
	return 0;
    }
    else
    {
	throw XmlValidationError(parser.getLine(),
	    "expected ERROR or IRETURNVALUE element");
    }

    return 0;
}

//------------------------------------------------------------------------------
//
// ClientHandler::handleModifyClassResponse()
//
//     Expect (ERROR|IRETURNVALUE).
//
//------------------------------------------------------------------------------

int ClientHandler::handleModifyClassResponse(
    XmlParser& parser, 
    Uint32 messageId) 
{
    XmlEntry entry;
    CimException::Code code;
    const char* description = 0;

    if (XmlReader::getErrorElement(parser, code, description))
    {
	_modifyClassResult = new ModifyClassResult;
	_modifyClassResult->code = code;
	_blocked = false;
	return 0;
    }
    else if (XmlReader::testStartTag(parser, entry, "IRETURNVALUE"))
    {
	XmlReader::testEndTag(parser, "IRETURNVALUE");

	_modifyClassResult = new ModifyClassResult;
	_modifyClassResult->code = CimException::SUCCESS;
	_blocked = false;
	return 0;
    }
    else
    {
	throw XmlValidationError(parser.getLine(),
	    "expected ERROR or IRETURNVALUE element");
    }

    return 0;
}

//------------------------------------------------------------------------------
//
// ClientHandler::handleDeleteClassResponse()
//
//     Expect (ERROR|IRETURNVALUE).
//
//------------------------------------------------------------------------------

int ClientHandler::handleDeleteClassResponse(
    XmlParser& parser, 
    Uint32 messageId) 
{
    XmlEntry entry;
    CimException::Code code;
    const char* description = 0;

    if (XmlReader::getErrorElement(parser, code, description))
    {
	_deleteClassResult = new DeleteClassResult;
	_deleteClassResult->code = code;
	_blocked = false;
	return 0;
    }
    else if (XmlReader::testStartTag(parser, entry, "IRETURNVALUE"))
    {
	XmlReader::testEndTag(parser, "IRETURNVALUE");

	_deleteClassResult = new DeleteClassResult;
	_deleteClassResult->code = CimException::SUCCESS;
	_blocked = false;
	return 0;
    }
    else
    {
	throw XmlValidationError(parser.getLine(),
	    "expected ERROR or IRETURNVALUE element");
    }

    return 0;
}

//------------------------------------------------------------------------------
//
// ClientHandler::waitForResponse()
//
//------------------------------------------------------------------------------

Boolean ClientHandler::waitForResponse(Uint32 timeOutMilliseconds)
{
    _blocked = true;
    long rem = long(timeOutMilliseconds);

    while (_blocked)
    {
	ACE_Time_Value start = ACE_OS::gettimeofday();

	ACE_Time_Value tv;
	tv.msec(rem);

	ACE_Reactor::instance()->handle_events(&tv);

	ACE_Time_Value stop = ACE_OS::gettimeofday();

	long diff = stop.msec() - start.msec();

	if (diff >= rem)
	    break;

	rem -= diff;
    }

    Boolean gotResponse = !_blocked;
    _blocked = false;
    return gotResponse;
}

////////////////////////////////////////////////////////////////////////////////
//
// Connector
//
////////////////////////////////////////////////////////////////////////////////

typedef ACE_Connector<ClientHandler, ACE_SOCK_CONNECTOR> Connector;

#if 0
class Connector : public BaseConnector
{
public:

    virtual int make_svc_handler(ClientHandler*& handler)
    {
	handler = new ClientHandler;

	if (reactor())
	    handler->reactor(reactor());

	return 0;
    }
};
#endif

////////////////////////////////////////////////////////////////////////////////
//
// ClientRep
//
////////////////////////////////////////////////////////////////////////////////

struct ClientRep
{
#ifdef PEGASUS_OS_TYPE_WINDOWS
    ACE_OS_Object_Manager ace_os_object_manager;
    ACE_Object_Manager ace_object_manager;
#endif
};

////////////////////////////////////////////////////////////////////////////////
//
// Client
//
////////////////////////////////////////////////////////////////////////////////


Client::Client(Uint32 timeOutMilliseconds) 
    : _handler(0), _timeOutMilliseconds(timeOutMilliseconds)
{
    _rep = new ClientRep;
}

Client::~Client()
{
    delete _rep;
}

void Client::connect(const char* hostName, Uint32 port)
{
    if (_handler)
	throw AlreadyConnected();

    ACE_INET_Addr addr(port, hostName);
    ClientHandler* handler = new ClientHandler(addr);

    // ATTN: Perform a blocking connect:

    Uint32 seconds = 5;
    ACE_Synch_Options options(
	ACE_Synch_Options::USE_TIMEOUT, 
	ACE_Time_Value(seconds));

    Connector connector;

    if (connector.connect(handler, addr, options) == -1)
	throw FailedToConnect();

    _handler = handler;
}

void Client::get(const char* document) const
{
    if (!_handler)
	throw NotConnected();

    Array<Sint8> message = XmlWriter::formatGetHeader(document);

    ((ClientHandler*)_handler)->peer().send_n(
	message.getData(), message.getSize());
}

void Client::runOnce()
{
    ACE_Reactor::instance()->handle_events();
}

void Client::runForever()
{
    for (;;)
    {
	ACE_Reactor::instance()->handle_events();
    }
}

//------------------------------------------------------------------------------
//
// Client::getClass()
//
//     <?xml version="1.0" encoding="utf-8" ?>
//     <CIM CIMVERSION="2.0" DTDVERSION="2.0">
//       <MESSAGE ID="87872" PROTOCOLVERSION="1.0">
//         <SIMPLEREQ>
//           <IMETHODCALL NAME="GetClass">
// 	       <LOCALNAMESPACEPATH>
// 	         <NAMESPACE NAME="root"/>
// 	         <NAMESPACE NAME="cimv20"/>
// 	       </LOCALNAMESPACEPATH>
// 	       <IPARAMVALUE NAME="ClassName"><CLASSNAME NAME="X"/></IPARAMVALUE>
// 	       <IPARAMVALUE NAME="LocalOnly"><VALUE>FALSE</VALUE></IPARAMVALUE>
// 	     </IMETHODCALL>
//         </SIMPLEREQ>
//       </MESSAGE>
//     </CIM>
//
//------------------------------------------------------------------------------

ClassDecl Client::getClass(
    const String& nameSpace,
    const String& className,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const Array<String>& propertyList)
{
    Uint32 messageId = XmlWriter::getNextMessageId();

    Array<Sint8> parameters;

    XmlWriter::appendClassNameParameter(
	parameters, "ClassName", className);
	
    if (localOnly != true)
	XmlWriter::appendBooleanParameter(
	    parameters, "LocalOnly", false);

    if (includeQualifiers != true)
	XmlWriter::appendBooleanParameter(
	    parameters, "IncludeQualifiers", false);

    if (includeClassOrigin != false)
	XmlWriter::appendBooleanParameter(
	    parameters, "IncludeClassOrigin", true);

    Array<Sint8> message = XmlWriter::formatSimpleReqMessage(
	((ClientHandler*)_handler)->getHostName(),
	nameSpace, "GetClass", parameters);

    ClientHandler* handler = (ClientHandler*)_handler;
    handler->sendMessage(message);

    if (!handler->waitForResponse(_timeOutMilliseconds))
	throw TimedOut();

    GetClassResult* result = handler->_getClassResult;
    ClassDecl classDecl = result->classDecl;
    CimException::Code code = result->code;
    delete result;
    handler->_getClassResult = 0;

    if (code != CimException::SUCCESS)
	throw CimException(code);

    return classDecl;
}

InstanceDecl Client::getInstance(
    const String& nameSpace,
    const Reference& instanceName,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const Array<String>& propertyList)
{
    // ATTN: the property list not passed here. Handle this later.
    // same story for getClass().

    Uint32 messageId = XmlWriter::getNextMessageId();

    Array<Sint8> parameters;

    XmlWriter::appendInstanceNameParameter(
	parameters, "InstanceName", instanceName);
	
    if (localOnly != true)
	XmlWriter::appendBooleanParameter(
	    parameters, "LocalOnly", false);

    if (includeQualifiers != false)
	XmlWriter::appendBooleanParameter(
	    parameters, "IncludeQualifiers", true);

    if (includeClassOrigin != false)
	XmlWriter::appendBooleanParameter(
	    parameters, "IncludeClassOrigin", true);

    Array<Sint8> message = XmlWriter::formatSimpleReqMessage(
	((ClientHandler*)_handler)->getHostName(),
	nameSpace, "GetInstance", parameters);

    ClientHandler* handler = (ClientHandler*)_handler;
    handler->sendMessage(message);

    if (!handler->waitForResponse(_timeOutMilliseconds))
	throw TimedOut();

    GetInstanceResult* result = handler->_getInstanceResult;
    InstanceDecl instanceDecl = result->instanceDecl;
    CimException::Code code = result->code;
    delete result;
    handler->_getInstanceResult = 0;

    if (code != CimException::SUCCESS)
	throw CimException(code);

    return instanceDecl;
}

void Client::deleteClass(
    const String& nameSpace,
    const String& className)
{
    Uint32 messageId = XmlWriter::getNextMessageId();

    Array<Sint8> parameters;

    if (className.getLength())
	XmlWriter::appendClassNameParameter(parameters, "ClassName", className);

    Array<Sint8> message = XmlWriter::formatSimpleReqMessage(
	((ClientHandler*)_handler)->getHostName(),
	nameSpace, "DeleteClass", parameters);
	
    ClientHandler* handler = (ClientHandler*)_handler;
    handler->sendMessage(message);

    if (!handler->waitForResponse(_timeOutMilliseconds))
	throw TimedOut();

    DeleteClassResult* result = handler->_deleteClassResult;
    CimException::Code code = result->code;
    delete result;
    handler->_deleteClassResult = 0;

    if (code != CimException::SUCCESS)
	throw CimException(code);
}

void Client::deleteInstance(
    const String& nameSpace,
    const Reference& instanceName)
{
    throw CimException(CimException::NOT_SUPPORTED);
}


void Client::createClass(
    const String& nameSpace,
    ClassDecl& newClass)
{
    Uint32 messageId = XmlWriter::getNextMessageId();

    Array<Sint8> parameters;
    XmlWriter::appendClassParameter(parameters, "NewClass", newClass);
	
    Array<Sint8> message = XmlWriter::formatSimpleReqMessage(
	((ClientHandler*)_handler)->getHostName(),
	nameSpace, "CreateClass", parameters);

    ClientHandler* handler = (ClientHandler*)_handler;
    handler->sendMessage(message);

    if (!handler->waitForResponse(_timeOutMilliseconds))
	throw TimedOut();

    CreateClassResult* result = handler->_createClassResult;
    CimException::Code code = result->code;
    delete result;
    handler->_createClassResult = 0;

    if (code != CimException::SUCCESS)
	throw CimException(code);
}


void Client::createInstance(
    const String& nameSpace,
    const InstanceDecl& newInstance) 
{
    throw CimException(CimException::NOT_SUPPORTED);
}

void Client::modifyClass(
    const String& nameSpace,
    ClassDecl& modifiedClass)
{
    Uint32 messageId = XmlWriter::getNextMessageId();

    Array<Sint8> parameters;
    XmlWriter::appendClassParameter(parameters, "ModifiedClass", modifiedClass);
	
    Array<Sint8> message = XmlWriter::formatSimpleReqMessage(
	((ClientHandler*)_handler)->getHostName(),
	nameSpace, "ModifyClass", parameters);

    ClientHandler* handler = (ClientHandler*)_handler;
    handler->sendMessage(message);

    if (!handler->waitForResponse(_timeOutMilliseconds))
	throw TimedOut();

    ModifyClassResult* result = handler->_modifyClassResult;
    CimException::Code code = result->code;
    delete result;
    handler->_modifyClassResult = 0;

    if (code != CimException::SUCCESS)
	throw CimException(code);
}


void Client::modifyInstance(
    const String& nameSpace,
    const InstanceDecl& modifiedInstance)
{
    throw CimException(CimException::NOT_SUPPORTED);
}

Array<ClassDecl> Client::enumerateClasses(
    const String& nameSpace,
    const String& className,
    Boolean deepInheritance,
    Boolean localOnly,
    Boolean includeQualifiers ,
    Boolean includeClassOrigin)
{
    Uint32 messageId = XmlWriter::getNextMessageId();

    Array<Sint8> parameters;

    if (className.getLength())
	XmlWriter::appendClassNameParameter(parameters, "ClassName", className);
	
    if (deepInheritance != false)
	XmlWriter::appendBooleanParameter(parameters, "DeepInheritance", true);

    if (localOnly != true)
	XmlWriter::appendBooleanParameter(parameters, "LocalOnly", false);

    if (includeQualifiers != true)
	XmlWriter::appendBooleanParameter(
	    parameters, "IncludeQualifiers", false);

    if (includeClassOrigin != false)
	XmlWriter::appendBooleanParameter(
	    parameters, "IncludeClassOrigin", true);

    Array<Sint8> message = XmlWriter::formatSimpleReqMessage(
	((ClientHandler*)_handler)->getHostName(),
	nameSpace, "EnumerateClasses", parameters);

    ClientHandler* handler = (ClientHandler*)_handler;
    handler->sendMessage(message);

    if (!handler->waitForResponse(_timeOutMilliseconds))
	throw TimedOut();

    EnumerateClassesResult* result = handler->_enumerateClassesResult;
    Array<ClassDecl> classDecls = result->classDecls;
    CimException::Code code = result->code;
    delete result;
    handler->_enumerateClassesResult = 0;

    if (code != CimException::SUCCESS)
	throw CimException(code);

    return classDecls;
}

//------------------------------------------------------------------------------
//
// Client::enumerateClassNames()
//
//------------------------------------------------------------------------------

Array<String> Client::enumerateClassNames(
    const String& nameSpace,
    const String& className,
    Boolean deepInheritance)
{
//STUB{:
    Uint32 messageId = XmlWriter::getNextMessageId();

    Array<Sint8> parameters;

    if (className.getLength())
	XmlWriter::appendClassNameParameter(parameters, "ClassName", className);
	
    if (deepInheritance != false)
	XmlWriter::appendBooleanParameter(parameters, "DeepInheritance", true);

    Array<Sint8> message = XmlWriter::formatSimpleReqMessage(
	((ClientHandler*)_handler)->getHostName(),
	nameSpace, "EnumerateClassNames", parameters);

    ClientHandler* handler = (ClientHandler*)_handler;
    handler->sendMessage(message);

    if (!handler->waitForResponse(_timeOutMilliseconds))
	throw TimedOut();

    EnumerateClassNamesResult* result = handler->_enumerateClassNamesResult;
    Array<String> classNames = result->classNames;
    CimException::Code code = result->code;
    delete result;
    handler->_enumerateClassNamesResult = 0;

    if (code != CimException::SUCCESS)
	throw CimException(code);

    return classNames;
//STUB}
}

Array<InstanceDecl> Client::enumerateInstances(
    const String& nameSpace,
    const String& className,
    Boolean deepInheritance,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const Array<String>& propertyList)
{
    throw CimException(CimException::NOT_SUPPORTED);
    return Array<InstanceDecl>();
}


Array<Reference> Client::enumerateInstanceNames(
    const String& nameSpace,
    const String& className)
{
    Uint32 messageId = XmlWriter::getNextMessageId();

    Array<Sint8> parameters;

    XmlWriter::appendClassNameParameter(parameters, "ClassName", className);
	
    Array<Sint8> message = XmlWriter::formatSimpleReqMessage(
	((ClientHandler*)_handler)->getHostName(),
	nameSpace, "EnumerateInstanceNames", parameters);

    ClientHandler* handler = (ClientHandler*)_handler;
    handler->sendMessage(message);

    if (!handler->waitForResponse(_timeOutMilliseconds))
	throw TimedOut();

    EnumerateInstanceNamesResult* result 
	= handler->_enumerateInstanceNamesResult;
    Array<Reference> instanceNames = result->instanceNames;
    CimException::Code code = result->code;
    delete result;
    handler->_enumerateInstanceNamesResult = 0;

    if (code != CimException::SUCCESS)
	throw CimException(code);

    return instanceNames;
}

Array<InstanceDecl> Client::execQuery(
    const String& queryLanguage,
    const String& query) 
{
    throw CimException(CimException::NOT_SUPPORTED);
    return Array<InstanceDecl>();
}


Array<InstanceDecl> Client::associators(
    const String& nameSpace,
    const Reference& objectName,
    const String& assocClass,
    const String& resultClass,
    const String& role,
    const String& resultRole,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const Array<String>& propertyList)
{
    throw CimException(CimException::NOT_SUPPORTED);
    return Array<InstanceDecl>();
}


Array<Reference> Client::associatorNames(
    const String& nameSpace,
    const Reference& objectName,
    const String& assocClass,
    const String& resultClass,
    const String& role,
    const String& resultRole)
{
    throw CimException(CimException::NOT_SUPPORTED);
    return Array<Reference>();
}


Array<InstanceDecl> Client::references(
    const String& nameSpace,
    const Reference& objectName,
    const String& resultClass,
    const String& role,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const Array<String>& propertyList)
{
    throw CimException(CimException::NOT_SUPPORTED);
    return Array<InstanceDecl>();
}


Array<Reference> Client::referenceNames(
    const String& nameSpace,
    const Reference& objectName,
    const String& resultClass,
    const String& role)
{
    throw CimException(CimException::NOT_SUPPORTED);
    return Array<Reference>();
}


Value Client::getProperty(
    const String& nameSpace,
    const Reference& instanceName,
    const String& propertyName)
{
    throw CimException(CimException::NOT_SUPPORTED);
    return Value();
}


void Client::setProperty(
    const String& nameSpace,
    const Reference& instanceName,
    const String& propertyName,
    const Value& newValue)
{
    throw CimException(CimException::NOT_SUPPORTED);
}


QualifierDecl Client::getQualifier(
    const String& nameSpace,
    const String& qualifierName)
{
    Uint32 messageId = XmlWriter::getNextMessageId();

    Array<Sint8> parameters;

    // ATTN: no way in the specification to pass a qualifier name within
    // an IPARAMVALUE. Need help to solve this one.

    if (qualifierName.getLength())
	XmlWriter::appendClassNameParameter(
	    parameters, "QualifierName", qualifierName);
	
    Array<Sint8> message = XmlWriter::formatSimpleReqMessage(
	((ClientHandler*)_handler)->getHostName(),
	nameSpace, "GetQualifier", parameters);

    ClientHandler* handler = (ClientHandler*)_handler;
    handler->sendMessage(message);

    if (!handler->waitForResponse(_timeOutMilliseconds))
	throw TimedOut();

    GetQualifierResult* result = handler->_getQualifierResult;
    QualifierDecl qualifierDecl = result->qualifierDecl;
    CimException::Code code = result->code;
    delete result;
    handler->_getQualifierResult = 0;

    if (code != CimException::SUCCESS)
	throw CimException(code);

    return qualifierDecl;
}

void Client::setQualifier(
    const String& nameSpace,
    const QualifierDecl& qualifierDeclaration)
{
    Uint32 messageId = XmlWriter::getNextMessageId();

    Array<Sint8> parameters;
    XmlWriter::appendQualifierDeclarationParameter(
	parameters, "QualifierDeclaration", qualifierDeclaration);

    Array<Sint8> message = XmlWriter::formatSimpleReqMessage(
	((ClientHandler*)_handler)->getHostName(),
	nameSpace, "SetQualifier", parameters);

    ClientHandler* handler = (ClientHandler*)_handler;
    handler->sendMessage(message);

    if (!handler->waitForResponse(_timeOutMilliseconds))
	throw TimedOut();

    SetQualifierResult* result = handler->_setQualifierResult;
    CimException::Code code = result->code;
    delete result;
    handler->_setQualifierResult = 0;

    if (code != CimException::SUCCESS)
	throw CimException(code);
}

void Client::deleteQualifier(
    const String& nameSpace,
    const String& qualifierName)
{
    Uint32 messageId = XmlWriter::getNextMessageId();

    Array<Sint8> parameters;

    // ATTN: no way in the specification to pass a qualifier name within
    // an IPARAMVALUE. Need help to solve this one.

    if (qualifierName.getLength())
	XmlWriter::appendClassNameParameter(
	    parameters, "QualifierName", qualifierName);
	
    Array<Sint8> message = XmlWriter::formatSimpleReqMessage(
	((ClientHandler*)_handler)->getHostName(),
	nameSpace, "DeleteQualifier", parameters);

    ClientHandler* handler = (ClientHandler*)_handler;
    handler->sendMessage(message);

    if (!handler->waitForResponse(_timeOutMilliseconds))
	throw TimedOut();

    DeleteQualifierResult* result = handler->_deleteQualifierResult;
    CimException::Code code = result->code;
    delete result;
    handler->_deleteQualifierResult = 0;

    if (code != CimException::SUCCESS)
	throw CimException(code);
}

Array<QualifierDecl> Client::enumerateQualifiers(
    const String& nameSpace)
{
    Uint32 messageId = XmlWriter::getNextMessageId();

    Array<Sint8> parameters;

    Array<Sint8> message = XmlWriter::formatSimpleReqMessage(
	((ClientHandler*)_handler)->getHostName(),
	nameSpace, "EnumerateQualifiers", parameters);

    ClientHandler* handler = (ClientHandler*)_handler;
    handler->sendMessage(message);

    if (!handler->waitForResponse(_timeOutMilliseconds))
	throw TimedOut();

    EnumerateQualifiersResult* result = handler->_enumerateQualifiersResult;
    Array<QualifierDecl> qualifierDecls = result->qualifierDecls;
    CimException::Code code = result->code;
    delete result;
    handler->_enumerateQualifiersResult = 0;

    if (code != CimException::SUCCESS)
	throw CimException(code);

    return qualifierDecls;
}

Value Client::invokeMethod(
    const String& nameSpace,
    const Reference& instanceName,
    const String& methodName,
    const Array<Value>& inParameters,
    Array<Value>& outParameters)
{
    throw CimException(CimException::NOT_SUPPORTED);
    return Value();
}

PEGASUS_NAMESPACE_END
