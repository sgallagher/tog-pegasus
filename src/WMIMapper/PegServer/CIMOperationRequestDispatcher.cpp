//%/////////////////////////////////////////////////////////////////////////////
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
//%/////////////////////////////////////////////////////////////////////////////
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
//				 Barbara Packard, Hewlett-Packard Company (barbara_packard@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include "CIMOperationRequestDispatcher.h"
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/XmlReader.h> // stringToValue(), stringArrayToValue()
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Config/ConfigManager.h>

PEGASUS_NAMESPACE_BEGIN

PEGASUS_USING_STD;

#ifndef TRC_WMI_MAPPER
#define TRC_WMI_MAPPER TRC_DISPATCHER
#endif

CIMOperationRequestDispatcher::CIMOperationRequestDispatcher( )
//    CIMRepository* repository,
//    ProviderRegistrationManager* providerRegistrationManager)
      :
      Base(PEGASUS_QUEUENAME_OPREQDISPATCHER)
//      _repository(repository),
//      _providerRegistrationManager(providerRegistrationManager)
{
   PEG_METHOD_ENTER(TRC_WMI_MAPPER,
      "CIMOperationRequestDispatcher::CIMOperationRequestDispatcher");

   // Check whether or not AssociationTraversal is supported.
   //
   ConfigManager* configManager = ConfigManager::getInstance();
   _enableAssociationTraversal = String::equal(
        configManager->getCurrentValue("enableAssociationTraversal"), "true");
//   _enableIndicationService = String::equal(
//        configManager->getCurrentValue("enableIndicationService"), "true");

   PEG_METHOD_EXIT();
}

CIMOperationRequestDispatcher::~CIMOperationRequestDispatcher(void)
{
   PEG_METHOD_ENTER(TRC_WMI_MAPPER,
      "CIMOperationRequestDispatcher::~CIMOperationRequestDispatcher");
   _dying = 1;
   PEG_METHOD_EXIT();
}


void CIMOperationRequestDispatcher::_enqueueResponse(
   CIMRequestMessage* request,
   CIMResponseMessage* response)
{
   PEG_METHOD_ENTER(TRC_WMI_MAPPER,
      "CIMOperationRequestDispatcher::_enqueueResponse");

   // Use the same key as used in the request:

   response->setKey(request->getKey());
   response->dest = request->queueIds.top();

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
   PEG_METHOD_ENTER(TRC_WMI_MAPPER,
      "CIMOperationRequestDispatcher::handleEnqueue(Message *request)");

/*	debug if want to - removed 6/27 - bbp
	if (getenv("PEG_DBG"))
	{
		DebugBreak();
	}
*/

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

      case CIM_EXEC_QUERY_REQUEST_MESSAGE:
	 handleExecQueryRequest(
	    (CIMExecQueryRequestMessage*)request);
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

	  default:
		throw CIMException(CIM_ERR_NOT_SUPPORTED);
   }

   delete request;
   PEG_METHOD_EXIT();
}

// allocate a CIM Operation_async,  opnode, context, and response handler
// initialize with pointers to async top and async bottom
// link to the waiting q
void CIMOperationRequestDispatcher::handleEnqueue()
{
   PEG_METHOD_ENTER(TRC_WMI_MAPPER,
      "CIMOperationRequestDispatcher::handleEnqueue");

   Message* request = dequeue();

   if(request)
      handleEnqueue(request);

   PEG_METHOD_EXIT();
}

void CIMOperationRequestDispatcher::handleGetClassRequest(
   CIMGetClassRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_WMI_MAPPER,
      "CIMOperationRequestDispatcher::handleGetClassRequest");

   CIMStatusCode errorCode = CIM_ERR_SUCCESS;
   String errorDescription;
   CIMClass cimClass;
   CIMException cimException;

	try
	{
	WMIClassProvider provider;

	provider.initialize();

	 WMIMapperUserInfoContainer container = request->operationContext.get(WMIMapperUserInfoContainer::NAME);

     cimClass = provider.getClass(
	 request->nameSpace,
	 request->userName,
	 container.getPassword(),
	 request->className,
	 request->localOnly,
	 request->includeQualifiers,
	 request->includeClassOrigin,
	 request->propertyList);

	//terminate the provider
	provider.terminate();
  }
   catch(CIMException& exception)
   {
	   cimException = exception;
   }
   catch(Exception& exception)
   {
	   cimException =
		   PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
   }
   catch(...)
   {
      cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
   }


   CIMGetClassResponseMessage* response = new CIMGetClassResponseMessage(
      request->messageId,
      cimException,
      request->queueIds.copyAndPop(),
      cimClass);

   // and send the response
   _enqueueResponse(request, response);
   PEG_METHOD_EXIT();
}

