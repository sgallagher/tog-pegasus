//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software, Hewlett-Packard Company, IBM,
// The Open Group, Tivoli Systems
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
//
//%/////////////////////////////////////////////////////////////////////////////

#include "ProviderManagerService.h"

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Common/OperationContext.h>
#include <Pegasus/Common/Destroyer.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/StatisticalData.h>
#include <Pegasus/Common/Logger.h>

#include <Pegasus/ProviderManager/ProviderManager.h>
#include <Pegasus/ProviderManager/Provider.h>
#include <Pegasus/ProviderManager/OperationResponseHandler.h>

#include <Pegasus/Config/ConfigManager.h>

#include <Pegasus/Server/ProviderRegistrationManager/ProviderRegistrationManager.h>

PEGASUS_NAMESPACE_BEGIN

//
// Provider module status
//
static const Uint16 _MODULE_OK       = 2;
static const Uint16 _MODULE_STOPPING = 9;
static const Uint16 _MODULE_STOPPED  = 10;


// provider manager
static ProviderManager providerManager;

ProviderManagerService::ProviderManagerService(
    ProviderRegistrationManager * providerRegistrationManager)
    : MessageQueueService(PEGASUS_QUEUENAME_PROVIDERMANAGER_CPP),
    _providerRegistrationManager(providerRegistrationManager)
{
}

ProviderManagerService::~ProviderManagerService(void)
{
}

Triad<String, String, String> _getProviderRegPair(
    const CIMInstance& pInstance, const CIMInstance& pmInstance)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "_getProviderRegPair");

    String providerName;
    String location;
    String interfaceName;

    Array<Uint16> operationalStatus;

    // get the OperationalStatus from the provider module instance
    Uint32 pos = pmInstance.findProperty(CIMName ("OperationalStatus"));

    if(pos == PEG_NOT_FOUND)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "OperationalStatus not found.");

        PEG_METHOD_EXIT();

        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "provider lookup failed.");
    }

    //
    //  ATTN-CAKG-P2-20020821: Check for null status?
    //
    pmInstance.getProperty(pos).getValue().get(operationalStatus);

    for(Uint32 i = 0; i < operationalStatus.size(); i++)
    {
        if(operationalStatus[i] == _MODULE_STOPPED ||
	   operationalStatus[i] == _MODULE_STOPPING)
        {
            PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                "Provider blocked.");

            PEG_METHOD_EXIT();

            throw PEGASUS_CIM_EXCEPTION(CIM_ERR_ACCESS_DENIED, "provider blocked.");
        }
    }

    // get the provider name from the provider instance
    pos = pInstance.findProperty(CIMName ("Name"));

    if(pos == PEG_NOT_FOUND)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Provider name not found.");

        PEG_METHOD_EXIT();

        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "provider lookup failed.");
    }

    pInstance.getProperty(pos).getValue().get(providerName);

    PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
        "providerName = " + providerName + " found.");

    // get the provider location from the provider module instance
    pos = pmInstance.findProperty(CIMName ("Location"));

    if(pos == PEG_NOT_FOUND)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Provider location not found.");

        PEG_METHOD_EXIT();

        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "provider lookup failed.");
    }

    pmInstance.getProperty(pos).getValue().get(location);

    // get the provider location from the provider module instance
    pos = pmInstance.findProperty(CIMName ("InterfaceType"));

    if (pos != PEG_NOT_FOUND)
    {
        pmInstance.getProperty(pos).getValue().get(interfaceName);

        if (String::equal(interfaceName,"C++Default") )
            interfaceName = String::EMPTY;
    }

    PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
        "location = " + location + " found.");

    String fileName;

    #ifdef PEGASUS_OS_TYPE_WINDOWS
    fileName = location + String(".dll");
    #elif defined(PEGASUS_OS_HPUX)
    fileName = ConfigManager::getHomedPath(ConfigManager::getInstance()->getCurrentValue("providerDir"));
    fileName.append(String("/lib") + location + String(".0"));
    #elif defined(PEGASUS_OS_OS400)
    fileName = location;
    #else
    fileName = ConfigManager::getHomedPath(ConfigManager::getInstance()->getCurrentValue("providerDir"));
    fileName.append(String("/lib") + location + String(".so"));
    #endif

    PEG_METHOD_EXIT();

    return(Triad<String, String, String>(fileName, providerName,interfaceName));
}
// ACTION: KS: Note that the assocClassName and ResultClassName are
// Strings here but CIMName below.  This must be corrected.
void ProviderManagerService::_lookupProviderForAssocClass(
    const CIMObjectPath & objectPath, const String& assocClassName,
    const String& resultClassName,
    Array<String>& Locations, Array<String>& providerNames,
    Array<String>& interfaceNames)
{
    Array<CIMInstance> pInstances;
    Array<CIMInstance> pmInstances;

    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "ProviderManagerService::_lookupProviderForAssocClass");

    PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
        "nameSpace = " + objectPath.getNameSpace().getString() + 
        "; className = " + objectPath.getClassName().getString());
    
    /*PEGASUS_STD(cout) << "KSTEST ProviderManagerService::_lookupProviderForAssocClass Entry"
        << " nameSpace = " << objectPath.getNameSpace().getString()
        << "; className = " << objectPath.getClassName().getString()  << PEGASUS_STD(endl);*/

    // Bugzilla 194 - Problem defined and the following is temporary patch to create either
    // NULL objects for result and assoc classes unless there is a valid input.
    
    CIMName myAssocClassName;
    if (assocClassName.size() != 0)
        myAssocClassName = assocClassName;
    
    CIMName myResultClassName;
    if (resultClassName.size() != 0)
        myResultClassName;
    
    // get the provider and provider module instance from the registration manager
    if(_providerRegistrationManager->lookupAssociationProvider(
        objectPath.getNameSpace(), objectPath.getClassName(),
        myAssocClassName, myResultClassName,
        //assocClassName, resultClassName,
        pInstances, pmInstances) == false)
        
    {
        /*PEGASUS_STD(cout) << "KSTEST ProviderManagerService_lookupProviderForAssocClass Error Rtn"
            << " nameSpace = " << objectPath.getNameSpace().getString()
            << "; className = " << objectPath.getClassName().getString()  << PEGASUS_STD(endl);*/
        
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Provider registration not found for " + objectPath.getNameSpace().getString() +
            " className " + objectPath.getClassName().getString());

        PEG_METHOD_EXIT();

        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "provider lookup failed.");
    }
    /*PEGASUS_STD(cout) << "KSTEST ProviderManagerService_lookupProviderForAssocClass provider Found"
        << " nameSpace = " << objectPath.getNameSpace().getString()
        << "; className = " << objectPath.getClassName().getString()  << PEGASUS_STD(endl);*/

    for(Uint32 i=0,n=pInstances.size(); i<n; i++)
    {
        // get the provider name from the provider instance
        Uint32 pos = pInstances[i].findProperty(CIMName ("Name"));

        String providerName, Location, interfaceName;

        if(pos == PEG_NOT_FOUND)
        {
            PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                "Provider name not found.");
        }

        pInstances[i].getProperty(pos).getValue().get(providerName);

        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "providerName = " + providerName + " found.");

        // get the provider location from the provider module instance
        pos = pmInstances[i].findProperty(CIMName ("Location"));

        if(pos == PEG_NOT_FOUND)
        {
            PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                "Provider location not found.");
        }

        pmInstances[i].getProperty(pos).getValue().get(Location);

        // get the provider location from the provider module instance
        pos = pmInstances[i].findProperty(CIMName ("InterfaceType"));

        if (pos != PEG_NOT_FOUND)
        {
            pmInstances[i].getProperty(pos).getValue().get(interfaceName);

            if (String::equal(interfaceName,"C++Default") )
                interfaceName = String::EMPTY;
        }

        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "location = " + Location + " found.");

        String fileName;

        #ifdef PEGASUS_OS_TYPE_WINDOWS
        fileName = Location + String(".dll");
        #elif defined(PEGASUS_OS_HPUX)
        fileName = ConfigManager::getHomedPath(ConfigManager::getInstance()->getCurrentValue("providerDir"));
        fileName.append(String("/lib") + Location + String(".sl"));
        #elif defined(PEGASUS_OS_OS400)
        fileName = Location;
        #else
        fileName = ConfigManager::getHomedPath(ConfigManager::getInstance()->getCurrentValue("providerDir"));
        fileName.append(String("/lib") + Location + String(".so"));
        #endif
        
        providerNames.append(providerName);
        Locations.append(fileName);
        interfaceNames.append(interfaceName);
    }

    PEG_METHOD_EXIT();

    return;
}

