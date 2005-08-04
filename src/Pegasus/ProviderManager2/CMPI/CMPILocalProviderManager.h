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
//              Adrian Schuur, schuur@de.ibm.com
//              Dan Gorey, IBM djgorey@us.ibm.com
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_CMPILocalProviderManager_h
#define Pegasus_CMPILocalProviderManager_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/IPC.h>
#include <Pegasus/Common/DQueue.h>
#include <Pegasus/Common/HashTable.h>

#include <Pegasus/ProviderManager2/CMPI/CMPIProvider.h>
#include <Pegasus/ProviderManager2/CMPI/CMPIResolverModule.h>

#include <Pegasus/ProviderManager2/Lockable.h>

#include <Pegasus/ProviderManager2/CMPI/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

class PEGASUS_CMPIPM_LINKAGE CMPILocalProviderManager
{

public:
    CMPILocalProviderManager(void);
    virtual ~CMPILocalProviderManager(void);

public:
    CMPIProvider::OpProviderHolder getProvider(const String & fileName, const String & providerName);
         
    CMPIProvider::OpProviderHolder getRemoteProvider(const String & fileName, const String & providerName);

    void unloadProvider(const String & fileName, const String & providerName);

    void shutdownAllProviders(void);

    Boolean hasActiveProviders();
    void unloadIdleProviders();

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

    friend class ProviderManagerService;

    ResolverTable _resolvers;
    ProviderTable _providers;
    ModuleTable _modules;
    Uint32 _idle_timeout;
    Sint32 _provider_ctrl(CTRL code, void *parm, void *ret);

    CMPIProvider* _initProvider(CMPIProvider * provider,
                            const String & moduleFileName); 

    void _unloadProvider(CMPIProvider * provider);

    CMPIProvider * _lookupProvider(const String & providerName);

    CMPIProviderModule * _lookupModule(const String & moduleFileName);
    Mutex _providerTableMutex;
	                                    
   /*
   *  The cleaning functions for provider threads.
   */

   static PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL _reaper(void *);

  /*
   * The data structures for holding the thread and the CMPIProvider
   */

   struct cleanupThreadRecord {
		cleanupThreadRecord(): thread(0), provider(0) {}
		cleanupThreadRecord(Thread *t, CMPIProvider *p): thread(t), provider(p) { }
		Thread *thread;
		CMPIProvider *provider;
   };

   static Thread* _reaperThread;
   static Semaphore _pollingSem;
   static AtomicInt _stopPolling;
   static Mutex _reaperMutex;
   static DQueue<cleanupThreadRecord> _finishedThreadList;

protected:

};

PEGASUS_NAMESPACE_END

#endif

