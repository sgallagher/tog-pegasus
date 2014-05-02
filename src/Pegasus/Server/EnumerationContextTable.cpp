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

// Enumeration Context objects are maintained in the following
// Pegasus hash table.
typedef HashTable<String, EnumerationContext* , EqualFunc<String>,
    HashFunc<String> > EnumContextTable;

static EnumContextTable enumContextTable(128);


// Thread execution function for timerThread(). This thread executes
// regular timeout tests on active contexts and closes them or marks them
// for close if timed out.  This is required for those cases where a
// pull sequence is terminated without either completing or closing the
// sequence.

ThreadReturnType PEGASUS_THREAD_CDECL operationContextTimerThread(void* parm)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "EnumerationContextTable::operationContextTimerThread");

    Thread *myself = reinterpret_cast<Thread *>(parm);
    EnumerationContextTable* et =
        reinterpret_cast<EnumerationContextTable*>(myself->get_parm());

    PEGASUS_DEBUG_ASSERT(et->valid());

    // execute loop at regular intervals until there are no more contexts
    // KS_TODO - The choice of existence of contexts as the loop controller
    // means that we will look through many inactive contexts to find any
    // that are timed out (much search for a very unusual condition).
    // This algorithm not optimum. Problem is that we are starting and
    // stopping thread timers many times a second so list does not make sense.
    while ((et->size() != 0) &&
        (!et->stopThread()))
    {
        // Sleep loop until time for next scan or thread quit requested
        while (!et->isNextScanTime() &&
               (!et->stopThread()))
        {
            Threads::sleep(et->timoutInterval());
        }

        et->removeExpiredContexts();
        et->updateNextTimeout();
    }

    // reset the timeout value to indicate that the thread is quiting
    et->setTimerThreadIdle();

    PEG_METHOD_EXIT();
    return ThreadReturnType(0);
}

/************************************************************************
**
**  Implementation of EnumerationContextTable Class
**
************************************************************************/

EnumerationContextTable::EnumerationContextTable(Uint32 maxOpenContextsLimit)
    :
    _timeoutInterval(0),
    _nextTimeout(0),
    _operationContextTimerThread(operationContextTimerThread, this, true),
    _responseCacheMaximumSize(0),
    _cacheHighWaterMark(0),
    _defaultOperationTimeout(0),
    _enumerationContextsOpened(0),
    _enumerationsTimedOut(0),
    _maxOpenContexts(0),
    _maxOpenContextsLimit(maxOpenContextsLimit)
{}

/*  Create the Enumeration table and set the values for
    InteroperationTimeOut and maximum size of the
    response Cache where objects are gathered from the
    providers to be distributed as pull client operations
    are processed.
*/
void EnumerationContextTable::setContextDefaultParameters(
    Uint32 defaultInteroperationTimeoutValue,
    Uint32 reponseCacheMaximumSize)
{
    _responseCacheMaximumSize = reponseCacheMaximumSize;
    _defaultOperationTimeout = defaultInteroperationTimeoutValue;
}

/* Remove all contexts and delete them. Only used on system shutdown.
*/
EnumerationContextTable::~EnumerationContextTable()
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "EnumerationContextTable::~EnumerationContextTable");

    setTimerThreadIdle();
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
    Uint32Arg&  operationTimeoutParam,
    Boolean continueOnError,
    MessageType pullRequestType,
    CIMResponseData::ResponseDataContent contentType)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,"EnumerationContextTable::createContext");

    AutoMutex autoMut(tableLock);

    // Test for Max Number of simultaneous contexts.
    if (enumContextTable.size() > _maxOpenContextsLimit)
    {
        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL1,
            "Error EnumerationContext Table exceeded Max limit of %u",
             _responseCacheMaximumSize));
        return NULL;
    }
    // set the operation timeout to either the default or current
    // value
    Uint32 operationTimeout = (operationTimeoutParam.isNull())?
        _defaultOperationTimeout
        :
        operationTimeoutParam.getValue();

    // Create new context name
    Uint32 rtnSize;
    char scratchBuffer[22];
    const char* contextId = Uint32ToString(scratchBuffer,
    _enumerationContextIDFactory.getID(), rtnSize);

    // Create new context, Context name is monolithically increasing counter.
    EnumerationContext* en = new EnumerationContext(contextId,
        operationTimeout,
        continueOnError,
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
    if(!enumContextTable.insert(contextId, en))
    {
        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL1,
            "Error Creating Enumeration Context %s. System Failed",
            contextId ));
        PEGASUS_ASSERT(false);  // This is a system failure
    }

    // KS_TODO DELETE. Delete this trace before checkin.  There is
    // a better trace on the enumerationContext constructor.
    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
        "CreateContext created ContextId=%s", contextId));

    _enumerationContextsOpened++;

    // set new highwater mark for max contexts if necessary
    if (enumContextTable.size() >_maxOpenContexts )
    {
        _maxOpenContexts = enumContextTable.size();
    }

    PEG_METHOD_EXIT();
    return en;
}

