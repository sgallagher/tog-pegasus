//%2005////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
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
// Author: Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//         Jenny Yu, Hewlett-Packard Company (jenny_yu@hp.com)
//
// Modified By: Carol Ann Krug Graves, Hewlett-Packard Company
//                  (carolann_graves@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/AutoPtr.h>
#include <Pegasus/Common/CIMMessageSerializer.h>
#include <Pegasus/Common/CIMMessageDeserializer.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Config/ConfigManager.h>

#include "ProviderAgent.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

/////////////////////////////////////////////////////////////////////////////
//
// ProviderAgentRequest
//
/////////////////////////////////////////////////////////////////////////////

/**
    This class encapsulates the data required by a work thread to process a
    request in a Provider Agent.
 */
class ProviderAgentRequest
{
public:
    ProviderAgentRequest(ProviderAgent* agent_, CIMRequestMessage* request_)
    {
        agent = agent_;
        request = request_;
        request->requestIsOOP = true;
    }

    ProviderAgent* agent;
    CIMRequestMessage* request;
};


/////////////////////////////////////////////////////////////////////////////
//
// ProviderAgent
//
/////////////////////////////////////////////////////////////////////////////

// Time values used in ThreadPool construction
static struct timeval deallocateWait = {300, 0};

ProviderAgent* ProviderAgent::_providerAgent = 0;

ProviderAgent::ProviderAgent(
    const String& agentId,
    AnonymousPipe* pipeFromServer,
    AnonymousPipe* pipeToServer)
  : _threadPool(0, "ProviderAgent", 0, 0, deallocateWait),
    _providerManagerRouter(_indicationCallback)
{
    PEG_METHOD_ENTER(TRC_PROVIDERAGENT, "ProviderAgent::ProviderAgent");

    _terminating = false;
    _agentId = agentId;
    _pipeFromServer = pipeFromServer;
    _pipeToServer = pipeToServer;
    _providerAgent = this;
    _subscriptionInitComplete = false;

    PEG_METHOD_EXIT();
}

ProviderAgent::~ProviderAgent()
{
    PEG_METHOD_ENTER(TRC_PROVIDERAGENT, "ProviderAgent::~ProviderAgent");

    _providerAgent = 0;

    PEG_METHOD_EXIT();
}

void ProviderAgent::run()
{
    PEG_METHOD_ENTER(TRC_PROVIDERAGENT, "ProviderAgent::run");

    // Enable the signal handler to terminate gracefully on SIGHUP and SIGTERM
    getSigHandle()->registerHandler(PEGASUS_SIGHUP, _terminateSignalHandler);
    getSigHandle()->activate(PEGASUS_SIGHUP);
    getSigHandle()->registerHandler(PEGASUS_SIGTERM, _terminateSignalHandler);
    getSigHandle()->activate(PEGASUS_SIGTERM);

    while (!_terminating)
    {
        Boolean active = true;
        try
        {
            //
            // Read and process the next request
            //
            active = _readAndProcessRequest();
        }
        catch (Exception& e)
        {
            PEG_TRACE_STRING(TRC_PROVIDERAGENT, Tracer::LEVEL2,
                String("Unexpected exception from _readAndProcessRequest(): ") +
                    e.getMessage());
            _terminating = true;
        }
        catch (...)
        {
            PEG_TRACE_STRING(TRC_PROVIDERAGENT, Tracer::LEVEL2,
                "Unexpected exception from _readAndProcessRequest().");
            _terminating = true;
        }

        if (_terminating)
        {
            //
            // Stop all providers
            //
            CIMStopAllProvidersRequestMessage stopRequest("0", QueueIdStack(0));
            AutoPtr<Message> stopResponse(_processRequest(&stopRequest));
        }
        else if (!active)
        {
            //
            // Stop agent process when no more providers are loaded
            //
            try
            {
                if (!_providerManagerRouter.hasActiveProviders())
                {
                    PEG_TRACE_STRING(TRC_PROVIDERAGENT, Tracer::LEVEL2,
                        "No active providers.  Exiting.");
                    _terminating = true;
                }
                else
                {
                    _threadPool.cleanupIdleThreads();
                }
            }
            catch (...)
            {
                // Do not terminate the agent on this exception
                PEG_TRACE_STRING(TRC_PROVIDERAGENT, Tracer::LEVEL2,
                    "Unexpected exception from hasActiveProviders()");
            }
        }
    }

    PEG_METHOD_EXIT();
}

