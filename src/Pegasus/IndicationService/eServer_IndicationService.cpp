//%///////////////////////-*-c++-*-/////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software, Hewlett-Packard Company, IBM,
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
// Author: Mike Day (mdday@us.ibm.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/CIMDateTime.h>
#include <Pegasus/Common/CIMProperty.h>
#include <Pegasus/Common/HashTable.h>
#include <Pegasus/Common/MessageQueue.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/PegasusVersion.h>
#include <Pegasus/Repository/CIMRepository.h>
#include <Pegasus/Common/String.h>
//#include <Pegasus/Provider/OperationFlag.h>
#include <Pegasus/Server/ProviderRegistrationManager/ProviderRegistrationManager.h>
#include "eServer_IndicationService.h"

PEGASUS_USING_STD;


PEGASUS_NAMESPACE_BEGIN

Mutex eServerIndicationService::_mutex;

eServerIndicationService::eServerIndicationService(
   CIMRepository *repository,
   ProviderRegistrationManager *providerRegManager)
   : Base(PEGASUS_QUEUENAME_ESERVER_INDICATIONSERVICE,
	  MessageQueue::getNextQueueId()),
     _repository(repository),
     _providerRegManager(providerRegManager)
{
   try 
   {
      _initialize();
   }
   catch(...)
   {
   }
   
}

eServerIndicationService::~eServerIndicationService(void)
{
}




void eServerIndicationService::handleEnqueue(void)
{
   Message * message = dequeue();

   PEGASUS_ASSERT(message != 0);
   handleEnqueue(message);
}

void eServerIndicationService::handleEnqueue(Message* message)
{


	  
   switch(message->getType())
   {
      case CIM_GET_INSTANCE_REQUEST_MESSAGE:
	 try 
	 {
	    _handleGetInstanceRequest(message);
	 }
	 catch( ... ) 
	 {

	    ;
	 }
	 
	 break;

      case CIM_ENUMERATE_INSTANCES_REQUEST_MESSAGE:
	 try 
	 {
	    _handleEnumerateInstancesRequest(message);
	 }
	 catch( ... ) 
	 {

	    ;
	 }
	 
	 break;

      case CIM_ENUMERATE_INSTANCE_NAMES_REQUEST_MESSAGE:
	 try 
	 {
	    
	    _handleEnumerateInstanceNamesRequest(message);
	 }
	 catch( ... ) 
	 {

	    ;
	 }
	 
	 break;

      case CIM_CREATE_INSTANCE_REQUEST_MESSAGE:
	 try 
	 {
	    
	    _handleCreateInstanceRequest(message);
	 }
	 catch( ... ) 
	 {

	    ;
	 }
	 break;

      case CIM_MODIFY_INSTANCE_REQUEST_MESSAGE:
	 try 
	 {
	    
	    _handleModifyInstanceRequest(message);
	 }
	 catch( ... ) 
	 {

	    ;
	 }
	 break;

      case CIM_DELETE_INSTANCE_REQUEST_MESSAGE:
	 try 
	 {
	    _handleDeleteInstanceRequest(message);
	 }
	 catch( ... ) 
	 {

	    ;
	 }
	 break;

      case CIM_PROCESS_INDICATION_REQUEST_MESSAGE:
	 try 
	 {
	    _handleProcessIndicationRequest(message);
	 }
	 catch( ... ) 
	 {

	    ;
	 }
	 break;

      default:
	 //
	 //  A message type not supported by the Indication Service
	 //  Should not reach here
	 //
	 // << Mon Apr 29 16:29:10 2002 mdd >>
	 PEG_TRACE_STRING(TRC_INDICATION_SERVICE, Tracer::LEVEL3, 
			  "IndicationService::handleEnqueue(msg *) rcv'd unsupported msg " 
			  + String(MessageTypeToString(message->getType())));
	 break;
   }
	  
   delete message;
}


// << Mon Dec  2 15:46:39 2002 mdd >> 
// NOTE: fix the provider manager so it NEVER unloads a 
// provider that is referred to by a subscription. Such a 
// provider could be an indication provider or an 
// indication consumer.

// <<< Tue Dec  3 14:19:16 2002 mdd >>>
// NOTE: add a message to the provider manager service to 
// deliver an indication to a consumer provider. 


void eServerIndicationService::_handle_async_request(AsyncRequest *req)
{
   if ( req->getType() == async_messages::CIMSERVICE_STOP )
   {
      req->op->processing();

      handle_CimServiceStop(static_cast<CimServiceStop *>(req));

      //
      //  Call _terminate
      //
      _terminate ();


   }
   else if (req->getType () == async_messages::CIMSERVICE_START)
   {
      req->op->processing ();

      handle_CimServiceStart (static_cast <CimServiceStart *> (req));
   }
   else if ( req->getType() == async_messages::ASYNC_LEGACY_OP_START )
   {
      try 
      {
	  
	 req->op->processing();
	 Message *legacy = 
	    (static_cast<AsyncLegacyOperationStart *>(req)->get_action());
	 legacy->put_async(req);
	  
	 handleEnqueue(legacy);
      }
      catch(Exception & )
      {
	 PEG_TRACE_STRING(TRC_INDICATION_SERVICE, Tracer::LEVEL3, 
			  "Caught Exception in IndicationService while handling a wrapped legacy  message ");
	 _make_response(req, async_results::CIM_NAK );
      }
       
      return;
   }
   else
      Base::_handle_async_request(req);
}



void eServerIndicationService::_initialize(void)
{
   // find the provider manager service

   Array<Uint32> provider_manager;
   find_services(PEGASUS_QUEUENAME_PROVIDERMANAGER_CPP, 0, 0, &provider_manager);
   // assumes only one provider manager service, which MUST be present 
   PEGASUS_ASSERT(provider_manager.size() == 1);
   _providerManager = provider_manager[0];
   

   // read persistent subscriptions from the repository

    Array <CIMInstance> activeSubscriptions;
    activeSubscriptions.clear();
    _getActiveSubscriptions(activeSubscriptions);
    // elements in activeSubscriptions have their name space 
    // as part of the path. 

    _create_enable_subscription(activeSubscriptions);
}


void eServerIndicationService::_terminate(void)
{

}



