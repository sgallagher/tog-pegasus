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

#ifndef PegasusEnumerationContext_h
#define PegasusEnumerationContext_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Thread.h>
#include <Pegasus/Common/AutoPtr.h>
#include <Pegasus/Common/CIMName.h>
#include <Pegasus/Common/HashTable.h>
#include <Pegasus/Common/List.h>
#include <Pegasus/Common/NumericArg.h>
#include <Pegasus/Server/Linkage.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/CIMInstance.h>
#include <Pegasus/Common/CIMResponseData.h>
#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Server/CIMOperationRequestDispatcher.h>
#include <Pegasus/Common/Magic.h>
#include <Pegasus/Common/Condition.h>

PEGASUS_NAMESPACE_BEGIN

// Conditional compiles for EnumerationContext and table Classes
// If the following define is set, the enumeration context interoperation
// timeout is tested with a separate thread.  If not, it is simply tested
// with each new operation context creation eliminating the extra thread.
// This means, however, that a context that times out will not be cleaned
// up until another operation sequence begins and another context created

//#define PEGASUS_USE_PULL_TIMEOUT_THREAD

// End of conditional compile variables for EnumerationContext and Table
// Classes

#define LOCAL_MIN(a, b) ((a < b) ? a : b)

/******************************************************************************
**
**    Class that caches each outstanding enumeration. Contains
**    the parameters and current status of existing enumerations
**    that the server is processing.  Enumerations are those 
**    sequences of operations starting with an Open... operation
**    and proceeding through Pull... and possible closeEnumeration
**    operations.  The enumerationContext is the glue information
**    that ties the sequence of operations together.  This struc
**    defines the information that is maintained througout the
**    life of the sequence.
**    This structure also contains the queue of CIMOperationData objects
**    that is fed from provider returns and accessed by the operation requests
** 
******************************************************************************/

/*
    Keep total and average statistics on a 32bit integer. Used by
    enumeration to keep statistics on info in the enumeration.
*/
class PEGASUS_SERVER_LINKAGE uint32Stats
{
public:
    uint32Stats();
    void reset();
    // Add an entry to the statistics
    void add(Uint32 newInfo);
    // get the various pieces of intformation.
    Uint32 getHighWaterMark();
    Uint32 getAverage();
    Uint32 getCounter();
private:
    Uint32 _highWaterMark;
    Uint32 _counter;
    Uint32 _average;
    Uint64 _total;
    bool _overflow;
};

/*
    Controls the EnumerationContext for pull operations.  An instance
    of this class is the controller for the sequence of operations representing
    a pull sequence from open to completion. This instance provides the
    tools for maintaining interoperation information including the cache
    of objects to be returned, request information that is used by pull and
    close operations (timers, open request parameters, etc.), and the state
    of the pull operation sequence.
    nameSpace - Namespace in which this context defined. Used to confirm
    that pulls and closes are operating on the same namespace.
    operationTimeoutSec - Number of seconds for the interoperation
    timeout for this pull sequence.  Set with the open and
    used by the startTimer to set the timeout value between operations
    pOperationAggregate - pointer to the operation aggregate
    that this enumeration controls
    interoperationTimeout - microsecond timer that defines
    the next timeout.  If zero,there is no timeout in process
    magicNumber - Diagnostic to be sure that the structure
    is valid.
    active - If true, an operation is active on this context
    and any pull or close operation request will be refused.
*/
class EnumerationTable;
class PEGASUS_SERVER_LINKAGE EnumerationContext
{
public:
    // KS_TODO move from the individual binary state indicators to a
    // single state variable and state test/change function.
//  enum enumerationState { ENUM_OPENING = 1,
//      ENUM_ACTIVE = 2,
//      ENUM_TIMER = 3,
//      ENUM_CLOSING = 4,
//      ENUM_CLOSED = 5,
//      ENUM_ERROR = 6
//  };

    // KS_TODO want this to be protected and available only to its friend
    // class.  We do not want others to create contexts.
    EnumerationContext(const CIMNamespaceName& nameSpace,
        Uint32 interOperationTimeoutValue,
        Boolean continueOnError_,
        MessageType originatingOpenRequestType,
        CIMResponseData::ResponseDataContent contentType);
    
    ~EnumerationContext();

