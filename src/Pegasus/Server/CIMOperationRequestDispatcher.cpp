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
// Modified By: Nag Boranna (nagaraja_boranna@hp.com)
//              Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com) 
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Repository/CIMRepository.h>
#include <Pegasus/Common/CIMOMHandle.h>
#include <Pegasus/Provider2/CIMMethodProvider.h>

#include "CIMOperationRequestDispatcher.h"
#include "ProviderTable.h"

PEGASUS_NAMESPACE_BEGIN

PEGASUS_USING_STD;

//#define DDD(X) X
#define DDD(X) // X

DDD(static const char* _DISPATCHER = "CIMOperationRequestDispatcher::";)

CIMOperationRequestDispatcher::CIMOperationRequestDispatcher(CIMRepository* repository)
    : _repository(repository)
{
    DDD(cout << _DISPATCHER << endl;)
}

CIMOperationRequestDispatcher::~CIMOperationRequestDispatcher()
{

}

CIMProvider* CIMOperationRequestDispatcher::_lookupProviderForClass(
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

	case CIM_INVOKE_METHOD_REQUEST_MESSAGE:
	    handleInvokeMethodRequest(
		(CIMInvokeMethodRequestMessage*)request);
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
	request->queueIds.copyAndPop(),
	cimInstance);

    _enqueueResponse(request, response);
}

void CIMOperationRequestDispatcher::handleDeleteClassRequest(
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
	    errorDescription,
	    request->queueIds.copyAndPop());

    _enqueueResponse(request, response);
}

void CIMOperationRequestDispatcher::handleCreateClassRequest(
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
	    errorDescription,
	    request->queueIds.copyAndPop());

    _enqueueResponse(request, response);
}

void CIMOperationRequestDispatcher::handleCreateInstanceRequest(
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
	    errorDescription,
	    request->queueIds.copyAndPop());

    _enqueueResponse(request, response);
}

void CIMOperationRequestDispatcher::handleModifyClassRequest(
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
	    errorDescription,
	    request->queueIds.copyAndPop());

    _enqueueResponse(request, response);
}

void CIMOperationRequestDispatcher::handleModifyInstanceRequest(
    CIMModifyInstanceRequestMessage* request)
{
    CIMStatusCode errorCode = CIM_ERR_SUCCESS;
    String errorDescription;

    try
    {
// HP-Nag
        CIMProvider* provider = _lookupProviderForClass(
            request->nameSpace, request->modifiedInstance.getClassName());

        if (provider)
        {
            provider->modifyInstance(
                request->nameSpace,
                request->modifiedInstance);
        }
// HP-Nag
        else
        {
            _repository->modifyInstance(
                request->nameSpace,
                request->modifiedInstance);
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
	    request->queueIds.copyAndPop(),
	    classNames);

    _enqueueResponse(request, response);
}

void CIMOperationRequestDispatcher::handleEnumerateInstancesRequest(
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
	    request->queueIds.copyAndPop(),
	    cimInstances);

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
	CIMProvider* provider = _lookupProviderForClass(
	    request->nameSpace, request->objectName.getClassName());

	if (provider) {
	    cimObjects = provider->associators(
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
	else {
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
	CIMProvider* provider = _lookupProviderForClass(
	    request->nameSpace, request->objectName.getClassName());

	if (provider) {
	    objectNames = provider->associatorNames(
	        request->nameSpace,
	        request->objectName,
	        request->assocClass,
	        request->resultClass,
	        request->role,
	        request->resultRole);
        }
	else {
	    objectNames = _repository->associatorNames(
	        request->nameSpace,
	        request->objectName,
	        request->assocClass,
	        request->resultClass,
	        request->role,
	        request->resultRole);
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
	CIMProvider* provider = _lookupProviderForClass(
	    request->nameSpace, request->objectName.getClassName());

	if (provider) {
	    cimObjects = provider->references(
	        request->nameSpace,
	        request->objectName,
	        request->resultClass,
	        request->role,
	        request->includeQualifiers,
	        request->includeClassOrigin,
	        request->propertyList);
        }
	else {
	    cimObjects = _repository->references(
	        request->nameSpace,
	        request->objectName,
	        request->resultClass,
	        request->role,
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
	CIMProvider* provider = _lookupProviderForClass(
	    request->nameSpace, request->objectName.getClassName());

	if (provider) {
	    objectNames = provider->referenceNames(
	        request->nameSpace,
	        request->objectName,
	        request->resultClass,
	        request->role);
        }
	else {
	    objectNames = _repository->referenceNames(
	        request->nameSpace,
	        request->objectName,
	        request->resultClass,
	        request->role);
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

void CIMOperationRequestDispatcher::handleGetQualifierRequest(
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
	    request->queueIds.copyAndPop(),
	    cimQualifierDecl);

    _enqueueResponse(request, response);
}

void CIMOperationRequestDispatcher::handleSetQualifierRequest(
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
	    errorDescription,
	    request->queueIds.copyAndPop());

    _enqueueResponse(request, response);
}

void CIMOperationRequestDispatcher::handleDeleteQualifierRequest(
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
    
    Array<CIMValue> outParams;
    Array<CIMValue> inParams;
    
    try
    {
	CIMProvider* provider = _lookupProviderForClass(
    	    request->nameSpace, request->instanceName.getClassName());

    	// converting Array<CIMargument> to Array<CIMvalue>
	for (Uint8 i = 0; i < request->inParameters.size(); i++)
	    inParams.append(request->inParameters[i].getValue());

	if (provider)
	{ 
	    retValue = provider->invokeMethod(
		request->nameSpace,
		request->instanceName, 
		request->methodName,
		inParams, 
		outParams);
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

    // converting Array<CIMvalue> to Array<CIMvalue> Array<CIMargument>
    /*for (Uint8 j = 0; j < outParams.size(); j++)
    {
	outParameters.append(CIMParamValue(
	    CIMParameter("ATTN: What's name?", outParams[j].getType()), 
	    outParams[j]));
    }*/

    // ATTN: Assuming provider returned true and following parameters
    outParameters.append(CIMParamValue(
	CIMParameter("param1", CIMType::STRING), 
	CIMValue("HP")));
    outParameters.append(CIMParamValue(
	CIMParameter("param2", CIMType::STRING), 
	CIMValue("CA")));
    
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

PEGASUS_NAMESPACE_END
