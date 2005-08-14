//%2005////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
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
//              Sushma Fernandes (sushma_fernandes@hp.com)
//              Heather Sterling, IBM (hsterl@us.ibm.com)
//              Amit K Arora, IBM (amita@in.ibm.com) for PEP#101
//              David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
//              John Alex, IBM (johnalex@us.ibm.com) - Bug#2290
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/HTTPAcceptor.h>
#include <Pegasus/Common/HTTPConnection.h>
#include <Pegasus/Common/HTTPMessage.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Config/ConfigManager.h>
#include <Pegasus/Common/Thread.h>
#include "HTTPAuthenticatorDelegator.h"
#include <Pegasus/Common/MessageLoader.h>
#include <Pegasus/Common/FileSystem.h>
 
#ifdef PEGASUS_KERBEROS_AUTHENTICATION
#include <Pegasus/Common/CIMKerberosSecurityAssociation.h>
#endif 

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN


HTTPAuthenticatorDelegator::HTTPAuthenticatorDelegator(
    Uint32 operationMessageQueueId,
    Uint32 exportMessageQueueId,
	CIMRepository* repository)
   : Base(PEGASUS_QUEUENAME_HTTPAUTHDELEGATOR,
          MessageQueue::getNextQueueId()),
    _operationMessageQueueId(operationMessageQueueId),
    _exportMessageQueueId(exportMessageQueueId),
	_repository(repository)
{
    PEG_METHOD_ENTER(TRC_HTTP,
        "HTTPAuthenticatorDelegator::HTTPAuthenticatorDelegator");

    _authenticationManager.reset(new AuthenticationManager());

    PEG_METHOD_EXIT();
}

HTTPAuthenticatorDelegator::~HTTPAuthenticatorDelegator()
{
    PEG_METHOD_ENTER(TRC_HTTP,
        "HTTPAuthenticatorDelegator::~HTTPAuthenticatorDelegator");

    PEG_METHOD_EXIT();
}

void HTTPAuthenticatorDelegator::enqueue(Message* message)
{
    handleEnqueue(message);
}

void HTTPAuthenticatorDelegator::_sendResponse(
    Uint32 queueId,
    Array<char>& message,
    Boolean closeConnect)
{
    PEG_METHOD_ENTER(TRC_HTTP,
        "HTTPAuthenticatorDelegator::_sendResponse");

    MessageQueue* queue = MessageQueue::lookup(queueId);

    if (queue)
    {
        HTTPMessage* httpMessage = new HTTPMessage(message);
	httpMessage->dest = queue->getQueueId();
    
        httpMessage->setCloseConnect(closeConnect);

        queue->enqueue(httpMessage);
    }

    PEG_METHOD_EXIT();
}

#ifdef PEGASUS_KERBEROS_AUTHENTICATION
void HTTPAuthenticatorDelegator::_sendSuccess(
    Uint32 queueId,
    const String& authResponse,
    Boolean closeConnect)
{
    PEG_METHOD_ENTER(TRC_HTTP,
        "HTTPAuthenticatorDelegator::_sendSuccess");

    //
    // build OK (200) response message
    //

    Array<char> message;
    XmlWriter::appendOKResponseHeader(message, authResponse);

    _sendResponse(queueId, message,closeConnect);

    PEG_METHOD_EXIT();
}
#endif

void HTTPAuthenticatorDelegator::_sendChallenge(
    Uint32 queueId,
    const String& authResponse,
    Boolean closeConnect)
{
    PEG_METHOD_ENTER(TRC_HTTP,
        "HTTPAuthenticatorDelegator::_sendChallenge");

    //
    // build unauthorized (401) response message
    //

    Array<char> message;
    XmlWriter::appendUnauthorizedResponseHeader(message, authResponse);

    _sendResponse(queueId, message,closeConnect);

    PEG_METHOD_EXIT();
}


