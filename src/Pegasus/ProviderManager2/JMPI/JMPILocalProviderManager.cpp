//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000 - 2003 BMC Software, Hewlett-Packard Company, IBM,
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
//              Mike Day IBM Corporation (mdday@us.ibm.com)
//              Adrian Schuur, schuur@de.ibm.com
//
//%/////////////////////////////////////////////////////////////////////////////

#include "JMPILocalProviderManager.h"

#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/PegasusVersion.h>

#include <Pegasus/ProviderManager2/JMPI/JMPIProvider.h>
#include <Pegasus/ProviderManager2/JMPI/JMPIProviderModule.h>
#include <Pegasus/ProviderManager2/ProviderManagerService.h>

PEGASUS_USING_STD;
PEGASUS_NAMESPACE_BEGIN

JMPILocalProviderManager::JMPILocalProviderManager(void)
    : _idle_timeout(IDLE_LIMIT)
{
}

JMPILocalProviderManager::~JMPILocalProviderManager(void)
{
    Uint32 ccode;

    _provider_ctrl(UNLOAD_ALL_PROVIDERS, this, &ccode);
}

Sint32 JMPILocalProviderManager::_provider_ctrl(CTRL code, void *parm, void *ret)
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

            JMPIProvider *pr = NULL;
            JMPIProvider::OpProviderHolder* ph =
	          reinterpret_cast< JMPIProvider::OpProviderHolder* >( ret );


            if(true == _providers.lookup( *(parms->providerName), pr ))
            {
                PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                    "Found JMPIProvider " + *(parms->providerName) + " in JMPIProvider Manager Cache");

                ph->SetProvider( pr );
//                ph->GetProvider().update_idle_timer();
                break;
            }


            PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                "Creating JMPIProvider " + *(parms->providerName) );
            JMPIProviderModule *module;

            if(false  == _modules.lookup(*(parms->fileName), module))
            {
                PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                    "Creating JMPIProvider Module " + *(parms->fileName) );

                module = new JMPIProviderModule(*(parms->fileName),*(parms->interfaceName));

                _modules.insert((*parms->fileName), module);
            }
            else
            {
                PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                    "Using Cached  JMPIProvider Module " + *(parms->fileName) );
            }


            PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                "Loading/Linking JMPIProvider Module " + *(parms->fileName) );

            ProviderVector base ;
            try
            {
                base = module->load(*(parms->fileName));
            }
            catch(...)
            {
                PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                    "Exception caught Loading/Linking JMPIProvider Module " + *(parms->fileName) );
                throw;
            }

            // create provider module

            MessageQueue * queue = MessageQueue::lookup(PEGASUS_QUEUENAME_PROVIDERMANAGER_CPP);
            PEGASUS_ASSERT(queue != 0);
            MessageQueueService * service = dynamic_cast<MessageQueueService *>(queue);
            PEGASUS_ASSERT(service != 0);
            pr = new JMPIProvider(*(parms->providerName), module, &base);
            if(0 == (pr->_cimom_handle =  new CIMOMHandle()))
                throw NullPointer();
            pr->_quantum=0;


            PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL2,
                "Loading JMPIProvider " +  pr->_name);
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

//            pr->update_idle_timer();

            _providers.insert(*(parms->providerName), pr);


            ph->SetProvider( pr );
            break;
        }

    case UNLOAD_PROVIDER:
        { 
	    break;
        }

    case LOOKUP_PROVIDER:
        {

            PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL2,
                "_provider_ctrl::LOOKUP_PROVIDER");

            if(true == _providers.lookup(*(parms->providerName),
                *(reinterpret_cast<JMPIProvider * *>(ret))))
            {
                PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                    "Found JMPIProvider in cache: " +
                    *(parms->providerName));

//                (*(reinterpret_cast<JMPIProvider * *>(ret)))->update_idle_timer();
            }
            else
            {

                PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                    "Could not find  JMPIProvider in cache: " +
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
                *(reinterpret_cast<JMPIProviderModule * *>(ret))))
            {
                PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                    "Could not find  JMPIProvider Module in cache: " +
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
                *reinterpret_cast<JMPIProvider * *>(parm)))
                ccode = -1;
            break;
        }
    case INSERT_MODULE:
        {
            PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL2,
                "_provider_ctrl::INSERT_MODULE");
            if(false  == _modules.insert(
                *(parms->fileName),
                *reinterpret_cast<JMPIProviderModule * *>(parm)))
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
           JMPIjvm::destroyJVM();

	   break;
        }
	
    case UNLOAD_IDLE_PROVIDERS:
         { 
	    break;
        }

    case UNLOAD_IDLE_MODULES:
        { 
	    break;
        }

    default:
        ccode = -1;
        break;
    }
    PEG_METHOD_EXIT();
    return(ccode);
}

JMPIProvider * JMPILocalProviderManager::_getResolver
   (const String & fileName, const String & interfaceType)
{
    JMPIProvider *pr;
    String id(interfaceType+String("::")+fileName);
    if (true == _resolvers.lookup(id,pr)) {
       return pr;
    }
    return NULL;
}
/*
CMPIResolverModule *JMPILocalProviderManager::_loadResolver
   (const String & fileName)
{
   CMPIResolverModule *rm=new CMPIResolverModule(fileName);
   rm->load();
   return rm;
}
*/
JMPIProvider::OpProviderHolder JMPILocalProviderManager::getProvider(
    const String & fileName,
    const String & providerName,
    const String & interfaceName)
{
    JMPIProvider::OpProviderHolder ph;
    CTRL_STRINGS strings;
    Sint32 ccode;
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "ProviderManager::getProvider");
    strings.fileName = &fileName;
    strings.providerName = &providerName;
    strings.interfaceName = &interfaceName;

    try {
        ccode = _provider_ctrl( GET_PROVIDER, &strings, &ph );
    }
    catch (Exception e) {
       cerr<<"--- loading proxy: "<<e.getMessage()<<endl;
       PEG_METHOD_EXIT();
        throw;
   }
    catch(...) {
        PEG_METHOD_EXIT();
        throw;
    }


    PEG_METHOD_EXIT();
    return(ph);

}

void JMPILocalProviderManager::unloadProvider(
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

void JMPILocalProviderManager::shutdownAllProviders(void)
{

    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "ProviderManager::shutdownAllProviders");
    _provider_ctrl(UNLOAD_ALL_PROVIDERS, (void *)this, (void *)0);
    PEG_METHOD_EXIT();
}


Boolean JMPILocalProviderManager::hasActiveProviders()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "ProviderManager::hasActiveProviders");

    PEG_METHOD_EXIT();
    return (_providers.size() > 0);
}

void JMPILocalProviderManager::unloadIdleProviders()
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

PEGASUS_NAMESPACE_END
