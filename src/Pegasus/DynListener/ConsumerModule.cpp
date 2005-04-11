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
// Modified By: Yi Zhou, Hewlett-Packard Company(yi_zhou@hp.com)
//              Mike Day, IBM (mdday@us.ibm.com)
//              Adrian Schuur, IBM (schuur@de.ibm.com)
//              Heather Sterling, IBM (hsterl@us.ibm.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include "ConsumerModule.h"

#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/MessageLoader.h> //l10n
#include <Pegasus/Common/Tracer.h>

#include <Pegasus/Provider/CIMIndicationConsumerProvider.h>
#include <Pegasus/DynListener/ConsumerManager.h>

PEGASUS_USING_STD;
PEGASUS_NAMESPACE_BEGIN


ConsumerModule::ConsumerModule() :
_ref_count(0),
_libraryHandle(0)
{
}

ConsumerModule::~ConsumerModule(void)
{
}

// The caller assumes the repsonsibility of making sure the libraryPath is correctly formatted
CIMIndicationConsumerProvider* ConsumerModule::load(const String & consumerName, const String & libraryPath)
{
    PEG_METHOD_ENTER(TRC_LISTENER, "ConsumerModule::load");

    _libraryPath = libraryPath;

    //check whether the module is cached; if it's not already in memory, load it
    if (!_libraryHandle)
    {
        if (!FileSystem::exists(libraryPath) || !FileSystem::canRead(libraryPath))
        {
            throw Exception(MessageLoaderParms("DynListener.ConsumerModule.INVALID_LIBRARY_PATH",
               "The library ($0:$1) does not exist or cannot be read.",
               libraryPath,
               consumerName));
        }

        PEG_TRACE_STRING(TRC_LISTENER, Tracer::LEVEL2, "Loading library: " + consumerName);

        _libraryHandle = System::loadDynamicLibrary((const char*)libraryPath.getCString());

        PEG_TRACE_STRING(TRC_LISTENER, Tracer::LEVEL2, "Successfully loaded library " + consumerName);
    }
    else
    {
        PEG_TRACE_STRING(TRC_LISTENER, Tracer::LEVEL2, "Library is cached in memory: " + consumerName);
    }

    if (!_libraryHandle)
    {
        throw Exception(MessageLoaderParms("DynListener.ConsumerModule.CANNOT_LOAD_LIBRARY",
                                   "Cannot load consumer library ($0:$1), load error $2",
                                   libraryPath,
                                   consumerName,
                                   System::dynamicLoadError()));
    }

    // locate the entry point
    CIMProvider* (*createProvider)(const String&) = 0;
    createProvider = (CIMProvider* (*)(const String&))System::loadDynamicSymbol(_libraryHandle, "PegasusCreateProvider");

    if (!createProvider)
    {
        throw Exception(MessageLoaderParms("DynListener.ConsumerModule.ENTRY_POINT_NOT_FOUND",
               "The entry point for consumer library ($0:$1) cannot be found.",
               libraryPath,
               consumerName));
    }

    // create the consumer provider
    CIMProvider* providerRef = 0;
    providerRef = createProvider(consumerName);

    if(!providerRef)
    {
        throw Exception(MessageLoaderParms("DynListener.ConsumerModule.CREATE_PROVIDER_FAILED",
               "createProvider failed for consumer library ($0:$1)",
               libraryPath,
               consumerName));
    }

    // test for the appropriate interface
    CIMIndicationConsumerProvider* consumerRef = dynamic_cast<CIMIndicationConsumerProvider *>(providerRef);
    if(!consumerRef)
    {
        throw Exception(MessageLoaderParms("DynListener.ConsumerModule.CONSUMER_IS_NOT_A",
            "Consumer ($0:$1) is not a CIMIndicationConsumerProvider.",
            libraryPath,
            consumerName));
    }

    _ref_count++;

    PEG_METHOD_EXIT();
    return consumerRef;
}

void ConsumerModule::unloadModule(void)
{
    PEG_METHOD_ENTER(TRC_LISTENER, "ConsumerModule::unloadModule");

    if (_ref_count.DecAndTestIfZero())
    {
        if(_libraryHandle != 0)
        {
            PEG_TRACE_STRING(TRC_LISTENER, Tracer::LEVEL3, "Unloading module " + _libraryPath);
            System::unloadDynamicLibrary(_libraryHandle);
            _libraryHandle = 0;
        }
    }

    PEG_METHOD_EXIT();
}

PEGASUS_NAMESPACE_END

