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
// Modified By:
//      Nag Boranna, Hewlett-Packard Company(nagaraja_boranna@hp.com)
//		Yi Zhou, Hewlett-Packard Company(yi_zhou@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include "ProviderModule.h"

#include <Pegasus/Common/Destroyer.h>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Provider/CIMInstanceProvider.h>

PEGASUS_NAMESPACE_BEGIN

ProviderModule::ProviderModule(const String & fileName,
                               const Uint32 & refCount)
    : _fileName(fileName), _providerName(String::EMPTY),
      _refCount(refCount), _interfaceName(String::EMPTY),
      _library(0), _provider(0)
{
}

ProviderModule::ProviderModule(const String & fileName,
                               const String & providerName)
    : _fileName(fileName), _providerName(providerName),
      _library(0), _provider(0)
{
}

ProviderModule::ProviderModule(const String & fileName,
                               const String & providerName,
                               const String & interfaceName,
			       const Uint32 & refCount)
    : _fileName(fileName), _providerName(providerName),
      _interfaceName(interfaceName),
      _refCount(refCount),
      _library(0), _provider(0)
{
    // currently without interface registration
    _interfaceFilename = String::EMPTY;

    if (_interfaceName.size() > 0)
        if (!( String::equalNoCase(_interfaceName, "C++Standard") ||
               String::equalNoCase(_interfaceName, "C++Default") ||
               String::equalNoCase(_interfaceName, "PG_DefaultC++") ))
        {
            #ifdef PEGASUS_OS_TYPE_WINDOWS
            _interfaceFilename = _interfaceName + String(".dll");
            #elif defined(PEGASUS_OS_HPUX)
            _interfaceFilename = ConfigManager::getHomedPath(
                ConfigManager::getInstance()->getCurrentValue("providerDir"));
            _interfaceFilename.append(
                String("/lib") + _interfaceName + String(".sl"));
            #elif defined(PEGASUS_OS_OS400)
            _interfaceFilename = _interfaceName;
            #else
            _interfaceFilename = ConfigManager::getHomedPath(
                ConfigManager::getInstance()->getCurrentValue("providerDir"));
            _interfaceFilename.append(
                String("/lib") + _interfaceName + String(".so"));
            #endif
        }
}

ProviderModule::ProviderModule(const ProviderModule & pm)
    : _fileName(pm._fileName),
      _providerName(pm._providerName),
      _interfaceName(pm._interfaceName),
      _interfaceFilename(pm._interfaceFilename),
      _library(pm._library),
      _provider(pm._provider),
      _refCount(pm._refCount)
{
}

ProviderModule::~ProviderModule(void)
{
}

void ProviderModule::load(void)
{
    // get the interface adapter library first
    _adapter = 0;
    if (_interfaceFilename.size() > 0)
    {
        _adapter = ProviderAdapterManager::get_pamgr()->addAdapter(
                                          _interfaceName, _interfaceFilename,
                                          _providerName);

        _provider = dynamic_cast<CIMProvider *>(_adapter);

        if (_provider == NULL)
        {
            String errorString = "ProviderAdapter is no CIMProvider";
            throw Exception("ProviderLoadFailure (" + _providerName + "):" +
                errorString);
        }

        return;
    }

    // dynamically load the provider library
    _library = System::loadDynamicLibrary(_fileName.getCString());

    if(_library == 0)
    {
        // ATTN: does unload() need to be called?

        String errorString = "Cannot load library, error: " + System::dynamicLoadError();
        throw Exception("ProviderLoadFailure (" + _fileName + ":" + _providerName + "):" + errorString);
    }

    // find library entry point
    CIMProvider * (*createProvider)(const String &) = 0;

    createProvider = (CIMProvider * (*)(const String &))System::loadDynamicSymbol(
        _library, "PegasusCreateProvider");

    if(createProvider == 0)
    {
        unload();

        String errorString = "entry point not found.";
        throw Exception("ProviderLoadFailure (" + _fileName + ":" + _providerName + "):" + errorString);
    }

    // invoke the provider entry point
    CIMProvider * provider = createProvider(_providerName);

    if(provider == 0)
    {
        unload();

        String errorString = "entry point returned null.";
        throw Exception("ProviderLoadFailure (" + _fileName + ":" + _providerName + "):" + errorString);
    }

    // test for the appropriate interface
    if(dynamic_cast<CIMProvider *>(provider) == 0)
    {
        unload();

        String errorString = "provider is not a CIMProvider.";
        throw Exception("ProviderLoadFailure (" + _fileName + ":" + _providerName + "):" + errorString);
    }

    // save provider handle
    _provider = provider;

    return;
}

void ProviderModule::unload(void)
{
    if(_adapter != 0)
    {
        delete _adapter;

        _adapter = 0;
    }

    /*
    // ATTN: cannot determine if provider is stack or heap based allocated.
    // the provider should delete, if necessary, during CIMProvider::terminate()

    if(_provider != 0)
    {
	delete _provider;
        delete _provider;

	_provider = 0;
        _provider = 0;
    }
    */

    if(_library != 0)
    {
	System::unloadDynamicLibrary(_library);

	_library = 0;
    }
}

void ProviderModule::unloadModule(void)
{
    if(_library != 0)
    {
        System::unloadDynamicLibrary(_library);

        _library = 0;
    }
}

PEGASUS_NAMESPACE_END
