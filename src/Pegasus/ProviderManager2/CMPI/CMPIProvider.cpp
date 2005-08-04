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

#include "CMPI_Version.h"

#include "CMPIProvider.h"

#include "CMPI_Object.h"
#include "CMPI_Broker.h"
#include "CMPI_ContextArgs.h"
#include "CMPI_Ftabs.h"

#include <Pegasus/Common/Tracer.h>
#include <Pegasus/ProviderManager2/CMPI/CMPIProvider.h>
#include <Pegasus/ProviderManager2/CMPI/CMPIProviderModule.h>
#include <Pegasus/ProviderManager2/CMPI/CMPILocalProviderManager.h>

PEGASUS_USING_STD;
PEGASUS_NAMESPACE_BEGIN

// set current operations to 1 to prevent an unload
// until the provider has had a chance to initialize
CMPIProvider::CMPIProvider(const String & name,
		   CMPIProviderModule *module,
		   ProviderVector *mv)
   : _status(UNINITIALIZED), _module(module), _cimom_handle(0), _name(name),
     _no_unload(0), _rm(0), _threadWatchList(true), _cleanedThreads(true)

{
   _current_operations = 1;
   _currentSubscriptions = 0;
   broker.hdl =0;
   broker.provider = this;
   if (mv) miVector=*mv;
   noUnload=false; 
}

CMPIProvider::CMPIProvider(CMPIProvider *pr)
  : _status(UNINITIALIZED), _module(pr->_module), _cimom_handle(0), _name(pr->_name),
    _no_unload(0), _rm(0), _threadWatchList(true), _cleanedThreads(true)
{
   _current_operations = 1;
   _currentSubscriptions = 0;
   miVector=pr->miVector;
   broker.hdl =0;
   broker.provider = this;
   _cimom_handle=new CIMOMHandle();
   noUnload=pr->noUnload; 
}

CMPIProvider::~CMPIProvider(void)
{
    delete (CIMOMHandle*)broker.hdl;
	PEGASUS_ASSERT ( _threadWatchList.size () == 0 );
	PEGASUS_ASSERT ( _cleanedThreads.size () == 0 );	
}

CMPIProvider::Status CMPIProvider::getStatus(void)
{
    AutoMutex lock(_statusMutex);
    return(_status);
}

void CMPIProvider::set(CMPIProviderModule *&module,
                    ProviderVector cmpiProvider,
                    CIMOMHandle *&cimomHandle)
{
    _module = module;
    miVector = cmpiProvider;
    _cimom_handle = cimomHandle;
}

void CMPIProvider::reset()
{
    _module = 0;
    _cimom_handle = 0;
    _no_unload = 0;
    _status = UNINITIALIZED;
}

CMPIProviderModule *CMPIProvider::getModule(void) const
{
    return(_module);
}

String CMPIProvider::getName(void) const
{
    return(_name.subString(1,PEG_NOT_FOUND));
}
void setError(ProviderVector &miVector,
                String &error,
		const String &realProviderName,
                  const char *generic,
                  const char *spec) 
{

   if (miVector.genericMode)
           error.append(generic);
   else
      {
           error.append(realProviderName);
           error.append(spec);
      }
  error.append(", ");
}

