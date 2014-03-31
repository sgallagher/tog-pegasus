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
    _cimException(CIMException()),
    _nameSpace(nameSpace),
    _operationTimeoutSec(interOperationTimeoutValue),
    _continueOnError(continueOnError_),
    _interOperationTimer(0),
    _pullRequestType(pullRequestType_),
    _clientClosed(false),
    _providersComplete(false),
    _processing(true),    // set true because always created during processing
    _error(false),
    _responseCache(contentType),
    _cacheTestCondMutex(Mutex::NON_RECURSIVE),
    _conditionCounter(0),
    _providerLimitConditionMutex(Mutex::NON_RECURSIVE),
    _pullOperationCounter(0),
    _consecutiveZeroLenMaxObjectRequestCounter(0),
    _cacheHighWaterMark(0)
{
    _responseCache.valid();             // KS_TEMP

    // set start time for this enumeration sequence
    _startTime = TimeValue::getCurrentTime().toMicroseconds();

    // KS_TODO Consider this one a temporary diagnostic. Delete before
    // checkin
    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,   // KS_TEMP TODO Delete
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
    _processing = x._processing;
    _clientClosed = x._clientClosed;
    _error = x._error;
}

void EnumerationContext::setRequestProperties(
    const Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
    // Set request properties into the responseCache that are required
    // later for pull operations. Not required for names operations
    // since the attributes defined characteristics of objects returned
    // (qualifiers, classorigin, propertylists).
    // Always sets includeQualifiers == false since this attribute
    // not supported for pull operations.
    _responseCache.setRequestProperties(
        false, includeClassOrigin, propertyList);
}

/*
    Set the inter-operation timer for the timeout to the start of the
    next operation of this enumeration sequence. If the operationTimeout
    value = 0 do not set the timer.
*/
void EnumerationContext::startTimer()
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,"EnumerationContext::startTimer");
    PEGASUS_ASSERT(valid());   // KS_TEMP

    Uint64 currentTime = TimeValue::getCurrentTime().toMicroseconds();
    _interOperationTimer = (_operationTimeoutSec == 0) ?
        0 : currentTime + (_operationTimeoutSec * 1000000);

#ifdef PEGASUS_USE_PULL_TIMEOUT_THREAD
// KS_TODO - Temporarily disabled the timer test thread to determine if this
// is causing the problem with crashes.  Right not it appears not because
// the problem occurred in testing 16 Jan.  Will leave this in for one day.
//    _enumerationContextTable->dispatchTimerThread((_operationTimeoutSec));
#endif

    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,   // KS_TEMP
        "Start Timer. timeout = %lu Operation Timer %u sec."
           " diff %ld Context %s",
        (long unsigned int)_interOperationTimer,
        _operationTimeoutSec,
        (long signed int)(_interOperationTimer - currentTime),
        (const char*)getName().getCString()));

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
    is greater than interoperation timeout (i.e timed out).
*/
Boolean EnumerationContext::isTimedOut(Uint64 currentTime)
{
    PEGASUS_ASSERT(valid());            // KS_TEMP
    if (_interOperationTimer == 0)
    {
        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,  // KS_TEMP
            "Context Timer _interoperationTimer == 0."));
            return false;
    }
    Boolean timedOut = (_interOperationTimer < currentTime)? true : false;

    // KS_TODO all of the following is diagnostic.
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

Boolean EnumerationContext::isTimedOut()
{
    Uint64 currentTime = TimeValue::getCurrentTime().toMicroseconds();
    return isTimedOut(currentTime);
}

// FUTURE: In future consider list of exceptions since there may be
// multiples.
void EnumerationContext::setErrorState(CIMException x)
{
    PEGASUS_ASSERT(valid());
    // Until we handle multiple errors, return only the first error
    if (_error)
    {
        return;
    }
    // Set exception first and use flag as indicator to avoid ipc issues.
    _cimException = x;
    _error = true;
}

