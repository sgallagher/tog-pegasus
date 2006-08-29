//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
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
//%/////////////////////////////////////////////////////////////////////////////

#include "DefaultProviderManager.h"

#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Common/OperationContext.h>
#include <Pegasus/Common/Time.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/StatisticalData.h>
#include <Pegasus/Common/Logger.h>
#include <Pegasus/Common/MessageLoader.h>
#include <Pegasus/Common/PegasusVersion.h>
#include <Pegasus/Common/Constants.h>

#include <Pegasus/Query/QueryExpression/QueryExpression.h>
#include <Pegasus/ProviderManager2/QueryExpressionFactory.h>

#include <Pegasus/ProviderManager2/OperationResponseHandler.h>

PEGASUS_NAMESPACE_BEGIN

//
// Default Provider Manager
//
DefaultProviderManager::DefaultProviderManager()
{
    _subscriptionInitComplete = false;
}

DefaultProviderManager::~DefaultProviderManager()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "DefaultProviderManager::~DefaultProviderManager");

    _shutdownAllProviders();

    for (ProviderTable::Iterator i = _providers.start(); i != 0; i++)
    {
        ProviderMessageHandler* provider = i.value();
        delete provider;
    }

    for (ModuleTable::Iterator j = _modules.start(); j != 0; j++)
    {
        ProviderModule* module = j.value();
        delete module;
    }

    PEG_METHOD_EXIT();
}

Message* DefaultProviderManager::processMessage(Message* request)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "DefaultProviderManager::processMessage()");

    Message* response = 0;

    // pass the request message to a handler method based on message type
    switch(request->getType())
    {
    case CIM_GET_INSTANCE_REQUEST_MESSAGE:
    case CIM_ENUMERATE_INSTANCES_REQUEST_MESSAGE:
    case CIM_ENUMERATE_INSTANCE_NAMES_REQUEST_MESSAGE:
    case CIM_CREATE_INSTANCE_REQUEST_MESSAGE:
    case CIM_MODIFY_INSTANCE_REQUEST_MESSAGE:
    case CIM_DELETE_INSTANCE_REQUEST_MESSAGE:
    case CIM_EXEC_QUERY_REQUEST_MESSAGE:
    case CIM_ASSOCIATORS_REQUEST_MESSAGE:
    case CIM_ASSOCIATOR_NAMES_REQUEST_MESSAGE:
    case CIM_REFERENCES_REQUEST_MESSAGE:
    case CIM_REFERENCE_NAMES_REQUEST_MESSAGE:
    case CIM_GET_PROPERTY_REQUEST_MESSAGE:
    case CIM_SET_PROPERTY_REQUEST_MESSAGE:
    case CIM_INVOKE_METHOD_REQUEST_MESSAGE:
    case CIM_CREATE_SUBSCRIPTION_REQUEST_MESSAGE:
    case CIM_MODIFY_SUBSCRIPTION_REQUEST_MESSAGE:
    case CIM_DELETE_SUBSCRIPTION_REQUEST_MESSAGE:
    case CIM_EXPORT_INDICATION_REQUEST_MESSAGE:
        {
            CIMRequestMessage* cimRequest =
                dynamic_cast<CIMRequestMessage*>(request);

            // resolve provider name
            ProviderName name = _resolveProviderName(
                cimRequest->operationContext.get(ProviderIdContainer::NAME));

            // get cached or load new provider module
            ProviderOperationCounter poc(
                _getProvider(name.getPhysicalName(), name.getLogicalName()));

            response = poc.GetProvider().processMessage(cimRequest);
            break;
        }

    case CIM_DISABLE_MODULE_REQUEST_MESSAGE:
        response = _handleDisableModuleRequest(request);
        break;

    case CIM_ENABLE_MODULE_REQUEST_MESSAGE:
        response = _handleEnableModuleRequest(request);
        break;

    case CIM_STOP_ALL_PROVIDERS_REQUEST_MESSAGE:
        response = _handleStopAllProvidersRequest(request);
        break;

    case CIM_INITIALIZE_PROVIDER_REQUEST_MESSAGE:
        response = _handleInitializeProviderRequest(request);
        break;

    case CIM_SUBSCRIPTION_INIT_COMPLETE_REQUEST_MESSAGE:
        response = _handleSubscriptionInitCompleteRequest(request);
        break;

    default:
        PEGASUS_ASSERT(0);
        break;
    }

    PEG_METHOD_EXIT();

    return response;
}

