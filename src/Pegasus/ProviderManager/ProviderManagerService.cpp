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

Pair<String, String> ProviderManagerService::_lookupProviderForClass(const CIMObjectPath & objectPath)
{
    CIMInstance pInstance;
    CIMInstance pmInstance;
    String providerName;
    String location;

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

    _handle_async_request(asyncRequest);

    //delete asyncRequest;
}

void ProviderManagerService::_handle_async_request(AsyncRequest * request)
{
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

	return;
    }

    // pass all other operations to the default handler
    MessageQueueService::_handle_async_request(request);
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
	CIM_ERR_SUCCESS,
	String::EMPTY,
	request->queueIds.copyAndPop(),
	CIMInstance());

    PEGASUS_ASSERT(response != 0);

    // propagate key
    response->setKey(request->getKey());

    // create a handler for this request
    class GetInstanceResponseHandler : public OperationResponseHandler<CIMInstance>
    {
    public:
	GetInstanceResponseHandler(
	    CIMGetInstanceRequestMessage * request,
	    CIMGetInstanceResponseMessage * response)
	: OperationResponseHandler<CIMInstance>(request, response)
	{
	}

	virtual void complete(const OperationContext & context)
	{
	    if(getObjects().size() > 0)
	    {
		((CIMGetInstanceResponseMessage *)getResponse())->cimInstance = getObjects()[0];
	    }
	    else
	    {
		// error? provider claims success, but did not deliver an instance.
		setStatus(CIM_ERR_NOT_FOUND);
	    }
	}

    } handler(request, response);

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
	CIM_ERR_SUCCESS,
	String::EMPTY,
	request->queueIds.copyAndPop(),
	Array<CIMNamedInstance>());

    PEGASUS_ASSERT(response != 0);

    // propagate key
    response->setKey(request->getKey());

    // create a handler for this request
    class EnumerateInstancesResponseHandler : public OperationResponseHandler<CIMInstance>
    {
    public:
	EnumerateInstancesResponseHandler(
	    CIMEnumerateInstancesRequestMessage * request,
	    CIMEnumerateInstancesResponseMessage * response)
	: OperationResponseHandler<CIMInstance>(request, response)
	{
	}

	virtual void complete(const OperationContext & context)
	{
	    Array<CIMNamedInstance> cimInstances;

	    // ATTN: can be removed once CIMNamedInstance is removed
	    for(Uint32 i = 0, n = getObjects().size(); i < n; i++)
	    {
		CIMInstance cimInstance(getObjects()[i]);

		cimInstances.append(CIMNamedInstance(cimInstance.getPath(), cimInstance));
	    }

	    ((CIMEnumerateInstancesResponseMessage *)getResponse())->cimNamedInstances = cimInstances;
	}

    } handler(request, response);

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
	CIM_ERR_SUCCESS,
	String::EMPTY,
	request->queueIds.copyAndPop(),
	Array<CIMReference>());

    PEGASUS_ASSERT(response != 0);

    // propagate key
    response->setKey(request->getKey());

    // create a handler for this request
    class EnumerateInstanceNamesResponseHandler : public OperationResponseHandler<CIMReference>
    {
    public:
	EnumerateInstanceNamesResponseHandler(
	    CIMEnumerateInstanceNamesRequestMessage * request,
	    CIMEnumerateInstanceNamesResponseMessage * response)
	: OperationResponseHandler<CIMReference>(request, response)
	{
	}

	virtual void complete(const OperationContext & context)
	{
	    ((CIMEnumerateInstanceNamesResponseMessage *)getResponse())->instanceNames = getObjects();
	}

    } handler(request, response);

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
	CIM_ERR_SUCCESS,
	String::EMPTY,
	request->queueIds.copyAndPop(),
	CIMReference());

    PEGASUS_ASSERT(response != 0);

    // propagate key
    response->setKey(request->getKey());

    // create a handler for this request
    class CreateInstanceResponseHandler : public OperationResponseHandler<CIMReference>
    {
    public:
	CreateInstanceResponseHandler(
	    CIMCreateInstanceRequestMessage * request,
	    CIMCreateInstanceResponseMessage * response)
	: OperationResponseHandler<CIMReference>(request, response)
	{
	}

	virtual void complete(const OperationContext & context)
	{
	    if(getObjects().size() > 0)
	    {
		((CIMCreateInstanceResponseMessage *)getResponse())->instanceName = getObjects()[0];
	    }

	    // ATTN: is it an error to not return instance name?
	}

    } handler(request, response);

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
	CIM_ERR_SUCCESS,
	String::EMPTY,
	request->queueIds.copyAndPop());

    PEGASUS_ASSERT(response != 0);

    // propagate key
    response->setKey(request->getKey());

    // create a handler for this request
    class ModifyInstanceResponseHandler : public OperationResponseHandler<CIMInstance>
    {
    public:
	ModifyInstanceResponseHandler(
	    CIMModifyInstanceRequestMessage * request,
	    CIMModifyInstanceResponseMessage * response)
	: OperationResponseHandler<CIMInstance>(request, response)
	{
	}

    } handler(request, response);

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
	CIM_ERR_SUCCESS,
	String::EMPTY,
	request->queueIds.copyAndPop());

    PEGASUS_ASSERT(response != 0);

    // propagate key
    response->setKey(request->getKey());

    // create a handler for this request
    class DeleteInstanceResponseHandler : public OperationResponseHandler<CIMInstance>
    {
    public:
	DeleteInstanceResponseHandler(
	    CIMDeleteInstanceRequestMessage * request,
	    CIMDeleteInstanceResponseMessage * response)
	: OperationResponseHandler<CIMInstance>(request, response)
	{
	}

    } handler(request, response);

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
    const CIMExecQueryRequestMessage * request =
	dynamic_cast<const CIMExecQueryRequestMessage *>(message);

    PEGASUS_ASSERT(request != 0);

    Array<CIMObjectWithPath> cimObjects;

    CIMExecQueryResponseMessage * response =
	new CIMExecQueryResponseMessage(
	request->messageId,
	CIM_ERR_FAILED,
	"not implemented",
	request->queueIds.copyAndPop(),
	cimObjects);

    PEGASUS_ASSERT(response != 0);

    // preserve message key
    response->setKey(request->getKey());

    // call the message queue service method to see if this is an async envelope
    _enqueueResponse(const_cast<Message *>(static_cast<const Message *>(request)), response);
}