    /**
     * get the name of this enumeration context. The id is the key to 
     * access the enumeration table.
     * // KS TODO - This string must be immutable 
     * @return Context name String. 
     */
    String& getContextName()
    {
        return _enumerationContextName;
    }

    /**
     * get the Type of the CIMResponseData object in the enumeration 
     * context. 
     * @paran return Type 
     */
    CIMResponseData::ResponseDataContent getCIMResponseDataType()
    {
        return _responseCache.getResponseDataContent();
    }

    // Start the interOperation timer for this context
    void startTimer();

    // Stop the interOperation timer for this context
    void stopTimer();

    /**
        Test if this context timed out.
        @return true if interoperation timer timed out
    */
    Boolean isTimedOut();

    // diagnostic tests magic number in context to see if valid
    // Diagnostic tool
    Boolean valid();

    Boolean isClosed();

    Boolean isErrorState();

    Boolean setErrorState(CIMException cimException);

    // Sets next state for Enumeration context
    // NOTE: The following two are not used now.  We want to move from
    // the bunch of flags approach to changing state.
    //void setState(enumerationState newState);

    //Boolean isCurrentState(enumerationState state);

    /**
        Test the Message type for the open message saved in the
        context against the type parameter.  This provides a test
        that insures that Pull message types match the open type.
        Ex. PullPaths can only be used with OpenPath contexts
        @paran type MessageType for (for pull operation)
        @return Returns true if type match is correct. Returns false
        if not
     */
    Boolean isValidPullRequestType(MessageType type);

    // Test this context to determine if it is active
    Boolean isActive();

    Uint32 getCurrentObjectCount(Uint32Arg& x);
    // Diagnostic to display the current context into the
    // trace file
    void trace();

    /**
       Put the CIMResponseDatae from the response message into the 
       enumerationContext cache and if isComplete is true, set the 
       enumeration state to providersComplete. This function signals 
       the getCache function conditional variable. 
       KS_TODO - Why is poA necessary here?? 
     */
    void putCache(OperationAggregate*& poA,
                  CIMResponseMessage*& response, Boolean isComplete);

    /**
     * Get up to the number of objects defined by count from the 
     * CIMResponseData cache in the enumerationContext.  This function 
     * waits for a number of possible events as defined below and 
     * returns only when one of these events is true. 
     *  
     * REMEMBER: This function gives up control while waiting. 
     *  
     * Wait events: 
     *     a. the number of objects to match or exceed count
     *     b. the _providersComplete flag to be set.
     *     c. Future - The errorState to be set KS_TODO
     * @param - count - Uint32 count of max number of objects to return 
     * @param rtn CIMResponseData containing the objects returned 
     * @return Returns true except if the  errorState 
     * has been set. 
     */
    Boolean getCacheResponseData(Uint32 count, CIMResponseData& rtn);

    /** 
        Return reference to the CIMResponseData in the Enumeration Context
        that holds CIMResponseData for this context.  
    */ 
    CIMResponseData& getResponseData()
    {
        return _responseCache;
    }
    
    /**
       Returns count of objects in the EnumerationContext CIMResponseData 
       cache. 
       @return  Uint32 count of objects currently in the cache 
     */
    Uint32 cacheSize();

     /**
     * Set the ProvidersComplete state.  This should be set from provider 
     * responses when all responses processed. 
     */
    void setProvidersComplete();

    // Determine if the aggregation can be closed and if so close it.
    // Should be done before it is removed from the table. Closed means
    // providers complete and either we are not going to deliver more.
    // When closed it is considered unusable
    Boolean setClosed();

    /**
        Sets the active state. Setting active = true stops the
        timer.  Otherwise the interoperation timer is started
        @param state Boolean defines whether to set active or inactive.
        active means request being processed.
        @return - NOT USED TODAY
        KS_TODO - It would be clearer if we called this the processing
        state.
     */
    Boolean setActiveState(Boolean state);

    /**
        Test if the provider processing is complete.
        @return true if provider processing is complete.
     */
    Boolean ifProvidersComplete();
    
    /**    
        Test the enumeration for completeness.  Complete is when
        providers are complete and there is nothing in the cache.
        This test does not consider error state.
        If not complete, the timer is started.
        @param Boolean true if the enumeration is complete.
    */ 
    Boolean ifEnumerationComplete();