Message* DefaultProviderManager::_handleInitializeProviderRequest(
    Message* message)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, 
	"DefaultProviderManager::_handleInitializeProviderRequest");

    CIMInitializeProviderRequestMessage* request =
        dynamic_cast<CIMInitializeProviderRequestMessage*>(message);
    PEGASUS_ASSERT(request != 0);

    CIMInitializeProviderResponseMessage* response =
        dynamic_cast<CIMInitializeProviderResponseMessage*>(
            request->buildResponse());
    PEGASUS_ASSERT(response != 0);

    try
    {
        // resolve provider name
	ProviderName name = _resolveProviderName(
	    request->operationContext.get(ProviderIdContainer::NAME));

        // get cached or load new provider module
        ProviderOperationCounter poc(
            _getProvider(name.getPhysicalName(), name.getLogicalName()));
    }
    catch (CIMException& e)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL3,
            "CIMException: " + e.getMessage());

        response->cimException = PEGASUS_CIM_EXCEPTION_LANG(
            e.getContentLanguages(), e.getCode(), e.getMessage());
    }
    catch (Exception& e)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL3,
            "Exception: " + e.getMessage());

        response->cimException = PEGASUS_CIM_EXCEPTION_LANG(
            e.getContentLanguages(), CIM_ERR_FAILED, e.getMessage());
    }
    catch (...)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL3,
            "Exception: Unknown");

        response->cimException = PEGASUS_CIM_EXCEPTION(
            CIM_ERR_FAILED, "Unknown error.");
    }

    PEG_METHOD_EXIT();
    return response;
}

Message* DefaultProviderManager::_handleDisableModuleRequest(Message* message)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "DefaultProviderManager::_handleDisableModuleRequest");

    CIMDisableModuleRequestMessage* request =
        dynamic_cast<CIMDisableModuleRequestMessage*>(message);
    PEGASUS_ASSERT(request != 0);

    Array<Uint16> operationalStatus;
    CIMException cimException;

    try
    {
        // get provider module name
        String moduleName;
        CIMInstance mInstance = request->providerModule;
        Uint32 pos = mInstance.findProperty(CIMName("Name"));
        PEGASUS_ASSERT(pos != PEG_NOT_FOUND);
        mInstance.getProperty(pos).getValue().get(moduleName);

        //
        // Unload providers
        //
        Array<CIMInstance> providerInstances = request->providers;

        String physicalName = _resolvePhysicalName(
            mInstance.getProperty(
                mInstance.findProperty("Location")).getValue().toString());

        for (Uint32 i = 0, n = providerInstances.size(); i < n; i++)
        {
            String pName;
            providerInstances[i].getProperty(
                providerInstances[i].findProperty("Name")).
                    getValue().get(pName);

            Sint16 ret_value = _disableProvider(pName);

            if (ret_value == 0)
            {
                // disable failed since there are pending requests,
                // stop trying to disable other providers in this module.
                operationalStatus.append(CIM_MSE_OPSTATUS_VALUE_OK);
                break;
            }
            else if (ret_value != 1)  // Not success
            {
                // disable failed for other reason, throw exception
                throw PEGASUS_CIM_EXCEPTION_L(
                    CIM_ERR_FAILED,
                    MessageLoaderParms(
                        "ProviderManager.ProviderManagerService."
                            "DISABLE_PROVIDER_FAILED",
                        "Failed to disable the provider."));
            }
        }
    }
    catch (CIMException & e)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                         "Exception: " + e.getMessage());
        cimException = e;
    }
    catch (Exception & e)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Exception: " + e.getMessage());
        cimException = CIMException(CIM_ERR_FAILED, e.getMessage());
    }
    catch (...)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Exception: Unknown");
        cimException = PEGASUS_CIM_EXCEPTION_L(
            CIM_ERR_FAILED,
            MessageLoaderParms(
                "ProviderManager.ProviderManagerService.UNKNOWN_ERROR",
                "Unknown Error"));
    }

    if (cimException.getCode() == CIM_ERR_SUCCESS)
    {
        // Status is set to OK if a provider was busy
        if (operationalStatus.size() == 0)
        {
            operationalStatus.append(CIM_MSE_OPSTATUS_VALUE_STOPPED);
        }
    }
    else
    {
        // If exception occurs, module is not stopped
        operationalStatus.append(CIM_MSE_OPSTATUS_VALUE_OK);
    }

    CIMDisableModuleResponseMessage* response =
        dynamic_cast<CIMDisableModuleResponseMessage*>(
            request->buildResponse());
    PEGASUS_ASSERT(response != 0);

    response->operationalStatus = operationalStatus;

    PEG_METHOD_EXIT();

    return response;
}

