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
// Author:  Nag Boranna,   Hewlett-Packard Company(nagaraja_boranna@hp.com)
//
// Modified By: Dave Rosckes (rosckes@us.ibm.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/HTTPAcceptor.h>
#include <Pegasus/Common/HTTPConnection.h>
#include <Pegasus/Common/HTTPMessage.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Config/ConfigManager.h>
#include "HTTPAuthenticatorDelegator.h"
 
#ifdef PEGASUS_KERBEROS_AUTHENTICATION
#include <Pegasus/Common/CIMKerberosSecurityAssociation.h>
#endif 

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

#ifdef PEGASUS_KERBEROS_AUTHENTICATION
/**
    Constant representing the Kerberos authentication challenge header.
*/
static const String KERBEROS_CHALLENGE_HEADER = "WWW-Authenticate: Negotiate ";
#endif

HTTPAuthenticatorDelegator::HTTPAuthenticatorDelegator(
    Uint32 operationMessageQueueId,
    Uint32 exportMessageQueueId)
   : Base(PEGASUS_QUEUENAME_HTTPAUTHDELEGATOR,
          MessageQueue::getNextQueueId()),
    _operationMessageQueueId(operationMessageQueueId),
    _exportMessageQueueId(exportMessageQueueId)
{
    PEG_METHOD_ENTER(TRC_HTTP,
        "HTTPAuthenticatorDelegator::HTTPAuthenticatorDelegator");

    _authenticationManager = new AuthenticationManager();

    PEG_METHOD_EXIT();
}

HTTPAuthenticatorDelegator::~HTTPAuthenticatorDelegator()
{
    PEG_METHOD_ENTER(TRC_HTTP,
        "HTTPAuthenticatorDelegator::~HTTPAuthenticatorDelegator");

    delete _authenticationManager;

    PEG_METHOD_EXIT();
}

void HTTPAuthenticatorDelegator::_sendResponse(
    Uint32 queueId,
    Array<Sint8>& message)
{
    PEG_METHOD_ENTER(TRC_HTTP,
        "HTTPAuthenticatorDelegator::_sendResponse");

    MessageQueue* queue = MessageQueue::lookup(queueId);

    if (queue)
    {
        HTTPMessage* httpMessage = new HTTPMessage(message);
	httpMessage->dest = queue->getQueueId();
	
        queue->enqueue(httpMessage);
    }

    PEG_METHOD_EXIT();
}

#ifdef PEGASUS_KERBEROS_AUTHENTICATION
void HTTPAuthenticatorDelegator::_sendSuccess(
    Uint32 queueId,
    const String& authResponse)
{
    PEG_METHOD_ENTER(TRC_HTTP,
        "HTTPAuthenticatorDelegator::_sendSuccess");

    //
    // build OK (200) response message
    //

    Array<Sint8> message;
    XmlWriter::appendOKResponseHeader(message, authResponse);

    _sendResponse(queueId, message);

    PEG_METHOD_EXIT();
}
#endif

void HTTPAuthenticatorDelegator::_sendChallenge(
    Uint32 queueId,
    const String& authResponse)
{
    PEG_METHOD_ENTER(TRC_HTTP,
        "HTTPAuthenticatorDelegator::_sendChallenge");

    //
    // build unauthorized (401) response message
    //

    Array<Sint8> message;
    XmlWriter::appendUnauthorizedResponseHeader(message, authResponse);

    _sendResponse(queueId, message);

    PEG_METHOD_EXIT();
}


void HTTPAuthenticatorDelegator::_sendError(
    Uint32 queueId,
    const String errorMessage)
{
    PEG_METHOD_ENTER(TRC_HTTP,
        "HTTPAuthenticatorDelegator::_sendError");

    //
    // build error response message
    //

    Array<Sint8> message;
    //
    //ATTN: Need an ErrorResponseHeader() in XmlWriter
    //
    //message = XmlWriter::formatErrorResponseHeader(errorMessage);

    _sendResponse(queueId, message);

    PEG_METHOD_EXIT();
}


void HTTPAuthenticatorDelegator::handleEnqueue(Message *message)
{
    PEG_METHOD_ENTER(TRC_HTTP,
        "HTTPAuthenticatorDelegator::handleEnqueue");

    if (!message)
    {
        PEG_METHOD_EXIT();
        return;
    }

    // Flag indicating whether the message should be deleted after handling.
    // This should be set to false by handleHTTPMessage when the message is
    // passed as is to another queue.
    Boolean deleteMessage = true;
   
    if (message->getType() == HTTP_MESSAGE)
    {
        handleHTTPMessage((HTTPMessage*)message, deleteMessage);
    }

    if (deleteMessage)
    {
        delete message;
    }

    PEG_METHOD_EXIT();
}

