//%2003////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002  BMC Software, Hewlett-Packard Development
// Company, L. P., IBM Corp., The Open Group, Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L. P.;
// IBM Corp.; EMC Corporation, The Open Group.
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
// Author: Chip Vincent (cvincent@us.ibm.com)
//
// Modified By: Carol Ann Krug Graves, Hewlett-Packard Company
//                  (carolann_graves@hp.com)
//              Mike Day, IBM (mdday@us.ibm.com)
//              Karl Schopmeyer(k.schopmeyer@opengroup.org) - Fix associators.
//              Yi Zhou, Hewlett-Packard Company (yi_zhou@hp.com)
//              Adrian Schuur, IBM (schuur@de.ibm.com)
//              Amit K Arora (amita@in.ibm.com) for PEP-101
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//              Seema Gupta (gseema@in.ibm.com for PEP135)
//
//%/////////////////////////////////////////////////////////////////////////////

#include "ProviderManagerService.h"

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Common/Thread.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/Logger.h>
#include <Pegasus/Common/AutoPtr.h>
#include <Pegasus/Common/Constants.h>

#include <Pegasus/Config/ConfigManager.h>

#include <Pegasus/ProviderManager2/BasicProviderManagerRouter.h>
#include <Pegasus/ProviderManager2/OOPProviderManagerRouter.h>

PEGASUS_NAMESPACE_BEGIN

inline Boolean _isSupportedRequestType(const Message * message)
{
    // ATTN: needs implementation

    // for now, assume all requests are valid

    return(true);
}

inline Boolean _isSupportedResponseType(const Message * message)
{
    // ATTN: needs implementation

    // for now, assume all responses are invalid

    return(false);
}

ProviderManagerService* ProviderManagerService::providerManagerService=NULL;
Uint32 ProviderManagerService::_indicationServiceQueueId = PEG_NOT_FOUND;

ProviderManagerService::ProviderManagerService(void)
    : MessageQueueService(PEGASUS_QUEUENAME_PROVIDERMANAGER_CPP)
{
    providerManagerService=this;
}

ProviderManagerService::ProviderManagerService(
        ProviderRegistrationManager * providerRegistrationManager,
        CIMRepository * repository)
    : MessageQueueService(PEGASUS_QUEUENAME_PROVIDERMANAGER_CPP)
{
    providerManagerService=this;
    _repository=repository;

    _providerRegistrationManager = providerRegistrationManager;

    _unloadIdleProvidersBusy = 0;

    // Determine whether Out-of-Process Provider support is enabled
    ConfigManager* configManager = ConfigManager::getInstance();
    if (String::equal(
        configManager->getCurrentValue("enableProviderProcesses"), "true"))
    {
        _providerManagerRouter =
            new OOPProviderManagerRouter(indicationCallback);
    }
    else
    {
        _providerManagerRouter =
            new BasicProviderManagerRouter(indicationCallback);
    }
}

ProviderManagerService::~ProviderManagerService(void)
{
    delete _providerManagerRouter;
    providerManagerService=NULL;
}

Boolean ProviderManagerService::messageOK(const Message * message)
{
    PEGASUS_ASSERT(message != 0);

    if(_isSupportedRequestType(message))
    {
        return(MessageQueueService::messageOK(message));
    }

    return(false);
}

void ProviderManagerService::handleEnqueue(void)
{
    Message * message = dequeue();

    handleEnqueue(message);
}

void ProviderManagerService::handleEnqueue(Message * message)
{
    PEGASUS_ASSERT(message != 0);

    AsyncLegacyOperationStart * asyncRequest;

    if(message->_async != NULL)
    {
        asyncRequest = static_cast<AsyncLegacyOperationStart *>(message->_async);
    }
    else
    {
        asyncRequest = new AsyncLegacyOperationStart(
            get_next_xid(),
            0,
            this->getQueueId(),
            message,
            this->getQueueId());
    }

    _handle_async_request(asyncRequest);
}

