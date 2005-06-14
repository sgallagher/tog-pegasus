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

int JMPILocalProviderManager::trace=0;

#ifdef PEGASUS_DEBUG
#define DDD(x) if (JMPILocalProviderManager::trace) x;
#else
#define DDD(x)
#endif

JMPILocalProviderManager::JMPILocalProviderManager(void)
    : _idle_timeout(IDLE_LIMIT)
{
#ifdef PEGASUS_DEBUG
   if (getenv("PEGASUS_JMPI_TRACE"))
      JMPILocalProviderManager::trace=1;
   else
      JMPILocalProviderManager::trace=0;
#else
   JMPILocalProviderManager::trace=0;
#endif
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
            PEG_TRACE_STRING(TRC_PROVIDERMANAGER,
                             Tracer::LEVEL2,
                             "_provider_ctrl::GET_PROVIDER");

            String providerName = *(parms->providerName);
            String moduleFileName = *(parms->fileName);
            String interfaceName = *(parms->interfaceName);

            DDD(PEGASUS_STD(cout)
                <<"--- JMPILocalProviderManager::_provider_ctrl: GET_PROVIDER "
                <<providerName
                <<PEGASUS_STD(endl));

            JMPIProvider::OpProviderHolder* ph =
               reinterpret_cast< JMPIProvider::OpProviderHolder* >( ret );
            JMPIProviderModule *newModule = NULL;
            JMPIProviderModule *module = NULL;
            JMPIProvider *newProvider = NULL;
            JMPIProvider *provider = NULL;
            ProviderVector base;

            try
            {
               {
                  AutoMutex lock (_providerTableMutex);

                  if (true == _providers.lookup(providerName, provider))
                  {
                      PEG_TRACE_STRING(TRC_PROVIDERMANAGER,
                                       Tracer::LEVEL4,
                                       "Found JMPIProvider "
                                       + providerName
                                       + " in JMPIProvider Manager Cache");
                      DDD(PEGASUS_STD(cout)
                          <<"--- JMPILocalProviderManager::_provider_ctrl: Found "
                          <<providerName
                          <<" in JMPIProvider Manager Cache"
                          <<PEGASUS_STD(endl));
                      DDD(PEGASUS_STD(cout)
                          <<"--- JMPILocalProviderManager::_provider_ctrl:"
                            " setting provider to "
                          <<PEGASUS_STD(hex)
                          <<(int)provider
                          <<PEGASUS_STD(dec)
                          <<PEGASUS_STD(endl));

                      ph->SetProvider(provider);

//////////////////////ph->GetProvider().update_idle_timer();
                      break;
                  }

                  PEG_TRACE_STRING(TRC_PROVIDERMANAGER,
                                   Tracer::LEVEL4,
                                   "Creating JMPIProvider " + providerName );
                  DDD(PEGASUS_STD(cout)
                      <<"--- JMPILocalProviderManager::_provider_ctrl: Creating "
                      <<providerName
                      <<PEGASUS_STD(endl));

                  if (false == _modules.lookup(moduleFileName, module))
                  {
                      PEG_TRACE_STRING(TRC_PROVIDERMANAGER,
                                       Tracer::LEVEL4,
                                       "Creating JMPIProvider Module "
                                       + moduleFileName );
                      DDD(PEGASUS_STD(cout)
                          <<"--- JMPILocalProviderManager::_provider_ctrl: "
                            "Creating module "
                            <<moduleFileName
                            <<PEGASUS_STD(endl));

                      newModule = new JMPIProviderModule(moduleFileName,
                                                         interfaceName);

                      if (0 == newModule)
                      {
                          PEG_TRACE_STRING(TRC_PROVIDERMANAGER,
                                           Tracer::LEVEL4,
                                           "new JMPIProviderModule is NULL!");
                          DDD(PEGASUS_STD(cout)
                              <<"--- JMPILocalProviderManager::_provider_ctrl:"
                                " new JMPIProviderModule is NULL!"
                                <<PEGASUS_STD(endl));

                          throw NullPointer();
                      }

                      module = newModule;

                      _modules.insert(moduleFileName, module);
                  }
                  else
                  {
                      PEG_TRACE_STRING(TRC_PROVIDERMANAGER,
                                       Tracer::LEVEL4,
                                       "Using Cached JMPIProvider Module "
                                       + moduleFileName);
                      DDD(PEGASUS_STD(cout)
                          <<"--- JMPILocalProviderManager::_provider_ctrl: "
                          "Using cached "
                          <<moduleFileName
                          <<PEGASUS_STD(endl));
                  }
               }

               PEG_TRACE_STRING(TRC_PROVIDERMANAGER,
                                Tracer::LEVEL4,
                                "Loading/Linking JMPIProvider Module "
                                + moduleFileName );
               DDD(PEGASUS_STD(cout)
                   <<"--- JMPILocalProviderManager::_provider_ctrl:"
                     " Loading/Linking module "
                     <<moduleFileName
                     <<PEGASUS_STD(endl));

               try
               {
                   base = module->load(moduleFileName);
               }
               catch(...)
               {
                   PEG_TRACE_STRING(TRC_PROVIDERMANAGER,
                                    Tracer::LEVEL4,
                                    "Exception caught Loading/Linking"
                                    " JMPIProvider Module "
                                    + moduleFileName );
                   DDD(PEGASUS_STD(cout)
                       <<"--- JMPILocalProviderManager::_provider_ctrl: "
                       "Exception caught Loading/Linking module "
                       <<moduleFileName
                       <<PEGASUS_STD(endl));

                   throw;
               }

               // create provider module
               newProvider = new JMPIProvider(providerName, module, &base);
               if (0 == newProvider)
               {
                   PEG_TRACE_STRING(TRC_PROVIDERMANAGER,
                                    Tracer::LEVEL4,
                                    "new JMPIProvider is NULL!");
                   DDD(PEGASUS_STD(cout)
                       <<"--- JMPILocalProviderManager::_provider_ctrl: "
                         "new JMPIProvider is NULL!"
                       <<PEGASUS_STD(endl));

                   throw NullPointer();
               }

               provider = newProvider;

               if (0 == (provider->_cimom_handle = new CIMOMHandle()))
               {
                   PEG_TRACE_STRING(TRC_PROVIDERMANAGER,
                                    Tracer::LEVEL4,
                                    "_cimom_handle is NULL!");
                   DDD(PEGASUS_STD(cout)
                       <<"--- JMPILocalProviderManager::_provider_ctrl:"
                         " _cimom_handle is NULL!"
                         <<PEGASUS_STD(endl));

                   throw NullPointer();
               }

               provider->_quantum = 0;

               {
                  AutoMutex lock(provider->_statusMutex);

                  PEG_TRACE_STRING(TRC_PROVIDERMANAGER,
                                   Tracer::LEVEL2,
                                   "Loading JMPIProvider "
                                   +  provider->_name);
                  DDD(PEGASUS_STD(cout)
                      <<"--- JMPILocalProviderManager::_provider_ctrl:"
                        " Loading "
                        <<provider->_name
                        <<PEGASUS_STD(endl));
                  try
                  {
                      provider->initialize(*(provider->_cimom_handle));
                  }
                  catch(...)
                  {
                      PEG_TRACE_STRING(TRC_PROVIDERMANAGER,
                                       Tracer::LEVEL4,
                                       "Exception caught calling initialize!");
                      DDD(PEGASUS_STD(cout)
                          <<"--- JMPILocalProviderManager::_provider_ctrl:"
                            " Exception caught calling initialize!"
                            <<PEGASUS_STD(endl));

                      throw UninitializedObjectException();
                  }
               }
            }
            catch (...)
            {
               if (newModule)
               {
                  _modules.remove(moduleFileName);
               }

               delete newModule;

               if (newProvider)
               {
                  delete newProvider->_cimom_handle;
               }

               delete newProvider;

               throw;
            }


//          provider->update_idle_timer();

            _providers.insert(providerName, provider);

            DDD(PEGASUS_STD(cout)
                <<"--- JMPILocalProviderManager::_provider_ctrl:"
                  "setting provider to "
                  <<PEGASUS_STD(hex)
                  <<(int)provider
                  <<PEGASUS_STD(dec)
                  <<PEGASUS_STD(endl));

            ph->SetProvider( provider );
            break;
        }

    case UNLOAD_PROVIDER:
        {
            DDD(PEGASUS_STD(cout)
                <<"--- JMPILocalProviderManager::_provider_ctrl: "
                  "UNLOAD_PROVIDER"
                  <<PEGASUS_STD(endl));
            break;
        }

    case LOOKUP_PROVIDER:
        {
            PEG_TRACE_STRING(TRC_PROVIDERMANAGER,
                             Tracer::LEVEL2,
                             "_provider_ctrl::LOOKUP_PROVIDER");
            DDD(PEGASUS_STD(cout)
                <<"--- JMPILocalProviderManager::_provider_ctrl: "
                "LOOKUP_PROVIDER "
                <<*(parms->providerName)
                <<PEGASUS_STD(endl));

            AutoMutex lock (_providerTableMutex);

            if (true == _providers.lookup(*(parms->providerName),
                                   *(reinterpret_cast<JMPIProvider * *>(ret))))
            {
                PEG_TRACE_STRING(TRC_PROVIDERMANAGER,
                                 Tracer::LEVEL4,
                                 "Found JMPIProvider in cache: "
                                 + *(parms->providerName));

////////////////(*(reinterpret_cast<JMPIProvider * *>(ret)))->update_idle_timer();
            }
            else
            {
                PEG_TRACE_STRING(TRC_PROVIDERMANAGER,
                                 Tracer::LEVEL4,
                                 "Could not find  JMPIProvider in cache: "
                                 + *(parms->providerName));
                ccode = -1;
            }

            break;
        }

    case LOOKUP_MODULE:
        {
            PEG_TRACE_STRING(TRC_PROVIDERMANAGER,
                             Tracer::LEVEL2,
                             "_provider_ctrl::LOOKUP_MODULE");
            DDD(PEGASUS_STD(cout)
                <<"--- JMPILocalProviderManager::_provider_ctrl: "
                  "LOOKUP_MODULE "
                <<*(parms->fileName)
                <<PEGASUS_STD(endl));

            AutoMutex lock (_providerTableMutex);

            if (false  == _modules.lookup(*(parms->fileName),
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
            PEG_TRACE_STRING(TRC_PROVIDERMANAGER,
                             Tracer::LEVEL2,
                             "_provider_ctrl::INSERT_PROVIDER");
            DDD(PEGASUS_STD(cout)
                <<"--- JMPILocalProviderManager::_provider_ctrl: "
                  "INSERT_PROVIDER "
                <<*(parms->providerName)
                <<PEGASUS_STD(endl));

            AutoMutex lock (_providerTableMutex);

            if (false  == _providers.insert(*(parms->providerName),
                                    *reinterpret_cast<JMPIProvider * *>(parm)))
                ccode = -1;
            break;
        }

    case INSERT_MODULE:
        {
            PEG_TRACE_STRING(TRC_PROVIDERMANAGER,
                             Tracer::LEVEL2,
                             "_provider_ctrl::INSERT_MODULE");
            DDD(PEGASUS_STD(cout)
                <<"--- JMPILocalProviderManager::_provider_ctrl: "
                  "INSERT_MODULE "
                <<*(parms->fileName)
                <<PEGASUS_STD(endl));

            AutoMutex lock (_providerTableMutex);

            if(false  == _modules.insert(*(parms->fileName),
                              *reinterpret_cast<JMPIProviderModule * *>(parm)))
                ccode = -1;
            break;
        }

    case REMOVE_PROVIDER:
        {
            PEG_TRACE_STRING(TRC_PROVIDERMANAGER,
                             Tracer::LEVEL2,
                             "_provider_ctrl::REMOVE_PROVIDER");
            DDD(PEGASUS_STD(cout)
                <<"--- JMPILocalProviderManager::_provider_ctrl: "
                  "REMOVE_PROVIDER "
                <<*(parms->providerName)
                <<PEGASUS_STD(endl));

            AutoMutex lock (_providerTableMutex);

            if (false == _providers.remove(*(parms->providerName)))
                ccode = -1;
            break;
        }

    case REMOVE_MODULE:
        {
            PEG_TRACE_STRING(TRC_PROVIDERMANAGER,
                             Tracer::LEVEL2,
                             "_provider_ctrl::REMOVE_MODULE");
            DDD(PEGASUS_STD(cout)
                <<"--- JMPILocalProviderManager::_provider_ctrl: REMOVE_MODULE "
                <<*(parms->fileName)
                <<PEGASUS_STD(endl));

            AutoMutex lock (_providerTableMutex);

            if (false == _modules.remove(*(parms->fileName)))
                ccode = -1;
            break;
        }

    case UNLOAD_ALL_PROVIDERS:
        {
           DDD(PEGASUS_STD(cout)
               <<"--- JMPILocalProviderManager::_provider_ctrl: "
                 "UNLOAD_ALL_PROVIDERS"
               <<PEGASUS_STD(endl));

           JMPIjvm::destroyJVM();
           break;
        }

    case UNLOAD_IDLE_PROVIDERS:
         {
           DDD(PEGASUS_STD(cout)
               <<"--- JMPILocalProviderManager::_provider_ctrl: "
                 "UNLOAD_IDLE_PROVIDERS"
               <<PEGASUS_STD(endl));
           break;
        }

    case UNLOAD_IDLE_MODULES:
        {
           DDD(PEGASUS_STD(cout)
               <<"--- JMPILocalProviderManager::_provider_ctrl: "
                 "UNLOAD_IDLE_MODULES"
               <<PEGASUS_STD(endl));
           break;
        }

    default:
        {
           DDD(PEGASUS_STD(cout)
               <<"--- JMPILocalProviderManager::_provider_ctrl: unknown! "
               <<code
               <<PEGASUS_STD(endl));
           ccode = -1;
           break;
        }
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

JMPIProvider::OpProviderHolder JMPILocalProviderManager::getProvider(
    const String & fileName,
    const String & providerName,
    const String & interfaceName)
{
    JMPIProvider::OpProviderHolder ph;
    CTRL_STRINGS strings;
    Sint32 ccode;
    strings.fileName = &fileName;
    strings.providerName = &providerName;
    strings.interfaceName = &interfaceName;

    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "ProviderManager::getProvider");

    try {
        ccode = _provider_ctrl( GET_PROVIDER, &strings, &ph );
    }
    catch (const Exception &e) {
        DDD(PEGASUS_STD(cout)
            <<"--- JMPILocalProviderManager::getProvider: loading proxy: "
            <<e.getMessage()
            <<PEGASUS_STD(endl));
        PEG_METHOD_EXIT();
        throw;
    }
    catch(...) {
        DDD(PEGASUS_STD(cout)
            <<"--- JMPILocalProviderManager::getProvider: catch (...) "
            <<PEGASUS_STD(endl));
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
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "ProviderManager::hasActiveProviders");

    AutoMutex lock (_providerTableMutex);

    Boolean fRet = _providers.size() > 0;

    PEG_METHOD_EXIT();
    return fRet;
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
            AutoMutex lock(_providerTableMutex);

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

Array <JMPIProvider *>
JMPILocalProviderManager::getIndicationProvidersToEnable ()
{
    PEG_METHOD_ENTER (TRC_PROVIDERMANAGER,
        "JMPILocalProviderManager::getIndicationProvidersToEnable");

    Array <JMPIProvider *> enableProviders;

    try
    {
        AutoMutex lock (_providerTableMutex);

        Tracer::trace (TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Number of providers in _providers table = %d", _providers.size ());

        //
        // Iterate through the _providers table
        //
        for (ProviderTable::Iterator i = _providers.start (); i != 0; i++)
        {
            //
            //  Enable any indication provider with current subscriptions
            //
            JMPIProvider * provider = i.value ();
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


PEGASUS_NAMESPACE_END
