//%2003////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002  BMC Software, Hewlett-Packard Development
// Company, L. P., IBM Corp., The Open Group, Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L. P.;
// IBM Corp.; EMC Corporation, The Open Group.
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
//
//%/////////////////////////////////////////////////////////////////////////////

#include "LocalProviderManager.h"

#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/MessageQueueService.h>
#include <Pegasus/Common/PegasusVersion.h>

PEGASUS_NAMESPACE_BEGIN

static LocalProviderManager *my_instance = 0;

LocalProviderManager::LocalProviderManager(void)
    : _idle_timeout(300), _unload_idle_flag(1)
{
    my_instance = this;
}

LocalProviderManager::~LocalProviderManager(void)
{
    Uint32 ccode;

    _provider_ctrl(UNLOAD_ALL_PROVIDERS, this, &ccode);
}

Sint32 LocalProviderManager::_provider_ctrl(CTRL code, void *parm, void *ret)
{
    static Uint32 quantum;
    AutoMutex monitor(_mut);
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "_provider_ctrl");

    Sint32 ccode = 0;
    CTRL_STRINGS *parms = reinterpret_cast<CTRL_STRINGS *>(parm);

    switch(code)
    {

    case GET_PROVIDER:
        {

            PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL2,
                "_provider_ctrl::GET_PROVIDER");

            Provider *pr = NULL;
            OpProviderHolder* ph = reinterpret_cast< OpProviderHolder* >( ret );


            if(true == _providers.lookup( *(parms->providerName), pr ))
            {
                PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                    "Found Provider " + *(parms->providerName) + " in Provider Manager Cache");

                ph->SetProvider( pr );
                ph->GetProvider().update_idle_timer();
                break;
            }


            PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                "Creating Provider " + *(parms->providerName) );
            ProviderModule *module;

            if(false  == _modules.lookup(*(parms->fileName), module))
            {
                PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                    "Creating Provider Module " + *(parms->fileName) );

                // added parameters to re-activate ProviderAdapter  ( A Schuur )

                module = new ProviderModule(*(parms->fileName),*(parms->interfaceName));
                _modules.insert((*parms->fileName), module);
            }
            else
            {
                PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                    "Using Cached  Provider Module " + *(parms->fileName) );
            }

            PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                "Loading/Linking Provider Module " + *(parms->fileName) );

            CIMProvider *base ;

            try
            {
                base = module->load(*(parms->providerName));
            }
            catch(...)
            {
                PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                    "Exception caught Loading/Linking Provider Module " + *(parms->fileName) );
                throw;

