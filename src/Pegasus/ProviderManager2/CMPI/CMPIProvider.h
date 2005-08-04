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
//              Adrian Schuur, schuur@de.ibm.com
//              Dan Gorey, IBM djgorey@us.ibm.com
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_CMPIProvider_h
#define Pegasus_CMPIProvider_h

#include "CMPI_Object.h"
#include "CMPI_Broker.h"
#include "CMPI_Version.h"
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/IPC.h>
#include <Pegasus/Common/Thread.h>
#include <Pegasus/Provider/CIMOMHandle.h>
#include <Pegasus/Provider/CIMOMHandle.h>
#include <Pegasus/Provider/CIMInstanceProvider.h>
#include <Pegasus/Provider/CIMAssociationProvider.h>
#include <Pegasus/Provider/CIMMethodProvider.h>
#include <Pegasus/ProviderManager2/CMPI/CMPIResolverModule.h>

#include <Pegasus/ProviderManager2/CMPI/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

class CMPIProviderModule;
class CMPIResolverModule;

#ifndef _CMPI_Broker_H_
struct CMPI_Broker;
#endif

#define CMPI_MIType_Instance    1
#define CMPI_MIType_Association 2
#define CMPI_MIType_Method      4
#define CMPI_MIType_Property    8
#define CMPI_MIType_Indication 16

/* Ver 1.00 CMPI spec - which added a new parameter. */
#ifdef CMPI_VER_100

typedef CMPIInstanceMI* 	(*CREATE_INST_MI)(const CMPIBroker*,const CMPIContext*,  CMPIStatus *rc);
typedef CMPIAssociationMI* 	(*CREATE_ASSOC_MI)(const CMPIBroker*,const CMPIContext*,  CMPIStatus *rc);
typedef CMPIMethodMI* 		(*CREATE_METH_MI)(const CMPIBroker*,const CMPIContext*,  CMPIStatus *rc);
typedef CMPIPropertyMI* 	(*CREATE_PROP_MI)(const CMPIBroker*,const CMPIContext*,  CMPIStatus *rc);
typedef CMPIIndicationMI* 	(*CREATE_IND_MI)(const CMPIBroker*,const CMPIContext*,  CMPIStatus *rc);

typedef CMPIInstanceMI*    (*CREATE_GEN_INST_MI)(const CMPIBroker*,const CMPIContext*,const char*,  CMPIStatus *rc);
typedef CMPIAssociationMI* (*CREATE_GEN_ASSOC_MI)(const CMPIBroker*,const CMPIContext*,const char*, CMPIStatus *rc);
typedef CMPIMethodMI* 	    (*CREATE_GEN_METH_MI)(const CMPIBroker*,const CMPIContext*,const char*, CMPIStatus *rc);
typedef CMPIPropertyMI*    (*CREATE_GEN_PROP_MI)(const CMPIBroker*,const CMPIContext*,const char*, CMPIStatus *rc);
typedef CMPIIndicationMI*  (*CREATE_GEN_IND_MI)(const CMPIBroker*,const CMPIContext*,const char*,  CMPIStatus *rc);

#else
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
#endif

#define _Generic_Create_InstanceMI "_Generic_Create_InstanceMI"
#define _Generic_Create_AssociationMI "_Generic_Create_AssociationMI"
#define _Generic_Create_MethodMI "_Generic_Create_MethodMI"
#define _Generic_Create_PropertyMI "_Generic_Create_PropertyMI"
#define _Generic_Create_IndicationMI "_Generic_Create_IndicationMI"

#define _Create_InstanceMI "_Create_InstanceMI"
#define _Create_AssociationMI "_Create_AssociationMI"
#define _Create_MethodMI "_Create_MethodMI"
#define _Create_PropertyMI "_Create_PropertyMI"
#define _Create_IndicationMI "_Create_IndicationMI"

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

