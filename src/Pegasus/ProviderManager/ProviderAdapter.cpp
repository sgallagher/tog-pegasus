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
// Author: Markus Mueller (sedgewick_de@yahoo.de)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/Destroyer.h>
#include <Pegasus/Common/Sharable.h>
#include <Pegasus/Common/IPC.h>

#include <Pegasus/Provider/CIMBaseProvider.h>
#include <Pegasus/Provider/CIMOMHandle.h>
#include "ProviderAdapter.h"
#include <Pegasus/Config/ConfigManager.h>
#define PROVIDERADAPTER_DEBUG(X)  //X


PEGASUS_NAMESPACE_BEGIN

// HACK
static ProviderAdapterManager * pamgr;

//
//
//

ProviderAdapter::ProviderAdapter(const String & adapterName,
                                 const String & providerName,
                                 const String & className) :
    _adapterName(adapterName), _providerName(providerName),
    _className(className)
{
PROVIDERADAPTER_DEBUG( cerr << "ProviderAdapter::ProviderAdapter\n"; )
}

ProviderAdapter::~ProviderAdapter()
{
PROVIDERADAPTER_DEBUG( cerr << "ProviderAdapter::~ProviderAdapter\n"; )
    ProviderAdapterManager::get_pamgr()->deleteAdapter(_adapterName);
}

