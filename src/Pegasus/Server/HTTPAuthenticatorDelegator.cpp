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
// Author:  Nag Boranna,   Hewlett-Packard Company(nagaraja_boranna@hp.com)
//
// Modified By: 
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/HTTPAcceptor.h>
#include <Pegasus/Common/HTTPConnection.h>
#include <Pegasus/Common/HTTPMessage.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Config/ConfigManager.h>
#include "HTTPAuthenticatorDelegator.h"


PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN



HTTPAuthenticatorDelegator::HTTPAuthenticatorDelegator(
    MessageQueue* operationMessageQueue,
    MessageQueue* exportMessageQueue)
    : 
    _operationMessageQueue(operationMessageQueue),
    _exportMessageQueue(exportMessageQueue)
{
    _authenticationManager = new AuthenticationManager();
}

HTTPAuthenticatorDelegator::~HTTPAuthenticatorDelegator()
{
    delete _authenticationManager;

}

void HTTPAuthenticatorDelegator::_sendResponse(
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

void HTTPAuthenticatorDelegator::_sendChallenge(
    Uint32 queueId,
    const String& authResponse)
{
    //
    // build unauthorized (401) response message
    //

    Array<Sint8> message;
    message = XmlWriter::formatUnauthorizedResponseHeader(authResponse);

    _sendResponse(queueId, message);
}


void HTTPAuthenticatorDelegator::_sendError(
    Uint32 queueId,
    const String errorMessage)
{
    //
    // build erro response message
    //

    Array<Sint8> message;
    //
    //ATTN: Need an ErrorResponseHeader() in XmlWriter
    //
    //message = XmlWriter::formatErrorResponseHeader(errorMessage);

    _sendResponse(queueId, message);
}

void HTTPAuthenticatorDelegator::handleEnqueue()
{
    Message* message = dequeue();

    if (!message)
        return;

    if (message->getType() == HTTP_MESSAGE)
    {
        handleHTTPMessage((HTTPMessage*)message);
    }

    delete message;
}

void HTTPAuthenticatorDelegator::handleHTTPMessage(HTTPMessage* httpMessage)
{  
    Boolean authenticated = false;

    //
    // get the configured authentication flag
    //
    ConfigManager* configManager = ConfigManager::getInstance();

    Boolean requireAuthentication = false;

    if (String::equal(
        configManager->getCurrentValue("requireAuthentication"), "true"))
    {
        requireAuthentication = true;
    }

    //
    // Save queueId:
    //
    Uint32 queueId = httpMessage->queueId;

    //
    // Parse the HTTP message:
    //
    String startLine;
    Array<HTTPHeader> headers;
    Sint8* content;
    Uint32 contentLength;

    httpMessage->parse(startLine, headers, content, contentLength);
    
    //
    // Parse the request line:
    //
    String methodName;
    String requestUri;
    String httpVersion;

    HTTPMessage::parseRequestLine(
        startLine, methodName, requestUri, httpVersion);

    //
    // Process M-POST and POST messages:
    //
    if (methodName == "M-POST" || methodName == "POST")
    {
	httpMessage->message.append('\0');

        //
        // Search for Authorization header:
        //
        String authorization = String::EMPTY;

        if ( HTTPMessage::lookupHeader(
             headers, "PegasusAuthorization", authorization, false) &&
             requireAuthentication
           )
        {
            //
            // Do pegasus/local authentication
            //
            authenticated = 
                _authenticationManager->performPegasusAuthentication(
                    authorization,
                    httpMessage->authInfo);

            if (!authenticated)
            {
                String authChallenge = String::EMPTY;
                String authResp = String::EMPTY;

                authResp = _authenticationManager->getPegasusAuthResponseHeader(
                    authorization,
                    httpMessage->authInfo);

                if (!String::equal(authResp, String::EMPTY))
                {
                    _sendChallenge(queueId, authResp);
                }
                else
                {
                    _sendError(queueId, "Invalid Request");
                }

                return;
            }
        }

        if ( HTTPMessage::lookupHeader(
             headers, "Authorization", authorization, false) &&
             requireAuthentication
           )
        {
            //
            // Do http authentication if not authenticated already
            //
            if (!authenticated)
            {
                authenticated =
                    _authenticationManager->performHttpAuthentication(
                        authorization,
                        httpMessage->authInfo);

                if (!authenticated)
                {
                    //ATTN: the number of challenges get sent for a 
                    //      request on a connection can be pre-set.

                    String authChallenge = String::EMPTY;

                    String authResp =
                        _authenticationManager->getHttpAuthResponseHeader();

                    //
                    // Set the challenge string in the Authentication Info
                    // object that was sent with the HTTPMessage
                    //
                    httpMessage->authInfo->setAuthChallenge(authChallenge);

                    _sendChallenge(queueId, authResp);

                    return;
                }
            }
        }

        if ( authenticated || !requireAuthentication )
        {
            //
            // Search for "CIMOperation" header:
            //
            String cimOperation;

            if (HTTPMessage::lookupHeader(
                headers, "*CIMOperation", cimOperation, true))
            {
                _operationMessageQueue->enqueue(httpMessage);
            }
            else if (HTTPMessage::lookupHeader(
                headers, "*CIMExport", cimOperation, true))
            {
                _exportMessageQueue->enqueue(httpMessage);
            }
            else
            {
                // ATTN: error discarded at this time!
                return;
            }
        }
        else
        {
            //String authChallenge;

            String authResp =
                _authenticationManager->getHttpAuthResponseHeader();

            //
            // Set the challenge string in the Authentication Info
            // object that was sent with the HTTPMessage
            //
            //ATTN: check this out later..
            //httpMessage->authInfo->setAuthChallenge(authChallenge);

            _sendChallenge(queueId, authResp);

        }

    }
}

const char* HTTPAuthenticatorDelegator::getQueueName() const
{
    return "HTTPAuthenticatorDelegator";
}

PEGASUS_NAMESPACE_END
