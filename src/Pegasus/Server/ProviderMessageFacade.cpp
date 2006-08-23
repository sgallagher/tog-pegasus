//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
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
//%/////////////////////////////////////////////////////////////////////////////

// This module provides the calling facade for control providers.
// Accepting CIMMessages on the input side it provides calls for each of
// the CIM provider operations to control providers setting up the
// appropriate context for each call and processing handler responses.
// NOTE: Today the execquery is NOT SUPPORTED by this facade.

#include "ProviderMessageFacade.h"

#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Common/OperationContext.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/Thread.h>
#include <Pegasus/Common/StatisticalData.h>
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
   	AcceptLanguageList *langs = new AcceptLanguageList(((AcceptLanguageListContainer)msg->operationContext.get
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
    PEGASUS_ASSERT(request != 0);

    AutoPtr<CIMGetInstanceResponseMessage> response(
        dynamic_cast<CIMGetInstanceResponseMessage*>(
            request->buildResponse()));
    PEGASUS_ASSERT(response.get() != 0);

    CIMException cimException;
    CIMInstance cimInstance;
    ContentLanguageList contentLangs;

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

        StatProviderTimeMeasurement providerTime(response.get());

	// forward request
	getInstance(
	    request->operationContext,
	    objectPath,
	    request->includeQualifiers,
	    request->includeClassOrigin,
	    propertyList,
	    handler);

	// error? provider claims success, but did not deliver an instance.
	if (handler.getObjects().size() == 0)
	{
	    cimException = PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
                MessageLoaderParms(
                    "Server.ProviderMessageFacade.UNKNOWN_ERROR",
                    "Unknown Error"));
	}

	// save returned instance
	cimInstance = handler.getObjects()[0];
        contentLangs = handler.getLanguages();
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
        cimException = PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
            MessageLoaderParms("Server.ProviderMessageFacade.UNKNOWN_ERROR",
                "Unknown Error"));
    }

    response->cimInstance = cimInstance;
    response->cimException = cimException;
    response->operationContext.set(ContentLanguageListContainer(contentLangs));

    return response.release();
}

Message * ProviderMessageFacade::_handleEnumerateInstancesRequest(Message * message) 
{
    const CIMEnumerateInstancesRequestMessage * request =
	dynamic_cast<CIMEnumerateInstancesRequestMessage *>(message);
    PEGASUS_ASSERT(request != 0);

    AutoPtr<CIMEnumerateInstancesResponseMessage> response(
        dynamic_cast<CIMEnumerateInstancesResponseMessage*>(
            request->buildResponse()));
    PEGASUS_ASSERT(response.get() != 0);

    CIMException cimException;
    Array<CIMInstance> cimInstances;
    ContentLanguageList contentLangs;

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

        StatProviderTimeMeasurement providerTime(response.get());

	enumerateInstances(
	    request->operationContext,
	    objectPath,
	    request->includeQualifiers,
	    request->includeClassOrigin,
	    propertyList,
	    handler);

	// save returned instances
        cimInstances = handler.getObjects();
        contentLangs = handler.getLanguages();
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
        cimException = PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
            MessageLoaderParms("Server.ProviderMessageFacade.UNKNOWN_ERROR",
                "Unknown Error"));
    }

    response->cimNamedInstances = cimInstances;
    response->cimException = cimException;
    response->operationContext.set(ContentLanguageListContainer(contentLangs));

    return response.release();
}

Message * ProviderMessageFacade::_handleEnumerateInstanceNamesRequest(Message * message) 
{
    const CIMEnumerateInstanceNamesRequestMessage * request =
	dynamic_cast<CIMEnumerateInstanceNamesRequestMessage *>(message);
    PEGASUS_ASSERT(request != 0);

    AutoPtr<CIMEnumerateInstanceNamesResponseMessage> response(
        dynamic_cast<CIMEnumerateInstanceNamesResponseMessage*>(
            request->buildResponse()));
    PEGASUS_ASSERT(response.get() != 0);

    CIMException cimException;
    Array<CIMObjectPath> cimReferences;
    ContentLanguageList contentLangs;

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

        StatProviderTimeMeasurement providerTime(response.get());

	enumerateInstanceNames(
	    request->operationContext,
	    objectPath,
	    handler);

	// save returned instance
	cimReferences = handler.getObjects();
        contentLangs = handler.getLanguages();
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
        cimException = PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
            MessageLoaderParms("Server.ProviderMessageFacade.UNKNOWN_ERROR",
                "Unknown Error"));
    }

    response->instanceNames = cimReferences;
    response->cimException = cimException;
    response->operationContext.set(ContentLanguageListContainer(contentLangs));

    return response.release();
}

