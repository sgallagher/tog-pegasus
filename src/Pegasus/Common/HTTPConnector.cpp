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
//
//%/////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include "Config.h"
#include "Socket.h"

#ifdef PEGASUS_OS_TYPE_WINDOWS
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
    : _monitor(monitor)
{
    _rep = new HTTPConnectorRep;
    Socket::initializeInterface();
}

HTTPConnector::~HTTPConnector()
{
    delete _rep;
    Socket::uninitializeInterface();
}

void HTTPConnector::handleEnqueue()
{
    // cout << "HTTPConnector::handleEnqueue()" << endl;

    Message* message = dequeue();

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

const char* HTTPConnector::getQueueName() const
{
    return "HTTPConnector";
}

HTTPConnection* HTTPConnector::connect(
    const String& locator, 
    MessageQueue* outputMessageQueue)
{
    // Parse the locator (get hostname and port):

    char* hostname;
    int port;

    if (!_ParseLocator(locator, hostname, port))
    {
	delete [] hostname;
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

    Sint32 socket = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (socket < 0)
	throw CannotCreateSocket();

    // Conect the socket to the address:

    if (::connect(socket, (sockaddr*)&address, sizeof(address)) < 0)
	throw CannotConnect(locator);

    // Create HTTPConnection object:

    HTTPConnection* connection = new HTTPConnection(_monitor, socket,
	this, outputMessageQueue);

    // Solicit events on this new connection's socket:

    if (!_monitor->solicitSocketMessages(
	socket,
	SocketMessage::READ | SocketMessage::EXCEPTION,
	connection->getQueueId()))
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
	HTTPConnection* connection = _rep->connections[i];	
	Sint32 socket = connection->getSocket();

	// Unsolicit SocketMessages:

	_monitor->unsolicitSocketMessages(socket);

	// Destroy the connection (causing it to close):

	delete connection;
    }

    _rep->connections.clear();
}

PEGASUS_NAMESPACE_END
