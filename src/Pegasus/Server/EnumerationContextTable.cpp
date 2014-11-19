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
//%////////////////////////////////////////////////////////////////////////////

#include "EnumerationContextTable.h"

#include <Pegasus/Common/Mutex.h>
#include <Pegasus/Common/Time.h>
#include <Pegasus/Common/List.h>
#include <Pegasus/General/Guid.h>
#include <Pegasus/Common/CIMResponseData.h>
#include <Pegasus/Common/Condition.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/General/Stopwatch.h>
#include <Pegasus/Common/Thread.h>
#include <Pegasus/Server/EnumerationContext.h>
#include <Pegasus/Common/StringConversion.h>
#include <Pegasus/Common/IDFactory.h>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Config/ConfigManager.h>
// Used only for the single static function call to issueSavedResponses
#include <Pegasus/Server/CIMOperationRequestDispatcher.h>

#include <Pegasus/Server/CIMServer.h>
#include <Pegasus/ProviderManagerService/ProviderManagerService.h>

PEGASUS_USING_STD;
PEGASUS_NAMESPACE_BEGIN

// Definition of static variable that can be set by
// config manager
Uint32 EnumerationContextTable::_defaultOperationTimeoutSec =
    PEGASUS_DEFAULT_PULL_OPERATION_TIMEOUT_SEC;

#ifndef PEGASUS_INTEGERS_BOUNDARY_ALIGNED
Mutex EnumerationContextTable::_defaultOperationTimeoutSecMutex;
#endif

// Define the table instance that will contain enumeration contexts for Open,
// Pull, Close, and countEnumeration operations.  The default interoperation
// timeout and max cache size are set as part of creating the table.
//
// TODO FUTURE: There are several system parameters here that should be
// set globally.  We need somewhere common to be able to define this and
// while it could be the config, it could also be a systemLimits file or
// something common.
#define PEGASUS_MAX_OPEN_ENUMERATE_CONTEXTS 256

    // Define the maximum size for the response cache in each
    // enumerationContext.  As responses are returned from providers this is the
    // maximum number that can be placed in the CIMResponseData cache waiting
    // for pull operations to send them as responses before responses
    // start backing up to the providers (i.e. delaying return from the provider
    // deliver calls.
    // FUTURE: As we develop more flexible resource management this value should
    // be modified for each context creation in terms of the object sizes
    // expected and the memory usage of the CIMServer.  Thus, it would be
    // logical to allow caching many more path responses than instance
    // responses because they are probably much smaller.
    // This variable is not externalized because we are not sure
    // if that is logical.
#define PEGASUS_PULL_RESPONSE_CACHE_DEFAULT_MAX_SIZE 1000

Uint32 responseCacheDefaultMaximumSize =
        PEGASUS_PULL_RESPONSE_CACHE_DEFAULT_MAX_SIZE;

EnumerationContextTable* EnumerationContextTable::pInstance = NULL;

// Thread execution function for timeoutThread(). This thread executes
// regular timeout tests on active contexts and closes them or marks them
// for close if timed out.  This is required for those cases where a
// pull sequence is terminated without either completing or closing the
// sequence.

ThreadReturnType PEGASUS_THREAD_CDECL operationContextTimeoutThread(void* parm)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "EnumerationContextTable::operationContextTimerThread");

    Thread *myself = reinterpret_cast<Thread *>(parm);
    EnumerationContextTable* et =
        reinterpret_cast<EnumerationContextTable*>(myself->get_parm());

    PEGASUS_DEBUG_ASSERT(et->valid());

    Uint32 nextTimeoutMsec = et->getTimeoutIntervalMsec();

    // execute loop at regular intervals until no more contexts or
    // stop flag set
    while (!et->_stopTimeoutThreadFlag.get() )
    {
        et->_timeoutThreadWaitSemaphore.time_wait(nextTimeoutMsec);

        // false return indicates table empty.
        if (!et->processExpiredContexts())
        {
            break;
        }
    }

    // reset the timeout value to indicate that the thread is quiting
    et->_timeoutThreadRunningFlag = 0;

    PEG_METHOD_EXIT();
    return ThreadReturnType(0);
}

