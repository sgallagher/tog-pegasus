//%2005////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
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
// This module provides the calling facade for control providers.
// Accepting CIMMessages on the input side it provides calls for each of
// the CIM provider operations to control providers setting up the
// appropriate context for each call and processing handler responses.
// NOTE: Today the execquery is NOT SUPPORTED by this facade.
//
// Author: Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
// Modified By:
//               Carol Ann Krug Graves, Hewlett-Packard Company
//                   (carolann_graves@hp.com)
//               Dan Gorey, IBM (djgorey@us.ibm.com)
//               Amit K Arora, IBM (amita@in.ibm.com) for PEP#101
//				 Seema Gupta (gseema@in.ibm.com) for PEP135
//				 Seema Gupta (gseema@in.ibm.com) for Bug#1441
//               Aruran, IBM (ashanmug@in.ibm.com) for Bug# 3766
//               John Alex, IBM (johnalex@us.ibm.com) - Bug#2290
//
//%/////////////////////////////////////////////////////////////////////////////

#include "ProviderMessageFacade.h"

#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Common/OperationContext.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/Thread.h>
// l10n
#include <Pegasus/Common/MessageLoader.h>
#include <Pegasus/Common/AutoPtr.h>

#include <Pegasus/ProviderManager2/SimpleResponseHandler.h>

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

ProviderMessageFacade::ProviderMessageFacade(CIMProvider* provider)
    : ProviderFacade(provider)
{
}

ProviderMessageFacade::~ProviderMessageFacade(void)
{  
   // Terminate the control provider. The 'terminate' calls the control
   // provider 'terminiate' function which should delete the object.
  terminate();
}


Message * ProviderMessageFacade::handleRequestMessage(Message * message) throw()
{
   Message * response = 0;

   CIMMessage * msg = dynamic_cast<CIMMessage *>(message);

   if(msg != NULL)
   {
   	AcceptLanguages *langs = new AcceptLanguages(((AcceptLanguageListContainer)msg->operationContext.get
		   (AcceptLanguageListContainer:: NAME)).getLanguages());
	Thread::setLanguages(langs);
   }
   else
   {
   	Thread::clearLanguages();
   }

   try
   {

    // pass the request message to a handler method based on message type
    switch(message->getType())
    {
    case CIM_GET_INSTANCE_REQUEST_MESSAGE:
        response = _handleGetInstanceRequest(message);
        break;
    case CIM_ENUMERATE_INSTANCES_REQUEST_MESSAGE:
	response = _handleEnumerateInstancesRequest(message);
	break;
    case CIM_ENUMERATE_INSTANCE_NAMES_REQUEST_MESSAGE:
	response = _handleEnumerateInstanceNamesRequest(message);
	break;
    case CIM_CREATE_INSTANCE_REQUEST_MESSAGE:
	response = _handleCreateInstanceRequest(message);
	break;
       case CIM_MODIFY_INSTANCE_REQUEST_MESSAGE:
       {
	
       //cout << " ProviderMessageFacade::handleRequestMessage " << endl;

       Message *ret = _handleModifyInstanceRequest(message);
       //cout << " modify instance response " << "type " << ret->getType() << " dest " << ret->dest << endl;

       //cout << " leaving ProviderMessageFacade::handleRequestMessage " << endl;
       response = ret;
       }

       break;

    case CIM_DELETE_INSTANCE_REQUEST_MESSAGE:
       response = _handleDeleteInstanceRequest(message);
	break;
    case CIM_EXEC_QUERY_REQUEST_MESSAGE:
	response = _handleExecuteQueryRequest(message);
	break;
    case CIM_ASSOCIATORS_REQUEST_MESSAGE:
	response = _handleAssociatorsRequest(message);
	break;
    case CIM_ASSOCIATOR_NAMES_REQUEST_MESSAGE:
	response = _handleAssociatorNamesRequest(message);
	break;
    case CIM_REFERENCES_REQUEST_MESSAGE:
	response = _handleReferencesRequest(message);
	break;
    case CIM_REFERENCE_NAMES_REQUEST_MESSAGE:
	response = _handleReferenceNamesRequest(message);
	break;
    case CIM_GET_PROPERTY_REQUEST_MESSAGE:
	response = _handleGetPropertyRequest(message);
	break;
    case CIM_SET_PROPERTY_REQUEST_MESSAGE:
	response = _handleSetPropertyRequest(message);
	break;
    case CIM_INVOKE_METHOD_REQUEST_MESSAGE:
	response = _handleInvokeMethodRequest(message);
	break;
    case CIM_GET_CLASS_REQUEST_MESSAGE:
    case CIM_ENUMERATE_CLASSES_REQUEST_MESSAGE:
    case CIM_ENUMERATE_CLASS_NAMES_REQUEST_MESSAGE:
    case CIM_CREATE_CLASS_REQUEST_MESSAGE:
    case CIM_MODIFY_CLASS_REQUEST_MESSAGE:
    case CIM_DELETE_CLASS_REQUEST_MESSAGE:
    default:
	// unsupported messages are ignored
	break;
    }
   }
   catch( ... )
   {
      //cout << "caught exception in ProviderMessageFacade::handleRequestMessage" << endl;
      ;
   }

    //delete message;

    //
    //  Set HTTP method, Key, closeConnect in response from request
    //
    ((CIMResponseMessage *)response)->syncAttributes((CIMRequestMessage*)message);

    return(response);
}

