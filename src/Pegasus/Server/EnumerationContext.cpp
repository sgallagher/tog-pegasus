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
#include <Pegasus/Server/EnumerationTable.h>

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
    _operationTimeoutSec(interOperationTimeoutValue),
    _continueOnError(continueOnError_),
    _interOperationTimer(0),
    _pullRequestType(pullRequestType_),
    _closed(false),
    _providersComplete(false),    
    _active(false),
    _error(false),
    _responseCache(contentType),
    _cimException(CIMException()),
    _cacheHighWaterMark(0),
    _cacheTestCondMutex(Mutex::NON_RECURSIVE),
    _conditionCounter(0),
    _providerLimitConditionMutex(Mutex::NON_RECURSIVE),
    _pullOperationCounter(0),
    _zeroRtnPullOperationCounter(0)
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
// KS_TODO - Temporarily disabled the timer test thread to determine if this
// is causing the problem with crashes.  Right not it appears not because
// the problem occurred in testing 16 Jan.  Will leave this in for one day.
//    _enumerationTable->dispatchTimerThread((_operationTimeoutSec));
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

void EnumerationContext::trace()
{
    PEGASUS_ASSERT(valid());
    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
        " EnumerationContext.ContextId= %s."
        " namespace %s timeOut %lu operationTimer %lu"
        " continueOnError %s pull msg Type %s"
        " providers complete %s"
        " closed %s"
        " timeOpen %lu millisec totalPullCount %u"
        " cache highWaterMark %u",
        (const char *)_enumerationContextName.getCString(),
        (const char *)_nameSpace.getString().getCString(),
        (long unsigned int)_operationTimeoutSec,
        (long unsigned int)_interOperationTimer,
        _toCharP(_continueOnError),
        MessageTypeToString(_pullRequestType),
        _toCharP(_providersComplete),
        _toCharP(_closed),
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
    _responseCache.valid(); // KS_TEMP TODO DELETE
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
    PEG_METHOD_EXIT();
}

void EnumerationContext::removeContext()
{
    PEG_METHOD_ENTER(TRC_DISPATCHER, "EnumerationContext::removeContext");
    PEGASUS_ASSERT(valid());   // KS_TEMP;

    // KS_TODO - We should be able to go to direct pointer function
    _enumerationTable->remove(_enumerationContextName);
    PEG_METHOD_EXIT();
}

/*
    Insert complete CIMResponseData entities into the cache, and if the
    cache is full, wait until it the size drops below the full limit.
    If the operation is closed, we discard the response. If
    this is the last response, remove the enumerationContext
*/
void EnumerationContext::putCache(MessageType type,
                                  CIMResponseMessage*& response,
                                  Boolean isComplete)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER, "EnumerationContext::putCache");
    PEGASUS_ASSERT(valid());   // KS_TEMP;
    trace();                   // KS_TEMP

    if (_providersComplete)
    {
        cout << "ERROR in Providers Complete "<< __FILE__ << __LINE__ << endl;
        trace();
    }

    PEGASUS_ASSERT(!_providersComplete);

    // set providersComplete flag in context.
    _providersComplete = isComplete;

    CIMResponseData& to = _responseCache;
    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,  // KS_TEMP
        "Enter putCache, response isComplete %s ResponseDataType %u",
        _toCharP(isComplete), to.getResponseDataContent() ));

    // If an operation has closed the enumerationContext can
    // ignore any received responses until the isComplete is received and
    // then remove the Context.
    if (_closed)
    {
        // if providers are complete, we can remove context
        /// KS_TODO. Is this correct. Is there any chance of a response
        /// in process at this point??
        if (isComplete)
        {
            ///// TODO Confirm this. removeContext();
        }
    }
    else
    {
        _insertResponseIntoCache(type, response);
    
        // test and set the high water mark for this cache.
        if (responseCacheSize() > _cacheHighWaterMark)
        {
            _cacheHighWaterMark = responseCacheSize();
        }
            
        // Signal that we have added to the CIMResponseData cache. Do this
        // before waiting to be sure any cache size wait is terminated.
        signalCacheSizeCondition();
    
        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,  // EXP_PULL_TEMP
            "After putCache responseCacheSize %u. CIMResponseData size %u."
            " signal CacheSizeConditon",
            responseCacheSize(), to.size() ));
    
        // Wait for the cache size to drop below the limit requested here
        // before returning to caller. This blocks providers until wait
        // completed.
        if (!_providersComplete)
        {
            waitProviderLimitCondition(_responseCacheMaximumSize);
        }
    }

    PEG_METHOD_EXIT();
}

