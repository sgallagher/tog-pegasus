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
//
//%/////////////////////////////////////////////////////////////////////////////

#include "Config.h"
#include <iostream>
#include "Constants.h"
#include "Socket.h"

#ifdef PEGASUS_OS_TYPE_WINDOWS
# include <windows.h>
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
# ifdef PEGASUS_LOCAL_DOMAIN_SOCKET
#  include <sys/un.h>
# endif
#endif

#include "Socket.h"
#include "TLS.h"
#include "HTTPConnector.h"
#include "HTTPConnection.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
//
// Local routines:
//
////////////////////////////////////////////////////////////////////////////////

static Boolean _ParseLocator(
   const String& locator, 
   char*& hostname,
   int& port)
{
   // Extract the hostname:port expression (e.g., www.book.com:8080):

   hostname = locator.allocateCString();
   char* p = strchr(hostname, ':');

   if (!p)
   {
      delete [] hostname;
      return false;
   }

   *p++ = '\0';

   char* end = 0;
   port = strtol(p, &end, 10);

   if (!end || *end != '\0')
   {
      delete [] hostname;
      return false;
   }

   return true;
}

static Boolean _MakeAddress(
   const char* hostname, 
   int port, 
   sockaddr_in& address)
{
   if (!hostname)
      return false;
	
   struct hostent *entry;
	
   if (isalpha(hostname[0]))
      entry = gethostbyname(hostname);
   else
   {
      unsigned long tmp = inet_addr((char *)hostname);
      entry = gethostbyaddr((char *)&tmp, sizeof(tmp), AF_INET);
   }

   if (!entry)
      return false;

   memset(&address, 0, sizeof(address));
   memcpy(&address.sin_addr, entry->h_addr, entry->h_length);
   address.sin_family = entry->h_addrtype;
   address.sin_port = htons(port);

   return true;
}

////////////////////////////////////////////////////////////////////////////////
//
// HTTPConnectorRep
//
////////////////////////////////////////////////////////////////////////////////

struct HTTPConnectorRep
{
      Array<HTTPConnection*> connections;
};

////////////////////////////////////////////////////////////////////////////////
//
// HTTPConnector
//
////////////////////////////////////////////////////////////////////////////////

HTTPConnector::HTTPConnector(Monitor* monitor)
   : Base(PEGASUS_QUEUENAME_HTTPCONNECTOR),
     _monitor(monitor)
{
   _rep = new HTTPConnectorRep;
   Socket::initializeInterface();
}

HTTPConnector::~HTTPConnector()
{
   delete _rep;
   Socket::uninitializeInterface();
}

void HTTPConnector::handleEnqueue(Message *message)
{

   if (!message)
      return;

   switch (message->getType())
   {
      // It might be useful to catch socket messages later to implement
      // asynchronous establishment of connections.

      case SOCKET_MESSAGE:
	 break;

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
	       //printf("closing socket\n");
	       _monitor->unsolicitSocketMessages(socket);
	       _rep->connections.remove(i);
	       
	       delete connection;
	       break;
	    }
	 }
      }

      default:
	 // ATTN: need unexpected message error!
	 break;
   };

   delete message;
}


void HTTPConnector::handleEnqueue()
{

   Message* message = dequeue();

   if (!message)
      return;

   handleEnqueue(message);
}

HTTPConnection* HTTPConnector::connect(
   const String& locator, 
   SSLContext * sslContext,
   MessageQueue* outputMessageQueue)
{
   Sint32 socket;

#ifdef PEGASUS_LOCAL_DOMAIN_SOCKET
   if (locator == String::EMPTY)
   {
      // Set up the domain socket for a local connection

      sockaddr_un address;
      address.sun_family = AF_UNIX;
      strcpy(address.sun_path, "/var/opt/wbem/cimxml.socket");

      socket = ::socket(AF_UNIX, SOCK_STREAM, 0);
      if (socket < 0)
         throw CannotCreateSocket();

      // Connect the socket to the address:

      if (::connect(socket,
                    reinterpret_cast<sockaddr*>(&address),
                    sizeof(address)) < 0)
      {
         throw CannotConnect("local CIM server");
      }
   }
   else
   {
#endif

   // Parse the locator (get hostname and port):

   char* hostname;
   int port;

   if (!_ParseLocator(locator, hostname, port))
   {
      throw InvalidLocator(locator);
   }

   // Make the internet address:

   sockaddr_in address;

   if (!_MakeAddress(hostname, port, address))
   {
      delete [] hostname;
      throw InvalidLocator(locator);
   }

   delete [] hostname;

   // Create the socket:

   socket = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

   if (socket < 0)
      throw CannotCreateSocket();

   // Conect the socket to the address:

   if (::connect(socket,
                 reinterpret_cast<sockaddr*>(&address),
                 sizeof(address)) < 0)
   {
      throw CannotConnect(locator);
   }

#ifdef PEGASUS_LOCAL_DOMAIN_SOCKET
   }
#endif

   // Create HTTPConnection object:

   MP_Socket * mp_socket = new MP_Socket(socket, sslContext);
   if (mp_socket->connect() < 0) {
      throw CannotConnect(locator);
   }
    
   HTTPConnection* connection = new HTTPConnection(_monitor, mp_socket,
						   this, static_cast<MessageQueueService *>(outputMessageQueue));

   // Solicit events on this new connection's socket:

   if (!_monitor->solicitSocketMessages(
	  socket,
	  SocketMessage::READ | SocketMessage::EXCEPTION,
	  connection->getQueueId(), Monitor::CONNECTOR))
   {
      delete connection;
      Socket::close(socket);
      throw UnexpectedFailure();
   }

   // Save the socket for cleanup later:

   _rep->connections.append(connection);

   return connection;
}

void HTTPConnector::destroyConnections()
{
   // For each connection created by this object:

   for (Uint32 i = 0, n = _rep->connections.size(); i < n; i++)
   {
      _deleteConnection(_rep->connections[i]);
   }

   _rep->connections.clear();
}

void HTTPConnector::disconnect(HTTPConnection* currentConnection)
{
    //
    // find and delete the specified connection
    //
    for (Uint32 i = 0, n = _rep->connections.size(); i < n; i++)
    {
        if (currentConnection == _rep->connections[i])
        {
            _deleteConnection(_rep->connections[i]);

            return;
        }
    }
}

void HTTPConnector::_deleteConnection(HTTPConnection* httpConnection)
{
    Sint32 socket = httpConnection->getSocket();

    // Unsolicit SocketMessages:

    _monitor->unsolicitSocketMessages(socket);

    // Destroy the connection (causing it to close):

    delete httpConnection;
}

PEGASUS_NAMESPACE_END
