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

#define HANDLE_PROVIDER_EXCEPTION(providerCall, handler)               \
    try                                                                \
    {                                                                  \
        providerCall;                                                  \
    }                                                                  \
    catch (CIMException& e)                                            \
    {                                                                  \
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,          \
            "Provider Exception: " + e.getMessage());                  \
        handler.setCIMException(e);                                    \
    }                                                                  \
    catch (Exception& e)                                               \
    {                                                                  \
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,          \
            "Provider Exception: " + e.getMessage());                  \
        handler.setStatus(                                             \
            CIM_ERR_FAILED, e.getContentLanguages(), e.getMessage());  \
    }                                                                  \
    catch (...)                                                        \
    {                                                                  \
        PEG_TRACE_CSTRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,         \
            "Provider Exception: Unknown");                            \
        handler.setStatus(CIM_ERR_FAILED, "Unknown error.");           \
    }


PEGASUS_NAMESPACE_BEGIN

template<class T>
inline T* getProviderInterface(CIMProvider* provider)
{
    T* p = dynamic_cast<T*>(provider);

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

CIMProvider* ProviderMessageHandler::getProvider()
{
    return _provider;
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

    try
    {
        _provider->terminate();
    }
    catch (...)
    {
        PEG_TRACE_STRING(TRC_DISCARDED_DATA, Tracer::LEVEL2,
            "Caught exception from provider " + _name +
                " terminate() method.");
    }
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

    CIMResponseMessage* response = 0;

    try
    {
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
    }
    catch (CIMException& e)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL2,
            "CIMException: " + e.getMessage());
        response = request->buildResponse();
        response->cimException = PEGASUS_CIM_EXCEPTION_LANG(
            e.getContentLanguages(), e.getCode(), e.getMessage());
    }
    catch (Exception& e)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL2,
            "Exception: " + e.getMessage());
        response = request->buildResponse();
        response->cimException = PEGASUS_CIM_EXCEPTION_LANG(
            e.getContentLanguages(), CIM_ERR_FAILED, e.getMessage());
    }
    catch (...)
    {
        PEG_TRACE_CSTRING(TRC_PROVIDERMANAGER, Tracer::LEVEL2,
            "Exception: Unknown");
        response = request->buildResponse();
        response->cimException = PEGASUS_CIM_EXCEPTION(
            CIM_ERR_FAILED, "Unknown error.");
    }

    PEG_METHOD_EXIT();

    return response;
}

OperationContext ProviderMessageHandler::_createProviderOperationContext(
    const OperationContext& context)
{
    OperationContext providerContext;

    providerContext.insert(context.get(IdentityContainer::NAME));
    providerContext.insert(context.get(AcceptLanguageListContainer::NAME));
    providerContext.insert(context.get(ContentLanguageListContainer::NAME));

    return providerContext;
}

CIMResponseMessage* ProviderMessageHandler::_handleGetInstanceRequest(
    CIMRequestMessage* message)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "ProviderMessageHandler::_handleGetInstanceRequest");

    CIMGetInstanceRequestMessage* request =
        dynamic_cast<CIMGetInstanceRequestMessage*>(message);
    PEGASUS_ASSERT(request != 0);

    AutoPtr<CIMGetInstanceResponseMessage> response(
        dynamic_cast<CIMGetInstanceResponseMessage*>(
            request->buildResponse()));
    PEGASUS_ASSERT(response.get() != 0);

    // create a handler for this request
    GetInstanceResponseHandler handler(
        request, response.get(), _responseChunkCallback);

    // make target object path
    CIMObjectPath objectPath(
        System::getHostName(),
        request->nameSpace,
        request->instanceName.getClassName(),
        request->instanceName.getKeyBindings());

    PEG_LOGGER_TRACE((Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
        "ProviderMessageHandler::_handleGetInstanceRequest - "
            "Object path: $0",
        objectPath.toString()));

    OperationContext providerContext(
        _createProviderOperationContext(request->operationContext));

    AutoPThreadSecurity threadLevelSecurity(request->operationContext);

    CIMInstanceProvider* provider =
        getProviderInterface<CIMInstanceProvider>(_provider);

    PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
        "Calling provider.getInstance: " + _name);

    StatProviderTimeMeasurement providerTime(response.get());

    HANDLE_PROVIDER_EXCEPTION(
        provider->getInstance(
            providerContext,
            objectPath,
            request->includeQualifiers,
            request->includeClassOrigin,
            request->propertyList,
            handler),
        handler)

    PEG_METHOD_EXIT();
    return response.release();
}