// Diagnostic display of data in the enumeration context object
void EnumerationContext::trace()
{
    PEGASUS_ASSERT(valid());
    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
        "EnumerationContext.ContextId=%s "
        "namespace %s timeOut %lu operationTimer=%lu "
        "continueOnError=%s pull msg Type=%s "
        "providers complete=%s "
        "closed=%s "
        "timeOpen %lu millisec totalPullCount=%u "
        "cache highWaterMark=%u ",
        (const char *)_enumerationContextName.getCString(),
        (const char *)_nameSpace.getString().getCString(),
        (long unsigned int)_operationTimeoutSec,
        (long unsigned int)_interOperationTimer,
        boolToString(_continueOnError),
        MessageTypeToString(_pullRequestType),
        boolToString(_providersComplete),
        boolToString(_clientClosed),
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
Boolean EnumerationContext::valid() const
{
    _responseCache.valid(); // KS_TEMP TODO DELETE
    return _magic;
}

EnumerationContext::~EnumerationContext()
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "EnumerationContext::~EnumerationContext()");
    PEG_METHOD_EXIT();
}

/*
    Insert complete CIMResponseData entities into the cache. If the
    cache is full (at its size limit), wait until it the size drops
    below the full limit.
    If the operation is closed, we discard the response. If
    this is the last response, remove the enumerationContext
    Return true if putCache worked, false if closed and nothing put
    NOTE: This is single threaded. It is based on the mutex in
    _enquueueResponse which serializes independent responses. See
    _enqueueResponseMutex.

*/
Boolean EnumerationContext::putCache(CIMResponseMessage*& response,
    Boolean providersComplete)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER, "EnumerationContext::putCache");

    PEGASUS_ASSERT(valid());   // KS_TEMP;

    // Design error if we ever get here with providers already set complete
    PEGASUS_ASSERT(!_providersComplete);

    //// KS_TODO Delete before checkin all this diagnostic
    CIMResponseData& to = _responseCache;
    CIMResponseDataMessage* localResponse =
        dynamic_cast<CIMResponseDataMessage*>(response);

    CIMResponseData & from = localResponse->getResponseData();
    from.traceResponseData();

    // KS_TODO Calling this for everything, not just Enum.
    // Need to work off of the poA and only for enumerateInstances.
    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,  // KS_TEMP
        "Test putCache hasBinaryData %s MsgType %s",
               (from.hasBinaryData()? "true" : "false"),
            MessageTypeToString(response->getType()) ));
    //// End of diagnostic delete

    // if there is any binary data, reformat it to SCMO.  There are no
    // size counters for the binary data/
    if (from.hasBinaryData())
    {
        from.resolveBinaryToSCMO();
    }

    from.traceResponseData();
    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,  // KS_TEMP
        "Enter putCache,  isComplete= %s ResponseDataType= %u "
            " cache size= %u put size= %u clientClosed= %s",
        boolToString(providersComplete), to.getResponseDataContent(),
        to.size(), from.size(),
        boolToString(_clientClosed)));

    // If an operation has closed the enumerationContext
    // ignore any received responses until the providersComplete is received
    // and then remove the Context.
    if (_clientClosed)
    {
        // If providers are complete, do not queue this response. If providers
        // are not complete, the providers will continue to generate
        // responses but they are discarded above here.
        if (providersComplete)
        {
            _providersComplete = providersComplete;
            return false;
        }
    }
    else  // client not closed
    {
        // put the current response into the cache. Lock cache for this
        // operation

        _responseCacheMutex.lock();
        to.appendResponseData(from);

        // set providersComplete flag from flag in call parameter.
        _providersComplete = providersComplete;

        // test and set the high water mark for this cache.
        if (responseCacheSize() > _cacheHighWaterMark)
        {
            _cacheHighWaterMark = responseCacheSize();
        }
        _responseCacheMutex.unlock();

        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,  // EXP_PULL_TEMP
            "After putCache insert responseCacheSize %u. CIMResponseData"
                " size %u. signal CacheSizeConditon",
            responseCacheSize(), to.size() ));

        // Signal addition to the CIMResponseData cache. Do this
        // before waiting to be sure any cache size wait is retested.
        // May lose control at this point to CacheSizeCondition.
        //
