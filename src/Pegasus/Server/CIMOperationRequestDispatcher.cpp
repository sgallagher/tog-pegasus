//%/////////////////////////////////////////////////////////////////////////////
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
#include <Pegasus/Repository/CIMRepository.h>
#include <Pegasus/Common/CIMOMHandle.h>
#include <Pegasus/Provider2/CIMIndicationProvider.h>
#include <Pegasus/Provider2/SimpleResponseHandler.h>
#include <Pegasus/Provider2/CIMBaseProviderFacade.h>

PEGASUS_NAMESPACE_BEGIN

PEGASUS_USING_STD;

//#define DDD(X) X
#define DDD(X) // X

DDD(static const char* _DISPATCHER = "CIMOperationRequestDispatcher::";)
   
   
struct timeval alloc_wait = { 0, 40 };
struct timeval dealloc_wait = { 10, 0 };
struct timeval deadlock_detect = { 5, 0 };



// called by the request handler to setup an asynchronous request
void async_operation_dispatch(CIMOperationRequestDispatcher *d, 
			      void (CIMOperationRequestDispatcher::*top)(AsyncOpNode *),
			      void (CIMOperationRequestDispatcher::*bottom)(AsyncOpNode *),
			      int rh_type, 
			      Message *request)
			      
   throw(IPCException)
{
   
   // allocate a CIM Operation_async,  opnode, context, and response handler
   // initialize with pointers to async top and async bottom
   // link to the waiting q
   

   //----------------------------------------------------------------- 
   // 1 - get all the resources we need to start an async operation
   //-----------------------------------------------------------------
   AsyncOpNodeLocal *node;
   CIMOperationRequestDispatcher::CIMOperation_async *operation;
   OperationContext *context;
   void *response_handler;
   
   node = static_cast<AsyncOpNodeLocal *>(d->_opnode_cache.remove_first());
   if(node == 0)
      node = new AsyncOpNodeLocal();

   operation = d->_async_cache.remove_first();
   if(operation == 0)
      operation = new CIMOperationRequestDispatcher::CIMOperation_async(d, top, bottom, node);
   
   context = d->_context_cache.remove_first();
   if(context == 0)
      context = new OperationContext();

   response_handler = d->_response_handler_cache[rh_type].remove_first();
   if(response_handler == 0)
      response_handler = create_rh(rh_type);

   //-----------------------------------------------------------------
   // 2 - lock the op node and initialize it with the other resources
   //-----------------------------------------------------------------

   node->lock();
   node->put_dispatch_async_struct(operation);
   node->put_request(request);
   node->put_req_context(context);
   node->put_response_handler(response_handler, rh_type);

   node->set_state_bits(ASYNC_OPSTATE_NORMAL | ASYNC_OPSTATE_SINGLE);
   node->set_flag_bits(ASYNC_OPFLAGS_UNKNOWN);
   
   //-----------------------------------------------------------------
   // 3 - unlock the node and put it on the waitng operations queue
   //-----------------------------------------------------------------
   
   node->unlock();
   d->_waiting_ops.insert_last(node);
   return;
}



CIMOperationRequestDispatcher::CIMOperationRequestDispatcher(CIMRepository* repository, CIMServer* server)
   : _repository(repository), _providerManager(this, repository, server),
     _dispatch_thread(_qthread, this, false),
     _dispatch_pool(10, "CIMOp dispatch", 5, 30, alloc_wait, dealloc_wait, deadlock_detect),
     _context_cache(true), _async_cache(true), _opnode_cache(true), _waiting_ops(true, 100),
     _started_ops(true, 100),  _completed_ops(true, 100), _dying(0)
   