Boolean ProviderAgent::_readAndProcessRequest()
{
    PEG_METHOD_ENTER(TRC_PROVIDERAGENT,
        "ProviderAgent::_readAndProcessRequest");

    CIMRequestMessage* request;

    //
    // Read the request from CIM Server
    //
    CIMMessage* cimMessage;
    AnonymousPipe::Status readStatus = _pipeFromServer->readMessage(cimMessage);
    request = dynamic_cast<CIMRequestMessage*>(cimMessage);

    // Read operation was interrupted
    if (readStatus == AnonymousPipe::STATUS_INTERRUPT)
    {
        PEG_TRACE_STRING(TRC_PROVIDERAGENT, Tracer::LEVEL2,
            "Read operation was interrupted.");
        PEG_METHOD_EXIT();
        return false;
    }

    if (readStatus == AnonymousPipe::STATUS_CLOSED)
    {
        // The CIM Server connection is closed
        PEG_TRACE_STRING(TRC_PROVIDERAGENT, Tracer::LEVEL2,
            "CIMServer connection closed. Exiting.");
        _terminating = true;
        PEG_METHOD_EXIT();
        return false;
    }

    if (readStatus == AnonymousPipe::STATUS_ERROR)
    {
        PEG_TRACE_STRING(TRC_PROVIDERAGENT, Tracer::LEVEL2,
            "Error reading from pipe. Exiting.");
        Logger::put_l(Logger::ERROR_LOG, System::CIMSERVER, Logger::WARNING,
            "ProviderManager.ProviderAgent.ProviderAgent."
                "CIMSERVER_COMMUNICATION_FAILED",
            "cimprovagt \"$0\" communication with CIM Server failed.  Exiting.",
            _agentId);
        _terminating = true;
        PEG_METHOD_EXIT();
        return false;
    }

    // A "wake up" message means we should unload idle providers
    if (request == 0)
    {
        PEG_TRACE_STRING(TRC_PROVIDERAGENT, Tracer::LEVEL4,
            "Got a wake up message.");
        try
        {
            _unloadIdleProviders();
        }
        catch (...)
        {
            // Ignore exceptions from idle provider unloading
            PEG_TRACE_STRING(TRC_PROVIDERAGENT, Tracer::LEVEL2,
                "Ignoring exception from _unloadIdleProviders()");
        }
        PEG_METHOD_EXIT();
        return false;
    }

    PEG_TRACE_STRING(TRC_PROVIDERAGENT, Tracer::LEVEL3,
        String("Received request from server with messageId ") +
            request->messageId);

    // Get the ProviderIdContainer to complete the provider module instance
    // optimization.  If the provider module instance is blank (optimized
    // out), fill it in from our cache.  If it is not blank, update our
    // cache.  (See the _providerModuleCache member description.)
    try
    {
        ProviderIdContainer pidc = request->operationContext.get(
            ProviderIdContainer::NAME);
        if (pidc.getModule().isUninitialized())
        {
            // Provider module is optimized out.  Fill it in from the cache.
            request->operationContext.set(ProviderIdContainer(
                _providerModuleCache, pidc.getProvider(),
                pidc.isRemoteNameSpace(), pidc.getRemoteInfo()));
        }
        else
        {
            // Update the cache with the new provider module instance.
            _providerModuleCache = pidc.getModule();
        }
    }
    catch (...)
    {
        // No ProviderIdContainer to optimize
    }

    //
    // Check for messages to be handled by the Agent itself.
    //
    if (request->getType() == CIM_INITIALIZE_PROVIDER_AGENT_REQUEST_MESSAGE)
    {
        // Process the request in this thread
        AutoPtr<CIMInitializeProviderAgentRequestMessage> ipaRequest(
            dynamic_cast<CIMInitializeProviderAgentRequestMessage*>(request));
        PEGASUS_ASSERT(ipaRequest.get() != 0);

        ConfigManager* configManager = ConfigManager::getInstance();
        configManager->setPegasusHome(ipaRequest->pegasusHome);

        // Initialize the configuration properties
        for (Uint32 i = 0; i < ipaRequest->configProperties.size(); i++)
        {
            configManager->initCurrentValue(
                ipaRequest->configProperties[i].first,
                ipaRequest->configProperties[i].second);
        }

        // Set the default resource bundle directory for the MessageLoader
        MessageLoader::setPegasusMsgHome(ConfigManager::getHomedPath(
            configManager->getCurrentValue("messageDir")));

        // Set the log file directory
#if !defined(PEGASUS_USE_SYSLOGS)
        Logger::setHomeDirectory(ConfigManager::getHomedPath(
            configManager->getCurrentValue("logdir")));
#endif
        System::bindVerbose = ipaRequest->bindVerbose;

        //
        //  Set _subscriptionInitComplete from value in 
        //  InitializeProviderAgent request
        //
        _subscriptionInitComplete = ipaRequest->subscriptionInitComplete;
        _providerManagerRouter.setSubscriptionInitComplete 
            (_subscriptionInitComplete);

        PEG_TRACE_STRING(TRC_PROVIDERAGENT, Tracer::LEVEL2,
            "Processed the agent initialization message.");

        // Do not write a response for this request
    }
    else if (request->getType() == CIM_NOTIFY_CONFIG_CHANGE_REQUEST_MESSAGE)
    {
        // Process the request in this thread
        AutoPtr<CIMNotifyConfigChangeRequestMessage> notifyRequest(
            dynamic_cast<CIMNotifyConfigChangeRequestMessage*>(request));
        PEGASUS_ASSERT(notifyRequest.get() != 0);

        //
        // Update the ConfigManager with the new property value
        //
        ConfigManager* configManager = ConfigManager::getInstance();
        CIMException responseException;
        try
        {
            if (notifyRequest->currentValueModified)
            {
                configManager->updateCurrentValue(
                    notifyRequest->propertyName,
                    notifyRequest->newPropertyValue,
                    false);
            }
            else
            {
                configManager->updatePlannedValue(
                    notifyRequest->propertyName,
                    notifyRequest->newPropertyValue,
                    true);
            }
        }
        catch (Exception& e)
        {
            responseException = PEGASUS_CIM_EXCEPTION(
                CIM_ERR_FAILED, e.getMessage());
        }

        AutoPtr<CIMResponseMessage> response(notifyRequest->buildResponse());
        response->cimException = responseException;

        // Return response to CIM Server
        _writeResponse(response.get());
    }
    else if ((request->getType() == CIM_DISABLE_MODULE_REQUEST_MESSAGE) ||
             (request->getType() == CIM_STOP_ALL_PROVIDERS_REQUEST_MESSAGE))
    {
        // Process the request in this thread
        AutoPtr<Message> response(_processRequest(request));
        _writeResponse(response.get());

        CIMResponseMessage * respMsg =
            dynamic_cast<CIMResponseMessage*>(response.get());

        // If StopAllProviders, terminate the agent process.
        // If DisableModule not successful, leave agent process running.
        if ((respMsg->cimException.getCode() == CIM_ERR_SUCCESS) ||
            (request->getType() == CIM_STOP_ALL_PROVIDERS_REQUEST_MESSAGE))
        {
            // Operation is successful. End the agent process.
            _terminating = true;
        }

        delete request;
    }
    else if (request->getType () == 
             CIM_SUBSCRIPTION_INIT_COMPLETE_REQUEST_MESSAGE)
    {
        _subscriptionInitComplete = true;

        //
        // Process the request in this thread
        //
        AutoPtr <Message> response (_processRequest (request));
        _writeResponse (response.get ());

        //
        //  Note: the response does not contain interesting data
        //

        delete request;
    }
    else
    {
        // Start a new thread to process the request
        ProviderAgentRequest* agentRequest =
            new ProviderAgentRequest(this, request);
        ThreadStatus rtn = PEGASUS_THREAD_OK;

        while ((rtn = _threadPool.allocate_and_awaken(agentRequest,
                   ProviderAgent::_processRequestAndWriteResponse)) !=
               PEGASUS_THREAD_OK)
        {
            if (rtn == PEGASUS_THREAD_INSUFFICIENT_RESOURCES)
            {
                pegasus_yield();
            }
            else
            {
                Logger::put(
                    Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
                    "Not enough threads to process agent request.");
 
                Tracer::trace(TRC_PROVIDERAGENT, Tracer::LEVEL2,
                    "Could not allocate thread to process agent request.");

                AutoPtr<CIMResponseMessage> response(request->buildResponse());
                response->cimException = PEGASUS_CIM_EXCEPTION_L(
                    CIM_ERR_FAILED,
                    MessageLoaderParms(
                        "ProviderManager.ProviderAgent.ProviderAgent."
                            "THREAD_ALLOCATION_FAILED",
                        "Failed to allocate a thread in cimprovagt \"$0\".",
                        _agentId));

                // Return response to CIM Server
                _writeResponse(response.get());

                delete agentRequest;
                delete request;

                break;
            }
        } 
    }

    PEG_METHOD_EXIT();
    return true;
}

