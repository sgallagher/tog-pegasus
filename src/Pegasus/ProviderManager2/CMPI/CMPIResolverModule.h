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
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_CMPIResolverModule_h
#define Pegasus_CMPIResolverModule_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/IPC.h>

#include <Pegasus/Provider/CIMProvider.h>
#include <Pegasus/ProviderManager2/CMPI/CMPIProvider.h>

#include <Pegasus/ProviderManager2/CMPI/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

// The CMPIResolverModule class represents the physical module, as defined by the
// operation, that contains a provider. This class effectively encapsulates the
// "physical" portion of a provider.


struct provider_address;

typedef provider_address* (*RESOLVE_INSTANCE) ( const char * provider,
				      CMPIObjectPath * cop,
				      CMPIContext * ctx );
typedef provider_address* (*RESOLVE_CLASS) ( const char * provider,
				      CMPIObjectPath * cop,
				      CMPIContext * ctx );

class PEGASUS_CMPIPM_LINKAGE CMPIResolverModule
{

    friend class CMPILocalProviderManager;


public:
    ~CMPIResolverModule(void) {}
    void load() {
        _library = System::loadDynamicLibrary((const char *)_fileName.getCString());
        String s0 = "ResolverLoadFailure";
        if(_library == 0) {
           throw Exception(MessageLoaderParms("ProviderManager.CMPIProviderModule.CANNOT_LOAD_LIBRARY",
               "$0 ($1):Cannot load library, error: $3",
               s0,
               _fileName,
               System::dynamicLoadError()));
         }
         resolveInstanceEntry=(RESOLVE_INSTANCE)
	    System::loadDynamicSymbol(_library,"resolve_instance");
         resolveClassEntry=(RESOLVE_CLASS)
	    System::loadDynamicSymbol(_library,"resolve_class");
	 if (!resolveInstanceEntry || !resolveClassEntry) {
           throw Exception(s0+" "+_fileName+String(": not a remote location resolver"));
	 }
   }
    void unloadModule(void) {}

    provider_address* resolveInstance(const char *provider,
                              CMPIObjectPath *cop, CMPIContext *ctx) {
        return resolveInstanceEntry(provider,cop,ctx);
    }
    provider_address* resolveClass(const char *provider,
                              CMPIObjectPath *cop, CMPIContext *ctx) {
        return resolveClassEntry(provider,cop,ctx);
    }

protected:
    String _fileName;
    DynamicLibraryHandle _library;
    RESOLVE_INSTANCE resolveInstanceEntry;
    RESOLVE_CLASS resolveClassEntry;

private:
    CMPIResolverModule(const String & fileName) {
       _fileName=fileName;
    }

};


PEGASUS_NAMESPACE_END

#endif
