///%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 BMC Software, Hewlett-Packard Company, IBM,
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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By:  Nag Boranna, Hewlett-Packard Company (nagaraja_boranna@hp.com)
//               Chip Vincent (cvincent@us.ibm.com)
//               Yi Zhou, Hewlett-Packard Company (yi_zhou@hp.com)
//               Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
//               Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//               Mike Day (mdday@us.ibm.com)
//               Carol Ann Krug Graves, Hewlett-Packard Company
//                   (carolann_graves@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include "CIMOperationRequestDispatcher.h"


PEGASUS_NAMESPACE_BEGIN

PEGASUS_USING_STD;

#define DDD(X) // X

DDD(static const char* _DISPATCHER = "CIMOperationRequestDispatcher::";)

CIMOperationRequestDispatcher::CIMOperationRequestDispatcher(
	CIMRepository* repository,
	CIMServer* server)
   :
   Base("CIMOpRequestDispatcher", MessageQueue::getNextQueueId()),
   _repository(repository),
   _cimom(this, server, repository),
   _configurationManager(_cimom),
   _indicationService(_cimom)
{
   DDD(cout << _DISPATCHER << endl;)

   _indicationService.initialize(_cimom);
}

CIMOperationRequestDispatcher::~CIMOperationRequestDispatcher(void)	
{
	_dying = 1;
}

void CIMOperationRequestDispatcher::_handle_async_request(AsyncRequest *req)
{
   if ( req->getType() == async_messages::ASYNC_LEGACY_OP_START )
   {
      req->op->processing();
   }

   Base::_handle_async_request(req);
}

// ATTN: this needs to return an array of names if it is possible
// to have more than one provider per class.
String CIMOperationRequestDispatcher::_lookupProviderForClass(
	const String& nameSpace,
	const String& className)
{
	MessageQueue * queue = MessageQueue::lookup("Server::ConfigurationManagerQueue");

	PEGASUS_ASSERT(queue != 0);

	Uint32 targetQueueId = queue->getQueueId();
	Uint32 sourceQueueId = this->getQueueId();
	
	// get all CIM_ProviderElementCapabilities instances
	Array<CIMInstance> providerElementCapabilitiesInstances;
	
	{
		// create request
		CIMRequestMessage * request = new CIMEnumerateInstancesRequestMessage(
			"golden snitch",
			nameSpace,
			"CIM_ProviderElementCapabilities",
			false,
			false,
			false,
			false,
			Array<String>(),
			QueueIdStack(targetQueueId, sourceQueueId));

		// save the message key because the lifetime of the message is not known.
		Uint32 messageKey = request->getKey();

		//	<< Tue Feb 12 08:29:38 2002 mdd >> example of conversion to meta dispatcher
		// automatically initializes backpointer
		AsyncLegacyOperationStart * async_req = new AsyncLegacyOperationStart(
			get_next_xid(),
			0,
			targetQueueId,
			request,
			sourceQueueId);

		// send request and wait for response
		AsyncReply * async_reply = SendWait(async_req);

		CIMEnumerateInstancesResponseMessage * response =
			reinterpret_cast<CIMEnumerateInstancesResponseMessage *>
				((static_cast<AsyncLegacyOperationResult *>(async_reply))->res);

		delete async_req;
		delete async_reply;

		// ATTN: temporary fix until CIMNamedInstance is removed
		for(Uint32 i = 0, n = response->cimNamedInstances.size(); i < n; i++)
		{
			providerElementCapabilitiesInstances.append(response->cimNamedInstances[i].getInstance());
		}
	}
	
	for(Uint32 i = 0, n = providerElementCapabilitiesInstances.size(); i < n; i++)
	{
		// get the associated CIM_ProviderCapabilities instance
		CIMInstance providerCapabilitiesInstance;
	
		{
			// the object path of the associated instance is in the 'Capabilities' property
			Uint32 pos = providerElementCapabilitiesInstances[i].findProperty("Capabilities");
			
			PEGASUS_ASSERT(pos != PEG_NOT_FOUND);
			
			CIMReference cimReference = providerElementCapabilitiesInstances[i].getProperty(pos).getValue().toString();
			
			//PEGASUS_STD(cout) << cimReference << PEGASUS_STD(endl);

			// create request
			CIMRequestMessage * request = new CIMGetInstanceRequestMessage(
				"golden snitch",
				nameSpace,
				cimReference,
				false,
				false,
				false,
				Array<String>(),
				QueueIdStack(targetQueueId, sourceQueueId));

			// save the message key because the lifetime of the message is not known.
			Uint32 messageKey = request->getKey();

			//	<< Tue Feb 12 08:29:38 2002 mdd >> example of conversion to meta dispatcher
			// automatically initializes backpointer
			AsyncLegacyOperationStart * async_req = new AsyncLegacyOperationStart(
				get_next_xid(),
				0,
				targetQueueId,
				request,
				sourceQueueId);

			// send request and wait for response
			AsyncReply * async_reply = SendWait(async_req);

			CIMGetInstanceResponseMessage * response =
				reinterpret_cast<CIMGetInstanceResponseMessage *>
					((static_cast<AsyncLegacyOperationResult *>(async_reply))->res);

			delete async_req;
			delete async_reply;

			providerCapabilitiesInstance = response->cimInstance;
		}

		try
		{
			// get the ClassName property value from the instance
			Uint32 pos = providerCapabilitiesInstance.findProperty("ClassName");

			PEGASUS_ASSERT(pos != PEG_NOT_FOUND);

			// compare the property value with the requested class name
			if(!String::equalNoCase(className, providerCapabilitiesInstance.getProperty(pos).getValue().toString()))
			{
				// go to the next CIM_ProviderCapabilities instance
				continue;
			}
		}
		catch(...)
		{
			// instance or property error, use different technique
			break;
		}
			
		// get the associated CIM_Provider instance
		CIMInstance providerInstance;

		{
			// the object path of the associated instance is in the 'ManagedElement' property
			Uint32 pos = providerElementCapabilitiesInstances[i].findProperty("ManagedElement");
			
			PEGASUS_ASSERT(pos != PEG_NOT_FOUND);
			
			CIMReference cimReference = providerElementCapabilitiesInstances[i].getProperty(pos).getValue().toString();
			
			//PEGASUS_STD(cout) << cimReference << PEGASUS_STD(endl);
			
			// create request
			CIMRequestMessage * request = new CIMGetInstanceRequestMessage(
				"golden snitch",
				nameSpace,
				cimReference,
				false,
				false,
				false,
				Array<String>(),
				QueueIdStack(targetQueueId, sourceQueueId));

			// save the message key because the lifetime of the message is not known.
			Uint32 messageKey = request->getKey();

			//	<< Tue Feb 12 08:29:38 2002 mdd >> example of conversion to meta dispatcher
			// automatically initializes backpointer
			AsyncLegacyOperationStart * async_req = new AsyncLegacyOperationStart(
				get_next_xid(),
				0,
				targetQueueId,
				request,
				sourceQueueId);

			// send request and wait for response
			AsyncReply * async_reply = SendWait(async_req);

			CIMGetInstanceResponseMessage * response =
				reinterpret_cast<CIMGetInstanceResponseMessage *>
					((static_cast<AsyncLegacyOperationResult *>(async_reply))->res);

			delete async_req;
			delete async_reply;

			providerInstance = response->cimInstance;
		}
		
		// extract provider information
		String providerName = providerInstance.getProperty(providerInstance.findProperty("Name")).getValue().toString();
		String providerLocation = providerInstance.getProperty(providerInstance.findProperty("Location")).getValue().toString();
				
		if((providerName.size() != 0) && (providerLocation.size() != 0))
		{
			return(providerName);
		}

		// provider information error, use different technique
		break;
	}
	
	return(String::EMPTY);
	/*
	// ATTN: still use qualifier to find provider if a provider did not use
	// PG_RegistrationProvider to register. Will remove in the future

	CIMClass cimClass;

	try
	{
		_repository->read_lock();

		cimClass = _repository->getClass(nameSpace, className);

		_repository->read_unlock();
	}
	catch(CIMException& e)
	{
		_repository->read_unlock();

		return(String::EMPTY);
	}

	//----------------------------------------------------------------------
	// Get the provider qualifier:
	//----------------------------------------------------------------------

	Uint32 pos = cimClass.findQualifier("provider");

	if(pos == PEG_NOT_FOUND)
	{
		return(String::EMPTY);
	}

	String providerId;
	
	cimClass.getQualifier(pos).getValue().get(providerId);

	return(providerId);
	*/
}

