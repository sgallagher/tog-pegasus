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

#include "ProviderMessageHandler.h"

#include <Pegasus/Common/OperationContextInternal.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/StatisticalData.h>
#include <Pegasus/Common/Logger.h>
#include <Pegasus/Common/MessageLoader.h>
#include <Pegasus/Common/Constants.h>

#include <Pegasus/Provider/CIMInstanceQueryProvider.h>
#include <Pegasus/Provider/CIMAssociationProvider.h>
#include <Pegasus/Provider/CIMMethodProvider.h>
#include <Pegasus/Provider/CIMIndicationProvider.h>
#include <Pegasus/Provider/CIMIndicationConsumerProvider.h>

#include <Pegasus/Query/QueryExpression/QueryExpression.h>
#include <Pegasus/ProviderManager2/QueryExpressionFactory.h>

#include <Pegasus/ProviderManager2/SimpleResponseHandler.h>
#include <Pegasus/ProviderManager2/OperationResponseHandler.h>
#include <Pegasus/ProviderManager2/AutoPThreadSecurity.h>

#define HandleCatch(handler)                                                   \
catch (CIMException& e)                                                        \
{                                                                              \
    PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,                      \
        "Exception: " + e.getMessage());                                       \
    handler.setStatus(e.getCode(), e.getContentLanguages(), e.getMessage());   \
}                                                                              \
catch (Exception& e)                                                           \
{                                                                              \
    PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,                      \
        "Exception: " + e.getMessage());                                       \
    handler.setStatus(CIM_ERR_FAILED, e.getContentLanguages(), e.getMessage());\
}                                                                              \
catch (...)                                                                    \
{                                                                              \
    PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,                      \
        "Exception: Unknown");                                                 \
    handler.setStatus(CIM_ERR_FAILED, "Unknown error.");                       \
}


PEGASUS_NAMESPACE_BEGIN

// auto variable to protect provider during operations
class pm_service_op_lock
{
public:
    pm_service_op_lock(ProviderStatus *providerStatus)
    : _providerStatus(providerStatus)
    {
        _providerStatus->protect();
    }

    ~pm_service_op_lock()
    {
        _providerStatus->unprotect();
    }

private:
    pm_service_op_lock();
    pm_service_op_lock(const pm_service_op_lock&);
    pm_service_op_lock& operator=(const pm_service_op_lock&);

    ProviderStatus* _providerStatus;
};

class op_counter
{
public:
    op_counter(AtomicInt* counter)
        : _counter(counter)
    {
        (*_counter)++;
    }

    ~op_counter()
    {
        (*_counter)--;
    }

private:
    op_counter();
    op_counter(const op_counter&);
    op_counter& operator=(const op_counter&);

    AtomicInt* _counter;
};

template<class T>
inline T* getProviderInterface(CIMProvider* provider)
{
    T * p = dynamic_cast<T *>(provider);

    if (p == 0)
    {
        throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_NOT_SUPPORTED, MessageLoaderParms(
            "ProviderManager.ProviderFacade.INVALID_PROVIDER_INTERFACE",
            "Invalid provider interface."));
    }

    return p;
}

//
// Default Provider Manager
//
ProviderMessageHandler::ProviderMessageHandler(
    const String& name,
    CIMProvider* provider,
    PEGASUS_INDICATION_CALLBACK_T indicationCallback,
    PEGASUS_RESPONSE_CHUNK_CALLBACK_T responseChunkCallback,
    Boolean subscriptionInitComplete)
    : _name(name),
      _provider(provider),
      _indicationCallback(indicationCallback),
      _responseChunkCallback(responseChunkCallback),
      _subscriptionInitComplete(subscriptionInitComplete),
      _indicationResponseHandler(0)
{
}

ProviderMessageHandler::~ProviderMessageHandler()
{
}

String ProviderMessageHandler::getName() const
{
    return _name;
}

void ProviderMessageHandler::setProvider(CIMProvider* provider)
{
    _provider = provider;
}

void ProviderMessageHandler::initialize(CIMOMHandle& cimom)
{
    _provider->initialize(cimom);
}

void ProviderMessageHandler::terminate()
{
    _disableIndications();
    _provider->terminate();
}

void ProviderMessageHandler::subscriptionInitComplete()
{
    if (status.testSubscriptions())
    {
        _enableIndications();
    }

    _subscriptionInitComplete = true;
}

CIMResponseMessage* ProviderMessageHandler::processMessage(
    CIMRequestMessage* request)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "ProviderMessageHandler::processMessage()");

    op_counter ops(&status._currentOperations);

    CIMResponseMessage* response = 0;

    // pass the request message to a handler method based on message type
    switch(request->getType())
    {
    case CIM_GET_INSTANCE_REQUEST_MESSAGE:
        response = _handleGetInstanceRequest(request);
        break;

    case CIM_ENUMERATE_INSTANCES_REQUEST_MESSAGE:
        response = _handleEnumerateInstancesRequest(request);
        break;

    case CIM_ENUMERATE_INSTANCE_NAMES_REQUEST_MESSAGE:
        response = _handleEnumerateInstanceNamesRequest(request);
        break;

    case CIM_CREATE_INSTANCE_REQUEST_MESSAGE:
        response = _handleCreateInstanceRequest(request);
        break;

    case CIM_MODIFY_INSTANCE_REQUEST_MESSAGE:
        response = _handleModifyInstanceRequest(request);
        break;

    case CIM_DELETE_INSTANCE_REQUEST_MESSAGE:
        response = _handleDeleteInstanceRequest(request);
        break;

    case CIM_EXEC_QUERY_REQUEST_MESSAGE:
        response = _handleExecQueryRequest(request);
        break;

    case CIM_ASSOCIATORS_REQUEST_MESSAGE:
        response = _handleAssociatorsRequest(request);
        break;

    case CIM_ASSOCIATOR_NAMES_REQUEST_MESSAGE:
        response = _handleAssociatorNamesRequest(request);
        break;

    case CIM_REFERENCES_REQUEST_MESSAGE:
        response = _handleReferencesRequest(request);
        break;

    case CIM_REFERENCE_NAMES_REQUEST_MESSAGE:
        response = _handleReferenceNamesRequest(request);
        break;

    case CIM_GET_PROPERTY_REQUEST_MESSAGE:
        response = _handleGetPropertyRequest(request);
        break;

    case CIM_SET_PROPERTY_REQUEST_MESSAGE:
        response = _handleSetPropertyRequest(request);
        break;

    case CIM_INVOKE_METHOD_REQUEST_MESSAGE:
        response = _handleInvokeMethodRequest(request);
        break;

    case CIM_CREATE_SUBSCRIPTION_REQUEST_MESSAGE:
        response = _handleCreateSubscriptionRequest(request);
        break;

    case CIM_MODIFY_SUBSCRIPTION_REQUEST_MESSAGE:
        response = _handleModifySubscriptionRequest(request);
        break;

    case CIM_DELETE_SUBSCRIPTION_REQUEST_MESSAGE:
        response = _handleDeleteSubscriptionRequest(request);
        break;

    case CIM_EXPORT_INDICATION_REQUEST_MESSAGE:
        response = _handleExportIndicationRequest(request);
        break;

    default:
        PEGASUS_ASSERT(0);
        break;
    }

    PEG_METHOD_EXIT();

    return response;
}