CIMResponseMessage* ProviderMessageHandler::_handleEnumerateInstancesRequest(
    CIMRequestMessage* message)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "ProviderMessageHandler::_handleEnumerateInstanceRequest");

    CIMEnumerateInstancesRequestMessage* request =
        dynamic_cast<CIMEnumerateInstancesRequestMessage*>(message);
    PEGASUS_ASSERT(request != 0);

    AutoPtr<CIMEnumerateInstancesResponseMessage> response(
        dynamic_cast<CIMEnumerateInstancesResponseMessage*>(
            request->buildResponse()));
    PEGASUS_ASSERT(response.get() != 0);

    // create a handler for this request
    EnumerateInstancesResponseHandler handler(
        request, response.get(), _responseChunkCallback);

    // make target object path
    CIMObjectPath objectPath(
        System::getHostName(),
        request->nameSpace,
        request->className);

    PEG_LOGGER_TRACE((Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
        "ProviderMessageHandler::_handleEnumerateInstancesRequest - "
            "Object path: $0",
        objectPath.toString()));

    OperationContext providerContext(
        _createProviderOperationContext(request->operationContext));

    AutoPThreadSecurity threadLevelSecurity(request->operationContext);

    CIMInstanceProvider* provider =
        getProviderInterface<CIMInstanceProvider>(_provider);

    PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
        "Calling provider.enumerateInstances: " + _name);

    StatProviderTimeMeasurement providerTime(response.get());

    HANDLE_PROVIDER_EXCEPTION(
        provider->enumerateInstances(
            providerContext,
            objectPath,
            request->includeQualifiers,
            request->includeClassOrigin,
            request->propertyList,
            handler),
        handler)

    PEG_METHOD_EXIT();
    return response.release();
}

CIMResponseMessage*
    ProviderMessageHandler::_handleEnumerateInstanceNamesRequest(
        CIMRequestMessage* message)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "ProviderMessageHandler::_handleEnumerateInstanceNamesRequest");

    CIMEnumerateInstanceNamesRequestMessage* request =
        dynamic_cast<CIMEnumerateInstanceNamesRequestMessage*>(message);
    PEGASUS_ASSERT(request != 0);

    AutoPtr<CIMEnumerateInstanceNamesResponseMessage> response(
        dynamic_cast<CIMEnumerateInstanceNamesResponseMessage*>(
            request->buildResponse()));
    PEGASUS_ASSERT(response.get() != 0);

    // create a handler for this request
    EnumerateInstanceNamesResponseHandler handler(
        request, response.get(), _responseChunkCallback);

    // make target object path
    CIMObjectPath objectPath(
        System::getHostName(),
        request->nameSpace,
        request->className);

    PEG_LOGGER_TRACE((Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
        "ProviderMessageHandler::_handleEnumerateInstanceNamesRequest - "
            "Object path: $0",
        objectPath.toString()));

    OperationContext providerContext(
        _createProviderOperationContext(request->operationContext));

    AutoPThreadSecurity threadLevelSecurity(request->operationContext);

    CIMInstanceProvider* provider =
        getProviderInterface<CIMInstanceProvider>(_provider);

    PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
        "Calling provider.enumerateInstanceNames: " + _name);

    StatProviderTimeMeasurement providerTime(response.get());

    HANDLE_PROVIDER_EXCEPTION(
        provider->enumerateInstanceNames(
            providerContext,
            objectPath,
            handler),
        handler)

    PEG_METHOD_EXIT();
    return response.release();
}

CIMResponseMessage* ProviderMessageHandler::_handleCreateInstanceRequest(
    CIMRequestMessage* message)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "ProviderMessageHandler::_handleCreateInstanceRequest");

    CIMCreateInstanceRequestMessage* request =
        dynamic_cast<CIMCreateInstanceRequestMessage*>(message);
    PEGASUS_ASSERT(request != 0);

    AutoPtr<CIMCreateInstanceResponseMessage> response(
        dynamic_cast<CIMCreateInstanceResponseMessage*>(
            request->buildResponse()));
    PEGASUS_ASSERT(response.get() != 0);

    // create a handler for this request
    CreateInstanceResponseHandler handler(
        request, response.get(), _responseChunkCallback);

    // make target object path
    CIMObjectPath objectPath(
        System::getHostName(),
        request->nameSpace,
        request->newInstance.getPath().getClassName(),
        request->newInstance.getPath().getKeyBindings());

    PEG_LOGGER_TRACE((Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
        "ProviderMessageHandler::_handleCreateInstanceRequest - "
            "Object path: $0",
        objectPath.toString()));

    OperationContext providerContext(
        _createProviderOperationContext(request->operationContext));

    AutoPThreadSecurity threadLevelSecurity(request->operationContext);

    CIMInstanceProvider* provider =
        getProviderInterface<CIMInstanceProvider>(_provider);

    PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
        "Calling provider.createInstance: " + _name);

    StatProviderTimeMeasurement providerTime(response.get());

    HANDLE_PROVIDER_EXCEPTION(
        provider->createInstance(
            providerContext,
            objectPath,
            request->newInstance,
            handler),
        handler)

    PEG_METHOD_EXIT();
    return response.release();
}