const String & ProviderAdapter::getAdapterName() const {return _adapterName;}
const String & ProviderAdapter::getProviderName() const {return _providerName;}
CIMBaseProvider * ProviderAdapter::getBaseProvider(void) const {return _adapter;}

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
                                 const String & providerName,
                                 const String & className)
{
PROVIDERADAPTER_DEBUG( cerr << "ProviderAdapterManager::addAdapter()\n"; )

   _listMutex.lock(pegasus_thread_self());
   ProviderAdapter * pad = NULL;
   DynamicLibraryHandle library;

   // lookup adapterName
   Uint32 n = _listOfAdapterNames.size();

   if (n == 0)
   {
       PROVIDERADAPTER_DEBUG (cerr << "No entries -> load adapter\n";)
       try {
           library = _loadlibrary(adapterName, adapterFileName);
           pad = _load(library,adapterName, providerName, className);
           _listOfAdapterLibs.append(library);
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
           PROVIDERADAPTER_DEBUG( cerr << "Found adapter entry\n";)
           _listOfAdapterCounts[i]++;

           library = _listOfAdapterLibs[i];
           try {
               pad = _load(library,adapterName, providerName, className);
           }
           catch (Exception & e) {
               PROVIDERADAPTER_DEBUG (cerr << e.getMessage();)
           }
           break;
       }

       // not found, so we have to load the adapter
       if (i==n-1)
       {
           PROVIDERADAPTER_DEBUG (cerr << "New entry -> load adapter\n";)
           try {
               library = _loadlibrary(adapterName, adapterFileName);
               pad = _load(library,adapterName, providerName, className);
              _listOfAdapterLibs.append(library);
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

DynamicLibraryHandle ProviderAdapterManager::_loadlibrary(
                                 const String & adapterName,
                                 const String & adapterFileName)
{
#if 0
    CIMOMHandle my_cimom;

    PROVIDERADAPTER_DEBUG (cerr << "_loadlibrary\n";)
    String fileName;

    CIMReference ref =
       String("WBEM_ProviderProtocolAdapter.name=\"")+ adapterName+
       String("\",CreationClassName=\"\",") +
       String("SystemCreationClassName=\"\",") +
       String("SystemName=\"\"");
    CIMInstance ci;

    try {
    ci = my_cimom.getInstance(OperationContext(),
                         String("root/cimv2"),
                         ref, false,true,true,CIMPropertyList());
    } catch (Exception & e) {
        PROVIDERADAPTER_DEBUG (cerr << "getInstance failed :" <<  e.getMessage() << endl;)
        //
        // HACK - just during test
        //          create the class we know to exist
        //
        CIMClass cc = my_cimom.getClass(OperationContext(),
                String("root/cimv2"),"WBEM_ProviderProtocolAdapter",false,
                true, true, CIMPropertyList());
        ci = CIMInstance("WBEM_ProviderProtocolAdapter");
        ci.addProperty(CIMProperty("name",CIMValue(adapterName)));
        ci.addProperty(CIMProperty("SystemCreationClassName",
                                   CIMValue(String::EMPTY)));
        ci.addProperty(CIMProperty("CreationClassName",
                                   CIMValue(String::EMPTY)));
        ci.addProperty(CIMProperty("SystemName",
                                   CIMValue(String::EMPTY)));
#ifdef PEGASUS_OS_TYPE_WINDOWS
    fileName = adapterName + String(".dll");
#elif defined(PEGASUS_OS_HPUX)
    fileName = ConfigManager::getHomedPath(ConfigManager::getInstance()->
               getCurrentValue("providerDir")) + String("/lib") +
               adapterName + String(".sl");
#else
    fileName = ConfigManager::getHomedPath(ConfigManager::getInstance()->
               getCurrentValue("providerDir")) + String("/lib") +
               adapterName + String(".so");
#endif
        ci.addProperty(CIMProperty("LibPath",CIMValue(fileName)));
        my_cimom.createInstance(OperationContext(), String("root/cimv2"), ci);
    }

    // getProperty "LibPath"
    CIMProperty cprop = ci.getProperty(ci.findProperty("LibPath"));
    cprop.getValue().get(fileName);

PROVIDERADAPTER_DEBUG( cerr << "ProviderAdapterManager::_loadlibrary() " << fileName << endl; )

#endif
    // dynamically load the provider library
    ArrayDestroyer<char> tempFileName = adapterFileName.allocateCString();


    DynamicLibraryHandle library =
            System::loadDynamicLibrary(tempFileName.getPointer());

    if (library == 0)
    {
        String errorString =
             "Cannot load library, error: " + System::dynamicLoadError();
        throw Exception("AdapterLoadFailure (" +
              adapterName + "):" + errorString);
    }
    return library;
}

ProviderAdapter * ProviderAdapterManager::_load(
                                 const DynamicLibraryHandle & library,
                                 const String & adapterName,
                                 const String & providerName,
                                 const String & className)
{
PROVIDERADAPTER_DEBUG( cerr << "ProviderAdapterManager::_load()\n"; )

    // Calling entry point of Provider Adapter
    ProviderAdapter * (* create_pa)(const String &,
                                    const String &) = 0;
    create_pa = (ProviderAdapter * (*)(const String &, const String &))
    System::loadDynamicSymbol(library,"PegasusCreateProviderAdapter");

    if (create_pa != 0)
    {
        ProviderAdapter * pa = create_pa(adapterName, providerName);
        if (pa == 0)
        {
            System::unloadDynamicLibrary(library);
            String errorString = "entry point returned null.";
            throw Exception("AdapterLoadFailure (" +
                   adapterName + "):" + errorString);
        }

        // test primary interface
        if (dynamic_cast<ProviderAdapter *>(pa) == 0)
        {
            System::unloadDynamicLibrary(library);
            String errorString = "adapter is not a ProviderAdapter.";
            throw Exception("AdapterLoadFailure (" +
                   adapterName + "):" + errorString);
        }

        // test secondary interface
        if (dynamic_cast<CIMBaseProvider *>(pa) == 0)
        {
            System::unloadDynamicLibrary(library);
            String errorString = "adapter is not a CIMBaseProvider.";
            throw Exception("AdapterLoadFailure (" +
                   adapterName + "):" + errorString);
        }

        // save library address to unload
        return pa;
    }
    else
    {
        String errorString = "CreateProviderAdapter missing.";
        throw Exception("AdapterLoadFailure (" +
               adapterName + "):" + errorString);
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
