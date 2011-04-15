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
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Time.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/StringConversion.h>
#include <Pegasus/Config/ConfigManager.h>
#include <Pegasus/Provider/CIMOMHandle.h>
#include "DestinationQueue.h"

PEGASUS_NAMESPACE_BEGIN

// Initialize with default values.
Uint16 DestinationQueue::_maxDeliveryRetryAttempts = 3;
Uint64 DestinationQueue::_minDeliveryRetryIntervalUsec = 20 * 1000000;
Uint64 DestinationQueue::_sequenceIdentifierLifetimeUsec = 600 * 1000000;
String DestinationQueue::_indicationServiceName = "PG:IndicationService";
String DestinationQueue::_objectManagerName = "Pegasus";

const char* DestinationQueue::_indDiscardedReasons[] = {
    "Listener not active",
    "Subscription not active",
    "DestinationQueue full",
    "SequenceIdentifierLifetTime expired",
    "DeliveryRetryAttempts exceeded",
    "CIMServer shutdown"
};

Uint64 DestinationQueue::_serverStartupTimeUsec
    = System::getCurrentTimeUsec();

Boolean DestinationQueue::_initialized = false;
static Mutex _intializeMutex;

CIMInstance DestinationQueue::_getInstance(const CIMName &className)
{
    CIMOMHandle cimomHandle;
    Array<CIMInstance> instances =
        cimomHandle.enumerateInstances(
            OperationContext(),
            PEGASUS_NAMESPACENAME_INTEROP,
            className,
            true,
            false,
            false,
            false,
            CIMPropertyList());

    return instances[0];
}

void DestinationQueue::_initIndicationServiceProperties()
{
    PEG_METHOD_ENTER(TRC_IND_HANDLER,
        "DestinationQueue::_initIndicationServiceProperties");

    CIMInstance instance =
        _getInstance(PEGASUS_CLASSNAME_CIM_INDICATIONSERVICE);

    instance.getProperty(
        instance.findProperty(
            _PROPERTY_DELIVERY_RETRYATTEMPTS)).getValue().get(
                _maxDeliveryRetryAttempts);

    instance.getProperty(
        instance.findProperty(
            PEGASUS_PROPERTYNAME_NAME)).getValue().get(
               _indicationServiceName);

    CIMValue value = instance.getProperty(
        instance.findProperty(
            _PROPERTY_DELIVERY_RETRYINTERVAL)).getValue();

    if (value.getType() == CIMTYPE_UINT32)
    {
        Uint32 tval;
        value.get(tval);
        _minDeliveryRetryIntervalUsec = Uint64(tval) * 1000000;
    }
    else
    {
        value.get(_minDeliveryRetryIntervalUsec);
        _minDeliveryRetryIntervalUsec*= 1000000;
    }
       // See DSP 1054 ver 1.1.0 Sec 7.10
    _sequenceIdentifierLifetimeUsec = _maxDeliveryRetryAttempts *
        _minDeliveryRetryIntervalUsec * 10;
    PEG_METHOD_EXIT();
}

void DestinationQueue::_initObjectManagerProperties()
{
    PEG_METHOD_ENTER(TRC_IND_HANDLER,
        "DestinationQueue::_initObjectManagerProperties");

    CIMInstance instance =
       _getInstance(PEGASUS_CLASSNAME_PG_OBJECTMANAGER);

    instance.getProperty(
        instance.findProperty(
            PEGASUS_PROPERTYNAME_NAME)).getValue().get(
                _objectManagerName);
    PEG_METHOD_EXIT();
}

