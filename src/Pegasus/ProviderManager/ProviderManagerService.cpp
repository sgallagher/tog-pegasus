//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 The Open group, BMC Software, Tivoli Systems, IBM
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
// Modified By:
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

#include <Pegasus/Provider/OperationFlag.h>

#include <Pegasus/Config/ConfigManager.h>

#include <Pegasus/Server/ProviderRegistrationManager/ProviderRegistrationManager.h>

PEGASUS_NAMESPACE_BEGIN

//
// Provider module status
//
static const Uint16 _MODULE_OK       = 2;
static const Uint16 _MODULE_STOPPING = 9;
static const Uint16 _MODULE_STOPPED  = 10;

// thread pool parameters
static struct timeval await = { 0, 40 };
static struct timeval dwait = { 10, 0 };
static struct timeval deadwait = { 1, 0 };

// provider manager
static ProviderManager providerManager;

ProviderManagerService::ProviderManagerService(
    ProviderRegistrationManager * providerRegistrationManager)
    : MessageQueueService(PEGASUS_QUEUENAME_PROVIDERMANAGER_CPP),
    _threadPool(10, "ProviderManagerService", 2, 7, await, dwait, deadwait),
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
    Uint32 pos = pmInstance.findProperty("OperationalStatus");

    if(pos == PEG_NOT_FOUND)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "OperationalStatus not found.");

        PEG_METHOD_EXIT();

        throw CIMException(CIM_ERR_FAILED, "provider lookup failed.");
    }

    pmInstance.getProperty(pos).getValue().get(operationalStatus);

    for(Uint32 i = 0; i < operationalStatus.size(); i++)
    {
        if(operationalStatus[i] == _MODULE_STOPPED ||
	   operationalStatus[i] == _MODULE_STOPPING)
        {
            PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                "Provider blocked.");

            PEG_METHOD_EXIT();

            throw CIMException(CIM_ERR_ACCESS_DENIED, "provider blocked.");
        }
    }

    // get the provider name from the provider instance
    pos = pInstance.findProperty("Name");

    if(pos == PEG_NOT_FOUND)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Provider name not found.");

        PEG_METHOD_EXIT();

        throw CIMException(CIM_ERR_FAILED, "provider lookup failed.");
    }

    pInstance.getProperty(pos).getValue().get(providerName);

    PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
        "providerName = " + providerName + " found.");

    // get the provider location from the provider module instance
    pos = pmInstance.findProperty("Location");

    if(pos == PEG_NOT_FOUND)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Provider location not found.");

        PEG_METHOD_EXIT();

        throw CIMException(CIM_ERR_FAILED, "provider lookup failed.");
    }

    pmInstance.getProperty(pos).getValue().get(location);

//
    // get the provider location from the provider module instance
    pos = pmInstance.findProperty("InterfaceType");

    if (pos != PEG_NOT_FOUND)
    {
        pmInstance.getProperty(pos).getValue().get(interfaceName);

        if (String::equal(interfaceName,"C++Default") )
            interfaceName = String::EMPTY;
    }
   
//

    PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
        "location = " + location + " found.");

    String fileName;

    #ifdef PEGASUS_OS_TYPE_WINDOWS
    fileName = location + String(".dll");
    #elif defined(PEGASUS_OS_HPUX)
    fileName = ConfigManager::getHomedPath(ConfigManager::getInstance()->getCurrentValue("providerDir"));
    fileName += String("/lib") + location + String(".0");
    #else
    fileName = ConfigManager::getHomedPath(ConfigManager::getInstance()->getCurrentValue("providerDir"));
    fileName += String("/lib") + location + String(".so");
    #endif

    PEG_METHOD_EXIT();

    return(Triad<String, String, String>(fileName, providerName,interfaceName));
}

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
        "nameSpace = " + objectPath.getNameSpace() + "; className = " + objectPath.getClassName());

    // get the provider and provider module instance from the registration manager
    if(_providerRegistrationManager->lookupAssociationProvider(
        objectPath.getNameSpace(), objectPath.getClassName(),
        assocClassName, resultClassName,
        pInstances, pmInstances) == false)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Provider registration not found.");

        PEG_METHOD_EXIT();

        throw CIMException(CIM_ERR_FAILED, "provider lookup failed.");
    }

    for(Uint32 i=0,n=pInstances.size(); i<n; i++)
    {
        // get the provider name from the provider instance
        Uint32 pos = pInstances[i].findProperty("Name");

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
        pos = pmInstances[i].findProperty("Location");

        if(pos == PEG_NOT_FOUND)
        {
            PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                "Provider location not found.");
        }

        pmInstances[i].getProperty(pos).getValue().get(Location);

//
        // get the provider location from the provider module instance
        pos = pmInstances[i].findProperty("InterfaceType");

        if (pos != PEG_NOT_FOUND)
        {
            pmInstances[i].getProperty(pos).getValue().get(interfaceName);

            if (String::equal(interfaceName,"C++Default") )
                interfaceName = String::EMPTY;
        }
   