/*
Message * CIMOperationRequestDispatcher::_waitForResponse(
	const Uint32 messageType,
	const Uint32 messageKey,
	const Uint32 timeout)
{
	// immediately attempt to locate a message of the requested type
	Message * message = ((MessageQueue *)this)->find(messageType, messageKey);

	// if the message is null and the timeout is greater than 0, go into
	// a sleep/retry mode until the timeout expires or a message of the
	// requested type arrives. a timeout value of 0xffffffff represents
	// infinity.
	for(Uint32 i = 0; ((i < timeout) || (timeout == 0xffffffff)) && (message == 0); i++)
	{
		System::sleep(1);

		message = ((MessageQueue *)this)->find(messageType, messageKey);
	}

	if(message == 0)
	{
		throw CIMException(CIM_ERR_FAILED, __FILE__, __LINE__, "queue underflow");
	}

	((MessageQueue *)this)->remove(message);

	return(message);
}
*/


void CIMOperationRequestDispatcher::_enqueueResponse(
	CIMRequestMessage* request,
	CIMResponseMessage* response)
{
	// Use the same key as used in the request:

	response->setKey(request->getKey());

	if( true == Base::_enqueueResponse(request, response))
	   return;
	
	// Lookup the message queue:

	MessageQueue* queue = MessageQueue::lookup(request->queueIds.top());
	PEGASUS_ASSERT(queue != 0);

	// Enqueue the response:

	queue->enqueue(response);
}

// allocate a CIM Operation_async,  opnode, context, and response handler
// initialize with pointers to async top and async bottom
// link to the waiting q
void CIMOperationRequestDispatcher::handleEnqueue()
{
	Message* request = dequeue();

	if(!request)
		return;
   	
	switch(request->getType())
	{
	
	case CIM_GET_CLASS_REQUEST_MESSAGE:
		handleGetClassRequest((CIMGetClassRequestMessage*)request);
		break;

	case CIM_GET_INSTANCE_REQUEST_MESSAGE:
		handleGetInstanceRequest((CIMGetInstanceRequestMessage*)request);
		break;

	case CIM_DELETE_CLASS_REQUEST_MESSAGE:
		handleDeleteClassRequest(
			(CIMDeleteClassRequestMessage*)request);
		break;

	case CIM_DELETE_INSTANCE_REQUEST_MESSAGE:
		handleDeleteInstanceRequest(
			(CIMDeleteInstanceRequestMessage*)request);
		break;

	case CIM_CREATE_CLASS_REQUEST_MESSAGE:
		handleCreateClassRequest((CIMCreateClassRequestMessage*)request);
		break;

	case CIM_CREATE_INSTANCE_REQUEST_MESSAGE:
		handleCreateInstanceRequest(
			(CIMCreateInstanceRequestMessage*)request);
		break;

	case CIM_MODIFY_CLASS_REQUEST_MESSAGE:
		handleModifyClassRequest((CIMModifyClassRequestMessage*)request);
		break;

	case CIM_MODIFY_INSTANCE_REQUEST_MESSAGE:
		handleModifyInstanceRequest(
			(CIMModifyInstanceRequestMessage*)request);
		break;

	case CIM_ENUMERATE_CLASSES_REQUEST_MESSAGE:
		handleEnumerateClassesRequest(
			(CIMEnumerateClassesRequestMessage*)request);
		break;

	case CIM_ENUMERATE_CLASS_NAMES_REQUEST_MESSAGE:
		handleEnumerateClassNamesRequest(
			(CIMEnumerateClassNamesRequestMessage*)request);
		break;

	case CIM_ENUMERATE_INSTANCES_REQUEST_MESSAGE:
		handleEnumerateInstancesRequest(
			(CIMEnumerateInstancesRequestMessage*)request);
		break;

	case CIM_ENUMERATE_INSTANCE_NAMES_REQUEST_MESSAGE:
		handleEnumerateInstanceNamesRequest(
			(CIMEnumerateInstanceNamesRequestMessage*)request);
		break;

		// ATTN: implement this!
	case CIM_EXEC_QUERY_REQUEST_MESSAGE:
		break;

	case CIM_ASSOCIATORS_REQUEST_MESSAGE:
		handleAssociatorsRequest((CIMAssociatorsRequestMessage*)request);
		break;

	case CIM_ASSOCIATOR_NAMES_REQUEST_MESSAGE:
		handleAssociatorNamesRequest(
			(CIMAssociatorNamesRequestMessage*)request);
		break;

	case CIM_REFERENCES_REQUEST_MESSAGE:
		handleReferencesRequest((CIMReferencesRequestMessage*)request);
		break;

	case CIM_REFERENCE_NAMES_REQUEST_MESSAGE:
		handleReferenceNamesRequest(
			(CIMReferenceNamesRequestMessage*)request);
		break;

	case CIM_GET_PROPERTY_REQUEST_MESSAGE:
		handleGetPropertyRequest(
			(CIMGetPropertyRequestMessage*)request);
		break;

	case CIM_SET_PROPERTY_REQUEST_MESSAGE:
		handleSetPropertyRequest(
			(CIMSetPropertyRequestMessage*)request);
		break;

	case CIM_GET_QUALIFIER_REQUEST_MESSAGE:
		handleGetQualifierRequest((CIMGetQualifierRequestMessage*)request);
		break;

	case CIM_SET_QUALIFIER_REQUEST_MESSAGE:
		handleSetQualifierRequest((CIMSetQualifierRequestMessage*)request);
		break;

	case CIM_DELETE_QUALIFIER_REQUEST_MESSAGE:
		handleDeleteQualifierRequest(
			(CIMDeleteQualifierRequestMessage*)request);
		break;

	case CIM_ENUMERATE_QUALIFIERS_REQUEST_MESSAGE:
		handleEnumerateQualifiersRequest(
			(CIMEnumerateQualifiersRequestMessage*)request);
		break;

	case CIM_INVOKE_METHOD_REQUEST_MESSAGE:
		handleInvokeMethodRequest(
			(CIMInvokeMethodRequestMessage*)request);
		break;

	case CIM_ENABLE_INDICATION_SUBSCRIPTION_REQUEST_MESSAGE:
		handleEnableIndicationSubscriptionRequest(
			(CIMEnableIndicationSubscriptionRequestMessage*)request);
		break;

	case CIM_MODIFY_INDICATION_SUBSCRIPTION_REQUEST_MESSAGE:
		handleModifyIndicationSubscriptionRequest(
			(CIMModifyIndicationSubscriptionRequestMessage*)request);
		break;

	case CIM_DISABLE_INDICATION_SUBSCRIPTION_REQUEST_MESSAGE:
		handleDisableIndicationSubscriptionRequest(
			(CIMDisableIndicationSubscriptionRequestMessage*)request);
		break;

	case CIM_PROCESS_INDICATION_REQUEST_MESSAGE:
		handleProcessIndicationRequest(
				(CIMProcessIndicationRequestMessage*)request);
			break;
	}

	delete request;
}

