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
#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Common/OperationContext.h>
#include <Pegasus/Common/Destroyer.h>
#include <Pegasus/Common/Tracer.h>

#include <Pegasus/ProviderManager/ProviderManager.h>
#include <Pegasus/ProviderManager/ProviderFacade.h>
#include <Pegasus/ProviderManager/OperationResponseHandler.h>

#include <Pegasus/Provider/OperationFlag.h>

#include <Pegasus/Config/ConfigManager.h>

#include <Pegasus/Server/ProviderRegistrationManager/ProviderRegistrationManager.h>

PEGASUS_NAMESPACE_BEGIN

static ProviderManager providerManager;

static struct timeval await = { 0, 40};
static struct timeval dwait = { 10, 0};
static struct timeval deadwait = { 1, 0};

ProviderManagerService::ProviderManagerService(
    ProviderRegistrationManager * providerRegistrationManager)
    : MessageQueueService("Server::ProviderManagerService", MessageQueue::getNextQueueId()),
    _threadPool(10, "ProviderManagerService", 2, 7, await, dwait, deadwait),
    _providerRegistrationManager(providerRegistrationManager)
{
}

ProviderManagerService::~ProviderManagerService(void)
{
}

ProviderManager * ProviderManagerService::getProviderManager(void)
{
    return(&providerManager);
}

Pair<String, String> _getProviderRegPair(const CIMInstance& pInstance, const CIMInstance& pmInstance)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "_getProviderRegPair");

    String providerName;
    String location;

    // get the provider name from the provider instance
    Uint32 pos = pInstance.findProperty("Name");

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

    PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
	"location = " + location + " found.");

    String fileName;

    #ifdef PEGASUS_OS_TYPE_WINDOWS
    fileName = location + String(".dll");
    #elif defined(PEGASUS_OS_HPUX)
    fileName = ConfigManager::getHomedPath(ConfigManager::getInstance()->getCurrentValue("providerDir"));
    fileName += String("/lib") + location + String(".sl");
    #else
    fileName = ConfigManager::getHomedPath(ConfigManager::getInstance()->getCurrentValue("providerDir"));
    fileName += String("/lib") + location + String(".so");
    #endif

    PEG_METHOD_EXIT();

    return(Pair<String, String>(fileName, providerName));
}

Pair<String, String> ProviderManagerService::_lookupProviderForClass(const CIMObjectPath & objectPath)
{
    CIMInstance pInstance;
    CIMInstance pmInstance;

    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "ProviderManagerService::_lookupProviderForClass");

    PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
	"nameSpace = " + objectPath.getNameSpace() + "; className = " + objectPath.getClassName());

    // ATTN: try all provider type lookups

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

    Pair<String, String> pair;

    pair = _getProviderRegPair(pInstance, pmInstance);

    PEG_METHOD_EXIT();

    return pair;
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

    AsyncOpNode * op = this->get_op();

    AsyncLegacyOperationStart * asyncRequest =
	new AsyncLegacyOperationStart(
	get_next_xid(),
	op,
	this->getQueueId(),
	message,
	this->getQueueId());

    //PEGASUS_ASSERT(asyncRequest != 0);

    //AsynReply * asyncReplay = SendWait(asyncRequest);

    //PEGASUS_ASSERT(asyncReplay != 0);

    //delete asyncRequest;
    //delete asyncReply;

    _handle_async_request(asyncRequest);
}

void ProviderManagerService::_handle_async_request(AsyncRequest * request)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
                     "ProviderManagerService::_handle_async_request");

    PEGASUS_ASSERT(request != 0);

    if(request->getType() == async_messages::ASYNC_LEGACY_OP_START)
    {
	request->op->processing();

	Message * message = (static_cast<AsyncLegacyOperationStart *>(request)->get_action());

	PEGASUS_ASSERT(message != 0);

	// place message on local queue
	_incomingQueue.enqueue(message);

	// get thread and start request method
	_threadPool.allocate_and_awaken((void *)this, ProviderManagerService::handleCimOperation);

        PEG_METHOD_EXIT();
	return;
    }

    // pass all other operations to the default handler
    MessageQueueService::_handle_async_request(request);
    PEG_METHOD_EXIT();
}

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

PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL ProviderManagerService::handleCimOperation(void * arg) throw()
{
    // get the service from argument
    ProviderManagerService * service = reinterpret_cast<ProviderManagerService *>(arg);

    PEGASUS_ASSERT(service != 0);

    // get message from service queue
    PEGASUS_ASSERT(service->_incomingQueue.size() != 0);

    Message * message = service->_incomingQueue.dequeue();

    PEGASUS_ASSERT(message != 0);

    // ensure the message is destoyed at end-of-scope
    Destroyer<Message> xmessage(message);

    // pass the request message to a handler method based on message type
    switch(message->getType())
    {
    case CIM_GET_CLASS_REQUEST_MESSAGE:
    case CIM_ENUMERATE_CLASSES_REQUEST_MESSAGE:
    case CIM_ENUMERATE_CLASS_NAMES_REQUEST_MESSAGE:
    case CIM_CREATE_CLASS_REQUEST_MESSAGE:
    case CIM_MODIFY_CLASS_REQUEST_MESSAGE:
    case CIM_DELETE_CLASS_REQUEST_MESSAGE:
	break;
    case CIM_GET_INSTANCE_REQUEST_MESSAGE:
	service->handleGetInstanceRequest(message);

	break;
    case CIM_ENUMERATE_INSTANCES_REQUEST_MESSAGE:
	service->handleEnumerateInstancesRequest(message);

	break;
    case CIM_ENUMERATE_INSTANCE_NAMES_REQUEST_MESSAGE:
	service->handleEnumerateInstanceNamesRequest(message);

	break;
    case CIM_CREATE_INSTANCE_REQUEST_MESSAGE:
	service->handleCreateInstanceRequest(message);

	break;
    case CIM_MODIFY_INSTANCE_REQUEST_MESSAGE:
	service->handleModifyInstanceRequest(message);

	break;
    case CIM_DELETE_INSTANCE_REQUEST_MESSAGE:
	service->handleDeleteInstanceRequest(message);

	break;
    case CIM_EXEC_QUERY_REQUEST_MESSAGE:
	service->handleExecuteQueryRequest(message);

	break;
    case CIM_ASSOCIATORS_REQUEST_MESSAGE:
	service->handleAssociatorsRequest(message);

	break;
    case CIM_ASSOCIATOR_NAMES_REQUEST_MESSAGE:
	service->handleAssociatorNamesRequest(message);

	break;
    case CIM_REFERENCES_REQUEST_MESSAGE:
	service->handleReferencesRequest(message);

	break;
    case CIM_REFERENCE_NAMES_REQUEST_MESSAGE:
	service->handleReferenceNamesRequest(message);

	break;
    case CIM_GET_PROPERTY_REQUEST_MESSAGE:
	service->handleGetPropertyRequest(message);

	break;
    case CIM_SET_PROPERTY_REQUEST_MESSAGE:
	service->handleSetPropertyRequest(message);

	break;
    case CIM_INVOKE_METHOD_REQUEST_MESSAGE:
	service->handleInvokeMethodRequest(message);

	break;
    case CIM_CREATE_SUBSCRIPTION_REQUEST_MESSAGE:
	service->handleCreateSubscriptionRequest(message);

	break;
    case CIM_MODIFY_SUBSCRIPTION_REQUEST_MESSAGE:
	service->handleModifySubscriptionRequest(message);

	break;
    case CIM_DELETE_SUBSCRIPTION_REQUEST_MESSAGE:
	service->handleDeleteSubscriptionRequest(message);

	break;
    case CIM_ENABLE_INDICATIONS_REQUEST_MESSAGE:
	service->handleEnableIndicationsRequest(message);

	break;
    case CIM_DISABLE_INDICATIONS_REQUEST_MESSAGE:
	service->handleDisableIndicationsRequest(message);

	break;
    default:
	// unsupported messages are ignored
	break;
    }

    return(0);
}

