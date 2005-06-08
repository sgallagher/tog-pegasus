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
// Author: Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
// Modified By: Warren Otsuka, Hewlett-Packard Company (warren_otsuka@hp.com)
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                  (carolann_graves@hp.com)
//              Dan Gorey, IBM (djgorey@us.ibm.com)
//              Amit K Arora (amita@in.ibm.com) for PEP-101
//              David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
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
#include <Pegasus/Common/AutoPtr.h>
#include <Pegasus/Client/ClientAuthenticator.h>

PEGASUS_NAMESPACE_BEGIN

/** This class provides the interface that a client uses to communicate
    with a CIMOM.
*/
class WbemExecClient : public MessageQueue
{
public:

    enum { DEFAULT_TIMEOUT_MILLISECONDS = 20000 };

    /** Constructor for a CIM Client object.
        @param timeoutMilliseconds Defines the number of milliseconds the
        WbemExecClient will wait for a response to an outstanding request.
        If a request times out, the connection gets reset (disconnected and
        reconnected).  Default is 20 seconds (20000 milliseconds).
    */
    WbemExecClient(Uint32 timeoutMilliseconds = DEFAULT_TIMEOUT_MILLISECONDS);

    ///
    virtual ~WbemExecClient();

    /**  TBD
    */
    // ATTN-RK-P3-20020416: This should be hidden from client apps
    virtual void handleEnqueue();

    /** TBD
    */
    Uint32 getTimeout() const
    {
	return _timeoutMilliseconds;
    }

    /** Sets the timeout in milliseconds for the WbemExecClient.
    */
    void setTimeout(Uint32 timeoutMilliseconds)
    {
	_timeoutMilliseconds = timeoutMilliseconds;
    }

    /** connect - Creates an HTTP connection with the server
        defined by the URL in address.
        @param host - String defining the server to which the client should
        connect
        @param portNumber - Uint32 defining the port number for the server
        to which the client should connect
        @param userName - String containing the name of the user
        the client is connecting as.
        @param password - String containing the password of the user
        the client is connecting as.
        @return - No return defined. Failure to connect throws an exception
        @exception AlreadyConnectedException
            If a connection has already been established.
        @exception InvalidLocatorException
            If the specified address is improperly formed.
        @exception CannotCreateSocketException
            If a socket cannot be created.
        @exception CannotConnectException
            If the socket connection fails.
        <PRE>
            TBD
        </PRE>
    */
    inline void connect(
        const String& host,
        const Uint32 portNumber,
        const String& userName,
        const String& password
    ) throw(AlreadyConnectedException, InvalidLocatorException,
            CannotCreateSocketException, CannotConnectException)
    {
        AutoPtr<SSLContext> sslContext;
        connect(host, portNumber, sslContext, userName, password);
    }

    /** connect - Creates an HTTP connection with the server
        defined by the URL in address.
        @param host - String defining the server to which the client should
        connect
        @param portNumber - Uint32 defining the port number for the server
        to which the client should connect
        @param sslContext - The SSL context to use for this connection
        @param userName - String containing the name of the user
        the client is connecting as.
        @param password - String containing the password of the user
        the client is connecting as.
        @return - No return defined. Failure to connect throws an exception
        @exception AlreadyConnectedException
            If a connection has already been established.
        @exception InvalidLocatorException
            If the specified address is improperly formed.
        @exception CannotCreateSocketException
            If a socket cannot be created.
        @exception CannotConnectException
            If the socket connection fails.
        <PRE>
            TBD
        </PRE>
    */
    void connect(
        const String& host,
        const Uint32 portNumber,
        AutoPtr<SSLContext>& sslContext,
        const String& userName,
        const String& password
    ) throw(AlreadyConnectedException, InvalidLocatorException,
            CannotCreateSocketException, CannotConnectException);

    /** connectLocal - Creates connection to the server for
        Local clients. The connectLocal connects to the CIM server
        running on the local system in the default location.  The
        connection is automatically authenticated for the current
        user.
        @return - No return defined. Failure to connect throws an exception.
        @see connect - The exceptions are defined in connect.
    */
    void connectLocal()
        throw(AlreadyConnectedException, InvalidLocatorException,
              CannotCreateSocketException, CannotConnectException);

    /** disconnect - Closes the connection with the server if the connection
        was open, simply returns if the connection was not open. Clients are
        expected to use this method to close the open connection before
        opening a new connection.
        @return - No return defined.
    */
    void disconnect();


    /** ATTN TBD
        @exception NotConnectedException
        @exception ConnectionTimeoutException
        @exception UnauthorizedAccess
    */
    Array<char> issueRequest(const Array<char>& request);

private:

    void _connect(
        const String& host,
        const Uint32 portNumber,
        AutoPtr<SSLContext>& sslContext)
      throw(CannotCreateSocketException, CannotConnectException,
            InvalidLocatorException);

    Message* _doRequest(HTTPMessage * request);

    void _addAuthHeader(HTTPMessage*& httpMessage);

    Boolean _checkNeedToResend(HTTPMessage* httpMessage);

    String _getLocalHostName();
    String _promptForPassword();

    AutoPtr<Monitor> _monitor;//PEP101
    AutoPtr<HTTPConnector> _httpConnector;//PEP101
    HTTPConnection* _httpConnection;

    Uint32 _timeoutMilliseconds;
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
