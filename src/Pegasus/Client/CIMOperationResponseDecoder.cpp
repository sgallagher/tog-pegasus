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
//              Yi Zhou, Hewlett-Packard Company (yi_zhou@hp.com)
//              Nag Boranna, Hewlett-Packard Company (nagaraja_boranna@hp.com)
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <iostream>
#include <Pegasus/Common/XmlParser.h>
#include <Pegasus/Common/XmlReader.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/HTTPMessage.h>
#include <Pegasus/Common/CIMMessage.h>
#include "CIMOperationResponseDecoder.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

CIMOperationResponseDecoder::CIMOperationResponseDecoder(
    MessageQueue* outputQueue,
    MessageQueue* encoderQueue,
    ClientAuthenticator* authenticator)
    :
    _outputQueue(outputQueue),
    _encoderQueue(encoderQueue),
    _authenticator(authenticator)
{

}

CIMOperationResponseDecoder::~CIMOperationResponseDecoder()
{

}

void  CIMOperationResponseDecoder::setEncoderQueue(MessageQueue* encoderQueue)
{
    _encoderQueue = encoderQueue;
}

void CIMOperationResponseDecoder::handleEnqueue()
{
    Message* message = dequeue();

    if (!message)
	return;

    switch (message->getType())
    {
	case HTTP_MESSAGE:
	{

	    HTTPMessage* httpMessage = (HTTPMessage*)message;
	    _handleHTTPMessage(httpMessage);
	    break;
	}

	default:
	    // ATTN: send this to the orphan queue!
	    break;
    }

    delete message;
}

const char* CIMOperationResponseDecoder::getQueueName() const
{
    return "CIMOperationResponseDecoder";
}

void CIMOperationResponseDecoder::_handleHTTPMessage(HTTPMessage* httpMessage)
{
    //
    // Parse the HTTP message:
    //

    String startLine;
    Array<HTTPHeader> headers;
    Sint8* content;
    Uint32 contentLength;

    httpMessage->parse(startLine, headers, contentLength);

    if (_authenticator->checkResponseHeaderForChallenge(headers))
    {
        //
        // Get the original request, put that in the encoder's queue for
        // re-sending with authentication challenge response.
        //
        Message* reqMessage = _authenticator->getRequestMessage();
        _encoderQueue->enqueue(reqMessage);

        return;
    }
    else
    {

        //
        // Received a valid/error response from the server.
        // We do not need the original request message anymore, hence delete
        // the request message by getting the handle from the ClientAuthenticator.
        //
        Message* reqMessage = _authenticator->getRequestMessage();
	_authenticator->clearRequest();
        if (reqMessage)
        {
            delete reqMessage;
        }
    }

    //
    // Search for "CIMOperation" header:
    //

    String cimOperation;

    if (!HTTPMessage::lookupHeader(
	headers, "*CIMOperation", cimOperation, true))
    {
	// ATTN: error discarded at this time!
	return;
    }

    //
    // Zero-terminate the message:
    //

    httpMessage->message.append('\0');

    // Calculate the beginning of the content from the message size and
    // the content length.  Subtract 1 to take into account the null
    // character we just added to the end of the message.

    content = (Sint8*) httpMessage->message.getData() +
	httpMessage->message.size() - contentLength - 1;

    //
    // If it is a method response, then dispatch it to be handled:
    //

    if (!String::equalNoCase(cimOperation, "MethodResponse"))
    {
	// ATTN: error discarded at this time!
	return;
    }

    _handleMethodResponse(content);
}

