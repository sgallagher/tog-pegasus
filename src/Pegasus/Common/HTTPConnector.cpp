//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
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
//              David Dillard, Symantec Corp (david_dillard@symantec.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include "Config.h"
#include <iostream>
#include "Constants.h"
#include "Socket.h"
#include "NamedPipe.h"
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
   unsigned long tmp_addr = inet_addr((char *)hostname);
#endif

   struct hostent* hostEntry;

// Note: 0xFFFFFFFF is actually a valid IP address (255.255.255.255).
//       A better solution would be to use inet_aton() or equivalent, as
//       inet_addr() is now considered "obsolete".

   if (tmp_addr == 0xFFFFFFFF)  // if hostname is not an IP address
   {
#if defined(PEGASUS_OS_LINUX)
      char hostEntryBuffer[8192];
      struct hostent hostEntryStruct;
      int hostEntryErrno;

      gethostbyname_r(
          hostname,
          &hostEntryStruct,
          hostEntryBuffer,
          sizeof(hostEntryBuffer),
          &hostEntry,
          &hostEntryErrno);
#elif defined(PEGASUS_OS_SOLARIS)
      char hostEntryBuffer[8192];
      struct hostent hostEntryStruct;
      int hostEntryErrno;

      hostEntry = gethostbyname_r(
          (char *)hostname,
          &hostEntryStruct,
          hostEntryBuffer,
          sizeof(hostEntryBuffer),
          &hostEntryErrno);
#elif defined(PEGASUS_OS_OS400)
      hostEntry = gethostbyname(ebcdicHost);
#elif defined(PEGASUS_OS_ZOS)
      if (String::equalNoCase("localhost",String(hostname)))
      {
          char hostName[PEGASUS_MAXHOSTNAMELEN + 1];
          gethostname( hostName, sizeof( hostName ) );
          hostName[sizeof(hostName)-1] = 0;
          hostEntry = gethostbyname(hostName);
      } else {
          hostEntry = gethostbyname((char *)hostname);
      }
#else
      hostEntry = gethostbyname((char *)hostname);
#endif
      if (!hostEntry)
      {
          return false;
      }

      memset(&address, 0, sizeof(address));
      memcpy(&address.sin_addr, hostEntry->h_addr, hostEntry->h_length);
      address.sin_family = hostEntry->h_addrtype;
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
        if(!connection->isNamedPipeConnection())
        {

            PEGASUS_SOCKET socket = connection->getSocket();

            if (socket == closeConnectionMessage->socket)
            {
                _monitor->unsolicitSocketMessages(socket);
                _rep->connections.remove(i);
                delete connection;
                break;
            }
        }
     
        else
        {
             NamedPipe namedPipe = connection->getNamedPipe();
             //NamedPipeMessage* namedPipeMessage = (NamedPipeMessage*)message;

             if (namedPipe.getPipe() == closeConnectionMessage->namedPipe.getPipe())
             {
                 _monitor->unsolicitPipeMessages(namedPipe);
                 _rep->connections.remove(i);
                 delete connection;
                 break;
             } 
         }


       }// for loop
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
   PEGASUS_SOCKET socket;

#ifdef PEGASUS_LOCALDOMAINSOCKET_DEBUG
  {
  AutoMutex automut(Monitor::_cout_mut);
  PEGASUS_STD(cout) << "HTTPConnector::connect after connectLocal on windows section" << PEGASUS_STD(endl);
  }
#endif

