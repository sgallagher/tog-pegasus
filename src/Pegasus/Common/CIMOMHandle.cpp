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
// Modified By: Mike Brasher (mbrasher@bmc.com)
//
// Modified By: Yi Zhou (yi_zhou@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Common/Destroyer.h>
#include <Pegasus/Common/System.h>
#include "CIMOMHandle.h"

PEGASUS_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
//
// CIMOMHandle:
//
////////////////////////////////////////////////////////////////////////////////

CIMOMHandle::CIMOMHandle(void) : _outputQueue(0), _inputQueue(0), _repository(0)
{
}

CIMOMHandle::CIMOMHandle(
	MessageQueue* outputQueue,
	CIMRepository * repository)
:
	_outputQueue(outputQueue),
	_repository(repository)
{
	_inputQueue = new MessageQueue;
}

CIMOMHandle::~CIMOMHandle(void)
{
	if(_inputQueue != 0)
	{
		delete _inputQueue;
	}
}

CIMOMHandle& CIMOMHandle::operator=(const CIMOMHandle& handle)
{
	if(this == &handle)
	{
		return(*this);
	}

	_repository = handle._repository;
	_outputQueue = handle._outputQueue;

	// create an input queue, if necessary
	if((_outputQueue != 0) && (_inputQueue == 0)) {
		_inputQueue = new MessageQueue;
	}

	return(*this);
}

CIMClass CIMOMHandle::getClass(
	const OperationContext & context,
	const String& nameSpace,
	const String& className,
	Boolean localOnly,
	Boolean includeQualifiers,
	Boolean includeClassOrigin,
	const Array<String>& propertyList)
{
	// encode request
	Message* request = new CIMGetClassRequestMessage(
		XmlWriter::getNextMessageId(),
		nameSpace,
		className,
		localOnly,
		includeQualifiers,
		includeClassOrigin,
		propertyList,
		QueueIdStack(_inputQueue->getQueueId()));

	// save message key
	Uint32 messageKey = request->getKey();
	
	// send request
	_outputQueue->enqueue(request);

	// wait for response
	Message* message = _waitForResponse(CIM_GET_CLASS_RESPONSE_MESSAGE, messageKey);

	// decode response
	CIMGetClassResponseMessage* response = (CIMGetClassResponseMessage*)message;

	Destroyer<CIMGetClassResponseMessage> destroyer(response);
	
	_checkError(response);

	return(response->cimClass);
}

