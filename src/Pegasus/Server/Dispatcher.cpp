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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include "Dispatcher.h"
#include <Pegasus/Repository/CIMRepository.h>
#include <Pegasus/Common/CIMOMHandle.h>
#include "ProviderTable.h"

PEGASUS_NAMESPACE_BEGIN

PEGASUS_USING_STD;

//#define DDD(X) X
#define DDD(X) // X

DDD(static const char* _DISPATCHER = "Dispatcher::";)

Dispatcher::Dispatcher(CIMRepository* repository)
    : _repository(repository)
{
    DDD(cout << _DISPATCHER << endl;)
}

Dispatcher::~Dispatcher()
{

}

CIMProvider* Dispatcher::_lookupProviderForClass(
    const String& nameSpace,
    const String& className)
{
    //----------------------------------------------------------------------
    // Look up the class:
    //----------------------------------------------------------------------

    CIMClass cimClass = _repository->getClass(nameSpace, className);
    DDD(cout << _DISPATCHER << "Lookup Provider for " << className << endl;)

    if (!cimClass)
	throw CIMException(CIM_ERR_INVALID_CLASS);

    // cimClass.print();

    //----------------------------------------------------------------------
    // Get the provider qualifier:
    //----------------------------------------------------------------------

    Uint32 pos = cimClass.findQualifier("provider");
    DDD(cout << _DISPATCHER << "Lookup Qualifier " << pos << endl;)

    if (pos == PEG_NOT_FOUND)
	return 0;

    CIMQualifier q = cimClass.getQualifier(pos);
    String providerId;

    q.getValue().get(providerId);
    DDD(cout << _DISPATCHER << "Provider " << providerId << endl;)

    //----------------------------------------------------------------------
    // Get the provider (initialize it if not already initialize)
    // ATTN: move this block so that it can be shared.
    //----------------------------------------------------------------------

    CIMProvider* provider = _providerTable.lookupProvider(providerId);

    if (!provider)
    {
	DDD(cout << _DISPATCHER << " Lookup Provider " << providerId << endl;)

        provider = _providerTable.loadProvider(providerId);

        if (!provider)
	    throw CIMException(CIM_ERR_FAILED);

	CIMOMHandle cimomHandle(this, _repository);
	provider->initialize(cimomHandle);
    }

    return provider;
}

////////////////////////////////////////////////////////////////////////////////

void Dispatcher::_enqueueResponse(
    CIMRequestMessage* request,
    CIMResponseMessage* response)
{
    // Use the same key as used in the request:

    response->setKey(request->getKey());

    // Lookup the message queue:

    MessageQueue* queue = MessageQueue::lookup(request->queueId);
    PEGASUS_ASSERT(queue != 0);

    // Enqueue the response:

    queue->enqueue(response);
}

void Dispatcher::handleEnqueue()
{
    Message* request = dequeue();

    if (!request)
	return;

    if (getenv("PEGASUS_TRACE"))
	request->print(cout);

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

	// ATTN: implement this!
	case CIM_GET_PROPERTY_REQUEST_MESSAGE:
	    break;

	// ATTN: implement this!
	case CIM_SET_PROPERTY_REQUEST_MESSAGE:
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

	// ATTN: implement this!
	case CIM_INVOKE_METHOD_REQUEST_MESSAGE:
	    break;
    }
}

void Dispatcher::handleGetClassRequest(
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
	    request->propertyList);
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
	cimClass);

    _enqueueResponse(request, response);

    delete request;
}

