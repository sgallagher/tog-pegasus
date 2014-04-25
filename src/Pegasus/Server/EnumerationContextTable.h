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

#ifndef PegasusEnumerationContextTable_h
#define PegasusEnumerationContextTable_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Thread.h>
#include <Pegasus/Common/AutoPtr.h>
#include <Pegasus/Common/CIMName.h>
#include <Pegasus/Common/HashTable.h>
#include <Pegasus/Common/List.h>
#include <Pegasus/Common/UintArgs.h>
#include <Pegasus/Server/Linkage.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/CIMInstance.h>
#include <Pegasus/Common/CIMResponseData.h>
#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Common/Magic.h>
#include <Pegasus/Common/Condition.h>
#include <Pegasus/Common/AtomicInt.h>
#include <Pegasus/Common/TimeValue.h>

PEGASUS_NAMESPACE_BEGIN
/****************************************************************************
**
**   EnumerationContextTable Class
**
**  This Class defines a table that contains all active EnumerationContext
**  class objects. It is organized as a hash table and includes
**  methods to create new contexts, find a context by name, manage
**  the information in a context and remove a context.
**  Each context object is represented by the EnumerationContext
**  class.
**  This class is expected to be used only with the Operation
**  Dispatcher.
**  This class includes functions to time out inactive enumeration context
**  entries in the table
*****************************************************************************/

class EnumerationContext;

class PEGASUS_SERVER_LINKAGE EnumerationContextTable
{
public:

    /**
        Create a new Enumeration Table Object and clear parameters.
        Note that this is really a singleton.
    */
    EnumerationContextTable();

    /**
       Set the default parameter values for Pull Operationsl.

       @param  defaultInteroperationTimeoutValue Uint32 default time
       between operations of a pull sequence if the request has
       this argument = NULL.

       @param responseCacheMaximumSize Uint32 Maximum number of
       objects in response queue before it backs up to providers
     */
    void setContextDefaultParameters(
        Uint32 defaultInteroperationTimeoutValue,
        Uint32 reponseCacheMaximumSize);

    ~EnumerationContextTable();

    /** Create a new EnumerationContext object and return a pointer
        to the object. This includes information
        required to process the pull and close operations for the enumeration
        sequence controlled by this context. The context instance will remain
        active for the life of the enumeration sequence.
        @param nameSpace - Namespace for this sequence.
        @param operationTimeOutParam Uint32Arg value of operation
        timeout in seconds.
        @param continueOnError Boolean containing the continueOnError flag for
         this context.  (CURRENTLY MUST BE FALSE)
        @param pullOpenRequestType - Type for the Pull request message so
         tests can be made when pull received.  Prevents trying
         to pull paths when instances required, etc.
        @param contentType - Content type for the CIMResponseData cache
         container
        @return EnumerationContext*
     */
    EnumerationContext* createContext(
        const CIMNamespaceName& nameSpace,
        Uint32Arg&  operationTimeOutParam,
        Boolean continueOnError,
        MessageType pullRequestType,
        CIMResponseData::ResponseDataContent contentType);

    /** Release the enumerationContext entry from the
        EnumerationContext table and delete the entry. This function
        should only be called when the client is closed and
        providers complete.

        @param enumerationContextName context to remove
    */
    void releaseContext(EnumerationContext* en);

    /** Return the number of enumeration context entries in the
       enumeration context table
       @return Uint32
    */
    Uint32 size();

    /** Remove any contexts that have expired inteoperation timers
    */
    void removeExpiredContexts();

    EnumerationContext* find(const String& enumerationContextName);

    /** Dispatch the Timer thread if it is not already dispatched.

       @param interval Uint32 interval defines the interval for this context
              in seconds.
    */
    void dispatchTimerThread(Uint32 interval);

    /** Return true if the Timer Thread is idle (i.e. not running)
    */
    Boolean timerThreadIdle() const;

    /** Set the Timer Thread Idle (i.e. Not running)
    */
    void setTimerThreadIdle();

    /** Update table timeout timer to next timeout
    */
    void updateNextTimeout();

    bool stopThread();

