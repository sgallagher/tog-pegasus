//%/////////////////////////////////////////////////////////////////////////////
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
// ACTION OF CONTRgACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//==============================================================================
//
// Author: Chip Vincent (cvincent@us.ibm.com)
//
// Modified By: Yi Zhou, Hewlett-Packard Company(yi_zhou@hp.com)
//              Mike Day, IBM (mdday@us.ibm.com)
//              Adrian Schuur, schuur@de.ibm.com
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_CMPIProvider_h
#define Pegasus_CMPIProvider_h

#include "CMPI_Object.h"
#include "CMPI_Broker.h"

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/IPC.h>
#include <Pegasus/Provider/CIMOMHandle.h>
#include <Pegasus/Provider/CIMInstanceProvider.h>
#include <Pegasus/Provider/CIMAssociationProvider.h>
#include <Pegasus/Provider/CIMMethodProvider.h>

#include <Pegasus/ProviderManager2/CMPI/CMPIResolverModule.h>

#include <Pegasus/Server/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

class CMPIProviderModule;
class CMPIResolverModule;

#define CMPI_MIType_Instance    1
#define CMPI_MIType_Association 2
#define CMPI_MIType_Method      4
#define CMPI_MIType_Property    8
#define CMPI_MIType_Indication 16


typedef CMPIInstanceMI* 	(*CREATE_INST_MI)(CMPIBroker*,CMPIContext*);
typedef CMPIAssociationMI* 	(*CREATE_ASSOC_MI)(CMPIBroker*,CMPIContext*);
typedef CMPIMethodMI* 		(*CREATE_METH_MI)(CMPIBroker*,CMPIContext*);
typedef CMPIPropertyMI* 	(*CREATE_PROP_MI)(CMPIBroker*,CMPIContext*);
typedef CMPIIndicationMI* 	(*CREATE_IND_MI)(CMPIBroker*,CMPIContext*);

typedef CMPIInstanceMI*    (*CREATE_GEN_INST_MI)(CMPIBroker*,CMPIContext*,const char*);
typedef CMPIAssociationMI* (*CREATE_GEN_ASSOC_MI)(CMPIBroker*,CMPIContext*,const char*);
typedef CMPIMethodMI* 	    (*CREATE_GEN_METH_MI)(CMPIBroker*,CMPIContext*,const char*);
typedef CMPIPropertyMI*    (*CREATE_GEN_PROP_MI)(CMPIBroker*,CMPIContext*,const char*);
typedef CMPIIndicationMI*  (*CREATE_GEN_IND_MI)(CMPIBroker*,CMPIContext*,const char*);

struct ProviderVector {
   int			miTypes;
   int			genericMode;
   CMPIInstanceMI 	*instMI;
   CMPIAssociationMI 	*assocMI;
   CMPIMethodMI 	*methMI;
   CMPIPropertyMI 	*propMI;
   CMPIIndicationMI 	*indMI;
   CREATE_INST_MI 	createInstMI;
   CREATE_ASSOC_MI 	createAssocMI;
   CREATE_METH_MI 	createMethMI;
   CREATE_PROP_MI 	createPropMI;
   CREATE_IND_MI 	createIndMI;
   CREATE_GEN_INST_MI 	createGenInstMI;
   CREATE_GEN_ASSOC_MI 	createGenAssocMI;
   CREATE_GEN_METH_MI 	createGenMethMI;
   CREATE_GEN_PROP_MI 	createGenPropMI;
   CREATE_GEN_IND_MI 	createGenIndMI;
};

// The CMPIProvider class represents the logical provider extracted from a
// provider module. It is wrapped in a facade to stabalize the interface
// and is directly tied to a module.

class PEGASUS_SERVER_LINKAGE CMPIProvider :
                       public virtual CIMProvider
{
public:

    enum Status
    {
        UNKNOWN,
        INITIALIZING,
        INITIALIZED,
        TERMINATING,
        TERMINATED
    };

public:


    class pm_service_op_lock {
    private:
       pm_service_op_lock(void);
    public:
       pm_service_op_lock(CMPIProvider *provider) : _provider(provider)
          { _provider->protect(); }
       ~pm_service_op_lock(void)
          { _provider->unprotect(); }
       CMPIProvider * _provider;
    };

 //  typedef CMPIProviderFacade Base;

    CMPIProvider(const String & name,
        CMPIProviderModule *module,
        ProviderVector *mv);
    CMPIProvider(CMPIProvider*);

    virtual ~CMPIProvider(void);

    virtual void initialize(CIMOMHandle & cimom);
    static void initialize(CIMOMHandle & cimom,
                           ProviderVector & miVector,
			   String & name,
                           CMPI_Broker & broker);

    virtual Boolean tryTerminate(void);
    virtual void terminate(void);
    virtual void _terminate(void);

    Status getStatus(void) const;
    String getName(void) const;
    void setResolver(CMPIResolverModule *rm) { _rm=rm; }

    CMPIProviderModule *getModule(void) const;

    // << Mon Oct 14 15:42:24 2002 mdd >> for use with DQueue template
    // to allow conversion from using Array<>
    Boolean operator == (const void *key) const;
    Boolean operator == (const CMPIProvider & prov) const;

    virtual void get_idle_timer(struct timeval *);
    virtual void update_idle_timer(void);
    virtual Boolean pending_operation(void);
    virtual Boolean unload_ok(void);

//   force provider manager to keep in memory
    virtual void protect(void);
// allow provider manager to unload when idle
    virtual void unprotect(void);

protected:
    Status _status;
    CMPIProviderModule *_module;
    ProviderVector miVector;
    CMPI_Broker broker;
    Boolean noUnload;

private:
    friend class CMPILocalProviderManager;
    friend class CMPIProviderManager;
    friend class CMPI_RProviderManager;
    friend class ProviderManagerService;
//    friend class OpProviderHolder;
    CIMOMHandle *_cimom_handle;
    String _name;
    AtomicInt _no_unload;
    CMPIResolverModule *_rm;
    Uint32 _quantum;
    AtomicInt _current_operations;
//};


//
// Used to encapsulate the incrementing/decrementing of the _current_operations
// for a CMPIProvider so it won't be unloaded during operations.
//

   class OpProviderHolder
   {
   private:
       CMPIProvider* _provider;

   public:
       OpProviderHolder(): _provider( NULL )
       {
       }
       OpProviderHolder( const OpProviderHolder& p ): _provider( NULL )
       {
           SetProvider( p._provider );
       }
       OpProviderHolder( CMPIProvider* p ): _provider( NULL )
       {
           SetProvider( p );
       }
       ~OpProviderHolder()
       {
           UnSetProvider();
       }
       CMPIProvider& GetProvider()
       {
           return(*_provider);
       }

       OpProviderHolder& operator=( const OpProviderHolder& x )
       {
           if(this == &x)
               return(*this);
           SetProvider( x._provider );

           return(*this);
       }

       void SetProvider( CMPIProvider* p )
       {
           UnSetProvider();
           if(p)
           {
               _provider = p;
               _provider->_current_operations++;
           }
       }

       void UnSetProvider()
       {
           if(_provider)
           {
               _provider->_current_operations--;
               _provider = NULL;
           }
       }
   };
};

PEGASUS_NAMESPACE_END

#endif