Message* DefaultProviderManager::_handleEnableModuleRequest(Message* message)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "DefaultProviderManager::_handleEnableModuleRequest");

    CIMEnableModuleRequestMessage* request =
        dynamic_cast<CIMEnableModuleRequestMessage*>(message);
    PEGASUS_ASSERT(request != 0);

    Array<Uint16> operationalStatus;
    operationalStatus.append(CIM_MSE_OPSTATUS_VALUE_OK);

    CIMEnableModuleResponseMessage* response =
        dynamic_cast<CIMEnableModuleResponseMessage*>(
            request->buildResponse());
    PEGASUS_ASSERT(response != 0);

    response->operationalStatus = operationalStatus;

    PEG_METHOD_EXIT();
    return response;
}

Message* DefaultProviderManager::_handleStopAllProvidersRequest(
    Message* message)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "DefaultProviderManager::_handleStopAllProvidersRequest");

    CIMStopAllProvidersRequestMessage* request =
        dynamic_cast<CIMStopAllProvidersRequestMessage*>(message);
    PEGASUS_ASSERT(request != 0);

    CIMStopAllProvidersResponseMessage* response =
        dynamic_cast<CIMStopAllProvidersResponseMessage*>(
            request->buildResponse());
    PEGASUS_ASSERT(response != 0);

    // tell the provider manager to shutdown all the providers
    _shutdownAllProviders();

    PEG_METHOD_EXIT();
    return response;
}

Message* DefaultProviderManager::_handleSubscriptionInitCompleteRequest(
    Message* message)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, 
        "DefaultProviderManager::_handleSubscriptionInitCompleteRequest");

    CIMSubscriptionInitCompleteRequestMessage* request =
        dynamic_cast<CIMSubscriptionInitCompleteRequestMessage*>(message);
    PEGASUS_ASSERT(request != 0);

    CIMSubscriptionInitCompleteResponseMessage* response =
        dynamic_cast<CIMSubscriptionInitCompleteResponseMessage*>(
            request->buildResponse());
    PEGASUS_ASSERT(response != 0);

    _subscriptionInitComplete = true;

    // Make a copy of the table so it is not locked during the provider calls
    Array<ProviderMessageHandler*> providerList;
    {
        AutoMutex lock(_providerTableMutex);

        for (ProviderTable::Iterator i = _providers.start(); i != 0; i++)
        {
            providerList.append(i.value());
        }
    }

    //
    // Notify all providers that subscription initialization is complete
    //
    for (Uint32 j = 0; j < providerList.size(); j++)
    {
        providerList[j]->subscriptionInitComplete();
    }

    PEG_METHOD_EXIT();
    return response;
}

ProviderName DefaultProviderManager::_resolveProviderName(
    const ProviderIdContainer & providerId)
{
    String providerName;
    String fileName;
    String interfaceName;
    CIMValue genericValue;

    genericValue = providerId.getProvider().getProperty(
        providerId.getProvider().findProperty("Name")).getValue();
    genericValue.get(providerName);

    genericValue = providerId.getModule().getProperty(
        providerId.getModule().findProperty("Location")).getValue();
    genericValue.get(fileName);
    fileName = _resolvePhysicalName(fileName);

    // ATTN: This attribute is probably not required
    genericValue = providerId.getModule().getProperty(
        providerId.getModule().findProperty("InterfaceType")).getValue();
    genericValue.get(interfaceName);

    return ProviderName(providerName, fileName, interfaceName, 0);
}