////      signalCacheSizeCondition();

        // Review this for possible conflicts with context removal.
        // The _waiting is temporary and should not be required.

        // Wait for the cache size to drop below the limit requested here
        // before returning to caller. This blocks providers until wait
        // completed.
        if (!_providersComplete)
        {
            //// KS_TODO remove all these traces
////         PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,  // EXP_PULL_TEMP
////             "After putCache providers waitProviderLimitCondition Not "
////             "complete insert responseCacheSize %u. CIMResponseData "
////             "size %u."
////             " signal CacheSizeConditon responseCacheMaximumSize %u",
////             responseCacheSize(), to.size(), _responseCacheMaximumSize));
            // start timer to get time of wait for statistics.
            Uint64 startTime = TimeValue::getCurrentTime().toMicroseconds();
            waitProviderLimitCondition(_responseCacheMaximumSize);
            Uint64 interval =
                TimeValue::getCurrentTime().toMicroseconds() - startTime;
            PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,  // EXP_PULL_TEMP
                "After putCache providers wait end ProviderLimitCondition Not "
                "complete insert responseCacheSize %u. CIMResponseData size %u."
                " signal CacheSizeConditon responseCacheMaximumSize %u."
                " Wait %lu usec",
                responseCacheSize(), to.size(), _responseCacheMaximumSize,
                (unsigned long int)interval ));
        }
    }

    // Return true indicating that input added to cache and cache is still open
    PEG_METHOD_EXIT();
    return true;
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
    Returns true if data acquired from cache. Returns false if CIMException
    found (i.e. returned an error).
*/
Boolean EnumerationContext::getCache(
    Uint32 count,
    CIMResponseData& rtnData)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "EnumerationContext::getCache");

    PEGASUS_ASSERT(valid());   // KS_TEMP;

    // Move attributes from Cache to new CIMResponseData object
    // sets the attributes for propertyList, includeQualifiers,
    // classOrigin
    rtnData.setResponseAttributes(_responseCache);

    // if Error set, honor this.
    if (isErrorState())
    {
        return false;
    }
    // Wait for cache size to match count or providers to complete
    // set mutex only for the move objects. We don't want to mutex for
    // the wait period since we expect things to be put into the
    // cache during this period. Called from a client operation and the
    // operation will hang until this completes.
    //// waitCacheSizeCondition(count);

    // Note that there is an issue here in that we should prioritize returning
    // data before returning errors.  In this case, it may leave data in the
    // cache and return error. However, have issue if error is on last
    // provider return where it drops the error.
    // KS_TODO TBD
////  if (isErrorState())
////  {
////      return false;
////  }

    // Lock the cache for the move function
    AutoMutex autoMut(_responseCacheMutex);

    // Move the defined number of objects from the cache to the return object.
    rtnData.moveObjects(_responseCache, count);

    // KS_TODO_DIAG_DELETETHIS
    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
      "EnumerationContext::getCacheResponseData moveObjects expected=%u"
          " actual %u", count, rtnData.size()));

    // Signal the ProviderLimitCondition that the cache size may
    // have changed.
    signalProviderLimitCondition();

    PEGASUS_ASSERT(valid());   // KS_TEMP diagnostic.
    PEG_METHOD_EXIT();
    return true;
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
    if (_providersComplete || _clientClosed)
    {
        PEG_METHOD_EXIT();
        return;
    }

    // start timer to get time of wait for statistics.
    Uint64 startTime = TimeValue::getCurrentTime().toMicroseconds();

    // condition variable wait loop. waits on cache size or
    // providers complete
    _cacheTestCondMutex.lock();
    while (!_providersComplete && (responseCacheSize() < size))
    {
        _cacheTestCondition.wait(_cacheTestCondMutex);
    }
    _cacheTestCondMutex.unlock();

    Uint64 interval = TimeValue::getCurrentTime().toMicroseconds() - startTime;

    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
        "waitCacheSizeConditon return "
        "Request Size %u complete %s result %s time %lu usec",
        size,
        boolToString(_providersComplete),
        boolToString((!_providersComplete && (responseCacheSize()) < size)),
        (unsigned long int)interval ));

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

    _providerLimitConditionMutex.lock();

    Uint64 startTime = TimeValue::getCurrentTime().toMicroseconds();

    while (!_clientClosed && (responseCacheSize() > limit))
    {
        _providerLimitCondition.wait(_providerLimitConditionMutex);
    }

    Uint64 interval = TimeValue::getCurrentTime().toMicroseconds() - startTime;

    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,  // EXP_PULL_TEMP
       "waitproviderLimitCondition exit state: %s responseCacheSize: %u "
       "size: %u client closed: %s wait time: %lu us",
         boolToString((!_clientClosed && (responseCacheSize() < limit))),
         responseCacheSize(), limit, boolToString(_clientClosed),
         (long unsigned int)interval ));

    _providerLimitConditionMutex.unlock();
    PEG_METHOD_EXIT();
}