{
   _response_handler_cache[RESPONSE_HANDLER_TYPE_CIM_CLASS] = DQueue<AsyncResponseHandler<CIMClass> >(true);
   _response_handler_cache[RESPONSE_HANDLER_TYPE_CIM_INSTANCE] = DQueue<AsyncResponseHandler<CIMInstance> >(true);
   _response_handler_cache[RESPONSE_HANDLER_TYPE_CIM_OBJECT] = DQueue<AsyncResponseHandler<CIMObject> >(true);
   _response_handler_cache[RESPONSE_HANDLER_TYPE_CIM_OBJECT_WITH_PATH] = DQueue<AsyncResponseHandler<CIMObjectWithPath> >(true);   
   _response_handler_cache[RESPONSE_HANDLER_TYPE_CIM_VALUE] = DQueue<AsyncResponseHandler<CIMValue> >(true);
   _response_handler_cache[RESPONSE_HANDLER_TYPE_CIM_INDICATION] = DQueue<AsyncResponseHandler<CIMIndication> >(true);
   _response_handler_cache[RESPONSE_HANDLER_TYPE_CIM_REFERENCE] = DQueue<AsyncResponseHandler<CIMReference> >(true);
   
    DDD(cout << _DISPATCHER << endl;)
}

CIMOperationRequestDispatcher::~CIMOperationRequestDispatcher()
{
   _dying = 1;
   
}

String CIMOperationRequestDispatcher::_lookupProviderForClass(
    const String& nameSpace,
    const String& className)
{
    //----------------------------------------------------------------------
    // Look up the class:
    //----------------------------------------------------------------------

    CIMClass cimClass;
    Array<CIMNamedInstance> enumInstances;
    CIMInstance instance;
    String classname;
    String providername;

   // check if it is asking for PG_RegistrationProvider
   if (className == "PG_Provider" || className == "CIM_ProviderCapabilities")
   {
	return ("PG_ProviderRegistration");
   }

   // use provider registration to find provider
    try
    {
      // get all the registered providers
       enumInstances = _repository->enumerateInstances(nameSpace, "CIM_ProviderCapabilities");
    }

    catch (CIMException& e)
    {
        // ATTN: Fail silently for now
        //if (e.getCode() == CIM_ERR_NOT_FOUND)
        //{
        //    throw CIMException(CIM_ERR_INVALID_CLASS);
        //}
        //else
        //{
        //    throw e;
        //}
    }

    for (Uint32 i = 0, n = enumInstances.size(); i < n ; i++)
    {
	instance = enumInstances[i].getInstance();
	// get className
	classname = instance.getProperty(instance.findProperty("ClassName")).getValue().toString();
	if (String::equal(classname, className))
	{
	  providername = instance.getProperty(instance.findProperty("ProviderName")).getValue().toString();

          if (_providerManager.isProviderBlocked(providername)) // blocked
          {
	     // if the provider is blocked

	     throw PEGASUS_CIM_EXCEPTION(CIM_ERR_ACCESS_DENIED, "provider is blocked"); 
	     return (String::EMPTY);
          }
          else
          {
	     return(providername);
          }
	} 
    }		

// ATTN: still use qualifier to find provider if a provider did not use 
// PG_RegistrationProvider to register. Will remove in the future 
    try
    {
        cimClass = _repository->getClass(nameSpace, className);
        DDD(cout << _DISPATCHER << "Lookup Provider for " << className << endl;)
    }
    catch (CIMException& e)
    {
        if (e.getCode() == CIM_ERR_NOT_FOUND)
        {
	    throw CIMException(CIM_ERR_INVALID_CLASS);
        }
        else
        {
            throw e;
        }
    }
    //----------------------------------------------------------------------
    // Get the provider qualifier:
    //----------------------------------------------------------------------

    Uint32 pos = cimClass.findQualifier("provider");
    DDD(cout << _DISPATCHER << "Lookup Qualifier " << pos << endl;)

    if (pos == PEG_NOT_FOUND)
	return String::EMPTY;

    CIMQualifier q = cimClass.getQualifier(pos);
    String providerId;

    q.getValue().get(providerId);
    DDD(cout << _DISPATCHER << "Provider " << providerId << endl;)

    if (_providerManager.isProviderBlocked(providerId)) // blocked
    {
	// if the provider is blocked

	throw PEGASUS_CIM_EXCEPTION(CIM_ERR_ACCESS_DENIED, "provider is blocked"); 
    }
    else
    {
	return(providerId);
    }
}