ProviderOperationCounter DefaultProviderManager::_getProvider(
    const String& moduleFileName,
    const String& providerName)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "DefaultProviderManager::_getProvider");

    ProviderMessageHandler* pr = _lookupProvider(providerName);

    if (!pr->status.isInitialized())
    {
        _initProvider(pr, moduleFileName);

        if (!pr->status.isInitialized())
        {
            PEG_METHOD_EXIT();
            throw PEGASUS_CIM_EXCEPTION(
                CIM_ERR_FAILED, "provider initialization failed");
        }
    }

    ProviderOperationCounter poc(pr);
    poc.GetProvider().status.update_idle_timer();

    PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
        "Returning Provider " + providerName);

    PEG_METHOD_EXIT();
    return poc;
}

ProviderMessageHandler* DefaultProviderManager::_lookupProvider(
    const String& providerName)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "DefaultProviderManager::_lookupProvider");

    // lock the providerTable mutex
    AutoMutex lock(_providerTableMutex);

    // look up provider in cache
    ProviderMessageHandler* pr = 0;
    if (_providers.lookup(providerName, pr))
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Found Provider " + providerName + " in Provider Manager Cache");
    }
    else
    {
        // create provider
        pr = new ProviderMessageHandler(
            providerName, 0, _indicationCallback, _responseChunkCallback,
            _subscriptionInitComplete);

        // insert provider in provider table
        _providers.insert(providerName, pr);

        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Created provider " + pr->getName());
    }

    PEG_METHOD_EXIT();
    return pr;
}

ProviderMessageHandler* DefaultProviderManager::_initProvider(
    ProviderMessageHandler* provider,
    const String& moduleFileName)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "DefaultProviderManager::_initProvider");

    ProviderModule* module = 0;
    CIMProvider* base;

    {
        // lock the providerTable mutex
        AutoMutex lock(_providerTableMutex);

        // lookup provider module
        module = _lookupModule(moduleFileName);
    }   // unlock the providerTable mutex

    {
        // lock the provider status mutex
        AutoMutex lock(provider->status._statusMutex);

        if (provider->status.isInitialized())
        {
            // Initialization is already complete
            return provider;
        }

        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Loading/Linking Provider Module " + moduleFileName);

        // load the provider
        try
        {
            base = module->load(provider->getName());
        }
        catch (...)
        {
            PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                "Exception caught Loading/Linking Provider Module " +
                moduleFileName);
            PEG_METHOD_EXIT();
            throw;
        }

        // initialize the provider
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL2,
            "Initializing Provider " + provider->getName());

        CIMOMHandle *cimomHandle = new CIMOMHandle();
        provider->status.setCIMOMHandle(cimomHandle);
        provider->status.setModule(module);
        provider->status._quantum=0;
        provider->setProvider(base);

        Boolean initializeError = false;

        try
        {
            provider->initialize(*(provider->status._cimom_handle));
        }
        catch (...)
        {
            initializeError = true;
        }

        provider->status.setInitialized(!initializeError);
        provider->status._currentOperations = 0;

        // The cleanup code executed when an exception occurs was previously
        // included in the catch block above. Unloading the provider module
        // from inside the catch block resulted in a crash when an exception
        // was thrown from a provider's initialize() method. The issue is that
        // when an exception is thrown, the program maintains internal
        // pointers related to the code that threw the exception. In the case
        // of an exception thrown from a provider during the initialize()
        // method, those pointers point into the provider library, so when
        // the LocalProviderManager unloads the library, the pointers into the
        // library that the program was holding are invalid.
        if (initializeError == true)
        {
            // delete the cimom handle
            delete provider->status._cimom_handle;

            // set provider status to uninitialized
            provider->status.reset();

            provider->setProvider(0);

            // unload provider module
            module->unloadModule();
        }
    }   // unlock the provider status mutex

    PEG_METHOD_EXIT();
    return provider;
}

