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
//         Jenny Yu, Hewlett-Packard Company (jenny_yu@hp.com)
//         Nag Boranna, Hewlett-Packard Company (nagaraja_boranna@hp.com)
//         Dave Rosckes (rosckes@us.ibm.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include "Config.h"
#include "Constants.h"
#include <iostream>
#include "Socket.h"

#ifdef PEGASUS_PLATFORM_WIN32_IX86_MSVC
#include <windows.h>
#else
# include <cctype>
# include <cstdlib>
# include <errno.h>
# include <fcntl.h>
# include <netdb.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <sys/socket.h>
# ifdef PEGASUS_LOCAL_DOMAIN_SOCKET
# include <unistd.h>
#  include <sys/un.h>
# endif
#endif

#include "Socket.h"
#include "TLS.h"
#include "HTTPAcceptor.h"
#include "HTTPConnection.h"
#include "Tracer.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
//
// HTTPAcceptorRep
//
////////////////////////////////////////////////////////////////////////////////

class HTTPAcceptorRep
{
public:
    HTTPAcceptorRep(Boolean local)
    {
        if (local)
        {
#ifdef PEGASUS_LOCAL_DOMAIN_SOCKET
            address = reinterpret_cast<struct sockaddr*>(new struct sockaddr_un);
            address_size = sizeof(struct sockaddr_un);
#else
            PEGASUS_ASSERT(false);
#endif
        }  
        else
        {
            address = reinterpret_cast<struct sockaddr*>(new struct sockaddr_in);
            address_size = sizeof(struct sockaddr_in);
        }
    }

    struct sockaddr* address;

#if defined(PEGASUS_PLATFORM_ZOS_ZSERIES_IBM)
   size_t address_size;
#elif defined(PEGASUS_PLATFORM_AIX_RS_IBMCXX) || defined(PEGASUS_PLATFORM_LINUX_GENERIC_GNU)
   socklen_t address_size;
#else
   int address_size;
#endif
      Mutex _connection_mut;
      
      Sint32 socket;
      Array<HTTPConnection*> connections;
};

////////////////////////////////////////////////////////////////////////////////
//
// HTTPAcceptor
//
////////////////////////////////////////////////////////////////////////////////

HTTPAcceptor::HTTPAcceptor(Monitor* monitor,
                           MessageQueue* outputMessageQueue,
                           Boolean localConnection,
                           Uint32 portNumber,
                           SSLContext * sslcontext)
   : Base(PEGASUS_QUEUENAME_HTTPACCEPTOR),  // ATTN: Need unique names?
     _monitor(monitor),
     _outputMessageQueue(outputMessageQueue),
     _rep(0),
     _entry_index(-1),
     _localConnection(localConnection),
     _portNumber(portNumber),
     _sslcontext(sslcontext)
{
   Socket::initializeInterface();
}

HTTPAcceptor::~HTTPAcceptor()
{
   unbind();
   // ATTN: Is this correct in a multi-HTTPAcceptor server?
   Socket::uninitializeInterface();
}

void HTTPAcceptor::handleEnqueue(Message *message)
{
   if (! message)
      return;
   
   switch (message->getType())
   {
      case SOCKET_MESSAGE:
      {
	 SocketMessage* socketMessage = (SocketMessage*)message;
	 
	 // If this is a connection request:

	 if (socketMessage->socket == _rep->socket &&
	     socketMessage->events | SocketMessage::READ)
	 {
	    _acceptConnection();
	 }
	 else
	 {
	    // ATTN! this can't happen!
	 }

	 break;
      }

      case CLOSE_CONNECTION_MESSAGE:
      {
	 CloseConnectionMessage* closeConnectionMessage 
	    = (CloseConnectionMessage*)message;

	 _rep->_connection_mut.lock(pegasus_thread_self());
	 
	 for (Uint32 i = 0, n = _rep->connections.size(); i < n; i++)
	 {
	    HTTPConnection* connection = _rep->connections[i];	
	    Sint32 socket = connection->getSocket();

	    if (socket == closeConnectionMessage->socket)
	    {
	       _monitor->unsolicitSocketMessages(socket);
	       _rep->connections.remove(i);
               delete connection;
	       break;
	    }
	 }
	 _rep->_connection_mut.unlock();
      }

      default:
      // ATTN: need unexpected message error!
      break;
   };

   delete message;
}


void HTTPAcceptor::handleEnqueue()
{
   Message* message = dequeue();

   if (!message)
      return;
   
   handleEnqueue(message);

}

void HTTPAcceptor::bind()
{
   if (_rep)
      throw BindFailedException("HTTPAcceptor already bound");

   _rep = new HTTPAcceptorRep(_localConnection);

   // bind address
   _bind();

   return;
}

