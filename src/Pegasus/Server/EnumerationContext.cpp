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

#include "EnumerationContext.h"
#include <Pegasus/Common/Mutex.h>
#include <Pegasus/Common/Time.h>
#include <Pegasus/Common/TimeValue.h>
#include <Pegasus/Common/List.h>
#include <Pegasus/General/Guid.h>
#include <Pegasus/Common/CIMResponseData.h>
#include <Pegasus/Common/Condition.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/General/Stopwatch.h>
#include <Pegasus/Common/Thread.h>

PEGASUS_USING_STD;
PEGASUS_NAMESPACE_BEGIN
#define _TRACE(X) cout << __FILE__ << ":" << __LINE__ << " " << X << endl;
#define LOCAL_MIN(a, b) ((a < b) ? a : b)

#define MAX_ZERO_PULL_OPERATIONS 1000

static const char * _toString(Boolean x)
{
    return (x? "true" : "false");
}
static const char* _toCharP(Boolean x)
{
    return (x? "true" : "false");
}
/*
    Simple statistics keeper.  Keeps total and highWaterMark
    on 32 bit counter and computes average.
    LIMITATION: keeps total in 64 bits so overrun of this messes up
    averages.
*/
uint32Stats::uint32Stats()
    : _highWaterMark(0),
      _counter(0),
      _average(0),
      _total(0),
      _overflow(false)
{}

void uint32Stats::reset()
{
    _highWaterMark = 0;
    _counter = 0;
    _average = 0;
    _total = 0;
}
void uint32Stats::add(Uint32 newInfo)
{
    _counter++;
    // reset high water mark if necessary
    if (newInfo > _highWaterMark)
    {
        _highWaterMark = newInfo;
    }
    // if overflow set overflow indicator
    if ((_total + newInfo) < _total)
    {
        _overflow = true;
    }
    else   // update the total
    {
        _total += newInfo;
    }
}
Uint32 uint32Stats::getHighWaterMark()
{
    return _highWaterMark;
}
Uint32 uint32Stats::getAverage()
{
    return (_counter != 0)? _total/_counter : 0;
}
Uint32 uint32Stats::getCounter()
{
    return _counter;
}


// Create a new context. This is called only from the enumerationTable
// createContext function.
EnumerationContext::EnumerationContext(
    const CIMNamespaceName& nameSpace,
    Uint32 interOperationTimeoutValue,
    const Boolean continueOnError_,
    MessageType pullRequestType_,
    CIMResponseData::ResponseDataContent contentType
    )
    :
    _nameSpace(nameSpace),
    _continueOnError(continueOnError_),
    _interOperationTimer(0),
    _active(false),
    _error(false),
    _closed(false),
    _pullRequestType(pullRequestType_),
    _providersComplete(false),    
    _responseCache(contentType),
    _cimException(CIMException()),
    _pullOperationCounter(0),
    _cacheHighWaterMark(0),
    _operationTimeoutSec(interOperationTimeoutValue)
{
    _responseCache.valid();             // KS_TEMP

    // set start time for this enumeration sequence
    _startTime = TimeValue::getCurrentTime().toMicroseconds();
    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,   // KS_TEMP
               "Create EnumerationContext operationTimeoutSec %u"
               " responseCacheDataType %u StartTime %lu",
               _operationTimeoutSec,
               _responseCache.getResponseDataContent(),
               (unsigned long int)_startTime));
}

EnumerationContext::EnumerationContext(const EnumerationContext& x)
     :
    _responseCache(CIMResponseData::RESP_INSTANCE)
{
    _providersComplete = x._providersComplete;
    _nameSpace = x._nameSpace;
    _continueOnError = x._continueOnError;
    _active = x._active;
    _closed = x._closed;
    _error = x._error;

    //// KS_TBD THIS GENERATES ERROR, MUTEX no copy 
    ///  allowed._responseCache = x._responseCache;
}

/*
    Set the inter-operation timer for the timeout to the start of the
    next operation of this enumeration sequence. If the operationTimeout
    value = 0 we do not set the timer.  NOTE: depends on request input
    processing to determine if this is legal value.
*/
void EnumerationContext::startTimer()
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,"EnumerationContext::startTimer");
    PEGASUS_ASSERT(valid());   // KS_TEMP

    Uint64 currentTime = TimeValue::getCurrentTime().toMicroseconds();
    _interOperationTimer = (_operationTimeoutSec == 0) ?
        0 : currentTime + (_operationTimeoutSec * 1000000);

    // KS_TODO - Regularize the _operationTimeoutSec. Should this just be
    // microsec???
