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
//              Nag Boranna, Hewlett-Packard Company(nagaraja_boranna@hp.com)
//              Yi Zhou, Hewlett-Packard Company(yi_zhou@hp.com)
//              Jenny Yu, Hewlett-Packard Company (jenny_yu@hp.com)
//              Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include "ProviderManagerService.h"

#include <Pegasus/Common/Config.h>
#include <Pegasus/Config/ConfigManager.h>
#include <Pegasus/ProviderManager/ProviderFacade.h>
#include <Pegasus/Common/OperationContext.h>
#include <Pegasus/Provider/OperationFlag.h>
#include <Pegasus/Provider/SimpleResponseHandler.h>

PEGASUS_NAMESPACE_BEGIN

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

ProviderManagerService::ProviderManagerService(void)
   : MessageQueueService("Server::ProviderManagerService", MessageQueue::getNextQueueId())
{
}

ProviderManagerService::~ProviderManagerService(void)
{
}

void ProviderManagerService::handleEnqueue(Message *message)
{
   if(! message)
      return;
   

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
	 handleGetInstanceRequest(message);

	 break;
      case CIM_ENUMERATE_INSTANCES_REQUEST_MESSAGE:
	 handleEnumerateInstancesRequest(message);

	 break;
      case CIM_ENUMERATE_INSTANCE_NAMES_REQUEST_MESSAGE:
	 handleEnumerateInstanceNamesRequest(message);

	 break;
      case CIM_CREATE_INSTANCE_REQUEST_MESSAGE:
	 handleCreateInstanceRequest(message);

	 break;
      case CIM_MODIFY_INSTANCE_REQUEST_MESSAGE:
	 handleModifyInstanceRequest(message);

	 break;
      case CIM_DELETE_INSTANCE_REQUEST_MESSAGE:
	 handleDeleteInstanceRequest(message);

	 break;
      case CIM_EXEC_QUERY_REQUEST_MESSAGE:
      case CIM_ASSOCIATORS_REQUEST_MESSAGE:
      case CIM_ASSOCIATOR_NAMES_REQUEST_MESSAGE:
      case CIM_REFERENCES_REQUEST_MESSAGE:
      case CIM_REFERENCE_NAMES_REQUEST_MESSAGE:
	 break;
      case CIM_GET_PROPERTY_REQUEST_MESSAGE:
	 handleGetPropertyRequest(message);

	 break;
      case CIM_SET_PROPERTY_REQUEST_MESSAGE:
	 handleSetPropertyRequest(message);

	 break;
      case CIM_GET_QUALIFIER_REQUEST_MESSAGE:
      case CIM_SET_QUALIFIER_REQUEST_MESSAGE:
      case CIM_DELETE_QUALIFIER_REQUEST_MESSAGE:
      case CIM_ENUMERATE_QUALIFIERS_REQUEST_MESSAGE:
	 break;
      case CIM_INVOKE_METHOD_REQUEST_MESSAGE:
	 handleInvokeMethodRequest(message);

	 break;
      case CIM_ENABLE_INDICATION_SUBSCRIPTION_REQUEST_MESSAGE:
	 handleEnableIndicationRequest(message);

	 break;

      case CIM_MODIFY_INDICATION_SUBSCRIPTION_REQUEST_MESSAGE:
	 handleModifyIndicationRequest(message);

	 break;
      case CIM_DISABLE_INDICATION_SUBSCRIPTION_REQUEST_MESSAGE:
	 handleDisableIndicationRequest(message);

	 break;
      default:
	 break;
   }

   delete message;
}


void ProviderManagerService::handleEnqueue(void)
{
   Message * message = dequeue();
	
   PEGASUS_ASSERT(message != 0);
   if( message )
      handleEnqueue(message);
	

}

