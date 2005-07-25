//%2005////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// THE ABOVE COPYRIGHT NOTICE AND THIS PERMISSION NOTICE SHALL BE INCLUDED IN
// ALL COPIES OR SUBSTANTIAL PORTIONS OF THE SOFTWARE. THE SOFTWARE IS PROVIDED
// "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
// LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//==============================================================================
//
// Author: Heather Sterling (hsterl@us.ibm.com)
//
// Modified By: 
//
//%/////////////////////////////////////////////////////////////////////////////

#include "DynamicConsumer.h"
#include "DynamicConsumerFacade.h"

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/IPC.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/XmlReader.h>
#include <Pegasus/Common/XmlParser.h>
#include <Pegasus/Common/FileSystem.h>

PEGASUS_NAMESPACE_BEGIN
PEGASUS_USING_STD;


DynamicConsumer::DynamicConsumer(): Base(0)
{
}

DynamicConsumer::DynamicConsumer(const String& name): 
Base(0),
_module(0),
_eventqueue(true),
_name(name),
_initialized(false),
_dieNow(0),
_no_unload(0)
{
    _check_queue = new Semaphore(0);
    _shutdownSemaphore = new Semaphore(0);
    _listeningSemaphore = new Semaphore(0);
}

//ATTN: For migration from old listener -- do we want to support it?
DynamicConsumer::DynamicConsumer(const String & name,
                                 ConsumerModule* consumerModule,
                                 CIMIndicationConsumerProvider* consumerRef) :
Base(consumerRef),
_module(consumerModule),
_eventqueue(true),
_name(name),
_initialized(false),
_dieNow(0),
_no_unload(0)
{
    _check_queue = new Semaphore(0);
    _shutdownSemaphore = new Semaphore(0);
    _listeningSemaphore = new Semaphore(0);
}

DynamicConsumer::~DynamicConsumer(void)
{
    //delete any outstanding events
    IndicationDispatchEvent* event;
    while (_eventqueue.size())
    {
        event = _eventqueue.remove_first();
        delete event;
    }

    //delete semaphores
    if (_check_queue)
    {
        delete _check_queue;
    }

    if (_shutdownSemaphore)
    {
        delete _shutdownSemaphore;
    }

    if (_listeningSemaphore)
    {
        delete _listeningSemaphore;
    }
}

CIMIndicationConsumerProvider* DynamicConsumer::getConsumer()
{
    return(_consumer);
}

ConsumerModule* DynamicConsumer::getModule(void) const
{
    return(_module);
}

String DynamicConsumer::getName(void) const
{
    return(_name);
}

Boolean DynamicConsumer::isLoaded(void) const
{
    return(_module == 0 ? false : true);
}

Boolean DynamicConsumer::isInitialized(void) const
{
    return(_initialized);
}

/** Initializes the consumer.
 *  Caller assumes responsibility for catching exceptions thrown by this method.
 */ 
void DynamicConsumer::initialize()
{
    PEG_METHOD_ENTER(TRC_LISTENER, "DynamicConsumer::initialize");

    if (!_initialized)
    {
        // yield before a potentially lengthy operation.
        pegasus_yield();

        try
        {
            //there is no cimom handle in the listener, so pass null
            CIMOMHandle* handle = 0;
            DynamicConsumerFacade::initialize(*(handle));

            updateIdleTimer();
            _initialized = true;

            PEG_TRACE_STRING(TRC_LISTENER, Tracer::LEVEL3, "Successfully initialized consumer.");

        } catch (...)
        {
            PEG_TRACE_STRING(TRC_LISTENER, Tracer::LEVEL4,
                             "Exception caught in DynamicConsumerFacade::initialize for " +
                             _name);
            throw;
        }
    }

    PEG_METHOD_EXIT();
}

void DynamicConsumer::setShutdownSemaphore(Semaphore* shutdownSemaphore)
{
    PEG_METHOD_ENTER(TRC_LISTENER, "DynamicConsumer::setShutdownSemaphore");

    _shutdownSemaphore = shutdownSemaphore;

    PEG_METHOD_EXIT();
}

Semaphore* DynamicConsumer::getShutdownSemaphore()
{
    return _shutdownSemaphore;
}

void DynamicConsumer::sendShutdownSignal()
{
    PEG_METHOD_ENTER(TRC_LISTENER, "DynamicConsumer::sendShutdownSignal");

    _dieNow = true;
    _check_queue->signal();

    PEG_METHOD_EXIT();
}

void DynamicConsumer::terminate(void)
{
    PEG_METHOD_ENTER(TRC_LISTENER, "DynamicConsumer::terminate");

    if (_initialized)
    {
        // yield before a potentially lengthy operation.
        pegasus_yield();

        //terminate consumer
        try
        {
            DynamicConsumerFacade::terminate();

        } catch (...)
        {
            PEG_TRACE_STRING(TRC_LISTENER, Tracer::LEVEL4,
                             "Exception caught in DynamicConsumerFacade::Terminate for " +
                             _name);
            throw;
        }

        //update status
        _initialized = false;
        _dieNow = false;
    }

    PEG_METHOD_EXIT();
}