class PEGASUS_CMPIPM_LINKAGE CMPIProvider :
                       public virtual CIMProvider
{
public:

    enum Status
    {
        UNINITIALIZED,
        INITIALIZED
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

    CMPIProvider(CMPIProvider*);

    virtual ~CMPIProvider(void);

    virtual void initialize(CIMOMHandle & cimom);

    void setLocation(String loc) { _location=loc; }

    virtual Boolean tryTerminate(void);
    virtual void terminate(void);

    Status getStatus(void);
    String getName(void) const;
    void setResolver(CMPIResolverModule *rm) { _rm=rm; }

    void reset();

    // Monitors threads that the provider has allocated.

	/* 
     * Adds the thread to the watch list. The watch list is monitored when the
     * provider is terminated and if any of the threads have not cleaned up by
     * that time, they are forcifully terminated and cleaned up.
     *
     * @argument t Thread is not NULL.
     */
    void addThreadToWatch(Thread *t);
  /*
  // Removes the thread from the watch list and schedule the CMPILocalProviderManager
  // to delete the thread. The CMPILocalProviderManager after deleting the thread calls
  // the CMPIProvider' "cleanupThread". The CMPILocalProviderManager notifies this
  // CMPIProvider object when the thread is truly dead by calling "threadDeleted" function.
  //
  // Note that this function is called from the thread that finished with
  // running the providers function, and returns immediately while scheduling the
  // a cleanup procedure. If you want to wait until the thread is truly deleted,
  // call 'waitUntilThreadsDone' - but DO NOT do it in the the thread that
  // the Thread owns - you will wait forever.
  //
  // @argument t Thread that is not NULL and finished with running the provider function.
  */
    void removeThreadFromWatch(Thread *t);

	/*
 	* Remove the thread from the list of threads that are being deleted
 	* by the CMPILocalProviderManager.
 	*
 	* @argument t Thread which has been previously provided to 'removeThreadFromWatch' function.
 	*/
	void threadDelete(Thread *t);
	/*
 	* Check if the Thread is owner by this CMPIProvider object	.
 	*
 	* @argument t Thread that is not NULL.
 	*/
	Boolean isThreadOwner( Thread *t); 

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

    /**
        Increments the count of current subscriptions for this provider, and
        determines if there were no current subscriptions before the increment.
        If there were no current subscriptions before the increment, the first
        subscription has been created, and the provider's enableIndications
        method should be called.

        @return  True, if before the increment there were no current
                       subscriptions for this provider;
                 False, otherwise
     */
    Boolean testIfZeroAndIncrementSubscriptions ();

    /**
        Decrements the count of current subscriptions for this provider, and
        determines if there are no current subscriptions after the decrement.
        If there are no current subscriptions after the decrement, the last
        subscription has been deleted, and the provider's disableIndications
        method should be called.

        @return  True, if after the decrement there are no current subscriptions
                       for this provider;
                 False, otherwise
     */
    Boolean decrementSubscriptionsAndTestIfZero ();

    /**
        Determines if there are current subscriptions for this provider.

        @return  True, if there is at least one current subscription
                       for this provider;
                 False, otherwise
     */
    Boolean testSubscriptions ();

    /**
        Resets the count of current subscriptions for the indication provider.
     */
    void resetSubscriptions ();

    /**
        Sets the provider instance for the provider.

        Note: the provider instance is set only for an indication provider, and
        is set when a Create Subscription request is processed for the provider.

        @param  instance  the Provider CIMInstance for the provider
     */
    void setProviderInstance (const CIMInstance & instance);

    /**
        Gets the provider instance for the provider.

        Note: the provider instance is set only for an indication provider, and
        only if a Create Subscription request has been processed for the
        provider.

        @return  the Provider CIMInstance for the provider
     */
    CIMInstance getProviderInstance ();

protected:
    String _location;
    Status _status;
    CMPIProviderModule *_module;
    ProviderVector miVector;
    CMPI_Broker broker;
    Boolean noUnload;

private:
    virtual void _terminate(Boolean term);
    CMPIProvider(const String & name,
        CMPIProviderModule *module,
        ProviderVector *mv);

    static void initialize(CIMOMHandle & cimom,
                           ProviderVector & miVector,
			   			   String & name,
                           CMPI_Broker & broker);
	
   /*
 	* Wait until all finished provider threads have been cleaned and deleted.
 	* Note: This should NEVER be called from the thread that IS the Thread object that was
 	* is finished and called 'removeThreadFromWatch()' . If you do it, you will
 	* wait forever.
 	*/
    void waitUntilThreadsDone();

    void set(CMPIProviderModule *&module,
            ProviderVector base,
            CIMOMHandle *&cimomHandle);

    friend class CMPILocalProviderManager;
    friend class CMPIProviderManager;
    friend class ProviderManagerService;
    class OpProviderHolder;
    friend class OpProviderHolder;
    CIMOMHandle *_cimom_handle;
    String _name;
    AtomicInt _no_unload;
    CMPIResolverModule *_rm;
    Uint32 _quantum;
    AtomicInt _current_operations;
    Mutex _statusMutex;

	/*
 		List of threads which are monitored and cleaned.
 	*/
    DQueue<Thread> _threadWatchList;
    DQueue<Thread> _cleanedThreads;


    /**
        Count of current subscriptions for this provider.  Access to this
        data member is controlled by the _currentSubscriptionsLock.
     */
    Uint32 _currentSubscriptions;

    /**
        A mutex to control access to the _currentSubscriptions member variable.
        Before any access (test, increment, decrement or reset) of the
        _currentSubscriptions member variable, the _currentSubscriptionsMutex is
        first locked.
     */
    Mutex _currentSubscriptionsMutex;

    /**
        The Provider CIMInstance for the provider.
        The Provider CIMInstance is set only for indication providers, and only
        if a Create Subscription request has been processed for the provider.
        The Provider CIMInstance is needed in order to construct the
        EnableIndicationsResponseHandler to send to the indication provider
        when the provider's enableIndications() method is called.
        The Provider CIMInstance is needed in the
        EnableIndicationsResponseHandler in order to construct the Process
        Indication request when an indication is delivered by the provider.
        The Provider CIMInstance is needed in the Process Indication request
        to enable the Indication Service to determine if the provider that
        generated the indication accepted a matching subscription.
     */
    CIMInstance _providerInstance;
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