Triad<String, String, String> ProviderManagerService::_lookupMethodProviderForClass(
    const CIMObjectPath & objectPath,
    const CIMName & methodName)
{
    CIMInstance pInstance;
    CIMInstance pmInstance;

    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "ProviderManagerService::_lookupMethodProviderForClass");

    PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
        "nameSpace = " + objectPath.getNameSpace().getString() + 
        "; className = " + objectPath.getClassName().getString() + 
        "; methodName = " + methodName.getString());

    // get the provider and provider module instance from the registration manager
    if(_providerRegistrationManager->lookupMethodProvider(
        objectPath.getNameSpace(), objectPath.getClassName(), methodName,
        pInstance, pmInstance) == false)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Provider registration not found.");

        PEG_METHOD_EXIT();

        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "provider lookup failed.");
    }

    Triad<String, String, String> triad;

    triad = _getProviderRegPair(pInstance, pmInstance);

    PEG_METHOD_EXIT();

    return(triad);
}

Triad<String, String, String> ProviderManagerService::_lookupProviderForClass(
    const CIMObjectPath & objectPath)
{
    CIMInstance pInstance;
    CIMInstance pmInstance;

    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "ProviderManagerService::_lookupProviderForClass");

    PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
        "nameSpace = " + objectPath.getNameSpace().getString() + 
        "; className = " + objectPath.getClassName().getString());

    // get the provider and provider module instance from the registration manager
    if(_providerRegistrationManager->lookupInstanceProvider(
        objectPath.getNameSpace(), objectPath.getClassName(),
        pInstance, pmInstance) == false)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Provider registration not found.");

        PEG_METHOD_EXIT();

        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "provider lookup failed.");
    }

    Triad<String, String, String> triad;

    triad = _getProviderRegPair(pInstance, pmInstance);

    PEG_METHOD_EXIT();

    return(triad);
}

Boolean ProviderManagerService::messageOK(const Message * message)
{
    PEGASUS_ASSERT(message != 0);

    /*
    Boolean rc = false;

    switch(message->getType())
    {
    case CIM_GET_INSTANCE_REQUEST_MESSAGE:
    case CIM_ENUMERATE_INSTANCES_REQUEST_MESSAGE:
    case CIM_ENUMERATE_INSTANCE_NAMES_REQUEST_MESSAGE:
    case CIM_CREATE_INSTANCE_REQUEST_MESSAGE:
    case CIM_MODIFY_INSTANCE_REQUEST_MESSAGE:
    case CIM_DELETE_INSTANCE_REQUEST_MESSAGE:
    case CIM_GET_PROPERTY_REQUEST_MESSAGE:
    case CIM_SET_PROPERTY_REQUEST_MESSAGE:
    case CIM_INVOKE_METHOD_REQUEST_MESSAGE:
    case CIM_ENABLE_INDICATION_SUBSCRIPTION_REQUEST_MESSAGE:
    case CIM_MODIFY_INDICATION_SUBSCRIPTION_REQUEST_MESSAGE:
    case CIM_DISABLE_INDICATION_SUBSCRIPTION_REQUEST_MESSAGE:
    rc = true;

    break;
    default:
    rc = false;

    break;
    }

    return(rc);
    */

    return(MessageQueueService::messageOK(message));
}

void ProviderManagerService::handleEnqueue(void)
{
    Message * message = dequeue();

    handleEnqueue(message);
}

void ProviderManagerService::handleEnqueue(Message * message)
{
    PEGASUS_ASSERT(message != 0);

    //*FIXME* Markus
    // catch response messages that should never appear here

    //    if (message->getType() == CIM_ENUMERATE_INSTANCE_NAMES_RESPONSE_MESSAGE)
    //        abort(); // handle double provider callback !

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

    PEGASUS_ASSERT(request != 0 && request->op != 0 );

    if(request->getType() == async_messages::ASYNC_LEGACY_OP_START)
    {
        request->op->processing();
        _incomingQueue.enqueue(request->op);

        _thread_pool->allocate_and_awaken((void *)this, ProviderManagerService::handleCimOperation);
    }
    else
    {
        // pass all other operations to the default handler
        MessageQueueService::_handle_async_request(request);
    }

    PEG_METHOD_EXIT();

    return;
}

/*
PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL ProviderManagerService::handleServiceOperation(void * arg) throw()
{
    // get the service from argument
    ProviderManagerService * service = reinterpret_cast<ProviderManagerService *>(arg);

    PEGASUS_ASSERT(service != 0);

    // get message from service queue
    Message * message = service->_incomingQueue.dequeue();

    PEGASUS_ASSERT(message != 0);

    return(0);
}
*/

PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL ProviderManagerService::handleCimOperation(void * arg) throw()
{
    // get the service from argument
    ProviderManagerService * service = reinterpret_cast<ProviderManagerService *>(arg);

    PEGASUS_ASSERT(service != 0);

    if(service->_incomingQueue.size() == 0)
    {
        // thread started with no message in queue.
        return(PEGASUS_THREAD_RETURN(1));
    }

    AsyncOpNode * op = service->_incomingQueue.dequeue();

    PEGASUS_ASSERT(op != 0 );

    if(op->_request.count() == 0)
    {
        MessageQueue * queue = MessageQueue::lookup(op->_source_queue);

        PEGASUS_ASSERT(queue != 0);

        // no request in op node
        return(PEGASUS_THREAD_RETURN(1));
    }

    AsyncRequest * request = static_cast<AsyncRequest *>(op->_request.next(0));

    PEGASUS_ASSERT(request != 0);

    //PEGASUS_STD(cout) << "KSTEST ProviderManagerService 1 " << PEGASUS_STD(endl);
    
    if(request->getType() == async_messages::ASYNC_LEGACY_OP_START)
    {
        Message * legacy = static_cast<AsyncLegacyOperationStart *>(request)->get_action();

        if(legacy != 0)
        {
            Destroyer<Message> xmessage(legacy);

            // pass the request message to a handler method based on message type
            switch(legacy->getType())
            {
            case CIM_GET_CLASS_REQUEST_MESSAGE:
            case CIM_ENUMERATE_CLASSES_REQUEST_MESSAGE:
            case CIM_ENUMERATE_CLASS_NAMES_REQUEST_MESSAGE:
            case CIM_CREATE_CLASS_REQUEST_MESSAGE:
            case CIM_MODIFY_CLASS_REQUEST_MESSAGE:
            case CIM_DELETE_CLASS_REQUEST_MESSAGE:
                break;
            case CIM_GET_INSTANCE_REQUEST_MESSAGE:
                service->handleGetInstanceRequest(op, legacy);

                break;
            case CIM_ENUMERATE_INSTANCES_REQUEST_MESSAGE:
                service->handleEnumerateInstancesRequest(op, legacy);

                break;
            case CIM_ENUMERATE_INSTANCE_NAMES_REQUEST_MESSAGE:
                service->handleEnumerateInstanceNamesRequest(op, legacy);

                break;
            case CIM_CREATE_INSTANCE_REQUEST_MESSAGE:
                service->handleCreateInstanceRequest(op, legacy);

                break;
            case CIM_MODIFY_INSTANCE_REQUEST_MESSAGE:
                service->handleModifyInstanceRequest(op, legacy);

                break;
            case CIM_DELETE_INSTANCE_REQUEST_MESSAGE:
                service->handleDeleteInstanceRequest(op, legacy);

                break;
            case CIM_EXEC_QUERY_REQUEST_MESSAGE:
                service->handleExecuteQueryRequest(op, legacy);

                break;
            case CIM_ASSOCIATORS_REQUEST_MESSAGE:
                service->handleAssociatorsRequest(op, legacy);

                break;
            case CIM_ASSOCIATOR_NAMES_REQUEST_MESSAGE:
                service->handleAssociatorNamesRequest(op, legacy);

                break;
            case CIM_REFERENCES_REQUEST_MESSAGE:
                service->handleReferencesRequest(op, legacy);

                break;
            case CIM_REFERENCE_NAMES_REQUEST_MESSAGE:
                service->handleReferenceNamesRequest(op, legacy);

                break;
            case CIM_GET_PROPERTY_REQUEST_MESSAGE:
                service->handleGetPropertyRequest(op, legacy);

                break;
            case CIM_SET_PROPERTY_REQUEST_MESSAGE:
                service->handleSetPropertyRequest(op, legacy);

                break;
            case CIM_INVOKE_METHOD_REQUEST_MESSAGE:
                service->handleInvokeMethodRequest(op, legacy);

                break;
            case CIM_CREATE_SUBSCRIPTION_REQUEST_MESSAGE:
                service->handleCreateSubscriptionRequest(op, legacy);

                break;
            case CIM_MODIFY_SUBSCRIPTION_REQUEST_MESSAGE:
                service->handleModifySubscriptionRequest(op, legacy);

                break;
            case CIM_DELETE_SUBSCRIPTION_REQUEST_MESSAGE:
                service->handleDeleteSubscriptionRequest(op, legacy);

                break;
            case CIM_ENABLE_INDICATIONS_REQUEST_MESSAGE:
                service->handleEnableIndicationsRequest(op, legacy);

                break;
            case CIM_DISABLE_INDICATIONS_REQUEST_MESSAGE:
                service->handleDisableIndicationsRequest(op, legacy);

                break;
            case CIM_DISABLE_MODULE_REQUEST_MESSAGE:
                service->handleDisableModuleRequest(op, legacy);

                break;
            case CIM_ENABLE_MODULE_REQUEST_MESSAGE:
                service->handleEnableModuleRequest(op, legacy);

                break;
            case CIM_STOP_ALL_PROVIDERS_REQUEST_MESSAGE:
                service->handleStopAllProvidersRequest(op, legacy);

                break;
            default:
                // unsupported messages are ignored
                break;
            }
        }
    }
    else
    {
        // reply with a NAK
    }

    return(0);
}

