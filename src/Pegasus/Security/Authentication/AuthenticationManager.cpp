//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 BMC Software, Hewlett-Packard Company, IBM,
// The Open Group, Tivoli Systems
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies of substantial portions of this software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
//==============================================================================
//
// Author: Nag Boranna, Hewlett-Packard Company(nagaraja_boranna@hp.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/Destroyer.h>
#include <Pegasus/Config/ConfigManager.h>

#include <Pegasus/Security/Authentication/LocalAuthenticationHandler.h>
#include <Pegasus/Security/Authentication/BasicAuthenticationHandler.h>
#include "AuthenticationManager.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

AuthenticationManager::AuthenticationManager(
    MessageQueue* outputQueue,
    Uint32 returnQueueId)
    : 
    _outputQueue(outputQueue),
    _returnQueueId(returnQueueId)
{
    // ATTN: Specify the right realm ?
    _realm.assign("hostname:5589"); //"hostname" + ":" + "portNo";

    _authChallenge = String::EMPTY;

    // ATTN: We will do Basic for now..
    _authHandler = _getAuthHandler("Basic");
}

AuthenticationManager::~AuthenticationManager()
{

}

void AuthenticationManager::sendResponse(
    Uint32 queueId, 
    Array<Sint8>& message)
{
    MessageQueue* queue = MessageQueue::lookup(queueId);

    if (queue)
    {
        HTTPMessage* httpMessage = new HTTPMessage(message);
        // ATTN: Add this once integrated in to the build
        //httpMessage->authChallenge = _authChallenge;
        queue->enqueue(httpMessage);
    }
}

void AuthenticationManager::sendChallenge(
    Uint32 queueId, 
    const String& authResponse) 
{
    // build unauthorized (401) response message

    Array<Sint8> message;
    //message = 
    //    XmlWriter::formatUnauthorizedResponseHeader(authResponse);
    
    sendResponse(queueId, message);
}

void AuthenticationManager::sendError(
    Uint32 queueId,
    const String& messageId,
    const String& cimMethodName,
    CIMStatusCode code,
    const String& description)
{
   //ATTN: TBD
}

void AuthenticationManager::handleEnqueue()
{
    Message* message = dequeue();

    if (!message)
    {
        return;
    }

    switch (message->getType())
    {
        case HTTP_MESSAGE:
            handleHTTPMessage((HTTPMessage*)message);
            break;
    }

    delete message;
}

const char* AuthenticationManager::getQueueName() const
{
    return "AuthenticationManager";
}

// 
//------------------------------------------------------------------------------

void AuthenticationManager::handleHTTPMessage(HTTPMessage* httpMessage)
{
    Boolean authenticated = false;

    // Save queueId:
    Uint32 queueId = httpMessage->queueId;

    // ATTN: Add this once integrated in to the build
    //_authenticationInfo = httpMessage->authInfo;
    _authenticationInfo = String::EMPTY;

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
        // Search for "Authorization" or "PegasusAuthorization" header:

        String authorization;

        if (HTTPMessage::lookupHeader(
            headers, "Authorization", authorization, false))
        {
            // Do Http authentication
            authenticated = _performHttpAuthentication(queueId, authorization);
        }
        else if (HTTPMessage::lookupHeader(
            headers, "PegasusAuthorization", authorization, false))
        {
            // Do Local authentication
            authenticated = _performLocalAuthentication(queueId, authorization);
        }

        //
        // get the configured authentication flag
        //
        ConfigManager* configManager;
        configManager = ConfigManager::getInstance();

        String requireAuthentication = 
            configManager->getCurrentValue("requireAuthentication");


        if (!authenticated && String::equal(requireAuthentication, "true"))
        {
            // ATTN: Send authentication challenge
            String authResp = 
                _authHandler->getAuthResponseHeader(_realm, _authChallenge);    
            sendChallenge(queueId, authResp);
        }
    }
}