/**
   _bind - creates a new server socket and bind socket to the port address.
   If PEGASUS_LOCAL_DOMAIN_SOCKET is defined, the port number is ignored and
   a domain socket is bound.
*/
void HTTPAcceptor::_bind()
{

   // Create address:

   memset(_rep->address, 0, sizeof(*_rep->address));

   if (_localConnection)
   {
#ifdef PEGASUS_LOCAL_DOMAIN_SOCKET
       reinterpret_cast<struct sockaddr_un*>(_rep->address)->sun_family =
           AF_UNIX;
       strcpy(reinterpret_cast<struct sockaddr_un*>(_rep->address)->sun_path,
              PEGASUS_LOCAL_DOMAIN_SOCKET_PATH);
       ::unlink(reinterpret_cast<struct sockaddr_un*>(_rep->address)->sun_path);
#else
       PEGASUS_ASSERT(false);
#endif
   }
   else
   {
       reinterpret_cast<struct sockaddr_in*>(_rep->address)->sin_addr.s_addr =
           INADDR_ANY;
       reinterpret_cast<struct sockaddr_in*>(_rep->address)->sin_family =
           AF_INET;
       reinterpret_cast<struct sockaddr_in*>(_rep->address)->sin_port =
           htons(_portNumber);
   }

   // Create socket:
    
   if (_localConnection)
   {
       _rep->socket = socket(AF_UNIX, SOCK_STREAM, 0);
   }
   else
   {
       _rep->socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
   }

   if (_rep->socket < 0)
   {
      delete _rep;
      _rep = 0;
      throw BindFailedException("Failed to create socket");
   }


// set the close-on-exec bit for this file handle.
// any unix that forks needs this bit set. 
#ifndef PEGASUS_PLATFORM_WIN32_IX86_MSVC
   int sock_flags;
 if( (sock_flags = fcntl(_rep->socket, F_GETFD, 0)) < 0)
   {
       PEG_TRACE_STRING(TRC_HTTP, Tracer::LEVEL2,
                        "HTTPAcceptor: fcntl(F_GETFD) failed");
   }
   else
   {
      sock_flags |= FD_CLOEXEC;
      if (fcntl(_rep->socket, F_SETFD, sock_flags) < 0)
      {
       PEG_TRACE_STRING(TRC_HTTP, Tracer::LEVEL2,
                        "HTTPAcceptor: fcntl(F_SETFD) failed");
      }
   }
#endif 


   //
   // Set the socket option SO_REUSEADDR to reuse the socket address so
   // that we can rebind to a new socket using the same address when we
   // need to resume the cimom as a result of a timeout during a Shutdown
   // operation.
   //
   int opt=1;
   if (setsockopt(_rep->socket, SOL_SOCKET, SO_REUSEADDR,
		  (char *)&opt, sizeof(opt)) < 0)
   {
      Socket::close(_rep->socket);
      delete _rep;
      _rep = 0;
      throw BindFailedException("Failed to set socket option");
   }

   // Bind socket to port:

   if (::bind(_rep->socket, _rep->address, _rep->address_size) < 0)
   {
      Socket::close(_rep->socket);
      delete _rep;
      _rep = 0;
      throw BindFailedException("Failed to bind socket");
   }

   // Set up listening on the given socket:

   int const MAX_CONNECTION_QUEUE_LENGTH = 5;

   if (listen(_rep->socket, MAX_CONNECTION_QUEUE_LENGTH) < 0)
   {
      Socket::close(_rep->socket);
      delete _rep;
      _rep = 0;
      throw BindFailedException("Failed to bind socket");
   }

   // Register to receive SocketMessages on this socket:

   if ( -1 == ( _entry_index = _monitor->solicitSocketMessages(
	  _rep->socket,
	  SocketMessage::READ | SocketMessage::EXCEPTION,
	  getQueueId(), 
	  Monitor::ACCEPTOR)))
   {
      Socket::close(_rep->socket);
      delete _rep;
      _rep = 0;
      throw BindFailedException("Failed to solicit socket messaeges");
   }
}

/**
   closeConnectionSocket - close the server listening socket to disallow
   new client connections.
*/
void HTTPAcceptor::closeConnectionSocket()
{
   if (_rep)
   {
      // unregister the socket
      _monitor->unsolicitSocketMessages(_rep->socket);

      // close the socket
      Socket::close(_rep->socket);
   }
}

/**
   reopenConnectionSocket - creates a new server socket.
*/
void HTTPAcceptor::reopenConnectionSocket()
{
   if (_rep)
   {
      _bind();
   }
}