const char* CIMOperationRequestDispatcher::getQueueName() const
{
	return(_name);
}

void CIMOperationRequestDispatcher::handleGetClassRequest(
	CIMGetClassRequestMessage* request)
{
	// ATTN: Need code here to expand partial class!

	CIMStatusCode errorCode = CIM_ERR_SUCCESS;
	String errorDescription;
	CIMClass cimClass;

	_repository->read_lock();

	try
	{
		cimClass = _repository->getClass(
			request->nameSpace,
			request->className,
			request->localOnly,
			request->includeQualifiers,
			request->includeClassOrigin,
			request->propertyList.getPropertyNameArray());
	}
	catch(CIMException& exception)
	{
		errorCode = exception.getCode();
		errorDescription = exception.getMessage();
	}
	catch(Exception& exception)
	{
		errorCode = CIM_ERR_FAILED;
		errorDescription = exception.getMessage();
	}
	catch(...)
	{
		errorCode = CIM_ERR_FAILED;
	}

	_repository->read_unlock();

	CIMGetClassResponseMessage* response = new CIMGetClassResponseMessage(
		request->messageId,
		errorCode,
		errorDescription,
		request->queueIds.copyAndPop(),
		cimClass);

	_enqueueResponse(request, response);
}

void CIMOperationRequestDispatcher::handleGetInstanceRequest(
	CIMGetInstanceRequestMessage* request)
{
	// ATTN: Need code here to expand partial instance!

	// get the class name
	String className = request->instanceName.getClassName();

	// check the class name for an "internal provider"
	if(String::equalNoCase(className, "CIM_Provider") ||
	   String::equalNoCase(className, "CIM_ProviderCapabilities") ||
	   String::equalNoCase(className, "CIM_ProviderElementCapabilities"))
	{
	    // send to the configuration manager. it will generate the
	    // appropriate response message.
	    _configurationManager.enqueue(new CIMGetInstanceRequestMessage(*request));

	    return;
	}

	//
	// check the class name for subscription, filter and handler
	//
	if(String::equalNoCase(className, "CIM_IndicationSubscription") ||
	    String::equalNoCase(className, "CIM_IndicationHandlerCIMXML") ||
	    String::equalNoCase(className, "CIM_IndicationHandlerSNMP") ||
	    String::equalNoCase(className, "CIM_IndicationFilter"))
	{
	    //
	    // Send to the indication service. It will generate the
	    // appropriate response message.
	    //
	    _indicationService.enqueue(new CIMGetInstanceRequestMessage(*request));

	    return;
	}

	// get provider for class
	String providerName = _lookupProviderForClass(request->nameSpace, className);

	if(providerName.size() != 0)
	{
		// lookup provider manager
		MessageQueue * queue = MessageQueue::lookup("Server::ProviderManagerService");

		PEGASUS_ASSERT(queue != 0);

		// forward to provider manager. make a copy becuase the original request is
		// deleted by this service.
		queue->enqueue(new CIMGetInstanceRequestMessage(*request));

		return;
	}

	CIMStatusCode errorCode = CIM_ERR_SUCCESS;
	String errorDescription;
	CIMInstance cimInstance;

	_repository->read_lock();

	try
	{
		cimInstance = _repository->getInstance(
			request->nameSpace,
			request->instanceName,
			request->localOnly,
			request->includeQualifiers,
			request->includeClassOrigin,
			request->propertyList.getPropertyNameArray());
	}
	catch(CIMException& exception)
	{
		errorCode = exception.getCode();
		errorDescription = exception.getMessage();
	}
	catch(Exception& exception)
	{
		errorCode = CIM_ERR_FAILED;
		errorDescription = exception.getMessage();
	}
	catch(...)
	{
		errorCode = CIM_ERR_FAILED;
	}
	
	_repository->read_unlock();

	CIMGetInstanceResponseMessage* response = new CIMGetInstanceResponseMessage(
		request->messageId,
		errorCode,
		errorDescription,
		request->queueIds.copyAndPop(),
		cimInstance);

	_enqueueResponse(request, response);
}

void CIMOperationRequestDispatcher::handleDeleteClassRequest(
	CIMDeleteClassRequestMessage* request)
{
	CIMStatusCode errorCode = CIM_ERR_SUCCESS;
	String errorDescription;

	_repository->write_lock();

	try
	{
		_repository->deleteClass(
			request->nameSpace,
			request->className);
	}

	catch(CIMException& exception)
	{
		errorCode = exception.getCode();
		errorDescription = exception.getMessage();
	}
	catch(Exception& exception)
	{
		errorCode = CIM_ERR_FAILED;
		errorDescription = exception.getMessage();
	}
	catch(...)
	{
		errorCode = CIM_ERR_FAILED;
	}

	_repository->write_unlock();

	CIMDeleteClassResponseMessage* response =
		new CIMDeleteClassResponseMessage(
		request->messageId,
		errorCode,
		errorDescription,
		request->queueIds.copyAndPop());

	_enqueueResponse(request, response);
}

void CIMOperationRequestDispatcher::handleDeleteInstanceRequest(
	CIMDeleteInstanceRequestMessage* request)
{
	// get the class name
	String className = request->instanceName.getClassName();

	// check the class name for an "internal provider"
	if(String::equalNoCase(className, "CIM_Provider") ||
	   String::equalNoCase(className, "CIM_ProviderCapabilities") ||
	   String::equalNoCase(className, "CIM_ProviderElementCapabilities"))
	{
		// send to the configuration manager. it will generate the
		// appropriate response message.
		_configurationManager.enqueue(new CIMDeleteInstanceRequestMessage(*request));

		return;
	}

	//
	// check the class name for subscription, filter and handler
	//
	if(String::equalNoCase(className, "CIM_IndicationSubscription") ||
	    String::equalNoCase(className, "CIM_IndicationHandlerCIMXML") ||
	    String::equalNoCase(className, "CIM_IndicationHandlerSNMP") ||
	    String::equalNoCase(className, "CIM_IndicationFilter"))
	{
	    //
	    // Send to the indication service. It will generate the
	    // appropriate response message.
	    //
	    _indicationService.enqueue(new CIMDeleteInstanceRequestMessage(*request));

	    return;
	}

	String providerName = _lookupProviderForClass(request->nameSpace, className);

	if(providerName.size() != 0)
	{
		// lookup provider manager
		MessageQueue * queue = MessageQueue::lookup("Server::ProviderManagerService");

		PEGASUS_ASSERT(queue != 0);

		// forward to provider manager. make a copy becuase the original request is
		// deleted by this service.
		queue->enqueue(new CIMDeleteInstanceRequestMessage(*request));
	
		return;
	}

	CIMStatusCode errorCode = CIM_ERR_SUCCESS;
	String errorDescription;

	_repository->write_lock();

	try
	{
		_repository->deleteInstance(
			request->nameSpace,
			request->instanceName);
	}
	catch(CIMException& exception)
	{
		errorCode = exception.getCode();
		errorDescription = exception.getMessage();
	}
	catch(Exception& exception)
	{
		errorCode = CIM_ERR_FAILED;
		errorDescription = exception.getMessage();
	}
	catch(...)
	{
		errorCode = CIM_ERR_FAILED;
	}

	_repository->write_unlock();

	CIMDeleteInstanceResponseMessage* response =
		new CIMDeleteInstanceResponseMessage(
		request->messageId,
		errorCode,
		errorDescription,
		request->queueIds.copyAndPop());

	_enqueueResponse(request, response);
}