void ProviderManagerService::handleAssociatorsRequest(const Message * message) throw()
{
    const CIMAssociatorsRequestMessage * request =
	dynamic_cast<const CIMAssociatorsRequestMessage *>(message);

    PEGASUS_ASSERT(request != 0);

    Array<CIMObjectWithPath> cimObjects;

    CIMAssociatorsResponseMessage * response =
	new CIMAssociatorsResponseMessage(
	request->messageId,
	CIM_ERR_FAILED,
	"not implemented",
	request->queueIds.copyAndPop(),
	cimObjects);

    PEGASUS_ASSERT(response != 0);

    // preserve message key
    response->setKey(request->getKey());

    // call the message queue service method to see if this is an async envelope
    _enqueueResponse(const_cast<Message *>(static_cast<const Message *>(request)), response);
}

void ProviderManagerService::handleAssociatorNamesRequest(const Message * message) throw()
{
    const CIMAssociatorNamesRequestMessage * request =
	dynamic_cast<const CIMAssociatorNamesRequestMessage *>(message);

    PEGASUS_ASSERT(request != 0);

    Array<CIMReference> cimReferences;

    CIMAssociatorNamesResponseMessage * response =
	new CIMAssociatorNamesResponseMessage(
	request->messageId,
	CIM_ERR_FAILED,
	"not implemented",
	request->queueIds.copyAndPop(),
	cimReferences);

    PEGASUS_ASSERT(response != 0);

    // preserve message key
    response->setKey(request->getKey());

    // call the message queue service method to see if this is an async envelope
    _enqueueResponse(const_cast<Message *>(static_cast<const Message *>(request)), response);
}