void Dispatcher::handleGetInstanceRequest(
    CIMGetInstanceRequestMessage* request)
{
    // ATTN: Need code here to expand partial instance!

    CIMStatusCode errorCode = CIM_ERR_SUCCESS;
    String errorDescription;
    CIMInstance cimInstance;

    try
    {
	CIMProvider* provider = _lookupProviderForClass(
	    request->nameSpace, request->instanceName.getClassName());

	if (provider)
	{
	    cimInstance = provider->getInstance(
		request->nameSpace,
		request->instanceName,
		request->localOnly,
		request->includeQualifiers,
		request->includeClassOrigin);
	}
	else
	{
	    cimInstance = _repository->getInstance(
		request->nameSpace,
		request->instanceName,
		request->localOnly,
		request->includeQualifiers,
		request->includeClassOrigin);
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
	cimInstance);

    _enqueueResponse(request, response);

    delete request;
}

void Dispatcher::handleDeleteClassRequest(
    CIMDeleteClassRequestMessage* request)
{
    CIMStatusCode errorCode = CIM_ERR_SUCCESS;
    String errorDescription;

    try
    {
	CIMProvider* provider = _lookupProviderForClass(
	    request->nameSpace, request->className);

	if (provider)
	{
	    provider->deleteClass(
		request->nameSpace,
		request->className);
	}
	else
	{
	    _repository->deleteClass(
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

    CIMDeleteClassResponseMessage* response =
	new CIMDeleteClassResponseMessage(
	    request->messageId,
	    errorCode,
	    errorDescription);

    _enqueueResponse(request, response);

    delete request;
}

void Dispatcher::handleDeleteInstanceRequest(
    CIMDeleteInstanceRequestMessage* request)
{
    CIMStatusCode errorCode = CIM_ERR_SUCCESS;
    String errorDescription;

    try
    {
	CIMProvider* provider = _lookupProviderForClass(
	    request->nameSpace, request->instanceName.getClassName());

	if (provider)
	{
	    provider->deleteInstance(
		request->nameSpace,
		request->instanceName);
	}
	else
	{
	    _repository->deleteInstance(
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
	    errorDescription);

    _enqueueResponse(request, response);

    delete request;
}

void Dispatcher::handleCreateClassRequest(
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
	    errorDescription);

    _enqueueResponse(request, response);

    delete request;
}

void Dispatcher::handleCreateInstanceRequest(
    CIMCreateInstanceRequestMessage* request)
{
    CIMStatusCode errorCode = CIM_ERR_SUCCESS;
    String errorDescription;

    try
    {
	CIMProvider* provider = _lookupProviderForClass(
	    request->nameSpace, request->newInstance.getClassName());

	if (provider)
	{
	    provider->createInstance(
		request->nameSpace,
		request->newInstance);
	}
	else
	{
	    _repository->createInstance(
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
	    errorDescription);

    _enqueueResponse(request, response);

    delete request;
}

void Dispatcher::handleModifyClassRequest(
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
	    errorDescription);

    _enqueueResponse(request, response);

    delete request;
}

void Dispatcher::handleModifyInstanceRequest(
    CIMModifyInstanceRequestMessage* request)
{
    CIMStatusCode errorCode = CIM_ERR_SUCCESS;
    String errorDescription;

    try
    {
	_repository->modifyInstance(
	    request->nameSpace,
	    request->modifiedInstance);
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
	    errorDescription);

    _enqueueResponse(request, response);

    delete request;
}

void Dispatcher::handleEnumerateClassesRequest(
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
	    cimClasses);

    _enqueueResponse(request, response);

    delete request;
}

void Dispatcher::handleEnumerateClassNamesRequest(
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
	    classNames);

    _enqueueResponse(request, response);

    delete request;
}

void Dispatcher::handleEnumerateInstancesRequest(
    CIMEnumerateInstancesRequestMessage* request)
{
    CIMStatusCode errorCode = CIM_ERR_SUCCESS;
    String errorDescription;
    Array<CIMInstance> cimInstances;

    try
    {
	CIMProvider* provider = _lookupProviderForClass(
	    request->nameSpace, request->className);

	if (provider)
	{
	    cimInstances = provider->enumerateInstances(
		request->nameSpace,
		request->className,
		request->deepInheritance,
		request->localOnly,
		request->includeQualifiers,
		request->includeClassOrigin,
		request->propertyList);
	}
	else
	{
	    cimInstances = _repository->enumerateInstances(
		request->nameSpace,
		request->className,
		request->deepInheritance,
		request->localOnly,
		request->includeQualifiers,
		request->includeClassOrigin,
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

    CIMEnumerateInstancesResponseMessage* response =
	new CIMEnumerateInstancesResponseMessage(
	    request->messageId,
	    errorCode,
	    errorDescription,
	    cimInstances);

    _enqueueResponse(request, response);

    delete request;
}

void Dispatcher::handleEnumerateInstanceNamesRequest(
    CIMEnumerateInstanceNamesRequestMessage* request)
{
    CIMStatusCode errorCode = CIM_ERR_SUCCESS;
    String errorDescription;
    Array<CIMReference> instanceNames;

    try
    {
	CIMProvider* provider = _lookupProviderForClass(
	    request->nameSpace,
	    request->className);

	if (provider)
	{
	    instanceNames = provider->enumerateInstanceNames(
		request->nameSpace,
		request->className);
	}
	else
	{
	    instanceNames = _repository->enumerateInstanceNames(
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
	    instanceNames);

    _enqueueResponse(request, response);

    delete request;
}

void Dispatcher::handleAssociatorsRequest(
    CIMAssociatorsRequestMessage* request)
{
    CIMStatusCode errorCode = CIM_ERR_SUCCESS;
    String errorDescription;
    Array<CIMObjectWithPath> cimObjects;

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
	    request->propertyList);
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
	    cimObjects);

    _enqueueResponse(request, response);

    delete request;
}

void Dispatcher::handleAssociatorNamesRequest(
    CIMAssociatorNamesRequestMessage* request)
{
    CIMStatusCode errorCode = CIM_ERR_SUCCESS;
    String errorDescription;
    Array<CIMReference> objectNames;

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
	    objectNames);

    _enqueueResponse(request, response);

    delete request;
}

void Dispatcher::handleReferencesRequest(
    CIMReferencesRequestMessage* request)
{
    CIMStatusCode errorCode = CIM_ERR_SUCCESS;
    String errorDescription;
    Array<CIMObjectWithPath> cimObjects;

    try
    {
	cimObjects = _repository->references(
	    request->nameSpace,
	    request->objectName,
	    request->resultClass,
	    request->role,
	    request->includeQualifiers,
	    request->includeClassOrigin,
	    request->propertyList);
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
	    cimObjects);

    _enqueueResponse(request, response);

    delete request;
}

void Dispatcher::handleReferenceNamesRequest(
    CIMReferenceNamesRequestMessage* request)
{
    CIMStatusCode errorCode = CIM_ERR_SUCCESS;
    String errorDescription;
    Array<CIMReference> objectNames;

    try
    {
	objectNames = _repository->referenceNames(
	    request->nameSpace,
	    request->objectName,
	    request->resultClass,
	    request->role);
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
	    objectNames);

    _enqueueResponse(request, response);

    delete request;
}

void Dispatcher::handleGetQualifierRequest(
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
	    cimQualifierDecl);

    _enqueueResponse(request, response);

    delete request;
}

void Dispatcher::handleSetQualifierRequest(
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
	    errorDescription);

    _enqueueResponse(request, response);

    delete request;
}

void Dispatcher::handleDeleteQualifierRequest(
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
	    errorDescription);

    _enqueueResponse(request, response);

    delete request;
}

void Dispatcher::handleEnumerateQualifiersRequest(
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
	    qualifierDeclarations);

    _enqueueResponse(request, response);

    delete request;
}

PEGASUS_NAMESPACE_END