void eServerIndicationService::_handleGetInstanceRequest(const Message *message)
{
   PEG_METHOD_ENTER(TRC_INDICATION_SERVICE,
		    "eServerIndicationService::_handleGetInstanceRequest");
   CIMGetInstanceRequestMessage* request = 
      static_cast<CIMGetInstanceRequestMessage*>(const_cast<Message *>(message));
   
   CIMException cimException;
   CIMInstance instance;
   try
   {
      _repository->read_lock();

      try
      {
	 
	 instance = _repository->getInstance (request->nameSpace, 
					      request->instanceName, request->localOnly, 
					      request->includeQualifiers, request->includeClassOrigin, 
					      request->propertyList);
	 
	 _repository->read_unlock();
      }
      catch (CIMException& exception)
      {
	 cimException = exception;
      }
      catch (Exception& exception)
      {
	 cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
					      exception.getMessage());
      }
      catch(...)
      {
	 cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
					      "Internal Error");
      }

      _repository->read_unlock();
   }
   catch(...)
   {
      cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
					   "Error locking repository");
   }
   

   CIMGetInstanceResponseMessage* response = new CIMGetInstanceResponseMessage
      (request->messageId,
       cimException,
       request->queueIds.copyAndPop(),
       instance);
   
   //
   //  Preserve message key
   //
   response->setKey (request->getKey ());
   
   //
   //  Set response destination
   //
   response->dest = request->queueIds.top ();
   
   Base::_enqueueResponse (request, response);
   
   PEG_METHOD_EXIT ();
}


void eServerIndicationService::_handleEnumerateInstancesRequest(const Message * message)
{

   PEG_METHOD_ENTER(TRC_INDICATION_SERVICE, 
		    "eServerIndicationService::_handleEnumerateInstancesRequest");
   
   CIMEnumerateInstancesRequestMessage* request = 
      static_cast<CIMEnumerateInstancesRequestMessage*>(const_cast<Message *>(message));
   
   Array <CIMInstance> enumInstances;
   
   CIMException cimException;
   CIMInstance cimInstance;
   
   try 
   {
      _repository->read_lock();

      try 
      {
	 enumInstances = _repository->enumerateInstances (request->nameSpace, 
							  request->className, 
							  request->deepInheritance, 
							  request->localOnly, 
							  request->includeQualifiers, 
							  request->includeClassOrigin, 
							  request->propertyList);
      }
      catch (CIMException& exception)
      {
	 cimException = exception;
      }
      catch (Exception& exception)
      {
	 cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
					      exception.getMessage());
      }
      catch(...)
      {
	 cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
					      "Internal Error");
      }
      _repository->read_unlock();
   }
   catch(...)
   {
      cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
					   "Error locking repository");
   }

   CIMEnumerateInstancesResponseMessage* response = 
      new CIMEnumerateInstancesResponseMessage(
	 request->messageId,
	 cimException,
	 request->queueIds.copyAndPop(),
	 enumInstances);
   
    //
    //  Preserve message key
    //
   response->setKey (request->getKey ());
   
   //
   //  Set response destination
   //
   response->dest = request->queueIds.top ();
   
   Base::_enqueueResponse (request, response);
   
   PEG_METHOD_EXIT ();
   
}

void eServerIndicationService::_handleEnumerateInstanceNamesRequest(const Message * message)
{
   PEG_METHOD_ENTER(TRC_INDICATION_SERVICE,
		    "eServerIndicationService::_handleEnumerateInstanceNamesRequest");
   
   CIMEnumerateInstanceNamesRequestMessage* request =
      (CIMEnumerateInstanceNamesRequestMessage*) message;
   
   Array<CIMObjectPath> enumInstanceNames;
   
   CIMException cimException;
   
   try
   {
      _repository->read_lock();
      
      try 
      {
	 enumInstanceNames = _repository->enumerateInstanceNames 
            (request->nameSpace, request->className);
      }
      catch (CIMException& exception)
      {
	 cimException = exception;
      }
      catch (Exception& exception)
      {
	 cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
					      exception.getMessage());
      }
      catch(...)
      {
	 cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
					      "Internal Error");
      }
      _repository->read_unlock();
   }
   catch(...)
   {
      cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
					   "Error locking repository");
   }
   
   CIMEnumerateInstanceNamesResponseMessage* response =
      new CIMEnumerateInstanceNamesResponseMessage(
	 request->messageId,
	 cimException,
	 request->queueIds.copyAndPop(),
	 enumInstanceNames);
    
   // preserve message key
   response->setKey(request->getKey());
    
   //  Set the response destination !!!
   response->dest = request->queueIds.top();
    
   Base::_enqueueResponse(request, response);

   PEG_METHOD_EXIT();
}


void eServerIndicationService::_handleCreateInstanceRequest(const Message * message)
{
   
   PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
		     "eServerIndicationService::_handleCreateInstanceRequest");

   
   CIMCreateInstanceRequestMessage* request = 
      static_cast<CIMCreateInstanceRequestMessage *>(const_cast<Message *>(message));
   
   CIMException cimException;
   
   CIMObjectPath instanceRef;

   CIMInstance instance = request->newInstance.clone ();

   // check to ensure it is a subscription we can create 
   if(instance.getClassName() == ESERVER_CLASSNAME_INDSUBSCRIPTION)
   {
      // check to ensure required properties exist
//      if( instance.existsProperty("Indication_Class") && instance.existsProperty("Handler"))
      if(instance.findProperty("Indication_Class") != PEG_NOT_FOUND && 
	 instance.findProperty("Handler") != PEG_NOT_FOUND)
      {

	 try
	 {

	 //
	 //  Create instance in repository
	 //
	    _repository->write_lock ();
	    
	    try 
	    {
	       instanceRef = _repository->createInstance (request->nameSpace, 
							  instance);
	    }
	    catch (CIMException & exception)
	    {
	       cimException = exception;
	    }
	    catch (Exception & exception)
	    {
	       cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
						    exception.getMessage());
	    }
	    catch(...)
	    {
	       cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
						    "Internal error");
	    }
	    
	    _repository->write_unlock ();
	    
	    if (cimException.getCode() != CIM_ERR_SUCCESS)
	    {
	       CIMCreateInstanceResponseMessage* response =
		  new CIMCreateInstanceResponseMessage(
		     request->messageId,
		     cimException,
		     request->queueIds.copyAndPop(),
		     instanceRef);
	       
	       //
	       //  Preserve message key
	       //
	       response->setKey (request->getKey ());
	       
	       //
	       //  Set response destination
	       //
	       response->dest = request->queueIds.top ();
	       
	       Base::_enqueueResponse (request, response);
	       
	       PEG_METHOD_EXIT ();
	       return;
	    }
	 }
	 catch(...)
	 {
	    cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
						 "Error locking repository");
	 }
	 

	 // Subscription exists in the repository. Now we need to 
	 // send a create and enable message to each provider. 

	 Array<CIMInstance> new_subscription;
	 new_subscription.append(instance);
	 try
	 {
	    _create_enable_subscription(new_subscription);
	 }
	 catch(...)
	 {
	    cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
						 "Internal Error");
	 }
      } // both references exist
   } // if an eserver indication subscription 
    
   else
   {
      String exceptionStr = "Required Property Missing";
      PEG_METHOD_EXIT ();
      throw PEGASUS_CIM_EXCEPTION (CIM_ERR_INVALID_PARAMETER,
				   exceptionStr);
   }
    
   PEG_METHOD_EXIT ();
}