/************************************************************************
**
**  Implementation of EnumerationContextTable Class
**
************************************************************************/
/*
    Constructor.  Note that the hashtable size is 1/2 the maximum number
    of simultaneous open contexts.  This was a guess based on notes in
    the Hashtable code indicating that 1/3 might be logical choice.
*/
EnumerationContextTable::EnumerationContextTable()
    :
    _timeoutIntervalMsec(0),
    // Defines the Context Hash table size as 1/2 the max number of entries
    _enumContextTable(PEGASUS_MAX_OPEN_ENUMERATE_CONTEXTS / 2),
    _operationContextTimeoutThread(operationContextTimeoutThread, this, true),
    _responseCacheMaximumSize(PEGASUS_PULL_RESPONSE_CACHE_DEFAULT_MAX_SIZE),
    _cacheHighWaterMark(0),
    _responseObjectCountHighWaterMark(0),
    _enumerationContextsOpened(0),
    _enumerationsTimedOut(0),
    _maxOpenContexts(0),
    _maxOpenContextsLimit(PEGASUS_MAX_OPEN_ENUMERATE_CONTEXTS),
    _requestedSize(0),
    _requestCount(0),
    _totalZeroLenDelayedResponses(0)
{
    // Setup the default value for the operation timeout value if the value
    // received  in a request is NULL.  This is the server defined default.
    ConfigManager* configManager = ConfigManager::getInstance();

    _defaultOperationTimeoutSec = ConfigManager::parseUint32Value(
    configManager->getCurrentValue("pullOperationsDefaultTimeout"));
}

// Create the singleton instance of the enumerationContextTable and return
// pointer to that instance. If created, return pointer to existing singleton
// instance
EnumerationContextTable* EnumerationContextTable::getInstance()
{
    if (!pInstance)
    {
        pInstance = new EnumerationContextTable();
    }
    return pInstance;
}


/* Remove all contexts and delete them. Only used on system shutdown.
*/
EnumerationContextTable::~EnumerationContextTable()
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "EnumerationContextTable::~EnumerationContextTable");

    _stopTimeoutThread();

    // Show shutdown statistics for EnumerationContextTable
    displayStatistics();

    removeContextTable();

    PEG_METHOD_EXIT();
}

/*
    Create a new context entry and return it. This includes information
    required to process the pull and close operations for the enumeration
    sequence controlled by this context. The context instance will remain
    active for the life of the enumeration sequence.
    Returns pointer to the new context except if:
       - Size exceeds system limit.
*/

static IDFactory _enumerationContextIDFactory(500000);

EnumerationContext* EnumerationContextTable::createContext(
    const CIMOpenOperationRequestMessage* request,
    MessageType pullRequestType,
    CIMResponseData::ResponseDataContent contentType)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,"EnumerationContextTable::createContext");

    AutoMutex autoMut(_tableLock);

    // Test for Max Number of simultaneous contexts.
    if (_enumContextTable.size() > _maxOpenContextsLimit)
    {
        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL1,
            "Error EnumerationContext Table exceeded Max limit of %u",
             _responseCacheMaximumSize));
        return NULL;
    }
    // set the operation timeout to either the default or current
    // value
    Uint32 operationTimeout = (request->operationTimeout.isNull())?
        _defaultOperationTimeoutSec
        :
        request->operationTimeout.getValue();

    // Create new context name
    Uint32 rtnSize;
    char scratchBuffer[22];
    const char* contextId = Uint32ToString(scratchBuffer,
    _enumerationContextIDFactory.getID(), rtnSize);

    // Create new context, Context name is monolithically increasing counter.
    EnumerationContext* en = new EnumerationContext(contextId,
        request->nameSpace,
        operationTimeout,
        request->continueOnError,
        pullRequestType,
        contentType);

    // Set the maximum size for the response Cache from the default
    // value in the table. This is for future where we could adjust the
    // size dynamically for each operation depending on resource utilization.
    // or expected response sizes (ex. paths vs instances)
    en->_responseCacheMaximumSize = _responseCacheMaximumSize;

    // Pointer back to this object
    en->_enumerationContextTable = this;

    // insert new context into the table. Failure to insert is a
    // system failure
    if(!_enumContextTable.insert(contextId, en))
    {
        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL1,
            "Error Creating Enumeration Context %s. System Failed",
            contextId ));
        PEGASUS_ASSERT(false);  // This is a system failure
    }

    _enumerationContextsOpened++;

    // set new highwater mark for max contexts if necessary
    if (_enumContextTable.size() >_maxOpenContexts )
    {
        _maxOpenContexts = _enumContextTable.size();
    }

    PEG_METHOD_EXIT();
    return en;
}

void EnumerationContextTable::displayStatistics(bool clearStats)
{
    // Show shutdown statistics for EnumerationContextTable
    // Should add avg size of requests.  Maybe some other info.
        cout << buildStatistics(clearStats) << endl;
}