void CIMOperationRequestDispatcher::handleCreateClassRequest(
	CIMCreateClassRequestMessage* request)
{
	CIMStatusCode errorCode = CIM_ERR_SUCCESS;
	String errorDescription;

	_repository->write_lock();

	try
	{
		_repository->createClass(
			request->nameSpace,
			request->newClass);
	}

	catch(CIMException& exception)
	{
		errorCode = exception.getCode();
		errorDescription = exception.getMessage();
	}
	catch(Exception& exception)
	{
		errorCode = CIM_ERR_FAILED;
		errorDescription = exception.getMessage();
	}
	catch(...)
	{
		errorCode = CIM_ERR_FAILED;
	}

	_repository->write_unlock();

	CIMCreateClassResponseMessage* response =
		new CIMCreateClassResponseMessage(
		request->messageId,
		errorCode,
		errorDescription,
		request->queueIds.copyAndPop());

	_enqueueResponse(request, response);
}

void CIMOperationRequestDispatcher::handleCreateInstanceRequest(
	CIMCreateInstanceRequestMessage* request)
{
	// get the class name
	String className = request->newInstance.getClassName();

	// check the class name for an "internal provider"
	if(String::equalNoCase(className, "CIM_Provider") ||
	   String::equalNoCase(className, "CIM_ProviderCapabilities") ||
	   String::equalNoCase(className, "CIM_ProviderElementCapabilities"))
	{
	    // send to the configuration manager. it will generate the
	    // appropriate response message.
	    _configurationManager.enqueue(new CIMCreateInstanceRequestMessage(*request));

	    return;
	}

	//
	// check the class name for subscription, filter and handler
	//

	if(String::equalNoCase(className, "CIM_IndicationSubscription") ||
	    String::equalNoCase(className, "CIM_IndicationHandlerCIMXML") ||
	    String::equalNoCase(className, "CIM_IndicationHandlerSNMP") ||
	    String::equalNoCase(className, "CIM_IndicationFilter"))
	{
	    //
	    // Send to the indication service. It will generate the
	    // appropriate response message.
	    //
	    _indicationService.enqueue(new CIMCreateInstanceRequestMessage(*request));

	    return;
	}

	String providerName = _lookupProviderForClass(request->nameSpace, className);

	if(providerName.size() != 0)
	{
		// lookup provider manager
		MessageQueue * queue = MessageQueue::lookup("Server::ProviderManagerService");

		PEGASUS_ASSERT(queue != 0);

		// forward to provider manager. make a copy becuase the original request is
		// deleted by this service.
		queue->enqueue(new CIMCreateInstanceRequestMessage(*request));
	
		return;
	}

	CIMStatusCode errorCode = CIM_ERR_SUCCESS;
	String errorDescription;
	CIMReference instanceName;

	_repository->write_lock();

	try
	{
		instanceName = _repository->createInstance(
			request->nameSpace,
			request->newInstance);
	}
	catch(CIMException& exception)
	{
		errorCode = exception.getCode();
		errorDescription = exception.getMessage();
	}
	catch(Exception& exception)
	{
		errorCode = CIM_ERR_FAILED;
		errorDescription = exception.getMessage();
	}
	catch(...)
	{
		errorCode = CIM_ERR_FAILED;
	}

	_repository->write_unlock();

	CIMCreateInstanceResponseMessage* response =
		new CIMCreateInstanceResponseMessage(
		request->messageId,
		errorCode,
		errorDescription,
		request->queueIds.copyAndPop(),
		instanceName);

	_enqueueResponse(request, response);
}

void CIMOperationRequestDispatcher::handleModifyClassRequest(
	CIMModifyClassRequestMessage* request)
{
	CIMStatusCode errorCode = CIM_ERR_SUCCESS;
	String errorDescription;

	_repository->write_lock();

	try
	{
		_repository->modifyClass(
			request->nameSpace,
			request->modifiedClass);
	}

	catch(CIMException& exception)
	{
		errorCode = exception.getCode();
		errorDescription = exception.getMessage();
	}
	catch(Exception& exception)
	{
		errorCode = CIM_ERR_FAILED;
		errorDescription = exception.getMessage();
	}
	catch(...)
	{
		errorCode = CIM_ERR_FAILED;
	}

	_repository->write_unlock();

	CIMModifyClassResponseMessage* response =
		new CIMModifyClassResponseMessage(
		request->messageId,
		errorCode,
		errorDescription,
		request->queueIds.copyAndPop());

	_enqueueResponse(request, response);
}

void CIMOperationRequestDispatcher::handleModifyInstanceRequest(
	CIMModifyInstanceRequestMessage* request)
{
	// ATTN: Who makes sure the instance name and the instance match?

	// get the class name
	String className = request->modifiedInstance.getInstance().getClassName();

	// check the class name for an "internal provider"
	if(String::equalNoCase(className, "CIM_Provider") ||
	   String::equalNoCase(className, "CIM_ProviderCapabilities") ||
	   String::equalNoCase(className, "CIM_ProviderElementCapabilities"))
	{
		// send to the configuration manager. it will generate the
		// appropriate response message.
		_configurationManager.enqueue(new CIMModifyInstanceRequestMessage(*request));

		return;
	}

	//
	// check the class name for subscription, filter and handler
	//
	if(String::equalNoCase(className, "CIM_IndicationSubscription") ||
	    String::equalNoCase(className, "CIM_IndicationHandlerCIMXML") ||
	    String::equalNoCase(className, "CIM_IndicationHandlerSNMP") ||
	    String::equalNoCase(className, "CIM_IndicationFilter"))
	{
	    //
	    // Send to the indication service. It will generate the
	    // appropriate response message.
	    //
	    _indicationService.enqueue(new CIMModifyInstanceRequestMessage(*request));

	    return;
	}

	// check the class name for an "external provider"
	String providerName = _lookupProviderForClass(request->nameSpace, className);

	if(providerName.size() != 0)
	{
		// lookup provider manager
		MessageQueue * queue = MessageQueue::lookup("Server::ProviderManagerService");

		PEGASUS_ASSERT(queue != 0);

		// forward to provider manager. make a copy becuase the original request is
		// deleted by this service.
		queue->enqueue(new CIMModifyInstanceRequestMessage(*request));

		return;
	}

	// translate and forward request to repository
	CIMStatusCode errorCode = CIM_ERR_SUCCESS;
	String errorDescription;

	_repository->write_lock();

	try
	{
		_repository->modifyInstance(
			request->nameSpace,
			request->modifiedInstance,
			request->includeQualifiers,request->propertyList);
	}
	catch(CIMException& exception)
	{
		errorCode = exception.getCode();
		errorDescription = exception.getMessage();
	}
	catch(Exception& exception)
	{
		errorCode = CIM_ERR_FAILED;
		errorDescription = exception.getMessage();
	}
	catch(...)
	{
		errorCode = CIM_ERR_FAILED;
	}

	_repository->write_unlock();

	CIMModifyInstanceResponseMessage* response =
		new CIMModifyInstanceResponseMessage(
		request->messageId,
		errorCode,
		errorDescription,
		request->queueIds.copyAndPop());

	_enqueueResponse(request, response);
}