CIMResponseMessage* ProviderMessageHandler::_handleModifyInstanceRequest(
    CIMRequestMessage* message)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "ProviderMessageHandler::_handleModifyInstanceRequest");

    CIMModifyInstanceRequestMessage* request =
        dynamic_cast<CIMModifyInstanceRequestMessage*>(message);
    PEGASUS_ASSERT(request != 0);

    AutoPtr<CIMModifyInstanceResponseMessage> response(
        dynamic_cast<CIMModifyInstanceResponseMessage*>(
            request->buildResponse()));
    PEGASUS_ASSERT(response.get() != 0);

    // create a handler for this request
    ModifyInstanceResponseHandler handler(
        request, response.get(), _responseChunkCallback);

    // make target object path
    CIMObjectPath objectPath(
        System::getHostName(),
        request->nameSpace,
        request->modifiedInstance.getPath().getClassName(),
        request->modifiedInstance.getPath().getKeyBindings());

    PEG_LOGGER_TRACE((Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
        "ProviderMessageHandler::_handleModifyInstanceRequest - "
            "Object path: $0",
        objectPath.toString()));

    OperationContext providerContext(
        _createProviderOperationContext(request->operationContext));

    AutoPThreadSecurity threadLevelSecurity(request->operationContext);

    CIMInstanceProvider* provider =
        getProviderInterface<CIMInstanceProvider>(_provider);

    PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
        "Calling provider.modifyInstance: " + _name);

    StatProviderTimeMeasurement providerTime(response.get());

    HANDLE_PROVIDER_EXCEPTION(
        provider->modifyInstance(
            providerContext,
            objectPath,
            request->modifiedInstance,
            request->includeQualifiers,
            request->propertyList,
            handler),
        handler)

    PEG_METHOD_EXIT();
    return response.release();
}

CIMResponseMessage* ProviderMessageHandler::_handleDeleteInstanceRequest(
    CIMRequestMessage* message)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "ProviderMessageHandler::_handleDeleteInstanceRequest");

    CIMDeleteInstanceRequestMessage* request =
        dynamic_cast<CIMDeleteInstanceRequestMessage*>(message);
    PEGASUS_ASSERT(request != 0);

    AutoPtr<CIMDeleteInstanceResponseMessage> response(
        dynamic_cast<CIMDeleteInstanceResponseMessage*>(
            request->buildResponse()));
    PEGASUS_ASSERT(response.get() != 0);

    // create a handler for this request
    DeleteInstanceResponseHandler handler(
        request, response.get(), _responseChunkCallback);

    // make target object path
    CIMObjectPath objectPath(
        System::getHostName(),
        request->nameSpace,
        request->instanceName.getClassName(),
        request->instanceName.getKeyBindings());

    PEG_LOGGER_TRACE((Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
        "ProviderMessageHandler::_handleDeleteInstanceRequest - "
            "Object path: $0",
        objectPath.toString()));

    OperationContext providerContext(
        _createProviderOperationContext(request->operationContext));

    AutoPThreadSecurity threadLevelSecurity(request->operationContext);

    CIMInstanceProvider* provider =
        getProviderInterface<CIMInstanceProvider>(_provider);

    PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
        "Calling provider.deleteInstance: " + _name);

    StatProviderTimeMeasurement providerTime(response.get());

    HANDLE_PROVIDER_EXCEPTION(
        provider->deleteInstance(
            providerContext,
            objectPath,
            handler),
        handler)

    PEG_METHOD_EXIT();
    return response.release();
}

CIMResponseMessage* ProviderMessageHandler::_handleExecQueryRequest(
    CIMRequestMessage* message)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "ProviderMessageHandler::_handleExecQueryRequest");

    CIMExecQueryRequestMessage* request =
        dynamic_cast<CIMExecQueryRequestMessage*>(message);
    PEGASUS_ASSERT(request != 0);

    AutoPtr<CIMExecQueryResponseMessage> response(
        dynamic_cast<CIMExecQueryResponseMessage*>(
            request->buildResponse()));
    PEGASUS_ASSERT(response.get() != 0);

    // create a handler for this request
    ExecQueryResponseHandler handler(
        request, response.get(), _responseChunkCallback);

    // make target object path
    CIMObjectPath objectPath(
        System::getHostName(),
        request->nameSpace,
        request->className);

    PEG_LOGGER_TRACE((Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
        "ProviderMessageHandler::_handleExecQueryRequest - "
            "Object path: $0",
        objectPath.toString()));

    QueryExpression qx(request->queryLanguage,request->query);

    OperationContext providerContext(
        _createProviderOperationContext(request->operationContext));

    AutoPThreadSecurity threadLevelSecurity(request->operationContext);

    CIMInstanceQueryProvider* provider =
        getProviderInterface<CIMInstanceQueryProvider>(_provider);

    PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
        "Calling provider.execQuery: " + _name);

    StatProviderTimeMeasurement providerTime(response.get());

    HANDLE_PROVIDER_EXCEPTION(
        provider->execQuery(
            providerContext,
            objectPath,
            qx,
            handler),
        handler)

    PEG_METHOD_EXIT();
    return response.release();
}