void CIMOperationRequestDispatcher::handleGetInstanceRequest(
   CIMGetInstanceRequestMessage* request)
{

	PEG_METHOD_ENTER(TRC_WMI_MAPPER,
      "CIMOperationRequestDispatcher::handleGetInstanceRequest");

	CIMStatusCode errorCode = CIM_ERR_SUCCESS;
	String errorDescription;

	CIMInstance cimInstance;
	CIMException cimException;

	// get the class name
	String className = request->instanceName.getClassName();

	WMIInstanceProvider provider;

	try
	{
		provider.initialize( );

		/*CIMObjectPath instanceReference(request->instanceName);
		instanceReference.setNameSpace(request->nameSpace);*/

	    WMIMapperUserInfoContainer container = request->operationContext.get(WMIMapperUserInfoContainer::NAME);

		cimInstance = provider.getInstance(
			request->nameSpace,
			request->userName,
			container.getPassword(),
			request->instanceName,
			request->localOnly,
			request->includeQualifiers,
			request->includeClassOrigin,
			request->propertyList);
	}
   catch(CIMException& exception)
   {
	   cimException = exception;
   }
   catch(Exception& exception)
   {
	   cimException =
		   PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
   }
   catch(...)
   {
      cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
   }

	// create the response message
	CIMGetInstanceResponseMessage * response =
		new CIMGetInstanceResponseMessage(
		request->messageId,
		cimException,
		request->queueIds.copyAndPop(),
		cimInstance);

	// cancel the provider
	provider.terminate();

	// and send the response
	_enqueueResponse(request, response);
	PEG_METHOD_EXIT();
}

void CIMOperationRequestDispatcher::handleDeleteClassRequest(
   CIMDeleteClassRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_WMI_MAPPER,
      "CIMOperationRequestDispatcher::handleDeleteClassRequest");

   CIMStatusCode errorCode = CIM_ERR_SUCCESS;
   String errorDescription;
   CIMException cimException;

   try
   {
		WMIClassProvider provider;

		provider.initialize();

	    WMIMapperUserInfoContainer container = request->operationContext.get(WMIMapperUserInfoContainer::NAME);
		
		provider.deleteClass(
			request->nameSpace,
			request->userName,
			container.getPassword(),
			request->className);

		//terminate the provider
		provider.terminate();
   }

   catch(CIMException& exception)
   {
	   cimException = exception;
   }
   catch(Exception& exception)
   {
	   cimException =
		   PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
   }
   catch(...)
   {
      cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
   }


   CIMDeleteClassResponseMessage* response =
      new CIMDeleteClassResponseMessage(
	 request->messageId,
	 cimException,
	 request->queueIds.copyAndPop());

	// and send the response
   _enqueueResponse(request, response);

	PEG_METHOD_EXIT();
}

void CIMOperationRequestDispatcher::handleDeleteInstanceRequest(
   CIMDeleteInstanceRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_WMI_MAPPER,
      "CIMOperationRequestDispatcher::handleDeleteInstanceRequest");

   CIMStatusCode errorCode = CIM_ERR_SUCCESS;
   String errorDescription;
   CIMException cimException;

   try
   {
		WMIInstanceProvider provider;

		provider.initialize();

	    WMIMapperUserInfoContainer container = request->operationContext.get(WMIMapperUserInfoContainer::NAME);

		provider.deleteInstance(
			request->nameSpace,
			request->userName,
			container.getPassword(),
			request->instanceName);

		//terminate the provider
		provider.terminate();
   }

   catch(CIMException& exception)
   {
	   cimException = exception;
   }
   catch(Exception& exception)
   {
	   cimException =
		   PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
   }
   catch(...)
   {
      cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
   }


   // create the response
	CIMDeleteInstanceResponseMessage* response =
		new CIMDeleteInstanceResponseMessage(
		request->messageId,
		cimException,
		request->queueIds.copyAndPop());

    // and send it
	_enqueueResponse(request, response);

	PEG_METHOD_EXIT();
}

void CIMOperationRequestDispatcher::handleCreateClassRequest(
   CIMCreateClassRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_WMI_MAPPER,
      "CIMOperationRequestDispatcher::handleCreateClassRequest");

   CIMStatusCode errorCode = CIM_ERR_SUCCESS;
   String errorDescription;
   CIMException cimException;

   try
   {
 		WMIClassProvider provider;

		provider.initialize();

	    WMIMapperUserInfoContainer container = request->operationContext.get(WMIMapperUserInfoContainer::NAME);

		provider.createClass(
			request->nameSpace,
			request->userName,
			container.getPassword(),
			request->newClass);

		//terminate the provider
		provider.terminate();
   }

    catch(CIMException& exception)
   {
	   cimException = exception;
   }
   catch(Exception& exception)
   {
	   cimException =
		   PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
   }
   catch(...)
   {
      cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
   }

   CIMCreateClassResponseMessage* response =
      new CIMCreateClassResponseMessage(
	 request->messageId,
	 cimException,
	 request->queueIds.copyAndPop());

   // and send the response
   _enqueueResponse(request, response);

	PEG_METHOD_EXIT();
}