void CIMOperationRequestDispatcher::handleEnumerateClassesRequest(
	CIMEnumerateClassesRequestMessage* request)
{
	CIMStatusCode errorCode = CIM_ERR_SUCCESS;
	String errorDescription;
	Array<CIMClass> cimClasses;

	_repository->read_lock();

	try
	{
		cimClasses = _repository->enumerateClasses(
			request->nameSpace,
			request->className,
			request->deepInheritance,
			request->localOnly,
			request->includeQualifiers,
			request->includeClassOrigin);
	}

	catch(CIMException& exception)
	{
		errorCode = exception.getCode();
		errorDescription = exception.getMessage();
	}
	catch(Exception& exception)
	{
		errorCode = CIM_ERR_FAILED;
		errorDescription = exception.getMessage();
	}
	catch(...)
	{
		errorCode = CIM_ERR_FAILED;
	}

	_repository->read_unlock();

	CIMEnumerateClassesResponseMessage* response =
		new CIMEnumerateClassesResponseMessage(
		request->messageId,
		errorCode,
		errorDescription,
		request->queueIds.copyAndPop(),
		cimClasses);

	_enqueueResponse(request, response);
}

void CIMOperationRequestDispatcher::handleEnumerateClassNamesRequest(
	CIMEnumerateClassNamesRequestMessage* request)
{
	CIMStatusCode errorCode = CIM_ERR_SUCCESS;
	String errorDescription;
	Array<String> classNames;

	_repository->read_lock();

	try
	{
		classNames = _repository->enumerateClassNames(
			request->nameSpace,
			request->className,
			request->deepInheritance);
	}

	catch(CIMException& exception)
	{
		errorCode = exception.getCode();
		errorDescription = exception.getMessage();
	}
	catch(Exception& exception)
	{
		errorCode = CIM_ERR_FAILED;
		errorDescription = exception.getMessage();
	}
	catch(...)
	{
		errorCode = CIM_ERR_FAILED;
	}

	_repository->read_unlock();

	CIMEnumerateClassNamesResponseMessage* response =
		new CIMEnumerateClassNamesResponseMessage(
		request->messageId,
		errorCode,
		errorDescription,
		request->queueIds.copyAndPop(),
		classNames);

	_enqueueResponse(request, response);
}

void CIMOperationRequestDispatcher::handleEnumerateInstancesRequest(
	CIMEnumerateInstancesRequestMessage* request)
{
	// get the class name
	String className = request->className;

	// check the class name for an "internal provider"
	if(String::equalNoCase(className, "CIM_Provider") ||
	   String::equalNoCase(className, "CIM_ProviderCapabilities") ||
	   String::equalNoCase(className, "CIM_ProviderElementCapabilities"))
	{
		// send to the configuration manager. it will generate the
		// appropriate response message.
		_configurationManager.enqueue(new CIMEnumerateInstancesRequestMessage(*request));

		return;
	}

	//
	// check the class name for subscription, filter and handler
	//
	if(String::equalNoCase(className, "CIM_IndicationSubscription") ||
	    String::equalNoCase(className, "CIM_IndicationHandler") ||
	    String::equalNoCase(className, "CIM_IndicationHandlerCIMXML") ||
	    String::equalNoCase(className, "CIM_IndicationHandlerSNMP") ||
	    String::equalNoCase(className, "CIM_IndicationFilter"))
	{
	    //
	    // Send to the indication service. It will generate the
	    // appropriate response message.
	    //
	    _indicationService.enqueue(new CIMEnumerateInstancesRequestMessage(*request));

	    return;
	}

	// check the class name for an "external provider"
	String providerName = _lookupProviderForClass(request->nameSpace, className);

	if(providerName.size() != 0)
	{
		// lookup provider manager
		MessageQueue * queue = MessageQueue::lookup("Server::ProviderManagerService");

		PEGASUS_ASSERT(queue != 0);

		// forward to provider manager. make a copy becuase the original request is
		// deleted by this service.
		queue->enqueue(new CIMEnumerateInstancesRequestMessage(*request));

		return;
	}

	CIMStatusCode errorCode = CIM_ERR_SUCCESS;
	String errorDescription;
	Array<CIMNamedInstance> cimNamedInstances;

	_repository->read_lock();

	try
	{
		cimNamedInstances = _repository->enumerateInstances(
			request->nameSpace,
			request->className,
			request->deepInheritance,
			request->localOnly,
			request->includeQualifiers,
			request->includeClassOrigin,
			request->propertyList.getPropertyNameArray());
	}
	catch(CIMException& exception)
	{
		errorCode = exception.getCode();
		errorDescription = exception.getMessage();
	}
	catch(Exception& exception)
	{
		errorCode = CIM_ERR_FAILED;
		errorDescription = exception.getMessage();
	}
	catch(...)
	{
		errorCode = CIM_ERR_FAILED;
	}

	_repository->read_unlock();

	CIMEnumerateInstancesResponseMessage* response =
		new CIMEnumerateInstancesResponseMessage(
		request->messageId,
		errorCode,
		errorDescription,
		request->queueIds.copyAndPop(),
		cimNamedInstances);

	_enqueueResponse(request, response);
}

