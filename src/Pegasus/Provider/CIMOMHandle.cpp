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
//
//%/////////////////////////////////////////////////////////////////////////////

#include "CIMOMHandle.h"
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Common/Destroyer.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/CIMReference.h>
//#include <Pegasus/Common/Exception.h>

PEGASUS_NAMESPACE_BEGIN

CIMOMHandle::CIMOMHandle(MessageQueueService * service)
    : _service(service), _cimom(0)
{
    MessageQueue * queue = MessageQueue::lookup("CIMOpRequestDispatcher");

    _cimom = dynamic_cast<MessageQueueService *>(queue);

    if((_service == 0) || (_cimom == 0))
    {
	throw NullPointer();
    }
}

CIMOMHandle::~CIMOMHandle(void)
{
}

CIMOMHandle & CIMOMHandle::operator=(const CIMOMHandle & handle)
{
    if(this == &handle)
    {
	return(*this);
    }

    _service = handle._service;

    return(*this);
}

CIMClass CIMOMHandle::getClass(
    const OperationContext & context,
    const String& nameSpace,
    const String& className,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
    // encode request
    CIMGetClassRequestMessage * request =
	new CIMGetClassRequestMessage(
	    XmlWriter::getNextMessageId(),
	    nameSpace,
	    className,
	    localOnly,
	    includeQualifiers,
	    includeClassOrigin,
	    propertyList,
	    QueueIdStack(_cimom->getQueueId(), _service->getQueueId()));

    // create an op node
    AsyncOpNode * op = _service->get_op();

    // create request envelope
    AsyncLegacyOperationStart * asyncRequest =
	new AsyncLegacyOperationStart (
	    _service->get_next_xid(),
	    op,
	    _cimom->getQueueId(),
	    request,
	    _cimom->getQueueId());

    // send request and wait for response
    AsyncReply * asyncReply = _service->SendWait(asyncRequest);

    CIMGetClassResponseMessage * response =
	reinterpret_cast<CIMGetClassResponseMessage *>(
	    (static_cast<AsyncLegacyOperationResult *>(asyncReply))->get_result());

    //_service->return_op(op);

    delete asyncRequest;
    delete asyncReply;

    CIMClass cimClass = response->cimClass;

    return(cimClass);
}

void CIMOMHandle::getClassAsync(
    const OperationContext & context,
    const String& nameSpace,
    const String& className,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList,
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
    CIMEnumerateClassesRequestMessage * request =
	new CIMEnumerateClassesRequestMessage(
	    XmlWriter::getNextMessageId(),
	    nameSpace,
	    className,
	    deepInheritance,
	    localOnly,
	    includeQualifiers,
	    includeClassOrigin,
	    QueueIdStack(_cimom->getQueueId(), _service->getQueueId()));

    Array<CIMClass> cimClasses;

    return(cimClasses);
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
    CIMEnumerateClassNamesRequestMessage * request =
	new CIMEnumerateClassNamesRequestMessage(
	    XmlWriter::getNextMessageId(),
	    nameSpace,
	    className,
	    deepInheritance,
	    QueueIdStack(_cimom->getQueueId(), _service->getQueueId()));

    Array<String> classNames;

    return(classNames);
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
    CIMCreateClassRequestMessage * request =
	new CIMCreateClassRequestMessage(
	    XmlWriter::getNextMessageId(),
	    nameSpace,
	    newClass,
	    QueueIdStack(_cimom->getQueueId(), _service->getQueueId()));

    return;
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
    CIMModifyClassRequestMessage * request =
	new CIMModifyClassRequestMessage(
	    XmlWriter::getNextMessageId(),
	    nameSpace,
	    modifiedClass,
	    QueueIdStack(_cimom->getQueueId(), _service->getQueueId()));

    return;
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
    CIMDeleteClassRequestMessage * request =
	new CIMDeleteClassRequestMessage(
	    XmlWriter::getNextMessageId(),
	    nameSpace,
	    className,
	    QueueIdStack(_cimom->getQueueId(), _service->getQueueId()));

    return;
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
    const CIMPropertyList& propertyList)
{
    // encode request
    CIMGetInstanceRequestMessage * request =
	new CIMGetInstanceRequestMessage(
	    XmlWriter::getNextMessageId(),
	    nameSpace,
	    instanceName,
	    localOnly,
	    includeQualifiers,
	    includeClassOrigin,
	    propertyList,
	    QueueIdStack(_cimom->getQueueId(), _service->getQueueId()));

    // create an op node
    AsyncOpNode * op = _service->get_op();

    // create request envelope
    AsyncLegacyOperationStart * asyncRequest =
	new AsyncLegacyOperationStart (
	    _service->get_next_xid(),
	    op,
	    _cimom->getQueueId(),
	    request,
	    _cimom->getQueueId());

    // send request and wait for response
    AsyncReply * asyncReply = _service->SendWait(asyncRequest);

    // decode response
    CIMGetInstanceResponseMessage * response =
	reinterpret_cast<CIMGetInstanceResponseMessage *>(
	    (static_cast<AsyncLegacyOperationResult *>(asyncReply))->get_result());

    CIMInstance cimInstance = response->cimInstance;

    delete asyncRequest;
    delete asyncReply;
    //delete op;

    return(cimInstance);
}

