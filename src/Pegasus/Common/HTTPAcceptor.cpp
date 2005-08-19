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
// Modified By:
//          Jenny Yu, Hewlett-Packard Company (jenny_yu@hp.com)
//          Nag Boranna, Hewlett-Packard Company (nagaraja_boranna@hp.com)
//          Dave Rosckes (rosckes@us.ibm.com)
//          Denise Eckstein (denise.eckstein@hp.com)
//          Alagaraja Ramasubramanian (alags_raj@in.ibm.com) for Bug#1090
//          Amit Arora, IBM (amita@in.ibm.com) for Bug#2541
//          Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//          Sean Keenan, Hewlett-Packard Company (sean.keenan@hp.com)
//          Josephine Eskaline Joyce, IBM (jojustin@in.ibm.com) for Bug#2065
//          David Dillard, VERITAS Software Corp.
//              (david.dillard@veritas.com)
//          John Alex, IBM (johnalex@us.ibm.com) for Bug#3312
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
# ifndef PEGASUS_DISABLE_LOCAL_DOMAIN_SOCKET
# include <unistd.h>
#  include <sys/un.h>
# endif
#endif

#include "Socket.h"
#include "TLS.h"
#include "HTTPAcceptor.h"
#include "HTTPConnection.h"
#include "Tracer.h"
#include <Pegasus/Common/MessageLoader.h> //l10n

#ifdef PEGASUS_PLATFORM_OS400_ISERIES_IBM
#include "OS400ConvertChar.h"
#endif

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN


//
// Determine the correct type to use for the length passed to getsockname().
// The default is to use the 'socklen_t'.
//

#if defined(PEGASUS_OS_TYPE_WINDOWS) || defined(PEGASUS_OS_OS400)
#define PEGASUS_SOCKLEN_T   int
#endif

#if defined(PEGASUS_OS_HPUX) && !defined(_XOPEN_SOURCE_EXTENDED)
#define PEGASUS_SOCKLEN_T   int
#endif


#if defined(PEGASUS_OS_VMS) || defined(PEGASUS_OS_ZOS)
#define PEGASUS_SOCKLEN_T   unsigned
#endif

#ifndef PEGASUS_SOCKLEN_T
#define PEGASUS_SOCKLEN_T   socklen_t
#endif

static int MAX_CONNECTION_QUEUE_LENGTH = -1;


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
#ifndef PEGASUS_DISABLE_LOCAL_DOMAIN_SOCKET
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
    ~HTTPAcceptorRep()
    {
	delete address;
    }
    struct sockaddr* address;

#if defined(PEGASUS_PLATFORM_ZOS_ZSERIES_IBM) || defined(PEGASUS_OS_VMS)
   size_t address_size;
#elif defined(PEGASUS_PLATFORM_AIX_RS_IBMCXX) || defined(PEGASUS_PLATFORM_LINUX_GENERIC_GNU) || (defined(PEGASUS_PLATFORM_SOLARIS_SPARC_CC) && !defined(SUNOS_5_6))
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
                           SSLContext * sslcontext,
                           Boolean exportConnection,
                           ReadWriteSem* sslContextObjectLock)
   : Base(PEGASUS_QUEUENAME_HTTPACCEPTOR),  // ATTN: Need unique names?
     _monitor(monitor),
     _outputMessageQueue(outputMessageQueue),
     _rep(0),
     _entry_index(-1),
     _localConnection(localConnection),
     _portNumber(portNumber),
     _sslcontext(sslcontext),
     _exportConnection(exportConnection),
     _sslContextObjectLock(sslContextObjectLock)
{
   Socket::initializeInterface();

   /*
        Platforms interpret the value of MAX_CONNECTION_QUEUE_LENGTH differently.  Some platforms interpret
        the value literally, while others multiply a fudge factor. When the server is under
        stress from multiple clients with multiple requests, toggling this number may prevent clients from
        being dropped.  Instead of hard coding the value, we allow an environment variable to be set which
        specifies a number greater than the maximum concurrent client connections possible.  If this environment
        var is not specified, then MAX_CONNECTION_QUEUE_LENGTH = 15.
   */

//To engage runtime backlog queue length: uncomment the following block AND comment out the line MAX_CONNECTION_QUEUE_LENGTH = 15

/*
   if(MAX_CONNECTION_QUEUE_LENGTH == -1){
#ifdef PEGASUS_PLATFORM_OS400_ISERIES_IBM
#pragma convert(37)
    const char* env = getenv("PEGASUS_MAX_BACKLOG_CONNECTION_QUEUE");
    EtoA(env);
#pragma convert(0)
#else
    const char* env = getenv("PEGASUS_MAX_BACKLOG_CONNECTION_QUEUE");
#endif
    if(!env){
        MAX_CONNECTION_QUEUE_LENGTH = 15;
    }else{
        char *end = NULL;
        MAX_CONNECTION_QUEUE_LENGTH = strtol(env, &end, 10);
        if(*end)
            MAX_CONNECTION_QUEUE_LENGTH = 15;
        cout << " MAX_CONNECTION_QUEUE_LENGTH = " << MAX_CONNECTION_QUEUE_LENGTH << endl;
    }
   }
*/
   MAX_CONNECTION_QUEUE_LENGTH = 15;

}