void CIMOperationRequestDispatcher::handleCreateInstanceRequest(
   CIMCreateInstanceRequestMessage* request)
{

   PEG_METHOD_ENTER(TRC_WMI_MAPPER,
      "CIMOperationRequestDispatcher::handleCreateInstanceRequest");

   CIMStatusCode errorCode = CIM_ERR_SUCCESS;
   String errorDescription;
   CIMObjectPath instanceName;
   CIMException cimException;

   try
   {
 		WMIInstanceProvider provider;

		provider.initialize();

	    WMIMapperUserInfoContainer container = request->operationContext.get(WMIMapperUserInfoContainer::NAME);

		instanceName = provider.createInstance(
			request->nameSpace,
			request->userName,
			container.getPassword(),
			request->newInstance);

		//terminate the provider
		provider.terminate();
   }

    catch(CIMException& exception)
   {
	   cimException = exception;
   }
   catch(Exception& exception)
   {
	   cimException =
		   PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
   }
   catch(...)
   {
      cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
   }

   //create the response
   CIMCreateInstanceResponseMessage* response =
         new CIMCreateInstanceResponseMessage(
	    request->messageId,
	    cimException,
	    request->queueIds.copyAndPop(),
	    instanceName);

   // and send it
   _enqueueResponse(request, response);

	PEG_METHOD_EXIT();
}

void CIMOperationRequestDispatcher::handleModifyClassRequest(
   CIMModifyClassRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_WMI_MAPPER,
      "CIMOperationRequestDispatcher::handleModifyClassRequest");

   CIMStatusCode errorCode = CIM_ERR_SUCCESS;
   String errorDescription;
   CIMException cimException;

   try
   {
 		WMIClassProvider provider;

		provider.initialize();

	  WMIMapperUserInfoContainer container = request->operationContext.get(WMIMapperUserInfoContainer::NAME);

	  provider.modifyClass(
		request->nameSpace,
		request->userName,
		container.getPassword(),
		request->modifiedClass);

		//terminate the provider
		provider.terminate();
   }

    catch(CIMException& exception)
   {
	   cimException = exception;
   }
   catch(Exception& exception)
   {
	   cimException =
		   PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
   }
   catch(...)
   {
      cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
   }

   CIMModifyClassResponseMessage* response =
      new CIMModifyClassResponseMessage(
	 request->messageId,
	 cimException,
	 request->queueIds.copyAndPop());

   // and send the response
   _enqueueResponse(request, response);

	PEG_METHOD_EXIT();
}

void CIMOperationRequestDispatcher::handleModifyInstanceRequest(
   CIMModifyInstanceRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_WMI_MAPPER,
      "CIMOperationRequestDispatcher::handleModifyInstanceRequest");

   CIMStatusCode errorCode = CIM_ERR_SUCCESS;
   String errorDescription;
   CIMException cimException;

   try
   {
	   WMIInstanceProvider provider;

	   provider.initialize();

	   WMIMapperUserInfoContainer container = request->operationContext.get(WMIMapperUserInfoContainer::NAME);

	   provider.modifyInstance(
		   request->nameSpace,
		   request->userName,
		   container.getPassword(),
		   request->modifiedInstance,
		   request->includeQualifiers,
		   request->propertyList);

	   //terminate the provider
	   provider.terminate();
   }

    catch(CIMException& exception)
   {
	   cimException = exception;
   }
   catch(Exception& exception)
   {
	   cimException =
		   PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
   }
   catch(...)
   {
      cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
   }

   // create the response
      CIMModifyInstanceResponseMessage* response =
         new CIMModifyInstanceResponseMessage(
	    request->messageId,
	    cimException,
	    request->queueIds.copyAndPop());

	// and send it
	  _enqueueResponse(request, response);

	PEG_METHOD_EXIT();
}

void CIMOperationRequestDispatcher::handleEnumerateClassesRequest(
   CIMEnumerateClassesRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_WMI_MAPPER,
      "CIMOperationRequestDispatcher::handleEnumerateClassesRequest");

   CIMStatusCode errorCode = CIM_ERR_SUCCESS;
   String errorDescription;
   CIMException cimException;
   Array<CIMClass> cimClasses;

   try
   {
 		WMIClassProvider provider;

		provider.initialize();

	    WMIMapperUserInfoContainer container = request->operationContext.get(WMIMapperUserInfoContainer::NAME);

		cimClasses = provider.enumerateClasses(
	 		request->nameSpace,
			request->userName,
			container.getPassword(),
	 		request->className,
	 		request->deepInheritance,
	 		request->localOnly,
	 		request->includeQualifiers,
	 		request->includeClassOrigin);

		//terminate the provider
		provider.terminate();
   }


    catch(CIMException& exception)
   {
	   cimException = exception;
   }
   catch(Exception& exception)
   {
	   cimException =
		   PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
   }
   catch(...)
   {
      cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
   }

   CIMEnumerateClassesResponseMessage* response =
      new CIMEnumerateClassesResponseMessage(
	 request->messageId,
	 cimException,
	 request->queueIds.copyAndPop(),
	 cimClasses);

   // and send the response
   _enqueueResponse(request, response);

	PEG_METHOD_EXIT();
}