DestinationQueue::DestinationQueue(
    const CIMInstance &handler):_handler(handler)
{
    PEG_METHOD_ENTER(TRC_IND_HANDLER,
        "DestinationQueue::DestinationQueue");

    if (!_initialized)
    {
        AutoMutex mtx(_intializeMutex);
        if (!_initialized)
        {
            try
            {
                PEG_TRACE_CSTRING(TRC_IND_HANDLER, Tracer::LEVEL4,
                    "Initializaing the Destination Queue");
                _initIndicationServiceProperties();
                _initObjectManagerProperties();
            }
            catch(const Exception &e)
            {
                PEG_TRACE((TRC_IND_HANDLER,Tracer::LEVEL1,
                    "Exception %s caught while initializing the "
                        "DestinationQueue, using default values.",
                    (const char*)e.getMessage().getCString()));
            }
            catch(...)
            {
                PEG_TRACE_CSTRING(TRC_IND_HANDLER,Tracer::LEVEL1,
                    "Unknown exception caught while initializing the "
                        "DestinationQueue, using default values.");
            }
            _initialized = true;
        }
    }

    // Build the sequence context
    _sequenceContext = _indicationServiceName;
    _sequenceContext.append("-");
    _sequenceContext.append(_objectManagerName);
    _sequenceContext.append("-");

    Uint32 len = 0;
    char buffer[22];
    const char* ptr = Uint64ToString(buffer, _serverStartupTimeUsec,len);
    _sequenceContext.append(String(ptr, len));
    _sequenceContext.append("-");

    Uint32 idx = handler.findProperty(
        PEGASUS_PROPERTYNAME_LSTNRDST_CREATIONTIME);

    if (idx != PEG_NOT_FOUND)
    {
        Uint64 tvalue;
        handler.getProperty(idx).getValue().get(tvalue);
        Uint32 llen = 0;
        char lbuffer[22];
        const char* lptr = Uint64ToString(lbuffer, tvalue, llen);
        _sequenceContext.append(String(lptr, llen));
    }
    else
    {
        _sequenceContext.append(String(ptr, len));
    }
    _lastDeliveryRetryStatus = FAIL;

    _sequenceNumber = 0;
    _queueFullDroppedIndications = 0;
    _lifetimeExpiredIndications = 0;
    _retryAttemptsExceededIndications = 0;
    _subscriptionDeleteDroppedIndications = 0;
    _calcMaxQueueSize = true;
    _lastSuccessfulDeliveryTimeUsec = 0;
    _maxIndicationDeliveryQueueSize = 2400;

    _queueCreationTimeUsec = System::getCurrentTimeUsec();
    PEG_METHOD_EXIT();
}

DestinationQueue::~DestinationQueue()
{
    PEG_METHOD_ENTER(TRC_IND_HANDLER,
        "DestinationQueue::~DestinationQueue");

    if (_queue.size() || _lastDeliveryRetryStatus == PENDING)
    {
        _cleanup(LISTENER_NOT_ACTIVE);
    }

    PEG_METHOD_EXIT();
}

Sint64 DestinationQueue::getSequenceNumber()
{
    AutoMutex mtx(_queueMutex);

    // Determine max queue size, See PEP 324 for the algorithm
    if (_calcMaxQueueSize)
    {
        if ((System::getCurrentTimeUsec() - _queueCreationTimeUsec)
            >= _sequenceIdentifierLifetimeUsec)
        {
            // (10 * DeliveryRetryInterval * DeliveryRetryAttempts) /
            // (Number of indications arrived over
            //   sequence-identifier-lifetime.)

            _maxIndicationDeliveryQueueSize = _sequenceNumber;

            if (_maxIndicationDeliveryQueueSize < 200)
            {
                _maxIndicationDeliveryQueueSize = 200;
            }
            else if (_maxIndicationDeliveryQueueSize > 2400)
            {
                _maxIndicationDeliveryQueueSize = 2400;
            }
            _calcMaxQueueSize = false;
        }
    }

    Sint64 nextSequenceNumber = _sequenceNumber++;

    if (_sequenceNumber < 0)
    {
        _sequenceNumber = 0;
    }

    return nextSequenceNumber;
}

String DestinationQueue::_getSequenceContext(
    const CIMInstance &indication)
{
    String sequenceContext;

    indication.getProperty(
        indication.findProperty(
            _PROPERTY_SEQUENCECONTEXT)).getValue().get(sequenceContext);

    return sequenceContext;
}

Sint64 DestinationQueue::_getSequenceNumber(
    const CIMInstance &indication)
{
    Sint64 sequenceNumber;

    indication.getProperty(
        indication.findProperty(
            _PROPERTY_SEQUENCENUMBER)).getValue().get(sequenceNumber);

    return sequenceNumber;
}