//

        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "location = " + Location + " found.");

        String fileName;

        #ifdef PEGASUS_OS_TYPE_WINDOWS
        fileName = Location + String(".dll");
        #elif defined(PEGASUS_OS_HPUX)
        fileName = ConfigManager::getHomedPath(ConfigManager::getInstance()->getCurrentValue("providerDir"));
        fileName += String("/lib") + Location + String(".sl");
        #else
        fileName = ConfigManager::getHomedPath(ConfigManager::getInstance()->getCurrentValue("providerDir"));
        fileName += String("/lib") + Location + String(".so");
        #endif

        providerNames.append(providerName);
        Locations.append(fileName);
        interfaceNames.append(interfaceName);
    }

    PEG_METHOD_EXIT();

    return;
}

Triad<String, String, String>
    ProviderManagerService::_lookupMethodProviderForClass(
  const CIMObjectPath & objectPath,
  const String & methodName)
{
    CIMInstance pInstance;
    CIMInstance pmInstance;

    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "ProviderManagerService::_lookupMethodProviderForClass");

    PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
        "nameSpace = " + objectPath.getNameSpace() + "; className = " + objectPath.getClassName() + "; methodName = " + methodName);

    // get the provider and provider module instance from the registration manager
    if(_providerRegistrationManager->lookupMethodProvider(
        objectPath.getNameSpace(), objectPath.getClassName(), methodName,
        pInstance, pmInstance) == false)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Provider registration not found.");

        PEG_METHOD_EXIT();

        throw CIMException(CIM_ERR_FAILED, "provider lookup failed.");
    }

    Triad<String, String, String> triad;

    triad = _getProviderRegPair(pInstance, pmInstance);

    PEG_METHOD_EXIT();

    return(triad);
}