void CIMOMHandle::getInstanceAsync(
    const OperationContext & context,
    const String& nameSpace,
    const CIMReference& instanceName,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList,
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
    const CIMPropertyList& propertyList)
{
    // encode request
    CIMEnumerateInstancesRequestMessage * request =
	new CIMEnumerateInstancesRequestMessage(
	    XmlWriter::getNextMessageId(),
	    nameSpace,
	    className,
	    deepInheritance,
	    localOnly,
	    includeQualifiers,
	    includeClassOrigin,
	    propertyList,
	    QueueIdStack(_cimom->getQueueId(), _service->getQueueId()));

    // obtain an op node
    AsyncOpNode * op = _service->get_op();

    // create request envelope
    AsyncLegacyOperationStart * asyncRequest =
	new AsyncLegacyOperationStart (
	    _service->get_next_xid(),
	    op,
	    _cimom->getQueueId(),
	    request,
	    _cimom->getQueueId());

    // send request and wait for response
    AsyncReply * asyncReply = _service->SendWait(asyncRequest);

    // decode response
    CIMEnumerateInstancesResponseMessage * response =
	reinterpret_cast<CIMEnumerateInstancesResponseMessage *>(
	    (static_cast<AsyncLegacyOperationResult *>(asyncReply))->get_result());

    Array<CIMInstance> cimInstances;

    for(Uint32 i = 0, n = response->cimNamedInstances.size(); i < n; i++)
    {
	cimInstances.append(response->cimNamedInstances[i].getInstance());
    }

    // release the op node
    //_service->return_op(op);

    // release the request objects
    delete asyncRequest;
    delete asyncReply;

    return(cimInstances);
}

