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
//         Mike Day (mdday@us.ibm.com)s
//         Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
//         Nag Boranna, Hewlett-Packard Company (nagaraja_boranna@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>

#include <iostream>
#include <cassert>
#include <cstdio>
#include <cctype>
#include <ctime>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/TCPChannel.h>
#include <Pegasus/Common/HTTPAcceptor.h>
#include <Pegasus/Common/HTTPDelegator.h>
#include <Pegasus/Repository/CIMRepository.h>
#include <Pegasus/ExportServer/CIMExportRequestDispatcher.h>
#include <Pegasus/ExportServer/CIMExportResponseEncoder.h>
#include <Pegasus/ExportServer/CIMExportRequestDecoder.h>
#include "CIMServer.h"
#include "CIMOperationRequestDispatcher.h"
#include "CIMOperationResponseEncoder.h"
#include "CIMOperationRequestDecoder.h"
#include "HTTPAuthenticatorDelegator.h"


#define DDD(X) // X

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

CIMServer::CIMServer(
    Monitor* monitor,
    const String& rootPath)
    : _dieNow(false), _rootPath(rootPath)
{
    static const char REPOSITORY[] = "/repository";

    // -- Save the monitor or create a new one:

    _monitor = monitor;

    // -- Create a repository:

    if (!FileSystem::isDirectory(_rootPath))
	throw NoSuchDirectory(_rootPath);

    _repositoryRootPath = rootPath;
    _repositoryRootPath.append(REPOSITORY);

    if (!FileSystem::isDirectory(_repositoryRootPath))
	throw NoSuchDirectory(_repositoryRootPath);

    CIMRepository* repository = new CIMRepository(rootPath + "/repository");

    // -- Create queue inter-connections:

    _cimOperationRequestDispatcher
	= new CIMOperationRequestDispatcher(repository);

    _cimOperationResponseEncoder
	= new CIMOperationResponseEncoder;

    _cimOperationRequestDecoder = new CIMOperationRequestDecoder(
	_cimOperationRequestDispatcher,
	_cimOperationResponseEncoder->getQueueId());

    _cimExportRequestDispatcher
	= new CIMExportRequestDispatcher(repository);

    _cimExportResponseEncoder
	= new CIMExportResponseEncoder;

    _cimExportRequestDecoder = new CIMExportRequestDecoder(
	_cimExportRequestDispatcher,
	_cimExportResponseEncoder->getQueueId());

    HTTPAuthenticatorDelegator* serevrQueue = new HTTPAuthenticatorDelegator(
        _cimOperationRequestDecoder,
        _cimExportRequestDecoder);

    _acceptor = new HTTPAcceptor(_monitor, serevrQueue);
}

CIMServer::~CIMServer()
{
    // Note: do not delete the acceptor because it belongs to the Monitor
    // which takes care of disposing of it.
}

void CIMServer::bind(Uint32 port)
{
    // not the best place to build the service url, but it works for now
    // because the address string is accessible  mdday

    _acceptor->bind(port);
}

void CIMServer::runForever()
{
    if(!_dieNow)
	_monitor->run(100);
}

PEGASUS_NAMESPACE_END