void ProviderManagerService::handleGetInstanceRequest(const Message * message) throw()
{
    CIMGetInstanceRequestMessage * request =
	dynamic_cast<CIMGetInstanceRequestMessage *>(const_cast<Message *>(message));

    PEGASUS_ASSERT(request != 0);

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
	Pair<String, String> pair = _lookupProviderForClass(objectPath);

	// get cached or load new provider module
	Provider provider = providerManager.getProvider(pair.first, pair.second);

	// convert arguments
	OperationContext context;

	// add the user name to the context
	context.add_context(
	    sizeof(String *),
	    const_cast<String *>(&(request->userName)),
	    0,
	    0,
	    CONTEXT_IDENTITY,
	    0,
	    0);

	// convert flags to bitmask
	Uint32 flags = OperationFlag::convert(false);

	// strip flags inappropriate for providers
	flags = flags & ~OperationFlag::LOCAL_ONLY & ~OperationFlag::DEEP_INHERITANCE;

	CIMPropertyList propertyList(request->propertyList);

	// forward request
	provider.getInstance(
	    context,
	    objectPath,
	    flags,
	    propertyList.getPropertyNameArray(),
	    handler);
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

    _enqueueResponse(handler.getRequest(), handler.getResponse());
}

void ProviderManagerService::handleEnumerateInstancesRequest(const Message * message) throw()
{
    CIMEnumerateInstancesRequestMessage * request =
	dynamic_cast<CIMEnumerateInstancesRequestMessage *>(const_cast<Message *>(message));

    PEGASUS_ASSERT(request != 0);

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
	Pair<String, String> pair = _lookupProviderForClass(objectPath);

	// get cached or load new provider module
	Provider provider = providerManager.getProvider(pair.first, pair.second);

	// convert arguments
	OperationContext context;

	// add the user name to the context
	context.add_context(
	    sizeof(String *),
	    const_cast<String *>(&(request->userName)),
	    0,
	    0,
	    CONTEXT_IDENTITY,
	    0,
	    0);

	// convert flags to bitmask
	Uint32 flags = OperationFlag::convert(false);

	// strip flags inappropriate for providers
	flags = flags & ~OperationFlag::LOCAL_ONLY & ~OperationFlag::DEEP_INHERITANCE;

	CIMPropertyList propertyList(request->propertyList);

	provider.enumerateInstances(
	    context,
	    objectPath,
	    flags,
	    propertyList.getPropertyNameArray(),
	    handler);
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

    _enqueueResponse(handler.getRequest(), handler.getResponse());
}

void ProviderManagerService::handleEnumerateInstanceNamesRequest(const Message * message) throw()
{
    CIMEnumerateInstanceNamesRequestMessage * request =
	dynamic_cast<CIMEnumerateInstanceNamesRequestMessage *>(const_cast<Message *>(message));

    PEGASUS_ASSERT(request != 0);

    CIMEnumerateInstanceNamesResponseMessage * response =
	new CIMEnumerateInstanceNamesResponseMessage(
	request->messageId,
	CIMException(),
	request->queueIds.copyAndPop(),
	Array<CIMReference>());

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
	Pair<String, String> pair = _lookupProviderForClass(objectPath);

	// get cached or load new provider module
	Provider provider = providerManager.getProvider(pair.first, pair.second);

	// convert arguments
	OperationContext context;

	// add the user name to the context
	context.add_context(
	    sizeof(String *),
	    const_cast<String *>(&(request->userName)),
	    0,
	    0,
	    CONTEXT_IDENTITY,
	    0,
	    0);

	provider.enumerateInstanceNames(
	    context,
	    objectPath,
	    handler);
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

    _enqueueResponse(handler.getRequest(), handler.getResponse());
}

