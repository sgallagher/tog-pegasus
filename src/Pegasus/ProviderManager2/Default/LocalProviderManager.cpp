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

LocalProviderManager::LocalProviderManager(void)
    : _idle_timeout(IDLE_LIMIT)
{
}

LocalProviderManager::~LocalProviderManager(void)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "~ProviderManager");
    Uint32 ccode;

    _provider_ctrl(UNLOAD_ALL_PROVIDERS, this, &ccode);

    ProviderTable::Iterator i = _providers.start();
    for(;i!=0;i++)
    {
      Provider *provider = i.value();
      delete provider;
    }
    ModuleTable::Iterator j = _modules.start();
    for(;j!=0;j++)
    {
      ProviderModule *module = j.value();
      delete module;
    }
    PEG_METHOD_EXIT();
}

Sint32 LocalProviderManager::_provider_ctrl(CTRL code, void *parm, void *ret)
{
    static Uint32 quantum;
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "_provider_ctrl");

    Sint32 ccode = 0;
    CTRL_STRINGS *parms = reinterpret_cast<CTRL_STRINGS *>(parm);

    switch(code)
    {

    case GET_PROVIDER:
        {

            PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL2,
                "_provider_ctrl::GET_PROVIDER");

            String providerName = *(parms->providerName);
            String moduleFileName = *(parms->fileName);
            Provider *pr =0;
            
            OpProviderHolder* ph = reinterpret_cast< OpProviderHolder* >( ret );
            pr =_lookupProvider(providerName);

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

            PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                "returning Provider" + providerName);

            ph->SetProvider( pr );
            ph->GetProvider().update_idle_timer();
            break;
        }

    case UNLOAD_PROVIDER:
        {

            PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL2,
                "_provider_ctrl::UNLOAD_PROVIDER");
            Provider *pr =_lookupProvider(*(parms->providerName));
            if(pr->getStatus() == Provider::INITIALIZED)
            {
                PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                    "Unloading Provider " + pr->_name );

                AutoMutex lock(_providerTableMutex);
                _unloadProvider(pr);
            }
            break;
        }

    case LOOKUP_PROVIDER:
        {

            PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL2,
                "_provider_ctrl::LOOKUP_PROVIDER");
            AutoMutex lock(_providerTableMutex);

            if(true == _providers.lookup(*(parms->providerName),
                *(reinterpret_cast<Provider * *>(ret))))
            {
                PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                    "Found Provider in cache: " +
                    *(parms->providerName));

                (*(reinterpret_cast<Provider * *>(ret)))->update_idle_timer();
            }
            else
            {

                PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                    "Could not find  Provider in cache: " +
                    *(parms->providerName));
                ccode = -1;
            }

            break;
        }

    case LOOKUP_MODULE:
        {

            PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL2,
                "_provider_ctrl::LOOKUP_MODULE");
            AutoMutex lock(_providerTableMutex);

            if(false  == _modules.lookup(*(parms->fileName),
                *(reinterpret_cast<ProviderModule * *>(ret))))
            {
                PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                    "Could not find  Provider Module in cache: " +
                    *(parms->fileName));
                ccode = -1;
            }

            break;
        }

    case INSERT_PROVIDER:
        {

            PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL2,
                "_provider_ctrl::INSERT_PROVIDER");
            AutoMutex lock(_providerTableMutex);    
            if(false  == _providers.insert(
                *(parms->providerName),
                *reinterpret_cast<Provider * *>(parm)))
                ccode = -1;
            break;
        }
    case INSERT_MODULE:
        {
            PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL2,
                "_provider_ctrl::INSERT_MODULE");
            AutoMutex lock(_providerTableMutex);
            if(false  == _modules.insert(
                *(parms->fileName),
                *reinterpret_cast<ProviderModule * *>(parm)))
                ccode = -1;
            break;
        }

    case UNLOAD_ALL_PROVIDERS:
        {
            PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL2,
                "_provider_ctrl::UNLOAD_ALL_PROVIDERS");
            LocalProviderManager *myself = reinterpret_cast<LocalProviderManager *>(parm);
            Provider * provider;
            AutoMutex lock(_providerTableMutex);
            Tracer::trace(TRC_PROVIDERMANAGER,Tracer::LEVEL4,"providers in cache =%d",
                    myself->_providers.size());
            ProviderTable::Iterator i = myself->_providers.start();
            if (myself->_providers.size()) try
            {
                for(; i != 0; i++)
                {
                    provider = i.value();
                    PEGASUS_ASSERT(provider!=0);
                    if(provider->getStatus()==Provider::UNINITIALIZED)
                    {
                        continue;
                    }
                    else
                    {
                     _unloadProvider(provider);
                    }
                }
                    
            }
            catch(...)
            {
                PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                    "Unexpected Exception in UNLOAD_ALL_PROVIDERS.");
            }
            break;
        }

    case UNLOAD_IDLE_PROVIDERS:
        {

            PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL2,
                "_provider_ctrl::UNLOAD_IDLE_PROVIDERS");
            AutoMutex lock(_providerTableMutex);

            quantum++;
            LocalProviderManager *myself = reinterpret_cast<LocalProviderManager *>(parm);
            Provider * provider;

            if(myself->_providers.size())
            {
                try
                {
                    struct timeval now;
                    gettimeofday(&now, NULL);
                    ProviderTable::Iterator i = myself->_providers.start();
                    for(; i != 0 ; i++)
                    {
                        provider = i.value();
                        PEGASUS_ASSERT(provider!=0);
                        if(provider->getStatus()==Provider::UNINITIALIZED)
                        {
                            continue;
                        }

                        if(provider->_quantum == quantum)
                        {
                            continue;
                        }

                        provider->_quantum = quantum;

                        if(provider->_current_operations.value())
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
                            " provider->unload_ok() returns: " +
                            provider->unload_ok() ? "true" : "false" );

                        if(provider->unload_ok() == true &&
                            (  now.tv_sec - timeout.tv_sec) > ((Sint32)myself->_idle_timeout))
                        {
                            AutoMutex pr_lock(provider->_statusMutex);
                            Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
                                "ProviderManager::_provider_crtl -  Unload idle provider $0",
                                provider->getName());

                            PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                                "Trying to Terminate Provider " + provider->getName());
                            try
                            {
                                if(false == provider->tryTerminate())
                                {
                                    PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                                        "Provider Refused Termination " + provider->getName());

                                    continue;
                                }
                                else
                                {
                                    PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                                        "Provider terminated: " +  provider->getName());
                                }
                            }
                            catch(...)
                            {
                                PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                                    "Exception terminating " +
                                    provider->getName());
                                i = myself->_providers.start();
                                continue;
                            }

                            PEGASUS_ASSERT(provider->_module!=0);
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
                    PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                        "Unexpected Exception in UNLOAD_IDLE_PROVIDERS.");
                }
            } // if there are any providers
            break;
        }

        default:
        ccode = -1;
        break;
    }
    PEG_METHOD_EXIT();
    return(ccode);
}

