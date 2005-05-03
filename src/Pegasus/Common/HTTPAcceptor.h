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
// Modified By:  Jenny Yu (jenny_yu@hp.com)
//               Nag Boranna (nagaraja.boranna@hp.com)
//               Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_HTTPAcceptor_h
#define Pegasus_HTTPAcceptor_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/MessageQueue.h>
#include <Pegasus/Common/HTTPConnection.h>
#include <Pegasus/Common/Monitor.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/TLS.h>
#include <Pegasus/Common/IPC.h>
#if defined(PEGASUS_HAS_SSL)
#include <Pegasus/Common/SSLContext.h>
#endif
#include <Pegasus/Common/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

class HTTPAcceptorRep;
class Monitor;
/** Instances of this class listen on a port and accept conections.
*/
class PEGASUS_COMMON_LINKAGE HTTPAcceptor : public MessageQueue
{
   public:
      typedef MessageQueue Base;
  
      /** Constructor.
	  @param monitor pointer to monitor object which this class uses to
	  solicit SocketMessages on the server port (socket).
	  @param outputMessageQueue output message queue for connections
	  created by this acceptor.
	  @param localConnection Boolean indicating whether this acceptor is
	  only for local connections.  If true, the portNumber argument is
	  ignored.
	  @param portNumber Specifies which port number this acceptor is to
	  listen on.  If this value is 0 then a port is automatically selected
	  by bind().  In this case, the actual port number used can be retrieved
	  using getPortNumber().
	  @param sslcontext If null, this acceptor does not create SSL
	  connections.  If non-null, the argument specifies an SSL context to
	  use for connections established by this acceptor.
          @param exportConnection Boolean indicating whether this acceptor is
          is only for export connections. If true, client SSL certificate 
          verification is enabled on the connection created by this acceptor.
          Ignored when sslcontext is null.
      */
      HTTPAcceptor(Monitor* monitor,
                   MessageQueue* outputMessageQueue,
                   Boolean localConnection,
                   Uint32 portNumber,
                   SSLContext * sslcontext,
                   Boolean exportConnection,
                   ReadWriteSem* sslContextObjectLock=0);

      /** Destructor. */
      ~HTTPAcceptor();

      /** This method is called whenever a SocketMessage is enqueued
	  on the input queue of the HTTPAcceptor object.
      */ 

      virtual void handleEnqueue(Message *);
    
      virtual void handleEnqueue();

      /** Bind the specified listen socket.
          @exception throws BindFailedException if unable to bind (either
          because the listen socket is invalid or the socket is in use).
      */
      void bind();

      /** Unbind from the given port.
       */
      void unbind();

      /** Close the connection socket.
       */
      void closeConnectionSocket();

      /** Reopen the connection socket.
       */
      void reopenConnectionSocket();

      /** Destroys all the connections created by this acceptor. */
      void destroyConnections();

      /** Returns the number of outstanding requests
       */
      Uint32 getOutstandingRequestCount() const;

      /** Returns the port number used for the connection.  If the number
          is 0 that means that the socket is not bound.
      */
      Uint32 getPortNumber() const;

   private:

      void _acceptConnection();
      void _bind();

      cimom *_meta_dispatcher;
    

      Monitor* _monitor;
      MessageQueue* _outputMessageQueue;
      HTTPAcceptorRep* _rep;

      int _entry_index;

      Boolean _localConnection;
      Uint32  _portNumber;
      SSLContext * _sslcontext;
      Boolean _exportConnection;
      ReadWriteSem*  _sslContextObjectLock;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_HTTPAcceptor_h */
