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
// Author: Chip Vincent (cvincent@us.ibm.com)
//
// Modified By: Yi Zhou, Hewlett-Packard Company(yi_zhou@hp.com)
//              Mike Day IBM Corporation (mdday@us.ibm.com)
//              Adrian Schuur, schuur@de.ibm.com
//              Dan Gorey, IBM djgorey@us.ibm.com
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                  (carolann_graves@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include "LocalProviderManager.h"

#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/MessageQueueService.h>
#include <Pegasus/Common/PegasusVersion.h>
#include <Pegasus/ProviderManager2/ProviderManagerService.h>

PEGASUS_NAMESPACE_BEGIN

LocalProviderManager::LocalProviderManager()
    : _idle_timeout(IDLE_LIMIT)
{
}

LocalProviderManager::~LocalProviderManager()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "LocalProviderManager::~LocalProviderManager");

    shutdownAllProviders();

    for (ProviderTable::Iterator i = _providers.start(); i != 0; i++)
    {
        Provider* provider = i.value();
        delete provider;
    }

    for (ModuleTable::Iterator j = _modules.start(); j != 0; j++)
    {
        ProviderModule* module = j.value();
        delete module;
    }

    PEG_METHOD_EXIT();
}

OpProviderHolder LocalProviderManager::getProvider(
    const String& moduleFileName,
    const String& providerName)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "LocalProviderManager::getProvider");

    OpProviderHolder ph;
    Provider* pr = _lookupProvider(providerName);

    if (pr->getStatus() != Provider::INITIALIZED)
    {
        _initProvider(pr, moduleFileName);

        if (pr->getStatus() != Provider::INITIALIZED)
        {
            PEG_METHOD_EXIT();
            throw PEGASUS_CIM_EXCEPTION(
                CIM_ERR_FAILED, "provider initialization failed");
        }
    }

    ph.SetProvider(pr);
    ph.GetProvider().update_idle_timer();

    PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
        "Returning Provider " + providerName);

    PEG_METHOD_EXIT();
    return(ph);
}

void LocalProviderManager::unloadProvider(
    const String& providerName)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "LocalProviderManager::unloadProvider");

    Provider* pr = _lookupProvider(providerName);
    if (pr->getStatus() == Provider::INITIALIZED)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Unloading Provider " + pr->_name);

        AutoMutex lock(_providerTableMutex);
        _unloadProvider(pr);
    }

    PEG_METHOD_EXIT();
}

void LocalProviderManager::shutdownAllProviders()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "LocalProviderManager::shutdownAllProviders");

    try
    {
        AutoMutex lock(_providerTableMutex);

        Tracer::trace(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "providers in cache = %d", _providers.size());

        for (ProviderTable::Iterator i = _providers.start(); i != 0; i++)
        {
            Provider* provider = i.value();
            PEGASUS_ASSERT(provider != 0);

            if (provider->getStatus() == Provider::INITIALIZED)
            {
                _unloadProvider(provider);
            }
        }
    }
    catch(...)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Unexpected Exception in shutdownAllProviders().");
    }

    PEG_METHOD_EXIT();
}