// build a string with the statistics info.
String EnumerationContextTable::buildStatistics(bool clearStats)
{
    String str;

    AutoMutex autoMut(_tableLock);
    if (_enumerationContextsOpened != 0)
    {
        str.appendPrintf("EnumerationTable Statistics:"
            "\n  EnumerationCache high water mark=%u"
            "\n  Max simultaneous enumerations=%u"
            "\n  Total enumerations opened=%llu",
            _cacheHighWaterMark,
            _maxOpenContexts,
            _enumerationContextsOpened);

        str.appendPrintf(
            "\n  Enumerations timed out=%u"
            "\n  Current open enumerations=%u"
            "\n  Average request size=%u"
            "\n  Response size max=%u",
            _enumerationsTimedOut,
            size(),
            _getAvgRequestSize(),
            _responseObjectCountHighWaterMark);

        str.appendPrintf(
            "\n  Total zero Length delayed responses=%u",
            _totalZeroLenDelayedResponses);
    }
    if (clearStats)
    {
        _cacheHighWaterMark = 0;
        _maxOpenContexts = 0;
        _enumerationContextsOpened = 0;
        _enumerationsTimedOut = 0;
        _requestCount = 0;
        _requestedSize = 0;
        _responseObjectCountHighWaterMark = 0;
        _totalZeroLenDelayedResponses = 0;
    }
    return str;
}

void EnumerationContextTable::removeContextTable()
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "EnumerationContextTable::removeContextTable");

    AutoMutex autoMut(_tableLock);
    // Clear out any existing enumerations.
    for (EnumContextTableType::Iterator i = _enumContextTable.start(); i; i++)
    {
        EnumerationContext* en = i.value();

        PEG_TRACE(( TRC_DISPATCHER, Tracer::LEVEL4,
            "EnumerationTable Close. Entry Found "
                " name %s, started %llu milliseconds before,",
             (const char *)en->getContextId().getCString(),
             ((TimeValue::getCurrentTime().toMilliseconds()
               - en->_startTimeUsec)/1000) ));
        delete en;
    }
    _enumContextTable.clear();
    PEG_METHOD_EXIT();
}

bool EnumerationContextTable::releaseContext(EnumerationContext* en)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,"EnumerationContextTable::releaseContext");

    AutoMutex autoMut(_tableLock);

    PEGASUS_DEBUG_ASSERT(valid());
    PEGASUS_DEBUG_ASSERT(en->valid());

    String contextId = en->getContextId();

    en->unlockContext();

    // Check to assure that the context ID is in the table.
    if (!find(contextId))
    {
        PEG_METHOD_EXIT();
        return false;
    }
    _removeContext(en);

    PEG_METHOD_EXIT();
    return true;
}

// Private remove function with no lock protection. The _tableLock must
// be set before this function is called to protect the table. This simply
// removes the context from the context table.
bool EnumerationContextTable::_removeContext(EnumerationContext* en)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,"EnumerationContextTable::_removeContext");

    PEGASUS_DEBUG_ASSERT(en->valid());

    // If it is valid and providers are complete, remove
    // the enumerationContext.  If providers not complete, only
    // completion of provider deliveries can initiate removal of
    // the enumeration context.
    // This function assumes that the sequence is really complete and
    // provider returns complete.  If that is not true, it just generates
    // an error and returns.
    // Any functions that set _ClientClosed and _ProvidersComplete
    // must insure that they block until finished with context.
    if (en->_clientClosed && en->_providersComplete)
    {
        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,  // KS_TEMP
            "EnumerationContext Remove. ContextId=%s",
            (const char *)en->getContextId().getCString() ));

        // test/set the highwater mark for the table
        // KS_TODO confirm that statistics below always get accumulated

        if (en->_cacheHighWaterMark > _cacheHighWaterMark)
        {
            _cacheHighWaterMark = en->_cacheHighWaterMark;
        }

        if (en->_responseObjectsCount > _responseObjectCountHighWaterMark)
        {
            _responseObjectCountHighWaterMark = en->_responseObjectsCount;
        }
        _totalZeroLenDelayedResponses+= en->_totalZeroLenObjectResponseCounter;

        _enumContextTable.remove(en->getContextId());

        delete en;

        PEG_METHOD_EXIT();
        return true;
    }

    PEG_METHOD_EXIT();
    return false;
}

Uint32 EnumerationContextTable::size()
{
    AutoMutex autoMut(_tableLock);
    return(_enumContextTable.size());
}

// If context name found, return pointer to that context.  Otherwise
// return 0
EnumerationContext* EnumerationContextTable::find(const String& contextId)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER, "EnumerationContextTable::find");

    AutoMutex autoMut(_tableLock);

    EnumerationContext* en = 0;

    if(_enumContextTable.lookup(contextId, en))
    {
        PEGASUS_DEBUG_ASSERT(en->valid());
    }
    // Return pointer or pointer = 0 if not found.
    PEG_METHOD_EXIT();
    return en;
}

