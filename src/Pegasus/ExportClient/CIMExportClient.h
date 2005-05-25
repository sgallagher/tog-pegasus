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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By: Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
//              Nag Boranna, Hewlett-Packard Company (nagaraja_boranna@hp.com)
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                  (carolann_graves@hp.com)
//              Yi Zhou, Hewlett-Packard Company (yi_zhou@hp.com)
//              Dan Gorey (djgorey@us.ibm.com)
//              David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_ExportClient_h
#define Pegasus_ExportClient_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/CIMName.h>
#include <Pegasus/Common/Monitor.h>
#include <Pegasus/Common/HTTPConnector.h>
#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Common/MessageQueue.h>
#include <Pegasus/Common/CIMObject.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/ContentLanguages.h>  //l10n
#include <Pegasus/Common/SSLContext.h>
#include <Pegasus/Common/AutoPtr.h>
#include <Pegasus/Client/ClientAuthenticator.h>
#include <Pegasus/ExportClient/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

class Monitor;
class CIMExportResponseDecoder;
class CIMExportRequestEncoder;

/** This class provides the interface that a client uses to communicate
    with a CIMOM.
*/
class PEGASUS_EXPORT_CLIENT_LINKAGE CIMExportClient : public MessageQueue
{
   public:
      enum { DEFAULT_TIMEOUT_MILLISECONDS = 20000 };

      /** Constructor for a CIM Export Client object.
      */
      CIMExportClient(
         Monitor* monitor,
         HTTPConnector* httpConnector,
         Uint32 timeoutMilliseconds = DEFAULT_TIMEOUT_MILLISECONDS);

      // Destructor for a CIM Export Client object.
      ~CIMExportClient();

      /** Gets the timeout in milliseconds for the CIMExportClient.
          Default is 20 seconds (20000 milliseconds).
      */
      Uint32 getTimeout() const
      {
         return _timeoutMilliseconds;
      }

      /** Sets the timeout in milliseconds for the CIMExportClient.
        @param timeoutMilliseconds Defines the number of milliseconds the
        CIMExportClient will wait for a response to an outstanding request.
        If a request times out, the connection gets reset (disconnected and
        reconnected).  Default is 20 seconds (20000 milliseconds).
      */
      void setTimeout(Uint32 timeoutMilliseconds)
      {
         _timeoutMilliseconds = timeoutMilliseconds;
      }

      /** Creates an HTTP connection with the server
        defined by the host and portNumber.
        @param host String defining the server to which the client should
        connect.
        @param portNumber Uint32 defining the port number for the server
        to which the client should connect.
        @exception AlreadyConnectedException
            If a connection has already been established.
        @exception InvalidLocatorException
            If the specified address is improperly formed.
        @exception CannotCreateSocketException
            If a socket cannot be created.
        @exception CannotConnectException
            If the socket connection fails.
      */
      void connect(
          const String& host,
          const Uint32 portNumber);

      /** Creates an HTTP connection with a Listener defined by
          the host and portNumber.
          @param host String defining the hostname of the listener.
          @param portNumber Uint32 defining the port number of the listener.
          @param sslContext SSL context to use for this connection.
          @exception AlreadyConnectedException
              If a connection has already been established.
          @exception InvalidLocatorException
              If the specified address is improperly formed.
          @exception CannotCreateSocketException
              If a socket cannot be created.
          @exception CannotConnectException
              If the socket connection fails.
      */
      void connect(
          const String& host,
          const Uint32 portNumber,
          const SSLContext& sslContext);


      /** Closes the connection with the server if the connection
        was open, simply returns if the connection was not open. Clients are
        expected to use this method to close the open connection before
        opening a new connection.
      */
      void disconnect();

// l10n
      /** Send indication message to the destination where the url input
        parameter defines the destination.

        @param url String defining the destination of the indication to be sent.
        @param instance CIMInstance is the indication instance which needs to
        be sent to the destination.
        @param contentLanguages The language of the indication
      */
      virtual void exportIndication(
         const String& url,
         const CIMInstance& instance,
         const ContentLanguages& contentLanguages = ContentLanguages::EMPTY);

   private:

      void _connect();

      void _disconnect();

      void _reconnect();

      Message* _doRequest(
        CIMRequestMessage * request,
        const Uint32 expectedResponseMessageType);

      String _getLocalHostName();

      Monitor* _monitor;
      HTTPConnector* _httpConnector;
      HTTPConnection* _httpConnection;

      Uint32 _timeoutMilliseconds;
      Boolean _connected;
      CIMExportResponseDecoder* _responseDecoder;
      CIMExportRequestEncoder* _requestEncoder;
      ClientAuthenticator _authenticator;
      String _connectHost;
      Uint32 _connectPortNumber;
      AutoPtr<SSLContext> _connectSSLContext;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_ExportClient_h */