void HTTPAuthenticatorDelegator::_sendHttpError(
    Uint32 queueId,
    const String& status,
    const String& cimError,
    const String& pegasusError,
    Boolean closeConnect)
{
    PEG_METHOD_ENTER(TRC_HTTP,
        "HTTPAuthenticatorDelegator::_sendHttpError");

    //
    // build error response message
    //

    Array<char> message;
    message = XmlWriter::formatHttpErrorRspMessage(
        status,
        cimError,
        pegasusError);

    _sendResponse(queueId, message,closeConnect);

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
        PEG_TRACE_STRING(TRC_HTTP, Tracer::LEVEL3,
                    "Deleting Message in HTTPAuthenticator::handleEnqueue");

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

    deleteMessage = true;

    // ATTN-RK-P3-20020408: This check probably shouldn't be necessary, but
    // we're getting an empty message when the client closes the connection
    if (httpMessage->message.size() == 0)
    {
        // The message is empty; just drop it
        PEG_METHOD_EXIT();
        return;
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
    Uint32 contentLength;
    String connectClose;
    Boolean closeConnect = false;

    httpMessage->parse(startLine, headers, contentLength);
    
    //
    // Check for Connection: Close
    //
    if(HTTPMessage::lookupHeader(headers, "Connection", connectClose, false))
    {
       if (String::equalNoCase(connectClose, "Close"))
       {
            PEG_TRACE_STRING(TRC_HTTP, Tracer::LEVEL3,
                    "Header in HTTP Message Contains a Connection: Close");
            closeConnect = true;
            httpMessage->setCloseConnect(closeConnect);
       }
    }


    //
    // Check if the request was received on the export connection.
    // Do not accept CIMOperation requests on the export connection.
    // Username-password authentication is not done for CIMExport requests 
    // received on the export connection. Instead the client is
    // authenticated by client certificate verification.
    //
    if ( httpMessage->authInfo->isExportConnection() )
    {
        //
        // Do not accept CIMOperation requests on the export connection
        //
        String operation;

        if ( HTTPMessage::lookupHeader(
             headers, "CIMOperation", operation, true) )
        {
            //
            //  CIMOperation requests are not supported on the export connection
            //
            MessageLoaderParms msgParms(
                "Pegasus.Server.HTTPAuthenticatorDelegator.CIMOPERATIONS_NOT_SUPPORTED",
                "CIMOperation request not supported on this port.");
            String msg(MessageLoader::getMessage(msgParms));

            PEG_METHOD_EXIT();
            _sendHttpError(
                queueId,
                HTTP_STATUS_FORBIDDEN,
                String::EMPTY,
                msg,
                closeConnect);
        }

        //
        // Do not require user-password authentication for CIMExport requests 
        // received on the export connection. Instead verify that the client 
        // is authenticated by client certificate verification.
        //
        if ( HTTPMessage::lookupHeader(
             headers, "CIMExport", operation, true) )
        {
            PEG_TRACE_STRING(TRC_HTTP, Tracer::LEVEL3,
                    "CIMExport request received on export connection");

            //
            // Client may have already authenticated by verifying peer 
            // certificate (in SSLSocket) during SSL handshake. 
            // In this case, no further attempts to authenticate the client are made
            //
            if ( httpMessage->authInfo->isAuthenticated() )
            {
                PEG_TRACE_STRING(TRC_HTTP, Tracer::LEVEL4,
                    "httpMessage->authInfo->isAuthenticated() is true");

                Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
                    "HTTPAuthenticatorDelegator - CIMExport: $0 ", operation);

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
                //
                // This condition is expected to be handled early on during
                // peer certificate verification and it should never come here.
                // We will treat this as a bad request if it does come here.
                //
                PEGASUS_ASSERT(0);
                PEG_METHOD_EXIT();
                _sendHttpError(
                    queueId, 
                    HTTP_STATUS_BADREQUEST,
                    String::EMPTY,
                    String::EMPTY,
                    closeConnect);
            }
            PEG_METHOD_EXIT();
            return;
        }
    }

    //
    // Handle authentication:
    //
    ConfigManager* configManager = ConfigManager::getInstance();
    Boolean enableAuthentication = false;
    Boolean authenticated = false;

    if (String::equal(
        configManager->getCurrentValue("enableAuthentication"), "true"))
    {
        enableAuthentication = true;
            
        // Client may have already authenticated via SSL.
        // In this case, no further attempts to authenticate the client are made
        authenticated = httpMessage->authInfo->isAuthenticated();

        // If the request was authenticated via SSL, append the username to the IdentityContainer
        String cimOperation;
        if (authenticated && 
			(String::equal(httpMessage->authInfo->getAuthType(), AuthenticationInfoRep::AUTH_TYPE_SSL)) &&
			HTTPMessage::lookupHeader(headers, "CIMOperation", cimOperation, true))
        {
			PEG_TRACE_STRING(TRC_HTTP, Tracer::LEVEL3, "Client was authenticated via trusted SSL certificate.");

			//PEP187
			String trustStore = configManager->getCurrentValue("sslTrustStore");

			if (FileSystem::isDirectory(ConfigManager::getHomedPath(trustStore))) 
			{
				//get the authenticated client certificate
				SSLCertificateInfo* clientCertificate = httpMessage->authInfo->getClientCertificate();
				
				if (!clientCertificate) 
				{
					MessageLoaderParms msgParms("Pegasus.Server.HTTPAuthenticatorDelegator.BAD_CERTIFICATE", 
                                                "The certificate used for authentication is not valid.");
					String msg(MessageLoader::getMessage(msgParms));
                                        _sendHttpError(
                                            queueId,
                                            HTTP_STATUS_UNAUTHORIZED,
                                            String::EMPTY,
                                            msg,
                                            closeConnect);
					PEG_METHOD_EXIT();
					return;
				}
				 
				//get certificate properties
				String issuerName = clientCertificate->getIssuerName();
				char serialNumber[256];
				sprintf(serialNumber, "%lu", clientCertificate->getSerialNumber());

				//ATTN: Use certificate provider constants
				String truststoreType = (httpMessage->authInfo->isExportConnection() ? String("3") : String("2"));
	
				//construct the corresponding PG_SSLCertificate instance
				Array<CIMKeyBinding> keyBindings;
				keyBindings.append(CIMKeyBinding("IssuerName", issuerName, CIMKeyBinding::STRING));
				keyBindings.append(CIMKeyBinding("SerialNumber", serialNumber, CIMKeyBinding::STRING));
				keyBindings.append(CIMKeyBinding("TruststoreType", truststoreType, CIMKeyBinding::NUMERIC));
	
				CIMObjectPath cimObjectPath("localhost",
											PEGASUS_NAMESPACENAME_CERTIFICATE,
											PEGASUS_CLASSNAME_CERTIFICATE,
											keyBindings);
	
				PEG_TRACE_STRING(TRC_HTTP, Tracer::LEVEL4, "Client Certificate COP: " + cimObjectPath.toString());
			   
                CIMInstance cimInstance;
				CIMValue value;
				Uint32 pos;
				String userName = String::EMPTY;
	
				//attempt to get the username registered to the certificate
				try 
				{
					cimInstance = _repository->getInstance(PEGASUS_NAMESPACENAME_CERTIFICATE, cimObjectPath);
					
				} catch (CIMException& e)
				{
                    //currently, we have an issue with the scenario of a root CA being added to the truststore and
					//authenticating a user certificate that the CA signed.  The SSL client verification works correctly,
					//but when we try to get the instance here, it fails because the serial number is that of the user
					//and not that of the CA.  We need to work out how to go about getting the correct serial number.
					//Complications also include the possibility of an intermediate CA or that a certificate for a root CA
					//and a user cert are registered for different usernames.  At any rate, this issue is being tracked by
					//Heather Sterling as part of PEP187.

					//For now, try a serial number of "0" to attempt to get the root CA
					try
					{
						keyBindings.remove(1); 
                        keyBindings.append(CIMKeyBinding("SerialNumber", "0", CIMKeyBinding::STRING));

				        CIMObjectPath rootCAPath("localhost",
												 PEGASUS_NAMESPACENAME_CERTIFICATE,
												 PEGASUS_CLASSNAME_CERTIFICATE,
												 keyBindings);

						PEG_TRACE_STRING(TRC_HTTP, Tracer::LEVEL4, "Trying root CA path: " + rootCAPath.toString());


						cimInstance = _repository->getInstance(PEGASUS_NAMESPACENAME_CERTIFICATE, rootCAPath);
					
					} catch (...)
					{
                    //this scenario can occur if a certificate cached on the server was deleted
                    //openssl would not pick up the deletion but we would pick it up here when we went to look it up
                    //in the repository
					Logger::put(Logger::ERROR_LOG, System::CIMSERVER, Logger::TRACE,
								"HTTPAuthenticatorDelegator - Bailing, the certificate used for authentication is not valid.");
                    MessageLoaderParms msgParms("Pegasus.Server.HTTPAuthenticatorDelegator.BAD_CERTIFICATE",
                                                "The certificate used for authentication is not valid.");
					String msg(MessageLoader::getMessage(msgParms));
                    PEG_TRACE_STRING(TRC_HTTP, Tracer::LEVEL3, msg);
                                       _sendHttpError(
                                            queueId,
                                            HTTP_STATUS_UNAUTHORIZED,
                                            String::EMPTY,
                                            msg,
                                            closeConnect);
					PEG_METHOD_EXIT();
					return;
				}
				}

                pos = cimInstance.findProperty("RegisteredUserName");
					
				if (pos != PEG_NOT_FOUND && !(value = cimInstance.getProperty(pos).getValue()).isNull())
				{
					value.get(userName);

                                        //
                                        // Validate user information
                                        //

                                        if (!_validateUser(userName, queueId))
                                        {
                                            PEG_METHOD_EXIT();
                                            return;
                                        }
  
					httpMessage->authInfo->setAuthenticatedUser(userName);

					PEG_TRACE_STRING(TRC_HTTP, Tracer::LEVEL3, "User name for certificate is " + userName);
					Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
								"HTTPAuthenticatorDelegator - The trusted client certificate is registered to $0.", userName);
				} 
                                else
				{
				    Logger::put(Logger::ERROR_LOG, System::CIMSERVER, Logger::TRACE,
								"HTTPAuthenticatorDelegator - Bailing, no username is registered to this certificate.");
					MessageLoaderParms msgParms("Pegasus.Server.HTTPAuthenticatorDelegator.BAD_CERTIFICATE_USERNAME",
                                                "No username is registered to this certificate.");
                                        String msg(MessageLoader::getMessage(msgParms));
                                        PEG_TRACE_STRING(TRC_HTTP, Tracer::LEVEL3, msg);
                                        _sendHttpError(
                                            queueId,
                                            HTTP_STATUS_UNAUTHORIZED,
                                            String::EMPTY,
                                            msg,
                                            closeConnect);
					PEG_METHOD_EXIT();
					return;
				}

			} 
                        else
			{
				//trustStore is a single CA file, lookup username
				//user was already verified as a valid system user during server startup
				String trustStoreUserName = configManager->getCurrentValue("sslTrustStoreUserName");

                                //
                                // Validate user information
                                //

                                if (!_validateUser(trustStoreUserName, queueId))
                                {
                                    PEG_METHOD_EXIT();
                                    return;
                                }


                                httpMessage->authInfo->setAuthenticatedUser(trustStoreUserName);

                                PEG_TRACE_STRING(
                                    TRC_HTTP,
                                    Tracer::LEVEL3,
                                    "User name for certificate is " + trustStoreUserName);
                                Logger::put(
                                    Logger::STANDARD_LOG,
                                    System::CIMSERVER,
                                    Logger::TRACE,
                                    "HTTPAuthenticatorDelegator - The trusted client certificate is registered to $0.",
                                    trustStoreUserName);

			}
		}

	} //end enableAuthentication

	PEG_TRACE_STRING(TRC_HTTP, Tracer::LEVEL4, "Exited authentication loop");


