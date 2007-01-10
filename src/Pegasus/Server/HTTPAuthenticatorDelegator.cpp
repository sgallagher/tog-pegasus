//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
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
#include <Pegasus/Common/LanguageParser.h>
#include <Pegasus/Common/Executor.h>

#ifdef PEGASUS_KERBEROS_AUTHENTICATION
# include <Pegasus/Common/CIMKerberosSecurityAssociation.h>
#endif

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

static const String _HTTP_VERSION_1_0 = "HTTP/1.0";

static const String _HTTP_METHOD_MPOST = "M-POST";
static const String _HTTP_METHOD = "POST";

static const String _HTTP_HEADER_CIMEXPORT = "CIMExport";
static const String _HTTP_HEADER_CONNECTION = "Connection";
static const String _HTTP_HEADER_CIMOPERATION = "CIMOperation";
static const String _HTTP_HEADER_ACCEPT_LANGUAGE = "Accept-Language";
static const String _HTTP_HEADER_CONTENT_LANGUAGE = "Content-Language";
static const String _HTTP_HEADER_AUTHORIZATION = "Authorization";
static const String _HTTP_HEADER_PEGASUSAUTHORIZATION = "PegasusAuthorization";

static const String _CONFIG_PARAM_ENABLEAUTHENTICATION = "enableAuthentication";

HTTPAuthenticatorDelegator::HTTPAuthenticatorDelegator(
    Uint32 operationMessageQueueId,
    Uint32 exportMessageQueueId,
    CIMRepository* repository)
    : Base(PEGASUS_QUEUENAME_HTTPAUTHDELEGATOR, MessageQueue::getNextQueueId()),
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
    Buffer& message,
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

    Buffer message;
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

    Buffer message;
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

    Buffer message;
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
    if (message)
       handleEnqueue(message);

    PEG_METHOD_EXIT();
}

void HTTPAuthenticatorDelegator::handleHTTPMessage(
    HTTPMessage* httpMessage,
    Boolean& deleteMessage)
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
    if (HTTPMessage::lookupHeader(
        headers, _HTTP_HEADER_CONNECTION, connectClose, false))
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
    // Handle authentication:
    //
    ConfigManager* configManager = ConfigManager::getInstance();
    Boolean enableAuthentication = false;
    Boolean authenticated = false;

#ifdef PEGASUS_KERBEROS_AUTHENTICATION
    CIMKerberosSecurityAssociation* sa = NULL;
    // The presence of a Security Association indicates that Kerberos is
    // being used.
    // Reset flag for subsequent calls to indicate that no Authorization
    // record was sent. If one was sent the flag will be appropriately reset
    // later.
    // The sa is maintained while the connection is active.
    sa = httpMessage->authInfo->getSecurityAssociation();
    if (sa)
    {
        sa->setClientSentAuthorization(false);
    }
