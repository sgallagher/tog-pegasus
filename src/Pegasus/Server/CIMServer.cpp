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
//         Yi Zhou, Hewlett-Packard Company (yi_zhou@hp.com)
//         Jenny Yu, Hewlett-Packard Company (jenny_yu@hp.com)
//         Sushma Fernandes, Hewlett-Packard Company (sushma_fernandes@hp.com)
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
#include <Pegasus/Common/Cimom.h>
#include <Pegasus/Repository/CIMRepository.h>
#include <Pegasus/ExportServer/CIMExportRequestDispatcher.h>
#include <Pegasus/ExportServer/CIMExportResponseEncoder.h>
#include <Pegasus/ExportServer/CIMExportRequestDecoder.h>
#include <Pegasus/Config/ConfigManager.h>
#include <Pegasus/Security/UserManager/UserManager.h>
#include <Pegasus/HandlerService/IndicationHandlerService.h>
#include <Pegasus/IndicationService/IndicationService.h>
#include <Pegasus/ProviderManager/ProviderManagerService.h>
#include "CIMServer.h"
#include "CIMOperationRequestDispatcher.h"
#include "CIMOperationResponseEncoder.h"
#include "CIMOperationRequestDecoder.h"
#include "CIMOperationRequestAuthorizer.h"
#include "HTTPAuthenticatorDelegator.h"


#define DDD(X) // X

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

CIMServer::CIMServer(
    Monitor* monitor,
    Boolean useSSL)
   : _dieNow(false), _useSSL(useSSL)
{
    const char METHOD_NAME[] = "CIMServer::CIMServer()";

    PEG_FUNC_ENTER(TRC_SERVER, METHOD_NAME);

    static const char CERTIFICATE[] = "/server.pem";

    String repositoryRootPath = String::EMPTY;

    // -- Save the monitor or create a new one:

    _monitor = monitor;

    repositoryRootPath =
	    ConfigManager::getHomedPath(ConfigManager::getInstance()->getCurrentValue("repositoryDir"));

    // -- Create a repository:

    if (!FileSystem::isDirectory(repositoryRootPath))
    {
        PEG_FUNC_EXIT(TRC_SERVER, METHOD_NAME);
	throw NoSuchDirectory(repositoryRootPath);
    }

    CIMRepository* repository = new CIMRepository(repositoryRootPath);

    // -- Create a CIMServerState object:

    _serverState = new CIMServerState();

    // -- Create queue inter-connections:
    _providerManager = new ProviderManagerService;
    _handlerService = new IndicationHandlerService;

    _cimOperationRequestDispatcher
	= new CIMOperationRequestDispatcher(repository, this);

    _indicationService = new IndicationService(repository, this);

    _cimOperationResponseEncoder
	= new CIMOperationResponseEncoder;

    //
    // get the configured authentication and authorization flags
    //
    ConfigManager* configManager = ConfigManager::getInstance();

    Boolean requireAuthentication = false;
    Boolean requireAuthorization = false;

    if (String::equal(
        configManager->getCurrentValue("requireAuthentication"), "true"))
    {
        requireAuthentication = true;
    }

    if (String::equal(
        configManager->getCurrentValue("requireAuthorization"), "true"))
    {
        requireAuthorization = true;
    }

    //
    // check if authentication and authorization are enabled
    //
    if ( requireAuthentication && requireAuthorization )
    {
        //
        // Create Authorization queue only if authorization and
        // authentication are enabled
        //
        _cimOperationRequestAuthorizer = new CIMOperationRequestAuthorizer(
            _cimOperationRequestDispatcher);

        _cimOperationRequestDecoder = new CIMOperationRequestDecoder(
            _cimOperationRequestAuthorizer,
            _cimOperationResponseEncoder->getQueueId());
    }
    else
    {
        _cimOperationRequestDecoder = new CIMOperationRequestDecoder(
// to test async cimom, substibute cimom for _cimOperationRequestDispatcher below
            _cimOperationRequestDispatcher,
// substitute the cimom as well for the _cimOperationResponseEncoder below
            _cimOperationResponseEncoder->getQueueId());

    }

    _cimExportRequestDispatcher
	= new CIMExportRequestDispatcher();

    _cimExportResponseEncoder
	= new CIMExportResponseEncoder;

    _cimExportRequestDecoder = new CIMExportRequestDecoder(
	_cimExportRequestDispatcher,
	_cimExportResponseEncoder->getQueueId());

    HTTPAuthenticatorDelegator* serverQueue = new HTTPAuthenticatorDelegator(
        _cimOperationRequestDecoder->getQueueId(),
        _cimExportRequestDecoder->getQueueId());

    UserManager* userManager = UserManager::getInstance(repository);

    // Create SSL context
    SSLContext * sslcontext;
    if (_useSSL)
    {
        String certPath = ConfigManager::getPegasusHome();
        certPath.append(CERTIFICATE);

        sslcontext = new SSLContext(certPath);
    }
    else
        sslcontext = NULL;

    _acceptor = new HTTPAcceptor(_monitor, serverQueue, sslcontext);

    /** load registered providers from repository, and creates
        provider block table
    */
    //_cimOperationRequestDispatcher->loadRegisteredProviders();

    PEG_FUNC_EXIT(TRC_SERVER, METHOD_NAME);
}