// l10n start
   AcceptLanguages acceptLanguages = AcceptLanguages::EMPTY;
   ContentLanguages contentLanguages = ContentLanguages::EMPTY;
   try
   {
                // Get and validate the Accept-Language header, if set
                String acceptLanguageHeader;
                if (HTTPMessage::lookupHeader(
                      headers,
                  "Accept-Language",
                      acceptLanguageHeader,
                  false) == true)
            {
                        acceptLanguages = AcceptLanguages(acceptLanguageHeader);
			httpMessage->acceptLanguagesDecoded = true;
            }
                                                                                                                                                             
                // Get and validate the Content-Language header, if set
                String contentLanguageHeader;
                if (HTTPMessage::lookupHeader(
                      headers,
                  "Content-Language",
                      contentLanguageHeader,
                  false) == true)
            {
                        contentLanguages = ContentLanguages(contentLanguageHeader);
			httpMessage->contentLanguagesDecoded = true;
            }
   }
   catch (Exception &e)
   {
	Thread::clearLanguages(); // clear any existing languages to force messages to come from the root bundle
	MessageLoaderParms msgParms("Pegasus.Server.HTTPAuthenticatorDelegator.REQUEST_NOT_VALID","request-not-valid");
	String msg(MessageLoader::getMessage(msgParms));

        _sendHttpError(
            queueId, 
            HTTP_STATUS_BADREQUEST,
            msg,
            e.getMessage(),
            closeConnect);
        PEG_METHOD_EXIT();
        return;
   }
   Thread::setLanguages(new AcceptLanguages(acceptLanguages));
   httpMessage->acceptLanguages = acceptLanguages;
   httpMessage->contentLanguages = contentLanguages;