void EnumerationContextTable::displayStatistics(Boolean clearStats)
{
    // Show shutdown statistics for EnumerationContextTable
    // Should add avg size of requests.  Maybe some other info.

    AutoMutex autoMut(tableLock);
    if (_enumerationContextsOpened != 0)
    {
        cout << "EnumerationTable Statistics:"
            << "\n  EnumerationCache High Water Mark: " << _cacheHighWaterMark
            << "\n  Max Simultaneous Enumerations: " << _maxOpenContexts
            << "\n  Total Enumerations Opened: " << _enumerationContextsOpened
            << "\n  Enumerations Aborted: " << _enumerationsTimedOut
            << "\n  Current Open Enumerations: " << size()
            << "\n  Average Request Size: " << _getAvgRequestSize()
            << "\n  max Open Contexts limit " << _maxOpenContextsLimit
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
    }
}

void EnumerationContextTable::removeContextTable()
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "EnumerationContextTable::removeContextTable");

    // Show shutdown statistics for EnumerationContextTable
    displayStatistics();

    AutoMutex autoMut(tableLock);
    // Clear out any existing enumerations.
    if (enumContextTable.size() != 0)
    {
        for (EnumContextTable::Iterator i = enumContextTable.start(); i; i++)
        {
            EnumerationContext* enumeration = i.value();

            PEG_TRACE(( TRC_DISPATCHER, Tracer::LEVEL4,
                "EnumerationTable Close. Entry Found "
                    " name %s, started %llu milliseconds before,",
                 (const char *)enumeration->getContextId().getCString(),
                 ((TimeValue::getCurrentTime().toMilliseconds()
                   - enumeration->_startTime)/1000) ));

            enumContextTable.remove(i.key());
            delete enumeration;
        }
    }
    PEG_METHOD_EXIT();
}

void EnumerationContextTable::releaseContext(EnumerationContext* en)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,"EnumerationContextTable::releaseContext");

    AutoMutex autoMut(tableLock);

    PEGASUS_DEBUG_ASSERT(valid());
    PEGASUS_DEBUG_ASSERT(en->valid());

    _removeContext(en);

    PEG_METHOD_EXIT();
}

// Private remove function with no lock protection. The tableLock must
// be set before this function is called to protect the table. This simply
// removes the context from the context table.
Boolean EnumerationContextTable::_removeContext(EnumerationContext* en)
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
    // NOTE: Any functions that set _ClientClosed and _ProvidersComplete
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
        // KS_TODO Temporary diagnostic trace of enumerateContext internal info
        en->trace();

        enumContextTable.remove(en->getContextId());

        // KS_TODO - Confirm no reason to clear cache.  Responses should
        // be cleared since they are smart pointers and cach is in
        // the enum context.
        if (en->responseCacheSize() != 0)
        {
            PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,  // KS_TEMP
                "WARNING. Cache != 0 EnumerationContext Remove. ContextId=%s "
                " items in cache =%u",
                (const char *)en->getContextId().getCString(),
                en->responseCacheSize() ));
        }

        // Delete the enumerationContext object
        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,  // KS_TEMP
            "Delete Context. ContextId=%s",
            (const char *)en->getContextId().getCString() ));
        delete en;

        PEG_METHOD_EXIT();
        return true;
    }
    else
    {
        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,  // KS_TEMP TODO
            "_removeContext ERROR %s  _providersComplete=%s"
                "  clientClosed=%s",
            (const char *)en->getContextId().getCString(),
            boolToString(en->_providersComplete),
            boolToString(en->_clientClosed) ));
        //// KS_TODO remove this.  Test Diagnostic only.
        cout << "remove ignored. "
            << " clientClosed " << boolToString(en->_clientClosed)
            << " providersComplete " <<boolToString(en->_providersComplete)
            << endl;
    }
    PEG_METHOD_EXIT();
    return false;
}

