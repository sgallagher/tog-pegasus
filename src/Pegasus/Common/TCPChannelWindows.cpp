//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 The Open group, BMC Software, Tivoli Systems, IBM
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
#include "TCPChannel.h"
#include <winsock.h>

using namespace std;

PEGASUS_NAMESPACE_BEGIN

#define DDD(X) // X

// ATTN-A: manage lifetime of all these objects. Do a walkthrough!
// ATTN-B: add methods for getting the remote hostname and port!

////////////////////////////////////////////////////////////////////////////////
//
// Routines for starting and stoping WinSock:
//
////////////////////////////////////////////////////////////////////////////////

static Uint32 _wsaCount = 0;

static void _WSAInc()
{
    if (_wsaCount == 0)
    {
	WSADATA tmp;

	if (WSAStartup(0x202, &tmp) == SOCKET_ERROR)
	    WSACleanup();
    }

    _wsaCount++;
}

static void _WSADec()
{
    _wsaCount--;

    if (_wsaCount == 0)
	WSACleanup();
}

////////////////////////////////////////////////////////////////////////////////
//
// TCPChannel
//
////////////////////////////////////////////////////////////////////////////////

TCPChannel::TCPChannel(
    Selector* selector, Uint32 desc, ChannelHandler* handler) 
    : _selector(selector), _desc(desc), _blocking(true), _handler(handler)
{

}

TCPChannel::~TCPChannel()
{
    if (_desc != -1)
	::closesocket(_desc);

    if (_handler)
	delete _handler;
}

Sint32 TCPChannel::read(void* ptr, Uint32 size)
{
    if (_desc == -1)
	return -1;

    return ::recv(_desc, (char*)ptr, size, 0);
}

Sint32 TCPChannel::write(const void* ptr, Uint32 size)
{
    if (_desc == -1)
	return -1;

    return ::send(_desc, (const char*)ptr, size, 0);
}

void TCPChannel::enableBlocking()
{
    unsigned long flag = 0;
    ioctlsocket(_desc, FIONBIO, &flag);
    _blocking = true;
}

void TCPChannel::disableBlocking()
{
    unsigned long flag = 1;
    ioctlsocket(_desc, FIONBIO, &flag);
    _blocking = false;
}

Boolean TCPChannel::wouldBlock() const
{
    return GetLastError() == WSAEWOULDBLOCK;
}

////////////////////////////////////////////////////////////////////////////////
//
// TCPChannelConnector
//
////////////////////////////////////////////////////////////////////////////////

static inline char* _Clone(const char* str)
{
    return strcpy(new char[strlen(str) + 1], str);
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

static Boolean _ParseAddress(
    const char* address, 
    char*& hostname,
    int& port)
{
    if (!address)
	return false;

    // Extract the hostname:port expression (e.g., www.book.com:8080):

    hostname = _Clone(address);
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

TCPChannelConnector::TCPChannelConnector(
    ChannelHandlerFactory* factory,
    Selector* selector)
    : ChannelConnector(factory), _selector(selector)
{
    _WSAInc();
}

TCPChannelConnector::~TCPChannelConnector()
{
    _WSADec();
}


Channel* TCPChannelConnector::connect(const char* addressString)
{
    if (!_factory)
	return 0;

    // Parse the address:

    char* hostname;
    int port;

    if (!_ParseAddress(addressString, hostname, port))
	return 0;

    // Make the internet address:

    sockaddr_in address;

    if (!_MakeAddress(hostname, port, address))
    {
	delete [] hostname;
        return false;
    }

    // Create the socket:

    int desc = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (desc < 0)
	return 0;

    // Conect the socket to the address:

    if (::connect(desc, (sockaddr*)&address, sizeof(address)) < 0)
	return 0;

    // Create the channel:

    ChannelHandler* handler = _factory->create();
    TCPChannel* channel = new TCPChannel(_selector, desc, handler);

    // ATTN-B: at this time, the selector does not manage the lifetime
    // of channel objects. The selector is responsible for this. When
    // the selector goes out of scope, it destroys all of its handlers
    // (including any channels that were registered by this object).

    // Register the channel to receive events:

    _selector->addHandler(
	desc, 
	// Selector::READ | Selector::WRITE | Selector::EXCEPTION,
	Selector::READ | Selector::EXCEPTION,
	channel);

    handler->handleOpen(channel);

    return channel;
}

void TCPChannelConnector::disconnect(Channel* channel)
{
    // ATTN-A: Implement this! But how?
}

////////////////////////////////////////////////////////////////////////////////
//
// TCPChannelAcceptor
//
////////////////////////////////////////////////////////////////////////////////

TCPChannelAcceptor::TCPChannelAcceptor(
    ChannelHandlerFactory* factory,
    Selector* selector)
    : ChannelAcceptor(factory), _selector(selector), _desc(-1)
{
    _WSAInc();
}

TCPChannelAcceptor::~TCPChannelAcceptor()
{
    _WSADec();
}

Boolean TCPChannelAcceptor::bind(const char* addressStr)
{
    // Extract the port:

    char* end = 0;
    Sint32 port = strtol(addressStr, &end, 10);

    if (!end || *end != '\0')
	return false;

    // Create address:

    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_family = AF_INET;
    address.sin_port = htons(port);

    // Create socket:
    
    _desc = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (_desc < 0)
    {
	_desc = -1;
	return false;
    }

    // Bind socket to address:

    if (::bind(_desc, (struct sockaddr*)(void*)&address, sizeof(address)) < 0)
	return false;

    // Listen:

    int const MAX_CONNECTION_QUEUE_LENGTH = 5;

    if (listen(_desc, MAX_CONNECTION_QUEUE_LENGTH) < 0)
	return false;

    // Register this acceptor to receive read events:

    _selector->addHandler(
	_desc, 
	Selector::READ | Selector::EXCEPTION,
	this);

    return true;
}

Boolean TCPChannelAcceptor::handle(Sint32 desc, Uint32 reasons)
{
    // If socket descriptor is invalid, bail out now!

    if (_desc == -1 || _desc != desc)
	return true;

    // If this was not called in connection with a read event, bail out!

    if (!(reasons | Selector::READ))
	return true;

    // Accept the connection (populate the address):

    sockaddr_in address;
    int n = sizeof(address);
    Sint32 slaveDesc = accept(_desc, (struct sockaddr*)&address, &n);

    DDD(cout << "Accepting Connection " << slaveDesc << endl;)

    if (slaveDesc < 0)
	return true;

    // Use factory to create handler; create channel:

    if (!_factory)
	return true;

    ChannelHandler* handler = _factory->create();
    TCPChannel* channel = new TCPChannel(_selector, slaveDesc, handler);

    // Register the channel to receive events:

    _selector->addHandler(
	slaveDesc, 
	Selector::READ | Selector::EXCEPTION,
	channel);
    DDD(cout << "TCP Handle Open " << endl;)
    handler->handleOpen(channel);

    return true;
}

PEGASUS_NAMESPACE_END
