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
// Author: Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include "ProviderMessageFacade.h"
#include <Pegasus/Provider/OperationFlag.h>
#include <Pegasus/Provider/SimpleResponseHandler.h>
#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Common/OperationContext.h>
#include <Pegasus/Common/Tracer.h>

PEGASUS_NAMESPACE_BEGIN

ProviderMessageFacade::ProviderMessageFacade(CIMBaseProvider* provider)
    : ProviderFacade(provider)
{
}

ProviderMessageFacade::~ProviderMessageFacade(void)
{
}


Message * ProviderMessageFacade::handleRequestMessage(Message * message) throw()
{
    // pass the request message to a handler method based on message type
    switch(message->getType())
    {
    case CIM_GET_INSTANCE_REQUEST_MESSAGE:
        return _handleGetInstanceRequest(message);
        break;
    case CIM_ENUMERATE_INSTANCES_REQUEST_MESSAGE:
	return _handleEnumerateInstancesRequest(message);
	break;
    case CIM_ENUMERATE_INSTANCE_NAMES_REQUEST_MESSAGE:
	return _handleEnumerateInstanceNamesRequest(message);
	break;
    case CIM_CREATE_INSTANCE_REQUEST_MESSAGE:
	return _handleCreateInstanceRequest(message);
	break;
    case CIM_MODIFY_INSTANCE_REQUEST_MESSAGE:
	return _handleModifyInstanceRequest(message);
	break;
    case CIM_DELETE_INSTANCE_REQUEST_MESSAGE:
	return _handleDeleteInstanceRequest(message);
	break;
    case CIM_EXEC_QUERY_REQUEST_MESSAGE:
	return _handleExecuteQueryRequest(message);
	break;
    case CIM_ASSOCIATORS_REQUEST_MESSAGE:
	return _handleAssociatorsRequest(message);
	break;
    case CIM_ASSOCIATOR_NAMES_REQUEST_MESSAGE:
	return _handleAssociatorNamesRequest(message);
	break;
    case CIM_REFERENCES_REQUEST_MESSAGE:
	return _handleReferencesRequest(message);
	break;
    case CIM_REFERENCE_NAMES_REQUEST_MESSAGE:
	return _handleReferenceNamesRequest(message);
	break;
    case CIM_GET_PROPERTY_REQUEST_MESSAGE:
	return _handleGetPropertyRequest(message);
	break;
    case CIM_SET_PROPERTY_REQUEST_MESSAGE:
	return _handleSetPropertyRequest(message);
	break;
    case CIM_INVOKE_METHOD_REQUEST_MESSAGE:
	return _handleInvokeMethodRequest(message);
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

    return(0);
}

Message * ProviderMessageFacade::_handleGetInstanceRequest(Message * message) throw()
{
    const CIMGetInstanceRequestMessage * request =
	dynamic_cast<CIMGetInstanceRequestMessage *>(message);

    PEGASUS_ASSERT(request != 0);

    CIMException cimException;
    CIMInstance cimInstance;

    try
    {
	// make target object path
	CIMObjectPath objectPath(
	    System::getHostName(),
	    request->nameSpace,
	    request->instanceName.getClassName(),
	    request->instanceName.getKeyBindings());

	// convert arguments
	OperationContext context;

	// add the user name to the context
	context.add_context(sizeof(String *),
	    const_cast<String *>(&(request->userName)),
	    0,
	    0,
	    CONTEXT_IDENTITY,
	    0,
	    0);

	// convert flags to bitmask
	Uint32 flags = OperationFlag::convert(false);

	// ATTN: strip flags inappropriate for providers
	flags = flags & ~OperationFlag::LOCAL_ONLY & ~OperationFlag::DEEP_INHERITANCE;

	CIMPropertyList propertyList(request->propertyList);

	SimpleResponseHandler<CIMInstance> handler;

	// forward request
	getInstance(
	    context,
	    objectPath,
	    flags,
	    propertyList,
	    handler);

	// error? provider claims success, but did not deliver an instance.
	if(handler.getObjects().size() == 0)
	{
	    throw CIMException(CIM_ERR_NOT_FOUND);
	}

	// save returned instance
	cimInstance = handler.getObjects()[0];
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
        cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "Unknown Error");
    }

    // create response message
    CIMGetInstanceResponseMessage * response =
	new CIMGetInstanceResponseMessage(
	    request->messageId,
	    cimException,
	    request->queueIds.copyAndPop(),
	    cimInstance);

    // preserve message key
    response->setKey(request->getKey());

    return response;
}