void CMPIProvider::initialize(CIMOMHandle & cimom,
                              ProviderVector & miVector,
			      String & name,
                              CMPI_Broker & broker)
{
        broker.hdl=new CIMOMHandle(cimom);
        broker.bft=CMPI_Broker_Ftab;
        broker.eft=CMPI_BrokerEnc_Ftab;
        broker.xft=CMPI_BrokerExt_Ftab;
        broker.clsCache=NULL;
        broker.name=name;

        const OperationContext opc;
        CMPI_ContextOnStack eCtx(opc);
        CMPI_ThreadContext thr(&broker,&eCtx);
	    CMPIStatus rc = {CMPI_RC_OK, NULL};
		String error = String::EMPTY;
  	    String realProviderName(name);

        if (miVector.genericMode) {
           CString mName=realProviderName.getCString();
           if (miVector.miTypes & CMPI_MIType_Instance)
              miVector.instMI=miVector.createGenInstMI(&broker,&eCtx,mName, &rc);
           if (miVector.miTypes & CMPI_MIType_Association)
              miVector.assocMI=miVector.createGenAssocMI(&broker,&eCtx,mName, &rc);
           if (miVector.miTypes & CMPI_MIType_Method) 
              miVector.methMI=miVector.createGenMethMI(&broker,&eCtx,mName, &rc);
           if (miVector.miTypes & CMPI_MIType_Property)
              miVector.propMI=miVector.createGenPropMI(&broker,&eCtx,mName, &rc);
           if (miVector.miTypes & CMPI_MIType_Indication)
              miVector.indMI=miVector.createGenIndMI(&broker,&eCtx,mName, &rc);
        }
        else {
           if (miVector.miTypes & CMPI_MIType_Instance)
              miVector.instMI=miVector.createInstMI(&broker,&eCtx, &rc);
           if (miVector.miTypes & CMPI_MIType_Association)
              miVector.assocMI=miVector.createAssocMI(&broker,&eCtx, &rc);
           if (miVector.miTypes & CMPI_MIType_Method) 
              miVector.methMI=miVector.createMethMI(&broker,&eCtx, &rc);
           if (miVector.miTypes & CMPI_MIType_Property)
              miVector.propMI=miVector.createPropMI(&broker,&eCtx, &rc);
           if (miVector.miTypes & CMPI_MIType_Indication)
              miVector.indMI=miVector.createIndMI(&broker,&eCtx, &rc);
        }
 
           if (miVector.miTypes & CMPI_MIType_Instance)
              if (miVector.instMI == NULL || rc.rc != CMPI_RC_OK)
			setError(miVector, error, realProviderName, _Generic_Create_InstanceMI, _Create_InstanceMI);
           if (miVector.miTypes & CMPI_MIType_Association)
              if (miVector.assocMI == NULL || rc.rc != CMPI_RC_OK)
			setError(miVector, error, realProviderName, _Generic_Create_AssociationMI, _Create_AssociationMI);
           if (miVector.miTypes & CMPI_MIType_Method) 
              if (miVector.methMI == NULL || rc.rc != CMPI_RC_OK)
			setError(miVector, error, realProviderName, _Generic_Create_MethodMI, _Create_MethodMI);
           if (miVector.miTypes & CMPI_MIType_Property)
			 if (miVector.propMI == NULL || rc.rc != CMPI_RC_OK)
			setError(miVector, error, realProviderName, _Generic_Create_PropertyMI, _Create_PropertyMI);
           if (miVector.miTypes & CMPI_MIType_Indication)
              if (miVector.indMI == NULL || rc.rc != CMPI_RC_OK)
			setError(miVector, error, realProviderName, _Generic_Create_IndicationMI, _Create_IndicationMI);

		if (error.size() != 0) 
		 {
			delete (CIMOMHandle*)broker.hdl;
			broker.hdl =0;
		    	throw Exception(MessageLoaderParms("ProviderManager.CMPI.CMPIProvider.CANNOT_INIT_API",
            			"ProviderInitFailure: Error initializing $0 the following API(s): $1",
				realProviderName,
				error.subString(0, error.size()-2)));
		  }
}

void CMPIProvider::initialize(CIMOMHandle & cimom)
{
    String providername(getName());

    if(_status == UNINITIALIZED)
  {
      String compoundName;
      if (_location.size() == 0)
			compoundName= providername;
      else
			compoundName=_location+":"+providername;
      try {
	 // yield before a potentially lengthy operation.
        pegasus_yield();
	 CMPIProvider::initialize(cimom,miVector,compoundName,broker);
	      if (miVector.miTypes & CMPI_MIType_Method) {
	        if (miVector.methMI->ft->miName==NULL) noUnload=true;
	      }
      }
      catch(...) {
        _current_operations = 0;
	      throw;
      }
      _status = INITIALIZED;
      _current_operations = 0;
  }
}