#ifdef PEGASUS_USE_PULL_TIMEOUT_THREAD
    _enumerationTable->dispatchTimerThread((_operationTimeoutSec));
#endif

    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,   // KS_TEMP
        "Start Timer. timeout = %lu Operation Timer %u sec."
           " diff %ld Context %s",
        (long unsigned int)_interOperationTimer,
        _operationTimeoutSec,
        (long signed int)(_interOperationTimer - currentTime),
        (const char*)getContextName().getCString()));

    PEG_METHOD_EXIT();
}

void EnumerationContext::stopTimer()
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,"EnumerationContext::stopTimer");
    PEGASUS_ASSERT(valid());              // KS_TEMP

    _interOperationTimer = 0;
    PEG_METHOD_EXIT();
}

/*
    Test interoperation timer against current time. Return true if timed out
    or timer set 0 zero indicating that the timer is not active.
    Returns boolean true if timer not zero and is Interoperation timer
    is greater than interoperation timeout.
*/
Boolean EnumerationContext::isTimedOut()
{
    PEGASUS_ASSERT(valid());            // KS_TEMP
    if (_interOperationTimer == 0)
    {
        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,  // KS_TEMP
            "Context Timer _interoperationTimer == 0."));
            return false;
    }
    Uint64 currentTime = TimeValue::getCurrentTime().toMicroseconds();
    Boolean timedOut = (_interOperationTimer < currentTime)? true : false;

    Uint64 diff;
    String sign;
    if (currentTime < _interOperationTimer)
    {
        sign = "+";
        diff = currentTime - _interOperationTimer;
    }
    else
    {
        sign = "-";
        diff = _interOperationTimer - currentTime;
    }

    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,      // KS_TEMP
        "Context Timer. timer(sec) %lu"
           "current set %lu diff %ld isTimedOut %s",
        (long unsigned int)((_interOperationTimer / 1000000)),
        (long unsigned int)currentTime / 1000000,
        (long signed int)(_interOperationTimer - currentTime) / 1000000,
        (timedOut? "true" : "false") ));

    // If it is timed out, set it inactive.
    if (timedOut)
    {
        _interOperationTimer = 0;
    }
    return(timedOut);
}

Boolean EnumerationContext::isActive()
{
    return _active;
}

Boolean EnumerationContext::isClosed()
{
    return _closed;
}

Boolean EnumerationContext::isErrorState()
{
    return _error;
}
Boolean EnumerationContext::setErrorState(CIMException x)
{
    _error = true;
    _cimException = x;
    return true;
}

//void EnumerationContext::setState(enumerationState newState)
//{
//    _enumerationState = newState;
//}

//Boolean EnumerationContext::isCurrentState(enumerationState state)
//{
//    return (_enumerationState == state)? true : false;
//}

void EnumerationContext::trace()
{
    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
        " Name %s. namespace %s timeOut %lu operationTimer %lu"
        " continueOnError %s pull msg Type %s"
        " providers complete %s"
        " timeOpen %lu millisec totalPullCount %u"
        " cache highWaterMark %u",
        (const char *)_enumerationContextName.getCString(),
        (const char *)_nameSpace.getString().getCString(),
        (long unsigned int)_operationTimeoutSec,
        (long unsigned int)_interOperationTimer,
        _toCharP(_continueOnError),
        MessageTypeToString(_pullRequestType),
        _toCharP(_providersComplete),
        (long unsigned int)
            (TimeValue::getCurrentTime().toMicroseconds() - _startTime)/1000,
        _pullOperationCounter,
        _cacheHighWaterMark));
}

/**
 * validate the magic object for this context
 * 
 * @return Boolean True if valid object.
 */
Boolean EnumerationContext::valid()
{
    return _magic;
}

/*
    Test the current pull message against the type set on the create
    context. they must match
*/
Boolean EnumerationContext::isValidPullRequestType(MessageType type)
{
    return(type == _pullRequestType);
}

EnumerationContext::~EnumerationContext()
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "EnumerationContext::~EnumerationContext()");
}