void CIMOperationResponseDecoder::_handleMethodResponse(char* content)
{
    Message* response = 0;

    //
    // Create and initialize XML parser:
    //

    XmlParser parser((char*)content);
    XmlEntry entry;

    try
    {
	//
	// Process <?xml ... >
	//

	XmlReader::expectXmlDeclaration(parser, entry);

	//
	// Process <CIM ... >
	//

	XmlReader::testCimStartTag(parser);

	//
	// Expect <MESSAGE ... >
	//

	String messageId;
	const char* protocolVersion = 0;

	if (!XmlReader::getMessageStartTag(parser, messageId, protocolVersion))
	    throw XmlValidationError(
		parser.getLine(), "expected MESSAGE element");

	if (strcmp(protocolVersion, "1.0") != 0)
	{
	    // ATTN: protocol version being ignored at present!

	    return;
	}

	//
	// Expect <SIMPLERSP ... >
	//

	XmlReader::expectStartTag(parser, entry, "SIMPLERSP");

	//
	// Expect <IMETHODRESPONSE ... >
	//

	const char* iMethodResponseName = 0;

	if (XmlReader::getIMethodResponseStartTag(parser, iMethodResponseName))
	{
	    //
	    // Dispatch the method:
	    //

	    if (EqualNoCase(iMethodResponseName, "GetClass"))
		response = _decodeGetClassResponse(parser, messageId);
	    else if (EqualNoCase(iMethodResponseName, "GetInstance"))
		response = _decodeGetInstanceResponse(parser, messageId);
	    else if (EqualNoCase(iMethodResponseName, "EnumerateClassNames"))
		response = _decodeEnumerateClassNamesResponse(parser,messageId);
	    else if (EqualNoCase(iMethodResponseName, "References"))
		response = _decodeReferencesResponse(parser, messageId);
	    else if (EqualNoCase(iMethodResponseName, "ReferenceNames"))
		response = _decodeReferenceNamesResponse(parser, messageId);
	    else if (EqualNoCase(iMethodResponseName, "AssociatorNames"))
		response = _decodeAssociatorNamesResponse(parser, messageId);
	    else if (EqualNoCase(iMethodResponseName, "Associators"))
		response = _decodeAssociatorsResponse(parser, messageId);
	    else if (EqualNoCase(iMethodResponseName, "CreateInstance"))
		response = _decodeCreateInstanceResponse(parser, messageId);
	    else if (EqualNoCase(iMethodResponseName,"EnumerateInstanceNames"))
		response = _decodeEnumerateInstanceNamesResponse(
                  parser, messageId);
	    else if (EqualNoCase(iMethodResponseName,"EnumerateInstances"))
		response = _decodeEnumerateInstancesResponse(parser, messageId);
	    else if (EqualNoCase(iMethodResponseName, "GetProperty"))
		response = _decodeGetPropertyResponse(parser, messageId);
	    else if (EqualNoCase(iMethodResponseName, "SetProperty"))
		response = _decodeSetPropertyResponse(parser, messageId);
	    else if (EqualNoCase(iMethodResponseName, "DeleteQualifier"))
		response = _decodeDeleteQualifierResponse(parser, messageId);
	    else if (EqualNoCase(iMethodResponseName, "GetQualifier"))
		response = _decodeGetQualifierResponse(parser, messageId);
	    else if (EqualNoCase(iMethodResponseName, "SetQualifier"))
		response = _decodeSetQualifierResponse(parser, messageId);
	    else if (EqualNoCase(iMethodResponseName, "EnumerateQualifiers"))
		response = _decodeEnumerateQualifiersResponse(parser,messageId);
	    else if (EqualNoCase(iMethodResponseName, "EnumerateClasses"))
		response = _decodeEnumerateClassesResponse(parser, messageId);
	    else if (EqualNoCase(iMethodResponseName, "CreateClass"))
		response = _decodeCreateClassResponse(parser, messageId);
	    else if (EqualNoCase(iMethodResponseName, "ModifyClass"))
		response = _decodeModifyClassResponse(parser, messageId);
	    else if (EqualNoCase(iMethodResponseName, "ModifyInstance"))
		response = _decodeModifyInstanceResponse(parser, messageId);
	    else if (EqualNoCase(iMethodResponseName, "DeleteClass"))
		response = _decodeDeleteClassResponse(parser, messageId);
	    else if (EqualNoCase(iMethodResponseName, "DeleteInstance"))
		response = _decodeDeleteInstanceResponse(parser, messageId);
	    else
	    {
                // Unrecognized IMethodResponse name attribute
	        throw XmlValidationError(parser.getLine(),
	            String("Unrecognized IMethodResponse name \"") +
                        iMethodResponseName + "\"");
	    }
	
	    //
	    // Handle end tag:
	    //

	    XmlReader::expectEndTag(parser, "IMETHODRESPONSE");
	}
	else if (XmlReader::getMethodResponseStartTag(parser, 
	    iMethodResponseName))
	{
	    response = _decodeInvokeMethodResponse(
              parser, messageId, iMethodResponseName);

	    //
	    // Handle end tag:
	    //
	    XmlReader::expectEndTag(parser, "METHODRESPONSE");
	}
	else
	{
	    throw XmlValidationError(parser.getLine(),
	        "expected METHODRESPONSE or IMETHODRESPONSE element");
	}

        //
        // Handle end tags:
        //
	XmlReader::expectEndTag(parser, "SIMPLERSP");
	XmlReader::expectEndTag(parser, "MESSAGE");
	XmlReader::expectEndTag(parser, "CIM");
    }
    catch (Exception& x)
    {
	// ATTN: ignore the exception for now!

	cout << x.getMessage() << endl;
	return;
    }

    _outputQueue->enqueue(response);
}