// Internal function to actually insert into the cache. This function
// operates with the cache locked.
void EnumerationContext::_insertResponseIntoCache(MessageType type,
                                  CIMResponseMessage*& response)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "EnumerationContext::_insertResponseIntoCache");

    CIMResponseData& to = _responseCache;

    AutoMutex autoMut(_responseCacheMutex);

    // Append the new Response to the CIMResponseData in the cache
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

        default:
            static const char failMsg[] =
                "Invalid response type to pull: ";
            PEG_TRACE(( TRC_DISCARDED_DATA, Tracer::LEVEL1,
                "%u",
                type));
            PEGASUS_ASSERT(0);
            break;
    } // end switch

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

    AutoMutex autoMut(_responseCacheMutex);

    // move the defined number of objects from the cache to the
    // return object.
    Uint32 rtncount = rtn.moveObjects(_responseCache, count);

    // Signal the ProviderLimitCondition that the cache size may
    // have changed.
    signalProviderLimitCondition();

    PEG_METHOD_EXIT();
    return true;
}

Uint32 EnumerationContext::responseCacheSize()
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "EnumerationContext::responseCacheSize");
    PEGASUS_ASSERT(valid());   // KS_TEMP
    PEGASUS_ASSERT(_responseCache.valid());   // KS_TEMP
    PEG_METHOD_EXIT();
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

    // if following conditions, we know that there are no more in cache
    // so we just return.
    if (_providersComplete || _closed)
    {
        PEG_METHOD_EXIT();
        return;
    }

    // start timer to get time of wait for statistics.
    Stopwatch waitTimer;        // KS_TEMP I think. should this be perm.
    waitTimer.start();

    // condition variable wait loop. waits on cache size or
    // providers complete
    // KS_TODO change this to automutex
    _cacheTestCondMutex.lock();
    while (!_providersComplete && (responseCacheSize() < size)) 
    {
        _cacheTestCondition.wait(_cacheTestCondMutex);
    }
    _cacheTestCondMutex.unlock();

    waitTimer.stop();

    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
        "waitCacheSizeConditon return "
        "Request Size %u complete %s result %s time %lu Usec",
        size,
        _toCharP(_providersComplete),
        _toCharP((!_providersComplete && (responseCacheSize()) < size)),
        (unsigned long int)waitTimer.getElapsedUsec() ));

    PEG_METHOD_EXIT();
}

void EnumerationContext::signalCacheSizeCondition()
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "EnumerationContext::signalCacheSizeCondition");
    PEGASUS_ASSERT(valid());   // KS_TEMP

    AutoMutex autoMut(_cacheTestCondMutex);

    _cacheTestCondition.signal();

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

    // KS_TODO change this to automutex
    _providerLimitConditionMutex.lock();

    Stopwatch waitTimer;
    waitTimer.start();
    while (!_closed && (responseCacheSize() > limit)) 
    {
        _providerLimitCondition.wait(_providerLimitConditionMutex);
    }

    waitTimer.stop();

    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,  // EXP_PULL_TEMP
       "waitproviderLimitCondition exit state %s responseCacheSize %u size %u "
       "closed %s time %lu",
         _toCharP((!_closed && (responseCacheSize() < limit))),
         responseCacheSize(), limit, _toCharP(_closed),
         (long unsigned int)waitTimer.getElapsedUsec() ));

    _providerLimitConditionMutex.unlock();
    PEG_METHOD_EXIT();
}

void EnumerationContext::signalProviderLimitCondition()
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "EnumerationContext::signalProviderLimitCondition()");

    PEGASUS_ASSERT(valid());   // KS_TEMP

    AutoMutex autoMut(_providerLimitConditionMutex);

    _providerLimitCondition.signal();

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
    if (ifProvidersComplete() && (responseCacheSize() == 0))
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
    // return false if already closed
    if (_closed)
    {
        return false;
    }
    _closed = true;

    if (_providersComplete)
    {
        // Providers are complete, close the context
        ///// KS_TODO Clean this up.  Do not remove yet.removeContext();
    }
    else
    {
        // Signal the limit on provider responses in case it is in wait
        signalProviderLimitCondition();
    }
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



PEGASUS_NAMESPACE_END