OpProviderHolder LocalProviderManager::getProvider(
    const String& fileName,
    const String& providerName)
{

    OpProviderHolder ph;
    CTRL_STRINGS strings;
    Sint32 ccode;
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "ProviderManager::getProvider");
    strings.fileName = &fileName;
    strings.providerName = &providerName;
    try
    {
        ccode = _provider_ctrl( GET_PROVIDER, &strings, &ph );
    }
    catch(...)
    {
        PEG_METHOD_EXIT();
        throw;
    }


    PEG_METHOD_EXIT();
    return(ph);

}

void LocalProviderManager::unloadProvider(
    const String & fileName,
    const String & providerName)
{
    CTRL_STRINGS strings;
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "ProviderManager::unloadProvider");
    strings.fileName = &fileName;
    strings.providerName = &providerName;
    _provider_ctrl(UNLOAD_PROVIDER, &strings, (void *)0);
    PEG_METHOD_EXIT();
}

void LocalProviderManager::shutdownAllProviders(void)
{

    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "ProviderManager::shutdownAllProviders");
    _provider_ctrl(UNLOAD_ALL_PROVIDERS, (void *)this, (void *)0);
    PEG_METHOD_EXIT();
}


Boolean LocalProviderManager::hasActiveProviders()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "ProviderManager::hasActiveProviders");

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
        "ProviderManager::unloadIdleProviders");

    static struct timeval first = {0,0}, now, last = {0,0};

    if(first.tv_sec == 0)
    {
        gettimeofday(&first, NULL);
    }
    gettimeofday(&now, NULL);

    if (((now.tv_sec - first.tv_sec) > IDLE_LIMIT) &&
        ((now.tv_sec - last.tv_sec) > IDLE_LIMIT))
    {
        gettimeofday(&last, NULL);
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Checking for Idle providers to unload.");
        try
        {
            _provider_ctrl(UNLOAD_IDLE_PROVIDERS, this, (void *)0);
        }
        catch(...)
        {
            PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL2,
                "Caught unexpected exception from UNLOAD_IDLE_PROVIDERS.");
        }
    }
    PEG_METHOD_EXIT();
}