void ProviderManagerService::handleGetInstanceRequest(AsyncOpNode *op, const Message *message) throw()
{
    CIMGetInstanceRequestMessage * request =
        dynamic_cast<CIMGetInstanceRequestMessage *>(const_cast<Message *>(message));

    AsyncRequest * async = static_cast<AsyncRequest *>(op->_request.next(0));

    PEGASUS_ASSERT(request != 0 && async != 0 );

    CIMGetInstanceResponseMessage * response =
        new CIMGetInstanceResponseMessage(
        request->messageId,
        CIMException(),
        request->queueIds.copyAndPop(),
        CIMInstance());

    PEGASUS_ASSERT(response != 0);

    // preserve message key
    response->setKey(request->getKey());

    // create a handler for this request
    GetInstanceResponseHandler handler(request, response);

    try
    {
        // make target object path
        CIMObjectPath objectPath(
            System::getHostName(),
            request->nameSpace,
            request->instanceName.getClassName(),
            request->instanceName.getKeyBindings());

        // get the provider file name and logical name
        Triad<String, String, String> triad =
                _lookupProviderForClass(objectPath);

        // get cached or load new provider module
        Provider provider =
                providerManager.getProvider(triad.first, triad.second, triad.third);

        // convert arguments
        OperationContext context;

        // add the user name to the context
        context.insert(IdentityContainer(request->userName));

        CIMPropertyList propertyList(request->propertyList);

        STAT_GETSTARTTIME;

        // forward request
        provider.getInstance(
            context,
            objectPath,
            request->includeQualifiers,
            request->includeClassOrigin,
            propertyList,
            handler);

        STAT_PMS_PROVIDEREND;
    }
    catch(CIMException & e)
    {
        handler.setStatus(e.getCode(), e.getMessage());
    }
    catch(Exception & e)
    {
        handler.setStatus(CIM_ERR_FAILED, e.getMessage());
    }
    catch(...)
    {
        handler.setStatus(CIM_ERR_FAILED, "Unknown error.");
    }

    AsyncLegacyOperationResult *async_result =
        new AsyncLegacyOperationResult(
        async->getKey(),
        async->getRouting(),
        op,
        response);

    _complete_op_node(op, ASYNC_OPSTATE_COMPLETE, 0, 0);
}

void ProviderManagerService::handleEnumerateInstancesRequest(AsyncOpNode *op, const Message * message) throw()
{
    CIMEnumerateInstancesRequestMessage * request =
        dynamic_cast<CIMEnumerateInstancesRequestMessage *>(const_cast<Message *>(message));

    AsyncRequest * async = static_cast<AsyncRequest *>(op->_request.next(0));

    PEGASUS_ASSERT(request != 0 && async != 0 );

    CIMEnumerateInstancesResponseMessage * response =
        new CIMEnumerateInstancesResponseMessage(
        request->messageId,
        CIMException(),
        request->queueIds.copyAndPop(),
        Array<CIMInstance>());

    PEGASUS_ASSERT(response != 0);

    // preserve message key
    response->setKey(request->getKey());

    // create a handler for this request
    EnumerateInstancesResponseHandler handler(request, response);

    try
    {
        // make target object path
        CIMObjectPath objectPath(
            System::getHostName(),
            request->nameSpace,
            request->className);

        // get the provider file name and logical name
        Triad<String, String, String> triad =
                _lookupProviderForClass(objectPath);

        // get cached or load new provider module
        Provider provider =
                providerManager.getProvider(triad.first, triad.second, triad.third);

        // convert arguments
        OperationContext context;

        // add the user name to the context
        context.insert(IdentityContainer(request->userName));

        CIMPropertyList propertyList(request->propertyList);

        STAT_GETSTARTTIME;

        provider.enumerateInstances(
            context,
            objectPath,
            request->includeQualifiers,
            request->includeClassOrigin,
            propertyList,
            handler);

        STAT_PMS_PROVIDEREND;
    }
    catch(CIMException & e)
    {
        handler.setStatus(e.getCode(), e.getMessage());
    }
    catch(Exception & e)
    {
        handler.setStatus(CIM_ERR_FAILED, e.getMessage());
    }
    catch(...)
    {
        handler.setStatus(CIM_ERR_FAILED, "Unknown error.");
    }

    AsyncLegacyOperationResult *async_result =
        new AsyncLegacyOperationResult(
        async->getKey(),
        async->getRouting(),
        op,
        response);

    _complete_op_node(op, ASYNC_OPSTATE_COMPLETE, 0, 0);
}

void ProviderManagerService::handleEnumerateInstanceNamesRequest(AsyncOpNode *op, const Message * message) throw()
{
    CIMEnumerateInstanceNamesRequestMessage * request =
        dynamic_cast<CIMEnumerateInstanceNamesRequestMessage *>(const_cast<Message *>(message));

    AsyncRequest *async = static_cast<AsyncRequest *>(op->_request.next(0));

    PEGASUS_ASSERT(request != 0 && async != 0 );

    CIMEnumerateInstanceNamesResponseMessage * response =
        new CIMEnumerateInstanceNamesResponseMessage(
        request->messageId,
        CIMException(),
        request->queueIds.copyAndPop(),
        Array<CIMObjectPath>());

    PEGASUS_ASSERT(response != 0);

    // preserve message key
    response->setKey(request->getKey());

    // create a handler for this request
    EnumerateInstanceNamesResponseHandler handler(request, response);

    // process the request
    try
    {
        // make target object path
        CIMObjectPath objectPath(
            System::getHostName(),
            request->nameSpace,
            request->className);

        // get the provider file name and logical name
        Triad<String, String, String> triad =
                _lookupProviderForClass(objectPath);

        // get cached or load new provider module
        Provider provider =
                providerManager.getProvider(triad.first, triad.second, triad.third);

        // convert arguments
        OperationContext context;

        // add the user name to the context
        context.insert(IdentityContainer(request->userName));

        STAT_GETSTARTTIME;

        provider.enumerateInstanceNames(
            context,
            objectPath,
            handler);

        STAT_PMS_PROVIDEREND;
    }
    catch(CIMException & e)
    {
        handler.setStatus(e.getCode(), e.getMessage());
    }
    catch(Exception & e)
    {
        handler.setStatus(CIM_ERR_FAILED, e.getMessage());
    }
    catch(...)
    {
        handler.setStatus(CIM_ERR_FAILED, "Unknown error.");
    }

    AsyncLegacyOperationResult *async_result =
        new AsyncLegacyOperationResult(
        async->getKey(),
        async->getRouting(),
        op,
        response);

    _complete_op_node(op, ASYNC_OPSTATE_COMPLETE, 0, 0);
}

