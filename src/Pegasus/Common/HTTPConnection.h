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
// Modified By:
//         Nag Boranna, Hewlett-Packard Company(nagaraja_boranna@hp.com)
//         Jenny Yu, Hewlett-Packard Company (jenny_yu@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_HTTPConnection_h
#define Pegasus_HTTPConnection_h

#include <iostream>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/MessageQueue.h>
#include <Pegasus/Common/Pair.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Message.h>
#include <Pegasus/Common/ArrayInternal.h>
#include <Pegasus/Common/Monitor.h>
#include <Pegasus/Common/AuthenticationInfo.h>
#include <Pegasus/Common/TLS.h>
#include <Pegasus/Common/HTTPAcceptor.h>
#include <Pegasus/Common/pegasus_socket.h>
#include <Pegasus/Common/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

class HTTPConnector;

class MessageQueueService;

struct HTTPConnectionRep;

/** This message is sent from a connection to its owner (so that the
    owner can do any necessary cleanup).
*/
class CloseConnectionMessage : public Message
{
   public:

      CloseConnectionMessage(Sint32 socket_) 
	 : Message(CLOSE_CONNECTION_MESSAGE), socket(socket_) { }

      Sint32 socket;
};

/** This class represents an HTTP listener.
*/
class Monitor;

class PEGASUS_COMMON_LINKAGE HTTPConnection : public MessageQueue
{
   public:
      typedef MessageQueue Base;
      
      /** Constructor. */
      HTTPConnection(
	 Monitor* monitor,
	 //Sint32 socket, 
	 MP_Socket * socket, 
	 MessageQueue * ownerMessageQueue,
	 MessageQueue * outputMessageQueue);
            

      /** Destructor. */
      ~HTTPConnection();

      /** This method is called whenever a SocketMessage is enqueued
	  on the input queue of the HTTPConnection object.
      */ 
      virtual void handleEnqueue(Message *);
      
      virtual void handleEnqueue();

      /** Return socket this connection is using. */
      Sint32 getSocket() { return _socket->getSocket();}
      
      /** Return the number of outstanding requests for all HTTPConnection 
	  instances.
      */
      Uint32 getRequestCount();

      Boolean run(Uint32 milliseconds);

      void lock_connection(void)
      {
         Tracer::trace(TRC_HTTP, Tracer::LEVEL4,
            "HTTPConnection::lock_connection - LOCK REQUESTED");
	 _connection_mut.lock(pegasus_thread_self());
         Tracer::trace(TRC_HTTP, Tracer::LEVEL4,
            "HTTPConnection::lock_connection - LOCK ACQUIRED");
      }
      
      void unlock_connection(void)
      {
	 _connection_mut.unlock();
         Tracer::trace(TRC_HTTP, Tracer::LEVEL4,
            "HTTPConnection::unlock_connection - LOCK RELEASED");
      } 
      
      Boolean is_dying(void)
      {
	 if( _dying.value() > 0 )
	    return true;
	 return false;
      }
      
      MessageQueue & get_owner(void)
      {
	 return *_ownerMessageQueue;
      }
      
      // ATTN-RK-P1-20020521: This is a major hack, required to get the CIM
      // server and tests to run successfully.  The problem is that the
      // HTTPAcceptor is deleting an HTTPConnection before all the threads
      // that are queued up to run in that HTTPConnection instance have had
      // a chance to finish.  This hack makes the HTTPAcceptor spin until
      // the HTTPConnection event threads have completed, before deleting
      // the HTTPConnection.  Note that this fix is not perfect, because
      // there is a window between when the HTTPConnection queue lookup is
      // done and when the refcount is incremented.  If the HTTPAcceptor
      // deletes the HTTPConnection in that window, the soon-to-be-launched
      // HTTPConnection event thread will fail (hard).
      AtomicInt refcount;

   private:

      void _clearIncoming();

      void _getContentLengthAndContentOffset();

      void _closeConnection();

      void _handleReadEvent();

      Monitor* _monitor;

      //Sint32 _socket;
      MP_Socket* _socket;
      MessageQueue* _ownerMessageQueue;
      MessageQueue* _outputMessageQueue;

      Sint32 _contentOffset; 
      Sint32 _contentLength;
      Array<Sint8> _incomingBuffer;
      AuthenticationInfo* _authInfo;
      static AtomicInt _requestCount;
      Mutex _connection_mut;
      AtomicInt _dying;
      int _entry_index;
      
      friend class Monitor;
      friend class HTTPAcceptor;
      friend class HTTPConnector;
};

class PEGASUS_COMMON_LINKAGE HTTPConnection2 : public MessageQueue
{
   public:
      typedef MessageQueue Base;
      
      /** Constructor. */
      HTTPConnection2( pegasus_socket socket,
		      MessageQueue * outputMessageQueue);

      /** Destructor. */
      ~HTTPConnection2();

      /** This method is called whenever a SocketMessage is enqueued
	  on the input queue of the HTTPConnection2 object.
      */ 
      virtual void handleEnqueue(Message *);
      
      virtual void handleEnqueue();

      /** Return socket this connection is using. */
      Sint32 getSocket();
      
      /** Return the number of outstanding requests for all HTTPConnection2 
	  instances.
      */
      Uint32 getRequestCount();


      void lock_connection(void)
      {
         Tracer::trace(TRC_HTTP, Tracer::LEVEL4,
            "HTTPConnection2::lock_connection - LOCK REQUESTED");
	 _connection_mut.lock(pegasus_thread_self());
         Tracer::trace(TRC_HTTP, Tracer::LEVEL4,
            "HTTPConnection2::lock_connection - LOCK ACQUIRED");
      }
      
      void unlock_connection(void)
      {
	 _connection_mut.unlock();
         Tracer::trace(TRC_HTTP, Tracer::LEVEL4,
            "HTTPConnection2::unlock_connection - LOCK RELEASED");
      } 
      
      Boolean is_dying(void)
      {
	 if( _dying.value() > 0 )
	    return true;
	 return false;
      }
      
      AtomicInt refcount;

      Boolean operator ==(const HTTPConnection2& );
      Boolean operator ==(void*);
      static void connection_dispatch(monitor_2_entry*);
      
   private:

      void _clearIncoming();

      void _getContentLengthAndContentOffset();

      void _closeConnection();

      void _handleReadEvent(monitor_2_entry* );

      pegasus_socket _socket;
      MessageQueue* _outputMessageQueue;

      Sint32 _contentOffset; 
      Sint32 _contentLength;
      Array<Sint8> _incomingBuffer;
      AuthenticationInfo* _authInfo;
      static AtomicInt _requestCount;
      Mutex _connection_mut;
      AtomicInt _dying;
      int _entry_index;
      
      friend class Monitor;
      friend class HTTPAcceptor;
      friend class HTTPConnector;
};


PEGASUS_NAMESPACE_END

#endif /* Pegasus_HTTPConnection_h */



// HINT: - you are not deleting the connection object !!!!! 