////////////////////////////////////////////////////////////////////////////////

void CIMOperationRequestDispatcher::_enqueueResponse(
    CIMRequestMessage* request,
    CIMResponseMessage* response)
{
    // Use the same key as used in the request:

    response->setKey(request->getKey());

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

    if (!request)
	return;
    

    

    switch (request->getType())
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
    }

    delete request;
}

const char* CIMOperationRequestDispatcher::getQueueName() const
{
    return "CIMOperationRequestDispatcher";
}

void CIMOperationRequestDispatcher::handleGetClassRequest(
    CIMGetClassRequestMessage* request)
{
    // ATTN: Need code here to expand partial class!

    CIMStatusCode errorCode = CIM_ERR_SUCCESS;
    String errorDescription;
    CIMClass cimClass;

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

    CIMStatusCode errorCode = CIM_ERR_SUCCESS;
    String errorDescription;
    CIMInstance cimInstance;

    try
    {
	// get provider for class
	String className = request->instanceName.getClassName();
	String providerName = _lookupProviderForClass(request->nameSpace, className);

        if ( (providerName.size() == 0) &&
                 !_repository->isDefaultInstanceProvider() )
        {
            // ATTN: Is this the right exception?
            throw CIMException(CIM_ERR_NOT_SUPPORTED);
        }
        else if ( (providerName.size() == 0) ||
                  (providerName == _repository->getProviderName()) )
        {
	    cimInstance = _repository->getInstance(
		request->nameSpace,
		request->instanceName,
		request->localOnly,
		request->includeQualifiers,
		request->includeClassOrigin,
		request->propertyList.getPropertyNameArray()); 
	}
	else
	{
	    // attempt to load provider
	    ProviderHandle * provider = _providerManager.getProvider(providerName, className);

	    cimInstance = provider->getInstance(
		OperationContext(),
		request->nameSpace,
		request->instanceName,
		request->localOnly,
		request->includeQualifiers,
		request->includeClassOrigin,
		request->propertyList.getPropertyNameArray()); 
	}
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

    try
    {
	_repository->deleteClass(
	    request->nameSpace,
	    request->className);
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
    CIMStatusCode errorCode = CIM_ERR_SUCCESS;
    String errorDescription;

    try
    {
	// get provider for class
	String className = request->instanceName.getClassName();
	String providerName = _lookupProviderForClass(request->nameSpace, className);

        if ( (providerName.size() == 0) &&
                 !_repository->isDefaultInstanceProvider() )
        {
            // ATTN: Is this the right exception?
            throw CIMException(CIM_ERR_NOT_SUPPORTED);
        }
        else if ( (providerName.size() == 0) ||
                  (providerName == _repository->getProviderName()) )
	{
	    _repository->deleteInstance(
		request->nameSpace,
		request->instanceName);
	}
	else
	{
	    // attempt to load provider
	    ProviderHandle * provider = _providerManager.getProvider(providerName, className);
	
	    provider->deleteInstance(
		OperationContext(),
		request->nameSpace,
		request->instanceName);
	}
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

    try
    {
	_repository->createClass(
	    request->nameSpace,
	    request->newClass);
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
    CIMStatusCode errorCode = CIM_ERR_SUCCESS;
    String errorDescription;
    CIMReference instanceName;

    CIMInstance cimInstance(request->newInstance);

    try
    {
	// get provider for class
	String className = request->newInstance.getClassName();
        CIMClass cimClass = className;
	String providerName = _lookupProviderForClass(request->nameSpace, className);

        if ( (providerName.size() == 0) &&
                 !_repository->isDefaultInstanceProvider() )
        {
            // ATTN: Is this the right exception?
            throw CIMException(CIM_ERR_NOT_SUPPORTED);
        }
        else if ( (providerName.size() == 0) ||
                  (providerName == _repository->getProviderName()) )
	{
	     instanceName = _repository->createInstance(
		request->nameSpace,
		request->newInstance);
	}
	else
	{
	    // attempt to load provider
	    ProviderHandle * provider = _providerManager.getProvider(providerName, className);

	    instanceName = provider->createInstance(
		OperationContext(),
		request->nameSpace,
		request->newInstance);
	}
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

    try
    {
	_repository->modifyClass(
	    request->nameSpace,
	    request->modifiedClass);
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
    CIMStatusCode errorCode = CIM_ERR_SUCCESS;
    String errorDescription;

    try
    {
        // ATTN: Who makes sure the instance name and the instance match?

	// get provider for class
	String className = request->modifiedInstance.getInstance().getClassName();
	String providerName = _lookupProviderForClass(request->nameSpace, className);

        if ( (providerName.size() == 0) &&
                 !_repository->isDefaultInstanceProvider() )
        {
            // ATTN: Is this the right exception?
            throw CIMException(CIM_ERR_NOT_SUPPORTED);
        }
        else if ( (providerName.size() == 0) ||
                  (providerName == _repository->getProviderName()) )
        {
            _repository->modifyInstance(
                request->nameSpace,
                request->modifiedInstance,
                request->includeQualifiers,
                request->propertyList);
        }
        else
	{
	    // attempt to load provider
	    ProviderHandle * provider = _providerManager.getProvider(providerName, className);

	    provider->modifyInstance(
		OperationContext(),
                request->nameSpace,
                request->modifiedInstance,
                request->includeQualifiers,
                request->propertyList);
        }
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

    try
    {
	classNames = _repository->enumerateClassNames(
	    request->nameSpace,
	    request->className,
	    request->deepInheritance);
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
    CIMStatusCode errorCode = CIM_ERR_SUCCESS;
    String errorDescription;
    Array<CIMNamedInstance> cimNamedInstances;

    try
    {
	// get provider for class
	String className = request->className;
	String providerName = _lookupProviderForClass(request->nameSpace, className);

        if ( (providerName.size() == 0) &&
                 !_repository->isDefaultInstanceProvider() )
        {
            // ATTN: Is this the right exception?
            throw CIMException(CIM_ERR_NOT_SUPPORTED);
        }
        else if ( (providerName.size() == 0) ||
                  (providerName == _repository->getProviderName()) )
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
	else
	{
	    // attempt to load provider
	    ProviderHandle * provider = _providerManager.getProvider(providerName, className);

	    cimNamedInstances = provider->enumerateInstances(
		OperationContext(),
		request->nameSpace,
		request->className,
		request->deepInheritance,
		request->localOnly,
		request->includeQualifiers,
		request->includeClassOrigin,
		request->propertyList.getPropertyNameArray());
	}
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
    CIMStatusCode errorCode = CIM_ERR_SUCCESS;
    String errorDescription;
    Array<CIMReference> instanceNames;

    try
    {
	// get provider for class
	String className = request->className;
	String providerName = _lookupProviderForClass(request->nameSpace, className);

        if ( (providerName.size() == 0) &&
                 !_repository->isDefaultInstanceProvider() )
        {
            // ATTN: Is this the right exception?
            throw CIMException(CIM_ERR_NOT_SUPPORTED);
        }
        else if ( (providerName.size() == 0) ||
                  (providerName == _repository->getProviderName()) )
	{
	    instanceNames = _repository->enumerateInstanceNames(
		request->nameSpace,
		request->className);
	}
	else
	{
	    // attempt to load provider
	    ProviderHandle * provider = _providerManager.getProvider(providerName, className);

	    instanceNames = provider->enumerateInstanceNames(
		OperationContext(),
		request->nameSpace,
		request->className);
	}
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
    CIMStatusCode errorCode = CIM_ERR_SUCCESS;
    String errorDescription;
    Array<CIMObjectWithPath> cimObjects;

    try
    {
	// get provider for class
	String className = request->objectName.getClassName();
	String providerName = _lookupProviderForClass(request->nameSpace, className);

        if ( (providerName.size() == 0) &&
                 !_repository->isDefaultInstanceProvider() )
        {
            // ATTN: Is this the right exception?
            throw CIMException(CIM_ERR_NOT_SUPPORTED);
        }
        else if ( (providerName.size() == 0) ||
                  (providerName == _repository->getProviderName()) )
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
	else
	{
	    // attempt to load provider
	    ProviderHandle * provider = _providerManager.getProvider(providerName, className);

	    cimObjects = provider->associators(
		OperationContext(),
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
    CIMStatusCode errorCode = CIM_ERR_SUCCESS;
    String errorDescription;
    Array<CIMReference> objectNames;

    try
    {
	// get provider for class
	String className = request->objectName.getClassName();
	String providerName = _lookupProviderForClass(request->nameSpace, className);

        if ( (providerName.size() == 0) &&
                 !_repository->isDefaultInstanceProvider() )
        {
            // ATTN: Is this the right exception?
            throw CIMException(CIM_ERR_NOT_SUPPORTED);
        }
        else if ( (providerName.size() == 0) ||
                  (providerName == _repository->getProviderName()) )
	{
	    objectNames = _repository->associatorNames(
	        request->nameSpace,
	        request->objectName,
	        request->assocClass,
	        request->resultClass,
	        request->role,
	        request->resultRole);
        }
	else
	{
	    // attempt to load provider
	    ProviderHandle * provider = _providerManager.getProvider(providerName, className);

	    objectNames = provider->associatorNames(
		OperationContext(),
	        request->nameSpace,
	        request->objectName,
	        request->assocClass,
	        request->resultClass,
	        request->role,
	        request->resultRole);
        }
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
    CIMStatusCode errorCode = CIM_ERR_SUCCESS;
    String errorDescription;
    Array<CIMObjectWithPath> cimObjects;

    try
    {
	// get provider for class
	String className = request->objectName.getClassName();
	String providerName = _lookupProviderForClass(request->nameSpace, className);

        if ( (providerName.size() == 0) &&
                 !_repository->isDefaultInstanceProvider() )
        {
            // ATTN: Is this the right exception?
            throw CIMException(CIM_ERR_NOT_SUPPORTED);
        }
        else if ( (providerName.size() == 0) ||
                  (providerName == _repository->getProviderName()) )
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
	else
	{
	    // attempt to load provider
	    ProviderHandle * provider = _providerManager.getProvider(providerName, className);

	    cimObjects = provider->references(
		OperationContext(),
	        request->nameSpace,
	        request->objectName,
	        request->resultClass,
	        request->role,
	        request->includeQualifiers,
	        request->includeClassOrigin,
	        request->propertyList.getPropertyNameArray());
        }
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
    CIMStatusCode errorCode = CIM_ERR_SUCCESS;
    String errorDescription;
    Array<CIMReference> objectNames;

    try
    {
	// get provider for class
	String className = request->objectName.getClassName();
	String providerName = _lookupProviderForClass(request->nameSpace, className);

        if ( (providerName.size() == 0) &&
                 !_repository->isDefaultInstanceProvider() )
        {
            // ATTN: Is this the right exception?
            throw CIMException(CIM_ERR_NOT_SUPPORTED);
        }
        else if ( (providerName.size() == 0) ||
                  (providerName == _repository->getProviderName()) )
	{
	    objectNames = _repository->referenceNames(
	        request->nameSpace,
	        request->objectName,
	        request->resultClass,
	        request->role);
        }
	else
	{
		// attempt to load provider
		ProviderHandle * provider = _providerManager.getProvider(providerName, className);

	    objectNames = provider->referenceNames(
		OperationContext(),
	        request->nameSpace,
	        request->objectName,
	        request->resultClass,
	        request->role);
        }
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
    CIMStatusCode errorCode = CIM_ERR_SUCCESS;
    String errorDescription;
    CIMValue value;

    try
    {
	// get provider for class
	String className = request->instanceName.getClassName();
	String providerName = _lookupProviderForClass(request->nameSpace, className);

        if ( (providerName.size() == 0) &&
                 !_repository->isDefaultInstanceProvider() )
        {
            // ATTN: Is this the right exception?
            throw CIMException(CIM_ERR_NOT_SUPPORTED);
        }
        else if ( (providerName.size() == 0) ||
                  (providerName == _repository->getProviderName()) )
        {
            value = _repository->getProperty(
                request->nameSpace,
                request->instanceName,
                request->propertyName);
        }
        else
	{
	    // attempt to load provider
	    ProviderHandle * provider = _providerManager.getProvider(providerName, className);

	    value = provider->getProperty(
		OperationContext(),
                request->nameSpace,
                request->instanceName,
                request->propertyName);
        }
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
    CIMStatusCode errorCode = CIM_ERR_SUCCESS;
    String errorDescription;

    try
    {
	// get provider for class
	String className = request->instanceName.getClassName();
	String providerName = _lookupProviderForClass(request->nameSpace, className);

        if ( (providerName.size() == 0) &&
                 !_repository->isDefaultInstanceProvider() )
        {
            // ATTN: Is this the right exception?
            throw CIMException(CIM_ERR_NOT_SUPPORTED);
        }
        else if ( (providerName.size() == 0) ||
                  (providerName == _repository->getProviderName()) )
        {
            _repository->setProperty(
                request->nameSpace,
                request->instanceName,
                request->propertyName,
                request->newValue);
        }
        else
	{
	    // attempt to load provider
	    ProviderHandle * provider = _providerManager.getProvider(providerName, className);

	    provider->setProperty(
		OperationContext(),
                request->nameSpace,
                request->instanceName,
                request->propertyName,
                request->newValue);
        }
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

    try
    {
	cimQualifierDecl = _repository->getQualifier(
	    request->nameSpace,
	    request->qualifierName);
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

    try
    {
	_repository->setQualifier(
	    request->nameSpace,
	    request->qualifierDeclaration);
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

    try
    {
	_repository->deleteQualifier(
	    request->nameSpace,
	    request->qualifierName);
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

    try
    {
	qualifierDeclarations = _repository->enumerateQualifiers(
	    request->nameSpace);
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
    CIMStatusCode errorCode = CIM_ERR_SUCCESS;
    String errorDescription;

    CIMValue retValue(0);

    Array<CIMParamValue> outParameters;

    try
    {
	// get provider for class
	String className = request->instanceName.getClassName();
	String providerName = _lookupProviderForClass(request->nameSpace, className);

	if(providerName.size() != 0)
	{
	    // attempt to load provider
	    ProviderHandle * provider = _providerManager.getProvider(providerName, className);

    	    retValue = provider->invokeMethod(
		OperationContext(),
		request->nameSpace,
		request->instanceName,
		request->methodName,
		request->inParameters,
		outParameters);
	}
	else
	{
	    retValue.set(1);
	    errorCode = CIM_ERR_SUCCESS;
	    errorDescription = "Provider not available";
	}
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
    CIMStatusCode errorCode = CIM_ERR_SUCCESS;
    String errorDescription = String::EMPTY;

    try
    {
        if (request->providerName.size () != 0)
        {
            //
            // attempt to load provider
            //
            //
            //  ATTN: Provider to be loaded is known to serve all classes
            //  in classNames list.  The getProvider function requires a class
            //  name.  There is no form that takes only the provider name.
            //  Currently, the first class name in the list is passed to
            //  getProvider.  It shouldn't matter which class name is passed in.
            //
            ProviderHandle * provider = _providerManager.getProvider
                (request->providerName, request->classNames [0]);

            SimpleResponseHandler <CIMIndication> responseHandler;

            provider->enableIndicationSubscription (
                //
                //  ATTN: pass thresholding parameter values in
                //  operation context
                //
                OperationContext (),
                request->nameSpace,
                request->classNames,
                request->propertyList,
                request->repeatNotificationPolicy,
                request->condition,
                request->queryLanguage,
                request->subscription,
                responseHandler);
        }
        else
        {
            errorCode = CIM_ERR_FAILED;
            errorDescription = "Provider not available";
        }
    }
    catch (CIMException& exception)
    {
        errorCode = exception.getCode ();
        errorDescription = exception.getMessage ();
    }
    catch (Exception& exception)
    {
        errorCode = CIM_ERR_FAILED;
        errorDescription = exception.getMessage ();
    }

    CIMEnableIndicationSubscriptionResponseMessage* response =
        new CIMEnableIndicationSubscriptionResponseMessage (
            request->messageId,
            errorCode,
            errorDescription,
            request->queueIds.copyAndPop ());

    _enqueueResponse (request, response);
}

void CIMOperationRequestDispatcher::handleModifyIndicationSubscriptionRequest(
    CIMModifyIndicationSubscriptionRequestMessage* request)
{
    CIMStatusCode errorCode = CIM_ERR_SUCCESS;
    String errorDescription = String::EMPTY;

    try
    {
        if (request->providerName.size () != 0)
        {
            //
            // attempt to load provider
            //
            //
            //  ATTN: Provider to be loaded is known to serve all classes
            //  in classNames list.  The getProvider function requires a class
            //  name.  There is no form that takes only the provider name.
            //  Currently, the first class name in the list is passed to
            //  getProvider.  It shouldn't matter which class name is passed in.
            //
            ProviderHandle * provider = _providerManager.getProvider
                (request->providerName, request->classNames [0]);

            SimpleResponseHandler <CIMIndication> responseHandler;

            provider->modifyIndicationSubscription (
                //
                //  ATTN: pass thresholding parameter values in
                //  operation context
                //
                OperationContext (),
                request->nameSpace,
                request->classNames,
                request->propertyList,
                request->repeatNotificationPolicy,
                request->condition,
                request->queryLanguage,
                request->subscription,
                responseHandler);
        }
        else
        {
            errorCode = CIM_ERR_FAILED;
            errorDescription = "Provider not available";
        }
    }
    catch (CIMException& exception)
    {
        errorCode = exception.getCode ();
        errorDescription = exception.getMessage ();
    }
    catch (Exception& exception)
    {
        errorCode = CIM_ERR_FAILED;
        errorDescription = exception.getMessage ();
    }

    CIMModifyIndicationSubscriptionResponseMessage* response =
        new CIMModifyIndicationSubscriptionResponseMessage (
            request->messageId,
            errorCode,
            errorDescription,
            request->queueIds.copyAndPop ());

    _enqueueResponse (request, response);
}

void CIMOperationRequestDispatcher::handleDisableIndicationSubscriptionRequest(
    CIMDisableIndicationSubscriptionRequestMessage* request)
{
    CIMStatusCode errorCode = CIM_ERR_SUCCESS;
    String errorDescription = String::EMPTY;

    try
    {
        if (request->providerName.size () != 0)
        {
            //
            // attempt to load provider
            //
            //
            //  ATTN: Provider to be loaded is known to serve all classes
            //  in classNames list.  The getProvider function requires a class
            //  name.  There is no form that takes only the provider name.
            //  Currently, the first class name in the list is passed to
            //  getProvider.  It shouldn't matter which class name is passed in.
            //
            ProviderHandle * provider = _providerManager.getProvider
                (request->providerName, request->classNames [0]);

            SimpleResponseHandler <CIMIndication> responseHandler;

            provider->disableIndicationSubscription (
                OperationContext (),
                request->nameSpace,
                request->classNames,
                request->subscription,
                responseHandler);
        }
        else
        {
            errorCode = CIM_ERR_FAILED;
            errorDescription = "Provider not available";
        }
    }
    catch (CIMException& exception)
    {
        errorCode = exception.getCode ();
        errorDescription = exception.getMessage ();
    }
    catch (Exception& exception)
    {
        errorCode = CIM_ERR_FAILED;
        errorDescription = exception.getMessage ();
    }

    CIMDisableIndicationSubscriptionResponseMessage* response =
        new CIMDisableIndicationSubscriptionResponseMessage (
            request->messageId,
            errorCode,
            errorDescription,
            request->queueIds.copyAndPop ());

    _enqueueResponse (request, response);
}


// this is a static class method
// called by a pooled thread.

//    subscription is an association between an indication handler
// change this to receive a worknode as the parameter
PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL CIMOperationRequestDispatcher::async_dispatcher(void *parm)
{

   AsyncOpNodeLocal *work = reinterpret_cast<AsyncOpNodeLocal *>(parm);
   work->lock();
   
   CIMOperation_async *op = reinterpret_cast<CIMOperation_async *>(work->get_dispatch_async_struct());
   CIMOperationRequestDispatcher *dispatcher = op->dispatcher;
   
   work->unlock();
   

   if(work->test_flag_bit(ASYNC_OPFLAGS_COMPLETE))
   {
      // 1 - unlink from started list
      // 2 - call bottom half
      // 3 - link to completed list for recycling
      (dispatcher->*(op->bottom_half))(static_cast<AsyncOpNode *>(work));
   }
   else
   {
      // unlink from waiting list
      // 1 - call top half
      // 2 - link to started list
      // 3 - return
      (dispatcher->*(op->top_half))(static_cast<AsyncOpNode *>(work));
   }
   
   return(0);
}

// static class method 

// 1 - pull all nodes off the waiting list and start them, then put them on the started list
// 2 - check the started list for nodes that are complete
//     unlink completed nodes and dispatch them
//     put completed nodes on the completed list
// 3 - if no nodes from step 2, recycle nodes on the completed list


// to start or dispatch an operation, call allocate_and_awaken with the pointer
//  to the dispatcher as the parm, 
PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL CIMOperationRequestDispatcher::_qthread(void *parm)
{
   Thread *myself = reinterpret_cast<Thread *>(parm);
   if(myself == 0)
      throw NullPointer();
   
   CIMOperationRequestDispatcher *dispatcher =
      reinterpret_cast<CIMOperationRequestDispatcher *>(myself->get_parm());
   if(dispatcher == 0)
      throw NullPointer();
      
   while(dispatcher->_dying.value() != 0)
   {

     // 1 - pull all nodes off the waiting list and start them, then put them on the started list
      AsyncOpNodeLocal *worknode = static_cast<AsyncOpNodeLocal *>(dispatcher->_waiting_ops.remove_first());
      while(worknode != 0)
      {
	 worknode->lock();
	 worknode->set_flag_bits(ASYNC_OPFLAGS_STARTED);
	 dispatcher->_dispatch_pool.allocate_and_awaken(worknode, 
							async_dispatcher);
	 worknode->unlock();
	 dispatcher->_started_ops.insert_first_wait(worknode);
	 worknode = static_cast<AsyncOpNodeLocal *>(dispatcher->_waiting_ops.remove_first());
      }
      
      
   }
   myself->test_cancel();
   myself->exit_self(0);
   return((PEGASUS_THREAD_RETURN)0);

}

// ATTN:: Temporaryly required to support provider registration service
ProviderManager* CIMOperationRequestDispatcher::getProviderManager(void)
{
    return &_providerManager;
}

void CIMOperationRequestDispatcher::loadRegisteredProviders(void)
{
    CIMStatusCode errorCode = CIM_ERR_SUCCESS;
    String errorDescription;
    Array<CIMNamedInstance> cimNamedInstances;

    // ATTN: May need change 
    const String& nameSpace = "root/cimv2";
    const String& className = "PG_Provider";

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

    _providerManager.createProviderBlockTable(cimNamedInstances);
}


PEGASUS_NAMESPACE_END