/**
   getOutstandingRequestCount - returns the number of outstanding requests.
*/
Uint32 HTTPAcceptor::getOutstandingRequestCount()
{
   Uint32 count = 0;
   
   _rep->_connection_mut.lock(pegasus_thread_self());
   if (_rep->connections.size() > 0)
   {
      HTTPConnection* connection = _rep->connections[0];	
      count = connection->getRequestCount();
   }
   _rep->_connection_mut.unlock();
   return count;
}

void HTTPAcceptor::unbind()
{
   if (_rep)
   {
      Socket::close(_rep->socket);

      if (_localConnection)
      {
#ifdef PEGASUS_LOCAL_DOMAIN_SOCKET
         ::unlink(
             reinterpret_cast<struct sockaddr_un*>(_rep->address)->sun_path);
#else
         PEGASUS_ASSERT(false);
#endif
      }

      delete _rep;
      _rep = 0;
   }
}

void HTTPAcceptor::destroyConnections()
{


   // For each connection created by this object:

   _rep->_connection_mut.lock(pegasus_thread_self());
   for (Uint32 i = 0, n = _rep->connections.size(); i < n; i++)
   {
      HTTPConnection* connection = _rep->connections[i];	
      Sint32 socket = connection->getSocket();

      // Unsolicit SocketMessages:

      _monitor->unsolicitSocketMessages(socket);

      // Destroy the connection (causing it to close):

      while (connection->refcount.value()) { }
      delete connection;
   }

   _rep->connections.clear();
   _rep->_connection_mut.unlock();
}

void HTTPAcceptor::_acceptConnection()
{
   // This function cannot be called on an invalid socket!

   PEGASUS_ASSERT(_rep != 0);

   if (!_rep)
      return;

   // Accept the connection (populate the address):

   struct sockaddr* accept_address;
#if defined(PEGASUS_PLATFORM_ZOS_ZSERIES_IBM)
   size_t address_size;
#elif defined(PEGASUS_PLATFORM_AIX_RS_IBMCXX) || defined(PEGASUS_PLATFORM_LINUX_GENERIC_GNU)
   socklen_t address_size;
#else
   int address_size;
#endif

   if (_localConnection)
   {
#ifdef PEGASUS_LOCAL_DOMAIN_SOCKET
       accept_address = reinterpret_cast<struct sockaddr*>(new struct sockaddr_un);
       address_size = sizeof(struct sockaddr_un);
#else
       PEGASUS_ASSERT(false);
#endif
   }
   else
   {
       accept_address = reinterpret_cast<struct sockaddr*>(new struct sockaddr_in);
       address_size = sizeof(struct sockaddr_in);
   }

   Sint32 socket = accept(_rep->socket, accept_address, &address_size);

   delete accept_address;

   if (socket < 0)
   {
       Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
		   "HTTPAcceptor - accept() failure.  errno: $0"
		   ,errno);

       PEG_TRACE_STRING(TRC_HTTP, Tracer::LEVEL2,
                        "HTTPAcceptor: accept() failed");
      return;
   }

// set the close on exec flag 
#ifndef PEGASUS_PLATFORM_WIN32_IX86_MSVC
   int sock_flags;
 if( (sock_flags = fcntl(socket, F_GETFD, 0)) < 0)
   {
       PEG_TRACE_STRING(TRC_HTTP, Tracer::LEVEL2,
                        "HTTPAcceptor: fcntl(F_GETFD) failed");
   }
   else
   {
      sock_flags |= FD_CLOEXEC;
      if (fcntl(socket, F_SETFD, sock_flags) < 0)
      {
       PEG_TRACE_STRING(TRC_HTTP, Tracer::LEVEL2,
                        "HTTPAcceptor: fcntl(F_SETFD) failed");
      }
   }
#endif 


   Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
	       "HTTPAcceptor - accept() success.  Socket: $1"
	       ,socket);

   // Create a new conection and add it to the connection list:

   MP_Socket * mp_socket = new MP_Socket(socket, _sslcontext);
   if (mp_socket->accept() < 0) 
   {
       PEG_TRACE_STRING(TRC_HTTP, Tracer::LEVEL2,
                        "HTTPAcceptor: SSL_accept() failed");
      return;
   }

   HTTPConnection* connection = new HTTPConnection(
      _monitor, mp_socket, this, static_cast<MessageQueue *>(_outputMessageQueue));

   // Solicit events on this new connection's socket:
   int index;
   
   if (-1 ==  (index = _monitor->solicitSocketMessages(
	  socket,
	  SocketMessage::READ | SocketMessage::EXCEPTION,
	  connection->getQueueId(), Monitor::CONNECTION)) )
   {
      delete connection;
      Socket::close(socket);
   }

   // Save the socket for cleanup later:
   connection->_entry_index = index;
      _rep->_connection_mut.lock(pegasus_thread_self());
   _rep->connections.append(connection);
   _rep->_connection_mut.unlock();
}

PEGASUS_NAMESPACE_END