void CIMOMHandle::enumerateInstancesAsync(
    const OperationContext & context,
    const String& nameSpace,
    const String& className,
    Boolean deepInheritance,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList,
    ResponseHandler<CIMInstance> & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

Array<CIMReference> CIMOMHandle::enumerateInstanceNames(
    const OperationContext & context,
    const String& nameSpace,
    const String& className)
{
    // encode request
    CIMEnumerateInstanceNamesRequestMessage * request =
	new CIMEnumerateInstanceNamesRequestMessage(
	    XmlWriter::getNextMessageId(),
	    nameSpace,
	    className,
	    QueueIdStack(_cimom->getQueueId(), _service->getQueueId()));

    // create an op node
    AsyncOpNode * op = _service->get_op();

    // create request envelope
    AsyncLegacyOperationStart * asyncRequest =
	new AsyncLegacyOperationStart (
	    _service->get_next_xid(),
	    op,
	    _cimom->getQueueId(),
	    request,
	    _cimom->getQueueId());

    // send request and wait for response
    AsyncReply * asyncReply = _service->SendWait(asyncRequest);

    // decode response
    CIMEnumerateInstanceNamesResponseMessage * response =
	reinterpret_cast<CIMEnumerateInstanceNamesResponseMessage *>(
	    (static_cast<AsyncLegacyOperationResult *>(asyncReply))->get_result());

    Array<CIMReference> cimReferences = response->instanceNames;

    //_service->return_op(op);

    delete asyncRequest;
    delete asyncReply;

    return(cimReferences);
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
    CIMCreateInstanceRequestMessage * request =
	new CIMCreateInstanceRequestMessage(
	    XmlWriter::getNextMessageId(),
	    nameSpace,
	    newInstance,
	    QueueIdStack(_cimom->getQueueId(), _service->getQueueId()));

    CIMReference cimReference;

    return(cimReference);
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
    const CIMInstance& modifiedInstance,
    Boolean includeQualifiers,
    const CIMPropertyList& propertyList)
{
    CIMModifyInstanceRequestMessage * request =
	new CIMModifyInstanceRequestMessage(
	    XmlWriter::getNextMessageId(),
	    nameSpace,
	    CIMNamedInstance(),
	    includeQualifiers,
	    propertyList,
	    QueueIdStack(_cimom->getQueueId(), _service->getQueueId()));

    return;
}

void CIMOMHandle::modifyInstanceAsync(
    const OperationContext & context,
    const String& nameSpace,
    const CIMInstance& modifiedInstance,
    Boolean includeQualifiers,
    const CIMPropertyList& propertyList,
    ResponseHandler<CIMInstance> & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

void CIMOMHandle::deleteInstance(
    const OperationContext & context,
    const String& nameSpace,
    const CIMReference& instanceName)
{
    CIMDeleteInstanceRequestMessage * request =
	new CIMDeleteInstanceRequestMessage(
	    XmlWriter::getNextMessageId(),
	    nameSpace,
	    instanceName,
	    QueueIdStack(_cimom->getQueueId(), _service->getQueueId()));

    return;
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
    CIMExecQueryRequestMessage * request =
	new CIMExecQueryRequestMessage(
	    XmlWriter::getNextMessageId(),
	    queryLanguage,
	    query,
	    QueueIdStack(_cimom->getQueueId(), _service->getQueueId()));

    Array<CIMInstance> cimInstances;

    return(cimInstances);
}

void CIMOMHandle::execQueryAsync(
    const OperationContext & context,
    const String& queryLanguage,
    const String& query,
    ResponseHandler<CIMObject> & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

Array<CIMObject> CIMOMHandle::associators(
    const OperationContext & context,
    const String& nameSpace,
    const CIMReference& objectName,
    const String& assocClass,
    const String& resultClass,
    const String& role,
    const String& resultRole,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
    CIMAssociatorsRequestMessage * request =
	new CIMAssociatorsRequestMessage(
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
	    QueueIdStack(_cimom->getQueueId(), _service->getQueueId()));

    Array<CIMObject> cimObjects;

    return(cimObjects);
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
    const CIMPropertyList& propertyList,
    ResponseHandler<CIMObject> & handler)
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
    CIMAssociatorNamesRequestMessage * request =
	new CIMAssociatorNamesRequestMessage(
	    XmlWriter::getNextMessageId(),
	    nameSpace,
	    objectName,
	    assocClass,
	    resultClass,
	    role,
	    resultRole,
	    QueueIdStack(_cimom->getQueueId(), _service->getQueueId()));

    Array<CIMObjectPath> cimObjectPaths;

    return(cimObjectPaths);
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

Array<CIMObject> CIMOMHandle::references(
    const OperationContext & context,
    const String& nameSpace,
    const CIMReference& objectName,
    const String& resultClass,
    const String& role,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
    CIMReferencesRequestMessage * request =
	new CIMReferencesRequestMessage(
	    XmlWriter::getNextMessageId(),
	    nameSpace,
	    objectName,
	    resultClass,
	    role,
	    includeQualifiers,
	    includeClassOrigin,
	    propertyList,
	    QueueIdStack(_cimom->getQueueId(), _service->getQueueId()));

    Array<CIMObject> cimObjects;

    return(cimObjects);
}

void CIMOMHandle::referencesAsync(
    const OperationContext & context,
    const String& nameSpace,
    const CIMReference& objectName,
    const String& resultClass,
    const String& role,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList,
    ResponseHandler<CIMObject> & handler)
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
    CIMReferenceNamesRequestMessage * request =
	new CIMReferenceNamesRequestMessage(
	    XmlWriter::getNextMessageId(),
	    nameSpace,
	    objectName,
	    resultClass,
	    role,
	    QueueIdStack(_cimom->getQueueId(), _service->getQueueId()));

    Array<CIMObjectPath> cimObjectPaths;

    return(cimObjectPaths);
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
    CIMGetPropertyRequestMessage * request =
	new CIMGetPropertyRequestMessage(
	    XmlWriter::getNextMessageId(),
	    nameSpace,
	    instanceName,
	    propertyName,
	    QueueIdStack(_cimom->getQueueId(), _service->getQueueId()));

    CIMValue cimValue;

    return(cimValue);
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
    CIMSetPropertyRequestMessage * request =
	new CIMSetPropertyRequestMessage(
	    XmlWriter::getNextMessageId(),
	    nameSpace,
	    instanceName,
	    propertyName,
	    newValue,
	    QueueIdStack(_cimom->getQueueId(), _service->getQueueId()));

    return;
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

/*
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
	    QueueIdStack(_cimom->getQueueId(), _service->getQueueId()));

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
*/

PEGASUS_NAMESPACE_END