void EnumerationContext::signalProviderLimitCondition()
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "EnumerationContext::signalProviderLimitCondition");

    PEGASUS_ASSERT(valid());   // KS_TEMP

    AutoMutex autoMut(_providerLimitConditionMutex);

    _providerLimitCondition.signal();

    PEG_METHOD_EXIT();
}

Boolean EnumerationContext::incAndTestPullCounters(Boolean isZeroLength)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "EnumerationContext::incAndTestPullCounters");
    PEGASUS_ASSERT(valid());   // KS_TEMP
    _pullOperationCounter++;

    if (isZeroLength)
    {
        _consecutiveZeroLenMaxObjectRequestCounter++;
    }
    else
    {
        _consecutiveZeroLenMaxObjectRequestCounter = 0;
        return false;
    }

    return (_consecutiveZeroLenMaxObjectRequestCounter <=
             MAX_ZERO_PULL_OPERATIONS);
}

// set providers complete flag and signal the CacheSizeCondition.
// This could awaken any wait at the cacheWait.
void EnumerationContext::setProvidersComplete()
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "EnumerationContext::setProvidersComplete");

    PEGASUS_ASSERT(valid());   // KS_TEMP

    _providersComplete = true;

    // Signal CacheSize Condition flag that providers have completed.
    signalCacheSizeCondition();

    PEG_METHOD_EXIT();
}

// End of Request operation processing. Set the next enumeration state.
// If providers Complete and  cache = 0. We can now close the enumeration.
// If no more from providers and no more in cache, we set the client closed
//
// TODO Is the responseCacheSize sufficient or could something be stuck between
// providers complete, etc.
// Returns true if there is no more to process (providers are complete and
// responseCacheSize = 0). Returns false if providers not complete or
// there is data in the cache

Boolean EnumerationContext::setNextEnumerationState(Boolean errorFound)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "EnumerationContext::setNextEnumerationState");

    PEGASUS_ASSERT(valid());   // KS_TEMP

    // Return true if client closed because of error or all responses complete,
    // else set ProcessingState false and return false
    if ((ifProvidersComplete() && (responseCacheSize() == 0)) ||
        (errorFound && !_continueOnError))
    {
        setClientClosed();
        return true;
    }

    setProcessingState(false);

    return false;
}

void EnumerationContext::setClientClosed()
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "EnumerationContext::setClientClosed");

    PEGASUS_ASSERT(valid());   // KS_TEMP

    _clientClosed = true;

    if (!_providersComplete)
    {
        // Signal the limit on provider responses in case it is in wait
        signalProviderLimitCondition();
    }
}

/*
    Set the processing state. Processing is true if the Dispatcher is
    actively handling a request. The dispatcher sets processing = true
    at the start of processing and false at the completion of processing.
*/
Boolean EnumerationContext::setProcessingState(Boolean state)
{
    // Diagnostic to confirm we are changing state
    PEGASUS_ASSERT(_processing != state);

    _processing = state;
    if (_processing)
    {
        stopTimer();
    }
    else
    {
        startTimer();
    }

    return _processing;
}

PEGASUS_NAMESPACE_END
