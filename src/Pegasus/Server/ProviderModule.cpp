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

#include "ProviderModule.h"

#include <Pegasus/Common/Destroyer.h>
#include <Pegasus/Common/FileSystem.h>

#include <Pegasus/Provider2/CIMBaseProviderHandle.h>
#include <Pegasus/Provider/CIMProviderHandle.h>

PEGASUS_NAMESPACE_BEGIN


ProviderModule::ProviderModule(const String & providerName, const String & className)
    : _providerName(providerName), _className(className), _library(0), _provider(0)
{
}

ProviderModule::~ProviderModule(void)
{
}

const String & ProviderModule::getProviderName(void) const
{
    return(_providerName);
}

const String & ProviderModule::getClassName(void) const
{
    return(_className);
}

ProviderHandle * ProviderModule::getProvider(void) const
{
    return(_provider);
}

String ProviderModule::_getProviderFileName()
{
    String fileName = String::EMPTY;

    //
    // translate the provider identifier into a file name
    //
#ifdef PEGASUS_OS_TYPE_WINDOWS
    fileName = _providerName + String(".dll");
#elif defined(PEGASUS_OS_HPUX)
    fileName = getenv("PEGASUS_HOME") + String("/lib/lib") + _providerName + String(".sl");
#else
    fileName = getenv("PEGASUS_HOME") + String("/lib/lib") + _providerName + String(".so");
#endif

    return(fileName);
}


void ProviderModule::load(void)
{
    String fileName = _getProviderFileName();

    // dynamically load the provider library
    ArrayDestroyer<char> tempFileName = fileName.allocateCString();
    
    _library = System::loadDynamicLibrary(tempFileName.getPointer());

    if(_library == 0)
    {
        String errorString = "Cannot load library, error: " +
             System::dynamicLoadError();
        throw ProviderFailure(fileName, _className, errorString);
    }
        
    // CIMBaseProvider support
    
    {
        // find libray entry point
        CIMBaseProvider * (*createProvider)(const String &) = 0;

        createProvider = (CIMBaseProvider * (*)(const String &))System::loadDynamicSymbol(
            _library, "PegasusCreateProvider");

        if(createProvider != 0)
        {
            // invoke the provider entry point
            CIMBaseProvider * provider = createProvider(_className);

            if(provider == 0)
            {
                unload();

                throw ProviderFailure(fileName, _className, "entry point returned null.");
            }

            // test for the appropriate interface
            if(dynamic_cast<CIMBaseProvider *>(provider) == 0)
            {
                unload();

                throw ProviderFailure(fileName, _className, "provider is not a CIMBaseProvider.");
            }

            // save provider handle
            _provider = new CIMBaseProviderHandle(provider);

            return;
        }
    }
    
    // CIMProvider support(legacy)

    {
        // find libray entry point
        CIMProvider * (*createProvider)(void) = 0;    

        // build entry point name
        String temp = String("PegasusCreateProvider_") + _providerName;

        ArrayDestroyer<char> functionName = temp.allocateCString();

        createProvider = (CIMProvider * (*)(void))System::loadDynamicSymbol(
            _library, functionName.getPointer());

        if(createProvider != 0)
        {
            // invoke the provider entry point
            CIMProvider * provider = createProvider();

            if(provider == 0)
            {
                unload();

                throw ProviderFailure(fileName, _className, "entry point returned null.");
            }

            // test for the appropriate interface
            if(dynamic_cast<CIMProvider *>(provider) == 0)
            {
                unload();

                throw ProviderFailure(fileName, _className, "provider is not a CIMProvider.");
            }

            // save provider handle
            _provider = new CIMProviderHandle(provider);

            return;
        }
    }

    unload();
    throw ProviderFailure(fileName, _className, "provider entry point not found.");
}

void ProviderModule::unload(void)
{
    if(_provider != 0)
    {
        delete _provider;

        _provider = 0;
    }
    
    if(_library != 0)
    {
        System::unloadDynamicLibrary(_library);

        _library = 0;
    }
}

PEGASUS_NAMESPACE_END