Message* ProviderAgent::_processRequest(CIMRequestMessage* request)
{
    PEG_METHOD_ENTER(TRC_PROVIDERAGENT, "ProviderAgent::_processRequest");

    Message* response = 0;

    try
    {
        // Forward the request to the ProviderManager
        response = _providerManagerRouter.processMessage(request);
    }
    catch (Exception& e)
    {
        PEG_TRACE_STRING(TRC_PROVIDERAGENT, Tracer::LEVEL2,
            String("Caught exception while processing request: ") +
                e.getMessage());
        CIMResponseMessage* cimResponse = request->buildResponse();
        cimResponse->cimException = PEGASUS_CIM_EXCEPTION(
            CIM_ERR_FAILED, e.getMessage());
        response = cimResponse;
    }
    catch (...)
    {
        PEG_TRACE_STRING(TRC_PROVIDERAGENT, Tracer::LEVEL2,
            "Caught exception while processing request.");
        CIMResponseMessage* cimResponse = request->buildResponse();
        cimResponse->cimException = PEGASUS_CIM_EXCEPTION(
            CIM_ERR_FAILED, String::EMPTY);
        response = cimResponse;
    }

    PEG_METHOD_EXIT();
    return response;
}

void ProviderAgent::_writeResponse(Message* message)
{
    PEG_METHOD_ENTER(TRC_PROVIDERAGENT, "ProviderAgent::_writeResponse");

    CIMMessage* response = dynamic_cast<CIMMessage*>(message);
    PEGASUS_ASSERT(response != 0);

    //
    // Write the response message to the pipe
    //
    try
    {
        // Use Mutex to prevent concurrent writes to the same pipe
        AutoMutex pipeLock(_pipeToServerMutex);

        AnonymousPipe::Status writeStatus =
            _pipeToServer->writeMessage(response);

        if (writeStatus != AnonymousPipe::STATUS_SUCCESS)
        {
            PEG_TRACE_STRING(TRC_PROVIDERAGENT, Tracer::LEVEL2,
                "Error writing response to pipe.");
            Logger::put_l(Logger::ERROR_LOG, System::CIMSERVER, Logger::WARNING,
                "ProviderManager.ProviderAgent.ProviderAgent."
                    "CIMSERVER_COMMUNICATION_FAILED",
                "cimprovagt \"$0\" communication with CIM Server failed.  "
                    "Exiting.",
                _agentId);
            _terminating = true;
        }
    }
    catch (...)
    {
        PEG_TRACE_STRING(TRC_PROVIDERAGENT, Tracer::LEVEL2,
            "Caught exception while writing response.");
        Logger::put_l(Logger::ERROR_LOG, System::CIMSERVER, Logger::WARNING,
            "ProviderManager.ProviderAgent.ProviderAgent."
                "CIMSERVER_COMMUNICATION_FAILED",
            "cimprovagt \"$0\" communication with CIM Server failed.  Exiting.",
            _agentId);
        _terminating = true;
    }

    PEG_METHOD_EXIT();
}

PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL
ProviderAgent::_processRequestAndWriteResponse(void* arg)
{
    PEG_METHOD_ENTER(TRC_PROVIDERAGENT,
        "ProviderAgent::_processRequestAndWriteResponse");

    AutoPtr<ProviderAgentRequest> agentRequest(
        reinterpret_cast<ProviderAgentRequest*>(arg));
    PEGASUS_ASSERT(agentRequest.get() != 0);

    try
    {
        // Get the ProviderAgent and request message from the argument
        ProviderAgent* agent = agentRequest->agent;
        AutoPtr<CIMRequestMessage> request(agentRequest->request);

        // Process the request
        AutoPtr<Message> response(agent->_processRequest(request.get()));

        // Write the response
        agent->_writeResponse(response.get());
    }
    catch (const Exception& e)
    {
        PEG_TRACE_STRING(TRC_DISCARDED_DATA, Tracer::LEVEL2,
            "Caught exception: \"" + e.getMessage() +
                "\".  Exiting _processRequestAndWriteResponse.");
    }
    catch (...)
    {
        PEG_TRACE_STRING(TRC_DISCARDED_DATA, Tracer::LEVEL2,
            "Caught unrecognized exception.  "
                "Exiting _processRequestAndWriteResponse.");
    }

    PEG_METHOD_EXIT();
    return(PEGASUS_THREAD_RETURN(0));
}