CIMCreateClassResponseMessage* CIMOperationResponseDecoder::_decodeCreateClassResponse(
    XmlParser& parser, 
    const String& messageId)
{
    XmlEntry entry;
    CIMStatusCode code;
    const char* description = 0;

    if (XmlReader::getErrorElement(parser, code, description))
    {
	return(new CIMCreateClassResponseMessage(
	    messageId,
	    code,
	    description,
	    QueueIdStack()));

    }
    else if (XmlReader::testStartTag(parser, entry, "IRETURNVALUE"))
    {
	XmlReader::testEndTag(parser, "IRETURNVALUE");

	return(new CIMCreateClassResponseMessage(
	    messageId,
	    CIM_ERR_SUCCESS,
	    String(),
	    QueueIdStack()));
    }
    else
    {
	throw XmlValidationError(parser.getLine(),
	    "expected ERROR or IRETURNVALUE element");
    }
}

CIMGetClassResponseMessage* CIMOperationResponseDecoder::_decodeGetClassResponse(
    XmlParser& parser, const String& messageId)
{
    XmlEntry entry;
    CIMStatusCode code;
    const char* description = 0;

    if (XmlReader::getErrorElement(parser, code, description))
    {
	return(new CIMGetClassResponseMessage(
	    messageId,
	    code,
	    description,
	    QueueIdStack(),
	    CIMClass()));
    }
    else if (XmlReader::testStartTag(parser, entry, "IRETURNVALUE"))
    {
	CIMClass cimClass;

	if (!XmlReader::getClassElement(parser, cimClass))
	    throw XmlValidationError(parser.getLine(),"expected CLASS element");

	XmlReader::testEndTag(parser, "IRETURNVALUE");

	return(new CIMGetClassResponseMessage(
	    messageId,
	    CIM_ERR_SUCCESS,
	    String(),
	    QueueIdStack(),
	    cimClass));
    }
    else
    {
	throw XmlValidationError(parser.getLine(),
	    "expected ERROR or IRETURNVALUE element");
    }
}

CIMModifyClassResponseMessage* CIMOperationResponseDecoder::_decodeModifyClassResponse(
    XmlParser& parser, const String& messageId)
{
    XmlEntry entry;
    CIMStatusCode code;
    const char* description = 0;

    if (XmlReader::getErrorElement(parser, code, description))
    {
	return(new CIMModifyClassResponseMessage(
	    messageId,
	    code,
	    description,
	    QueueIdStack()));
    }
    else if (XmlReader::testStartTag(parser, entry, "IRETURNVALUE"))
    {
	XmlReader::testEndTag(parser, "IRETURNVALUE");

	return(new CIMModifyClassResponseMessage(
	    messageId,
	    CIM_ERR_SUCCESS,
	    String(),
	    QueueIdStack()));
    }
    else
    {
	throw XmlValidationError(parser.getLine(),
	    "expected ERROR or IRETURNVALUE element");
    }
}

CIMEnumerateClassNamesResponseMessage* CIMOperationResponseDecoder::_decodeEnumerateClassNamesResponse(
    XmlParser& parser, const String& messageId)
{
    XmlEntry entry;
    CIMStatusCode code;
    const char* description = 0;

    if (XmlReader::getErrorElement(parser, code, description))
    {
	return(new CIMEnumerateClassNamesResponseMessage(
	    messageId,
	    code,
	    description,
	    QueueIdStack(),
	    Array<String>()));
    }
    else if (XmlReader::testStartTag(parser, entry, "IRETURNVALUE"))
    {
	Array<String> classNames;
	String className;

	while (XmlReader::getClassNameElement(parser, className, false))
	    classNames.append(className);

	XmlReader::testEndTag(parser, "IRETURNVALUE");

	return(new CIMEnumerateClassNamesResponseMessage(
	    messageId,
	    CIM_ERR_SUCCESS,
	    String(),
	    QueueIdStack(),
	    classNames));
    }
    else
    {
	throw XmlValidationError(parser.getLine(),
	    "expected ERROR or IRETURNVALUE element");
    }
}