void CIMOperationRequestDispatcher::handleEnumerateInstanceNamesRequest(
	CIMEnumerateInstanceNamesRequestMessage* request)
{
	// get the class name
	String className = request->className;

	// check the class name for an "internal provider"
	if(String::equalNoCase(className, "CIM_Provider") ||
	   String::equalNoCase(className, "CIM_ProviderCapabilities") ||
	   String::equalNoCase(className, "CIM_ProviderElementCapabilities"))
	{
		// send to the configuration manager. it will generate the
		// appropriate response message.
		_configurationManager.enqueue(new CIMEnumerateInstanceNamesRequestMessage(*request));

		return;
	}

	//
	// check the class name for subscription, filter and handler
	//
	if(String::equalNoCase(className, "CIM_IndicationSubscription") ||
		String::equalNoCase(className, "CIM_IndicationHandler") ||
		String::equalNoCase(className, "CIM_IndicationHandlerCIMXML") ||
		String::equalNoCase(className, "CIM_IndicationHandlerSNMP") ||
		String::equalNoCase(className, "CIM_IndicationFilter"))
	{
		//
		// Send to the indication service. It will generate the
		// appropriate response message.
		//
		_indicationService.enqueue(new CIMEnumerateInstanceNamesRequestMessage(*request));

		return;
	}

	// check the class name for an "external provider"
	String providerName = _lookupProviderForClass(request->nameSpace, className);

	if(providerName.size() != 0)
	{
		// lookup provider manager
		MessageQueue * queue = MessageQueue::lookup("Server::ProviderManagerService");

		PEGASUS_ASSERT(queue != 0);

		// forward to provider manager. make a copy becuase the original request is
		// deleted by this service.
		queue->enqueue(new CIMEnumerateInstanceNamesRequestMessage(*request));

		return;
	}
	
	CIMStatusCode errorCode = CIM_ERR_SUCCESS;
	String errorDescription;
	Array<CIMReference> instanceNames;

	_repository->read_lock();
	
	try
	{
		instanceNames = _repository->enumerateInstanceNames(
			request->nameSpace,
			request->className);
	}
	catch(CIMException& exception)
	{
		errorCode = exception.getCode();
		errorDescription = exception.getMessage();
	}
	catch(Exception& exception)
	{
		errorCode = CIM_ERR_FAILED;
		errorDescription = exception.getMessage();
	}
	catch(...)
	{
		errorCode = CIM_ERR_FAILED;
	}

	_repository->read_unlock();

	CIMEnumerateInstanceNamesResponseMessage* response =
		new CIMEnumerateInstanceNamesResponseMessage(
		request->messageId,
		errorCode,
		errorDescription,
		request->queueIds.copyAndPop(),
		instanceNames);

	_enqueueResponse(request, response);
}

void CIMOperationRequestDispatcher::handleAssociatorsRequest(
	CIMAssociatorsRequestMessage* request)
{
	String className = request->objectName.getClassName();
	
	// check the class name for an "external provider"
	String providerName = _lookupProviderForClass(request->nameSpace, className);

	if(providerName.size() != 0)
	{
		// lookup provider manager
		MessageQueue * queue = MessageQueue::lookup("Server::ProviderManagerService");

		PEGASUS_ASSERT(queue != 0);

		// forward to provider manager. make a copy becuase the original request is
		// deleted by this service.
		queue->enqueue(new CIMAssociatorsRequestMessage(*request));

		return;
	}

	CIMStatusCode errorCode = CIM_ERR_SUCCESS;
	String errorDescription;
	Array<CIMObjectWithPath> cimObjects;

	_repository->read_lock();

	try
	{
		cimObjects = _repository->associators(
			request->nameSpace,
			request->objectName,
			request->assocClass,
			request->resultClass,
			request->role,
			request->resultRole,
			request->includeQualifiers,
			request->includeClassOrigin,
			request->propertyList.getPropertyNameArray());
	}
	catch(CIMException& exception)
	{
		errorCode = exception.getCode();
		errorDescription = exception.getMessage();
	}
	catch(Exception& exception)
	{
		errorCode = CIM_ERR_FAILED;
		errorDescription = exception.getMessage();
	}
	catch(...)
	{
		errorCode = CIM_ERR_FAILED;
	}

	_repository->read_unlock();

	CIMAssociatorsResponseMessage* response =
		new CIMAssociatorsResponseMessage(
		request->messageId,
		errorCode,
		errorDescription,
		request->queueIds.copyAndPop(),
		cimObjects);

	_enqueueResponse(request, response);
}

void CIMOperationRequestDispatcher::handleAssociatorNamesRequest(
	CIMAssociatorNamesRequestMessage* request)
{
	String className = request->objectName.getClassName();
	
	// check the class name for an "external provider"
	String providerName = _lookupProviderForClass(request->nameSpace, className);

	if(providerName.size() != 0)
	{
		// lookup provider manager
		MessageQueue * queue = MessageQueue::lookup("Server::ProviderManagerService");

		PEGASUS_ASSERT(queue != 0);

		// forward to provider manager. make a copy becuase the original request is
		// deleted by this service.
		queue->enqueue(new CIMAssociatorNamesRequestMessage(*request));

		return;
	}

	CIMStatusCode errorCode = CIM_ERR_SUCCESS;
	String errorDescription;
	Array<CIMReference> objectNames;

	_repository->read_lock();

	try
	{
		objectNames = _repository->associatorNames(
			request->nameSpace,
			request->objectName,
			request->assocClass,
			request->resultClass,
			request->role,
			request->resultRole);
	}
	catch(CIMException& exception)
	{
		errorCode = exception.getCode();
		errorDescription = exception.getMessage();
	}
	catch(Exception& exception)
	{
		errorCode = CIM_ERR_FAILED;
		errorDescription = exception.getMessage();
	}
	catch(...)
	{
		errorCode = CIM_ERR_FAILED;
	}

	_repository->read_unlock();

	CIMAssociatorNamesResponseMessage* response =
		new CIMAssociatorNamesResponseMessage(
		request->messageId,
		errorCode,
		errorDescription,
		request->queueIds.copyAndPop(),
		objectNames);

	_enqueueResponse(request, response);
}

void CIMOperationRequestDispatcher::handleReferencesRequest(
	CIMReferencesRequestMessage* request)
{
	String className = request->objectName.getClassName();
	
	// check the class name for an "external provider"
	String providerName = _lookupProviderForClass(request->nameSpace, className);

	if(providerName.size() != 0)
	{
		// lookup provider manager
		MessageQueue * queue = MessageQueue::lookup("Server::ProviderManagerService");

		PEGASUS_ASSERT(queue != 0);

		// forward to provider manager. make a copy becuase the original request is
		// deleted by this service.
		queue->enqueue(new CIMReferencesRequestMessage(*request));

		return;
	}

	CIMStatusCode errorCode = CIM_ERR_SUCCESS;
	String errorDescription;
	Array<CIMObjectWithPath> cimObjects;

	_repository->read_lock();

	try
	{
		cimObjects = _repository->references(
			request->nameSpace,
			request->objectName,
			request->resultClass,
			request->role,
			request->includeQualifiers,
			request->includeClassOrigin,
			request->propertyList.getPropertyNameArray());
	}
	catch(CIMException& exception)
	{
		errorCode = exception.getCode();
		errorDescription = exception.getMessage();
	}
	catch(Exception& exception)
	{
		errorCode = CIM_ERR_FAILED;
		errorDescription = exception.getMessage();
	}
	catch(...)
	{
		errorCode = CIM_ERR_FAILED;
	}

	_repository->read_unlock();

	CIMReferencesResponseMessage* response =
		new CIMReferencesResponseMessage(
		request->messageId,
		errorCode,
		errorDescription,
		request->queueIds.copyAndPop(),
		cimObjects);

	_enqueueResponse(request, response);
}

