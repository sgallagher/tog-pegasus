//%2003////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002  BMC Software, Hewlett-Packard Development
// Company, L. P., IBM Corp., The Open Group, Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L. P.;
// IBM Corp.; EMC Corporation, The Open Group.
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
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_ProviderManager_h
#define Pegasus_ProviderManager_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/IPC.h>
#include <Pegasus/Common/DQueue.h>
#include <Pegasus/Common/HashTable.h>
#include <Pegasus/Provider/CIMNullProvider.h>
#include <Pegasus/ProviderManager/Lockable.h>
#include <Pegasus/ProviderManager/Provider.h>
#include <Pegasus/ProviderManager/ProviderModule.h>
#include <Pegasus/Server/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

class PEGASUS_SERVER_LINKAGE ProviderManager
{
  
public:
    ProviderManager(void);
    virtual ~ProviderManager(void);

public:
    OpProviderHolder getProvider(const String & fileName, const String & providerName,
                         const String & interfaceName = String::EMPTY) ;

    void unloadProvider(const String & fileName, const String & providerName) ;

    void shutdownAllProviders(void) ;

    static PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL provider_monitor(void *);
    
    void unload_idle_providers(void) ;

    Sint16 disableProvider(const String & fileName, const String & providerName);
    //
    // If there are pending requests, do not disable the provider and return 0;
    // Otherwise, disable the provider. If success, return 1, otherwise, return -1
    //
    Sint16 disableIndicationProvider(const String & fileName, 
				     const String & providerName);

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
        
    typedef HashTable<String, Provider *, 
      EqualFunc<String>,  HashFunc<String> > ProviderTable;
    
    typedef HashTable<String, ProviderModule *,
      EqualFunc<String>, HashFunc<String> > ModuleTable;


    typedef struct 
    {
      const String *providerName;
      const String *fileName;
      const String *interfaceName;
    } CTRL_STRINGS;

    friend class ProviderManagerService;
    
    /**
       A list of Provider entries.  Entries in this table are not to be
       removed.  If a provider is terminated, its corresponding Provider
       entry will be reset, but not deleted.
    */
    ProviderTable _providers;

    /**
       A list of ProviderModule entries.  Entries in this table are not to
       be removed.  If a provider module is unloaded, its corresponding 
       ProviderModule entry will be reset, but not deleted.
    */
    ModuleTable _modules;

    Uint32 _idle_timeout;

    Sint32 _provider_ctrl(CTRL code, void *parm, void *ret);
    AtomicInt _unload_idle_flag;

    /**
       Loads and initializes the specified provider if the provider is
       not already initialized.  If initialization fails, the Provider
       object remains uninitialized.  The _providerTableMutex must NOT
       be locked before calling this method.
    */
    Provider* _initProvider(Provider * provider,
                            const String & moduleFileName, 
                            const String & interfaceName);

    /**
       Terminates and unloads the specified provider if it is not busy.
       The _providerTableMutex must be locked before calling this method.
    */
    void _unloadProvider(Provider * provider);

    /**
       Looks up the specified provider in the provider table.  If the
       provider is not in the provider table, this method creates a
       new Provider entry and insert it into the provider table.
       The _providerTableMutex must NOT be locked before calling this 
       method.
    */
    Provider * _lookupProvider( const String & providerName );

    /**
       Looks up the specified provider module in the module table.  If the
       module is not in the module table, this method creates a new 
       ProviderModule entry and insert it into the provider module table.
       The _providerTableMutex must be locked before calling this method.
    */
    ProviderModule * _lookupModule( const String & moduleFileName,
	                            const String & interfaceName );

    /**
       A mutex to synchronize access to the ProviderTable.  This mutex
       must also be held when loading and unloading provider modules.  In
       order to prevent deadlocks, one should never attempt to acquire a
       lock on the _providerTableMutex while holding a lock on a 
       Provider's _statusMutex.  
    */
    Mutex _providerTableMutex;
    
protected:
    
};

PEGASUS_NAMESPACE_END

#endif
