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

#ifndef Pegasus_CMPIResolverModule_h
#define Pegasus_CMPIResolverModule_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/DynamicLibrary.h>
#include <Pegasus/Common/System.h>

#include <Pegasus/Provider/CIMProvider.h>
#include <Pegasus/ProviderManager2/CMPI/CMPIProvider.h>

#include <Pegasus/ProviderManager2/CMPI/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

// The CMPIResolverModule class represents the physical module, as defined by 
// the operation, that contains a provider. This class effectively 
// encapsulates the "physical" portion of a provider.


struct provider_address;

typedef provider_address* (*RESOLVE_INSTANCE) ( const char * provider,
    CMPIObjectPath * cop, CMPIContext * ctx );
typedef provider_address* (*RESOLVE_CLASS) ( const char * provider,
    CMPIObjectPath * cop, CMPIContext * ctx );

class PEGASUS_CMPIPM_LINKAGE CMPIResolverModule
{

    friend class CMPILocalProviderManager;

public:
    ~CMPIResolverModule() {}
    void load() 
    {
        _library = DynamicLibrary(_fileName);
        String s0 = "ResolverLoadFailure";
        if (!_library.load()) 
        {
            throw Exception(MessageLoaderParms(
            "ProviderManager.CMPIProviderModule.CANNOT_LOAD_LIBRARY",
            "$0 ($1):Cannot load library, error: $3",
            s0,
            _fileName,
            _library.getLoadErrorMessage()));
        }
        resolveInstanceEntry=(RESOLVE_INSTANCE)
            _library.getSymbol("resolve_instance");
        resolveClassEntry=(RESOLVE_CLASS)
            _library.getSymbol("resolve_class");
        if (!resolveInstanceEntry || !resolveClassEntry) 
        {
            throw Exception(
            s0+" "+_fileName+String(": not a remote location resolver"));
        }
    }
    void unloadModule() {}

    provider_address* resolveInstance(
        const char *provider,
        CMPIObjectPath *cop, 
        CMPIContext *ctx) 
    {
        return resolveInstanceEntry(provider,cop,ctx);
    }
    provider_address* resolveClass(
        const char *provider,
        CMPIObjectPath *cop, 
        CMPIContext *ctx) 
    {
        return resolveClassEntry(provider,cop,ctx);
    }

protected:
    String _fileName;
    DynamicLibrary _library;
    RESOLVE_INSTANCE resolveInstanceEntry;
    RESOLVE_CLASS resolveClassEntry;

private:
    CMPIResolverModule(const String & fileName) 
    {
        _fileName=fileName;
    }

};


PEGASUS_NAMESPACE_END

#endif