void CIMOMHandle::getClassAsync(
	const OperationContext & context,
    const String& nameSpace,
    const String& className,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const Array<String>& propertyList,
	ResponseHandler<CIMClass> & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

Array<CIMClass> CIMOMHandle::enumerateClasses(
	const OperationContext & context,
	const String& nameSpace,
	const String& className,
	Boolean deepInheritance,
	Boolean localOnly,
	Boolean includeQualifiers,
	Boolean includeClassOrigin)
{
	Message* request = new CIMEnumerateClassesRequestMessage(
		XmlWriter::getNextMessageId(),
		nameSpace,
		className,
		deepInheritance,
		localOnly,
		includeQualifiers,
		includeClassOrigin,
		QueueIdStack(_inputQueue->getQueueId()));

	// save message key
	Uint32 messageKey = request->getKey();

	// send request
	_outputQueue->enqueue(request);

	// wait for response
	Message* message = _waitForResponse(CIM_ENUMERATE_CLASSES_RESPONSE_MESSAGE, messageKey);

	// decode response
	CIMEnumerateClassesResponseMessage* response = (CIMEnumerateClassesResponseMessage*)message;

	Destroyer<CIMEnumerateClassesResponseMessage> destroyer(response);

	_checkError(response);

	return(response->cimClasses);
}

void CIMOMHandle::enumerateClassesAsync(
	const OperationContext & context,
    const String& nameSpace,
    const String& className,
    Boolean deepInheritance,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
	ResponseHandler<CIMClass> & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

Array<String> CIMOMHandle::enumerateClassNames(
	const OperationContext & context,
	const String& nameSpace,
	const String& className,
	Boolean deepInheritance)
{
	Message* request = new CIMEnumerateClassNamesRequestMessage(
		XmlWriter::getNextMessageId(),
		nameSpace,
		className,
		deepInheritance,
		QueueIdStack(_inputQueue->getQueueId()));

	// save message key
	Uint32 messageKey = request->getKey();

	// send request
	_outputQueue->enqueue(request);

	// wait for response
	Message* message = _waitForResponse(CIM_ENUMERATE_CLASS_NAMES_RESPONSE_MESSAGE, messageKey);

	// decode response
	CIMEnumerateClassNamesResponseMessage* response = (CIMEnumerateClassNamesResponseMessage*)message;

	Destroyer<CIMEnumerateClassNamesResponseMessage> destroyer(response);

	_checkError(response);

	return(response->classNames);
}

void CIMOMHandle::enumerateClassNamesAsync(
	const OperationContext & context,
    const String& nameSpace,
    const String& className,
    Boolean deepInheritance,
	ResponseHandler<CIMReference> & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

void CIMOMHandle::createClass(
	const OperationContext & context,
	const String& nameSpace,
	const CIMClass& newClass)
{
	Message* request = new CIMCreateClassRequestMessage(
		XmlWriter::getNextMessageId(),
		nameSpace,
		newClass,
		QueueIdStack(_inputQueue->getQueueId()));

	// save message key
	Uint32 messageKey = request->getKey();

	// send request
	_outputQueue->enqueue(request);

	// wait for response
	Message* message = _waitForResponse(CIM_CREATE_CLASS_RESPONSE_MESSAGE, messageKey);

	// decode response
	CIMCreateClassResponseMessage* response = (CIMCreateClassResponseMessage*)message;

	Destroyer<CIMCreateClassResponseMessage> destroyer(response);

	_checkError(response);
}

void CIMOMHandle::createClassAsync(
	const OperationContext & context,
    const String& nameSpace,
    const CIMClass& newClass,
	ResponseHandler<CIMClass> & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

void CIMOMHandle::modifyClass(
	const OperationContext & context,
	const String& nameSpace,
	const CIMClass& modifiedClass)
{
	Message* request = new CIMModifyClassRequestMessage(
		XmlWriter::getNextMessageId(),
		nameSpace,
		modifiedClass,
		QueueIdStack(_inputQueue->getQueueId()));

	// save message key
	Uint32 messageKey = request->getKey();

	// send request
	_outputQueue->enqueue(request);

	// wait for response
	Message* message = _waitForResponse(CIM_MODIFY_CLASS_RESPONSE_MESSAGE, messageKey);

	// decode response
	CIMModifyClassResponseMessage* response = (CIMModifyClassResponseMessage*)message;

	Destroyer<CIMModifyClassResponseMessage> destroyer(response);

	_checkError(response);
}

void CIMOMHandle::modifyClassAsync(
	const OperationContext & context,
    const String& nameSpace,
    const CIMClass& modifiedClass,
	ResponseHandler<CIMClass> & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

void CIMOMHandle::deleteClass(
	const OperationContext & context,
	const String& nameSpace,
	const String& className)
{
	// encode request
	Message* request = new CIMDeleteClassRequestMessage(
		XmlWriter::getNextMessageId(),
		nameSpace,
		className,
		QueueIdStack(_inputQueue->getQueueId()));

	// save message key
	Uint32 messageKey = request->getKey();

	// send request
	_outputQueue->enqueue(request);

	// wait for response
	Message* message = _waitForResponse(CIM_DELETE_CLASS_RESPONSE_MESSAGE, messageKey);

	// decode response
	CIMDeleteClassResponseMessage* response = (CIMDeleteClassResponseMessage*)message;

	Destroyer<CIMDeleteClassResponseMessage> destroyer(response);
	
	_checkError(response);
}

void CIMOMHandle::deleteClassAsync(
	const OperationContext & context,
    const String& nameSpace,
    const String& className,
	ResponseHandler<CIMClass> & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

CIMInstance CIMOMHandle::getInstance(
	const OperationContext & context,
	const String& nameSpace,
	const CIMReference& instanceName,
	Boolean localOnly,
	Boolean includeQualifiers,
	Boolean includeClassOrigin,
	const Array<String>& propertyList)
{
	// encode request
	Message* request = new CIMGetInstanceRequestMessage(
		XmlWriter::getNextMessageId(),
		nameSpace,
		instanceName,
		localOnly,
		includeQualifiers,
		includeClassOrigin,
		propertyList,
		QueueIdStack(_inputQueue->getQueueId()));

	// save message key
	Uint32 messageKey = request->getKey();
	
	// send request
	_outputQueue->enqueue(request);

	// wait for response
	Message* message = _waitForResponse(CIM_GET_INSTANCE_RESPONSE_MESSAGE, messageKey);

	// decode response
	CIMGetInstanceResponseMessage* response = (CIMGetInstanceResponseMessage*)message;

	Destroyer<CIMGetInstanceResponseMessage> destroyer(response);
	
	_checkError(response);

	return(response->cimInstance);
}

void CIMOMHandle::getInstanceAsync(
	const OperationContext & context,
    const String& nameSpace,
    const CIMReference& instanceName,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const Array<String>& propertyList,
	ResponseHandler<CIMInstance> & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

Array<CIMInstance> CIMOMHandle::enumerateInstances(
	const OperationContext & context,
	const String& nameSpace,
	const String& className,
	Boolean deepInheritance,
	Boolean localOnly,
	Boolean includeQualifiers,
	Boolean includeClassOrigin,
	const Array<String>& propertyList)
{
	Message* request = new CIMEnumerateInstancesRequestMessage(
		XmlWriter::getNextMessageId(),
		nameSpace,
		className,
		deepInheritance,
		localOnly,
		includeQualifiers,
		includeClassOrigin,
		propertyList,
		QueueIdStack(_inputQueue->getQueueId()));

	// save message key
	Uint32 messageKey = request->getKey();

	// send request
	_outputQueue->enqueue(request);

	// wait for response
	Message* message = _waitForResponse(CIM_ENUMERATE_INSTANCES_RESPONSE_MESSAGE, messageKey);

	// decode response
	CIMEnumerateInstancesResponseMessage* response = (CIMEnumerateInstancesResponseMessage*)message;

	Destroyer<CIMEnumerateInstancesResponseMessage> destroyer(response);

	_checkError(response);

	return(response->cimInstances);
}

void CIMOMHandle::enumerateInstancesAsync(
	const OperationContext & context,
    const String& nameSpace,
    const String& className,
    Boolean deepInheritance,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const Array<String>& propertyList,
	ResponseHandler<CIMInstance> & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

Array<CIMReference> CIMOMHandle::enumerateInstanceNames(
	const OperationContext & context,
	const String& nameSpace,
	const String& className)
{
	Message* request = new CIMEnumerateInstanceNamesRequestMessage(
		XmlWriter::getNextMessageId(),
		nameSpace,
		className,
		QueueIdStack(_inputQueue->getQueueId()));

	// save message key
	Uint32 messageKey = request->getKey();

	// send request
	_outputQueue->enqueue(request);

	// wait for response
	Message* message = _waitForResponse(CIM_ENUMERATE_INSTANCE_NAMES_RESPONSE_MESSAGE, messageKey);

	// decode response
	CIMEnumerateInstanceNamesResponseMessage* response = (CIMEnumerateInstanceNamesResponseMessage*)message;

	Destroyer<CIMEnumerateInstanceNamesResponseMessage> destroyer(response);

	_checkError(response);

	return(response->instanceNames);
}

void CIMOMHandle::enumerateInstanceNamesAsync(
	const OperationContext & context,
    const String& nameSpace,
    const String& className,
	ResponseHandler<CIMReference> & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

CIMReference CIMOMHandle::createInstance(
	const OperationContext & context,
	const String& nameSpace,
	const CIMInstance& newInstance)
{
	Message* request = new CIMCreateInstanceRequestMessage(
		XmlWriter::getNextMessageId(),
		nameSpace,
		newInstance,
		QueueIdStack(_inputQueue->getQueueId()));

	// save message key
	Uint32 messageKey = request->getKey();

	// send request
	_outputQueue->enqueue(request);

	// wait for response
	Message* message = _waitForResponse(CIM_CREATE_INSTANCE_RESPONSE_MESSAGE, messageKey);

	// decode response
	CIMCreateInstanceResponseMessage* response = (CIMCreateInstanceResponseMessage*)message;

	Destroyer<CIMCreateInstanceResponseMessage> destroyer(response);

	_checkError(response);

	return(response->instanceName);
}

void CIMOMHandle::createInstanceAsync(
	const OperationContext & context,
    const String& nameSpace,
    const CIMInstance& newInstance,
	ResponseHandler<CIMInstance> & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

void CIMOMHandle::modifyInstance(
	const OperationContext & context,
	const String& nameSpace,
	const CIMInstance& modifiedInstance)
{
	Message* request = new CIMModifyInstanceRequestMessage(
		XmlWriter::getNextMessageId(),
		nameSpace,
		modifiedInstance,
		QueueIdStack(_inputQueue->getQueueId()));

	// save message key
	Uint32 messageKey = request->getKey();

	// send request
	_outputQueue->enqueue(request);

	// wait for response
	Message* message = _waitForResponse(CIM_MODIFY_INSTANCE_RESPONSE_MESSAGE, messageKey);

	// decode response
	CIMModifyInstanceResponseMessage* response = (CIMModifyInstanceResponseMessage*)message;

	Destroyer<CIMModifyInstanceResponseMessage> destroyer(response);

	_checkError(response);
}

void CIMOMHandle::modifyInstanceAsync(
	const OperationContext & context,
    const String& nameSpace,
    const CIMInstance& modifiedInstance,
	ResponseHandler<CIMInstance> & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

void CIMOMHandle::deleteInstance(
	const OperationContext & context,
	const String& nameSpace,
	const CIMReference& instanceName)
{
	Message* request = new CIMDeleteInstanceRequestMessage(
		XmlWriter::getNextMessageId(),
		nameSpace,
		instanceName,
		QueueIdStack(_inputQueue->getQueueId()));

	// save message key
	Uint32 messageKey = request->getKey();

	// send request
	_outputQueue->enqueue(request);

	// wait for response
	Message* message = _waitForResponse(CIM_DELETE_INSTANCE_RESPONSE_MESSAGE, messageKey);

	// decode response
	CIMDeleteInstanceResponseMessage* response = (CIMDeleteInstanceResponseMessage*)message;

	Destroyer<CIMDeleteInstanceResponseMessage> destroyer(response);

	_checkError(response);
}

void CIMOMHandle::deleteInstanceAsync(
	const OperationContext & context,
    const String& nameSpace,
    const CIMReference& instanceName,
	ResponseHandler<CIMInstance> & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

Array<CIMInstance> CIMOMHandle::execQuery(
	const OperationContext & context,
	const String& queryLanguage,
	const String& query)
{
	Message* request = new CIMExecQueryRequestMessage(
		XmlWriter::getNextMessageId(),
		queryLanguage,
		query,
		QueueIdStack(_inputQueue->getQueueId()));

	// save message key
	Uint32 messageKey = request->getKey();

	// send request
	_outputQueue->enqueue(request);

	// wait for response
	Message* message = _waitForResponse(CIM_EXEC_QUERY_RESPONSE_MESSAGE, messageKey);

	// decode response
	CIMExecQueryResponseMessage* response = (CIMExecQueryResponseMessage*)message;

	Destroyer<CIMExecQueryResponseMessage> destroyer(response);

	_checkError(response);

	return(response->cimInstances);
}

void CIMOMHandle::execQueryAsync(
	const OperationContext & context,
    const String& queryLanguage,
    const String& query,
	ResponseHandler<CIMObject> & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

Array<CIMObjectWithPath> CIMOMHandle::associators(
	const OperationContext & context,
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
	Message* request = new CIMAssociatorsRequestMessage(
		XmlWriter::getNextMessageId(),
		nameSpace,
		objectName,
		assocClass,
		resultClass,
		role,
		resultRole,
		includeQualifiers,
		includeClassOrigin,
		propertyList,
		QueueIdStack(_inputQueue->getQueueId()));

	// save message key
	Uint32 messageKey = request->getKey();

	// send request
	_outputQueue->enqueue(request);

	// wait for response
	Message* message = _waitForResponse(CIM_ASSOCIATORS_RESPONSE_MESSAGE, messageKey);

	// decode response
	CIMAssociatorsResponseMessage* response = (CIMAssociatorsResponseMessage*)message;

	Destroyer<CIMAssociatorsResponseMessage> destroyer(response);

	_checkError(response);

	return(response->cimObjects);
}

void CIMOMHandle::associatorsAsync(
	const OperationContext & context,
    const String& nameSpace,
    const CIMReference& objectName,
    const String& assocClass,
    const String& resultClass,
    const String& role,
    const String& resultRole,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const Array<String>& propertyList,
	ResponseHandler<CIMObjectWithPath> & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

Array<CIMReference> CIMOMHandle::associatorNames(
	const OperationContext & context,
	const String& nameSpace,
	const CIMReference& objectName,
	const String& assocClass,
	const String& resultClass,
	const String& role,
	const String& resultRole)
{
	Message* request = new CIMAssociatorNamesRequestMessage(
		XmlWriter::getNextMessageId(),
		nameSpace,
		objectName,
		assocClass,
		resultClass,
		role,
		resultRole,
		QueueIdStack(_inputQueue->getQueueId()));

	// save message key
	Uint32 messageKey = request->getKey();

	// send request
	_outputQueue->enqueue(request);

	// wait for response
	Message* message = _waitForResponse(CIM_ASSOCIATOR_NAMES_RESPONSE_MESSAGE, messageKey);

	// decode response
	CIMAssociatorNamesResponseMessage* response = (CIMAssociatorNamesResponseMessage*)message;

	Destroyer<CIMAssociatorNamesResponseMessage> destroyer(response);

	_checkError(response);

	return(response->objectNames);
}

void CIMOMHandle::associatorNamesAsync(
	const OperationContext & context,
    const String& nameSpace,
    const CIMReference& objectName,
    const String& assocClass,
    const String& resultClass,
    const String& role,
    const String& resultRole,
	ResponseHandler<CIMReference> & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

Array<CIMObjectWithPath> CIMOMHandle::references(
	const OperationContext & context,
	const String& nameSpace,
	const CIMReference& objectName,
	const String& resultClass,
	const String& role,
	Boolean includeQualifiers,
	Boolean includeClassOrigin,
	const Array<String>& propertyList)
{
	Message* request = new CIMReferencesRequestMessage(
		XmlWriter::getNextMessageId(),
		nameSpace,
		objectName,
		resultClass,
		role,
		includeQualifiers,
		includeClassOrigin,
		propertyList,
		QueueIdStack(_inputQueue->getQueueId()));

	// save message key
	Uint32 messageKey = request->getKey();

	// send request
	_outputQueue->enqueue(request);

	// wait for response
	Message* message = _waitForResponse(CIM_REFERENCES_RESPONSE_MESSAGE, messageKey);

	// decode response
	CIMReferencesResponseMessage* response = (CIMReferencesResponseMessage*)message;

	Destroyer<CIMReferencesResponseMessage> destroyer(response);

	_checkError(response);

	return(response->cimObjects);
}

void CIMOMHandle::referencesAsync(
	const OperationContext & context,
    const String& nameSpace,
    const CIMReference& objectName,
    const String& resultClass,
    const String& role,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const Array<String>& propertyList,
    ResponseHandler<CIMObjectWithPath> & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

Array<CIMReference> CIMOMHandle::referenceNames(
	const OperationContext & context,
	const String& nameSpace,
	const CIMReference& objectName,
	const String& resultClass,
	const String& role)
{
	Message* request = new CIMReferenceNamesRequestMessage(
		XmlWriter::getNextMessageId(),
		nameSpace,
		objectName,
		resultClass,
		role,
		QueueIdStack(_inputQueue->getQueueId()));

	// save message key
	Uint32 messageKey = request->getKey();
	
	// send request
	_outputQueue->enqueue(request);

	// wait for response
	Message* message = _waitForResponse(CIM_REFERENCE_NAMES_RESPONSE_MESSAGE, messageKey);

	// decode response
	CIMReferenceNamesResponseMessage* response = (CIMReferenceNamesResponseMessage*)message;

	Destroyer<CIMReferenceNamesResponseMessage> destroyer(response);

	_checkError(response);

	return(response->objectNames);
}

void CIMOMHandle::referenceNamesAsync(
	const OperationContext & context,
    const String& nameSpace,
    const CIMReference& objectName,
    const String& resultClass,
    const String& role,
	ResponseHandler<CIMReference> & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

CIMValue CIMOMHandle::getProperty(
	const OperationContext & context,
	const String& nameSpace,
	const CIMReference& instanceName,
	const String& propertyName)
{
	Message* request = new CIMGetPropertyRequestMessage(
		XmlWriter::getNextMessageId(),
		nameSpace,
		instanceName,
		propertyName,
		QueueIdStack(_inputQueue->getQueueId()));

	// save message key
	Uint32 messageKey = request->getKey();
	
	// send request
	_outputQueue->enqueue(request);

	// wait for response
	Message* message = _waitForResponse(CIM_GET_PROPERTY_RESPONSE_MESSAGE, messageKey);

	// decode response
	CIMGetPropertyResponseMessage* response = (CIMGetPropertyResponseMessage*)message;

	Destroyer<CIMGetPropertyResponseMessage> destroyer(response);

	_checkError(response);

	return(response->value);
}

void CIMOMHandle::getPropertyAsync(
	const OperationContext & context,
    const String& nameSpace,
    const CIMReference& instanceName,
    const String& propertyName,
	ResponseHandler<CIMValue> & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

void CIMOMHandle::setProperty(
	const OperationContext & context,
	const String& nameSpace,
	const CIMReference& instanceName,
	const String& propertyName,
	const CIMValue& newValue)
{
	Message* request = new CIMSetPropertyRequestMessage(
		XmlWriter::getNextMessageId(),
		nameSpace,
		instanceName,
		propertyName,
		newValue,
		QueueIdStack(_inputQueue->getQueueId()));

	// save message key
	Uint32 messageKey = request->getKey();
	
	// send request
	_outputQueue->enqueue(request);

	// wait for response
	Message* message = _waitForResponse(CIM_SET_PROPERTY_RESPONSE_MESSAGE, messageKey);

	// decode response
	CIMSetPropertyResponseMessage* response = (CIMSetPropertyResponseMessage*)message;

	Destroyer<CIMSetPropertyResponseMessage> destroyer(response);

	_checkError(response);
}

void CIMOMHandle::setPropertyAsync(
	const OperationContext & context,
    const String& nameSpace,
    const CIMReference& instanceName,
    const String& propertyName,
    const CIMValue& newValue,
	ResponseHandler<CIMValue> & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

CIMValue CIMOMHandle::invokeMethod(
	const OperationContext & context,
	const String& nameSpace,
	const CIMReference& instanceName,
	const String& methodName,
	const Array<CIMParamValue>& inParameters,
	Array<CIMParamValue>& outParameters)
{
	Message* request = new CIMInvokeMethodRequestMessage(
		XmlWriter::getNextMessageId(),
		nameSpace,
		instanceName,
		methodName,
		inParameters,
		QueueIdStack(_inputQueue->getQueueId()));

	// save message key
	Uint32 messageKey = request->getKey();
	
	// send request
	_outputQueue->enqueue(request);

	// wait for response
	Message* message = _waitForResponse(CIM_INVOKE_METHOD_RESPONSE_MESSAGE, messageKey);

	// decode response
	CIMInvokeMethodResponseMessage* response = (CIMInvokeMethodResponseMessage*)message;

	Destroyer<CIMInvokeMethodResponseMessage> destroyer(response);

	_checkError(response);

	outParameters = response->outParameters;
	
	return(response->retValue);
}

void CIMOMHandle::invokeMethodAsync(
	const OperationContext & context,
    const String& nameSpace,
    const CIMReference& instanceName,
    const String& methodName,
    const Array<CIMParamValue>& inParameters,
    Array<CIMParamValue>& outParameters,
	ResponseHandler<CIMValue> & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

Message * CIMOMHandle::_waitForResponse(
	const Uint32 messageType,
	const Uint32 messageKey,
	const Uint32 timeout)
{
	if(_inputQueue == 0) {
		throw UnitializedHandle();
	}
	
	// immediately attempt to locate a message of the requested type
	Message * message = _inputQueue->find(messageType, messageKey);

	// if the message is null and the timeout is greater than 0, go into
	// a sleep retry mode until the
	// timeout expires or a message of the requested type arrives. a timeout value of 0xffffffff represents
	// infinity.
	for(Uint32 i = 0; ((i < timeout) || (timeout == 0xffffffff)) && (message == 0); i += 100)
	{
		System::sleep(100);
		
		message = _inputQueue->find(messageType, messageKey);
	}

	if(message == 0) {
		throw CIMException(CIM_ERR_FAILED, __FILE__, __LINE__, "queue underflow");
	}

	_inputQueue->remove(message);

	return(message);
}

void CIMOMHandle::_checkError(const CIMResponseMessage* responseMessage)
{
    if (responseMessage && (responseMessage->errorCode != CIM_ERR_SUCCESS))
    {
	throw CIMException(responseMessage->errorCode,
	    __FILE__, __LINE__, responseMessage->errorDescription);
    }
}

PEGASUS_NAMESPACE_END