CIMResponseMessage* ProviderMessageHandler::_handleGetInstanceRequest(
    CIMRequestMessage* message)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "ProviderMessageHandler::_handleGetInstanceRequest");

    CIMGetInstanceRequestMessage* request =
        dynamic_cast<CIMGetInstanceRequestMessage*>(message);
    PEGASUS_ASSERT(request != 0);

    CIMGetInstanceResponseMessage* response =
        dynamic_cast<CIMGetInstanceResponseMessage*>(
            request->buildResponse());
    PEGASUS_ASSERT(response != 0);

    // create a handler for this request
    GetInstanceResponseHandler handler(
        request, response, _responseChunkCallback);

    try
    {
        Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            "ProviderMessageHandler::_handleGetInstanceRequest - "
                "Host name: $0  Name space: $1  Class name: $2",
            System::getHostName(),
            request->nameSpace.getString(),
            request->instanceName.getClassName().getString());

        // make target object path
        CIMObjectPath objectPath(
            System::getHostName(),
            request->nameSpace,
            request->instanceName.getClassName(),
            request->instanceName.getKeyBindings());

        // convert arguments
        OperationContext context;

        context.insert(request->operationContext.get(IdentityContainer::NAME));
        context.insert(request->operationContext.get(AcceptLanguageListContainer::NAME));
        context.insert(request->operationContext.get(ContentLanguageListContainer::NAME));

        // forward request
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Calling provider.getInstance: " + _name);

        AutoPThreadSecurity threadLevelSecurity(context);

        pm_service_op_lock op_lock(&status);

        StatProviderTimeMeasurement providerTime(response);

        CIMInstanceProvider* provider =
            getProviderInterface<CIMInstanceProvider>(_provider);

        provider->getInstance(
            context,
            objectPath,
            request->includeQualifiers,
            request->includeClassOrigin,
            request->propertyList,
            handler);
    }
    HandleCatch(handler);

    PEG_METHOD_EXIT();
    return(response);
}

CIMResponseMessage* ProviderMessageHandler::_handleEnumerateInstancesRequest(
    CIMRequestMessage* message)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "ProviderMessageHandler::_handleEnumerateInstanceRequest");

    CIMEnumerateInstancesRequestMessage* request =
        dynamic_cast<CIMEnumerateInstancesRequestMessage*>(message);
    PEGASUS_ASSERT(request != 0);

    CIMEnumerateInstancesResponseMessage* response =
        dynamic_cast<CIMEnumerateInstancesResponseMessage*>(
            request->buildResponse());
    PEGASUS_ASSERT(response != 0);

    // create a handler for this request
    EnumerateInstancesResponseHandler handler(
        request, response, _responseChunkCallback);

    try
    {
        PEG_LOGGER_TRACE((Logger::STANDARD_LOG, System::CIMSERVER,
            Logger::TRACE,
            "ProviderMessageHandler::_handleEnumerateInstancesRequest - "
                "Host name: $0  Name space: $1  Class name: $2",
            System::getHostName(),
            request->nameSpace.getString(),
            request->className.getString()));

        // make target object path
        CIMObjectPath objectPath(
            System::getHostName(),
            request->nameSpace,
            request->className);

        // convert arguments
        OperationContext context;

        context.insert(request->operationContext.get(IdentityContainer::NAME));
        context.insert(request->operationContext.get(AcceptLanguageListContainer::NAME));
        context.insert(request->operationContext.get(ContentLanguageListContainer::NAME));

        // forward request
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Calling provider.enumerateInstances: " + _name);

        AutoPThreadSecurity threadLevelSecurity(context);

        pm_service_op_lock op_lock(&status);

        StatProviderTimeMeasurement providerTime(response);

        CIMInstanceProvider* provider =
            getProviderInterface<CIMInstanceProvider>(_provider);

        provider->enumerateInstances(
            context,
            objectPath,
            request->includeQualifiers,
            request->includeClassOrigin,
            request->propertyList,
            handler);

    }
    HandleCatch(handler);

    PEG_METHOD_EXIT();

    return(response);
}

CIMResponseMessage* ProviderMessageHandler::_handleEnumerateInstanceNamesRequest(
    CIMRequestMessage* message)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "ProviderMessageHandler::_handleEnumerateInstanceNamesRequest");

    CIMEnumerateInstanceNamesRequestMessage* request =
        dynamic_cast<CIMEnumerateInstanceNamesRequestMessage*>(message);
    PEGASUS_ASSERT(request != 0);

    CIMEnumerateInstanceNamesResponseMessage* response =
        dynamic_cast<CIMEnumerateInstanceNamesResponseMessage*>(
            request->buildResponse());
    PEGASUS_ASSERT(response != 0);

    // create a handler for this request
    EnumerateInstanceNamesResponseHandler handler(
        request, response, _responseChunkCallback);

    // process the request
    try
    {
        PEG_LOGGER_TRACE((Logger::STANDARD_LOG, System::CIMSERVER,
        Logger::TRACE,
            "ProviderMessageHandler::_handleEnumerateInstanceNamesRequest - "
                "Host name: $0  Name space: $1  Class name: $2",
            System::getHostName(),
            request->nameSpace.getString(),
            request->className.getString()));

        // make target object path
        CIMObjectPath objectPath(
            System::getHostName(),
            request->nameSpace,
            request->className);

        // convert arguments
        OperationContext context;

        context.insert(request->operationContext.get(IdentityContainer::NAME));
        context.insert(request->operationContext.get(AcceptLanguageListContainer::NAME));
        context.insert(request->operationContext.get(ContentLanguageListContainer::NAME));

        // forward request
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Calling provider.enumerateInstanceNames: " + _name);

        AutoPThreadSecurity threadLevelSecurity(context);

        pm_service_op_lock op_lock(&status);

        StatProviderTimeMeasurement providerTime(response);

        CIMInstanceProvider* provider =
            getProviderInterface<CIMInstanceProvider>(_provider);

        provider->enumerateInstanceNames(
            context,
            objectPath,
            handler);

    }
    HandleCatch(handler);

    PEG_METHOD_EXIT();

    return(response);
}

