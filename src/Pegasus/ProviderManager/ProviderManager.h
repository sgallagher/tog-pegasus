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
//              Jenny Yu, Hewlett-Packard Company(jenny_yu@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_ProviderManager_h
#define Pegasus_ProviderManager_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/IPC.h>
#include <Pegasus/Common/DQueue.h>
#include <Pegasus/Common/HashTable.h>

#include <Pegasus/ProviderManager/Lockable.h>
#include <Pegasus/ProviderManager/Provider.h>
#include <Pegasus/ProviderManager/ProviderModule.h>

PEGASUS_NAMESPACE_BEGIN

class PEGASUS_SERVER_LINKAGE ProviderManager
{
public:
    ProviderManager(void);
    virtual ~ProviderManager(void);

public:
    Provider getProvider(const String & fileName, const String & providerName,
                         const String & interfaceName = String::EMPTY) ;

    void unloadProvider(const String & fileName, const String & providerName) ;

    void shutdownAllProviders(void) ;

    static PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL provider_monitor(void *);
    
    void unload_idle_providers(void) ;
private:
    enum CTRL 
      {
	INSERT_PROVIDER,
	INSERT_MODULE,
	REMOVE_PROVIDER, 
	REMOVE_MODULE,
	LOOKUP_PROVIDER, 
	LOOKUP_MODULE, 
	GET_PROVIDER, 
	UNLOAD_PROVIDER, 
	UNLOAD_ALL_PROVIDERS, 
	UNLOAD_IDLE_PROVIDERS, 
	UNLOAD_IDLE_MODULES
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
    
    ProviderTable _providers;
    ModuleTable _modules;
    Uint32 _idle_timeout;

    Sint32 _provider_ctrl(CTRL code, void *parm, void *ret);
    AtomicInt _unload_idle_flag;
    
    Mutex _mut;
    
protected:
    
};

PEGASUS_NAMESPACE_END

#endif
