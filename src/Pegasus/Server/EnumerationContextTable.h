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
        Note that this is a singleton.
        @param maxOpenContexts Uint32 sets limit on maximum number
            of contexts that may be open simultaneously. Note that this number
            may have effect on the Hashtable size which is also set in
            this header file.

        @param responseCacheMaximumSize Uint32 Maximum number of
            objects in response queue before it backs up to
            providers
    */
    EnumerationContextTable(Uint32 maxOpenContexts,
        Uint32 reponseCacheMaximumSize);

    ~EnumerationContextTable();

    /* Create a new EnumerationContext object and return a pointer
        to the object. This includes information
        required to process the pull and close operations for the enumeration
        sequence controlled by this context. The context instance will remain
        active for the life of the enumeration sequence.
        @param request CIMOpenOperationRequestMessage. Input
            Arguments from the request are saved in the EnumerationContext

        @param pullOpenRequestType - Message Type for the Pull request message
            so tests can be made when pull received.  Prevents trying
         to pull paths when instances required, etc.

        @param contentType - Content type for the CIMResponseData cache
         container. Defines the response object tyoes (objects, instances,
         CIMObjectPaths)
        @return EnumerationContext* pointer to the created context or
        NULL indicating that a new context could not be created. The only
        reasons a context could not be created are memory issues or
        this request exceeds the maximum open Enumeration context limit.
     */

    EnumerationContext* createContext(
        const CIMOpenOperationRequestMessage* request,
        MessageType pullRequestType,
        CIMResponseData::ResponseDataContent contentType);

    /** Release the enumerationContext entry from the
        EnumerationContext table and delete the entry. This function
        should only be called when the client is closed and
        providers complete.

        @param enumerationContextName context to remove
    */
    bool releaseContext(EnumerationContext* en);

    /** Return the number of enumeration context entries in the
       enumeration context table
       @return Uint32
    */
    Uint32 size();

    /** Remove any contexts that have expired inteoperation timers
     *  @return returns false if the table is empty.
    */
    bool removeExpiredContexts();

    /* Find the EnumerationContext defined by the input parameter.
       @param contextId String defining Id for the context to be located
       @return EnumerationContext* containing context or NULL if
       no context found.
    */
    EnumerationContext* find(const String& contextId);

    /** Dispatch the Timer thread if it is not already dispatched.

       @param interval Uint32 interval defines the interval for this context
              in seconds.
    */
    void dispatchTimerThread(Uint32 intervalSec);

    // Diagnostic tests magic number in context to see if valid
    bool valid();

    // KS_TEMP TODO REMOVE This diagnostic should be removed. It  validates
    // every entry in the table. This was just a diagnostic during development
    void tableValidate();

    // Diagnostic to output info on all entries in table to trace
      void trace();

    /**Clear the Context Table.  This is part of system shutdown
    */
    void removeContextTable();

    void displayStatistics(bool clear = false);

    void setRequestSizeStatistics(Uint32 requestSize)
    {
        _requestCount++;
        _requestedSize += requestSize;
    }
    Uint32 getTimeoutIntervalMsec();

    AtomicInt _timeoutThreadRunningFlag;
    AtomicInt _stopTimeoutThreadFlag;
    Semaphore _timeoutThreadWaitSemaphore;

    /*
        Sets the static parameter DefaultOperationTimeoutSec
        Called from config mgr if runtime parameter changes
    */
    static void setDefaultOperationTimeoutSec(Uint32 seconds);

private:

    // hide default assignment and copy constructor
    EnumerationContextTable(const EnumerationContextTable& x);

    EnumerationContextTable& operator=(const EnumerationContextTable&);
    // Scan interval timeout for timeout thread scans
    Uint32 _timeoutIntervalMsec;

    void _stopTimeoutThread();

    // Private remove.  This is function that actually executes the remove
    // Not protected by mutex.
    bool _removeContext(EnumerationContext* en);

    // Enumeration Context objects are maintained in the following
    // Pegasus hash table.
    typedef HashTable<String, EnumerationContext* , EqualFunc<String>,
        HashFunc<String> > EnumContextTableType;

    EnumContextTableType _enumContextTable;

    Mutex _tableLock;

    Thread _operationContextTimeoutThread;

    // Maximum number of objects allowed in the ResponseData cache in
    // any enumerationContext object. This sets the maximum number of
    // of instances or instance names that will be placed in this cache
    // before responses are not processed and back up to providers.
    Uint32 _responseCacheMaximumSize;

    // Default time interval allowed for interoperation timeout
    // when NULL is specified in the request.
    // This is static and set externally through
    // setDefaultOperationTimeoutSec
    static Uint32 _defaultOperationTimeoutSec;
#ifndef PEGASUS_INTEGERS_BOUNDARY_ALIGNED
    static Mutex _defaultOperationTimeoutSecMutex;
#endif

    // Systemwide highwater mark of number of objects in context cache
    Uint32 _cacheHighWaterMark;

    Uint32 _responseObjectCountHighWaterMark;



    // Count of enumerations Opened total since last statistics reset
    Uint64 _enumerationContextsOpened;

    // Count of enumerations TimedOut
    Uint32 _enumerationsTimedOut;

    // maximum number of Simultaneous Contexts open since last statistics
    // reset
    Uint32 _maxOpenContexts;

    // Maximum limit on number of open contexts. Tested on each create
    Uint32 _maxOpenContextsLimit;

    Uint64 _requestsPerEnumerationSequence;

    // Statistic to keep track of average size requested for all
    // operations.
    Uint64 _requestedSize;
    Uint64 _requestCount;
    Uint32 _getAvgRequestSize()
    {
        return (Uint32)((_requestCount != 0)?
            (_requestedSize / _requestCount)
            :
            0);
    }

    // magic number that acts as validator of enumerationContextTable
    Magic<0x57D11474> _magic;
};

//
//  inline EnumerationContextTable functions
//

inline Uint32 EnumerationContextTable::getTimeoutIntervalMsec()
{
    return _timeoutIntervalMsec;
}

inline bool EnumerationContextTable::valid()
{
    return _magic;
}

PEGASUS_NAMESPACE_END

#endif /* PegasusEnumerationContextTable_h */