void HTTPAuthenticatorDelegator::handleEnqueue()
{
    PEG_METHOD_ENTER(TRC_HTTP,
        "HTTPAuthenticatorDelegator::handleEnqueue");

    Message* message = dequeue();
    if(message)
       handleEnqueue(message);

    PEG_METHOD_EXIT();
}

void HTTPAuthenticatorDelegator::handleHTTPMessage(
    HTTPMessage* httpMessage,
    Boolean & deleteMessage)
{  
    PEG_METHOD_ENTER(TRC_HTTP,
        "HTTPAuthenticatorDelegator::handleHTTPMessage");

    Boolean authenticated = false;
    deleteMessage = true;

    // ATTN-RK-P3-20020408: This check probably shouldn't be necessary, but
    // we're getting an empty message when the client closes the connection
    if (httpMessage->message.size() == 0)
    {
        // The message is empty; just drop it
        return;
    }

    //
    // get the configured authentication flag
    //
    ConfigManager* configManager = ConfigManager::getInstance();

    Boolean enableAuthentication = false;

    if (String::equal(
        configManager->getCurrentValue("enableAuthentication"), "true"))
    {
        enableAuthentication = true;
    }

    //
    // Save queueId:
    //
    Uint32 queueId = httpMessage->queueId;

#ifdef PEGASUS_KERBEROS_AUTHENTICATION
    // This still needs work and is not functional.

    CIMKerberosSecurityAssociation *sa = httpMessage->authInfo->getSecurityAssociation();
    char* outmessage = NULL;
    Uint32   outlength = 0;
    if ( sa )
    {
        if (sa->getClientAuthenticated())
        { 
            if (sa->unwrap_message((const char*)httpMessage->message.getData(),
                                    httpMessage->message.size(),
                                    outmessage,
                                    outlength))
            {
                // build a bad request
                Array<Sint8> statusMsg;
                statusMsg = XmlWriter::formatHttpErrorRspMessage(HTTP_STATUS_BADREQUEST);
                _sendResponse(queueId, statusMsg);
                PEG_METHOD_EXIT();
                return;
            }
        }
        else
        {
            // set authenticated flag in _authInfo to not authenticated because the
            // unwrap resulted in an expired token or credential.
            httpMessage->authInfo->setAuthStatus(AuthenticationInfoRep::CHALLENGE_SENT);
            // build a 401 response 
            Array<Sint8> statusMsg;
            // do we need to add a token here or just restart the negotiate again???
            // authResponse.append(sa->getServerToken());
            XmlWriter::appendUnauthorizedResponseHeader(statusMsg, KERBEROS_CHALLENGE_HEADER);
            _sendResponse(queueId, statusMsg);
            PEG_METHOD_EXIT();
            return;
        }
    }           
#endif

    //
    // Parse the HTTP message:
    //
    String startLine;
    Array<HTTPHeader> headers;
    Uint32 contentLength;

    httpMessage->parse(startLine, headers, contentLength);
    
    //
    // Parse the request line:
    //
    String methodName;
    String requestUri;
    String httpVersion;
    HttpMethod httpMethod = HTTP_METHOD__POST;

    HTTPMessage::parseRequestLine(
        startLine, methodName, requestUri, httpVersion);

    //
    //  Set HTTP method for the request
    //
    if (methodName == "M-POST")
    {
        httpMethod = HTTP_METHOD_M_POST;
    }

    if (methodName != "M-POST" && methodName != "POST")
    {
        // Only POST and M-POST are implemented by this server
        Array<Sint8> message;
        message = XmlWriter::formatHttpErrorRspMessage(
            HTTP_STATUS_NOTIMPLEMENTED);
        _sendResponse(queueId, message);
    }
    else if ((httpMethod == HTTP_METHOD_M_POST) &&
             (httpVersion == "HTTP/1.0"))
    {
        //
        //  M-POST method is not valid with version 1.0
        //
        Array<Sint8> message;
        message = XmlWriter::formatHttpErrorRspMessage(
            HTTP_STATUS_BADREQUEST);
        _sendResponse(queueId, message);
    }
    else
    {
        //
        // Process M-POST and POST messages:
        //
	Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
	    "HTTPAuthenticatorDelegator - M-POST/POST processing start");

	httpMessage->message.append('\0');

        //
        // Search for Authorization header:
        //
        String authorization = String::EMPTY;

        if ( HTTPMessage::lookupHeader(
             headers, "PegasusAuthorization", authorization, false) &&
             enableAuthentication
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

                PEG_METHOD_EXIT();
                return;
            }
        }

        if ( HTTPMessage::lookupHeader(
             headers, "Authorization", authorization, false) &&
             enableAuthentication
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
#ifdef PEGASUS_KERBEROS_AUTHENTICATION
                    // Kerberos authentication needs access to the AuthenticationInfo
                    // object for this session in order to set up the reference to the
                    // CIMKerberosSecurityAssociation object for this session.
                    String authResp =   
                        _authenticationManager->getHttpAuthResponseHeader(httpMessage->authInfo);
#else
                    String authResp =
                        _authenticationManager->getHttpAuthResponseHeader();
#endif
                    if (!String::equal(authResp, String::EMPTY))
                    {
                        _sendChallenge(queueId, authResp);
                    }
                    else
                    {
                        _sendError(queueId, "Invalid Request");
                    }

                    PEG_METHOD_EXIT();
                    return;
                }
