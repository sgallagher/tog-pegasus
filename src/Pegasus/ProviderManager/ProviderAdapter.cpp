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
// Author: Markus Mueller (sedgewick_de@yahoo.de)
//
// Modified By: Adrian Schuur - schuur@de.ibm.com
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/Destroyer.h>
#include <Pegasus/Common/Sharable.h>
#include <Pegasus/Common/IPC.h>

#include <Pegasus/Provider/CIMProvider.h>
#include <Pegasus/Provider/CIMOMHandle.h>
#include "ProviderAdapter.h"
#include <Pegasus/Config/ConfigManager.h>
#include <Pegasus/Common/MessageLoader.h> //l10n

#define PROVIDERADAPTER_DEBUG(X) //X

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

// HACK
static ProviderAdapterManager * pamgr;

//
//
//

ProviderAdapter::ProviderAdapter(const String & adapterName,
                                 const String & providerLocation,
                                 const String & providerName) :
    _adapterName(adapterName), _providerLocation(providerLocation),
    _providerName(providerName)
{
PROVIDERADAPTER_DEBUG( cerr << "ProviderAdapter::ProviderAdapter "<<
     _adapterName<<" "<<_providerLocation<<"\n"; )
}

ProviderAdapter::~ProviderAdapter()
{
PROVIDERADAPTER_DEBUG( cerr << "ProviderAdapter::~ProviderAdapter\n"; )
    ProviderAdapterManager::get_pamgr()->deleteAdapter(_adapterName);
}

const String & ProviderAdapter::getAdapterName() const {return _adapterName;}
const String & ProviderAdapter::getProviderName() const {return _providerLocation;}
CIMProvider * ProviderAdapter::getProvider(void) const {return _adapter;}

//
//
//

ProviderAdapterManager::ProviderAdapterManager(void)
{
PROVIDERADAPTER_DEBUG( cerr << "ProviderAdapterManager::ProviderAdapterManager\n"; )
}

ProviderAdapterManager::~ProviderAdapterManager(void)
{
PROVIDERADAPTER_DEBUG( cerr << "ProviderAdapterManager::~ProviderAdapterManager\n"; )
    // unload all adapter modules
    _listMutex.lock(pegasus_thread_self());

    for(Uint32 i=0,n=_listOfAdapterLibs.size();i<n;i++)
        System::unloadDynamicLibrary(_listOfAdapterLibs[i]);

    _listOfAdapterNames.clear();
    _listOfAdapterCounts.clear();
    _listOfAdapterLibs.clear();
    _listMutex.unlock();
}

ProviderAdapterManager * ProviderAdapterManager::get_pamgr()
{
PROVIDERADAPTER_DEBUG( cerr << "ProviderAdapterManager::get_pamgr()\n"; )
     // HACK
     if (pamgr == 0) pamgr = new ProviderAdapterManager();
     return pamgr;
}

ProviderAdapter * ProviderAdapterManager::addAdapter(
                                 const String & adapterName,
                                 const String & adapterFileName,
                                 const String & providerLocation,
                                 const String & providerName)
{
PROVIDERADAPTER_DEBUG( cerr << "ProviderAdapterManager::addAdapter(): "
       <<adapterName<<" "<<providerLocation<<" "<<providerName<<"\n"; )

   _listMutex.lock(pegasus_thread_self());
   ProviderAdapter *pad=NULL;
   DynamicLibraryHandle adapter;

   // lookup adapterName
   Uint32 n = _listOfAdapterNames.size();

   if (n == 0) {
       PROVIDERADAPTER_DEBUG (
          cerr<<"ProviderAdapterManager::addAdapter(): No entries -> load adapter\n";)
       try {
           adapter=loadAdapter(adapterName, adapterFileName);
           pad=loadProvider(adapter,adapterName,providerLocation,providerName);
           _listOfAdapterLibs.append(adapter);
           _listOfAdapterNames.append(adapterName);
           _listOfAdapterCounts.append(1);
       }
       catch (Exception & e) {
            PROVIDERADAPTER_DEBUG (cerr << e.getMessage();)
       }
       _listMutex.unlock();
       return pad;
   }

   for(Uint32 i=0,n=_listOfAdapterNames.size();i<n;i++)
   {
       if (String::equal(_listOfAdapterNames[i],adapterName))
       {
           PROVIDERADAPTER_DEBUG(
	     cerr<<"ProviderAdapterManager::addAdapter(): Found adapter entry\n";)
           _listOfAdapterCounts[i]++;

           adapter=_listOfAdapterLibs[i];
           try {
               pad=loadProvider(adapter,adapterName,providerLocation,providerName);
           }
           catch (Exception & e) {
               PROVIDERADAPTER_DEBUG (cerr << e.getMessage();)
           }
           break;
       }

       // not found, so we have to load the adapter
       if (i==n-1)
       {
           PROVIDERADAPTER_DEBUG (
	      cerr<<"ProviderAdapterManager::addAdapter(): New entry -> load adapter\n";)
           try {
               adapter=loadAdapter(adapterName, adapterFileName);
               pad=loadProvider(adapter,adapterName,providerLocation,providerName);
              _listOfAdapterLibs.append(adapter);
              _listOfAdapterNames.append(adapterName);
              _listOfAdapterCounts.append(1);
           }
           catch (Exception & e) {
               PROVIDERADAPTER_DEBUG (cerr << e.getMessage();)
           }
       } // endif of not found 
   } 
   _listMutex.unlock();
   return pad;
}