Message * ProviderMessageFacade::_handleCreateInstanceRequest(Message * message) 
{
    const CIMCreateInstanceRequestMessage * request =
	dynamic_cast<CIMCreateInstanceRequestMessage *>(message);
    PEGASUS_ASSERT(request != 0);

    AutoPtr<CIMCreateInstanceResponseMessage> response(
        dynamic_cast<CIMCreateInstanceResponseMessage*>(
            request->buildResponse()));
    PEGASUS_ASSERT(response.get() != 0);

    CIMException cimException;
    CIMInstance cimInstance;
    CIMObjectPath instanceName;
    ContentLanguageList contentLangs;

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

        StatProviderTimeMeasurement providerTime(response.get());

	// forward request
	createInstance(
	    request->operationContext,
	    objectPath,
	    request->newInstance,
	    handler);

	// error? provider claims success, but did not deliver an
	// instance name.
	if (handler.getObjects().size() == 0)
	{
	    cimException = PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
                MessageLoaderParms(
                    "Server.ProviderMessageFacade.UNKNOWN_ERROR",
                    "Unknown Error"));
	}

	// save returned instance name
	instanceName = handler.getObjects()[0];
        contentLangs = handler.getLanguages();
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
        cimException = PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
            MessageLoaderParms("Server.ProviderMessageFacade.UNKNOWN_ERROR",
                "Unknown Error"));
    }

    response->instanceName = instanceName;
    response->cimException = cimException;
    response->operationContext.set(ContentLanguageListContainer(contentLangs));

    return response.release();
}

Message * ProviderMessageFacade::_handleModifyInstanceRequest(Message * message) 
{
    const CIMModifyInstanceRequestMessage * request =
	dynamic_cast<CIMModifyInstanceRequestMessage *>(message);
    PEGASUS_ASSERT(request != 0);

    AutoPtr<CIMModifyInstanceResponseMessage> response(
        dynamic_cast<CIMModifyInstanceResponseMessage*>(
            request->buildResponse()));
    PEGASUS_ASSERT(response.get() != 0);

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

        StatProviderTimeMeasurement providerTime(response.get());

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
        cimException = PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
            MessageLoaderParms("Server.ProviderMessageFacade.UNKNOWN_ERROR",
                "Unknown Error"));
    }

    response->cimException = cimException;
    return response.release();
}

Message * ProviderMessageFacade::_handleDeleteInstanceRequest(Message * message) 
{
    const CIMDeleteInstanceRequestMessage * request =
	dynamic_cast<CIMDeleteInstanceRequestMessage *>(message);
    PEGASUS_ASSERT(request != 0);

    AutoPtr<CIMDeleteInstanceResponseMessage> response(
        dynamic_cast<CIMDeleteInstanceResponseMessage*>(
            request->buildResponse()));
    PEGASUS_ASSERT(response.get() != 0);

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

        StatProviderTimeMeasurement providerTime(response.get());

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
        cimException = PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
            MessageLoaderParms("Server.ProviderMessageFacade.UNKNOWN_ERROR",
                "Unknown Error"));
    }

    response->cimException = cimException;

    return response.release();
}

Message * ProviderMessageFacade::_handleExecuteQueryRequest(Message * message) 
{
    const CIMExecQueryRequestMessage * request =
        dynamic_cast<CIMExecQueryRequestMessage *>(message);
    PEGASUS_ASSERT(request != 0);

    AutoPtr<CIMExecQueryResponseMessage> response(
        dynamic_cast<CIMExecQueryResponseMessage*>(
            request->buildResponse()));
    PEGASUS_ASSERT(response.get() != 0);

    response->cimException = PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
        MessageLoaderParms("Server.ProviderMessageFacade.NOT_IMPLEMENTED",
            "not implemented"));

    return response.release();
}

