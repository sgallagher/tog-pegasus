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
// Modified By: Yi Zhou, Hewlett-Packard Company(yi_zhou@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_ProviderModule_h
#define Pegasus_ProviderModule_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/IPC.h>

#include <Pegasus/Provider/CIMProvider.h>
#include <Pegasus/Config/ConfigManager.h>
#include <Pegasus/ProviderManager/Linkage.h>
PEGASUS_NAMESPACE_BEGIN

// The ProviderModule class represents the physical module, as defined by the
// operating, that contains a provider. This class effectively encapsulates the
// "physical" portion of a provider.
class PEGASUS_SERVER_LINKAGE ProviderModule
{
public:
    ProviderModule(const String & fileName);
    virtual ~ProviderModule(void);

    const String & getFileName(void) const;

    CIMBaseProvider *load(const String & providerName);
    void unloadModule(void);

    Boolean operator == (const void *key) const;
    Boolean operator == (const ProviderModule & pmod) const;

protected:
    String _fileName;
    AtomicInt _ref_count;
    DynamicLibraryHandle _library;


    friend class ProviderManager;
    friend class Provider;

 private:
    ProviderModule(const String & fileName, const Uint32 & refCount) {};
    ProviderModule(const String & fileName, const String & providerName){};
    ProviderModule(const String & fileName, const String & providerName,
                   const String & interfaceName, const Uint32 & refCount){};
    ProviderModule(const ProviderModule & pm){};
};

inline const String & ProviderModule::getFileName(void) const
{
    return(_fileName);
}

PEGASUS_NAMESPACE_END

#endif