CIMResponseMessage* ProviderMessageHandler::_handleAssociatorsRequest(
    CIMRequestMessage* message)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "ProviderMessageHandler::_handleAssociatorsRequest");

    CIMAssociatorsRequestMessage* request =
        dynamic_cast<CIMAssociatorsRequestMessage*>(message);
    PEGASUS_ASSERT(request != 0);

    AutoPtr<CIMAssociatorsResponseMessage> response(
        dynamic_cast<CIMAssociatorsResponseMessage*>(
            request->buildResponse()));
    PEGASUS_ASSERT(response.get() != 0);

    // create a handler for this request
    AssociatorsResponseHandler handler(
        request, response.get(), _responseChunkCallback);

    // make target object path
    CIMObjectPath objectPath(
        System::getHostName(),
        request->nameSpace,
        request->objectName.getClassName());

    objectPath.setKeyBindings(request->objectName.getKeyBindings());

    PEG_LOGGER_TRACE((Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
        "ProviderMessageHandler::_handleAssociatorsRequest - "
            "Object path: $0",
        objectPath.toString()));

    CIMObjectPath assocPath(
        System::getHostName(),
        request->nameSpace,
        request->assocClass.getString());

    OperationContext providerContext(
        _createProviderOperationContext(request->operationContext));

    AutoPThreadSecurity threadLevelSecurity(request->operationContext);

    CIMAssociationProvider* provider =
        getProviderInterface<CIMAssociationProvider>(_provider);

    PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
        "Calling provider.associators: " + _name);

    StatProviderTimeMeasurement providerTime(response.get());

    HANDLE_PROVIDER_EXCEPTION(
        provider->associators(
            providerContext,
            objectPath,
            request->assocClass,
            request->resultClass,
            request->role,
            request->resultRole,
            request->includeQualifiers,
            request->includeClassOrigin,
            request->propertyList,
            handler),
        handler)

    PEG_METHOD_EXIT();
    return response.release();
}

CIMResponseMessage* ProviderMessageHandler::_handleAssociatorNamesRequest(
    CIMRequestMessage* message)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "ProviderMessageHandler::_handleAssociatorNamesRequest");

    CIMAssociatorNamesRequestMessage* request =
        dynamic_cast<CIMAssociatorNamesRequestMessage*>(message);
    PEGASUS_ASSERT(request != 0);

    AutoPtr<CIMAssociatorNamesResponseMessage> response(
        dynamic_cast<CIMAssociatorNamesResponseMessage*>(
            request->buildResponse()));
    PEGASUS_ASSERT(response.get() != 0);

    // create a handler for this request
    AssociatorNamesResponseHandler handler(
        request, response.get(), _responseChunkCallback);

    // make target object path
    CIMObjectPath objectPath(
        System::getHostName(),
        request->nameSpace,
        request->objectName.getClassName());

    objectPath.setKeyBindings(request->objectName.getKeyBindings());

    PEG_LOGGER_TRACE((Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
        "ProviderMessageHandler::_handleAssociationNamesRequest - "
            "Object path: $0",
        objectPath.toString()));

    CIMObjectPath assocPath(
        System::getHostName(),
        request->nameSpace,
        request->assocClass.getString());

    OperationContext providerContext(
        _createProviderOperationContext(request->operationContext));

    AutoPThreadSecurity threadLevelSecurity(request->operationContext);

    CIMAssociationProvider* provider =
        getProviderInterface<CIMAssociationProvider>(_provider);

    PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
        "Calling provider.associatorNames: " + _name);

    StatProviderTimeMeasurement providerTime(response.get());

    HANDLE_PROVIDER_EXCEPTION(
        provider->associatorNames(
            providerContext,
            objectPath,
            request->assocClass,
            request->resultClass,
            request->role,
            request->resultRole,
            handler),
        handler)

    PEG_METHOD_EXIT();
    return response.release();
}

CIMResponseMessage* ProviderMessageHandler::_handleReferencesRequest(
    CIMRequestMessage* message)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "ProviderMessageHandler::_handleReferencesRequest");

    CIMReferencesRequestMessage* request =
        dynamic_cast<CIMReferencesRequestMessage*>(message);
    PEGASUS_ASSERT(request != 0);

    AutoPtr<CIMReferencesResponseMessage> response(
        dynamic_cast<CIMReferencesResponseMessage*>(
            request->buildResponse()));
    PEGASUS_ASSERT(response.get() != 0);

    // create a handler for this request
    ReferencesResponseHandler handler(
        request, response.get(), _responseChunkCallback);

    // make target object path
    CIMObjectPath objectPath(
        System::getHostName(),
        request->nameSpace,
        request->objectName.getClassName());

    objectPath.setKeyBindings(request->objectName.getKeyBindings());

    PEG_LOGGER_TRACE((Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
        "ProviderMessageHandler::_handleReferencesRequest - "
            "Object path: $0",
        objectPath.toString()));

    CIMObjectPath resultPath(
        System::getHostName(),
        request->nameSpace,
        request->resultClass.getString());

    OperationContext providerContext(
        _createProviderOperationContext(request->operationContext));

    AutoPThreadSecurity threadLevelSecurity(request->operationContext);

    CIMAssociationProvider* provider =
        getProviderInterface<CIMAssociationProvider>(_provider);

    PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
        "Calling provider.references: " + _name);

    StatProviderTimeMeasurement providerTime(response.get());

    HANDLE_PROVIDER_EXCEPTION(
        provider->references(
            providerContext,
            objectPath,
            request->resultClass,
            request->role,
            request->includeQualifiers,
            request->includeClassOrigin,
            request->propertyList,
            handler),
        handler)

    PEG_METHOD_EXIT();
    return response.release();
}