Boolean LocalProviderManager::hasActiveProviders()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "LocalProviderManager::hasActiveProviders");

    try
    {
        AutoMutex lock(_providerTableMutex);
        Tracer::trace(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "providers in _providers table = %d", _providers.size());

        // Iterate through the _providers table looking for an active provider
        for (ProviderTable::Iterator i = _providers.start(); i != 0; i++)
        {
            if (i.value()->getStatus() == Provider::INITIALIZED)
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


void LocalProviderManager::unloadIdleProviders()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "LocalProviderManager::unloadIdleProviders");

    static Uint32 quantum = 0;
    static struct timeval first = {0,0}, now, last = {0,0};

    if (first.tv_sec == 0)
    {
        gettimeofday(&first, NULL);
    }
    gettimeofday(&now, NULL);

    if (!(((now.tv_sec - first.tv_sec) > IDLE_LIMIT) &&
          ((now.tv_sec - last.tv_sec) > IDLE_LIMIT)))
    {
        // It's not time yet to check for idle providers
        PEG_METHOD_EXIT();
        return;
    }

    gettimeofday(&last, NULL);
    PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
        "Checking for Idle providers to unload.");

    try
    {
        AutoMutex lock(_providerTableMutex);

        quantum++;

        struct timeval now;
        gettimeofday(&now, NULL);

        for (ProviderTable::Iterator i = _providers.start(); i != 0 ; i++)
        {
            Provider* provider = i.value();
            PEGASUS_ASSERT(provider != 0);

            if (provider->getStatus() == Provider::UNINITIALIZED)
            {
                continue;
            }

            if (provider->_quantum == quantum)
            {
                continue;
            }

            provider->_quantum = quantum;

            if (provider->_current_operations.value())
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
            provider->get_idle_timer(&timeout);

            PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                "provider->unload_ok() returns: " +
                provider->unload_ok() ? "true" : "false");

            if (provider->unload_ok() &&
                ((now.tv_sec - timeout.tv_sec) > ((Sint32)_idle_timeout)))
            {
                AutoMutex pr_lock(provider->_statusMutex);
                Logger::put(
                    Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
                    "LocalProviderManager::unloadIdleProviders - "
                        "Unload idle provider $0", provider->getName());

                PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                    "Trying to Terminate Provider " + provider->getName());
                try
                {
                    if (!provider->tryTerminate())
                    {
                        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                            "Provider Refused Termination " +
                                provider->getName());
                        continue;
                    }
                    else
                    {
                        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                            "Provider terminated: " + provider->getName());
                    }
                }
                catch(...)
                {
                    PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                        "Exception terminating " + provider->getName());
                    i = _providers.start();
                    continue;
                }

                PEGASUS_ASSERT(provider->_module != 0);
                PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                    "unloading Provider module" + provider->getName());
                provider->_module->unloadModule();

                PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                    "Destroying Provider's CIMOM Handle: " +
                    provider->getName());
                delete provider->_cimom_handle;
                provider->reset();
            }
        }
    }
    catch(...)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL2,
            "Caught unexpected exception in unloadIdleProviders.");
    }

    PEG_METHOD_EXIT();
}

Sint16 LocalProviderManager::disableProvider(const String& providerName)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "LocalProviderManager::disableProvider");

    Provider* pr=_lookupProvider(providerName);
    if (pr->getStatus() == Provider::INITIALIZED)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Disable Provider " + pr->_name);
        //
        // Check to see if there are pending requests. If there are pending
        // requests and the disable timeout has not expired, loop and wait one
        // second until either there is no pending requests or until timeout
        // expires.
        //
        Uint32 waitTime = PROVIDER_DISABLE_TIMEOUT;
        while ((pr->_current_operations.value() > 0) && (waitTime > 0))
        {
            System::sleep(1);
            waitTime = waitTime - 1;
        }

        // There are still pending requests, do not disable
        if (pr->_current_operations.value() > 0)
        {
            PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                "Disable failed since there are pending requests.");
            PEG_METHOD_EXIT();
            return(0);
        }
    }
    else
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL2,
            "Provider " +
            providerName + " is not loaded");
        PEG_METHOD_EXIT();
        return(1);
    }

    try
    {
        unloadProvider(providerName);
    }
    catch(...)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Unload provider failed " + pr->_name);
        PEG_METHOD_EXIT();
        return(-1);
    }
    PEG_METHOD_EXIT();
    return(1);
}

Array<Provider*> LocalProviderManager::getIndicationProvidersToEnable()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "LocalProviderManager::getIndicationProvidersToEnable");

    Array<Provider*> enableProviders;

    Tracer::trace (TRC_PROVIDERMANAGER, Tracer::LEVEL4,
        "Number of providers in _providers table = %d", _providers.size());

    try
    {
        AutoMutex lock(_providerTableMutex);

        //
        // Iterate through the _providers table
        //
        for (ProviderTable::Iterator i = _providers.start (); i != 0; i++)
        {
            //
            //  Enable any indication provider with current subscriptions
            //
            Provider* provider = i.value();

            if (provider->testSubscriptions ())
            {
                enableProviders.append (provider);
            }
        }

    }
    catch (CIMException & e)
    {
        PEG_TRACE_STRING (TRC_DISCARDED_DATA, Tracer::LEVEL2,
            "CIMException: " + e.getMessage ());
    }
    catch (Exception & e)
    {
        PEG_TRACE_STRING (TRC_DISCARDED_DATA, Tracer::LEVEL2,
            "Exception: " + e.getMessage ());
    }
    catch (...)
    {
        PEG_TRACE_STRING (TRC_DISCARDED_DATA, Tracer::LEVEL2,
            "Unexpected error in getIndicationProvidersToEnable");
    }

    Tracer::trace (TRC_PROVIDERMANAGER, Tracer::LEVEL4,
        "Number of indication providers to enable = %d",
        enableProviders.size ());

    PEG_METHOD_EXIT ();
    return enableProviders;
}

