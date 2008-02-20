//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
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
//%/////////////////////////////////////////////////////////////////////////////

#include "CMPI_Version.h"

#include "CMPIProvider.h"

#include "CMPI_Object.h"
#include "CMPI_Broker.h"
#include "CMPI_ContextArgs.h"
#include "CMPI_Ftabs.h"

#include <Pegasus/Common/Time.h>
#include <Pegasus/ProviderManager2/CMPI/CMPIProvider.h>
#include <Pegasus/ProviderManager2/CMPI/CMPIProviderModule.h>
#include <Pegasus/ProviderManager2/CMPI/CMPILocalProviderManager.h>

PEGASUS_USING_STD;
PEGASUS_NAMESPACE_BEGIN

// set current operations to 1 to prevent an unload
// until the provider has had a chance to initialize
CMPIProvider::CMPIProvider(
    const String & name,
    CMPIProviderModule *module,
    ProviderVector *mv)
    : _status(UNINITIALIZED), _module(module), _cimom_handle(0), _name(name),
    _no_unload(0),  _threadWatchList(), _cleanedThreads()
{
    PEG_METHOD_ENTER(
        TRC_CMPIPROVIDERINTERFACE,
        "CMPIProvider::CMPIProvider()");
    _current_operations = 1;
    _currentSubscriptions = 0;
    broker.hdl =0;
    broker.provider = this;
    if (mv) miVector=*mv;
    unloadStatus = CMPI_RC_DO_NOT_UNLOAD;
    Time::gettimeofday(&_idleTime);
    PEG_METHOD_EXIT();
}

CMPIProvider::~CMPIProvider(void)
{
}

CMPIProvider::Status CMPIProvider::getStatus()
{
    AutoMutex lock(_statusMutex);
    return(_status);
}