/** Test all table entries and remove the ones timed out.

*/
bool EnumerationContextTable::processExpiredContexts()
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "EnumerationContextTable::processExpiredContexts");

    PEGASUS_DEBUG_ASSERT(valid());

    // Lock the EnumerationContextTable so no operations can be accepted
    // during this process

    if (size() == 0)
    {
        return false;
    }

    Array<String> removeList;
    Array<String> issueSavedResponseList;

    Uint64 currentTimeUsec = System::getCurrentTimeUsec();
    {
        AutoMutex autoMut(_tableLock);

        // Search enumeration table for entries timed out. Sets any entries
        // that have timed out into a secondary list for processing
        for (EnumContextTableType::Iterator i = _enumContextTable.start();i;i++)
        {
            EnumerationContext* en = i.value();
            if (en->valid())
            {
                PEGASUS_DEBUG_ASSERT(en->valid());     // diagnostic. KS_TEMP

                if (en->_operationTimerUsec != 0)
                {
                    // Only set lock if there is a chance the timer is active.
                    // redoes the above test after setting lock. Bypass this
                    // enumerationContext if locked
                    if (en->tryLockContext())
                    {
                        // test if entry is active (timer not zero)
                        if (en->isTimedOut(currentTimeUsec))
                        {
                            en->stopTimer();
                            _enumerationsTimedOut++;

                            // If providers are complete we can remove the
                            // context. Otherwise depend on provider completion
                            // to clean up the enumeration
                            if (en->providersComplete())
                            {
                                removeList.append(en->getContextId());
                            }
                            else if (en->isProcessing())
                            {
                                issueSavedResponseList.append(
                                    en->getContextId());
                            }
                            else
                            {
                                // depend on provider completion to close
                                // context.
                                en->unlockContext();
                            }
                        }
                        else
                        {
                            en->unlockContext();
                        }
                    }
                }
            }
        }

        // remove any contexts in the remove list
        for (Uint32 i = 0; i < removeList.size(); i++)
        {
            // unlock before removing the context
            EnumerationContext* en = find(removeList[i]);
            PEGASUS_DEBUG_ASSERT(en->valid());
            en->setClientClosed();
            en->unlockContext();
            _removeContext(en);
        }
    }   // release the table lock

    // For any contexts that are in the savedOperationList,
    // issue the response and test for consecutive retries that pass
    // the defined limits. The context is locked at this point
    // but the table lock has been removed
    // Consecutive resends of Zero Len response >
    //  PEGASUS_MAX_CONSECUTIVE_WAITS_BEFORE_ERR will results in setting
    // the providers closed and issuing message.
    // After 4 of these messages sent, just close the enumeration.
    for (Uint32 i = 0; i < issueSavedResponseList.size(); i++)
    {
        // recheck by finding, validating and checking state again
        //
        EnumerationContext* en = find(issueSavedResponseList[i]);
        PEGASUS_DEBUG_ASSERT(en->valid());
        PEGASUS_DEBUG_ASSERT(en->isProcessing());

        // Increment the counter for sending zero length responses.
        // This is used to determine when providers stuck incomplete
        Uint32 ctr = en->incConsecutiveZeroLenObjectResponseCounter();

        Uint32 targetCount = PEGASUS_MAX_CONSECUTIVE_WAITS_BEFORE_ERR;
        Uint32 finalTargetCount = targetCount + 3;
        // If past final target count, set providers complete so we issue
        // next msg with EOS set. That should close out the client.
        if (ctr >= finalTargetCount)
        {
            en->setProvidersComplete();
            // Set an error. NOTE: Just guessing if this is the correct error
            CIMException e = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
                             "Provider Responses Failed");
            en->setErrorState(e);
        }
        // Issue an empty response to the dispatcher.
        // Set the count to zero so can issue with no responses in
        // cache.
        en->_savedOperationMaxObjectCount = 0;
        CIMOperationRequestDispatcher::issueSavedResponse(en);

        // If we have tried the clean up several times and it did not work, just
        // discard the enumeration context. Just picked the number 4
        // as the number of retries for the cleanup before we just close the
        // whole thing.
        if (ctr >= finalTargetCount)
        {
            PEGASUS_DEBUG_ASSERT(en->valid());
            PEG_TRACE((TRC_DISCARDED_DATA, Tracer::LEVEL1,
                  "Enumeration Context %s removed after providers did not"
                  "respond for at least %u min",
                  (const char*)en->getContextId().getCString(),
                  ((finalTargetCount * PEGASUS_PULL_MAX_OPERATION_WAIT_SEC)/60)
                  ));

            // Close the provider side, generate response to client, and
            // remove the context.
            // KS_TODO Do not think I need this final setClientClosed
            en->setClientClosed();
            en->unlockContext();
            _removeContext(en);
        }
        // The initial test and cleanup.  Started at targetCount and
        // repeated at least 3 times before we discard the provider
        else if (ctr >= targetCount)
        {
            PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
                "%u Consecutive 0 len responses issued for ContextId=%s",
                ctr,
                CSTRING(en->getContextId()) ));
            // send cleanup message to provider manager.  Need pointer to
            // this service.
            CIMServer * cimserver = CIMServer::getInstance();
            // What do we do if false return
            cimserver->_providerManager->enumerationContextCleanup(
                en->getContextId());
            en->unlockContext();
        }
        else
        {
            en->unlockContext();
        }

    }

    PEG_METHOD_EXIT();

    return (size() == 0)? true : false;
}