CIMEnumerateClassesResponseMessage* CIMOperationResponseDecoder::_decodeEnumerateClassesResponse(
    XmlParser& parser, const String& messageId)
{
    XmlEntry entry;
    CIMStatusCode code;
    const char* description = 0;

    if (XmlReader::getErrorElement(parser, code, description))
    {
	return(new CIMEnumerateClassesResponseMessage(
	    messageId,
	    code,
	    description,
	    QueueIdStack(),
	    Array<CIMClass>()));
    }
    else if (XmlReader::testStartTag(parser, entry, "IRETURNVALUE"))
    {
	Array<CIMClass> cimClasses;
	CIMClass cimClass;

	while (XmlReader::getClassElement(parser, cimClass))
	    cimClasses.append(cimClass);

	XmlReader::testEndTag(parser, "IRETURNVALUE");

	return(new CIMEnumerateClassesResponseMessage(
	    messageId,
	    CIM_ERR_SUCCESS,
	    String(),
	    QueueIdStack(),
	    cimClasses));
    }
    else
    {
	throw XmlValidationError(parser.getLine(),
	    "expected ERROR or IRETURNVALUE element");
    }
}

CIMDeleteClassResponseMessage* CIMOperationResponseDecoder::_decodeDeleteClassResponse(
    XmlParser& parser, const String& messageId)
{
    XmlEntry entry;
    CIMStatusCode code;
    const char* description = 0;

    if (XmlReader::getErrorElement(parser, code, description))
    {
	return(new CIMDeleteClassResponseMessage(
	    messageId,
	    code,
	    description,
	    QueueIdStack()));
    }
    else if (XmlReader::testStartTag(parser, entry, "IRETURNVALUE"))
    {
	XmlReader::testEndTag(parser, "IRETURNVALUE");

	return(new CIMDeleteClassResponseMessage(
	    messageId,
	    CIM_ERR_SUCCESS,
	    String(),
	    QueueIdStack()));
    }
    else
    {
	throw XmlValidationError(parser.getLine(),
	    "expected ERROR or IRETURNVALUE element");
    }
}

CIMCreateInstanceResponseMessage* CIMOperationResponseDecoder::_decodeCreateInstanceResponse(
    XmlParser& parser, const String& messageId)
{
    XmlEntry entry;
    CIMStatusCode code;
    const char* description = 0;

    if (XmlReader::getErrorElement(parser, code, description))
    {
	return(new CIMCreateInstanceResponseMessage(
	    messageId,
	    code,
	    description,
	    QueueIdStack(),
	    CIMReference()));
    }
    else if (XmlReader::testStartTag(parser, entry, "IRETURNVALUE"))
    {
	CIMReference instanceName;
	XmlReader::getInstanceNameElement(parser, instanceName);

	XmlReader::testEndTag(parser, "IRETURNVALUE");

	return(new CIMCreateInstanceResponseMessage(
	    messageId,
	    CIM_ERR_SUCCESS,
	    String(),
	    QueueIdStack(),
	    instanceName));
    }
    else
    {
	throw XmlValidationError(parser.getLine(),
	    "expected ERROR or IRETURNVALUE element");
    }
}

CIMGetInstanceResponseMessage* CIMOperationResponseDecoder::_decodeGetInstanceResponse(
    XmlParser& parser, const String& messageId)
{
    XmlEntry entry;
    CIMStatusCode code;
    const char* description = 0;

    if (XmlReader::getErrorElement(parser, code, description))
    {
	return(new CIMGetInstanceResponseMessage(
	    messageId,
	    code,
	    description,
	    QueueIdStack(),
	    CIMInstance()));
    }
    else if (XmlReader::testStartTag(parser, entry, "IRETURNVALUE"))
    {
	CIMInstance cimInstance;

	if (!XmlReader::getInstanceElement(parser, cimInstance))
	{
	    throw XmlValidationError(
		parser.getLine(), "expected INSTANCE element");
	}

	XmlReader::testEndTag(parser, "IRETURNVALUE");

	return(new CIMGetInstanceResponseMessage(
	    messageId,
	    CIM_ERR_SUCCESS,
	    String(),
	    QueueIdStack(),
	    cimInstance));
    }
    else
    {
	throw XmlValidationError(parser.getLine(),
	    "expected ERROR or IRETURNVALUE element");
    }
}

