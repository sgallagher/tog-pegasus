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
	_threadPool(10, "ProviderManagerService", 5, 15, await, dwait, deadwait),
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
	/*
	Array<CIMInstance> providers;
	
	try
	{
		SimpleResponseHandler<CIMInstance> handler;

		_cimom.enumerateInstances(
			OperationContext(),
			"PG_Provider",
			OperationFlag::LOCAL_ONLY,
			CIMPropertyList(),
			handler);

		providers = handler.getObjects();
	}
	catch(...)
	{
	}

	Array<CIMInstance> modules;
	
	try
	{
		SimpleResponseHandler<CIMInstance> handler;

		_cimom.enumerateInstances(
			OperationContext(),
			"PG_Provider",
			OperationFlag::LOCAL_ONLY,
			CIMPropertyList(),
			handler);

		modules = handler.getObjects();
	}
	catch(...)
	{
	}
	*/

	/*
	// find the cim operation processor service
	Array<Uint32> serviceIds;

	find_services("CIMOpRequestDispatcher", 0, 0, &serviceIds);

	PEGASUS_ASSERT(serviceIds.size() != 0);

	MessageQueue * queue = MessageQueue::lookup(serviceIds[0]);

	PEGASUS_ASSERT(queue != 0);

	CIMOperationRequestDispatcher * dispatcher = (CIMOperationRequestDispatcher *)queue;

	ProviderRegistrationManager * registrar = dispatcher->getProviderRegistrationManager();

	PEGASUS_ASSERT(registrar != 0);
	
	CIMInstance providerInstance;
	CIMInstance moduleInstance;

	registrar->lookupInstanceProvider(
		objectPath.getNameSpace(),
		objectPath.getClassName(),
		providerInstance,
		moduleInstance);

	// check interface type
	String interfaceType = moduleInstance.getProperty(moduleInstance.findProperty("InterfaceType")).getValue().toString();
		
	if(!String::equalNoCase(interfaceType, "PG_DefaultC++"))
	{
		throw CIMException(CIM_ERR_FAILED, "Unsupported or undefined provider interface type.");
	}
	
	// check interface version
	String interfaceVersion = moduleInstance.getProperty(moduleInstance.findProperty("InterfaceVersion")).getValue().toString();
		
	if(!String::equalNoCase(interfaceVersion, "2.0.0"))
	{
		throw CIMException(CIM_ERR_FAILED, "Invalid provider interface version.");
	}
	
	// get the module location
	String location = moduleInstance.getProperty(moduleInstance.findProperty("Location")).getValue().toString();
	
	if(location.size == 0)
	{
		throw CIMException(CIM_ERR_FAILED, "Invalid provider location.");
	}
	
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

	// get the provider name
	String providerName = providerInstance.getProperty(providerInstance.findProperty("Name")).getValue().toString();

	if(providerName.size == 0)
	{
		throw CIMException(CIM_ERR_FAILED, "Invalid provider name.");
	}
	
	return(Pair<String, String>(fileName, providerName));
	*/
	return(Pair<String, String>("", ""));
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
	return;
}

void ProviderManagerService::handleEnqueue(Message * message)
{
	return;
}

void ProviderManagerService::_handle_async_request(AsyncRequest * request)
{
	PEGASUS_ASSERT(request != 0);

	if(request->getType() == async_messages::ASYNC_LEGACY_OP_START)
	{
		request->op->processing();

		Message * message = (static_cast<AsyncLegacyOperationStart *>(request)->get_action());

		PEGASUS_ASSERT(message != 0);

		messageQueue.enqueue(message);

		handleCimOperation();

		return;
	}

	// pass all other operations to the default handler
	MessageQueueService::_handle_async_request(request);
}