void CIMOperationRequestDispatcher::handleEnumerateClassNamesRequest(
   CIMEnumerateClassNamesRequestMessage* request)
{
  PEG_METHOD_ENTER(TRC_WMI_MAPPER,
      "CIMOperationRequestDispatcher::handleEnumerateClassNamesRequest");

  CIMStatusCode errorCode = CIM_ERR_SUCCESS;
   String errorDescription;
   CIMException cimException;
   Array<CIMName> classNames;

   try
   {
 		WMIClassProvider provider;

		provider.initialize();

	  WMIMapperUserInfoContainer container = request->operationContext.get(WMIMapperUserInfoContainer::NAME);

      classNames = provider.enumerateClassNames(
			request->nameSpace,
			request->userName,
			container.getPassword(),
			request->className,
			request->deepInheritance);

		//terminate the provider
		provider.terminate();
   }

    catch(CIMException& exception)
   {
	   cimException = exception;
   }
   catch(Exception& exception)
   {
	   cimException =
		   PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
   }
   catch(...)
   {
      cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
   }

   CIMEnumerateClassNamesResponseMessage* response =
      new CIMEnumerateClassNamesResponseMessage(
	 request->messageId,
	 cimException,
	 request->queueIds.copyAndPop(),
	 classNames);

   // and send the response
   _enqueueResponse(request, response);

	PEG_METHOD_EXIT();
}

void CIMOperationRequestDispatcher::handleEnumerateInstancesRequest(
   CIMEnumerateInstancesRequestMessage* request)
{
    PEG_METHOD_ENTER(TRC_WMI_MAPPER,"CIMOperationRequestDispatcher::"
                                  "handleEnumerateInstanceRequest");

	CIMStatusCode errorCode = CIM_ERR_SUCCESS;
	String errorDescription;
   CIMException cimException;

	Array<CIMInstance> cimInstances;

	// get the class name
//	String className = request->className;

	WMIInstanceProvider provider;
//	CIMOMHandle cimom;

	try
	{
		provider.initialize( );

		CIMPropertyList propertyList(request->propertyList);

		// ATTN: fix parameter list

	    WMIMapperUserInfoContainer container = request->operationContext.get(WMIMapperUserInfoContainer::NAME);

		cimInstances = provider.enumerateInstances(
			request->nameSpace,
			request->userName,
			container.getPassword(),
			request->className,
			true, true, false, false,
			propertyList);

	}
    catch(CIMException& exception)
   {
	   cimException = exception;
   }
   catch(Exception& exception)
   {
	   cimException =
		   PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
   }
   catch(...)
   {
      cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
   }

	// create the response message
	CIMEnumerateInstancesResponseMessage * response =
		new CIMEnumerateInstancesResponseMessage(
		request->messageId,
		cimException,
		request->queueIds.copyAndPop(),
		cimInstances);

	// cancel the provider
	provider.terminate();

	// and send the response
	_enqueueResponse(request, response);
   PEG_METHOD_EXIT();
}

void CIMOperationRequestDispatcher::handleEnumerateInstanceNamesRequest(
   CIMEnumerateInstanceNamesRequestMessage* request)
{
	PEG_METHOD_ENTER(TRC_WMI_MAPPER,
      "CIMOperationRequestDispatcher::handleEnumerateInstanceNamesRequest");

	CIMStatusCode errorCode = CIM_ERR_SUCCESS;
	String errorDescription;
	CIMException cimException;

	Array<CIMObjectPath> instanceNames;

	WMIInstanceProvider provider;

	try
	{
		// make the request...
		provider.initialize( );

	    WMIMapperUserInfoContainer container = request->operationContext.get(WMIMapperUserInfoContainer::NAME);

		instanceNames = provider.enumerateInstanceNames(
			request->nameSpace,
			request->userName,
			container.getPassword(),
			request->className);
	}
    catch(CIMException& exception)
   {
	   cimException = exception;
   }
   catch(Exception& exception)
   {
	   cimException =
		   PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
   }
   catch(...)
   {
      cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
   }

	// create the response message
	CIMEnumerateInstanceNamesResponseMessage * response =
		new CIMEnumerateInstanceNamesResponseMessage(
			request->messageId,
			cimException,
			request->queueIds.copyAndPop(),
			instanceNames);

	// cancel the provider
	provider.terminate();

	// and send the response
	_enqueueResponse(request, response);
   PEG_METHOD_EXIT();
}

void CIMOperationRequestDispatcher::handleAssociatorsRequest(
   CIMAssociatorsRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_WMI_MAPPER,
      "CIMOperationRequestDispatcher::handleAssociatorsRequest");

	CIMStatusCode errorCode = CIM_ERR_SUCCESS;
	String errorDescription;
	CIMException cimException;

	Array<CIMObject> cimObjects;

	// get the class name
//	String className = request->className;

	WMIAssociatorProvider provider;

	try
	{
		provider.initialize( );

		CIMPropertyList propertyList(request->propertyList);

		// ATTN: fix parameter list

	    WMIMapperUserInfoContainer container = request->operationContext.get(WMIMapperUserInfoContainer::NAME);

		cimObjects = provider.associators(
			request->nameSpace,
			request->userName,
			container.getPassword(),
			request->objectName,
			request->assocClass,
			request->resultClass,
			request->role,
			request->resultRole,
			request->includeQualifiers,
			request->includeClassOrigin,
			propertyList);
	}
    catch(CIMException& exception)
   {
	   cimException = exception;
   }
   catch(Exception& exception)
   {
	   cimException =
		   PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
   }
   catch(...)
   {
      cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
   }

    // create the response
	CIMAssociatorsResponseMessage* response =
         new CIMAssociatorsResponseMessage(
	    request->messageId,
	    cimException,
	    request->queueIds.copyAndPop(),
	    cimObjects);

	// cancel the provider
	provider.terminate();

    // and send it
	_enqueueResponse(request, response);

   PEG_METHOD_EXIT();
}

