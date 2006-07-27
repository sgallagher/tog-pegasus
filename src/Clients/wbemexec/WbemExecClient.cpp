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
// Author: Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
// Modified By: Warren Otsuka, Hewlett-Packard Company (warren_otsuka@hp.com)
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                  (carolann_graves@hp.com)
//              Dan Gorey, IBM (djgorey@us.ibm.com)
//              Amit Arora, IBM (amita@in.ibm.com) for Bug#1170, PEP-101
//              David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Signal.h>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/HTTPConnection.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/TimeValue.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/HTTPMessage.h>
#include <Pegasus/Common/AutoPtr.h>

#include "HttpConstants.h"
#include "WbemExecClient.h"
#include <Pegasus/Common/Network.h>
#include <iostream>

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN


static const char PASSWORD_PROMPT []  =
                     "Please enter your password: ";

static const char PASSWORD_BLANK []  =
                     "Password cannot be blank. Please re-enter your password.";

static const Uint32 MAX_PW_RETRIES =  3;


static Boolean verifyServerCertificate(SSLCertificateInfo &certInfo)
{
    //
    // If server certificate was found in CA trust store and validated, then
    // return 'true' to accept the certificate, otherwise return 'false'.
    //
    if (certInfo.getResponseCode() == 1)
    {
        return true;
    }
    else
    {
        return false;
    }
}

WbemExecClient::WbemExecClient(Uint32 timeoutMilliseconds)
    :
    MessageQueue(PEGASUS_QUEUENAME_WBEMEXECCLIENT),
    _timeoutMilliseconds(timeoutMilliseconds),
    _connected(false),
    _isRemote( false ),
    _password(String::EMPTY)
{
    // CAUTION: 
    //    Using private AutoPtr<> data members for these objects causes linker
    //    errors on some SOLARIS_SPARC_CC platforms.

    _monitor = 0;
    _httpConnector = 0;

    AutoPtr<Monitor> monitor(new Monitor());
    AutoPtr<HTTPConnector> httpConnector(new HTTPConnector(monitor.get()));

    _monitor = monitor.release();
    _httpConnector = httpConnector.release();
}

WbemExecClient::~WbemExecClient()
{
    disconnect();
    delete _httpConnector;
    delete _monitor;
}

void WbemExecClient::handleEnqueue()
{

}

void WbemExecClient::_connect(
    const String& host,
    const Uint32 portNumber,
    AutoPtr<SSLContext>& sslContext
) throw(CannotCreateSocketException, CannotConnectException,
        InvalidLocatorException)
{
    //
    // Attempt to establish a connection:
    //
    //try
    //{
    _httpConnection = _httpConnector->connect(
         host, portNumber, sslContext.get(), this);
    sslContext.release();

    //}
    // Could catch CannotCreateSocketException, CannotConnectException,
    // or InvalidLocatorException
    //catch (Exception& e)
    //{
    //    throw;
    //}

    _connected = true;
    _isRemote  = true;
    _httpConnection->setSocketWriteTimeout(_timeoutMilliseconds/1000+1);
}

void WbemExecClient::connect(
    const String& host,
    const Uint32 portNumber,
    AutoPtr<SSLContext>& sslContext,
    const String& userName,
    const String& password
) throw(AlreadyConnectedException, InvalidLocatorException,
        CannotCreateSocketException, CannotConnectException)
{
    //
    // If already connected, bail out!
    //
    if (_connected)
        throw AlreadyConnectedException();

    //
    //  If the host is empty, set hostName to "localhost"
    //
    String hostName = host;
    if (host == String::EMPTY)
    {
        hostName = "localhost";
    }

    //
    // Set authentication information
    //
    _authenticator.clear();

    if (userName.size())
    {
        _authenticator.setUserName(userName);
    }

    if (password.size())
    {
        _authenticator.setPassword(password);
    _password = password;
    }

    _connect(hostName, portNumber, sslContext);
    _isRemote  = true;
}


void WbemExecClient::connectLocal()
    throw(AlreadyConnectedException, InvalidLocatorException,
          CannotCreateSocketException, CannotConnectException)
{
    //
    // If already connected, bail out!
    //
    if (_connected)
        throw AlreadyConnectedException();

    String host = String::EMPTY;
    Uint32 portNumber = 0;

    //
    // Set authentication type
    //
    _authenticator.clear();
    _authenticator.setAuthType(ClientAuthenticator::LOCAL);

    AutoPtr<SSLContext>  sslContext;
#ifndef PEGASUS_DISABLE_LOCAL_DOMAIN_SOCKET
    _connect(host, portNumber, sslContext);
#else

    try
    {
        //
        // Look up the WBEM HTTP port number for the local system
        //
        portNumber = System::lookupPort(WBEM_HTTP_SERVICE_NAME,
            WBEM_DEFAULT_HTTP_PORT);

        //
        //  Assign host
        //
        host.assign(_getLocalHostName());

        _connect(host, portNumber, sslContext);
    }
    catch(CannotConnectException &)
    {
        //
        // Look up the WBEM HTTPS port number for the local system
        //
        portNumber = System::lookupPort(WBEM_HTTPS_SERVICE_NAME,
            WBEM_DEFAULT_HTTPS_PORT);

        //
        //  Assign host
        //
        host.assign(_getLocalHostName());

        //
        // Create SSLContext
        //

        const char* pegasusHome = getenv("PEGASUS_HOME");

        String certpath = FileSystem::getAbsolutePath(
            pegasusHome, PEGASUS_SSLCLIENT_CERTIFICATEFILE);

        String randFile = String::EMPTY;

#ifdef PEGASUS_SSL_RANDOMFILE
        randFile = FileSystem::getAbsolutePath(
            pegasusHome, PEGASUS_SSLCLIENT_RANDOMFILE);
#endif

        AutoPtr<SSLContext> sslContext(new SSLContext(certpath, verifyServerCertificate, randFile));//PEP101

        _connect(host, portNumber, sslContext);
    }
#endif
    _isRemote = false;
}

