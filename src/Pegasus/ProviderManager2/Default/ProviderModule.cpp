//%////////////-*-c++-*-///////////////////////////////////////////////////////////
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
// Modified By:
//      Nag Boranna, Hewlett-Packard Company(nagaraja_boranna@hp.com)
//		Yi Zhou, Hewlett-Packard Company(yi_zhou@hp.com)
//     Mike Day, IBM (mdday@us.ibm.com)
//     Adrian Schuur, IBM (schuur@de.ibm.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include "ProviderModule.h"

#include <Pegasus/Common/Destroyer.h>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/MessageLoader.h> //l10n
#include <Pegasus/Config/ConfigManager.h>

PEGASUS_NAMESPACE_BEGIN


// added support to re-activate ProviderAdapter  ( A Schuur )

/*
ProviderModule::ProviderModule(const String & fileName)
   : _fileName(fileName),
     _ref_count(0),
     _library(0)
{
    _interfaceFileName=String::EMPTY;
    _library = System::loadDynamicLibrary((const char *)_fileName.getCString());
}
*/

ProviderModule::ProviderModule(const String & fileName, const String & interfaceName)
    : _fileName(fileName),
    _interfaceName(interfaceName),
    _ref_count(0),
    _library(0)
{
    _interfaceFileName=String::EMPTY;
    if(_interfaceName.size()>0)
    {
        #ifdef PEGASUS_OS_TYPE_WINDOWS
        _interfaceFileName=_interfaceName+String("Adapter.dll");
        #elif defined(PEGASUS_OS_HPUX)
        _interfaceFileName=ConfigManager::getHomedPath(
            ConfigManager::getInstance()->getCurrentValue("providerDir"))+
            String("/lib")+interfaceName+String("Adapter.sl");
        #elif defined(PEGASUS_OS_OS400)
        _interfaceFileName=interfaceName+String("Adapter");
        #else
        _interfaceFileName=ConfigManager::getHomedPath(
            ConfigManager::getInstance()->getCurrentValue("providerDir"))+
            String("/lib")+interfaceName+String("Adapter.so");
        #endif
    }
}

/*
ProviderModule::ProviderModule(const String & fileName,
                               const String & providerName)
    : _fileName(fileName),
      _library(0),
      _providerName(providerName),
      _provider(0)
{
}
*/

ProviderModule::ProviderModule(const String & fileName,
    const String & providerName,
    const String & interfaceName,
    const Uint32 & refCount)
    : _fileName(fileName),
    _library(0),
    _providerName(providerName),
    _interfaceName(interfaceName),
    _provider(0),
    _refCount(refCount)

{
    // currently without interface registration
    _interfaceFileName = String::EMPTY;

    if(_interfaceName.size() > 0)
        if(!( String::equalNoCase(_interfaceName, "C++Standard") ||
            String::equalNoCase(_interfaceName, "C++Default") ||
            String::equalNoCase(_interfaceName, "PG_DefaultC++") ))
        {
            #ifdef PEGASUS_OS_TYPE_WINDOWS
            _interfaceFileName = _interfaceName + String(".dll");
            #elif defined(PEGASUS_OS_HPUX)
            _interfaceFileName = ConfigManager::getHomedPath(
                ConfigManager::getInstance()->getCurrentValue("providerDir"));
            _interfaceFileName.append(
                String("/lib") + _interfaceName + String(".sl"));
            #elif defined(PEGASUS_OS_OS400)
            _interfaceFileName = _interfaceName;
            #else
            _interfaceFileName = ConfigManager::getHomedPath(
                ConfigManager::getInstance()->getCurrentValue("providerDir"));
            _interfaceFileName.append(
                String("/lib") + _interfaceName + String(".so"));
            #endif
        }
}

ProviderModule::ProviderModule(const ProviderModule & pm)
    : _fileName(pm._fileName),
    _library(pm._library),
    _providerName(pm._providerName),
    _interfaceName(pm._interfaceName),
    _interfaceFileName(pm._interfaceFileName),
    _provider(pm._provider),
    _refCount(pm._refCount)
{
}