/* Since the caller of _handleXXX methods (handleRequestMessage)  is not expected to 
throw an exception (indicated by the empty throw() clause on handleRequestMessage), all the 
_handleXXX methods also shouldn't throw an exception */

Message * ProviderMessageFacade::_handleGetInstanceRequest(Message * message) 
{
    const CIMGetInstanceRequestMessage * request =
	dynamic_cast<CIMGetInstanceRequestMessage *>(message);

    STAT_GETSTARTTIME

    PEGASUS_ASSERT(request != 0);

    CIMException cimException;
    CIMInstance cimInstance;
    ContentLanguages contentLangs;  // l10n

    try
    {
	// make target object path
	CIMObjectPath objectPath(
	    System::getHostName(),
	    request->nameSpace,
	    request->instanceName.getClassName(),
	    request->instanceName.getKeyBindings());

	/* convert arguments
	OperationContext context;

	 add the user name and accept and content Languages to the context
	context.insert(request->operationContext.get(IdentityContainer::NAME));
	context.insert(request->operationContext.get(AcceptLanguageListContainer::NAME)); 
	context.insert(request->operationContext.get(ContentLanguageListContainer::NAME)); 
    */
	CIMPropertyList propertyList(request->propertyList);

	SimpleInstanceResponseHandler handler;

	// forward request
	getInstance(
	    request->operationContext,
	    objectPath,
	    request->includeQualifiers,
	    request->includeClassOrigin,
	    propertyList,
	    handler);

	// error? provider claims success, but did not deliver an instance.
	if(handler.getObjects().size() == 0)
	{
	   // << Mon Apr 29 12:40:36 2002 mdd >>
//	    throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_FOUND, String::EMPTY);

	  // l10n

	  cimException = PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED, MessageLoaderParms("Server.ProviderMessageFacade.UNKNOWN_ERROR", "Unknown Error"));

	  // cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "Unknown Error");

	}

	// save returned instance
	cimInstance = handler.getObjects()[0];
        contentLangs = handler.getLanguages();  // l10n
    }
    catch(CIMException & e)
    {
        cimException = e;
    }
    catch(Exception & e)
    {
        cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, e.getMessage());
    }
    catch(...)
    {
      // l10n

      cimException = PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED, MessageLoaderParms("Server.ProviderMessageFacade.UNKNOWN_ERROR", "Unknown Error"));
      // cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "Unknown Error");
    }

    // create response message
    AutoPtr<CIMGetInstanceResponseMessage> response(
	new CIMGetInstanceResponseMessage(
	    request->messageId,
	    cimException,
	    request->queueIds.copyAndPop(),
	    cimInstance));  // l10n

    STAT_PMS_PROVIDEREND

	response->operationContext.set(ContentLanguageListContainer(contentLangs));

    // preserve message key
    response->setKey(request->getKey());

    return response.release();
}

