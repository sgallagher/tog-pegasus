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
//
//%/////////////////////////////////////////////////////////////////////////////

#include "ProviderManager.h"

PEGASUS_NAMESPACE_BEGIN

ProviderManager::ProviderManager(MessageQueue * outputQueue, CIMRepository * repository) : _cimom(outputQueue, repository)
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
    provider->initialize(_cimom);
    
    // add provider to list
    _providers.append(module);

    // recurse to get the provider as it resides in the array (rather than the local instance)
    return(getProvider(providerName, className));
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
        for(Uint32 i = _this->_providers.size()-1; i >= 0; i--)
        {
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