#ifndef PEGASUS_DISABLE_LOCAL_DOMAIN_SOCKET
   if (host == String::EMPTY)  //connect request was made with CIMClient::connectLocal
   {
      // Set up the domain socket for a local connection

#ifdef PEGASUS_OS_TYPE_WINDOWS
   {
#ifdef PEGASUS_LOCALDOMAINSOCKET_DEBUG
   AutoMutex automut(Monitor::_cout_mut);
   PEGASUS_STD(cout) << "HTTPConnector::connect before connectLocal on windows section" << PEGASUS_STD(endl);

      //CIMClient::connectLocal [host == String::EMPTY] use NamedPipes on windows
       PEGASUS_STD(cout) << "HTTPConnector::connect at connectLocal on windows section" << PEGASUS_STD(endl);
#endif
   }
       HTTPConnection* pipeConnection = _connectNamedPipe(outputMessageQueue);

       //error checking needed here


       if (pipeConnection->isNamedPipeConnection()) //this if/else is a small bit of error checking - it needs to be better
       {
#ifdef PEGASUS_LOCALDOMAINSOCKET_DEBUG
           AutoMutex automut(Monitor::_cout_mut);
           PEGASUS_STD(cout) <<" named pipe HTTPConnetion has this as an owner - " <<
               pipeConnection->get_owner().getQueueName() << " it should be " <<
                this->getQueueName() << PEGASUS_STD(endl);
#endif
       }
       else
           {
#ifdef PEGASUS_LOCALDOMAINSOCKET_DEBUG
           AutoMutex automut(Monitor::_cout_mut);
           PEGASUS_STD(cout) <<"HTTPConnection returned from _connectNamedPipe is not a pipe conection " << PEGASUS_STD(endl);
#endif
           }
           // We may need to Assert here...

#ifdef PEGASUS_LOCALDOMAINSOCKET_DEBUG
       {
       AutoMutex automut(Monitor::_cout_mut);
       PEGASUS_STD(cout) << "HTTPConnector::connect after call to _connectNamedPipe" << PEGASUS_STD(endl);

       PEGASUS_STD(cout) << "HTTPConnector::connect check HTTPConnection retruned be _connectNamePipe " << endl;
       PEGASUS_STD(cout) << "HTTPConnector::connect pipeConnection->getNamedPipe().getName() = " << pipeConnection->getNamedPipe().getName() << endl;
       }
#endif
      return pipeConnection;
   }

#else
      sockaddr_un address;
      address.sun_family = AF_UNIX;
      strcpy(address.sun_path, PEGASUS_LOCAL_DOMAIN_SOCKET_PATH);
#ifdef PEGASUS_PLATFORM_OS400_ISERIES_IBM
      AtoE(address.sun_path);
#endif

      socket = ::socket(AF_UNIX, SOCK_STREAM, 0);
      if (socket == PEGASUS_INVALID_SOCKET)
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
#endif
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
   if (socket == PEGASUS_INVALID_SOCKET)
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
#ifdef PEGASUS_LOCALDOMAINSOCKET_DEBUG
      //this is a failure block
      AutoMutex automut(Monitor::_cout_mut);
      PEGASUS_STD(cout) << "_monitor->solicitSocketMessages failed " << PEGASUS_STD(endl);
#endif
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

    if (!currentConnection->isNamedPipeConnection())
    {
#ifdef PEGASUS_LOCALDOMAINSOCKET_DEBUG
        {
            AutoMutex automut(Monitor::_cout_mut);
            cout << " in HTTPConnector::disconnect before currentConnection->getSocket " << endl;
        }
#endif
        PEGASUS_SOCKET socket = currentConnection->getSocket(); 
        _monitor->unsolicitSocketMessages(socket);
    }
    else
    {
#ifdef PEGASUS_LOCALDOMAINSOCKET_DEBUG
        {
            AutoMutex automut(Monitor::_cout_mut);
            cout << " in HTTPConnector::disconnect before currentConnection->getNamedPipe " << endl;
        }
#endif
        NamedPipe namedPipe = currentConnection->getNamedPipe();
        
        _monitor->unsolicitPipeMessages(namedPipe);
        
#ifdef PEGASUS_LOCALDOMAINSOCKET_DEBUG
        {
            AutoMutex automut(Monitor::_cout_mut);
            cout << " in HTTPConnector::disconnecting named pipe handle: " << namedPipe.getPipe() << endl;
        }
#endif
       //namedPipe.disconnect(namedPipe.getPipe()); 
    }
    
    _rep->connections.remove(index);
    delete currentConnection;
}