Message * ProviderMessageFacade::_handleEnumerateInstancesRequest(Message * message) 
{
    

    const CIMEnumerateInstancesRequestMessage * request =
	dynamic_cast<CIMEnumerateInstancesRequestMessage *>(message);

    STAT_GETSTARTTIME

    PEGASUS_ASSERT(request != 0);

    CIMException cimException;
    Array<CIMInstance> cimInstances;
    ContentLanguages contentLangs;  // l10n

    try
    {
	// make target object path
	CIMObjectPath objectPath(
	    System::getHostName(),
	    request->nameSpace,
	    request->className);

	/* convert arguments
	OperationContext context;

	// add the user name and accept and content Languages to the context
	context.insert(request->operationContext.get(IdentityContainer::NAME));
	context.insert(request->operationContext.get(AcceptLanguageListContainer::NAME)); 
	context.insert(request->operationContext.get(ContentLanguageListContainer::NAME)); 
    */

	CIMPropertyList propertyList(request->propertyList);

	SimpleInstanceResponseHandler handler;

	enumerateInstances(
	    request->operationContext,
	    objectPath,
	    request->includeQualifiers,
	    request->includeClassOrigin,
	    propertyList,
	    handler);

	// save returned instances
        cimInstances = handler.getObjects();
        contentLangs = handler.getLanguages();  // l10n
    }
    catch(CIMException & e)
    {
        cimException = e;
    }
    catch(Exception & e)
    {
        cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, e.getMessage());
    }
    catch(...)
    {
      // l10n

      cimException = PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED, MessageLoaderParms("Server.ProviderMessageFacade.UNKNOWN_ERROR", "Unknown Error"));
      // cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "Unknown Error");
    }

    // create response message
    AutoPtr<CIMEnumerateInstancesResponseMessage> response(
	new CIMEnumerateInstancesResponseMessage(
	    request->messageId,
	    cimException,
	    request->queueIds.copyAndPop(),
	    cimInstances));  // l10n

    STAT_PMS_PROVIDEREND

	response->operationContext.set(ContentLanguageListContainer(contentLangs));

    // preserve message key
    response->setKey(request->getKey());

    return response.release();
}

Message * ProviderMessageFacade::_handleEnumerateInstanceNamesRequest(Message * message) 
{
    

    const CIMEnumerateInstanceNamesRequestMessage * request =
	dynamic_cast<CIMEnumerateInstanceNamesRequestMessage *>(message);

    STAT_GETSTARTTIME

    PEGASUS_ASSERT(request != 0);

    CIMException cimException;
    Array<CIMObjectPath> cimReferences;
    ContentLanguages contentLangs;  // l10n

    try
    {
	// make target object path
	CIMObjectPath objectPath(
	    System::getHostName(),
	    request->nameSpace,
	    request->className);

	/* convert arguments
	OperationContext context;

	 add the user name and accept and content Languages to the context
	context.insert(request->operationContext.get(IdentityContainer::NAME));
	context.insert(request->operationContext.get(AcceptLanguageListContainer::NAME)); 
	context.insert(request->operationContext.get(ContentLanguageListContainer::NAME)); 
    */
	SimpleObjectPathResponseHandler handler;

	enumerateInstanceNames(
	    request->operationContext,
	    objectPath,
	    handler);

	// save returned instance
	cimReferences = handler.getObjects();
        contentLangs = handler.getLanguages();  // l10n
    }
    catch(CIMException & e)
    {
        cimException = e;
    }
    catch(Exception & e)
    {
        cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, e.getMessage());
    }
    catch(...)
    {
      // l10n

      cimException = PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED, MessageLoaderParms("Server.ProviderMessageFacade.UNKNOWN_ERROR", "Unknown Error"));
      // cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "Unknown Error");
    }

    // create response message
    AutoPtr<CIMEnumerateInstanceNamesResponseMessage> response(
	new CIMEnumerateInstanceNamesResponseMessage(
	    request->messageId,
	    cimException,
	    request->queueIds.copyAndPop(),
	    cimReferences));  //l10n

    STAT_PMS_PROVIDEREND

	response->operationContext.set(ContentLanguageListContainer(contentLangs));
    // preserve message key
    response->setKey(request->getKey());

    return response.release();
}