CIMResponseMessage* ProviderMessageHandler::_handleCreateInstanceRequest(
    CIMRequestMessage* message)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "ProviderMessageHandler::_handleCreateInstanceRequest");

    CIMCreateInstanceRequestMessage* request =
        dynamic_cast<CIMCreateInstanceRequestMessage*>(message);
    PEGASUS_ASSERT(request != 0);

    // create response message
    CIMCreateInstanceResponseMessage* response =
        dynamic_cast<CIMCreateInstanceResponseMessage*>(
            request->buildResponse());
    PEGASUS_ASSERT(response != 0);

    // create a handler for this request
    CreateInstanceResponseHandler handler(
        request, response, _responseChunkCallback);

    try
    {
        PEG_LOGGER_TRACE((Logger::STANDARD_LOG, System::CIMSERVER,
        Logger::TRACE,
            "ProviderMessageHandler::_handleCreateInstanceRequest - "
                "Host name: $0  Name space: $1  Class name: $2",
            System::getHostName(),
            request->nameSpace.getString(),
            request->newInstance.getPath().getClassName().getString()));

        // make target object path
        CIMObjectPath objectPath(
            System::getHostName(),
            request->nameSpace,
            request->newInstance.getPath().getClassName(),
            request->newInstance.getPath().getKeyBindings());

        // convert arguments
        OperationContext context;

        context.insert(request->operationContext.get(IdentityContainer::NAME));
        context.insert(request->operationContext.get(AcceptLanguageListContainer::NAME));
        context.insert(request->operationContext.get(ContentLanguageListContainer::NAME));

        // forward request
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Calling provider.createInstance: " + _name);

        AutoPThreadSecurity threadLevelSecurity(context);

        pm_service_op_lock op_lock(&status);

        StatProviderTimeMeasurement providerTime(response);

        CIMInstanceProvider* provider =
            getProviderInterface<CIMInstanceProvider>(_provider);

        provider->createInstance(
            context,
            objectPath,
            request->newInstance,
            handler);

    }
    HandleCatch(handler);

    PEG_METHOD_EXIT();

    return(response);
}

CIMResponseMessage* ProviderMessageHandler::_handleModifyInstanceRequest(
    CIMRequestMessage* message)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "ProviderMessageHandler::_handleModifyInstanceRequest");

    CIMModifyInstanceRequestMessage* request =
        dynamic_cast<CIMModifyInstanceRequestMessage*>(message);
    PEGASUS_ASSERT(request != 0);

    // create response message
    CIMModifyInstanceResponseMessage* response =
        dynamic_cast<CIMModifyInstanceResponseMessage*>(
            request->buildResponse());
    PEGASUS_ASSERT(response != 0);

    // create a handler for this request
    ModifyInstanceResponseHandler handler(
        request, response, _responseChunkCallback);

    try
    {
        PEG_LOGGER_TRACE((Logger::STANDARD_LOG, System::CIMSERVER,
        Logger::TRACE,
            "ProviderMessageHandler::_handleModifyInstanceRequest - "
                "Host name: $0  Name space: $1  Class name: $2",
            System::getHostName(),
            request->nameSpace.getString(),
            request->modifiedInstance.getPath().getClassName().getString()));

        // make target object path
        CIMObjectPath objectPath(
            System::getHostName(),
            request->nameSpace,
            request->modifiedInstance.getPath ().getClassName(),
            request->modifiedInstance.getPath ().getKeyBindings());

        // convert arguments
        OperationContext context;

        context.insert(request->operationContext.get(IdentityContainer::NAME));
        context.insert(request->operationContext.get(AcceptLanguageListContainer::NAME));
        context.insert(request->operationContext.get(ContentLanguageListContainer::NAME));

        // forward request
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Calling provider.modifyInstance: " + _name);

        AutoPThreadSecurity threadLevelSecurity(context);

        pm_service_op_lock op_lock(&status);

        StatProviderTimeMeasurement providerTime(response);

        CIMInstanceProvider* provider =
            getProviderInterface<CIMInstanceProvider>(_provider);

        provider->modifyInstance(
            context,
            objectPath,
            request->modifiedInstance,
            request->includeQualifiers,
            request->propertyList,
            handler);

    }
    HandleCatch(handler);

    PEG_METHOD_EXIT();

    return(response);
}

CIMResponseMessage* ProviderMessageHandler::_handleDeleteInstanceRequest(
    CIMRequestMessage* message)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "ProviderMessageHandler::_handleDeleteInstanceRequest");

    CIMDeleteInstanceRequestMessage* request =
        dynamic_cast<CIMDeleteInstanceRequestMessage*>(message);
    PEGASUS_ASSERT(request != 0);

    // create response message
    CIMDeleteInstanceResponseMessage* response =
        dynamic_cast<CIMDeleteInstanceResponseMessage*>(
            request->buildResponse());
    PEGASUS_ASSERT(response != 0);

    // create a handler for this request
    DeleteInstanceResponseHandler handler(
        request, response, _responseChunkCallback);

    try
    {
        PEG_LOGGER_TRACE((Logger::STANDARD_LOG, System::CIMSERVER,
        Logger::TRACE,
            "ProviderMessageHandler::_handleDeleteInstanceRequest - "
                "Host name: $0  Name space: $1  Class name: $2",
            System::getHostName(),
            request->nameSpace.getString(),
            request->instanceName.getClassName().getString()));

        // make target object path
        CIMObjectPath objectPath(
            System::getHostName(),
            request->nameSpace,
            request->instanceName.getClassName(),
            request->instanceName.getKeyBindings());

        // convert arguments
        OperationContext context;

        context.insert(request->operationContext.get(IdentityContainer::NAME));
        context.insert(request->operationContext.get(AcceptLanguageListContainer::NAME));
        context.insert(request->operationContext.get(ContentLanguageListContainer::NAME));

        // forward request
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Calling provider.deleteInstance: " + _name);

        AutoPThreadSecurity threadLevelSecurity(context);

        pm_service_op_lock op_lock(&status);

        StatProviderTimeMeasurement providerTime(response);

        CIMInstanceProvider* provider =
            getProviderInterface<CIMInstanceProvider>(_provider);

        provider->deleteInstance(
            context,
            objectPath,
            handler);

    }
    HandleCatch(handler);

    PEG_METHOD_EXIT();

    return(response);
}