CIMModifyInstanceResponseMessage* CIMOperationResponseDecoder::_decodeModifyInstanceResponse(
    XmlParser& parser, const String& messageId)
{
    XmlEntry entry;
    CIMStatusCode code;
    const char* description = 0;

    if (XmlReader::getErrorElement(parser, code, description))
    {
	return(new CIMModifyInstanceResponseMessage(
	    messageId,
	    code,
	    description,
	    QueueIdStack()));
    }
    else if (XmlReader::testStartTag(parser, entry, "IRETURNVALUE"))
    {
	XmlReader::testEndTag(parser, "IRETURNVALUE");

	return(new CIMModifyInstanceResponseMessage(
	    messageId,
	    CIM_ERR_SUCCESS,
	    String(),
	    QueueIdStack()));
    }
    else
    {
	throw XmlValidationError(parser.getLine(),
	    "expected ERROR or IRETURNVALUE element");
    }
}

CIMEnumerateInstanceNamesResponseMessage* CIMOperationResponseDecoder::_decodeEnumerateInstanceNamesResponse(
    XmlParser& parser, const String& messageId)
{
    XmlEntry entry;
    CIMStatusCode code;
    const char* description = 0;

    if (XmlReader::getErrorElement(parser, code, description))
    {
	return(new CIMEnumerateInstanceNamesResponseMessage(
	    messageId,
	    code,
	    description,
	    QueueIdStack(),
	    Array<CIMReference>()));
    }
    else if (XmlReader::testStartTag(parser, entry, "IRETURNVALUE"))
    {
	Array<CIMReference> instanceNames;
	String className;
	Array<KeyBinding> keyBindings;

	while (XmlReader::getInstanceNameElement(
	    parser, className, keyBindings))
	{
	    CIMReference r(
		String::EMPTY,
		String::EMPTY,
		className,
		keyBindings);
	    instanceNames.append(r);
	}

	XmlReader::testEndTag(parser, "IRETURNVALUE");

	return(new CIMEnumerateInstanceNamesResponseMessage(
	    messageId,
	    CIM_ERR_SUCCESS,
	    String(),
	    QueueIdStack(),
	    instanceNames));
    }
    else
    {
	throw XmlValidationError(parser.getLine(),
	    "expected ERROR or IRETURNVALUE element");
    }
}

CIMEnumerateInstancesResponseMessage* CIMOperationResponseDecoder::_decodeEnumerateInstancesResponse(
    XmlParser& parser, const String& messageId)
{
    XmlEntry entry;
    CIMStatusCode code;
    const char* description = 0;

    if (XmlReader::getErrorElement(parser, code, description))
    {
	return(new CIMEnumerateInstancesResponseMessage(
	    messageId,
	    code,
	    description,
	    QueueIdStack(),
	    Array<CIMNamedInstance>()));
    }
    else if (XmlReader::testStartTag(parser, entry, "IRETURNVALUE"))
    {
	Array<CIMNamedInstance> namedInstances;
	CIMNamedInstance tmp;

	while (XmlReader::getNamedInstanceElement(parser, tmp))
	    namedInstances.append(tmp);

	XmlReader::testEndTag(parser, "IRETURNVALUE");

	return(new CIMEnumerateInstancesResponseMessage(
	    messageId,
	    CIM_ERR_SUCCESS,
	    String(),
	    QueueIdStack(),
	    namedInstances));
    }
    else
    {
	throw XmlValidationError(parser.getLine(),
	    "expected ERROR or IRETURNVALUE element");
    }
}

CIMDeleteInstanceResponseMessage* CIMOperationResponseDecoder::_decodeDeleteInstanceResponse(
    XmlParser& parser, const String& messageId)
{
    XmlEntry entry;
    CIMStatusCode code;
    const char* description = 0;

    if (XmlReader::getErrorElement(parser, code, description))
    {
	return(new CIMDeleteInstanceResponseMessage(
	    messageId,
	    code,
	    description,
	    QueueIdStack()));
    }
    else if (XmlReader::testStartTag(parser, entry, "IRETURNVALUE"))
    {
	XmlReader::testEndTag(parser, "IRETURNVALUE");

	return(new CIMDeleteInstanceResponseMessage(
	    messageId,
	    CIM_ERR_SUCCESS,
	    String(),
	    QueueIdStack()));
    }
    else
    {
	throw XmlValidationError(parser.getLine(),
	    "expected ERROR or IRETURNVALUE element");
    }
}