Message * ProviderMessageFacade::_handleEnumerateInstancesRequest(Message * message) throw()
{
    const CIMEnumerateInstancesRequestMessage * request =
	dynamic_cast<CIMEnumerateInstancesRequestMessage *>(message);

    PEGASUS_ASSERT(request != 0);

    CIMException cimException;
    Array<CIMNamedInstance> cimInstances;

    try
    {
	// make target object path
	CIMObjectPath objectPath(
	    System::getHostName(),
	    request->nameSpace,
	    request->className);

	// convert arguments
	OperationContext context;

	// add the user name to the context
	context.add_context(sizeof(String *),
	    const_cast<String *>(&(request->userName)),
	    0,
	    0,
	    CONTEXT_IDENTITY,
	    0,
	    0);

	// convert flags to bitmask
	Uint32 flags = OperationFlag::convert(false);

	// ATTN: strip flags inappropriate for providers
	flags = flags & ~OperationFlag::LOCAL_ONLY & ~OperationFlag::DEEP_INHERITANCE;

	CIMPropertyList propertyList(request->propertyList);

	SimpleResponseHandler<CIMInstance> handler;

	enumerateInstances(
	    context,
	    objectPath,
	    flags,
	    propertyList,
	    handler);

	// save returned instance

	// ATTN: can be removed once CIMNamedInstance is removed
	for(Uint32 i = 0, n = handler.getObjects().size(); i < n; i++)
	{
	    cimInstances.append(CIMNamedInstance(handler.getObjects()[i].getPath(), handler.getObjects()[i]));
	}
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
        cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "Unknown Error");
    }

    // create response message
    CIMEnumerateInstancesResponseMessage * response =
	new CIMEnumerateInstancesResponseMessage(
	    request->messageId,
	    cimException,
	    request->queueIds.copyAndPop(),
	    cimInstances);

    // preserve message key
    response->setKey(request->getKey());

    return response;
}

Message * ProviderMessageFacade::_handleEnumerateInstanceNamesRequest(Message * message) throw()
{
    const CIMEnumerateInstanceNamesRequestMessage * request =
	dynamic_cast<CIMEnumerateInstanceNamesRequestMessage *>(message);

    PEGASUS_ASSERT(request != 0);

    CIMException cimException;
    Array<CIMReference> cimReferences;

    try
    {
	// make target object path
	CIMObjectPath objectPath(
	    System::getHostName(),
	    request->nameSpace,
	    request->className);

	// convert arguments
	OperationContext context;

	// add the user name to the context
	context.add_context(sizeof(String *),
	    const_cast<String *>(&(request->userName)),
	    0,
	    0,
	    CONTEXT_IDENTITY,
	    0,
	    0);

	SimpleResponseHandler<CIMReference> handler;

	enumerateInstanceNames(
	    context,
	    objectPath,
	    handler);

	// save returned instance
	cimReferences = handler.getObjects();
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
        cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "Unknown Error");
    }

    // create response message
    CIMEnumerateInstanceNamesResponseMessage * response =
	new CIMEnumerateInstanceNamesResponseMessage(
	    request->messageId,
	    cimException,
	    request->queueIds.copyAndPop(),
	    cimReferences);

    // preserve message key
    response->setKey(request->getKey());

    return response;
}

Message * ProviderMessageFacade::_handleCreateInstanceRequest(Message * message) throw()
{
    const CIMCreateInstanceRequestMessage * request =
	dynamic_cast<CIMCreateInstanceRequestMessage *>(message);

    PEGASUS_ASSERT(request != 0);

    CIMException cimException;
    CIMInstance cimInstance;
    CIMReference instanceName;

    try
    {
	// make target object path
	CIMObjectPath objectPath(
	    System::getHostName(),
	    request->nameSpace,
	    request->newInstance.getPath().getClassName(),
	    request->newInstance.getPath().getKeyBindings());

	// convert arguments
	OperationContext context;

	// add the user name to the context
	context.add_context(sizeof(String *),
	    const_cast<String *>(&(request->userName)),
	    0,
	    0,
	    CONTEXT_IDENTITY,
	    0,
	    0);

	SimpleResponseHandler<CIMReference> handler;

	// forward request
	createInstance(
	    context,
	    objectPath,
	    request->newInstance,
	    handler);

	// error? provider claims success, but did not deliver an
	// instance name.
	if(handler.getObjects().size() == 0)
	{
	    throw CIMException(CIM_ERR_NOT_FOUND);
	}

	// save returned instance name
	instanceName = handler.getObjects()[0];
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
        cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "Unknown Error");
    }

    // create response message
    CIMCreateInstanceResponseMessage * response =
	new CIMCreateInstanceResponseMessage(
	    request->messageId,
	    cimException,
	    request->queueIds.copyAndPop(),
	    instanceName);

    // preserve message key
    response->setKey(request->getKey());

    return response;
}