/*
    Insert complete CIMResponseData entities into the cache, protecting the
    cache from simultaneous puts/gets
*/

void EnumerationContext::putCache(OperationAggregate*& poA,
                                  CIMResponseMessage*& response,
                                  Boolean isComplete)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER, "EnumerationContext::putCache");
    PEGASUS_ASSERT(valid());   // KS_TEMP;
    CIMResponseData & to = _responseCache;

    _providersComplete = isComplete;

    Uint32 type = poA->getRequestType();

    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,  // KS_TEMP
        "Enter putCache, response isComplete %s Type %u",
        _toCharP(isComplete), to.getResponseDataContent() ));

    // Cannot use Automutex because must release lock before the provider
    // wait at end of code.  Alternate is to put into the cache before
    // we do the automutex. KS_TODO Review this.
    ///AutoMutex autoMut(_cacheBlock);
    _cacheBlock.lock();

    // do move for each type.
    switch(type)
    {
        case CIM_ENUMERATE_INSTANCES_REQUEST_MESSAGE :
        {
            CIMEnumerateInstancesResponseMessage* rsp =
                (CIMEnumerateInstancesResponseMessage*)response;
            CIMResponseData & from = rsp->getResponseData();
            to.appendResponseData(from);
            break;
        }

        case CIM_ENUMERATE_INSTANCE_NAMES_REQUEST_MESSAGE :
        {
            CIMEnumerateInstanceNamesResponseMessage* rsp =
                (CIMEnumerateInstanceNamesResponseMessage*)response;
            CIMResponseData & from = rsp->getResponseData();
            to.appendResponseData(from);
            break;
        }

        case CIM_REFERENCES_REQUEST_MESSAGE :
        {
            CIMReferencesResponseMessage* rsp =
                (CIMReferencesResponseMessage*)response;
            CIMResponseData & from = rsp->getResponseData();
            to.appendResponseData(from);
            break;
        }

        case CIM_REFERENCE_NAMES_REQUEST_MESSAGE :
        {
            CIMReferenceNamesResponseMessage* rsp =
                (CIMReferenceNamesResponseMessage*)response;
            CIMResponseData & from = rsp->getResponseData();
            to.appendResponseData(from);
            break;
        }

        case CIM_ASSOCIATORS_REQUEST_MESSAGE :
        {
            CIMAssociatorsResponseMessage* rsp =
                (CIMAssociatorsResponseMessage*)response;
            CIMResponseData & from = rsp->getResponseData();
            to.appendResponseData(from);
            break;
        }

        case CIM_ASSOCIATOR_NAMES_REQUEST_MESSAGE :
        {
            CIMAssociatorNamesResponseMessage* rsp =
                (CIMAssociatorNamesResponseMessage*)response;
            CIMResponseData & from = rsp->getResponseData();
            to.appendResponseData(from);
            break;
        }

//
//      case CIM_EXEC_QUERY_REQUEST_MESSAGE :
//          //handleExecQueryResponseAggregation(poA);
//          break;

        default:
            static const char failMsg[] =
                "Invalid response type to pull: ";
            PEG_TRACE(( TRC_DISCARDED_DATA, Tracer::LEVEL1,
                "%u",
                type));
            PEGASUS_ASSERT(0);
            break;
    } // switch

    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,  // EXP_PULL_TEMP
        "After putCache cacheSize %u. CIMResponseData size %u."
        " signal CacheSizeConditon",
        cacheSize(), to.size()  ));

    // test and set the high water mark for this cache.
    if (cacheSize() > _cacheHighWaterMark)
    {
        _cacheHighWaterMark = cacheSize();
    }
    
    // signal that we have added to the CIMResponseData cache. Do this
    // before waiting to be sure any cache size wait is terminated.
    signalCacheSizeCondition();

    // unlock the cache.  Must do this before the provider wait.
    _cacheBlock.unlock();

    // KS_TODO externalize the cache maximum size.
    //Wait for the cache size to drop below the limit requeste here.
    waitProviderLimitCondition(1000);

    PEG_METHOD_EXIT();
}