CIMResponseMessage* ProviderMessageHandler::_handleReferenceNamesRequest(
    CIMRequestMessage* message)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "ProviderMessageHandler::_handleReferenceNamesRequest");

    CIMReferenceNamesRequestMessage* request =
        dynamic_cast<CIMReferenceNamesRequestMessage*>(message);
    PEGASUS_ASSERT(request != 0);

    AutoPtr<CIMReferenceNamesResponseMessage> response(
        dynamic_cast<CIMReferenceNamesResponseMessage*>(
            request->buildResponse()));
    PEGASUS_ASSERT(response.get() != 0);

    // create a handler for this request
    ReferenceNamesResponseHandler handler(
        request, response.get(), _responseChunkCallback);

    // make target object path
    CIMObjectPath objectPath(
        System::getHostName(),
        request->nameSpace,
        request->objectName.getClassName());

    objectPath.setKeyBindings(request->objectName.getKeyBindings());

    PEG_LOGGER_TRACE((Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
        "ProviderMessageHandler::_handleReferenceNamesRequest - "
            "Object path: $0",
        objectPath.toString()));

    CIMObjectPath resultPath(
        System::getHostName(),
        request->nameSpace,
        request->resultClass.getString());

    OperationContext providerContext(
        _createProviderOperationContext(request->operationContext));

    AutoPThreadSecurity threadLevelSecurity(request->operationContext);

    CIMAssociationProvider* provider =
        getProviderInterface<CIMAssociationProvider>(_provider);

    PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
        "Calling provider.referenceNames: " + _name);

    StatProviderTimeMeasurement providerTime(response.get());

    HANDLE_PROVIDER_EXCEPTION(
        provider->referenceNames(
            providerContext,
            objectPath,
            request->resultClass,
            request->role,
            handler),
        handler)

    PEG_METHOD_EXIT();
    return response.release();
}

CIMResponseMessage* ProviderMessageHandler::_handleGetPropertyRequest(
    CIMRequestMessage* message)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "ProviderMessageHandler::_handleGetPropertyRequest");

    CIMGetPropertyRequestMessage* request =
        dynamic_cast<CIMGetPropertyRequestMessage*>(message);
    PEGASUS_ASSERT(request != 0);

    AutoPtr<CIMGetPropertyResponseMessage> response(
        dynamic_cast<CIMGetPropertyResponseMessage*>(
            request->buildResponse()));
    PEGASUS_ASSERT(response.get() != 0);

    //
    // Translate the GetProperty request to a GetInstance request message
    //

    Array<CIMName> propertyList;
    propertyList.append(request->propertyName);

    CIMGetInstanceRequestMessage getInstanceRequest(
        request->messageId,
        request->nameSpace,
        request->instanceName,
        false,  // localOnly
        false,  // includeQualifiers
        false,  // includeClassOrigin
        propertyList,
        request->queueIds);

    getInstanceRequest.operationContext = request->operationContext;

    AutoPtr<CIMGetInstanceResponseMessage> getInstanceResponse(
        dynamic_cast<CIMGetInstanceResponseMessage*>(
            getInstanceRequest.buildResponse()));
    PEGASUS_ASSERT(getInstanceResponse.get() != 0);

    //
    // Process the GetInstance operation
    //

    // create a handler for this request (with chunking disabled)
    GetInstanceResponseHandler handler(
        &getInstanceRequest, getInstanceResponse.get(), 0);

    // make target object path
    CIMObjectPath objectPath(
        System::getHostName(),
        getInstanceRequest.nameSpace,
        getInstanceRequest.instanceName.getClassName(),
        getInstanceRequest.instanceName.getKeyBindings());

    PEG_LOGGER_TRACE((Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
        "ProviderMessageHandler::_handleGetPropertyRequest - "
            "Object path: $0, Property: $1",
        objectPath.toString(), request->propertyName.getString()));

    OperationContext providerContext(
        _createProviderOperationContext(getInstanceRequest.operationContext));

    AutoPThreadSecurity threadLevelSecurity(request->operationContext);

    CIMInstanceProvider* provider =
        getProviderInterface<CIMInstanceProvider>(_provider);

    PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
        "Calling provider.getInstance: " + _name);

    StatProviderTimeMeasurement providerTime(response.get());

    HANDLE_PROVIDER_EXCEPTION(
        provider->getInstance(
            providerContext,
            objectPath,
            getInstanceRequest.includeQualifiers,
            getInstanceRequest.includeClassOrigin,
            getInstanceRequest.propertyList,
            handler),
        handler)

    //
    // Copy the GetInstance response into the GetProperty response message
    //

    response->cimException = getInstanceResponse->cimException;

    if (response->cimException.getCode() == CIM_ERR_SUCCESS)
    {
        CIMInstance instance = getInstanceResponse->cimInstance;

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

    response->operationContext = getInstanceResponse->operationContext;

    PEG_METHOD_EXIT();
    return response.release();
}