void DestinationQueue::_traceDiscardedIndication(
    Uint32 reasonCode, const CIMInstance &indication)
{

    PEGASUS_ASSERT(reasonCode <
        sizeof(_indDiscardedReasons)/sizeof(const char*));

    PEG_TRACE((TRC_IND_HANDLER, Tracer::LEVEL3,
        "%s, Indication with SequenceContext %s and SequenceNumber %"
            PEGASUS_64BIT_CONVERSION_WIDTH "d is discarded",
        _indDiscardedReasons[reasonCode],
        (const char*)_getSequenceContext(indication).getCString(),
        _getSequenceNumber(indication)));
}

void DestinationQueue::enqueue(CIMHandleIndicationRequestMessage *message)
{
    PEG_METHOD_ENTER(TRC_IND_HANDLER,
        "DestinationQueue::enqueue");

    Uint32 idx;
    CIMProperty prop;
    CIMInstance &indication = message->indicationInstance;

    if ((idx = indication.findProperty(_PROPERTY_SEQUENCECONTEXT))
        != PEG_NOT_FOUND)
    {
        prop = indication.getProperty(idx);
        prop.setValue(getSequenceContext());
        indication.removeProperty(idx);
    }
    else
    {
        prop = CIMProperty(
            _PROPERTY_SEQUENCECONTEXT,
            getSequenceContext());
    }
    indication.addProperty(prop);

    AutoMutex mtx(_queueMutex);
    Sint64 sequenceNumber = getSequenceNumber();
    if ((idx = indication.findProperty(_PROPERTY_SEQUENCENUMBER))
        != PEG_NOT_FOUND)
    {
        prop = indication.getProperty(idx);
        prop.setValue(sequenceNumber);
        indication.removeProperty(idx);
    }
    else
    {
        prop = CIMProperty(
            _PROPERTY_SEQUENCENUMBER,
            sequenceNumber);
    }
    indication.addProperty(prop);

    IndicationInfo *info = new IndicationInfo(
        message->indicationInstance,
        message->subscriptionInstance,
        message->operationContext,
        message->nameSpace.getString(),
        this);
    _queue.insert_back(info);

    info->lastDeliveryRetryTimeUsec = 0;
    info->arrivalTimeUsec = System::getCurrentTimeUsec();

    if (_queue.size() > _maxIndicationDeliveryQueueSize)
    {
        _queueFullDroppedIndications++;
        IndicationInfo *temp = _queue.remove_front();
        _traceDiscardedIndication(
            DESTINATIONQUEUE_FULL,
            temp->indication);
        delete temp;
    }

    PEG_METHOD_EXIT();
}

void DestinationQueue::updateDeliveryRetrySuccess(IndicationInfo *info)
{
    PEG_METHOD_ENTER(TRC_IND_HANDLER,
        "DestinationQueue::updateDeliveryRetrySuccess");

    AutoMutex mtx(_queueMutex);
    PEGASUS_ASSERT(_lastDeliveryRetryStatus == PENDING);
    _lastSuccessfulDeliveryTimeUsec = System::getCurrentTimeUsec();
    _lastDeliveryRetryStatus = SUCCESS;

    PEG_TRACE((TRC_IND_HANDLER, Tracer::LEVEL4,
        "Indication with SequenceContext %s and SequenceNumber %"
            PEGASUS_64BIT_CONVERSION_WIDTH "d is successfully delivered",
        (const char*)_getSequenceContext(info->indication).getCString(),
        _getSequenceNumber(info->indication)));

    delete info;

    PEG_METHOD_EXIT();
}

