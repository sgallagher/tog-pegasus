//%LICENSE////////////////////////////////////////////////////////////////
//
// Licensed to The Open Group (TOG) under one or more contributor license
// agreements.  Refer to the OpenPegasusNOTICE.txt file distributed with
// this work for additional information regarding copyright ownership.
// Each contributor licenses this file to you under the OpenPegasus Open
// Source License; you may not use this file except in compliance with the
// License.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//////////////////////////////////////////////////////////////////////////
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_CMPILocalProviderManager_h
#define Pegasus_CMPILocalProviderManager_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/HashTable.h>
#include <Pegasus/Common/ArrayInternal.h>

#include <Pegasus/ProviderManager2/CMPI/CMPIProvider.h>

#include <Pegasus/ProviderManager2/CMPI/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

class PEGASUS_CMPIPM_LINKAGE CMPILocalProviderManager
{

public:
    CMPILocalProviderManager();
    virtual ~CMPILocalProviderManager();

public:
    OpProviderHolder getProvider(
        const String & fileName, 
        const String & providerName);

    OpProviderHolder getRemoteProvider(
        const String & fileName, 
        const String & providerName);

    Boolean unloadProvider(
        const String & fileName,
        const String & providerName);

    void shutdownAllProviders();

    Boolean hasActiveProviders();
    void unloadIdleProviders();
    Boolean isProviderActive(const String &providerName);

    /**
         Gets list of indication providers to be enabled.
         Once IndicationService initialization has been completed, the
         enableIndications() method must be called on each indication provider
         that has current subscriptions.

         @return list of providers whose enableIndications() method must be
                 called
     */
    Array <CMPIProvider *> getIndicationProvidersToEnable ();
    static void cleanupThread(Thread *t, CMPIProvider *p);

private:
    enum CTRL
    {
        INSERT_PROVIDER,
        INSERT_MODULE,
        LOOKUP_PROVIDER,
        LOOKUP_MODULE,
        GET_PROVIDER,
        UNLOAD_PROVIDER,
        UNLOAD_ALL_PROVIDERS,
        UNLOAD_IDLE_PROVIDERS
    };

    typedef HashTable<String, CMPIProvider *,
        EqualFunc<String>,  HashFunc<String> > ResolverTable;

    typedef HashTable<String, CMPIProvider *,
        EqualFunc<String>,  HashFunc<String> > ProviderTable;

    typedef HashTable<String, CMPIProviderModule *,
        EqualFunc<String>, HashFunc<String> > ModuleTable;

    typedef struct
    {
        const String *providerName;
        const String *fileName;
        const String *location;
    } CTRL_STRINGS;

    ResolverTable _resolvers;
    ProviderTable _providers;
    ModuleTable _modules;
    Uint32 _idle_timeout;
    Sint32 _provider_ctrl(CTRL code, void *parm, void *ret);

    CMPIProvider* _initProvider(CMPIProvider * provider,
        const String & moduleFileName); 

    void _unloadProvider(CMPIProvider * provider, Boolean forceUnload = false);

    void _terminateUnloadPendingProviders(
        Array<CMPIProvider*> &unloadPendingProviders);

    CMPIProvider * _lookupProvider(const String & providerName);

    CMPIProviderModule * _lookupModule(const String & moduleFileName);
    Mutex _providerTableMutex;

    /*
    *  The cleaning functions for provider threads.
    */

    static ThreadReturnType PEGASUS_THREAD_CDECL _reaper(void *);

    /*
     * The data structures for holding the thread and the CMPIProvider
     */

    struct cleanupThreadRecord : public Linkable 
    {
        cleanupThreadRecord(): thread(0), provider(0)
        {
        }
        cleanupThreadRecord(Thread *t, CMPIProvider *p): thread(t), provider(p)
        {
        }
        Thread *thread;
        CMPIProvider *provider;
    };

    static Thread* _reaperThread;
    static Semaphore _pollingSem;
    static AtomicInt _stopPolling;
    static Mutex _reaperMutex;
    static List<cleanupThreadRecord,Mutex> _finishedThreadList;

protected:

};

PEGASUS_NAMESPACE_END

#endif

    
