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
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Common/Destroyer.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/CIMReference.h>
//#include <Pegasus/Common/Exception.h>

PEGASUS_NAMESPACE_BEGIN

CIMOMHandle::CIMOMHandle(void)
   : _service(0), _cimom(0), _id(peg_credential_types::PROVIDER)
{
   _controller = &(ModuleController::get_client_handle(_id, &_client_handle));
   if(_client_handle == NULL)
      ThrowUnitializedHandle();
   
}

CIMOMHandle::CIMOMHandle(MessageQueueService * service)
   : _service(service), _cimom(0), _id(peg_credential_types::PROVIDER)
{
    MessageQueue * queue =
        MessageQueue::lookup(PEGASUS_QUEUENAME_OPREQDISPATCHER);

    _cimom = dynamic_cast<MessageQueueService *>(queue);
    _controller = &(ModuleController::get_client_handle(_id, &_client_handle));

    if((_service == 0) || (_cimom == 0) || (_client_handle == NULL))
    {
	ThrowUnitializedHandle();
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
    _cimom = handle._cimom;

    return(*this);
}


void CIMOMHandle::async_callback(Uint32 user_data, 
				 Message *reply, 
				 void *parm)
{
   callback_data *cb_data = reinterpret_cast<callback_data *>(parm);
   cb_data->reply = reply;
   cb_data->client_sem.signal();
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
    if((_service == 0) || (_cimom == 0))
    {
	ThrowUnitializedHandle();
    }

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
    //    AsyncOpNode * op = _service->get_op();

    callback_data *cb_data = new callback_data(this);
    

    // create request envelope
    AsyncLegacyOperationStart * asyncRequest =
	new AsyncLegacyOperationStart (
	    _service->get_next_xid(),
	    NULL,
	    _cimom->getQueueId(),
	    request,
	    _cimom->getQueueId());

    if( false  == _controller->ClientSendAsync(*_client_handle, 
					       0, 
					       _cimom->getQueueId(),
					       asyncRequest,
					       async_callback,
					       (void *)cb_data) )
    {
       delete asyncRequest;
       delete cb_data;
       throw CIMException(CIM_ERR_NOT_FOUND);
       
    }
       cb_data->client_sem.wait();
       AsyncReply * asyncReply = static_cast<AsyncReply *>(cb_data->get_reply()) ;
       CIMGetClassResponseMessage * response =
	  reinterpret_cast<CIMGetClassResponseMessage *>(
	     (static_cast<AsyncLegacyOperationResult *>(asyncReply))->get_result());
       CIMClass cimClass = response->cimClass;
       delete asyncRequest;
       delete asyncReply;
       delete response;
       delete cb_data;
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
    if((_service == 0) || (_cimom == 0))
    {
	ThrowUnitializedHandle();
    }

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
    if((_service == 0) || (_cimom == 0))
    {
	ThrowUnitializedHandle();
    }

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
    if((_service == 0) || (_cimom == 0))
    {
	ThrowUnitializedHandle();
    }

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
    if((_service == 0) || (_cimom == 0))
    {
	ThrowUnitializedHandle();
    }

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
    if((_service == 0) || (_cimom == 0))
    {
	ThrowUnitializedHandle();
    }

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
    if((_service == 0) || (_cimom == 0))
    {
	ThrowUnitializedHandle();
    }

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
    //    AsyncOpNode * op = _service->get_op();

    // create request envelope
    AsyncLegacyOperationStart * asyncRequest =
	new AsyncLegacyOperationStart (
	    _service->get_next_xid(),
	    0,
	    _cimom->getQueueId(),
	    request,
	    _cimom->getQueueId());


    callback_data *cb_data = new callback_data(this);
    
    if ( false == _controller->ClientSendAsync(*_client_handle, 
					       0, 
					       _cimom->getQueueId(), 
					       asyncRequest, 
					       async_callback, 
					       (void *)cb_data) )
    {
       delete asyncRequest;
       delete cb_data;
       throw CIMException(CIM_ERR_NOT_FOUND);
    }
    
    cb_data->client_sem.wait();
    AsyncReply *asyncReply = static_cast<AsyncReply *>(cb_data->get_reply());
    
    // send request and wait for response 
    // <<< Wed Apr 10 20:24:22 2002 mdd >>>
    //    AsyncReply * asyncReply = _service->SendWait(asyncRequest);

    // decode response
    CIMGetInstanceResponseMessage * response =
	reinterpret_cast<CIMGetInstanceResponseMessage *>(
	    (static_cast<AsyncLegacyOperationResult *>(asyncReply))->get_result());

    CIMInstance cimInstance = response->cimInstance;

    delete asyncRequest;
    delete asyncReply;
    delete response;
    delete cb_data;
    
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
    if((_service == 0) || (_cimom == 0))
    {
	ThrowUnitializedHandle();
    }

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
    // AsyncOpNode * op = _service->get_op();

    callback_data *cb_data = new callback_data(this);

    // create request envelope
    AsyncLegacyOperationStart * asyncRequest =
	new AsyncLegacyOperationStart (
	    _service->get_next_xid(),
	    0,
	    _cimom->getQueueId(),
	    request,
	    _cimom->getQueueId());
    

    // send request and wait for response
    // <<< Wed Apr 10 20:24:36 2002 mdd >>>
    // AsyncReply * asyncReply = _service->SendWait(asyncRequest);


    if(false == _controller->ClientSendAsync(*_client_handle, 
					     0, 
					     _cimom->getQueueId(),
					     asyncRequest,
					     async_callback, 
					     (void *)cb_data) )
    {
       delete asyncRequest;
       delete cb_data;
       throw CIMException(CIM_ERR_NOT_FOUND);
    }
    
    cb_data->client_sem.wait();
    AsyncReply *asyncReply = static_cast<AsyncReply *>(cb_data->get_reply());
    
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
    delete response;
    delete cb_data;
    
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
    if((_service == 0) || (_cimom == 0))
    {
	ThrowUnitializedHandle();
    }

    // encode request
    CIMEnumerateInstanceNamesRequestMessage * request =
	new CIMEnumerateInstanceNamesRequestMessage(
	    XmlWriter::getNextMessageId(),
	    nameSpace,
	    className,
	    QueueIdStack(_cimom->getQueueId(), _service->getQueueId()));

    // create an op node
    // AsyncOpNode * op = _service->get_op();

    callback_data *cb_data = new callback_data(this);
    
    // create request envelope
    AsyncLegacyOperationStart * asyncRequest =
	new AsyncLegacyOperationStart (
	    _service->get_next_xid(),
	    0,
	    _cimom->getQueueId(),
	    request,
	    _cimom->getQueueId());
    
    if(false == _controller->ClientSendAsync(*_client_handle,
					     0,
					     _cimom->getQueueId(),
					     asyncRequest, 
					     async_callback, 
					     (void *)cb_data))
       {
	  delete asyncRequest;
	  delete cb_data;
	  throw CIMException(CIM_ERR_NOT_FOUND);
       }
    
    
    
    // send request and wait for response
    // <<< Wed Apr 10 20:30:31 2002 mdd >>>
    // AsyncReply * asyncReply = _service->SendWait(asyncRequest);
    
    AsyncReply * asyncReply = static_cast<AsyncReply *>(cb_data->get_reply());
    // decode response
    CIMEnumerateInstanceNamesResponseMessage * response =
       reinterpret_cast<CIMEnumerateInstanceNamesResponseMessage *>(
	  (static_cast<AsyncLegacyOperationResult *>(asyncReply))->get_result());
    
    Array<CIMReference> cimReferences = response->instanceNames;
    
    //_service->return_op(op);

    delete asyncRequest;
    delete asyncReply;
    delete response;
    delete cb_data;
    
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
    if((_service == 0) || (_cimom == 0))
    {
	ThrowUnitializedHandle();
    }

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
    if((_service == 0) || (_cimom == 0))
    {
	ThrowUnitializedHandle();
    }

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
    if((_service == 0) || (_cimom == 0))
    {
	ThrowUnitializedHandle();
    }

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
    const String& nameSpace,
    const String& queryLanguage,
    const String& query)
{
    if((_service == 0) || (_cimom == 0))
    {
	ThrowUnitializedHandle();
    }

    CIMExecQueryRequestMessage * request =
	new CIMExecQueryRequestMessage(
	    XmlWriter::getNextMessageId(),
	    nameSpace,
	    queryLanguage,
	    query,
	    QueueIdStack(_cimom->getQueueId(), _service->getQueueId()));

    Array<CIMInstance> cimInstances;

    return(cimInstances);
}

void CIMOMHandle::execQueryAsync(
    const OperationContext & context,
    const String& nameSpace,
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
    if((_service == 0) || (_cimom == 0))
    {
	ThrowUnitializedHandle();
    }

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
    if((_service == 0) || (_cimom == 0))
    {
	ThrowUnitializedHandle();
    }

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
    if((_service == 0) || (_cimom == 0))
    {
	ThrowUnitializedHandle();
    }

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
    if((_service == 0) || (_cimom == 0))
    {
	ThrowUnitializedHandle();
    }

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
    if((_service == 0) || (_cimom == 0))
    {
	ThrowUnitializedHandle();
    }

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
    if((_service == 0) || (_cimom == 0))
    {
	ThrowUnitializedHandle();
    }

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
    if((_service == 0) || (_cimom == 0))
    {
	ThrowUnitializedHandle();
    }

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