// l10n end   

 
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
        _sendHttpError(
            queueId,
            HTTP_STATUS_NOTIMPLEMENTED,
            String::EMPTY,
            String::EMPTY,
            closeConnect);
    }
    else if ((httpMethod == HTTP_METHOD_M_POST) &&
             (httpVersion == "HTTP/1.0"))
    {
        //
        //  M-POST method is not valid with version 1.0
        //
        _sendHttpError(
            queueId,
            HTTP_STATUS_BADREQUEST,
            String::EMPTY,
            String::EMPTY,
            closeConnect);
    }
    else
    {
        //
        // Process M-POST and POST messages:
        //
	Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
	    "HTTPAuthenticatorDelegator - M-POST/POST processing start");

	httpMessage->message.append('\0');

    if (!authenticated && enableAuthentication) 
    {
        //
        // Search for Authorization header:
        //
        String authorization = String::EMPTY;

        if ( HTTPMessage::lookupHeader(
             headers, "PegasusAuthorization", authorization, false) &&
             enableAuthentication
           )
        {
            try 
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
                        _sendChallenge(queueId, authResp,closeConnect);
                    }
                    else
                    {
			MessageLoaderParms msgParms("Pegasus.Server.HTTPAuthenticatorDelegator.AUTHORIZATION_HEADER_ERROR","Authorization header error");
		        String msg(MessageLoader::getMessage(msgParms));
                        _sendHttpError(
                            queueId,
                            HTTP_STATUS_BADREQUEST,
                            String::EMPTY,
                            msg,
                            closeConnect);
                    }

                    PEG_METHOD_EXIT();
                    return;
                }
            }
            catch (const CannotOpenFile &)
            {
                _sendHttpError(
                    queueId,
                    HTTP_STATUS_INTERNALSERVERERROR,
                    String::EMPTY,
                    String::EMPTY,
                    closeConnect);
                PEG_METHOD_EXIT();
                return;
                
            }
        }

