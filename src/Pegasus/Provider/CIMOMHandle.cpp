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
// Author: Chip Vincent (cvincent@us.ibm.com)
//
// Modified By: Carol Ann Krug Graves, Hewlett-Packard Company
//                  (carolann_graves@hp.com)
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include "CIMOMHandle.h"

#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Common/Destroyer.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/ModuleController.h>
#include <Pegasus/Provider/CIMOMHandleInternal.h>

PEGASUS_NAMESPACE_BEGIN

class callback_data
{
public:
    Message *reply;
    Semaphore client_sem;
    CIMOMHandle & cimom_handle;

    callback_data(CIMOMHandle *handle)
        : reply(0), client_sem(0), cimom_handle(*handle)
    {
    }

    ~callback_data()
    {
        delete reply;
    }

    Message *get_reply(void)
    {
        Message *ret = reply;
        reply = NULL;
        return(ret);
    }

private:
    callback_data(void);
};

void async_callback(Uint32 user_data, Message *reply, void *parm)
{
    callback_data *cb_data = reinterpret_cast<callback_data *>(parm);
    cb_data->reply = reply;
    cb_data->client_sem.signal();
}


CIMOMHandle::CIMOMHandle(void)
{
    _rep = new CIMOMHandleRep();
    _rep->service = 0;
    _rep->cimom = 0;
    _rep->controller = 0;
    _rep->client_handle = 0;
}

CIMOMHandle::~CIMOMHandle(void)
{
    if(_rep->controller != 0)
    {
        _rep->controller->return_client_handle(_rep->client_handle);
    }

    delete _rep;
}

CIMOMHandle & CIMOMHandle::operator=(const CIMOMHandle & handle)
{
    if(this == &handle)
    {
        return(*this);
    }

    _rep->service = handle._rep->service;
    _rep->cimom = handle._rep->cimom;
    _rep->controller = handle._rep->controller;
    _rep->client_handle = handle._rep->client_handle;
    _rep->client_handle->reference_count++;  // ATTN-RK-P2-20020712: This is a hack

    return(*this);
}

