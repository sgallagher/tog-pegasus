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

#include <Pegasus/Common/XmlReader.h> // stringToValue(), stringArrayToValue()
#include <Pegasus/Common/Tracer.h>

PEGASUS_NAMESPACE_BEGIN

PEGASUS_USING_STD;

#define DDD(X) // X

DDD(static const char* _DISPATCHER = "CIMOperationRequestDispatcher::";)

CIMOperationRequestDispatcher::CIMOperationRequestDispatcher(
    CIMRepository* repository,
    ProviderRegistrationManager* providerRegistrationManager)
      :
      Base("CIMOpRequestDispatcher", MessageQueue::getNextQueueId()),
      _repository(repository),
      _providerRegistrationManager(providerRegistrationManager)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDispatcher::CIMOperationRequestDispatcher()");
   DDD(cout << _DISPATCHER << endl;)
   PEG_METHOD_EXIT();
}

CIMOperationRequestDispatcher::~CIMOperationRequestDispatcher(void)	
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDispatcher::~CIMOperationRequestDispatcher()");
   _dying = 1;
   PEG_METHOD_EXIT();
}

void CIMOperationRequestDispatcher::_handle_async_request(AsyncRequest *req)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDispatcher::_handle_async_request()");

   	// pass legacy operations to handleEnqueue
	if(req->getType() == async_messages::ASYNC_LEGACY_OP_START)
	{
	   req->op->processing();

	   Message * message = (static_cast<AsyncLegacyOperationStart *>(req)->get_action());

	   handleEnqueue(message);

	   PEG_METHOD_EXIT();
	   return;
	}

	// pass all other operations to the default handler
	Base::_handle_async_request(req);
	PEG_METHOD_EXIT();
}

Boolean CIMOperationRequestDispatcher::_lookupInternalProvider(
   const String& nameSpace,
   const String& className,
   String& service,
   String& provider)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::_lookupInternalProvider()");

    if (String::equalNoCase(className, "PG_ConfigSetting"))
    {
        service = "ModuleController";
        provider = "ModuleController::ConfigProvider";
        PEG_METHOD_EXIT();
        return true;
    }
    if (String::equalNoCase(className, "PG_Authorization") ||
        String::equalNoCase(className, "PG_User"))
    {
        service = "ModuleController";
        provider = "ModuleController::UserAuthProvider";
        PEG_METHOD_EXIT();
        return true;
    }
    if (String::equalNoCase(className, "PG_Provider") ||
        String::equalNoCase(className, "PG_ProviderCapabilities") ||
        String::equalNoCase(className, "PG_ProviderModule"))
    {
        service = "ModuleController";
        provider = "ModuleController::ProviderRegistrationProvider";
        PEG_METHOD_EXIT();
        return true;
    }
    if (String::equalNoCase(className, "PG_IndicationSubscription") ||
        String::equalNoCase(className, "PG_IndicationHandler") ||
        String::equalNoCase(className, "PG_IndicationHandlerCIMXML") ||
        String::equalNoCase(className, "PG_IndicationHandlerSNMPMapper") ||
        String::equalNoCase(className, "PG_IndicationFilter"))
    {
        service = "Server::IndicationService";
        provider = String::EMPTY;
        PEG_METHOD_EXIT();
        return true;
    }
    PEG_METHOD_EXIT();
    return false;
}

String CIMOperationRequestDispatcher::_lookupInstanceProvider(
   const String& nameSpace,
   const String& className)
{
    CIMInstance pInstance;
    CIMInstance pmInstance;
    String providerName;

    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
                     "CIMOperationRequestDispatcher::_lookupInstanceProvider");

    if (_providerRegistrationManager->lookupInstanceProvider(
	nameSpace, className, pInstance, pmInstance))
    {
	// get the provder name
	Uint32 pos = pInstance.findProperty("Name");

	if ( pos != PEG_NOT_FOUND )
	{
	    pInstance.getProperty(pos).getValue().get(providerName);

            PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                             "providerName = " + providerName + " found.");
            PEG_METHOD_EXIT();
	    return (providerName);
	}
	else
	{
            Tracer::trace(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                          "Provider name not found.");
            PEG_METHOD_EXIT();
   	    return(String::EMPTY);
	}
    }
    else
    {
        Tracer::trace(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                      "Provider not found.");
        PEG_METHOD_EXIT();
   	return(String::EMPTY);
    }
}

String CIMOperationRequestDispatcher::_lookupMethodProvider(
   const String& nameSpace,
   const String& className,
   const String& methodName)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "CIMOperationRequestDispatcher::_lookupMethodProvider()");

    CIMInstance pInstance;
    CIMInstance pmInstance;
    String providerName;

    if (_providerRegistrationManager->lookupMethodProvider(
	nameSpace, className, methodName, pInstance, pmInstance))
    {
	// get the provder name
	Uint32 pos = pInstance.findProperty("Name");

	if ( pos != PEG_NOT_FOUND )
	{
	    pInstance.getProperty(pos).getValue().get(providerName);

	    PEG_METHOD_EXIT();
	    return (providerName);
	}
	else
	{
	    PEG_METHOD_EXIT();
   	    return(String::EMPTY);
	}
    }
    else
    {
	PEG_METHOD_EXIT();
	return(String::EMPTY);
    }
}

// ATTN-YZ-P1-20020305: Implement this interface
String CIMOperationRequestDispatcher::_lookupIndicationProvider(
   const String& nameSpace,
   const String& className)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "CIMOperationRequestDispatcher::_lookupIndicationProvider()");

    PEG_METHOD_EXIT();
    return(String::EMPTY);
}