/***************************************************************************** 
** 
**     Methods to support the EnumerationContext CIMResponseData Cache
** 
*****************************************************************************/ 
/*
    Move the number of objects defined by count from the CIMResponseData
    cache for this EnumerationContext to theCIMResponseData object 
     defined by the input parameter.
    The wait function is called before removing items from the cache and
    only completes when a. there are sufficient objects, b. the providers
    have completed, c. an error has occurred.
    // KS_TODO - The error return has not been implemented. Do we need it??
*/
Boolean EnumerationContext::getCacheResponseData(
    Uint32 count,
    CIMResponseData& rtn)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "EnumerationContext::getCacheResponseData");

    PEGASUS_ASSERT(valid());   // KS_TEMP;

    // Wait for cache size to match count or providers to complete
    // set mutex only for the move objects. We don't want to mutex for
    // the wait period since we expect things to be put into the
    // cache during this period.
    waitCacheSizeCondition(count);

    AutoMutex autoMut(_cacheBlock);

    // move the defined number of objects from the cache to the
    // return object.
    Uint32 rtncount = rtn.moveObjects(_responseCache, count);

    // Signal the ProviderLimitCondition that the cache size may
    // have changed.
    signalProviderLimitCondition();

    PEG_METHOD_EXIT();
    return true;
}

Uint32 EnumerationContext::cacheSize()
{
    PEGASUS_ASSERT(_responseCache.valid());
    return _responseCache.size();
}

// Test the cache to determine if we are ready to send a response.
// The test is two parts, a) enough objects (i.e. GE size input parameter)
// or end-of-sequence set indicating that we have completed provider
// processing.
// The return is executed only when one of these conditions has been met.
// This function uses a condition variable to control the return.
    
void EnumerationContext::waitCacheSizeCondition(Uint32 size)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "EnumerationContext::waitCacheSizeCondition");
    PEGASUS_ASSERT(valid());   // KS_TEMP
    CacheTestCondMutex.lock();

    // start timer to get time of wait for statistics.
    Stopwatch waitTimer;
    waitTimer.start();

    // condition variable wait loop
    while (!_providersComplete && (cacheSize() < size)) 
    {
        CacheTestCondition.wait(CacheTestCondMutex);
    }
    CacheTestCondMutex.unlock();

    waitTimer.stop();

    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
               "waitCacheSizeConditon return "
               "Request Size %u complete %s result %s time %lu Usec",
               size,
               _toCharP(_providersComplete),
               _toCharP((!_providersComplete && (cacheSize()) < size)),
               (unsigned long int)waitTimer.getElapsedUsec() ));

    PEG_METHOD_EXIT();
}

void EnumerationContext::signalCacheSizeCondition()
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "EnumerationContext::signalCacheSizeCondition");

    PEGASUS_ASSERT(valid());   // KS_TEMP
    CacheTestCondMutex.lock();
    CacheTestCondition.signal();
    CacheTestCondMutex.unlock();

    PEG_METHOD_EXIT();
}

/***************************************************************************** 
** 
**  Provider Limit Condition Variable functions. wait, signal
**  
*****************************************************************************/
/*
    Wait condition on returning to providers from putcache.  This allows
    dispatcher to stop responses from providers while pull operations
    reduce the size of the cache.  The condition variable should be
    signaled when the cache size drops below a defined point OR
    if a close is received (so the responses can be discarded) 
*/ 
void EnumerationContext::waitProviderLimitCondition(Uint32 limit)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "EnumerationContext::waitProviderLimitCondition");

    PEGASUS_ASSERT(valid());   // KS_TEMP
    providerLimitConditionMutex.lock();

    Stopwatch waitTimer;
    waitTimer.start();
    while (!_closed && (cacheSize() > limit)) 
    {
        providerLimitCondition.wait(providerLimitConditionMutex);
    }

    waitTimer.stop();

    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,  // EXP_PULL_TEMP
       "waitproviderLimitCondition exit state %s cacheSize %u size %u "
       "closed %s time %lu",
         _toCharP((!_closed && (cacheSize() < limit))),
         cacheSize(), limit, _toCharP(_closed),
         (long unsigned int)waitTimer.getElapsedUsec() ));

    providerLimitConditionMutex.unlock();
    PEG_METHOD_EXIT();
}

void EnumerationContext::signalProviderLimitCondition()
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "EnumerationContext::signalProviderLimitCondition()");

    PEGASUS_ASSERT(valid());   // KS_TEMP
    providerLimitConditionMutex.lock();
    providerLimitCondition.signal();
    providerLimitConditionMutex.unlock();

    PEG_METHOD_EXIT();
}

