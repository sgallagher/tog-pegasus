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
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/HTTPAcceptor.h>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/HTTPConnection.h>
#include <Pegasus/Common/HTTPMessage.h>

#include "HTTPDelegator.h"

PEGASUS_NAMESPACE_BEGIN

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

HTTPDelegator::HTTPDelegator(
    MessageQueue* operationMessageQueue,
    MessageQueue* exportMessageQueue)
    : 
    _operationMessageQueue(operationMessageQueue),
    _exportMessageQueue(exportMessageQueue)
{
    
}

HTTPDelegator::~HTTPDelegator()
{

}

void HTTPDelegator::handleEnqueue()
{
    // cout << "HTTPDelegator::handleEnqueue()" << endl;

    Message* message = dequeue();
    if (message)
    {
	if (message->getType() == HTTP_MESSAGE)
	{
	    handleHTTPMessage((HTTPMessage*)message);
	}

	//delete message;
    }
}

void HTTPDelegator::handleHTTPMessage(HTTPMessage* httpMessage)
{
 
    // Save queueId:

    Uint32 queueId = httpMessage->queueId;

    // Parse the HTTP message:
    String startLine;
    Array<HTTPHeader> headers;
    Sint8* content;
    Uint32 contentLength;

    httpMessage->parse(startLine, headers, content, contentLength);
    
    // Parse the request line:

    String methodName;
    String requestUri;
    String httpVersion;

    HTTPMessage::parseRequestLine(
        startLine, methodName, requestUri, httpVersion);

    // Process M-POST and POST messages:

    if (methodName == "M-POST" || methodName == "POST")
    {
	// Search for "CIMOperation" header:

	String cimOperation;
	httpMessage->message.append('\0');

	//SocketMessage* socketMessage = (SocketMessage*)httpMessage;

	if (HTTPMessage::lookupHeader(
	    headers, "*CIMOperation", cimOperation, true))
	{
	    //_operationMessageQueue->enqueue(socketMessage);
	    _operationMessageQueue->enqueue(httpMessage);
	}
	else if (HTTPMessage::lookupHeader(
	    headers, "*CIMExport", cimOperation, true))
	{
	    //_exportMessageQueue->enqueue(socketMessage);
	    _exportMessageQueue->enqueue(httpMessage);
	}
	else
	{
	    // ATTN: error discarded at this time if post is not
            // ATTN CIMOperation or CIMExport
            // ATTN Should return an error but not sure of code yet
	    return;
	}

    }
    // ATTN: KS Add the else here for Post, etc.  Also go to OPTIONS here.
    // ATTN: Or return a 501 - not implemented
}

const char* HTTPDelegator::getQueueName() const
{
    return "HTTPDelegator";
}

PEGASUS_NAMESPACE_END