Uint32 EnumerationContextTable::size()
{
    AutoMutex autoMut(tableLock);
    return(enumContextTable.size());
}

// If context name found, return pointer to that context.  Otherwise
// return 0
EnumerationContext* EnumerationContextTable::find(
    const String& enumerationContextName)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER, "EnumerationContextTable::find");

    AutoMutex autoMut(tableLock);

    EnumerationContext* en = 0;

    if(enumContextTable.lookup(enumerationContextName, en))
    {
        PEGASUS_DEBUG_ASSERT(en->valid());
    }
    // Return pointer or pointer = 0 if not found.
    PEG_METHOD_EXIT();
    return en;
}

/** Test all table entries and remove the ones timed out.

*/
void EnumerationContextTable::removeExpiredContexts()
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "EnumerationContextTable::removeExpiredContexts");

    PEGASUS_DEBUG_ASSERT(valid());

    if (stopThread())
    {
         return;
    }

    tableValidate(); // KS_TODO delete this.
    trace(); // KS_TODO delete this.

    // Lock the EnumerationContextTable so no operations can be accepted
    // during this process
    AutoMutex autoMut(tableLock);
    Array<String> removeList;

    Uint64 currentTime = TimeValue::getCurrentTime().toMicroseconds();
    // KS_TODO Remove this variable.  Used to help find crash.
    Uint32 ctr = 0;
    //// cout << "Expired test. Table size = " << size() << endl;
    // Search enumeration table for entries timed out
    for (EnumContextTable::Iterator i = enumContextTable.start(); i; i++)
    {
        ctr++;
        EnumerationContext* en = i.value();
        if (en->valid())
        {
            PEGASUS_DEBUG_ASSERT(en->valid());     // diagnostic. KS_TEMP

            if (en->_interOperationTimer != 0)
            {
                // Only set lock if there is a chance the timer is active.
                // redoes the above test after setting lock. Bypass if
                // locked
                if (en->tryLockContext())
                {
                    PEG_TRACE((TRC_DISPATCHER,Tracer::LEVEL4,
                        "EnumerationContextLock lock %s",
                               (const char*)en->getContextId().getCString() ));
                    //// cout << "locked " << en->getContextId() << endl;
                    // test if entry is active (timer not zero)
                    if (en->isTimedOut(currentTime))
                    {
                        //// cout << "timed out " << en->getContextId() << endl;
                        en->stopTimer();
                        // Force the client closed so nothing more accepted.
                        en->setClientClosed();

                        // If providers are complete we can remove the context
                        // Otherwise depend on provider completion to
                        // clean up the enumeration
                        if (en->providersComplete())
                        {
                            _enumerationsTimedOut++;

                            PEG_TRACE((TRC_DISPATCHER,Tracer::LEVEL4,
                                "EnumerationContextLock unlock %s",
                                (const char*)en->getContextId().getCString()));

                            // Insert in list to remove after this loop
                            removeList.append(en->getContextId());
                        }
                        else
                        {
                            //// cout << "Not timed out" << en->getContextId()
                            ////       << endl;
                            // depend on provider completion to close
                            // context.
                            // FUTURE Could use this to find cases
                            // where providers never complete
                            // KS-TODO
                            en->unlockContext();
                        }
                    }
                    else
                    {
                        //// cout << "not timed out " <<en->getContextId()
                        ///       << endl;
                        PEG_TRACE((TRC_DISPATCHER,Tracer::LEVEL4,
                            "EnumerationContextLock ERROR unlock %s",
                                (const char*)en->getContextId().getCString()));
                        en->unlockContext();
                    }
                }
            }
        }
        // KS_TODO- DELETE - Following test code only - REMOVE
        else
        {
            PEG_TRACE((TRC_DISPATCHER,Tracer::LEVEL4,
                "removeExpiredContexts ERROR, invalid context ctr=%u",
                 ctr));
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
    return;
}

// Validate every entry in the table. KS_TODO Diagnostic
void EnumerationContextTable::tableValidate()
{
    AutoMutex autoMut(tableLock);
    for (EnumContextTable::Iterator i = enumContextTable.start(); i; i++)
    {
        EnumerationContext* en = i.value();
        if (!en->valid())
        {
            en->trace();             // diagnostic. KS_TEMP
            PEGASUS_ASSERT(en->valid());
        }
    }
}

// interval is the timeout for the current operation that
// initiated this call.  It helps the timer thread decide how often
// to scan for timeouts.
void EnumerationContextTable::dispatchTimerThread(Uint32 interval)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
                     "EnumerationContextTable::dispatchTimerThread");

    PEGASUS_DEBUG_ASSERT(valid());

    AutoMutex autoMut(tableLock);

    // If the timer thread is idle, start it.
    if (timerThreadIdle())
    {
        // convert second timer to milliseconds and set it for double
        // the input defined interval. Set for the timer thread to
        // loop through the table every 30 seconds.

        _timeoutInterval = 5*1000;

        _nextTimeout = _timeoutInterval +
            TimeValue::getCurrentTime().toMilliseconds();

        // Start a detached thread to execute the timeout tests.
        // Thread runs until the timer is cleared or there are
        // no more contexts.
        if (_operationContextTimerThread.run() != PEGASUS_THREAD_OK)
        {
            MessageLoaderParms parms(
                "Server.EnumerationContextTable.THREAD_ERROR",
                "Failed to start pull operation timer thread.");

            Logger::put_l(
                Logger::ERROR_LOG, System::CIMSERVER, Logger::SEVERE,
                parms);
            PEGASUS_DEBUG_ASSERT(false); // This is system failure
        }
    }
    PEG_METHOD_EXIT();
}