CIMResponseMessage* ProviderMessageHandler::_handleSetPropertyRequest(
    CIMRequestMessage* message)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "ProviderMessageHandler::_handleSetPropertyRequest");

    CIMSetPropertyRequestMessage* request =
        dynamic_cast<CIMSetPropertyRequestMessage*>(message);
    PEGASUS_ASSERT(request != 0);

    AutoPtr<CIMSetPropertyResponseMessage> response(
        dynamic_cast<CIMSetPropertyResponseMessage*>(
            request->buildResponse()));
    PEGASUS_ASSERT(response.get() != 0);

    //
    // Translate the SetProperty request to a ModifyInstance request message
    //

    // make target object path
    CIMObjectPath objectPath(
        System::getHostName(),
        request->nameSpace,
        request->instanceName.getClassName(),
        request->instanceName.getKeyBindings());

    CIMInstance instance(request->instanceName.getClassName());
    instance.addProperty(CIMProperty(
        request->propertyName, request->newValue));
    instance.setPath(objectPath);

    Array<CIMName> propertyList;
    propertyList.append(request->propertyName);

    CIMModifyInstanceRequestMessage modifyInstanceRequest(
        request->messageId,
        request->nameSpace,
        instance,
        false,  // includeQualifiers
        propertyList,
        request->queueIds);

    modifyInstanceRequest.operationContext = request->operationContext;

    AutoPtr<CIMModifyInstanceResponseMessage> modifyInstanceResponse(
        dynamic_cast<CIMModifyInstanceResponseMessage*>(
            modifyInstanceRequest.buildResponse()));
    PEGASUS_ASSERT(modifyInstanceResponse.get() != 0);

    //
    // Process the ModifyInstance operation
    //

    // create a handler for this request (with chunking disabled)
    ModifyInstanceResponseHandler handler(
        &modifyInstanceRequest, modifyInstanceResponse.get(), 0);

    PEG_LOGGER_TRACE((Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
        "ProviderMessageHandler::_handleSetPropertyRequest - "
            "Object path: $0, Property: $1",
        objectPath.toString(), request->propertyName.getString()));

    OperationContext providerContext(
        _createProviderOperationContext(request->operationContext));

    AutoPThreadSecurity threadLevelSecurity(request->operationContext);

    CIMInstanceProvider* provider =
        getProviderInterface<CIMInstanceProvider>(_provider);

    PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
        "Calling provider.modifyInstance: " + _name);

    StatProviderTimeMeasurement providerTime(response.get());

    HANDLE_PROVIDER_EXCEPTION(
        provider->modifyInstance(
            providerContext,
            objectPath,
            modifyInstanceRequest.modifiedInstance,
            modifyInstanceRequest.includeQualifiers,
            modifyInstanceRequest.propertyList,
            handler),
        handler)

    //
    // Copy the ModifyInstance response into the GetProperty response message
    //

    response->cimException = modifyInstanceResponse->cimException;
    response->operationContext = modifyInstanceResponse->operationContext;

    PEG_METHOD_EXIT();
    return response.release();
}

CIMResponseMessage* ProviderMessageHandler::_handleInvokeMethodRequest(
    CIMRequestMessage* message)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "ProviderMessageHandler::_handleInvokeMethodRequest");

    CIMInvokeMethodRequestMessage* request =
        dynamic_cast<CIMInvokeMethodRequestMessage*>(message);
    PEGASUS_ASSERT(request != 0);

    AutoPtr<CIMInvokeMethodResponseMessage> response(
        dynamic_cast<CIMInvokeMethodResponseMessage*>(
            request->buildResponse()));
    PEGASUS_ASSERT(response.get() != 0);

    // create a handler for this request
    InvokeMethodResponseHandler handler(
        request, response.get(), _responseChunkCallback);

    // make target object path
    CIMObjectPath objectPath(
        System::getHostName(),
        request->nameSpace,
        request->instanceName.getClassName(),
        request->instanceName.getKeyBindings());

    PEG_LOGGER_TRACE((Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
        "ProviderMessageHandler::_handleInvokeMethodRequest - "
            "Object path: $0, Method: $1",
        objectPath.toString(), request->methodName.getString()));

    OperationContext providerContext(
        _createProviderOperationContext(request->operationContext));

    AutoPThreadSecurity threadLevelSecurity(request->operationContext);

    CIMMethodProvider* provider =
        getProviderInterface<CIMMethodProvider>(_provider);

    PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
        "Calling provider.invokeMethod: " + _name);

    StatProviderTimeMeasurement providerTime(response.get());

    HANDLE_PROVIDER_EXCEPTION(
        provider->invokeMethod(
            providerContext,
            objectPath,
            request->methodName,
            request->inParameters,
            handler),
        handler)

    PEG_METHOD_EXIT();
    return response.release();
}