void CIMOperationRequestDispatcher::handleAssociatorNamesRequest(
   CIMAssociatorNamesRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_WMI_MAPPER,
      "CIMOperationRequestDispatcher::handleAssociatorNamesRequest");

	CIMStatusCode errorCode = CIM_ERR_SUCCESS;
	String errorDescription;
	CIMException cimException;

	Array<CIMObjectPath> objectNames;

	WMIAssociatorProvider provider;

	try
	{
		provider.initialize( );

	    WMIMapperUserInfoContainer container = request->operationContext.get(WMIMapperUserInfoContainer::NAME);

		objectNames = provider.associatorNames(
			request->nameSpace,
			request->userName,
			container.getPassword(),
			request->objectName,
			request->assocClass,
			request->resultClass,
			request->role,
			request->resultRole);
	}
    catch(CIMException& exception)
   {
	   cimException = exception;
   }
   catch(Exception& exception)
   {
	   cimException =
		   PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
   }
   catch(...)
   {
      cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
   }


	// create the response
	CIMAssociatorNamesResponseMessage* response =
		new CIMAssociatorNamesResponseMessage(
		request->messageId,
		cimException,
		request->queueIds.copyAndPop(),
		objectNames);

	// cancel the provider
	provider.terminate();

    // and send it
	_enqueueResponse(request, response);

   PEG_METHOD_EXIT();
}

void CIMOperationRequestDispatcher::handleReferencesRequest(
   CIMReferencesRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_WMI_MAPPER,
      "CIMOperationRequestDispatcher::handleReferencesRequest");

	CIMStatusCode errorCode = CIM_ERR_SUCCESS;
	String errorDescription;
	CIMException cimException;

	Array<CIMObject> cimObjects;

	// get the class name
//	String className = request->className;

	WMIReferenceProvider provider;

	try
	{
		provider.initialize( );

		CIMPropertyList propertyList(request->propertyList);

		// ATTN: fix parameter list

	    WMIMapperUserInfoContainer container = request->operationContext.get(WMIMapperUserInfoContainer::NAME);

		cimObjects = provider.references(
			request->nameSpace,
			request->userName,
			container.getPassword(),
			request->objectName,
			request->resultClass,
			request->role,
			request->includeQualifiers,
			request->includeClassOrigin,
			propertyList);
	}
    catch(CIMException& exception)
   {
	   cimException = exception;
   }
   catch(Exception& exception)
   {
	   cimException =
		   PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
   }
   catch(...)
   {
      cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
   }


    // create the response
	CIMReferencesResponseMessage* response =
         new CIMReferencesResponseMessage(
	    request->messageId,
	    cimException,
	    request->queueIds.copyAndPop(),
	    cimObjects);

	// cancel the provider
	provider.terminate();

    // and send it
	_enqueueResponse(request, response);

   PEG_METHOD_EXIT();
}

void CIMOperationRequestDispatcher::handleReferenceNamesRequest(
   CIMReferenceNamesRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_WMI_MAPPER,
      "CIMOperationRequestDispatcher::handleReferenceNamesRequest");

	CIMStatusCode errorCode = CIM_ERR_SUCCESS;
	String errorDescription;
	CIMException cimException;

	Array<CIMObjectPath> objectNames;

	WMIReferenceProvider provider;

	try
	{
		provider.initialize( );

	    WMIMapperUserInfoContainer container = request->operationContext.get(WMIMapperUserInfoContainer::NAME);

		objectNames = provider.referenceNames(
			request->nameSpace,
			request->userName,
			container.getPassword(),
			request->objectName,
			request->resultClass,
			request->role);
	}
    catch(CIMException& exception)
   {
	   cimException = exception;
   }
   catch(Exception& exception)
   {
	   cimException =
		   PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
   }
   catch(...)
   {
      cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
   }

    // create the response
	CIMReferenceNamesResponseMessage* response =
         new CIMReferenceNamesResponseMessage(
	    request->messageId,
	    cimException,
	    request->queueIds.copyAndPop(),
	    objectNames);

	// cancel the provider
	provider.terminate();

    // and send it
	_enqueueResponse(request, response);

   PEG_METHOD_EXIT();
}

