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

#include "ProviderModule.h"

#include <Pegasus/Common/Destroyer.h>
#include <Pegasus/Common/FileSystem.h>

#include <Pegasus/Provider2/CIMBaseProviderHandle.h>
#include <Pegasus/Provider/CIMProviderHandle.h>

PEGASUS_NAMESPACE_BEGIN

ProviderModule::ProviderModule(const String & fileName, const String & className)
	: _fileName(fileName), _className(className), _library(0), _provider(0)
{
}

ProviderModule::~ProviderModule(void)
{
}

const String & ProviderModule::getFileName(void) const
{
	return(_fileName);
}

const String & ProviderModule::getClassName(void) const
{
	return(_className);
}

ProviderHandle * ProviderModule::getProvider(void) const
{
	return(_provider);
}

void ProviderModule::load(void)
{
	// dynamically load the provider library
	ArrayDestroyer<char> tempFileName = _fileName.allocateCString();
	
	_library = System::loadDynamicLibrary(tempFileName.getPointer());

	if(_library == 0)
	{
		throw ProviderFailure(_fileName, _className, "cannot load library.");
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

				throw ProviderFailure(_fileName, _className, "entry point returned null.");
			}

			// test for the appropriate interface
			if(dynamic_cast<CIMBaseProvider *>(provider) == 0)
			{
				unload();

				throw ProviderFailure(_fileName, _className, "provider is not a CIMBaseProvider.");
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

		// get the base file name from the file name parameter in the event that a path
		// is given. the original provider entry point function was composed of a prefix
		// and the providerId or base file name.
		String fileName = _fileName;
		
		// convert slashes
		FileSystem::translateSlashes(fileName);
			
		// find last slash
		Uint32 pos = fileName.reverseFind(Char16('/'));
			
		// the base file name is the regison between the last slash and a period
		String baseName = fileName.subString(pos == PEG_NOT_FOUND ? 0 : pos, fileName.find(Char16('.')));

		// build entry point name
		String temp = String("PegasusCreateProvider_") + baseName;

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

				throw ProviderFailure(_fileName, _className, "entry point returned null.");
			}

			// test for the appropriate interface
			if(dynamic_cast<CIMProvider *>(provider) == 0)
			{
				unload();

				throw ProviderFailure(_fileName, _className, "provider is not a CIMProvider.");
			}

			// save provider handle
			_provider = new CIMProviderHandle(provider);

			return;
		}
	}

	unload();

	throw ProviderFailure(_fileName, _className, "provider entry point not found.");
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