////// Validate every entry in the table.This is a diagnostic that should only
////// be used in testing changes during development.
////void EnumerationContextTable::tableValidate()
////{
////    AutoMutex autoMut(_tableLock);
////    for (EnumContextTableType::Iterator i = _enumContextTable.start(); i;
////        i++)
////    {
////        EnumerationContext* en = i.value();
////        if (!en->valid())
////        {
////            en->trace();
////            PEGASUS_ASSERT(en->valid());
////        }
////    }
////}

// interval is the timeout for the current operation that
// initiated this call.  It helps the timer thread decide how often
// to scan for timeouts.
void EnumerationContextTable::dispatchTimerThread(Uint32 intervalSec)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
                     "EnumerationContextTable::dispatchTimerThread");

    PEGASUS_DEBUG_ASSERT(valid());

    AutoMutex autoMut(_tableLock);

    // If timeout thread not running, start it
    if (_timeoutThreadRunningFlag.get() == 0)
    {
        // convert second timer to milliseconds and set it for double
        // the input defined interval. Set for the timer thread to
        // loop through the table every PEGASUS_PULL_MAX_OPERATION_WAIT_SEC
        // seconds.

        _timeoutIntervalMsec = PEGASUS_PULL_MAX_OPERATION_WAIT_SEC*1000;

        // Start the detached thread to execute the timeout tests.
        // Thread runs until the timer is cleared or there are
        // no more contexts.
        if (_operationContextTimeoutThread.run() != PEGASUS_THREAD_OK)
        {
            MessageLoaderParms parms(
                "Server.EnumerationContextTable.THREAD_ERROR",
                "Failed to start pull operation timer thread.");

            Logger::put_l(
                Logger::ERROR_LOG, System::CIMSERVER, Logger::SEVERE,
                parms);
            PEGASUS_DEBUG_ASSERT(false); // This is system failure
        }

        _timeoutThreadRunningFlag++;
    }
    PEG_METHOD_EXIT();
}

void EnumerationContextTable::_stopTimeoutThread()
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "EnumerationContextTable::_stopTimeoutThread");

    PEGASUS_DEBUG_ASSERT(valid());

    if (_timeoutThreadRunningFlag.get() != 0)
    {
        _stopTimeoutThreadFlag++;
        _timeoutThreadWaitSemaphore.signal();

        while (_timeoutThreadRunningFlag.get())
        {
            Threads::yield();
            Threads::sleep(PEGASUS_PULL_MAX_OPERATION_WAIT_SEC);
        }
    }
    PEG_TRACE_CSTRING(TRC_DISPATCHER, Tracer::LEVEL4,
        "EnumerationContextTable timeout thread stopped");
    PEG_METHOD_EXIT();
}


void EnumerationContextTable::setDefaultOperationTimeoutSec(Uint32 seconds)
{
#ifndef PEGASUS_INTEGERS_BOUNDARY_ALIGNED
    AutoMutex lock(_defaultOperationTimeoutSecMutex);
#endif
    _defaultOperationTimeoutSec = seconds;
}


void EnumerationContextTable::trace()
{
    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
        "EnumerationContextTable Trace. size=%u", _enumContextTable.size()));

    AutoMutex autoMut(_tableLock);

    for (EnumContextTableType::Iterator i = _enumContextTable.start(); i; i++)
    {
        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
            "ContextTable Entry: key [%s]",
                   (const char*)i.key().getCString() ));
        EnumerationContext* enumeration = i.value();
        enumeration->trace();
    }
}

PEGASUS_NAMESPACE_END
