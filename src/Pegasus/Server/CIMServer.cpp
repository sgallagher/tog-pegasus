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
//         Mike Day (mdday@us.ibm.com)s
//
//%/////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <cassert>
#include <cstdio>
#include <cctype>
#include <time.h>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/TCPChannel.h>
#include <Pegasus/Repository/CIMRepository.h>
#include <Pegasus/Server/Dispatcher.h>
#include "ClientConnection.h"
#include "CIMServer.h"

#define DDD(X) // X

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
//
// ClientConnectionFactory
//
////////////////////////////////////////////////////////////////////////////////

class ClientConnectionFactory : public ChannelHandlerFactory
{
public:

    ClientConnectionFactory(MessageQueue* inputQueue) 
	: _inputQueue(inputQueue) { }

    virtual ~ClientConnectionFactory() { }

    virtual ChannelHandler* create() 
	{ return new ClientConnection(_inputQueue); }

private:

    MessageQueue* _inputQueue;
};

////////////////////////////////////////////////////////////////////////////////
//
// CIMServer
//
////////////////////////////////////////////////////////////////////////////////

CIMServer::CIMServer(
    Selector* selector,
    const String& rootPath) 
    : _rootPath(rootPath), _dieNow(false)
{
    static const char REPOSITORY[] = "/repository";

    // -- Save the selector or create a new one:

    _selector = selector;

    // -- Create a repository:

    if (!FileSystem::isDirectory(_rootPath))
	throw NoSuchDirectory(_rootPath);

    _repositoryRootPath = rootPath;
    _repositoryRootPath.append(REPOSITORY);

    if (!FileSystem::isDirectory(_repositoryRootPath))
	throw NoSuchDirectory(_repositoryRootPath);

    CIMRepository* repository = new CIMRepository(rootPath + "/repository");

    // -- Create a dispatcher object:

    Dispatcher* dispatcher = new Dispatcher(repository);

    // -- Create a channel factory:

    ClientConnectionFactory* factory = new ClientConnectionFactory(dispatcher);

    // Create an acceptor:

    _acceptor = new TCPChannelAcceptor(factory, _selector);
}

CIMServer::~CIMServer()
{
    // Note: do not delete the acceptor because it belongs to the Selector
    // which takes care of disposing of it.
}

void CIMServer::bind(const char* address)
{

  // not the best place to build the service url, but it works for now
  // because the address string is accessible  mdday

  if (!_acceptor->bind(address))
    throw CannotBindToAddress(address);
  
}

void CIMServer::runForever()
{
    if(!_dieNow)
	_selector->select(100);
}

PEGASUS_NAMESPACE_END