void ProviderManagerService::handleReferencesRequest(const Message * message) throw()
{
    const CIMReferencesRequestMessage * request =
	dynamic_cast<const CIMReferencesRequestMessage *>(message);

    PEGASUS_ASSERT(request != 0);

    Array<CIMObjectWithPath> cimObjects;

    CIMReferencesResponseMessage * response =
	new CIMReferencesResponseMessage(
	request->messageId,
	CIM_ERR_FAILED,
	"not implemented",
	request->queueIds.copyAndPop(),
	cimObjects);

    PEGASUS_ASSERT(response != 0);

    // preserve message key
    response->setKey(request->getKey());

    // call the message queue service method to see if this is an async envelope
    _enqueueResponse(const_cast<Message *>(static_cast<const Message *>(request)), response);
}

void ProviderManagerService::handleReferenceNamesRequest(const Message * message) throw()
{
    const CIMReferenceNamesRequestMessage * request =
	dynamic_cast<const CIMReferenceNamesRequestMessage *>(message);

    PEGASUS_ASSERT(request != 0);

    Array<CIMReference> cimReferences;

    CIMReferenceNamesResponseMessage * response =
	new CIMReferenceNamesResponseMessage(
	request->messageId,
	CIM_ERR_FAILED,
	"not implemented",
	request->queueIds.copyAndPop(),
	cimReferences);

    // preserve message key
    response->setKey(request->getKey());

    // call the message queue service method to see if this is an async envelope
    _enqueueResponse(const_cast<Message *>(static_cast<const Message *>(request)), response);
}

void ProviderManagerService::handleGetPropertyRequest(const Message * message) throw()
{
    const CIMGetPropertyRequestMessage * request =
	dynamic_cast<const CIMGetPropertyRequestMessage *>(message);

    PEGASUS_ASSERT(request != 0);

    CIMValue cimValue;

    // create response message
    CIMGetPropertyResponseMessage * response =
	new CIMGetPropertyResponseMessage(
	request->messageId,
	CIM_ERR_FAILED,
	"not implemented",
	request->queueIds.copyAndPop(),
	cimValue);

    PEGASUS_ASSERT(response != 0);

    // preserve message key
    response->setKey(request->getKey());

    // call the message queue service method to see if this is an async envelope
    _enqueueResponse(const_cast<Message *>(static_cast<const Message *>(request)), response);
}

void ProviderManagerService::handleSetPropertyRequest(const Message * message) throw()
{
    const CIMSetPropertyRequestMessage * request =
	dynamic_cast<const CIMSetPropertyRequestMessage *>(message);

    PEGASUS_ASSERT(request != 0);

    // create response message
    CIMSetPropertyResponseMessage * response =
	new CIMSetPropertyResponseMessage(
	request->messageId,
	CIM_ERR_FAILED,
	"not implemented",
	request->queueIds.copyAndPop());

    PEGASUS_ASSERT(response != 0);

    // preserve message key
    response->setKey(request->getKey());

    // call the message queue service method to see if this is an async envelope
    _enqueueResponse(const_cast<Message *>(static_cast<const Message *>(request)), response);
}

