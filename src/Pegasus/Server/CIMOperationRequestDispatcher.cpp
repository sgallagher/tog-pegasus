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
#include <Pegasus/Provider/CIMOMHandle.h>
#include <Pegasus/Provider/CIMIndicationProvider.h>
#include <Pegasus/Provider/SimpleResponseHandler.h>
#include <Pegasus/Provider/OperationFlag.h>

#include <Pegasus/Server/CIMBaseProviderFacade.h>

PEGASUS_NAMESPACE_BEGIN

PEGASUS_USING_STD;

//#define DDD(X) X
#define DDD(X) // X

DDD(static const char* _DISPATCHER = "CIMOperationRequestDispatcher::";)


CIMOperationRequestDispatcher::CIMOperationRequestDispatcher(
    CIMRepository* repository,
    CIMServer* server)
   :
   Base("CIMOpRequestDispatcher", true),
   _repository(repository),
   _providerManager(this, repository, server)
{

    DDD(cout << _DISPATCHER << endl;)
}

CIMOperationRequestDispatcher::~CIMOperationRequestDispatcher()
{
   _dying = 1;

}



// ATTN
// this needs to return an array of names. i.e., there could be
// more than one provider per class
//

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

   // get all the registered providers
   _repository->read_lock();

   try
   {
      enumInstances = _repository->enumerateInstances(nameSpace, "CIM_ProviderCapabilities");
   }
   catch(CIMException&)
   {
      // ATTN: Fail silently for now
   }

   _repository->read_unlock();

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
   _repository->read_lock();
   try
   {
      cimClass = _repository->getClass(nameSpace, className);
   }
   catch (CIMException& e)
   {
      _repository->read_unlock();

      if (e.getCode() == CIM_ERR_NOT_FOUND)
      {
	 throw CIMException(CIM_ERR_INVALID_CLASS);
      }
      else
      {
	 throw e;
      }
   }
   _repository->read_unlock();

   DDD(cout << _DISPATCHER << "Lookup Provider for " << className << endl;)



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

