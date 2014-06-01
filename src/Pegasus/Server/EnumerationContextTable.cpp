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

PEGASUS_NAMESPACE_BEGIN

PEGASUS_USING_STD;

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
        if (!et->removeExpiredContexts())
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
EnumerationContextTable::EnumerationContextTable(
    Uint32 maxOpenContextsLimit,
    Uint32 defaultInteroperationTimeoutValue,
    Uint32 reponseCacheMaximumSize)
    :
    _timeoutIntervalMsec(0),
    _enumContextTable(maxOpenContextsLimit / 2),
    _operationContextTimeoutThread(operationContextTimeoutThread, this, true),
    _responseCacheMaximumSize(reponseCacheMaximumSize),
    _cacheHighWaterMark(0),
    _responseObjectCountHighWaterMark(0),
    _defaultOperationTimeoutSec(defaultInteroperationTimeoutValue),
    _enumerationContextsOpened(0),
    _enumerationsTimedOut(0),
    _maxOpenContexts(0),
    _maxOpenContextsLimit(maxOpenContextsLimit),
    _requestedSize(0),
    _requestCount(0)
{}


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

static IDFactory _enumerationContextIDFactory(6000);

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

    AutoMutex autoMut(_tableLock);
    if (_enumerationContextsOpened != 0)
    {
        cout << "EnumerationTable Statistics:"
            << "\n  EnumerationCache high water mark: " << _cacheHighWaterMark
            << "\n  Max simultaneous enumerations: " << _maxOpenContexts
            << "\n  Total enumerations opened: " << _enumerationContextsOpened
            << "\n  Enumerations aborted: " << _enumerationsTimedOut
            << "\n  Current open enumerations: " << size()
            << "\n  Average request size: " << _getAvgRequestSize()
            << "\n  Response size max: " << _responseObjectCountHighWaterMark
            << "\n  Simultaneous Open Contexts limit " << _maxOpenContextsLimit
            // KS_TODO avg requests per enum. Should do by en, not total
            << endl;
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
    }
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
    tableValidate();                        // KS_TEMP Diagnostic

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
        if (en->_cacheHighWaterMark > _cacheHighWaterMark)
        {
            _cacheHighWaterMark = en->_cacheHighWaterMark;
        }

        if (en->_responseObjectsCount > _responseObjectCountHighWaterMark)
        {
            _responseObjectCountHighWaterMark = en->_responseObjectsCount;
        }

        _enumContextTable.remove(en->getContextId());

        // Delete the enumerationContext object
        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,  // KS_TEMP
            "Delete Context. ContextId=%s",
            (const char *)en->getContextId().getCString() ));

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
bool EnumerationContextTable::removeExpiredContexts()
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "EnumerationContextTable::removeExpiredContexts");

    PEGASUS_DEBUG_ASSERT(valid());

    // Lock the EnumerationContextTable so no operations can be accepted
    // during this process
    AutoMutex autoMut(_tableLock);

    if (size() == 0)
    {
        return false;
    }

    Array<String> removeList;

    Uint64 currentTimeUsec = System::getCurrentTimeUsec();

    // Search enumeration table for entries timed out
    for (EnumContextTableType::Iterator i = _enumContextTable.start(); i; i++)
    {
        EnumerationContext* en = i.value();
        if (en->valid())
        {
            PEGASUS_DEBUG_ASSERT(en->valid());     // diagnostic. KS_TEMP

            if (en->_interOperationTimerUsec != 0)
            {
                // Only set lock if there is a chance the timer is active.
                // redoes the above test after setting lock. Bypass if
                // locked
                if (en->tryLockContext())
                {
                    // test if entry is active (timer not zero)
                    if (en->isTimedOut(currentTimeUsec))
                    {
                        en->stopTimer();
                        en->setClientClosed();
                        _enumerationsTimedOut++;

                        // If providers are complete we can remove the context
                        // Otherwise depend on provider completion to
                        // clean up the enumeration
                        if (en->providersComplete())
                        {
                            removeList.append(en->getContextId());
                        }
                        else
                        {
                            // depend on provider completion to close context.
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

    // Release all EnumerationContexts in list
    for (Uint32 i = 0; i < removeList.size(); i++)
    {
        // unlock before removing the context
        EnumerationContext* en = find(removeList[i]);
        PEGASUS_DEBUG_ASSERT(en->valid());

        en->unlockContext();
        _removeContext(en);
    }

    PEG_METHOD_EXIT();

    return (size() == 0)? true : false;
}

// Validate every entry in the table.This is a diagnostic that should only
// be used in testing changes during development.
void EnumerationContextTable::tableValidate()
{
    AutoMutex autoMut(_tableLock);
    for (EnumContextTableType::Iterator i = _enumContextTable.start(); i; i++)
    {
        EnumerationContext* en = i.value();
        if (!en->valid())
        {
            en->trace();
            PEGASUS_ASSERT(en->valid());
        }
    }
}

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
        // loop through the table every 30 seconds.

        _timeoutIntervalMsec = 30*1000;

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
            Threads::sleep(50);
        }
    }
    PEG_TRACE_CSTRING(TRC_DISPATCHER, Tracer::LEVEL4,
        "EnumerationContextTable timeout thread stopped");
    PEG_METHOD_EXIT();
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