void HTTPConnector::_deleteConnection(HTTPConnection* httpConnection)
{
    if (!httpConnection->isNamedPipeConnection())
    {
#ifdef PEGASUS_LOCALDOMAINSOCKET_DEBUG
        {
            AutoMutex automut(Monitor::_cout_mut);
            cout << " in HTTPConnector::_deleteConnection before httpConnection->getSocket " << endl;
        }
#endif

        PEGASUS_SOCKET socket = httpConnection->getSocket(); 

        // Unsolicit SocketMessages:

        _monitor->unsolicitSocketMessages(socket);
    }
    else 
    {
#ifdef PEGASUS_LOCALDOMAINSOCKET_DEBUG
        {
            AutoMutex automut(Monitor::_cout_mut);
            cout << " in HTTPConnector::_deleteConnection before httpConnection->getNamedPipe " << endl;
        }
#endif


        NamedPipe namedPipe = httpConnection->getNamedPipe();

        _monitor->unsolicitPipeMessages(namedPipe);

#ifdef PEGASUS_LOCALDOMAINSOCKET_DEBUG
        {
            AutoMutex automut(Monitor::_cout_mut);
            cout << " in HTTPConnector::disconnecting named pipe handle: " << namedPipe.getPipe() << endl;
        }
#endif
       //namedPipe.disconnect(namedPipe.getPipe()); 
    }
    // Destroy the connection (causing it to close):

    delete httpConnection;
}

 HTTPConnection* HTTPConnector::_connectNamedPipe(MessageQueue* outputMessageQueue)
{
    NamedPipeClient client("\\\\.\\pipe\\MyNamedPipe");
#ifdef PEGASUS_LOCALDOMAINSOCKET_DEBUG    
 {
    AutoMutex automut(Monitor::_cout_mut);
    PEGASUS_STD(cout) << "In HTTPConnector::_connectNamedPipe after client constuctor" << PEGASUS_STD(endl);
 }
#endif
    NamedPipeClientEndPiont nPCEndPoint = client.connect();

#ifdef PEGASUS_LOCALDOMAINSOCKET_DEBUG
    {
    AutoMutex automut(Monitor::_cout_mut);
    cout << "In HTTPConnector::_connectNamedPipe just creaed a pipe named - " << nPCEndPoint.getName() << endl;
    }
#endif


    // Will need to catch the exception from connect...

    //HANDLE handle = client.connect();
   /* if(nPCEndPoint == 0)
    {
        cout << "NamedPipeClient::connect() failed" << endl;

        return(false);
    } */


    //PEGASUS_STD(cout) << "In HTTPConnector::_connectNamedPipe after client.connect()" << PEGASUS_STD(endl);

    HTTPConnection* connection = new HTTPConnection(_monitor, nPCEndPoint,
        this, static_cast<MessageQueueService *>(outputMessageQueue), false);
    
#ifdef PEGASUS_LOCALDOMAINSOCKET_DEBUG   
    {
    AutoMutex automut(Monitor::_cout_mut);
    PEGASUS_STD(cout) << "In HTTPConnector::_connectNamedPipe after creating HTTPConnection" << PEGASUS_STD(endl);

    PEGASUS_STD(cout) << "In HTTPConnector::_connectNamedPipe pipe in HTTPConnection is - " << (connection->getNamedPipe()).getName() << endl;
    }
#endif
    // Solicit events on this new connection's socket:

   if (-1 == (_entry_index = _monitor->solicitPipeMessages(
      connection->getNamedPipe(),
      NamedPipeMessage::READ | NamedPipeMessage::EXCEPTION,
      connection->getQueueId(), Monitor::CONNECTOR/*Monitor::CONNECTION*/)))
   {
      //this is a failure block
#ifdef PEGASUS_LOCALDOMAINSOCKET_DEBUG
      AutoMutex automut(Monitor::_cout_mut);
      PEGASUS_STD(cout) << "_monitor->solicitSocketMessages failed " << PEGASUS_STD(endl);
#endif
   }
#ifdef PEGASUS_LOCALDOMAINSOCKET_DEBUG
  {
  AutoMutex automut(Monitor::_cout_mut);
  PEGASUS_STD(cout) << "In HTTPConnector::_connectNamedPipe after solicitPipeMessages "<< PEGASUS_STD(endl);
  }
#endif

   // Save the socket for cleanup later:

   _rep->connections.append(connection);

#ifdef PEGASUS_LOCALDOMAINSOCKET_DEBUG
   {
   AutoMutex automut(Monitor::_cout_mut);
   PEGASUS_STD(cout) << "In HTTPConnector::_connectNamedPipeabout to return HTTPConnetion" << PEGASUS_STD(endl);
   }
#endif

    return connection;

}

PEGASUS_NAMESPACE_END