#ifdef PEGASUS_KERBEROS_AUTHENTICATION
	// The presence of a Security Association indicates that Kerberos is being used
	// Reset flag for subsequent calls to indicate that no Authorization
        // record was sent. If one was sent the flag will be appropriately reset later.
	// The sa is maintained while the connection is active.
        CIMKerberosSecurityAssociation *sa = httpMessage->authInfo->getSecurityAssociation();
        if (sa)
        {
            sa->setClientSentAuthorization(false);
        }
#endif	

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
                        _sendChallenge(queueId, authResp,closeConnect);
                    }
                    else
                    {
			MessageLoaderParms msgParms("Pegasus.Server.HTTPAuthenticatorDelegator.AUTHORIZATION_HEADER_ERROR","Authorization header error");
                        String msg(MessageLoader::getMessage(msgParms));
                        _sendHttpError(
                            queueId,
                            HTTP_STATUS_BADREQUEST,
                            String::EMPTY,
                            msg,
                            closeConnect);
                    }

                    PEG_METHOD_EXIT();
                    return;
                }
	    }  // first not authenticated check
	}  // "Authorization" header check

#ifdef PEGASUS_KERBEROS_AUTHENTICATION
	// The pointer to the sa is created in the authenticator so we need to also
	// assign it here.
	sa = httpMessage->authInfo->getSecurityAssociation();
	if (sa)
	{
	    Uint32 sendAction = 0;  // 0 - continue, 1 = send success, 2 = send response
	    // The following is processing to unwrap (decrypt) the request from the
	    // client when using kerberos authentication.
	    sa->unwrapRequestMessage(httpMessage->message, contentLength,
				     authenticated, sendAction);
	    if (sendAction)  // send success or send response
	    {
		if (httpMessage->message.size() == 0)
		{
			MessageLoaderParms msgParms("Pegasus.Server.HTTPAuthenticatorDelegator.AUTHORIZATION_HEADER_ERROR","Authorization header error");
                        String msg(MessageLoader::getMessage(msgParms));
                        _sendHttpError(
                            queueId,
                            HTTP_STATUS_BADREQUEST,
                            String::EMPTY,
                            msg,
                            closeConnect);
		}
		else
		{
		    if (sendAction == 1)  // Send success
		    {
                        _sendSuccess(
                            queueId,
                            String(
                                httpMessage->message.getData(),httpMessage->message.size()), 
                            closeConnect);
		    }

		    if (sendAction == 2)  // Send response
		    {
			_sendResponse(queueId, httpMessage->message,closeConnect);
		    }
		}

		PEG_METHOD_EXIT();
		return;
	    }
	}
