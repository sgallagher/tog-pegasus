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
#include <Pegasus/Common/NumericArg.h>
#include <Pegasus/Server/Linkage.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/CIMInstance.h>
#include <Pegasus/Common/CIMResponseData.h>
#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Common/Magic.h>
#include <Pegasus/Common/Condition.h>
#include <Pegasus/Common/AtomicInt.h>

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
       Create a new Enumeration Table defining the defaults for
       operation Timeout and response Cache Maximum Size.
     */
    EnumerationContextTable(Uint32 defaultInteroperationTimeoutValue,
        Uint32 reponseCacheDefaultMaximumSize);

    ~EnumerationContextTable();

    /**
        Create a new EnumerationContext object and return a pointer to
        the object.
        @param nameSpace - Namespace for this sequence.
        @param Uint32 value of operation timeout.
        @param continueOnError Boolean containing the continueOnError flag for
         this context.  (CURRENTLY MUST BE FALSE)
        @param pullOpenRequestType - Type for the Pull request message so
         tests can be made when pull received.  Used to prevent trying
         to pull paths when instances required, etc.
        @param contentType - Content type for the CIMResponseData cache
         container
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
       Return the number of enumeration context entries in the
       enumeration context table
       @return Uint32
     */
    Uint32 size();

    /**
       Remove any contexts that have expired inteoperation timers
    */
    void removeExpiredContexts();

    EnumerationContext* find(const String& enumerationContextName);

    /**
       Diagnostic to output info on all entries in table to trace log
    */
    void trace();

    /**
       Dispatch the Timer thread if it is not already dispatched.

       @param interval Uint32 interval defines the interval for this context
              in seconds.
    */
    void dispatchTimerThread(Uint32 interval);

    Uint32 getMinPullDefaultTimeout() const;

    /**
       Return true if the Timer Thread is idle (i.e. not running)
     */
    Boolean timerThreadIdle();

    /**
       Set the Timer Thread Idle (i.e. Not running)
    */
    void setTimerThreadIdle();

    /**
     * Update table timeout timer to next timeout
     */
    void updateNextTimeout();

    // KS_TODO think some of these are overkill
    Uint32 timoutInterval();

    // Test if the timeout thread next timeout has passed.
    Boolean isTimedOut() const;

    // diagnostic tests magic number in context to see if valid
    // Diagnostic tool
    Boolean valid();

    // KS_TEMP TODO This diagnostic should be removed
    void tableValidate();

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
    AtomicInt _enumContextCounter;

    Mutex tableLock;

    // Maximum number of objects allowed in the ResponseData cache in
    // the enumerationContextObject. This sets the maximum number of
    // of objects that will be placed in this cache before we start
    // sitting on responses.
    Uint32 _responseCacheDefaultMaximumSize;

    // systemwide highwater mark of number of objects in context cache
    // maintained from max of each context close/removal.
    Uint32 _cacheHighWaterMark;

    // default interoperationTimeout
    Uint32 _pullOperationDefaultTimeout;

    Magic<0x57D11474> _magic;
};

//
//  inline EnumerationContextTable functions
//
inline Boolean EnumerationContextTable::timerThreadIdle()
{
    return _nextTimeout == 0;
}

inline void EnumerationContextTable::setTimerThreadIdle()
{
    _nextTimeout = 0;
}

inline void EnumerationContextTable::updateNextTimeout()
{
    _nextTimeout += _timeoutInterval;
}

inline Uint32 EnumerationContextTable::timoutInterval()
{
    return _timeoutInterval;
}

PEGASUS_NAMESPACE_END

#endif /* PegasusEnumerationContextTable_h */