void  ProviderAdapterManager::deleteAdapter(const String & adapterName)
{
PROVIDERADAPTER_DEBUG( cerr << "ProviderAdapterManager::deleteAdapter()\n"; )
    _listMutex.lock(pegasus_thread_self());

    // lookup adapterName
    for(Uint32 i=0,n=_listOfAdapterNames.size();i<n;i++)
    {
        if (String::equal(_listOfAdapterNames[i],adapterName))
        {
            if (--_listOfAdapterCounts[i] == 0)
            {
                // call the terminate method of the adapter
               System::unloadDynamicLibrary(_listOfAdapterLibs[i]);
            }
            break;
        }
    }
    _listMutex.unlock();
}

void  ProviderAdapterManager::deleteAdapter(
              const DynamicLibraryHandle & adapterlib)
{
PROVIDERADAPTER_DEBUG( cerr << "ProviderAdapterManager::deleteAdapter()\n"; )
    _listMutex.lock(pegasus_thread_self());

    // lookup adapterName
    for(Uint32 i=0,n=_listOfAdapterLibs.size();i<n;i++)
    {
        if (_listOfAdapterLibs[i] == adapterlib)
        {
            --_listOfAdapterCounts[i];
            if (_listOfAdapterCounts[i] == 0)
            {
                // call the terminate method of the adapter
               System::unloadDynamicLibrary(_listOfAdapterLibs[i]);
            }
            break;
        }
    }
    _listMutex.unlock();
}

DynamicLibraryHandle ProviderAdapterManager::loadAdapter(
                                 const String & adapterName,
                                 const String & adapterFileName)
{
PROVIDERADAPTER_DEBUG( cerr << "ProviderAdapterManager::loadAdapter(): "<<
                            adapterFileName<<"\n"; )
    // dynamically load the provider library
    DynamicLibraryHandle library =
            System::loadDynamicLibrary(adapterFileName.getCString());

    if (library == 0)
    {
    	//l10n
    	
        //String errorString =
             //"Cannot load providerAdapter, error: " + System::dynamicLoadError();
        //throw Exception("AdapterLoadFailure (" +
              //adapterName + "):" + errorString);

        throw Exception(MessageLoaderParms("ProviderManager.ProviderAdapter.CANNOT_LOAD_PROVIDER_ADAPTER",
        								   "AdapterLoadFailure ($0):Cannot load providerAdapter, error: $1",
        								   adapterName,
        								   System::dynamicLoadError()));
    }
    return library;
}

ProviderAdapter * ProviderAdapterManager::loadProvider(
                                 const DynamicLibraryHandle & library,
                                 const String & adapterName,
                                 const String & providerLocation,
				 const String & providerName)
{
PROVIDERADAPTER_DEBUG( cerr << "ProviderAdapterManager::loadProvider(): "<<
             providerLocation<<"\n"; )

    // Calling entry point of Provider Adapter
    ProviderAdapter*(*create_pa)(const String &,const String &) = 0;

    create_pa=(ProviderAdapter*(*)(const String &,const String &))
    System::loadDynamicSymbol(library,"PegasusCreateProviderAdapter");

    if (create_pa!=0) {
        ProviderAdapter *pa=create_pa(providerLocation,providerName);
        if (pa==0)
        {
            System::unloadDynamicLibrary(library);
            //l10n
            //String errorString = "entry point returned null\n";
            //throw Exception("AdapterLoadFailure (" +
                   //adapterName + "):" + errorString);
            throw Exception(MessageLoaderParms("ProviderManager.ProviderAdapter.ENTRY_POINT_RETURNED_NULL",
            								   "AdapterLoadFailure ($0):entry point returned null\n",
            								   adapterName));
        }

        // test primary interface
        if (dynamic_cast<ProviderAdapter*>(pa) == 0)
        {
            System::unloadDynamicLibrary(library);
            //l10n
            //String errorString = "adapter is not a ProviderAdapter\n";
            //throw Exception("AdapterLoadFailure (" +
                   //adapterName + "):" + errorString);
            throw Exception(MessageLoaderParms("ProviderManager.ProviderAdapter.ADAPTER_NOT_PA",
            								   "AdapterLoadFailure ($0):adapter is not a ProviderAdapter\n",
            								   adapterName));
        }

        // test secondary interface
        if (dynamic_cast<CIMProvider*>(pa) == 0)
        {
            System::unloadDynamicLibrary(library);
            //l10n
            //String errorString = "adapter is not a CIMProvider\n";
            //throw Exception("AdapterLoadFailure (" +
                   //adapterName + "):" + errorString);

            throw Exception(MessageLoaderParms("ProviderManager.ProviderAdapter.ADAPTER_NOT_CP",
            								   "AdapterLoadFailure ($0):adapter is not a CIMProvider\n",
            								   adapterName));
        }

        // save library address to unload
        return pa;
    }
    else
    {
    	//l10n
        //String errorString = "CreateProviderAdapter missing.";
       // throw Exception("AdapterLoadFailure (" +
               //adapterName + "):" + errorString);

       throw Exception(MessageLoaderParms("ProviderManager.ProviderAdapter.MISSING",
       									  "AdapterLoadFailure ($0):CreateProviderAdapter missing.",
       									  adapterName));
    }
} 

void ProviderAdapterManager::list(void)
{
    _listMutex.lock(pegasus_thread_self());
    for(Uint32 i=0,n=_listOfAdapterNames.size(); i<n; i++)
    {
        PEGASUS_STD(cout) << "Entry " << i << "  " << _listOfAdapterNames[i] <<
        PEGASUS_STD(endl);
    }
    _listMutex.unlock();
}

PEGASUS_NAMESPACE_END