void eServerIndicationService::_handleModifyInstanceRequest(const Message * message)
{
   PEG_METHOD_ENTER(TRC_INDICATION_SERVICE, 
		    "eServerIndicationService::_handleMofifyInstanceRequest");
   
   CIMModifyInstanceRequestMessage* request = 
      (CIMModifyInstanceRequestMessage*) message;
   
   CIMException cimException;
   CIMInstance subscription;
   Boolean caught_exception = false;
   
   CIMObjectPath instance_ref = request->modifiedInstance.getPath();
   try
   {
      _repository->read_lock();
	 
      try 
      {
	 subscription = _repository->getInstance (request->nameSpace, 
						  instance_ref);
      }
      catch (CIMException & exception)
      {
	 caught_exception = true;
	 cimException = exception;
      }
      catch (Exception & exception)
      {
	 caught_exception = true;
	 cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
					      exception.getMessage());
      }
      catch(...)
      {
	 caught_exception = true;
	 cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
					      "Internal error");
      }
      _repository->read_unlock ();
   }
   catch(...)
   {
      caught_exception = true;
      cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
					   "Error locking repository");
   } // catch repository read lock exception
   if(caught_exception == false)
   {
      String none = "None";
      try
      {	       
	 _repository->write_lock();
	 
	 try 
	 {
	    _repository->modifyInstance(instance_ref.getNameSpace(),
					subscription, 
					request->includeQualifiers, 
					request->propertyList);
	 }
	 catch (CIMException & exception)
	 {
	    caught_exception = true;
	    cimException = exception;
	 }
	 catch (Exception & exception)
	 {
	    caught_exception = true;
	    cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
						 exception.getMessage());
	 }
	 catch(...)
	 {
	    caught_exception = true;
	    cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
						 "Internal error");
	 }
	 _repository->write_unlock();
      }
      catch(...)
      {
	 caught_exception = true;
	 cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
					      "Error locking repository");
      }
      if(caught_exception == false)
      {

	 try
	 {
	    Array<ProviderClassList> class_list;
	    _get_provider_class_list(
	       subscription, class_list);
	    _send_modify_subscription(class_list, 
				      instance_ref.getNameSpace(),
				      request->propertyList, 
				      none,
				      none, 
				      subscription, 
				      none, 
				      none);
	 }
	 catch(...)
	 {
	    cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
						 "Internal error");
	 }
      } // if no exception inner
   } // if no exception outer 
   
   
   CIMModifyInstanceResponseMessage* response =
      new CIMModifyInstanceResponseMessage(
	 request->messageId,
	 cimException,
	 request->queueIds.copyAndPop());
   
   //
   //  Preserve message key
   //
   response->setKey (request->getKey ());
   
   //
   //  Set response destination
   //
   response->dest = request->queueIds.top ();
   
   Base::_enqueueResponse (request, response);
   
   PEG_METHOD_EXIT ();
}

void eServerIndicationService::_handleDeleteInstanceRequest(const Message *message)
{
   PEG_METHOD_ENTER(TRC_INDICATION_SERVICE, 
		    "eServerIndicationService::_handleDeleteInstanceRequest");
   
   CIMDeleteInstanceRequestMessage* request = 
      static_cast<CIMDeleteInstanceRequestMessage*>(const_cast<Message *>(message));
   
   CIMInstance subscription;
   CIMException cimException;
   Boolean caught_exception = false;
   
   
   try // read lock
   {
      _repository->read_lock();
      
      try // read instance 
      {
	 subscription = _repository->getInstance(request->nameSpace, 
						 request->instanceName);
      }
      catch (CIMException & exception)
      {
	 caught_exception = true;
	 cimException = exception;
      }
      catch (Exception & exception)
      {
	 caught_exception = true;
	 cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
					      exception.getMessage());
      }
      catch(...)
      {
	 caught_exception = true;
	 cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
					      "Internal error");
      }
      _repository->read_unlock();
   }
   catch(...)
   {
      caught_exception = true;
      cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
						 "Error locking repository");
   }
   


   if(caught_exception == false)
   {

      try  // write lock 
      {
	 _repository->write_lock();

	 try  // delete and send notice 
	 {
	    _repository->deleteInstance(request->nameSpace, 
					request->instanceName);
	    
	 }
	 catch (CIMException & exception)
	 {
	    caught_exception = true;
	    cimException = exception;
	 }
	 catch (Exception & exception)
	 {
	    caught_exception = true;
	    cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
						 exception.getMessage());
	 }
	 catch(...)
	 {
	    caught_exception = true;
	    cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
						 "Internal error");
	 }
	 _repository->write_unlock();
      }
      catch(...)
      {
	 caught_exception = true;
	 cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
					      "Error locking repository");
      } // write lock 
      
      
      if(caught_exception == false)
      {

	 try
	 {
	    String none("None");
	    
	    Array<ProviderClassList> class_list;
	    _get_provider_class_list(subscription, class_list);
	    _send_delete_subscription(class_list, 
				      request->nameSpace,
				      subscription, 
				      none,
				      none);
	 }
	 catch(...)
	 {
	    cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
						 "Internal error");
	 }
      } // if no exception inner
   } // if no exception outer 
   
   CIMDeleteInstanceResponseMessage* response =
      new CIMDeleteInstanceResponseMessage(
	 request->messageId,
	 cimException,
	 request->queueIds.copyAndPop());
   
   //
   //  Preserve message key
   //
   response->setKey (request->getKey ());
   
   //
   //  Set response destination
   //
   response->dest = request->queueIds.top ();
   
   Base::_enqueueResponse (request, response);
   
   PEG_METHOD_EXIT ();
}