CIMClass CIMOMHandle::getClass(
    const OperationContext & context,
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
    if((_rep->service == 0) || (_rep->cimom == 0))
    {
        throw UninitializedObjectException();
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
        QueueIdStack(_rep->cimom->getQueueId(), _rep->service->getQueueId()));

    // create an op node
    //    AsyncOpNode * op = _rep->service->get_op();

    callback_data *cb_data = new callback_data(this);


    // create request envelope
    AsyncLegacyOperationStart * asyncRequest =
        new AsyncLegacyOperationStart (
        _rep->service->get_next_xid(),
        NULL,
        _rep->cimom->getQueueId(),
        request,
        _rep->cimom->getQueueId());

    if(false  == _rep->controller->ClientSendAsync(*_rep->client_handle,
        0,
        _rep->cimom->getQueueId(),
        asyncRequest,
        async_callback,
        (void *)cb_data))
    {
        delete asyncRequest;
        delete cb_data;
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_FOUND, String::EMPTY);

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

Array<CIMClass> CIMOMHandle::enumerateClasses(
    const OperationContext & context,
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    Boolean deepInheritance,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin)
{
    if((_rep->service == 0) || (_rep->cimom == 0))
    {
        throw UninitializedObjectException();
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
        QueueIdStack(_rep->cimom->getQueueId(), _rep->service->getQueueId()));

    Array<CIMClass> cimClasses;

    return(cimClasses);
}

Array<CIMName> CIMOMHandle::enumerateClassNames(
    const OperationContext & context,
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    Boolean deepInheritance)
{
    if((_rep->service == 0) || (_rep->cimom == 0))
    {
        throw UninitializedObjectException();
    }

    CIMEnumerateClassNamesRequestMessage * request =
        new CIMEnumerateClassNamesRequestMessage(
        XmlWriter::getNextMessageId(),
        nameSpace,
        className,
        deepInheritance,
        QueueIdStack(_rep->cimom->getQueueId(), _rep->service->getQueueId()));

    Array<CIMName> classNames;

    return(classNames);
}

void CIMOMHandle::createClass(
    const OperationContext & context,
    const CIMNamespaceName& nameSpace,
    const CIMClass& newClass)
{
    if((_rep->service == 0) || (_rep->cimom == 0))
    {
        throw UninitializedObjectException();
    }

    CIMCreateClassRequestMessage * request =
        new CIMCreateClassRequestMessage(
        XmlWriter::getNextMessageId(),
        nameSpace,
        newClass,
        QueueIdStack(_rep->cimom->getQueueId(), _rep->service->getQueueId()));

    return;
}

void CIMOMHandle::modifyClass(
    const OperationContext & context,
    const CIMNamespaceName& nameSpace,
    const CIMClass& modifiedClass)
{
    if((_rep->service == 0) || (_rep->cimom == 0))
    {
        throw UninitializedObjectException();
    }

    CIMModifyClassRequestMessage * request =
        new CIMModifyClassRequestMessage(
        XmlWriter::getNextMessageId(),
        nameSpace,
        modifiedClass,
        QueueIdStack(_rep->cimom->getQueueId(), _rep->service->getQueueId()));

    return;
}

void CIMOMHandle::deleteClass(
    const OperationContext & context,
    const CIMNamespaceName& nameSpace,
    const CIMName& className)
{
    if((_rep->service == 0) || (_rep->cimom == 0))
    {
        throw UninitializedObjectException();
    }

    // encode request
    CIMDeleteClassRequestMessage * request =
        new CIMDeleteClassRequestMessage(
        XmlWriter::getNextMessageId(),
        nameSpace,
        className,
        QueueIdStack(_rep->cimom->getQueueId(), _rep->service->getQueueId()));

    return;
}

CIMInstance CIMOMHandle::getInstance(
    const OperationContext & context,
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& instanceName,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
    if((_rep->service == 0) || (_rep->cimom == 0))
    {
        throw UninitializedObjectException();
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
        QueueIdStack(_rep->cimom->getQueueId(), _rep->service->getQueueId()));

    // create an op node
    //    AsyncOpNode * op = _rep->service->get_op();

    // create request envelope
    AsyncLegacyOperationStart * asyncRequest =
        new AsyncLegacyOperationStart (
        _rep->service->get_next_xid(),
        0,
        _rep->cimom->getQueueId(),
        request,
        _rep->cimom->getQueueId());


    callback_data *cb_data = new callback_data(this);

    if(false == _rep->controller->ClientSendAsync(*_rep->client_handle,
        0,
        _rep->cimom->getQueueId(),
        asyncRequest,
        async_callback,
        (void *)cb_data))
    {
        delete asyncRequest;
        delete cb_data;
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_FOUND, String::EMPTY);
    }

    cb_data->client_sem.wait();
    AsyncReply *asyncReply = static_cast<AsyncReply *>(cb_data->get_reply());

    // send request and wait for response
    // <<< Wed Apr 10 20:24:22 2002 mdd >>>
    //    AsyncReply * asyncReply = _rep->service->SendWait(asyncRequest);

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

Array<CIMInstance> CIMOMHandle::enumerateInstances(
    const OperationContext & context,
    const CIMNamespaceName& nameSpace,
    const CIMName& className,
    Boolean deepInheritance,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
    if((_rep->service == 0) || (_rep->cimom == 0))
    {
        throw UninitializedObjectException();
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
        QueueIdStack(_rep->cimom->getQueueId(), _rep->service->getQueueId()));

    // obtain an op node
    // AsyncOpNode * op = _rep->service->get_op();

    callback_data *cb_data = new callback_data(this);

    // create request envelope
    AsyncLegacyOperationStart * asyncRequest =
        new AsyncLegacyOperationStart (
        _rep->service->get_next_xid(),
        0,
        _rep->cimom->getQueueId(),
        request,
        _rep->cimom->getQueueId());


    // send request and wait for response
    // <<< Wed Apr 10 20:24:36 2002 mdd >>>
    // AsyncReply * asyncReply = _rep->service->SendWait(asyncRequest);


    if(false == _rep->controller->ClientSendAsync(*_rep->client_handle,
        0,
        _rep->cimom->getQueueId(),
        asyncRequest,
        async_callback,
        (void *)cb_data))
    {
        delete asyncRequest;
        delete cb_data;
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_FOUND, String::EMPTY);
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
        cimInstances.append(response->cimNamedInstances[i]);
    }

    // release the op node
    //_rep->service->return_op(op);

    // release the request objects
    delete asyncRequest;
    delete asyncReply;
    delete response;
    delete cb_data;

    return(cimInstances);
}