void ProviderManagerService::handleCreateInstanceRequest(AsyncOpNode *op, const Message * message) throw()
{
    CIMCreateInstanceRequestMessage * request =
        dynamic_cast<CIMCreateInstanceRequestMessage *>(const_cast<Message *>(message));

    AsyncRequest *async = static_cast<AsyncRequest *>(op->_request.next(0));

    PEGASUS_ASSERT(request != 0 && async != 0 );

    // create response message
    CIMCreateInstanceResponseMessage * response =
        new CIMCreateInstanceResponseMessage(
        request->messageId,
        CIMException(),
        request->queueIds.copyAndPop(),
        CIMObjectPath());

    PEGASUS_ASSERT(response != 0);

    // preserve message key
    response->setKey(request->getKey());

    // create a handler for this request
    CreateInstanceResponseHandler handler(request, response);

    try
    {
        // make target object path
        CIMObjectPath objectPath(
            System::getHostName(),
            request->nameSpace,
            request->newInstance.getPath().getClassName(),
            request->newInstance.getPath().getKeyBindings());

        // get the provider file name and logical name
        Triad<String, String, String> triad =
                _lookupProviderForClass(objectPath);

        // get cached or load new provider module
        Provider provider =
                providerManager.getProvider(triad.first, triad.second, triad.third);

        // convert arguments
        OperationContext context;

        // add the user name to the context
        context.insert(IdentityContainer(request->userName));

        // forward request

        STAT_GETSTARTTIME;

        provider.createInstance(
            context,
            objectPath,
            request->newInstance,
            handler);

        STAT_PMS_PROVIDEREND;
    }
    catch(CIMException & e)
    {
        handler.setStatus(e.getCode(), e.getMessage());
    }
    catch(Exception & e)
    {
        handler.setStatus(CIM_ERR_FAILED, e.getMessage());
    }
    catch(...)
    {
        handler.setStatus(CIM_ERR_FAILED, "Unknown error.");
    }

    AsyncLegacyOperationResult *async_result =
        new AsyncLegacyOperationResult(
        async->getKey(),
        async->getRouting(),
        op,
        response);

    _complete_op_node(op, ASYNC_OPSTATE_COMPLETE, 0, 0);
}

void ProviderManagerService::handleModifyInstanceRequest(AsyncOpNode *op, const Message * message) throw()
{
    CIMModifyInstanceRequestMessage * request =
        dynamic_cast<CIMModifyInstanceRequestMessage *>(const_cast<Message *>(message));

    AsyncRequest *async = static_cast<AsyncRequest *>(op->_request.next(0));
    PEGASUS_ASSERT(request != 0 && async != 0 );

    // create response message
    CIMModifyInstanceResponseMessage * response =
        new CIMModifyInstanceResponseMessage(
        request->messageId,
        CIMException(),
        request->queueIds.copyAndPop());

    PEGASUS_ASSERT(response != 0);

    // preserve message key
    response->setKey(request->getKey());

    // create a handler for this request
    ModifyInstanceResponseHandler handler(request, response);

    try
    {
        // make target object path
        CIMObjectPath objectPath(
            System::getHostName(),
            request->nameSpace,
            request->modifiedInstance.getPath ().getClassName(),
            request->modifiedInstance.getPath ().getKeyBindings());

        // get the provider file name and logical name
        Triad<String, String, String> triad =
                _lookupProviderForClass(objectPath);

        // get cached or load new provider module
        Provider provider =
                providerManager.getProvider(triad.first, triad.second, triad.third);

        // convert arguments
        OperationContext context;

        // add the user name to the context
        context.insert(IdentityContainer(request->userName));

        CIMPropertyList propertyList(request->propertyList);

        // forward request
        STAT_GETSTARTTIME;

        provider.modifyInstance(
            context,
            objectPath,
            request->modifiedInstance,
            request->includeQualifiers,
            propertyList,
            handler);

        STAT_PMS_PROVIDEREND;
    }
    catch(CIMException & e)
    {
        handler.setStatus(e.getCode(), e.getMessage());
    }
    catch(Exception & e)
    {
        handler.setStatus(CIM_ERR_FAILED, e.getMessage());
    }
    catch(...)
    {
        handler.setStatus(CIM_ERR_FAILED, "Unknown error.");
    }

    AsyncLegacyOperationResult *async_result =
        new AsyncLegacyOperationResult(
        async->getKey(),
        async->getRouting(),
        op,
        response);

    _complete_op_node(op, ASYNC_OPSTATE_COMPLETE, 0, 0);
}

void ProviderManagerService::handleDeleteInstanceRequest(AsyncOpNode *op, const Message * message) throw()
{
    CIMDeleteInstanceRequestMessage * request =
        dynamic_cast<CIMDeleteInstanceRequestMessage *>(const_cast<Message *>(message));

    AsyncRequest *async = static_cast<AsyncRequest *>(op->_request.next(0));

    PEGASUS_ASSERT(request != 0 && async != 0 );

    // create response message
    CIMDeleteInstanceResponseMessage * response =
        new CIMDeleteInstanceResponseMessage(
        request->messageId,
        CIMException(),
        request->queueIds.copyAndPop());

    PEGASUS_ASSERT(response != 0);

    // preserve message key
    response->setKey(request->getKey());

    // create a handler for this request
    DeleteInstanceResponseHandler handler(request, response);

    try
    {
        // make target object path
        CIMObjectPath objectPath(
            System::getHostName(),
            request->nameSpace,
            request->instanceName.getClassName(),
            request->instanceName.getKeyBindings());

        // get the provider file name and logical name
        Triad<String, String, String> triad =
                _lookupProviderForClass(objectPath);

        // get cached or load new provider module
        Provider provider =
            providerManager.getProvider(triad.first, triad.second, triad.third);

        // convert arguments
        OperationContext context;

        context.insert(IdentityContainer(request->userName));

        // forward request
        STAT_GETSTARTTIME;

        provider.deleteInstance(
            context,
            objectPath,
            handler);

        STAT_PMS_PROVIDEREND;
    }
    catch(CIMException & e)
    {
        handler.setStatus(e.getCode(), e.getMessage());
    }
    catch(Exception & e)
    {
        handler.setStatus(CIM_ERR_FAILED, e.getMessage());
    }
    catch(...)
    {
        handler.setStatus(CIM_ERR_FAILED, "Unknown error.");
    }

    AsyncLegacyOperationResult *async_result =
        new AsyncLegacyOperationResult(
        async->getKey(),
        async->getRouting(),
        op,
        response);

    _complete_op_node(op, ASYNC_OPSTATE_COMPLETE, 0, 0);
}

void ProviderManagerService::handleExecuteQueryRequest(AsyncOpNode *op, const Message * message) throw()
{
    CIMExecQueryRequestMessage * request =
        dynamic_cast<CIMExecQueryRequestMessage *>(const_cast<Message *>(message));

    AsyncRequest *async = static_cast<AsyncRequest *>(op->_request.next(0));

    PEGASUS_ASSERT(request != 0 && async != 0 );

    Array<CIMObject> cimObjects;

    CIMExecQueryResponseMessage * response =
        new CIMExecQueryResponseMessage(
        request->messageId,
        PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "not implemented"),
        request->queueIds.copyAndPop(),
        cimObjects);

    PEGASUS_ASSERT(response != 0);

    // preserve message key
    response->setKey(request->getKey());

    AsyncLegacyOperationResult *async_result =
        new AsyncLegacyOperationResult(
        async->getKey(),
        async->getRouting(),
        op,
        response);

    _complete_op_node(op, ASYNC_OPSTATE_COMPLETE, 0, 0);
}