void ProviderManagerService::_handle_async_request(AsyncRequest * request)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "ProviderManagerService::_handle_async_request");

    PEGASUS_ASSERT((request != 0) && (request->op != 0));

    if(request->getType() == async_messages::ASYNC_LEGACY_OP_START)
    {
        request->op->processing();

        _incomingQueue.enqueue(request->op);

         while (!_thread_pool->allocate_and_awaken(
                     (void *)this, ProviderManagerService::handleCimOperation))
         {
             pegasus_yield();
         }
    }
    else
    {
        // pass all other operations to the default handler
        MessageQueueService::_handle_async_request(request);
    }

    PEG_METHOD_EXIT();

    return;
}

// Note: This method should not throw an exception.  It is used as a thread
// entry point, and any exceptions thrown are ignored.
PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL
ProviderManagerService::handleCimOperation(void * arg) 
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "ProviderManagerService::handleCimOperation");

    if(arg == 0)
    {
        // thread started with invalid argument.
        return(PEGASUS_THREAD_RETURN(1));
    }

    // get the service from argument
    ProviderManagerService * service =
        reinterpret_cast<ProviderManagerService *>(arg);

    if(service->_incomingQueue.size() == 0)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "ProviderManagerService::handleCimOperation() called with no "
                "op node in queue");

        PEG_METHOD_EXIT();

        // thread started with no message in queue.
        return(PEGASUS_THREAD_RETURN(1));
    }

    AsyncOpNode * op = service->_incomingQueue.dequeue();

    if((op == 0) || (op->_request.count() == 0))
    {
        // ATTN: This may dereference a null pointer!
        MessageQueue * queue = MessageQueue::lookup(op->_source_queue);

        PEGASUS_ASSERT(queue != 0);

        PEG_METHOD_EXIT();

        // no request in op node
        return(PEGASUS_THREAD_RETURN(1));
    }

    AsyncRequest * request = static_cast<AsyncRequest *>(op->_request.next(0));

    if ((request == 0) ||
        (request->getType() != async_messages::ASYNC_LEGACY_OP_START))
    {
        // reply with NAK
        PEG_METHOD_EXIT();
        return(PEGASUS_THREAD_RETURN(0));
    }

    try
    {
        Message* legacy =
            static_cast<AsyncLegacyOperationStart *>(request)->get_action();

        if(_isSupportedRequestType(legacy))
        {
            AutoPtr<Message> xmessage(legacy);

            // Set the client's requested language into this service thread.
            // This will allow functions in this service to return messages
            // in the correct language.
            CIMMessage* msg = dynamic_cast<CIMMessage *>(legacy);

            if (msg != 0)
            {
		        AcceptLanguages* langs =
                    new AcceptLanguages(((AcceptLanguageListContainer)msg->operationContext.get
											(AcceptLanguageListContainer::NAME)).getLanguages());
                Thread::setLanguages(langs);
            }
            else
            {
                Thread::clearLanguages();
            }

            service->handleCimRequest(op, legacy);
        }
    }
    catch(...)
    {
        // ATTN: log error
    }

    PEG_METHOD_EXIT();

    return(PEGASUS_THREAD_RETURN(0));
}

