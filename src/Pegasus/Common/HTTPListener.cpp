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

#include "HTTPListener.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
//
// Static (file scope) functions:
//
////////////////////////////////////////////////////////////////////////////////

static void _CloseSocket(Sint32 socket)
{
#ifdef PEGASUS_OS_TYPE_WINDOWS
    closesocket(socket);
#else
    close(socket);
#endif
}

////////////////////////////////////////////////////////////////////////////////
//
// HTTPListenerRep
//
////////////////////////////////////////////////////////////////////////////////

struct HTTPListenerRep
{
    struct sockaddr_in address;
    Sint32 socket;
};

////////////////////////////////////////////////////////////////////////////////
//
// HTTPListener
//
////////////////////////////////////////////////////////////////////////////////

HTTPListener::HTTPListener(Monitor* monitor) : _monitor(monitor), _rep(0)
{
    // Note: we are couting on the monitor to initialize the socket
    // interface (in Windows you have to call WSAInitialize() and
    // WSAShutDown()).
}

HTTPListener::~HTTPListener()
{
    unbind();
}

void HTTPListener::handleEnqueue()
{
    Message* message = dequeue();

    if (!message)
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
		cout << "ACCEPTING CONNECTION" << endl;
		_acceptConnection();
	    }
	    else
	    {
		cout << "ACCEPTING ON A CONNECTION" << endl;
	    }

	    break;
	}

	default:
	    // ATTN: need unexpected message error!
	    break;
    };

    delete message;
}

void HTTPListener::bind(Uint32 portNumber)
{
    if (_rep)
	throw BindFailed("HTTP listener already bound");

    _rep = new HTTPListenerRep;

    // Create address:

    memset(&_rep->address, 0, sizeof(_rep->address));
    _rep->address.sin_addr.s_addr = INADDR_ANY;
    _rep->address.sin_family = AF_INET;
    _rep->address.sin_port = htons(portNumber);

    // Create socket:
    
    _rep->socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (_rep->socket < 0)
    {
	delete _rep;
	_rep = 0;
	throw BindFailed("Failed to create socket");
    }

    // Bind socket to port:

    if (::bind(_rep->socket, 
	(struct sockaddr*)(void*)&_rep->address, 
	sizeof(_rep->address)) < 0)
    {
	_CloseSocket(_rep->socket);
	delete _rep;
	_rep = 0;
	throw BindFailed("Failed to bind socket to port");
    }

    // Set up listening on the given port:

    int const MAX_CONNECTION_QUEUE_LENGTH = 5;

    if (listen(_rep->socket, MAX_CONNECTION_QUEUE_LENGTH) < 0)
    {
	_CloseSocket(_rep->socket);
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
	_CloseSocket(_rep->socket);
	delete _rep;
	_rep = 0;
	throw BindFailed("Failed to solicit socket messaeges");
    }
}

void HTTPListener::unbind()
{
    if (_rep)
    {
	_CloseSocket(_rep->socket);
	delete _rep;
	_rep = 0;
    }
}

void HTTPListener::_acceptConnection()
{
    // This function cannot be called on an invalid socket!

    PEGASUS_ASSERT(_rep != 0);

    if (!_rep)
	return;

    // Accept the connection (populate the address):

    sockaddr_in address;
    int n = sizeof(address);
    Sint32 socket = accept(_rep->socket, (struct sockaddr*)&address, &n);

    if (socket < 0)
    {
	if (getenv("PEGASUS_TRACE"))
	    cerr <<"HTTPListener: accept() failed" << endl;

	return;
    }

    // Solicit events on this new connection socket:

    if (!_monitor->solicitSocketMessages(
	socket,
	SocketMessage::READ | SocketMessage::EXCEPTION,
	getQueueId()))
    {
	_CloseSocket(socket);
    }
}

PEGASUS_NAMESPACE_END