Message * ProviderMessageFacade::_handleCreateInstanceRequest(Message * message) 
{
    const CIMCreateInstanceRequestMessage * request =
	dynamic_cast<CIMCreateInstanceRequestMessage *>(message);

    STAT_GETSTARTTIME

    PEGASUS_ASSERT(request != 0);

    CIMException cimException;
    CIMInstance cimInstance;
    CIMObjectPath instanceName;
    ContentLanguages contentLangs;  // l10n

    try
    {
	// make target object path
	CIMObjectPath objectPath(
	    System::getHostName(),
	    request->nameSpace,
	    request->newInstance.getPath().getClassName(),
	    request->newInstance.getPath().getKeyBindings());

	/* convert arguments
	OperationContext context;

	// add the user name and accept and content Languages to the context
	context.insert(request->operationContext.get(IdentityContainer::NAME));
	context.insert(request->operationContext.get(AcceptLanguageListContainer::NAME)); 
	context.insert(request->operationContext.get(ContentLanguageListContainer::NAME)); 
    */
	SimpleObjectPathResponseHandler handler;

	// forward request
	createInstance(
	    request->operationContext,
	    objectPath,
	    request->newInstance,
	    handler);

	// error? provider claims success, but did not deliver an
	// instance name.
	if(handler.getObjects().size() == 0)
	{
//	    throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_FOUND, String::EMPTY);
	   // << Mon Apr 29 12:40:57 2002 mdd >>
	
	  // l10n

	  cimException = PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED, MessageLoaderParms("Server.ProviderMessageFacade.UNKNOWN_ERROR", "Unknown Error"));
	  // cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "Unknown Error");
	}

	// save returned instance name
	instanceName = handler.getObjects()[0];
        contentLangs = handler.getLanguages();  // l10n
    }
    catch(CIMException & e)
    {
        cimException = e;
    }
    catch(Exception & e)
    {
        cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, e.getMessage());
    }
    catch(...)
    {
      // l10n

      cimException = PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED, MessageLoaderParms("Server.ProviderMessageFacade.UNKNOWN_ERROR", "Unknown Error"));
      // cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "Unknown Error");
    }

    // create response message
    AutoPtr<CIMCreateInstanceResponseMessage> response(
	new CIMCreateInstanceResponseMessage(
	    request->messageId,
	    cimException,
	    request->queueIds.copyAndPop(),
	    instanceName));  // l10n

    STAT_PMS_PROVIDEREND

	response->operationContext.set(ContentLanguageListContainer(contentLangs));

    // preserve message key
    response->setKey(request->getKey());

    return response.release();
}

Message * ProviderMessageFacade::_handleModifyInstanceRequest(Message * message) 
{
    const CIMModifyInstanceRequestMessage * request =
	dynamic_cast<CIMModifyInstanceRequestMessage *>(message);

    STAT_GETSTARTTIME

    PEGASUS_ASSERT(request != 0);

    CIMException cimException;
    CIMObjectPath instanceName;

    //cout << "ProviderMessageFacade::_handleModifyInstanceRequest" << endl;


    try
    {
	// make target object path
	CIMObjectPath objectPath(
	    System::getHostName(),
	    request->nameSpace,
	    request->modifiedInstance.getPath ().getClassName(),
	    request->modifiedInstance.getPath ().getKeyBindings());

	// convert arguments
    /*	OperationContext context;

	// add the user name and accept and content Languages to the context
	context.insert(request->operationContext.get(IdentityContainer::NAME));
	context.insert(request->operationContext.get(AcceptLanguageListContainer::NAME)); 
	context.insert(request->operationContext.get(ContentLanguageListContainer::NAME)); 
    */
	CIMPropertyList propertyList(request->propertyList);

	SimpleResponseHandler handler;

	// forward request
	modifyInstance(
	    request->operationContext,
	    objectPath,
	    request->modifiedInstance,
	    request->includeQualifiers,
	    propertyList,
	    handler);
    }
    catch(CIMException & e)
    {
        cimException = e;
    }
    catch(Exception & e)
    {
        cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, e.getMessage());
    }
    catch(...)
    {
      // l10n

      cimException = PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED, MessageLoaderParms("Server.ProviderMessageFacade.UNKNOWN_ERROR", "Unknown Error"));
      //  cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "Unknown Error");
    }

    // create response message
    AutoPtr<CIMModifyInstanceResponseMessage> response(
	new CIMModifyInstanceResponseMessage(
	    request->messageId,
	    cimException,
	    request->queueIds.copyAndPop()));

    STAT_PMS_PROVIDEREND

    // preserve message key
    response->setKey(request->getKey());

    return response.release();
}