void CMPIProvider::set(
    CMPIProviderModule *&module,
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

CMPIProviderModule *CMPIProvider::getModule() const
{
    return(_module);
}

String CMPIProvider::getName() const
{
    return(_name.subString(1,PEG_NOT_FOUND));
}
void setError(
    ProviderVector &miVector,
    String &error,
    const String &realProviderName,
    const char *generic,
    const char *spec,
    const CMPIString *optMsg)
{
    PEG_METHOD_ENTER(TRC_CMPIPROVIDERINTERFACE, "CMPIProvider:setError()");
    if (error.size() > 0)
    {
        error.append("; ");
    }

    String MItype;
    if (miVector.genericMode)
    {
        MItype.append(generic);
    }
    else
    {
        MItype.append(realProviderName);
        MItype.append(spec);
    }

    if (optMsg && CMGetCharsPtr(optMsg,NULL))
    {
        MessageLoaderParms mlp(
            "ProviderManager.CMPI.CMPIProvider.MESSAGE_WAS",
            "$0, message was: $1",
            MItype,
            CMGetCharsPtr(optMsg,NULL));

        error.append(MessageLoader::getMessage(mlp));
    }
    else
    {
        error.append(MItype);
    }
    PEG_METHOD_EXIT();
}

void CMPIProvider::initialize(
    CIMOMHandle & cimom,
    ProviderVector & miVector,
    String & name,
    CMPI_Broker & broker)
{
    PEG_METHOD_ENTER(TRC_CMPIPROVIDERINTERFACE, "CMPIProvider::initialize()");
    broker.hdl=& cimom;
    broker.bft=CMPI_Broker_Ftab;
    broker.eft=CMPI_BrokerEnc_Ftab;
    broker.xft=CMPI_BrokerExt_Ftab;
    broker.mft=NULL;    // CMPI memory services not supported

    broker.clsCache.reset(new ClassCache());
    broker.name=name;

    const OperationContext opc;
    CMPI_ContextOnStack eCtx(opc);
    CMPI_ThreadContext thr(&broker,&eCtx);
    CMPIStatus rcInst = {CMPI_RC_OK, NULL};
    CMPIStatus rcAssoc = {CMPI_RC_OK, NULL};
    CMPIStatus rcMeth = {CMPI_RC_OK, NULL};
    CMPIStatus rcProp = {CMPI_RC_OK, NULL};
    CMPIStatus rcInd = {CMPI_RC_OK, NULL};
    String error;
    String realProviderName(name);

    if (miVector.genericMode)
    {
        CString mName=realProviderName.getCString();

        if (miVector.miTypes & CMPI_MIType_Instance)
        {
            miVector.instMI =
                miVector.createGenInstMI(&broker,&eCtx,mName, &rcInst);
        }
        if (miVector.miTypes & CMPI_MIType_Association)
        {
            miVector.assocMI = 
                miVector.createGenAssocMI(&broker,&eCtx,mName, &rcAssoc);
        }
        if (miVector.miTypes & CMPI_MIType_Method)
        {
            miVector.methMI = 
                miVector.createGenMethMI(&broker,&eCtx,mName, &rcMeth);
        }
        if (miVector.miTypes & CMPI_MIType_Property)
        {
            miVector.propMI = 
                miVector.createGenPropMI(&broker,&eCtx,mName, &rcProp);
        }
        if (miVector.miTypes & CMPI_MIType_Indication)
        {
            miVector.indMI = 
                miVector.createGenIndMI(&broker,&eCtx,mName, &rcInd);
        }
    }
    else
    {
        if (miVector.miTypes & CMPI_MIType_Instance)
            miVector.instMI=miVector.createInstMI(&broker,&eCtx, &rcInst);
        if (miVector.miTypes & CMPI_MIType_Association)
            miVector.assocMI=miVector.createAssocMI(&broker,&eCtx, &rcAssoc);
        if (miVector.miTypes & CMPI_MIType_Method)
            miVector.methMI=miVector.createMethMI(&broker,&eCtx, &rcMeth);
        if (miVector.miTypes & CMPI_MIType_Property)
            miVector.propMI=miVector.createPropMI(&broker,&eCtx, &rcProp);
        if (miVector.miTypes & CMPI_MIType_Indication)
            miVector.indMI=miVector.createIndMI(&broker,&eCtx, &rcInd);
    }

    if (miVector.miTypes & CMPI_MIType_Instance)
    {
        if (miVector.instMI == NULL || rcInst.rc != CMPI_RC_OK)
        {
            setError(miVector, error, realProviderName,
                _Generic_Create_InstanceMI, _Create_InstanceMI,
                rcInst.msg);
        }
    }
    if (miVector.miTypes & CMPI_MIType_Association)
    {
        if (miVector.assocMI == NULL || rcAssoc.rc != CMPI_RC_OK)
        {
            setError(miVector, error, realProviderName,
                _Generic_Create_AssociationMI, _Create_AssociationMI,
                rcAssoc.msg);
        }
    }
    if (miVector.miTypes & CMPI_MIType_Method)
    {
        if (miVector.methMI == NULL || rcMeth.rc != CMPI_RC_OK)
        {
            setError(miVector, error, realProviderName,
                _Generic_Create_MethodMI, _Create_MethodMI,
                rcMeth.msg);
        }
    }
    if (miVector.miTypes & CMPI_MIType_Property)
    {
        if (miVector.propMI == NULL || rcProp.rc != CMPI_RC_OK)
        {
            setError(miVector, error, realProviderName,
                _Generic_Create_PropertyMI, _Create_PropertyMI,
                rcProp.msg);
        }
    }
    if (miVector.miTypes & CMPI_MIType_Indication)
    {
        if (miVector.indMI == NULL || rcInd.rc != CMPI_RC_OK)
        {
            setError(miVector, error, realProviderName,
                _Generic_Create_IndicationMI, _Create_IndicationMI,
                rcInd.msg);
        }
    }

    if (error.size() != 0)
    {
        throw Exception(MessageLoaderParms(
            "ProviderManager.CMPI.CMPIProvider.CANNOT_INIT_API",
            "Error initializing CMPI MI $0, "
            "the following MI factory function(s) returned an error: $1",
            realProviderName,
            error));
    }
    PEG_METHOD_EXIT();
}

void CMPIProvider::initialize(CIMOMHandle & cimom)
{
    PEG_METHOD_ENTER(TRC_CMPIPROVIDERINTERFACE, "CMPIProvider::initialize()");
    String providername(getName());

    if (_status == UNINITIALIZED)
    {
        String compoundName;
        if (_location.size() == 0)
            compoundName= providername;
        else
            compoundName=_location+":"+providername;
        try
        {
            CMPIProvider::initialize(cimom,miVector,compoundName,broker);
            if (miVector.miTypes & CMPI_MIType_Method)
            {
                if (miVector.methMI->ft->miName==NULL) 
                {
                    unloadStatus = CMPI_RC_OK;
                }
            }
        }
        catch (...)
        {
            _current_operations = 0;
            throw;
        }
        _status = INITIALIZED;
        _current_operations = 0;
    }
    PEG_METHOD_EXIT();
}

Boolean CMPIProvider::tryTerminate()
{
    PEG_METHOD_ENTER(
        TRC_CMPIPROVIDERINTERFACE,
        "CMPIProvider::tryTerminate()");

    Boolean terminated = false;

    if (_status == INITIALIZED)
    {
        if (false == unload_ok())
        {
            PEG_METHOD_EXIT();
            return false;
        }

        Status savedStatus=_status;

        try
        {
            if (unloadStatus != CMPI_RC_OK)
            {
                // False means that the CIMServer is not shutting down.
                _terminate(false);
                if (unloadStatus != CMPI_RC_OK)
                {
                    _status=savedStatus;
                    PEG_METHOD_EXIT();
                    return false;
                }
                terminated=true;
            }
        }
        catch (...)
        {
            PEG_TRACE_STRING(
                TRC_PROVIDERMANAGER,
                Tracer::LEVEL2,
                "Exception caught in CMPIProviderFacade::tryTerminate() for " +
                getName());
            terminated = false;

        }
        if (terminated == true)
        {
            _status = UNINITIALIZED;
        }
    }
    PEG_METHOD_EXIT();
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
    PEG_METHOD_ENTER(TRC_CMPIPROVIDERINTERFACE, "CMPIProvider::_terminate()");
    const OperationContext opc;
    CMPIStatus rc={CMPI_RC_OK,NULL};
    CMPI_ContextOnStack eCtx(opc);
    CMPI_ThreadContext thr(&broker,&eCtx);
/*
 @param terminating When true, the terminating argument indicates that the MB
     is in the process of terminating and that cleanup must be done. When
     set to false, the MI may respond with
     CMPI_IRC_DO_NOT_UNLOAD, or CMPI_IRC_NEVER_UNLOAD,
     indicating that unload will interfere with current MI processing.
     @return Function return status. The following CMPIrc codes shall
     be recognized:
        CMPI_RC_OK Operation successful.
        CMPI_RC_ERR_FAILED Unspecific error occurred.
        CMPI_RC_DO_NOT_UNLOAD Operation successful - do not unload now.
        CMPI_RC_NEVER_UNLOAD Operation successful - never unload.
*/
    unloadStatus = CMPI_RC_OK;
    if (miVector.miTypes & CMPI_MIType_Instance)
    {
        rc=miVector.instMI->ft->cleanup(miVector.instMI,&eCtx, terminating);
        unloadStatus = rc.rc;
    }
    if (miVector.miTypes & CMPI_MIType_Association)
    {
        rc=miVector.assocMI->ft->cleanup(miVector.assocMI,&eCtx, terminating);
        if (unloadStatus == CMPI_RC_OK)
        {
            unloadStatus = rc.rc;
        }
    }
    if (miVector.miTypes & CMPI_MIType_Method)
    {
        rc=miVector.methMI->ft->cleanup(miVector.methMI,&eCtx, terminating);
        if (unloadStatus == CMPI_RC_OK)
        {
            unloadStatus = rc.rc;
        }
    }
    if (miVector.miTypes & CMPI_MIType_Property)
    {
        rc=miVector.propMI->ft->cleanup(miVector.propMI,&eCtx, terminating);
        if (unloadStatus == CMPI_RC_OK)
        {
            unloadStatus = rc.rc;
        }
    }
    if (miVector.miTypes & CMPI_MIType_Indication)
    {
        rc=miVector.indMI->ft->cleanup(miVector.indMI,&eCtx, terminating);
        if (unloadStatus == CMPI_RC_OK)
        {
            unloadStatus = rc.rc;
        }
    }

    if (unloadStatus == CMPI_RC_OK || terminating)
    {
        // Check the thread list to make sure the thread has been de-allocated
        if (_threadWatchList.size() != 0)
        {
            PEG_TRACE((TRC_PROVIDERMANAGER, Tracer::LEVEL2,
                "There are %d provider threads in %s that have to be cleaned "
                "up.",
                _threadWatchList.size(), (const char *)getName().getCString()));

            // Walk through the list and terminate the threads. After they are
            // terminated, put them back on the watch list, call the cleanup
            //  function and wait until the cleanup is completed.
            while (_threadWatchList.size() > 0)
            {
                // Remove the thread from the watch list and kill it.
                Thread *t = _threadWatchList.remove_front();

                /* If this a non-production build, DO NOT do the cancellation.
                   This is done so that the provider developer will notice 
                   incorrect behaviour when unloading his/her provider and 
                   hopefully fix that.
                */
#if !defined(PEGASUS_DEBUG)
# if defined(PEGASUS_PLATFORM_LINUX_GENERIC_GNU)
                Logger::put(Logger::STANDARD_LOG, System::CIMSERVER,
                    Logger::WARNING,
                    "Provider thread in $0 did not exit after cleanup function."
                    " Attempting to terminate it.",
                    (const char *)getName().getCString());
                t->cancel();
# else
                // Every other OS that we do not want to do cancellation for.
                Logger::put(Logger::STANDARD_LOG, System::CIMSERVER,
                    Logger::WARNING,
                    "Provider thread in $0 did not exit after cleanup"
                    " function. Ignoring it.",
                    (const char *)getName().getCString());
# endif
#else
                // For the non-release
                Logger::put(Logger::STANDARD_LOG, System::CIMSERVER,
                    Logger::WARNING,
                    "Provider thread in $0 did not exit after cleanup"
                    " function. Ignoring it.",
                    (const char *)getName().getCString());
                // The cancellation is disabled so that when the join happends
                // the CIMServer will hang. This should help the provider
                //   writer to fix his/her providers.
                //t->cancel();
#endif
                //  and perform the normal  cleanup procedure
                _threadWatchList.insert_back(t);
                removeThreadFromWatch(t);
            }
        }
        // threadWatchList size ZERO doesn't mean that all threads have
        //  been cleaned-up. While unloading communication libraries,
        //  Threads waiting for MB UP calls might have
        // just got removed from watchlist and not cleaned.

        // Wait until all of the threads have been cleaned.
        waitUntilThreadsDone();
    }
    PEG_METHOD_EXIT();
}


void CMPIProvider::terminate()
{
    PEG_METHOD_ENTER(
        TRC_CMPIPROVIDERINTERFACE,
        "CMPIProvider::terminate()");
    Status savedStatus=_status;
    if (_status == INITIALIZED)
    {
        try
        {

            _terminate(true);
            if (unloadStatus != CMPI_RC_OK)
            {
                _status=savedStatus;
                PEG_METHOD_EXIT();
                return;
            }
        }
        catch (...)
        {
            PEG_TRACE_STRING(
                TRC_PROVIDERMANAGER,
                Tracer::LEVEL2,
                "Exception caught in CMPIProviderFacade::Terminate for " +
                getName());
            throw;
        }
    }
    _status = UNINITIALIZED;
    PEG_METHOD_EXIT();
}

/*
 * Wait until all finished provider threads have been cleaned and deleted.
 * Note: This should NEVER be called from the thread that 
 * IS the Thread object that was is finished and called 
 * 'removeThreadFromWatch()' . If you do it, you will
 * wait forever.
 */
void CMPIProvider::waitUntilThreadsDone()
{
    while (_cleanedThreads.size() > 0)
    {
        Threads::yield();
    }
}
/*
 * Check if the Thread is owner by this CMPIProvider object.
 *
 * @argument t Thread that is not NULL.
 */
Boolean CMPIProvider::isThreadOwner(Thread *t)
{
    PEGASUS_ASSERT ( t != NULL );
    if (_cleanedThreads.contains(t))
    {
        return true;
    }
    if (!_threadWatchList.contains(t))
    {
        return true;
    }
    return false;
}
/*
 * Remove the thread from the list of threads that are being deleted
 * by the CMPILocalProviderManager.
 *
 * @argument t Thread which has been previously provided
 * to 'removeThreadFromWatch' function.
 */
void CMPIProvider::threadDelete(Thread *t)
{
    PEGASUS_ASSERT ( _cleanedThreads.contains(t) );
    PEGASUS_ASSERT ( !_threadWatchList.contains(t) );
    _cleanedThreads.remove( t );
}

/*
// Removes the thread from the watch list and schedule the
// CMPILocalProviderManager to delete the thread. The 
// CMPILocalProviderManager after deleting the thread calls
// the CMPIProvider' "cleanupThread". The CMPILocalProviderManager 
// notifies this CMPIProvider object when the thread
// is truly dead by calling "threadDeleted" function.
//
// Note that this function is called from the thread that finished with
// running the providers function, and returns immediately while scheduling
// the a cleanup procedure. If you want to wait until the thread is truly
//  deleted, call 'waitUntilThreadsDone' - but DO NOT do it in the the thread
// that the Thread owns - you will wait forever.
//
// @argument t Thread that is not NULL and finished with running
//  the provider function.
*/
void CMPIProvider::removeThreadFromWatch(Thread *t)
{
    PEG_METHOD_ENTER(
        TRC_CMPIPROVIDERINTERFACE,
        "CMPIProvider::removeThreadFromWatch()");
    PEGASUS_ASSERT( t != 0 );

    PEGASUS_ASSERT (_threadWatchList.contains (t));
    PEGASUS_ASSERT (!_cleanedThreads.contains (t));

    // and remove it from the watched list
    _threadWatchList.remove(t);

    // Add the thread to the CMPIProvider's list.
    // We use this list to keep track of threads that are
    // being cleaned (this way 'waitUntilThreadsDone' can stall until the
    // threads are truly deleted).
    _cleanedThreads.insert_back(t);

    CMPILocalProviderManager::cleanupThread(t, this);
    PEG_METHOD_EXIT();
}

/*
 * Adds the thread to the watch list. The watch list is monitored when the
 * provider is terminated and if any of the threads have not cleaned up by
 * that time, they are forcifully terminated and cleaned up.
 *
 * @argument t Thread is not NULL.
*/
void CMPIProvider::addThreadToWatch(Thread *t)
{
    PEGASUS_ASSERT( t != 0 );

    _threadWatchList.insert_back(t);
}

void CMPIProvider::get_idle_timer(struct timeval *t)
{
    PEGASUS_ASSERT(t != 0);
    AutoMutex lock(_idleTimeMutex);
    memcpy(t, &_idleTime, sizeof(struct timeval));
}

void CMPIProvider::update_idle_timer()
{
    AutoMutex lock(_idleTimeMutex);
    Time::gettimeofday(&_idleTime);
}

/*
 * This method returns "false" if there are any requests pending with
 * the provider or Provider has returned CMPI_RC_NEVER_UNLOAD in the last
 * cleanup() invocation cyle.
*/
Boolean CMPIProvider::unload_ok()
{
    PEG_METHOD_ENTER(TRC_CMPIPROVIDERINTERFACE, "CMPIProvider::unload_ok()");
    if (unloadStatus == CMPI_RC_NEVER_UNLOAD)
    {
        PEG_METHOD_EXIT();
        return false;
    }
    if (_no_unload.get())
    {
        PEG_METHOD_EXIT();
        return false;
    }
    // Do not call CIMOMHandle::unload_ok here. 
    // CIMOMHandle::unload_ok method tests for _providerUnloadProtect
    // and if zero returns true. _providerUnloadProtect is
    // incremented when CIMOMHandle::disallowProviderUnload()
    // is called and decremented when
    // CIMOMHandle::allowProviderUnload() is called. There is
    // no way these functions are called from CMPI Providers.(Bug 6642)
    PEG_METHOD_EXIT();
    return true;
}

//   force provider manager to keep in memory
void CMPIProvider::protect()
{
    _no_unload++;
}

// allow provider manager to unload when idle
void CMPIProvider::unprotect()
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
    