void DestinationQueue::updateDeliveryRetryFailure(
    IndicationInfo *info,
    const CIMException &e)
{
    PEG_METHOD_ENTER(TRC_IND_HANDLER,
        "DestinationQueue::updateDeliveryRetryFailure");

    AutoMutex mtx(_queueMutex);

    // Log a warning message on first delivery attempt failure.
    if (info->lastDeliveryRetryTimeUsec == 0)
    {
        Logger::put_l(Logger::ERROR_LOG, System::CIMSERVER, Logger::WARNING,
            MessageLoaderParms(
                "HandlerService.DestinationQueue.INDICATION_DELIVERY_FAILED",
                "Failed to deliver an indication with SequenceContext \"$0\" "
                    "and SequenceNumber \"$1\" : $2.",
                (const char*)_getSequenceContext(info->indication).getCString(),
                _getSequenceNumber(info->indication),
                (const char*)e.getMessage().getCString()));
    }

    PEGASUS_ASSERT(_lastDeliveryRetryStatus == PENDING);
    _lastDeliveryRetryStatus = FAIL;
    info->deliveryRetryAttemptsMade++;

    // Check for DeliveryRetryAttempts by adding the original delivery attempt.
    if (info->deliveryRetryAttemptsMade >= _maxDeliveryRetryAttempts + 1)
    {
        _retryAttemptsExceededIndications++;
        _traceDiscardedIndication(
            DRA_EXCEEDED,
            info->indication);
        delete info;
    }
    else if (_queue.size() >= _maxIndicationDeliveryQueueSize)
    {
        _queueFullDroppedIndications++;
        _traceDiscardedIndication(
            DESTINATIONQUEUE_FULL,
            info->indication);
        delete info;
    }
    else
    {
        // To deliver the indications in the correct order, insert the
        // delivery retry failed indications at the front of the queue.
        _queue.insert_front(info);
        if (info->lastDeliveryRetryTimeUsec)
        {
            PEG_TRACE((TRC_IND_HANDLER,Tracer::LEVEL1,
                "Delivery failure for indication with SequenceContext %s and "
                    "SequenceNumber %" PEGASUS_64BIT_CONVERSION_WIDTH "d."
                        " DeliveryRetryAttempts made %u. Exception : %s",
                (const char*)_getSequenceContext(info->indication).getCString(),
                _getSequenceNumber(info->indication),
                info->deliveryRetryAttemptsMade,
                (const char*)e.getMessage().getCString()));
        }
        info->lastDeliveryRetryTimeUsec = System::getCurrentTimeUsec();
    }

    PEG_METHOD_EXIT();
}

void DestinationQueue::_waitForNonPendingDeliveryStatus()
{
    PEG_METHOD_ENTER(TRC_IND_HANDLER,
        "DestinationQueue::_waitForNonPendingDeliveryStatus");

    while (true)
    {
        {
            AutoMutex mtx(_queueMutex);
            if (_lastDeliveryRetryStatus != PENDING)
            {
                break;
            }
        }
        Threads::yield();
        Threads::sleep(50);
    }
    PEG_METHOD_EXIT();
}

void DestinationQueue::deleteMatchedIndications(
    const CIMObjectPath &subscriptionPath)
{
    PEG_METHOD_ENTER(TRC_IND_HANDLER,
        "DestinationQueue::deleteMatchedIndications");

    _waitForNonPendingDeliveryStatus();

    IndicationInfo *info;
    AutoMutex mtx(_queueMutex);

    for(Uint32 i = 0, n = _queue.size(); i < n; ++i)
    {
        info = _queue.remove_front();
        if (info->subscription.getPath().identical(subscriptionPath))
        {
            _subscriptionDeleteDroppedIndications++;
            _traceDiscardedIndication(
                SUBSCRIPTION_NOT_ACTIVE,
                info->indication);
            delete info;
        }
        else
        {
            _queue.insert_back(info);
        }
    }
    PEG_METHOD_EXIT();
}

void DestinationQueue::cleanup()
{
    PEG_METHOD_ENTER(TRC_IND_HANDLER,
        "DestinationQueue::cleanup");

    _cleanup(LISTENER_NOT_ACTIVE);

    PEG_METHOD_EXIT();
}

void DestinationQueue::shutdown()
{
    PEG_METHOD_ENTER(TRC_IND_HANDLER,
        "DestinationQueue::shutdown");

    _cleanup(CIMSERVER_SHUTDOWN);

    PEG_METHOD_EXIT();
}

void DestinationQueue::_cleanup(int reasonCode)
{
    _waitForNonPendingDeliveryStatus();

    IndicationInfo *info;
    while ((info = _queue.remove_front()))
    {
        _traceDiscardedIndication(
            reasonCode,
            info->indication);
        delete info;
    }
}