CIMResponseMessage* ProviderMessageHandler::_handleCreateSubscriptionRequest(
    CIMRequestMessage* message)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "ProviderMessageHandler::_handleCreateSubscriptionRequest");

    CIMCreateSubscriptionRequestMessage* request =
        dynamic_cast<CIMCreateSubscriptionRequestMessage*>(message);
    PEGASUS_ASSERT(request != 0);

    AutoPtr<CIMCreateSubscriptionResponseMessage> response(
        dynamic_cast<CIMCreateSubscriptionResponseMessage*>(
            request->buildResponse()));
    PEGASUS_ASSERT(response.get() != 0);

    OperationResponseHandler handler(
        request, response.get(), _responseChunkCallback);

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

    Array<CIMObjectPath> classNames;

    for (Uint32 i = 0, n = request->classNames.size(); i < n; i++)
    {
        CIMObjectPath className(
            System::getHostName(),
            request->nameSpace,
            request->classNames[i]);

        classNames.append(className);
    }

    OperationContext providerContext(
        _createProviderOperationContext(request->operationContext));
    providerContext.insert(request->operationContext.get(
        SubscriptionInstanceContainer::NAME));
    providerContext.insert(request->operationContext.get(
        SubscriptionFilterConditionContainer::NAME));
    providerContext.insert(request->operationContext.get(
        SubscriptionFilterQueryContainer::NAME));

    AutoPThreadSecurity threadLevelSecurity(request->operationContext);

    CIMIndicationProvider* provider =
        getProviderInterface<CIMIndicationProvider>(_provider);

    PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
        "Calling provider.createSubscription: " + _name);

    StatProviderTimeMeasurement providerTime(response.get());

    HANDLE_PROVIDER_EXCEPTION(
        provider->createSubscription(
            providerContext,
            request->subscriptionInstance.getPath(),
            classNames,
            request->propertyList,
            request->repeatNotificationPolicy),
        handler)

    //
    //  Increment count of current subscriptions for this provider
    //
    if (status.testIfZeroAndIncrementSubscriptions())
    {
        PEG_TRACE_CSTRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
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

    PEG_METHOD_EXIT();
    return response.release();
}

CIMResponseMessage* ProviderMessageHandler::_handleModifySubscriptionRequest(
    CIMRequestMessage* message)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "ProviderMessageHandler::_handleModifySubscriptionRequest");

    CIMModifySubscriptionRequestMessage* request =
        dynamic_cast<CIMModifySubscriptionRequestMessage*>(message);
    PEGASUS_ASSERT(request != 0);

    AutoPtr<CIMModifySubscriptionResponseMessage> response(
        dynamic_cast<CIMModifySubscriptionResponseMessage*>(
            request->buildResponse()));
    PEGASUS_ASSERT(response.get() != 0);

    OperationResponseHandler handler(
        request, response.get(), _responseChunkCallback);

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

    Array<CIMObjectPath> classNames;

    for (Uint32 i = 0, n = request->classNames.size(); i < n; i++)
    {
        CIMObjectPath className(
            System::getHostName(),
            request->nameSpace,
            request->classNames[i]);

        classNames.append(className);
    }

    OperationContext providerContext(
        _createProviderOperationContext(request->operationContext));
    providerContext.insert(request->operationContext.get(
        SubscriptionInstanceContainer::NAME));
    providerContext.insert(request->operationContext.get(
        SubscriptionFilterConditionContainer::NAME));
    providerContext.insert(request->operationContext.get(
        SubscriptionFilterQueryContainer::NAME));

    AutoPThreadSecurity threadLevelSecurity(request->operationContext);

    CIMIndicationProvider* provider =
        getProviderInterface<CIMIndicationProvider>(_provider);

    PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
        "Calling provider.modifySubscription: " + _name);

    StatProviderTimeMeasurement providerTime(response.get());

    HANDLE_PROVIDER_EXCEPTION(
        provider->modifySubscription(
            providerContext,
            request->subscriptionInstance.getPath(),
            classNames,
            request->propertyList,
            request->repeatNotificationPolicy),
        handler)

    PEG_METHOD_EXIT();
    return response.release();
}