Provider* LocalProviderManager::_initProvider(
    Provider* provider,
    const String& moduleFileName)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "LocalProviderManager::_initProvider");

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
        AutoMutex lock(provider->_statusMutex);

        if (provider->_status == Provider::INITIALIZED)
        {
            // Initialization is already complete
            return provider;
        }

        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Loading/Linking Provider Module " + moduleFileName);

        // load the provider
        try
        {
            base = module->load(provider->_name);
        }
        catch(...)
        {
            PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                "Exception caught Loading/Linking Provider Module " +
                moduleFileName);
            PEG_METHOD_EXIT();
            throw;
        }

        // initialize the provider
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL2,
            "Initializing Provider " + provider->_name);

        CIMOMHandle *cimomHandle = new CIMOMHandle();
        provider->set(module, base, cimomHandle);
        provider->_quantum=0;

        Boolean initializeError = false;

        try
        {
          provider->initialize(*(provider->_cimom_handle));
        }
        catch(...)
        {
          initializeError = true;
        }

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
            delete provider->_cimom_handle;

            // set provider status to UNINITIALIZED
            provider->reset();

            provider->_provider = 0;

            // unload provider module
            module->unloadModule();
        }
    }   // unlock the provider status mutex

    PEG_METHOD_EXIT();
    return(provider);
}

void LocalProviderManager::_unloadProvider(Provider* provider)
{
    //
    // NOTE:  It is the caller's responsibility to make sure that
    // the ProviderTable mutex is locked before calling this method.
    //
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "LocalProviderManager::_unloadProvider");

    PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
        "Unloading Provider " + provider->_name);

    if (provider->_current_operations.value() > 0)
    {
      PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
          "Provider cannot be unloaded due to pending operations: " +
          provider->_name);
    }
    else
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Terminating Provider " + provider->_name);

        // lock the provider mutex
        AutoMutex pr_lock(provider->_statusMutex);

        try
        {
           if (provider->_indications_enabled)
           {
               provider->disableIndications();
           }
        }
        catch(...)
        {
            PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL3,
                "Error occured disabling provider " + provider->_name);
        }

        try
        {
            provider->terminate();
        }
        catch(...)
        {
            PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL3,
                "Error occured terminating provider " + provider->_name);
        }

        PEGASUS_ASSERT(provider->_module != 0);

        // unload provider module
        provider->_module->unloadModule();

        // NOTE: The "delete provider->_cimom_handle" operation was
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
            "Destroying Provider's CIMOM Handle " + provider->_name);
        delete provider->_cimom_handle;
        
        Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            "ProviderManager::_provider_crtl -  Unload provider $0",
            provider->getName());

        // set provider status to UNINITIALIZED
        provider->reset();
    }

    PEG_METHOD_EXIT();
}

Provider* LocalProviderManager::_lookupProvider(const String& providerName)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "LocalProviderManager::_lookupProvider");

    // lock the providerTable mutex
    AutoMutex lock(_providerTableMutex);

    // look up provider in cache
    Provider* pr = 0;
    if (_providers.lookup(providerName, pr))
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Found Provider " + providerName + " in Provider Manager Cache");
    }
    else
    {
        // create provider
        pr = new Provider(providerName, 0, 0);

        // insert provider in provider table
        _providers.insert(providerName, pr);

        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Created provider " + pr->getName());
    }

    PEG_METHOD_EXIT();
    return (pr);
}

ProviderModule* LocalProviderManager::_lookupModule(
    const String& moduleFileName)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "LocalProviderManager::_lookupModule");

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
    return (module);
}

PEGASUS_NAMESPACE_END
