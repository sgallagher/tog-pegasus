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
//              Jenny Yu, Hewlett-Packard Company(jenny_yu@hp.com)
//              Mike Day, IBM (mdday@us.ibm.com)
//              Dan Gorey, IBM djgorey@us.ibm.com
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                  (carolann_graves@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_LocalProviderManager_h
#define Pegasus_LocalProviderManager_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/IPC.h>
#include <Pegasus/Common/DQueue.h>
#include <Pegasus/Common/HashTable.h>

#include <Pegasus/ProviderManager2/Lockable.h>
#include <Pegasus/ProviderManager2/Default/Provider.h>
#include <Pegasus/ProviderManager2/Default/ProviderModule.h>

#include <Pegasus/ProviderManager2/Default/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

class PEGASUS_DEFPM_LINKAGE LocalProviderManager
{
public:
    LocalProviderManager();
    virtual ~LocalProviderManager();

    OpProviderHolder getProvider(
        const String& moduleFileName,
        const String& providerName);

    void unloadProvider(const String& providerName);

    void shutdownAllProviders();

    Boolean hasActiveProviders();
    void unloadIdleProviders();

    Sint16 disableProvider(const String& providerName);

    /**
        Get the list of indication providers to be enabled.
        Once IndicationService initialization has been completed, the
        enableIndications() method must be called on each indication provider
        that has current subscriptions.

        @return list of providers whose enableIndications() method must be
                called
     */
    Array<Provider*> getIndicationProvidersToEnable();

private:

    Provider* _initProvider(
        Provider* provider,
        const String& moduleFileName);

    void _unloadProvider(Provider* provider);

    Provider* _lookupProvider(const String& providerName);

    ProviderModule* _lookupModule(const String& moduleFileName);

    typedef HashTable<String, Provider*,
        EqualFunc<String>,  HashFunc<String> > ProviderTable;

    typedef HashTable<String, ProviderModule*,
        EqualFunc<String>, HashFunc<String> > ModuleTable;

    /**
        The _providerTableMutex must be locked whenever accessing the
        _providers table or the _modules table.  It is okay to lock a
        Provider::_statusMutex while holding the _providerTableMutex,
        but one should never lock the _providerTableMutex while holding
        a Provider::_statusMutex.
     */
    Mutex _providerTableMutex;

    ProviderTable _providers;
    ModuleTable _modules;
    Uint32 _idle_timeout;
};

PEGASUS_NAMESPACE_END

#endif