/** This method should be called after the physical consumer is loaded and before initialization.  
 */ 
void DynamicConsumer::set(ConsumerModule* consumerModule,
                          CIMIndicationConsumerProvider* consumerRef)
{
    PEG_METHOD_ENTER(TRC_LISTENER, "DynamicConsumer::set");

    if (_initialized)
    {
        throw Exception(MessageLoaderParms("DynListener.DynamicConsumer.CONSUMER_INVALID_STATE",
                                           "Error: The consumer is not in the correct state to perform the operation."));
    }

    _module = consumerModule;
    _consumer = consumerRef;

    PEG_METHOD_EXIT();
}

/** This method should be called after the consumer is terminated and the module is unloaded.  Note that we cannot test
 * for a loaded condition, since the _module reference here may still exist (if more than one consumer is using the module).
 * Simply test whether the consumer is initialized.  If it was terminated properly, initialized will be false and the _module
 * ref count will be decremented.
 */
void DynamicConsumer::reset()
{
    PEG_METHOD_ENTER(TRC_LISTENER, "DynamicConsumer::reset");

    if (_initialized)
    {
        throw Exception(MessageLoaderParms("DynListener.DynamicConsumer.CONSUMER_INVALID_STATE",
                                           "Error: The consumer is not in the correct state to perform the operation."));
    }

    _module = 0;    // do not delete it, that is taken care of in ConsumerModule itself 
    _consumer = 0;  // ATTN: attempting to delete this causes an exception -- why??

    Tracer::trace(__FILE__,__LINE__,TRC_LISTENER,Tracer::LEVEL4,
                  "Deleting %d outstanding requests for %s",
                  _eventqueue.size(),
                  (const char*)_name.getCString());

    //delete outstanding requests
    IndicationDispatchEvent* event = 0;
    for (Uint32 i = 0; i < _eventqueue.size(); i++)
    {
        event = _eventqueue.remove_first();
        delete event;
    }

    PEG_METHOD_EXIT();
}

void DynamicConsumer::enqueueEvent(IndicationDispatchEvent* event)
{
    PEG_METHOD_ENTER(TRC_LISTENER, "DynamicConsumer::enqueueEvent");

    if (!isLoaded())
    {
        PEG_TRACE_STRING(TRC_LISTENER, Tracer::LEVEL3, "Error: The consumer is not loaded and therefore cannot handle events.");
        return;
    }

    try
    {
        PEG_TRACE_STRING(TRC_LISTENER, Tracer::LEVEL4, "enqueueEvent before " + _name);
        //always insert new events at the head of the queue to be processed immediately
        //we put unsuccessful indications at the back of the queue to be processed after all new indications
        _eventqueue.insert_first(event);
        _check_queue->signal();

        PEG_TRACE_STRING(TRC_LISTENER, Tracer::LEVEL4, "enqueueEvent after " + _name);

    } catch (Exception& ex)
    {
        //ATTN: Log missed indication
        PEGASUS_STD(cout) << "Error enqueueing event" << ex.getMessage() << "\n";

    } catch (...)
    {
        //ATTN: Log missed indication
        PEGASUS_STD(cout) << "Unknown exception";
    }

    PEG_METHOD_EXIT();
}

void DynamicConsumer::getIdleTimer(struct timeval *tv)
{
    if (tv == 0)
    {
        return;
    }

    try
    {
        AutoMutex lock(_idleTimeMutex);
        memcpy(tv, &_idleTime, sizeof(struct timeval));
    } catch (...)
    {
        gettimeofday(tv, NULL);
    }
}

void DynamicConsumer::updateIdleTimer()
{
    try
    {
        AutoMutex lock(_idleTimeMutex);
        gettimeofday(&_idleTime, NULL);

    } catch (...)
    {
    }
}

Uint32 DynamicConsumer::getPendingIndications()
{
    return _eventqueue.size();
}

String DynamicConsumer::toString()
{
    PEG_METHOD_ENTER(TRC_LISTENER, "DynamicConsumer::toString");

    String buffer = String::EMPTY;
    if (_initialized)
    {
        buffer.append("Consumer " + _name + " is initialized.\n");
        buffer.append("Module name " + _module->getFileName() + "\n");
    }

    PEG_METHOD_EXIT();
    return buffer;
}

/** Returns true if the consumer has been inactive for longer than the idle period.
 */ 