void eServerIndicationService::_handleProcessIndicationRequest(const Message *message)
{
   PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
		     "eServerIndicationService::_handleProcessIndicationRequest");

   CIMProcessIndicationRequestMessage* request =
      (CIMProcessIndicationRequestMessage*) message;
   
   CIMException cimException;

   CIMProcessIndicationResponseMessage* response =
      new CIMProcessIndicationResponseMessage(
	 request->messageId,
	 cimException,
	 request->queueIds.copyAndPop());
   try 
   {
      /* get each name space */
      Array<CIMNamespaceName> nameSpaceNames;
      _getNameSpaceNames(nameSpaceNames);

      Array<CIMInstance> subscriptions;
      CIMInstance indication = request->indicationInstance.clone();

      _getMatchingSubscriptions(indication, subscriptions);

      // keep this super simple
      // always deliver all properties of the indication class to the consumer
      // every subscription has one indication class and one consumer
      // the consumer is responsible for filtering and multiplexing

      Array<CIMObjectPath> absent_consumers;

      _deliverIndication(subscriptions, 
			 indication,
			 absent_consumers);
   
   }
   catch(CIMException & e)
   {
      response->cimException = e;
   }
   catch(Exception & e)
   {
      response->cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, 
						     e.getMessage());
   }
   catch(...)
   {
   response->cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, 
						  "Internal Error");
   }
   _enqueueResponse(request, response);

   PEG_METHOD_EXIT();
}


void eServerIndicationService::_getActiveSubscriptions (
   Array<CIMInstance> & activeSubscriptions) const
{
   Array <CIMNamespaceName> nameSpaceNames;
   Array <CIMInstance> subscriptions;
   CIMValue subscriptionStateValue;
   Uint16 subscriptionState;

   PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
		     "eServerIndicationService::_getActiveSubscriptions");
   activeSubscriptions.clear();
   
   //
   //  Get list of namespaces in repository
   //
   _getNameSpaceNames(nameSpaceNames);
   //
   //  Get existing subscriptions from each namespace in the repository
   //
   for (Uint8 i = 0; i < nameSpaceNames.size (); i++)
   {

      //
      //  Get existing subscriptions in current namespace
      //
      _getSubscriptions (nameSpaceNames [i], subscriptions);

      //
      //  Process each subscription
      //
      for (Uint8 j = 0; j < subscriptions.size (); j++)
      {
	 //
         //  CIMInstances returned from repository do not include 
	 //  namespace
	 //  Set namespace here
	 //
	 CIMObjectPath instanceName = 
	    subscriptions [j].getPath ();
	 instanceName.setNameSpace (nameSpaceNames [i]);
	 subscriptions [j].setPath (instanceName);
	 activeSubscriptions.append (subscriptions [j]);
      }  // for each subscription
   }  // for each namespace

   PEG_METHOD_EXIT ();
}

//%/////////////////////////////////////////////////////////////////
// Get all the name spaces in the repository. 
// This should really be a static method that any class should be 
// able to call
///////////////////////////////////////////////////////////////////
void eServerIndicationService::_getNameSpaceNames (Array<CIMNamespaceName> & nameSpaceNames) const
{
   
   PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
		     "eServerIndicationService::__getNameSpaceNames");
   nameSpaceNames.clear();
   
   _repository->read_lock ();
   
   try
   {
      nameSpaceNames = _repository->enumerateNameSpaces ();
   }
   catch (Exception &)
   {
      _repository->read_unlock ();
      PEG_METHOD_EXIT ();
   }
   
   _repository->read_unlock ();
   
   PEG_METHOD_EXIT ();
}

//%/////////////////////////////////////////////////////////////////
// Get all subscriptions that refer to a specific indication class
// returns subscriptions in all name spaces. Calls lower-level
// _getMatchingSubscriptions. 
///////////////////////////////////////////////////////////////////
void eServerIndicationService::_getMatchingSubscriptions(
   const CIMInstance & indication,
   Array<CIMInstance> & subscriptions) const 
{
   Array<CIMNamespaceName> nameSpaceNames;
   _getNameSpaceNames(nameSpaceNames);
   
   for(Uint8 i = 0; i < nameSpaceNames.size() ; ++i)
   {
      _getMatchingSubscriptions( nameSpaceNames[i],
				 indication, 
				 subscriptions);
   }
}

//%/////////////////////////////////////////////////////////////////
// Get all subscriptions that refer to a specific indication class
// for a specific name space. 
///////////////////////////////////////////////////////////////////

void eServerIndicationService::_getMatchingSubscriptions(
   const String & nameSpaceName, 
   const CIMInstance & indication,
   Array<CIMInstance> & subscriptions) const
{
   Array<CIMInstance> all_subscriptions;
   _getSubscriptions(nameSpaceName, all_subscriptions);
   
   CIMName indication_class_name = indication.getClassName();
   CIMObjectPath compare_path;

   for(Uint8 x = 0; x < all_subscriptions.size() ; ++x, compare_path.clear())
   {
      _get_object_path_from_association("Indication_Class", 
					all_subscriptions[x],
					compare_path);
      compare_path.setNameSpace(nameSpaceName);
      
      if(0 == String::compareNoCase(compare_path.toString(), indication_class_name.getString()))
      {
	 CIMObjectPath temp = all_subscriptions[x].getPath();
	 temp.setNameSpace(nameSpaceName);
	 all_subscriptions[x].setPath(temp);
	 subscriptions.append(all_subscriptions[x]);
      }
    }
   return;
}

