//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software, Hewlett-Packard Company, IBM,
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
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                  (carolann_graves@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <iostream>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/XmlParser.h>
#include <Pegasus/Common/XmlReader.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/HTTPMessage.h>
#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Common/Exception.h> 
#include "CIMOperationResponseDecoder.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

CIMOperationResponseDecoder::CIMOperationResponseDecoder(
    MessageQueue* outputQueue,
    MessageQueue* encoderQueue,
    ClientAuthenticator* authenticator)
    :
    MessageQueue(PEGASUS_QUEUENAME_OPRESPDECODER),
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
            PEGASUS_ASSERT(0);
	    break;
    }

    delete message;
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

    if (httpMessage->message.size() == 0)
    {
        CIMClientMalformedHTTPException* malformedHTTPException =
            new CIMClientMalformedHTTPException("Empty HTTP response message.");
        ClientExceptionMessage * response =
            new ClientExceptionMessage(malformedHTTPException);

        _outputQueue->enqueue(response);
        return;
    }

    httpMessage->parse(startLine, headers, contentLength);

    //
    // Get the status line info
    //

    String httpVersion;
    Uint32 statusCode;
    String reasonPhrase;

    Boolean parsableMessage = HTTPMessage::parseStatusLine(
        startLine, httpVersion, statusCode, reasonPhrase);
    if (!parsableMessage)
    {
        CIMClientMalformedHTTPException* malformedHTTPException = new
            CIMClientMalformedHTTPException("Malformed HTTP response message.");
        ClientExceptionMessage * response =
            new ClientExceptionMessage(malformedHTTPException);

        _outputQueue->enqueue(response);
        return;
    }

    try
    {
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
    }
    catch(InvalidAuthHeader& e)
    {
        CIMClientMalformedHTTPException* malformedHTTPException =
            new CIMClientMalformedHTTPException(e.getMessage());
        ClientExceptionMessage * response =
            new ClientExceptionMessage(malformedHTTPException);

        _outputQueue->enqueue(response);
        return;
    }
    catch(UnauthorizedAccess& e)
    {
         CannotConnectException* cannotConnectException =
            new CannotConnectException(e.getMessage());
	 ClientExceptionMessage * response =
            new ClientExceptionMessage(cannotConnectException);

        _outputQueue->enqueue(response);
        return;
    }

    //
    // Check for a success (200 OK) response
    //

    if (statusCode != HTTP_STATUSCODE_OK)
    {
        String cimError;
        String pegasusError;

        HTTPMessage::lookupHeader(headers, "CIMError", cimError);
        HTTPMessage::lookupHeader(headers, PEGASUS_HTTPHEADERTAG_ERRORDETAIL, pegasusError);

        CIMClientHTTPError* httpError =
            new CIMClientHTTPError(statusCode, cimError, pegasusError);
        ClientExceptionMessage * response =
            new ClientExceptionMessage(httpError);

        _outputQueue->enqueue(response);
        return;
    }

    //
    // Search for "CIMOperation" header:
    //

    String cimOperation;

    if (!HTTPMessage::lookupHeader(
	headers, "CIMOperation", cimOperation, true))
    {
        CIMClientMalformedHTTPException* malformedHTTPException = new
            CIMClientMalformedHTTPException("Missing CIMOperation HTTP header");
        ClientExceptionMessage * response =
            new ClientExceptionMessage(malformedHTTPException);

        _outputQueue->enqueue(response);
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
        CIMClientMalformedHTTPException* malformedHTTPException =
            new CIMClientMalformedHTTPException(
                String("Received CIMOperation HTTP header value \"") +
                cimOperation + "\", expected \"MethodResponse\"");
        ClientExceptionMessage * response =
            new ClientExceptionMessage(malformedHTTPException);

        _outputQueue->enqueue(response);
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

        const char* xmlVersion = 0;
        const char* xmlEncoding = 0;

	XmlReader::getXmlDeclaration(parser, xmlVersion, xmlEncoding);

	//
	// Process <CIM ... >
	//

        const char* cimVersion = 0;
        const char* dtdVersion = 0;

        // ATTN-RK-P3-20020416: Need to validate these versions?
	XmlReader::getCimStartTag(parser, cimVersion, dtdVersion);

	//
	// Expect <MESSAGE ... >
	//

	String messageId;
	String protocolVersion;

	if (!XmlReader::getMessageStartTag(parser, messageId, protocolVersion))
	    throw XmlValidationError(
		parser.getLine(), "expected MESSAGE element");

        if (!String::equalNoCase(protocolVersion, "1.0"))
	{
            CIMClientResponseException* responseException =
                new CIMClientResponseException(
                    String("Received unsupported protocol version \"") +
                    protocolVersion + "\", expected \"1.0\"");
            ClientExceptionMessage * response =
                new ClientExceptionMessage(responseException);

            _outputQueue->enqueue(response);
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

	    if (System::strcasecmp(iMethodResponseName, "GetClass") == 0)
		response = _decodeGetClassResponse(parser, messageId);
	    else if (System::strcasecmp(iMethodResponseName, "GetInstance") == 0)
		response = _decodeGetInstanceResponse(parser, messageId);
	    else if (System::strcasecmp(iMethodResponseName, "EnumerateClassNames") == 0)
		response = _decodeEnumerateClassNamesResponse(parser,messageId);
	    else if (System::strcasecmp(iMethodResponseName, "References") == 0)
		response = _decodeReferencesResponse(parser, messageId);
	    else if (System::strcasecmp(iMethodResponseName, "ReferenceNames") == 0)
		response = _decodeReferenceNamesResponse(parser, messageId);
	    else if (System::strcasecmp(iMethodResponseName, "AssociatorNames") == 0)
		response = _decodeAssociatorNamesResponse(parser, messageId);
	    else if (System::strcasecmp(iMethodResponseName, "Associators") == 0)
		response = _decodeAssociatorsResponse(parser, messageId);
	    else if (System::strcasecmp(iMethodResponseName, "CreateInstance") == 0)
		response = _decodeCreateInstanceResponse(parser, messageId);
	    else if (System::strcasecmp(iMethodResponseName,"EnumerateInstanceNames") == 0)
		response = _decodeEnumerateInstanceNamesResponse(
                  parser, messageId);
	    else if (System::strcasecmp(iMethodResponseName,"EnumerateInstances") == 0)
		response = _decodeEnumerateInstancesResponse(parser, messageId);
	    else if (System::strcasecmp(iMethodResponseName, "GetProperty") == 0)
		response = _decodeGetPropertyResponse(parser, messageId);
	    else if (System::strcasecmp(iMethodResponseName, "SetProperty") == 0)
		response = _decodeSetPropertyResponse(parser, messageId);
	    else if (System::strcasecmp(iMethodResponseName, "DeleteQualifier") == 0)
		response = _decodeDeleteQualifierResponse(parser, messageId);
	    else if (System::strcasecmp(iMethodResponseName, "GetQualifier") == 0)
		response = _decodeGetQualifierResponse(parser, messageId);
	    else if (System::strcasecmp(iMethodResponseName, "SetQualifier") == 0)
		response = _decodeSetQualifierResponse(parser, messageId);
	    else if (System::strcasecmp(iMethodResponseName, "EnumerateQualifiers") == 0)
		response = _decodeEnumerateQualifiersResponse(parser,messageId);
	    else if (System::strcasecmp(iMethodResponseName, "EnumerateClasses") == 0)
		response = _decodeEnumerateClassesResponse(parser, messageId);
	    else if (System::strcasecmp(iMethodResponseName, "CreateClass") == 0)
		response = _decodeCreateClassResponse(parser, messageId);
	    else if (System::strcasecmp(iMethodResponseName, "ModifyClass") == 0)
		response = _decodeModifyClassResponse(parser, messageId);
	    else if (System::strcasecmp(iMethodResponseName, "ModifyInstance") == 0)
		response = _decodeModifyInstanceResponse(parser, messageId);
	    else if (System::strcasecmp(iMethodResponseName, "DeleteClass") == 0)
		response = _decodeDeleteClassResponse(parser, messageId);
	    else if (System::strcasecmp(iMethodResponseName, "DeleteInstance") == 0)
		response = _decodeDeleteInstanceResponse(parser, messageId);
	    else if (System::strcasecmp(iMethodResponseName, "ExecQuery") == 0)
		response = _decodeExecQueryResponse(parser, messageId);
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
    catch (XmlException& x)
    {
        if (response)
        {
            delete response;
        }

        response = new ClientExceptionMessage(
            new CIMClientXmlException(x.getMessage()));
    }
    catch (Exception& x)
    {
        // Shouldn't ever get exceptions other than XmlExceptions.
        PEGASUS_ASSERT(0);

        if (response)
        {
            delete response;
        }

        response = new ClientExceptionMessage(
            new Exception(x.getMessage()));
    }

    _outputQueue->enqueue(response);
}

CIMCreateClassResponseMessage* CIMOperationResponseDecoder::_decodeCreateClassResponse(
    XmlParser& parser, 
    const String& messageId)
{
    XmlEntry entry;
    CIMException cimException;

    if (XmlReader::getErrorElement(parser, cimException))
    {
	return(new CIMCreateClassResponseMessage(
	    messageId,
	    cimException,
	    QueueIdStack()));

    }
    else
    {
	if (XmlReader::testStartTag(parser, entry, "IRETURNVALUE"))
	{
	    XmlReader::expectEndTag(parser, "IRETURNVALUE");
	}

	return(new CIMCreateClassResponseMessage(
	    messageId,
	    cimException,
	    QueueIdStack()));
    }
}

CIMGetClassResponseMessage* CIMOperationResponseDecoder::_decodeGetClassResponse(
    XmlParser& parser, const String& messageId)
{
    XmlEntry entry;
    CIMException cimException;

    if (XmlReader::getErrorElement(parser, cimException))
    {
	return(new CIMGetClassResponseMessage(
	    messageId,
	    cimException,
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
	    cimException,
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
    CIMException cimException;

    if (XmlReader::getErrorElement(parser, cimException))
    {
	return(new CIMModifyClassResponseMessage(
	    messageId,
	    cimException,
	    QueueIdStack()));
    }
    else
    {
	if (XmlReader::testStartTag(parser, entry, "IRETURNVALUE"))
	{
	    XmlReader::expectEndTag(parser, "IRETURNVALUE");
	}

	return(new CIMModifyClassResponseMessage(
	    messageId,
	    cimException,
	    QueueIdStack()));
    }
}

CIMEnumerateClassNamesResponseMessage* CIMOperationResponseDecoder::_decodeEnumerateClassNamesResponse(
    XmlParser& parser, const String& messageId)
{
    XmlEntry entry;
    CIMException cimException;

    if (XmlReader::getErrorElement(parser, cimException))
    {
	return(new CIMEnumerateClassNamesResponseMessage(
	    messageId,
	    cimException,
	    QueueIdStack(),
	    Array<String>()));
    }
    else
    {
	Array<String> classNames;

	if (XmlReader::testStartTag(parser, entry, "IRETURNVALUE"))
	{
	    String className;

	    while (XmlReader::getClassNameElement(parser, className, false))
	        classNames.append(className);

	    XmlReader::expectEndTag(parser, "IRETURNVALUE");
	}

	return(new CIMEnumerateClassNamesResponseMessage(
	    messageId,
	    cimException,
	    QueueIdStack(),
	    classNames));
    }
}

CIMEnumerateClassesResponseMessage* CIMOperationResponseDecoder::_decodeEnumerateClassesResponse(
    XmlParser& parser, const String& messageId)
{
    XmlEntry entry;
    CIMException cimException;

    if (XmlReader::getErrorElement(parser, cimException))
    {
	return(new CIMEnumerateClassesResponseMessage(
	    messageId,
	    cimException,
	    QueueIdStack(),
	    Array<CIMClass>()));
    }
    else
    {
	Array<CIMClass> cimClasses;

	if (XmlReader::testStartTag(parser, entry, "IRETURNVALUE"))
	{
	    CIMClass cimClass;

	    while (XmlReader::getClassElement(parser, cimClass))
	        cimClasses.append(cimClass);

	    XmlReader::expectEndTag(parser, "IRETURNVALUE");
	}

	return(new CIMEnumerateClassesResponseMessage(
	    messageId,
	    cimException,
	    QueueIdStack(),
	    cimClasses));
    }
}

CIMDeleteClassResponseMessage* CIMOperationResponseDecoder::_decodeDeleteClassResponse(
    XmlParser& parser, const String& messageId)
{
    XmlEntry entry;
    CIMException cimException;

    if (XmlReader::getErrorElement(parser, cimException))
    {
	return(new CIMDeleteClassResponseMessage(
	    messageId,
	    cimException,
	    QueueIdStack()));
    }
    else
    {
	if (XmlReader::testStartTag(parser, entry, "IRETURNVALUE"))
	{
	    XmlReader::expectEndTag(parser, "IRETURNVALUE");
	}

	return(new CIMDeleteClassResponseMessage(
	    messageId,
	    cimException,
	    QueueIdStack()));
    }
}

CIMCreateInstanceResponseMessage* CIMOperationResponseDecoder::_decodeCreateInstanceResponse(
    XmlParser& parser, const String& messageId)
{
    XmlEntry entry;
    CIMException cimException;

    if (XmlReader::getErrorElement(parser, cimException))
    {
	return(new CIMCreateInstanceResponseMessage(
	    messageId,
	    cimException,
	    QueueIdStack(),
	    CIMObjectPath()));
    }
    else if (XmlReader::testStartTag(parser, entry, "IRETURNVALUE"))
    {
	CIMObjectPath instanceName;
	XmlReader::getInstanceNameElement(parser, instanceName);

	XmlReader::testEndTag(parser, "IRETURNVALUE");

	return(new CIMCreateInstanceResponseMessage(
	    messageId,
	    cimException,
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
    CIMException cimException;

    if (XmlReader::getErrorElement(parser, cimException))
    {
	return(new CIMGetInstanceResponseMessage(
	    messageId,
	    cimException,
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
	    cimException,
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
    CIMException cimException;

    if (XmlReader::getErrorElement(parser, cimException))
    {
	return(new CIMModifyInstanceResponseMessage(
	    messageId,
	    cimException,
	    QueueIdStack()));
    }
    else
    {
	if (XmlReader::testStartTag(parser, entry, "IRETURNVALUE"))
	{
	    XmlReader::expectEndTag(parser, "IRETURNVALUE");
	}

	return(new CIMModifyInstanceResponseMessage(
	    messageId,
	    cimException,
	    QueueIdStack()));
    }
}

CIMEnumerateInstanceNamesResponseMessage* CIMOperationResponseDecoder::_decodeEnumerateInstanceNamesResponse(
    XmlParser& parser, const String& messageId)
{
    XmlEntry entry;
    CIMException cimException;

    if (XmlReader::getErrorElement(parser, cimException))
    {
	return(new CIMEnumerateInstanceNamesResponseMessage(
	    messageId,
	    cimException,
	    QueueIdStack(),
	    Array<CIMObjectPath>()));
    }
    else
    {
	Array<CIMObjectPath> instanceNames;

	if (XmlReader::testStartTag(parser, entry, "IRETURNVALUE"))
	{
	    String className;
	    Array<KeyBinding> keyBindings;

	    while (XmlReader::getInstanceNameElement(
	        parser, className, keyBindings))
	    {
	        CIMObjectPath r(
		    String::EMPTY,
		    String::EMPTY,
		    className,
		    keyBindings);
	        instanceNames.append(r);
	    }

	    XmlReader::expectEndTag(parser, "IRETURNVALUE");
	}

	return(new CIMEnumerateInstanceNamesResponseMessage(
	    messageId,
	    cimException,
	    QueueIdStack(),
	    instanceNames));
    }
}

CIMEnumerateInstancesResponseMessage* CIMOperationResponseDecoder::_decodeEnumerateInstancesResponse(
    XmlParser& parser, const String& messageId)
{
    XmlEntry entry;
    CIMException cimException;

    if (XmlReader::getErrorElement(parser, cimException))
    {
	return(new CIMEnumerateInstancesResponseMessage(
	    messageId,
	    cimException,
	    QueueIdStack(),
	    Array<CIMInstance>()));
    }
    else
    {
	Array<CIMInstance> namedInstances;

	if (XmlReader::testStartTag(parser, entry, "IRETURNVALUE"))
	{
	    CIMInstance namedInstance;

	    while (XmlReader::getNamedInstanceElement(parser, namedInstance))
	        namedInstances.append(namedInstance);

	    XmlReader::expectEndTag(parser, "IRETURNVALUE");
	}

	return(new CIMEnumerateInstancesResponseMessage(
	    messageId,
	    cimException,
	    QueueIdStack(),
	    namedInstances));
    }
}

CIMDeleteInstanceResponseMessage* CIMOperationResponseDecoder::_decodeDeleteInstanceResponse(
    XmlParser& parser, const String& messageId)
{
    XmlEntry entry;
    CIMException cimException;

    if (XmlReader::getErrorElement(parser, cimException))
    {
	return(new CIMDeleteInstanceResponseMessage(
	    messageId,
	    cimException,
	    QueueIdStack()));
    }
    else
    {
	if (XmlReader::testStartTag(parser, entry, "IRETURNVALUE"))
	{
	    XmlReader::expectEndTag(parser, "IRETURNVALUE");
	}

	return(new CIMDeleteInstanceResponseMessage(
	    messageId,
	    cimException,
	    QueueIdStack()));
    }
}

CIMGetPropertyResponseMessage* CIMOperationResponseDecoder::_decodeGetPropertyResponse(
    XmlParser& parser, const String& messageId)
{
    XmlEntry entry;
    CIMException cimException;

    if (XmlReader::getErrorElement(parser, cimException))
    {
	return(new CIMGetPropertyResponseMessage(
	    messageId,
	    cimException,
	    QueueIdStack(),
	    CIMValue()));
    }
    else
    {
	CIMValue cimValue(CIMTYPE_STRING, false);

	if (XmlReader::testStartTag(parser, entry, "IRETURNVALUE"))
	{
	    if (!XmlReader::getPropertyValue(parser, cimValue))
	    {
                // No value given; just return a null String value
	    }

	    XmlReader::expectEndTag(parser, "IRETURNVALUE");
	}
	else
	{
            // No value given; just return a null String value
	}

	return(new CIMGetPropertyResponseMessage(
	    messageId,
	    cimException,
	    QueueIdStack(),
	    cimValue));
    }
}

CIMSetPropertyResponseMessage* CIMOperationResponseDecoder::_decodeSetPropertyResponse(
    XmlParser& parser, const String& messageId)
{
    XmlEntry entry;
    CIMException cimException;

    if (XmlReader::getErrorElement(parser, cimException))
    {
	return(new CIMSetPropertyResponseMessage(
	    messageId,
	    cimException,
	    QueueIdStack()));
    }
    else
    {
	if (XmlReader::testStartTag(parser, entry, "IRETURNVALUE"))
	{
	    XmlReader::expectEndTag(parser, "IRETURNVALUE");
	}

	return(new CIMSetPropertyResponseMessage(
	    messageId,
	    cimException,
	    QueueIdStack()));
    }
}

CIMSetQualifierResponseMessage* CIMOperationResponseDecoder::_decodeSetQualifierResponse(
    XmlParser& parser, const String& messageId)
{
    XmlEntry entry;
    CIMException cimException;

    if (XmlReader::getErrorElement(parser, cimException))
    {
	return(new CIMSetQualifierResponseMessage(
	    messageId,
	    cimException,
	    QueueIdStack()));
    }
    else
    {
	if (XmlReader::testStartTag(parser, entry, "IRETURNVALUE"))
	{
	    XmlReader::expectEndTag(parser, "IRETURNVALUE");
	}

	return(new CIMSetQualifierResponseMessage(
	    messageId,
	    cimException,
	    QueueIdStack()));
    }
}

CIMGetQualifierResponseMessage* CIMOperationResponseDecoder::_decodeGetQualifierResponse(
    XmlParser& parser, const String& messageId)
{
    XmlEntry entry;
    CIMException cimException;

    if (XmlReader::getErrorElement(parser, cimException))
    {
	return(new CIMGetQualifierResponseMessage(
	    messageId,
	    cimException,
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
	    cimException,
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
    CIMException cimException;

    if (XmlReader::getErrorElement(parser, cimException))
    {
	return(new CIMEnumerateQualifiersResponseMessage(
	    messageId,
	    cimException,
	    QueueIdStack(),
	    Array<CIMQualifierDecl>()));
    }
    else
    {
	Array<CIMQualifierDecl> qualifierDecls;

	if (XmlReader::testStartTag(parser, entry, "IRETURNVALUE"))
	{
	    CIMQualifierDecl qualifierDecl;

	    while (XmlReader::getQualifierDeclElement(parser, qualifierDecl))
	        qualifierDecls.append(qualifierDecl);

	    XmlReader::expectEndTag(parser, "IRETURNVALUE");
	}

	return(new CIMEnumerateQualifiersResponseMessage(
	    messageId,
	    cimException,
	    QueueIdStack(),
	    qualifierDecls));
    }
}

CIMDeleteQualifierResponseMessage* CIMOperationResponseDecoder::_decodeDeleteQualifierResponse(
    XmlParser& parser, const String& messageId)
{
    XmlEntry entry;
    CIMException cimException;

    if (XmlReader::getErrorElement(parser, cimException))
    {
	return(new CIMDeleteQualifierResponseMessage(
	    messageId,
	    cimException,
	    QueueIdStack()));
    }
    else
    {
	if (XmlReader::testStartTag(parser, entry, "IRETURNVALUE"))
	{
	    XmlReader::expectEndTag(parser, "IRETURNVALUE");
	}

	return(new CIMDeleteQualifierResponseMessage(
	    messageId,
	    cimException,
	    QueueIdStack()));
    }
}

//MEB:

CIMReferenceNamesResponseMessage* CIMOperationResponseDecoder::_decodeReferenceNamesResponse(
    XmlParser& parser, const String& messageId)
{
    XmlEntry entry;
    CIMException cimException;

    if (XmlReader::getErrorElement(parser, cimException))
    {
	return(new CIMReferenceNamesResponseMessage(
	    messageId,
	    cimException,
	    QueueIdStack(),
	    Array<CIMObjectPath>()));
    }
    else
    {
	Array<CIMObjectPath> objectPaths;

	if (XmlReader::testStartTag(parser, entry, "IRETURNVALUE"))
	{
	    CIMObjectPath objectPath;

	    while (XmlReader::getObjectPathElement(parser, objectPath))
	        objectPaths.append(objectPath);

	    XmlReader::expectEndTag(parser, "IRETURNVALUE");
	}

	return(new CIMReferenceNamesResponseMessage(
	    messageId,
	    cimException,
	    QueueIdStack(),
	    objectPaths));
    }
}

CIMReferencesResponseMessage* CIMOperationResponseDecoder::_decodeReferencesResponse(
    XmlParser& parser, const String& messageId)
{
    XmlEntry entry;
    CIMException cimException;

    if (XmlReader::getErrorElement(parser, cimException))
    {
	return(new CIMReferencesResponseMessage(
	    messageId,
	    cimException,
	    QueueIdStack(),
	    Array<CIMObject>()));
    }
    else
    {
	Array<CIMObject> objectWithPathArray;

	if (XmlReader::testStartTag(parser, entry, "IRETURNVALUE"))
	{
	    CIMObject objectWithPath;

	    while (XmlReader::getValueObjectWithPathElement(parser, objectWithPath))
	        objectWithPathArray.append(objectWithPath);

	    XmlReader::expectEndTag(parser, "IRETURNVALUE");
	}

	return(new CIMReferencesResponseMessage(
	    messageId,
	    cimException,
	    QueueIdStack(),
	    objectWithPathArray));
    }
}

CIMAssociatorNamesResponseMessage* CIMOperationResponseDecoder::_decodeAssociatorNamesResponse(
    XmlParser& parser, const String& messageId)
{
    XmlEntry entry;
    CIMException cimException;

    if (XmlReader::getErrorElement(parser, cimException))
    {
	return(new CIMAssociatorNamesResponseMessage(
	    messageId,
	    cimException,
	    QueueIdStack(),
	    Array<CIMObjectPath>()));
    }
    else
    {
	Array<CIMObjectPath> objectPaths;

	if (XmlReader::testStartTag(parser, entry, "IRETURNVALUE"))
	{
	    CIMObjectPath objectPath;

	    while (XmlReader::getObjectPathElement(parser, objectPath))
	        objectPaths.append(objectPath);

	    XmlReader::expectEndTag(parser, "IRETURNVALUE");
	}

	return(new CIMAssociatorNamesResponseMessage(
	    messageId,
	    cimException,
	    QueueIdStack(),
	    objectPaths));
    }
}

CIMAssociatorsResponseMessage* CIMOperationResponseDecoder::_decodeAssociatorsResponse(
    XmlParser& parser, const String& messageId)
{
    XmlEntry entry;
    CIMException cimException;

    if (XmlReader::getErrorElement(parser, cimException))
    {
	return(new CIMAssociatorsResponseMessage(
	    messageId,
	    cimException,
	    QueueIdStack(),
	    Array<CIMObject>()));
    }
    else
    {
	Array<CIMObject> objectWithPathArray;

	if (XmlReader::testStartTag(parser, entry, "IRETURNVALUE"))
	{
	    CIMObject objectWithPath;

	    while (XmlReader::getValueObjectWithPathElement(parser, objectWithPath))
	        objectWithPathArray.append(objectWithPath);

	    XmlReader::expectEndTag(parser, "IRETURNVALUE");
	}

	return(new CIMAssociatorsResponseMessage(
	    messageId,
	    cimException,
	    QueueIdStack(),
	    objectWithPathArray));
    }
}

CIMExecQueryResponseMessage* CIMOperationResponseDecoder::_decodeExecQueryResponse(
    XmlParser& parser, const String& messageId)
{
    XmlEntry entry;
    CIMException cimException;

    if (XmlReader::getErrorElement(parser, cimException))
    {
	return(new CIMExecQueryResponseMessage(
	    messageId,
	    cimException,
	    QueueIdStack(),
	    Array<CIMObject>()));
    }
    else
    {
	Array<CIMObject> objectWithPathArray;

	if (XmlReader::testStartTag(parser, entry, "IRETURNVALUE"))
	{
            XmlReader::getObjectArray(parser, objectWithPathArray);

	    XmlReader::expectEndTag(parser, "IRETURNVALUE");
	}

	return(new CIMExecQueryResponseMessage(
	    messageId,
	    cimException,
	    QueueIdStack(),
	    objectWithPathArray));
    }
}

CIMInvokeMethodResponseMessage* CIMOperationResponseDecoder::_decodeInvokeMethodResponse(
    XmlParser& parser, const String& messageId, const String& methodName)
{
    XmlEntry entry;
    CIMException cimException;

    CIMParamValue paramValue;
    Array<CIMParamValue> outParameters;
    CIMValue returnValue;
    const char* paramName;
    String inValue;

    if (XmlReader::getErrorElement(parser, cimException))
    {
	return(new CIMInvokeMethodResponseMessage(
	    messageId,
	    cimException,
	    QueueIdStack(),
	    returnValue,
	    outParameters,
	    methodName));
    }
    else
    {
        Boolean isReturnValue = false;
        Boolean isParamValue = false;
        Boolean gotReturnValue = false;

        while ((isReturnValue =
                    XmlReader::getReturnValueElement(parser, returnValue)) ||
               (isParamValue =
		    XmlReader::getParamValueElement(parser, paramValue)))
        {
            if (isReturnValue)
            {
                if (gotReturnValue)
                {
	            throw XmlValidationError(parser.getLine(),
	                "unexpected RETURNVALUE element");
                }
                gotReturnValue = true;
            }
            else    // isParamValue == true
            {
	        outParameters.append(paramValue);
            }

            isReturnValue = false;
            isParamValue = false;
        }

	return(new CIMInvokeMethodResponseMessage(
	    messageId,
	    cimException,
	    QueueIdStack(),
	    returnValue,
	    outParameters,
	    methodName));
    }
}

PEGASUS_NAMESPACE_END