CIMGetPropertyResponseMessage* CIMOperationResponseDecoder::_decodeGetPropertyResponse(
    XmlParser& parser, const String& messageId)
{
    XmlEntry entry;
    CIMStatusCode code;
    const char* description = 0;

    if (XmlReader::getErrorElement(parser, code, description))
    {
	return(new CIMGetPropertyResponseMessage(
	    messageId,
	    code,
	    description,
	    QueueIdStack(),
	    CIMValue()));
    }
    else if (XmlReader::testStartTag(parser, entry, "IRETURNVALUE"))
    {
	CIMValue cimValue;

	if (!XmlReader::getPropertyValue(parser, cimValue))
	{
            // ATTN: Don't know what type of CIMValue to expect
	    throw XmlValidationError(
		parser.getLine(),
                "expected VALUE, VALUE.ARRAY, or VALUE.REFERENCE element");
	}

	XmlReader::testEndTag(parser, "IRETURNVALUE");

	return(new CIMGetPropertyResponseMessage(
	    messageId,
	    CIM_ERR_SUCCESS,
	    String(),
	    QueueIdStack(),
	    cimValue));
    }
    else
    {
	throw XmlValidationError(parser.getLine(),
	    "expected ERROR or IRETURNVALUE element");
    }
}

CIMSetPropertyResponseMessage* CIMOperationResponseDecoder::_decodeSetPropertyResponse(
    XmlParser& parser, const String& messageId)
{
    XmlEntry entry;
    CIMStatusCode code;
    const char* description = 0;

    if (XmlReader::getErrorElement(parser, code, description))
    {
	return(new CIMSetPropertyResponseMessage(
	    messageId,
	    code,
	    description,
	    QueueIdStack()));
    }
    else if (XmlReader::testStartTag(parser, entry, "IRETURNVALUE"))
    {
	XmlReader::testEndTag(parser, "IRETURNVALUE");

	return(new CIMSetPropertyResponseMessage(
	    messageId,
	    CIM_ERR_SUCCESS,
	    String(),
	    QueueIdStack()));
    }
    else
    {
	throw XmlValidationError(parser.getLine(),
	    "expected ERROR or IRETURNVALUE element");
    }
}

CIMSetQualifierResponseMessage* CIMOperationResponseDecoder::_decodeSetQualifierResponse(
    XmlParser& parser, const String& messageId)
{
    XmlEntry entry;
    CIMStatusCode code;
    const char* description = 0;

    if (XmlReader::getErrorElement(parser, code, description))
    {
	return(new CIMSetQualifierResponseMessage(
	    messageId,
	    code,
	    description,
	    QueueIdStack()));
    }
    else if (XmlReader::testStartTag(parser, entry, "IRETURNVALUE"))
    {
	XmlReader::testEndTag(parser, "IRETURNVALUE");

	return(new CIMSetQualifierResponseMessage(
	    messageId,
	    CIM_ERR_SUCCESS,
	    String(),
	    QueueIdStack()));
    }
    else
    {
	throw XmlValidationError(parser.getLine(),
	    "expected ERROR or IRETURNVALUE element");
    }
}

CIMGetQualifierResponseMessage* CIMOperationResponseDecoder::_decodeGetQualifierResponse(
    XmlParser& parser, const String& messageId)
{
    XmlEntry entry;
    CIMStatusCode code;
    const char* description = 0;

    if (XmlReader::getErrorElement(parser, code, description))
    {
	return(new CIMGetQualifierResponseMessage(
	    messageId,
	    code,
	    description,
	    QueueIdStack(),
	    CIMQualifierDecl()));
    }
    else if (XmlReader::testStartTag(parser, entry, "IRETURNVALUE"))
    {
	CIMQualifierDecl qualifierDecl;
	XmlReader::getQualifierDeclElement(parser, qualifierDecl);

	XmlReader::testEndTag(parser, "IRETURNVALUE");

	return(new CIMGetQualifierResponseMessage(
	    messageId,
	    CIM_ERR_SUCCESS,
	    String(),
	    QueueIdStack(),
	    qualifierDecl));
    }
    else
    {
	throw XmlValidationError(parser.getLine(),
	    "expected ERROR or IRETURNVALUE element");
    }
}