Message * ProviderMessageFacade::_handleModifyInstanceRequest(Message * message) throw()
{
    const CIMModifyInstanceRequestMessage * request =
	dynamic_cast<CIMModifyInstanceRequestMessage *>(message);

    PEGASUS_ASSERT(request != 0);

    CIMException cimException;
    CIMReference instanceName;

    try
    {
	// make target object path
        // ATTN-RK-P2-20020329: The KeyBinding array does not get set
        // [correctly] here.
	CIMObjectPath objectPath(
	    System::getHostName(),
	    request->nameSpace,
	    request->modifiedInstance.getInstanceName().getClassName(),
	    request->modifiedInstance.getInstanceName().getKeyBindings());
// ATTN:    request->modifiedInstance.getInstance().getPath().getClassName(),
// ATTN:    request->modifiedInstance.getInstance().getPath().getKeyBindings());

	// convert arguments
	OperationContext context;

	// add the user name to the context
	context.add_context(sizeof(String *),
	    const_cast<String *>(&(request->userName)),
	    0,
	    0,
	    CONTEXT_IDENTITY,
	    0,
	    0);

	// convert flags to bitmask
	Uint32 flags = OperationFlag::convert(false);

	// ATTN: strip flags inappropriate for providers
	flags = flags & ~OperationFlag::LOCAL_ONLY & ~OperationFlag::DEEP_INHERITANCE;

	CIMPropertyList propertyList(request->propertyList);

	SimpleResponseHandler<CIMInstance> handler;

	// forward request
	modifyInstance(
	    context,
	    objectPath,
	    request->modifiedInstance.getInstance(),
	    flags,
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
        cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "Unknown Error");
    }

    // create response message
    CIMModifyInstanceResponseMessage * response =
	new CIMModifyInstanceResponseMessage(
	    request->messageId,
	    cimException,
	    request->queueIds.copyAndPop());

    // preserve message key
    response->setKey(request->getKey());

    return response;
}

Message * ProviderMessageFacade::_handleDeleteInstanceRequest(Message * message) throw()
{
    const CIMDeleteInstanceRequestMessage * request =
	dynamic_cast<CIMDeleteInstanceRequestMessage *>(message);

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
	OperationContext context;

	context.add_context(sizeof(String *),
	    const_cast<String *>(&(request->userName)),
	    0,
	    0,
	    CONTEXT_IDENTITY,
	    0,
	    0);

	SimpleResponseHandler<CIMInstance> handler;

	// forward request
	deleteInstance(
	    context,
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
        cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "Unknown Error");
    }

    // create response message
    CIMDeleteInstanceResponseMessage * response =
	new CIMDeleteInstanceResponseMessage(
	    request->messageId,
	    cimException,
	    request->queueIds.copyAndPop());

    // preserve message key
    response->setKey(request->getKey());

    return response;
}

Message * ProviderMessageFacade::_handleExecuteQueryRequest(Message * message) throw()
{
    const CIMExecQueryRequestMessage * request =
	dynamic_cast<CIMExecQueryRequestMessage *>(message);

    PEGASUS_ASSERT(request != 0);

    Array<CIMObjectWithPath> cimObjects;

    CIMExecQueryResponseMessage * response =
	new CIMExecQueryResponseMessage(
	    request->messageId,
	    PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "not implemented"),
	    request->queueIds.copyAndPop(),
	    cimObjects);

    // preserve message key
    response->setKey(request->getKey());

    return response;
}

Message * ProviderMessageFacade::_handleAssociatorsRequest(Message * message) throw()
{
    const CIMAssociatorsRequestMessage * request =
	dynamic_cast<CIMAssociatorsRequestMessage *>(message);

    PEGASUS_ASSERT(request != 0);

    Array<CIMObjectWithPath> cimObjects;

    CIMAssociatorsResponseMessage * response =
	new CIMAssociatorsResponseMessage(
	    request->messageId,
	    PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "not implemented"),
	    request->queueIds.copyAndPop(),
	    cimObjects);

    // preserve message key
    response->setKey(request->getKey());

    return response;
}