void WbemExecClient::disconnect()
{
    if (_connected)
    {
        //
        // Close the connection
        //
        _httpConnector->disconnect(_httpConnection);
        _httpConnection = 0;

        _authenticator.clear();

        _connected = false;
    }
}

/**

    Prompt for password.

    @return  String value of the user entered password

 */
String WbemExecClient::_promptForPassword()
{
  //
  // Password is not set, prompt for the old password once
  //
  String pw = String::EMPTY;
  Uint32 retries = 1;
  do
    {
      pw = System::getPassword( PASSWORD_PROMPT );

      if ( pw == String::EMPTY || pw == "" )
    {
      if( retries < MAX_PW_RETRIES )
        {
          retries++;

        }
      else
        {
          break;
        }
      cerr << PASSWORD_BLANK << endl;
      pw = String::EMPTY;
      continue;
    }
    }
  while ( pw == String::EMPTY );
  return( pw );
}


Buffer WbemExecClient::issueRequest(
    const Buffer& request
)
{
    if (!_connected)
    {
        throw NotConnectedException();
    }

    HTTPMessage* httpRequest = new HTTPMessage(request);

    _authenticator.setRequestMessage(httpRequest);

    Boolean finished = false;
    Boolean challenge = false;
    HTTPMessage* httpResponse;
    do
    {
        HTTPMessage* httpRequestCopy =
            new HTTPMessage(*(HTTPMessage*)_authenticator.getRequestMessage());
        _addAuthHeader(httpRequestCopy);

        Message* response = _doRequest(httpRequestCopy);
        PEGASUS_ASSERT(response->getType() == HTTP_MESSAGE);
        httpResponse = (HTTPMessage*)response;

        finished = !_checkNeedToResend(httpResponse);
        if (!finished)
        {
            if (!challenge)
            {
                challenge = true;
        if( ( _password == String::EMPTY ) && _isRemote )
          {
            _password = _promptForPassword();
            _authenticator.setPassword( _password );
          }
            }
            else
            {
                break;
            }
            delete httpResponse;
        }
    } while (!finished);

    AutoPtr<HTTPMessage> origRequest(
        (HTTPMessage*)_authenticator.releaseRequestMessage());

    AutoPtr<HTTPMessage> destroyer(httpResponse);

    return(httpResponse->message);
}

Message* WbemExecClient::_doRequest(HTTPMessage * request)
{
    // ATTN-RK-P2-20020416: We should probably clear out the queue first.
    PEGASUS_ASSERT(getCount() == 0);  // Shouldn't be any messages in our queue

    _httpConnection->enqueue(request);

    Uint64 startMilliseconds = TimeValue::getCurrentTime().toMilliseconds();
    Uint64 nowMilliseconds = startMilliseconds;
    Uint64 stopMilliseconds = nowMilliseconds + _timeoutMilliseconds;

    while (nowMilliseconds < stopMilliseconds)
    {
    //
    // Wait until the timeout expires or an event occurs:
    //
    _monitor->run(Uint32(stopMilliseconds - nowMilliseconds));

    //
    // Check to see if incoming queue has a message
    //

    Message* response = dequeue();

    if (response)
    {
            return response;
    }

        nowMilliseconds = TimeValue::getCurrentTime().toMilliseconds();
    }

    //
    // Throw timed out exception:
    //

    throw ConnectionTimeoutException();
}

String WbemExecClient::_getLocalHostName()
{
    static String hostname;

    if (!hostname.size())
    {
        hostname.assign(System::getHostName());
    }

    return hostname;
}

void WbemExecClient::_addAuthHeader(HTTPMessage*& httpMessage)
{
    //
    // Add authentication headers to the message
    //
    String authHeader = _authenticator.buildRequestAuthHeader();
    if (authHeader != String::EMPTY)
    {
        // Find the separator between the start line and the headers, so we
        // can add the authorization header there.

        httpMessage->message.append('\0');
        char* messageStart = (char*) httpMessage->message.getData();
        char* headerStart =
            (char*)memchr(messageStart, '\n', httpMessage->message.size());

        if (headerStart)
        {
            // Handle a CRLF or LF separator
            if ((headerStart != messageStart) && (headerStart[-1] == '\r'))
            {
                headerStart[-1] = 0;
            }
            else
            {
                *headerStart = 0;
            }

            headerStart++;

            // Build a new message with the original start line, the new
            // authorization header, and the original headers and content.

            Buffer newMessageBuffer;
            newMessageBuffer << messageStart << HTTP_CRLF;
            newMessageBuffer << authHeader << HTTP_CRLF;
            newMessageBuffer << headerStart << HTTP_CRLF;

            HTTPMessage* newMessage = new HTTPMessage(newMessageBuffer);
            delete httpMessage;
            httpMessage = newMessage;
        }
    }
}

Boolean WbemExecClient::_checkNeedToResend(HTTPMessage* httpMessage)
{
    //
    // Parse the HTTP message:
    //

    String startLine;
    Array<HTTPHeader> headers;
    Uint32 contentLength;

    httpMessage->parse(startLine, headers, contentLength);

    try
    {
        return _authenticator.checkResponseHeaderForChallenge(headers);
    }
    catch(InvalidAuthHeader&)
    {
        // We're done, send (garbage) response back to the user.
        return false;
    }
}

PEGASUS_NAMESPACE_END