Message * ProviderMessageFacade::_handleDeleteInstanceRequest(Message * message) 
{
    const CIMDeleteInstanceRequestMessage * request =
	dynamic_cast<CIMDeleteInstanceRequestMessage *>(message);

    STAT_GETSTARTTIME

    PEGASUS_ASSERT(request != 0);

    CIMException cimException;

    try
    {
	// make target object path
	CIMObjectPath objectPath(
	    System::getHostName(),
	    request->nameSpace,
	    request->instanceName.getClassName(),
	    request->instanceName.getKeyBindings());

	// convert arguments
    /*	OperationContext context;

	// add the user name and accept and content Languages to the context
	context.insert(request->operationContext.get(IdentityContainer::NAME));
	context.insert(request->operationContext.get(AcceptLanguageListContainer::NAME)); 
	context.insert(request->operationContext.get(ContentLanguageListContainer::NAME)); 
    */
	SimpleResponseHandler handler;

	// forward request
	deleteInstance(
	    request->operationContext,
	    objectPath,
	    handler);
    }
    catch(CIMException & e)
    {
        cimException = e;
    }
    catch(Exception & e)
    {
        cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, e.getMessage());
    }
    catch(...)
    {
      // l10n

      cimException = PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED, MessageLoaderParms("Server.ProviderMessageFacade.UNKNOWN_ERROR", "Unknown Error"));
      // cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "Unknown Error");
    }

    // create response message
    AutoPtr<CIMDeleteInstanceResponseMessage> response(
	new CIMDeleteInstanceResponseMessage(
	    request->messageId,
	    cimException,
	    request->queueIds.copyAndPop()));

    STAT_PMS_PROVIDEREND

    // preserve message key
    response->setKey(request->getKey());

    return response.release();
}

Message * ProviderMessageFacade::_handleExecuteQueryRequest(Message * message) 
{
    const CIMExecQueryRequestMessage * request =
        dynamic_cast<CIMExecQueryRequestMessage *>(message);

    STAT_GETSTARTTIME

    PEGASUS_ASSERT(request != 0);

    Array<CIMObject> cimObjects;

    // l10n

    AutoPtr<CIMExecQueryResponseMessage> response(
	new CIMExecQueryResponseMessage(
	    request->messageId,
	    PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED, MessageLoaderParms("Server.ProviderMessageFacade.NOT_IMPLEMENTED", "not implemented")),
	    request->queueIds.copyAndPop(),
	    cimObjects));

    // CIMExecQueryResponseMessage * response =
    // new CIMExecQueryResponseMessage(
    //    request->messageId,
    //    PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "not implemented"),
    //    request->queueIds.copyAndPop(),
    //    cimObjects);

    STAT_PMS_PROVIDEREND

    // preserve message key
    response->setKey(request->getKey());

    return response.release();
}