void ProviderManagerService::handleAssociatorsRequest(AsyncOpNode *op, const Message * message) throw()
{
    CIMAssociatorsRequestMessage * request =
        dynamic_cast<CIMAssociatorsRequestMessage *>(const_cast<Message *>(message));

    AsyncRequest *async = static_cast<AsyncRequest *>(op->_request.next(0));

    PEGASUS_ASSERT(request != 0 && async != 0);

    Array<CIMObject> cimObjects;
    /****************************************** Old Code replaced
    CIMAssociatorsResponseMessage * response =
        new CIMAssociatorsResponseMessage(
        request->messageId,
        PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "not implemented"),
        request->queueIds.copyAndPop(),
        cimObjects);

    PEGASUS_ASSERT(response != 0);

    // preserve message key
    response->setKey(request->getKey());

    AsyncLegacyOperationResult *async_result =
        new AsyncLegacyOperationResult(
        async->getKey(),
        async->getRouting(),
        op,
        response);
   ***********************************************************************/
   // Bugzilla Report xxx, 16 Feb 2003, Associators function in Provider Manager service 
   // returns not implemented.
       CIMAssociatorsResponseMessage * response =
           new CIMAssociatorsResponseMessage(
           request->messageId,
           CIMException(),
           request->queueIds.copyAndPop(),
           cimObjects);

       PEGASUS_ASSERT(response != 0);

       // preserve message key
       response->setKey(request->getKey());
       // create a handler for this request
       AssociatorsResponseHandler handler(request, response);

       // process the request
       try
       {
           // make target object path
           CIMObjectPath objectPath(
               System::getHostName(),
               request->nameSpace,
               request->objectName.getClassName());

           objectPath.setKeyBindings(request->objectName.getKeyBindings());

           // get the provider file name and logical name
           Array<String> first;
           Array<String> second;
           Array<String> third;

           _lookupProviderForAssocClass(objectPath,
           //                             request->associationClass,
           //                             request->resultClass,
                                        String::EMPTY,
                                        String::EMPTY,
                                        first, second, third);

           for(Uint32 i=0,n=first.size(); i<n; i++)
           {
               // get cached or load new provider module
               Provider provider =
                  providerManager.getProvider(first[i], second[i], third[i]);

               // convert arguments
               OperationContext context;

               // add the user name to the context
               context.insert(IdentityContainer(request->userName));

               STAT_GETSTARTTIME;

               provider.associators(
                   context,
                   objectPath,
                   request->assocClass,
                   request->resultClass,
                   request->role,
                   request->resultRole,
                   request->includeQualifiers,
                   request->includeClassOrigin,
                   request->propertyList.getPropertyNameArray(),
                   handler);

               STAT_PMS_PROVIDEREND;

           } // end for loop
       }
       catch(CIMException & e)
       {
           handler.setStatus(e.getCode(), e.getMessage());
       }
       catch(Exception & e)
       {
           handler.setStatus(CIM_ERR_FAILED, e.getMessage());
       }
       catch(...)
       {
           handler.setStatus(CIM_ERR_FAILED, "Unknown error.");
       }

       AsyncLegacyOperationResult *async_result =
           new AsyncLegacyOperationResult(
           async->getKey(),
           async->getRouting(),
           op,
           response);


   //****************************************** Replacement to here
    _complete_op_node(op, ASYNC_OPSTATE_COMPLETE, 0, 0);
}

void ProviderManagerService::handleAssociatorNamesRequest(AsyncOpNode *op, const Message * message) throw()
{
    CIMAssociatorNamesRequestMessage * request =
        dynamic_cast<CIMAssociatorNamesRequestMessage *>(const_cast<Message *>(message));

    AsyncRequest *async = static_cast<AsyncRequest *>(op->_request.next(0));

    PEGASUS_ASSERT(request != 0 && async != 0 );

    Array<CIMObjectPath> cimReferences;

    CIMAssociatorNamesResponseMessage * response =
        new CIMAssociatorNamesResponseMessage(
        request->messageId,
        CIMException(),
        request->queueIds.copyAndPop(),
        cimReferences);

    PEGASUS_ASSERT(response != 0);

    // preserve message key
    response->setKey(request->getKey());

    // create a handler for this request
    AssociatorNamesResponseHandler handler(request, response);

    // process the request
    try
    {
        // make target object path
        CIMObjectPath objectPath(
            System::getHostName(),
            request->nameSpace,
            request->objectName.getClassName());

        objectPath.setKeyBindings(request->objectName.getKeyBindings());

        // get the provider file name and logical name
        Array<String> first;
        Array<String> second;
        Array<String> third;

        _lookupProviderForAssocClass(objectPath,
        //                             request->associationClass,
        //                             request->resultClass,
                                     String::EMPTY,
                                     String::EMPTY,
                                     first, second, third);

        for(Uint32 i=0,n=first.size(); i<n; i++)
        {
            // get cached or load new provider module
            Provider provider =
               providerManager.getProvider(first[i], second[i], third[i]);

            // convert arguments
            OperationContext context;

            // add the user name to the context
            context.insert(IdentityContainer(request->userName));

            STAT_GETSTARTTIME;

            provider.associatorNames(
                context,
                objectPath,
                request->assocClass,
                request->resultClass,
                request->role,
                request->resultRole,
                handler);

            STAT_PMS_PROVIDEREND;

        } // end for loop
    }
    catch(CIMException & e)
    {
        handler.setStatus(e.getCode(), e.getMessage());
    }
    catch(Exception & e)
    {
        handler.setStatus(CIM_ERR_FAILED, e.getMessage());
    }
    catch(...)
    {
        handler.setStatus(CIM_ERR_FAILED, "Unknown error.");
    }

    AsyncLegacyOperationResult *async_result =
        new AsyncLegacyOperationResult(
        async->getKey(),
        async->getRouting(),
        op,
        response);

    _complete_op_node(op, ASYNC_OPSTATE_COMPLETE, 0, 0);
}

void ProviderManagerService::handleReferencesRequest(AsyncOpNode *op, const Message * message) throw()
{
    CIMReferencesRequestMessage * request =
        dynamic_cast<CIMReferencesRequestMessage *>(const_cast<Message *>(message));

    AsyncRequest *async = static_cast<AsyncRequest *>(op->_request.next(0));

    PEGASUS_ASSERT(request != 0 && async != 0 );

    Array<CIMObject> cimObjects;

    CIMReferencesResponseMessage * response =
        new CIMReferencesResponseMessage(
        request->messageId,
        CIMException(),
        request->queueIds.copyAndPop(),
        cimObjects);

    PEGASUS_ASSERT(response != 0);

    // preserve message key
    response->setKey(request->getKey());
    // create a handler for this request
    ReferencesResponseHandler handler(request, response);

    // process the request
    try
    {
        // make target object path
        CIMObjectPath objectPath(
            System::getHostName(),
            request->nameSpace,
            request->objectName.getClassName());

        objectPath.setKeyBindings(request->objectName.getKeyBindings());

        // get the provider file name and logical name
        Array<String> first;
        Array<String> second;
        Array<String> third;

        _lookupProviderForAssocClass(objectPath,
        //                             request->associationClass,
        //                             request->resultClass,
                                     String::EMPTY,
                                     String::EMPTY,
                                     first, second, third);

        for(Uint32 i=0,n=first.size(); i<n; i++)
        {
            // get cached or load new provider module
            Provider provider =
               providerManager.getProvider(first[i], second[i], third[i]);

            // convert arguments
            OperationContext context;

            // add the user name to the context
            context.insert(IdentityContainer(request->userName));

            STAT_GETSTARTTIME;

            provider.references(
                context,
                objectPath,
                request->resultClass,
                request->role,
                request->includeQualifiers,
                request->includeClassOrigin,
                request->propertyList.getPropertyNameArray(),
                handler);

            STAT_PMS_PROVIDEREND;

        } // end for loop
    }
    catch(CIMException & e)
    {
        handler.setStatus(e.getCode(), e.getMessage());
    }
    catch(Exception & e)
    {
        handler.setStatus(CIM_ERR_FAILED, e.getMessage());
    }
    catch(...)
    {
        handler.setStatus(CIM_ERR_FAILED, "Unknown error.");
    }

    AsyncLegacyOperationResult *async_result =
        new AsyncLegacyOperationResult(
        async->getKey(),
        async->getRouting(),
        op,
        response);

    _complete_op_node(op, ASYNC_OPSTATE_COMPLETE, 0, 0);
}

void ProviderManagerService::handleReferenceNamesRequest(AsyncOpNode *op, const Message * message) throw()
{
    CIMReferenceNamesRequestMessage * request =
        dynamic_cast<CIMReferenceNamesRequestMessage *>(const_cast<Message *>(message));

    AsyncRequest *async = static_cast<AsyncRequest *>(op->_request.next(0));

    PEGASUS_ASSERT(request != 0 && async != 0 );

    Array<CIMObjectPath> cimReferences;
    
    CIMReferenceNamesResponseMessage * response =
        new CIMReferenceNamesResponseMessage(
        request->messageId,
        CIMException(),
        request->queueIds.copyAndPop(),
        cimReferences);

    // preserve message key
    response->setKey(request->getKey());

    // create a handler for this request
    ReferenceNamesResponseHandler handler(request, response);

    // process the request
    try
    {
        // make target object path
        CIMObjectPath objectPath(
            System::getHostName(),
            request->nameSpace,
            request->objectName.getClassName());

        objectPath.setKeyBindings(request->objectName.getKeyBindings());
        
        // get the provider file name and logical name
        Array<String> first;
        Array<String> second;
        Array<String> third;

        _lookupProviderForAssocClass(objectPath,
        //                             request->associationClass,
        //                             request->resultClass,
                                     String::EMPTY,
                                     String::EMPTY,
                                     first, second, third);

        for(Uint32 i=0,n=first.size(); i<n; i++)
        {
            // get cached or load new provider module
            Provider provider =
               providerManager.getProvider(first[i], second[i], third[i]);

            // convert arguments
            OperationContext context;

            // add the user name to the context
            context.insert(IdentityContainer(request->userName));

            STAT_GETSTARTTIME;

            provider.referenceNames(
                context,
                objectPath,
                request->resultClass,
                request->role,
                handler);

            STAT_PMS_PROVIDEREND;

        } // end for loop
    }
    catch(CIMException & e)
    {
        handler.setStatus(e.getCode(), e.getMessage());
    }
    catch(Exception & e)
    {
        handler.setStatus(CIM_ERR_FAILED, e.getMessage());
    }
    catch(...)
    {
        handler.setStatus(CIM_ERR_FAILED, "Unknown error.");
    }

    AsyncLegacyOperationResult *async_result =
        new AsyncLegacyOperationResult(
        async->getKey(),
        async->getRouting(),
        op,
        response);

    _complete_op_node(op, ASYNC_OPSTATE_COMPLETE, 0, 0);
}

