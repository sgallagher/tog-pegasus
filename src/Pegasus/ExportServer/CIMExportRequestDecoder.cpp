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
//              Nag Boranna, Hewlett-Packard Company (nagaraja_boranna@hp.com)
//              Jenny Yu, Hewlett-Packard Company (jenny_yu@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <cctype>
#include <cstdio>
#include <Pegasus/Common/XmlParser.h>
#include <Pegasus/Common/XmlReader.h>
#include <Pegasus/Common/Destroyer.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/Logger.h>
#include <Pegasus/Config/ConfigManager.h>
#include "CIMExportRequestDecoder.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

CIMExportRequestDecoder::CIMExportRequestDecoder(
    MessageQueue* outputQueue,
    Uint32 returnQueueId)
    : 
    _outputQueue(outputQueue),
    _returnQueueId(returnQueueId)
{
    _serverState = CIMServerState::getInstance();
}

CIMExportRequestDecoder::~CIMExportRequestDecoder()
{

}

void CIMExportRequestDecoder::sendResponse(
    Uint32 queueId, 
    Array<Sint8>& message)
{
    MessageQueue* queue = MessageQueue::lookup(queueId);

    if (queue)
    {
	HTTPMessage* httpMessage = new HTTPMessage(message);
	queue->enqueue(httpMessage);
    }
}

void CIMExportRequestDecoder::sendError(
    Uint32 queueId, 
    const String& messageId,
    const String& cimMethodName,
    CIMStatusCode code,
    const String& description) 
{
    ArrayDestroyer<char> tmp1(cimMethodName.allocateCString());
    ArrayDestroyer<char> tmp2(description.allocateCString());

    Array<Sint8> message = XmlWriter::formatEMethodResponseHeader(
	XmlWriter::formatMessageElement(
	    messageId,
	    XmlWriter::formatSimpleExportRspElement(
		XmlWriter::formatEMethodResponseElement(
		    tmp1.getPointer(),
		    XmlWriter::formatErrorElement(code, tmp2.getPointer())))));
    
    sendResponse(queueId, message);
}

void CIMExportRequestDecoder::handleEnqueue()
{
    Message* message = dequeue();

    if (!message)
	return;

    switch (message->getType())
    {
	case HTTP_MESSAGE:
	    handleHTTPMessage((HTTPMessage*)message);
	    break;
    }

    delete message;
}

const char* CIMExportRequestDecoder::getQueueName() const
{
    return "CIMExportRequestDecoder";
}

//------------------------------------------------------------------------------
//
// From the HTTP/1.1 Specification (RFC 2626):
//
// Both types of message consist of a start-line, zero or more header fields 
// (also known as "headers"), an empty line (i.e., a line with nothing 
// preceding the CRLF) indicating the end of the header fields, and possibly 
// a message-body.
//
// Example CIM request:
//
//     M-POST /cimom HTTP/1.1 
//     HOST: www.erewhon.com 
//     Content-Type: application/xml; charset="utf-8" 
//     Content-Length: xxxx 
//     Man: http://www.dmtf.org/cim/operation ; ns=73 
//     73-CIMExport: MethodRequest
//     73-CIMExportMethod: ExportIndication
//     73-CIMObject: root/cimv2 
// 
//------------------------------------------------------------------------------

void CIMExportRequestDecoder::handleHTTPMessage(HTTPMessage* httpMessage)
{
    // Save queueId:

    Uint32 queueId = httpMessage->queueId;

    // Save userName:

    String userName;

    if ( httpMessage->authInfo->isAuthenticated() )
    {
        userName = httpMessage->authInfo->getAuthenticatedUser();
    }

    // Parse the HTTP message:

    String startLine;
    Array<HTTPHeader> headers;
    Sint8* content;
    Uint32 contentLength;

    httpMessage->parse(startLine, headers, contentLength);

    // Parse the request line:

    String methodName;
    String requestUri;
    String httpVersion;

    HTTPMessage::parseRequestLine(
        startLine, methodName, requestUri, httpVersion);

    // Process M-POST and POST messages:

    if (methodName == "M-POST" || methodName == "POST")
    {
	String url;

        if (HTTPMessage::lookupHeader(
	    headers, "*HOST", url, true))
	   _url = url;
        else
           return;

	// Search for "CIMOperation" header:
	String cimOperation;

	if (!HTTPMessage::lookupHeader(
	    headers, "*CIMExport", cimOperation, true))
	{
	    // ATTN: error discarded at this time!
	    return;
	}

	// Zero-terminate the message:

	httpMessage->message.append('\0');

	// Calculate the beginning of the content from the message size and
	// the content length.  Subtract 1 to take into account the null
	// character we just added to the end of the message.

	content = (Sint8*) httpMessage->message.getData() +
	    httpMessage->message.size() - contentLength - 1;

	// If it is a method call, then dispatch it to be handled:

	if (!String::equalNoCase(cimOperation, "MethodRequest"))
	{
	    // ATTN: error discarded at this time!
	    return;
	}

        handleMethodRequest(queueId, content, userName);
    }
}