// ATTN-YZ-P1-20020305: Implement this interface
String CIMOperationRequestDispatcher::_lookupAssociationProvider(
   const String& nameSpace,
   const String& className)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "CIMOperationRequestDispatcher::_lookupAssociationProvider()");

    PEG_METHOD_EXIT();
    return(String::EMPTY);
}

void CIMOperationRequestDispatcher::_forwardRequestToService(
    const String& serviceName,
    CIMRequestMessage* request,
    CIMResponseMessage*& response)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::_forwardRequestToService()");

    Array<Uint32> serviceIds;
    find_services(serviceName, 0, 0, &serviceIds);
    PEGASUS_ASSERT(serviceIds.size() != 0);

    AsyncOpNode * op = this->get_op();

    AsyncLegacyOperationStart * asyncRequest =
        new AsyncLegacyOperationStart(
	    get_next_xid(),
	    op,
	    serviceIds[0],
	    request,
	    this->getQueueId());

    AsyncReply * asyncReply = SendWait(asyncRequest);
    PEGASUS_ASSERT(asyncReply != 0);

    response = reinterpret_cast<CIMResponseMessage *>
        ((static_cast<AsyncLegacyOperationResult *>(asyncReply))->get_result());
    PEGASUS_ASSERT(response != 0);

    delete asyncReply;
    delete asyncRequest;

    PEG_METHOD_EXIT();
}

void CIMOperationRequestDispatcher::_forwardRequestToControlProvider(
    const String& serviceName,
    const String& controlProviderName,
    CIMRequestMessage* request,
    CIMResponseMessage*& response)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::_forwardRequestToControlProvider()");

    Array<Uint32> serviceIds;
    find_services(serviceName, 0, 0, &serviceIds);
    PEGASUS_ASSERT(serviceIds.size() != 0);

    AsyncOpNode * op = this->get_op();

    AsyncModuleOperationStart * moduleControllerRequest =
        new AsyncModuleOperationStart(
	    get_next_xid(),
	    op,
	    serviceIds[0],
	    this->getQueueId(),
	    true,
	    controlProviderName,
	    request);

    AsyncReply * moduleControllerReply = SendWait(moduleControllerRequest);
    PEGASUS_ASSERT(moduleControllerReply != 0);

    response = reinterpret_cast<CIMResponseMessage *>
        ((static_cast<AsyncModuleOperationResult *>(moduleControllerReply))->
            get_result());
    PEGASUS_ASSERT(response != 0);

    delete moduleControllerReply;
    delete moduleControllerRequest;

    PEG_METHOD_EXIT();
}

void CIMOperationRequestDispatcher::_enqueueResponse(
   CIMRequestMessage* request,
   CIMResponseMessage* response)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDispatcher::_enqueueResponse()");

   // Use the same key as used in the request:

   response->setKey(request->getKey());

   if( true == Base::_enqueueResponse(request, response))
   {
      PEG_METHOD_EXIT();
      return;
   }
	
   MessageQueue * queue = MessageQueue::lookup(request->queueIds.top());
   PEGASUS_ASSERT(queue != 0 );
	
   queue->enqueue(response);

   PEG_METHOD_EXIT();
}


void CIMOperationRequestDispatcher::handleEnqueue(Message *request)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDispatcher::handleEnqueue(Message *request)");

   if(!request)
   {
      PEG_METHOD_EXIT();
      return;
   }
   	
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
   PEG_METHOD_EXIT();
}

// allocate a CIM Operation_async,  opnode, context, and response handler
// initialize with pointers to async top and async bottom
// link to the waiting q
void CIMOperationRequestDispatcher::handleEnqueue()
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDispatcher::handleEnqueue()");

   Message* request = dequeue();

   if(request)
      handleEnqueue(request);

   PEG_METHOD_EXIT();
}

const char* CIMOperationRequestDispatcher::getQueueName() const
{
   return(_name);
}

void CIMOperationRequestDispatcher::handleGetClassRequest(
   CIMGetClassRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDispatcher::handleGetClassRequest()");

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
   PEG_METHOD_EXIT();
}

void CIMOperationRequestDispatcher::handleGetInstanceRequest(
   CIMGetInstanceRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDispatcher::handleGetInstanceRequest()");

   // ATTN: Need code here to expand partial instance!

   // get the class name
   String className = request->instanceName.getClassName();
   CIMResponseMessage * response;

   String serviceName = String::EMPTY;
   String controlProviderName = String::EMPTY;

   // Check for class provided by an internal provider
   if (_lookupInternalProvider(request->nameSpace, className, serviceName,
           controlProviderName))
   {
      CIMGetInstanceRequestMessage* requestCopy =
         new CIMGetInstanceRequestMessage(*request);

      if (controlProviderName == String::EMPTY)
      {
         _forwardRequestToService(serviceName, requestCopy, response);
      }
      else
      {
         _forwardRequestToControlProvider(
            serviceName, controlProviderName, requestCopy, response);
      }

      _enqueueResponse(request, response);

      PEG_METHOD_EXIT();
      return;
   }

   // get provider for class
   String providerName = _lookupInstanceProvider(request->nameSpace, className);

   if(providerName.size() != 0)
   {
      CIMGetInstanceRequestMessage* requestCopy =
          new CIMGetInstanceRequestMessage(*request);

      _forwardRequestToService(
          String("Server::ProviderManagerService"), requestCopy, response);

      _enqueueResponse(request, response);

      PEG_METHOD_EXIT();
      return;
   }
   else if (_repository->isDefaultInstanceProvider())
   {
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

      CIMGetInstanceResponseMessage* response =
         new CIMGetInstanceResponseMessage(
            request->messageId,
            errorCode,
            errorDescription,
            request->queueIds.copyAndPop(),
            cimInstance);

      _enqueueResponse(request, response);
   }
   else // No provider is registered and the repository isn't the default
   {
      CIMGetInstanceResponseMessage* response =
         new CIMGetInstanceResponseMessage(
            request->messageId,
            CIM_ERR_NOT_SUPPORTED,
            CIMException(CIM_ERR_NOT_SUPPORTED).getMessage(),
            request->queueIds.copyAndPop(),
            CIMInstance());

      _enqueueResponse(request, response);
   }
   PEG_METHOD_EXIT();
}

