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
// Author: Mike Brasher
//
// $Log: Comm.h,v $
// Revision 1.1  2001/02/17 02:20:17  mike
// new
//
//
//END_HISTORY

#ifndef Pegasus_Comm_h
#define Pegasus_Comm_h

#include <Pegasus/Common/Config.h>

PEGASUS_NAMESPACE_BEGIN

class Handler;

class Connection
{
public:

    Connection(Handler* handler = 0) : _handler(handler) { }

    virtual ~Connection();

    virtual Sint32 read(void* ptr, Uint32 size) = 0;

    virtual Sint32 write(const void* ptr, Uint32 size) = 0;

    virtual Sint32 readN(void* ptr, Uint32 size) = 0;

    virtual Sint32 writeN(const void* ptr, Uint32 size) = 0;

    virtual void enableBlocking() = 0;

    virtual void disableBlocking() = 0;

protected:

    Handler* _handler;
};

class Handler
{
public:

    Handler() { }

    virtual ~Handler();

    virtual Boolean handleOpen(Connection* connection) = 0;

    virtual Boolean handleInput(Connection* connection) = 0;

    virtual Boolean handleClose(Connection* connection) = 0;
};

class HandlerFactory
{
public:

    virtual Handler* create() = 0;
};

class Connector
{
public:

    Connector(HandlerFactory* factory) : _factory(factory) { }

    virtual ~Connector();

    virtual Connection* connect(const char* serviceLocator) = 0;

    virtual void disconnect(Connection* connection) = 0;

    virtual void run() = 0;

protected:

    HandlerFactory* _factory;
};

class Acceptor
{
public:

    Acceptor(HandlerFactory* factory) : _factory(factory) { }

    virtual ~Acceptor();

    virtual void bind(const char* bindString) = 0;

    virtual void accept(Connection* connection) = 0;

    virtual void run() = 0;

protected:

    HandlerFactory* _factory;
};

class CommFactory
{
public:

    virtual Acceptor* createAcceptor(HandlerFactory* factory) = 0;

    virtual Connector* createConnector(HandlerFactory* factory) = 0;
};


PEGASUS_NAMESPACE_END

#endif /* Pegasus_Comm_h */