CIMResponseMessage* ProviderMessageHandler::_handleExecQueryRequest(
    CIMRequestMessage* message)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "ProviderMessageHandler::_handleExecQueryRequest");

    CIMExecQueryRequestMessage* request =
        dynamic_cast<CIMExecQueryRequestMessage*>(message);
    PEGASUS_ASSERT(request != 0);

    CIMExecQueryResponseMessage* response =
        dynamic_cast<CIMExecQueryResponseMessage*>(
            request->buildResponse());
    PEGASUS_ASSERT(response != 0);

    // create a handler for this request
    ExecQueryResponseHandler handler(
        request, response, _responseChunkCallback);

    try
    {
        PEG_LOGGER_TRACE((Logger::STANDARD_LOG, System::CIMSERVER,
        Logger::TRACE,
            "ProviderMessageHandler::_handleExecQueryRequest - "
                "Host name: $0  Name space: $1  Class name: $2",
            System::getHostName(),
            request->nameSpace.getString(),
            request->className.getString()));

        // make target object path
        CIMObjectPath objectPath(
            System::getHostName(),
            request->nameSpace,
            request->className);

        // convert arguments
        OperationContext context;

        context.insert(request->operationContext.get(IdentityContainer::NAME));
        context.insert(request->operationContext.get(AcceptLanguageListContainer::NAME));
        context.insert(request->operationContext.get(ContentLanguageListContainer::NAME));

        QueryExpression qx(request->queryLanguage,request->query);

        // forward request
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Calling provider.execQuery: " + _name);

        AutoPThreadSecurity threadLevelSecurity(context);

        pm_service_op_lock op_lock(&status);

        StatProviderTimeMeasurement providerTime(response);

        CIMInstanceQueryProvider* provider =
            getProviderInterface<CIMInstanceQueryProvider>(_provider);

        provider->execQuery(
            context,
            objectPath,
            qx,
            handler);

    }
    HandleCatch(handler);

    PEG_METHOD_EXIT();

    return(response);
}

CIMResponseMessage* ProviderMessageHandler::_handleAssociatorsRequest(
    CIMRequestMessage* message)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "ProviderMessageHandler::_handleAssociatorsRequest");

    CIMAssociatorsRequestMessage* request =
        dynamic_cast<CIMAssociatorsRequestMessage*>(message);
    PEGASUS_ASSERT(request != 0);

    CIMAssociatorsResponseMessage* response =
        dynamic_cast<CIMAssociatorsResponseMessage*>(
            request->buildResponse());
    PEGASUS_ASSERT(response != 0);

    // create a handler for this request
    AssociatorsResponseHandler handler(
        request, response, _responseChunkCallback);

    // process the request
    try
    {
        PEG_LOGGER_TRACE((Logger::STANDARD_LOG, System::CIMSERVER,
        Logger::TRACE,
            "ProviderMessageHandler::_handleAssociatorsRequest - "
                "Host name: $0  Name space: $1  Class name: $2",
            System::getHostName(),
            request->nameSpace.getString(),
            request->objectName.getClassName().getString()));

        // make target object path
        CIMObjectPath objectPath(
            System::getHostName(),
            request->nameSpace,
            request->objectName.getClassName());

        objectPath.setKeyBindings(request->objectName.getKeyBindings());

        CIMObjectPath assocPath(
            System::getHostName(),
            request->nameSpace,
            request->assocClass.getString());

        // convert arguments
        OperationContext context;

        context.insert(request->operationContext.get(IdentityContainer::NAME));
        context.insert(request->operationContext.get(AcceptLanguageListContainer::NAME));
        context.insert(request->operationContext.get(ContentLanguageListContainer::NAME));

        AutoPThreadSecurity threadLevelSecurity(context);

        StatProviderTimeMeasurement providerTime(response);

        pm_service_op_lock op_lock(&status);

        CIMAssociationProvider* provider =
            getProviderInterface<CIMAssociationProvider>(_provider);

        provider->associators(
            context,
            objectPath,
            request->assocClass,
            request->resultClass,
            request->role,
            request->resultRole,
            request->includeQualifiers,
            request->includeClassOrigin,
            request->propertyList,
            handler);

    }
    HandleCatch(handler);

    PEG_METHOD_EXIT();

    return(response);
}

CIMResponseMessage* ProviderMessageHandler::_handleAssociatorNamesRequest(
    CIMRequestMessage* message)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "ProviderMessageHandler::_handleAssociatorNamesRequest");

    CIMAssociatorNamesRequestMessage* request =
        dynamic_cast<CIMAssociatorNamesRequestMessage*>(message);
    PEGASUS_ASSERT(request != 0);

    CIMAssociatorNamesResponseMessage* response =
        dynamic_cast<CIMAssociatorNamesResponseMessage*>(
            request->buildResponse());
    PEGASUS_ASSERT(response != 0);

    // create a handler for this request
    AssociatorNamesResponseHandler handler(
        request, response, _responseChunkCallback);

    // process the request
    try
    {
        PEG_LOGGER_TRACE((Logger::STANDARD_LOG, System::CIMSERVER,
        Logger::TRACE,
            "ProviderMessageHandler::_handleAssociationNamesRequest - "
                "Host name: $0  Name space: $1  Class name: $2",
            System::getHostName(),
            request->nameSpace.getString(),
            request->objectName.getClassName().getString()));

        // make target object path
        CIMObjectPath objectPath(
            System::getHostName(),
            request->nameSpace,
            request->objectName.getClassName());

        objectPath.setKeyBindings(request->objectName.getKeyBindings());

        CIMObjectPath assocPath(
            System::getHostName(),
            request->nameSpace,
            request->assocClass.getString());

        // convert arguments
        OperationContext context;

        context.insert(request->operationContext.get(IdentityContainer::NAME));
        context.insert(request->operationContext.get(AcceptLanguageListContainer::NAME));
        context.insert(request->operationContext.get(ContentLanguageListContainer::NAME));

        AutoPThreadSecurity threadLevelSecurity(context);

        StatProviderTimeMeasurement providerTime(response);

        pm_service_op_lock op_lock(&status);

        CIMAssociationProvider* provider =
            getProviderInterface<CIMAssociationProvider>(_provider);

        provider->associatorNames(
            context,
            objectPath,
            request->assocClass,
            request->resultClass,
            request->role,
            request->resultRole,
            handler);

    }
    HandleCatch(handler);

    PEG_METHOD_EXIT();

    return(response);
}