Array<CIMObjectPath> CIMOMHandle::enumerateInstanceNames(
    const OperationContext & context,
    const CIMNamespaceName& nameSpace,
    const CIMName& className)
{
    if((_rep->service == 0) || (_rep->cimom == 0))
    {
        throw UninitializedObjectException();
    }

    // encode request
    CIMEnumerateInstanceNamesRequestMessage * request =
        new CIMEnumerateInstanceNamesRequestMessage(
        XmlWriter::getNextMessageId(),
        nameSpace,
        className,
        QueueIdStack(_rep->cimom->getQueueId(), _rep->service->getQueueId()));

    // create an op node
    // AsyncOpNode * op = _rep->service->get_op();

    callback_data *cb_data = new callback_data(this);

    // create request envelope
    AsyncLegacyOperationStart * asyncRequest =
        new AsyncLegacyOperationStart (
        _rep->service->get_next_xid(),
        0,
        _rep->cimom->getQueueId(),
        request,
        _rep->cimom->getQueueId());

    if(false == _rep->controller->ClientSendAsync(*_rep->client_handle,
        0,
        _rep->cimom->getQueueId(),
        asyncRequest,
        async_callback,
        (void *)cb_data))
    {
        delete asyncRequest;
        delete cb_data;
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_FOUND, String::EMPTY);
    }



    // send request and wait for response
    // <<< Wed Apr 10 20:30:31 2002 mdd >>>
    // AsyncReply * asyncReply = _rep->service->SendWait(asyncRequest);
    cb_data->client_sem.wait();
    AsyncReply * asyncReply = static_cast<AsyncReply *>(cb_data->get_reply());
    // decode response
    CIMEnumerateInstanceNamesResponseMessage * response =
        reinterpret_cast<CIMEnumerateInstanceNamesResponseMessage *>(
        (static_cast<AsyncLegacyOperationResult *>(asyncReply))->get_result());
    Array<CIMObjectPath> cimReferences = response->instanceNames;
    //_rep->service->return_op(op);

    delete asyncRequest;
    delete asyncReply;
    delete response;
    delete cb_data;

    return(cimReferences);
}

CIMObjectPath CIMOMHandle::createInstance(
    const OperationContext & context,
    const CIMNamespaceName& nameSpace,
    const CIMInstance& newInstance)
{
    if((_rep->service == 0) || (_rep->cimom == 0))
    {
        throw UninitializedObjectException();
    }

    CIMCreateInstanceRequestMessage * request =
        new CIMCreateInstanceRequestMessage(
        XmlWriter::getNextMessageId(),
        nameSpace,
        newInstance,
        QueueIdStack(_rep->cimom->getQueueId(), _rep->service->getQueueId()));

    CIMObjectPath cimReference;

    return(cimReference);
}

void CIMOMHandle::modifyInstance(
    const OperationContext & context,
    const CIMNamespaceName& nameSpace,
    const CIMInstance& modifiedInstance,
    Boolean includeQualifiers,
    const CIMPropertyList& propertyList)
{
    if((_rep->service == 0) || (_rep->cimom == 0))
    {
        throw UninitializedObjectException();
    }

    CIMModifyInstanceRequestMessage * request =
        new CIMModifyInstanceRequestMessage(
        XmlWriter::getNextMessageId(),
        nameSpace,
        CIMInstance(),
        includeQualifiers,
        propertyList,
        QueueIdStack(_rep->cimom->getQueueId(), _rep->service->getQueueId()));

    return;
}

void CIMOMHandle::deleteInstance(
    const OperationContext & context,
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& instanceName)
{
    if((_rep->service == 0) || (_rep->cimom == 0))
    {
        throw UninitializedObjectException();
    }

    CIMDeleteInstanceRequestMessage * request =
        new CIMDeleteInstanceRequestMessage(
        XmlWriter::getNextMessageId(),
        nameSpace,
        instanceName,
        QueueIdStack(_rep->cimom->getQueueId(), _rep->service->getQueueId()));

    return;
}

Array<CIMInstance> CIMOMHandle::execQuery(
    const OperationContext & context,
    const CIMNamespaceName& nameSpace,
    const String& queryLanguage,
    const String& query)
{
    if((_rep->service == 0) || (_rep->cimom == 0))
    {
        throw UninitializedObjectException();
    }

    CIMExecQueryRequestMessage * request =
        new CIMExecQueryRequestMessage(
        XmlWriter::getNextMessageId(),
        nameSpace,
        queryLanguage,
        query,
        QueueIdStack(_rep->cimom->getQueueId(), _rep->service->getQueueId()));

    Array<CIMInstance> cimInstances;

    return(cimInstances);
}

