//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software, Hewlett-Packard Company, IBM,
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
//         Yi Zhou, Hewlett-Packard Company (yi_zhou@hp.com)
//         Jenny Yu, Hewlett-Packard Company (jenny_yu@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>

#include <iostream>
#include <cassert>
#include <cstdio>
#include <cctype>
#include <ctime>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/HTTPAcceptor.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/PegasusVersion.h>

#include <Pegasus/Repository/CIMRepository.h>
#include <Pegasus/ExportServer/CIMExportRequestDispatcher.h>
#include <Pegasus/ExportServer/CIMExportResponseEncoder.h>
#include <Pegasus/ExportServer/CIMExportRequestDecoder.h>

#include "CIMListener.h"

#define DDD(X) // X

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

CIMListener::CIMListener(
    Monitor* monitor,
    const String& rootPath,
    Boolean dynamicReg,
    Boolean staticConsumers,
    Boolean persistence)
    : _dieNow(false), _rootPath(rootPath),
    _dynamicReg(dynamicReg), 
    _staticConsumers(staticConsumers), 
    _persistence(persistence)
{
    PEG_METHOD_ENTER(TRC_SERVER, "CIMListener::CIMListener()");

    // -- Save the monitor or create a new one:

    _monitor = monitor;

    // -- Create a CIMListenerState object:

    _cimExportRequestDispatcher
	= new CIMExportRequestDispatcher(dynamicReg, staticConsumers, persistence);

    _cimExportResponseEncoder
	= new CIMExportResponseEncoder;

    _cimExportRequestDecoder = new CIMExportRequestDecoder(
	_cimExportRequestDispatcher,
	_cimExportResponseEncoder->getQueueId());

    SSLContext * sslcontext = NULL;

    _acceptor = new HTTPAcceptor(_monitor, _cimExportRequestDecoder, sslcontext);

    PEG_METHOD_EXIT();
}

CIMListener::~CIMListener()
{
    PEG_METHOD_ENTER(TRC_SERVER, "CIMListener::~CIMListener()");

    // Note: do not delete the acceptor because it belongs to the Monitor
    // which takes care of disposing of it.

    PEG_METHOD_EXIT();
}

void CIMListener::bind(Uint32 port)
{
    PEG_METHOD_ENTER(TRC_SERVER, "CIMListener::bind()");

    // not the best place to build the service url, but it works for now
    // because the address string is accessible  mdday

    _acceptor->bind(port);

    PEG_METHOD_EXIT();
}

void CIMListener::runForever()
{
    //ATTN: Do not add Trace code in this method.
    if(!_dieNow)
	_monitor->run(100);
}

void CIMListener::stopClientConnection()
{
    PEG_METHOD_ENTER(TRC_SERVER, "CIMListener::stopClientConnection()");

    _acceptor->closeConnectionSocket();

    PEG_METHOD_EXIT();
}

void CIMListener::shutdown()
{
    PEG_METHOD_ENTER(TRC_SERVER, "CIMListener::shutdown()");

    _dieNow = true;

    PEG_METHOD_EXIT();
}

void CIMListener::resume()
{
    PEG_METHOD_ENTER(TRC_SERVER, "CIMListener::resume()");

    _acceptor->reopenConnectionSocket();

    PEG_METHOD_EXIT();
}

CIMExportRequestDispatcher* CIMListener::getDispatcher()
{
    PEG_METHOD_ENTER(TRC_SERVER, "CIMListener::getDispatcher()");

    PEG_METHOD_EXIT();

    return _cimExportRequestDispatcher;
}

Uint32 CIMListener::getOutstandingRequestCount()
{
    PEG_METHOD_ENTER(TRC_SERVER, "CIMListener::getOutstandingRequestCount()");

    PEG_METHOD_EXIT();

    return (_acceptor->getOutstandingRequestCount());
}

PEGASUS_NAMESPACE_END