CIMResponseMessage* ProviderMessageHandler::_handleReferencesRequest(
    CIMRequestMessage* message)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "ProviderMessageHandler::_handleReferencesRequest");

    CIMReferencesRequestMessage* request =
        dynamic_cast<CIMReferencesRequestMessage*>(message);
    PEGASUS_ASSERT(request != 0);

    CIMReferencesResponseMessage* response =
        dynamic_cast<CIMReferencesResponseMessage*>(
            request->buildResponse());
    PEGASUS_ASSERT(response != 0);

    // create a handler for this request
    ReferencesResponseHandler handler(
        request, response, _responseChunkCallback);

    // process the request
    try
    {
        PEG_LOGGER_TRACE((Logger::STANDARD_LOG, System::CIMSERVER,
        Logger::TRACE,
            "ProviderMessageHandler::_handleReferencesRequest - "
                "Host name: $0  Name space: $1  Class name: $2",
            System::getHostName(),
            request->nameSpace.getString(),
            request->objectName.getClassName().getString()));

        // make target object path
        CIMObjectPath objectPath(
            System::getHostName(),
            request->nameSpace,
            request->objectName.getClassName());

        objectPath.setKeyBindings(request->objectName.getKeyBindings());

        CIMObjectPath resultPath(
            System::getHostName(),
            request->nameSpace,
            request->resultClass.getString());

        // convert arguments
        OperationContext context;

        context.insert(request->operationContext.get(IdentityContainer::NAME));
        context.insert(request->operationContext.get(AcceptLanguageListContainer::NAME));
        context.insert(request->operationContext.get(ContentLanguageListContainer::NAME));

        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Calling provider.references: " + _name);

        AutoPThreadSecurity threadLevelSecurity(context);

        StatProviderTimeMeasurement providerTime(response);

        pm_service_op_lock op_lock(&status);

        CIMAssociationProvider* provider =
            getProviderInterface<CIMAssociationProvider>(_provider);

        provider->references(
            context,
            objectPath,
            request->resultClass,
            request->role,
            request->includeQualifiers,
            request->includeClassOrigin,
            request->propertyList,
            handler);

    }
    HandleCatch(handler);

    PEG_METHOD_EXIT();

    return(response);
}

CIMResponseMessage* ProviderMessageHandler::_handleReferenceNamesRequest(
    CIMRequestMessage* message)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "ProviderMessageHandler::_handleReferenceNamesRequest");

    CIMReferenceNamesRequestMessage* request =
        dynamic_cast<CIMReferenceNamesRequestMessage*>(message);
    PEGASUS_ASSERT(request != 0);

    CIMReferenceNamesResponseMessage* response =
        dynamic_cast<CIMReferenceNamesResponseMessage*>(
            request->buildResponse());
    PEGASUS_ASSERT(response != 0);

    // create a handler for this request
    ReferenceNamesResponseHandler handler(
        request, response, _responseChunkCallback);

    // process the request
    try
    {
        PEG_LOGGER_TRACE((Logger::STANDARD_LOG, System::CIMSERVER,
        Logger::TRACE,
            "ProviderMessageHandler::_handleReferenceNamesRequest - "
                "Host name: $0  Name space: $1  Class name: $2",
            System::getHostName(),
            request->nameSpace.getString(),
            request->objectName.getClassName().getString()));

        // make target object path
        CIMObjectPath objectPath(
            System::getHostName(),
            request->nameSpace,
            request->objectName.getClassName());

        objectPath.setKeyBindings(request->objectName.getKeyBindings());

        CIMObjectPath resultPath(
            System::getHostName(),
            request->nameSpace,
            request->resultClass.getString());

        // convert arguments
        OperationContext context;

        context.insert(request->operationContext.get(IdentityContainer::NAME));
        context.insert(request->operationContext.get(AcceptLanguageListContainer::NAME));
        context.insert(request->operationContext.get(ContentLanguageListContainer::NAME));

        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Calling provider.referenceNames: " + _name);

        AutoPThreadSecurity threadLevelSecurity(context);

        StatProviderTimeMeasurement providerTime(response);

        pm_service_op_lock op_lock(&status);

        CIMAssociationProvider* provider =
            getProviderInterface<CIMAssociationProvider>(_provider);

        provider->referenceNames(
            context,
            objectPath,
            request->resultClass,
            request->role,
            handler);

    }
    HandleCatch(handler);

    PEG_METHOD_EXIT();

    return(response);
}

CIMResponseMessage* ProviderMessageHandler::_handleGetPropertyRequest(
    CIMRequestMessage* message)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "ProviderMessageHandler::_handleGetPropertyRequest");

    CIMGetPropertyRequestMessage* request =
        dynamic_cast<CIMGetPropertyRequestMessage*>(message);
    PEGASUS_ASSERT(request != 0);

    CIMGetPropertyResponseMessage* response =
        dynamic_cast<CIMGetPropertyResponseMessage*>(
            request->buildResponse());
    PEGASUS_ASSERT(response != 0);

    // create a handler for this request
    SimpleInstanceResponseHandler handler;

    try
    {
        Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            "ProviderMessageHandler::_handleGetPropertyRequest - "
                "Host name: $0  Name space: $1  Class name: $2  Property: $3",
            System::getHostName(),
            request->nameSpace.getString(),
            request->instanceName.getClassName().getString(),
            request->propertyName.getString());

        // make target object path
        CIMObjectPath objectPath(
            System::getHostName(),
            request->nameSpace,
            request->instanceName.getClassName(),
            request->instanceName.getKeyBindings());

        Array<CIMName> propertyList;
        propertyList.append(request->propertyName);

        // convert arguments
        OperationContext context;

        context.insert(request->operationContext.get(IdentityContainer::NAME));
        context.insert(request->operationContext.get(AcceptLanguageListContainer::NAME));
        context.insert(request->operationContext.get(ContentLanguageListContainer::NAME));

        // forward request
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Calling provider.getInstance: " + _name);

        AutoPThreadSecurity threadLevelSecurity(context);

        pm_service_op_lock op_lock(&status);

        StatProviderTimeMeasurement providerTime(response);

        CIMInstanceProvider* provider =
            getProviderInterface<CIMInstanceProvider>(_provider);

        provider->getInstance(
            context,
            objectPath,
            false,  // includeQualifiers
            false,  // includeClassOrigin
            propertyList,
            handler);

        if (handler.getObjects().size())
        {
            CIMInstance instance = handler.getObjects()[0];

            Uint32 pos = instance.findProperty(request->propertyName);

            if (pos != PEG_NOT_FOUND)
            {
                response->value = instance.getProperty(pos).getValue();
            }
            else    // Property not found. Return CIM_ERR_NO_SUCH_PROPERTY.
            {
                response->cimException = PEGASUS_CIM_EXCEPTION(
                    CIM_ERR_NO_SUCH_PROPERTY,
                    request->propertyName.getString());
            }
        }

        response->operationContext.set(
            ContentLanguageListContainer(handler.getLanguages()));
    }
    catch (CIMException& e)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Exception: " + e.getMessage());
        response->cimException = PEGASUS_CIM_EXCEPTION_LANG(
            e.getContentLanguages(), e.getCode(), e.getMessage());
    }
    catch (Exception& e)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Exception: " + e.getMessage());
        response->cimException = PEGASUS_CIM_EXCEPTION_LANG(
            e.getContentLanguages(), CIM_ERR_FAILED, e.getMessage());
    }
    catch (...)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Exception: Unknown");
        response->cimException = PEGASUS_CIM_EXCEPTION(
            CIM_ERR_FAILED, "Unknown error.");
    }

    PEG_METHOD_EXIT();
    return(response);
}

