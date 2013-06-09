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
#include <Pegasus/Common/TimeValue.h>
#include <Pegasus/Common/List.h>
#include <Pegasus/General/Guid.h>
#include <Pegasus/Common/CIMResponseData.h>
#include <Pegasus/Common/Condition.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/General/Stopwatch.h>
#include <Pegasus/Common/Thread.h>
#include <Pegasus/Server/EnumerationContext.h>
#include <Pegasus/Common/StringConversion.h>

PEGASUS_NAMESPACE_BEGIN

PEGASUS_USING_STD;
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
    EnumerationContextTable* et =
        (EnumerationContextTable *)my_handle->get_parm();
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
EnumerationContextTable::EnumerationContextTable(
    Uint32 maxInteroperationTimeoutValue,
    Uint32 responseCacheMaximumSize)
    :
    _timeoutInterval(0),
    _nextTimeout(0),
    _enumContextCounter(1),
    _responseCacheMaximumSize(responseCacheMaximumSize),
    _cacheHighWaterMark(0),
    _maxOperationTimeout(maxInteroperationTimeoutValue)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "EnumerationContextTable::EnumerationContextTable");

    PEG_METHOD_EXIT();
}

/* remove all contexts and delete them. Only used on system shutdown.
*/
EnumerationContextTable::~EnumerationContextTable()
{
    // remove any existing entries
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "EnumerationContextTable::~EnumerationContextTable");
    for (HT::Iterator i = ht.start(); i; i++)
    {
        EnumerationContext* enumeration = i.value();

        // KS_TODO _ clean up threads before closing the table

        delete enumeration;
        ht.remove(i.key());
    }

    PEG_METHOD_EXIT();
}

EnumerationContext* EnumerationContextTable::createContext(
    const CIMNamespaceName& nameSpace,
    Uint32Arg&  operationTimeoutParam,
    Boolean continueOnError,
    MessageType pullRequestType,
    CIMResponseData::ResponseDataContent contentType,
    String& enumerationContextName)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,"EnumerationContextTable::createContext");

    AutoMutex autoMut(tableLock);
    // set the operation timeout to either the default or current
    // value
    Uint32 operationTimeout =
        (operationTimeoutParam.isNull())? _maxOperationTimeout
                                          :
                                          operationTimeoutParam.getValue();

    // Create the new context, Context name is
    // monolithically increasing counter. The interoperationTimeout is
    // defined by either the default or input value.
    EnumerationContext* enumCtxt = new EnumerationContext(nameSpace,
        operationTimeout,
        continueOnError,
        pullRequestType,
        contentType);

    // set the pointer to the enumeration table into the context
    enumCtxt->_enumerationContextTable = this;

    // Set the maximum size for the response Cache from the default
    // value in the table. This is for future where we could adjust the
    // size dynamically for each operation depending on resource utilization.
    enumCtxt->_responseCacheMaximumSize = _responseCacheMaximumSize;

    // Create new context name
    // KS_TODO - Modify this whole thing to a Uint64 so we are not mapping
    // Strings.  Works since passed as PCDATA
    //
    _enumContextCounter++;
    Uint32 size;
    char t[22];
    const char* x = Uint32ToString(t,_enumContextCounter.get(),size);

    // Put the name into the context and return value
    // KS_TODO We should not be duplicating the name.
    enumCtxt->_enumerationContextName = x;
    enumerationContextName = x;

    // insert new context into the table
    if(!ht.insert(enumerationContextName, enumCtxt))
    {
        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL1,
            "Error Creating Enumeration Context %s. System Failed",
            (const char*)enumerationContextName.getCString() ));
        enumCtxt = 0;
        PEGASUS_ASSERT(false);
    }
#ifndef PEGASUS_USE_PULL_TIMEOUT_THREAD
    if (isTimedOut())
    {
        removeExpiredContexts();
    }
#endif
    PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,
        "CreateContext ContextId= %s",
               (const char*)enumerationContextName.getCString()));
    enumCtxt->trace();    // KS_TEMP
    return enumCtxt;
}

Boolean EnumerationContextTable::remove(
    const String& enumerationContextName)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER, "EnumerationContextTable::remove");
    AutoMutex autoMut(tableLock);
    EnumerationContext* en = find(enumerationContextName);
    if (en == 0)
    {
        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,  // KS_TEMP
            "remove Context ERRORERROR en == 0 enName %s",
            (const char *)enumerationContextName.getCString()));
        cout << "EnumTable.remove where could not find en" << endl;
        return false;
    }

    PEG_METHOD_EXIT();
    return _removeContext(en);
}

