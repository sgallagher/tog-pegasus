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

CIMClass Dispatcher::getClass(
    const String& nameSpace,
    const String& className,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const Array<String>& propertyList)
{
    // Obsolete!
    PEGASUS_ASSERT(0);
}

CIMInstance Dispatcher::getInstance(
    const String& nameSpace,
    const CIMReference& instanceName,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const Array<String>& propertyList)
{
    // Obsolete!
    PEGASUS_ASSERT(0);
}

void Dispatcher::deleteClass(
    const String& nameSpace,
    const String& className)
{
}

void Dispatcher::deleteInstance(
    const String& nameSpace,
    const CIMReference& instanceName)
{
    // Obsolete!
    PEGASUS_ASSERT(0);
}

void Dispatcher::createClass(
    const String& nameSpace,
    const CIMClass& newClass)
{
    _repository->createClass(nameSpace, newClass);
}

void Dispatcher::createInstance(
    const String& nameSpace,
    const CIMInstance& newInstance)
{
    String className = newInstance.getClassName();
    CIMProvider* provider = _lookupProviderForClass(nameSpace, className);
    DDD(cout << _DISPATCHER << "createInstance of " << className << endl;)

    if (provider)
	provider->createInstance(nameSpace, newInstance);
    else
	_repository->createInstance(nameSpace, newInstance);
}

void Dispatcher::modifyClass(
    const String& nameSpace,
    const CIMClass& modifiedClass)
{
    _repository->modifyClass(nameSpace, modifiedClass);
}

void Dispatcher::modifyInstance(
    const String& nameSpace,
    const CIMInstance& modifiedInstance)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

Array<CIMClass> Dispatcher::enumerateClasses(
    const String& nameSpace,
    const String& className,
    Boolean deepInheritance,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin)
{
    return _repository->enumerateClasses(
	nameSpace,
	className,
	deepInheritance,
	localOnly,
	includeQualifiers,
	includeClassOrigin);
}

Array<String> Dispatcher::enumerateClassNames(
    const String& nameSpace,
    const String& className,
    Boolean deepInheritance)
{
    DDD(cout << _DISPATCHER << "emumClass " << className << endl;)

    return _repository->enumerateClassNames(
	nameSpace,
	className,
	deepInheritance);
}

Array<CIMInstance> Dispatcher::enumerateInstances(
    const String& nameSpace,
    const String& className,
    Boolean deepInheritance,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const Array<String>& propertyList)
{
    DDD(cout << _DISPATCHER << "emumInstance of " << className << endl;)

    CIMProvider* provider = _lookupProviderForClass(nameSpace, className);

    if (provider)
    {
	return provider->enumerateInstances(nameSpace, className, 
	    deepInheritance, localOnly, includeQualifiers, 
	    includeClassOrigin, propertyList);
    }
    else
    {
	return _repository->enumerateInstances(
	    nameSpace, className, deepInheritance, localOnly, 
	    includeQualifiers, includeClassOrigin, propertyList);
    }

    return Array<CIMInstance>();
}

Array<CIMReference> Dispatcher::enumerateInstanceNames(
    const String& nameSpace,
    const String& className)
{
    DDD(cout << _DISPATCHER << "emumInstanceNames of " << className << endl;)

    CIMProvider* provider = _lookupProviderForClass(nameSpace, className);

    if (provider)
	return provider->enumerateInstanceNames(nameSpace, className);
    else
	return _repository->enumerateInstanceNames(nameSpace, className);
}

Array<CIMInstance> Dispatcher::execQuery(
    const String& queryLanguage,
    const String& query)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
    return Array<CIMInstance>();
}

Array<CIMObjectWithPath> Dispatcher::associators(
    const String& nameSpace,
    const CIMReference& objectName,
    const String& assocClass,
    const String& resultClass,
    const String& role,
    const String& resultRole,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const Array<String>& propertyList)
{
    return _repository->associators(
	nameSpace, objectName, assocClass, resultClass, role, resultRole,
	    includeQualifiers, includeClassOrigin, propertyList);
}

Array<CIMReference> Dispatcher::associatorNames(
    const String& nameSpace,
    const CIMReference& objectName,
    const String& assocClass,
    const String& resultClass,
    const String& role,
    const String& resultRole)
{
    return _repository->associatorNames(
	nameSpace, objectName, assocClass, resultClass, role, resultRole);
}

Array<CIMObjectWithPath> Dispatcher::references(
    const String& nameSpace,
    const CIMReference& objectName,
    const String& resultClass,
    const String& role,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const Array<String>& propertyList)
{
    return _repository->references(nameSpace, objectName, resultClass, role, 
	includeQualifiers, includeClassOrigin, propertyList);
}