CIMResponseMessage* ProviderMessageHandler::_handleSetPropertyRequest(
    CIMRequestMessage* message)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "ProviderMessageHandler::_handleSetPropertyRequest");

    CIMSetPropertyRequestMessage* request =
        dynamic_cast<CIMSetPropertyRequestMessage*>(message);
    PEGASUS_ASSERT(request != 0);

    CIMSetPropertyResponseMessage* response =
        dynamic_cast<CIMSetPropertyResponseMessage*>(
            request->buildResponse());
    PEGASUS_ASSERT(response != 0);

    // create a handler for this request
    SimpleInstanceResponseHandler handler;

    try
    {
        Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            "ProviderMessageHandler::_handleSetPropertyRequest - "
                "Host name: $0  Name space: $1  Class name: $2  Property: $3",
            System::getHostName(),
            request->nameSpace.getString(),
            request->instanceName.getClassName().getString(),
            request->propertyName.getString());

        // make target object path
        CIMObjectPath objectPath(
            System::getHostName(),
            request->nameSpace,
            request->instanceName.getClassName(),
            request->instanceName.getKeyBindings());

        CIMInstance instance(request->instanceName.getClassName());
        instance.addProperty(CIMProperty(
            request->propertyName, request->newValue));

        Array<CIMName> propertyList;
        propertyList.append(request->propertyName);

        // convert arguments
        OperationContext context;

        context.insert(request->operationContext.get(IdentityContainer::NAME));
        context.insert(request->operationContext.get(AcceptLanguageListContainer::NAME));
        context.insert(request->operationContext.get(ContentLanguageListContainer::NAME));

        // forward request
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Calling provider.modifyInstance: " + _name);

        AutoPThreadSecurity threadLevelSecurity(context);

        pm_service_op_lock op_lock(&status);

        StatProviderTimeMeasurement providerTime(response);

        CIMInstanceProvider* provider =
            getProviderInterface<CIMInstanceProvider>(_provider);

        provider->modifyInstance(
            context,
            objectPath,
            instance,
            false,  // includeQualifiers
            propertyList,
            handler);

        response->operationContext.set(
            ContentLanguageListContainer(handler.getLanguages()));
    }
    catch (CIMException& e)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Exception: " + e.getMessage());
        response->cimException = PEGASUS_CIM_EXCEPTION_LANG(
            e.getContentLanguages(), e.getCode(), e.getMessage());
    }
    catch (Exception& e)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Exception: " + e.getMessage());
        response->cimException = PEGASUS_CIM_EXCEPTION_LANG(
            e.getContentLanguages(), CIM_ERR_FAILED, e.getMessage());
    }
    catch (...)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Exception: Unknown");
        response->cimException = PEGASUS_CIM_EXCEPTION(
            CIM_ERR_FAILED, "Unknown error.");
    }

    PEG_METHOD_EXIT();
    return(response);
}

CIMResponseMessage* ProviderMessageHandler::_handleInvokeMethodRequest(
    CIMRequestMessage* message)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "ProviderMessageHandler::_handleInvokeMethodRequest");

    CIMInvokeMethodRequestMessage* request =
        dynamic_cast<CIMInvokeMethodRequestMessage*>(message);
    PEGASUS_ASSERT(request != 0);

    // create response message
    CIMInvokeMethodResponseMessage* response =
        dynamic_cast<CIMInvokeMethodResponseMessage*>(
            request->buildResponse());
    PEGASUS_ASSERT(response != 0);

    // create a handler for this request
    InvokeMethodResponseHandler handler(
        request, response, _responseChunkCallback);

    try
    {
        PEG_LOGGER_TRACE((Logger::STANDARD_LOG, System::CIMSERVER,
        Logger::TRACE,
            "ProviderMessageHandler::_handleInvokeMethodRequest - "
                "Host name: $0  Name space: $1  Class name: $2",
            System::getHostName(),
            request->nameSpace.getString(),
            request->instanceName.getClassName().getString()));

        // make target object path
        CIMObjectPath objectPath(
            System::getHostName(),
            request->nameSpace,
            request->instanceName.getClassName(),
            request->instanceName.getKeyBindings());

        // convert arguments
        OperationContext context;

        context.insert(request->operationContext.get(IdentityContainer::NAME));
        context.insert(request->operationContext.get(AcceptLanguageListContainer::NAME));
        context.insert(request->operationContext.get(ContentLanguageListContainer::NAME));

        CIMObjectPath instanceReference(request->instanceName);
        instanceReference.setNameSpace(request->nameSpace);

        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Calling provider.invokeMethod: " + _name);

        AutoPThreadSecurity threadLevelSecurity(context);

        StatProviderTimeMeasurement providerTime(response);

        pm_service_op_lock op_lock(&status);

        CIMMethodProvider* provider =
            getProviderInterface<CIMMethodProvider>(_provider);

        provider->invokeMethod(
            context,
            instanceReference,
            request->methodName,
            request->inParameters,
            handler);

    }
    HandleCatch(handler);

    PEG_METHOD_EXIT();

    return(response);
}