Boolean EnumerationContext::incPullOperationCounter(Boolean isZeroLength)
{
    PEGASUS_ASSERT(valid());   // KS_TEMP
    _pullOperationCounter++;

    if (isZeroLength)
    {
        _zeroRtnPullOperationCounter++;
    }
    else
    {
        _zeroRtnPullOperationCounter = 0;
    }
    return (_zeroRtnPullOperationCounter > MAX_ZERO_PULL_OPERATIONS);
}

// set providers complete flag and signal the CacheSizeCondition.
// This could awaken any wait at the cacheWait.

void EnumerationContext::setProvidersComplete()
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "EnumerationContext::setProvidersComplete()");

    PEGASUS_ASSERT(valid());   // KS_TEMP

    _providersComplete = true;

    // Signal CacheSize Condition flag that providers have completed.
    signalCacheSizeCondition();

    PEG_METHOD_EXIT();
}


Boolean EnumerationContext::ifProvidersComplete()
{
    return _providersComplete;
}

Boolean EnumerationContext::ifEnumerationComplete()
{
    PEGASUS_ASSERT(valid());   // KS_TEMP
    if (ifProvidersComplete() && (cacheSize() == 0))
    {
        return true;
    }
    else
    {
        setActiveState(false);
    }
  
    return false;
}

Boolean EnumerationContext::setClosed()
{
    PEGASUS_ASSERT(valid());   // KS_TEMP
    // KS_TODO - Is this all the functionality we need to close enum??
    // signal closed because that could force a blocked put to continue.
    signalProviderLimitCondition();
    _closed = true;
    return true;
}

Boolean EnumerationContext::setActiveState(Boolean state)
{
    // KS_TODO - Clean this one up.  What we should really do is
    // error if new active same as old active
    // Active means processing operation.
    _active = state;
    if (_active)
    {
        stopTimer();
    }
    else
    {
        startTimer();
    }
    return _active;
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
/************************************************************************
// 
//      EnumerationContextTable Implementation
//
************************************************************************/
// Thread execution function for timerThread(). This thread executes
// regular timeout tests on active contexts and closes them or marks them
// for close if timed out.  This is required for those cases where a
// pull sequence is terminated without either completing or closing the
// sequence.
#ifdef PEGASUS_USE_PULL_TIMEOUT_THREAD
ThreadReturnType PEGASUS_THREAD_CDECL operationContextTimerThread(void* parm)
{
    Thread *my_handle = (Thread *)parm;
    EnumerationTable* et = (EnumerationTable *)my_handle->get_parm();
    PEGASUS_ASSERT(et->valid());   // KS_TEMP
    ////cout << "Start Timer Thread" << endl;

    Uint32 loopInterval = 2000; // time milliseconds for the loop itself

    // execute loop at regular intervals until there are no more contexts
    // KS_TODO - The choice of existence of contexts as the loop controller
    // means that we will look through many inactive contexts to find any
    // that are both active and timed out (much search for a very unusual
    // condition). This algorithm probably not optimum.
    while (et->size() != 0)
    {
        Uint32 counter = 0;
        // Short timeout loop to be sure we can shutdown this thread.
        while (!et->isTimedOut())
        {
            Threads::sleep(loopInterval);
        }
        ////cout << "Before removeExpiredContexts " << counter++ << endl; 
        et->removeExpiredContexts();
        et->updateNextTimeout();
        ////cout << "after remove expired contracts " << et->size() << endl;
    }

    // reset the timeout value to indicate that the thread is quiting
    et->setTimerThreadIdle();
    ////cout << "Close Timer Thread" << endl;
    return ThreadReturnType(0);
}
#endif

/*
    Create the Enumeration table and set the maximum/minimum values for
    input parameters that will require this.
*/
EnumerationTable::EnumerationTable(
    Uint32 defaultInteroperationTimeoutValue)
    :
    _timeoutInterval(0),
    _nextTimeout(0),
    _enumContextCounter(1),
    _cacheHighWaterMark(0),
    _pullOperationDefaultTimeout(defaultInteroperationTimeoutValue)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER, "EnumerationTable::EnumerationTable");

    PEG_METHOD_EXIT();
}