Boolean DynamicConsumer::isIdle() 
{
    PEG_METHOD_ENTER(TRC_LISTENER, "DynamicConsumer::isIdle");

    if (!isLoaded())
    {
        PEG_TRACE_STRING(TRC_LISTENER, Tracer::LEVEL4, "Consumer is not loaded.");
        return false;
    }

    struct timeval now;
    gettimeofday(&now, NULL);

    struct timeval timeout = {0,0};
    getIdleTimer(&timeout);

    if (!_current_operations.value())
    {
        PEG_METHOD_EXIT();
        return true;
    }

    PEG_METHOD_EXIT();
    return false;
}

/** This method waits until the event thread is ready to accept incoming indications.  Otherwise, there is a miniscule chance that 
 * the first event will be enqueued before the consumer is waiting for it and the first indication after loading the consumer will be lost.
 */ 
void DynamicConsumer::waitForEventThread()
{
    _listeningSemaphore->wait();
}

/** This method is called when the consumer is initialized for the first time.
 *  It reads the outstanding requests from the dat file and enqueues them.
 * 
 * ATTN: This method will only get called when a consumer is initialized.  Therefore,
 * when the listener starts, the outstanding indications for this consumer will not get sent
 * UNTIL a new indication comes in.  This is not really an acceptable scenario.  Maybe the consumer
 * manager needs to check the .dat files upon startup and load if they are not empty.
 * 
 */ 
void DynamicConsumer::_loadOutstandingIndications(Array<CIMInstance> indications)
{
    PEG_METHOD_ENTER(TRC_LISTENER, "DynamicConsumer::_loadOutstandingIndications");

    //create dispatch events from the instances
    IndicationDispatchEvent* event;
    for (Uint32 i=0; i < indications.size(); i++)
    {
        event = new IndicationDispatchEvent(OperationContext(),  //ATTN: Do we need to store this?
                                            _name,
                                            indications[i]);
        _eventqueue.insert_last(event);
    }

    //signal the worker thread so it falls into the queue processing code
    if (_eventqueue.size())
    {
        _check_queue->signal();
    }

    PEG_METHOD_EXIT();
}

/** This method serializes the remaining indications in the queue. It should be called when the
 * consumer is shutting down.  Each time the consumer is loaded, these indications will be
 * reloaded into the queue.  Therefore, the file should be overwritten each time to eliminate
 * duplicating outstanding indications.
 * 
 * ATTN: Should we let another method delete the instances?
 */ 
Array<CIMInstance> DynamicConsumer::_retrieveOutstandingIndications()
{
    PEG_METHOD_ENTER(TRC_LISTENER, "DynamicConsumer::_retrieveOutstandingIndications");

    Array<CIMInstance> indications;
    IndicationDispatchEvent* temp = 0;

    try
    {
        _eventqueue.try_lock();
        temp = _eventqueue.next(temp);
        while (temp)
        {
            PEG_TRACE_STRING(TRC_LISTENER, Tracer::LEVEL4, "retrieving");
            indications.append(temp->getIndicationInstance());
            temp = _eventqueue.next(temp);
        }
        _eventqueue.unlock();

    } catch (...)
    {
        PEG_TRACE_STRING(TRC_LISTENER, Tracer::LEVEL4, "Unknown Exception");
    }

    PEG_METHOD_EXIT();
    return indications;
}


////////////////////////////////
// IndicationDispatchEvent
////////////////////////////////

IndicationDispatchEvent::IndicationDispatchEvent(OperationContext context,
                                                 String url,
                                                 CIMInstance instance) :
_context(context),
_url(url),
_instance(instance),
_retries(0),
_lastAttemptTime(CIMDateTime())
{
}

IndicationDispatchEvent::~IndicationDispatchEvent()
{
}

OperationContext IndicationDispatchEvent::getContext() const
{
    return _context;
}

String IndicationDispatchEvent::getURL() const
{
    return _url;
}

CIMInstance IndicationDispatchEvent::getIndicationInstance() const
{
    return _instance;
}

AtomicInt IndicationDispatchEvent::getRetries()
{
    return _retries;
}

void IndicationDispatchEvent::increaseRetries()
{
    PEG_TRACE_STRING(TRC_LISTENER, Tracer::LEVEL4, "Increasing retries\n");
    _retries++;
    _lastAttemptTime = CIMDateTime::getCurrentDateTime();
    PEG_TRACE_STRING(TRC_LISTENER, Tracer::LEVEL4, "Last attempt time " + _lastAttemptTime.toString());
}

CIMDateTime IndicationDispatchEvent::getLastAttemptTime()
{
    return _lastAttemptTime;
}

Boolean IndicationDispatchEvent::operator==(const IndicationDispatchEvent& event) const
{
    if (String::equal(this->_url, event._url) && 
        (this->_instance.identical(event._instance)))
    {
        return true;
    }
    return false;
}

PEGASUS_NAMESPACE_END

