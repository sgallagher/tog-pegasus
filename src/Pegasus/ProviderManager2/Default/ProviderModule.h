//%/////////////////////-*-c++-*-///////////////////////////////////////////////
//
// Copyright (c) 2000 - 2003 BMC Software, Hewlett-Packard Company, IBM,
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
//              Mike Day, IBM (mdday@us.ibm.com)
//              Adrian Schuur, IBM (schuur@de.ibm.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_ProviderModule_h
#define Pegasus_ProviderModule_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/IPC.h>

#include <Pegasus/Provider/CIMProvider.h>

#include <Pegasus/Server/Linkage.h>

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

    CIMProvider * load(const String & providerName);
    void unloadModule(void);

    Boolean operator==(const void *key) const;
    Boolean operator==(const ProviderModule & pmod) const;

protected:
    String _fileName;
    AtomicInt _ref_count;
    DynamicLibraryHandle _library;

    friend class LocalProviderManager;
    friend class Provider;

private:
// refCount is deprecated << Wed Apr  9 12:03:13 2003 mdd >>
// reference counting is done automatically by provider manager
    ProviderModule(const String & fileName, const Uint32 & refCount);
// providerName is deprecated because a provider module may have more than
// one provider loaded. << Wed Apr  9 12:04:05 2003 mdd >>
    ProviderModule(const String & fileName, const String & providerName);
    ProviderModule(const String & fileName, const String & providerName,
        const String & interfaceName, const Uint32 & refCount);
// do not use !! not safe !! << Wed Apr  9 12:07:02 2003 mdd >>
    ProviderModule(const ProviderModule & pm);

    const String & getProviderName(void) const;
    const String & getInterfaceName(void) const ;

    const Uint32 & getRefCount(void) const;
//      ProviderAdapter * getAdapter(void) const;
    virtual CIMProvider * getProvider(void) const;

    String _providerName;

    String _interfaceName;
    String _interfaceFileName; // for later use with interface registration
//      ProviderAdapter * _adapter;

    CIMProvider * _provider;

    Uint32 _refCount;

};

inline const String & ProviderModule::getFileName(void) const
{
    return(_fileName);
}

inline const String & ProviderModule::getInterfaceName(void) const
{
    return(_interfaceName);
}

//inline ProviderAdapter * ProviderModule::getAdapter(void) const
//{
//   return _adapter;
//}

inline const String & ProviderModule::getProviderName(void) const
{
    return(_providerName);
}

inline CIMProvider * ProviderModule::getProvider(void) const
{
    return(_provider);
}

inline const Uint32 & ProviderModule::getRefCount(void) const
{
    return(_refCount);
}

PEGASUS_NAMESPACE_END

#endif