Boolean AuthenticationManager::_performHttpAuthentication(
    Uint32 queueId,
    String authHeader)
{
    //
    // get the configured authentication type
    //
    ConfigManager* configManager;
    configManager = ConfigManager::getInstance();

    String authType = configManager->getCurrentValue("HttpAuthType");

    //
    // Check whether the auth header has the authentication 
    // information or not.
    //
    if (String::equalNoCase(authHeader, "Basic"))
    {
        //
        // Check if Basic authentication is supported or not.
        //
        if (!String::equalNoCase(authType, "Basic"))
        {
            // ATTN: Log basic authentication not supported
            return( false );
        }

        Uint32 pos = authHeader.find(authType);

        String cookie = authHeader.subString(pos + 6);

        return (_authHandler->authenticate(cookie, _authenticationInfo));
    }
    // else  ATTN: add support for digest authentication

    // else  ATTN: Log authentication type not supported message

    return ( false );
}

Boolean AuthenticationManager::_performLocalAuthentication(
    Uint32 queueId,
    String authHeader)
{
    Boolean authenticated = false;
    Authenticator* localAuthenticator;

    //
    // Check whether the auth header has the authentication 
    // information or not.
    //
    if (String::equalNoCase(authHeader, "Local"))
    {
        Uint32 pos = authHeader.find("Local");

        String cookie = authHeader.subString(pos + 6);

        localAuthenticator = (Authenticator* ) new LocalAuthenticationHandler();

        authenticated = 
            localAuthenticator->authenticate(cookie, _authenticationInfo);

        if (!authenticated)
        {
            // ATTN: Make sure we does not send unlimited 
            // authentication challenges on the same connection and for the
            // same user.
            // 
            String challenge = String::EMPTY;

            String authResp = 
                localAuthenticator->getAuthResponseHeader(cookie, challenge);
            _authChallenge = challenge;

            sendChallenge(queueId, authResp);
        }
    }
    else if (String::equalNoCase(authHeader, "LocalPrivileged"))
    {
        String privillegedUser = "root";

        Uint32 pos = authHeader.find("LocalPrivileged");

        String cookie = authHeader.subString(pos + 16);

        localAuthenticator = (Authenticator* ) new LocalAuthenticationHandler();

        authenticated = 
            localAuthenticator->authenticate(cookie, _authenticationInfo);

        if (!authenticated)
        {
            // ATTN: Make sure we does not send unlimited 
            // authentication challenges on the same connection and for the
            // same user.
            // 
            String challenge = String::EMPTY;

            //
            // root user is the only privileged user
            // ATTN: Can the privileged user taken from configuration?
            //
            cookie.assign("root");

            String authResp = 
                localAuthenticator->getAuthResponseHeader(cookie, challenge);
            _authChallenge = challenge;

            sendChallenge(queueId, authResp);
        }
    }

    return ( authenticated );
}

/**
Get an instance of an authentication handler module.
*/
Authenticator* AuthenticationManager::_getAuthHandler(String type)
{
    Authenticator* handler;

    if ( String::equalNoCase(type, "Local") ||
         String::equalNoCase(type, "LocalPrivileged") ) 
    {
        handler = (Authenticator*) new LocalAuthenticationHandler( );
        return ( handler );
    }

    //
    // get the configured/default authentication type
    //
    ConfigManager* configManager;
    configManager = ConfigManager::getInstance();

    String authType = configManager->getCurrentValue("HttpAuthType");
    
    //
    // If Basic authentication is configured then 
    // create a basic auth handler.
    //
    if (String::equal(authType, "Basic"))
    {
        handler = (Authenticator* ) new BasicAuthenticationHandler( );
    }

    //ATTN: add support for Digest authentication.
    //else if (authType.equalNoCase("Digest"))
    //{
    //    handler = (Authenticator* ) new DigestAuthenticationHandler( );
    //}
    
    return ( handler );
}


PEGASUS_NAMESPACE_END