void CIMExportRequestDecoder::handleMethodRequest(
    Uint32 queueId,
    Sint8* content,
    String userName)
{
    Message* request;

    //
    // get the configured authentication flag
    //
    ConfigManager* configManager = ConfigManager::getInstance();

    Boolean requireAuthentication = false;
    Boolean requireAuthorization = false;

    if (String::equal(
        configManager->getCurrentValue("requireAuthentication"), "true"))
    {
        requireAuthentication = true;
    }

    if (String::equal(
        configManager->getCurrentValue("requireAuthorization"), "true"))
    {
        requireAuthorization = true;
    }

    // Create a parser:

    XmlParser parser(content);
    XmlEntry entry;
    String messageId;
    const char* cimMethodName = "";

    try
    {
	// Expect <?xml ...>

	XmlReader::expectXmlDeclaration(parser, entry);

	// Expect <CIM ...>

	XmlReader::testCimStartTag(parser);

	// Expect <MESSAGE ...>

	const char* protocolVersion = 0;

	if (!XmlReader::getMessageStartTag(
	    parser, messageId, protocolVersion))
	{
	    throw XmlValidationError(
		parser.getLine(), "expected MESSAGE element");
	}

	if (strcmp(protocolVersion, "1.0") != 0)
	{
	    throw XmlSemanticError(parser.getLine(), 
		"Expected MESSAGE.PROTOCOLVERSION to be \"1.0\"");
	}
    }
    catch (Exception&)
    {
	// ATTN: no error can be sent back to the client yet since
	// errors require a message-id (which was in the process of
	// being obtained above).
	return;
    }

    try
    {
	// Expect <SIMPLEEXPREQ ...>

	XmlReader::expectStartTag(parser, entry, "SIMPLEEXPREQ");
	
	// Expect <EXPMETHODCALL ...>

	if (!XmlReader::getEMethodCallStartTag(parser, cimMethodName))
	{
	    throw XmlValidationError(parser.getLine(), 
		"expected EXPMETHODCALL element");
	}
	
	// Expect <LOCALNAMESPACEPATH ...>

	String nameSpace;

	//if (!XmlReader::getLocalNameSpacePathElement(parser, nameSpace))
	//{
	//    throw XmlValidationError(parser.getLine(), 
	//	"expected LOCALNAMESPACEPATH element");
	//}

	// Delegate to appropriate method to handle:

	if (CompareNoCase(cimMethodName, "ExportIndication") == 0)
        {
            //
            // If CIMOM is shutting down, return error response
            //
            // ATTN:  need to define a new CIM Error.
            //
            if (_serverState->getState() == CIMServerState::TERMINATING)
            {
                String description = "CIMServer is shutting down.  Request cannot be processed: ";
                description += cimMethodName;

                sendError(
                    queueId,
                    messageId,
                    cimMethodName,
                    CIM_ERR_FAILED,
                    description);

                return;
            }
            else
            {
	        request = decodeExportIndicationRequest(queueId, parser, messageId, _url);
            }
        }
	else
	{
	    String description = "Unknown intrinsic method: ";
	    description += cimMethodName;

	    sendError(
		queueId, 
		messageId,
		cimMethodName,
		CIM_ERR_FAILED,
		description);

	    return;
	}

	// Expect </EXPMETHODCALL>

	XmlReader::expectEndTag(parser, "EXPMETHODCALL");

	// Expect </SIMPLEEXPREQ>

	XmlReader::expectEndTag(parser, "SIMPLEEXPREQ");

	// Expect </MESSAGE>

	XmlReader::expectEndTag(parser, "MESSAGE");

	// Expect </CIM>

	XmlReader::expectEndTag(parser, "CIM");
    }
    catch (Exception& e)
    {
	sendError(
	    queueId, 
	    messageId,
	    cimMethodName,
	    CIM_ERR_FAILED,
	    e.getMessage());
        return;
    }

    _outputQueue->enqueue(request);
}

CIMExportIndicationRequestMessage* CIMExportRequestDecoder::decodeExportIndicationRequest(
    Uint32 queueId,
    XmlParser& parser, 
    const String& messageId,
    const String& url)
{
    // ATTN: handle property lists!

    CIMInstance instanceName;

    for (const char* name; XmlReader::getIParamValueTag(parser, name);)
    {
	if (CompareNoCase(name, "NewIndication") == 0)
	    XmlReader::getInstanceElement(parser, instanceName);

	XmlReader::expectEndTag(parser, "IPARAMVALUE");
    }
    
    CIMExportIndicationRequestMessage* request = new CIMExportIndicationRequestMessage(
	messageId,  
	url,
	instanceName,
	QueueIdStack(queueId, _returnQueueId));
    
    return(request);
}

PEGASUS_NAMESPACE_END