HTTPAcceptor::~HTTPAcceptor()
{
   destroyConnections();
   unbind();
   // ATTN: Is this correct in a multi-HTTPAcceptor server?
   Socket::uninitializeInterface();
}

void HTTPAcceptor::handleEnqueue(Message *message)
{
   if (! message)
      return;

   PEGASUS_ASSERT(_rep != 0);
   switch (message->getType())
   {
      case SOCKET_MESSAGE:
      {
     SocketMessage* socketMessage = (SocketMessage*)message;

     // If this is a connection request:

     if (socketMessage->socket == _rep->socket &&
         socketMessage->events & SocketMessage::READ)
     {
        _acceptConnection();
     }
     else
     {
        // ATTN! this can't happen!
            Tracer::trace(TRC_DISCARDED_DATA, Tracer::LEVEL2,
              "HTTPAcceptor::handleEnqueue: Invalid SOCKET_MESSAGE received.");
     }

     break;
      }

      case CLOSE_CONNECTION_MESSAGE:
      {
     CloseConnectionMessage* closeConnectionMessage
        = (CloseConnectionMessage*)message;

     AutoMutex autoMut(_rep->_connection_mut);

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

     break;
      }

      default:
      // ATTN: need unexpected message error!
      Tracer::trace(TRC_DISCARDED_DATA, Tracer::LEVEL2,
           "HTTPAcceptor::handleEnqueue: Invalid MESSAGE received.");
      break;
   };

   delete message;
}


void HTTPAcceptor::handleEnqueue()
{
   Message* message = dequeue();

   if (!message)
   {
      Tracer::trace(TRC_DISCARDED_DATA, Tracer::LEVEL2,
           "HTTPAcceptor::handleEnqueue(): No message on queue.");
      return;
   }

   handleEnqueue(message);

}

void HTTPAcceptor::bind()
{
   if (_rep){
    //l10n
      //throw BindFailedException("HTTPAcceptor already bound");

      MessageLoaderParms parms("Common.HTTPAcceptor.ALREADY_BOUND",
                   "HTTPAcceptor already bound");

      Tracer::trace(TRC_DISCARDED_DATA, Tracer::LEVEL2,
           "HTTPAcceptor::bind: HTTPAcceptor already bound.");
      throw BindFailedException(parms);
   }

   _rep = new HTTPAcceptorRep(_localConnection);

   // bind address
   _bind();

   return;
}