#endif

    if (ConfigManager::parseBooleanValue(configManager->getCurrentValue(
            _CONFIG_PARAM_ENABLEAUTHENTICATION)))
    {
        enableAuthentication = true;
#ifdef PEGASUS_KERBEROS_AUTHENTICATION
        // If we are using Kerberos (sa pointer is set), the client has
        // already authenticated, and the client is NOT attempting to
        // re-authenticate (dermined by an Authorization record being sent),
        // then we want to set the local authenticate flag to true so that
        // the authentication logic is skipped.
        String authstr;
        if (sa && sa->getClientAuthenticated() &&
            !HTTPMessage::lookupHeader(
                 headers, "Authorization", authstr, false))
        {
            authenticated = true;
        }

        if (!sa)
        {
            authenticated = httpMessage->authInfo->isAuthenticated();
        }
#else
        // Client may have already authenticated via SSL.
        // In this case, no further attempts to authenticate the client are made
        authenticated = httpMessage->authInfo->isAuthenticated();
#endif

        // Get the user name associated with the certificate (using the
        // certificate chain, if necessary).

/*
MEB: SSL certificate auhtentication begins:
*/
        String certUserName;
        if (authenticated &&
            (String::equal(httpMessage->authInfo->getAuthType(),
                AuthenticationInfoRep::AUTH_TYPE_SSL)))
        {
            PEG_TRACE_STRING(TRC_HTTP, Tracer::LEVEL3,
                "Client was authenticated via trusted SSL certificate.");

            String trustStore = configManager->getCurrentValue("sslTrustStore");

            if (FileSystem::isDirectory(
                    ConfigManager::getHomedPath(trustStore)))
            {
                PEG_TRACE_STRING(TRC_HTTP, Tracer::LEVEL4,
                    "Truststore is a directory, lookup username");

                // Get the client certificate chain to determine the correct
                // username mapping.  Starting with the peer certificate,
                // work your way up the chain towards the root certificate
                // until a match is found in the repository.
                Array<SSLCertificateInfo*> clientCertificateChain =
                    httpMessage->authInfo->getClientCertificateChain();
                SSLCertificateInfo* clientCertificate = NULL;

                Tracer::trace(TRC_HTTP, Tracer::LEVEL4,
                    "Client certificate chain length: %d.",
                    clientCertificateChain.size());

                Uint32 loopCount = clientCertificateChain.size() - 1;
                for (Uint32 i = 0; i <= loopCount ; i++)
                {
                    clientCertificate = clientCertificateChain[i];
                    if (clientCertificate == NULL)
                    {
                        MessageLoaderParms msgParms(
                            "Pegasus.Server.HTTPAuthenticatorDelegator."
                                "BAD_CERTIFICATE",
                            "The certificate used for authentication is not "
                                "valid.");
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
                    PEG_TRACE_STRING(TRC_HTTP, Tracer::LEVEL4,
                        "Certificate toString " +
                            clientCertificate->toString());

                    //get certificate properties
                    String issuerName = clientCertificate->getIssuerName();
                    char serialNumber[256];
                    sprintf(serialNumber, "%lu",
                        clientCertificate->getSerialNumber());

                    //
                    // The truststore type key property is deprecated. To retain
                    // backward compatibility, add the truststore type property
                    // to the key bindings and set it to cimserver truststore.
                    //

/*
MEB: Consider adding executor call here to check client certificate.
*/
                    //construct the corresponding PG_SSLCertificate instance
                    Array<CIMKeyBinding> keyBindings;
                    keyBindings.append(CIMKeyBinding(
                        "IssuerName", issuerName, CIMKeyBinding::STRING));
                    keyBindings.append(CIMKeyBinding(
                        "SerialNumber", serialNumber, CIMKeyBinding::STRING));
                    keyBindings.append(CIMKeyBinding("TruststoreType",
                        PG_SSLCERTIFICATE_TSTYPE_VALUE_SERVER));

                    CIMObjectPath cimObjectPath(
                        "localhost",
                        PEGASUS_NAMESPACENAME_CERTIFICATE,
                        PEGASUS_CLASSNAME_CERTIFICATE,
                        keyBindings);

                    PEG_TRACE_STRING(TRC_HTTP, Tracer::LEVEL4,
                        "Client Certificate COP: " + cimObjectPath.toString());

                    CIMInstance cimInstance;
                    CIMValue value;
                    Uint32 pos;
                    String userName = String::EMPTY;

                    //attempt to get the username registered to the certificate
                    try
                    {
                        cimInstance = _repository->getInstance(
                            PEGASUS_NAMESPACENAME_CERTIFICATE, cimObjectPath);

                        pos = cimInstance.findProperty("RegisteredUserName");

                        if (pos != PEG_NOT_FOUND &&
                            !(value = cimInstance.getProperty(pos).
                                  getValue()).isNull())
                        {
                            value.get(userName);

                            //
                            // If a user name is specified, our search is
                            // complete
                            //
                            if (userName.size())
                            {
                                PEG_TRACE_STRING(TRC_HTTP, Tracer::LEVEL3,
                                    "User name for certificate is " + userName);
                                certUserName = userName;
                                break;
                            }

                            // No user name is specified; continue up the chain
                            Tracer::trace(TRC_HTTP, Tracer::LEVEL4,
                                "The certificate at level %u has no "
                                    "associated username, moving up the chain",
                                i);
                        }
                        else
                        {
                            Logger::put(
                                Logger::ERROR_LOG,
                                System::CIMSERVER,
                                Logger::TRACE,
                                "HTTPAuthenticatorDelegator - Bailing, no "
                                    "username is registered to this "
                                    "certificate.");
                        }
                    }
                    catch (CIMException& e)
                    {
                        // this certificate did not have a registration
                        // associated with it; continue up the chain
                        if (e.getCode() == CIM_ERR_NOT_FOUND)
                        {
                            PEG_TRACE_STRING(TRC_HTTP, Tracer::LEVEL4,
                                "No registration for this certificate, try "
                                    "next certificate in chain");
                            continue;
                        }
                        else
                        {
                            Logger::put(
                                Logger::ERROR_LOG,
                                System::CIMSERVER,
                                Logger::TRACE,
                                "HTTPAuthenticatorDelegator - Bailing, "
                                    "the certificate used for authentication "
                                    "is not valid.");
                            MessageLoaderParms msgParms(
                                "Pegasus.Server.HTTPAuthenticatorDelegator."
                                    "BAD_CERTIFICATE",
                                "The certificate used for authentication is "
                                    "not valid.");
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
                    catch (...)
                    {
                        // this scenario can occur if a certificate cached
                        // on the server was deleted openssl would not pick
                        // up the deletion but we would pick it up here when
                        // we went to look it up in the repository
                        Logger::put(
                            Logger::ERROR_LOG, System::CIMSERVER, Logger::TRACE,
                            "HTTPAuthenticatorDelegator - Bailing, the "
                                "certificate used for authentication is not "
                                "valid.");
                        MessageLoaderParms msgParms(
                            "Pegasus.Server.HTTPAuthenticatorDelegator."
                                "BAD_CERTIFICATE",
                            "The certificate used for authentication is not "
                                "valid.");
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
                } //end for clientcertificatechain
            } //end sslTrustStore directory
            else
            {
                // trustStore is a single CA file, lookup username
                // user was already verified as a valid system user during
                // server startup
                certUserName =
                    configManager->getCurrentValue("sslTrustStoreUserName");
            }

            //
            // Validate user information and assign a new session key for
            // this connection.
            //

            SessionKey key;

            if (!_authenticationManager->validateUserForHttpAuth(certUserName))
            {
                MessageLoaderParms msgParms(
                    "Pegasus.Server.HTTPAuthenticatorDelegator."
                        "BAD_CERTIFICATE_USERNAME",
                    "The username registered to this certificate is not a "
                        "valid user.");
                _sendHttpError(
                    queueId,
                    HTTP_STATUS_UNAUTHORIZED,
                    String::EMPTY,
                    MessageLoader::getMessage(msgParms),
                    closeConnect);
                PEG_METHOD_EXIT();
                return;
            }

            if (Executor::newSessionKey(certUserName.getCString(), key) != 0)
            {
                MessageLoaderParms msgParms(
                    "Pegasus.Server.HTTPAuthenticatorDelegator."
                        "BAD_CERTIFICATE_USERNAME",
                    "Failed to create new session key for user.");
                _sendHttpError(
                    queueId,
                    HTTP_STATUS_UNAUTHORIZED,
                    String::EMPTY,
                    MessageLoader::getMessage(msgParms),
                    closeConnect);
                PEG_METHOD_EXIT();
                return;
            }

            httpMessage->authInfo->setSessionKey(key);

            httpMessage->authInfo->setAuthenticatedUser(certUserName);

            PEG_TRACE_STRING(
                TRC_HTTP,
                Tracer::LEVEL3,
                "User name for certificate is " + certUserName);
            Logger::put(
                Logger::STANDARD_LOG,
                System::CIMSERVER,
                Logger::TRACE,
                "HTTPAuthenticatorDelegator - The trusted client certificate "
                    "is registered to $0.",
                certUserName);
        }
    } //end enableAuthentication

    PEG_TRACE_STRING(TRC_HTTP, Tracer::LEVEL4, "Exited authentication loop");

    AcceptLanguageList acceptLanguages;
    ContentLanguageList contentLanguages;
    try
    {
        // Get and validate the Accept-Language header, if set
        String acceptLanguageHeader;
        if (HTTPMessage::lookupHeader(
                headers,
                _HTTP_HEADER_ACCEPT_LANGUAGE,
                acceptLanguageHeader,
                false))
        {
            acceptLanguages = LanguageParser::parseAcceptLanguageHeader(
                acceptLanguageHeader);
            httpMessage->acceptLanguagesDecoded = true;
        }

        // Get and validate the Content-Language header, if set
        String contentLanguageHeader;
        if (HTTPMessage::lookupHeader(
                headers,
                _HTTP_HEADER_CONTENT_LANGUAGE,
                contentLanguageHeader,
                false))
        {
            contentLanguages = LanguageParser::parseContentLanguageHeader(
                contentLanguageHeader);
            httpMessage->contentLanguagesDecoded = true;
        }
    }
    catch (Exception& e)
    {
        // clear any existing languages to force messages to come from the
        // root bundle
        Thread::clearLanguages();
        MessageLoaderParms msgParms(
            "Pegasus.Server.HTTPAuthenticatorDelegator.REQUEST_NOT_VALID",
            "request-not-valid");
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

    Thread::setLanguages(new AcceptLanguageList(acceptLanguages));
    httpMessage->acceptLanguages = acceptLanguages;
    httpMessage->contentLanguages = contentLanguages;

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
    if (methodName == _HTTP_METHOD_MPOST)
    {
        httpMethod = HTTP_METHOD_M_POST;
    }

    if (methodName != _HTTP_METHOD_MPOST && methodName != _HTTP_METHOD)
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
             (httpVersion == _HTTP_VERSION_1_0))
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

        PEG_LOGGER_TRACE((
            Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            "HTTPAuthenticatorDelegator - M-POST/POST processing start"));

        httpMessage->message.append('\0');

        if (!authenticated && enableAuthentication)
        {
            //
            // Search for Authorization header:
            //
            String authorization;

            if (HTTPMessage::lookupHeader(headers,
                    _HTTP_HEADER_PEGASUSAUTHORIZATION, authorization, false) &&
                enableAuthentication)
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
                        String authChallenge;
                        String authResp;

                        authResp = _authenticationManager->
                            getPegasusAuthResponseHeader(
                                authorization,
                                httpMessage->authInfo);

                        if (!String::equal(authResp, String::EMPTY))
                        {
                            _sendChallenge(queueId, authResp,closeConnect);
                        }
                        else
                        {
                            MessageLoaderParms msgParms(
                                "Pegasus.Server.HTTPAuthenticatorDelegator."
                                    "AUTHORIZATION_HEADER_ERROR",
                                "Authorization header error");
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
                catch (const CannotOpenFile&)
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

            if (HTTPMessage::lookupHeader(
                    headers, _HTTP_HEADER_AUTHORIZATION, authorization, false)
                && enableAuthentication)
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
                        // Kerberos authentication needs access to the
                        // AuthenticationInfo object for this session in
                        // order to set up the reference to the
                        // CIMKerberosSecurityAssociation object for this
                        // session.

                        String authResp =
                            _authenticationManager->getHttpAuthResponseHeader(
                                httpMessage->authInfo);
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
                            MessageLoaderParms msgParms(
                                "Pegasus.Server.HTTPAuthenticatorDelegator."
                                    "AUTHORIZATION_HEADER_ERROR",
                                "Authorization header error");
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
        } //end if (!authenticated && enableAuthentication)

#ifdef PEGASUS_KERBEROS_AUTHENTICATION
        // The pointer to the sa is created in the authenticator so we need
        // to also assign it here.
        sa = httpMessage->authInfo->getSecurityAssociation();
        if (sa)
        {
            // 0 - continue, 1 = send success, 2 = send response
            Uint32 sendAction = 0;

            // The following is processing to unwrap (decrypt) the request
            // from the client when using kerberos authentication.
            sa->unwrapRequestMessage(
                httpMessage->message, contentLength, authenticated, sendAction);

            if (sendAction)  // send success or send response
            {
                if (httpMessage->message.size() == 0)
                {
                    MessageLoaderParms msgParms(
                        "Pegasus.Server.HTTPAuthenticatorDelegator."
                            "AUTHORIZATION_HEADER_ERROR",
                        "Authorization header error");
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
                                httpMessage->message.getData(),
                                httpMessage->message.size()),
                            closeConnect);
                    }

                    if (sendAction == 2)  // Send response
                    {
                        _sendResponse(
                            queueId,
                            httpMessage->message,
                            closeConnect);
                    }
                }

                PEG_METHOD_EXIT();
                return;
            }
        }
#endif

        if (authenticated || !enableAuthentication)
        {
            // Final bastion to ensure the remote privileged user access
            // check is done as it should be
            // check for remote privileged User Access
            if (!httpMessage->authInfo->getRemotePrivilegedUserAccessChecked())
            {
                // the AuthenticationHandler did not process the
                // enableRemotePrivilegedUserAccess check
                // time to do it ourselves
                String userName = httpMessage->authInfo->getAuthenticatedUser();
                if (!AuthenticationManager::isRemotePrivilegedUserAccessAllowed(
                        userName))
                {
                    // Send client a message that we can't proceed to talk
                    // to him
                    // HTTP 401 ?
                    MessageLoaderParms msgParms(
                        "Server.CIMOperationRequestAuthorizer."
                            "REMOTE_NOT_ENABLED",
                        "Remote privileged user access is not enabled.");
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
                httpMessage->authInfo->setRemotePrivilegedUserAccessChecked();
            }

            //
            // Search for "CIMOperation" header:
            //
            String cimOperation;

            if (HTTPMessage::lookupHeader(
                headers, _HTTP_HEADER_CIMOPERATION, cimOperation, true))
            {
                PEG_LOGGER_TRACE(
                    (Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
                     "HTTPAuthenticatorDelegator - CIMOperation: $0 ",
                     cimOperation));

                MessageQueue* queue =
                    MessageQueue::lookup(_operationMessageQueueId);

                if (queue)
                {
                    httpMessage->dest = queue->getQueueId();

                    try
                    {
                        queue->enqueue(httpMessage);
                    }
                    catch (const bad_alloc&)
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
                headers, _HTTP_HEADER_CIMEXPORT, cimOperation, true))
            {
                Logger::put(
                    Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
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
                _authenticationManager->getHttpAuthResponseHeader(
                    httpMessage->authInfo);
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
                MessageLoaderParms msgParms(
                    "Pegasus.Server.HTTPAuthenticatorDelegator."
                        "AUTHORIZATION_HEADER_ERROR",
                    "Authorization header error");
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

PEGASUS_NAMESPACE_END
