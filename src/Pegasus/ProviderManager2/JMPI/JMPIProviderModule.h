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

#ifndef Pegasus_JMPIProviderModule_h
#define Pegasus_JMPIProviderModule_h

#include "JMPIImpl.h"

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/IPC.h>

#include <Pegasus/Provider/CIMProvider.h>
#include <Pegasus/ProviderManager2/JMPI/JMPIProvider.h>

#include <Pegasus/Server/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

// The JMPIProviderModule class represents the physical module, as defined by the
// operation, that contains a provider. This class effectively encapsulates the
// "physical" portion of a provider.


class PEGASUS_SERVER_LINKAGE JMPIProviderModule
{

    friend class JMPILocalProviderManager;

public:
    virtual ~JMPIProviderModule(void);
    const String & getFileName(void) const;
    ProviderVector load(const String & providerName);
    void unloadModule(void);

protected:
    String _fileName;
    String _className;
    String _interfaceName;
    AtomicInt _ref_count;
    DynamicLibraryHandle _library;
    Uint32 _refCount;
    void *jProviderClass;
    void *jProvider;

private:
    JMPIProviderModule(const String & fileName, const String & interfaceName);
    const String & getProviderName(void) const;
    const String & getInterfaceName(void) const ;
    virtual CIMProvider * getProvider(void) const;

    String _providerName;
    CIMProvider * _provider;
};

inline const String & JMPIProviderModule::getFileName(void) const
{
    return(_fileName);
}

inline const String & JMPIProviderModule::getInterfaceName(void) const
{
    return(_interfaceName);
}

inline const String & JMPIProviderModule::getProviderName(void) const
{
    return(_providerName);
}

inline CIMProvider * JMPIProviderModule::getProvider(void) const
{
    return(_provider);
}

PEGASUS_NAMESPACE_END

#endif
