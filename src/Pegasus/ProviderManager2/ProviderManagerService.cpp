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
//
//%/////////////////////////////////////////////////////////////////////////////

#include "ProviderManagerService.h"

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/Logger.h>
#include <Pegasus/Common/AutoPtr.h>
#include <Pegasus/ProviderManager2/OperationResponseHandler.h>

#include <Pegasus/Config/ConfigManager.h>

#include <Pegasus/ProviderManager2/ProviderManagerModule.h>
#include <Pegasus/ProviderManager2/ProviderManager.h>
#include <Pegasus/ProviderManager2/ProviderType.h>
#include <Pegasus/ProviderManager2/ProviderName.h>

PEGASUS_NAMESPACE_BEGIN

static const Uint16 _MODULE_OK       = 2;
static const Uint16 _MODULE_STOPPING = 9;
static const Uint16 _MODULE_STOPPED  = 10;

// BEGIN TEMP SECTION
class ProviderManagerContainer
{
public:
    ProviderManagerContainer(void) : _manager(0)
    {
    }

    ProviderManagerContainer(const ProviderManagerContainer & container) : _manager(0)
    {
        *this = container;
    }

    ProviderManagerContainer(const String & physicalName, const String & logicalName, const String & interfaceName) : _manager(0)
    {
        _physicalName=ProviderManager::_resolvePhysicalName(physicalName);

        _logicalName = logicalName;

        _interfaceName = interfaceName;

        _module = ProviderManagerModule(_physicalName);

        _module.load();

        _manager = _module.getProviderManager(_logicalName);

        PEGASUS_ASSERT(_manager != 0);
    }

    ~ProviderManagerContainer(void)
    {
        _module.unload();
    }

    ProviderManagerContainer & operator=(const ProviderManagerContainer & container)
    {
        if(this == &container)
        {
            return(*this);
        }

        _logicalName = container._logicalName;
        _physicalName = container._physicalName;
        _interfaceName = container._interfaceName;

        _module = container._module;
        _manager = container._manager;

        return(*this);
    }

    ProviderManager *getProviderManager(void)
    {
        return _manager;
    }

    const String & getPhysicalName(void) const
    {
        return(_physicalName);
    }

    const String & getLogicalName(void) const
    {
        return(_logicalName);
    }

    const String & getInterfaceName(void) const
    {
        return(_interfaceName);
    }

private:
    String _physicalName;
    String _logicalName;
    String _interfaceName;

    ProviderManagerModule _module;
    ProviderManager * _manager;

};

static Array<ProviderManagerContainer*> _providerManagers;
// END TEMP SECTION

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
CIMRepository* ProviderManagerService::_repository=NULL;

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

    // ATTN: this section is a temporary solution to populate the list of enabled
    // provider managers for a given distribution. it includes another temporary
    // solution for converting a generic file name into a file name useable by
    // each platform.

    // BEGIN TEMP SECTION
    //#if defined(PEGASUS_OS_OS400)
    //_providerManagers.append(ProviderManagerContainer("QSYS/??????????", "INTERNAL", "INTERNAL"));
    //#else
    //_providerManager.append(ProviderManagerContainer("InternalProviderManager", "DEFAULT", "INTERNAL"));
    //#endif

    #if defined(ENABLE_DEFAULT_PROVIDER_MANAGER)
    #if defined(PEGASUS_OS_OS400)
    _providerManagers.append(
       new ProviderManagerContainer("QSYS/QYCMDFTPVM", "DEFAULT", "C++Default"));
    #else
    _providerManagers.append(
       new ProviderManagerContainer("DefaultProviderManager", "DEFAULT", "C++Default"));
    #endif
    #endif

    #if defined(ENABLE_CMPI_PROVIDER_MANAGER)
    #if defined(PEGASUS_OS_OS400)
    _providerManagers.append(
       new ProviderManagerContainer("QSYS/QYCMCMPIPM", "CMPI", "CMPI"));
    #else
    _providerManagers.append(
       new ProviderManagerContainer("CMPIProviderManager", "CMPI", "CMPI"));
    #endif
    #endif

    #if defined(ENABLE_JMPI_PROVIDER_MANAGER)
    #if defined(PEGASUS_OS_OS400)
    _providerManagers.append(
       new ProviderManagerContainer("QSYS/QYCMJMPIPM", "JMPI", "JMPI"));
    #else
    _providerManagers.append(
       new ProviderManagerContainer("JMPIProviderManager", "JMPI", "JMPI"));
    #endif
    #endif
    
    // END TEMP SECTION
}