void CIMOperationRequestDispatcher::handleGetPropertyRequest(
   CIMGetPropertyRequestMessage* request)
{
	PEG_METHOD_ENTER(TRC_WMI_MAPPER,
      "CIMOperationRequestDispatcher::handleGetPropertyRequest");

	CIMStatusCode errorCode = CIM_ERR_SUCCESS;
	String errorDescription;
	CIMException cimException;

	CIMValue value;

	try
	{
		WMIInstanceProvider provider;

		provider.initialize();

	    WMIMapperUserInfoContainer container = request->operationContext.get(WMIMapperUserInfoContainer::NAME);

		value = provider.getProperty(
			request->nameSpace,
			request->userName,
			container.getPassword(),
			request->instanceName,
			request->propertyName);
		
		provider.terminate();
	}
    catch(CIMException& exception)
   {
	   cimException = exception;
   }
   catch(Exception& exception)
   {
	   cimException =
		   PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
   }
   catch(...)
   {
      cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
   }

	// create the response message
      CIMGetPropertyResponseMessage* response =
         new CIMGetPropertyResponseMessage(
	    request->messageId,
	    cimException,
	    request->queueIds.copyAndPop(),
	    value);

	// and send it
	  _enqueueResponse(request, response);

	PEG_METHOD_EXIT();
}

void CIMOperationRequestDispatcher::handleSetPropertyRequest(
   CIMSetPropertyRequestMessage* request)
{
 	PEG_METHOD_ENTER(TRC_WMI_MAPPER,
      "CIMOperationRequestDispatcher::handleSetPropertyRequest");

	CIMStatusCode errorCode = CIM_ERR_SUCCESS;
	String errorDescription;
	CIMException cimException;

	CIMValue value;

  /* {
      CIMException cimException;
      try
      {
         _fixSetPropertyValueType(request);
      }
      catch (CIMException& exception)
      {
         cimException = exception;
      }
      catch(Exception& exception)
      {
         cimException =
            PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
      }
      catch(...)
      {
         cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
      }

      if (cimException.getCode() != CIM_ERR_SUCCESS)
      {
         CIMSetPropertyResponseMessage* response =
            new CIMSetPropertyResponseMessage(
               request->messageId,
               cimException,
               request->queueIds.copyAndPop());

         STAT_COPYDISPATCHER

         _enqueueResponse(request, response);

         PEG_METHOD_EXIT();
         return;
      }
	}*/

	try
	{
		WMIInstanceProvider provider;

		provider.initialize();

	    WMIMapperUserInfoContainer container = request->operationContext.get(WMIMapperUserInfoContainer::NAME);

		provider.setProperty(
			request->nameSpace,
			request->userName,
			container.getPassword(),
			request->instanceName,
			request->propertyName,
			request->newValue);

		provider.terminate();
	}
   catch(CIMException& exception)
   {
	   cimException = exception;
   }
   catch(Exception& exception)
   {
	   cimException =
		   PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
   }
   catch(...)
   {
      cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
   }

    // create the response
	CIMSetPropertyResponseMessage* response =
         new CIMSetPropertyResponseMessage(
	    request->messageId,
	    cimException,
	    request->queueIds.copyAndPop());

    // and send it
	_enqueueResponse(request, response);

	PEG_METHOD_EXIT();
}

void CIMOperationRequestDispatcher::handleGetQualifierRequest(
   CIMGetQualifierRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_WMI_MAPPER,
      "CIMOperationRequestDispatcher::handleGetQualifierRequest");

   CIMStatusCode errorCode = CIM_ERR_SUCCESS;
   String errorDescription;
   CIMException cimException;

   CIMQualifierDecl cimQualifierDecl;

	try
	{
		WMIQualifierProvider provider;

		provider.initialize();

	    WMIMapperUserInfoContainer container = request->operationContext.get(WMIMapperUserInfoContainer::NAME);

		cimQualifierDecl = provider.getQualifier(
			request->nameSpace,
			request->userName,
			container.getPassword(),
			request->qualifierName);

		provider.terminate();
	}
   catch(CIMException& exception)
   {
	   cimException = exception;
   }
   catch(Exception& exception)
   {
	   cimException =
		   PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
   }
   catch(...)
   {
      cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
   }

	// create the response message
   CIMGetQualifierResponseMessage* response =
      new CIMGetQualifierResponseMessage(
	 request->messageId,
	 cimException,
	 request->queueIds.copyAndPop(),
	 cimQualifierDecl);

   // and send it
   _enqueueResponse(request, response);

   PEG_METHOD_EXIT();
}

void CIMOperationRequestDispatcher::handleSetQualifierRequest(
   CIMSetQualifierRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_WMI_MAPPER,
      "CIMOperationRequestDispatcher::handleSetQualifierRequest");

   CIMStatusCode errorCode = CIM_ERR_SUCCESS;
   String errorDescription;
   CIMException cimException;

	try
	{
		WMIQualifierProvider provider;

		provider.initialize();

	    WMIMapperUserInfoContainer container = request->operationContext.get(WMIMapperUserInfoContainer::NAME);

		provider.setQualifier(
			request->nameSpace,
			request->userName,
			container.getPassword(),
			request->qualifierDeclaration);

		provider.terminate();
	}
   catch(CIMException& exception)
   {
	   cimException = exception;
   }
   catch(Exception& exception)
   {
	   cimException =
		   PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
   }
   catch(...)
   {
      cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
   }

	// create the response message
   CIMSetQualifierResponseMessage* response =
      new CIMSetQualifierResponseMessage(
	 request->messageId,
	 cimException,
	 request->queueIds.copyAndPop());

   // and send it
   _enqueueResponse(request, response);

   PEG_METHOD_EXIT();
}