void ProviderManagerService::handleGetPropertyRequest(AsyncOpNode *op, const Message * message) throw()
{
    CIMGetPropertyRequestMessage * request =
        dynamic_cast<CIMGetPropertyRequestMessage *>(const_cast<Message *>(message));

    AsyncRequest *async = static_cast<AsyncRequest *>(op->_request.next(0));

    PEGASUS_ASSERT(request != 0 && async != 0 );

    CIMValue cimValue;

    // create response message
    CIMGetPropertyResponseMessage * response =
        new CIMGetPropertyResponseMessage(
        request->messageId,
        CIMException(),
        request->queueIds.copyAndPop(),
        cimValue);

    PEGASUS_ASSERT(response != 0);

    // preserve message key
    response->setKey(request->getKey());

    GetPropertyResponseHandler handler(request, response);

    try
    {
    	// make target object path
    	CIMObjectPath objectPath(
    	    System::getHostName(),
    	    request->nameSpace,
    	    request->instanceName.getClassName(),
    	    request->instanceName.getKeyBindings());

    	// get the provider file name and logical name
    	Triad<String, String, String> triad =
                _lookupProviderForClass(objectPath);

    	// get cached or load new provider module
    	Provider provider =
                providerManager.getProvider(triad.first, triad.second, triad.third);

    	// convert arguments
    	OperationContext context;

    	// add the user name to the context
    	context.insert(IdentityContainer(request->userName));

        CIMName propertyName = request->propertyName;

        STAT_GETSTARTTIME;

        // forward request
        provider.getProperty(
            context,
            objectPath,
            propertyName,
            handler);

        STAT_PMS_PROVIDEREND;
    }
    catch(CIMException & e)
    {
        handler.setStatus(e.getCode(), e.getMessage());
    }
    catch(Exception & e)
    {
        handler.setStatus(CIM_ERR_FAILED, e.getMessage());
    }
    catch(...)
    {
        handler.setStatus(CIM_ERR_FAILED, "Unknown error.");
    }

    AsyncLegacyOperationResult *async_result =
        new AsyncLegacyOperationResult(
        async->getKey(),
        async->getRouting(),
        op,
        response);

    _complete_op_node(op, ASYNC_OPSTATE_COMPLETE, 0, 0);
}

void ProviderManagerService::handleSetPropertyRequest(AsyncOpNode *op, const Message * message) throw()
{
    CIMSetPropertyRequestMessage * request =
        dynamic_cast<CIMSetPropertyRequestMessage *>(const_cast<Message *>(message));

    AsyncRequest *async = static_cast<AsyncRequest *>(op->_request.next(0));

    PEGASUS_ASSERT(request != 0 && async != 0);

    // create response message
    CIMSetPropertyResponseMessage * response =
        new CIMSetPropertyResponseMessage(
        request->messageId,
        CIMException(),
        request->queueIds.copyAndPop());

    PEGASUS_ASSERT(response != 0);

    // preserve message key
    response->setKey(request->getKey());

    SetPropertyResponseHandler handler(request, response);

    try
    {
    	// make target object path
    	CIMObjectPath objectPath(
    	    System::getHostName(),
    	    request->nameSpace,
    	    request->instanceName.getClassName(),
    	    request->instanceName.getKeyBindings());

    	// get the provider file name and logical name
    	Triad<String, String, String> triad =
                _lookupProviderForClass(objectPath);

    	// get cached or load new provider module
    	Provider provider =
                providerManager.getProvider(triad.first, triad.second, triad.third);

    	// convert arguments
    	OperationContext context;

    	// add the user name to the context
    	context.insert(IdentityContainer(request->userName));

    	CIMName propertyName = request->propertyName;
    	CIMValue propertyValue = request->newValue;

        STAT_GETSTARTTIME;

        // forward request
        provider.setProperty(
            context,
            objectPath,
            propertyName,
            propertyValue,
            handler);

        STAT_PMS_PROVIDEREND;
    }
    catch(CIMException & e)
    {
        handler.setStatus(e.getCode(), e.getMessage());
    }
    catch(Exception & e)
    {
        handler.setStatus(CIM_ERR_FAILED, e.getMessage());
    }
    catch(...)
    {
        handler.setStatus(CIM_ERR_FAILED, "Unknown error.");
    }

    AsyncLegacyOperationResult *async_result =
        new AsyncLegacyOperationResult(
        async->getKey(),
        async->getRouting(),
        op,
        response);

    _complete_op_node(op, ASYNC_OPSTATE_COMPLETE, 0, 0);
}

void ProviderManagerService::handleInvokeMethodRequest(AsyncOpNode *op, const Message * message) throw()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "ProviderManagerService::handleInvokeMethodRequest");

    CIMInvokeMethodRequestMessage * request =
        dynamic_cast<CIMInvokeMethodRequestMessage *>(const_cast<Message *>(message));

    AsyncRequest *async = static_cast<AsyncRequest *>(op->_request.next(0));

    PEGASUS_ASSERT(request != 0 && async != 0 );

    // create response message
    CIMInvokeMethodResponseMessage * response =
        new CIMInvokeMethodResponseMessage(
        request->messageId,
        CIMException(),
        request->queueIds.copyAndPop(),
        CIMValue(),
        Array<CIMParamValue>(),
        request->methodName);

    PEGASUS_ASSERT(response != 0);

    // propagate message key
    response->setKey(request->getKey());

    // create a handler for this request
    InvokeMethodResponseHandler handler(request, response);

    try
    {
        // make target object path
        CIMObjectPath objectPath(
            System::getHostName(),
            request->nameSpace,
            request->instanceName.getClassName(),
            request->instanceName.getKeyBindings());

        // get the provider file name and logical name
        Triad<String, String, String> triad =
            _lookupMethodProviderForClass(objectPath, request->methodName);

	// get cached or load new provider module
	Provider provider =
            providerManager.getProvider(triad.first, triad.second, triad.third);

        // convert arguments
        OperationContext context;

        // add the user name to the context
        context.insert(IdentityContainer(request->userName));

        CIMObjectPath instanceReference(request->instanceName);

        // ATTN: propagate namespace
        instanceReference.setNameSpace(request->nameSpace);

        // forward request
        STAT_GETSTARTTIME;

        provider.invokeMethod(
            context,
            instanceReference,
            request->methodName,
            request->inParameters,
            handler);

        STAT_PMS_PROVIDEREND;
    }
    catch(CIMException & e)
    {
        handler.setStatus(e.getCode(), e.getMessage());
    }
    catch(Exception & e)
    {
        handler.setStatus(CIM_ERR_FAILED, e.getMessage());
    }
    catch(...)
    {
        handler.setStatus(CIM_ERR_FAILED, "Unknown error.");
    }

    AsyncLegacyOperationResult *async_result =
        new AsyncLegacyOperationResult(
        async->getKey(),
        async->getRouting(),
        op,
        response);

    _complete_op_node(op, ASYNC_OPSTATE_COMPLETE, 0, 0);

    PEG_METHOD_EXIT();
}