Message * ProviderMessageFacade::_handleAssociatorsRequest(Message * message) 
{
    const CIMAssociatorsRequestMessage * request =
        dynamic_cast<CIMAssociatorsRequestMessage *>(message);
    PEGASUS_ASSERT(request != 0);

    AutoPtr<CIMAssociatorsResponseMessage> response(
        dynamic_cast<CIMAssociatorsResponseMessage*>(
            request->buildResponse()));
    PEGASUS_ASSERT(response.get() != 0);

    CIMException cimException;
    Array<CIMObject> cimObjects;
    ContentLanguageList contentLangs;

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

        StatProviderTimeMeasurement providerTime(response.get());

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
        contentLangs = handler.getLanguages();
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
        cimException = PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
            MessageLoaderParms("Server.ProviderMessageFacade.UNKNOWN_ERROR",
                "Unknown Error"));
    }

    response->cimObjects = cimObjects;
    response->cimException = cimException;
    response->operationContext.set(ContentLanguageListContainer(contentLangs));

    return response.release();
}

Message * ProviderMessageFacade::_handleAssociatorNamesRequest(Message * message) 
{
    const CIMAssociatorNamesRequestMessage * request =
        dynamic_cast<CIMAssociatorNamesRequestMessage *>(message);
    PEGASUS_ASSERT(request != 0);

    AutoPtr<CIMAssociatorNamesResponseMessage> response(
        dynamic_cast<CIMAssociatorNamesResponseMessage*>(
            request->buildResponse()));
    PEGASUS_ASSERT(response.get() != 0);

    CIMException cimException;
    Array<CIMObjectPath> cimReferences;
    ContentLanguageList contentLangs;

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

        StatProviderTimeMeasurement providerTime(response.get());

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
        contentLangs = handler.getLanguages();
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
        cimException = PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
            MessageLoaderParms("Server.ProviderMessageFacade.UNKNOWN_ERROR",
                "Unknown Error"));
    }

    response->objectNames = cimReferences;
    response->cimException = cimException;
    response->operationContext.set(ContentLanguageListContainer(contentLangs));

    return response.release();
}

Message * ProviderMessageFacade::_handleReferencesRequest(Message * message) 
{
    const CIMReferencesRequestMessage * request =
        dynamic_cast<CIMReferencesRequestMessage *>(message);
    PEGASUS_ASSERT(request != 0);

    AutoPtr<CIMReferencesResponseMessage> response(
        dynamic_cast<CIMReferencesResponseMessage*>(
            request->buildResponse()));
    PEGASUS_ASSERT(response.get() != 0);

    CIMException cimException;
    Array<CIMObject> cimObjects;
    ContentLanguageList contentLangs;

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

        StatProviderTimeMeasurement providerTime(response.get());

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
        contentLangs = handler.getLanguages();
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
        cimException = PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
            MessageLoaderParms("Server.ProviderMessageFacade.UNKNOWN_ERROR",
                "Unknown Error"));
    }

    response->cimObjects = cimObjects;
    response->cimException = cimException;
    response->operationContext.set(ContentLanguageListContainer(contentLangs));

    return response.release();
}

Message * ProviderMessageFacade::_handleReferenceNamesRequest(Message * message) 
{
    const CIMReferenceNamesRequestMessage * request =
        dynamic_cast<CIMReferenceNamesRequestMessage *>(message);
    PEGASUS_ASSERT(request != 0);

    AutoPtr<CIMReferenceNamesResponseMessage> response(
        dynamic_cast<CIMReferenceNamesResponseMessage*>(
            request->buildResponse()));
    PEGASUS_ASSERT(response.get() != 0);

    CIMException cimException;
    Array<CIMObjectPath> cimReferences;
    ContentLanguageList contentLangs;

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

        StatProviderTimeMeasurement providerTime(response.get());

	referenceNames(
	    request->operationContext,
	    objectPath,
        request->resultClass,
        request->role,
	    handler);

	// save returned Objects
        cimReferences = handler.getObjects();
        contentLangs = handler.getLanguages();
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
        cimException = PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
            MessageLoaderParms("Server.ProviderMessageFacade.UNKNOWN_ERROR",
                "Unknown Error"));
    }

    response->objectNames = cimReferences;
    response->cimException = cimException;
    response->operationContext.set(ContentLanguageListContainer(contentLangs));

    return response.release();
}

