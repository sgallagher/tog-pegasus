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
//
//%/////////////////////////////////////////////////////////////////////////////

#include <cstdlib>
#include <Pegasus/Common/Destroyer.h>
#include <Pegasus/Common/System.h>
#include "ProviderManager.h"

PEGASUS_NAMESPACE_BEGIN

ProviderFailure::ProviderFailure(const String & fileName, const String & className)
	: Exception("Failure in " + fileName + " with provider for " + className)
{
}

ProviderLoadFailure::ProviderLoadFailure(const String & fileName, const String & className)
	: ProviderFailure(fileName, className)
{
}

ProviderCreateFailure::ProviderCreateFailure(const String & fileName, const String & className)
	: ProviderFailure(fileName, className)
{
}

ProviderInitializationFailure::ProviderInitializationFailure(const String & fileName, const String & className)
	: ProviderFailure(fileName, className)
{
}

ProviderTerminationFailure::ProviderTerminationFailure(const String & fileName, const String & className)
	: ProviderFailure(fileName, className)
{
}

typedef CIMProvider* (*CreateProviderFunction)(const String &);

ProviderModule::ProviderModule(const String & fileName, const String & className)
	: _fileName(fileName), _className(className), _libraryHandle(0), _provider(0)
{
	load();
}

ProviderModule::~ProviderModule(void)
{
	unload();
}

void ProviderModule::load(void)
{
	unload();
	
	// load the library
	ArrayDestroyer<char> tempFileName = _fileName.allocateCString();
	
	_libraryHandle = System::loadDynamicLibrary(tempFileName.getPointer());

    if(_libraryHandle == 0)
	{
		throw ProviderLoadFailure(_fileName, _className);
	}

	// find libray entry point
    CreateProviderFunction createProvider = (CreateProviderFunction)System::loadDynamicSymbol(
		_libraryHandle, "PegasusCreateProvider");

    if(createProvider == 0)
	{
		unload();
		
		throw ProviderCreateFailure(_fileName, _className);
	}

    // invoke the provider entry point
    _provider = createProvider(_className);

    if(_provider == 0)
	{
		unload();

		throw ProviderCreateFailure(_fileName, _className);
	}
}

void ProviderModule::unload(void)
{
	if(_provider != 0) {
		try {
			_provider->terminate();
		}
		catch(...)
		{
		}
		
		delete _provider;
		_provider = 0;
	}

	if(_libraryHandle != 0)
	{
		System::unloadDynamicLibrary(_libraryHandle);
		_libraryHandle = 0;
	}
}

ProviderManager::ProviderManager(CIMOMHandle & cimom) : _cimom(cimom)
{
}

ProviderManager::~ProviderManager(void)
{
}

CIMProvider * ProviderManager::getProvider(const String & fileName, const String & className)
{
    // check list for requested provider and return if found
	for(Uint32 i = 0, n = _providers.size(); i < n; i++)
	{
		if((fileName == _providers[i].getFileName()) && (className == _providers[i].getClassName()))
		{
			return(_providers[i]);
		}
	}

    // create a logical provider module to represent the physical provider module
	ProviderModule providerModule(fileName, className);

	try {	
		((CIMProvider *)providerModule)->initialize(_cimom);
		
		// add provider to list
		_providers.append(providerModule);
	}
	catch(...)
	{
		throw ProviderInitializationFailure(fileName, className);
	}

	return(providerModule);
}

PEGASUS_NAMESPACE_END