void CIMOperationRequestDispatcher::handleDeleteClassRequest(
   CIMDeleteClassRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDispatcher::handleDeleteClassRequest()");

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

   PEG_METHOD_EXIT();
}

void CIMOperationRequestDispatcher::handleDeleteInstanceRequest(
   CIMDeleteInstanceRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDispatcher::handleDeleteInstanceRequest()");

   // get the class name
   String className = request->instanceName.getClassName();
   CIMResponseMessage * response;

   String serviceName = String::EMPTY;
   String controlProviderName = String::EMPTY;

   // Check for class provided by an internal provider
   if (_lookupInternalProvider(request->nameSpace, className, serviceName,
           controlProviderName))
   {
      CIMDeleteInstanceRequestMessage* requestCopy =
         new CIMDeleteInstanceRequestMessage(*request);

      if (controlProviderName == String::EMPTY)
      {
         _forwardRequestToService(serviceName, requestCopy, response);
      }
      else
      {
         _forwardRequestToControlProvider(
            serviceName, controlProviderName, requestCopy, response);
      }

      _enqueueResponse(request, response);

      PEG_METHOD_EXIT();
      return;
   }

   String providerName = _lookupInstanceProvider(request->nameSpace, className);

   if(providerName.size() != 0)
   {
      CIMDeleteInstanceRequestMessage* requestCopy =
          new CIMDeleteInstanceRequestMessage(*request);

      _forwardRequestToService(
          String("Server::ProviderManagerService"), requestCopy, response);

      _enqueueResponse(request, response);

      PEG_METHOD_EXIT();
      return;
   }
   else if (_repository->isDefaultInstanceProvider())
   {
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
   else // No provider is registered and the repository isn't the default
   {
      CIMDeleteInstanceResponseMessage* response =
         new CIMDeleteInstanceResponseMessage(
            request->messageId,
            CIM_ERR_NOT_SUPPORTED,
            CIMException(CIM_ERR_NOT_SUPPORTED).getMessage(),
            request->queueIds.copyAndPop());

      _enqueueResponse(request, response);
   }

   PEG_METHOD_EXIT();
}

void CIMOperationRequestDispatcher::handleCreateClassRequest(
   CIMCreateClassRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDispatcher::handleCreateClassRequest()");

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

   PEG_METHOD_EXIT();
}

void CIMOperationRequestDispatcher::handleCreateInstanceRequest(
   CIMCreateInstanceRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDispatcher::handleCreateInstanceRequest()");

   // get the class name
   String className = request->newInstance.getClassName();
   CIMResponseMessage * response;

   String serviceName = String::EMPTY;
   String controlProviderName = String::EMPTY;

   // Check for class provided by an internal provider
   if (_lookupInternalProvider(request->nameSpace, className, serviceName,
           controlProviderName))
   {
      CIMCreateInstanceRequestMessage* requestCopy =
         new CIMCreateInstanceRequestMessage(*request);

      if (controlProviderName == String::EMPTY)
      {
         _forwardRequestToService(serviceName, requestCopy, response);
      }
      else
      {
         _forwardRequestToControlProvider(
            serviceName, controlProviderName, requestCopy, response);
      }

      _enqueueResponse(request, response);

      PEG_METHOD_EXIT();
      return;
   }

   // ATTN: TEMP: Test code for ProcessIndication
   if ((className == "TestSoftwarePkg") ||
       (className == "nsatrap"))
   {
      CIMProcessIndicationRequestMessage* message =
	 new CIMProcessIndicationRequestMessage(
	    "1234",
	    request->nameSpace,
	    request->newInstance,
	    request->queueIds);
	
      //_indicationService.enqueue(message);
      //return;

      Array<Uint32> iService;

      find_services(String("Server::IndicationService"), 0, 0, &iService);

      AsyncOpNode* op = this->get_op();

      AsyncLegacyOperationStart *req =
	 new AsyncLegacyOperationStart(
	    get_next_xid(),
	    op,
	    iService[0],
	    message,
	    _queueId);
      //getQueueId());

      Boolean ret = SendForget(req);

      CIMCreateInstanceResponseMessage* response =
	 new CIMCreateInstanceResponseMessage(
	    request->messageId,
	    CIM_ERR_SUCCESS,
	    "",
	    request->queueIds.copyAndPop(),
	    CIMReference());

      _enqueueResponse(request, response);
      PEG_METHOD_EXIT();
      return;
   }
   // End test block

   String providerName = _lookupInstanceProvider(request->nameSpace, className);

   if(providerName.size() != 0)
   {
      CIMCreateInstanceRequestMessage* requestCopy =
          new CIMCreateInstanceRequestMessage(*request);

      _forwardRequestToService(
          String("Server::ProviderManagerService"), requestCopy, response);

      _enqueueResponse(request, response);

      PEG_METHOD_EXIT();
      return;
   }
   else if (_repository->isDefaultInstanceProvider())
   {
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
   else // No provider is registered and the repository isn't the default
   {
      CIMCreateInstanceResponseMessage* response =
         new CIMCreateInstanceResponseMessage(
            request->messageId,
            CIM_ERR_NOT_SUPPORTED,
            CIMException(CIM_ERR_NOT_SUPPORTED).getMessage(),
            request->queueIds.copyAndPop(),
            CIMReference());

      _enqueueResponse(request, response);
   }

   PEG_METHOD_EXIT();
}

void CIMOperationRequestDispatcher::handleModifyClassRequest(
   CIMModifyClassRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDispatcher::handleModifyClassRequest()");

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

   PEG_METHOD_EXIT();
}

void CIMOperationRequestDispatcher::handleModifyInstanceRequest(
   CIMModifyInstanceRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDispatcher::handleModifyInstanceRequest()");

   // ATTN: Who makes sure the instance name and the instance match?

   // get the class name
   String className = request->modifiedInstance.getInstance().getClassName();
   CIMResponseMessage * response;

   String serviceName = String::EMPTY;
   String controlProviderName = String::EMPTY;

   // Check for class provided by an internal provider
   if (_lookupInternalProvider(request->nameSpace, className, serviceName,
           controlProviderName))
   {
      CIMModifyInstanceRequestMessage* requestCopy =
         new CIMModifyInstanceRequestMessage(*request);

      if (controlProviderName == String::EMPTY)
      {
         _forwardRequestToService(serviceName, requestCopy, response);
      }
      else
      {
         _forwardRequestToControlProvider(
            serviceName, controlProviderName, requestCopy, response);
      }

      _enqueueResponse(request, response);

      PEG_METHOD_EXIT();
      return;
   }

   // check the class name for an "external provider"
   String providerName = _lookupInstanceProvider(request->nameSpace, className);

   if(providerName.size() != 0)
   {
      CIMModifyInstanceRequestMessage* requestCopy =
          new CIMModifyInstanceRequestMessage(*request);

      _forwardRequestToService(
          String("Server::ProviderManagerService"), requestCopy, response);

      _enqueueResponse(request, response);

      PEG_METHOD_EXIT();
      return;
   }
   else if (_repository->isDefaultInstanceProvider())
   {
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
   else // No provider is registered and the repository isn't the default
   {
      CIMModifyInstanceResponseMessage* response =
         new CIMModifyInstanceResponseMessage(
	    request->messageId,
            CIM_ERR_NOT_SUPPORTED,
            CIMException(CIM_ERR_NOT_SUPPORTED).getMessage(),
	    request->queueIds.copyAndPop());

      _enqueueResponse(request, response);
   }

   PEG_METHOD_EXIT();
}

void CIMOperationRequestDispatcher::handleEnumerateClassesRequest(
   CIMEnumerateClassesRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDispatcher::handleEnumerateClassesRequest()");

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

   PEG_METHOD_EXIT();
}

void CIMOperationRequestDispatcher::handleEnumerateClassNamesRequest(
   CIMEnumerateClassNamesRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDispatcher::handleEnumerateClassNamesRequest()");

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

   PEG_METHOD_EXIT();
}

void CIMOperationRequestDispatcher::handleEnumerateInstancesRequest(
   CIMEnumerateInstancesRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDispatcher::handleEnumerateInstancesRequest()");

   // get the class name
   String className = request->className;
   CIMResponseMessage * response;

   String serviceName = String::EMPTY;
   String controlProviderName = String::EMPTY;

   // Check for class provided by an internal provider
   if (_lookupInternalProvider(request->nameSpace, className, serviceName,
           controlProviderName))
   {
      CIMEnumerateInstancesRequestMessage* requestCopy =
         new CIMEnumerateInstancesRequestMessage(*request);

      if (controlProviderName == String::EMPTY)
      {
         _forwardRequestToService(serviceName, requestCopy, response);
      }
      else
      {
         _forwardRequestToControlProvider(
            serviceName, controlProviderName, requestCopy, response);
      }

      _enqueueResponse(request, response);

      PEG_METHOD_EXIT();
      return;
   }

   // check the class name for an "external provider"
   String providerName = _lookupInstanceProvider(request->nameSpace, className);

   if(providerName.size() != 0)
   {
      CIMEnumerateInstancesRequestMessage* requestCopy =
          new CIMEnumerateInstancesRequestMessage(*request);

      _forwardRequestToService(
          String("Server::ProviderManagerService"), requestCopy, response);

      _enqueueResponse(request, response);

      PEG_METHOD_EXIT();
      return;
   }
   else if (_repository->isDefaultInstanceProvider())
   {
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
   else // No provider is registered and the repository isn't the default
   {
      CIMEnumerateInstancesResponseMessage* response =
         new CIMEnumerateInstancesResponseMessage(
            request->messageId,
            CIM_ERR_NOT_SUPPORTED,
            CIMException(CIM_ERR_NOT_SUPPORTED).getMessage(),
            request->queueIds.copyAndPop(),
            Array<CIMNamedInstance>());

      _enqueueResponse(request, response);
   }

   PEG_METHOD_EXIT();
}

void CIMOperationRequestDispatcher::handleEnumerateInstanceNamesRequest(
   CIMEnumerateInstanceNamesRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDispatcher::handleEnumerateInstanceNamesRequest()");

   // get the class name
   String className = request->className;
   CIMResponseMessage * response;

   String serviceName = String::EMPTY;
   String controlProviderName = String::EMPTY;

   // Check for class provided by an internal provider
   if (_lookupInternalProvider(request->nameSpace, className, serviceName,
           controlProviderName))
   {
      CIMEnumerateInstanceNamesRequestMessage* requestCopy =
         new CIMEnumerateInstanceNamesRequestMessage(*request);

      if (controlProviderName == String::EMPTY)
      {
         _forwardRequestToService(serviceName, requestCopy, response);
      }
      else
      {
         _forwardRequestToControlProvider(
            serviceName, controlProviderName, requestCopy, response);
      }

      _enqueueResponse(request, response);

      PEG_METHOD_EXIT();
      return;
   }

   // check the class name for an "external provider"
   String providerName = _lookupInstanceProvider(request->nameSpace, className);

   if(providerName.size() != 0)
   {
      CIMEnumerateInstanceNamesRequestMessage* requestCopy =
          new CIMEnumerateInstanceNamesRequestMessage(*request);

      _forwardRequestToService(
          String("Server::ProviderManagerService"), requestCopy, response);

      _enqueueResponse(request, response);

      PEG_METHOD_EXIT();
      return;
   }
   else if (_repository->isDefaultInstanceProvider())
   {
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
   else // No provider is registered and the repository isn't the default
   {
      CIMEnumerateInstanceNamesResponseMessage* response =
         new CIMEnumerateInstanceNamesResponseMessage(
            request->messageId,
            CIM_ERR_NOT_SUPPORTED,
            CIMException(CIM_ERR_NOT_SUPPORTED).getMessage(),
            request->queueIds.copyAndPop(),
            Array<CIMReference>());

      _enqueueResponse(request, response);
   }

   PEG_METHOD_EXIT();
}

void CIMOperationRequestDispatcher::handleAssociatorsRequest(
   CIMAssociatorsRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDispatcher::handleAssociatorsRequest()");

   String className = request->objectName.getClassName();
   CIMResponseMessage * response;
	
   // check the class name for an "external provider"
   String providerName = _lookupAssociationProvider(request->nameSpace, className);

   if(providerName.size() != 0)
   {
      CIMAssociatorsRequestMessage* requestCopy =
          new CIMAssociatorsRequestMessage(*request);

      _forwardRequestToService(
          String("Server::ProviderManagerService"), requestCopy, response);

      _enqueueResponse(request, response);

      PEG_METHOD_EXIT();
      return;
   }
   else if (_repository->isDefaultInstanceProvider())
   {
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
   else // No provider is registered and the repository isn't the default
   {
      CIMAssociatorsResponseMessage* response =
         new CIMAssociatorsResponseMessage(
            request->messageId,
            CIM_ERR_NOT_SUPPORTED,
            CIMException(CIM_ERR_NOT_SUPPORTED).getMessage(),
            request->queueIds.copyAndPop(),
            Array<CIMObjectWithPath>());

      _enqueueResponse(request, response);
   }

   PEG_METHOD_EXIT();
}

void CIMOperationRequestDispatcher::handleAssociatorNamesRequest(
   CIMAssociatorNamesRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDispatcher::handleAssociatorNamesRequest()");

   String className = request->objectName.getClassName();
   CIMResponseMessage * response;
	
   // check the class name for an "external provider"
   String providerName = _lookupAssociationProvider(request->nameSpace, className);

   if(providerName.size() != 0)
   {
      CIMAssociatorNamesRequestMessage* requestCopy =
          new CIMAssociatorNamesRequestMessage(*request);

      _forwardRequestToService(
          String("Server::ProviderManagerService"), requestCopy, response);

      _enqueueResponse(request, response);

      PEG_METHOD_EXIT();
      return;
   }
   else if (_repository->isDefaultInstanceProvider())
   {
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
   else // No provider is registered and the repository isn't the default
   {
      CIMAssociatorNamesResponseMessage* response =
         new CIMAssociatorNamesResponseMessage(
            request->messageId,
            CIM_ERR_NOT_SUPPORTED,
            CIMException(CIM_ERR_NOT_SUPPORTED).getMessage(),
            request->queueIds.copyAndPop(),
            Array<CIMReference>());

      _enqueueResponse(request, response);
   }

   PEG_METHOD_EXIT();
}

void CIMOperationRequestDispatcher::handleReferencesRequest(
   CIMReferencesRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDispatcher::handleReferencesRequest()");

   String className = request->objectName.getClassName();
   CIMResponseMessage * response;
	
   // check the class name for an "external provider"
   String providerName = _lookupAssociationProvider(request->nameSpace, className);

   if(providerName.size() != 0)
   {
      CIMReferencesRequestMessage* requestCopy =
          new CIMReferencesRequestMessage(*request);

      _forwardRequestToService(
          String("Server::ProviderManagerService"), requestCopy, response);

      _enqueueResponse(request, response);

      PEG_METHOD_EXIT();
      return;
   }
   else if (_repository->isDefaultInstanceProvider())
   {
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
   else // No provider is registered and the repository isn't the default
   {
      CIMReferencesResponseMessage* response =
         new CIMReferencesResponseMessage(
            request->messageId,
            CIM_ERR_NOT_SUPPORTED,
            CIMException(CIM_ERR_NOT_SUPPORTED).getMessage(),
            request->queueIds.copyAndPop(),
            Array<CIMObjectWithPath>());

      _enqueueResponse(request, response);
   }

   PEG_METHOD_EXIT();
}

void CIMOperationRequestDispatcher::handleReferenceNamesRequest(
   CIMReferenceNamesRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDispatcher::handleReferenceNamesRequest()");

   String className = request->objectName.getClassName();
   CIMResponseMessage * response;
	
   // check the class name for an "external provider"
   String providerName = _lookupAssociationProvider(request->nameSpace, className);

   if(providerName.size() != 0)
   {
      CIMReferenceNamesRequestMessage* requestCopy =
          new CIMReferenceNamesRequestMessage(*request);

      _forwardRequestToService(
          String("Server::ProviderManagerService"), requestCopy, response);

      _enqueueResponse(request, response);

      PEG_METHOD_EXIT();
      return;
   }
   else if (_repository->isDefaultInstanceProvider())
   {
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
   else // No provider is registered and the repository isn't the default
   {
      CIMReferenceNamesResponseMessage* response =
         new CIMReferenceNamesResponseMessage(
            request->messageId,
            CIM_ERR_NOT_SUPPORTED,
            CIMException(CIM_ERR_NOT_SUPPORTED).getMessage(),
            request->queueIds.copyAndPop(),
            Array<CIMReference>());

      _enqueueResponse(request, response);
   }
   PEG_METHOD_EXIT();
}

void CIMOperationRequestDispatcher::handleGetPropertyRequest(
   CIMGetPropertyRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDispatcher::handleGetPropertyRequest()");

   String className = request->instanceName.getClassName();
   CIMResponseMessage * response;
	
   // check the class name for an "external provider"
   String providerName = _lookupInstanceProvider(request->nameSpace, className);

   if(providerName.size() != 0)
   {
      CIMGetPropertyRequestMessage* requestCopy =
          new CIMGetPropertyRequestMessage(*request);

      _forwardRequestToService(
          String("Server::ProviderManagerService"), requestCopy, response);

      _enqueueResponse(request, response);

      PEG_METHOD_EXIT();
      return;
   }
   else if (_repository->isDefaultInstanceProvider())
   {
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
   else // No provider is registered and the repository isn't the default
   {
      CIMGetPropertyResponseMessage* response =
         new CIMGetPropertyResponseMessage(
            request->messageId,
            CIM_ERR_NOT_SUPPORTED,
            CIMException(CIM_ERR_NOT_SUPPORTED).getMessage(),
            request->queueIds.copyAndPop(),
            CIMValue());

      _enqueueResponse(request, response);
   }
   PEG_METHOD_EXIT();
}

void CIMOperationRequestDispatcher::handleSetPropertyRequest(
   CIMSetPropertyRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDispatcher::handleSetPropertyRequest()");

   {
      CIMStatusCode errorCode = CIM_ERR_SUCCESS;
      String errorDescription;
      try
      {
         _fixSetPropertyValueType(request);
      }
      catch (CIMException& exception)
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

      if (errorCode != CIM_ERR_SUCCESS)
      {
         CIMSetPropertyResponseMessage* response =
            new CIMSetPropertyResponseMessage(
               request->messageId,
               errorCode,
               errorDescription,
               request->queueIds.copyAndPop());

         _enqueueResponse(request, response);

         PEG_METHOD_EXIT();
         return;
      }
   }

   String className = request->instanceName.getClassName();
   CIMResponseMessage * response;
	
   // check the class name for an "external provider"
   String providerName = _lookupInstanceProvider(request->nameSpace, className);

   if(providerName.size() != 0)
   {
      CIMSetPropertyRequestMessage* requestCopy =
          new CIMSetPropertyRequestMessage(*request);

      _forwardRequestToService(
          String("Server::ProviderManagerService"), requestCopy, response);

      _enqueueResponse(request, response);

      PEG_METHOD_EXIT();
      return;
   }
   else if (_repository->isDefaultInstanceProvider())
   {
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
   else // No provider is registered and the repository isn't the default
   {
      CIMSetPropertyResponseMessage* response =
         new CIMSetPropertyResponseMessage(
            request->messageId,
            CIM_ERR_NOT_SUPPORTED,
            CIMException(CIM_ERR_NOT_SUPPORTED).getMessage(),
            request->queueIds.copyAndPop());

      _enqueueResponse(request, response);
   }

   PEG_METHOD_EXIT();
}

void CIMOperationRequestDispatcher::handleGetQualifierRequest(
   CIMGetQualifierRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDispatcher::handleGetQualifierRequest()");

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

   PEG_METHOD_EXIT();
}

void CIMOperationRequestDispatcher::handleSetQualifierRequest(
   CIMSetQualifierRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDispatcher::handleSetQualifierRequest()");

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

   PEG_METHOD_EXIT();
}

void CIMOperationRequestDispatcher::handleDeleteQualifierRequest(
   CIMDeleteQualifierRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDispatcher::handleDeleteQualifierRequest()");

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

   PEG_METHOD_EXIT();
}

void CIMOperationRequestDispatcher::handleEnumerateQualifiersRequest(
   CIMEnumerateQualifiersRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDispatcher::handleEnumerateQualifiersRequest()");

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

   PEG_METHOD_EXIT();
}

void CIMOperationRequestDispatcher::handleInvokeMethodRequest(
   CIMInvokeMethodRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDispatcher::handleInvokeMethodRequest()");

   CIMResponseMessage * response;

   {
      CIMStatusCode errorCode = CIM_ERR_SUCCESS;
      String errorDescription;
      try
      {
         _fixInvokeMethodParameterTypes(request);
      }
      catch (CIMException& exception)
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

      if (errorCode != CIM_ERR_SUCCESS)
      {
         response =
            new CIMInvokeMethodResponseMessage(
               request->messageId,
               errorCode,
               errorDescription,
               request->queueIds.copyAndPop(),
               CIMValue(),
               Array<CIMParamValue>(),
               request->methodName);

         _enqueueResponse(request, response);

         PEG_METHOD_EXIT();
         return;
      }
   }


   String className = request->instanceName.getClassName();

   String serviceName = String::EMPTY;
   String controlProviderName = String::EMPTY;

   // Check for class provided by an internal provider
   if (_lookupInternalProvider(request->nameSpace, className, serviceName,
           controlProviderName))
   {
      CIMInvokeMethodRequestMessage* requestCopy =
         new CIMInvokeMethodRequestMessage(*request);

      if (controlProviderName == String::EMPTY)
      {
         _forwardRequestToService(serviceName, requestCopy, response);
      }
      else
      {
         _forwardRequestToControlProvider(
            serviceName, controlProviderName, requestCopy, response);
      }

      _enqueueResponse(request, response);

      PEG_METHOD_EXIT();
      return;
   }

   // check the class name for an "external provider"
   String providerName = _lookupMethodProvider(request->nameSpace,
			 className, request->methodName);

   if(providerName.size() != 0)
   {
      CIMInvokeMethodRequestMessage* requestCopy =
          new CIMInvokeMethodRequestMessage(*request);

      _forwardRequestToService(
          String("Server::ProviderManagerService"), requestCopy, response);

      _enqueueResponse(request, response);

      PEG_METHOD_EXIT();
      return;
   }
	
   CIMStatusCode errorCode = CIM_ERR_FAILED;
   String errorDescription = "Provider not available";
   CIMValue retValue(1);
   Array<CIMParamValue> outParameters;

   response =
      new CIMInvokeMethodResponseMessage(
	 request->messageId,
	 errorCode,
	 errorDescription,
	 request->queueIds.copyAndPop(),
	 retValue,
	 outParameters,
	 request->methodName);

   _enqueueResponse(request, response);

   PEG_METHOD_EXIT();
}

void CIMOperationRequestDispatcher::handleEnableIndicationSubscriptionRequest(
   CIMEnableIndicationSubscriptionRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDispatcher::"
          "handleEnableIndicationSubscriptionRequest()");

    //
    //  ATTN-CAKG-P3-20020326: This method to be removed - no longer used
    //

   PEG_METHOD_EXIT();
}

void CIMOperationRequestDispatcher::handleModifyIndicationSubscriptionRequest(
   CIMModifyIndicationSubscriptionRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDispatcher::"
          "handleModifyIndicationSubscriptionRequest()");

    //
    //  ATTN-CAKG-P3-20020326: This method to be removed - no longer used
    //

   PEG_METHOD_EXIT();
}

void CIMOperationRequestDispatcher::handleDisableIndicationSubscriptionRequest(
   CIMDisableIndicationSubscriptionRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDispatcher::"
          "handleDisableIndicationSubscriptionRequest()");

    //
    //  ATTN-CAKG-P3-20020326: This method to be removed - no longer used
    //

   PEG_METHOD_EXIT();
}

void CIMOperationRequestDispatcher::handleProcessIndicationRequest(
   CIMProcessIndicationRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDispatcher::handleProcessIndicationRequest()");

   //
   // forward request to IndicationService. IndicartionService will take care
   // of response to this request.
   //

   // lookup IndicationService
   MessageQueue * queue = MessageQueue::lookup("Server::IndicationService");

   PEGASUS_ASSERT(queue != 0);

   // forward to indication service. make a copy becuase the original request is
   // deleted by this service.
   queue->enqueue(new CIMProcessIndicationRequestMessage(*request));

   PEG_METHOD_EXIT();
   return;
}

/**
   Convert the specified CIMValue to the specified type, and return it in
   a new CIMValue.
*/
CIMValue CIMOperationRequestDispatcher::_convertValueType(
   const CIMValue& value,
   CIMType type)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDispatcher::_convertValueType()");

   CIMValue newValue;

   if (value.isArray())
   {
      Array<String> stringArray;
      Array<char*> charPtrArray;
      Array<const char*> constCharPtrArray;

      //
      // Convert the value to Array<const char*> to send to conversion method
      //
      // ATTN-RK-P3-20020221: Deal with TypeMismatch exception
      // (Shouldn't really ever get that exception)
      value.get(stringArray);

      for (Uint32 k=0; k<stringArray.size(); k++)
      {
	 // Need to build an Array<const char*> to send to the conversion
	 // routine, but also need to keep track of them pointers as char*
	 // because Windows won't let me delete a const char*.
	 char* charPtr = stringArray[k].allocateCString();
	 charPtrArray.append(charPtr);
	 constCharPtrArray.append(charPtr);
      }

      //
      // Convert the value to the specified type
      //
      try
      {
	 newValue = XmlReader::stringArrayToValue(0, constCharPtrArray, type);
      }
      catch (XmlSemanticError e)
      {
         PEG_METHOD_EXIT();
	 throw PEGASUS_CIM_EXCEPTION(
	    CIM_ERR_INVALID_PARAMETER,
	    String("Malformed ") + TypeToString(type) +
	    "value");
      }

      for (Uint32 k=0; k<charPtrArray.size(); k++)
      {
	 delete charPtrArray[k];
      }
   }
   else
   {
      String stringValue;

      // ATTN-RK-P3-20020221: Deal with TypeMismatch exception
      // (Shouldn't really ever get that exception)
      value.get(stringValue);

      try
      {
	 newValue = XmlReader::stringToValue(0, _CString(stringValue), type);
      }
      catch (XmlSemanticError e)
      {
         PEG_METHOD_EXIT();
	 throw PEGASUS_CIM_EXCEPTION(
	    CIM_ERR_INVALID_PARAMETER,
	    String("Malformed ") + TypeToString(type) +
	    "value");
      }
   }

   PEG_METHOD_EXIT();
   return newValue;
}

/**
   Find the CIMParamValues in the InvokeMethod request whose types were
   not specified in the XML encoding, and convert them to the types
   specified in the method schema.
*/
void CIMOperationRequestDispatcher::_fixInvokeMethodParameterTypes(
   CIMInvokeMethodRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDispatcher::_fixInvokeMethodParameterTypes()");

   Boolean gotMethodDefinition = false;
   CIMMethod method;

    //
    // Cycle through the input parameters, converting the untyped ones.
    //
    Array<CIMParamValue> inParameters = request->inParameters;
    Uint32 numInParamValues = inParameters.size();
    for (Uint32 i=0; i<numInParamValues; i++)
    {
        if (!inParameters[i].isTyped())
        {
            //
            // Retrieve the method definition, if we haven't already done so
            // (only look up the method if we have an untyped parameter value)
            //
            if (!gotMethodDefinition)
            {
                //
                // Get the class definition for this method
                //
                CIMClass cimClass;
                _repository->read_lock();
                try
                {
                    cimClass = _repository->getClass(
                        request->nameSpace,
                        request->instanceName.getClassName(),
                        false, //localOnly,
                        false, //includeQualifiers,
                        false, //includeClassOrigin,
                        CIMPropertyList());
                }
                catch (CIMException& exception)
                {
                    _repository->read_unlock();
                    PEG_METHOD_EXIT();
                    throw exception;
                }
                catch (Exception& exception)
                {
                    _repository->read_unlock();
                    PEG_METHOD_EXIT();
                    throw exception;
                }
                catch (...)
                {
                    _repository->read_unlock();
                    PEG_METHOD_EXIT();
                    throw CIMException(CIM_ERR_FAILED);
                }
                _repository->read_unlock();

                //
                // Get the method definition from the class
                //
                Uint32 methodPos = cimClass.findMethod(request->methodName);
                if (methodPos == PEG_NOT_FOUND)
                {
                    throw CIMException(CIM_ERR_METHOD_NOT_FOUND);
                }
                method = cimClass.getMethod(methodPos);

                gotMethodDefinition = true;
            }

            //
            // Find the parameter definition for this input parameter
            //
            String paramName = inParameters[i].getParameterName();
            Uint32 numParams = method.getParameterCount();
            for (Uint32 j=0; j<numParams; j++)
            {
                CIMParameter param = method.getParameter(j);
                if (paramName == param.getName())
                {
                    //
                    // Retype the input parameter value according to the
                    // type defined in the class/method schema
                    //
                    CIMType paramType = param.getType();
                    CIMValue newValue;

                    if (inParameters[i].getValue().isNull())
                    {
                        newValue.setNullValue(param.getType(), param.isArray());
                    }
                    else if (inParameters[i].getValue().isArray() !=
                                 param.isArray())
                    {
                        // ATTN-RK-P1-20020222: Who catches this?  They aren't.
                        PEG_METHOD_EXIT();
                        throw CIMException(CIM_ERR_TYPE_MISMATCH);
                    }
                    else
                    {
                        newValue = _convertValueType(inParameters[i].getValue(),
                            paramType);
                    }

                    inParameters[i].setValue(newValue);
                    inParameters[i].setIsTyped(true);
                    break;
                }
            }
        }
    }

    PEG_METHOD_EXIT();
}

/**
   Convert the CIMValue given in a SetProperty request to the correct
   type according to the schema, because it is not possible to specify
   the property type in the XML encoding.
*/
void CIMOperationRequestDispatcher::_fixSetPropertyValueType(
   CIMSetPropertyRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDispatcher::_fixSetPropertyValueType()");

   CIMValue inValue = request->newValue;

   //
   // Only do the conversion if the type is not already set
   //
   if ((inValue.getType() != CIMType::STRING) &&
       (inValue.getType() != CIMType::NONE))
   {
      PEG_METHOD_EXIT();
      return;
   }

   //
   // Get the class definition for this property
   //
   CIMClass cimClass;
   _repository->read_lock();
   try
   {
      cimClass = _repository->getClass(
         request->nameSpace,
         request->instanceName.getClassName(),
         false, //localOnly,
         false, //includeQualifiers,
         false, //includeClassOrigin,
         CIMPropertyList());
   }
   catch (CIMException& exception)
   {
      _repository->read_unlock();
      PEG_METHOD_EXIT();
      throw exception;
   }
   catch (Exception& exception)
   {
      _repository->read_unlock();
      PEG_METHOD_EXIT();
      throw exception;
   }
   catch (...)
   {
      _repository->read_unlock();
      PEG_METHOD_EXIT();
      throw CIMException(CIM_ERR_FAILED);
   }
   _repository->read_unlock();

   //
   // Get the property definition from the class
   //
   Uint32 propertyPos = cimClass.findProperty(request->propertyName);
   if (propertyPos == PEG_NOT_FOUND)
   {
      PEG_METHOD_EXIT();
      throw CIMException(CIM_ERR_NO_SUCH_PROPERTY);
   }
   CIMProperty property = cimClass.getProperty(propertyPos);

   //
   // Retype the input property value according to the
   // type defined in the schema
   //
   CIMValue newValue;

   if (inValue.isNull())
   {
      newValue.setNullValue(property.getType(), property.isArray());
   }
   else if (inValue.isArray() != property.isArray())
   {
      // ATTN-RK-P1-20020222: Who catches this?  They aren't.
      PEG_METHOD_EXIT();
      throw CIMException(CIM_ERR_TYPE_MISMATCH);
   }
   else
   {
      newValue = _convertValueType(inValue, property.getType());
   }

   //
   // Put the retyped value back into the message
   //
   request->newValue = newValue;

   PEG_METHOD_EXIT();
}

PEGASUS_NAMESPACE_END