void CIMOperationRequestDispatcher::handleDeleteQualifierRequest(
   CIMDeleteQualifierRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_WMI_MAPPER,
      "CIMOperationRequestDispatcher::handleDeleteQualifierRequest");

   CIMStatusCode errorCode = CIM_ERR_SUCCESS;
   String errorDescription;
   CIMException cimException;

	try
	{
		WMIQualifierProvider provider;

		provider.initialize();

	    WMIMapperUserInfoContainer container = request->operationContext.get(WMIMapperUserInfoContainer::NAME);

		provider.deleteQualifier(
			request->nameSpace,
			request->userName,
			container.getPassword(),
			request->qualifierName);

		provider.terminate();
	}
   catch(CIMException& exception)
   {
	   cimException = exception;
   }
   catch(Exception& exception)
   {
	   cimException =
		   PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
   }
   catch(...)
   {
      cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
   }

	// create the response message
   CIMDeleteQualifierResponseMessage* response =
      new CIMDeleteQualifierResponseMessage(
	 request->messageId,
	 cimException,
	 request->queueIds.copyAndPop());

   // and send it
   _enqueueResponse(request, response);

   PEG_METHOD_EXIT();
}

void CIMOperationRequestDispatcher::handleEnumerateQualifiersRequest(
   CIMEnumerateQualifiersRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_WMI_MAPPER,
      "CIMOperationRequestDispatcher::handleEnumerateQualifiersRequest");

   CIMStatusCode errorCode = CIM_ERR_SUCCESS;
   String errorDescription;
   CIMException cimException;

   Array<CIMQualifierDecl> qualifierDeclarations;

	try
	{
		WMIQualifierProvider provider;

		provider.initialize();

	    WMIMapperUserInfoContainer container = request->operationContext.get(WMIMapperUserInfoContainer::NAME);

		qualifierDeclarations = provider.enumerateQualifiers(
			request->nameSpace,
			request->userName,
			container.getPassword());

		provider.terminate();
	}
   catch(CIMException& exception)
   {
	   cimException = exception;
   }
   catch(Exception& exception)
   {
	   cimException =
		   PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
   }
   catch(...)
   {
      cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
   }

	// create the response message
   CIMEnumerateQualifiersResponseMessage* response =
      new CIMEnumerateQualifiersResponseMessage(
	 request->messageId,
	 cimException,
	 request->queueIds.copyAndPop(),
	 qualifierDeclarations);

	// and send it
   _enqueueResponse(request, response);

   PEG_METHOD_EXIT();
}

void CIMOperationRequestDispatcher::handleExecQueryRequest(
   CIMExecQueryRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_WMI_MAPPER,
      "CIMOperationRequestDispatcher::handleExecQueryRequest");

   CIMStatusCode errorCode = CIM_ERR_SUCCESS;
   String errorDescription;
   CIMException cimException;

   Array<CIMObject> cimObjects;

	try
	{
		WMIQueryProvider provider;

		provider.initialize();

	    WMIMapperUserInfoContainer container = request->operationContext.get(WMIMapperUserInfoContainer::NAME);

		cimObjects = provider.execQuery(
			request->nameSpace,
			request->userName,
			container.getPassword(),
			request->queryLanguage,
			request->query);

		provider.terminate();
	}
   catch(CIMException& exception)
   {
	   cimException = exception;
   }
   catch(Exception& exception)
   {
	   cimException =
		   PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
   }
   catch(...)
   {
      cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
   }

	// create the response message
   CIMExecQueryResponseMessage* response =
      new CIMExecQueryResponseMessage(
	 request->messageId,
	 cimException,
	 request->queueIds.copyAndPop(),
	 cimObjects);

   // and send it
   _enqueueResponse(request, response);

   PEG_METHOD_EXIT();
}

void CIMOperationRequestDispatcher::handleInvokeMethodRequest(
   CIMInvokeMethodRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_WMI_MAPPER,
      "CIMOperationRequestDispatcher::handleInvokeMethodRequest");

   CIMStatusCode errorCode = CIM_ERR_SUCCESS;
   String errorDescription;
   CIMException cimException;

   /*{
      try
      {
         _fixInvokeMethodParameterTypes(request);
      }
   catch(CIMException& exception)
   {
	   cimException = exception;
   }
   catch(Exception& exception)
   {
	   cimException =
		   PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
   }
   catch(...)
   {
      cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
   }

      if (errorCode != CIM_ERR_SUCCESS)
      {
         CIMInvokeMethodResponseMessage *response =
            new CIMInvokeMethodResponseMessage(
               request->messageId,
               cimException,
               request->queueIds.copyAndPop(),
               CIMValue(),
               Array<CIMParamValue>(),
               request->methodName);

         _enqueueResponse(request, response);

         PEG_METHOD_EXIT();
         return;
      }
   }*/

   CIMValue retValue;
   Array<CIMParamValue> outParameters;

	try
	{
		WMIMethodProvider provider;

		provider.initialize();

	    WMIMapperUserInfoContainer container = request->operationContext.get(WMIMapperUserInfoContainer::NAME);

		retValue = provider.invokeMethod(
			request->nameSpace,
			request->userName,
			container.getPassword(),
			request->instanceName,
			request->methodName,
			request->inParameters,
			outParameters);

		// ATTN:  I need some exception handling here

		provider.terminate();
	}
   catch(CIMException& exception)
   {
	   cimException = exception;
   }
   catch(Exception& exception)
   {
	   cimException =
		   PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
   }
   catch(...)
   {
      cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
   }

	// create the response message
   CIMInvokeMethodResponseMessage *response =
      new CIMInvokeMethodResponseMessage(
	 request->messageId,
	 cimException,
	 request->queueIds.copyAndPop(),
	 retValue,
	 outParameters,
	 request->methodName);

   // and send it
   _enqueueResponse(request, response);
}