CIMResponseMessage* ProviderMessageHandler::_handleCreateSubscriptionRequest(
    CIMRequestMessage* message)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "ProviderMessageHandler::_handleCreateSubscriptionRequest");

    CIMCreateSubscriptionRequestMessage* request =
        dynamic_cast<CIMCreateSubscriptionRequestMessage*>(message);
    PEGASUS_ASSERT(request != 0);

    CIMCreateSubscriptionResponseMessage* response =
        dynamic_cast<CIMCreateSubscriptionResponseMessage*>(
            request->buildResponse());
    PEGASUS_ASSERT(response != 0);

    OperationResponseHandler handler(
        request, response, _responseChunkCallback);

    try
    {
        String temp;

        for (Uint32 i = 0, n = request->classNames.size(); i < n; i++)
        {
            temp.append(request->classNames[i].getString());

            if (i < (n - 1))
            {
                temp.append(", ");
            }
        }

        PEG_LOGGER_TRACE((Logger::STANDARD_LOG, System::CIMSERVER,
            Logger::TRACE,
            "ProviderMessageHandler::_handleCreateSubscriptionRequest - "
                "Host name: $0  Name space: $1  Class name(s): $2",
            System::getHostName(),
            request->nameSpace.getString(),
            temp));

        //
        //  Save the provider instance from the request
        //
        ProviderIdContainer pidc = (ProviderIdContainer)
            request->operationContext.get(ProviderIdContainer::NAME);
        status.setProviderInstance(pidc.getProvider());

        // convert arguments
        OperationContext context;

        context.insert(request->operationContext.get(IdentityContainer::NAME));
        context.insert(request->operationContext.get(AcceptLanguageListContainer::NAME));
        context.insert(request->operationContext.get(ContentLanguageListContainer::NAME));
        context.insert(request->operationContext.get(SubscriptionInstanceContainer::NAME));
        context.insert(request->operationContext.get(SubscriptionFilterConditionContainer::NAME));
        context.insert(request->operationContext.get(SubscriptionFilterQueryContainer::NAME));

        Array<CIMObjectPath> classNames;

        for (Uint32 i = 0, n = request->classNames.size(); i < n; i++)
        {
            CIMObjectPath className(
                System::getHostName(),
                request->nameSpace,
                request->classNames[i]);

            classNames.append(className);
        }

        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Calling provider.createSubscription: " + _name);

        AutoPThreadSecurity threadLevelSecurity(context);

        pm_service_op_lock op_lock(&status);

        CIMIndicationProvider* provider =
            getProviderInterface<CIMIndicationProvider>(_provider);

        provider->createSubscription(
            context,
            request->subscriptionInstance.getPath(),
            classNames,
            request->propertyList,
            request->repeatNotificationPolicy);

        //
        //  Increment count of current subscriptions for this provider
        //
        if (status.testIfZeroAndIncrementSubscriptions())
        {
            PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                "First accepted subscription");

            //
            //  If there were no current subscriptions before the increment,
            //  the first subscription has been created
            //  Call the provider's enableIndications method
            //
            if (_subscriptionInitComplete)
            {
                _enableIndications();
            }
        }
    }
    HandleCatch(handler);

    PEG_METHOD_EXIT();
    return(response);
}

CIMResponseMessage* ProviderMessageHandler::_handleModifySubscriptionRequest(
    CIMRequestMessage* message)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "ProviderMessageHandler::_handleModifySubscriptionRequest");

    CIMModifySubscriptionRequestMessage* request =
        dynamic_cast<CIMModifySubscriptionRequestMessage*>(message);
    PEGASUS_ASSERT(request != 0);

    CIMModifySubscriptionResponseMessage* response =
        dynamic_cast<CIMModifySubscriptionResponseMessage*>(
            request->buildResponse());
    PEGASUS_ASSERT(response != 0);

    OperationResponseHandler handler(
        request, response, _responseChunkCallback);

    try
    {
        String temp;

        for (Uint32 i = 0, n = request->classNames.size(); i < n; i++)
        {
            temp.append(request->classNames[i].getString());

            if (i < (n - 1))
            {
                temp.append(", ");
            }
        }

        PEG_LOGGER_TRACE((Logger::STANDARD_LOG, System::CIMSERVER,
        Logger::TRACE,
            "ProviderMessageHandler::_handleCreateSubscriptionRequest - "
                "Host name: $0  Name space: $1  Class name(s): $2",
            System::getHostName(),
            request->nameSpace.getString(),
            temp));

        // convert arguments
        OperationContext context;
        context.insert(request->operationContext.get(IdentityContainer::NAME));
        context.insert(request->operationContext.get(AcceptLanguageListContainer::NAME));
        context.insert(request->operationContext.get(ContentLanguageListContainer::NAME));
        context.insert(request->operationContext.get(SubscriptionInstanceContainer::NAME));
        context.insert(request->operationContext.get(SubscriptionFilterConditionContainer::NAME));
        context.insert(request->operationContext.get(SubscriptionFilterQueryContainer::NAME));

        Array<CIMObjectPath> classNames;

        for (Uint32 i = 0, n = request->classNames.size(); i < n; i++)
        {
            CIMObjectPath className(
                System::getHostName(),
                request->nameSpace,
                request->classNames[i]);

            classNames.append(className);
        }

        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Calling provider.modifySubscription: " + _name);

        AutoPThreadSecurity threadLevelSecurity(context);

        pm_service_op_lock op_lock(&status);

        CIMIndicationProvider* provider =
            getProviderInterface<CIMIndicationProvider>(_provider);

        provider->modifySubscription(
            context,
            request->subscriptionInstance.getPath(),
            classNames,
            request->propertyList,
            request->repeatNotificationPolicy);

    }
    HandleCatch(handler);

    PEG_METHOD_EXIT();
    return(response);
}