//%/////////////////////////////////////////////////////////////////
// Get all subscriptions in the repository for a specific name space
///////////////////////////////////////////////////////////////////
void eServerIndicationService::_getSubscriptions (
   const String & nameSpaceName, Array <CIMInstance> & subscriptions) const
{
   

   PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
		     "eServerIndicationService::_getSubscriptions");
   
   //
   //  Get existing subscriptions in current namespace
   //
   _repository->read_lock ();

   try
   {
      subscriptions  = _repository->enumerateInstances
	 (nameSpaceName, ESERVER_CLASSNAME_INDSUBSCRIPTION);
   }
   catch (CIMException e)
   {
      //
      //  Some namespaces may not include the subscription class
      //  In that case, just return no subscriptions
      //  Any other exception is an error
      //
      if (e.getCode () != CIM_ERR_INVALID_CLASS)
      {
	 _repository->read_unlock ();
	 PEG_METHOD_EXIT ();
      }
   }

   _repository->read_unlock ();

   PEG_METHOD_EXIT ();
}


void eServerIndicationService::_getIndicationSubclasses (
        const CIMNamespaceName & nameSpace,
        const CIMName & indicationClassName, 
	Array<CIMName> & indicationSubclasses) const
{


    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
                      "eServerIndicationService::_getIndicationSubclasses");
    indicationSubclasses.clear();
    _repository->read_lock ();

    try
    {
        indicationSubclasses = _repository->enumerateClassNames
            (nameSpace, indicationClassName, true);
    }
    catch (Exception &)
    {
        _repository->read_unlock ();
        PEG_METHOD_EXIT ();
    }

    _repository->read_unlock ();

    indicationSubclasses.append (indicationClassName);

    PEG_METHOD_EXIT ();
}


void eServerIndicationService::_getIndicationProviders (
       const CIMNamespaceName & nameSpace,
       const CIMName & indicationClassName,
       const Array <CIMName> & indicationSubclasses,
       const CIMPropertyList & requiredPropertyList, 
       Array <ProviderClassList> & indicationProviders) const
{
   ProviderClassList provider;
 
    Array <CIMInstance> providerInstances;
    Array <CIMInstance> providerModuleInstances;
    Boolean duplicate = false;

    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
                      "eServerIndicationService::_getIndicationProviders");
    
    //
    //  For each indication subclass, get providers
    //
    for (Uint8 i = 0; i < indicationSubclasses.size (); i++)
    {
        //
        //  Get providers that can serve the subscription
        //

       // always call with a null requiredProperty list. That will allow the 
       // provider registration manager to support the simplified eServer event schema
        providerInstances.clear ();
        providerModuleInstances.clear ();
	
        if (_providerRegManager->getIndicationProviders
                (nameSpace,
                 indicationSubclasses [i],
                 requiredPropertyList,
                 providerInstances,
                 providerModuleInstances))
        {
            PEGASUS_ASSERT (providerInstances.size () == 
                            providerModuleInstances.size ());
            //
            //  Merge into list of ProviderClassList structs 
            //
            for (Uint8 j = 0; j < providerInstances.size () && !duplicate; j++)
            {
                provider.classList.clear ();
                duplicate = false;

                //
                //  See if indication provider is already in list
                //
                for (Uint8 k = 0; 
                     k < indicationProviders.size () && !duplicate; k++)
                {
                    if ((providerInstances [j].identical 
                        (indicationProviders [k].provider)) &&
                        (providerModuleInstances [j].identical
                        (indicationProviders [k].providerModule)))
                    {
                        //
                        //  Indication provider is already in list
                        //  Add subclass to provider's class list
                        //
                        indicationProviders [k].classList.append 
                            (indicationSubclasses [i]);
                        duplicate = true;
                    }
                }
            
                if (!duplicate)
                {
                    //
                    //  Current provider is not yet in list
                    //  Create new list entry
                    //
                    provider.provider = providerInstances [j];
                    provider.providerModule = providerModuleInstances [j];
                    provider.classList.append (indicationSubclasses [i]);
                    indicationProviders.append (provider);
                }
            }  // for each indication provider instance
        }  // if any providers
    }  // for each indication subclass
        
    PEG_METHOD_EXIT ();
}

void eServerIndicationService::_generateKey (
    const CIMInstance & subscription,
    const CIMInstance & provider,
    String & tableKey) const
{

   // the key is the name space, plus the indication class name, 
   // plus the stringized object path of the handler reference. 

    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
                      "eServerIndicationService::_generateKey");
    tableKey.clear();
    //
    //  Append subscription namespace name to key
    //
    tableKey.append (subscription.getPath().getNameSpace());
    
    // get the reference to the indication class and stringize it 
    String ic("Indication_Class");
    CIMObjectPath indication_class;

    CIMValue indication_ref = subscription.getProperty(
       subscription.findProperty(ic)).getValue();
    indication_ref.get(indication_class);
    tableKey.append(indication_class.toString());


    // get the reference to the handler instance and stringize it 
    String ha("Handler");    
    CIMObjectPath handler_instance;
    CIMValue handler_ref = subscription.getProperty(
       subscription.findProperty(ha)).getValue();
    handler_ref.get(handler_instance);
    tableKey.append(handler_instance.toString());

    // append the stringized object path of the provider 

    tableKey.append(provider.getPath().toString());

    PEG_METHOD_EXIT ();
}


void eServerIndicationService::_insertEntry (
   const CIMInstance & subscription,
   const CIMInstance & provider,
   const Array <CIMName> & classList)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
                      "eServerIndicationService::_insertEntry");

    String tableKey;
    _generateKey(subscription, provider, tableKey);
    SubscriptionTableEntry entry;
    entry.subscription = subscription;
    entry.provider = provider;
    entry.classList = classList;
    _subscriptionTable.insert (tableKey, entry);

    PEG_METHOD_EXIT ();
}


