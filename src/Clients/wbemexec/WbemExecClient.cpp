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
// Author: Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
// Modified By: Warren Otsuka, Hewlett-Packard Company (warren_otsuka@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/HTTPConnection.h>
#include <Pegasus/Common/Destroyer.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/TimeValue.h>
#include <Pegasus/Common/PegasusVersion.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/HTTPMessage.h>

#include "HttpConstants.h"
#include "WbemExecClient.h"

#include <iostream>
#ifdef PEGASUS_PLATFORM_WIN32_IX86_MSVC
# include <windows.h>
#else
# include <netinet/in.h>
# include <arpa/inet.h>
# include <sys/socket.h>
#endif

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN


static const char PASSWORD_PROMPT []  =
                     "Please enter your password: ";

static const char PASSWORD_BLANK []  = 
                     "Password cannot be blank. Please re-enter your password.";

static const Uint32 MAX_PW_RETRIES =  3;


static Boolean verifyServerCertificate(CertificateInfo &certInfo)
{
    //ATTN-NB-03-05132002: Add code to handle server certificate verification.
    return true;
}

WbemExecClient::WbemExecClient(Uint32 timeOutMilliseconds)
    : 
    MessageQueue(PEGASUS_QUEUENAME_WBEMEXECCLIENT),
    _httpConnection(0),
    _timeOutMilliseconds(timeOutMilliseconds),
    _connected(false),
    _isRemote( false ),
    _password( String::EMPTY )
{
    //
    // Create Monitor and HTTPConnector
    //
    _monitor = new Monitor();
    _httpConnector = new HTTPConnector(_monitor);
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
    const String& address,
    SSLContext* sslContext
) throw(CannotCreateSocketException, CannotConnectException,
        InvalidLocatorException, UnexpectedFailureException)
{
    //
    // Attempt to establish a connection:
    //
    //try
    //{
	_httpConnection = _httpConnector->connect(address,
                                                  sslContext,
                                                  this);
    //}
    // Could catch CannotCreateSocketException, CannotConnectException,
    // InvalidLocatorException, or UnexpectedFailureException
    //catch (Exception& e)
    //{
    //    throw e;
    //}
    
    _connected = true;
    _isRemote  = true;
}

void WbemExecClient::connect(
    const String& address,
    SSLContext* sslContext,
    const String& userName,
    const String& password
) throw(AlreadyConnectedException, InvalidLocatorException,
        CannotCreateSocketException, CannotConnectException,
        UnexpectedFailureException)
{
    //
    // If already connected, bail out!
    //
    if (_connected)
	throw AlreadyConnectedException();

    //
    // If the address is empty, reject it
    //
    if (address == String::EMPTY)
	throw InvalidLocatorException(address);

    //
    // Set authentication information
    //
    _authenticator.clearRequest(true);
    _authenticator.setAuthType(ClientAuthenticator::NONE);

    if (userName.size())
    {
        _authenticator.setUserName(userName);
    }

    if (password.size())
    {
        _authenticator.setPassword(password);
	_password = password;
    }

    _connect(address, sslContext);
    _isRemote  = true;
}


void WbemExecClient::connectLocal()
    throw(AlreadyConnectedException, InvalidLocatorException,
          CannotCreateSocketException, CannotConnectException,
          UnexpectedFailureException)
{
    //
    // If already connected, bail out!
    //
    if (_connected)
	throw AlreadyConnectedException();

    String      address = String::EMPTY;

    //
    // Set authentication type
    //
    _authenticator.clearRequest(true);
    _authenticator.setAuthType(ClientAuthenticator::LOCAL);

#ifdef PEGASUS_LOCAL_DOMAIN_SOCKET
    _connect(address, NULL);
#else

    try
    {
        //
        // Look up the WBEM HTTP port number for the local system
        //
        Uint32 portNum = System::lookupPort(WBEM_HTTP_SERVICE_NAME, WBEM_DEFAULT_HTTP_PORT);
        char port[32];
        sprintf(port, "%u", portNum);

        //
        // Build address string using local host name and port number
        //
        address.assign(_getLocalHostName());
        address.append(":");
        address.append(port);

        SSLContext  *sslContext = NULL;

        _connect(address, sslContext);
    }
    catch(CannotConnectException &e)
    {
        //
        // Look up the WBEM HTTPS port number for the local system
        //
        Uint32 portNum = System::lookupPort(WBEM_HTTPS_SERVICE_NAME, WBEM_DEFAULT_HTTPS_PORT);
        char port[32];
        sprintf(port, "%u", portNum);

        //
        // Build address string using local host name and port number
        //
        address.assign(_getLocalHostName());
        address.append(":");
        address.append(port);

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

        SSLContext * sslContext =
            new SSLContext(certpath, verifyServerCertificate, randFile, true);

        _connect(address, sslContext);
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

        _authenticator.clearRequest(true);

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


Array<Sint8> WbemExecClient::issueRequest(
    const Array<Sint8>& request
) throw(NotConnectedException, ConnectionTimeoutException, UnauthorizedAccess)
{
    if (!_connected)
    {
	throw NotConnectedException();
    }
    
    HTTPMessage* httpRequest = new HTTPMessage(request);
    
    _authenticator.clearRequest();
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
    
    HTTPMessage* origRequest = (HTTPMessage*)_authenticator.getRequestMessage();
    _authenticator.clearRequest();
    delete origRequest;

    Destroyer<HTTPMessage> destroyer(httpResponse);
    
    return(httpResponse->message);
}

Message* WbemExecClient::_doRequest(HTTPMessage * request)
    throw(ConnectionTimeoutException)
{
    // ATTN-RK-P2-20020416: We should probably clear out the queue first.
    PEGASUS_ASSERT(getCount() == 0);  // Shouldn't be any messages in our queue

    _httpConnection->enqueue(request);

    Uint64 startMilliseconds = TimeValue::getCurrentTime().toMilliseconds();
    Uint64 nowMilliseconds = startMilliseconds;
    Uint64 stopMilliseconds = nowMilliseconds + _timeOutMilliseconds;

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
        //
        // Parse the HTTP message:
        //

        String startLine;
        Array<HTTPHeader> headers;
        Sint8* content;
        Uint32 contentLength;
        httpMessage->parse(startLine, headers, contentLength);

        // Calculate the beginning of the content from the message size and
        // the content length

        content = (Sint8*) httpMessage->message.getData() +
	  httpMessage->message.size() - contentLength;

        Array<Sint8> newMessageBuffer;
        newMessageBuffer << startLine << HTTP_CRLF;
        newMessageBuffer << authHeader << HTTP_CRLF;
        for (Uint32 i=0; i<headers.size(); i++)
        {
            newMessageBuffer << headers[i].first << HEADER_SEPARATOR <<
                HTTP_SP << headers[i].second << HTTP_CRLF;
        }
        newMessageBuffer << HTTP_CRLF;
        newMessageBuffer << content << HTTP_CRLF;

        HTTPMessage* newMessage = new HTTPMessage(newMessageBuffer);
        delete httpMessage;
        httpMessage = newMessage;
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