// KS_TODO - Clean up fact that we repeat code above and here and have
// 2 parallel functions for deletion (pointer and name)
Boolean EnumerationContextTable::removeContext(EnumerationContext* en)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,"EnumerationContextTable::remove");
    AutoMutex autoMut(tableLock);

    PEG_METHOD_EXIT();
    return _removeContext(en);
}

// Private remove function with no lock protection. The tableLock must
// be set before this function is called to protect the table. This simply
// removes the context from the context table.
Boolean EnumerationContextTable::_removeContext(
    EnumerationContext* en, Boolean deleteContext)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,"EnumerationContextTable::_remove");

    PEGASUS_ASSERT(en->valid());            // KS_TEMP
    tableValidate();

    // If it is valid and providers are complete, we can delete
    // the enumerationContext.  If providers not complete, only
    // completion of provider deliveries can initiate removal of
    // the enumeration context.
    if (en != 0 && en->_clientClosed && en->_providersComplete && !en->_waiting)
    {
        PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,  // KS_TEMP
            "EnumerationContext Remove. ContextId= %s",
            (const char *)en->getContextName().getCString() ));
        // Remove from EnumerationContextTable.

        if (en->_cacheHighWaterMark > _cacheHighWaterMark)
        {
            _cacheHighWaterMark = en->_cacheHighWaterMark;
        }

        //// KS_TODO pointer or use name
        ht.remove(en->getContextName());

        // KS_TODO - Clear the cache?

        // Delete the enumerationContext object
        if (deleteContext)
        {
            delete en;
        }

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
                "_removeContext ERRORERROR en == 0"));
        }        // KS_TODO_TBD - Should we remove this and depend completely on
        // the normal flow to clean it up.
        else
        {
            PEG_TRACE((TRC_DISPATCHER, Tracer::LEVEL4,  // KS_TEMP
                "_removeContext ERRORERROR %s  _!providersComplete=%s"
                    "  or waiting=%s",
                (const char *)en->getContextName().getCString(),
                (const char *)(en->_providersComplete? "true" : "false"),
                (const char*) (en->_waiting? "true" : "false" )       ));
        }
        cout << "remove ignored. " << (en->_waiting? "true" : "false") << endl;
    }
    PEG_METHOD_EXIT();
    return false;
}

Uint32 EnumerationContextTable::size()
{
    AutoMutex autoMut(tableLock);
    return(ht.size());
}

Uint32 EnumerationContextTable::getMinPullDefaultTimeout() const
{
    return _maxOperationTimeout;
}

// KS_TODO - Clean this one up to one return.
// If context name found, return pointer to that context.  Otherwise
// return 0
EnumerationContext* EnumerationContextTable::find(
    const String& enumerationContextName)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER, "EnumerationContextTable::find");

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
void EnumerationContextTable::removeExpiredContexts()
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "EnumerationContextTable::removeExpiredContexts");

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
            if (en->isTimedOut(currentTime))
            {
//              cout << "Entry timed out " << en->getContextName()
//                  << " " << en->_interOperationTimer <<  endl;
                en->_interOperationTimer = 0;
                //// KS_TODO this should really just client close it.
                _removeContext(en);
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
void EnumerationContextTable::tableValidate()
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

void EnumerationContextTable::dispatchTimerThread(Uint32 interval)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
                     "EnumerationContextTable::dispatchTimerThread");

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
            // KS_TODO add to msg bundle.
            MessageLoaderParms parms(
                "Server.EnumerationContextTale.THREAD_ERROR",
                "Failed to start thread.");

            Logger::put_l(
                Logger::ERROR_LOG, System::CIMSERVER, Logger::SEVERE,
                parms);
            PEGASUS_ASSERT(false);
            PEG_METHOD_EXIT();
            return;
        }
#endif
    }
    PEG_METHOD_EXIT();
}

Boolean EnumerationContextTable::isTimedOut() const
{
    return (_nextTimeout < TimeValue::getCurrentTime().toMilliseconds() );
}

void EnumerationContextTable::trace()
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
Boolean EnumerationContextTable::valid()
{
    return _magic;
}

PEGASUS_NAMESPACE_END
