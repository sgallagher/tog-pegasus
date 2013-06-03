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

#include "EnumerationTable.h"

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
#include <Pegasus/Server/EnumerationContext.h>

PEGASUS_USING_STD;
PEGASUS_NAMESPACE_BEGIN
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

// KS_TODO Remove this
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

        et->removeExpiredContexts();
        et->updateNextTimeout();

    }

    // reset the timeout value to indicate that the thread is quiting
    et->setTimerThreadIdle();

    return ThreadReturnType(0);
}
#endif

/*
    Create the Enumeration table and set the maximum/minimum values for
    input parameters that will require this.
*/
EnumerationTable::EnumerationTable(
    Uint32 defaultInteroperationTimeoutValue,
    Uint32 responseCacheDefaultMaximumSize)
    :
    _timeoutInterval(0),
    _nextTimeout(0),
    _enumContextCounter(1),
    _responseCacheDefaultMaximumSize(responseCacheDefaultMaximumSize),
    _cacheHighWaterMark(0),
    _pullOperationDefaultTimeout(defaultInteroperationTimeoutValue)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER, "EnumerationTable::EnumerationTable");

    PEG_METHOD_EXIT();
}

/* remove all contexts and delete them. Only used on system shutdown.
*/
EnumerationTable::~EnumerationTable()
{
    // remove any existing entries
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
        contentType);

    // set the pointer to the enumeration table into the context
    ec->_enumerationTable = this;

    // Set the maximum size for the response Cache from the default
    // value in the table
    ec->_responseCacheMaximumSize = _responseCacheDefaultMaximumSize;

    // Create new context name (guid + monolithic increasing counter)
    // KS_TODO - Do not need guid for each enum.  We should be able to
    // use this once and then just append the counter for the life of
    // the context table. KS_TODO - What issues might there be with
    // a context that is simply monolithicly increasing?? i.e. security.
    // KS_TODO - Modify this to use Pegasus StringConversion 
    // KS_TODO - Modify this whole thing to a Uint64 so we are not mapping
    // Strings.  Works since passed as PCDATA
    String ecn = Guid::getGuid();
    char t[24];
    _enumContextCounter++;
    sprintf(t, "-%u", (unsigned int)_enumContextCounter.get());
    ecn.append(t);

    // KS_TODO clean up the next two lines.  Don't need ecn at all.
    ec->_enumerationContextName = ecn;
    enumerationContextName = ecn;

    // insert new context into the table
    if(!ht.insert(ecn, ec))
    {
        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL1,
            "Error Creating Enumeration Context %s. System Failed",
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
    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
        "CreateContext ContextId= %s", (const char*)ecn.getCString()));
    ec->trace();    // KS_TEMP
    return ec;
}

Boolean EnumerationTable::remove(const String& enumerationContextName)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER, "EnumerationTable::remove");
    AutoMutex autoMut(tableLock);
    EnumerationContext* en = find(enumerationContextName);
    if (en == 0)
    {
        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,  // KS_TEMP
            "remove ERRORERROR en == 0 enName %s",
            (const char *)enumerationContextName.getCString()));
        cout << "EnumTable.remove where could not find en" << endl;
        return false;
    }
   
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
    tableValidate();

    // If it is valid and providers are complete, we can delete
    // the enumerationContext.  If providers not complete, only
    // completion of provider deliveries can initiate removal of
    // the enumeration context.
    if (en != 0 && en->_providersComplete && !en->_waiting)
    {
        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,  // KS_TEMP
            "EnumerationContext Remove. ContextId= %s",
            (const char *)en->getContextName().getCString() ));
        // Remove from enumerationTable.
        // KS_TODO_TBD - Should we remove this and depend completely on
        // the normal flow to clean it up.

        if (en->_cacheHighWaterMark > _cacheHighWaterMark)
        {
            _cacheHighWaterMark = en->_cacheHighWaterMark;
        }

        ///////ht.remove(en->getContextName());

        // KS_TODO - Do we need to clear the cache?

        // Delete the enumerationContext object
        // KS_TODO_TEMPORARILY DISABLE ACTUAL DELETE
        //delete en;

        // KS_TODO - Diagnostic
        tableValidate();

        PEG_METHOD_EXIT();
        return true;
    }
    else
    {
        if (en == 0)
        {
            PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,  // KS_TEMP
                "_remove ERRORERROR en == 0"));
        }
        else
        {
            PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,  // KS_TEMP
                "_remove ERRORERROR %s  _!providersComplete=%s  or waiting=%s",
                (const char *)en->getContextName().getCString(),
                (const char *)(en->_providersComplete? "true" : "false"),
                (const char*) (en->_waiting? "true" : "false" )       ));
        }
        cout << "remove ignored. " << (en->_waiting? "true" : "false") << endl;
        PEG_METHOD_EXIT();
        return false;
    }
}
Uint32 EnumerationTable::size()
{
    AutoMutex autoMut(tableLock);
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
// If context name found, return pointer to that context.  Otherwise
// return 0
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
    // Return not found indicator
    PEG_METHOD_EXIT();
    return 0;     
}

/** Test all table entries and remove the ones timed out.

*/
void EnumerationTable::removeExpiredContexts()
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,"EnumerationTable::removeExpiredContexts");

    AutoMutex autoMut(tableLock);
////  Uint64 currentTime = TimeValue::getCurrentTime().toMicroseconds();

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

// Validate every entry in the table.
void EnumerationTable::tableValidate()
{
    for (HT::Iterator i = ht.start(); i; i++)
    {
        EnumerationContext* en = i.value();
        if (!en->valid())
        {
            trace();
            PEGASUS_ASSERT(en->valid());             // diagnostic. KS_TEMP
        }
    }
}

void EnumerationTable::dispatchTimerThread(Uint32 interval)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER, "EnumerationTable::dispatchTimerThread");

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