void ProviderAgent::_indicationCallback(
    CIMProcessIndicationRequestMessage* message)
{
    PEG_METHOD_ENTER(TRC_PROVIDERAGENT, "ProviderAgent::_indicationCallback");

    // Send request back to the server to process
    _providerAgent->_writeResponse(message);

    PEG_METHOD_EXIT();
}

void ProviderAgent::_unloadIdleProviders()
{
    PEG_METHOD_ENTER(TRC_PROVIDERAGENT, "ProviderAgent::_unloadIdleProviders");
    ThreadStatus rtn = PEGASUS_THREAD_OK;
    // Ensure that only one _unloadIdleProvidersHandler thread runs at a time
    _unloadIdleProvidersBusy++;
    if ((_unloadIdleProvidersBusy.value() == 1) &&
        ((rtn =_threadPool.allocate_and_awaken(
             (void*)this, ProviderAgent::_unloadIdleProvidersHandler))==PEGASUS_THREAD_OK))
    {
        // _unloadIdleProvidersBusy is decremented in
        // _unloadIdleProvidersHandler
    }
    else
    {
        // If we fail to allocate a thread, don't retry now.
        _unloadIdleProvidersBusy--;
    }
    if (rtn != PEGASUS_THREAD_OK) 
    {

         Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
             "Not enough threads to unload idle providers.");
 
         Tracer::trace(TRC_PROVIDERAGENT, Tracer::LEVEL2,
             "Could not allocate thread to unload idle providers.");
    }
    PEG_METHOD_EXIT();
}

PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL
ProviderAgent::_unloadIdleProvidersHandler(void* arg) throw()
{
    try
    {
        PEG_METHOD_ENTER(TRC_PROVIDERAGENT,
            "ProviderAgent::unloadIdleProvidersHandler");

        ProviderAgent* myself = reinterpret_cast<ProviderAgent*>(arg);

        try
        {
            myself->_providerManagerRouter.unloadIdleProviders();
        }
        catch (...)
        {
            // Ignore errors
            PEG_TRACE_STRING(TRC_PROVIDERAGENT, Tracer::LEVEL2,
                "Unexpected exception in _unloadIdleProvidersHandler");
        }

        myself->_unloadIdleProvidersBusy--;
    }
    catch (...)
    {
        // Ignore errors
        try
        {
            PEG_TRACE_STRING(TRC_PROVIDERAGENT, Tracer::LEVEL2,
                "Unexpected exception in _unloadIdleProvidersHandler");
        }
        catch (...)
        {
        }
    }

    // PEG_METHOD_EXIT();    // Note: This statement could throw an exception
    return(PEGASUS_THREAD_RETURN(0));
}

void ProviderAgent::_terminateSignalHandler(
    int s_n, PEGASUS_SIGINFO_T* s_info, void* sig)
{
    PEG_METHOD_ENTER(TRC_PROVIDERAGENT,
        "ProviderAgent::_terminateSignalHandler");

    if (_providerAgent != 0)
    {
        _providerAgent->_terminating = true;
    }

    PEG_METHOD_EXIT();
}

PEGASUS_NAMESPACE_END