CIMResponseMessage* ProviderMessageHandler::_handleDeleteSubscriptionRequest(
    CIMRequestMessage* message)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "ProviderMessageHandler::_handleDeleteSubscriptionRequest");

    CIMDeleteSubscriptionRequestMessage* request =
        dynamic_cast<CIMDeleteSubscriptionRequestMessage*>(message);
    PEGASUS_ASSERT(request != 0);

    CIMDeleteSubscriptionResponseMessage* response =
        dynamic_cast<CIMDeleteSubscriptionResponseMessage*>(
            request->buildResponse());
    PEGASUS_ASSERT(response != 0);

    OperationResponseHandler handler(
        request, response, _responseChunkCallback);

    try
    {
        String temp;

        for (Uint32 i = 0, n = request->classNames.size(); i < n; i++)
        {
            temp.append(request->classNames[i].getString());

            if (i < (n - 1))
            {
                temp.append(", ");
            }
        }

        PEG_LOGGER_TRACE((Logger::STANDARD_LOG, System::CIMSERVER,
        Logger::TRACE,
            "ProviderMessageHandler::_handleDeleteSubscriptionRequest - "
                "Host name: $0  Name space: $1  Class name(s): $2",
            System::getHostName(),
            request->nameSpace.getString(),
            temp));

        // convert arguments
        OperationContext context;

        context.insert(request->operationContext.get(IdentityContainer::NAME));
        context.insert(request->operationContext.get(AcceptLanguageListContainer::NAME));
        context.insert(request->operationContext.get(ContentLanguageListContainer::NAME));
        context.insert(request->operationContext.get(SubscriptionInstanceContainer::NAME));

        Array<CIMObjectPath> classNames;

        for (Uint32 i = 0, n = request->classNames.size(); i < n; i++)
        {
            CIMObjectPath className(
                System::getHostName(),
                request->nameSpace,
                request->classNames[i]);

            classNames.append(className);
        }

        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Calling provider.deleteSubscription: " + _name);

        AutoPThreadSecurity threadLevelSecurity(context);

        pm_service_op_lock op_lock(&status);

        CIMIndicationProvider* provider =
            getProviderInterface<CIMIndicationProvider>(_provider);

        provider->deleteSubscription(
            context,
            request->subscriptionInstance.getPath(),
            classNames);

        //
        //  Decrement count of current subscriptions for this provider
        //
        if (status.decrementSubscriptionsAndTestIfZero())
        {
            //
            //  If there are no current subscriptions after the decrement,
            //  the last subscription has been deleted
            //  Call the provider's disableIndications method
            //
            if (_subscriptionInitComplete)
            {
                _disableIndications();
            }
        }
    }
    HandleCatch(handler);

    PEG_METHOD_EXIT();
    return(response);
}

CIMResponseMessage* ProviderMessageHandler::_handleExportIndicationRequest(
    CIMRequestMessage* message)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "ProviderMessageHandler::_handleExportIndicationRequest");

    CIMExportIndicationRequestMessage* request =
        dynamic_cast<CIMExportIndicationRequestMessage*>(message);
    PEGASUS_ASSERT(request != 0);

    CIMExportIndicationResponseMessage* response =
        dynamic_cast<CIMExportIndicationResponseMessage*>(
            request->buildResponse());
    PEGASUS_ASSERT(response != 0);

    OperationResponseHandler handler(
        request, response, _responseChunkCallback);

    try
    {
        OperationContext context;

        context.insert(request->operationContext.get(IdentityContainer::NAME));

//L10N_TODO
// ATTN-CEC 06/04/03 NOTE: I can't find where the consume msg is sent.  This
// does not appear to be hooked up.  When it is added, need to
// make sure that Content-Language is set in the consume msg.
// NOTE: A-L is not needed to be set in the consume msg.

        // add the langs to the context
        context.insert(request->operationContext.get(ContentLanguageListContainer::NAME));

        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Calling provider.consumeIndication: " + _name);

        StatProviderTimeMeasurement providerTime(response);

        pm_service_op_lock op_lock(&status);

        CIMIndicationConsumerProvider* provider =
            getProviderInterface<CIMIndicationConsumerProvider>(_provider);

        provider->consumeIndication(
            context,
            request->destinationPath,
            request->indicationInstance);
    }
    HandleCatch(handler);

    PEG_METHOD_EXIT();
    return(response);
}

void ProviderMessageHandler::_enableIndications()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "DefaultProviderManager::_enableIndications");

    try
    {
        EnableIndicationsResponseHandler* indicationResponseHandler =
            new EnableIndicationsResponseHandler(
                0,    // request
                0,    // response
                status.getProviderInstance(),
                _indicationCallback,
                _responseChunkCallback);

        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Calling provider.enableIndications: " + _name);

        pm_service_op_lock op_lock(&status);

        status.protect();

        status._indicationsEnabled = true;

        CIMIndicationProvider* provider =
            getProviderInterface<CIMIndicationProvider>(_provider);

        provider->enableIndications(*indicationResponseHandler);

        _indicationResponseHandler = indicationResponseHandler;
    }
    catch (CIMException& e)
    {
        PEG_TRACE_STRING (TRC_PROVIDERMANAGER, Tracer::LEVEL2,
            "CIMException: " + e.getMessage ());

        Logger::put_l(Logger::ERROR_LOG, System::CIMSERVER, Logger::WARNING,
            "ProviderManager.Default.DefaultProviderManager."
                "ENABLE_INDICATIONS_FAILED",
            "Failed to enable indications for provider $0: $1.",
            _name, e.getMessage());
    }
    catch (Exception& e)
    {
        PEG_TRACE_STRING (TRC_PROVIDERMANAGER, Tracer::LEVEL2,
            "Exception: " + e.getMessage ());

        Logger::put_l (Logger::ERROR_LOG, System::CIMSERVER, Logger::WARNING,
            "ProviderManager.Default.DefaultProviderManager."
                "ENABLE_INDICATIONS_FAILED",
            "Failed to enable indications for provider $0: $1.",
            _name, e.getMessage());
    }
    catch(...)
    {
        PEG_TRACE_STRING (TRC_PROVIDERMANAGER, Tracer::LEVEL2,
            "Unexpected error in _enableIndications");

        Logger::put_l (Logger::ERROR_LOG, System::CIMSERVER, Logger::WARNING,
            "ProviderManager.Default.DefaultProviderManager."
                "ENABLE_INDICATIONS_FAILED_UNKNOWN",
            "Failed to enable indications for provider $0.",
            _name);
    }

    PEG_METHOD_EXIT();
}

void ProviderMessageHandler::_disableIndications()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "DefaultProviderManager::_disableIndications");

    try
    {
        if (status._indicationsEnabled)
        {
            pm_service_op_lock op_lock(&status);

            CIMIndicationProvider* provider =
                getProviderInterface<CIMIndicationProvider>(_provider);

            provider->disableIndications();

            status._indicationsEnabled = false;

            status.unprotect();

            status.resetSubscriptions();

            PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                "Destroying indication response handler for " + _name);

            delete _indicationResponseHandler;
            _indicationResponseHandler = 0;
        }
    }
    catch(...)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL3,
            "Error occured disabling indications in provider " + _name);
    }
}

PEGASUS_NAMESPACE_END
