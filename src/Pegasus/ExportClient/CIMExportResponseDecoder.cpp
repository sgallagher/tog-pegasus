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

#include <iostream>
#include <Pegasus/Common/Config.h>
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
    MessageQueue* outputQueue) : _outputQueue(outputQueue)
{

}

CIMExportResponseDecoder::~CIMExportResponseDecoder()
{

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

    httpMessage->parse(startLine, headers, content, contentLength);

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

	if (!XmlReader::getIMethodResponseStartTag(parser, iMethodResponseName))
	{
	    // ATTN: error ignored for now!

	    return;
	}

	//
	// Dispatch the method:
	//

	if (EqualNoCase(iMethodResponseName, "GetClass"))
	    _decodeGetClassResponse(parser, messageId);

	//
	// Handle end tags:
	//

	XmlReader::expectEndTag(parser, "IMETHODRESPONSE");
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
}

void CIMExportResponseDecoder::_decodeGetClassResponse(
    XmlParser& parser, const String& messageId)
{
    XmlEntry entry;
    CIMStatusCode code;
    const char* description = 0;

    if (XmlReader::getErrorElement(parser, code, description))
    {
	_outputQueue->enqueue(new CIMGetClassResponseMessage(
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

	_outputQueue->enqueue(new CIMGetClassResponseMessage(
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

#if 0
void CIMExportClient::deliverIndication(
    const char* address, 
    CIMInstance& indicationInstance, 
    String nameSpace)
{
    if (_channel)
	throw AlreadyConnected();

    ChannelHandlerFactory* factory = new ExportClientHandlerFactory(_selector);

    TCPChannelConnector* connector
	= new TCPChannelConnector(factory, _selector);

    // ATTN-A: need connection timeout here:

    _channel = connector->connect(address);

    if (!_channel)
	throw FailedToConnect();

    String messageId = XmlWriter::getNextMessageId();

    Array<Sint8> params;

    XmlWriter::appendInstanceParameter(
	params, "NewIndication", indicationInstance);
	
    Array<Sint8> message = XmlWriter::formatSimpleIndicationReqMessage(
	_getHostName(), nameSpace, "ExportIndication", messageId, params);

    _channel->writeN(message.getData(), message.size());

    if (!_getHandler()->waitForResponse(_timeOutMilliseconds))
	throw TimedOut();

    CreateInstanceResult* result = _getHandler()->_createInstanceResult;
    CIMStatusCode code = result->code;
    delete result;
    _getHandler()->_createInstanceResult = 0;

    if (code != CIM_ERR_SUCCESS)
	throw CIMException(code);
}
#endif

PEGASUS_NAMESPACE_END