void ProviderManagerService::handleCreateSubscriptionRequest(AsyncOpNode *op, const Message * message) throw()
{
    CIMCreateSubscriptionRequestMessage * request =
        dynamic_cast<CIMCreateSubscriptionRequestMessage *>(const_cast<Message *>(message));

    AsyncRequest *async = static_cast<AsyncRequest *>(op->_request.next(0));

    PEGASUS_ASSERT(request != 0 && async != 0 );

    CIMCreateSubscriptionResponseMessage * response =
        new CIMCreateSubscriptionResponseMessage(
        request->messageId,
        CIMException(),
        request->queueIds.copyAndPop());

    PEGASUS_ASSERT(response != 0);

    // preserve message key
    response->setKey(request->getKey());

    OperationResponseHandler handler(request, response);

    try
    {
	// get the provider file name and logical name
	Triad<String, String, String> triad =
	    _getProviderRegPair(request->provider, request->providerModule);

	// get cached or load new provider module
	Provider provider =
            providerManager.getProvider(triad.first, triad.second, triad.third);


	// convert arguments
	OperationContext context;

	context.insert(IdentityContainer(request->userName));
	
	CIMObjectPath subscriptionName = request->subscriptionInstance.getPath();
	
	Array<CIMObjectPath> classNames;

	for(Uint32 i = 0, n = request->classNames.size(); i < n; i++)
	{
	    CIMObjectPath className(
		System::getHostName(),
		request->nameSpace,
		request->classNames[i]);

	    classNames.append(className);
	}

	CIMPropertyList propertyList = request->propertyList;
	
	Uint16 repeatNotificationPolicy = request->repeatNotificationPolicy;
	
	provider.createSubscription(
	    context,
	    subscriptionName,
	    classNames,
	    propertyList,
	    repeatNotificationPolicy);
    }
    catch(CIMException & e)
    {
        handler.setStatus(e.getCode(), e.getMessage());
    }
    catch(Exception & e)
    {
        handler.setStatus(CIM_ERR_FAILED, e.getMessage());
    }
    catch(...)
    {
        handler.setStatus(CIM_ERR_FAILED, "Unknown Error");
    }

    AsyncLegacyOperationResult *async_result =
        new AsyncLegacyOperationResult(
        async->getKey(),
        async->getRouting(),
        op,
        response);

    _complete_op_node(op, ASYNC_OPSTATE_COMPLETE, 0, 0);
}

void ProviderManagerService::handleModifySubscriptionRequest(AsyncOpNode *op, const Message * message) throw()
{
    CIMModifySubscriptionRequestMessage * request =
        dynamic_cast<CIMModifySubscriptionRequestMessage *>(const_cast<Message *>(message));

    AsyncRequest *async = static_cast<AsyncRequest *>(op->_request.next(0));

    PEGASUS_ASSERT(request != 0 && async != 0 );

    CIMModifySubscriptionResponseMessage * response =
        new CIMModifySubscriptionResponseMessage(
        request->messageId,
        CIMException(),
        request->queueIds.copyAndPop());

    PEGASUS_ASSERT(response != 0);

    // preserve message key
    response->setKey(request->getKey());

    OperationResponseHandler handler(request, response);

    try
    {
	// get the provider file name and logical name
	Triad<String, String, String> triad =
	    _getProviderRegPair(request->provider, request->providerModule);

	// get cached or load new provider module
	Provider provider =
            providerManager.getProvider(triad.first, triad.second, triad.third);

        // convert arguments
        OperationContext context;

        context.insert(IdentityContainer(request->userName));

        CIMObjectPath subscriptionName = request->subscriptionInstance.getPath();

        Array<CIMObjectPath> classNames;

        for(Uint32 i = 0, n = request->classNames.size(); i < n; i++)
        {
            CIMObjectPath className(
                System::getHostName(),
                request->nameSpace,
                request->classNames[i]);

            classNames.append(className);
        }

        CIMPropertyList propertyList = request->propertyList;

        Uint16 repeatNotificationPolicy = request->repeatNotificationPolicy;

        provider.modifySubscription(
            context,
            subscriptionName,
            classNames,
            propertyList,
            repeatNotificationPolicy);
    }
    catch(CIMException & e)
    {
        handler.setStatus(e.getCode(), e.getMessage());
    }
    catch(Exception & e)
    {
        handler.setStatus(CIM_ERR_FAILED, e.getMessage());
    }
    catch(...)
    {
        handler.setStatus(CIM_ERR_FAILED, "Unknown Error");
    }

    AsyncLegacyOperationResult *async_result =
        new AsyncLegacyOperationResult(
        async->getKey(),
        async->getRouting(),
        op,
        response);

    _complete_op_node(op, ASYNC_OPSTATE_COMPLETE, 0, 0);
}

void ProviderManagerService::handleDeleteSubscriptionRequest(AsyncOpNode *op, const Message * message) throw()
{
    CIMDeleteSubscriptionRequestMessage * request =
        dynamic_cast<CIMDeleteSubscriptionRequestMessage *>(const_cast<Message *>(message));

    AsyncRequest *async = static_cast<AsyncRequest *>(op->_request.next(0));

    PEGASUS_ASSERT(request != 0 && async != 0 );

    CIMDeleteSubscriptionResponseMessage * response =
        new CIMDeleteSubscriptionResponseMessage(
        request->messageId,
        CIMException(),
        request->queueIds.copyAndPop());

    PEGASUS_ASSERT(response != 0);

    // preserve message key
    response->setKey(request->getKey());

    OperationResponseHandler handler(request, response);

    try
    {
	// get the provider file name and logical name
	Triad<String, String, String> triad =
	    _getProviderRegPair(request->provider, request->providerModule);

	// get cached or load new provider module
	Provider provider =
            providerManager.getProvider(triad.first, triad.second, triad.third);

        // convert arguments
        OperationContext context;

        context.insert(IdentityContainer(request->userName));

        CIMObjectPath subscriptionName = request->subscriptionInstance.getPath();

        Array<CIMObjectPath> classNames;

        for(Uint32 i = 0, n = request->classNames.size(); i < n; i++)
        {
            CIMObjectPath className(
                System::getHostName(),
                request->nameSpace,
                request->classNames[i]);

            classNames.append(className);
        }

        provider.deleteSubscription(
            context,
            subscriptionName,
            classNames);
    }
    catch(CIMException & e)
    {
        handler.setStatus(e.getCode(), e.getMessage());
    }
    catch(Exception & e)
    {
        handler.setStatus(CIM_ERR_FAILED, e.getMessage());
    }
    catch(...)
    {
        handler.setStatus(CIM_ERR_FAILED, "Unknown Error");
    }

    AsyncLegacyOperationResult *async_result =
        new AsyncLegacyOperationResult(
        async->getKey(),
        async->getRouting(),
        op,
        response);

    _complete_op_node(op, ASYNC_OPSTATE_COMPLETE, 0, 0);
}

void ProviderManagerService::handleEnableIndicationsRequest(AsyncOpNode *op, const Message * message) throw()
{
    CIMEnableIndicationsRequestMessage * request =
        dynamic_cast<CIMEnableIndicationsRequestMessage *>(const_cast<Message *>(message));

    AsyncRequest *async = static_cast<AsyncRequest *>(op->_request.next(0));

    PEGASUS_ASSERT(request != 0 && async != 0 );

    CIMEnableIndicationsResponseMessage * response =
        new CIMEnableIndicationsResponseMessage(
        request->messageId,
        CIMException(),
        request->queueIds.copyAndPop());

    PEGASUS_ASSERT(response != 0);

    // preserve message key
    response->setKey(request->getKey());

    response->dest = request->queueIds.top();


    EnableIndicationsResponseHandler *handler = 
       new EnableIndicationsResponseHandler(request, response, this);

    try
    {
       // get the provider file name and logical name
       Triad<String, String, String> triad =
	  _getProviderRegPair(request->provider, request->providerModule);
	  
       // get cached or load new provider module
       Provider provider =
	  providerManager.getProvider(triad.first, triad.second, triad.third);

       provider.enableIndications(*handler);


       // if no exception, store the handler so it is persistent for as 
       // long as the provider has indications enabled. 
       _insertEntry(provider, handler);
    }
    catch(CIMException & e)
    {
       response->cimException = CIMException(e);
    }
    catch(Exception & e)
    {
       response->cimException = CIMException(CIM_ERR_FAILED, "Internal Error");
    }
    catch(...)
    {
       response->cimException = CIMException(CIM_ERR_FAILED, "Unknown Error");
    }
       
    AsyncLegacyOperationResult *async_result =
        new AsyncLegacyOperationResult(
        async->getKey(),
        async->getRouting(),
        op,
        response);

    _complete_op_node(op, ASYNC_OPSTATE_COMPLETE, 0, 0);
}

