//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000 The Open Group, BMC Software, Tivoli Systems, IBM
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
//==============================================================================
//
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Destroyer.h>
#include <Pegasus/Common/System.h>
#include "ProviderTable.h"

PEGASUS_NAMESPACE_BEGIN

ProviderTable::ProviderTable()
{

}

CIMProvider* ProviderTable::lookupProvider(const String& providerId)
{
    for (Uint32 i = 0, n = _providers.getSize(); i < n; i++)
	if (String::equal(_providers[i].providerId, providerId))
	    return _providers[i].provider;

    return 0;
}

typedef CIMProvider* (*CreateProviderFunc)();

CIMProvider* ProviderTable::loadProvider(const String& providerId)
{
    // Load the dynamic library:

#ifdef PEGASUS_OS_TYPE_WINDOWS
    ArrayDestroyer<char> libraryName = providerId.allocateCString();
#else
    String unixLibName = "lib";
    unixLibName += providerId;
    unixLibName += ".so";
    ArrayDestroyer<char> libraryName = unixLibName.allocateCString();
#endif

    DynamicLibraryHandle libraryHandle = 
	System::loadDynamicLibrary(libraryName.getPointer());

    if (!libraryHandle)
	throw DynamicLoadFailed(libraryName.getPointer());

    // Lookup the create provider symbol:

    String tmp = "PegasusCreateProvider_";
    tmp.append(providerId);
    ArrayDestroyer<char> functionName = tmp.allocateCString();

    CreateProviderFunc func = (CreateProviderFunc)System::loadDynamicSymbol(
	libraryHandle, functionName.getPointer());

    if (!func)
	throw DynamicLookupFailed(functionName.getPointer());

    // Create the provider:

    CIMProvider* provider = func();

    if (!provider)
	throw CreateProviderReturnedNull(
	    libraryName.getPointer(), functionName.getPointer());

    if (provider)
    {
	Entry entry;
	entry.providerId = providerId;
	entry.provider = provider;
	_providers.append(entry);
    }

    return provider;
}

PEGASUS_NAMESPACE_END