void ProviderManagerService::handleCimRequest(
    AsyncOpNode * op,
    Message * message)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "ProviderManagerService::handleCimRequest");

    CIMRequestMessage * request = dynamic_cast<CIMRequestMessage *>(message);
    PEGASUS_ASSERT(request != 0);

    // get request from op node
    AsyncRequest * async = static_cast<AsyncRequest *>(op->_request.next(0));
    PEGASUS_ASSERT(async != 0);

    Message * response = 0;
    Boolean consumerLookupFailed = false;

    if (request->getType() == CIM_EXPORT_INDICATION_REQUEST_MESSAGE)
    {
        //
        // Get a ProviderIdContainer for ExportIndicationRequestMessage.
        // Note: This can be removed when the CIMExportRequestDispatcher
        // is updated to add the ProviderIdContainer to the message.
        //
        CIMInstance providerModule;
        CIMInstance provider;
        const CIMExportIndicationRequestMessage* expRequest =
            dynamic_cast<const CIMExportIndicationRequestMessage*>(request);
        if (_providerRegistrationManager->lookupIndicationConsumer(
                expRequest->destinationPath, provider, providerModule))
        {
            request->operationContext.insert(
                ProviderIdContainer(providerModule, provider));
        }
        else
        {
            consumerLookupFailed = true;
        }
    }

    if (consumerLookupFailed)
    {
        CIMResponseMessage* cimResponse = request->buildResponse();
        cimResponse->cimException = PEGASUS_CIM_EXCEPTION(
            CIM_ERR_NOT_SUPPORTED, String::EMPTY);
        response = cimResponse;
    }
    else if ((dynamic_cast<CIMOperationRequestMessage*>(request) != 0) ||
        (dynamic_cast<CIMIndicationRequestMessage*>(request) != 0) ||
        (request->getType() == CIM_EXPORT_INDICATION_REQUEST_MESSAGE) ||
        (request->getType() == CIM_INITIALIZE_PROVIDER_REQUEST_MESSAGE))
    {
        // Handle CIMOperationRequestMessage, CIMExportIndicationRequestMessage,
        // CIMIndicationRequestMessage, and CIMInitializeProviderRequestMessage.
        // (These should be blocked when the provider module is disabled.)

        //
        // Get the provider module instance to check for a disabled module
        //
        CIMInstance providerModule;

        // The provider ID container is added to the OperationContext
        // by the CIMOperationRequestDispatcher for all CIM operation
        // requests to providers, so it does not need to be added again.
        // CIMInitializeProviderRequestMessage also has a provider ID
        // container.
        ProviderIdContainer pidc =
            request->operationContext.get(ProviderIdContainer::NAME);
        providerModule = pidc.getModule();

        //
        // Check if the target provider is disabled
        //
        Boolean moduleDisabled = false;
        Uint32 pos = providerModule.findProperty(CIMName("OperationalStatus"));
        PEGASUS_ASSERT(pos != PEG_NOT_FOUND);
        Array<Uint16> operationalStatus;
        providerModule.getProperty(pos).getValue().get(operationalStatus);

        for(Uint32 i = 0; i < operationalStatus.size(); i++)
        {
            if ((operationalStatus[i] == CIM_MSE_OPSTATUS_VALUE_STOPPED) ||
                (operationalStatus[i] == CIM_MSE_OPSTATUS_VALUE_STOPPING))
            {
                moduleDisabled = true;
                break;
            }
        }

        if (moduleDisabled)
        {
            //
            // Send a "provider blocked" response
            //
            CIMResponseMessage* cimResponse = request->buildResponse();
            cimResponse->cimException = PEGASUS_CIM_EXCEPTION_L(
                CIM_ERR_ACCESS_DENIED,
                MessageLoaderParms(
                    "ProviderManager.ProviderManagerService.PROVIDER_BLOCKED",
                    "provider blocked."));
            response = cimResponse;
        }
        else
        {
            //
            // Forward the request to the appropriate ProviderManagerRouter
            //
            response = _providerManagerRouter->processMessage(request);
        }
    }
    else if (request->getType() == CIM_ENABLE_MODULE_REQUEST_MESSAGE)
    {
        // Handle CIMEnableModuleRequestMessage
        CIMEnableModuleRequestMessage * emReq =
            dynamic_cast<CIMEnableModuleRequestMessage*>(request);

        CIMInstance providerModule = emReq->providerModule;

        try
        {
            // Forward the request to the ProviderManager
            response = _providerManagerRouter->processMessage(request);

            // If successful, update provider module status to OK
            // ATTN: Use CIMEnableModuleResponseMessage operationalStatus?
            CIMEnableModuleResponseMessage * emResp =
                dynamic_cast<CIMEnableModuleResponseMessage*>(response);
            if (emResp->cimException.getCode() == CIM_ERR_SUCCESS)
            {
                _updateProviderModuleStatus(
                    providerModule, CIM_MSE_OPSTATUS_VALUE_STOPPED, 
                    CIM_MSE_OPSTATUS_VALUE_OK);
            }
        }
        catch (Exception& e)
        {
            // Get the OperationalStatus property from the provider module
            Array<Uint16> operationalStatus;
            CIMValue itValue = emReq->providerModule.getProperty(
                emReq->providerModule.findProperty("OperationalStatus"))
                    .getValue();
            itValue.get(operationalStatus);

            if (response != 0)
            {
                delete response;
            }

            response = new CIMEnableModuleResponseMessage(
                request->messageId,
                CIMException(CIM_ERR_FAILED, e.getMessage()),
                request->queueIds.copyAndPop(),
                operationalStatus);
        }
    }
    else if (request->getType() == CIM_DISABLE_MODULE_REQUEST_MESSAGE)
    {
        // Handle CIMDisableModuleRequestMessage
        CIMDisableModuleRequestMessage * dmReq =
            dynamic_cast<CIMDisableModuleRequestMessage*>(request);

        CIMInstance providerModule = dmReq->providerModule;
        Boolean updateModuleStatus = !dmReq->disableProviderOnly;

        try
        {
            // Change module status from OK to STOPPING
            if (updateModuleStatus)
            {
                _updateProviderModuleStatus(
                    providerModule, CIM_MSE_OPSTATUS_VALUE_OK, 
                    CIM_MSE_OPSTATUS_VALUE_STOPPING);
            }

            // Forward the request to the ProviderManager
            response = _providerManagerRouter->processMessage(request);

            // Update provider module status based on success or failure
            if (updateModuleStatus)
            {
                CIMDisableModuleResponseMessage * dmResp =
                    dynamic_cast<CIMDisableModuleResponseMessage*>(response);
                if (dmResp->cimException.getCode() != CIM_ERR_SUCCESS)
                {
                    // Disable operation failed.  Module not stopped.
                    _updateProviderModuleStatus(
                        providerModule, CIM_MSE_OPSTATUS_VALUE_STOPPING, 
                        CIM_MSE_OPSTATUS_VALUE_OK);
                }
                else
                {
                    // Disable may or may not have been successful,
                    // depending on whether there are outstanding requests.
                    // Use last operationalStatus entry.
                    _updateProviderModuleStatus(
                        providerModule, CIM_MSE_OPSTATUS_VALUE_STOPPING,
                        dmResp->operationalStatus[
                            dmResp->operationalStatus.size()-1]);
                }
            }
        }
        catch (Exception& e)
        {
            // Get the OperationalStatus property from the provider module
            Array<Uint16> operationalStatus;
            CIMValue itValue = dmReq->providerModule.getProperty(
                dmReq->providerModule.findProperty("OperationalStatus"))
                    .getValue();
            itValue.get(operationalStatus);

            if (response != 0)
            {
                delete response;
            }

            response = new CIMDisableModuleResponseMessage(
                request->messageId,
                CIMException(CIM_ERR_FAILED, e.getMessage()),
                request->queueIds.copyAndPop(),
                operationalStatus);
        }
    }
    else
    {
        response = _providerManagerRouter->processMessage(request);
    }

    AsyncLegacyOperationResult * async_result =
        new AsyncLegacyOperationResult(
        async->getKey(),
        async->getRouting(),
        op,
        response);

    _complete_op_node(op, ASYNC_OPSTATE_COMPLETE, 0, 0);

    PEG_METHOD_EXIT();
}