void ProviderManagerService::handleInvokeMethodRequest(const Message * message) throw()
{
    CIMInvokeMethodRequestMessage * request =
	dynamic_cast<CIMInvokeMethodRequestMessage *>(const_cast<Message *>(message));

    PEGASUS_ASSERT(request != 0);

    // create response message
    CIMInvokeMethodResponseMessage * response =
	new CIMInvokeMethodResponseMessage(
	request->messageId,
	CIM_ERR_SUCCESS,
	"",
	request->queueIds.copyAndPop(),
	CIMValue(),
	Array<CIMParamValue>(),
	request->methodName);

    PEGASUS_ASSERT(response != 0);

    // propagate key
    response->setKey(request->getKey());

    // create a handler for this request
    class InvokeMethodResponseHandler : public OperationResponseHandler<CIMValue>
    {
    public:
	InvokeMethodResponseHandler(
	    CIMInvokeMethodRequestMessage * request,
	    CIMInvokeMethodResponseMessage * response)
	: OperationResponseHandler<CIMValue>(request, response)
	{
	}

	virtual void complete(const OperationContext & context)
	{
	    // error? provider claims success, but did not deliver a CIMValue.
	    if(getObjects().size() == 0)
	    {
		((CIMInvokeMethodResponseMessage *)getResponse())->retValue = getObjects()[0];
	    }
	}

    } handler(request, response);

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
}

/*
// ATTN: deprecated
void ProviderManagerService::handleEnableIndicationRequest(const Message * message) throw()
{
    const CIMEnableIndicationSubscriptionRequestMessage * request =
	dynamic_cast<const CIMEnableIndicationSubscriptionRequestMessage *>(message);

    PEGASUS_ASSERT(request != 0);

    Status status;

    try
    {
	// make target object path
	CIMObjectPath objectPath(
	    System::getHostName(),
	    request->nameSpace,
	    request->classNames[0]);

	// get the provider file name and logical name
	Pair<String, String> pair = _lookupProviderForClass(objectPath);

	// get cached or load new provider module
	Provider provider = providerManager.getProvider(pair.first, pair.second);

	SimpleResponseHandler<CIMInstance> handler;

	try
	{
	    //
	    //  ATTN: pass thresholding parameter values in
	    //  operation context
	    //
	    provider.enableIndication(
		OperationContext(),
		request->nameSpace,
		request->classNames,
		request->propertyList,
		request->repeatNotificationPolicy,
		request->otherRepeatNotificationPolicy,
		request->repeatNotificationInterval,
		request->repeatNotificationGap,
		request->repeatNotificationCount,
		request->condition,
		request->queryLanguage,
		request->subscription,
		handler);
	}
	catch(...)
	{
	    status = Status(CIM_ERR_FAILED, "Provider not available");
	}
    }
    catch(CIMException & e)
    {
	status = Status(e.getCode(), e.getMessage());
    }
    catch(Exception & e)
    {
	status = Status(CIM_ERR_FAILED, e.getMessage());
    }
    catch(...)
    {
	status = Status(CIM_ERR_FAILED, "Unknown Error");
    }

    CIMEnableIndicationSubscriptionResponseMessage * response =
	new CIMEnableIndicationSubscriptionResponseMessage(
	request->messageId,
	CIMStatusCode(status.getCode()),
	status.getMessage(),
	request->queueIds.copyAndPop());

    PEGASUS_ASSERT(response != 0);

    // preserve message key
    response->setKey(request->getKey());

    // call the message queue service method to see if this is an async envelope
    _enqueueResponse(const_cast<Message *>(static_cast<const Message *>(request)), response);
}

// ATTN: deprecated
void ProviderManagerService::handleModifyIndicationRequest(const Message * message) throw()
{
    const CIMModifyIndicationSubscriptionRequestMessage * request =
	dynamic_cast<const CIMModifyIndicationSubscriptionRequestMessage *>(message);

    PEGASUS_ASSERT(request != 0);

    CIMModifyIndicationSubscriptionResponseMessage * response =
	new CIMModifyIndicationSubscriptionResponseMessage(
	request->messageId,
	CIM_ERR_FAILED,
	"not implemented",
	request->queueIds.copyAndPop());

    PEGASUS_ASSERT(response != 0);

    // preserve message key
    response->setKey(request->getKey());

    // call the message queue service method to see if this is an async envelope
    _enqueueResponse(const_cast<Message *>(static_cast<const Message *>(request)), response);
}

// ATTN: deprecated
void ProviderManagerService::handleDisableIndicationRequest(const Message * message) throw()
{
    const CIMDisableIndicationSubscriptionRequestMessage * request =
	dynamic_cast<const CIMDisableIndicationSubscriptionRequestMessage *>(message);

    PEGASUS_ASSERT(request != 0);

    CIMDisableIndicationSubscriptionResponseMessage * response =
	new CIMDisableIndicationSubscriptionResponseMessage(
	request->messageId,
	CIM_ERR_FAILED,
	"not implemented",
	request->queueIds.copyAndPop());

    PEGASUS_ASSERT(response != 0);

    // preserve message key
    response->setKey(request->getKey());

    // call the message queue service method to see if this is an async envelope
    _enqueueResponse(const_cast<Message *>(static_cast<const Message *>(request)), response);
}
*/