Pair<String, String> ProviderManagerService::_lookupProviderForClass(const CIMObjectPath & objectPath)
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
	 objectPath.getNameSpace(),
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
	 ((static_cast<AsyncLegacyOperationResult *>(async_reply))->get_result());

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

	 // create request
	 CIMRequestMessage * request = new CIMGetInstanceRequestMessage(
	    "golden snitch",
	    objectPath.getNameSpace(),
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
	    ((static_cast<AsyncLegacyOperationResult *>(async_reply))->get_result());

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
	 if(!String::equalNoCase(objectPath.getClassName(), providerCapabilitiesInstance.getProperty(pos).getValue().toString()))
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

	 // create request
	 CIMRequestMessage * request = new CIMGetInstanceRequestMessage(
	    "golden snitch",
	    objectPath.getNameSpace(),
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
	    ((static_cast<AsyncLegacyOperationResult *>(async_reply))->get_result());

	 delete async_req;
	 delete async_reply;

	 providerInstance = response->cimInstance;
      }

      // extract provider information
      String providerName = providerInstance.getProperty(providerInstance.findProperty("Name")).getValue().toString();
      String providerLocation = providerInstance.getProperty(providerInstance.findProperty("Location")).getValue().toString();

      if((providerName.size() != 0) && (providerLocation.size() != 0))
      {
	 String fileName;

	 //
	 // translate the provider identifier into a file name
	 //
#ifdef PEGASUS_OS_TYPE_WINDOWS
	 fileName = providerLocation + String(".dll");
#elif defined(PEGASUS_OS_HPUX)
	 fileName = ConfigManager::getHomedPath(
	    ConfigManager::getInstance()->getCurrentValue(
	       "providerDir")) + String("/lib") +
	    providerName + String(".sl");
#else
	 // fileName = providerLocation + getenv("PEGASUS_HOME") + String("/lib/lib") + _providerName + String(".so");
	 fileName = ConfigManager::getHomedPath(
	    ConfigManager::getInstance()->getCurrentValue(
	       "providerDir")) + String("/lib") +
	    providerName + String(".so");
#endif

	 return(Pair<String, String>(fileName, providerName));
      }

      // provider information error, use different technique
      break;
   }

   return(Pair<String, String>(String::EMPTY, String::EMPTY));
}

void ProviderManagerService::handleGetInstanceRequest(const Message * message)
{
   const CIMGetInstanceRequestMessage * request =
      (const CIMGetInstanceRequestMessage *)message;

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
      ProviderModule module = providerManager.getProviderModule(pair.first, pair.second);

      // encapsulate the physical provider in a facade
      ProviderFacade facade(module.getProvider());

      // convert arguments
      OperationContext context;
      CIMReference instanceReference(request->instanceName);

      // ATTN: propagate namespace
      instanceReference.setNameSpace(request->nameSpace);

      // ATTN: convert flags to bitmask
      Uint32 flags = OperationFlag::convert(false);
      CIMPropertyList propertyList(request->propertyList);

      SimpleResponseHandler<CIMInstance> handler;

      // forward request
      facade.getInstance(
	 context,
	 instanceReference,
	 flags,
	 propertyList.getPropertyNameArray(),
	 handler);

      // error? provider claims success, but did not deliver an instance.
      if(handler._objects.size() == 0)
      {
	 throw CIMException(CIM_ERR_NOT_FOUND);
      }

      // save returned instance
      cimInstance = handler._objects[0];
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

   // lookup the message queue
   MessageQueue * queue = MessageQueue::lookup(request->queueIds.top());

   PEGASUS_ASSERT(queue != 0);

   // enqueue the response
   queue->enqueue(response);
}

