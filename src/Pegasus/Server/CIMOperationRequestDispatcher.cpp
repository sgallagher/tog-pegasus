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
    ProviderRegistrationManager* providerRegistrationManager,
    CIMServer* server)
      :
      Base("CIMOpRequestDispatcher", MessageQueue::getNextQueueId()),
      _repository(repository),
      _providerRegistrationManager(providerRegistrationManager),
      _cimom(this, server, repository),
      _configurationManager(_cimom)
{
   DDD(cout << _DISPATCHER << endl;)
}

CIMOperationRequestDispatcher::~CIMOperationRequestDispatcher(void)	
{
   _dying = 1;
}

void CIMOperationRequestDispatcher::_handle_async_request(AsyncRequest *req)
{
   	// pass legacy operations to handleEnqueue
	if(req->getType() == async_messages::ASYNC_LEGACY_OP_START)
	{
	   req->op->processing();

	   Message * message = (static_cast<AsyncLegacyOperationStart *>(req)->get_action());

	   handleEnqueue(message);

	   return;
	}

	// pass all other operations to the default handler
	Base::_handle_async_request(req);
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

	    return (providerName);
	}
	else
	{
   	    return(String::EMPTY);
	}
    }
    else
    {
   	return(String::EMPTY);
    }
}

// ATTN-YZ-P1-20020305: Implement this interface
String CIMOperationRequestDispatcher::_lookupIndicationProvider(
   const String& nameSpace,
   const String& className)
{
    return(String::EMPTY);
}

// ATTN-YZ-P1-20020305: Implement this interface
String CIMOperationRequestDispatcher::_lookupAssociationProvider(
   const String& nameSpace,
   const String& className)
{
    return(String::EMPTY);
}