Boolean CMPIProvider::tryTerminate(void)
{
  Boolean terminated = false;

  if(_status == INITIALIZED)
  { 
   if(false == unload_ok())
   {
      return false;
   }

   Status savedStatus=_status;

      // yield before a potentially lengthy operation.
      pegasus_yield();
      try
      {
	if (noUnload==false) {
		// False means that the CIMServer is not shutting down.
	   _terminate(false);
	   if (noUnload==true) {
	      _status=savedStatus;
	      return false;
	   }
	   terminated=true;
	 }
      }
      catch(...)
      {
	 PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
			  "Exception caught in CMPIProviderFacade::tryTerminate() for " +
			  getName());
	 terminated = false;
	 
      }
   if(terminated == true)
   {   
    _status = UNINITIALIZED;
   }
  }
  return terminated;
}

/*
 Terminates the CMPIProvider by cleaning its class cache and 
 calling its cleanup funtions.

 @argument terminating When set to false, the provider may resist terminating.
  	If true, provider MUST clean up.
*/ 
void CMPIProvider::_terminate(Boolean terminating)
{
    if (broker.clsCache) {
	ClassCache::Iterator i=broker.clsCache->start();
	for (; i; i++) {
        delete i.value(); }
	delete broker.clsCache;
	broker.clsCache=NULL;
    }

    const OperationContext opc;
    CMPIStatus rc={CMPI_RC_OK,NULL};
    CMPI_ContextOnStack eCtx(opc);
    CMPI_ThreadContext thr(&broker,&eCtx);
/*
 @param terminating When true, the terminating argument indicates that the MB is in the process of
     terminating and that cleanup must be done. When set to false, the MI may respond with
     CMPI_IRC_DO_NOT_UNLOAD, or CMPI_IRC_NEVER_UNLOAD, indicating that unload will
     interfere with current MI processing.
     @return Function return status. The following CMPIrc codes shall be recognized:
        CMPI_RC_OK Operation successful.
        CMPI_RC_ERR_FAILED Unspecific error occurred.
        CMPI_RC_DO_NOT_UNLOAD Operation successful - do not unload now.
        CMPI_RC_NEVER_UNLOAD Operation successful - never unload.
*/
    if (miVector.miTypes & CMPI_MIType_Instance) {
       rc=miVector.instMI->ft->cleanup(miVector.instMI,&eCtx, terminating);
       if (rc.rc==CMPI_RC_ERR_NOT_SUPPORTED) noUnload=true;
	   if ((rc.rc == CMPI_RC_DO_NOT_UNLOAD) || (rc.rc==CMPI_RC_NEVER_UNLOAD)) noUnload =true;
    }   
    if (miVector.miTypes & CMPI_MIType_Association) {
       rc=miVector.assocMI->ft->cleanup(miVector.assocMI,&eCtx, terminating);
       if (rc.rc==CMPI_RC_ERR_NOT_SUPPORTED) noUnload=true;
	   if ((rc.rc == CMPI_RC_DO_NOT_UNLOAD) || (rc.rc==CMPI_RC_NEVER_UNLOAD)) noUnload =true;
    }   
    if (miVector.miTypes & CMPI_MIType_Method) {
       rc=miVector.methMI->ft->cleanup(miVector.methMI,&eCtx, terminating);
       if (rc.rc==CMPI_RC_ERR_NOT_SUPPORTED) noUnload=true;
	   if ((rc.rc == CMPI_RC_DO_NOT_UNLOAD) || (rc.rc==CMPI_RC_NEVER_UNLOAD)) noUnload =true;
    }   
    if (miVector.miTypes & CMPI_MIType_Property) {
       rc=miVector.propMI->ft->cleanup(miVector.propMI,&eCtx, terminating);
       if (rc.rc==CMPI_RC_ERR_NOT_SUPPORTED) noUnload=true;
	   if ((rc.rc == CMPI_RC_DO_NOT_UNLOAD) || (rc.rc==CMPI_RC_NEVER_UNLOAD)) noUnload =true;
    }   
    if (miVector.miTypes & CMPI_MIType_Indication) {
       rc=miVector.indMI->ft->cleanup(miVector.indMI,&eCtx, terminating);
       if (rc.rc==CMPI_RC_ERR_NOT_SUPPORTED) noUnload=true;
	   if ((rc.rc == CMPI_RC_DO_NOT_UNLOAD) || (rc.rc==CMPI_RC_NEVER_UNLOAD)) noUnload =true;
    }   
    if (noUnload == false) 
    {
	  // Check the thread list to make sure the thread has been de-allocated
	  if (_threadWatchList.size() != 0)
	  {
	 	Tracer::trace(TRC_PROVIDERMANAGER, Tracer::LEVEL2,
			  "There are %d provider threads in %s that have to be cleaned up.",
			_threadWatchList.size(), (const char *)getName().getCString());
		
		// Walk through the list and terminate the threads. After they are 
		// terminated, put them back on the watch list, call the cleanup function
		// and wait until the cleanup is completed.
		while (_threadWatchList.count() > 0) {
				// Remove the thread from the watch list and kill it.
                Thread *t = _threadWatchList.remove_first();

		// If this a non-production build, DO NOT do the cancellation. This is 
		// done so that the provider developer will notice incorrect behaviour
		// when unloading his/her provider and hopefully fix that.
#if !defined(PEGASUS_DEBUG)
	#if defined(PEGASUS_PLATFORM_LINUX_GENERIC_GNU)
	 			Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, 
							Logger::WARNING,
							"Provider thread in $0 did not exit after cleanup function. Attempting to terminate it.",
					 		(const char *)getName().getCString());
				t->cancel();
	#else
	// Every other OS that we do not want to do cancellation for.
	 			Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, 
							Logger::WARNING,
			  				"Provider thread in $0 did not exit after cleanup function. Ignoring it.",
					 		(const char *)getName().getCString());
	#endif
#else 
	// For the non-release
	 			Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, 
							Logger::WARNING,
			  				"Provider thread in $0 did not exit after cleanup function. Ignoring it.",
					 		(const char *)getName().getCString());
				// The cancellation is disabled so that when the join happends
				// the CIMServer will hang. This should help the provider writer to fix 
				// his/her providers.
				//t->cancel();
#endif
				//  and perform the normal  cleanup procedure
				_threadWatchList.insert_last(t);
				removeThreadFromWatch(t);	
		}
		// Wait until all of the threads have been cleaned.
		waitUntilThreadsDone();
	  }
    }
}