#ifdef PEGASUS_KERBEROS_AUTHENTICATION
                else if (String::equalNoCase(httpMessage->authInfo->getAuthType(),
                                             "Kerberos") &&
                         httpMessage->authInfo->isAuthenticated() &&
                         "Client requested mutual authentication")
                {

                    String authResp =    
                        _authenticationManager->getHttpAuthResponseHeader(httpMessage->authInfo);
                    if (!String::equal(authResp, String::EMPTY))
                    {
                        _sendSuccess(queueId, authResp);
                    }
                    else
                    {
                        /* Should never fall into here.  Add code to add a trace
                           statement in the event that it does fall into this 
                           else. */
                    }
                }
#endif
            }
        }

        if ( authenticated || !enableAuthentication )
        {
            //
            // Search for "CIMOperation" header:
            //
            String cimOperation;

            if (HTTPMessage::lookupHeader(
                headers, "CIMOperation", cimOperation, true))
            {
		Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
			    "HTTPAuthenticatorDelegator - CIMOperation: $0 ",cimOperation);

                MessageQueue* queue =
                    MessageQueue::lookup(_operationMessageQueueId);

                if (queue)
                {
		   httpMessage->dest = queue->getQueueId();
		   
                    queue->enqueue(httpMessage);
                    deleteMessage = false;
                }
            }
            else if (HTTPMessage::lookupHeader(
                headers, "CIMExport", cimOperation, true))
            {
		Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
			    "HTTPAuthenticatorDelegator - CIMExport: $0 ",cimOperation);

                MessageQueue* queue =
                    MessageQueue::lookup(_exportMessageQueueId);

                if (queue)
                {
		   httpMessage->dest = queue->getQueueId();

		   queue->enqueue(httpMessage);
		   deleteMessage = false;
                }
            }
            else
            {
                // We don't recognize this request message type

                // The Specification for CIM Operations over HTTP reads:
                //
                //     3.3.4. CIMOperation
                //
                //     If a CIM Server receives a CIM Operation request without
                //     this [CIMOperation] header, it MUST NOT process it as if
                //     it were a CIM Operation Request.  The status code
                //     returned by the CIM Server in response to such a request
                //     is outside of the scope of this specification.
                //
                //     3.3.5. CIMExport
                //
                //     If a CIM Listener receives a CIM Export request without
                //     this [CIMExport] header, it MUST NOT process it.  The
                //     status code returned by the CIM Listener in response to
                //     such a request is outside of the scope of this
                //     specification.
                //
                // The author has chosen to send a 400 Bad Request error, but
                // without the CIMError header since this request must not be
                // processed as a CIM request.

                Array<Sint8> message;
                message = XmlWriter::formatHttpErrorRspMessage(
                    HTTP_STATUS_BADREQUEST);
                _sendResponse(queueId, message);
                PEG_METHOD_EXIT();
                return;
            }
        }
        else
        {
#ifdef PEGASUS_KERBEROS_AUTHENTICATION
            String authResp =    
                _authenticationManager->getHttpAuthResponseHeader(httpMessage->authInfo);
#else
            String authResp =
                _authenticationManager->getHttpAuthResponseHeader();
#endif

            if (!String::equal(authResp, String::EMPTY))
            {
                _sendChallenge(queueId, authResp);
            }
            else
            {
                _sendError(queueId, "Invalid Request");
#ifdef PEGASUS_KERBEROS_AUTHENTICATION
                /* Should never fall into here.  Add code to add a trace
                   statement in the event that it does fall into this 
                   else. */
#endif
            }
        }
    }

    PEG_METHOD_EXIT();
}

PEGASUS_NAMESPACE_END
