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

#include <Pegasus/ProviderManager/ProviderManager.h>
#include <Pegasus/ProviderManager/ProviderFacade.h>

#include <Pegasus/Provider/OperationFlag.h>
#include <Pegasus/Provider/SimpleResponseHandler.h>

#include <Pegasus/Config/ConfigManager.h>

//#include <Pegasus/Server/CIMOperationRequestDispatcher.h>

PEGASUS_NAMESPACE_BEGIN

static ProviderManager providerManager;

class Status
{
public:
    Status(void)
    : _code(0), _message("")
    {
    }

    Status(const Uint32 code, const String & message)
    : _code(code), _message(message)
    {
    }

    Uint32 getCode(void) const
    {
	return(_code);
    }

    String getMessage(void) const
    {
	return(_message);
    }

private:
    Uint32 _code;
    String _message;

};

static struct timeval await = { 0, 40};
static struct timeval dwait = { 10, 0};
static struct timeval deadwait = { 1, 0};

ProviderManagerService::ProviderManagerService(void)
    : MessageQueueService("Server::ProviderManagerService", MessageQueue::getNextQueueId()),
    _threadPool(10, "ProviderManagerService", 1, 5, await, dwait, deadwait),
    _threadSemaphore(0)
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
    CIMOMHandle _cimom(this);

    // get all provider capability instances
    Array<CIMInstance> cimInstances =
	_cimom.enumerateInstances(
	    OperationContext(),
	    "root/cimv2",
	    "PG_ProviderCapabilities",
	    false, false, false, false,
	    CIMPropertyList());

    CIMInstance cimInstance;

    for(Uint32 i = 0, n = cimInstances.size(); i < n; i++)
    {
	// check class name
	String className = cimInstances[i].getProperty(cimInstances[i].findProperty("ClassName")).getValue().toString();

	if(String::equalNoCase(className, objectPath.getClassName()))
	{
	    cimInstance = cimInstances[i];

	    break;
	}
    }

    // get provider and provider module name
    String providerName = cimInstance.getProperty(cimInstance.findProperty("ProviderName")).getValue().toString();
    String moduleName = cimInstance.getProperty(cimInstance.findProperty("ProviderModuleName")).getValue().toString();

    // get the module instance
    CIMInstance moduleInstance =
	_cimom.getInstance(
	    OperationContext(),
	    "root/cimv2",
	    CIMObjectPath("root/cimv2:PG_ProviderModule.Name=\"" + moduleName +"\""),
	    false, false, false,
	    CIMPropertyList());

    // get the module location
    String location = moduleInstance.getProperty(moduleInstance.findProperty("Location")).getValue().toString();

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

    return(Pair<String, String>(fileName, providerName));
}

Boolean ProviderManagerService::messageOK(const Message * message)
{
    PEGASUS_ASSERT(message != 0);

    return(MessageQueueService::messageOK(message));
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

    //delete asyncReply;
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

	// wait for method to accept
	_threadSemaphore.wait();

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

    // notify the service that the request has been accepted
    service->_threadSemaphore.signal();

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

    // notify the service that the request has been accepted
    service->_threadSemaphore.signal();

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
    case CIM_ENABLE_INDICATION_SUBSCRIPTION_REQUEST_MESSAGE:
	service->handleEnableIndicationRequest(message);

	break;
    case CIM_MODIFY_INDICATION_SUBSCRIPTION_REQUEST_MESSAGE:
	service->handleModifyIndicationRequest(message);

	break;
    case CIM_DISABLE_INDICATION_SUBSCRIPTION_REQUEST_MESSAGE:
	service->handleDisableIndicationRequest(message);

	break;
    default:
	// unsupported messages are ignored
	break;
    }

    return(0);
}