void eServerIndicationService::_delete_subscription_cb(AsyncOpNode * op, 
						 MessageQueue * q, 
						 void * parm)
{
   PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
		     "eServerIndicationService::_delete_subscription_cb");

    eServerIndicationService * service = static_cast <eServerIndicationService *> (q);
    struct enableProviderList * epl = 
        reinterpret_cast <struct enableProviderList *> (parm);

    AsyncRequest * asyncRequest = static_cast <AsyncRequest *>
        (op->get_request ());
    AsyncReply * asyncReply = static_cast <AsyncReply *> (op->get_response ());
    CIMRequestMessage * request = reinterpret_cast <CIMRequestMessage *>
        ((static_cast <AsyncLegacyOperationStart *> 
        (asyncRequest))->get_action ());

    CIMDeleteSubscriptionResponseMessage * response =
        reinterpret_cast <CIMDeleteSubscriptionResponseMessage *>
        ((static_cast <AsyncLegacyOperationResult *>
	(asyncReply))->get_result());
   
    //
    //  ATTN: Check for return value indicating invalid queue ID
    //  If received, need to find Provider Manager Service queue ID again
    //
   
    PEGASUS_ASSERT (response != 0);
    if (response->cimException.getCode () == CIM_ERR_SUCCESS)
    {
        //  Remove entry from subscription table 
        String tableKey; 
	service->_generateKey(*(epl->cni), epl->pcl->provider, tableKey);
        service->_subscriptionTable.remove(tableKey);
    }
    else
    {
       // log error
    }

    delete epl;
    delete request;
    delete response;
    delete asyncRequest;
    delete asyncReply;
    op->release ();
    service->return_op (op);
}


void eServerIndicationService::_send_delete_subscription(
   const Array <ProviderClassList> & indicationProviders,
   const CIMNamespaceName & nameSpace,
   const CIMInstance & subscription,
   const String & userName,
   const String & authType)
{
   PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
		     "eServerIndicationService::_sendDeleteRequests");
   
   //
   //  Send Delete request to each provider
   //
   for (Uint8 i = 0; i < indicationProviders.size (); i++)
   {
      
      struct enableProviderList *epl = 
	 new enableProviderList(indicationProviders[i], 
				subscription);
      
      CIMDeleteSubscriptionRequestMessage * request =
	 new CIMDeleteSubscriptionRequestMessage
	 (XmlWriter::getNextMessageId (),
	  nameSpace,
	  subscription,
	  epl->pcl->classList,
	  epl->pcl->provider, 
	  epl->pcl->providerModule,
	  QueueIdStack (_providerManager, getQueueId ()),
	  authType,
	  userName);
	
      AsyncOpNode* op = this->get_op();
      
      AsyncLegacyOperationStart * async_req =
	 new AsyncLegacyOperationStart
	 (get_next_xid (),
	  op,
	  _providerManager,
	  request,
	  _queueId);
      
      SendAsync(op, 
		_providerManager, 
		eServerIndicationService::_delete_subscription_cb,
		this, 
		(void *)epl);
      
   }
    PEG_METHOD_EXIT ();
}


void eServerIndicationService::_create_subscription_cb(AsyncOpNode *op, 
						       MessageQueue *q, 
						       void *parm)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
                      "eServerIndicationService::_create_subscription_cb");

    eServerIndicationService *service = 
      static_cast<eServerIndicationService *>(q);
    struct enableProviderList *epl = 
       reinterpret_cast<struct enableProviderList *>(parm);
    
   AsyncRequest *asyncRequest = static_cast<AsyncRequest *>(op->get_request());
   AsyncReply *asyncReply = static_cast<AsyncReply *>(op->get_response());
   CIMRequestMessage *request = reinterpret_cast<CIMRequestMessage *>
      ((static_cast<AsyncLegacyOperationStart *>(asyncRequest))->get_action());
   CIMCreateSubscriptionResponseMessage * response =
      reinterpret_cast 
      <CIMCreateSubscriptionResponseMessage *>
      ((static_cast <AsyncLegacyOperationResult *>
	(asyncReply))->get_result());
   
   PEGASUS_ASSERT(response != 0);
   if (response->cimException.getCode() == CIM_ERR_SUCCESS)
   {
      service->_insertEntry(*(epl->cni), epl->pcl->provider, epl->pcl->classList);
   }
   delete epl;
   delete request;
   delete response;
   delete asyncRequest;
   delete asyncReply;
   op->release();
   service->return_op(op);

    PEG_METHOD_EXIT ();
}


Boolean eServerIndicationService::_send_create_subscription(
   const Array <ProviderClassList> & indicationProviders,
   const String & nameSpace,
   const CIMPropertyList & propertyList,
   const String & condition,
   const String & queryLanguage,
   const CIMInstance & subscription,
   const String & userName,
   const String & authType)
{

    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
                      "eServerIndicationService::_send_create_subscription");

    //
    //  Send Create request to each provider
    //

    for (Uint8 i = 0; i < indicationProviders.size (); i++)
    {
       
       struct enableProviderList *epl = 
           new enableProviderList(indicationProviders[i], 
				 subscription);
       
        CIMCreateSubscriptionRequestMessage * request =
            new CIMCreateSubscriptionRequestMessage
                (XmlWriter::getNextMessageId (),
                nameSpace,
                subscription,
		 epl->pcl->classList, 
		 epl->pcl->provider, 
		 epl->pcl->providerModule,
		 propertyList,
		 0,
		 condition,
		 queryLanguage,
		 QueueIdStack (_providerManager, getQueueId ()),
		 authType,
		 userName);
	
        AsyncOpNode* op = this->get_op(); 

        AsyncLegacyOperationStart * async_req =
            new AsyncLegacyOperationStart
                (get_next_xid (),
		 op,
		 _providerManager,
		 request,
		 _queueId);
	
	SendAsync(op, 
		  _providerManager, 
		  eServerIndicationService::_create_subscription_cb,
		  this, 
		  (void *)epl);
	

    }

    // always return true. the indication service can find out how many subscriptions
    // were succesfully created by checking the size of the subscription table after 
    // a brief moment. 
    return true;
    PEG_METHOD_EXIT ();
}


