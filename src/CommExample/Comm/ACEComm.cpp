//BEGIN_LICENSE
//
// Copyright (c) 2000 The Open Group, BMC Software, Tivoli Systems, IBM
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
//END_LICENSE
//BEGIN_HISTORY
//
// Author:
//
// $Log: ACEComm.cpp,v $
// Revision 1.1  2001/02/18 02:59:21  mike
// new
//
// Revision 1.1  2001/02/17 02:20:17  mike
// new
//
//
//END_HISTORY

#include <cstring>
#include <ace/SOCK_Connector.h>
#include <ace/INET_Addr.h>
#include <ace/Reactor.h>
#include <ace/Service_Config.h>
#include <ace/Connector.h>
#include <ace/SOCK_Stream.h>
#include <ace/OS.h>
#include "ACEComm.h"

PEGASUS_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
//
// Alternative to ACE main procedure workaround!
//
////////////////////////////////////////////////////////////////////////////////

#ifdef PEGASUS_OS_TYPE_WINDOWS
ACE_OS_Object_Manager ace_os_object_manager;
ACE_Object_Manager ace_object_manager;
#endif

////////////////////////////////////////////////////////////////////////////////
//
// ACEConnection
//
////////////////////////////////////////////////////////////////////////////////

typedef ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_NULL_SYNCH> ACESvcHandler;

class ACEConnection : 
    public virtual ACESvcHandler,
    public virtual Connection
{
public:

    ACEConnection();

    ACEConnection(Handler* handler);

    virtual ~ACEConnection();

    virtual Sint32 read(void* ptr, Uint32 size);

    virtual Sint32 write(const void* ptr, Uint32 size);

    virtual Sint32 readN(void* ptr, Uint32 size);

    virtual Sint32 writeN(const void* ptr, Uint32 size);

    virtual void enableBlocking();

    virtual void disableBlocking();

    //--------------------------------------------------------------------------
    // ACE Methods:
    //--------------------------------------------------------------------------

    virtual int open(void* = 0);

    virtual int handle_input(ACE_HANDLE);

    virtual int handle_close(ACE_HANDLE, ACE_Reactor_Mask mask);

private:
};

ACEConnection::ACEConnection() : Connection(0)
{
    assert(0);
}

ACEConnection::ACEConnection(Handler* handler) : Connection(handler)
{
    assert(handler != 0);
}

ACEConnection::~ACEConnection()
{
    delete _handler;
}

Sint32 ACEConnection::read(void* ptr, Uint32 size)
{
    return peer().recv(ptr, size);
}

Sint32 ACEConnection::write(const void* ptr, Uint32 size)
{
    return peer().send(ptr, size);
}

Sint32 ACEConnection::readN(void* ptr, Uint32 size)
{
    return peer().recv_n(ptr, size);
}

Sint32 ACEConnection::writeN(const void* ptr, Uint32 size)
{
    return peer().send_n(ptr, size);
}

void ACEConnection::enableBlocking()
{
    peer().enable(ACE_NONBLOCK);
}

void ACEConnection::disableBlocking()
{
    peer().disable(ACE_NONBLOCK);
}

int ACEConnection::open(void* argument)
{
    int result = ACESvcHandler::open(argument);

    if (_handler)
    {
	if (!_handler->handleOpen(this))
	    return -1;
    }

    return result;
}

int ACEConnection::handle_input(ACE_HANDLE)
{
    if (_handler)
    {
	if (!_handler->handleInput(this))
	    return -1;
    }
    
    return 0;
}

int ACEConnection::handle_close(ACE_HANDLE handle, ACE_Reactor_Mask mask)
{
    if (_handler)
	_handler->handleClose(this);

    return ACESvcHandler::handle_close(handle, mask);
}

////////////////////////////////////////////////////////////////////////////////
//
// ACEConnector
//
////////////////////////////////////////////////////////////////////////////////

class ACEConnector : public Connector
{
public:

    ACEConnector(HandlerFactory* factory);

    virtual ~ACEConnector();

    virtual Connection* connect(const char* serviceLocator);

    virtual void disconnect(Connection* connection);

    virtual void run();

private:
};

static Boolean _ParseServiceLocator(
    const char* serviceLocator,
    char*& hostName,
    long& port)
{
    hostName = 0;
    port = 0;

    // Get the host name:

    hostName = strcpy(new char[strlen(serviceLocator) + 1], serviceLocator);

    if (!hostName)
	return false;

    char* colon = strchr(hostName, ':');

    if (!colon)
    {
	delete [] hostName;
	serviceLocator = 0;
	return false;
    }

    *colon = '\0';

    // Get the port:
    
    const char* portString = colon + 1;

    char* end = 0;

    port = strtol(portString, &end, 10);

    if (!end || *end)
    {
	delete [] hostName;
	serviceLocator = 0;
	port = 0;
	return false;
    }

    return true;
}

ACEConnector::ACEConnector(HandlerFactory* factory) : Connector(factory)
{

}

ACEConnector::~ACEConnector()
{

}

typedef ACE_Connector<ACEConnection, ACE_SOCK_CONNECTOR> ACEConnectorImpl;

Connection* ACEConnector::connect(const char* serviceLocator)
{
    assert(_factory != 0);
    assert(serviceLocator != 0);

    // Extract hostname and port form service locator: it is in this form: 
    // <hostname>:<port>. For example: "www.myplace.com:1234".

    char* hostName = 0;
    long port = 0;

    if (!_ParseServiceLocator(serviceLocator, hostName, port))
    {
	// ATTN: throw exception here:
	return 0;
    }

    // Create an address:

    ACE_INET_Addr addr((unsigned short)port, hostName);
    delete [] hostName;

    Handler* handler = _factory->create();
    assert(handler != 0);
    ACEConnection* connection = new ACEConnection(handler);

    ACEConnectorImpl connectorImpl;

    if (connectorImpl.connect(connection, addr) == -1)
    {
	// ATTN: throw exception here:
	return 0;
    }
    return connection;
}

void ACEConnector::disconnect(Connection* connection)
{

}

void ACEConnector::run()
{
    for (;;)
	ACE_Reactor::instance()->handle_events();
}

////////////////////////////////////////////////////////////////////////////////
//
// ACEAcceptor
//
////////////////////////////////////////////////////////////////////////////////

class ACEAcceptor : public Acceptor
{
public:

    ACEAcceptor(HandlerFactory* factory);

    virtual ~ACEAcceptor();

    virtual void bind(const char* bindString);

    virtual void accept(Connection* connection);

    virtual void run();

private:
    HandlerFactory* _factory;
};

ACEAcceptor::ACEAcceptor(HandlerFactory* factory) : Acceptor(factory)
{

}

ACEAcceptor::~ACEAcceptor()
{

}

void ACEAcceptor::bind(const char* bindString)
{

}

void ACEAcceptor::accept(Connection* connection)
{

}

void ACEAcceptor::run()
{
    for (;;)
	ACE_Reactor::instance()->handle_events();
}

////////////////////////////////////////////////////////////////////////////////
//
// ACECommFactory
//
////////////////////////////////////////////////////////////////////////////////

Acceptor* ACECommFactory::createAcceptor(HandlerFactory* factory)
{
    return new ACEAcceptor(factory);
}

Connector* ACECommFactory::createConnector(HandlerFactory* factory)
{
    return new ACEConnector(factory);
}

PEGASUS_NAMESPACE_END