Array<CIMReference> Dispatcher::referenceNames(
    const String& nameSpace,
    const CIMReference& objectName,
    const String& resultClass,
    const String& role)
{
    return _repository->referenceNames(
	nameSpace, objectName, resultClass, role);
}

CIMValue Dispatcher::getProperty(
    const String& nameSpace,
    const CIMReference& instanceName,
    const String& propertyName)
{
    String className = instanceName.getClassName();
    CIMProvider* provider = _lookupProviderForClass(nameSpace, className);

    if (provider)
	return provider->getProperty(nameSpace,instanceName, propertyName );
    else
	return _repository->getProperty(nameSpace, instanceName,propertyName);
}

void Dispatcher::setProperty(
    const String& nameSpace,
    const CIMReference& instanceName,
    const String& propertyName,
    const CIMValue& newValue)
{
    String className = instanceName.getClassName();
    CIMProvider* provider = _lookupProviderForClass(nameSpace, className);

    if (provider)
	provider->setProperty(nameSpace,instanceName,propertyName,newValue);
    else
	_repository->setProperty(nameSpace,instanceName,propertyName,newValue);
}

CIMQualifierDecl Dispatcher::getQualifier(
    const String& nameSpace,
    const String& qualifierName)
{
    return _repository->getQualifier(nameSpace, qualifierName);
}

void Dispatcher::setQualifier(
    const String& nameSpace,
    const CIMQualifierDecl& qualifierDecl)
{
    _repository->setQualifier(nameSpace, qualifierDecl);
}

void Dispatcher::deleteQualifier(
    const String& nameSpace,
    const String& qualifierName)
{
    _repository->deleteQualifier(nameSpace, qualifierName);
}

Array<CIMQualifierDecl> Dispatcher::enumerateQualifiers(
    const String& nameSpace)
{
    return _repository->enumerateQualifiers(nameSpace);
}

CIMValue Dispatcher::invokeMethod(
    const String& nameSpace,
    const CIMReference& instanceName,
    const String& methodName,
    const Array<CIMValue>& inParameters,
    Array<CIMValue>& outParameters)
{
    String className = instanceName.getClassName();
    CIMProvider* provider = _lookupProviderForClass(nameSpace, className);

    if (provider)
	return provider->invokeMethod(nameSpace,
				    instanceName,
				    methodName,
				    inParameters,
				    outParameters);
    else
	return _repository->invokeMethod(nameSpace,
				    instanceName,
				    methodName,
				    inParameters,
				    outParameters);

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

	provider->initialize(*_repository);
    }

    return provider;
}

////////////////////////////////////////////////////////////////////////////////

void Dispatcher::enqueueResponse(
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

    // request->print(cout);

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
	case CIM_CREATE_INSTANCE_REQUEST_MESSAGE:
	case CIM_MODIFY_CLASS_REQUEST_MESSAGE:
	case CIM_MODIFY_INSTANCE_REQUEST_MESSAGE:
	case CIM_ENUMERATE_CLASSES_REQUEST_MESSAGE:
	case CIM_ENUMERATE_CLASS_NAMES_REQUEST_MESSAGE:
	case CIM_ENUMERATE_INSTANCES_REQUEST_MESSAGE:
	case CIM_ENUMERATE_INSTANCE_NAMES_REQUEST_MESSAGE:
	case CIM_EXEC_QUERY_REQUEST_MESSAGE:
	case CIM_ASSOCIATORS_REQUEST_MESSAGE:
	case CIM_ASSOCIATOR_NAMES_REQUEST_MESSAGE:
	case CIM_REFERENCES_REQUEST_MESSAGE:
	case CIM_REFERENCE_NAMES_REQUEST_MESSAGE:
	case CIM_GET_PROPERTY_REQUEST_MESSAGE:
	case CIM_SET_PROPERTY_REQUEST_MESSAGE:
	case CIM_GET_QUALIFIER_REQUEST_MESSAGE:
	case CIM_SET_QUALIFIER_REQUEST_MESSAGE:
	case CIM_DELETE_QUALIFIER_REQUEST_MESSAGE:
	case CIM_ENUMERATE_QUALIFIERS_REQUEST_MESSAGE:
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

    enqueueResponse(request, response);

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

    enqueueResponse(request, response);

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

    enqueueResponse(request, response);

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

    enqueueResponse(request, response);

    delete request;
}

PEGASUS_NAMESPACE_END