void CMPIProvider::terminate()
{
  Status savedStatus=_status;     
  if(_status == INITIALIZED)
  {
	  // yield before a potentially lengthy operation.
	  pegasus_yield();
	  try 
    {
	
        _terminate(true);
	      if (noUnload==true) {
            _status=savedStatus;
	          return;
	      }
    }
        catch(...)
    {
	      PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4, 
			       "Exception caught in CMPIProviderFacade::Terminate for " + 
			       getName());
      throw;
    }
  }
  _status = UNINITIALIZED;
}

/*
 * Wait until all finished provider threads have been cleaned and deleted.
 * Note: This should NEVER be called from the thread that IS the Thread object that was
 * is finished and called 'removeThreadFromWatch()' . If you do it, you will
 * wait forever.
 */
void 
CMPIProvider::waitUntilThreadsDone()
{
	while (_cleanedThreads.size() > 0)
	{
		pegasus_yield();
	}
}
/*
 * Check if the Thread is owner by this CMPIProvider object.
 *
 * @argument t Thread that is not NULL.
 */
Boolean
CMPIProvider::isThreadOwner(Thread *t)  
{
	PEGASUS_ASSERT ( t != NULL );
	if  ( _cleanedThreads.exists(t) )
		return true;
	if  ( !_threadWatchList.exists(t) )
		return true;

	return false;
}
/*
 * Remove the thread from the list of threads that are being deleted
 * by the CMPILocalProviderManager.
 *
 * @argument t Thread which has been previously provided to 'removeThreadFromWatch' function.
 */
