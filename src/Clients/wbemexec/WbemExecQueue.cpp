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
// Author: Warren Otsuka (warren_otsuka@hp.com)
//
// Modified By: 
//
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <iostream>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/HTTPMessage.h>
#include "WbemExecQueue.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

WbemExecQueue::WbemExecQueue(
    MessageQueue* encoderQueue,
    ostream& os,
    Boolean debugOutput,
    ClientAuthenticator* authenticator)
    :
    _encoderQueue(encoderQueue),
    _os(os),
    _debugOutput (debugOutput),
    _authenticator(authenticator)
{

}

WbemExecQueue::~WbemExecQueue()
{

}

void  WbemExecQueue::setEncoderQueue(MessageQueue* encoderQueue)
{
    _encoderQueue = encoderQueue;
}

void WbemExecQueue::handleEnqueue()
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

const char* WbemExecQueue::getQueueName() const
{
    return "WbemExecQueue";
}

void WbemExecQueue::_handleHTTPMessage(HTTPMessage* httpMessage)
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
    // Zero-terminate the message:
    //

    httpMessage->message.append('\0');

    _printAll( httpMessage );
}


/**
  
    Prints the response to the specified output stream.
  
 */

void WbemExecQueue::_printAll( HTTPMessage* httpMessage )
{
    String startLine;
    Array<HTTPHeader> headers;
    Uint32 contentLength;
    Uint32 contentOffset;

    httpMessage->parse( startLine, headers, contentLength );
    contentOffset = httpMessage->message.size() - contentLength;
    const char* content = httpMessage->message.getData() + contentOffset;

    if (_debugOutput)
    {
      // Print the first line:
      
      _os << startLine << endl;
      
      // Print the headers:

      for (Uint32 i = 0; i < headers.size(); i++)
	{
	  _os << headers[i].first << ": " << headers[i].second << endl;
	}

      _os << endl;
    }

    //
    //  Print XML response to the ostream
    //
    XmlWriter::indentedPrint ( _os, content, 0 );
}


PEGASUS_NAMESPACE_END