CIMServer::~CIMServer()
{
    const char METHOD_NAME[] = "CIMServer::~CIMServer()";

    PEG_FUNC_ENTER(TRC_SERVER, METHOD_NAME);

    // Note: do not delete the acceptor because it belongs to the Monitor
    // which takes care of disposing of it.

    PEG_FUNC_EXIT(TRC_SERVER, METHOD_NAME);
}

void CIMServer::bind(Uint32 port)
{
    const char METHOD_NAME[] = "CIMServer::bind()";

    PEG_FUNC_ENTER(TRC_SERVER, METHOD_NAME);

    // not the best place to build the service url, but it works for now
    // because the address string is accessible  mdday

    _acceptor->bind(port);

    PEG_FUNC_EXIT(TRC_SERVER, METHOD_NAME);
}

void CIMServer::runForever()
{
    //ATTN: Do not add Trace code in this method.
    if(!_dieNow)
	_monitor->run(100);
}

void CIMServer::stopClientConnection()
{
    const char METHOD_NAME[] = "CIMServer::stopClientConnection()";

    PEG_FUNC_ENTER(TRC_SERVER, METHOD_NAME);

    _acceptor->closeConnectionSocket();

    PEG_FUNC_EXIT(TRC_SERVER, METHOD_NAME);
}

void CIMServer::shutdown()
{
    const char METHOD_NAME[] = "CIMServer::shutdown()";

    PEG_FUNC_ENTER(TRC_SERVER, METHOD_NAME);

    _dieNow = true;

    PEG_FUNC_EXIT(TRC_SERVER, METHOD_NAME);
}

void CIMServer::resume()
{
    const char METHOD_NAME[] = "CIMServer::resume()";

    PEG_FUNC_ENTER(TRC_SERVER, METHOD_NAME);

    _acceptor->reopenConnectionSocket();

    PEG_FUNC_EXIT(TRC_SERVER, METHOD_NAME);
}

CIMOperationRequestDispatcher* CIMServer::getDispatcher()
{
    const char METHOD_NAME[] = "CIMServer::getDispatcher()";

    PEG_FUNC_ENTER(TRC_SERVER, METHOD_NAME);

    PEG_FUNC_EXIT(TRC_SERVER, METHOD_NAME);

    return _cimOperationRequestDispatcher;
}

void CIMServer::setState(Uint32 state)
{
    const char METHOD_NAME[] = "CIMServer::setState()";

    PEG_FUNC_ENTER(TRC_SERVER, METHOD_NAME);

    _serverState->setState(state);

    if (state == CIMServerState::TERMINATING)
    {
        // tell decoder that CIMServer is terminating
        _cimOperationRequestDecoder->setServerTerminating(true);
        _cimExportRequestDecoder->setServerTerminating(true);
    }
    else
    {
        // tell decoder that CIMServer is not terminating
        _cimOperationRequestDecoder->setServerTerminating(false);
        _cimExportRequestDecoder->setServerTerminating(false);
    }
    PEG_FUNC_EXIT(TRC_SERVER, METHOD_NAME);
}

Uint32 CIMServer::getOutstandingRequestCount()
{
    const char METHOD_NAME[] = "CIMServer::getOutstandingRequestCount()";

    PEG_FUNC_ENTER(TRC_SERVER, METHOD_NAME);

    PEG_FUNC_EXIT(TRC_SERVER, METHOD_NAME);

    return (_acceptor->getOutstandingRequestCount());
}

PEGASUS_NAMESPACE_END
