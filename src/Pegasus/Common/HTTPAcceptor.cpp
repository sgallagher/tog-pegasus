//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 BMC Software, Hewlett-Packard Company, IBM, 
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
//
//%/////////////////////////////////////////////////////////////////////////////

#include "Config.h"
#include <iostream>
#include "Socket.h"

#ifdef PEGASUS_PLATFORM_WIN32_IX86_MSVC
# include <winsock.h>
#else
# include <cctype>
# include <unistd.h>
# include <cstdlib>
# include <errno.h>
# include <fcntl.h>
# include <netdb.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <sys/socket.h>
#endif

#include "Socket.h"
#include "TLS.h"
#include "HTTPAcceptor.h"
#include "HTTPConnection.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
//
// HTTPAcceptorRep
//
////////////////////////////////////////////////////////////////////////////////

struct HTTPAcceptorRep
{
      struct sockaddr_in address;
      Sint32 socket;
      Array<HTTPConnection*> connections;
};

////////////////////////////////////////////////////////////////////////////////
//
// HTTPAcceptor
//
////////////////////////////////////////////////////////////////////////////////

HTTPAcceptor::HTTPAcceptor(Monitor* monitor, MessageQueueService* outputMessageQueue)
   : Base("HTTPAcceptor", MessageQueue::getNextQueueId()), 
     _monitor(monitor), _outputMessageQueue(outputMessageQueue), 
     _rep(0), _sslcontext(NULL)
{

   Socket::initializeInterface();
}

HTTPAcceptor::HTTPAcceptor(Monitor* monitor, MessageQueueService* outputMessageQueue,
                           SSLContext * sslcontext)
   :       Base("HTTPAcceptor", MessageQueue::getNextQueueId()), 
	   _monitor(monitor), _outputMessageQueue(outputMessageQueue), _rep(0),
	   _sslcontext(sslcontext)
{
   Socket::initializeInterface();
}

HTTPAcceptor::~HTTPAcceptor()
{
   unbind();
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
      }

     Default:
      // Attn: need unexpected message error!
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

const char* HTTPAcceptor::getQueueName() const
{
   return "HTTPAcceptor";
}

void HTTPAcceptor::bind(Uint32 portNumber)
{
   if (_rep)
      throw BindFailed("HTTPAcceptor already bound");

   _rep = new HTTPAcceptorRep;

   _portNumber = portNumber;

   // bind address
   _bind();

   return;
}

/**
   _bind - creates a new server socket and bind socket to the port address.
*/
void HTTPAcceptor::_bind()
{

   // Create address:

   memset(&_rep->address, 0, sizeof(_rep->address));

#ifdef PEGASUS_ACCEPT_ONLY_LOCAL_CONNECTIONS
   _rep->address.sin_addr.s_addr = INADDR_LOOPBACK;
#else
   _rep->address.sin_addr.s_addr = INADDR_ANY;
#endif

   _rep->address.sin_family = AF_INET;
   _rep->address.sin_port = htons(_portNumber);

   // Create socket:
    
   _rep->socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

   if (_rep->socket < 0)
   {
      delete _rep;
      _rep = 0;
      throw BindFailed("Failed to create socket");
   }

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
      delete _rep;
      _rep = 0;
      throw BindFailed("Failed to set socket option");
   }

   // Bind socket to port:

   if (::bind(_rep->socket, 
	      (struct sockaddr*)(void*)&_rep->address, 
	      sizeof(_rep->address)) < 0)
   {
      Socket::close(_rep->socket);
      delete _rep;
      _rep = 0;
      throw BindFailed("Failed to bind socket to port");
   }

   // Set up listening on the given port:

   int const MAX_CONNECTION_QUEUE_LENGTH = 5;

   if (listen(_rep->socket, MAX_CONNECTION_QUEUE_LENGTH) < 0)
   {
      Socket::close(_rep->socket);
      delete _rep;
      _rep = 0;
      throw BindFailed("Failed to bind socket to port");
   }

   // Register to receive SocketMessages on this socket:

   if (!_monitor->solicitSocketMessages(
	  _rep->socket,
	  SocketMessage::READ | SocketMessage::EXCEPTION,
	  getQueueId()))
   {
      Socket::close(_rep->socket);
      delete _rep;
      _rep = 0;
      throw BindFailed("Failed to solicit socket messaeges");
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
   if (_rep->connections.size() > 0)
   {
      HTTPConnection* connection = _rep->connections[0];	
      return(connection->getRequestCount());
   }
   else
   {
      return(0);
   }
}

void HTTPAcceptor::unbind()
{
   if (_rep)
   {
      Socket::close(_rep->socket);
      delete _rep;
      _rep = 0;
   }
}

void HTTPAcceptor::destroyConnections()
{
   // For each connection created by this object:

   for (Uint32 i = 0, n = _rep->connections.size(); i < n; i++)
   {
      HTTPConnection* connection = _rep->connections[i];	
      Sint32 socket = connection->getSocket();

      // Unsolicit SocketMessages:

      _monitor->unsolicitSocketMessages(socket);

      // Destroy the connection (causing it to close):

      delete connection;
   }

   _rep->connections.clear();
}

void HTTPAcceptor::_acceptConnection()
{
   // This function cannot be called on an invalid socket!

   PEGASUS_ASSERT(_rep != 0);

   if (!_rep)
      return;

   // Accept the connection (populate the address):

   sockaddr_in address;

#if defined(PEGASUS_PLATFORM_ZOS_ZSERIES_IBM) || defined(PEGASUS_PLATFORM_AIX_RS_IBMCXX)
   size_t n = sizeof(address);
#else
   int n = sizeof(address);
#endif

#if defined(PEGASUS_PLATFORM_LINUX_IX86_GNU)    
   Sint32 socket = accept(
      _rep->socket, (struct sockaddr*)&address, (socklen_t *)&n);
#else
   Sint32 socket = accept(_rep->socket, (struct sockaddr*)&address, &n);
#endif

   if (socket < 0)
   {
      if (getenv("PEGASUS_TRACE"))
	 cerr <<"HTTPAcceptor: accept() failed" << endl;

      return;
   }

   // Create a new conection and add it to the connection list:

   MP_Socket * mp_socket = new MP_Socket(socket, _sslcontext);
   if (mp_socket->accept() < 0) {
      if (getenv("PEGASUS_TRACE"))
	 cerr <<"HTTPAcceptor: SSL_accept() failed" << endl;

      return;
   }

   HTTPConnection* connection = new HTTPConnection(
      _monitor, mp_socket, this, static_cast<MessageQueueService *>(_outputMessageQueue));

   // Solicit events on this new connection's socket:

   if (!_monitor->solicitSocketMessages(
	  socket,
	  SocketMessage::READ | SocketMessage::EXCEPTION,
	  connection->getQueueId()))
   {
      delete connection;
      Socket::close(socket);
   }

   // Save the socket for cleanup later:

   _rep->connections.append(connection);
}

PEGASUS_NAMESPACE_END