    // copy constructor
    EnumerationContext(const EnumerationContext& x);

    /*
        Increment the count of the number of pull operations executed
        for this context. This method also controls the zero
        @isZeroLength Boolean indicating if this operation is a request
        for zero objects which is used to count consecutive zero length
        pull operations.
        @return true if the count of consecutive zero length pull operations
        exceeds a predefined maximum.
    */
    Boolean incPullOperationCounter(Boolean isZeroLength);

    // Exception placed here in case of error. This is set by the operation
    // aggregate with any CIMException recieved from providers.  Note that
    // Only one is allowed.
    CIMException _cimException;
private:
    friend class EnumerationTable;

    Mutex contextLock;

    // Name of this EnumerationContext. Used to find the context by
    // pull and close operations.
    String _enumerationContextName;

    CIMNamespaceName _nameSpace;
    // interopertion timeout value in seconds.  From operation request
    // parameters.
    Uint32 _operationTimeoutSec;
    // ContinueONError request flag. Currently always FALSE since we do not
    // allow it in this version of Pegasus
    Boolean _continueOnError;

    // Timeout absolute time value for interoperation timeout.  If 0 indicates
    // timer not active.
    Uint64 _interOperationTimer;
    //enumerationState _enumerationState;

    // Request Type for pull operations for this pull sequence.
    // Set by open and all pulls must match this type.
    MessageType _pullRequestType;

    Magic<0x57D11474> _magic;

    // status flags.
    Boolean _closed;
    Boolean _providersComplete;
    Boolean _active;
    Boolean _error;

    // Object cache for this context.  All pull responses feed their
    // CIMResponseData into this cache using putCache(..) and all 
    // Open and Pull responses get data from the cache using getCache()
    // Simultaneous access to the cache is controlled with _cacheBlock mutex.
    Mutex _cacheBlock;
    CIMResponseData _responseCache;

    // Condition variable and mutex for the  cache size tests. This condition
    // variable is used by getCache(..) to force a wait until specific
    // conditions have been met by the EnumerationContext.
    Condition CacheTestCondition;
    Mutex CacheTestCondMutex;
    Uint32 conditionCounter;
    /**
        Tests the cache to determine if we are ready to send a response.
        The test is two parts, a) enough objects (i.e. GE size input parameter)
        or end-of-sequence set indicating that we have completed provider
        processing. 
    */ 
    void waitCacheSizeCondition(Uint32 size);
    
    /**
        Signal that the cache size condition may have been met. Normally
        called for every putcache and when providers complete set.
        Uses the CacheTestCondition Condition Variable and used inc
        conjunction with waitCacheSizeCondition
    */
    void signalCacheSizeCondition();

    // Condition variable and mutex for the provider wait
    // condition.  This is a hold on returns from putcache when cache
    // reaches a defined limit that is cleared when the cache level drops
    // to below the defined level (See functions waitProviderLimitCondition and
    // signalProviderLimitCondition)
    Condition providerLimitCondition;
    Mutex providerLimitConditionMutex;

    // Functions for using providerLimitCondition.  Wait executes a wait
    // until the conditions of the condition variable are met.
    // @param size defines the limit at which wait occurs.
    void waitProviderLimitCondition(Uint32 limit);

    // signalProviderLimitCondition signals the condition variable that
    // it should test the wait conditions.
    void signalProviderLimitCondition();

    // statistics for the context
    // Count Of pull operations executed in this context.  Used for statistics
    Uint32 _pullOperationCounter;

    // Counter of consecutive zeroLength requests. Used to limit client
    // stalling by executing excessive number of zero return pull opreations.
    Uint32 _zeroRtnPullOperationCounter;

    Uint64 _startTime;
    // Max number of objects in the cache.
    Uint32 _cacheHighWaterMark;
    
    Uint32 _cacheProviderWaitHighWaterMark;
    Uint32 _cacheProviderWaitAvg;
    Uint32 _cacheProviderWaitCount;
    
    Uint32 _cacheGeyHighWaterMark;
    Uint32 _cacheGetAvg;
    Uint32 _cacheGetCount;

    // Pointer to enumeration table.
    EnumerationTable* _enumerationTable;
};