Message * ProviderMessageFacade::_handleAssociatorsRequest(Message * message) 
{
    const CIMAssociatorsRequestMessage * request =
        dynamic_cast<CIMAssociatorsRequestMessage *>(message);

    STAT_GETSTARTTIME

    PEGASUS_ASSERT(request != 0);

    CIMException cimException;
    Array<CIMObject> cimObjects;
    ContentLanguages contentLangs;  // l10n

    // l10n
    try
    {
	// make target object path
	CIMObjectPath objectPath(
	    System::getHostName(),
	    request->nameSpace,
	    request->objectName.getClassName(),
        request->objectName.getKeyBindings());

	// convert arguments
    /*	OperationContext context;

	// add the user name and accept and content Languages to the context
	context.insert(request->operationContext.get(IdentityContainer::NAME));
	context.insert(request->operationContext.get(AcceptLanguageListContainer::NAME)); 
	context.insert(request->operationContext.get(ContentLanguageListContainer::NAME)); 
    */
	CIMPropertyList propertyList(request->propertyList);

	SimpleObjectResponseHandler handler;

	associators(
	    request->operationContext,
	    objectPath,
        request->assocClass,
        request->resultClass,
        request->role,
        request->resultRole,
	    request->includeQualifiers,
	    request->includeClassOrigin,
	    propertyList,
	    handler);

	// save returned Objects
        cimObjects = handler.getObjects();
        contentLangs = handler.getLanguages();  // l10n
    }
    catch(CIMException & e)
    {
        cimException = e;
    }
    catch(Exception & e)
    {
        cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, e.getMessage());
    }
    catch(...)
    {
      // l10n

      cimException = PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED, MessageLoaderParms("Server.ProviderMessageFacade.UNKNOWN_ERROR", "Unknown Error"));
      // cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "Unknown Error");
    }

    // create response message
    AutoPtr<CIMAssociatorsResponseMessage> response(
	new CIMAssociatorsResponseMessage(
	    request->messageId,
	    cimException,
	    request->queueIds.copyAndPop(),
	    cimObjects));  // l10n

    STAT_PMS_PROVIDEREND

    response->operationContext.set(ContentLanguageListContainer(contentLangs));

    // preserve message key
    response->setKey(request->getKey());

    return response.release();
}

Message * ProviderMessageFacade::_handleAssociatorNamesRequest(Message * message) 
{
    const CIMAssociatorNamesRequestMessage * request =
        dynamic_cast<CIMAssociatorNamesRequestMessage *>(message);

    STAT_GETSTARTTIME

    PEGASUS_ASSERT(request != 0);

    CIMException cimException;
    Array<CIMObjectPath> cimReferences;
    ContentLanguages contentLangs;  // l10n

    try
    {
	// make target object path
	CIMObjectPath objectPath(
	    System::getHostName(),
	    request->nameSpace,
	    request->objectName.getClassName(),
        request->objectName.getKeyBindings());

	// convert arguments
    /*	OperationContext context;

	// add the user name and accept and content Languages to the context
	context.insert(request->operationContext.get(IdentityContainer::NAME));
	context.insert(request->operationContext.get(AcceptLanguageListContainer::NAME)); 
	context.insert(request->operationContext.get(ContentLanguageListContainer::NAME)); 
    */
	SimpleObjectPathResponseHandler handler;

	associatorNames(
	    request->operationContext,
	    objectPath,
        request->assocClass,
        request->resultClass,
        request->role,
        request->resultRole,
	    handler);

	// save returned Objects
        cimReferences = handler.getObjects();
        contentLangs = handler.getLanguages();  // l10n
    }
    catch(CIMException & e)
    {
        cimException = e;
    }
    catch(Exception & e)
    {
        cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, e.getMessage());
    }
    catch(...)
    {
      // l10n

      cimException = PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED, MessageLoaderParms("Server.ProviderMessageFacade.UNKNOWN_ERROR", "Unknown Error"));
      // cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "Unknown Error");
    }

    // create response message
    AutoPtr<CIMAssociatorNamesResponseMessage> response(
	new CIMAssociatorNamesResponseMessage(
	    request->messageId,
	    cimException,
	    request->queueIds.copyAndPop(),
	    cimReferences));  // l10n

    STAT_PMS_PROVIDEREND

    response->operationContext.set(ContentLanguageListContainer(contentLangs));

    // preserve message key
    response->setKey(request->getKey());

    return response.release();
}
Message * ProviderMessageFacade::_handleReferencesRequest(Message * message) 
{
    const CIMReferencesRequestMessage * request =
        dynamic_cast<CIMReferencesRequestMessage *>(message);

    STAT_GETSTARTTIME

    PEGASUS_ASSERT(request != 0);

    CIMException cimException;
    Array<CIMObject> cimObjects;
    ContentLanguages contentLangs;  // l10n

    // l10n

    try
    {
	// make target object path
	CIMObjectPath objectPath(
	    System::getHostName(),
	    request->nameSpace,
	    request->objectName.getClassName(),
        request->objectName.getKeyBindings());

	// convert arguments
    /*	OperationContext context;

	// add the user name and accept and content Languages to the context
	context.insert(request->operationContext.get(IdentityContainer::NAME));
	context.insert(request->operationContext.get(AcceptLanguageListContainer::NAME)); 
	context.insert(request->operationContext.get(ContentLanguageListContainer::NAME));  
    */
	CIMPropertyList propertyList(request->propertyList);

	SimpleObjectResponseHandler handler;

	references(
	    request->operationContext,
	    objectPath,
        request->resultClass,
        request->role,
	    request->includeQualifiers,
	    request->includeClassOrigin,
	    propertyList,
	    handler);

	// save returned Objects
        cimObjects = handler.getObjects();
        contentLangs = handler.getLanguages();  // l10n
    }
    catch(CIMException & e)
    {
        cimException = e;
    }
    catch(Exception & e)
    {
        cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, e.getMessage());
    }
    catch(...)
    {
      // l10n

      cimException = PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED, MessageLoaderParms("Server.ProviderMessageFacade.UNKNOWN_ERROR", "Unknown Error"));
      // cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "Unknown Error");
    }

    // create response message
    AutoPtr<CIMReferencesResponseMessage> response(
	new CIMReferencesResponseMessage(
	    request->messageId,
	    cimException,
	    request->queueIds.copyAndPop(),
	    cimObjects));  // l10n

    STAT_PMS_PROVIDEREND

    response->operationContext.set(ContentLanguageListContainer(contentLangs));
    // preserve message key
    response->setKey(request->getKey());

    return response.release();
}