void ProviderManagerService::handleCimOperation(void) throw()
{
	// peek at the message in fron of the queue. a specialized handler method will
	// remove the message once processing begins.
	const Message * message = messageQueue.front();

	PEGASUS_ASSERT(message != 0);

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
		// forward request to specialized method
		_threadPool.allocate_and_awaken((void *)this, handleGetInstanceRequest);

		// wait for specialized method to initialize
		_threadSemaphore.wait();

		break;
	case CIM_ENUMERATE_INSTANCES_REQUEST_MESSAGE:
		// forward request to specialized method
		_threadPool.allocate_and_awaken((void *)this, handleEnumerateInstancesRequest);

		// wait for specialized method to initialize
		_threadSemaphore.wait();

		break;
	case CIM_ENUMERATE_INSTANCE_NAMES_REQUEST_MESSAGE:
		// forward request to specialized method
		_threadPool.allocate_and_awaken((void *)this, handleEnumerateInstanceNamesRequest);

		// wait for specialized method to initialize
		_threadSemaphore.wait();

		break;
	case CIM_CREATE_INSTANCE_REQUEST_MESSAGE:
		// forward request to specialized method
		_threadPool.allocate_and_awaken((void *)this, handleCreateInstanceRequest);

		// wait for specialized method to initialize
		_threadSemaphore.wait();

		break;
	case CIM_MODIFY_INSTANCE_REQUEST_MESSAGE:
		// forward request to specialized method
		_threadPool.allocate_and_awaken((void *)this, handleModifyInstanceRequest);

		// wait for specialized method to initialize
		_threadSemaphore.wait();

		break;
	case CIM_DELETE_INSTANCE_REQUEST_MESSAGE:
		// forward request to specialized method
		_threadPool.allocate_and_awaken((void *)this, handleDeleteInstanceRequest);

		// wait for specialized method to initialize
		_threadSemaphore.wait();

		break;
	case CIM_EXEC_QUERY_REQUEST_MESSAGE:
		// forward request to specialized method
		_threadPool.allocate_and_awaken((void *)this, handleExecuteQueryRequest);

		// wait for specialized method to initialize
		_threadSemaphore.wait();

		break;
	case CIM_ASSOCIATORS_REQUEST_MESSAGE:
		// forward request to specialized method
		_threadPool.allocate_and_awaken((void *)this, handleAssociatorsRequest);

		// wait for specialized method to initialize
		_threadSemaphore.wait();

		break;
	case CIM_ASSOCIATOR_NAMES_REQUEST_MESSAGE:
		// forward request to specialized method
		_threadPool.allocate_and_awaken((void *)this, handleAssociatorNamesRequest);

		// wait for specialized method to initialize
		_threadSemaphore.wait();

		break;
	case CIM_REFERENCES_REQUEST_MESSAGE:
		// forward request to specialized method
		_threadPool.allocate_and_awaken((void *)this, handleReferencesRequest);

		// wait for specialized method to initialize
		_threadSemaphore.wait();

		break;
	case CIM_REFERENCE_NAMES_REQUEST_MESSAGE:
		// forward request to specialized method
		_threadPool.allocate_and_awaken((void *)this, handleReferenceNamesRequest);

		// wait for specialized method to initialize
		_threadSemaphore.wait();

		break;
	case CIM_GET_PROPERTY_REQUEST_MESSAGE:
		// forward request to specialized method
		_threadPool.allocate_and_awaken((void *)this, handleGetPropertyRequest);

		// wait for specialized method to initialize
		_threadSemaphore.wait();

		break;
	case CIM_SET_PROPERTY_REQUEST_MESSAGE:
		// forward request to specialized method
		_threadPool.allocate_and_awaken((void *)this, handleSetPropertyRequest);

		// wait for specialized method to initialize
		_threadSemaphore.wait();

		break;
	case CIM_INVOKE_METHOD_REQUEST_MESSAGE:
		// forward request to specialized method
		_threadPool.allocate_and_awaken((void *)this, handleInvokeMethodRequest);

		// wait for specialized method to initialize
		_threadSemaphore.wait();

		break;
	case CIM_ENABLE_INDICATION_SUBSCRIPTION_REQUEST_MESSAGE:
		// forward request to specialized method
		_threadPool.allocate_and_awaken((void *)this, handleEnableIndicationRequest);

		// wait for specialized method to initialize
		_threadSemaphore.wait();

		break;
	case CIM_MODIFY_INDICATION_SUBSCRIPTION_REQUEST_MESSAGE:
		// forward request to specialized method
		_threadPool.allocate_and_awaken((void *)this, handleModifyIndicationRequest);

		// wait for specialized method to initialize
		_threadSemaphore.wait();

		break;
	case CIM_DISABLE_INDICATION_SUBSCRIPTION_REQUEST_MESSAGE:
		// forward request to specialized method
		_threadPool.allocate_and_awaken((void *)this, handleDisableIndicationRequest);

		// wait for specialized method to initialize
		_threadSemaphore.wait();

		break;
	default:
		// unsupported messages are ignored
		break;
	}
}

PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL ProviderManagerService::handleGetInstanceRequest(void * arg) throw()
{
	// get the service from argument
	ProviderManagerService * service = reinterpret_cast<ProviderManagerService *>(arg);

	PEGASUS_ASSERT(service != 0);

	// get message from service queue
	const CIMGetInstanceRequestMessage * request =
		(const CIMGetInstanceRequestMessage *)service->messageQueue.dequeue();

	PEGASUS_ASSERT(request != 0);

	// notify the service that the request has been accepted
	service->_threadSemaphore.signal();

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
		Pair<String, String> pair = service->_lookupProviderForClass(classReference);

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

		// to retrieve the identity key from the context within a provider :
		// NOTE FOR ROGER << Thu Mar  7 17:18:11 2002 mdd >>
		// 		context *identity;
		// 		identity = op_context.remove_context_key(CONTEXT_IDENTITY);
		// 		if(identity != 0 )
		// 		{
		// 		   String *userName ;
		// 		   Uint32 size;
		// 		   userName = (String *)identity->get_data((void **)&userName, &size);
		// 		   if(userName != 0 )
		// 		      userName->print();
		// 		}

		CIMReference instanceReference(request->instanceName);

		// ATTN: propagate namespace
		instanceReference.setNameSpace(request->nameSpace);

		// ATTN: convert flags to bitmask
		Uint32 flags = OperationFlag::convert(false);
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
	service->_enqueueResponse((Message *)request, (Message *)response);
	
	delete request;
		
	return(0);
}

PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL ProviderManagerService::handleEnumerateInstancesRequest(void * arg) throw()
{
	// get the service from argument
	ProviderManagerService * service = reinterpret_cast<ProviderManagerService *>(arg);

	PEGASUS_ASSERT(service != 0);

	// get message from service queue
	const CIMEnumerateInstancesRequestMessage * request =
		(const CIMEnumerateInstancesRequestMessage *)service->messageQueue.dequeue();

	PEGASUS_ASSERT(request != 0);

	// notify the service that the request has been accepted
	service->_threadSemaphore.signal();

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
		Pair<String, String> pair = service->_lookupProviderForClass(classReference);

		// get cached or load new provider module
		Provider provider = providerManager.getProvider(pair.first, pair.second);

		// convert arguments
		OperationContext context;

		// ATTN: propagate namespace
		classReference.setNameSpace(request->nameSpace);

		// ATTN: convert flags to bitmask
		Uint32 flags = OperationFlag::convert(false);
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
	service->_enqueueResponse((Message *)request, (Message *)response);
	
	delete request;
		
	return(0);
}

PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL ProviderManagerService::handleEnumerateInstanceNamesRequest(void * arg) throw()
{
	// get the service from argument
	ProviderManagerService * service = reinterpret_cast<ProviderManagerService *>(arg);

	PEGASUS_ASSERT(service != 0);

	// get message from service queue
	const CIMEnumerateInstanceNamesRequestMessage * request =
		(const CIMEnumerateInstanceNamesRequestMessage *)service->messageQueue.dequeue();

	PEGASUS_ASSERT(request != 0);

	// notify the service that the request has been accepted
	service->_threadSemaphore.signal();

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
		Pair<String, String> pair = service->_lookupProviderForClass(classReference);

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
	service->_enqueueResponse((Message *)request, (Message *)response);
	
	delete request;
		
	return(0);
}

PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL ProviderManagerService::handleCreateInstanceRequest(void * arg) throw()
{
	// get the service from argument
	ProviderManagerService * service = reinterpret_cast<ProviderManagerService *>(arg);

	PEGASUS_ASSERT(service != 0);

	// get message from service queue
	const CIMCreateInstanceRequestMessage * request =
		(const CIMCreateInstanceRequestMessage *)service->messageQueue.dequeue();

	PEGASUS_ASSERT(request != 0);

	// notify the service that the request has been accepted
	service->_threadSemaphore.signal();

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
	   Pair<String, String> pair = service->_lookupProviderForClass(classReference);

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
	service->_enqueueResponse((Message *)request, (Message *)response);
	
	delete request;
		
	return(0);
}

PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL ProviderManagerService::handleModifyInstanceRequest(void * arg) throw()
{
	// get the service from argument
	ProviderManagerService * service = reinterpret_cast<ProviderManagerService *>(arg);

	PEGASUS_ASSERT(service != 0);

	// get message from service queue
	const CIMModifyInstanceRequestMessage * request =
		(const CIMModifyInstanceRequestMessage *)service->messageQueue.dequeue();

	PEGASUS_ASSERT(request != 0);

	// notify the service that the request has been accepted
	service->_threadSemaphore.signal();

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
	   Pair<String, String> pair = service->_lookupProviderForClass(classReference);

	   // get cached or load new provider module
	   Provider provider = providerManager.getProvider(pair.first, pair.second);

	   // convert arguments
	   OperationContext context;

	   // ATTN: convert flags to bitmask
	   Uint32 flags = OperationFlag::convert(false);
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
	service->_enqueueResponse((Message *)request, (Message *)response);
	
	delete request;
		
	return(0);
}

PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL ProviderManagerService::handleDeleteInstanceRequest(void * arg) throw()
{
	// get the service from argument
	ProviderManagerService * service = reinterpret_cast<ProviderManagerService *>(arg);

	PEGASUS_ASSERT(service != 0);

	// get message from service queue
	const CIMDeleteInstanceRequestMessage * request =
		(const CIMDeleteInstanceRequestMessage *)service->messageQueue.dequeue();

	PEGASUS_ASSERT(request != 0);

	// notify the service that the request has been accepted
	service->_threadSemaphore.signal();

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
	   Pair<String, String> pair = service->_lookupProviderForClass(classReference);

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
	service->_enqueueResponse((Message *)request, (Message *)response);
	
	delete request;
		
	return(0);
}

PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL ProviderManagerService::handleAssociatorsRequest(void * arg) throw()
{
	// get the service from argument
	ProviderManagerService * service = reinterpret_cast<ProviderManagerService *>(arg);

	PEGASUS_ASSERT(service != 0);

	// get message from service queue
	const CIMAssociatorsRequestMessage * request =
		(const CIMAssociatorsRequestMessage *)service->messageQueue.dequeue();

	PEGASUS_ASSERT(request != 0);

	// notify the service that the request has been accepted
	service->_threadSemaphore.signal();

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
	service->_enqueueResponse((Message *)request, (Message *)response);
	
	delete request;
		
	return(0);
}

PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL ProviderManagerService::handleExecuteQueryRequest(void * arg) throw()
{
	// get the service from argument
	ProviderManagerService * service = reinterpret_cast<ProviderManagerService *>(arg);

	PEGASUS_ASSERT(service != 0);

	// get message from service queue
	const CIMExecQueryRequestMessage * request =
		(const CIMExecQueryRequestMessage *)service->messageQueue.dequeue();

	PEGASUS_ASSERT(request != 0);

	// notify the service that the request has been accepted
	service->_threadSemaphore.signal();
	
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
	service->_enqueueResponse((Message *)request, (Message *)response);
	
	delete request;
		
	return(0);
}

PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL ProviderManagerService::handleAssociatorNamesRequest(void * arg) throw()
{
	// get the service from argument
	ProviderManagerService * service = reinterpret_cast<ProviderManagerService *>(arg);

	PEGASUS_ASSERT(service != 0);

	// get message from service queue
	const CIMAssociatorNamesRequestMessage * request =
		(const CIMAssociatorNamesRequestMessage *)service->messageQueue.dequeue();

	PEGASUS_ASSERT(request != 0);

	// notify the service that the request has been accepted
	service->_threadSemaphore.signal();
	
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
	service->_enqueueResponse((Message *)request, (Message *)response);
	
	delete request;
		
	return(0);
}

PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL ProviderManagerService::handleReferencesRequest(void * arg) throw()
{
	// get the service from argument
	ProviderManagerService * service = reinterpret_cast<ProviderManagerService *>(arg);

	PEGASUS_ASSERT(service != 0);

	// get message from service queue
	const CIMReferencesRequestMessage * request =
		(const CIMReferencesRequestMessage *)service->messageQueue.dequeue();

	PEGASUS_ASSERT(request != 0);

	// notify the service that the request has been accepted
	service->_threadSemaphore.signal();
	
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
	service->_enqueueResponse((Message *)request, (Message *)response);
	
	delete request;
		
	return(0);
}

PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL ProviderManagerService::handleReferenceNamesRequest(void * arg) throw()
{
	// get the service from argument
	ProviderManagerService * service = reinterpret_cast<ProviderManagerService *>(arg);

	PEGASUS_ASSERT(service != 0);

	// get message from service queue
	const CIMReferenceNamesRequestMessage * request =
		(const CIMReferenceNamesRequestMessage *)service->messageQueue.dequeue();

	PEGASUS_ASSERT(request != 0);

	// notify the service that the request has been accepted
	service->_threadSemaphore.signal();
	
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
	service->_enqueueResponse((Message *)request, (Message *)response);
	
	delete request;
		
	return(0);
}

PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL ProviderManagerService::handleGetPropertyRequest(void * arg) throw()
{
	// get the service from argument
	ProviderManagerService * service = reinterpret_cast<ProviderManagerService *>(arg);

	PEGASUS_ASSERT(service != 0);

	// get message from service queue
	const CIMGetPropertyRequestMessage * request =
		(const CIMGetPropertyRequestMessage *)service->messageQueue.dequeue();

	PEGASUS_ASSERT(request != 0);

	// notify the service that the request has been accepted
	service->_threadSemaphore.signal();

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
	service->_enqueueResponse((Message *)request, (Message *)response);
	
	delete request;
		
	return(0);
}

PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL ProviderManagerService::handleSetPropertyRequest(void * arg) throw()
{
	// get the service from argument
	ProviderManagerService * service = reinterpret_cast<ProviderManagerService *>(arg);

	PEGASUS_ASSERT(service != 0);

	// get message from service queue
	const CIMSetPropertyRequestMessage * request =
		(const CIMSetPropertyRequestMessage *)service->messageQueue.dequeue();

	PEGASUS_ASSERT(request != 0);

	// notify the service that the request has been accepted
	service->_threadSemaphore.signal();

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
	service->_enqueueResponse((Message *)request, (Message *)response);
	
	delete request;
		
	return(0);
}

PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL ProviderManagerService::handleInvokeMethodRequest(void * arg) throw()
{
	// get the service from argument
	ProviderManagerService * service = reinterpret_cast<ProviderManagerService *>(arg);

	PEGASUS_ASSERT(service != 0);

	// get message from service queue
	const CIMInvokeMethodRequestMessage * request =
		(const CIMInvokeMethodRequestMessage *)service->messageQueue.dequeue();

	PEGASUS_ASSERT(request != 0);

	// notify the service that the request has been accepted
	service->_threadSemaphore.signal();

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
		Pair<String, String> pair = service->_lookupProviderForClass(classReference);

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
	service->_enqueueResponse((Message *)request, (Message *)response);
	
	delete request;
		
	return(0);
}

PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL ProviderManagerService::handleEnableIndicationRequest(void * arg) throw()
{
	// get the service from argument
	ProviderManagerService * service = reinterpret_cast<ProviderManagerService *>(arg);

	PEGASUS_ASSERT(service != 0);

	// get message from service queue
	const CIMEnableIndicationSubscriptionRequestMessage * request =
		(const CIMEnableIndicationSubscriptionRequestMessage *)service->messageQueue.dequeue();

	PEGASUS_ASSERT(request != 0);

	// notify the service that the request has been accepted
	service->_threadSemaphore.signal();

	Status status;

	try
	{
		// make class reference
		CIMReference classReference(
			"",
			request->nameSpace,
			request->classNames[0]);

		// get the provider file name and logical name
		Pair<String, String> pair = service->_lookupProviderForClass(classReference);

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
	service->_enqueueResponse((Message *)request, (Message *)response);
	
	delete request;
		
	return(0);
}

PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL ProviderManagerService::handleModifyIndicationRequest(void * arg) throw()
{
	// get the service from argument
	ProviderManagerService * service = reinterpret_cast<ProviderManagerService *>(arg);

	PEGASUS_ASSERT(service != 0);

	// get message from service queue
	const CIMModifyIndicationSubscriptionRequestMessage * request =
		(const CIMModifyIndicationSubscriptionRequestMessage *)service->messageQueue.dequeue();

	PEGASUS_ASSERT(request != 0);

	// notify the service that the request has been accepted
	service->_threadSemaphore.signal();

	CIMModifyIndicationSubscriptionResponseMessage * response =
		new CIMModifyIndicationSubscriptionResponseMessage(
			request->messageId,
			CIM_ERR_FAILED,
			"not implemented",
			request->queueIds.copyAndPop());

	// preserve message key
	response->setKey(request->getKey());

	// call the message queue service method to see if this is an async envelope
	service->_enqueueResponse((Message *)request, (Message *)response);
	
	delete request;
		
	return(0);
}

PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL ProviderManagerService::handleDisableIndicationRequest(void * arg) throw()
{
	// get the service from argument
	ProviderManagerService * service = reinterpret_cast<ProviderManagerService *>(arg);

	PEGASUS_ASSERT(service != 0);

	// get message from service queue
	const CIMDisableIndicationSubscriptionRequestMessage * request =
		(const CIMDisableIndicationSubscriptionRequestMessage *)service->messageQueue.dequeue();

	PEGASUS_ASSERT(request != 0);

	// notify the service that the request has been accepted
	service->_threadSemaphore.signal();

	CIMDisableIndicationSubscriptionResponseMessage * response =
		new CIMDisableIndicationSubscriptionResponseMessage(
			request->messageId,
			CIM_ERR_FAILED,
			"not implemented",
			request->queueIds.copyAndPop());

	// preserve message key
	response->setKey(request->getKey());

	// call the message queue service method to see if this is an async envelope
	service->_enqueueResponse((Message *)request, (Message *)response);
	
	delete request;
		
	return(0);
}

PEGASUS_NAMESPACE_END
