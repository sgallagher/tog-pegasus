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

#ifndef Pegasus_WbemExecClient_h
#define Pegasus_WbemExecClient_h

#include <fstream>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Monitor.h>
#include <Pegasus/Common/HTTPConnector.h>
#include <Pegasus/Common/SSLContext.h>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Client/ClientAuthenticator.h>

PEGASUS_NAMESPACE_BEGIN

static const char CERTIFICATE[] = "server.pem";

static const char RANDOMFILE[]  = "ssl.rnd";


/** This class provides the interface that a client uses to communicate
    with a CIMOM.
*/
class WbemExecClient : public MessageQueue
{
public:

    enum { DEFAULT_TIMEOUT_MILLISECONDS = 20000 };

    /** Constructor for a CIM Client object.
    @param timeOutMilliseconds Defines the number of milliseconds
    of inactivity before the connection will timeout
    // ATTN-RK-P3-20020308: This means the time the CIMClient will wait for
    // a response to an outstanding request, right?  Not inactivity on the
    // connection?  If a request times out, does the connection remain
    // active?
    
    <PRE>
        CIMClient client(60 * 1000);

        char * connection = connectionList[i].allocateCString();
        cout << "connecting to " << connection << endl;
        client.connect(connection);
    </PRE>
    @exception ATTN-TBD
    */
    WbemExecClient(Uint32 timeOutMilliseconds = DEFAULT_TIMEOUT_MILLISECONDS);

    ///
    virtual ~WbemExecClient();

    /**  TBD
    */
    // ATTN-RK-P3-20020416: This should be hidden from client apps
    virtual void handleEnqueue();

    /** TBD
    */
    Uint32 getTimeOut() const
    {
	return _timeOutMilliseconds;
    }

    /** Sets the timeout in milliseconds for the CIMClient.
    */
    void setTimeOut(Uint32 timeOutMilliseconds)
    {
	_timeOutMilliseconds = timeOutMilliseconds;
    }

    /** connect - Creates an HTTP connection with the server
        defined by the URL in address.
        @param address - String defining the URL of the server
        to which the client should connect
        @param userName - String containing the name of the user
        the client is connecting as.
        @param password - String containing the password of the user
        the client is connecting as.
        @return - No return defined. Failure to connect throws an exception
        @exception AlreadyConnected
            If a connection has already been established.
        @exception InvalidLocator
            If the specified address is improperly formed.
        @exception CannotCreateSocket
            If a socket cannot be created.
        @exception CannotConnect
            If the socket connection fails.
        @exception UnexpectedFailure
            If any other failure occurs.
        <PRE>
            TBD
        </PRE>
    */
    inline void connect(
        const String& address,
        const String& userName = String::EMPTY,
        const String& password = String::EMPTY
    ) throw(AlreadyConnected, InvalidLocator, CannotCreateSocket,
            CannotConnect, UnexpectedFailure)
    {
        connect(address, NULL, userName, password);
    }

    /** connect - Creates an HTTP connection with the server
        defined by the URL in address.
        @param address - String defining the URL of the server
        to which the client should connect
        @param sslContext - The SSL context to use for this connection
        @param userName - String containing the name of the user
        the client is connecting as.
        @param password - String containing the password of the user
        the client is connecting as.
        @return - No return defined. Failure to connect throws an exception
        @exception AlreadyConnected
            If a connection has already been established.
        @exception InvalidLocator
            If the specified address is improperly formed.
        @exception CannotCreateSocket
            If a socket cannot be created.
        @exception CannotConnect
            If the socket connection fails.
        @exception UnexpectedFailure
            If any other failure occurs.
        <PRE>
            TBD
        </PRE>
    */
    void connect(
        const String& address,
        SSLContext* sslContext,
        const String& userName = String::EMPTY,
        const String& password = String::EMPTY
    ) throw(AlreadyConnected, InvalidLocator, CannotCreateSocket,
            CannotConnect, UnexpectedFailure);

    /** connectLocal - Creates connection to the server for
        Local clients. The connectLocal connects to the CIM server
        running on the local system in the default location.  The
        connection is automatically authenticated for the current
        user.
        @return - No return defined. Failure to connect throws an exception.
        @SeeAlso connect - The exceptions are defined in connect.
    */
    void connectLocal()
        throw(AlreadyConnected, InvalidLocator, CannotCreateSocket,
              CannotConnect, UnexpectedFailure);

    /** disconnect - Closes the connection with the server if the connection
        was open, simply returns if the connection was not open. Clients are
        expected to use this method to close the open connection before
        opening a new connection.
        @return - No return defined.
    */
    void disconnect();


    /** ATTN TBD
    */
    Array<Sint8> issueRequest(
	const Array<Sint8>& request
    ) throw(NotConnected, TimedOut, UnauthorizedAccess);

private:

    void _connect(
        const String& address,
        SSLContext* sslContext)
      throw(CannotCreateSocket, CannotConnect, InvalidLocator,
            UnexpectedFailure);

    Message* _doRequest(HTTPMessage * request) throw(TimedOut);

    void _addAuthHeader(HTTPMessage*& httpMessage);

    Boolean _checkNeedToResend(HTTPMessage* httpMessage);

    String _getLocalHostName();
    String _promptForPassword();

    Monitor* _monitor;
    HTTPConnector* _httpConnector;
    HTTPConnection* _httpConnection;
    Uint32 _timeOutMilliseconds;
    Boolean _connected;
    ClientAuthenticator _authenticator;
    Boolean _isRemote;

    /**
        The password to be used for authorization of the operation.
     */
    String _password;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_WbemExecClient_h */
