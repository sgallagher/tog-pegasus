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
// Modified By:
//              Nag Boranna, Hewlett-Packard Company (nagaraja_boranna@hp.com)
//              Yi Zhou, Hewlett-Packard Company (yi_zhou@hp.com)
//              Mike Day, IBM (mdday@us.ibm.com)
//              Adrian Schuur, IBM (schuur@de.ibm.com)
//              Josephine Eskaline Joyce (jojustin@in.ibm.com) for PEP#101
//              Sean Keenan, Hewlett-Packard Company <sean.keenan@hp.com>
//              Roger Kumpf, Hewlett-Packard Company <roger_kumpf@hp.com>
//
//%/////////////////////////////////////////////////////////////////////////////

#include "ProviderModule.h"

#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/MessageLoader.h>

#ifdef PEGASUS_OS_OS400
# include "CreateProviderOS400SystemState.h"
#endif


PEGASUS_NAMESPACE_BEGIN

ProviderModule::ProviderModule(const String& fileName)
    : _fileName(fileName),
      _refCount(0),
      _library(0)
{
}

ProviderModule::~ProviderModule()
{
}

CIMProvider* ProviderModule::load(const String& providerName)
{
    CIMProvider* provider = 0;

    // dynamically load the provider library
    if (_library == 0)
    {
        _library =
            System::loadDynamicLibrary((const char *)_fileName.getCString());

        if (_library == 0)
        {
            // ATTN: does unload() need to be called?
            throw Exception(MessageLoaderParms(
                "ProviderManager.ProviderModule.CANNOT_LOAD_LIBRARY",
                "ProviderLoadFailure ($0:$1):Cannot load library, error: $2",
                _fileName,
                providerName,
                System::dynamicLoadError()));
        }
    }

    // find library entry point
    CIMProvider * (*createProvider)(const String &) = 0;

    createProvider = (CIMProvider* (*)(const String&))System::loadDynamicSymbol(
        _library, "PegasusCreateProvider");

    if (createProvider == 0)
    {
        throw Exception(MessageLoaderParms(
            "ProviderManager.ProviderModule.ENTRY_POINT_NOT_FOUND",
            "ProviderLoadFailure ($0:$1):entry point not found.",
            _fileName,
            providerName));
    }

    // invoke the provider entry point
#ifndef PEGASUS_OS_OS400
    provider = createProvider(providerName);
#else
    // On OS/400, need to call a layer of code that does platform-specific
    // checks before calling the provider
    provider = OS400_CreateProvider(
        providerName.getCString(), createProvider, _fileName);
#endif

    // test for the appropriate interface
    if(dynamic_cast<CIMProvider *>(provider) == 0)
    {
        throw Exception(MessageLoaderParms(
            "ProviderManager.ProviderModule.PROVIDER_IS_NOT_A",
            "ProviderLoadFailure ($0:$1):provider is not a CIMProvider.",
            _fileName,
            providerName));
    }
    _refCount++;
    return(provider);
}

void ProviderModule::unloadModule()
{
    if (_refCount.DecAndTestIfZero())
    {
        if (_library != 0)
        {
            System::unloadDynamicLibrary(_library);
            _library = 0;
        }
    }
}

PEGASUS_NAMESPACE_END