/**
   Convert the specified CIMValue to the specified type, and return it in
   a new CIMValue.
*/
CIMValue CIMOperationRequestDispatcher::_convertValueType(
   const CIMValue& value,
   CIMType type)
{
   PEG_METHOD_ENTER(TRC_WMI_MAPPER,
      "CIMOperationRequestDispatcher::_convertValueType");

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
	 char* charPtr = strdup(stringArray[k].getCString());
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
	    String("Malformed ") + cimTypeToString (type) + " value");
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
	 newValue = XmlReader::stringToValue(0, stringValue.getCString(), type);
      }
      catch (XmlSemanticError e)
      {
         PEG_METHOD_EXIT();
	 throw PEGASUS_CIM_EXCEPTION(
	    CIM_ERR_INVALID_PARAMETER,
	    String("Malformed ") + cimTypeToString (type) + " value");
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
   PEG_METHOD_ENTER(TRC_WMI_MAPPER,
      "CIMOperationRequestDispatcher::_fixInvokeMethodParameterTypes");

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

                try
                {
					WMIClassProvider provider;

					provider.initialize();

	                WMIMapperUserInfoContainer container = request->operationContext.get(WMIMapperUserInfoContainer::NAME);

					cimClass = provider.getClass(
                        request->nameSpace,
						request->userName,
						container.getPassword(),
                        request->instanceName.getClassName(),
                        false, //localOnly,
                        false, //includeQualifiers,
                        false, //includeClassOrigin,
                        CIMPropertyList());
                }
                catch (CIMException& exception)
                {
                    PEG_METHOD_EXIT();
                    throw exception;
                }
                catch (Exception& exception)
                {
                    PEG_METHOD_EXIT();
                    throw exception;
                }
                catch (...)
                {
                    PEG_METHOD_EXIT();
                    throw PEGASUS_CIM_EXCEPTION(CIM_ERR_METHOD_NOT_FOUND,
                                                String::EMPTY);
                }

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
            CIMName paramName = inParameters[i].getParameterName();
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
                        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_TYPE_MISMATCH,
                                                    String::EMPTY);
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
   Modified - bbp 08/15/02
*/

void CIMOperationRequestDispatcher::_fixSetPropertyValueType(
   CIMSetPropertyRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_WMI_MAPPER,
      "CIMOperationRequestDispatcher::_fixSetPropertyValueType");


   String sClassName;
   sClassName = String((request->instanceName).getClassName());

   String propertyName;
   propertyName = request->propertyName;

   Array<CIMName> propertyNames;

   CIMName propName = propertyName;
   propertyNames.append(propName);

   CIMPropertyList propertyList = CIMPropertyList(propertyNames);	

   CIMValue inValue = request->newValue;

   //
   // Only do the conversion if the type is not already set
   //
   if ((inValue.getType() != CIMTYPE_STRING))
   {
      PEG_METHOD_EXIT();
      return;
   }

   //
   // Get the class definition for this property
   //
   CIMClass cimClass;
   CIMException cimException;

	try
	{
	WMIClassProvider provider;

	provider.initialize();


	 WMIMapperUserInfoContainer container = request->operationContext.get(WMIMapperUserInfoContainer::NAME);

	 cimClass = provider.getClass(
	 request->nameSpace,
	 request->userName,
	 container.getPassword(),
	 sClassName,
	 false,
	 false,
	 false,
	 propertyList);

    //terminate the provider
	provider.terminate();
  }
   catch(CIMException& exception)
   {
	   cimException = exception;
   }
   catch(Exception& exception)
   {
	   cimException =
		   PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
   }
   catch(...)
   {
      cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
   }

   //
   // Get the property definition from the class
   //
   Uint32 propertyPos = cimClass.findProperty(request->propertyName);
   if (propertyPos == PEG_NOT_FOUND)
   {
      PEG_METHOD_EXIT();
	  throw PEGASUS_CIM_EXCEPTION(CIM_ERR_TYPE_MISMATCH, String::EMPTY);
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
	  throw PEGASUS_CIM_EXCEPTION(CIM_ERR_TYPE_MISMATCH, String::EMPTY);
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