void ProviderManagerService::handleCreateInstanceRequest(const Message * message) throw()
{
    CIMCreateInstanceRequestMessage * request =
	dynamic_cast<CIMCreateInstanceRequestMessage *>(const_cast<Message *>(message));

    PEGASUS_ASSERT(request != 0);

    // create response message
    CIMCreateInstanceResponseMessage * response =
	new CIMCreateInstanceResponseMessage(
	request->messageId,
	CIMException(),
	request->queueIds.copyAndPop(),
	CIMReference());

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
	Pair<String, String> pair = _lookupProviderForClass(objectPath);

	// get cached or load new provider module
	Provider provider = providerManager.getProvider(pair.first, pair.second);

	// convert arguments
	OperationContext context;

	// add the user name to the context
	context.add_context(sizeof(String *),
	    const_cast<String *>(&(request->userName)),
	    0,
	    0,
	    CONTEXT_IDENTITY,
	    0,
	    0);

	// forward request
	provider.createInstance(
	    context,
	    objectPath,
	    request->newInstance,
	    handler);
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

    _enqueueResponse(handler.getRequest(), handler.getResponse());
}

void ProviderManagerService::handleModifyInstanceRequest(const Message * message) throw()
{
    CIMModifyInstanceRequestMessage * request =
	dynamic_cast<CIMModifyInstanceRequestMessage *>(const_cast<Message *>(message));

    PEGASUS_ASSERT(request != 0);

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
	Pair<String, String> pair = _lookupProviderForClass(objectPath);

	// get cached or load new provider module
	Provider provider = providerManager.getProvider(pair.first, pair.second);

	// convert arguments
	OperationContext context;

	// add the user name to the context
	context.add_context(
	    sizeof(String *),
	    const_cast<String *>(&(request->userName)),
	    0,
	    0,
	    CONTEXT_IDENTITY,
	    0,
	    0);

	// convert flags to bitmask
	Uint32 flags = OperationFlag::convert(false);

	// strip flags inappropriate for providers
	flags = flags & ~OperationFlag::LOCAL_ONLY & ~OperationFlag::DEEP_INHERITANCE;

	CIMPropertyList propertyList(request->propertyList);

	// forward request
	provider.modifyInstance(
	    context,
	    objectPath,
	    request->modifiedInstance.getInstance(),
	    flags,
	    propertyList.getPropertyNameArray(),
	    handler);
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

    _enqueueResponse(handler.getRequest(), handler.getResponse());
}

void ProviderManagerService::handleDeleteInstanceRequest(const Message * message) throw()
{
    CIMDeleteInstanceRequestMessage * request =
	dynamic_cast<CIMDeleteInstanceRequestMessage *>(const_cast<Message *>(message));

    PEGASUS_ASSERT(request != 0);

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
	Pair<String, String> pair = _lookupProviderForClass(objectPath);

	// get cached or load new provider module
	Provider provider = providerManager.getProvider(pair.first, pair.second);

	// convert arguments
	OperationContext context;

	context.add_context(
	    sizeof(String *),
	    const_cast<String *>(&(request->userName)),
	    0,
	    0,
	    CONTEXT_IDENTITY,
	    0,
	    0);

	// forward request
	provider.deleteInstance(
	    context,
	    objectPath,
	    handler);
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

    _enqueueResponse(handler.getRequest(), handler.getResponse());
}

void ProviderManagerService::handleExecuteQueryRequest(const Message * message) throw()
{
    CIMExecQueryRequestMessage * request =
	dynamic_cast<CIMExecQueryRequestMessage *>(const_cast<Message *>(message));

    PEGASUS_ASSERT(request != 0);

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

    _enqueueResponse(request, response);
}

void ProviderManagerService::handleAssociatorsRequest(const Message * message) throw()
{
    CIMAssociatorsRequestMessage * request =
	dynamic_cast<CIMAssociatorsRequestMessage *>(const_cast<Message *>(message));

    PEGASUS_ASSERT(request != 0);

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

    _enqueueResponse(request, response);
}

void ProviderManagerService::handleAssociatorNamesRequest(const Message * message) throw()
{
    CIMAssociatorNamesRequestMessage * request =
	dynamic_cast<CIMAssociatorNamesRequestMessage *>(const_cast<Message *>(message));

    PEGASUS_ASSERT(request != 0);

    Array<CIMReference> cimReferences;

    CIMAssociatorNamesResponseMessage * response =
	new CIMAssociatorNamesResponseMessage(
	request->messageId,
	PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "not implemented"),
	request->queueIds.copyAndPop(),
	cimReferences);

    PEGASUS_ASSERT(response != 0);

    // preserve message key
    response->setKey(request->getKey());

    _enqueueResponse(request, response);
}