void EnumerationContextTable::trace()
{
    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
        "EnumerationContextTable Trace. size=%u", enumContextTable.size()));

    AutoMutex autoMut(tableLock);

    for (EnumContextTable::Iterator i = enumContextTable.start(); i; i++)
    {
        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
            "ContextTable Entry: key [%s]",
                   (const char*)i.key().getCString() ));
        EnumerationContext* enumeration = i.value();
        enumeration->trace();
    }
}

/************************************************************************
//
//      TimerClass - List entry for List of all Enumeration Contexts for
//      which the interoperation timer is active. This uses the list class
//      and inserts entries into the list in order of timeout. Note: There
//      should never be more than one entry for any single context
//      since operations on a context are NOT concurrent.
//      NOTE: Incomplete: Consider converting the timer to a list mechanism
//      as cheaper way to execute timeouts.
//
************************************************************************/

// KS_TODO Remove this or finish it.
//class Timer : public Linkable
//{
//public:
//    Timer(const Uint32 timeout, EnumerationContext& ec
//          : _timeout(timeout), _ec(ec)) { }
//    ~Timer() { }
//    insertTimer()
//    bool isTimedOut():
//
//    // Test for equality of a context name
//    static bool equal(const Person* person, const void* ecName)
//    {
//        return *((String*)client_data) == ec->getContextName();
//    }
//private:
//    Timer(const Timer& x);
//    Timer& operation=(const Timer& x);
//    Uint64 _timeout;
//    EnumerationContext* -ec;
//};
//
//Typedef List<Timer, NullLock> TimerList;
//TimerList timerList;
//
//// if the head of list timer has expired return true and remove it from
//// the list
//bool Timer::isTimedOut()
//{
//    if (timerList.front(_ec->isTimedOut()))
//    {
//
//    }
//    return timerList.front(_ec->isTimedOut());
//
//}

PEGASUS_NAMESPACE_END