void ProviderManagerService::unloadIdleProviders()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "ProviderManagerService::unloadIdleProviders");

    // Ensure that only one _unloadIdleProvidersHandler thread runs at a time
    _unloadIdleProvidersBusy++;
    if ((_unloadIdleProvidersBusy.value() == 1) &&
        (_thread_pool->allocate_and_awaken(
             (void*)this, ProviderManagerService::_unloadIdleProvidersHandler)))
    {
        // _unloadIdleProvidersBusy is decremented in
        // _unloadIdleProvidersHandler
    }
    else
    {
        // If we fail to allocate a thread, don't retry now.
        _unloadIdleProvidersBusy--;
    }

    PEG_METHOD_EXIT();
}

PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL
ProviderManagerService::_unloadIdleProvidersHandler(void* arg) throw()
{
    try
    {
        PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
            "ProviderManagerService::unloadIdleProvidersHandler");

        ProviderManagerService* myself =
            reinterpret_cast<ProviderManagerService*>(arg);

        try
        {
            myself->_providerManagerRouter->unloadIdleProviders();
        }
        catch (...)
        {
            // Ignore errors
            PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL2,
                "Unexpected exception in _unloadIdleProvidersHandler");
        }

        myself->_unloadIdleProvidersBusy--;
        PEG_METHOD_EXIT();
    }
    catch (...)
    {
        // Ignore errors
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL2,
            "Unexpected exception in _unloadIdleProvidersHandler");
    }

    return(PEGASUS_THREAD_RETURN(0));
}