void CIMOperationRequestDispatcher::handleReferenceNamesRequest(
	CIMReferenceNamesRequestMessage* request)
{
	String className = request->objectName.getClassName();
	
	// check the class name for an "external provider"
	String providerName = _lookupProviderForClass(request->nameSpace, className);

	if(providerName.size() != 0)
	{
		// lookup provider manager
		MessageQueue * queue = MessageQueue::lookup("Server::ProviderManagerService");

		PEGASUS_ASSERT(queue != 0);

		// forward to provider manager. make a copy becuase the original request is
		// deleted by this service.
		queue->enqueue(new CIMReferenceNamesRequestMessage(*request));

		return;
	}

	CIMStatusCode errorCode = CIM_ERR_SUCCESS;
	String errorDescription;
	Array<CIMReference> objectNames;

	_repository->read_lock();

	try
	{
		objectNames = _repository->referenceNames(
			request->nameSpace,
			request->objectName,
			request->resultClass,
			request->role);
	}
	catch(CIMException& exception)
	{
		errorCode = exception.getCode();
		errorDescription = exception.getMessage();
	}
	catch(Exception& exception)
	{
		errorCode = CIM_ERR_FAILED;
		errorDescription = exception.getMessage();
	}
	catch(...)
	{
		errorCode = CIM_ERR_FAILED;
	}

	_repository->read_unlock();

	CIMReferenceNamesResponseMessage* response =
		new CIMReferenceNamesResponseMessage(
		request->messageId,
		errorCode,
		errorDescription,
		request->queueIds.copyAndPop(),
		objectNames);

	_enqueueResponse(request, response);
}

void CIMOperationRequestDispatcher::handleGetPropertyRequest(
	CIMGetPropertyRequestMessage* request)
{
	String className = request->instanceName.getClassName();
	
	// check the class name for an "external provider"
	String providerName = _lookupProviderForClass(request->nameSpace, className);

	if(providerName.size() != 0)
	{
		// lookup provider manager
		MessageQueue * queue = MessageQueue::lookup("Server::ProviderManagerService");

		PEGASUS_ASSERT(queue != 0);

		// forward to provider manager. make a copy becuase the original request is
		// deleted by this service.
		queue->enqueue(new CIMGetPropertyRequestMessage(*request));

		return;
	}
	
	CIMStatusCode errorCode = CIM_ERR_SUCCESS;
	String errorDescription;
	CIMValue value;

	_repository->read_lock();
	
	try
	{
		value = _repository->getProperty(
			request->nameSpace,
			request->instanceName,
			request->propertyName);
	}
	catch(CIMException& exception)
	{
		errorCode = exception.getCode();
		errorDescription = exception.getMessage();
	}
	catch(Exception& exception)
	{
		errorCode = CIM_ERR_FAILED;
		errorDescription = exception.getMessage();
	}
	catch(...)
	{
		errorCode = CIM_ERR_FAILED;
	}
		
	_repository->read_unlock();
	
	CIMGetPropertyResponseMessage* response =
		new CIMGetPropertyResponseMessage(
		request->messageId,
		errorCode,
		errorDescription,
		request->queueIds.copyAndPop(),
		value);

	_enqueueResponse(request, response);
}

void CIMOperationRequestDispatcher::handleSetPropertyRequest(
	CIMSetPropertyRequestMessage* request)
{
	String className = request->instanceName.getClassName();
	
	// check the class name for an "external provider"
	String providerName = _lookupProviderForClass(request->nameSpace, className);

	if(providerName.size() != 0)
	{
		// lookup provider manager
		MessageQueue * queue = MessageQueue::lookup("Server::ProviderManagerService");

		PEGASUS_ASSERT(queue != 0);

		// forward to provider manager. make a copy becuase the original request is
		// deleted by this service.
		queue->enqueue(new CIMSetPropertyRequestMessage(*request));

		return;
	}
	
	CIMStatusCode errorCode = CIM_ERR_SUCCESS;
	String errorDescription;

	_repository->write_lock();

	try
	{
		_repository->setProperty(
			request->nameSpace,
			request->instanceName,
			request->propertyName,
			request->newValue);
	}
	catch(CIMException& exception)
	{
		errorCode = exception.getCode();
		errorDescription = exception.getMessage();
	}
	catch(Exception& exception)
	{
		errorCode = CIM_ERR_FAILED;
		errorDescription = exception.getMessage();
	}
	catch(...)
	{
		errorCode = CIM_ERR_FAILED;
	}

	_repository->write_unlock();

	CIMSetPropertyResponseMessage* response =
		new CIMSetPropertyResponseMessage(
		request->messageId,
		errorCode,
		errorDescription,
		request->queueIds.copyAndPop());

	_enqueueResponse(request, response);
}

void CIMOperationRequestDispatcher::handleGetQualifierRequest(
	CIMGetQualifierRequestMessage* request)
{
	CIMStatusCode errorCode = CIM_ERR_SUCCESS;
	String errorDescription;
	CIMQualifierDecl cimQualifierDecl;

	_repository->read_lock();
	
	try
	{
		cimQualifierDecl = _repository->getQualifier(
			request->nameSpace,
			request->qualifierName);
	}
	catch(CIMException& exception)
	{
		errorCode = exception.getCode();
		errorDescription = exception.getMessage();
	}
	catch(Exception& exception)
	{
		errorCode = CIM_ERR_FAILED;
		errorDescription = exception.getMessage();
	}
	catch(...)
	{
		errorCode = CIM_ERR_FAILED;
	}

	_repository->read_unlock();

	CIMGetQualifierResponseMessage* response =
		new CIMGetQualifierResponseMessage(
		request->messageId,
		errorCode,
		errorDescription,
		request->queueIds.copyAndPop(),
		cimQualifierDecl);

	_enqueueResponse(request, response);
}

void CIMOperationRequestDispatcher::handleSetQualifierRequest(
	CIMSetQualifierRequestMessage* request)
{
	CIMStatusCode errorCode = CIM_ERR_SUCCESS;
	String errorDescription;

	_repository->write_lock();
	
	try
	{
		_repository->setQualifier(
			request->nameSpace,
			request->qualifierDeclaration);
	}
	catch(CIMException& exception)
	{
		errorCode = exception.getCode();
		errorDescription = exception.getMessage();
	}
	catch(Exception& exception)
	{
		errorCode = CIM_ERR_FAILED;
		errorDescription = exception.getMessage();
	}
	catch(...)
	{
		errorCode = CIM_ERR_FAILED;
	}

	_repository->write_unlock();

	CIMSetQualifierResponseMessage* response =
		new CIMSetQualifierResponseMessage(
		request->messageId,
		errorCode,
		errorDescription,
		request->queueIds.copyAndPop());

	_enqueueResponse(request, response);
}

void CIMOperationRequestDispatcher::handleDeleteQualifierRequest(
	CIMDeleteQualifierRequestMessage* request)
{
	CIMStatusCode errorCode = CIM_ERR_SUCCESS;
	String errorDescription;

	_repository->write_lock();

	try
	{
		_repository->deleteQualifier(
			request->nameSpace,
			request->qualifierName);
	}

	catch(CIMException& exception)
	{
		errorCode = exception.getCode();
		errorDescription = exception.getMessage();
	}
	catch(Exception& exception)
	{
		errorCode = CIM_ERR_FAILED;
		errorDescription = exception.getMessage();
	}
	catch(...)
	{
		errorCode = CIM_ERR_FAILED;
	}

	_repository->write_unlock();

	CIMDeleteQualifierResponseMessage* response =
		new CIMDeleteQualifierResponseMessage(
		request->messageId,
		errorCode,
		errorDescription,
		request->queueIds.copyAndPop());

	_enqueueResponse(request, response);
}

