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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By:  Jenny Yu (jenny_yu@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_HTTPAcceptor_h
#define Pegasus_HTTPAcceptor_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/MessageQueue.h>
#include <Pegasus/Common/Monitor.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/TLS.h>
#if defined(PEGASUS_HAS_SSL)
#include <Pegasus/Common/SSLContext.h>
#endif
#include <Pegasus/Common/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

struct HTTPAcceptorRep;

/** Instances of this class listen on a port and accept conections.
*/
class PEGASUS_COMMON_LINKAGE HTTPAcceptor : public MessageQueue
{
   public:
      typedef MessageQueue Base;
  
      /** Constructor.
	  @param monitor pointer to monitor object which this class uses to
	  solicit SocketMessages on the server port (socket).
	  @param outputMessageQueue ouptut message queue for connections
	  created by this acceptor.
      */
      HTTPAcceptor(Monitor* monitor, MessageQueue* outputMessageQueue);

      HTTPAcceptor(Monitor* monitor, MessageQueue* outputMessageQueue, 
		   SSLContext * sslcontext);

      /** Destructor. */
      ~HTTPAcceptor();

      /** This method is called whenever a SocketMessage is enqueued
	  on the input queue of the HTTPAcceptor object.
      */ 

      virtual void handleEnqueue(Message *);
    
      virtual void handleEnqueue();

      /** Bind to the given port (the port on which this acceptor listens for 
	  connections).
	  @param portNumber the number of the port used to listen for
	  connection requests.
	  @exception throws BindFailed is unable to bind to the given
	  port (either because the port number is invalid or the
	  port is in use).
      */
      void bind(Uint32 portNumber);

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
      Uint32 getOutstandingRequestCount();

   private:

      void _acceptConnection();
      void _bind();

      cimom *_meta_dispatcher;
    

      Monitor* _monitor;
      MessageQueue* _outputMessageQueue;
      HTTPAcceptorRep* _rep;

      SSLContext * _sslcontext;
      Uint32  _portNumber;
      int _entry_index;
      
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_HTTPAcceptor_h */
