//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 The Open group, BMC Software, Tivoli Systems, IBM
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
// Modified By:
//              Nag Boranna, Hewlett-Packard Company(nagaraja_boranna@hp.com)
//              Yi Zhou, Hewlett-Packard Company(yi_zhou@hp.com)
//              Jenny Yu, Hewlett-Packard Company (jenny_yu@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include "ProviderManager.h"

PEGASUS_NAMESPACE_BEGIN

ProviderManager::ProviderManager(
    MessageQueue * outputQueue,
    CIMRepository * repository,
    CIMServer * server)
:
    _cimom(outputQueue, repository),
    _serviceCimom(outputQueue, repository, server)
{
    Thread * thread = new Thread(monitorThread, this, false);

    thread->run();
}

ProviderManager::~ProviderManager(void)    
{
    // terminate all providers
    for(Uint32 i = 0, n = _providers.size(); i < n; i++)
    {
        ProviderHandle * provider = _providers[i].getProvider();
        
        if(provider != 0)
        {
            _providers[i].getProvider()->terminate();
        }
    }
}

ProviderHandle * ProviderManager::getProvider(const String & providerName, const String & className)
{
    // check list for requested provider and return if found
    for(Uint32 i = 0, n = _providers.size(); i < n; i++)
    {
        if(String::equalNoCase(providerName, _providers[i].getProviderName()) &&
           String::equalNoCase(className, _providers[i].getClassName()))
        {
            return(_providers[i].getProvider());
        }
    }
    
    PEGASUS_STD(cout) << "loading provider for " << className << " in " << providerName << PEGASUS_STD(endl);
    
    // create provider module
    ProviderModule module(providerName, className);

    module.load();
    
    // get provider handle
    ProviderHandle * provider = module.getProvider();

    if(provider == 0)
    {
        throw ProviderFailure(providerName, className, "invalid provider handle.");
    }
    
    // initialize provider
    // ATTN: Distinguish between services and other providers and send the
    // appropriate CIMOMHandle
    // ATTN: Is it okay to pass the same CIMOMHandle to multiple providers?
    // Is that object reentrant?  (it has a waitForResponse method)
    provider->initialize(_serviceCimom);
    
    // add provider to list
    _providers.append(module);

    // recurse to get the provider as it resides in the array (rather than the local instance)
    return(getProvider(providerName, className));
}


void ProviderManager::_addProviderToTable
		(const String & providerName, Boolean blockFlag)
{
    ProviderBlockedEntry providerInfo(providerName, blockFlag);

    // add providerInfo to provider block table
    _providerBT.append(providerInfo);
}

void ProviderManager::addProviderToTable
		(const String & providerName, Boolean BlockFlag)
{
    _addProviderToTable(providerName, BlockFlag);
}

void ProviderManager::removeProviderFromTable
		(const String & providerName)
{
	for (Uint32 i=0, n=_providerBT.size(); i<n; i++)
	{
	   if(String::equalNoCase(providerName,_providerBT[i].getProviderName()))
	   {
		_providerBT.remove(i);
	   }
	}
}

Uint32 ProviderManager::blockProvider(const String & providerName)
{
	for (Uint32 i=0, n=_providerBT.size(); i<n; i++)
	{
	   if(String::equalNoCase(providerName,_providerBT[i].getProviderName()))
	   {
		_providerBT[i].setProviderBlockFlag(true);
		return (0);
	   }	
	}

	return (1);
}

Uint32 ProviderManager::unblockProvider(const String & providerName)
{
	for (Uint32 i=0, n=_providerBT.size(); i<n; i++)
	{
	   if(String::equalNoCase(providerName,_providerBT[i].getProviderName()))
	   {
		_providerBT[i].setProviderBlockFlag(false);
		return (0);
	   }	
	}

	return (1);
}

Boolean ProviderManager::isProviderBlocked(const String & providerName)
{
	for (Uint32 i=0, n=_providerBT.size(); i<n; i++)
	{
	   if(String::equalNoCase(providerName,_providerBT[i].getProviderName()))
	   {
		return(_providerBT[i].getProviderBlockFlag());
	   }	
	}
	return (false);
}

void ProviderManager::createProviderBlockTable(Array<CIMNamedInstance> & namedinstances)
{

	String providerName;
	Boolean blockFlag;
	CIMInstance instance;
	
	for(Uint32 i = 0, n = namedinstances.size(); i < n; i++)
	{
	    instance = namedinstances[i].getInstance(); 
	    providerName = instance.getProperty(
             instance.findProperty("Name")).getValue().toString();
	    instance.getProperty(instance.findProperty("blocked")).
		getValue().get(blockFlag);
	    _addProviderToTable(providerName, blockFlag);
	}
}

Uint32 ProviderManager::_stopProvider(Uint32 providerIndex)
{
    // Terminate the provider, unload its library, and remove its entry
    _providers[providerIndex].getProvider()->terminate();
    // ATTN: Only unload if this is the last provider loaded from this library
    _providers[providerIndex].unload();
    _providers.remove(providerIndex);
    return(0);
}
    
Uint32 ProviderManager::stopProvider(const String & providerName)
{
    // check list for requested provider. If found, terminate the
    // provider and unload library
    for(Uint32 i = 0, n = _providers.size(); i < n; i++)
    {
        if(String::equalNoCase(providerName, _providers[i].getProviderName()))
        {
	    return _stopProvider(i);
        }
    }
    
    // if provider is not loaded, just return
    return (0);
}
    
void ProviderManager::shutdownAllProviders(const String & providerName, const String & className)
{
    //
    // For each provider in the list, call its terminate() method, skipping
    // the specified provider.
    //
    for(Uint32 i = 0, n = _providers.size(); i < n; i++)
    {
        if ( !(String::equalNoCase(providerName, _providers[i].getProviderName()) &&
               String::equalNoCase(className, _providers[i].getClassName())))
        {
	    _stopProvider(i);
        }
    }
}
    
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

        for (Uint32 i = _this->_providers.size(); i > 0; )
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
                PEGASUS_STD(cout) << "unloading provider for " << _this->_providers[i].getClassName() << " in " << _this->_providers[i].getProviderName() << PEGASUS_STD(endl);
                void * mypr = (void *)_this->_providers[i].getProvider();

                _this->_providers[i].getProvider()->terminate();
                _this->_providers.remove(i);
            }
        }
    }
    
    PEGASUS_STD(cout) << "provider monitor stopped" << PEGASUS_STD(endl);
    
    return(0);
}

PEGASUS_NAMESPACE_END