void CIMOperationRequestDispatcher::handleEnumerateQualifiersRequest(
	CIMEnumerateQualifiersRequestMessage* request)
{
	CIMStatusCode errorCode = CIM_ERR_SUCCESS;
	String errorDescription;
	Array<CIMQualifierDecl> qualifierDeclarations;

	_repository->read_lock();

	try
	{
		qualifierDeclarations = _repository->enumerateQualifiers(
			request->nameSpace);
	}

	catch(CIMException& exception)
	{
		errorCode = exception.getCode();
		errorDescription = exception.getMessage();
	}
	catch(Exception& exception)
	{
		errorCode = CIM_ERR_FAILED;
		errorDescription = exception.getMessage();
	}
	catch(...)
	{
		errorCode = CIM_ERR_FAILED;
	}

	_repository->read_unlock();

	CIMEnumerateQualifiersResponseMessage* response =
		new CIMEnumerateQualifiersResponseMessage(
		request->messageId,
		errorCode,
		errorDescription,
		request->queueIds.copyAndPop(),
		qualifierDeclarations);

	_enqueueResponse(request, response);
}

void CIMOperationRequestDispatcher::handleInvokeMethodRequest(
	CIMInvokeMethodRequestMessage* request)
{
	String className = request->instanceName.getClassName();
	
	// check the class name for an "external provider"
	String providerName = _lookupProviderForClass(request->nameSpace, className);

	if(providerName.size() != 0)
	{
		// lookup provider manager
		MessageQueue * queue = MessageQueue::lookup("Server::ProviderManagerService");

		PEGASUS_ASSERT(queue != 0);

		// forward to provider manager. make a copy becuase the original request is
		// deleted by this service.
		queue->enqueue(new CIMInvokeMethodRequestMessage(*request));

		return;
	}
	
	CIMStatusCode errorCode = CIM_ERR_FAILED;
	String errorDescription = "Provider not available";
	CIMValue retValue(1);
	Array<CIMParamValue> outParameters;

	CIMInvokeMethodResponseMessage* response =
		new CIMInvokeMethodResponseMessage(
		request->messageId,
		errorCode,
		errorDescription,
		request->queueIds.copyAndPop(),
		retValue,
		outParameters,
		request->methodName);

	_enqueueResponse(request, response);
}

void CIMOperationRequestDispatcher::handleEnableIndicationSubscriptionRequest(
	CIMEnableIndicationSubscriptionRequestMessage* request)
{
	//
	// check the class name for an "external provider"
	//
	String providerName = _lookupProviderForClass(
	    request->nameSpace, request->classNames[0]);

	if(providerName.size() != 0)
	{
	    //
	    // forward request to the provider manager
	    //
		// lookup provider manager
		MessageQueue * queue = MessageQueue::lookup("Server::ProviderManagerService");

		PEGASUS_ASSERT(queue != 0);

		// forward to provider manager. make a copy becuase the original request is
		// deleted by this service.
		queue->enqueue(new CIMEnableIndicationSubscriptionRequestMessage(*request));
	
		return;
	}

	CIMEnableIndicationSubscriptionResponseMessage* response =
	new CIMEnableIndicationSubscriptionResponseMessage (
	request->messageId,
	CIM_ERR_FAILED,
	"Class in request is without Provider",
	request->queueIds.copyAndPop ());

    _enqueueResponse (request, response);
}

void CIMOperationRequestDispatcher::handleModifyIndicationSubscriptionRequest(
	CIMModifyIndicationSubscriptionRequestMessage* request)
{
	//  ATTN: Provider to be loaded is known to serve all classes
	//  in classNames list.  The getProvider function requires a class
	//  name.  There is no form that takes only the provider name.
	//  Currently, the first class name in the list is passed to
	//  getProvider.  It shouldn't matter which class name is passed in.

	//
	// check the class name for an "external provider"
	//
	String providerName = _lookupProviderForClass(
	    request->nameSpace, request->classNames[0]);

	if(providerName.size() != 0)
	{
		//
	    // forward request to the provider manager
	    //
		// lookup provider manager
		MessageQueue * queue = MessageQueue::lookup("Server::ProviderManagerService");

		PEGASUS_ASSERT(queue != 0);

		// forward to provider manager. make a copy becuase the original request is
		// deleted by this service.
		queue->enqueue(new CIMModifyIndicationSubscriptionRequestMessage(*request));
	
		return;
	}

	CIMModifyIndicationSubscriptionResponseMessage* response =
	new CIMModifyIndicationSubscriptionResponseMessage (
	request->messageId,
	CIM_ERR_FAILED,
	"Provider not available",
	request->queueIds.copyAndPop ());

    _enqueueResponse (request, response);
}

void CIMOperationRequestDispatcher::handleDisableIndicationSubscriptionRequest(
	CIMDisableIndicationSubscriptionRequestMessage* request)
{
	//  ATTN: Provider to be loaded is known to serve all classes
	//  in classNames list.  The getProvider function requires a class
	//  name.  There is no form that takes only the provider name.
	//  Currently, the first class name in the list is passed to
	//  getProvider.  It shouldn't matter which class name is passed in.

	//
	// check the class name for an "external provider"
	//
	String providerName = _lookupProviderForClass(
	    request->nameSpace, request->classNames[0]);

	if(providerName.size() != 0)
	{
		// lookup provider manager
		MessageQueue * queue = MessageQueue::lookup("Server::ProviderManagerService");

		PEGASUS_ASSERT(queue != 0);

		// forward to provider manager. make a copy becuase the original request is
		// deleted by this service.
		queue->enqueue(new CIMDisableIndicationSubscriptionRequestMessage(*request));
	
		return;
	}

    CIMDisableIndicationSubscriptionResponseMessage* response =
	new CIMDisableIndicationSubscriptionResponseMessage (
	request->messageId,
	CIM_ERR_FAILED,
	"Provider not available",
	request->queueIds.copyAndPop ());

    _enqueueResponse (request, response);
}

void CIMOperationRequestDispatcher::handleProcessIndicationRequest(
        CIMProcessIndicationRequestMessage* request)
{
    //
    // forward request to IndicationService. IndicartionService will take care
    // of response to this request.
    //
    _indicationService.enqueue(new CIMProcessIndicationRequestMessage(*request));

	return;
}

//
// ATTN : Do we need following code now?
//

/*
void CIMOperationRequestDispatcher::loadRegisteredProviders(void)
{
	CIMStatusCode errorCode = CIM_ERR_SUCCESS;
	String errorDescription;
	Array<CIMNamedInstance> cimNamedInstances;

	// ATTN: May need change
	const String& nameSpace = "root/cimv2";
	const String& className = "PG_Provider";

	_repository->read_lock();

	try
	{
		// ATTN: Exceptions are silently ignored for now
	cimNamedInstances = _repository->enumerateInstances(
		nameSpace,
		className);

	}

	catch (CIMException& exception)
	{
	errorCode = exception.getCode();
	errorDescription = exception.getMessage();
	}
	catch (Exception& exception)
	{
	errorCode = CIM_ERR_FAILED;
	errorDescription = exception.getMessage();
	}
	_repository->read_unlock();

	_providerManager.createProviderBlockTable(cimNamedInstances);
}
*/

PEGASUS_NAMESPACE_END