Message * ProviderMessageFacade::_handleAssociatorNamesRequest(Message * message) throw()
{
    const CIMAssociatorNamesRequestMessage * request =
	dynamic_cast<CIMAssociatorNamesRequestMessage *>(message);

    PEGASUS_ASSERT(request != 0);

    Array<CIMReference> cimReferences;

    CIMAssociatorNamesResponseMessage * response =
	new CIMAssociatorNamesResponseMessage(
	    request->messageId,
	    PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "not implemented"),
	    request->queueIds.copyAndPop(),
	    cimReferences);

    // preserve message key
    response->setKey(request->getKey());

    return response;
}

Message * ProviderMessageFacade::_handleReferencesRequest(Message * message) throw()
{
    const CIMReferencesRequestMessage * request =
	dynamic_cast<CIMReferencesRequestMessage *>(message);

    PEGASUS_ASSERT(request != 0);

    Array<CIMObjectWithPath> cimObjects;

    CIMReferencesResponseMessage * response =
	new CIMReferencesResponseMessage(
	    request->messageId,
	    PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "not implemented"),
	    request->queueIds.copyAndPop(),
	    cimObjects);

    // preserve message key
    response->setKey(request->getKey());

    return response;
}

Message * ProviderMessageFacade::_handleReferenceNamesRequest(Message * message) throw()
{
    const CIMReferenceNamesRequestMessage * request =
	dynamic_cast<CIMReferenceNamesRequestMessage *>(message);

    PEGASUS_ASSERT(request != 0);

    Array<CIMReference> cimReferences;

    CIMReferenceNamesResponseMessage * response =
	new CIMReferenceNamesResponseMessage(
	    request->messageId,
	    PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "not implemented"),
	    request->queueIds.copyAndPop(),
	    cimReferences);

    // preserve message key
    response->setKey(request->getKey());

    return response;
}

Message * ProviderMessageFacade::_handleGetPropertyRequest(Message * message) throw()
{
    const CIMGetPropertyRequestMessage * request =
	dynamic_cast<CIMGetPropertyRequestMessage *>(message);

    PEGASUS_ASSERT(request != 0);

    CIMValue cimValue;

    // create response message
    CIMGetPropertyResponseMessage * response =
	new CIMGetPropertyResponseMessage(
	request->messageId,
	PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "not implemented"),
	request->queueIds.copyAndPop(),
	cimValue);

    // preserve message key
    response->setKey(request->getKey());

    return response;
}

Message * ProviderMessageFacade::_handleSetPropertyRequest(Message * message) throw()
{
    const CIMSetPropertyRequestMessage * request =
	dynamic_cast<CIMSetPropertyRequestMessage *>(message);

    PEGASUS_ASSERT(request != 0);

    // create response message
    CIMSetPropertyResponseMessage * response =
	new CIMSetPropertyResponseMessage(
	    request->messageId,
	    PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "not implemented"),
	    request->queueIds.copyAndPop());

    // preserve message key
    response->setKey(request->getKey());

    return response;
}

Message * ProviderMessageFacade::_handleInvokeMethodRequest(Message * message) throw()
{
    const CIMInvokeMethodRequestMessage * request =
	dynamic_cast<CIMInvokeMethodRequestMessage *>(message);

    PEGASUS_ASSERT(request != 0);

    CIMException cimException;
    CIMValue returnValue;
    Array<CIMParamValue> outParameters;
    CIMInstance cimInstance;

    try
    {
	// make target object path
	CIMReference classReference(
	    System::getHostName(),
	    request->nameSpace,
	    request->instanceName.getClassName());

	// convert arguments
	OperationContext context;

	// add the user name to the context
	context.add_context(sizeof(String *),
	    const_cast<String *>(&(request->userName)),
	    0,
	    0,
	    CONTEXT_IDENTITY,
	    0,
	    0);

	CIMObjectPath instanceReference(request->instanceName);

	// ATTN: propagate namespace
	instanceReference.setNameSpace(request->nameSpace);

	SimpleResponseHandler<CIMValue> handler;

	// forward request
	invokeMethod(
	    context,
	    instanceReference,
	    request->methodName,
	    request->inParameters,
	    outParameters,
	    handler);

	// error? provider claims success, but did not deliver a CIMValue.
	if(handler.getObjects().size() == 0)
	{
	    throw CIMException(CIM_ERR_NOT_FOUND);
	}

	returnValue = handler.getObjects()[0];
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
        cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "Unknown Error");
    }

    // create response message
    CIMInvokeMethodResponseMessage * response =
	new CIMInvokeMethodResponseMessage(
	    request->messageId,
	    cimException,
	    request->queueIds.copyAndPop(),
	    returnValue,
	    outParameters,
	    request->methodName);

    // preserve message key
    response->setKey(request->getKey());

    return response;
}

PEGASUS_NAMESPACE_END