CIMEnumerateQualifiersResponseMessage* CIMOperationResponseDecoder::_decodeEnumerateQualifiersResponse(
    XmlParser& parser, const String& messageId)
{
    XmlEntry entry;
    CIMStatusCode code;
    const char* description = 0;

    if (XmlReader::getErrorElement(parser, code, description))
    {
	return(new CIMEnumerateQualifiersResponseMessage(
	    messageId,
	    code,
	    description,
	    QueueIdStack(),
	    Array<CIMQualifierDecl>()));
    }
    else if (XmlReader::testStartTag(parser, entry, "IRETURNVALUE"))
    {
	Array<CIMQualifierDecl> qualifierDecls;
	CIMQualifierDecl qualifierDecl;

	while (XmlReader::getQualifierDeclElement(parser, qualifierDecl))
	    qualifierDecls.append(qualifierDecl);

	XmlReader::testEndTag(parser, "IRETURNVALUE");

	return(new CIMEnumerateQualifiersResponseMessage(
	    messageId,
	    CIM_ERR_SUCCESS,
	    String(),
	    QueueIdStack(),
	    qualifierDecls));
    }
    else
    {
	throw XmlValidationError(parser.getLine(),
	    "expected ERROR or IRETURNVALUE element");
    }
}

CIMDeleteQualifierResponseMessage* CIMOperationResponseDecoder::_decodeDeleteQualifierResponse(
    XmlParser& parser, const String& messageId)
{
    XmlEntry entry;
    CIMStatusCode code;
    const char* description = 0;

    if (XmlReader::getErrorElement(parser, code, description))
    {
	return(new CIMDeleteQualifierResponseMessage(
	    messageId,
	    code,
	    description,
	    QueueIdStack()));
    }
    else if (XmlReader::testStartTag(parser, entry, "IRETURNVALUE"))
    {
	XmlReader::testEndTag(parser, "IRETURNVALUE");

	return(new CIMDeleteQualifierResponseMessage(
	    messageId,
	    CIM_ERR_SUCCESS,
	    String(),
	    QueueIdStack()));
    }
    else
    {
	throw XmlValidationError(parser.getLine(),
	    "expected ERROR or IRETURNVALUE element");
    }
}

//MEB:

CIMReferenceNamesResponseMessage* CIMOperationResponseDecoder::_decodeReferenceNamesResponse(
    XmlParser& parser, const String& messageId)
{
    XmlEntry entry;
    CIMStatusCode code;
    const char* description = 0;

    if (XmlReader::getErrorElement(parser, code, description))
    {
	return(new CIMReferenceNamesResponseMessage(
	    messageId,
	    code,
	    description,
	    QueueIdStack(),
	    Array<CIMReference>()));
    }
    else if (XmlReader::testStartTag(parser, entry, "IRETURNVALUE"))
    {
	CIMReference objectPath;
	Array<CIMReference> objectPaths;

	while (XmlReader::getObjectPathElement(parser, objectPath))
	    objectPaths.append(objectPath);

	XmlReader::testEndTag(parser, "IRETURNVALUE");

	return(new CIMReferenceNamesResponseMessage(
	    messageId,
	    CIM_ERR_SUCCESS,
	    String(),
	    QueueIdStack(),
	    objectPaths));
    }
    else
    {
	throw XmlValidationError(parser.getLine(),
	    "expected ERROR or IRETURNVALUE element");
    }
}

CIMReferencesResponseMessage* CIMOperationResponseDecoder::_decodeReferencesResponse(
    XmlParser& parser, const String& messageId)
{
    XmlEntry entry;
    CIMStatusCode code;
    const char* description = 0;

    if (XmlReader::getErrorElement(parser, code, description))
    {
	return(new CIMReferencesResponseMessage(
	    messageId,
	    code,
	    description,
	    QueueIdStack(),
	    Array<CIMObjectWithPath>()));
    }
    else if (XmlReader::testStartTag(parser, entry, "IRETURNVALUE"))
    {
	Array<CIMObjectWithPath> objectWithPathArray;
	CIMObjectWithPath tmp;

	while (XmlReader::getObjectWithPath(parser, tmp))
	    objectWithPathArray.append(tmp);

	XmlReader::testEndTag(parser, "IRETURNVALUE");

	return(new CIMReferencesResponseMessage(
	    messageId,
	    CIM_ERR_SUCCESS,
	    String(),
	    QueueIdStack(),
	    objectWithPathArray));
    }
    else
    {
	throw XmlValidationError(parser.getLine(),
	    "expected ERROR or IRETURNVALUE element");
    }
}