Message * ProviderMessageFacade::_handleReferenceNamesRequest(Message * message) 
{
    const CIMReferenceNamesRequestMessage * request =
        dynamic_cast<CIMReferenceNamesRequestMessage *>(message);

    STAT_GETSTARTTIME

    PEGASUS_ASSERT(request != 0);

    CIMException cimException;
    Array<CIMObjectPath> cimReferences;
    ContentLanguages contentLangs;  // l10n

    try
    {
	// make target object path
	CIMObjectPath objectPath(
	    System::getHostName(),
	    request->nameSpace,
	    request->objectName.getClassName(),
        request->objectName.getKeyBindings());

	// convert arguments
    /*	OperationContext context;

	// add the user name and accept and content Languages to the context
	context.insert(request->operationContext.get(IdentityContainer::NAME));
	context.insert(request->operationContext.get(AcceptLanguageListContainer::NAME)); 
	context.insert(request->operationContext.get(ContentLanguageListContainer::NAME)); 
    */
	SimpleObjectPathResponseHandler handler;

	referenceNames(
	    request->operationContext,
	    objectPath,
        request->resultClass,
        request->role,
	    handler);

	// save returned Objects
        cimReferences = handler.getObjects();
        contentLangs = handler.getLanguages();  // l10n
    }
    catch(CIMException & e)
    {
        cimException = e;
    }
    catch(Exception & e)
    {
        cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, e.getMessage());
    }
    catch(...)
    {
      // l10n

      cimException = PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED, MessageLoaderParms("Server.ProviderMessageFacade.UNKNOWN_ERROR", "Unknown Error"));
      // cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "Unknown Error");
    }

    // create response message
    AutoPtr<CIMReferenceNamesResponseMessage> response(
	new CIMReferenceNamesResponseMessage(
	    request->messageId,
	    cimException,
	    request->queueIds.copyAndPop(),
	    cimReferences));  // l10n

    STAT_PMS_PROVIDEREND

    response->operationContext.set(ContentLanguageListContainer(contentLangs));

    // preserve message key
    response->setKey(request->getKey());

    return response.release();
}

Message * ProviderMessageFacade::_handleGetPropertyRequest(Message * message) 
{
    const CIMGetPropertyRequestMessage * request =
        dynamic_cast<CIMGetPropertyRequestMessage *>(message);

    STAT_GETSTARTTIME

    PEGASUS_ASSERT(request != 0);

    CIMValue cimValue;

    // l10n

    // create response message
    AutoPtr<CIMGetPropertyResponseMessage> response(
	new CIMGetPropertyResponseMessage(
	request->messageId,
	PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED, MessageLoaderParms("Server.ProviderMessageFacade.NOT_IMPLEMENTED", "not implemented")),
	request->queueIds.copyAndPop(),
	cimValue));

    // CIMGetPropertyResponseMessage * response =
    // new CIMGetPropertyResponseMessage(
    //request->messageId,
    //PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "not implemented"),
    //request->queueIds.copyAndPop(),
    //cimValue);

    STAT_PMS_PROVIDEREND

    // preserve message key
    response->setKey(request->getKey());

    return response.release();
}