void ProviderManagerService::handleEnumerateInstancesRequest(const Message * message)
{
   const CIMEnumerateInstancesRequestMessage * request =
      (const CIMEnumerateInstancesRequestMessage *)message;

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
      ProviderModule module = providerManager.getProviderModule(pair.first, pair.second);

      // encapsulate the physical provider in a facade
      ProviderFacade facade(module.getProvider());

      // convert arguments
      OperationContext context;

      // ATTN: propagate namespace
      classReference.setNameSpace(request->nameSpace);

      // ATTN: convert flags to bitmask
      Uint32 flags = OperationFlag::convert(false);
      CIMPropertyList propertyList(request->propertyList);

      SimpleResponseHandler<CIMInstance> handler;

      facade.enumerateInstances(
	 context,
	 classReference,
	 flags,
	 propertyList.getPropertyNameArray(),
	 handler);

      // save returned instance

      // ATTN: can be removed once CIMNamedInstance is removed
      for(Uint32 i = 0, n = handler._objects.size(); i < n; i++)
      {
	 cimInstances.append(CIMNamedInstance(handler._objects[i].getPath(), handler._objects[i]));
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

   // lookup the message queue
   MessageQueue * queue = MessageQueue::lookup(request->queueIds.top());

   PEGASUS_ASSERT(queue != 0);

   // enqueue the response
   queue->enqueue(response);
}

void ProviderManagerService::handleEnumerateInstanceNamesRequest(const Message * message)
{
   const CIMEnumerateInstanceNamesRequestMessage * request =
      (const CIMEnumerateInstanceNamesRequestMessage *)message;

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
      ProviderModule module = providerManager.getProviderModule(pair.first, pair.second);

      // encapsulate the physical provider in a facade
      ProviderFacade facade(module.getProvider());

      // convert arguments
      OperationContext context;

      // ATTN: propagate namespace
      classReference.setNameSpace(request->nameSpace);

      SimpleResponseHandler<CIMReference> handler;

      facade.enumerateInstanceNames(
	 context,
	 classReference,
	 handler);

      // save returned instance
      cimReferences = handler._objects;
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

   // lookup the message queue
   MessageQueue * queue = MessageQueue::lookup(request->queueIds.top());

   PEGASUS_ASSERT(queue != 0);

   // enqueue the response
   queue->enqueue(response);
}

void ProviderManagerService::handleCreateInstanceRequest(const Message * message)
{
   const CIMCreateInstanceRequestMessage * request =
      (const CIMCreateInstanceRequestMessage *)message;

   CIMReference cimReference;

   // create response message
   CIMCreateInstanceResponseMessage * response =
      new CIMCreateInstanceResponseMessage(
	 request->messageId,
	 CIM_ERR_FAILED,
	 "not implemented",
	 request->queueIds.copyAndPop(),
	 cimReference);

   // preserve message key
   response->setKey(request->getKey());

   // lookup the message queue
   MessageQueue * queue = MessageQueue::lookup(request->queueIds.top());

   PEGASUS_ASSERT(queue != 0);

   // enqueue the response
   queue->enqueue(response);
}

void ProviderManagerService::handleModifyInstanceRequest(const Message * message)
{
   const CIMModifyInstanceRequestMessage * request =
      (const CIMModifyInstanceRequestMessage *)message;

   // create response message
   CIMModifyInstanceResponseMessage * response =
      new CIMModifyInstanceResponseMessage(
	 request->messageId,
	 CIM_ERR_FAILED,
	 "not implemented",
	 request->queueIds.copyAndPop());

   // preserve message key
   response->setKey(request->getKey());

   // lookup the message queue
   MessageQueue * queue = MessageQueue::lookup(request->queueIds.top());

   PEGASUS_ASSERT(queue != 0);

   // enqueue the response
   queue->enqueue(response);
}

void ProviderManagerService::handleDeleteInstanceRequest(const Message * message)
{
   const CIMDeleteInstanceRequestMessage * request =
      (const CIMDeleteInstanceRequestMessage *)message;

   // create response message
   CIMDeleteInstanceResponseMessage * response =
      new CIMDeleteInstanceResponseMessage(
	 request->messageId,
	 CIM_ERR_FAILED,
	 "not implemented",
	 request->queueIds.copyAndPop());

   // preserve message key
   response->setKey(request->getKey());

   // lookup the message queue
   MessageQueue * queue = MessageQueue::lookup(request->queueIds.top());

   PEGASUS_ASSERT(queue != 0);

   // enqueue the response
   queue->enqueue(response);
}

void ProviderManagerService::handleGetPropertyRequest(const Message * message)
{
   const CIMGetPropertyRequestMessage * request =
      (const CIMGetPropertyRequestMessage *)message;

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

   // lookup the message queue
   MessageQueue * queue = MessageQueue::lookup(request->queueIds.top());

   PEGASUS_ASSERT(queue != 0);

   // enqueue the response
   queue->enqueue(response);
}

void ProviderManagerService::handleSetPropertyRequest(const Message * message)
{
   const CIMSetPropertyRequestMessage * request =
      (const CIMSetPropertyRequestMessage *)message;

   // create response message
   CIMSetPropertyResponseMessage * response =
      new CIMSetPropertyResponseMessage(
	 request->messageId,
	 CIM_ERR_FAILED,
	 "not implemented",
	 request->queueIds.copyAndPop());

   // preserve message key
   response->setKey(request->getKey());

   // lookup the message queue
   MessageQueue * queue = MessageQueue::lookup(request->queueIds.top());

   PEGASUS_ASSERT(queue != 0);

   // enqueue the response
   queue->enqueue(response);
}

void ProviderManagerService::handleInvokeMethodRequest(const Message * message)
{
   CIMValue returnValue;
   Array<CIMParamValue> outParameters;
   Status status;
   CIMInstance cimInstance;

   const CIMInvokeMethodRequestMessage * request =
      (const CIMInvokeMethodRequestMessage *)message;

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
      ProviderModule module = providerManager.getProviderModule(pair.first, pair.second);

      // encapsulate the physical provider in a facade
      ProviderFacade facade(module.getProvider());

      // convert arguments
      OperationContext context;
      CIMReference instanceReference(request->instanceName);

      // ATTN: propagate namespace
      instanceReference.setNameSpace(request->nameSpace);

      SimpleResponseHandler<CIMValue> handler;

      // forward request
      facade.invokeMethod(
	 context,
	 instanceReference,
	 request->methodName,
	 request->inParameters,
	 outParameters,
	 handler);

      // error? provider claims success, but did not deliver an instance.
      if(handler._objects.size() == 0)
      {
	 throw CIMException(CIM_ERR_NOT_FOUND);
      }

      returnValue = handler._objects[0];
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

   // lookup the message queue
   MessageQueue * queue = MessageQueue::lookup(request->queueIds.top());

   PEGASUS_ASSERT(queue != 0);

   // enqueue the response
   queue->enqueue(response);
}

void ProviderManagerService::handleEnableIndicationRequest(const Message * message)
{
   const CIMEnableIndicationSubscriptionRequestMessage * request =
      (const CIMEnableIndicationSubscriptionRequestMessage *)message;

   CIMStatusCode errorCode = CIM_ERR_SUCCESS;
   String errorDescription = String::EMPTY;

   try
   {
      // make class reference
      CIMReference classReference(
	 "",
	 request->nameSpace,
	 request->classNames[0]);

      // ATTN: this needs to be message based
      Pair<String, String> pair = _lookupProviderForClass(classReference);
      ProviderModule module = providerManager.getProviderModule(pair.first, pair.second);
      ProviderFacade facade(module.getProvider());

      SimpleResponseHandler<CIMInstance> handler;

      try
      {
	 //
	 //  ATTN: pass thresholding parameter values in
	 //  operation context
	 //
	 facade.enableIndication(
	    OperationContext (),
	    request->nameSpace,
	    request->classNames,
	    request->providerName,
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
	 errorCode = CIM_ERR_FAILED;
	 errorDescription = "Provider not available";
      }
   }
   catch(CIMException& exception)
   {
      errorCode = exception.getCode ();
      errorDescription = exception.getMessage ();
   }
   catch(Exception& exception)
   {
      errorCode = CIM_ERR_FAILED;
      errorDescription = exception.getMessage ();
   }
   catch(...)
   {
      errorCode = CIM_ERR_FAILED;
   }

   CIMEnableIndicationSubscriptionResponseMessage * response =
      new CIMEnableIndicationSubscriptionResponseMessage(
	 request->messageId,
	 errorCode,
	 errorDescription,
	 request->queueIds.copyAndPop());

   // preserve message key
   response->setKey(request->getKey());

   // lookup the message queue
   MessageQueue * queue = MessageQueue::lookup(request->queueIds.top());

   PEGASUS_ASSERT(queue != 0);

   // enqueue the response
   queue->enqueue(response);
}

void ProviderManagerService::handleModifyIndicationRequest(const Message * message)
{
   const CIMModifyIndicationSubscriptionRequestMessage * request =
      (const CIMModifyIndicationSubscriptionRequestMessage *)message;

   CIMModifyIndicationSubscriptionResponseMessage * response =
      new CIMModifyIndicationSubscriptionResponseMessage(
	 request->messageId,
	 CIM_ERR_FAILED,
	 "not implemented",
	 request->queueIds.copyAndPop());

   // preserve message key
   response->setKey(request->getKey());

   // lookup the message queue
   MessageQueue * queue = MessageQueue::lookup(request->queueIds.top());

   PEGASUS_ASSERT(queue != 0);

   // enqueue the response
   queue->enqueue(response);
}

void ProviderManagerService::handleDisableIndicationRequest(const Message * message)
{
   const CIMDisableIndicationSubscriptionRequestMessage * request =
      (const CIMDisableIndicationSubscriptionRequestMessage *)message;

   CIMDisableIndicationSubscriptionResponseMessage * response =
      new CIMDisableIndicationSubscriptionResponseMessage(
	 request->messageId,
	 CIM_ERR_FAILED,
	 "not implemented",
	 request->queueIds.copyAndPop());

   // preserve message key
   response->setKey(request->getKey());

   // lookup the message queue
   MessageQueue * queue = MessageQueue::lookup(request->queueIds.top());

   PEGASUS_ASSERT(queue != 0);

   // enqueue the response
   queue->enqueue(response);
}

PEGASUS_NAMESPACE_END