void eServerIndicationService::_send_enable_indication_cb(AsyncOpNode *op,
							  MessageQueue *q,
							  void *parm)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
                      "eServerIndicationService::_sendEnableCallBack");

   eServerIndicationService *service = 
      static_cast<eServerIndicationService *>(q);
   
   AsyncRequest *asyncRequest = static_cast<AsyncRequest *>(op->get_request());
   AsyncReply *asyncReply = static_cast<AsyncReply *>(op->get_response());
   CIMEnableIndicationsRequestMessage *request = 
      static_cast<CIMEnableIndicationsRequestMessage *>
      ((static_cast<AsyncLegacyOperationStart *>(asyncRequest))->get_action());
   
   CIMEnableIndicationsResponseMessage * response =
      reinterpret_cast<CIMEnableIndicationsResponseMessage *>
      ((static_cast <AsyncLegacyOperationResult *>
        (asyncReply))->get_result());

    if (response->cimException.getCode () == CIM_ERR_SUCCESS)
    {
       // log sucess or failure
       ;
    }
    else
    {
       ;
       
    }

    delete request;
    delete response;
    delete asyncRequest;
    delete asyncReply;
    op->release();
    service->return_op(op);
    PEG_METHOD_EXIT ();
}


void eServerIndicationService::_send_enable_indication (
    const ProviderClassList & enableProvider)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE, "eServerIndicationService::_sendEnable");
 
    CIMEnableIndicationsRequestMessage * request =
        new CIMEnableIndicationsRequestMessage
            (XmlWriter::getNextMessageId (),
             enableProvider.provider,
             enableProvider.providerModule,
             QueueIdStack (_providerManager, getQueueId ()));

    AsyncOpNode* op = this->get_op (); 

    AsyncLegacyOperationStart * async_req =
        new AsyncLegacyOperationStart
            (get_next_xid (),
            op,
            _providerManager,
            request,
            _queueId);

    SendAsync (op, 
	       _providerManager, 
	       eServerIndicationService::_send_enable_indication_cb,
	       this, 
	       NULL);

    PEG_METHOD_EXIT ();
}




void eServerIndicationService::_send_modify_subscription_callback(
    AsyncOpNode * op,
    MessageQueue * q,
    void * parm)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
                      "eServerIndicationService::_send_modify_subscription_callback");

    eServerIndicationService *service = static_cast <eServerIndicationService *> (q);
    struct enableProviderList * epl = reinterpret_cast
        <struct enableProviderList *> (parm);
   
    AsyncRequest * asyncRequest = static_cast <AsyncRequest *>
        (op->get_request ());
    AsyncReply * asyncReply = static_cast <AsyncReply *> (op->get_response ());
    CIMRequestMessage * request = reinterpret_cast <CIMRequestMessage *>
        ((static_cast <AsyncLegacyOperationStart *> 
        (asyncRequest))->get_action ());

    //
    //  ATTN: Check for return value indicating invalid queue ID
    //  If received, need to find Provider Manager Service queue ID
    //  again
    //
   
    CIMModifySubscriptionResponseMessage * response = reinterpret_cast
        <CIMModifySubscriptionResponseMessage *>
        ((static_cast <AsyncLegacyOperationResult *>
	(asyncReply))->get_result ());
   
    PEGASUS_ASSERT(response != 0);
    if (response->cimException.getCode () == CIM_ERR_SUCCESS)
    {
   
        //
        //  Remove old entry from subscription table and insert 
        //  updated entry
        //
        String tableKey;
	service->_generateKey(* (epl->cni), epl->pcl->provider, tableKey);
        service->_subscriptionTable.remove (tableKey);
        service->_insertEntry (* (epl->cni), epl->pcl->provider, 
            epl->pcl->classList);
    }
    else
    {
        // log the error 
    }

    delete epl;
    delete request;
    delete response;
    delete asyncRequest;
    delete asyncReply;
    op->release ();
    service->return_op (op);
}


void eServerIndicationService::_send_modify_subscription(
   const Array <ProviderClassList> & indicationProviders,
   const String & nameSpace,
   const CIMPropertyList & propertyList,
   const String & condition,
   const String & queryLanguage,
   const CIMInstance & subscription,
   const String & userName,
   const String & authType)
{
   CIMValue propValue;
   Uint16 repeatNotificationPolicy;

   PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
		     "eServerIndicationService::_send_modify_subscription");

   //
   //  Send Modify request to each provider
   //
   for (Uint8 i = 0; i < indicationProviders.size (); i++)
   {

      struct enableProviderList *epl = 
	 new enableProviderList(indicationProviders[i], 
				subscription);

      CIMModifySubscriptionRequestMessage * request =
	 new CIMModifySubscriptionRequestMessage(
	    XmlWriter::getNextMessageId (),
	    nameSpace,
	    subscription,
	    epl->pcl->classList, 
	    epl->pcl->provider, 
	    epl->pcl->providerModule,
	    propertyList,
	    repeatNotificationPolicy,
	    condition,
	    queryLanguage,
	    QueueIdStack (_providerManager, getQueueId ()),
	    authType,
	    userName);

      AsyncOpNode* op = this->get_op();

      AsyncLegacyOperationStart * async_req =
	 new AsyncLegacyOperationStart
	 (get_next_xid (),
	  op,
	  _providerManager,
	  request,
	  _queueId);

      SendAsync(op, 
		_providerManager, 
		eServerIndicationService::_send_modify_subscription_callback,
		this, 
		(void *)epl);

   } 

   PEG_METHOD_EXIT ();
}

void eServerIndicationService::_get_provider_class_list(
   const CIMInstance & subscription,
   Array<ProviderClassList> & class_list) const
{
   PEG_METHOD_ENTER (TRC_INDICATION_SERVICE, 
		     "eServerIndicationService::_get_provider_class_list");
   
   CIMPropertyList list;
   String class_name;
   String name_space;
   
   Array<CIMInstance> providers;
   Array<CIMInstance> modules;
   Array<CIMName> subclasses;
   CIMObjectPath path;   
   try 
   {
      _get_object_path_from_association("Indication_Class", 
				       subscription,
				       path);
      name_space = subscription.getPath().getNameSpace();

      
      // get the indication subclasses	  
      _getIndicationSubclasses(name_space, 
			       path.getClassName(),
			       subclasses);
      
      
      // get the providers
      _getIndicationProviders (
	 name_space,
	 path.getClassName(),
	 subclasses, 
	 list, 
	 class_list);
   }
   catch(...)
   {
      // fail gracefully and return an empty array 
   }
   
}