Message * ProviderMessageFacade::_handleSetPropertyRequest(Message * message) 
{
    const CIMSetPropertyRequestMessage * request =
	dynamic_cast<CIMSetPropertyRequestMessage *>(message);

    STAT_GETSTARTTIME

    PEGASUS_ASSERT(request != 0);

    // l10n

    // create response message
    AutoPtr<CIMSetPropertyResponseMessage> response(
	new CIMSetPropertyResponseMessage(
	    request->messageId,
	    PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED, MessageLoaderParms("Server.ProviderMessageFacade.NOT_IMPLEMENTED", "not implemented")),
	    request->queueIds.copyAndPop()));

    // CIMSetPropertyResponseMessage * response =
    // new CIMSetPropertyResponseMessage(
    //    request->messageId,
    //    PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "not implemented"),
    //    request->queueIds.copyAndPop());

    // preserve message key

    STAT_PMS_PROVIDEREND

    response->setKey(request->getKey());

    return response.release();
}

Message * ProviderMessageFacade::_handleInvokeMethodRequest(Message * message) 
{
    const CIMInvokeMethodRequestMessage * request =
	dynamic_cast<CIMInvokeMethodRequestMessage *>(message);

    STAT_GETSTARTTIME

    PEGASUS_ASSERT(request != 0);

    CIMException cimException;
    CIMValue returnValue;
    Array<CIMParamValue> outParameters;
    CIMInstance cimInstance;
    ContentLanguages contentLangs;  // l10n

    try
    {
	// make target object path
	CIMObjectPath classReference(
	    System::getHostName(),
	    request->nameSpace,
	    request->instanceName.getClassName());

	// convert arguments
    /*	OperationContext context;

	// add the user name and accept and content Languages to the context
	context.insert(request->operationContext.get(IdentityContainer::NAME));
	context.insert(request->operationContext.get(AcceptLanguageListContainer::NAME)); 
	context.insert(request->operationContext.get(ContentLanguageListContainer::NAME)); 
    */
	CIMObjectPath instanceReference(request->instanceName);

	// ATTN: propagate namespace
	instanceReference.setNameSpace(request->nameSpace);

	SimpleMethodResultResponseHandler handler;

	// forward request
	invokeMethod(
	    request->operationContext,
	    instanceReference,
	    request->methodName,
	    request->inParameters,
	    handler);

	// error? provider claims success, but did not deliver a CIMValue.
        // ATTN-RK-20020903: Can the return value be null?
	//if(handler.getReturnValue().isNull())
	//{
//	    throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_FOUND, String::EMPTY);
// << Mon Apr 29 12:41:15 2002 mdd >>
	//    cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "Unknown Error");
	//}

	outParameters = handler.getParamValues();
	returnValue = handler.getReturnValue();
        contentLangs = handler.getLanguages();  // l10n
    }
    catch(CIMException & e)
    {
        cimException = e;
    }
    catch(Exception & e)
    {
        cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, e.getMessage());
    }
    catch(...)
    {
      // l10n

      cimException = PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED, MessageLoaderParms("Server.ProviderMessageFacade.UNKNOWN_ERROR", "Unknown Error"));
      // cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "Unknown Error");
    }

    // create response message
    AutoPtr<CIMInvokeMethodResponseMessage> response(
	new CIMInvokeMethodResponseMessage(
	    request->messageId,
	    cimException,
	    request->queueIds.copyAndPop(),
	    returnValue,
	    outParameters,
	    request->methodName));  // l10n

    STAT_PMS_PROVIDEREND

	response->operationContext.set(ContentLanguageListContainer(contentLangs));

    // preserve message key
    response->setKey(request->getKey());

    return response.release();
}

PEGASUS_NAMESPACE_END