// Updates the providerModule instance and the ProviderRegistrationManager
void ProviderManagerService::_updateProviderModuleStatus(
    CIMInstance& providerModule,
    Uint16 fromStatus,
    Uint16 toStatus)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "ProviderManagerService::_updateProviderModuleStatus");

    Array<Uint16> operationalStatus;
    String providerModuleName;

    Uint32 pos = providerModule.findProperty(CIMName("Name"));
    PEGASUS_ASSERT(pos != PEG_NOT_FOUND);
    providerModule.getProperty(pos).getValue().get(providerModuleName);

    //
    // get operational status
    //
    pos = providerModule.findProperty(CIMName("OperationalStatus"));
    PEGASUS_ASSERT(pos != PEG_NOT_FOUND);
    CIMProperty operationalStatusProperty = providerModule.getProperty(pos);
    CIMValue operationalStatusValue = operationalStatusProperty.getValue();

    if (!operationalStatusValue.isNull())
    {
        operationalStatusValue.get(operationalStatus);
    }

    //
    // update module status
    //
    for (Uint32 i = operationalStatus.size(); i > 0; i--)
    {
        if (operationalStatus[i-1] == fromStatus)
        {
            operationalStatus.remove(i-1);
        }
    }

    operationalStatus.append(toStatus);

    if (_providerRegistrationManager->setProviderModuleStatus(
            providerModuleName, operationalStatus) == false)
    {
        throw PEGASUS_CIM_EXCEPTION_L(
            CIM_ERR_FAILED,
            MessageLoaderParms(
                "ProviderManager.ProviderManagerService."
                    "SET_MODULE_STATUS_FAILED",
                "set module status failed."));
    }

    operationalStatusProperty.setValue(CIMValue(operationalStatus));

    PEG_METHOD_EXIT();
}

void ProviderManagerService::indicationCallback(
    CIMProcessIndicationRequestMessage* request)
{
 	try
	{
		AcceptLanguageListContainer cntr = request->operationContext.get(AcceptLanguageListContainer::NAME);
	}catch(const Exception &)
	{
		request->operationContext.insert(AcceptLanguageListContainer(AcceptLanguages::EMPTY));
	}    
	
	if (_indicationServiceQueueId == PEG_NOT_FOUND)
    {
        Array<Uint32> serviceIds;

        providerManagerService->find_services(
            PEGASUS_QUEUENAME_INDICATIONSERVICE, 0, 0, &serviceIds);
        PEGASUS_ASSERT(serviceIds.size() != 0);

        _indicationServiceQueueId = serviceIds[0];
    }

    request->queueIds = QueueIdStack(
        _indicationServiceQueueId, providerManagerService->getQueueId());

    AsyncLegacyOperationStart * asyncRequest =
        new AsyncLegacyOperationStart(
        providerManagerService->get_next_xid(),
        0,
        _indicationServiceQueueId,
        request,
        _indicationServiceQueueId);

    providerManagerService->SendForget(asyncRequest);
}

PEGASUS_NAMESPACE_END