    // KS_TODO think some of these are overkill
    Uint32 timoutInterval() const;

    /** isNextScanTime tests if the next defined timeout for the
        context monitor is less than the current time meaning that
        the timer has timed out.
        @return Boolean returns true if timed out.
     */
    Boolean isNextScanTime() const;

    // Diagnostic tests magic number in context to see if valid
    // This is a Diagnostic tool and is enabled only when PEGASUS_DEBUG
    // set
    Boolean valid();

    // KS_TEMP TODO REMOVE This diagnostic should be removed. It  validates
    // every entry in the table.
    void tableValidate();

    /** Diagnostic to output info on all entries in table to trace log
    */
    void trace();

    /**Clear the Context Table.  This is part of system shutdown
    */
    void removeContextTable();

    void displayStatistics(Boolean clear = false);

    void setRequestSizeStatistics(Uint32 requestSize)
    {
        _requestCount++;
        _requestedSize += requestSize;
    }
protected:

    // Timers for timer thread in milliseconds
    // Current minimum timeout time for active pull sequences
    Uint32 _timeoutInterval;

    // next time the test for timed out pull sequences will be activated.
    // Absolute time in milliseconds
    // KS_TODO - This is incorrect since it will be the minimum for all
    // time.  How can we keep minimum for the active ones?
    Uint64 _nextTimeout;

private:
    // hide default assignment and copy constructor
    EnumerationContextTable(const EnumerationContextTable& x);
    EnumerationContextTable& operator=(const EnumerationContextTable&);

    // Enumeration Context objects are maintained in the following
    // Pegasus hash table
    typedef HashTable<String,
        EnumerationContext* ,
        EqualFunc<String>,
        HashFunc<String> > HT;
    HT ht;

    // Private remove.  This is function that actually executes the remove
    // Not protected by mutex.
    Boolean _removeContext(
        EnumerationContext* en,
        Boolean deleteContext = false);

    // Lock on EnumerationContextTable
    Mutex tableLock;

    // Maximum number of objects allowed in the ResponseData cache in
    // any enumerationContext object. This sets the maximum number of
    // of instances or instance names that will be placed in this cache
    // before responses are not processed and back up to providers.
    Uint32 _responseCacheMaximumSize;

    // Systemwide highwater mark of number of objects in context cache
    Uint32 _cacheHighWaterMark;

    // Default time interval allowed for interoperation timeout in seconds
    // when NULL is specified in the request.
    Uint32 _defaultOperationTimeout;

    // Count of enumerations Opened total
    Uint64 _enumerationContextsOpened;

    // Count of enumerations TimedOut
    Uint32 _enumerationsTimedOut;

    // maximum number of Simultaneous Contexts open.
    Uint32 _maxSimultaneousContexts;

    Uint64 _requestsPerEnumerationSequence;

    // Statistic to keep track of average size requested for all
    // operations.
    Uint64 _requestedSize;
    Uint64 _requestCount;
    Uint32 _getAvgRequestSize()
    {
        return ((_requestCount != 0)? (_requestedSize / _requestCount) : 0);
    }

    // magic number that acts as validator of enumerationContextTable
    Magic<0x57D11474> _magic;
};

//
//  inline EnumerationContextTable functions
//
inline Boolean EnumerationContextTable::timerThreadIdle() const
{
    return (_nextTimeout == 0);
}

inline void EnumerationContextTable::setTimerThreadIdle()
{
    _nextTimeout = 0;
}

inline void EnumerationContextTable::updateNextTimeout()
{
    _nextTimeout += _timeoutInterval;
}
inline bool EnumerationContextTable::stopThread()
{
    return (_nextTimeout == 0);
}

inline Uint32 EnumerationContextTable::timoutInterval() const
{
    return _timeoutInterval;
}

inline Boolean EnumerationContextTable::isNextScanTime() const
{
    return (_nextTimeout < TimeValue::getCurrentTime().toMilliseconds() );
}

inline Boolean EnumerationContextTable::valid()
{
    return _magic;
}

PEGASUS_NAMESPACE_END

#endif /* PegasusEnumerationContextTable_h */