Sint16 LocalProviderManager::disableProvider(
    const String & fileName,
    const String & providerName)
{
    CTRL_STRINGS strings;
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "ProviderManager::disableProvider");

    Provider *pr=_lookupProvider(providerName);
    if(pr->getStatus()==Provider::INITIALIZED)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Disable Provider " + pr->_name );
        //
        // Check to see if there are pending requests. If there are pending
        // requests and the disable timeout has not expired, loop and wait one
        // second until either there is no pending requests or until timeout expires.
        //
        Uint32 waitTime = PROVIDER_DISABLE_TIMEOUT;
        while(pr->_current_operations.value() > 0  &&  waitTime > 0)
        {
            System::sleep(1);
            waitTime = waitTime - 1;
        }

        // There are still pending requests, do not disable
        if(pr->_current_operations.value() > 0)
        {
            PEG_TRACE_STRING(TRC_PROVIDERMANAGER,Tracer::LEVEL4,
              "Disable failed since there are pending requests.");
            PEG_METHOD_EXIT();
            return(0);
        }
    }
    else
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL2,
            "Provider " +
            providerName + "is not loaded");
        PEG_METHOD_EXIT();
        return(1);
    }

    strings.fileName = &fileName;
    strings.providerName = &providerName;
    try
    {
        _provider_ctrl(UNLOAD_PROVIDER, &strings, (void *)0);
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

Array <Provider *> LocalProviderManager::getIndicationProvidersToEnable ()
{
    PEG_METHOD_ENTER (TRC_PROVIDERMANAGER, 
        "LocalProviderManager::getIndicationProvidersToEnable");

    Array <Provider *> enableProviders;

    Tracer::trace (TRC_PROVIDERMANAGER, Tracer::LEVEL4,
        "Number of providers in _providers table = %d", _providers.size ());

    try
    {
        AutoMutex lock (_providerTableMutex);

        //
        // Iterate through the _providers table
        //
        for (ProviderTable::Iterator i = _providers.start (); i != 0; i++)
        {
            //
            //  Enable any indication provider with current subscriptions
            //
            Provider * provider = i.value ();
            
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
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "_initProvider");

    ProviderModule *module = 0;
    CIMProvider *base;

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
            "Initializing Provider " +  provider->_name);

        CIMOMHandle *cimomHandle = new CIMOMHandle();
        provider->set(module, base, cimomHandle);
        provider->_quantum=0;

        try
        {
            provider->initialize(*(provider->_cimom_handle));
        }
        catch(...)
        {
            // delete the cimom handle
            delete provider->_cimom_handle;

            // set provider status to UNINITIALIZED
            provider->reset();

            // unload provider module
            module->unloadModule();
        }
    }   // unlock the provider status mutex

    PEG_METHOD_EXIT();
    return(provider);
}


void LocalProviderManager::_unloadProvider( Provider * provider)
{
    //
    // NOTE:  It is the caller's responsibility to make sure that
    // the ProviderTable mutex is locked before calling this method.
    //
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "_unloadProvider");

    PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                     "Unloading Provider " + provider->_name );

    if ( provider->_current_operations.value()>0)
    {
      PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                         "Provider cannot be unloaded due to pending operations: " +
                         provider->_name );
    }
    else
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                         "Terminating Provider " + provider->_name );

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
                             "Error occured disabling provider " + provider->_name );
        }
        
        try
        {
            provider->terminate();
        }
        catch(...)
        {
            PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL3,
                             "Error occured terminating provider " + provider->_name );
        }

        // delete the cimom handle
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                         "Destroying Provider's CIMOM Handle " + provider->_name );
        delete provider->_cimom_handle;

        PEGASUS_ASSERT(provider->_module != 0);

        // unload provider module
        provider->_module->unloadModule();

        Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            "ProviderManager::_provider_crtl -  Unload provider $0",
            provider->getName());

        // set provider status to UNINITIALIZED
        provider->reset();
    }

    PEG_METHOD_EXIT();
}

Provider * LocalProviderManager::_lookupProvider(
    const String & providerName)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "_lookupProvider");

    // lock the providerTable mutex
    AutoMutex lock(_providerTableMutex);

    // look up provider in cache
    Provider * pr = 0;
    if ( true == _providers.lookup(providerName, pr) )
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                         "Found Provider " + providerName +
			 " in Provider Manager Cache");
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


ProviderModule * LocalProviderManager::_lookupModule(
    const String& moduleFileName)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "_lookupModule");

    // look up provider module in cache
    ProviderModule * module = 0;

    if ( true == _modules.lookup(moduleFileName, module) )
    {
	// found provider module in cache
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                         "Found Provider Module" + moduleFileName +
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