ProviderModule* DefaultProviderManager::_lookupModule(
    const String& moduleFileName)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "DefaultProviderManager::_lookupModule");

    // look up provider module in cache
    ProviderModule* module = 0;

    if (_modules.lookup(moduleFileName, module))
    {
        // found provider module in cache
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Found Provider Module " + moduleFileName +
            " in Provider Manager Cache");
    }
    else
    {
        // provider module not found in cache, create provider module
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Creating Provider Module " + moduleFileName);

        module = new ProviderModule(moduleFileName);

        // insert provider module in module table
        _modules.insert(moduleFileName, module);
    }

    PEG_METHOD_EXIT();
    return module;
}

Boolean DefaultProviderManager::hasActiveProviders()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "DefaultProviderManager::hasActiveProviders");

    try
    {
        AutoMutex lock(_providerTableMutex);
        Tracer::trace(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Number of providers in _providers table = %d", _providers.size());

        // Iterate through the _providers table looking for an active provider
        for (ProviderTable::Iterator i = _providers.start(); i != 0; i++)
        {
            if (i.value()->status.isInitialized())
            {
                PEG_METHOD_EXIT();
                return true;
            }
        }
    }
    catch (...)
    {
        // Unexpected exception; do not assume that no providers are loaded
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL2,
            "Unexpected Exception in hasActiveProviders.");
        PEG_METHOD_EXIT();
        return true;
    }

    // No active providers were found in the _providers table
    PEG_METHOD_EXIT();
    return false;
}

void DefaultProviderManager::unloadIdleProviders()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "LocalProviderManager::unloadIdleProviders");

    static Uint32 quantum = 0;
    static struct timeval first = {0,0}, now, last = {0,0};

    if (first.tv_sec == 0)
    {
        Time::gettimeofday(&first);
    }
    Time::gettimeofday(&now);

    if (!(((now.tv_sec - first.tv_sec) > IDLE_LIMIT) &&
          ((now.tv_sec - last.tv_sec) > IDLE_LIMIT)))
    {
        // It's not time yet to check for idle providers
        PEG_METHOD_EXIT();
        return;
    }

    Time::gettimeofday(&last);

    Time::gettimeofday(&last);
    PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
        "Checking for Idle providers to unload.");

    try
    {
        AutoMutex lock(_providerTableMutex);

        quantum++;

        struct timeval now;
        Time::gettimeofday(&now);

        for (ProviderTable::Iterator i = _providers.start(); i != 0 ; i++)
        {
            ProviderMessageHandler* provider = i.value();
            PEGASUS_ASSERT(provider != 0);

            if (!provider->status.isInitialized())
            {
                continue;
            }

            if (provider->status._quantum == quantum)
            {
                continue;
            }

            provider->status._quantum = quantum;

            if (provider->status._currentOperations.get())
            {
                PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                    "Provider has pending operations: " +
                    provider->getName());
                continue;
            }

            PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                "Checking timeout data for Provider: " +
                provider->getName());
            struct timeval timeout = {0,0};
            provider->status.get_idle_timer(&timeout);

            PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                "provider->status.unload_ok() returns: " +
                provider->status.unload_ok() ? "true" : "false");

            if (provider->status.unload_ok() &&
                ((now.tv_sec - timeout.tv_sec) > ((Sint32)IDLE_LIMIT)))
            {
                AutoMutex pr_lock(provider->status._statusMutex);
                Logger::put(
                    Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
                    "LocalProviderManager::unloadIdleProviders - "
                        "Unload idle provider $0", provider->getName());

                PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                    "Trying to Terminate Provider " + provider->getName());
                try
                {
                    if (!provider->status.unload_ok())
                    {
                        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                            "Provider not OK to unload: " +
                                provider->getName());
                        continue;
                    }

                    provider->terminate();

                    PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                        "Provider terminated: " + provider->getName());
                    provider->status.setInitialized(false);
                }
                catch (...)
                {
                    PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                        "Exception terminating " + provider->getName());
                    i = _providers.start();
                    continue;
                }

                PEGASUS_ASSERT(provider->status._module != 0);
                PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                    "unloading Provider module" + provider->getName());
                provider->status._module->unloadModule();

                PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                    "Destroying Provider's CIMOM Handle: " +
                    provider->getName());
                delete provider->status._cimom_handle;

                provider->status.reset();
            }
        }
    }
    catch (...)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL2,
            "Caught unexpected exception in unloadIdleProviders.");
    }

    PEG_METHOD_EXIT();
}