Message * ProviderMessageFacade::_handleGetPropertyRequest(Message * message) 
{
    const CIMGetPropertyRequestMessage * request =
        dynamic_cast<CIMGetPropertyRequestMessage *>(message);
    PEGASUS_ASSERT(request != 0);

    AutoPtr<CIMGetPropertyResponseMessage> response(
        dynamic_cast<CIMGetPropertyResponseMessage*>(
            request->buildResponse()));
    PEGASUS_ASSERT(response.get() != 0);

    CIMValue cimValue;
    CIMException cimException;
    ContentLanguageList contentLangs;

    try
    {
        // make target object path
        CIMObjectPath objectPath(
            System::getHostName(),
            request->nameSpace,
            request->instanceName.getClassName(),
            request->instanceName.getKeyBindings());

        Array<CIMName> propertyListArray;
        propertyListArray.append(request->propertyName);
        CIMPropertyList propertyList(propertyListArray);

        SimpleInstanceResponseHandler handler;

        StatProviderTimeMeasurement providerTime(response.get());

        getInstance(
            request->operationContext,
            objectPath,
            false,
            false,
            propertyList,
            handler);

        if (handler.size() == 1)
        {
            // Retrieve the requested property
            CIMInstance cimInstance = handler.getObjects()[0];
            Uint32 pos = cimInstance.findProperty(request->propertyName);
            if (pos != PEG_NOT_FOUND)
            {
                cimValue = cimInstance.getProperty(pos).getValue();
            }
        }
        else
        {
            PEGASUS_ASSERT(0);
        }

        contentLangs = handler.getLanguages();
    }
    catch (CIMException& e)
    {
        cimException = e;
    }
    catch (Exception& e)
    {
        cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, e.getMessage());
    }
    catch (...)
    {
        cimException = PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
            MessageLoaderParms("Server.ProviderMessageFacade.UNKNOWN_ERROR",
                "Unknown Error"));
    }

    response->value = cimValue;
    response->cimException = cimException;
    response->operationContext.set(ContentLanguageListContainer(contentLangs));

    return response.release();
}

Message * ProviderMessageFacade::_handleSetPropertyRequest(Message * message) 
{
    const CIMSetPropertyRequestMessage * request =
	dynamic_cast<CIMSetPropertyRequestMessage *>(message);
    PEGASUS_ASSERT(request != 0);

    AutoPtr<CIMSetPropertyResponseMessage> response(
        dynamic_cast<CIMSetPropertyResponseMessage*>(
            request->buildResponse()));
    PEGASUS_ASSERT(response.get() != 0);

    CIMException cimException;

    try
    {
        // make target object path
        CIMObjectPath objectPath(
            System::getHostName(),
            request->nameSpace,
            request->instanceName.getClassName(),
            request->instanceName.getKeyBindings());

        CIMInstance cimInstance;
        cimInstance.addProperty(
            CIMProperty(request->propertyName, request->newValue));

        Array<CIMName> propertyListArray;
        propertyListArray.append(request->propertyName);
        CIMPropertyList propertyList(propertyListArray);

        SimpleResponseHandler handler;

        StatProviderTimeMeasurement providerTime(response.get());

        modifyInstance(
            request->operationContext,
            objectPath,
            cimInstance,
            false,
            propertyList,
            handler);
    }
    catch (CIMException& e)
    {
        cimException = e;
    }
    catch (Exception& e)
    {
        cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, e.getMessage());
    }
    catch (...)
    {
        cimException = PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
            MessageLoaderParms("Server.ProviderMessageFacade.UNKNOWN_ERROR",
                "Unknown Error"));
    }

    response->cimException = cimException;

    return response.release();
}

Message * ProviderMessageFacade::_handleInvokeMethodRequest(Message * message) 
{
    const CIMInvokeMethodRequestMessage * request =
	dynamic_cast<CIMInvokeMethodRequestMessage *>(message);
    PEGASUS_ASSERT(request != 0);

    AutoPtr<CIMInvokeMethodResponseMessage> response(
        dynamic_cast<CIMInvokeMethodResponseMessage*>(
            request->buildResponse()));
    PEGASUS_ASSERT(response.get() != 0);

    CIMException cimException;
    CIMValue returnValue;
    Array<CIMParamValue> outParameters;
    CIMInstance cimInstance;
    ContentLanguageList contentLangs;

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

        StatProviderTimeMeasurement providerTime(response.get());

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
        contentLangs = handler.getLanguages();
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
        cimException = PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,
            MessageLoaderParms("Server.ProviderMessageFacade.UNKNOWN_ERROR",
                "Unknown Error"));
    }

    response->retValue = returnValue;
    response->outParameters = outParameters;
    response->cimException = cimException;
    response->operationContext.set(ContentLanguageListContainer(contentLangs));

    return response.release();
}

PEGASUS_NAMESPACE_END