// 	case CIM_CANCEL_INDICATION_REQUEST_MESSAGE:
// 	    handleCancelIndicationRequest(
// 		(CIMCancelIndicationRequestMessage*)request);
// 	    break;

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

	// NOTE - getProviderName reads a const string and does not
	// need to use the rw lock
        else if ( (providerName.size() == 0) ||
                  (providerName == _repository->getProviderName()) )
        {
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
	   catch (...)
	   {
	      _repository->read_unlock();
	      throw;
	   }
	   _repository->read_unlock();
	}
	
	else
	{
	    // attempt to load provider
	    CIMBaseProvider * provider = _providerManager.getProvider(providerName, className);
		CIMBaseProviderFacade facade(provider);

		Uint32 flags = OperationFlag::convert(
			request->localOnly,
			request->includeQualifiers,
			request->includeClassOrigin);
		
		SimpleResponseHandler<CIMInstance> handler;

	    facade.getInstance(
		OperationContext(),
		request->instanceName,
		flags,
		request->propertyList.getPropertyNameArray(),
		handler);

		cimInstance = handler._objects[0];
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

    _repository->write_lock();

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
	   _repository->write_lock();
	   try
	   {
	      _repository->deleteInstance(
		 request->nameSpace,
		 request->instanceName);
	   }
	   catch( ... )
	   {
	      _repository->write_unlock();
	      throw;
	   }
	
	   _repository->write_unlock();
	
	}
	else
	{
	    // attempt to load provider
	    CIMBaseProvider * provider = _providerManager.getProvider(providerName, className);
		CIMBaseProviderFacade facade(provider);
	
	    SimpleResponseHandler<CIMInstance> handler;
		
		facade.deleteInstance(
		OperationContext(),
		request->instanceName,
		handler);
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

    _repository->write_lock();

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
    CIMStatusCode errorCode = CIM_ERR_SUCCESS;
    String errorDescription;
    CIMReference instanceName;

    CIMInstance cimInstance(request->newInstance);

	// ATTN: SPECIAL CASE
	{
		// CreateInstance requests for classes of type CIM_ObjectManager and CIM_Provider
		// go to the ConfigurationManager.
		// ATTN: should check class inheritance, not just the name
		String className = request->newInstance.getClassName();
	
		if(String::equalNoCase(className, "CIM_ObjectManager") ||
           String::equalNoCase(className, "CIM_Provider"))
		{
			// ATTN: function under construction
			
			//_configurationManager->handleEnqueue(request);
	
			CIMResponseMessage * response = new CIMCreateInstanceResponseMessage(
				request->messageId,
				CIM_ERR_FAILED,
				"function under construction",
				request->queueIds.copyAndPop(),
				CIMReference());
	
			_enqueueResponse(request, response);
	
			return;
		}
		
		// CreateInstance requests for classes of type CIM_IndicationSubscription
		// go to the Subscription Processor
		// ATTN: should check class inheritance, not just the name
		if(String::equalNoCase(className, "CIM_IndicationSubscription"))
		{
			// ATTN: function under construction
			
			//_subscriptionProcessor->handleEnqueue(request);
	
			CIMResponseMessage * response = new CIMCreateInstanceResponseMessage(
				request->messageId,
				CIM_ERR_FAILED,
				"function under construction",
				request->queueIds.copyAndPop(),
				CIMReference());
	
			_enqueueResponse(request, response);
	
			return;
		}
	}
	
	try
    {
	// get provider for class
	String className = request->newInstance.getClassName();
        //CIMClass cimClass = className;
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

	   _repository->write_lock();

	   try
	   {
	      instanceName = _repository->createInstance(
		 request->nameSpace,
		 request->newInstance);
	   }
	   catch( ... )
	   {
	      _repository->write_unlock();
	      throw;
	   }
	
	   _repository->write_unlock();

	}
	else
	{
	    // attempt to load provider
	    CIMBaseProvider * provider = _providerManager.getProvider(providerName, className);
		CIMBaseProviderFacade facade(provider);

	    SimpleResponseHandler<CIMReference> handler;
		
		facade.createInstance(
		OperationContext(),
		CIMReference(),
		request->newInstance,
		handler);

		//instanceName = handler._objects[0];
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

    _repository->write_lock();

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
    CIMStatusCode errorCode = CIM_ERR_SUCCESS;
    String errorDescription;

	// ATTN: SPECIAL CASE
	{
		// ModifyInstance requests for classes of type CIM_ObjectManager and CIM_Provider
		// go to the ConfigurationManager.
		// ATTN: should check class inheritance, not just the name
		String className = request->modifiedInstance.getInstance().getClassName();
	
		if(String::equalNoCase(className, "CIM_ObjectManager") ||
           String::equalNoCase(className, "CIM_Provider"))
		{
			// ATTN: function under construction
			
			//_configurationManager->handleEnqueue(request);
	
			CIMResponseMessage * response = new CIMCreateInstanceResponseMessage(
				request->messageId,
				CIM_ERR_FAILED,
				"function under construction",
				request->queueIds.copyAndPop(),
				CIMReference());
	
			_enqueueResponse(request, response);
	
			return;
		}
		
		// ModifyInstance requests for classes of type CIM_IndicationSubscription
		// go to the Subscription Processor
		// ATTN: should check class inheritance, not just the name
		if(String::equalNoCase(className, "CIM_IndicationSubscription"))
		{
			// ATTN: function under construction
			
			//_subscriptionProcessor->handleEnqueue(request);
	
			CIMResponseMessage * response = new CIMCreateInstanceResponseMessage(
				request->messageId,
				CIM_ERR_FAILED,
				"function under construction",
				request->queueIds.copyAndPop(),
				CIMReference());
	
			_enqueueResponse(request, response);
	
			return;
		}
	}
	
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
	
	   _repository->write_lock();
	
	   try
	   {
	      _repository->modifyInstance(
		 request->nameSpace,
		 request->modifiedInstance,
		 request->includeQualifiers,
		 request->propertyList);
	   }

	   catch( ... )
	   {
	      _repository->write_unlock();
	      throw;
	   }

	   _repository->write_unlock();

        }
        else
	{
	    // attempt to load provider
		CIMBaseProvider * provider = _providerManager.getProvider(providerName, className);
		CIMBaseProviderFacade facade(provider);

	    SimpleResponseHandler<CIMInstance> handler;
		
		facade.modifyInstance(
		OperationContext(),
        request->nameSpace,
        request->modifiedInstance.getInstance(),
        OperationFlag::convert(false, request->includeQualifiers),
        Array<String>(), //request->propertyList,
		handler);
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
	   catch( ...)
	   {
	      _repository->read_unlock();
	      throw;
	   }
	
	   _repository->read_unlock();
	}
	
	else
	{
	    // attempt to load provider
	    CIMBaseProvider * provider = _providerManager.getProvider(providerName, className);
		CIMBaseProviderFacade facade(provider);

		SimpleResponseHandler<CIMInstance> handler;

	    facade.enumerateInstances(
		OperationContext(),
		request->className,
		OperationFlag::convert(request->localOnly, request->includeQualifiers, request->includeClassOrigin, request->deepInheritance),
		request->propertyList.getPropertyNameArray(),
		handler);

		//cimNamedInstances = CIMNamedInstance(CIMReference(), handler._objects[0]);
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
	   _repository->read_lock();
	   try
	   {
	      instanceNames = _repository->enumerateInstanceNames(
		 request->nameSpace,
		 request->className);
	   }
	   catch( ... )
	   {
	      _repository->read_unlock();
	      throw;
	   }
	
	   _repository->read_unlock();
	}

	else
	{
	    // attempt to load provider
	    CIMBaseProvider * provider = _providerManager.getProvider(providerName, className);
		CIMBaseProviderFacade facade(provider);

		SimpleResponseHandler<CIMReference> handler;

	    facade.enumerateInstanceNames(
		OperationContext(),
		request->className,
		handler);

		instanceNames = handler._objects;
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
	   catch( ... )
	   {
	      _repository->read_unlock();
	      throw;
	   }

	   _repository->read_unlock();
 	}
	else
	{
	    // attempt to load provider
	    CIMBaseProvider * provider = _providerManager.getProvider(providerName, className);
		CIMBaseProviderFacade facade(provider);

		SimpleResponseHandler<CIMObject> handler;

	    facade.associators(
		OperationContext(),
	    request->objectName,
	    request->assocClass,
	    request->resultClass,
	    request->role,
	    request->resultRole,
	    OperationFlag::convert(false, request->includeQualifiers, request->includeClassOrigin),
	    request->propertyList.getPropertyNameArray(),
		handler);

		//cimObjects = handler._objects;
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
	   catch( ... )
	   {
	      _repository->read_unlock();
	      throw;
	   }

	   _repository->read_unlock();
        }
	else
	{
	    // attempt to load provider
	    CIMBaseProvider * provider = _providerManager.getProvider(providerName, className);
		CIMBaseProviderFacade facade(provider);

		SimpleResponseHandler<CIMReference> handler;

	    facade.associatorNames(
		OperationContext(),
	    request->objectName,
	    request->assocClass,
	    request->resultClass,
	    request->role,
	    request->resultRole,
		handler);
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
	   catch( ... )
	   {
	      _repository->read_unlock();
	      throw;
	   }

	   _repository->read_unlock();
	}
	else
	{
	    // attempt to load provider
	    CIMBaseProvider * provider = _providerManager.getProvider(providerName, className);
		CIMBaseProviderFacade facade(provider);

		SimpleResponseHandler<CIMObject> handler;

	    facade.references(
		OperationContext(),
	        request->objectName,
	        request->resultClass,
	        request->role,
	        OperationFlag::convert(false, request->includeQualifiers, request->includeClassOrigin),
	        request->propertyList.getPropertyNameArray(),
		handler);

		//cimObjects = handler._objects;
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
	   _repository->read_lock();

	   try
	   {
	      objectNames = _repository->referenceNames(
		 request->nameSpace,
		 request->objectName,
		 request->resultClass,
		 request->role);
	   }
	   catch( ... )
	   {
	      _repository->read_unlock();
	      throw;
	   }

	   _repository->read_unlock();
        }
	else
	{
	   // attempt to load provider
	    CIMBaseProvider * provider = _providerManager.getProvider(providerName, className);
		CIMBaseProviderFacade facade(provider);

		SimpleResponseHandler<CIMReference> handler;
	
	   facade.referenceNames(
	      OperationContext(),
	        request->objectName,
	        request->resultClass,
	        request->role,
		  handler);
	   //objectNames =
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
	   _repository->read_lock();
	   try
	   {
	      value = _repository->getProperty(
		 request->nameSpace,
		 request->instanceName,
		 request->propertyName);
	   }
	   catch( ... )
	   {
	      _repository->read_unlock();
	      throw;
	   }
        }
        else
	{
	    // attempt to load provider
		CIMBaseProvider * provider = _providerManager.getProvider(providerName, className);
		CIMBaseProviderFacade facade(provider);

	    SimpleResponseHandler<CIMValue> handler;
		
		facade.getProperty(
		OperationContext(),
		request->instanceName,
		request->propertyName,
		handler);

		value = handler._objects[0];						
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
	   _repository->write_lock();

	   try
	   {
	      _repository->setProperty(
		 request->nameSpace,
		 request->instanceName,
		 request->propertyName,
		 request->newValue);
	   }
	   catch( ... )
	   {
	      _repository->write_unlock();
	      throw;
	   }

	   _repository->write_unlock();
        }
        else
	{
	    // attempt to load provider
		CIMBaseProvider * provider = _providerManager.getProvider(providerName, className);
		CIMBaseProviderFacade facade(provider);

		SimpleResponseHandler<CIMValue> handler;

	    facade.setProperty(
		OperationContext(),
		request->instanceName,
		request->propertyName,
		request->newValue,
		handler);
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

    _repository->read_lock();
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
	    CIMBaseProvider * provider = _providerManager.getProvider(providerName, className);
		CIMBaseProviderFacade facade(provider);

		SimpleResponseHandler<CIMValue> handler;

    	facade.invokeMethod(
		OperationContext(),
		request->instanceName,
		request->methodName,
		request->inParameters,
		outParameters,
		handler);

		retValue = 0;
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
            CIMBaseProvider * provider = _providerManager.getProvider
                (request->providerName, request->classNames [0]);

			CIMBaseProviderFacade facade(provider);

            SimpleResponseHandler<CIMIndication> responseHandler;

            /*
			facade.enableIndicationSubscription (
                //
                //  ATTN: pass thresholding parameter values in
                //  operation context
                //
                OperationContext (),
                request->classNames,
                request->propertyList,
                request->repeatNotificationPolicy,
                request->condition,
                request->queryLanguage,
                request->subscription,
                responseHandler);
			*/
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
            CIMBaseProvider * provider = _providerManager.getProvider
                (request->providerName, request->classNames [0]);

			CIMBaseProviderFacade facade(provider);

            SimpleResponseHandler<CIMIndication> responseHandler;

            /*
			facade.modifyIndicationSubscription (
                //
                //  ATTN: pass thresholding parameter values in
                //  operation context
                //
                OperationContext (),
                request->classNames,
                request->propertyList,
                request->repeatNotificationPolicy,
                request->condition,
                request->queryLanguage,
                request->subscription,
                responseHandler);
			*/
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
            CIMBaseProvider * provider = _providerManager.getProvider
                (request->providerName, request->classNames [0]);

			CIMBaseProviderFacade facade(provider);

            SimpleResponseHandler<CIMIndication> responseHandler;

            /*
			facade.disableIndicationSubscription (
                OperationContext (),
                request->classNames,
                request->subscription,
                responseHandler);
			*/
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


PEGASUS_NAMESPACE_END