/****************************************************************************
**
**   EnumerationTable Class

**  This Class defines a table contains all active EnumerationContext
**  elements (structs). It is organized as a list and includes
**  methods to create new contexts, find a context by name, manage
**  the information in a context amd remove a context.
**  Each context element is represented by the EnumerationContext
**  structure.
**  This class is expected to be used only within the Operation
**  Dispatcher.
*****************************************************************************/

class PEGASUS_SERVER_LINKAGE EnumerationTable
{
public:

    EnumerationTable(Uint32 defaultInteroperationTimeoutValue);

    ~EnumerationTable();

    /**
      Create a new EnumerationContext object and return a pointer to
      the object. 
     @param nameSpace - Namespace for this sequence.
     @param Uint32 value of operation timeout.
     @param continueOnError Boolean containing the continueOnError flag for 
          this context.  (CURRENT MUST BE FALSE) 
     @param pullOpenRequestType - Type for the Pull request message so 
         tests can be made when pull received.  Used to prevent trying
         to pullpaths when instances required, etc.
     @param contentType - Content type for the CIMResponseData cache container
     @param enumerationContextName  - Output parameter containing the 
         string which is the enumerationContext name that is used with
         the find command and as the context in the operations
 
     @return EnumerationContext* 
     */
    EnumerationContext* createContext(
        const CIMNamespaceName& nameSpace,
        Uint32Arg&  operationTimeOutParam,
        Boolean continueOnError,
        MessageType pullRequestType,
        CIMResponseData::ResponseDataContent contentType,
        String& enumerationContextName);

    /** Find and remove the enumeration context from the table
    */
    Boolean remove(const String& enumerationContextName);

    Boolean remove(EnumerationContext* en);

    /**
     * Returns the number of enumeration context entries in the 
     * enumeration context table 
     * 
     * @return Uint32 
     */
    Uint32 size() const;

    void clear();

    /**
     * Remove any contexts that have expired inteoperation timers
     */
    void removeExpiredContexts();

    EnumerationContext* find(const String& enumerationContextName);

    /**
     * Diagnostic to output info on all entries in table to trace log
     */
    void trace();

    /**
     * Dispatch the Timer thread if it is not already dispatched.  

     * @param interval Uint32 interval defines the interval for this context 
     *        in seconds. 
     */
    void dispatchTimerThread(Uint32 interval);

    Uint32 getMinPullDefaultTimeout() const;

    /**
     * Return true if the Timer Thread is idle (i.e. not running
     */
    Boolean timerThreadIdle()
    {

        return _nextTimeout == 0;
    }

    /**
     * Set the Timer Thread Idle (i.e. Not running)
     */
    void setTimerThreadIdle()
    {
        _nextTimeout = 0;
    }

    void updateNextTimeout()
    {
        _nextTimeout += _timeoutInterval;
    }

    Uint32 timoutInterval()
    {
        return _timeoutInterval;
    }

    // Test if the timeout thread next timeout has passed.
    Boolean isTimedOut() const;


    // diagnostic tests magic number in context to see if valid
    // Diagnostic tool
    Boolean valid();
protected:
    // timers for timer thread in seconds
    // Current minimum timeout time for active pull sequences
    Uint32 _timeoutInterval;
    // next time the test for timed out pull sequences will be activated.
    // KS_TODO - This is incorrect since it will be the minimum for all
    // time.  How can we keep minimum for the active ones?
    Uint64 _nextTimeout;
private:
    // Enumeration Context objects are maintained in the following
    // Pegasus hash table
    typedef HashTable<String,
        EnumerationContext* ,
        EqualFunc<String>,
        HashFunc<String> > HT;
    HT ht;

    // Private remove.  This is function that actually executes the remove
    // Not protected by mutex.
    Boolean _remove(EnumerationContext* en);

    // monolithic increasing counter forms part of context id string
    Uint64 _enumContextCounter;

    Mutex tableLock;

    // systemwide highwater mark of number of objects in context cache
    // maintained from max of each context close/removal.
    Uint32 _cacheHighWaterMark;

    // default interoperationTimeout
    Uint32 _pullOperationDefaultTimeout;

    Magic<0x57D11474> _magic;
};

//KS_PULL_END

PEGASUS_NAMESPACE_END

#endif /* PegasusEnumerationContext_h */
