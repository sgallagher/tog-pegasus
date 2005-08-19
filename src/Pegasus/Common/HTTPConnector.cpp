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
// Modified By: Sushma Fernandes, Hewlett-Packard Company
//                (sushma_fernandes@hp.com)
// Modified By: Dan Gorey, IBM (djgorey@us.ibm.com)
// Modified By: Amit Arora (amita@in.ibm.com) for Bug#1170
//              Dave Sudlik, IBM (dsudlik@us.ibm.com) for Bug#1462
//              Amit Arora, IBM (amita@in.ibm.com) for Bug#2541
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include "Config.h"
#include <iostream>
#include "Constants.h"
#include "Socket.h"
#include <Pegasus/Common/MessageLoader.h> //l10n

#ifdef PEGASUS_OS_TYPE_WINDOWS
# include <windows.h>
#else
# include <cctype>
# include <cstdlib>
# include <errno.h>
# include <fcntl.h>
# include <netdb.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <sys/socket.h>
# ifndef PEGASUS_DISABLE_LOCAL_DOMAIN_SOCKET
# include <unistd.h>
#  include <sys/un.h>
# endif
#endif

#include "Socket.h"
#include "TLS.h"
#include "HTTPConnector.h"
#include "HTTPConnection.h"

#ifdef PEGASUS_OS_OS400
#  include "OS400ConvertChar.h"
#endif

#ifdef PEGASUS_OS_ZOS
#  include <resolv.h>  // MAXHOSTNAMELEN
#endif

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

class bsd_socket_rep;

////////////////////////////////////////////////////////////////////////////////
//
// Local routines:
//
////////////////////////////////////////////////////////////////////////////////