void
CMPIProvider::threadDelete(Thread *t)
{
	PEGASUS_ASSERT ( _cleanedThreads.exists(t) );
	PEGASUS_ASSERT ( !_threadWatchList.exists(t) );
	_cleanedThreads.remove( t );
}

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
void 
CMPIProvider::removeThreadFromWatch(Thread *t)
{
	PEGASUS_ASSERT( t != 0 );

	PEGASUS_ASSERT (_threadWatchList.exists (t));
	PEGASUS_ASSERT (!_cleanedThreads.exists (t));

	// Add the thread to the CMPIProvider's list.
	// We use this list to keep track of threads that are 
	// being cleaned (this way 'waitUntilThreadsDone' can stall until the
	// threads are truly deleted). 
	_cleanedThreads.insert_last (t);

	// and remove it from the watched list
    _threadWatchList.remove(t);
 
	CMPILocalProviderManager::cleanupThread(t, this);
}

/* 
 * Adds the thread to the watch list. The watch list is monitored when the
 * provider is terminated and if any of the threads have not cleaned up by
 * that time, they are forcifully terminated and cleaned up.
 *
 * @argument t Thread is not NULL.
*/
void 
CMPIProvider::addThreadToWatch(Thread *t)
{
	PEGASUS_ASSERT( t != 0 );

    _threadWatchList.insert_last(t);
}
Boolean CMPIProvider::operator == (const void *key) const 
{
   if( (void *)this == key)
      return true;
   return false;
}

Boolean CMPIProvider::operator == (const CMPIProvider &prov) const
{
   if(String::equalNoCase(_name, prov._name))
      return true;
   return false;
}

void CMPIProvider::get_idle_timer(struct timeval *t)
{
   if(t && _cimom_handle)
      _cimom_handle->get_idle_timer(t);
}

void CMPIProvider::update_idle_timer(void)
{
   if(_cimom_handle)
      _cimom_handle->update_idle_timer();
}

Boolean CMPIProvider::pending_operation(void)
{
   if(_cimom_handle)
      return _cimom_handle->pending_operation();
   return false; 
}


Boolean CMPIProvider::unload_ok(void)
{
   if (noUnload==true) return false;
   if(_no_unload.value() )
      return false;
   
   if(_cimom_handle)
      return _cimom_handle->unload_ok();
   return true;
}

//   force provider manager to keep in memory
void CMPIProvider::protect(void)
{ 
   _no_unload++;
}

// allow provider manager to unload when idle 
void CMPIProvider::unprotect(void)
{ 
   _no_unload--;
}

Boolean CMPIProvider::testIfZeroAndIncrementSubscriptions ()
{
    AutoMutex lock (_currentSubscriptionsMutex);
    Boolean isZero = (_currentSubscriptions == 0);
    _currentSubscriptions++;

    return isZero;
}

Boolean CMPIProvider::decrementSubscriptionsAndTestIfZero ()
{
    AutoMutex lock (_currentSubscriptionsMutex);
    _currentSubscriptions--;
    Boolean isZero = (_currentSubscriptions == 0);

    return isZero;
}

Boolean CMPIProvider::testSubscriptions ()
{
    AutoMutex lock (_currentSubscriptionsMutex);
    Boolean currentSubscriptions = (_currentSubscriptions > 0);

    return currentSubscriptions;
}

void CMPIProvider::resetSubscriptions ()
{
    AutoMutex lock (_currentSubscriptionsMutex);
    _currentSubscriptions = 0;
}

void CMPIProvider::setProviderInstance (const CIMInstance & instance)
{
    _providerInstance = instance;
}

CIMInstance CMPIProvider::getProviderInstance ()
{
    return _providerInstance;
}

PEGASUS_NAMESPACE_END