/**
   _bind - creates a new server socket and bind socket to the port address.
   If PEGASUS_DISABLE_LOCAL_DOMAIN_SOCKET is not defined, the port number is ignored and
   a domain socket is bound.
*/
void HTTPAcceptor::_bind()
{

   PEGASUS_ASSERT(_rep != 0);
   // Create address:

   memset(_rep->address, 0, sizeof(*_rep->address));

   if (_localConnection)
   {
#ifndef PEGASUS_DISABLE_LOCAL_DOMAIN_SOCKET
       reinterpret_cast<struct sockaddr_un*>(_rep->address)->sun_family =
           AF_UNIX;
       strcpy(reinterpret_cast<struct sockaddr_un*>(_rep->address)->sun_path,
              PEGASUS_LOCAL_DOMAIN_SOCKET_PATH);
#ifdef PEGASUS_PLATFORM_OS400_ISERIES_IBM
       AtoE(reinterpret_cast<struct sockaddr_un*>(_rep->address)->sun_path);
#endif
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
      //l10n
      //throw BindFailedException("Failed to create socket");
      MessageLoaderParms parms("Common.HTTPAcceptor.FAILED_CREATE_SOCKET",
                   "Failed to create socket");
      Tracer::trace(TRC_DISCARDED_DATA, Tracer::LEVEL2,
           "HTTPAcceptor::_bind _rep->socket < 0");
      throw BindFailedException(parms);
   }


// set the close-on-exec bit for this file handle.
// any unix that forks needs this bit set.
#if !defined PEGASUS_PLATFORM_WIN32_IX86_MSVC && !defined(PEGASUS_OS_VMS)
   int sock_flags;
 if( (sock_flags = fcntl(_rep->socket, F_GETFD, 0)) < 0)
   {
       PEG_TRACE_STRING(TRC_DISCARDED_DATA, Tracer::LEVEL2,
                  "HTTPAcceptor::_bind: fcntl(F_GETFD) failed");
   }
   else
   {
      sock_flags |= FD_CLOEXEC;
      if (fcntl(_rep->socket, F_SETFD, sock_flags) < 0)
      {
       PEG_TRACE_STRING(TRC_DISCARDED_DATA, Tracer::LEVEL2,
                  "HTTPAcceptor::_bind: fcntl(F_SETFD) failed");
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
      //l10n
      //throw BindFailedException("Failed to set socket option");
      MessageLoaderParms parms("Common.HTTPAcceptor.FAILED_SET_SOCKET_OPTION",
                   "Failed to set socket option");
      PEG_TRACE_STRING(TRC_DISCARDED_DATA, Tracer::LEVEL2,
                   "HTTPAcceptor::_bind: Failed to set socket option.");
      throw BindFailedException(parms);
   }


   //
   // Bind socket to port:
   //
   if (::bind(_rep->socket, _rep->address, _rep->address_size) < 0)
   {
      Socket::close(_rep->socket);
      delete _rep;
      _rep = 0;
      //l10n
      //throw BindFailedException("Failed to bind socket");
      MessageLoaderParms parms("Common.HTTPAcceptor.FAILED_BIND_SOCKET",
                   "Failed to bind socket");
      PEG_TRACE_STRING(TRC_DISCARDED_DATA, Tracer::LEVEL2,
                        "HTTPAcceptor::_bind: Failed to bind socket.");
      throw BindFailedException(parms);
   }


   //
   // Get the actual port value used if the caller specified a port value of 0.
   //
   if ( _portNumber == 0 )
   {
      sockaddr_in buf;
      PEGASUS_SOCKLEN_T bufSize = sizeof(buf);
      if ( getsockname(_rep->socket, reinterpret_cast<sockaddr *>(&buf), &bufSize) == 0 )
      {
          _portNumber = ntohs(buf.sin_port);
      }
   }


   //
   //  Change permissions on Linux local domain socket to allow writes by others.
   //
#if !defined(PEGASUS_DISABLE_LOCAL_DOMAIN_SOCKET) && defined(PEGASUS_PLATFORM_LINUX_GENERIC_GNU)
   if (_localConnection)
   {
     if (::chmod( PEGASUS_LOCAL_DOMAIN_SOCKET_PATH,
                  S_IRUSR | S_IWUSR | S_IXUSR |
                  S_IRGRP | S_IWGRP | S_IXGRP |
                  S_IROTH | S_IWOTH | S_IXOTH ) < 0 )
     {
       Socket::close(_rep->socket);
       delete _rep;
       _rep = 0;
       //l10n
       //throw BindFailedException("Failed to bind socket");
       MessageLoaderParms parms("Common.HTTPAcceptor.FAILED_BIND_SOCKET",
                   "Failed to bind socket");
       PEG_TRACE_STRING(TRC_DISCARDED_DATA, Tracer::LEVEL2,
              "HTTPAcceptor::_bind: Failed to set domain socket permissions.");
       throw BindFailedException(parms);
     }
   }
#endif

   // Set up listening on the given socket:

   //int const MAX_CONNECTION_QUEUE_LENGTH = 15;

   if (listen(_rep->socket, MAX_CONNECTION_QUEUE_LENGTH) < 0)
   {
      Socket::close(_rep->socket);
      delete _rep;
      _rep = 0;
      //l10n
      //throw BindFailedException("Failed to bind socket");
      MessageLoaderParms parms("Common.HTTPAcceptor.FAILED_BIND_SOCKET",
                   "Failed to bind socket");
      PEG_TRACE_STRING(TRC_DISCARDED_DATA, Tracer::LEVEL2,
                  "HTTPAcceptor::_bind: Failed to bind socket(1).");
      throw BindFailedException(parms);
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
      //l10n
      //throw BindFailedException("Failed to solicit socket messaeges");
      MessageLoaderParms parms("Common.HTTPAcceptor.FAILED_SOLICIT_SOCKET_MESSAGES",
                   "Failed to solicit socket messaeges");
      PEG_TRACE_STRING(TRC_DISCARDED_DATA, Tracer::LEVEL2,
                  "HTTPAcceptor::_bind: Failed to solicit socket messages(2).");
      throw BindFailedException(parms);
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

      // ATTN - comment out - see CIMServer::stopClientConnection()
      //_monitor->unsolicitSocketMessages(_rep->socket);

      // close the socket
      Socket::close(_rep->socket);
      // Unlink Local Domain Socket Bug# 3312
      if (_localConnection)
      {
#ifndef PEGASUS_DISABLE_LOCAL_DOMAIN_SOCKET
          PEG_TRACE_STRING(TRC_HTTP, Tracer::LEVEL2,
                        "HTTPAcceptor::closeConnectionSocket Unlinking local connection." );
         ::unlink(
             reinterpret_cast<struct sockaddr_un*>(_rep->address)->sun_path);
#else
         PEGASUS_ASSERT(false);
#endif
      }

   }
   else
   {
      PEG_TRACE_STRING(TRC_DISCARDED_DATA, Tracer::LEVEL2,
                        "HTTPAcceptor::closeConnectionSocket failure _rep is null." );
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
   else
   {
      PEG_TRACE_STRING(TRC_DISCARDED_DATA, Tracer::LEVEL2,
                        "HTTPAcceptor::reopenConnectionSocket failure _rep is null." );
   }
}

/**
   getOutstandingRequestCount - returns the number of outstanding requests.
*/
Uint32 HTTPAcceptor::getOutstandingRequestCount() const
{
   Uint32 count = 0;
   if (_rep)
   {
      AutoMutex autoMut(_rep->_connection_mut);
      if (_rep->connections.size() > 0)
      {
         HTTPConnection* connection = _rep->connections[0];
         count = connection->getRequestCount();
      }
   }
   return count;
}


/**
    getPortNumber - returns the port number used for the connection
*/
Uint32 HTTPAcceptor::getPortNumber() const
{
    return _portNumber;
}

void HTTPAcceptor::unbind()
{
   if (_rep)
   {
      _portNumber = 0;
      Socket::close(_rep->socket);

      if (_localConnection)
      {
#ifndef PEGASUS_DISABLE_LOCAL_DOMAIN_SOCKET
         ::unlink(
             reinterpret_cast<struct sockaddr_un*>(_rep->address)->sun_path);
#else
         PEGASUS_ASSERT(false);
#endif
      }

      delete _rep;
      _rep = 0;
   }
   else
   {
      PEG_TRACE_STRING(TRC_DISCARDED_DATA, Tracer::LEVEL2,
              "HTTPAcceptor::unbind failure _rep is null." );
   }
}

void HTTPAcceptor::destroyConnections()
{
   if (_rep)
   {
     // For each connection created by this object:

     AutoMutex autoMut(_rep->_connection_mut);
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
   }
}

void HTTPAcceptor::_acceptConnection()
{
   // This function cannot be called on an invalid socket!

   PEGASUS_ASSERT(_rep != 0);

   if (!_rep)
      return;

   // Accept the connection (populate the address):

   struct sockaddr* accept_address;
#if defined(PEGASUS_PLATFORM_ZOS_ZSERIES_IBM) || defined(PEGASUS_OS_VMS)
   size_t address_size;
#elif defined(PEGASUS_PLATFORM_AIX_RS_IBMCXX) || defined(PEGASUS_PLATFORM_LINUX_GENERIC_GNU) || (defined(PEGASUS_OS_SOLARIS) && !defined(SUNOS_5_6))
   socklen_t address_size;
#else
   int address_size;
#endif

   if (_localConnection)
   {
#ifndef PEGASUS_DISABLE_LOCAL_DOMAIN_SOCKET
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

       PEG_TRACE_STRING(TRC_DISCARDED_DATA, Tracer::LEVEL2,
                        "HTTPAcceptor: accept() failed");
      return;
   }

// set the close on exec flag
#if !defined PEGASUS_PLATFORM_WIN32_IX86_MSVC && !defined(PEGASUS_OS_VMS)
   int sock_flags;
 if( (sock_flags = fcntl(socket, F_GETFD, 0)) < 0)
   {
       PEG_TRACE_STRING(TRC_DISCARDED_DATA, Tracer::LEVEL2,
                        "HTTPAcceptor: fcntl(F_GETFD) failed");
   }
   else
   {
      sock_flags |= FD_CLOEXEC;
      if (fcntl(socket, F_SETFD, sock_flags) < 0)
      {
       PEG_TRACE_STRING(TRC_DISCARDED_DATA, Tracer::LEVEL2,
                        "HTTPAcceptor: fcntl(F_SETFD) failed");
      }
   }
#endif


   Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
           "HTTPAcceptor - accept() success.  Socket: $1"
           ,socket);

   AutoPtr<MP_Socket> mp_socket(new MP_Socket(
       socket, _sslcontext, _sslContextObjectLock, _exportConnection));

   // Perform the SSL handshake, if applicable.  Make the socket non-blocking
   // for this operation so we can send it back to the Monitor's select() loop
   // if it takes a while.

   mp_socket->disableBlocking();
   Sint32 socketAcceptStatus = mp_socket->accept();
   mp_socket->enableBlocking();

   if (socketAcceptStatus < 0)
   {
       PEG_TRACE_STRING(TRC_DISCARDED_DATA, Tracer::LEVEL2,
                        "HTTPAcceptor: SSL_accept() failed");
       mp_socket->close();
       return;
   }

   // Create a new connection and add it to the connection list:

   HTTPConnection* connection = new HTTPConnection(_monitor, mp_socket,
       this, static_cast<MessageQueue *>(_outputMessageQueue), _exportConnection);

   if (socketAcceptStatus == 0)
   {
       PEG_TRACE_STRING(TRC_HTTP, Tracer::LEVEL2,
           "HTTPAcceptor: SSL_accept() pending");
       connection->_acceptPending = true;
   }

   // Solicit events on this new connection's socket:
   int index;

   if (-1 ==  (index = _monitor->solicitSocketMessages(
      connection->getSocket(),
      SocketMessage::READ | SocketMessage::EXCEPTION,
      connection->getQueueId(), Monitor::CONNECTION)) )
   {
      // ATTN-DE-P2-2003100503::TODO::Need to enhance code to return
      // an error message to Client application.
      Tracer::trace(TRC_DISCARDED_DATA, Tracer::LEVEL2,
          "HTTPAcceptor::_acceptConnection: Attempt to allocate entry in _entries table failed.");
      delete connection;
      Socket::close(socket);
      return;
   }

   // Save the socket for cleanup later:
   connection->_entry_index = index;
   AutoMutex autoMut(_rep->_connection_mut);
   _rep->connections.append(connection);
}

PEGASUS_NAMESPACE_END