void ProviderManagerService::handleCreateSubscriptionRequest(const Message * message) throw()
{
    CIMCreateSubscriptionRequestMessage * request =
	dynamic_cast<CIMCreateSubscriptionRequestMessage *>(const_cast<Message *>(message));

    PEGASUS_ASSERT(request != 0);

    CIMCreateSubscriptionResponseMessage * response =
	new CIMCreateSubscriptionResponseMessage(
	request->messageId,
	CIM_ERR_SUCCESS,
	"",
	request->queueIds.copyAndPop());

    PEGASUS_ASSERT(response != 0);

    // preserve message key
    response->setKey(request->getKey());

    OperationResponseHandler<CIMIndication> handler(request, response);

    try
    {
	// make target object path
	CIMObjectPath objectPath(
	    System::getHostName(),
	    request->nameSpace,
	    request->classNames[0]);

	// get the provider file name and logical name
	Pair<String, String> pair = _lookupProviderForClass(objectPath);

	// get cached or load new provider module
	Provider provider = providerManager.getProvider(pair.first, pair.second);

	throw CIMException(CIM_ERR_NOT_SUPPORTED, "under construction.");
	
	/*
	provider.createSubscription(
	    OperationContext(),
	    subscriptionName,
	    classNames,
	    repeatNotificationPolicy);
	*/
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
	CIM_ERR_SUCCESS,
	"",
	request->queueIds.copyAndPop());

    PEGASUS_ASSERT(response != 0);

    // preserve message key
    response->setKey(request->getKey());

    OperationResponseHandler<CIMIndication> handler(request, response);

    try
    {
	// make target object path
	CIMObjectPath objectPath(
	    System::getHostName(),
	    request->nameSpace,
	    request->classNames[0]);

	// get the provider file name and logical name
	Pair<String, String> pair = _lookupProviderForClass(objectPath);

	// get cached or load new provider module
	Provider provider = providerManager.getProvider(pair.first, pair.second);
	
	throw CIMException(CIM_ERR_NOT_SUPPORTED, "under construction.");
	
	/*
	provider.modifySubscription(
	    OperationContext(),
	    subscriptionName,
	    classNames,
	    repeatNotificationPolicy);
	*/
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
	CIM_ERR_SUCCESS,
	"",
	request->queueIds.copyAndPop());

    PEGASUS_ASSERT(response != 0);

    // preserve message key
    response->setKey(request->getKey());

    OperationResponseHandler<CIMIndication> handler(request, response);

    try
    {
	// make target object path
	CIMObjectPath objectPath(
	    System::getHostName(),
	    request->nameSpace,
	    request->classNames[0]);

	// get the provider file name and logical name
	Pair<String, String> pair = _lookupProviderForClass(objectPath);

	// get cached or load new provider module
	Provider provider = providerManager.getProvider(pair.first, pair.second);

	throw CIMException(CIM_ERR_NOT_SUPPORTED, "under construction.");
	
	/*
	provider.deleteSubscription(
	    OperationContext(),
	    subscriptionName,
	    classNames);
	*/	
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

/*
void ProviderManagerService::handleEnableIndications(const Message * message)
{
}

void ProviderManagerService::handleDisableIndications(const Message * message)
{
}
*/

PEGASUS_NAMESPACE_END