//<< Wed Jul 30 17:43:05 2003 mdd >> bugzilla 286
//   now just propogate the exception.
//   if this causes problems, fix it in the ProviderManagerService, which should be able to catch this exception
// 	    CIMNullProvider *dummy = new CIMNullProvider();
// 	    if(dummy == 0)
// 	    {
// 	       throw NullPointer();
// 	    }
// 	    base = static_cast<CIMProvider *>(dummy);
            }

            // create provider module

            MessageQueue * queue = MessageQueue::lookup(PEGASUS_QUEUENAME_PROVIDERMANAGER_CPP);
            PEGASUS_ASSERT(queue != 0);
            MessageQueueService * service = dynamic_cast<MessageQueueService *>(queue);
            PEGASUS_ASSERT(service != 0);
            pr = new Provider(*(parms->providerName), module, base);
            if(0 == (pr->_cimom_handle =  new CIMOMHandle()))
                throw NullPointer();
            pr->_quantum=0;

            PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL2,
                "Loading Provider " +  pr->_name);
            try
            {
                pr->initialize(*(pr->_cimom_handle));
            }
            catch(...)
            {
                delete pr->_cimom_handle;
                delete pr;
                throw UninitializedObjectException();
            }

            pr->update_idle_timer();

            _providers.insert(*(parms->providerName), pr);


            ph->SetProvider( pr );
            break;
        }

    case UNLOAD_PROVIDER:
        {

            PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL2,
                "_provider_ctrl::UNLOAD_PROVIDER");
            CTRL_STRINGS *parms = reinterpret_cast<CTRL_STRINGS *>(parm);
            Provider *pr;
            if(true == _providers.lookup(*(parms->providerName), pr ))
            {
                PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                    "Unloading Provider " + pr->_name );
                if(pr->_current_operations.value())
                {
                    PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                        "Provider cannot be unloaded due to pending operations: " +
                        pr->_name );
                    break;
                }
                _providers.remove(pr->_name);
                PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                    "Terminating Provider " + pr->_name );
                try
                {
                    pr->terminate();
                }
                catch(...)
                {

                }

                if((pr->_module != 0 ) && pr->_module->_ref_count == 0)
                {
                    _modules.remove(pr->_module->_fileName);
                    try
                    {
                        pr->_module->unloadModule();
                    }
                    catch(...)
                    {

                    }
                    PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                        "Destroying Provider " + pr->_name );

                    delete pr->_module;
                }

                PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                    "Destroying Provider's CIMOM Handle " + pr->_name );

                Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
                    "ProviderManager::_provider_crtl -  Unload provider $0",
                    pr->_name);

                delete pr->_cimom_handle;
                delete pr;
            }
            else
            {
                PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL2,
                    "Unable to find Provider in cache: " +
                    *(parms->providerName));
            }
            break;
        }

    case LOOKUP_PROVIDER:
        {

            PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL2,
                "_provider_ctrl::LOOKUP_PROVIDER");

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
            if(false  == _modules.insert(
                *(parms->fileName),
                *reinterpret_cast<ProviderModule * *>(parm)))
                ccode = -1;
            break;
        }
    case REMOVE_PROVIDER:
        {

            PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL2,
                "_provider_ctrl::REMOVE_PROVIDER");
            if(false == _providers.remove(*(parms->providerName)))
                ccode = -1;
            break;
        }
    case REMOVE_MODULE:
        {

            PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL2,
                "_provider_ctrl::REMOVE_MODULE");
            if(false == _modules.remove(*(parms->fileName)))
                ccode = -1;
            break;
        }

    case UNLOAD_ALL_PROVIDERS:
        {
            PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL2,
                "_provider_ctrl::UNLOAD_ALL_PROVIDERS");
            LocalProviderManager *myself = reinterpret_cast<LocalProviderManager *>(parm);
            Provider * provider;
            ProviderTable::Iterator i = myself->_providers.start();
            if (myself->_providers.size()) try
            {
                for(; i != 0; i++)
                {
                    provider = i.value();
                    if(provider == 0)
                    {
                        continue;
                    }

                    PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                        "Terminating Provider " + provider->getName());
                    try
                    {
                        provider->terminate();
                    }
                    catch(...)
                    {
                        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                            "Exception terminating " +
                            provider->getName());
                        continue;
                    }
                    if((provider->_module != 0 ) &&
                        provider->_module->_ref_count.value() == 0)
                    {

                        if(true == _modules.lookup(provider->_module->_fileName, provider->_module))
                        {
                            PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                                "Removing Provider's Module " + provider->getName());
                            _modules.remove(provider->_module->_fileName);
                            PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                                "Destroying Provider's Module " + provider->getName());
                            delete provider->_module;
                        }
                    }
                    PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                        "Destroying Provider's CIMOM Handle: " + provider->getName());
                    delete provider->_cimom_handle;
                    PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                        "Destroying Provider: " + provider->getName());

                    Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
                        "ProviderManager::_provider_crtl -  Unload provider $0",
                        provider->getName());

                    delete provider;
                }
                PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                    "Clearing Provider Cache" );
                myself->_providers.clear();
                PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                    "Clearing Module Cache");
                myself->_modules.clear();
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

            quantum++;
            LocalProviderManager *myself = reinterpret_cast<LocalProviderManager *>(parm);
            Provider * provider;
            PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4, "providers loaded: " + myself->_providers.size());

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
                        if(provider == 0)
                        {
                            myself->_providers.remove(i.key());
                            i = myself->_providers.start();
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
                            PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                                "Provider has pending operations" + provider->getName() );

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

                            Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
                                "ProviderManager::_provider_crtl -  Unload idle provider $0",
                                provider->getName());

                            /*
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
                            */

                            PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                                "Removing Provider " + provider->getName());
                            myself->_providers.remove(provider->_name);

                            // Important = reset iterator to beginning of list. quantum value assures we do
                            // not inspect providers more than once even though we traverse them.
                            i = myself->_providers.start();
                            PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                                provider->_name + " Removed, looking at Module" );
                            if(provider->_module != 0)
                            {
                                PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                                    "Module ref: " + provider->_module->_ref_count.value() );
                                if(provider->_module->_ref_count.value() == 0)
                                {
                                    PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                                        "Removing Module " + provider->_module->_fileName);
                                    _modules.remove(provider->_module->_fileName);
                                    delete provider->_module;

                                }
                            }

                            try
                            {
                                PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                                    "Destroying Provider's CIMOM Handle: " +
                                    provider->getName());
                                delete provider->_cimom_handle;
                                PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                                    "Destroying Provider: " +
                                    provider->getName());
                                delete provider;
                            }
                            catch(...)
                            {
                                // we may leak if any of the destructors
                                // throws an exception
                            }
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

    case UNLOAD_IDLE_MODULES:
        {

            PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL2,
                "_provider_ctrl::UNLOAD_IDLE_MODULES");

            LocalProviderManager *myself = reinterpret_cast<LocalProviderManager *>(parm);
            ProviderModule *module;
            ModuleTable::Iterator i = myself->_modules.start();
            for(; i ; i++)
            {
                module = i.value();
                if(module->_ref_count.value() == 0)
                {
                    myself->_modules.remove(module->_fileName);
                    module->unloadModule();
                    delete module;
                    i = myself->_modules.start();
                }
            }
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
    const String & fileName,
    const String & providerName,
    const String & interfaceName)
{

    OpProviderHolder ph;
    CTRL_STRINGS strings;
    Sint32 ccode;
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "ProviderManager::getProvider");
    strings.fileName = &fileName;
    strings.providerName = &providerName;
    strings.interfaceName = &interfaceName;
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


