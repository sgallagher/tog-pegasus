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
//
//              Nag Boranna, Hewlett-Packard Company (nagaraja_boranna@hp.com)
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
#include "CIMExportResponseDecoder.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

CIMExportResponseDecoder::CIMExportResponseDecoder(
    MessageQueue* outputQueue,
    MessageQueue* encoderQueue,
    ClientAuthenticator* authenticator)
    :
   Base("CIMExportResponseDecoder", MessageQueue::getNextQueueId()),
    _outputQueue(outputQueue),
    _encoderQueue(encoderQueue),
    _authenticator(authenticator)
{

}

CIMExportResponseDecoder::~CIMExportResponseDecoder()
{

}

void  CIMExportResponseDecoder::setEncoderQueue(MessageQueue* encoderQueue)
{
    _encoderQueue = encoderQueue;
}

void CIMExportResponseDecoder::handleEnqueue()
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

const char* CIMExportResponseDecoder::getQueueName() const
{
    return "CIMExportResponseDecoder";
}

void CIMExportResponseDecoder::_handleHTTPMessage(HTTPMessage* httpMessage)
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
	headers, "*CIMExport", cimOperation, true))
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

void CIMExportResponseDecoder::_handleMethodResponse(char* content)
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
	// Expect <SIMPLEEXPRSP ... >
	//

	XmlReader::expectStartTag(parser, entry, "SIMPLEEXPRSP");

	//
	// Expect <EXPMETHODRESPONSE ... >
	//

	const char* iMethodResponseName = 0;

	if (!XmlReader::getEMethodResponseStartTag(parser, iMethodResponseName))
	{
	    // ATTN: error ignored for now!

	    return;
	}

	//
	// Dispatch the method:
	//

	if (EqualNoCase(iMethodResponseName, "ExportIndication"))
	    response = _decodeExportIndicationResponse(parser, messageId);
	else
	{
	    // ATTN: this was being ignored; what should we do. Shouldn't
	    // we form an error response?

	    cout << "INFORM: " << __FILE__ << "(" << __LINE__ << "): ";
	    cout << "Unexpected case" << endl;
	    return;
	}
	
	//
	// Handle end tags:
	//

	XmlReader::expectEndTag(parser, "EXPMETHODRESPONSE");
	XmlReader::expectEndTag(parser, "SIMPLEEXPRSP");
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

CIMExportIndicationResponseMessage* CIMExportResponseDecoder::_decodeExportIndicationResponse(
    XmlParser& parser, const String& messageId)
{
    XmlEntry entry;
    CIMStatusCode code;
    const char* description = 0;

    if (XmlReader::getErrorElement(parser, code, description))
    {
	return(new CIMExportIndicationResponseMessage(
	    messageId,
	    code,
	    description,
	    QueueIdStack()));
    }
    else if (XmlReader::testStartTag(parser, entry, "IRETURNVALUE"))
    {
	XmlReader::testEndTag(parser, "IRETURNVALUE");

	return(new CIMExportIndicationResponseMessage(
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

PEGASUS_NAMESPACE_END