/* remove all contexts and delete them
*/
EnumerationTable::~EnumerationTable()
{
    PEG_METHOD_ENTER(TRC_DISPATCHER, "EnumerationTable::~EnumerationTable");
    for (HT::Iterator i = ht.start(); i; i++)
    {
        EnumerationContext* enumeration = i.value();

        // KS_TODO _ clean up thread before closing the table

        delete enumeration;
        ht.remove(i.key());
    }
    PEG_METHOD_EXIT();
}

EnumerationContext* EnumerationTable::createContext(
    const CIMNamespaceName& nameSpace,
    Uint32Arg&  operationTimeoutParam,
    Boolean continueOnError,
    MessageType pullRequestType,
    CIMResponseData::ResponseDataContent contentType,
    String& enumerationContextName)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,"EnumerationTable::createContext");

    AutoMutex autoMut(tableLock);
    // set the operation timeout to either the default or current
    // value
    Uint32 operationTimeout = 
        (operationTimeoutParam.isNull())? _pullOperationDefaultTimeout
                                          :
                                          operationTimeoutParam.getValue();

    // Create the new context, Context name is guid appended by
    // monolithically increasing counter. The interoperationTimeout is
    // defined by either the default or input value.
    EnumerationContext* ec = new EnumerationContext(nameSpace,
        operationTimeout,
        continueOnError,
        pullRequestType,
        contentType );

    // set the pointer to the enumeration table into the context
    ec->_enumerationTable = this;

    // Create new context name (guid + monolithic increasing counter)
    // KS_TODO - Do not need guid for each enum.  We should be able to
    // use this once and then just append the counter for the life of
    // the context table. KS_TODO - What issues might there be with
    // a context that is simply monolithicly increasing?? i.e. security.
    // KS_TODO - Modify this to use Pegasus StringConversion 
    String ecn = Guid::getGuid();
    char t[24];
    sprintf(t, "-%lu", (long unsigned int)_enumContextCounter++);
    ecn.append(t);

    // KS_TODO - Put this into the context constructor?
    ec->_enumerationContextName = ecn;
    enumerationContextName = ecn;

    // insert new context into the table
    if(!ht.insert(ecn, ec))
    {
        // KS_TODO - This should be a serious system error log rather than
        // just a trace.
        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL1,
            "Error Creating Enumeration Context %s",
            (const char*)ecn.getCString() ));
        ec = 0;
        PEGASUS_ASSERT(false);
    }
#ifndef PEGASUS_USE_PULL_TIMEOUT_THREAD
    if (isTimedOut())
    {
        removeExpiredContexts();
    }
#endif
    return ec;
}

Boolean EnumerationTable::remove(const String& enumerationContextName)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER, "EnumerationTable::remove");
    AutoMutex autoMut(tableLock);
    EnumerationContext* en = find(enumerationContextName);
   
    PEG_METHOD_EXIT();  
    return _remove(en);
}

// KS_TODO - Clean up fact that we repeat code above and here and have
// 2 parallel functions for deletion (pointer and name)
Boolean EnumerationTable::remove(EnumerationContext* en)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,"EnumerationTable::remove");
    AutoMutex autoMut(tableLock);

    PEG_METHOD_EXIT();    
    return _remove(en);
}

// Private remove function with no lock protection. The tableLock must
// be set before this function is called to protect the table
Boolean EnumerationTable::_remove(EnumerationContext* en)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,"EnumerationTable::_remove");
    PEGASUS_ASSERT(en->valid());            // KS_TEMP

    // If it is valid and providers are complete, we can delete
    // the enumerationContext.  If providers not complete, only
    // completion of provider deliveries can initiate removal of
    // the enumeration context.
    if (en != 0 && en->_providersComplete)
    {
        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,  // KS_TEMP
            "remove Context from ptr %s",
            (const char *)en->getContextName().getCString() ));
        // Remove from enumerationTable.

        if (en->_cacheHighWaterMark > _cacheHighWaterMark)
        {
            _cacheHighWaterMark = en->_cacheHighWaterMark;
        }

        ht.remove(en->getContextName());

        // KS_TODO - Do we need to clear the cache?

        // Delete the enumerationContext object
        delete en;

        PEG_METHOD_EXIT();
        return true;
    }
    else
    {
        // KS_TODO - Should we mark this closed just to be sure.
        PEG_METHOD_EXIT();
        return false;
    }
}
Uint32 EnumerationTable::size() const
{
    //AutoMutex autoMut(tableLock);
    return(ht.size());
}