CIMAssociatorNamesResponseMessage* CIMOperationResponseDecoder::_decodeAssociatorNamesResponse(
    XmlParser& parser, const String& messageId)
{
    XmlEntry entry;
    CIMStatusCode code;
    const char* description = 0;

    if (XmlReader::getErrorElement(parser, code, description))
    {
	return(new CIMAssociatorNamesResponseMessage(
	    messageId,
	    code,
	    description,
	    QueueIdStack(),
	    Array<CIMReference>()));
    }
    else if (XmlReader::testStartTag(parser, entry, "IRETURNVALUE"))
    {
	CIMReference objectPath;
	Array<CIMReference> objectPaths;

	while (XmlReader::getObjectPathElement(parser, objectPath))
	    objectPaths.append(objectPath);

	XmlReader::testEndTag(parser, "IRETURNVALUE");

	return(new CIMAssociatorNamesResponseMessage(
	    messageId,
	    CIM_ERR_SUCCESS,
	    String(),
	    QueueIdStack(),
	    objectPaths));
    }
    else
    {
	throw XmlValidationError(parser.getLine(),
	    "expected ERROR or IRETURNVALUE element");
    }
}

CIMAssociatorsResponseMessage* CIMOperationResponseDecoder::_decodeAssociatorsResponse(
    XmlParser& parser, const String& messageId)
{
    XmlEntry entry;
    CIMStatusCode code;
    const char* description = 0;

    if (XmlReader::getErrorElement(parser, code, description))
    {
	return(new CIMAssociatorsResponseMessage(
	    messageId,
	    code,
	    description,
	    QueueIdStack(),
	    Array<CIMObjectWithPath>()));
    }
    else if (XmlReader::testStartTag(parser, entry, "IRETURNVALUE"))
    {
	Array<CIMObjectWithPath> objectWithPathArray;
	CIMObjectWithPath tmp;

	while (XmlReader::getObjectWithPath(parser, tmp))
	    objectWithPathArray.append(tmp);

	XmlReader::testEndTag(parser, "IRETURNVALUE");

	return(new CIMAssociatorsResponseMessage(
	    messageId,
	    CIM_ERR_SUCCESS,
	    String(),
	    QueueIdStack(),
	    objectWithPathArray));
    }
    else
    {
	throw XmlValidationError(parser.getLine(),
	    "expected ERROR or IRETURNVALUE element");
    }
}

CIMInvokeMethodResponseMessage* CIMOperationResponseDecoder::_decodeInvokeMethodResponse(
    XmlParser& parser, const String& messageId, const String& methodName)
{
    XmlEntry entry;
    CIMStatusCode code;
    const char* description = 0;

    CIMValue value;
    Array<CIMParamValue> outParameters;
    const char* paramName;
    String inValue;

    if (XmlReader::getErrorElement(parser, code, description))
    {
	return(new CIMInvokeMethodResponseMessage(
	    messageId,
	    code,
	    description,
	    QueueIdStack(),
	    value,
	    outParameters,
	    methodName));
    }
    else if (XmlReader::testStartTag(parser, entry, "RETURNVALUE"))
    {
	XmlReader::getValueElement(parser, CIMType::STRING, value);

	XmlReader::testEndTag(parser, "RETURNVALUE");

	while (XmlReader::getParamValueTag(parser, paramName))
	{
	    //XmlReader::getValueElement(parser, CIMType::NONE, inValue);
	    XmlReader::getStringValueElement(parser, inValue, true);

	    outParameters.append(CIMParamValue(
		CIMParameter(paramName, CIMType::STRING),
		CIMValue(inValue)));
	
	    XmlReader::expectEndTag(parser, "PARAMVALUE");
	}

	return(new CIMInvokeMethodResponseMessage(
	    messageId,
	    CIM_ERR_SUCCESS,
	    String(),
	    QueueIdStack(),
	    value,
	    outParameters,
	    methodName));
    }
    else
    {
	throw XmlValidationError(parser.getLine(),
	    "expected ERROR or RETURNVALUE element");
    }
}

PEGASUS_NAMESPACE_END