void eServerIndicationService::_create_enable_subscription(
   const Array<CIMInstance> &subscriptions)
{

   PEG_METHOD_ENTER (TRC_INDICATION_SERVICE, "eServerIndicationService::_create_enable_subscription");

   // find providers that support subscribed indications

   Uint8 i = 0;

   Array<ProviderClassList> class_list;
   String none = "None";
   for( ; i < subscriptions.size() ; ++i)
   {
      try 
      {
	 class_list.clear();
	 _get_provider_class_list( subscriptions[i], class_list);
	 
	 CIMPropertyList list;
	 
	 // asynchronously send create subscription requests to each provider
	 _send_create_subscription(class_list, 
				   subscriptions[i].getPath().getNameSpace(), 
				   list, 
				   none, 
				   none,
				   subscriptions[i],
				   none,
				   none);


	 // now send an enable message to the same list of providers. 
	 Uint8 y, z;
	 Array<ProviderClassList> unique_class_list;
	 Boolean duplicate = false;
	  
	 for( y = 0; y < class_list.size(); ++y, duplicate = false)
	 {
	    for(z = 0; z < unique_class_list.size(); ++z)
	    {
	       if(true == class_list[y].provider.identical(unique_class_list[z].provider))
	       {
		  duplicate = true;
		  break;
	       }
	    }
	    if(duplicate == false)
	    {
	       unique_class_list.append(class_list[y]);
	    }
	 }

	 for( y = 0; y < unique_class_list.size() ; ++y)
	 {
	    _send_enable_indication(unique_class_list[y]);
	 }
      }

      catch(...)
      {

      }
   } // for each subscription
    
   PEG_METHOD_EXIT();
}


//*/////////////////////////////////////////////////////////////////
// Take an association class and a name of one or the references. 
// Return a CIMObjectPath that contains the instance of the referred 
// to object. 
///////////////////////////////////////////////////////////////////
Boolean eServerIndicationService::_get_object_path_from_association(
   const String & reference_name,
   const CIMInstance & association, 
   CIMObjectPath & path) const 
{
   
   path.clear();
   try
   {
      CIMValue path_value = association.getProperty(
	 association.findProperty(reference_name)).getValue();
      path_value.get(path);
   }
   catch(...)
   {
      return false;
   }
   
   return true;
}


// will return true if ONE subscription is successful. 
// e.g., if one out of three subscriptions results in a 
// successful delivery, will return true. if zero are 
// successful, returns false. 
// consumers that are absent; i.e., not found by the provider registration 
// manager are returned in the absent_consumer
Boolean  eServerIndicationService::_deliverIndication(
   const Array<CIMInstance> & subscriptions, 
   const CIMInstance & indication,
   Array<CIMObjectPath> & absent_consumers)
{
   
   PEG_METHOD_ENTER (TRC_INDICATION_SERVICE, "eServerIndicationService::_deliverIndication");
   CIMObjectPath consumer;
   CIMInstance provider, providerModule;   
   Boolean ccode = false;
   try 
   {
      
      for(Uint8 i = 0 ; i < subscriptions.size(); ++i, consumer.clear())
      {
	 if(true == _get_object_path_from_association("Handler", 
						      subscriptions[i],
						      consumer))
	 {

	    consumer.setNameSpace(subscriptions[i].getPath().getNameSpace());

	    // get the name space 
	    if(true == _providerRegManager->lookupConsumerProvider(
		  subscriptions[i].getPath().getNameSpace(),
		  consumer.getClassName(),
		  provider, 
		  providerModule))
	    {
	       
	       CIMConsumeIndicationRequestMessage *request = 
		  new CIMConsumeIndicationRequestMessage(
		     XmlWriter::getNextMessageId (),
		     indication.getPath().getNameSpace(), 
		     indication, 
		     provider, 
		     providerModule, 
		     QueueIdStack(_providerManager, _queueId));
	    
	       AsyncOpNode *op = get_op();
	    
	       AsyncLegacyOperationStart *async_req  = 
		  new AsyncLegacyOperationStart(
		     get_next_xid(), 
		     op, 
		     _providerManager, 
		     request, 
		     _queueId);
	       try 
	       {
		  if( false == SendForget(async_req) )
		  {
		     PEG_TRACE_STRING(TRC_INDICATION_SERVICE, Tracer::LEVEL4, 
				      "Error Sending (SendForget) Indication to " + 
				      ((MessageQueue::lookup(_providerManager)) ? 
				       String( ((MessageQueue::lookup(_providerManager))->getQueueName())) : 
				       String("BAD queue name")) + 
				      "via CIMConsumeIndicationRequestMessage");
	       
		     async_req = static_cast<AsyncLegacyOperationStart *>(op->get_request());
		     if(async_req)
			delete async_req->get_action();
		     delete async_req;
		     delete op;
		     // the consumer may not truly be absent, but we couldn't get 
		     // the message to him. Maybe the service can do something 
		     // with this information. 
		     absent_consumers.append(consumer);
		  }
		  else 
		  {
		     PEG_TRACE_STRING(TRC_INDICATION_SERVICE, Tracer::LEVEL4, 
				      "Successfuly sent (SendForget) Indication to " + 
				      ((MessageQueue::lookup(_providerManager)) ? 
				       String( ((MessageQueue::lookup(_providerManager))->getQueueName())) : 
				       String("BAD queue name")) + 
				      " via CIMConsumeIndicationRequestMessage");
		     ccode = true;
		  }
	       }
	       catch(...)
	       {

		  async_req = static_cast<AsyncLegacyOperationStart *>(op->get_request());
		  if(async_req)
		     delete async_req->get_action();
		  delete async_req;
		  delete op;
		  // the consumer may not truly be absent, but we couldn't get 
		  // the message to him. Maybe the service can do something 
		  // with this information. 
		  absent_consumers.append(consumer);

	       }
	    } // handler is registered as a consumer provider 
	    else 
	    {
	       absent_consumers.append(consumer);
	    }
	 
	 } // found the handler path 
      } // for each subscription
   } // try 
   catch(...)
   {
      throw;
   }
   
   // each subscription contains a reference to a provider 
   // that can handle the subscription. extract the handler
   // instance from the reference 
   PEG_METHOD_EXIT();
   return ccode;
}

PEGASUS_NAMESPACE_END