// << Tue Jul 29 16:51:25 2003 mdd >> change to run every 300 seconds
void LocalProviderManager::unload_idle_providers(void)
{
    static struct timeval first = {0,0}, now, last = {0,0};
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "ProviderManager::unload_idle_providers");
    if(first.tv_sec == 0)
        gettimeofday(&first, NULL);
    gettimeofday(&now, NULL);
    if(((now.tv_sec - first.tv_sec) > 300 ) && ( (now.tv_sec - last.tv_sec) > 300))
    {
        gettimeofday(&last, NULL);
        if(_unload_idle_flag.value() == 1)
        {
            PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL2,
                "Unload Idle Flag Set: Starting Provider Monitor Thread");
            _unload_idle_flag = 0;
            MessageQueueService::get_thread_pool()->allocate_and_awaken(this, provider_monitor);

        }
    }
    PEG_METHOD_EXIT();
}


PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL LocalProviderManager::provider_monitor(void *parm)
{
    Thread *th = reinterpret_cast<Thread *>(parm);

    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "ProviderManager::provider_monitor");
    LocalProviderManager *myself =    my_instance ;

    PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
        "Checking for Idle providers to unload.");
    try
    {
        myself->_provider_ctrl(UNLOAD_IDLE_PROVIDERS, myself, (void *)0);
        myself->_unload_idle_flag = 1;
    }
    catch(...)
    {
    }
    PEG_METHOD_EXIT();
    return(0);
}

Sint16 LocalProviderManager::disableProvider(
    const String & fileName,
    const String & providerName)
{
    CTRL_STRINGS strings;
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "ProviderManager::disableProvider");

    Provider *pr;
    if(true == _providers.lookup(providerName, pr ))
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
            PEG_METHOD_EXIT();
            return(0);
        }
    }
    else
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL2,
            "Unable to find Provider in cache: " +
            providerName);
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

Sint16 LocalProviderManager::disableIndicationProvider(
    const String & fileName,
    const String & providerName)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "ProviderManager::disableIndicationProvider");

    Provider *pr;
    if(_providers.lookup(providerName, pr ))
    {
        //
        // Check to see if there are pending requests. If there are pending
        // requests and the disable timeout has not expired, loop and wait one
        // second until either there are no pending requests or until timeout expires.
        //
        Uint32 waitTime = PROVIDER_DISABLE_TIMEOUT;
        while(pr->_current_ind_operations.value() > 0  &&  waitTime > 0)
        {
            System::sleep(1);
            waitTime = waitTime - 1;
        }

        // If there are still pending requests, do not disable
        if(pr->_current_ind_operations.value() > 0)
        {
            PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                "Disable failed since there are pending requests." );
            PEG_METHOD_EXIT();
            return(0);
        }

        _providers.remove(pr->_name);
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Disabling Provider " + pr->_name );
        try
        {
            pr->disableIndications();
            pr->terminate();
        }
        catch(...)
        {
            PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                "Disable failed." );
            PEG_METHOD_EXIT();
            return(-1);
        }

        if((pr->_module != 0 ) && pr->_module->_ref_count.value() <= 1)
        {
            _modules.remove(pr->_module->_fileName);
            try
            {
                pr->_module->unloadModule();
            }
            catch(...)
            {
                PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                    "unloadModule failed.");
                PEG_METHOD_EXIT();
                return(-1);
            }

            PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                "Destroying Provider " + pr->_name );

            delete pr->_module;
        }

        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Destroying Provider's CIMOM Handle " + pr->_name );

        Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            "ProviderManager::_provider_crtl -  Unload provider $0",
            pr->_name);

        delete pr->_cimom_handle;
        delete pr;
        return(1);
    }
    else
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL2,
            "Unable to find Provider in cache: " +
            providerName);
        PEG_METHOD_EXIT();
        return(1);
    }
}

PEGASUS_NAMESPACE_END
