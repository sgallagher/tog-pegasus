//%////////////////////////////////////////////////////////////////////////////
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
//=============================================================================
//
// Author: Jenny Yu, Hewlett-Packard Company (jenny_yu@hp.com)
//
// Modified By:
//
//%////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
//  ShutdownService
/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Destroyer.h>
#include <Pegasus/Server/ShutdownExceptions.h>
#include <Pegasus/Server/CIMServerState.h>
#include <Pegasus/Server/ShutdownService.h>
#include <Pegasus/Server/CIMOperationRequestDispatcher.h>

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

/**
    The constants representing the shutdown timeout property names
*/
static String TIMEOUT_PROPERTY = "operationTimeout";
static String SHUTDOWN_TIMEOUT_PROPERTY = "shutdownTimeout";

/**
Initialize ShutdownService instance
*/
ShutdownService* ShutdownService::_instance = 0;

/**
Initialize all other class variables
*/
CIMServer*              ShutdownService::_cimserver = 0;
ProviderManagerService* ShutdownService::_providerManagerService = 0;
ProviderManager*        ShutdownService::_providerManager = 0;
Uint32                  ShutdownService::_operationTimeout = 0;
Uint32                  ShutdownService::_shutdownTimeout = 0;

/** Constructor. */
ShutdownService::ShutdownService(CIMServer* cimserver)
{
    _cimserver = cimserver;
}

/** Destructor. */
ShutdownService::~ShutdownService()
{
}

/**
    return a pointer to the ShutdownService instance.
*/
ShutdownService* ShutdownService::getInstance(CIMServer* cimserver)
{
    if (!_instance)
    {
        _instance = new ShutdownService(cimserver);
    }
    return _instance;
}

/**
    The shutdown method to be called by the ShutdownProvider to
    process a shutdown request from the CLI client.
*/
void ShutdownService::shutdown(Boolean force, Uint32 timeout)
{
    //
    // Initialize variables
    //
    Boolean timeoutExpired = false;
    Boolean noMoreRequests = false;

    //
    // get ProviderManagerService
    //
    MessageQueue * providerManagerServiceQueue =  
        MessageQueue::lookup(PEGASUS_QUEUENAME_PROVIDERMANAGER_CPP);

    _providerManagerService =
       dynamic_cast<ProviderManagerService *> (providerManagerServiceQueue);

    //
    // get an instance of the ProviderManager
    //
    _providerManager = _providerManagerService->getProviderManager();

    //
    // set CIMServer state to TERMINATING
    //
    _cimserver->setState(CIMServerState::TERMINATING);

    //
    // Tell the CIMServer to stop accepting new client connection requests.
    //
    _cimserver->stopClientConnection();

    //
    // get shutdown timeout values
    //
    _initTimeoutValues(timeout);

    //
    // Determine if there are any outstanding CIM operation requests
    // (take into account that one of the request is the shutdown request).
    //
    // Loop and wait one second until either there is no more requests
    // or until timeout expires.
    //
    Uint32 maxWaitTime = _operationTimeout; // maximum wait time in milliseconds
    Uint32 waitTime = 1000;                 // one second wait interval

    Uint32 requestCount = _cimserver->getOutstandingRequestCount();

    while ( requestCount > 1 && maxWaitTime > 0)
    {
         System::sleep(waitTime);
         requestCount = _cimserver->getOutstandingRequestCount();
         maxWaitTime = maxWaitTime - waitTime;
    }

    if (requestCount == 1)
    {
        noMoreRequests = true;
    }

    //
    // If no more requests or force shutdown option is specified, proceed
    // to shutdown the CIMServer
    //
    if ( noMoreRequests || force )
    {
        _shutdownCIMServer();
    }
    else
    {
        _resumeCIMServer();
    }

    //
    // All done
    //
    return;
}

/**********************************************************/
/*  private methods                                       */
/**********************************************************/

void ShutdownService::_initTimeoutValues(Uint32 timeout)
{
    //
    // get an instance of the ConfigManager
    //
    ConfigManager*  configManager;
    configManager = ConfigManager::getInstance();

    //
    // if timeout was not specified, get timeout value from ConfigManager
    //
    if (timeout > 0)
    {
        _operationTimeout = timeout;
    }
    else
    {
        String configTimeout= configManager->getCurrentValue(TIMEOUT_PROPERTY);
        ArrayDestroyer<char> timeoutCString(configTimeout.allocateCString());
        _operationTimeout = strtol(timeoutCString.getPointer(), (char **)0,10);
    }

    //
    // get shutdown timeout value from ConfigManager
    //
    String shutdownTimeout= configManager->getCurrentValue(SHUTDOWN_TIMEOUT_PROPERTY);
    ArrayDestroyer<char> shutdownTimeoutCString(shutdownTimeout.allocateCString());
    _shutdownTimeout = strtol(shutdownTimeoutCString.getPointer(), (char **)0, 10);

    return;
}

void ShutdownService::_shutdownCIMServer()
{
/*
    //
    // Shutdown the Indication Subscription Service
    //
    _shutdownSubscriptionService();
*/

    // ATTN: Shutdown the Indication Handlers?

    // ATTN: Shutdown the Indication Processing Service?

    //
    // Shutdown the providers
    //
    _shutdownProviders();

    //
    // Tell CIMServer to shutdown completely.
    //
    _cimserver->shutdown();

    return;
}

void ShutdownService::_resumeCIMServer()
{
    //
    // resume CIMServer
    //
    try
    {
        //_cimserver->resume();
    }
    catch (Exception& e)
    {
        throw UnableToResumeServerException();
    }

    //
    // reset CIMServer state to RUNNING
    //
    //_cimserver->setState(CIMServerState::RUNNING);

    //
    // now inform the client that CIM server has resumed.
    //
    throw ServerResumedException();
}

void ShutdownService::_shutdownSubscriptionService()
{
    String subscriptionProviderName = PEGASUS_CLASSNAME_INDSUBSCRIPTION;

    //
    // find the Subscription Service provider and shut it down
    //
    //_providerManager->stopProvider(subscriptionProviderName);

    return;
}

void ShutdownService::_shutdownProviders()
{
    String shutdownProviderName = "ShutdownProvider";

    //
    // Terminate all the providers (except the ShutdownProvider!)
    //
    // ATTN:  Need to make use of the provider shutdown timeout
    //        when asyn provider API is supported.
    //
    _providerManager->shutdownAllProviders(shutdownProviderName,
                                           PEGASUS_CLASSNAME_SHUTDOWN);

    return;
}

PEGASUS_NAMESPACE_END