static Boolean _MakeAddress(
   const char* hostname, 
   int port, 
   sockaddr_in& address)
{
   if (!hostname)
      return false;

#ifdef PEGASUS_OS_OS400
    char ebcdicHost[256];
    if (strlen(hostname) < 256)
    strcpy(ebcdicHost, hostname);
    else
    return false;
    AtoE(ebcdicHost);
#endif
    
////////////////////////////////////////////////////////////////////////////////
// This code used to check if the first character of "hostname" was alphabetic
// to indicate hostname instead of IP address. But RFC 1123, section 2.1, relaxed
// this requirement to alphabetic character *or* digit. So bug 1462 changed the
// flow here to call inet_addr first to check for a valid IP address in dotted
// decimal notation. If it's not a valid IP address, then try to validate
// it as a hostname.
// RFC 1123 states: The host SHOULD check the string syntactically for a 
// dotted-decimal number before looking it up in the Domain Name System. 
// Hence the call to inet_addr() first.
////////////////////////////////////////////////////////////////////////////////

#ifdef PEGASUS_OS_OS400
   unsigned long tmp_addr = inet_addr(ebcdicHost);
#else
    #ifdef PEGASUS_OS_ZOS
   unsigned long tmp_addr = inet_addr_ebcdic((char *)hostname);
    #else
   unsigned long tmp_addr = inet_addr((char *)hostname);
    #endif
#endif

   struct hostent *entry;

// Note: 0xFFFFFFFF is actually a valid IP address (255.255.255.255).
//       A better solution would be to use inet_aton() or equivalent, as
//       inet_addr() is now considered "obsolete".

   if (tmp_addr == 0xFFFFFFFF)  // if hostname is not an IP address
   {
#ifdef PEGASUS_PLATFORM_SOLARIS_SPARC_CC
#define HOSTENT_BUFF_SIZE        8192
      char      buf[HOSTENT_BUFF_SIZE];
      int       h_errorp;
      struct    hostent hp;

      entry = gethostbyname_r((char *)hostname, &hp, buf,
                                HOSTENT_BUFF_SIZE, &h_errorp);
#elif defined(PEGASUS_OS_OS400)
      entry = gethostbyname(ebcdicHost);
#elif defined(PEGASUS_OS_ZOS)
      char hostName[ MAXHOSTNAMELEN + 1 ];
      if (String::equalNoCase("localhost",String(hostname)))
      {
          gethostname( hostName, sizeof( hostName ) );
          entry = gethostbyname(hostName);
      } else {
          entry = gethostbyname((char *)hostname);
      }
#else
      entry = gethostbyname((char *)hostname);
#endif
      if(!entry)
      {
    return false;
      }

      memset(&address, 0, sizeof(address));
      memcpy(&address.sin_addr, entry->h_addr, entry->h_length);
      address.sin_family = entry->h_addrtype;
      address.sin_port = htons(port);
   }     
   else    // else hostname *is* a dotted-decimal IP address
   {
      memset(&address, 0, sizeof(address));
      address.sin_family = AF_INET;
      address.sin_addr.s_addr = tmp_addr;
      address.sin_port = htons(port);
   }

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
     _monitor(monitor), _entry_index(-1)
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
   const String& host, 
   const Uint32 portNumber,
   SSLContext * sslContext,
   MessageQueue* outputMessageQueue)
{
   Sint32 socket;

#ifndef PEGASUS_DISABLE_LOCAL_DOMAIN_SOCKET
   if (host == String::EMPTY)
   {
      // Set up the domain socket for a local connection

      sockaddr_un address;
      address.sun_family = AF_UNIX;
      strcpy(address.sun_path, PEGASUS_LOCAL_DOMAIN_SOCKET_PATH);
#ifdef PEGASUS_PLATFORM_OS400_ISERIES_IBM
      AtoE(address.sun_path);
#endif

      socket = ::socket(AF_UNIX, SOCK_STREAM, 0);
      if (socket < 0)
         throw CannotCreateSocketException();

      // Connect the socket to the address:

      if (::connect(socket,
                    reinterpret_cast<sockaddr*>(&address),
                    sizeof(address)) < 0)
      {
     
        //l10n
         //throw CannotConnectException("Cannot connect to local CIM server. Connection failed.");
         MessageLoaderParms parms("Common.HTTPConnector.CONNECTION_FAILED_LOCAL_CIM_SERVER",
                                  "Cannot connect to local CIM server. Connection failed.");
         Socket::close(socket);
         throw CannotConnectException(parms);
      }
   }
   else
   {
#endif

   // Make the internet address:

   sockaddr_in address;

   if (!_MakeAddress((const char*)host.getCString(), portNumber, address))
   {
      char portStr [32];
      sprintf (portStr, "%u", portNumber);
      throw InvalidLocatorException(host + ":" + portStr);
   }


   // Create the socket:

   socket = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

   if (socket < 0)
      throw CannotCreateSocketException();

   // Conect the socket to the address:

   if (::connect(socket,
                 reinterpret_cast<sockaddr*>(&address),
                 sizeof(address)) < 0)
   {
      char portStr [32];
      sprintf (portStr, "%u", portNumber);
      //l10n
      //throw CannotConnectException("Cannot connect to " + host + ":" + portStr +". Connection failed.");
      MessageLoaderParms parms("Common.HTTPConnector.CONNECTION_FAILED_TO",
                               "Cannot connect to $0:$1. Connection failed.",
                               host,
                               portStr);
      Socket::close(socket);
      throw CannotConnectException(parms);
   }

#ifndef PEGASUS_DISABLE_LOCAL_DOMAIN_SOCKET
   }
#endif

   // Create HTTPConnection object:

   AutoPtr<MP_Socket> mp_socket(new MP_Socket(socket, sslContext, 0, false));
   if (mp_socket->connect() < 0) {
      char portStr [32];
      sprintf (portStr, "%u", portNumber);
      //l10n
      //throw CannotConnectException("Cannot connect to " + host + ":" + portStr +". Connection failed.");
      MessageLoaderParms parms("Common.HTTPConnector.CONNECTION_FAILED_TO",
                               "Cannot connect to $0:$1. Connection failed.",
                               host,
                               portStr);
      mp_socket->close();
      throw CannotConnectException(parms);
   }
    
   HTTPConnection* connection = new HTTPConnection(_monitor, mp_socket,
        this, static_cast<MessageQueueService *>(outputMessageQueue), false);

   // Solicit events on this new connection's socket:

   if (-1 == (_entry_index = _monitor->solicitSocketMessages(
      connection->getSocket(),
      SocketMessage::READ | SocketMessage::EXCEPTION,
      connection->getQueueId(), Monitor::CONNECTOR)))
   {
      (connection->getMPSocket()).close();
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

    Uint32 index = PEG_NOT_FOUND;
    for (Uint32 i = 0, n = _rep->connections.size(); i < n; i++)
    {
        if (currentConnection == _rep->connections[i])
        {
            index = i;
            break;
        }
     }

    PEGASUS_ASSERT(index != PEG_NOT_FOUND);

    Sint32 socket = currentConnection->getSocket();
    _monitor->unsolicitSocketMessages(socket);
    _rep->connections.remove(index);
    delete currentConnection;
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