void DefaultProviderManager::_shutdownAllProviders()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "DefaultProviderManager::_shutdownAllProviders");

    try
    {
        AutoMutex lock(_providerTableMutex);

        Tracer::trace(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "providers in cache = %d", _providers.size());

        for (ProviderTable::Iterator i = _providers.start(); i != 0; i++)
        {
            ProviderMessageHandler* provider = i.value();
            PEGASUS_ASSERT(provider != 0);

            if (provider->status.isInitialized())
            {
                _unloadProvider(provider);
            }
        }
    }
    catch (...)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Unexpected Exception in _shutdownAllProviders().");
    }

    PEG_METHOD_EXIT();
}

Sint16 DefaultProviderManager::_disableProvider(const String& providerName)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "DefaultProviderManager::_disableProvider");

    ProviderMessageHandler* pr = _lookupProvider(providerName);
    if (!pr->status.isInitialized())
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL2,
            "Provider " + providerName + " is not loaded");
        PEG_METHOD_EXIT();
        return 1;
    }

    PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
        "Disable Provider " + pr->getName());
    //
    // Check to see if there are pending requests. If there are pending
    // requests and the disable timeout has not expired, loop and wait one
    // second until either there is no pending requests or until timeout
    // expires.
    //
    Uint32 waitTime = PROVIDER_DISABLE_TIMEOUT;
    while ((pr->status._currentOperations.get() > 0) && (waitTime > 0))
    {
        Threads::sleep(1000);
        waitTime = waitTime - 1;
    }

    // There are still pending requests, do not disable
    if (pr->status._currentOperations.get() > 0)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Disable failed since there are pending requests.");
        PEG_METHOD_EXIT();
        return 0;
    }

    try
    {
        AutoMutex lock(_providerTableMutex);

        if (pr->status.isInitialized())
        {
            PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                "Unloading Provider " + pr->getName());
            _unloadProvider(pr);
        }
    }
    catch (...)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Unload provider failed " + pr->getName());
        PEG_METHOD_EXIT();
        return -1;
    }

    PEG_METHOD_EXIT();
    return 1;
}

void DefaultProviderManager::_unloadProvider(ProviderMessageHandler* provider)
{
    //
    // NOTE:  It is the caller's responsibility to make sure that
    // the ProviderTable mutex is locked before calling this method.
    //
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "DefaultProviderManager::_unloadProvider");

    PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
        "Unloading Provider " + provider->getName());

    if (provider->status._currentOperations.get() > 0)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Provider cannot be unloaded due to pending operations: " +
            provider->getName());
    }
    else
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Terminating Provider " + provider->getName());

        // lock the provider mutex
        AutoMutex pr_lock(provider->status._statusMutex);

        try
        {
            provider->terminate();
        }
        catch (...)
        {
            PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL3,
                "Error occured terminating provider " + provider->getName());
        }
        provider->status.setInitialized(false);

        PEGASUS_ASSERT(provider->status._module != 0);

        // unload provider module
        provider->status._module->unloadModule();

        // NOTE: The "delete provider->status._cimom_handle" operation was
        //   moved to be called after the unloadModule() call above
        //   as part of a fix for bugzilla 3669. For some providers
        //   run out-of-process on Windows platforms (i.e. running
        //   the cimserver with the forceProviderProcesses config option
        //   set to "true"), deleting the provider's CIMOMHandle before
        //   unloading the provider library caused the unload mechanism
        //   to deadlock, making that provider unavailable and preventing
        //   the cimserver from shutting down. It should NOT be moved back
        //   above the unloadModule() call. See bugzilla 3669 for details.
        // delete the cimom handle
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Destroying Provider's CIMOM Handle " + provider->getName());
        delete provider->status._cimom_handle;

        Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            "ProviderManager::_provider_crtl -  Unload provider $0",
            provider->getName());

        // set provider status to uninitialized
        provider->status.reset();
    }

    PEG_METHOD_EXIT();
}

ProviderManager* DefaultProviderManager::createDefaultProviderManagerCallback()
{
    return new DefaultProviderManager();
}

PEGASUS_NAMESPACE_END