Triad<String, String,String> ProviderManagerService::_lookupProviderForClass(
    const CIMObjectPath & objectPath)
{
    CIMInstance pInstance;
    CIMInstance pmInstance;

    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "ProviderManagerService::_lookupProviderForClass");

    PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
        "nameSpace = " + objectPath.getNameSpace() + "; className = " + objectPath.getClassName());

    // get the provider and provider module instance from the registration manager
    if(_providerRegistrationManager->lookupInstanceProvider(
        objectPath.getNameSpace(), objectPath.getClassName(),
        pInstance, pmInstance) == false)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Provider registration not found.");

        PEG_METHOD_EXIT();

        throw CIMException(CIM_ERR_FAILED, "provider lookup failed.");
    }

    Triad<String, String, String> triad;

    triad = _getProviderRegPair(pInstance, pmInstance);

    PEG_METHOD_EXIT();

    return triad;
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

        _threadPool.allocate_and_awaken((void *)this, ProviderManagerService::handleCimOperation);
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

    AsyncOpNode *op = service->_incomingQueue.dequeue();

    PEGASUS_ASSERT(op != 0 );

    AsyncRequest *request = static_cast<AsyncRequest *>(op->_request.next(0));

    if(request->getType() == async_messages::ASYNC_LEGACY_OP_START)
    {
        Message *legacy = static_cast<AsyncLegacyOperationStart *>(request)->get_action();

        if(legacy != NULL)
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

        // convert flags to bitmask
        Uint32 flags = OperationFlag::convert(false);

        // strip flags inappropriate for providers
        flags = flags & ~OperationFlag::LOCAL_ONLY & ~OperationFlag::DEEP_INHERITANCE;

        CIMPropertyList propertyList(request->propertyList);

        STAT_GETSTARTTIME;

        // forward request
        provider.getInstance(
            context,
            objectPath,
            flags,
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
        Array<CIMNamedInstance>());

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

        // convert flags to bitmask
        Uint32 flags = OperationFlag::convert(false);

        // strip flags inappropriate for providers
        flags = flags & ~OperationFlag::LOCAL_ONLY & ~OperationFlag::DEEP_INHERITANCE;

        CIMPropertyList propertyList(request->propertyList);

        STAT_GETSTARTTIME;

        provider.enumerateInstances(
            context,
            objectPath,
            flags,
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
            request->modifiedInstance.getInstanceName().getClassName(),    //request->modifiedInstance.getInstance().getPath().getClassName(),
            request->modifiedInstance.getInstanceName().getKeyBindings()); //request->modifiedInstance.getInstance().getPath().getKeyBindings());

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

        // convert flags to bitmask
        Uint32 flags = OperationFlag::convert(false);

        // strip flags inappropriate for providers
        flags = flags & ~OperationFlag::LOCAL_ONLY & ~OperationFlag::DEEP_INHERITANCE;

        CIMPropertyList propertyList(request->propertyList);

        // forward request
        STAT_GETSTARTTIME;

        provider.modifyInstance(
            context,
            objectPath,
            request->modifiedInstance.getInstance(),
            flags,
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

    Array<CIMObjectWithPath> cimObjects;

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

    Array<CIMObjectWithPath> cimObjects;

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

            // convert flags to bitmask
            Uint32 flags = OperationFlag::convert(false);

            // strip flags inappropriate for providers
            flags = flags | ~OperationFlag::LOCAL_ONLY |
                ~OperationFlag::DEEP_INHERITANCE;

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

    Array<CIMObjectWithPath> cimObjects;

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

            // convert flags to bitmask
            Uint32 flags = OperationFlag::convert(false);

            // strip flags inappropriate for providers
            flags = flags | ~OperationFlag::LOCAL_ONLY |
                ~OperationFlag::DEEP_INHERITANCE;

            //CIMPropertyList propertyList(request->propertyList);

            //SimpleResponseHandler<CIMObjectPath> handler;

            STAT_GETSTARTTIME;

            provider.references(
                context,
                objectPath,
                request->resultClass,
                request->role,
                flags,
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

            // convert flags to bitmask
            Uint32 flags = OperationFlag::convert(false);

            // strip flags inappropriate for providers
            flags = flags | ~OperationFlag::LOCAL_ONLY |
                ~OperationFlag::DEEP_INHERITANCE;

            //CIMPropertyList propertyList(request->propertyList);

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

        // convert flags to bitmask
        Uint32 flags = 0;

        // strip flags inappropriate for providers
        //flags = flags & ~OperationFlag::LOCAL_ONLY & ~OperationFlag::DEEP_INHERITANCE;

        String propertyName = request->propertyName;

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
        PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "not implemented"),
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

        // convert flags to bitmask
        Uint32 flags = 0;

	String propertyName; // = request->propertyName;
	CIMValue propertyValue; // = request->propertyValue;

        // strip flags inappropriate for providers
        //flags = flags & ~OperationFlag::LOCAL_ONLY & ~OperationFlag::DEEP_INHERITANCE;

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

        Array<CIMParamValue> outParameters;

        // forward request
        STAT_GETSTARTTIME;

        provider.invokeMethod(
            context,
            instanceReference,
            request->methodName,
            request->inParameters,
            outParameters,
            handler);

        // ATTN-RK-P1-20020502: This needs to go through the response handler
        response->outParameters = outParameters;

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

    OperationResponseHandler<CIMIndication> handler(request, response);

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

    OperationResponseHandler<CIMIndication> handler(request, response);

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

    OperationResponseHandler<CIMIndication> handler(request, response);

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

    EnableIndicationsResponseHandler handler(request, response, this);

    try
    {
	// get the provider file name and logical name
	Triad<String, String, String> triad =
	    _getProviderRegPair(request->provider, request->providerModule);

	// get cached or load new provider module
	Provider provider = 
            providerManager.getProvider(triad.first, triad.second, triad.third);

        provider.enableIndications(handler);
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

    OperationResponseHandler<CIMIndication> handler(request, response);

    try
    {
	// get the provider file name and logical name
	Triad<String, String, String> triad =
	    _getProviderRegPair(request->provider, request->providerModule);

	// get cached or load new provider module
	Provider provider = 
            providerManager.getProvider(triad.first, triad.second, triad.third);

        provider.disableIndications();
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

void ProviderManagerService::handleDisableModuleRequest(AsyncOpNode *op, const Message * message) throw()
{
    CIMDisableModuleRequestMessage * request =
        dynamic_cast<CIMDisableModuleRequestMessage *>(const_cast<Message *>(message));

    AsyncRequest *async = static_cast<AsyncRequest *>(op->_request.next(0));

    PEGASUS_ASSERT(request != 0 && async != 0);

    Array<Uint16> operationalStatus;

    // get provider module name
    String moduleName;
    CIMInstance mInstance = request->providerModule;
    Uint32 pos = mInstance.findProperty("Name");

    if(pos == PEG_NOT_FOUND)
    {
        throw CIMException(CIM_ERR_FAILED, "Provider module name not found");
    }

    mInstance.getProperty(pos).getValue().get(moduleName);

    // set module status to be Stopping
    operationalStatus.append(_MODULE_STOPPING);

    if(_providerRegistrationManager->setProviderModuleStatus
        (moduleName, operationalStatus) == false)
    {
        throw CIMException(CIM_ERR_FAILED, "set module status failed.");
    }

    Array<CIMInstance> _pInstances = request->providers;

    for(Uint32 i = 0, n = _pInstances.size(); i<n; i++)
    {
	// get the provider file name and logical name
	Triad<String, String, String> triad =
            _getProviderRegPair(_pInstances[i], mInstance);

        providerManager.unloadProvider(triad.first, triad.second);
    }

    // set module status to be Stopped
    operationalStatus.clear();
    operationalStatus.append(_MODULE_STOPPED);

    if(_providerRegistrationManager->setProviderModuleStatus
        (moduleName, operationalStatus) == false)
    {
        throw CIMException(CIM_ERR_FAILED, "set module status failed.");
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

    Array<Uint16> operationalStatus;

    // set module status to be OK
    operationalStatus.append(_MODULE_OK);

    if(_providerRegistrationManager->setProviderModuleStatus
        (request->moduleName, operationalStatus) == false)
    {
        throw CIMException(CIM_ERR_FAILED, "set module status failed.");
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

PEGASUS_NAMESPACE_END