ProviderManagerService::~ProviderManagerService(void)
{
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


PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL ProviderManagerService::handleCimOperation(void * arg) throw()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "ProviderManagerService::handleCimOperation");

    if(arg == 0)
    {
        // thread started with invalid argument.
        return(PEGASUS_THREAD_RETURN(1));
    }

    // get the service from argument
    ProviderManagerService * service = reinterpret_cast<ProviderManagerService *>(arg);

    if(service->_incomingQueue.size() == 0)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "ProviderManagerService::handleCimOperation() called with no op node in queue" );

        PEG_METHOD_EXIT();

        // thread started with no message in queue.
        return(PEGASUS_THREAD_RETURN(1));
    }

    AsyncOpNode * op = service->_incomingQueue.dequeue();

    if((op == 0) || (op->_request.count() == 0))
    {
        MessageQueue * queue = MessageQueue::lookup(op->_source_queue);

        PEGASUS_ASSERT(queue != 0);

        PEG_METHOD_EXIT();

        // no request in op node
        return(PEGASUS_THREAD_RETURN(1));
    }

    AsyncRequest * request = static_cast<AsyncRequest *>(op->_request.next(0));

    if((request == 0) || (request->getType() != async_messages::ASYNC_LEGACY_OP_START))
    {
        // reply with NAK

        PEG_METHOD_EXIT();

        return(PEGASUS_THREAD_RETURN(0));
    }

    try
    {
        Message * legacy = static_cast<AsyncLegacyOperationStart *>(request)->get_action();

        if(_isSupportedRequestType(legacy))
        {
            AutoPtr<Message> xmessage(legacy);

            // Set the client's requested language into this service thread.
            // This will allow functions in this service to return messages
            // in the correct language.
            CIMMessage * msg = dynamic_cast<CIMMessage *>(legacy);

            if(msg != 0)
            {
                AcceptLanguages * langs = new AcceptLanguages(msg->acceptLanguages);

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

void ProviderManagerService::handleCimRequest(AsyncOpNode * op, Message * message)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "ProviderManagerService::handleCimRequest");

    CIMRequestMessage * request = dynamic_cast<CIMRequestMessage *>(message);
    PEGASUS_ASSERT(request != 0);

    // get request from op node
    AsyncRequest * async = static_cast<AsyncRequest *>(op->_request.next(0));
    PEGASUS_ASSERT(async != 0);

    Message * response = 0;

    if ((dynamic_cast<CIMOperationRequestMessage*>(request) != 0) ||
        (request->getType() == CIM_EXPORT_INDICATION_REQUEST_MESSAGE))
    {
        // Handle CIMOperationRequestMessage and
        // CIMExportIndicationRequestMessage

        // Add provider information to OperationContext for the ProviderManager
        ProviderIdContainer pidc = _getProviderIdContainer(request);
        request->operationContext.insert(pidc);

        // Retrieve the provider interface type
        String interfaceType;
        CIMValue itValue = pidc.getModule().getProperty(
            pidc.getModule().findProperty("InterfaceType")).getValue();
        itValue.get(interfaceType);

        // Forward the request to the appropriate ProviderManager
        ProviderManager* pm = _lookupProviderManager(interfaceType);
        response = pm->processMessage(request);
    }
    else if (dynamic_cast<CIMIndicationRequestMessage*>(request) != 0)
    {
        // Handle CIMIndicationRequestMessage
        CIMIndicationRequestMessage* indReq =
            dynamic_cast<CIMIndicationRequestMessage*>(request);

        // Get the InterfaceType property from the provider module instance
        String interfaceType;
        CIMValue itValue = indReq->providerModule.getProperty(
            indReq->providerModule.findProperty("InterfaceType")).getValue();
        itValue.get(interfaceType);

        // Forward the request to the appropriate ProviderManager
        ProviderManager* pm = _lookupProviderManager(interfaceType);
        response = pm->processMessage(request);
    }
    else if (request->getType() == CIM_ENABLE_MODULE_REQUEST_MESSAGE)
    {
        // Handle CIMEnableModuleRequestMessage
        CIMEnableModuleRequestMessage * emReq =
            dynamic_cast<CIMEnableModuleRequestMessage*>(request);

        CIMInstance providerModule = emReq->providerModule;

        try
        {
            // Get the InterfaceType property from the provider module instance
            String interfaceType;
            CIMValue itValue = emReq->providerModule.getProperty(
                emReq->providerModule.findProperty("InterfaceType")).getValue();
            itValue.get(interfaceType);

            // Forward the request to the appropriate ProviderManager
            // ATTN: Why is this sent to a provider manager at all?
            ProviderManager* pm = _lookupProviderManager(interfaceType);
            response = pm->processMessage(request);

            // If successful, update provider module status to OK
            // ATTN: Use CIMEnableModuleResponseMessage operationalStatus?
            CIMEnableModuleResponseMessage * emResp =
                dynamic_cast<CIMEnableModuleResponseMessage*>(response);
            if (emResp->cimException.getCode() == CIM_ERR_SUCCESS)
            {
                _updateProviderModuleStatus(
                    providerModule, _MODULE_STOPPED, _MODULE_OK);
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
            // Get the InterfaceType property from the provider module instance
            String interfaceType;
            CIMValue itValue = dmReq->providerModule.getProperty(
                dmReq->providerModule.findProperty("InterfaceType")).getValue();
            itValue.get(interfaceType);

            // Change module status from OK to STOPPING
            if (updateModuleStatus)
            {
                _updateProviderModuleStatus(
                    providerModule, _MODULE_OK, _MODULE_STOPPING);
            }

            // Forward the request to the appropriate ProviderManager
            // ATTN: Why is this sent to a provider manager at all?
            ProviderManager* pm = _lookupProviderManager(interfaceType);
            response = pm->processMessage(request);

            // Update provider module status based on success or failure
            if (updateModuleStatus)
            {
                CIMDisableModuleResponseMessage * dmResp =
                    dynamic_cast<CIMDisableModuleResponseMessage*>(response);
                if (dmResp->cimException.getCode() != CIM_ERR_SUCCESS)
                {
                    // Disable operation failed.  Module not stopped.
                    _updateProviderModuleStatus(
                        providerModule, _MODULE_STOPPING, _MODULE_OK);
                }
                else
                {
                    // Disable may or may not have been successful,
                    // depending on whether there are outstanding requests.
                    // Use last operationalStatus entry.
                    _updateProviderModuleStatus(
                        providerModule, _MODULE_STOPPING,
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
    else if (request->getType() == CIM_STOP_ALL_PROVIDERS_REQUEST_MESSAGE)
    {
        // Handle CIMStopAllProvidersRequestMessage
        // Send CIMStopAllProvidersRequestMessage to all ProviderManagers
        for (Uint32 i = 0, n = _providerManagers.size(); i < n; i++)
        {
            ProviderManagerContainer* pmc=_providerManagers[i];
            Message* resp = pmc->getProviderManager()->processMessage(request);
            if (resp)
            {
                response = resp;
            }
        }
    }
    else
    {
        // ERROR: Unrecognized message type.
        PEGASUS_ASSERT(0);
        CIMResponseMessage* resp = new CIMResponseMessage(
            0, request->messageId, CIMException(),
            request->queueIds.copyAndPop());
        resp->synch_response(request);
        OperationResponseHandler handler(request, resp);
        handler.setStatus(CIM_ERR_FAILED, "Unknown message type.");
        response = resp;
    }

    // preserve message key
    response->setKey(request->getKey());

    // set HTTP method in response from request
    response->setHttpMethod(request->getHttpMethod());

    AsyncLegacyOperationResult * async_result =
        new AsyncLegacyOperationResult(
        async->getKey(),
        async->getRouting(),
        op,
        response);

    _complete_op_node(op, ASYNC_OPSTATE_COMPLETE, 0, 0);

    PEG_METHOD_EXIT();
}

// ATTN: May need to add interfaceVersion parameter to further constrain lookup
ProviderManager* ProviderManagerService::_lookupProviderManager(
    const String& interfaceType)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "ProviderManagerService::_lookupProviderManager");

    // find provider manager for specified provider interface type
    for(Uint32 i = 0, n = _providerManagers.size(); i < n; i++)
    {
        if (interfaceType == _providerManagers[i]->getInterfaceName())
        {
            ProviderManagerContainer* pmc=_providerManagers[i];
            PEG_METHOD_EXIT();
            return pmc->getProviderManager();
        }
    }

    // ProviderManager not found for the specified interface type
    PEGASUS_ASSERT(0);
    PEG_METHOD_EXIT();
    return 0;
    //ProviderManagerContainer *pmc=_providerManagers[0];
    //return pmc->getProviderManager();
}

void ProviderManagerService::unload_idle_providers(void)
{
    for(Uint32 i = 0, n = _providerManagers.size(); i < n; i++)
    {
      ProviderManagerContainer *pmc=_providerManagers[i];
           pmc->getProviderManager()->unload_idle_providers();
    }
}

ProviderIdContainer ProviderManagerService::_getProviderIdContainer(
    const CIMRequestMessage* message)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "ProviderManagerService::_getProviderIdContainer");

    CIMInstance providerModule;
    CIMInstance provider;

    switch (message->getType())
    {
    case CIM_GET_CLASS_REQUEST_MESSAGE:
    case CIM_DELETE_CLASS_REQUEST_MESSAGE:
    case CIM_CREATE_CLASS_REQUEST_MESSAGE:
    case CIM_MODIFY_CLASS_REQUEST_MESSAGE:
    case CIM_ENUMERATE_CLASSES_REQUEST_MESSAGE:
    case CIM_ENUMERATE_CLASS_NAMES_REQUEST_MESSAGE:
    case CIM_GET_QUALIFIER_REQUEST_MESSAGE:
    case CIM_SET_QUALIFIER_REQUEST_MESSAGE:
    case CIM_DELETE_QUALIFIER_REQUEST_MESSAGE:
    case CIM_ENUMERATE_QUALIFIERS_REQUEST_MESSAGE:
        // The ProviderManagerService does not support class operations
        PEGASUS_ASSERT(0);
        break;

    case CIM_GET_INSTANCE_REQUEST_MESSAGE:
    case CIM_DELETE_INSTANCE_REQUEST_MESSAGE:
    case CIM_CREATE_INSTANCE_REQUEST_MESSAGE:
    case CIM_MODIFY_INSTANCE_REQUEST_MESSAGE:
    case CIM_ENUMERATE_INSTANCES_REQUEST_MESSAGE:
    case CIM_ENUMERATE_INSTANCE_NAMES_REQUEST_MESSAGE:
    case CIM_GET_PROPERTY_REQUEST_MESSAGE:
    case CIM_SET_PROPERTY_REQUEST_MESSAGE:
    {
        // Look up instance provider
        const CIMOperationRequestMessage* request =
            dynamic_cast<const CIMOperationRequestMessage*>(message);
        _providerRegistrationManager->lookupInstanceProvider(
            request->nameSpace, request->className, provider, providerModule);
        break;
    }

    case CIM_EXEC_QUERY_REQUEST_MESSAGE:
    {
        // Look up instance query provider
        const CIMOperationRequestMessage* request =
            dynamic_cast<const CIMOperationRequestMessage*>(message);
        Boolean hasNoQuery;
        _providerRegistrationManager->lookupInstanceProvider(
            request->nameSpace, request->className, provider, providerModule,
            0, &hasNoQuery);
        // We shouldn't have gotten this far if this isn't a query provider
        PEGASUS_ASSERT(!hasNoQuery);
        break;
    }

    case CIM_ASSOCIATORS_REQUEST_MESSAGE:
    {
        // Look up association provider
        const CIMAssociatorsRequestMessage* request =
            dynamic_cast<const CIMAssociatorsRequestMessage*>(message);
        Array<CIMInstance> providerModules;
        Array<CIMInstance> providers;
        _providerRegistrationManager->lookupAssociationProvider(
            request->nameSpace, request->assocClass, providers, providerModules);
        providerModule = providerModules[0];
        provider = providers[0];
        break;
    }

    case CIM_ASSOCIATOR_NAMES_REQUEST_MESSAGE:
    {
        // Look up association provider
        const CIMAssociatorNamesRequestMessage* request =
            dynamic_cast<const CIMAssociatorNamesRequestMessage*>(message);
        Array<CIMInstance> providerModules;
        Array<CIMInstance> providers;
        _providerRegistrationManager->lookupAssociationProvider(
            request->nameSpace, request->assocClass, providers, providerModules);
        providerModule = providerModules[0];
        provider = providers[0];
        break;
    }

    case CIM_REFERENCES_REQUEST_MESSAGE:
    {
        // Look up association provider
        const CIMReferencesRequestMessage* request =
            dynamic_cast<const CIMReferencesRequestMessage*>(message);
        Array<CIMInstance> providerModules;
        Array<CIMInstance> providers;
        _providerRegistrationManager->lookupAssociationProvider(
            request->nameSpace, request->resultClass, providers, providerModules);
        providerModule = providerModules[0];
        provider = providers[0];
        break;
    }

    case CIM_REFERENCE_NAMES_REQUEST_MESSAGE:
    {
        // Look up association provider
        const CIMReferenceNamesRequestMessage* request =
            dynamic_cast<const CIMReferenceNamesRequestMessage*>(message);
        Array<CIMInstance> providerModules;
        Array<CIMInstance> providers;
        _providerRegistrationManager->lookupAssociationProvider(
            request->nameSpace, request->resultClass, providers, providerModules);
        providerModule = providerModules[0];
        provider = providers[0];
        break;
    }

    case CIM_INVOKE_METHOD_REQUEST_MESSAGE:
    {
        // Look up method provider
        const CIMInvokeMethodRequestMessage* request =
            dynamic_cast<const CIMInvokeMethodRequestMessage*>(message);
        _providerRegistrationManager->lookupMethodProvider(
            request->nameSpace, request->className, request->methodName,
            provider, providerModule);
        break;
    }

    case CIM_EXPORT_INDICATION_REQUEST_MESSAGE:
    {
        const CIMExportIndicationRequestMessage* request =
            dynamic_cast<const CIMExportIndicationRequestMessage*>(message);
        _providerRegistrationManager->lookupIndicationConsumer(
            request->destinationPath, provider, providerModule);
        break;
    }

    case CIM_ENABLE_INDICATIONS_REQUEST_MESSAGE:
    case CIM_DISABLE_INDICATIONS_REQUEST_MESSAGE:
    case CIM_CREATE_SUBSCRIPTION_REQUEST_MESSAGE:
    case CIM_MODIFY_SUBSCRIPTION_REQUEST_MESSAGE:
    case CIM_DELETE_SUBSCRIPTION_REQUEST_MESSAGE:
    {
        // Provider information is already in the message
        const CIMIndicationRequestMessage* request =
            dynamic_cast<const CIMIndicationRequestMessage*>(message);
        providerModule = request->providerModule;
        provider = request->provider;
        break;
    }

    case CIM_ENABLE_INDICATION_SUBSCRIPTION_REQUEST_MESSAGE:
    case CIM_MODIFY_INDICATION_SUBSCRIPTION_REQUEST_MESSAGE:
    case CIM_DISABLE_INDICATION_SUBSCRIPTION_REQUEST_MESSAGE:
    case CIM_PROCESS_INDICATION_REQUEST_MESSAGE:
    case CIM_HANDLE_INDICATION_REQUEST_MESSAGE:
    case CIM_NOTIFY_PROVIDER_REGISTRATION_REQUEST_MESSAGE:
    case CIM_NOTIFY_PROVIDER_TERMINATION_REQUEST_MESSAGE:
        // These messages are not handled by the ProviderManagerService
        PEGASUS_ASSERT(0);
        break;

    case CIM_DISABLE_MODULE_REQUEST_MESSAGE:
    case CIM_ENABLE_MODULE_REQUEST_MESSAGE:
    case CIM_STOP_ALL_PROVIDERS_REQUEST_MESSAGE:
        // These messages are handled specially by the ProviderManagerService
        PEGASUS_ASSERT(0);
        break;
    }

    PEGASUS_ASSERT(!providerModule.isUninitialized());
    PEGASUS_ASSERT(!provider.isUninitialized());

    PEG_METHOD_EXIT();
    return ProviderIdContainer(providerModule, provider);
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

PEGASUS_NAMESPACE_END