void EnumerationTable::clear()
{
    AutoMutex autoMut(tableLock);
    ht.clear();
}

Uint32 EnumerationTable::getMinPullDefaultTimeout() const
{
    return _pullOperationDefaultTimeout;
}

// KS_TODO - Clean this one up to one return.
EnumerationContext* EnumerationTable::find(
    const String& enumerationContextName)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER, "EnumerationTable::find");

    AutoMutex autoMut(tableLock);

    EnumerationContext* x;
    if(ht.lookup(enumerationContextName, x))
    {
        PEG_METHOD_EXIT();
        return x;
    }
    else
    {   
        // Return not found indicator
        PEG_METHOD_EXIT();
        return 0;     
    }
}

/** Test all table entries and remove the ones timed out.

*/
void EnumerationTable::removeExpiredContexts()
{
    ////cout << "Enter removeExpiredContexts" << endl;
    PEG_METHOD_ENTER(TRC_DISPATCHER, "EnumerationTable::findExpiredContexts");

    AutoMutex autoMut(tableLock);
    Uint64 currentTime = TimeValue::getCurrentTime().toMicroseconds();

    // Search enumeration table for entries timed out
    for (HT::Iterator i = ht.start(); i; i++)
    {
        EnumerationContext* en = i.value();
        PEGASUS_ASSERT(en->valid());             // diagnostic. KS_TEMP

        // test if entry is active (timer not zero)
        if (en->_interOperationTimer != 0)
        {
            if (en->isTimedOut())
            {
//              cout << "Entry timed out " << en->getContextName()
//                  << " " << en->_interOperationTimer <<  endl;
                en->_interOperationTimer = 0;
                _remove(en);
            }
//          else
//          {
//              cout << "Entry NOT timed out " << en->getContextName()
//                  << " " << en->_interOperationTimer <<  endl;
//          }
        }
    }
//  cout << "exit removeExpiredContexts" << endl;
    PEG_METHOD_EXIT();
    return;
}
void EnumerationTable::dispatchTimerThread(Uint32 interval)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER, "EnumerationTable::dispatchTimerThread");
//  cout <<"Enter disptchTimerThread " << size() << endl;  // KS_TEMP

    AutoMutex autoMut(tableLock);
    if (timerThreadIdle())
    {
        // convert second timer to milliseconds
        if (interval < _timeoutInterval)
        {
            _timeoutInterval = interval * 1000;
        }
        Uint64 nextTimeout = interval +
            TimeValue::getCurrentTime().toMilliseconds();
        if (nextTimeout < _nextTimeout)
        {
            _nextTimeout = nextTimeout;
        }

#ifdef PEGASUS_USE_PULL_TIMEOUT_THREAD
        // Start a detached thread that executes the timeout tests.
        // This thread runs until the timer is cleared or there are
        // no more contexts.

//      cout <<"Calling Dispatch timer Thread " 
//           << _timeoutInterval << endl;  // KS_TEMP

        PEGASUS_ASSERT(valid());   // KS_TEMP

        Thread thread(operationContextTimerThread, this, true);
        if (thread.run() != PEGASUS_THREAD_OK)
        {
            // KS_TODO - Convert this to log error or something but it
            // is a fatal error.
            cerr << "Error. Thread Run returned Error "
                << endl;
            PEGASUS_ASSERT(false);
            PEG_METHOD_EXIT();
            return;
        }
#endif
    }
    PEG_METHOD_EXIT();
}


Boolean EnumerationTable::isTimedOut() const
{
    return (_nextTimeout < TimeValue::getCurrentTime().toMilliseconds() );
}

void EnumerationTable::trace()
{
    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
        "Context Table Trace. size = %u", ht.size()));

    AutoMutex autoMut(tableLock);
    for (HT::Iterator i = ht.start(); i; i++)
    {
        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
            "key [%s]", (const char*)i.key().getCString() ));
        EnumerationContext* enumeration = i.value();
        enumeration->trace();
    }
}
/**
 * validate the magic object for this context
 * 
 * @return Boolean True if valid object.
 */
Boolean EnumerationTable::valid()
{
    return _magic;
}


PEGASUS_NAMESPACE_END