IndicationInfo* DestinationQueue::getNextIndicationForDelivery(
    Uint64 &timeNowUsec, Uint64 &nextIndDRIExpTimeUsec)
{
    AutoMutex mtx(_queueMutex);

    if (!_queue.size() || _lastDeliveryRetryStatus == PENDING)
    {
        // Maximum expiration time is equals to DeliveryRetryInterval.
        nextIndDRIExpTimeUsec = _minDeliveryRetryIntervalUsec;
        return 0;
    }

    nextIndDRIExpTimeUsec = 0;

    IndicationInfo *info;

    while (_queue.size())
    {
        info = _queue.front();

        if (timeNowUsec < info->arrivalTimeUsec ||
            timeNowUsec < info->lastDeliveryRetryTimeUsec)
        {
            timeNowUsec = System::getCurrentTimeUsec();
        }

        if ((timeNowUsec - info->arrivalTimeUsec) >=
            _sequenceIdentifierLifetimeUsec)
        {
            _lifetimeExpiredIndications++;
            IndicationInfo *temp = _queue.remove_front();
            _traceDiscardedIndication(
                SIL_EXPIRED,
                temp->indication);
            delete temp;
        }
        else if ((timeNowUsec - info->lastDeliveryRetryTimeUsec)
            >= _minDeliveryRetryIntervalUsec)
        {
            _lastDeliveryRetryStatus = PENDING;
            _queue.remove_front();
            IndicationInfo *temp = _queue.front();

            // The following algorithm is used to determine the elapsed
            // DeliveryRetryAttempts. To deliver the indication in order,
            // Server delays the newer indications until older indications
            // in the queue are attempted for delivery and their
            // DeliveryRetyAttempts are exceeded. The following algorithm
            // ensures that indications won't stay in the queue more than
            // (DeliveryRetryInterval * (DeliveryRetyAttempts + 1) time.
            Uint32 elapsedDeliveryRetryAttempts;
            if (info->lastDeliveryRetryTimeUsec)
            {
                elapsedDeliveryRetryAttempts =
                    ((timeNowUsec - info->lastDeliveryRetryTimeUsec) 
                        / _minDeliveryRetryIntervalUsec);
            }
            else
            {
                elapsedDeliveryRetryAttempts = 
                    ((timeNowUsec - info->arrivalTimeUsec) 
                        / _minDeliveryRetryIntervalUsec);
            }

            if (elapsedDeliveryRetryAttempts)
            {
                info->deliveryRetryAttemptsMade +=
                    elapsedDeliveryRetryAttempts - 1;
            }

            if (temp)
            {
                if (timeNowUsec - temp->lastDeliveryRetryTimeUsec
                        < _minDeliveryRetryIntervalUsec)
                {
                    nextIndDRIExpTimeUsec = _minDeliveryRetryIntervalUsec -
                        (timeNowUsec - temp->lastDeliveryRetryTimeUsec);
                }

                PEGASUS_ASSERT(nextIndDRIExpTimeUsec
                    <= _minDeliveryRetryIntervalUsec);
            }

            return info;
        }
        else
        {
            if (timeNowUsec - info->lastDeliveryRetryTimeUsec
                    < _minDeliveryRetryIntervalUsec)
            {
                nextIndDRIExpTimeUsec = _minDeliveryRetryIntervalUsec -
                    (timeNowUsec - info->lastDeliveryRetryTimeUsec);
            }

            PEGASUS_ASSERT(nextIndDRIExpTimeUsec
                <= _minDeliveryRetryIntervalUsec);

            break;
        }
    }

    return 0;
}

void DestinationQueue::getInfo(QueueInfo &qinfo)
{
    AutoMutex mtx(_queueMutex);

    qinfo.handlerName = _handler.getPath();
    qinfo.queueCreationTimeUsec = _queueCreationTimeUsec;
    qinfo.sequenceContext = _sequenceContext;
    qinfo.nextSequenceNumber = _sequenceNumber;
    qinfo.maxQueueLength = _maxIndicationDeliveryQueueSize;
    qinfo.sequenceIdentifierLifetimeSeconds =
        _sequenceIdentifierLifetimeUsec / 1000000;
    qinfo.size = _queue.size();
    qinfo.queueFullDroppedIndications = _queueFullDroppedIndications;
    qinfo.lifetimeExpiredIndications = _lifetimeExpiredIndications;
    qinfo.retryAttemptsExceededIndications = _retryAttemptsExceededIndications;
    qinfo.subscriptionDisableDroppedIndications =
        _subscriptionDeleteDroppedIndications;
    qinfo.lastSuccessfulDeliveryTimeUsec = _lastSuccessfulDeliveryTimeUsec;
}

PEGASUS_NAMESPACE_END