void CIMOperationRequestDispatcher::_enqueueResponse(
   CIMRequestMessage* request,
   CIMResponseMessage* response)
{
   // Use the same key as used in the request:

   response->setKey(request->getKey());

   if( true == Base::_enqueueResponse(request, response))
      return;
	
   MessageQueue * queue = MessageQueue::lookup(request->queueIds.top());
   PEGASUS_ASSERT(queue != 0 );
	
   queue->enqueue(response);
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
   // ATTN: Need code here to expand partial instance!

   // get the class name
   String className = request->instanceName.getClassName();

   // check the class name for an "internal provider"
   if(String::equalNoCase(className, "PG_Provider") ||
      String::equalNoCase(className, "PG_ProviderCapabilities") ||
      String::equalNoCase(className, "PG_ProviderModule"))
   {
       Array<Uint32> serviceIds;

       find_services(String("Server::ConfigurationManagerQueue"), 0, 0, &serviceIds);

       PEGASUS_ASSERT(serviceIds.size() != 0);

       AsyncOpNode * op = this->get_op();

       AsyncLegacyOperationStart * asyncRequest =
	       new AsyncLegacyOperationStart(
		       get_next_xid(),
		       op,
		       serviceIds[0],
		       new CIMGetInstanceRequestMessage(*request),
		       this->getQueueId());

       AsyncReply * asyncReply = SendWait(asyncRequest);

       PEGASUS_ASSERT(asyncReply != 0);

       CIMResponseMessage * response =
	       reinterpret_cast<CIMGetInstanceResponseMessage *>
	       ((static_cast<AsyncLegacyOperationResult *>(asyncReply))->get_result());

       PEGASUS_ASSERT(response != 0);

       _enqueueResponse(request, response);

       delete asyncReply;
       delete asyncRequest;

       return;
   }

    //
    // check the class name for subscription, filter and handler
    //
    if(String::equalNoCase(className, "PG_IndicationSubscription") ||
       String::equalNoCase(className, "PG_IndicationHandlerCIMXML") ||
       String::equalNoCase(className, "PG_IndicationHandlerSNMP") ||
       String::equalNoCase(className, "PG_IndicationFilter"))
    {
        //
        //  Send to the indication service
        //
        Array <Uint32> iService;

        find_services (String ("Server::IndicationService"), 0, 0, &iService);

        AsyncOpNode * op = this->get_op ();

        AsyncLegacyOperationStart * req =
            new AsyncLegacyOperationStart (
                get_next_xid (),
                op,
                iService [0],
                new CIMGetInstanceRequestMessage (*request),
                this->getQueueId ());

        AsyncReply* reply = SendWait (req);
        CIMGetInstanceResponseMessage * response =
            reinterpret_cast <CIMGetInstanceResponseMessage *>
            ((static_cast <AsyncLegacyOperationResult *>
            (reply))->get_result ());

        _enqueueResponse (request, response);

        delete reply;
        delete req;
        return;
    }

   // get provider for class
   String providerName = _lookupInstanceProvider(request->nameSpace, className);

   if(providerName.size() != 0)
   {
       Array<Uint32> serviceIds;

       find_services(String("Server::ProviderManagerService"), 0, 0, &serviceIds);

       PEGASUS_ASSERT(serviceIds.size() != 0);

       AsyncOpNode * op = this->get_op();

       AsyncLegacyOperationStart * asyncRequest =
	       new AsyncLegacyOperationStart(
		       get_next_xid(),
		       op,
		       serviceIds[0],
		       new CIMGetInstanceRequestMessage(*request),
		       this->getQueueId());

       AsyncReply * asyncReply = SendWait(asyncRequest);

       PEGASUS_ASSERT(asyncReply != 0);

       CIMResponseMessage * response =
	       reinterpret_cast<CIMGetInstanceResponseMessage *>
	       ((static_cast<AsyncLegacyOperationResult *>(asyncReply))->get_result());

       PEGASUS_ASSERT(response != 0);

       _enqueueResponse(request, response);

       delete asyncReply;
       delete asyncRequest;

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
   if(String::equalNoCase(className, "PG_Provider") ||
      String::equalNoCase(className, "PG_ProviderCapabilities") ||
      String::equalNoCase(className, "PG_ProviderModule"))
   {
       Array<Uint32> serviceIds;

       find_services(String("Server::ConfigurationManagerQueue"), 0, 0, &serviceIds);

       PEGASUS_ASSERT(serviceIds.size() != 0);

       AsyncOpNode * op = this->get_op();

       AsyncLegacyOperationStart * asyncRequest =
	       new AsyncLegacyOperationStart(
		       get_next_xid(),
		       op,
		       serviceIds[0],
		       new CIMDeleteInstanceRequestMessage(*request),
		       this->getQueueId());

       AsyncReply * asyncReply = SendWait(asyncRequest);

       PEGASUS_ASSERT(asyncReply != 0);

       CIMResponseMessage * response =
	       reinterpret_cast<CIMDeleteInstanceResponseMessage *>
	       ((static_cast<AsyncLegacyOperationResult *>(asyncReply))->get_result());

       PEGASUS_ASSERT(response != 0);

       _enqueueResponse(request, response);

       delete asyncReply;
       delete asyncRequest;

       return;
   }

    //
    // check the class name for subscription, filter and handler
    //
    if(String::equalNoCase(className, "PG_IndicationSubscription") ||
       String::equalNoCase(className, "PG_IndicationHandlerCIMXML") ||
       String::equalNoCase(className, "PG_IndicationHandlerSNMP") ||
       String::equalNoCase(className, "PG_IndicationFilter"))
    {
        //
        //  Send to the indication service
        //
        Array <Uint32> iService;

        find_services (String ("Server::IndicationService"), 0, 0, &iService);

        AsyncOpNode * op = this->get_op ();

        AsyncLegacyOperationStart * req =
            new AsyncLegacyOperationStart (
                get_next_xid (),
                op,
                iService [0],
                new CIMDeleteInstanceRequestMessage (*request),
                this->getQueueId ());

        AsyncReply* reply = SendWait (req);
        CIMDeleteInstanceResponseMessage * response =
            reinterpret_cast <CIMDeleteInstanceResponseMessage *>
            ((static_cast <AsyncLegacyOperationResult *>
            (reply))->get_result ());

        _enqueueResponse (request, response);

        delete reply;
        delete req;
        return;
    }

   String providerName = _lookupInstanceProvider(request->nameSpace, className);

   if(providerName.size() != 0)
   {
       Array<Uint32> serviceIds;

       find_services(String("Server::ProviderManagerService"), 0, 0, &serviceIds);

       PEGASUS_ASSERT(serviceIds.size() != 0);

       AsyncOpNode * op = this->get_op();

       AsyncLegacyOperationStart * asyncRequest =
	       new AsyncLegacyOperationStart(
		       get_next_xid(),
		       op,
		       serviceIds[0],
		       new CIMDeleteInstanceRequestMessage(*request),
		       this->getQueueId());

       AsyncReply * asyncReply = SendWait(asyncRequest);

       PEGASUS_ASSERT(asyncReply != 0);

       CIMResponseMessage * response =
	       reinterpret_cast<CIMDeleteInstanceResponseMessage *>
	       ((static_cast<AsyncLegacyOperationResult *>(asyncReply))->get_result());

       PEGASUS_ASSERT(response != 0);

       _enqueueResponse(request, response);

       delete asyncReply;
       delete asyncRequest;

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
   if(String::equalNoCase(className, "PG_Provider") ||
      String::equalNoCase(className, "PG_ProviderCapabilities") ||
      String::equalNoCase(className, "PG_ProviderModule"))
   {
       Array<Uint32> serviceIds;

       find_services(String("Server::ConfigurationManagerQueue"), 0, 0, &serviceIds);

       PEGASUS_ASSERT(serviceIds.size() != 0);

       AsyncOpNode * op = this->get_op();

       AsyncLegacyOperationStart * asyncRequest =
	       new AsyncLegacyOperationStart(
		       get_next_xid(),
		       op,
		       serviceIds[0],
		       new CIMCreateInstanceRequestMessage(*request),
		       this->getQueueId());

       AsyncReply * asyncReply = SendWait(asyncRequest);

       PEGASUS_ASSERT(asyncReply != 0);

       CIMResponseMessage * response =
	       reinterpret_cast<CIMCreateInstanceResponseMessage *>
	       ((static_cast<AsyncLegacyOperationResult *>(asyncReply))->get_result());

       PEGASUS_ASSERT(response != 0);

       _enqueueResponse(request, response);

       delete asyncReply;
       delete asyncRequest;

       return;
   }

   //
   // check the class name for subscription, filter and handler
   //

   if(String::equalNoCase(className, "PG_IndicationSubscription") ||
      String::equalNoCase(className, "PG_IndicationHandlerCIMXML") ||
      String::equalNoCase(className, "PG_IndicationHandlerSNMP") ||
      String::equalNoCase(className, "PG_IndicationFilter"))
   {
      Array<Uint32> iService;

      find_services(String("Server::IndicationService"), 0, 0, &iService);

      AsyncOpNode* op = this->get_op();

      AsyncLegacyOperationStart *req =
	 new AsyncLegacyOperationStart(
	    get_next_xid(),
	    op,
	    iService[0],
	    new CIMCreateInstanceRequestMessage(*request),
	    this->getQueueId());

      AsyncReply* reply = SendWait(req);
      CIMCreateInstanceResponseMessage * response =
	 reinterpret_cast<CIMCreateInstanceResponseMessage *>
	 ((static_cast<AsyncLegacyOperationResult *>(reply))->get_result());

      _enqueueResponse(request, response);

      delete reply;
//      delete response;
// << Tue Feb 26 18:41:38 2002 mdd >>
      delete req;
      return;
   }

   // TEMP: Test code for ProcessIndication
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
      return;
   }
   // End test block

   String providerName = _lookupInstanceProvider(request->nameSpace, className);

   if(providerName.size() != 0)
   {
       Array<Uint32> serviceIds;

       find_services(String("Server::ProviderManagerService"), 0, 0, &serviceIds);

       PEGASUS_ASSERT(serviceIds.size() != 0);

       AsyncOpNode * op = this->get_op();

       AsyncLegacyOperationStart * asyncRequest =
	       new AsyncLegacyOperationStart(
		       get_next_xid(),
		       op,
		       serviceIds[0],
		       new CIMCreateInstanceRequestMessage(*request),
		       this->getQueueId());

       AsyncReply * asyncReply = SendWait(asyncRequest);

       PEGASUS_ASSERT(asyncReply != 0);

       CIMResponseMessage * response =
	       reinterpret_cast<CIMCreateInstanceResponseMessage *>
	       ((static_cast<AsyncLegacyOperationResult *>(asyncReply))->get_result());

       PEGASUS_ASSERT(response != 0);

       _enqueueResponse(request, response);

       delete asyncReply;
       delete asyncRequest;

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
   if(String::equalNoCase(className, "PG_Provider") ||
      String::equalNoCase(className, "PG_ProviderCapabilities") ||
      String::equalNoCase(className, "PG_ProviderModule"))
   {
       Array<Uint32> serviceIds;

       find_services(String("Server::ConfigurationManagerQueue"), 0, 0, &serviceIds);

       PEGASUS_ASSERT(serviceIds.size() != 0);

       AsyncOpNode * op = this->get_op();

       AsyncLegacyOperationStart * asyncRequest =
	       new AsyncLegacyOperationStart(
		       get_next_xid(),
		       op,
		       serviceIds[0],
		       new CIMModifyInstanceRequestMessage(*request),
		       this->getQueueId());

       AsyncReply * asyncReply = SendWait(asyncRequest);

       PEGASUS_ASSERT(asyncReply != 0);

       CIMResponseMessage * response =
	       reinterpret_cast<CIMModifyInstanceResponseMessage *>
	       ((static_cast<AsyncLegacyOperationResult *>(asyncReply))->get_result());

       PEGASUS_ASSERT(response != 0);

       _enqueueResponse(request, response);

       delete asyncReply;
       delete asyncRequest;

       return;
   }

    //
    // check the class name for subscription, filter and handler
    //
    if(String::equalNoCase(className, "PG_IndicationSubscription") ||
       String::equalNoCase(className, "PG_IndicationHandlerCIMXML") ||
       String::equalNoCase(className, "PG_IndicationHandlerSNMP") ||
       String::equalNoCase(className, "PG_IndicationFilter"))
    {
        //
        //  Send to the indication service
        //
        Array <Uint32> iService;

        find_services (String ("Server::IndicationService"), 0, 0, &iService);

        AsyncOpNode * op = this->get_op ();

        AsyncLegacyOperationStart * req =
            new AsyncLegacyOperationStart (
                get_next_xid (),
                op,
                iService [0],
                new CIMModifyInstanceRequestMessage (*request),
                this->getQueueId ());

        AsyncReply* reply = SendWait (req);
        CIMModifyInstanceResponseMessage * response =
            reinterpret_cast <CIMModifyInstanceResponseMessage *>
            ((static_cast <AsyncLegacyOperationResult *>
            (reply))->get_result ());

        _enqueueResponse (request, response);

        delete reply;
        delete req;
        return;
    }

   // check the class name for an "external provider"
   String providerName = _lookupInstanceProvider(request->nameSpace, className);

   if(providerName.size() != 0)
   {
       Array<Uint32> serviceIds;

       find_services(String("Server::ProviderManagerService"), 0, 0, &serviceIds);

       PEGASUS_ASSERT(serviceIds.size() != 0);

       AsyncOpNode * op = this->get_op();

       AsyncLegacyOperationStart * asyncRequest =
	       new AsyncLegacyOperationStart(
		       get_next_xid(),
		       op,
		       serviceIds[0],
		       new CIMModifyInstanceRequestMessage(*request),
		       this->getQueueId());

       AsyncReply * asyncReply = SendWait(asyncRequest);

       PEGASUS_ASSERT(asyncReply != 0);

       CIMResponseMessage * response =
	       reinterpret_cast<CIMModifyInstanceResponseMessage *>
	       ((static_cast<AsyncLegacyOperationResult *>(asyncReply))->get_result());

       PEGASUS_ASSERT(response != 0);

       _enqueueResponse(request, response);

       delete asyncReply;
       delete asyncRequest;

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
   if(String::equalNoCase(className, "PG_Provider") ||
      String::equalNoCase(className, "PG_ProviderCapabilities") ||
      String::equalNoCase(className, "PG_ProviderModule"))
   {
       Array<Uint32> serviceIds;

       find_services(String("Server::ConfigurationManagerQueue"), 0, 0, &serviceIds);

       PEGASUS_ASSERT(serviceIds.size() != 0);

       AsyncOpNode * op = this->get_op();

       AsyncLegacyOperationStart * asyncRequest =
	       new AsyncLegacyOperationStart(
		       get_next_xid(),
		       op,
		       serviceIds[0],
		       new CIMEnumerateInstancesRequestMessage(*request),
		       this->getQueueId());

       AsyncReply * asyncReply = SendWait(asyncRequest);

       PEGASUS_ASSERT(asyncReply != 0);

       CIMResponseMessage * response =
	       reinterpret_cast<CIMEnumerateInstancesResponseMessage *>
	       ((static_cast<AsyncLegacyOperationResult *>(asyncReply))->get_result());

       PEGASUS_ASSERT(response != 0);

       _enqueueResponse(request, response);

       delete asyncReply;
       delete asyncRequest;

       return;
   }

    //
    // check the class name for subscription, filter and handler
    //
    if(String::equalNoCase(className, "PG_IndicationSubscription") ||
       String::equalNoCase(className, "PG_IndicationHandler") ||
       String::equalNoCase(className, "PG_IndicationHandlerCIMXML") ||
       String::equalNoCase(className, "PG_IndicationHandlerSNMP") ||
       String::equalNoCase(className, "PG_IndicationFilter"))
    {
        //
        //  Send to the indication service
        //
        Array <Uint32> iService;

        find_services (String ("Server::IndicationService"), 0, 0, &iService);

        AsyncOpNode * op = this->get_op ();

        AsyncLegacyOperationStart * req =
            new AsyncLegacyOperationStart (
                get_next_xid (),
                op,
                iService [0],
                new CIMEnumerateInstancesRequestMessage (*request),
                this->getQueueId ());

        AsyncReply* reply = SendWait (req);
        CIMEnumerateInstancesResponseMessage * response =
            reinterpret_cast <CIMEnumerateInstancesResponseMessage *>
            ((static_cast <AsyncLegacyOperationResult *>
            (reply))->get_result ());

        _enqueueResponse (request, response);

        delete reply;
        delete req;
        return;
    }

   // check the class name for an "external provider"
   String providerName = _lookupInstanceProvider(request->nameSpace, className);

   if(providerName.size() != 0)
   {
       Array<Uint32> serviceIds;

       find_services(String("Server::ProviderManagerService"), 0, 0, &serviceIds);

       PEGASUS_ASSERT(serviceIds.size() != 0);

       AsyncOpNode * op = this->get_op();

       AsyncLegacyOperationStart * asyncRequest =
	       new AsyncLegacyOperationStart(
		       get_next_xid(),
		       op,
		       serviceIds[0],
		       new CIMEnumerateInstancesRequestMessage(*request),
		       this->getQueueId());

       AsyncReply * asyncReply = SendWait(asyncRequest);

       PEGASUS_ASSERT(asyncReply != 0);

       CIMResponseMessage * response =
	       reinterpret_cast<CIMEnumerateInstancesResponseMessage *>
	       ((static_cast<AsyncLegacyOperationResult *>(asyncReply))->get_result());

       PEGASUS_ASSERT(response != 0);

       _enqueueResponse(request, response);

       delete asyncReply;
       delete asyncRequest;

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
}

void CIMOperationRequestDispatcher::handleEnumerateInstanceNamesRequest(
   CIMEnumerateInstanceNamesRequestMessage* request)
{
   // get the class name
   String className = request->className;

   // check the class name for an "internal provider"
   if(String::equalNoCase(className, "PG_Provider") ||
      String::equalNoCase(className, "PG_ProviderCapabilities") ||
      String::equalNoCase(className, "PG_ProviderModule"))
   {
       Array<Uint32> serviceIds;

       find_services(String("Server::ConfigurationManagerQueue"), 0, 0, &serviceIds);

       PEGASUS_ASSERT(serviceIds.size() != 0);

       AsyncOpNode * op = this->get_op();

       AsyncLegacyOperationStart * asyncRequest =
	       new AsyncLegacyOperationStart(
		       get_next_xid(),
		       op,
		       serviceIds[0],
		       new CIMEnumerateInstanceNamesRequestMessage(*request),
		       this->getQueueId());

       AsyncReply * asyncReply = SendWait(asyncRequest);

       PEGASUS_ASSERT(asyncReply != 0);

       CIMResponseMessage * response =
	       reinterpret_cast<CIMEnumerateInstanceNamesResponseMessage *>
	       ((static_cast<AsyncLegacyOperationResult *>(asyncReply))->get_result());

       PEGASUS_ASSERT(response != 0);

       _enqueueResponse(request, response);

       delete asyncReply;
       delete asyncRequest;

       return;
   }

    //
    // check the class name for subscription, filter and handler
    //
    if(String::equalNoCase(className, "PG_IndicationSubscription") ||
       String::equalNoCase(className, "PG_IndicationHandler") ||
       String::equalNoCase(className, "PG_IndicationHandlerCIMXML") ||
       String::equalNoCase(className, "PG_IndicationHandlerSNMP") ||
       String::equalNoCase(className, "PG_IndicationFilter"))
    {
        //
        //  Send to the indication service
        //
        Array <Uint32> iService;

        find_services (String ("Server::IndicationService"), 0, 0, &iService);

        AsyncOpNode * op = this->get_op ();

        AsyncLegacyOperationStart * req =
            new AsyncLegacyOperationStart (
                get_next_xid (),
                op,
                iService [0],
                new CIMEnumerateInstanceNamesRequestMessage (*request),
                this->getQueueId ());

        AsyncReply* reply = SendWait (req);
        CIMEnumerateInstanceNamesResponseMessage * response =
            reinterpret_cast <CIMEnumerateInstanceNamesResponseMessage *>
            ((static_cast <AsyncLegacyOperationResult *>
            (reply))->get_result ());

        _enqueueResponse (request, response);

        delete reply;
        delete req;
        return;
    }

   // check the class name for an "external provider"
   String providerName = _lookupInstanceProvider(request->nameSpace, className);

   if(providerName.size() != 0)
   {
       Array<Uint32> serviceIds;

       find_services(String("Server::ProviderManagerService"), 0, 0, &serviceIds);

       PEGASUS_ASSERT(serviceIds.size() != 0);

       AsyncOpNode * op = this->get_op();

       AsyncLegacyOperationStart * asyncRequest =
	       new AsyncLegacyOperationStart(
		       get_next_xid(),
		       op,
		       serviceIds[0],
		       new CIMEnumerateInstanceNamesRequestMessage(*request),
		       this->getQueueId());

       AsyncReply * asyncReply = SendWait(asyncRequest);

       PEGASUS_ASSERT(asyncReply != 0);

       CIMResponseMessage * response =
	       reinterpret_cast<CIMEnumerateInstanceNamesResponseMessage *>
	       ((static_cast<AsyncLegacyOperationResult *>(asyncReply))->get_result());

       PEGASUS_ASSERT(response != 0);

       _enqueueResponse(request, response);

       delete asyncReply;
       delete asyncRequest;

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
}

void CIMOperationRequestDispatcher::handleAssociatorsRequest(
   CIMAssociatorsRequestMessage* request)
{
   String className = request->objectName.getClassName();
	
   // check the class name for an "external provider"
   String providerName = _lookupAssociationProvider(request->nameSpace, className);

   if(providerName.size() != 0)
   {
       Array<Uint32> serviceIds;

       find_services(String("Server::ProviderManagerService"), 0, 0, &serviceIds);

       PEGASUS_ASSERT(serviceIds.size() != 0);

       AsyncOpNode * op = this->get_op();

       AsyncLegacyOperationStart * asyncRequest =
	       new AsyncLegacyOperationStart(
		       get_next_xid(),
		       op,
		       serviceIds[0],
		       new CIMAssociatorsRequestMessage(*request),
		       this->getQueueId());

       AsyncReply * asyncReply = SendWait(asyncRequest);

       PEGASUS_ASSERT(asyncReply != 0);

       CIMResponseMessage * response =
	       reinterpret_cast<CIMAssociatorNamesResponseMessage *>
	       ((static_cast<AsyncLegacyOperationResult *>(asyncReply))->get_result());

       PEGASUS_ASSERT(response != 0);

       _enqueueResponse(request, response);

       delete asyncReply;
       delete asyncRequest;

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
}

void CIMOperationRequestDispatcher::handleAssociatorNamesRequest(
   CIMAssociatorNamesRequestMessage* request)
{
   String className = request->objectName.getClassName();
	
   // check the class name for an "external provider"
   String providerName = _lookupAssociationProvider(request->nameSpace, className);

   if(providerName.size() != 0)
   {
       Array<Uint32> serviceIds;

       find_services(String("Server::ProviderManagerService"), 0, 0, &serviceIds);

       PEGASUS_ASSERT(serviceIds.size() != 0);

       AsyncOpNode * op = this->get_op();

       AsyncLegacyOperationStart * asyncRequest =
	       new AsyncLegacyOperationStart(
		       get_next_xid(),
		       op,
		       serviceIds[0],
		       new CIMAssociatorNamesRequestMessage(*request),
		       this->getQueueId());

       AsyncReply * asyncReply = SendWait(asyncRequest);

       PEGASUS_ASSERT(asyncReply != 0);

       CIMResponseMessage * response =
	       reinterpret_cast<CIMAssociatorNamesResponseMessage *>
	       ((static_cast<AsyncLegacyOperationResult *>(asyncReply))->get_result());

       PEGASUS_ASSERT(response != 0);

       _enqueueResponse(request, response);

       delete asyncReply;
       delete asyncRequest;

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
}

void CIMOperationRequestDispatcher::handleReferencesRequest(
   CIMReferencesRequestMessage* request)
{
   String className = request->objectName.getClassName();
	
   // check the class name for an "external provider"
   String providerName = _lookupAssociationProvider(request->nameSpace, className);

   if(providerName.size() != 0)
   {
       Array<Uint32> serviceIds;

       find_services(String("Server::ProviderManagerService"), 0, 0, &serviceIds);

       PEGASUS_ASSERT(serviceIds.size() != 0);

       AsyncOpNode * op = this->get_op();

       AsyncLegacyOperationStart * asyncRequest =
	       new AsyncLegacyOperationStart(
		       get_next_xid(),
		       op,
		       serviceIds[0],
		       new CIMReferencesRequestMessage(*request),
		       this->getQueueId());

       AsyncReply * asyncReply = SendWait(asyncRequest);

       PEGASUS_ASSERT(asyncReply != 0);

       CIMResponseMessage * response =
	       reinterpret_cast<CIMReferencesResponseMessage *>
	       ((static_cast<AsyncLegacyOperationResult *>(asyncReply))->get_result());

       PEGASUS_ASSERT(response != 0);

       _enqueueResponse(request, response);

       delete asyncReply;
       delete asyncRequest;

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
}

void CIMOperationRequestDispatcher::handleReferenceNamesRequest(
   CIMReferenceNamesRequestMessage* request)
{
   String className = request->objectName.getClassName();
	
   // check the class name for an "external provider"
   String providerName = _lookupAssociationProvider(request->nameSpace, className);

   if(providerName.size() != 0)
   {
       Array<Uint32> serviceIds;

       find_services(String("Server::ProviderManagerService"), 0, 0, &serviceIds);

       PEGASUS_ASSERT(serviceIds.size() != 0);

       AsyncOpNode * op = this->get_op();

       AsyncLegacyOperationStart * asyncRequest =
	       new AsyncLegacyOperationStart(
		       get_next_xid(),
		       op,
		       serviceIds[0],
		       new CIMReferenceNamesRequestMessage(*request),
		       this->getQueueId());

       AsyncReply * asyncReply = SendWait(asyncRequest);

       PEGASUS_ASSERT(asyncReply != 0);

       CIMResponseMessage * response =
	       reinterpret_cast<CIMReferenceNamesResponseMessage *>
	       ((static_cast<AsyncLegacyOperationResult *>(asyncReply))->get_result());

       PEGASUS_ASSERT(response != 0);

       _enqueueResponse(request, response);

       delete asyncReply;
       delete asyncRequest;

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
}

void CIMOperationRequestDispatcher::handleGetPropertyRequest(
   CIMGetPropertyRequestMessage* request)
{
   String className = request->instanceName.getClassName();
	
   // check the class name for an "external provider"
   String providerName = _lookupInstanceProvider(request->nameSpace, className);

   if(providerName.size() != 0)
   {
       Array<Uint32> serviceIds;

       find_services(String("Server::ProviderManagerService"), 0, 0, &serviceIds);

       PEGASUS_ASSERT(serviceIds.size() != 0);

       AsyncOpNode * op = this->get_op();

       AsyncLegacyOperationStart * asyncRequest =
	       new AsyncLegacyOperationStart(
		       get_next_xid(),
		       op,
		       serviceIds[0],
		       new CIMGetPropertyRequestMessage(*request),
		       this->getQueueId());

       AsyncReply * asyncReply = SendWait(asyncRequest);

       PEGASUS_ASSERT(asyncReply != 0);

       CIMResponseMessage * response =
	       reinterpret_cast<CIMGetPropertyResponseMessage *>
	       ((static_cast<AsyncLegacyOperationResult *>(asyncReply))->get_result());

       PEGASUS_ASSERT(response != 0);

       _enqueueResponse(request, response);

       delete asyncReply;
       delete asyncRequest;

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
}

void CIMOperationRequestDispatcher::handleSetPropertyRequest(
   CIMSetPropertyRequestMessage* request)
{
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
      }
   }

   String className = request->instanceName.getClassName();
	
   // check the class name for an "external provider"
   String providerName = _lookupInstanceProvider(request->nameSpace, className);

   if(providerName.size() != 0)
   {
       Array<Uint32> serviceIds;

       find_services(String("Server::ProviderManagerService"), 0, 0, &serviceIds);

       PEGASUS_ASSERT(serviceIds.size() != 0);

       AsyncOpNode * op = this->get_op();

       AsyncLegacyOperationStart * asyncRequest =
	       new AsyncLegacyOperationStart(
		       get_next_xid(),
		       op,
		       serviceIds[0],
		       new CIMSetPropertyRequestMessage(*request),
		       this->getQueueId());

       AsyncReply * asyncReply = SendWait(asyncRequest);

       PEGASUS_ASSERT(asyncReply != 0);

       CIMResponseMessage * response =
	       reinterpret_cast<CIMSetPropertyResponseMessage *>
	       ((static_cast<AsyncLegacyOperationResult *>(asyncReply))->get_result());

       PEGASUS_ASSERT(response != 0);

       _enqueueResponse(request, response);

       delete asyncReply;
       delete asyncRequest;

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
         CIMInvokeMethodResponseMessage* response =
            new CIMInvokeMethodResponseMessage(
               request->messageId,
               errorCode,
               errorDescription,
               request->queueIds.copyAndPop(),
               CIMValue(),
               Array<CIMParamValue>(),
               request->methodName);

         _enqueueResponse(request, response);
      }
   }


   String className = request->instanceName.getClassName();
	
   // check the class name for an "external provider"
   String providerName = _lookupMethodProvider(request->nameSpace,
			 className, request->methodName);

   if(providerName.size() != 0)
   {
       Array<Uint32> serviceIds;

       find_services(String("Server::ProviderManagerService"), 0, 0, &serviceIds);

       PEGASUS_ASSERT(serviceIds.size() != 0);

       AsyncOpNode * op = this->get_op();

       AsyncLegacyOperationStart * asyncRequest =
	       new AsyncLegacyOperationStart(
		       get_next_xid(),
		       op,
		       serviceIds[0],
		       new CIMInvokeMethodRequestMessage(*request),
		       this->getQueueId());

       AsyncReply * asyncReply = SendWait(asyncRequest);

       PEGASUS_ASSERT(asyncReply != 0);

       CIMResponseMessage * response =
	       reinterpret_cast<CIMInvokeMethodResponseMessage *>
	       ((static_cast<AsyncLegacyOperationResult *>(asyncReply))->get_result());

       PEGASUS_ASSERT(response != 0);

       _enqueueResponse(request, response);

       delete asyncReply;
       delete asyncRequest;

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
   String providerName = _lookupIndicationProvider(
      request->nameSpace, request->classNames[0]);

   if(providerName.size() != 0)
   {
       Array<Uint32> serviceIds;

       find_services(String("Server::ProviderManagerService"), 0, 0, &serviceIds);

       PEGASUS_ASSERT(serviceIds.size() != 0);

       AsyncOpNode * op = this->get_op();

       AsyncLegacyOperationStart * asyncRequest =
	       new AsyncLegacyOperationStart(
		       get_next_xid(),
		       op,
		       serviceIds[0],
		       new CIMEnableIndicationSubscriptionRequestMessage(*request),
		       this->getQueueId());

       AsyncReply * asyncReply = SendWait(asyncRequest);

       PEGASUS_ASSERT(asyncReply != 0);

       CIMResponseMessage * response =
	       reinterpret_cast<CIMEnableIndicationSubscriptionResponseMessage *>
	       ((static_cast<AsyncLegacyOperationResult *>(asyncReply))->get_result());

       PEGASUS_ASSERT(response != 0);

       _enqueueResponse(request, response);

       delete asyncReply;
       delete asyncRequest;

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
   String providerName = _lookupIndicationProvider(
      request->nameSpace, request->classNames[0]);

   if(providerName.size() != 0)
   {
       Array<Uint32> serviceIds;

       find_services(String("Server::ProviderManagerService"), 0, 0, &serviceIds);

       PEGASUS_ASSERT(serviceIds.size() != 0);

       AsyncOpNode * op = this->get_op();

       AsyncLegacyOperationStart * asyncRequest =
	       new AsyncLegacyOperationStart(
		       get_next_xid(),
		       op,
		       serviceIds[0],
		       new CIMModifyIndicationSubscriptionRequestMessage(*request),
		       this->getQueueId());

       AsyncReply * asyncReply = SendWait(asyncRequest);

       PEGASUS_ASSERT(asyncReply != 0);

       CIMResponseMessage * response =
	       reinterpret_cast<CIMModifyIndicationSubscriptionResponseMessage *>
	       ((static_cast<AsyncLegacyOperationResult *>(asyncReply))->get_result());

       PEGASUS_ASSERT(response != 0);

       _enqueueResponse(request, response);

       delete asyncReply;
       delete asyncRequest;

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
   String providerName = _lookupIndicationProvider(
      request->nameSpace, request->classNames[0]);

   if(providerName.size() != 0)
   {
       Array<Uint32> serviceIds;

       find_services(String("Server::ProviderManagerService"), 0, 0, &serviceIds);

       PEGASUS_ASSERT(serviceIds.size() != 0);

       AsyncOpNode * op = this->get_op();

       AsyncLegacyOperationStart * asyncRequest =
	       new AsyncLegacyOperationStart(
		       get_next_xid(),
		       op,
		       serviceIds[0],
		       new CIMDisableIndicationSubscriptionRequestMessage(*request),
		       this->getQueueId());

       AsyncReply * asyncReply = SendWait(asyncRequest);

       PEGASUS_ASSERT(asyncReply != 0);

       CIMResponseMessage * response =
	       reinterpret_cast<CIMDisableIndicationSubscriptionResponseMessage *>
	       ((static_cast<AsyncLegacyOperationResult *>(asyncReply))->get_result());

       PEGASUS_ASSERT(response != 0);

       _enqueueResponse(request, response);

       delete asyncReply;
       delete asyncRequest;

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

   // lookup IndicationService
   MessageQueue * queue = MessageQueue::lookup("Server::IndicationService");

   PEGASUS_ASSERT(queue != 0);

   // forward to indication service. make a copy becuase the original request is
   // deleted by this service.
   queue->enqueue(new CIMProcessIndicationRequestMessage(*request));

   return;
}

ProviderRegistrationManager* CIMOperationRequestDispatcher::getProviderRegistrationManager(void)
{
    return _providerRegistrationManager;
}

/**
   Convert the specified CIMValue to the specified type, and return it in
   a new CIMValue.
*/
CIMValue CIMOperationRequestDispatcher::_convertValueType(
   const CIMValue& value,
   CIMType type)
{
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
	 throw PEGASUS_CIM_EXCEPTION(
	    CIM_ERR_INVALID_PARAMETER,
	    String("Malformed ") + TypeToString(type) +
	    "value");
      }
   }

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
                    throw exception;
                }
                catch (Exception& exception)
                {
                    _repository->read_unlock();
                    throw exception;
                }
                catch (...)
                {
                    _repository->read_unlock();
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
}

/**
   Convert the CIMValue given in a SetProperty request to the correct
   type according to the schema, because it is not possible to specify
   the property type in the XML encoding.
*/
void CIMOperationRequestDispatcher::_fixSetPropertyValueType(
   CIMSetPropertyRequestMessage* request)
{
   CIMValue inValue = request->newValue;

   //
   // Only do the conversion if the type is not already set
   //
   if ((inValue.getType() != CIMType::STRING) &&
       (inValue.getType() != CIMType::NONE))
   {
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
      throw exception;
   }
   catch (Exception& exception)
   {
      _repository->read_unlock();
      throw exception;
   }
   catch (...)
   {
      _repository->read_unlock();
      throw CIMException(CIM_ERR_FAILED);
   }
   _repository->read_unlock();

   //
   // Get the property definition from the class
   //
   Uint32 propertyPos = cimClass.findProperty(request->propertyName);
   if (propertyPos == PEG_NOT_FOUND)
   {
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
}

PEGASUS_NAMESPACE_END
