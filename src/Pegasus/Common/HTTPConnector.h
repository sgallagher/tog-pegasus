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
// Modified By: Carol Ann Krug Graves, Hewlett-Packard Company
//                (carolann_graves@hp.com)
//              Dan Gorey, IBM (djgorey@us.ibm.com)
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_HTTPConnector_h
#define Pegasus_HTTPConnector_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/MessageQueueService.h>
#include <Pegasus/Common/Monitor.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/TLS.h>
#include <Pegasus/Common/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

struct HTTPConnectorRep;

class HTTPConnection;

/** This class is used by clients to establish a connection with a
    server. For each established connection, a HTTPConnection object
    is created.
*/
class PEGASUS_COMMON_LINKAGE HTTPConnector : public MessageQueueService
{
   public:
   
      typedef MessageQueueService Base;
  
      /** Constructor.
	  @param monitor pointer to monitor object which this class uses to
	  solicit SocketMessages on the server port (socket).
	  @param outputMessageQueue ouptut message queue for connections
	  created by this connector.
      */
      HTTPConnector(Monitor* monitor);

      HTTPConnector(Monitor* monitor, SSLContext * sslcontext);
   
      /** Destructor. */
      ~HTTPConnector();

      /** This method is called whenever a SocketMessage is enqueued
	  on the input queue of the HTTPConnector object.
      */ 

      virtual void handleEnqueue(Message *);
      virtual void handleEnqueue();

      /** Establishes a new connection and creates an HTTPConnection object
	  to represent it.

	  @param host indicates host to connect to
	  @param portNumber indicates port number to use
	  @param outputMessageQueue output message queue for the HTTPConnection
	  that will be created.
	  @exception InvalidLocatorException
	  @exception CannotCreateSocketException
	  @exception CannotConnectException
      */
      inline HTTPConnection* connect(
         const String& host, 
         const Uint32 portNumber,
	 MessageQueue* outputMessageQueue)
      {
          return connect(host, portNumber, NULL, outputMessageQueue);
      }

      /** Establishes a new connection and creates an HTTPConnection object
	  to represent it.

	  @param host indicates host to connect to
	  @param portNumber indicates port number to use
	  @param sslContext Specifies the SSL context to use for this connection
	  @param outputMessageQueue output message queue for the HTTPConnection
	  that will be created.
	  @exception InvalidLocatorException
	  @exception CannotCreateSocketException
	  @exception CannotConnectException
      */
      HTTPConnection* connect(
         const String& host, 
         const Uint32 portNumber,
	 SSLContext * sslContext,
	 MessageQueue* outputMessageQueue);

      /** Destroys all the connections created by this connector. */
      void destroyConnections();

      /** Close the specified connection. */
      void disconnect(HTTPConnection* connection);

   private:

      /** Delete the specified connection. */
      void _deleteConnection(HTTPConnection* httpConnection);

      Monitor* _monitor;
      HTTPConnectorRep* _rep;
    
      SSLContext * _sslcontext;
      int _entry_index;
      
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_HTTPConnector_h */