Array<CIMObject> CIMOMHandle::associators(
    const OperationContext & context,
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& objectName,
    const CIMName& assocClass,
    const CIMName& resultClass,
    const String& role,
    const String& resultRole,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
    if((_rep->service == 0) || (_rep->cimom == 0))
    {
        throw UninitializedObjectException();
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
        QueueIdStack(_rep->cimom->getQueueId(), _rep->service->getQueueId()));

    Array<CIMObject> cimObjects;

    return(cimObjects);
}

Array<CIMObjectPath> CIMOMHandle::associatorNames(
    const OperationContext & context,
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& objectName,
    const CIMName& assocClass,
    const CIMName& resultClass,
    const String& role,
    const String& resultRole)
{
    if((_rep->service == 0) || (_rep->cimom == 0))
    {
        throw UninitializedObjectException();
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
        QueueIdStack(_rep->cimom->getQueueId(), _rep->service->getQueueId()));

    Array<CIMObjectPath> cimObjectPaths;

    return(cimObjectPaths);
}

Array<CIMObject> CIMOMHandle::references(
    const OperationContext & context,
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& objectName,
    const CIMName& resultClass,
    const String& role,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
    if((_rep->service == 0) || (_rep->cimom == 0))
    {
        throw UninitializedObjectException();
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
        QueueIdStack(_rep->cimom->getQueueId(), _rep->service->getQueueId()));

    Array<CIMObject> cimObjects;

    return(cimObjects);
}

Array<CIMObjectPath> CIMOMHandle::referenceNames(
    const OperationContext & context,
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& objectName,
    const CIMName& resultClass,
    const String& role)
{
    if((_rep->service == 0) || (_rep->cimom == 0))
    {
        throw UninitializedObjectException();
    }

    CIMReferenceNamesRequestMessage * request =
        new CIMReferenceNamesRequestMessage(
        XmlWriter::getNextMessageId(),
        nameSpace,
        objectName,
        resultClass,
        role,
        QueueIdStack(_rep->cimom->getQueueId(), _rep->service->getQueueId()));

    Array<CIMObjectPath> cimObjectPaths;

    return(cimObjectPaths);
}

CIMValue CIMOMHandle::getProperty(
    const OperationContext & context,
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& instanceName,
    const CIMName& propertyName)
{
    if((_rep->service == 0) || (_rep->cimom == 0))
    {
        throw UninitializedObjectException();
    }

    CIMGetPropertyRequestMessage * request =
        new CIMGetPropertyRequestMessage(
        XmlWriter::getNextMessageId(),
        nameSpace,
        instanceName,
        propertyName,
        QueueIdStack(_rep->cimom->getQueueId(), _rep->service->getQueueId()));

    CIMValue cimValue;

    return(cimValue);
}

void CIMOMHandle::setProperty(
    const OperationContext & context,
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& instanceName,
    const CIMName& propertyName,
    const CIMValue& newValue)
{
    if((_rep->service == 0) || (_rep->cimom == 0))
    {
        throw UninitializedObjectException();
    }

    CIMSetPropertyRequestMessage * request =
        new CIMSetPropertyRequestMessage(
        XmlWriter::getNextMessageId(),
        nameSpace,
        instanceName,
        propertyName,
        newValue,
        QueueIdStack(_rep->cimom->getQueueId(), _rep->service->getQueueId()));

    return;
}

CIMValue CIMOMHandle::invokeMethod(
    const OperationContext & context,
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& instanceName,
    const CIMName& methodName,
    const Array<CIMParamValue>& inParameters,
    Array<CIMParamValue>& outParameters)
{
    if((_rep->service == 0) || (_rep->cimom == 0))
    {
        throw UninitializedObjectException();
    }

    Message* request = new CIMInvokeMethodRequestMessage(
    XmlWriter::getNextMessageId(),
    nameSpace,
    instanceName,
    methodName,
    inParameters,
        QueueIdStack(_rep->cimom->getQueueId(), _rep->service->getQueueId()));

    //outParameters = response->outParameters;
    //return(response->retValue);
    return CIMValue();
}

PEGASUS_NAMESPACE_END