CIMResponseMessage* ProviderMessageHandler::_handleDeleteSubscriptionRequest(
    CIMRequestMessage* message)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "ProviderMessageHandler::_handleDeleteSubscriptionRequest");

    CIMDeleteSubscriptionRequestMessage* request =
        dynamic_cast<CIMDeleteSubscriptionRequestMessage*>(message);
    PEGASUS_ASSERT(request != 0);

    AutoPtr<CIMDeleteSubscriptionResponseMessage> response(
        dynamic_cast<CIMDeleteSubscriptionResponseMessage*>(
            request->buildResponse()));
    PEGASUS_ASSERT(response.get() != 0);

    OperationResponseHandler handler(
        request, response.get(), _responseChunkCallback);

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

    Array<CIMObjectPath> classNames;

    for (Uint32 i = 0, n = request->classNames.size(); i < n; i++)
    {
        CIMObjectPath className(
            System::getHostName(),
            request->nameSpace,
            request->classNames[i]);

        classNames.append(className);
    }

    OperationContext providerContext(
        _createProviderOperationContext(request->operationContext));
    providerContext.insert(request->operationContext.get(
        SubscriptionInstanceContainer::NAME));

    AutoPThreadSecurity threadLevelSecurity(request->operationContext);

    CIMIndicationProvider* provider =
        getProviderInterface<CIMIndicationProvider>(_provider);

    PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
        "Calling provider.deleteSubscription: " + _name);

    StatProviderTimeMeasurement providerTime(response.get());

    HANDLE_PROVIDER_EXCEPTION(
        provider->deleteSubscription(
            providerContext,
            request->subscriptionInstance.getPath(),
            classNames),
        handler)

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

    PEG_METHOD_EXIT();
    return response.release();
}

CIMResponseMessage* ProviderMessageHandler::_handleExportIndicationRequest(
    CIMRequestMessage* message)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "ProviderMessageHandler::_handleExportIndicationRequest");

    CIMExportIndicationRequestMessage* request =
        dynamic_cast<CIMExportIndicationRequestMessage*>(message);
    PEGASUS_ASSERT(request != 0);

    AutoPtr<CIMExportIndicationResponseMessage> response(
        dynamic_cast<CIMExportIndicationResponseMessage*>(
            request->buildResponse()));
    PEGASUS_ASSERT(response.get() != 0);

    OperationResponseHandler handler(
        request, response.get(), _responseChunkCallback);

    // NOTE: Accept-Languages do not need to be set in the consume msg.
    OperationContext providerContext;
    providerContext.insert(request->operationContext.get(
        IdentityContainer::NAME));
//L10N_TODO
// ATTN-CEC 06/04/03 NOTE: I can't find where the consume msg is sent.  This
// does not appear to be hooked up.  When it is added, need to
// make sure that Content-Language is set in the consume msg.
    providerContext.insert(request->operationContext.get(
        ContentLanguageListContainer::NAME));

    AutoPThreadSecurity threadLevelSecurity(providerContext);

    CIMIndicationConsumerProvider* provider =
        getProviderInterface<CIMIndicationConsumerProvider>(_provider);

    PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
        "Calling provider.consumeIndication: " + _name);

    StatProviderTimeMeasurement providerTime(response.get());

    HANDLE_PROVIDER_EXCEPTION(
        provider->consumeIndication(
            providerContext,
            request->destinationPath,
            request->indicationInstance),
        handler)

    PEG_METHOD_EXIT();
    return response.release();
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

        _indicationResponseHandler = indicationResponseHandler;

        status.setIndicationsEnabled(true);

        CIMIndicationProvider* provider =
            getProviderInterface<CIMIndicationProvider>(_provider);

        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Calling provider.enableIndications: " + _name);

        provider->enableIndications(*indicationResponseHandler);
    }
    catch (Exception& e)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL2,
            "Exception: " + e.getMessage());

        Logger::put_l(Logger::ERROR_LOG, System::CIMSERVER, Logger::WARNING,
            "ProviderManager.Default.DefaultProviderManager."
                "ENABLE_INDICATIONS_FAILED",
            "Failed to enable indications for provider $0: $1.",
            _name, e.getMessage());
    }
    catch(...)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL2,
            "Unexpected error in _enableIndications");

        Logger::put_l(Logger::ERROR_LOG, System::CIMSERVER, Logger::WARNING,
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
        if (status.getIndicationsEnabled())
        {
            CIMIndicationProvider* provider =
                getProviderInterface<CIMIndicationProvider>(_provider);

            PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                "Calling provider.disableIndications: " + _name);

            try
            {
                provider->disableIndications();
            }
            catch (...)
            {
                PEG_TRACE_STRING(TRC_DISCARDED_DATA, Tracer::LEVEL2,
                    "Caught exception from provider " + _name +
                        " disableIndications() method.");
            }

            status.setIndicationsEnabled(false);

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