ProviderModule::~ProviderModule(void)
{

}

CIMProvider *ProviderModule::load(const String & providerName)
{
    // get the interface adapter library first
    CIMProvider *provider = 0;

    /*
    if (_interfaceFileName.size()>0) {
      _adapter=ProviderAdapterManager::get_pamgr()->addAdapter(
          _interfaceName,_interfaceFileName,_fileName,providerName);
      provider=dynamic_cast<CIMProvider*>(_adapter);
      if (provider==NULL) {
        //l10n
    //throw Exception("ProviderLoadFailure ("+providerName+
            //"Provider is not a BaseProvider");
            String s0 = "ProviderLoadFailure";
            String s2 = "Provider";
            String s3 = "BaseProvider";
            throw Exception(MessageLoaderParms("ProviderManager.ProviderModule.IS_NOT_A"
                                               "$0 ($1$2 is not a $3)",
                                               s0,
                                               providerName,
                                               s2,
                                               s3));
        }
      _ref_count++;
      return provider;
    }
    */

    // dynamically load the provider library
    if(_library == 0)
    {
        _library = System::loadDynamicLibrary((const char *)_fileName.getCString());
    }

    if(_library == 0)
    {

        // ATTN: does unload() need to be called?
        //l10n
        //String errorString = "Cannot load library, error: " + System::dynamicLoadError();
        //throw Exception("ProviderLoadFailure (" + _fileName + ":" + providerName + "):" + errorString);
        String s0 = "ProviderLoadFailure";
        throw Exception(MessageLoaderParms("ProviderManager.ProviderModule.CANNOT_LOAD_LIBRARY",
            "$0 ($1:$2):Cannot load library, error: $3",
            s0,
            _fileName,
            providerName,
            System::dynamicLoadError()));
    }

    // find libray entry point
    CIMProvider * (*createProvider)(const String &) = 0;

    createProvider = (CIMProvider * (*)(const String &))System::loadDynamicSymbol(
        _library, "PegasusCreateProvider");

    if(createProvider == 0)
    {
        //l10n
        //String errorString = "entry point not found.";
        //throw Exception("ProviderLoadFailure (" + _fileName + ":" + providerName + "):" + errorString);
        String s0 = "ProviderLoadFailure";
        throw Exception(MessageLoaderParms("ProviderManager.ProviderModule.ENTRY_POINT_NOT_FOUND",
            "$0 ($1:$2):entry point not found.",
            s0,
            _fileName,
            providerName));
    }

    // invoke the provider entry point
    provider = createProvider(providerName);

    // test for the appropriate interface
    if(dynamic_cast<CIMProvider *>(provider) == 0)
    {
        //l10n
        //String errorString = "provider is not a CIMProvider.";
        //throw Exception("ProviderLoadFailure (" + _fileName + ":" + providerName + "):" + errorString);
        String s0 = "ProviderLoadFailure";
        String s3 = "CIMProvider";
        throw Exception(MessageLoaderParms("ProviderManager.ProviderModule.PROVIDER_IS_NOT_A",
            "$0 ($1:$2):provider is not a $3.",
            s0,
            _fileName,
            providerName,
            s3));
    }

    _ref_count++;

    return(provider);
}

void ProviderModule::unloadModule(void)
{
    _ref_count--;

    if(_ref_count.value() > 0)
        return;

    _ref_count = 0;

    if(_library != 0)
    {
        System::unloadDynamicLibrary(_library);
        _library = 0;
    }
}

Boolean ProviderModule::operator == (const void *key) const
{
    String *prov = reinterpret_cast<String *>(const_cast<void *>(key));

    if(String::equalNoCase(_fileName, *prov))
        return(true);

    return(false);
}

Boolean ProviderModule::operator == (const ProviderModule &pm) const
{
    if(String::equalNoCase(_fileName, pm._fileName))
        return(true);

    return(false);
}

PEGASUS_NAMESPACE_END