#endif

    } //end if(!authenticated && enableAuthentication)


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
		   
		   try
		     {
                       queue->enqueue(httpMessage);
		     }
		   catch(const bad_alloc &)
		     {
		       delete httpMessage;
                       _sendHttpError(
                           queueId,
                           HTTP_STATUS_REQUEST_TOO_LARGE,
                           String::EMPTY,
                           String::EMPTY,
                           closeConnect);
		       PEG_METHOD_EXIT();
		       deleteMessage = false;
		       return;
		     }
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

                _sendHttpError(
                    queueId,
                    HTTP_STATUS_BADREQUEST,
                    String::EMPTY,
                    String::EMPTY,
                    closeConnect);
                PEG_METHOD_EXIT();
                return;
            } // bad request
        } // authenticated and enableAuthentication check
        else
        {  // client not authenticated; send challenge
#ifdef PEGASUS_KERBEROS_AUTHENTICATION
            String authResp =    
                _authenticationManager->getHttpAuthResponseHeader(httpMessage->authInfo);
#else
            String authResp =
                _authenticationManager->getHttpAuthResponseHeader();
#endif

            if (!String::equal(authResp, String::EMPTY))
            {
                _sendChallenge(queueId, authResp,closeConnect);
            }
            else
            {
		MessageLoaderParms msgParms("Pegasus.Server.HTTPAuthenticatorDelegator.AUTHORIZATION_HEADER_ERROR","Authorization header error");
                String msg(MessageLoader::getMessage(msgParms));
                _sendHttpError(
                    queueId,
                    HTTP_STATUS_BADREQUEST,
                    String::EMPTY,
                    msg,
                    closeConnect);
            }
        }
    } // M-POST and POST processing

    PEG_METHOD_EXIT();
}

Boolean HTTPAuthenticatorDelegator::_validateUser(
    const String& userName,
    Uint32 queueId)
{
    Boolean authenticated = false;

    authenticated = _authenticationManager->validateUserForHttpAuth(userName);

    if (!authenticated)
    {
         MessageLoaderParms msgParms(
           "Pegasus.Server.HTTPAuthenticatorDelegator.BAD_CERTIFICATE_USERNAME",
           "The username registered to this certificate is not a valid user.");

        String msg(MessageLoader::getMessage(msgParms));
        _sendHttpError(queueId,
                       HTTP_STATUS_UNAUTHORIZED,
                       String::EMPTY,
                       msg);
    }

    return(authenticated);
}

PEGASUS_NAMESPACE_END