void ProviderManagerService::handleReferencesRequest(const Message * message) throw()
{
    CIMReferencesRequestMessage * request =
	dynamic_cast<CIMReferencesRequestMessage *>(const_cast<Message *>(message));

    PEGASUS_ASSERT(request != 0);

    Array<CIMObjectWithPath> cimObjects;

    CIMReferencesResponseMessage * response =
	new CIMReferencesResponseMessage(
	request->messageId,
	PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "not implemented"),
	request->queueIds.copyAndPop(),
	cimObjects);

    PEGASUS_ASSERT(response != 0);

    // preserve message key
    response->setKey(request->getKey());

    _enqueueResponse(request, response);
}

void ProviderManagerService::handleReferenceNamesRequest(const Message * message) throw()
{
    CIMReferenceNamesRequestMessage * request =
	dynamic_cast<CIMReferenceNamesRequestMessage *>(const_cast<Message *>(message));

    PEGASUS_ASSERT(request != 0);

    Array<CIMReference> cimReferences;

    CIMReferenceNamesResponseMessage * response =
	new CIMReferenceNamesResponseMessage(
	request->messageId,
	PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "not implemented"),
	request->queueIds.copyAndPop(),
	cimReferences);

    // preserve message key
    response->setKey(request->getKey());

    _enqueueResponse(request, response);
}

void ProviderManagerService::handleGetPropertyRequest(const Message * message) throw()
{
    CIMGetPropertyRequestMessage * request =
	dynamic_cast<CIMGetPropertyRequestMessage *>(const_cast<Message *>(message));

    PEGASUS_ASSERT(request != 0);

    CIMValue cimValue;

    // create response message
    CIMGetPropertyResponseMessage * response =
	new CIMGetPropertyResponseMessage(
	request->messageId,
	PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "not implemented"),
	request->queueIds.copyAndPop(),
	cimValue);

    PEGASUS_ASSERT(response != 0);

    // preserve message key
    response->setKey(request->getKey());

    _enqueueResponse(request, response);
}

void ProviderManagerService::handleSetPropertyRequest(const Message * message) throw()
{
    CIMSetPropertyRequestMessage * request =
	dynamic_cast<CIMSetPropertyRequestMessage *>(const_cast<Message *>(message));

    PEGASUS_ASSERT(request != 0);

    // create response message
    CIMSetPropertyResponseMessage * response =
	new CIMSetPropertyResponseMessage(
	request->messageId,
	PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "not implemented"),
	request->queueIds.copyAndPop());

    PEGASUS_ASSERT(response != 0);

    // preserve message key
    response->setKey(request->getKey());

    _enqueueResponse(request, response);
}

void ProviderManagerService::handleInvokeMethodRequest(const Message * message) throw()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
                     "ProviderManagerService::handleInvokeMethodRequest");

    CIMInvokeMethodRequestMessage * request =
	dynamic_cast<CIMInvokeMethodRequestMessage *>(const_cast<Message *>(message));

    PEGASUS_ASSERT(request != 0);

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
	Pair<String, String> pair = _lookupProviderForClass(objectPath);

	// get cached or load new provider module
	Provider provider = providerManager.getProvider(pair.first, pair.second);

	// convert arguments
	OperationContext context;

	// add the user name to the context
	context.add_context(sizeof(String *),
	    const_cast<String *>(&(request->userName)),
	    0,
	    0,
	    CONTEXT_IDENTITY,
	    0,
	    0);

	CIMObjectPath instanceReference(request->instanceName);

	// ATTN: propagate namespace
	instanceReference.setNameSpace(request->nameSpace);

	Array<CIMParamValue> outParameters;

	// forward request
	provider.invokeMethod(
	    context,
	    instanceReference,
	    request->methodName,
	    request->inParameters,
	    outParameters,
	    handler);
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

    _enqueueResponse(handler.getRequest(), handler.getResponse());
    PEG_METHOD_EXIT();
}