void ProviderManagerService::handleDisableIndicationsRequest(AsyncOpNode *op, const Message * message) throw()
{
    CIMDisableIndicationsRequestMessage * request =
        dynamic_cast<CIMDisableIndicationsRequestMessage *>(const_cast<Message *>(message));

    AsyncRequest *async = static_cast<AsyncRequest *>(op->_request.next(0));

    PEGASUS_ASSERT(request != 0 && async != 0 );

    CIMDisableIndicationsResponseMessage * response =
        new CIMDisableIndicationsResponseMessage(
        request->messageId,
        CIMException(),
        request->queueIds.copyAndPop());

    PEGASUS_ASSERT(response != 0);

    // preserve message key
    response->setKey(request->getKey());

    OperationResponseHandler handler(request, response);

    try
    {
	// get the provider file name and logical name
	Triad<String, String, String> triad =
	    _getProviderRegPair(request->provider, request->providerModule);

	// get cached or load new provider module
	Provider provider =
            providerManager.getProvider(triad.first, triad.second, triad.third);

        provider.disableIndications();
	delete _removeEntry(_generateKey(provider));
    }

    catch(CIMException & e)
    {
       response->cimException = CIMException(e);
    }
    catch(Exception & e)
    {
       response->cimException = CIMException(CIM_ERR_FAILED, "Internal Error");
    }
    catch(...)
    {
       response->cimException = CIMException(CIM_ERR_FAILED, "Unknown Error");
    }

    AsyncLegacyOperationResult *async_result =
        new AsyncLegacyOperationResult(
        async->getKey(),
        async->getRouting(),
        op,
        response);

    _complete_op_node(op, ASYNC_OPSTATE_COMPLETE, 0, 0);
}

void ProviderManagerService::handleDisableModuleRequest(AsyncOpNode *op, const Message * message) throw()
{
    CIMDisableModuleRequestMessage * request =
        dynamic_cast<CIMDisableModuleRequestMessage *>(const_cast<Message *>(message));

    AsyncRequest *async = static_cast<AsyncRequest *>(op->_request.next(0));

    PEGASUS_ASSERT(request != 0 && async != 0);

    // get provider module name
    String moduleName;
    CIMInstance mInstance = request->providerModule;
    Uint32 pos = mInstance.findProperty(CIMName ("Name"));

    if(pos != PEG_NOT_FOUND)
    {
    	mInstance.getProperty(pos).getValue().get(moduleName);
    }

    //
    // get operational status
    //
    Array<Uint16> operationalStatus;
    Uint32 pos2 = mInstance.findProperty(CIMName ("OperationalStatus"));

    if (pos2 != PEG_NOT_FOUND)
    {
        //
        //  ATTN-CAKG-P2-20020821: Check for null status?
        //
        mInstance.getProperty(pos2).getValue().get(operationalStatus);
    }

    //
    // update module status from OK to Stopping
    //
    for (Uint32 i=0, n = operationalStatus.size(); i < n; i++)
    {
        if (operationalStatus[i] == _MODULE_OK)
        {
            operationalStatus.remove(i);
        }
    }
    operationalStatus.append(_MODULE_STOPPING);

    if(_providerRegistrationManager->setProviderModuleStatus
        (moduleName, operationalStatus) == false)
    {
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "set module status failed.");
    }

    Array<CIMInstance> _pInstances = request->providers;

    for(Uint32 i = 0, n = _pInstances.size(); i < n; i++)
    {
	// get the provider file name and logical name
	Triad<String, String, String> triad =
            _getProviderRegPair(_pInstances[i], mInstance);

        providerManager.unloadProvider(triad.first, triad.second);
    }

    // update module status from Stopping to Stopped
    for(Uint32 i=0, n = operationalStatus.size(); i < n; i++)
    {
        if (operationalStatus[i] == _MODULE_STOPPING)
        {
            operationalStatus.remove(i);
        }
    }
    operationalStatus.append(_MODULE_STOPPED);

    if(_providerRegistrationManager->setProviderModuleStatus
        (moduleName, operationalStatus) == false)
    {
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "set module status failed.");
    }

    CIMDisableModuleResponseMessage * response =
        new CIMDisableModuleResponseMessage(
        request->messageId,
        CIMException(),
        request->queueIds.copyAndPop(),
        operationalStatus);

    PEGASUS_ASSERT(response != 0);

    // preserve message key
    response->setKey(request->getKey());

    AsyncLegacyOperationResult *async_result =
        new AsyncLegacyOperationResult(
        async->getKey(),
        async->getRouting(),
        op,
        response);

    _complete_op_node(op, ASYNC_OPSTATE_COMPLETE, 0, 0);
}

void ProviderManagerService::handleEnableModuleRequest(AsyncOpNode *op, const Message * message) throw()
{
    CIMEnableModuleRequestMessage * request =
        dynamic_cast<CIMEnableModuleRequestMessage *>(const_cast<Message *>(message));

    AsyncRequest * async = static_cast<AsyncRequest *>(op->_request.next(0));

    PEGASUS_ASSERT(request != 0 && async != 0 );

    //
    // get module status
    //
    CIMInstance mInstance = request->providerModule;
    Array<Uint16> operationalStatus;
    Uint32 pos = mInstance.findProperty(CIMName ("OperationalStatus"));

    if (pos != PEG_NOT_FOUND)
    {
        //
        //  ATTN-CAKG-P2-20020821: Check for null status?
        //
        mInstance.getProperty(pos).getValue().get(operationalStatus);
    }

    // update module status from Stopped to OK
    for(Uint32 i=0, n = operationalStatus.size(); i < n; i++)
    {
        if (operationalStatus[i] == _MODULE_STOPPED)
        {
            operationalStatus.remove(i);
        }
    }
    operationalStatus.append(_MODULE_OK);

    //
    // get module name
    //
    String moduleName;
    Uint32 pos2 = mInstance.findProperty(CIMName ("Name"));
    if (pos2 != PEG_NOT_FOUND)
    {
	mInstance.getProperty(pos2).getValue().get(moduleName);
    }

    if(_providerRegistrationManager->setProviderModuleStatus
        (moduleName, operationalStatus) == false)
    {
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "set module status failed.");
    }

    CIMEnableModuleResponseMessage * response =
        new CIMEnableModuleResponseMessage(
        request->messageId,
        CIMException(),
        request->queueIds.copyAndPop(),
        operationalStatus);

    PEGASUS_ASSERT(response != 0);

    // preserve message key
    response->setKey(request->getKey());

    AsyncLegacyOperationResult *async_result =
        new AsyncLegacyOperationResult(
        async->getKey(),
        async->getRouting(),
        op,
        response);

    _complete_op_node(op, ASYNC_OPSTATE_COMPLETE, 0, 0);
}

void ProviderManagerService::handleStopAllProvidersRequest(AsyncOpNode *op, const
    Message * message) throw()
{
    CIMStopAllProvidersRequestMessage * request =
        dynamic_cast<CIMStopAllProvidersRequestMessage *>(const_cast<Message *>(message));

    AsyncRequest * async = static_cast<AsyncRequest *>(op->_request.next(0));

    PEGASUS_ASSERT(request != 0 && async != 0);

    //
    // tell the provider manager to shutdown all the providers
    //
    providerManager.shutdownAllProviders();

    CIMStopAllProvidersResponseMessage * response =
        new CIMStopAllProvidersResponseMessage(
        request->messageId,
        CIMException(),
        request->queueIds.copyAndPop());

    PEGASUS_ASSERT(response != 0);

    // preserve message key
    response->setKey(request->getKey());

    AsyncLegacyOperationResult *async_result =
       new AsyncLegacyOperationResult(
          async->getKey(),
          async->getRouting(),
          op,
          response);

    _complete_op_node(op, ASYNC_OPSTATE_COMPLETE, 0, 0);
}


void ProviderManagerService::_insertEntry (
    const Provider & provider,
    const EnableIndicationsResponseHandler *handler)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
                      "ProviderManagerService::_insertEntry");

    String tableKey = _generateKey 
        (provider);
    
    _responseTable.insert (tableKey, const_cast<EnableIndicationsResponseHandler *>(handler));

    PEG_METHOD_EXIT ();
}


EnableIndicationsResponseHandler * ProviderManagerService::_removeEntry(
   const String & key)
{
   PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
		     "ProviderManagerService::_removeEntry");
   EnableIndicationsResponseHandler *ret = 0;
   
   _responseTable.lookup(key, ret);
   PEG_METHOD_EXIT ();
   return ret;
}


String ProviderManagerService::_generateKey (
    const Provider & provider)
{
    String tableKey;

    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
                      "ProviderManagerService::_generateKey");

    //
    //  Append provider key values to key
    //
    String providerName = provider.getName();
    String providerFileName = provider.getModule().getFileName();
    tableKey.append (providerName);
    tableKey.append (providerFileName);

    PEG_METHOD_EXIT ();
    return tableKey;
}


PEGASUS_NAMESPACE_END
