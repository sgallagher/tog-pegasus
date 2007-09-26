//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
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
//%/////////////////////////////////////////////////////////////////////////////

#include "ProviderModule.h"

#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/MessageLoader.h>

PEGASUS_NAMESPACE_BEGIN

ProviderModule::ProviderModule(const String& fileName)
    : _library(fileName)
{
}

ProviderModule::~ProviderModule()
{
}

CIMProvider* ProviderModule::load(const String& providerName)
{
/*
BOOKMARK3:
*/
std::cout << "ProviderModule::load[" << providerName << "]" << std::endl;

    // dynamically load the provider library
    if (!_library.load())
    {
        throw Exception(MessageLoaderParms(
            "ProviderManager.ProviderModule.CANNOT_LOAD_LIBRARY",
            "ProviderLoadFailure ($0:$1):Cannot load library, error: $2",
            _library.getFileName(),
            providerName,
            _library.getLoadErrorMessage()));
    }

    // find library entry point
    CIMProvider * (*createProvider)(const String&) =
        (CIMProvider* (*)(const String&))
            _library.getSymbol("PegasusCreateProvider");

    if (createProvider == 0)
    {
        _library.unload();
        throw Exception(MessageLoaderParms(
            "ProviderManager.ProviderModule.ENTRY_POINT_NOT_FOUND",
            "ProviderLoadFailure ($0:$1):entry point not found.",
            _library.getFileName(),
            providerName));
    }

    // invoke the provider entry point
    CIMProvider* provider = createProvider(providerName);

    // test for the appropriate interface
    if (dynamic_cast<CIMProvider *>(provider) == 0)
    {
        _library.unload();
        throw Exception(MessageLoaderParms(
            "ProviderManager.ProviderModule.PROVIDER_IS_NOT_A",
            "ProviderLoadFailure ($0:$1):provider is not a CIMProvider.",
            _library.getFileName(),
            providerName));
    }

    return provider;
}

void ProviderModule::unloadModule()
{
    _library.unload();
}

PEGASUS_NAMESPACE_END