void ProviderManagerService::handleCreateSubscriptionRequest(const Message * message) throw()
{
    CIMCreateSubscriptionRequestMessage * request =
	dynamic_cast<CIMCreateSubscriptionRequestMessage *>(const_cast<Message *>(message));

    PEGASUS_ASSERT(request != 0);

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
	Pair<String, String> pair = _getProviderRegPair(request->provider, request->providerModule);

	// get cached or load new provider module
	Provider provider = providerManager.getProvider(pair.first, pair.second);

	// convert arguments
	OperationContext context;

	CIMReference subscriptionName = request->subscriptionInstance.getPath();
	
	Array<CIMReference> classNames;

	CIMPropertyList propertyList;
	
	Uint16 repeatNotificationPolicy = 2;
	
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
	
    _enqueueResponse(handler.getRequest(), handler.getResponse());
}

void ProviderManagerService::handleModifySubscriptionRequest(const Message * message) throw()
{
    CIMModifySubscriptionRequestMessage * request =
	dynamic_cast<CIMModifySubscriptionRequestMessage *>(const_cast<Message *>(message));

    PEGASUS_ASSERT(request != 0);

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
	Pair<String, String> pair = _getProviderRegPair(request->provider, request->providerModule);

	// get cached or load new provider module
	Provider provider = providerManager.getProvider(pair.first, pair.second);
	
	OperationContext context;
	
	CIMReference subscriptionName;

	Array<CIMReference> classNames;

	CIMPropertyList propertyList;
	
	Uint16 repeatNotificationPolicy = 2;
	
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
	
    _enqueueResponse(handler.getRequest(), handler.getResponse());
}

void ProviderManagerService::handleDeleteSubscriptionRequest(const Message * message) throw()
{
    CIMDeleteSubscriptionRequestMessage * request =
	dynamic_cast<CIMDeleteSubscriptionRequestMessage *>(const_cast<Message *>(message));

    PEGASUS_ASSERT(request != 0);

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
	Pair<String, String> pair = _getProviderRegPair(request->provider, request->providerModule);

	// get cached or load new provider module
	Provider provider = providerManager.getProvider(pair.first, pair.second);

	OperationContext context;
	
	CIMReference subscriptionName;

	Array<CIMReference> classNames;

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
	
    _enqueueResponse(handler.getRequest(), handler.getResponse());
}

void ProviderManagerService::handleEnableIndicationsRequest(const Message * message) throw()
{
    CIMEnableIndicationsRequestMessage * request =
	dynamic_cast<CIMEnableIndicationsRequestMessage *>(const_cast<Message *>(message));

    PEGASUS_ASSERT(request != 0);

    CIMEnableIndicationsResponseMessage * response =
	new CIMEnableIndicationsResponseMessage(
	request->messageId,
	CIMException(),
	request->queueIds.copyAndPop());

    PEGASUS_ASSERT(response != 0);

    // preserve message key
    response->setKey(request->getKey());

    static EnableIndicationsResponseHandler handler(request, response, this);

    try
    {
	// get the provider file name and logical name
	Pair<String, String> pair = _getProviderRegPair(request->provider, request->providerModule);

	// get cached or load new provider module
	Provider provider = providerManager.getProvider(pair.first, pair.second);

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
	
    _enqueueResponse(handler.getRequest(), handler.getResponse());
}

void ProviderManagerService::handleDisableIndicationsRequest(const Message * message) throw()
{
    CIMDisableIndicationsRequestMessage * request =
	dynamic_cast<CIMDisableIndicationsRequestMessage *>(const_cast<Message *>(message));

    PEGASUS_ASSERT(request != 0);

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
	Pair<String, String> pair = _getProviderRegPair(request->provider, request->providerModule);

	// get cached or load new provider module
	Provider provider = providerManager.getProvider(pair.first, pair.second);

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
	
    _enqueueResponse(handler.getRequest(), handler.getResponse());
}

PEGASUS_NAMESPACE_END
