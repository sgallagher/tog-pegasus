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
// Author: Chip Vincent (cvincent@us.ibm.com)
//
// Modified By: Yi Zhou, Hewlett-Packard Company(yi_zhou@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include "ProviderManager.h"

#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/PegasusVersion.h>

#include "MutexLock.h"

PEGASUS_NAMESPACE_BEGIN

ProviderManager::ProviderManager(void)
{
}

ProviderManager::~ProviderManager(void)
{
    String fileName;
    String providerName;

    // terminate all providers
    for(Uint32 i = 0, n = _providers.size(); i < n; i++)
    {
        try
        {
            //_providers[i].terminate();
	    providerName = _providers[i].getName();
	    fileName = _providers[i].getModule().getFileName();

	    unloadProvider(fileName, providerName);
        }
        catch(...)
        {
        }
    }
}

Provider ProviderManager::getProvider(
    const String & fileName,
    const String & providerName,
    const String & interfaceName)
{
    // check list for requested provider and return if found
    for(Uint32 i = 0, n = _providers.size(); i < n; i++)
    {
        if(String::equalNoCase(providerName, _providers[i].getName()))
        {
            return(_providers[i]);
        }
    }

    loadProvider(fileName, providerName, interfaceName);

    return(getProvider(fileName, providerName, interfaceName));
}

void ProviderManager::loadProvider(
    const String & fileName,
    const String & providerName,
    const String & interfaceName)
{
    MutexLock lock(_mutex);

    //_loadProvider(fileName, providerName, interfaceName);

    // NOTE:
    // check the list before attempting to load the provider
    // to prevent multiple threads from attempting to load
    // a provider during its initialization.

    // check list for requested provider and do nothing if found
    for(Uint32 i = 0, n = _providers.size(); i < n; i++)
    {
        if(String::equalNoCase(providerName, _providers[i].getName()))
        {
            return;
        }
    }

    Uint32 refCount = 0;

    // get reference count 
    _getRefCount(fileName, refCount);

    refCount = refCount + 1;

    // create provider module
    Provider provider(providerName, fileName, interfaceName, refCount);


    // ATTN: need optimization - create CIMOMHandle once

    // create a CIMOMHandle
    MessageQueue * queue = MessageQueue::lookup(PEGASUS_QUEUENAME_PROVIDERMANAGER_CPP);

    PEGASUS_ASSERT(queue != 0);

    MessageQueueService * service = dynamic_cast<MessageQueueService *>(queue);

    PEGASUS_ASSERT(service != 0);

    CIMOMHandle _cimom(service);

    // initialize provider
    provider.initialize(_cimom);

    // if module is already in the array, remove the old module, add the new module in 
    if (refCount > 1)
    {
	// module is already in the array
	_updateRefCount(fileName, refCount);	
    }
    else
    {
	// module is not in the array, create the module and add it in the array
	ProviderModule module(fileName, refCount);
    	_modules.append(module);
    } 
    _providers.append(provider);
}


#if 0
Provider ProviderManager::_loadProvider(
    const String & fileName,
    const String & providerName,
    const String & interfaceName = String::EMPTY)
{
    // NOTE: _loadProvider SHOULD ONLY BE CALLED AFTER OBTAINING THE LOCK
    //
#endif

void ProviderManager::unloadProvider(
    const String & fileName,
    const String & providerName)
{
    MutexLock lock(_mutex);

    // check list for requested provider and return if found
    for(Uint32 i = 0, n = _providers.size(); i < n; i++)
    {
        if(String::equalNoCase(providerName, _providers[i].getName()))
        {
            Provider provider(_providers[i]);

            _providers.remove(i);

	    // get module reference count
	    Uint32 refCount = 0;

 	    _getRefCount(fileName, refCount);

	    // if refCount is greater than 1, just terminate provider, not unload
 	    // otherwise, terminate and unload
	    if (refCount > 1)
	    {
		provider.terminate();
    	    }
	    else
	    {
            	provider.terminate();
		provider.getModule().unloadModule();
	    }
	    
	    // update reference count
	    _updateRefCount(fileName, refCount - 1);

	    return;
        }
    }
}

void ProviderManager::shutdownAllProviders(void)
{
    //
    // terminate all providers
    //
    //for (Uint32 i = 0, n = _providers.size(); i < n; i++)
    //for(Uint32 i = _providers.size(); i > 0;)

    Uint32 numProviders = _providers.size();

    while (numProviders > 0)
    {
	try
	{
	    //_providers[0].terminateProvider();
	    _providers[0].terminate();

	    _providers.remove(0);
            numProviders--;
	}
	catch(...)
	{
            // may want to log to Pegasus log in the future
	}
    }
}

void ProviderManager::_getRefCount(const String & fileName, Uint32 & refCount)
{
    for(Uint32 i = 0, n = _modules.size(); i < n; i++)
    {
	if(String::equalNoCase(fileName, _modules[i].getFileName()))
	{
	    refCount = _modules[i].getRefCount();

	    return;
	}
    }
}

void ProviderManager::_updateRefCount(const String & fileName, const Uint32 & refCount)
{
    for(Uint32 i = 0, n = _modules.size(); i < n; i++)
    {
	if(String::equalNoCase(fileName, _modules[i].getFileName()))
	{
	    // remove the module from array
	    _modules.remove(i);

	    // if reference count is none zero, update the array
	    if ( refCount > 0 )
	    {
	    	// Add the module to the array
	    	ProviderModule module(fileName, refCount);
	    	_modules.append(module);
	    }
	    return;
	}
    }
}

// ATTN: disabled temporarily
/*
PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL ProviderManager::monitorThread(void * arg)
{
    Thread * thread = reinterpret_cast<Thread *>(arg);

    ProviderManager * _this = reinterpret_cast<ProviderManager *>(thread->get_parm());

    // check provider list every 30 seconds for providers to unload
    for(Uint32 timeout = 0; true; timeout += 30)
    {
    thread->sleep(30000);

    // check each provider for timeouts less than the current timeout
    //for(Uint32 i = 0, n = _this->_providers.size(); i < n; i++)

    // start with highest entry to prevent out-of-bounds
    // exception in case a removed entry - Markus

    for(Uint32 i = _this->_providers.size(); i > 0;)
    {
        // We want to count down to 0, but Uint32 will never go < 0
        i--;

        // get provider timeout

        #if defined(PEGASUS_OS_HPUX)
        Uint32 provider_timeout = 0xffffffff;
        #else
        Uint32 provider_timeout = 30;
        #endif

        if((provider_timeout != 0xffffffff) && (provider_timeout <= timeout))
        {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL2,
            "Unloading provider for " + _this->_providers[i].getClassName() +
                " in " + _this->_providers[i].getProviderName());
        void * mypr = (void *)_this->_providers[i].getProvider();

        _this->_providers[i].getProvider()->terminate();
        _this->_providers.remove(i);
        }
    }
    }

    PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL2,
        "Provider monitor stopped");

    return(0);
}
*/

PEGASUS_NAMESPACE_END
