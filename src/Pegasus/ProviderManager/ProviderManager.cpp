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

/*****

<< Fri Oct 11 16:26:12 2002 mdd >>


DO NOT delete the provider handle UnloadProvider() 

// << Fri Oct 11 16:32:11 2002 mdd >>

// CHANGED this algorithm to FIRST terminate each provider THEN remove each provider. 

// The array traversal was not working as expected originally when all of the processing
// was in one loop. providers were having their terminate functions cal

****/


#include "ProviderManager.h"

#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/PegasusVersion.h>

PEGASUS_NAMESPACE_BEGIN

ProviderManager *my_instance = 0;


ProviderManager::ProviderManager(void)
   : _idle_timeout(300), _unload_idle_flag(1)
{
   my_instance = this;
   
}

ProviderManager::~ProviderManager(void)
{
   // terminate providers
}


Sint32 ProviderManager::_provider_ctrl(CTRL code, void *parm, void *ret)
{
   static Mutex _monitor;
   Sint32 ccode = 0;
   CTRL_STRINGS *parms = reinterpret_cast<CTRL_STRINGS *>(parm);
   _monitor.lock(pegasus_thread_self());
   switch(code)
   {

      case GET_PROVIDER:
      {
	 Provider *pr;
	 
	 if(true == _providers.lookup(*(parms->providerName), *(reinterpret_cast<Provider * *>(ret))))
	 {
	    gettimeofday(&(*(reinterpret_cast<Provider * *>(ret)))->_timeout, NULL);
	    break;
	 }
	 
	 ProviderModule *module;
	 if( false  == _modules.lookup(*(parms->fileName), module) )
	 {
	    module = new ProviderModule(*(parms->fileName));
	    _modules.insert((*parms->fileName), module);
	 }
	 
	 CIMBaseProvider *base = module->load(*(parms->providerName));
   
	 // create provider module
   
	 MessageQueue * queue = MessageQueue::lookup(PEGASUS_QUEUENAME_PROVIDERMANAGER_CPP);
	 PEGASUS_ASSERT(queue != 0);
	 MessageQueueService * service = dynamic_cast<MessageQueueService *>(queue);
	 PEGASUS_ASSERT(service != 0);
	 pr = new Provider(*(parms->providerName), module, base);
	 if(0 == (pr->_cimom_handle =  new CIMOMHandle(service)))
	    throw NullPointer();

	 PEGASUS_STD(cout) << "Loading Provider " << pr->_name << PEGASUS_STD(endl);
	 
	 pr->initialize(*(pr->_cimom_handle));
	 gettimeofday(&(pr->_timeout), NULL);
	 _providers.insert(*(parms->providerName), pr);
	 *(reinterpret_cast<Provider * *>(ret)) = pr;
	 break;
      }
      
      case UNLOAD_PROVIDER:
      {
	 CTRL_STRINGS *parms = reinterpret_cast<CTRL_STRINGS *>(parm);
	 Provider *pr;
	 if(true == _providers.lookup(*(parms->providerName), pr ))
	 {
	    PEGASUS_STD(cout) << "Unloading Provider " << pr->_name << PEGASUS_STD(endl);
	    _providers.remove(pr->_name);
	    pr->terminate();
	    if((pr->_module != 0 ) && pr->_module->_ref_count.value() == 0)
	      {
		pr->_module->unloadModule();
		_modules.remove(pr->_module->_fileName);
		delete pr->_module;
	      }
	    delete pr->_cimom_handle;
	    delete pr;
	 }
      }

      case LOOKUP_PROVIDER:
      {
	 if(true == _providers.lookup(*(parms->providerName), 
				      *(reinterpret_cast<Provider * *>(ret))))
	 {
	    gettimeofday(&((*(reinterpret_cast<Provider * *>(ret)))->_timeout), NULL);
	 }
	 else
	    ccode = -1;
	 break;
      }
      
      case LOOKUP_MODULE:
      {
	 if(false  == _modules.lookup(*(parms->fileName), 
				      *(reinterpret_cast<ProviderModule * *>(ret))))
	   ccode = -1;
	 break;
      }

      case INSERT_PROVIDER: 
      {
	 if( false  == _providers.insert(
		*(parms->providerName), 
		*reinterpret_cast<Provider * *>(parm)) )
	    ccode = -1;
	 break;
      }
      case INSERT_MODULE: 
      {
	 if( false  == _modules.insert(
		*(parms->fileName), 
		*reinterpret_cast<ProviderModule * *>(parm)) )
	    ccode = -1;
	 break;
      }
      case REMOVE_PROVIDER:
      {

	 if(false == _providers.remove(*(parms->providerName)))
	    ccode = -1;
	 break;
      }
      case REMOVE_MODULE:
      {
	 if(false == _modules.remove(*(parms->fileName)))
	    ccode = -1;
	 break;
      }

      case UNLOAD_ALL_PROVIDERS:
      {
	 ProviderManager *myself = reinterpret_cast<ProviderManager *>(parm);
	 Provider * provider;
	 ProviderTable::Iterator i = myself->_providers.start();
	 for(; i ; i++)
	 {
	    provider = i.value();
	    PEGASUS_STD(cout) << "Removing Provider " << provider->getName() << PEGASUS_STD(endl);
	    provider->terminate();
	    if((provider->_module != 0 ) && 
	       provider->_module->_ref_count.value() == 0)
	      {
		if(true == _modules.lookup(provider->_module->_fileName, provider->_module))
		  {
		    PEGASUS_STD(cout) << "Removing Module " << provider->_module->_fileName << PEGASUS_STD(endl);
		    _modules.remove(provider->_module->_fileName);
		    delete provider->_module;
		  }
	      }
	    delete provider->_cimom_handle;
	    delete provider;
	 }
	 myself->_providers.clear();
	 myself->_modules.clear();
	 break;
	 
      }

      case UNLOAD_IDLE_PROVIDERS:
      {
	 ProviderManager *myself = reinterpret_cast<ProviderManager *>(parm);
	 Provider * provider;
	 if(myself->_providers.size())
	 {
	    struct timeval now;
	    gettimeofday(&now, NULL);
	    ProviderTable::Iterator i = myself->_providers.start();
	    for(; i ; i++)
	    {
	       provider = i.value();
	       if(( now.tv_sec - provider->_timeout.tv_sec) > (Sint32)myself->_idle_timeout )
	       {
		  PEGASUS_STD(cout) << "Removing Provider " << provider->getName() << PEGASUS_STD(endl);
		  provider->terminate();
		  myself->_providers.remove(provider->_name);
		  PEGASUS_STD(cout) << "Removed, looking at Module." << PEGASUS_STD(endl);
		  if(provider->_module != 0 )
		    PEGASUS_STD(cout) << "Module ref: " << provider->_module->_ref_count.value() << PEGASUS_STD(endl);
		  if((provider->_module != 0 ) && 
		     provider->_module->_ref_count.value() == 0)
		    {
		      PEGASUS_STD(cout) << "Removing Module " << provider->_module->_fileName << PEGASUS_STD(endl);
		      _modules.remove(provider->_module->_fileName);
		      delete provider->_module;
		    }
		  delete provider;
		  delete provider->_cimom_handle;
		  i = myself->_providers.start();
	       }
	    }
	 }
	 break;
      }

      case UNLOAD_IDLE_MODULES:
      {
	 ProviderManager *myself = reinterpret_cast<ProviderManager *>(parm);
	 ProviderModule *module;
	 ModuleTable::Iterator i = myself->_modules.start();
	 for( ; i ; i++)
	 {
	    module = i.value();
	    if( module->_ref_count.value() == 0)
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

   _monitor.unlock();
   return ccode;
}




Provider ProviderManager::getProvider(
    const String & fileName,
    const String & providerName,
    const String & interfaceName)
{

   Provider *provider = 0;
   CTRL_STRINGS strings;
   Sint32 ccode;
   
   strings.fileName = &fileName;
   strings.providerName = &providerName;
   strings.interfaceName = &interfaceName;

   ccode = _provider_ctrl(GET_PROVIDER, &strings, &provider);
   
   return *provider;
   
}

void ProviderManager::unloadProvider(
    const String & fileName,
    const String & providerName)
{
   CTRL_STRINGS strings;
   
   strings.fileName = &fileName;
   strings.providerName = &providerName;

   _provider_ctrl(UNLOAD_PROVIDER, &strings, (void *)0);
}

void ProviderManager::shutdownAllProviders(void)
{
 
   PEGASUS_STD(cout) << "ProviderManager::shutdownAllProviders" << PEGASUS_STD(endl);
   
   _provider_ctrl(UNLOAD_ALL_PROVIDERS, (void *)this, (void *)0);
   
}

void ProviderManager::unload_idle_providers(void)
{
   static struct timeval first = {0,0}, now, last = {0,0};
   if(first.tv_sec == 0)
      gettimeofday(&first, NULL);
   gettimeofday(&now, NULL);
   if(((now.tv_sec - first.tv_sec) > 10 ) && ( (now.tv_sec - last.tv_sec) > 10)) 
   {
      gettimeofday(&last, NULL);
      if(_unload_idle_flag.value() == 1 )
      { 
	 _unload_idle_flag = 0;
	 Thread th(provider_monitor, this , true);
	 th.run();
      }
   }
}


PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL ProviderManager::provider_monitor(void *parm)
{
   Thread *th = reinterpret_cast<Thread *>(parm);
   
   ProviderManager *myself =    my_instance ;
      
   PEGASUS_STD(cout) << "Checking for Idle providers to unload. (I currently have " << 
      myself->_providers.size() << " providers loaded)." << PEGASUS_STD(endl);
   try 
   {
      myself->_provider_ctrl(UNLOAD_IDLE_PROVIDERS, myself, (void *)0);
      myself->_unload_idle_flag = 1;

   }
   catch(...)
   {
   }
   exit_thread((PEGASUS_THREAD_RETURN)1);
   return 0;
}

PEGASUS_NAMESPACE_END