void ProviderManagerService::handleGetInstanceRequest(Message * message) throw()
{
    const CIMGetInstanceRequestMessage * request =
	dynamic_cast<CIMGetInstanceRequestMessage *>(message);

    PEGASUS_ASSERT(request != 0);

    Status status;

    CIMInstance cimInstance;

    try
    {
	// make class reference
	CIMReference classReference(
	    request->instanceName.getHost(),
	    request->nameSpace,
	    request->instanceName.getClassName());

	// get the provider file name and logical name
	Pair<String, String> pair = _lookupProviderForClass(classReference);

	// get cached or load new provider module
	Provider provider = providerManager.getProvider(pair.first, pair.second);

	// convert arguments
	OperationContext context;

	context.add_context(sizeof(String *),
	    const_cast<String *>(&(request->userName)),
	    0,
	    0,
	    CONTEXT_IDENTITY,
	    0,
	    0);

	CIMReference instanceReference(request->instanceName);

	// ATTN: propagate namespace
	instanceReference.setNameSpace(request->nameSpace);

	// convert flags to bitmask
	Uint32 flags = OperationFlag::convert(false);
	
	// ATTN: strip flags inappropriate for providers
	flags = flags | ~OperationFlag::LOCAL_ONLY | ~OperationFlag::DEEP_INHERITANCE;

	CIMPropertyList propertyList(request->propertyList);

	SimpleResponseHandler<CIMInstance> handler;

	// forward request
	provider.getInstance(
	    context,
	    instanceReference,
	    flags,
	    propertyList.getPropertyNameArray(),
	    handler);

	// error? provider claims success, but did not deliver an instance.
	if(handler.getObjects().size() == 0)
	{
	    throw CIMException(CIM_ERR_NOT_FOUND);
	}

	// save returned instance
	cimInstance = handler.getObjects()[0];
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

    // create response message
    CIMGetInstanceResponseMessage * response =
	new CIMGetInstanceResponseMessage(
	request->messageId,
	CIMStatusCode(status.getCode()),
	status.getMessage(),
	request->queueIds.copyAndPop(),
	cimInstance);

    // preserve message key
    response->setKey(request->getKey());

    // call the message queue service method to see if this is an async envelope
    _enqueueResponse((Message *)request, (Message *)response);
}

void ProviderManagerService::handleEnumerateInstancesRequest(Message * message) throw()
{
    const CIMEnumerateInstancesRequestMessage * request =
	dynamic_cast<CIMEnumerateInstancesRequestMessage *>(message);

    PEGASUS_ASSERT(request != 0);

    Status status;

    Array<CIMNamedInstance> cimInstances;

    try
    {
	// make class reference
	CIMReference classReference(
	    "",
	    request->nameSpace,
	    request->className);

	// get the provider file name and logical name
	Pair<String, String> pair = _lookupProviderForClass(classReference);

	// get cached or load new provider module
	Provider provider = providerManager.getProvider(pair.first, pair.second);

	// convert arguments
	OperationContext context;

	// ATTN: propagate namespace
	classReference.setNameSpace(request->nameSpace);

	// convert flags to bitmask
	Uint32 flags = OperationFlag::convert(false);
	
	// ATTN: strip flags inappropriate for providers
	flags = flags | ~OperationFlag::LOCAL_ONLY | ~OperationFlag::DEEP_INHERITANCE;

	CIMPropertyList propertyList(request->propertyList);

	SimpleResponseHandler<CIMInstance> handler;

	provider.enumerateInstances(
	    context,
	    classReference,
	    flags,
	    propertyList.getPropertyNameArray(),
	    handler);

	// save returned instance

	// ATTN: can be removed once CIMNamedInstance is removed
	for(Uint32 i = 0, n = handler.getObjects().size(); i < n; i++)
	{
	    cimInstances.append(CIMNamedInstance(handler.getObjects()[i].getPath(), handler.getObjects()[i]));
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

    // create response message
    CIMEnumerateInstancesResponseMessage * response =
	new CIMEnumerateInstancesResponseMessage(
	request->messageId,
	CIMStatusCode(status.getCode()),
	status.getMessage(),
	request->queueIds.copyAndPop(),
	cimInstances);

    // preserve message key
    response->setKey(request->getKey());

    // call the message queue service method to see if this is an async envelope
    _enqueueResponse((Message *)request, (Message *)response);
}

void ProviderManagerService::handleEnumerateInstanceNamesRequest(Message * message) throw()
{
    const CIMEnumerateInstanceNamesRequestMessage * request =
	dynamic_cast<CIMEnumerateInstanceNamesRequestMessage *>(message);

    PEGASUS_ASSERT(request != 0);

    Status status;

    Array<CIMReference> cimReferences;

    try
    {
	// make class reference
	CIMReference classReference(
	    "",
	    request->nameSpace,
	    request->className);

	// get the provider file name and logical name
	Pair<String, String> pair = _lookupProviderForClass(classReference);

	// get cached or load new provider module
	Provider provider = providerManager.getProvider(pair.first, pair.second);

	// convert arguments
	OperationContext context;

	// ATTN: propagate namespace
	classReference.setNameSpace(request->nameSpace);

	SimpleResponseHandler<CIMReference> handler;

	provider.enumerateInstanceNames(
	    context,
	    classReference,
	    handler);

	// save returned instance
	cimReferences = handler.getObjects();
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

    // create response message
    CIMEnumerateInstanceNamesResponseMessage * response =
	new CIMEnumerateInstanceNamesResponseMessage(
	request->messageId,
	CIMStatusCode(status.getCode()),
	status.getMessage(),
	request->queueIds.copyAndPop(),
	cimReferences);

    // preserve message key
    response->setKey(request->getKey());

    // call the message queue service method to see if this is an async envelope
    _enqueueResponse((Message *)request, (Message *)response);
}

void ProviderManagerService::handleCreateInstanceRequest(Message * message) throw()
{
    const CIMCreateInstanceRequestMessage * request =
	dynamic_cast<CIMCreateInstanceRequestMessage *>(message);

    PEGASUS_ASSERT(request != 0);

    CIMInstance cimInstance;
    CIMReference instanceName;
    Status status;

    try
    {
	String className = request->newInstance.getClassName();

	// make class reference
	CIMReference classReference(
	    "",
	    request->nameSpace,
	    className);

	// get the provider file name and logical name
	Pair<String, String> pair = _lookupProviderForClass(classReference);

	// get cached or load new provider module
	Provider provider = providerManager.getProvider(pair.first, pair.second);

	// convert arguments
	OperationContext context;

	CIMReference instanceReference;

	// ATTN: propagate namespace
	instanceReference.setNameSpace(request->nameSpace);

	// ATTN: need to handle key binding
	instanceReference.setClassName(className);

	SimpleResponseHandler<CIMReference> handler;

	// forward request
	provider.createInstance(
	    context,
	    instanceReference,
	    request->newInstance,
	    handler);

	// error? provider claims success, but did not deliver an
	// instance name.
	if(handler.getObjects().size() == 0)
	{
	    throw CIMException(CIM_ERR_NOT_FOUND);
	}

	// save returned instance name
	instanceName = handler.getObjects()[0];
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

    // create response message
    CIMCreateInstanceResponseMessage * response =
	new CIMCreateInstanceResponseMessage(
	request->messageId,
	CIMStatusCode(status.getCode()),
	status.getMessage(),
	request->queueIds.copyAndPop(),
	instanceName);

    // preserve message key
    response->setKey(request->getKey());

    // call the message queue service method to see if this is an async envelope
    _enqueueResponse((Message *)request, (Message *)response);
}

void ProviderManagerService::handleModifyInstanceRequest(Message * message) throw()
{
    const CIMModifyInstanceRequestMessage * request =
	dynamic_cast<CIMModifyInstanceRequestMessage *>(message);

    PEGASUS_ASSERT(request != 0);

    CIMReference instanceName;
    Status status;

    try
    {
	instanceName = request->modifiedInstance.getInstanceName();
	String className = instanceName.getClassName();

	// make class reference
	CIMReference classReference(
	    instanceName.getHost(),
	    request->nameSpace,
	    className);

	// get the provider file name and logical name
	Pair<String, String> pair = _lookupProviderForClass(classReference);

	// get cached or load new provider module
	Provider provider = providerManager.getProvider(pair.first, pair.second);

	// convert arguments
	OperationContext context;

	// convert flags to bitmask
	Uint32 flags = OperationFlag::convert(false);
	
	// ATTN: strip flags inappropriate for providers
	flags = flags | ~OperationFlag::LOCAL_ONLY | ~OperationFlag::DEEP_INHERITANCE;

	CIMPropertyList propertyList(request->propertyList);

	SimpleResponseHandler<CIMInstance> handler;

	// forward request
	provider.modifyInstance(
	    context,
	    instanceName,
	    request->modifiedInstance.getInstance(),
	    flags,
	    propertyList.getPropertyNameArray(),
	    handler);
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

    // create response message
    CIMModifyInstanceResponseMessage * response =
	new CIMModifyInstanceResponseMessage(
	request->messageId,
	CIMStatusCode(status.getCode()),
	status.getMessage(),
	request->queueIds.copyAndPop());

    // preserve message key
    response->setKey(request->getKey());

    // call the message queue service method to see if this is an async envelope
    _enqueueResponse((Message *)request, (Message *)response);
}

void ProviderManagerService::handleDeleteInstanceRequest(Message * message) throw()
{
    const CIMDeleteInstanceRequestMessage * request =
	dynamic_cast<CIMDeleteInstanceRequestMessage *>(message);

    PEGASUS_ASSERT(request != 0);

    Status status;

    try
    {
	String className = request->instanceName.getClassName();

	// make class reference
	CIMReference classReference(
	    request->instanceName.getHost(),
	    request->nameSpace,
	    className);

	// get the provider file name and logical name
	Pair<String, String> pair = _lookupProviderForClass(classReference);

	// get cached or load new provider module
	Provider provider = providerManager.getProvider(pair.first, pair.second);

	// convert arguments
	OperationContext context;

	context.add_context(sizeof(String *),
	    const_cast<String *>(&(request->userName)),
	    0,
	    0,
	    CONTEXT_IDENTITY,
	    0,
	    0);

	SimpleResponseHandler<CIMInstance> handler;

	// forward request
	provider.deleteInstance(
	    context,
	    request->instanceName,
	    handler);
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

    // create response message
    CIMDeleteInstanceResponseMessage * response =
	new CIMDeleteInstanceResponseMessage(
	request->messageId,
	CIMStatusCode(status.getCode()),
	status.getMessage(),
	request->queueIds.copyAndPop());

    // preserve message key
    response->setKey(request->getKey());

    // call the message queue service method to see if this is an async envelope
    _enqueueResponse((Message *)request, (Message *)response);
}

void ProviderManagerService::handleExecuteQueryRequest(Message * message) throw()
{
    const CIMExecQueryRequestMessage * request =
	dynamic_cast<CIMExecQueryRequestMessage *>(message);

    PEGASUS_ASSERT(request != 0);

    Array<CIMInstance> cimInstances;

    CIMExecQueryResponseMessage * response =
	new CIMExecQueryResponseMessage(
	request->messageId,
	CIM_ERR_FAILED,
	"not implemented",
	request->queueIds.copyAndPop(),
	cimInstances);

    // preserve message key
    response->setKey(request->getKey());

    // call the message queue service method to see if this is an async envelope
    _enqueueResponse((Message *)request, (Message *)response);
}

void ProviderManagerService::handleAssociatorsRequest(Message * message) throw()
{
    const CIMAssociatorsRequestMessage * request =
	dynamic_cast<CIMAssociatorsRequestMessage *>(message);

    PEGASUS_ASSERT(request != 0);

    Array<CIMObjectWithPath> cimObjects;

    CIMAssociatorsResponseMessage * response =
	new CIMAssociatorsResponseMessage(
	request->messageId,
	CIM_ERR_FAILED,
	"not implemented",
	request->queueIds.copyAndPop(),
	cimObjects);

    // preserve message key
    response->setKey(request->getKey());

    // call the message queue service method to see if this is an async envelope
    _enqueueResponse((Message *)request, (Message *)response);
}

void ProviderManagerService::handleAssociatorNamesRequest(Message * message) throw()
{
    const CIMAssociatorNamesRequestMessage * request =
	dynamic_cast<CIMAssociatorNamesRequestMessage *>(message);

    PEGASUS_ASSERT(request != 0);

    Array<CIMReference> cimReferences;

    CIMAssociatorNamesResponseMessage * response =
	new CIMAssociatorNamesResponseMessage(
	request->messageId,
	CIM_ERR_FAILED,
	"not implemented",
	request->queueIds.copyAndPop(),
	cimReferences);

    // preserve message key
    response->setKey(request->getKey());

    // call the message queue service method to see if this is an async envelope
    _enqueueResponse((Message *)request, (Message *)response);
}

void ProviderManagerService::handleReferencesRequest(Message * message) throw()
{
    const CIMReferencesRequestMessage * request =
	dynamic_cast<CIMReferencesRequestMessage *>(message);

    PEGASUS_ASSERT(request != 0);

    Array<CIMObjectWithPath> cimObjects;

    CIMReferencesResponseMessage * response =
	new CIMReferencesResponseMessage(
	request->messageId,
	CIM_ERR_FAILED,
	"not implemented",
	request->queueIds.copyAndPop(),
	cimObjects);

    // preserve message key
    response->setKey(request->getKey());

    // call the message queue service method to see if this is an async envelope
    _enqueueResponse((Message *)request, (Message *)response);
}

void ProviderManagerService::handleReferenceNamesRequest(Message * message) throw()
{
    const CIMReferenceNamesRequestMessage * request =
	dynamic_cast<CIMReferenceNamesRequestMessage *>(message);

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
    _enqueueResponse((Message *)request, (Message *)response);
}

void ProviderManagerService::handleGetPropertyRequest(Message * message) throw()
{
    const CIMGetPropertyRequestMessage * request =
	dynamic_cast<CIMGetPropertyRequestMessage *>(message);

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

    // preserve message key
    response->setKey(request->getKey());

    // call the message queue service method to see if this is an async envelope
    _enqueueResponse((Message *)request, (Message *)response);
}

void ProviderManagerService::handleSetPropertyRequest(Message * message) throw()
{
    const CIMSetPropertyRequestMessage * request =
	dynamic_cast<CIMSetPropertyRequestMessage *>(message);

    PEGASUS_ASSERT(request != 0);

    // create response message
    CIMSetPropertyResponseMessage * response =
	new CIMSetPropertyResponseMessage(
	request->messageId,
	CIM_ERR_FAILED,
	"not implemented",
	request->queueIds.copyAndPop());

    // preserve message key
    response->setKey(request->getKey());

    // call the message queue service method to see if this is an async envelope
    _enqueueResponse((Message *)request, (Message *)response);
}

void ProviderManagerService::handleInvokeMethodRequest(Message * message) throw()
{
    const CIMInvokeMethodRequestMessage * request =
	dynamic_cast<CIMInvokeMethodRequestMessage *>(message);

    PEGASUS_ASSERT(request != 0);

    // process request
    CIMValue returnValue;
    Array<CIMParamValue> outParameters;
    Status status;
    CIMInstance cimInstance;

    try
    {
	// make class reference
	CIMReference classReference(
	    request->instanceName.getHost(),
	    request->nameSpace,
	    request->instanceName.getClassName());

	// get the provider file name and logical name
	Pair<String, String> pair = _lookupProviderForClass(classReference);

	// get cached or load new provider module
	Provider provider = providerManager.getProvider(pair.first, pair.second);

	// convert arguments
	OperationContext context;

	CIMReference instanceReference(request->instanceName);

	// ATTN: propagate namespace
	instanceReference.setNameSpace(request->nameSpace);

	SimpleResponseHandler<CIMValue> handler;

	// forward request
	provider.invokeMethod(
	    context,
	    instanceReference,
	    request->methodName,
	    request->inParameters,
	    outParameters,
	    handler);

	// error? provider claims success, but did not deliver a CIMValue.
	if(handler.getObjects().size() == 0)
	{
	    throw CIMException(CIM_ERR_NOT_FOUND);
	}

	returnValue = handler.getObjects()[0];
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

    // create response message
    CIMInvokeMethodResponseMessage * response =
	new CIMInvokeMethodResponseMessage(
	request->messageId,
	CIMStatusCode(status.getCode()),
	status.getMessage(),
	request->queueIds.copyAndPop(),
	returnValue,
	outParameters,
	request->methodName);

    // preserve message key
    response->setKey(request->getKey());

    // call the message queue service method to see if this is an async envelope
    _enqueueResponse((Message *)request, (Message *)response);
}

void ProviderManagerService::handleEnableIndicationRequest(Message * message) throw()
{
    const CIMEnableIndicationSubscriptionRequestMessage * request =
	dynamic_cast<CIMEnableIndicationSubscriptionRequestMessage *>(message);

    PEGASUS_ASSERT(request != 0);

    Status status;

    try
    {
	// make class reference
	CIMReference classReference(
	    "",
	    request->nameSpace,
	    request->classNames[0]);

	// get the provider file name and logical name
	Pair<String, String> pair = _lookupProviderForClass(classReference);

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

    // preserve message key
    response->setKey(request->getKey());

    // call the message queue service method to see if this is an async envelope
    _enqueueResponse((Message *)request, (Message *)response);
}

void ProviderManagerService::handleModifyIndicationRequest(Message * message) throw()
{
    const CIMModifyIndicationSubscriptionRequestMessage * request =
	dynamic_cast<CIMModifyIndicationSubscriptionRequestMessage *>(message);

    PEGASUS_ASSERT(request != 0);

    CIMModifyIndicationSubscriptionResponseMessage * response =
	new CIMModifyIndicationSubscriptionResponseMessage(
	request->messageId,
	CIM_ERR_FAILED,
	"not implemented",
	request->queueIds.copyAndPop());

    // preserve message key
    response->setKey(request->getKey());

    // call the message queue service method to see if this is an async envelope
    _enqueueResponse((Message *)request, (Message *)response);
}

void ProviderManagerService::handleDisableIndicationRequest(Message * message) throw()
{
    const CIMDisableIndicationSubscriptionRequestMessage * request =
	dynamic_cast<CIMDisableIndicationSubscriptionRequestMessage *>(message);

    PEGASUS_ASSERT(request != 0);

    CIMDisableIndicationSubscriptionResponseMessage * response =
	new CIMDisableIndicationSubscriptionResponseMessage(
	request->messageId,
	CIM_ERR_FAILED,
	"not implemented",
	request->queueIds.copyAndPop());

    // preserve message key
    response->setKey(request->getKey());

    // call the message queue service method to see if this is an async envelope
    _enqueueResponse((Message *)request, (Message *)response);
}

PEGASUS_NAMESPACE_END
