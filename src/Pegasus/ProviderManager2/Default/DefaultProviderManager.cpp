//%2003////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002  BMC Software, Hewlett-Packard Development
// Company, L. P., IBM Corp., The Open Group, Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L. P.;
// IBM Corp.; EMC Corporation, The Open Group.
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
//              Mike Day, IBM (mdday@us.ibm.com)
//              Karl Schopmeyer(k.schopmeyer@opengroup.org) - Fix associators.
//		        Yi Zhou, Hewlett-Packard Company (yi_zhou@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include "DefaultProviderManager.h"

#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Common/OperationContext.h>
#include <Pegasus/Common/Destroyer.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/StatisticalData.h>
#include <Pegasus/Common/Logger.h>
#include <Pegasus/Common/MessageLoader.h> //l10n

#include <Pegasus/Config/ConfigManager.h>

#include <Pegasus/ProviderManager2/Default/Provider.h>
#include <Pegasus/ProviderManager2/Default/OperationResponseHandler.h>

#include <Pegasus/Server/ProviderRegistrationManager/ProviderRegistrationManager.h>

PEGASUS_NAMESPACE_BEGIN

// auto variable to protect provider during operations
class pm_service_op_lock
{
private:
    pm_service_op_lock(void);

public:
    pm_service_op_lock(Provider *provider) : _provider(provider)
    {
        _provider->protect();
    }

    ~pm_service_op_lock(void)
    {
        _provider->unprotect();
    }

    Provider * _provider;
};

//
// Provider module status
//
static const Uint16 _MODULE_OK       = 2;
static const Uint16 _MODULE_STOPPING = 9;
static const Uint16 _MODULE_STOPPED  = 10;

// provider manager
LocalProviderManager providerManager;

DefaultProviderManager::DefaultProviderManager(void)
{
}

DefaultProviderManager::~DefaultProviderManager(void)
{
}

Message * DefaultProviderManager::processMessage(Message * request) throw()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "DefaultProviderManager::processMessage()");

    Message * response = 0;

    // pass the request message to a handler method based on message type
    switch(request->getType())
    {
    case CIM_GET_INSTANCE_REQUEST_MESSAGE:
        response = handleGetInstanceRequest(request);

        break;
    case CIM_ENUMERATE_INSTANCES_REQUEST_MESSAGE:
        response = handleEnumerateInstancesRequest(request);

        break;
    case CIM_ENUMERATE_INSTANCE_NAMES_REQUEST_MESSAGE:
        response = handleEnumerateInstanceNamesRequest(request);

        break;
    case CIM_CREATE_INSTANCE_REQUEST_MESSAGE:
        response = handleCreateInstanceRequest(request);

        break;
    case CIM_MODIFY_INSTANCE_REQUEST_MESSAGE:
        response = handleModifyInstanceRequest(request);

        break;
    case CIM_DELETE_INSTANCE_REQUEST_MESSAGE:
        response = handleDeleteInstanceRequest(request);

        break;
    case CIM_EXEC_QUERY_REQUEST_MESSAGE:
        response = handleExecuteQueryRequest(request);

        break;
    case CIM_ASSOCIATORS_REQUEST_MESSAGE:
        response = handleAssociatorsRequest(request);

        break;
    case CIM_ASSOCIATOR_NAMES_REQUEST_MESSAGE:
        response = handleAssociatorNamesRequest(request);

        break;
    case CIM_REFERENCES_REQUEST_MESSAGE:
        response = handleReferencesRequest(request);

        break;
    case CIM_REFERENCE_NAMES_REQUEST_MESSAGE:
        response = handleReferenceNamesRequest(request);

        break;
    case CIM_GET_PROPERTY_REQUEST_MESSAGE:
        response = handleGetPropertyRequest(request);

        break;
    case CIM_SET_PROPERTY_REQUEST_MESSAGE:
        response = handleSetPropertyRequest(request);

        break;
    case CIM_INVOKE_METHOD_REQUEST_MESSAGE:
        response = handleInvokeMethodRequest(request);

        break;
    case CIM_CREATE_SUBSCRIPTION_REQUEST_MESSAGE:
        response = handleCreateSubscriptionRequest(request);

        break;
    case CIM_MODIFY_SUBSCRIPTION_REQUEST_MESSAGE:
        response = handleModifySubscriptionRequest(request);

        break;
    case CIM_DELETE_SUBSCRIPTION_REQUEST_MESSAGE:
        response = handleDeleteSubscriptionRequest(request);

        break;
    case CIM_ENABLE_INDICATIONS_REQUEST_MESSAGE:
        response = handleEnableIndicationsRequest(request);

        break;
    case CIM_DISABLE_INDICATIONS_REQUEST_MESSAGE:
        response = handleDisableIndicationsRequest(request);

        break;
    case CIM_CONSUME_INDICATION_REQUEST_MESSAGE:
        response = handleConsumeIndicationRequest(request);
        break;

    case CIM_DISABLE_MODULE_REQUEST_MESSAGE:
        response = handleDisableModuleRequest(request);

        break;
    case CIM_ENABLE_MODULE_REQUEST_MESSAGE:
        response = handleEnableModuleRequest(request);

        break;
    case CIM_STOP_ALL_PROVIDERS_REQUEST_MESSAGE:
        response = handleStopAllProvidersRequest(request);

        break;
    default:
        response = handleUnsupportedRequest(request);

        break;
    }

    PEG_METHOD_EXIT();

    return(response);
}

Message * DefaultProviderManager::handleUnsupportedRequest(const Message * message) throw()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "DefaultProviderManager::handleUnsupportedRequest");

    PEG_METHOD_EXIT();

    // a null response implies unsupported or unknown operation
    return(0);
}

Message * DefaultProviderManager::handleGetInstanceRequest(const Message * message) throw()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "DefaultProviderManager::handleGetInstanceRequest");

    CIMGetInstanceRequestMessage * request =
        dynamic_cast<CIMGetInstanceRequestMessage *>(const_cast<Message *>(message));

    PEGASUS_ASSERT(request != 0);

    CIMGetInstanceResponseMessage * response =
        new CIMGetInstanceResponseMessage(
        request->messageId,
        CIMException(),
        request->queueIds.copyAndPop(),
        CIMInstance());

    PEGASUS_ASSERT(response != 0);

    // preserve message key
    response->setKey(request->getKey());

    //  Set HTTP method in response from request
    response->setHttpMethod(request->getHttpMethod());

    // create a handler for this request
    GetInstanceResponseHandler handler(request, response);

    try
    {
        Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            "DefaultProviderManager::handleGetInstanceRequest - Host name: $0  Name space: $1  Class name: $2",
            System::getHostName(),
            request->nameSpace.getString(),
            request->instanceName.getClassName().getString());

        // make target object path
        CIMObjectPath objectPath(
            System::getHostName(),
            request->nameSpace,
            request->instanceName.getClassName(),
            request->instanceName.getKeyBindings());

        ProviderName name(
            objectPath.toString(),
            String::EMPTY,
            String::EMPTY,
            String::EMPTY,
            0);

        // resolve provider name
        name = _resolveProviderName(name);

        // get cached or load new provider module
        OpProviderHolder ph =
            providerManager.getProvider(name.getPhysicalName(), name.getLogicalName(), String::EMPTY);

        // convert arguments
        OperationContext context;

        context.insert(IdentityContainer(request->userName));
        context.insert(AcceptLanguageListContainer(request->acceptLanguages));
        context.insert(ContentLanguageListContainer(request->contentLanguages));

        CIMPropertyList propertyList(request->propertyList);

        // forward request
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Calling provider.getInstance: " +
            ph.GetProvider().getName());

        pm_service_op_lock op_lock(&ph.GetProvider());

        STAT_GETSTARTTIME;

        ph.GetProvider().getInstance(
            context,
            objectPath,
            request->includeQualifiers,
            request->includeClassOrigin,
            propertyList,
            handler);

        STAT_PMS_PROVIDEREND;
    }
    catch(CIMException & e)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Exception: " + e.getMessage());

        handler.setStatus(e.getCode(), e.getContentLanguages(), e.getMessage()); // l10n
    }
    catch(Exception & e)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Exception: " + e.getMessage());

        handler.setStatus(CIM_ERR_FAILED, e.getContentLanguages(), e.getMessage()); // l10n
    }
    catch(...)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Exception: Unknown");

        handler.setStatus(CIM_ERR_FAILED, "Unknown error.");
    }

    PEG_METHOD_EXIT();

    return(response);
}

Message * DefaultProviderManager::handleEnumerateInstancesRequest(const Message * message) throw()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "DefaultProviderManager::handleEnumerateInstanceRequest");

    CIMEnumerateInstancesRequestMessage * request =
        dynamic_cast<CIMEnumerateInstancesRequestMessage *>(const_cast<Message *>(message));

    PEGASUS_ASSERT(request != 0);

    CIMEnumerateInstancesResponseMessage * response =
        new CIMEnumerateInstancesResponseMessage(
        request->messageId,
        CIMException(),
        request->queueIds.copyAndPop(),
        Array<CIMInstance>());

    PEGASUS_ASSERT(response != 0);

    // preserve message key
    response->setKey(request->getKey());

    //  Set HTTP method in response from request
    response->setHttpMethod (request->getHttpMethod ());

    // create a handler for this request
    EnumerateInstancesResponseHandler handler(request, response);

    try
    {
        Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            "DefaultProviderManager::handleEnumerateInstancesRequest - Host name: $0  Name space: $1  Class name: $2",
            System::getHostName(),
            request->nameSpace.getString(),
            request->className.getString());

        // make target object path
        CIMObjectPath objectPath(
            System::getHostName(),
            request->nameSpace,
            request->className);

        ProviderName name(
            objectPath.toString(),
            String::EMPTY,
            String::EMPTY,
            String::EMPTY,
            0);

        // resolve provider name
        name = _resolveProviderName(name);

        // get cached or load new provider module
        OpProviderHolder ph =
            providerManager.getProvider(name.getPhysicalName(), name.getLogicalName(), String::EMPTY);

        // convert arguments
        OperationContext context;

        context.insert(IdentityContainer(request->userName));
        context.insert(AcceptLanguageListContainer(request->acceptLanguages));
        context.insert(ContentLanguageListContainer(request->contentLanguages));

        CIMPropertyList propertyList(request->propertyList);

        // forward request
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Calling provider.enumerateInstances: " +
            ph.GetProvider().getName());

        pm_service_op_lock op_lock(&ph.GetProvider());

        STAT_GETSTARTTIME;

        ph.GetProvider().enumerateInstances(
            context,
            objectPath,
            request->includeQualifiers,
            request->includeClassOrigin,
            propertyList,
            handler);

        STAT_PMS_PROVIDEREND;
    }
    catch(CIMException & e)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Exception: " + e.getMessage());

        handler.setStatus(e.getCode(), e.getContentLanguages(), e.getMessage()); // l10n
    }
    catch(Exception & e)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Exception: " + e.getMessage());

        handler.setStatus(CIM_ERR_FAILED, e.getContentLanguages(), e.getMessage()); // l10n
    }
    catch(...)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Exception: Unknown");

        handler.setStatus(CIM_ERR_FAILED, "Unknown error.");
    }

    PEG_METHOD_EXIT();

    return(response);
}

Message * DefaultProviderManager::handleEnumerateInstanceNamesRequest(const Message * message) throw()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "DefaultProviderManager::handleEnumerateInstanceNamesRequest");

    CIMEnumerateInstanceNamesRequestMessage * request =
        dynamic_cast<CIMEnumerateInstanceNamesRequestMessage *>(const_cast<Message *>(message));

    PEGASUS_ASSERT(request != 0);

    CIMEnumerateInstanceNamesResponseMessage * response =
        new CIMEnumerateInstanceNamesResponseMessage(
        request->messageId,
        CIMException(),
        request->queueIds.copyAndPop(),
        Array<CIMObjectPath>());

    PEGASUS_ASSERT(response != 0);

    // preserve message key
    response->setKey(request->getKey());

    //set HTTP method in response from request
    response->setHttpMethod(request->getHttpMethod());;

    // create a handler for this request
    EnumerateInstanceNamesResponseHandler handler(request, response);

    // process the request
    try
    {
        Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            "DefaultProviderManager::handleEnumerateInstanceNamesRequest - Host name: $0  Name space: $1  Class name: $2",
            System::getHostName(),
            request->nameSpace.getString(),
            request->className.getString());

        // make target object path
        CIMObjectPath objectPath(
            System::getHostName(),
            request->nameSpace,
            request->className);

        // build an internal provider name from the request arguments
        ProviderName name(
            objectPath.toString(),
            String::EMPTY,
            String::EMPTY,
            String::EMPTY,
            0);

        // resolve provider name
        name = _resolveProviderName(name);

        // get cached or load new provider module
        OpProviderHolder ph =
            providerManager.getProvider(name.getPhysicalName(), name.getLogicalName());

        // convert arguments
        OperationContext context;

        context.insert(IdentityContainer(request->userName));
        context.insert(AcceptLanguageListContainer(request->acceptLanguages));
        context.insert(ContentLanguageListContainer(request->contentLanguages));

        // forward request
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Calling provider.enumerateInstanceNames: " +
            ph.GetProvider().getName());

        pm_service_op_lock op_lock(&ph.GetProvider());

        STAT_GETSTARTTIME;

        ph.GetProvider().enumerateInstanceNames(
            context,
            objectPath,
            handler);

        STAT_PMS_PROVIDEREND;
    }
    catch(CIMException & e)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Exception: " + e.getMessage());

        handler.setStatus(e.getCode(), e.getContentLanguages(), e.getMessage()); // l10n
    }
    catch(Exception & e)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Exception: " + e.getMessage());

        handler.setStatus(CIM_ERR_FAILED, e.getContentLanguages(), e.getMessage()); // l10n
    }
    catch(...)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Exception: Unknown");

        handler.setStatus(CIM_ERR_FAILED, "Unknown error.");
    }

    PEG_METHOD_EXIT();

    return(response);
}

Message * DefaultProviderManager::handleCreateInstanceRequest(const Message * message) throw()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "DefaultProviderManager::handleCreateInstanceRequest");

    CIMCreateInstanceRequestMessage * request =
        dynamic_cast<CIMCreateInstanceRequestMessage *>(const_cast<Message *>(message));

    PEGASUS_ASSERT(request != 0);

    // create response message
    CIMCreateInstanceResponseMessage * response =
        new CIMCreateInstanceResponseMessage(
        request->messageId,
        CIMException(),
        request->queueIds.copyAndPop(),
        CIMObjectPath());

    PEGASUS_ASSERT(response != 0);

    // preserve message key
    response->setKey(request->getKey());

    //  Set HTTP method in response from request
    response->setHttpMethod (request->getHttpMethod ());

    // create a handler for this request
    CreateInstanceResponseHandler handler(request, response);

    try
    {
        Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            "DefaultProviderManager::handleCreateInstanceRequest - Host name: $0  Name space: $1  Class name: $2",
            System::getHostName(),
            request->nameSpace.getString(),
            request->newInstance.getPath().getClassName().getString());

        // make target object path
        CIMObjectPath objectPath(
            System::getHostName(),
            request->nameSpace,
            request->newInstance.getPath().getClassName(),
            request->newInstance.getPath().getKeyBindings());

        ProviderName name(
            objectPath.toString(),
            String::EMPTY,
            String::EMPTY,
            String::EMPTY,
            0);

        // resolve provider name
        name = _resolveProviderName(name);

        // get cached or load new provider module
        OpProviderHolder ph =
            providerManager.getProvider(name.getPhysicalName(), name.getLogicalName(), String::EMPTY);

        // convert arguments
        OperationContext context;

        context.insert(IdentityContainer(request->userName));
        context.insert(AcceptLanguageListContainer(request->acceptLanguages));
        context.insert(ContentLanguageListContainer(request->contentLanguages));

        // forward request
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Calling provider.createInstance: " +
            ph.GetProvider().getName());

        pm_service_op_lock op_lock(&ph.GetProvider());

        STAT_GETSTARTTIME;

        ph.GetProvider().createInstance(
            context,
            objectPath,
            request->newInstance,
            handler);

        STAT_PMS_PROVIDEREND;
    }
    catch(CIMException & e)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Exception: " + e.getMessage());

        handler.setStatus(e.getCode(), e.getContentLanguages(), e.getMessage()); // l10n
    }
    catch(Exception & e)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Exception: " + e.getMessage());

        handler.setStatus(CIM_ERR_FAILED, e.getContentLanguages(), e.getMessage()); // l10n
    }
    catch(...)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Exception: Unknown");

        handler.setStatus(CIM_ERR_FAILED, "Unknown error.");
    }

    PEG_METHOD_EXIT();

    return(response);
}

Message * DefaultProviderManager::handleModifyInstanceRequest(const Message * message) throw()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "DefaultProviderManager::handleModifyInstanceRequest");

    CIMModifyInstanceRequestMessage * request =
        dynamic_cast<CIMModifyInstanceRequestMessage *>(const_cast<Message *>(message));

    PEGASUS_ASSERT(request != 0);

    // create response message
    CIMModifyInstanceResponseMessage * response =
        new CIMModifyInstanceResponseMessage(
        request->messageId,
        CIMException(),
        request->queueIds.copyAndPop());

    PEGASUS_ASSERT(response != 0);

    // preserve message key
    response->setKey(request->getKey());

    //  Set HTTP method in response from request
    response->setHttpMethod (request->getHttpMethod ());

    // create a handler for this request
    ModifyInstanceResponseHandler handler(request, response);

    try
    {
        Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            "DefaultProviderManager::handleModifyInstanceRequest - Host name: $0  Name space: $1  Class name: $2",
            System::getHostName(),
            request->nameSpace.getString(),
            request->modifiedInstance.getPath().getClassName().getString());

        // make target object path
        CIMObjectPath objectPath(
            System::getHostName(),
            request->nameSpace,
            request->modifiedInstance.getPath ().getClassName(),
            request->modifiedInstance.getPath ().getKeyBindings());

        ProviderName name(
            objectPath.toString(),
            String::EMPTY,
            String::EMPTY,
            String::EMPTY,
            0);

        // resolve provider name
        name = _resolveProviderName(name);

        // get cached or load new provider module
        OpProviderHolder ph =
            providerManager.getProvider(name.getPhysicalName(), name.getLogicalName(), String::EMPTY);

        // convert arguments
        OperationContext context;

        context.insert(IdentityContainer(request->userName));
        context.insert(AcceptLanguageListContainer(request->acceptLanguages));
        context.insert(ContentLanguageListContainer(request->contentLanguages));

        CIMPropertyList propertyList(request->propertyList);

        // forward request
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Calling provider.modifyInstance: " +
            ph.GetProvider().getName());

        pm_service_op_lock op_lock(&ph.GetProvider());

        STAT_GETSTARTTIME;

        ph.GetProvider().modifyInstance(
            context,
            objectPath,
            request->modifiedInstance,
            request->includeQualifiers,
            propertyList,
            handler);

        STAT_PMS_PROVIDEREND;
    }
    catch(CIMException & e)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Exception: " + e.getMessage());

        handler.setStatus(e.getCode(), e.getContentLanguages(), e.getMessage()); // l10n
    }
    catch(Exception & e)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Exception: " + e.getMessage());

        handler.setStatus(CIM_ERR_FAILED, e.getContentLanguages(), e.getMessage()); // l10n
    }
    catch(...)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Exception: Unknown");

        handler.setStatus(CIM_ERR_FAILED, "Unknown error.");
    }

    PEG_METHOD_EXIT();

    return(response);
}

Message * DefaultProviderManager::handleDeleteInstanceRequest(const Message * message) throw()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "DefaultProviderManager::handleDeleteInstanceRequest");

    CIMDeleteInstanceRequestMessage * request =
        dynamic_cast<CIMDeleteInstanceRequestMessage *>(const_cast<Message *>(message));

    PEGASUS_ASSERT(request != 0);

    // create response message
    CIMDeleteInstanceResponseMessage * response =
        new CIMDeleteInstanceResponseMessage(
        request->messageId,
        CIMException(),
        request->queueIds.copyAndPop());

    PEGASUS_ASSERT(response != 0);

    // preserve message key
    response->setKey(request->getKey());

    //  Set HTTP method in response from request
    response->setHttpMethod (request->getHttpMethod ());

    // create a handler for this request
    DeleteInstanceResponseHandler handler(request, response);

    try
    {
        Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            "DefaultProviderManager::handleDeleteInstanceRequest - Host name: $0  Name space: $1  Class name: $2",
            System::getHostName(),
            request->nameSpace.getString(),
            request->instanceName.getClassName().getString());

        // make target object path
        CIMObjectPath objectPath(
            System::getHostName(),
            request->nameSpace,
            request->instanceName.getClassName(),
            request->instanceName.getKeyBindings());

        ProviderName name(
            objectPath.toString(),
            String::EMPTY,
            String::EMPTY,
            String::EMPTY,
            0);

        // resolve provider name
        name = _resolveProviderName(name);

        // get cached or load new provider module
        OpProviderHolder ph =
            providerManager.getProvider(name.getPhysicalName(), name.getLogicalName(), String::EMPTY);

        // convert arguments
        OperationContext context;

        context.insert(IdentityContainer(request->userName));
        context.insert(AcceptLanguageListContainer(request->acceptLanguages));
        context.insert(ContentLanguageListContainer(request->contentLanguages));

        // forward request
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Calling provider.deleteInstance: " +
            ph.GetProvider().getName());

        pm_service_op_lock op_lock(&ph.GetProvider());

        STAT_GETSTARTTIME;

        ph.GetProvider().deleteInstance(
            context,
            objectPath,
            handler);

        STAT_PMS_PROVIDEREND;
    }
    catch(CIMException & e)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Exception: " + e.getMessage());

        handler.setStatus(e.getCode(), e.getContentLanguages(), e.getMessage()); // l10n
    }
    catch(Exception & e)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Exception: " + e.getMessage());

        handler.setStatus(CIM_ERR_FAILED, e.getContentLanguages(), e.getMessage()); // l10n
    }
    catch(...)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Exception: Unknown");

        handler.setStatus(CIM_ERR_FAILED, "Unknown error.");
    }

    PEG_METHOD_EXIT();

    return(response);
}

Message * DefaultProviderManager::handleExecuteQueryRequest(const Message * message) throw()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "DefaultProviderManager::handleExecuteQueryRequest");

    CIMExecQueryRequestMessage * request =
        dynamic_cast<CIMExecQueryRequestMessage *>(const_cast<Message *>(message));

    PEGASUS_ASSERT(request != 0);

    //l10n
    CIMExecQueryResponseMessage * response =
        new CIMExecQueryResponseMessage(
        request->messageId,
        PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED, MessageLoaderParms(
        "ProviderManager.DefaultProviderManager.NOT_IMPLEMENTED",
        "not implemented")),
        request->queueIds.copyAndPop(),
        Array<CIMObject>());

    PEGASUS_ASSERT(response != 0);

    // preserve message key
    response->setKey(request->getKey());

    //  Set HTTP method in response from request
    response->setHttpMethod(request->getHttpMethod());

    // l10n
    // ATTN: when this is implemented, need to add the language containers to the
    // OperationContext.  See how the other requests do it.

    PEG_METHOD_EXIT();

    return(response);
}

Message * DefaultProviderManager::handleAssociatorsRequest(const Message * message) throw()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "DefaultProviderManager::handleAssociatorsRequest");

    CIMAssociatorsRequestMessage * request =
        dynamic_cast<CIMAssociatorsRequestMessage *>(const_cast<Message *>(message));

    PEGASUS_ASSERT(request != 0);

    CIMAssociatorsResponseMessage * response =
        new CIMAssociatorsResponseMessage(
        request->messageId,
        CIMException(),
        request->queueIds.copyAndPop(),
        Array<CIMObject>());

    PEGASUS_ASSERT(response != 0);

    // preserve message key
    response->setKey(request->getKey());

    // create a handler for this request
    AssociatorsResponseHandler handler(request, response);

    // process the request
    try
    {
        Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            "DefaultProviderManager::handleAssociatorsRequest - Host name: $0  Name space: $1  Class name: $2",
            System::getHostName(),
            request->nameSpace.getString(),
            request->objectName.getClassName().getString());

        // make target object path
        CIMObjectPath objectPath(
            System::getHostName(),
            request->nameSpace,
            request->objectName.getClassName());

        objectPath.setKeyBindings(request->objectName.getKeyBindings());

        ProviderName name(
            objectPath.toString(),
            String::EMPTY,
            String::EMPTY,
            String::EMPTY,
            0);

        // resolve provider name
        name = _resolveProviderName(name);

        // get cached or load new provider module
        OpProviderHolder ph =
            providerManager.getProvider(name.getPhysicalName(), name.getLogicalName(), String::EMPTY);

        // convert arguments
        OperationContext context;

        context.insert(IdentityContainer(request->userName));
        context.insert(AcceptLanguageListContainer(request->acceptLanguages));
        context.insert(ContentLanguageListContainer(request->contentLanguages));

        // ATTN KS STAT_GETSTARTTIME;
        pm_service_op_lock op_lock(&ph.GetProvider());

        ph.GetProvider().associators(
            context,
            objectPath,
            request->assocClass,
            request->resultClass,
            request->role,
            request->resultRole,
            request->includeQualifiers,
            request->includeClassOrigin,
            request->propertyList.getPropertyNameArray(),
            handler);

        STAT_PMS_PROVIDEREND;
    }
    catch(CIMException & e)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Exception: " + e.getMessage());

        handler.setStatus(e.getCode(), e.getContentLanguages(), e.getMessage()); // l10n
    }
    catch(Exception & e)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Exception: " + e.getMessage());

        handler.setStatus(CIM_ERR_FAILED, e.getContentLanguages(), e.getMessage()); // l10n
    }
    catch(...)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Exception: Unknown");

        handler.setStatus(CIM_ERR_FAILED, "Unknown error.");
    }

    PEG_METHOD_EXIT();

    return(response);
}

Message * DefaultProviderManager::handleAssociatorNamesRequest(const Message * message) throw()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "DefaultProviderManager::handleAssociatorNamesRequest");

    CIMAssociatorNamesRequestMessage * request =
        dynamic_cast<CIMAssociatorNamesRequestMessage *>(const_cast<Message *>(message));

    PEGASUS_ASSERT(request != 0);

    CIMAssociatorNamesResponseMessage * response =
        new CIMAssociatorNamesResponseMessage(
        request->messageId,
        CIMException(),
        request->queueIds.copyAndPop(),
        Array<CIMObjectPath>());

    PEGASUS_ASSERT(response != 0);

    // preserve message key
    response->setKey(request->getKey());

    //  Set HTTP method in response from request
    response->setHttpMethod(request->getHttpMethod());

    // create a handler for this request
    AssociatorNamesResponseHandler handler(request, response);

    // process the request
    try
    {
        Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            "DefaultProviderManager::handleAssociationNamesRequest - Host name: $0  Name space: $1  Class name: $2",
            System::getHostName(),
            request->nameSpace.getString(),
            request->objectName.getClassName().getString());

        // make target object path
        CIMObjectPath objectPath(
            System::getHostName(),
            request->nameSpace,
            request->objectName.getClassName());

        objectPath.setKeyBindings(request->objectName.getKeyBindings());

        ProviderName name(
            objectPath.toString(),
            String::EMPTY,
            String::EMPTY,
            String::EMPTY,
            0);

        // resolve provider name
        name = _resolveProviderName(name);

        // get cached or load new provider module
        OpProviderHolder ph =
            providerManager.getProvider(name.getPhysicalName(), name.getLogicalName(), String::EMPTY);

        // convert arguments
        OperationContext context;

        context.insert(IdentityContainer(request->userName));
        context.insert(AcceptLanguageListContainer(request->acceptLanguages));
        context.insert(ContentLanguageListContainer(request->contentLanguages));

        pm_service_op_lock op_lock(&ph.GetProvider());

        ph.GetProvider().associatorNames(
            context,
            objectPath,
            request->assocClass,
            request->resultClass,
            request->role,
            request->resultRole,
            handler);

        STAT_PMS_PROVIDEREND;
    }
    catch(CIMException & e)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Exception: " + e.getMessage());

        handler.setStatus(e.getCode(), e.getContentLanguages(), e.getMessage()); // l10n
    }
    catch(Exception & e)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Exception: " + e.getMessage());

        handler.setStatus(CIM_ERR_FAILED, e.getContentLanguages(), e.getMessage()); // l10n
    }
    catch(...)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Exception: Unknown");

        handler.setStatus(CIM_ERR_FAILED, "Unknown error.");
    }

    PEG_METHOD_EXIT();

    return(response);
}

Message * DefaultProviderManager::handleReferencesRequest(const Message * message) throw()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "DefaultProviderManager::handleReferencesRequest");

    CIMReferencesRequestMessage * request =
        dynamic_cast<CIMReferencesRequestMessage *>(const_cast<Message *>(message));

    PEGASUS_ASSERT(request != 0);

    CIMReferencesResponseMessage * response =
        new CIMReferencesResponseMessage(
        request->messageId,
        CIMException(),
        request->queueIds.copyAndPop(),
        Array<CIMObject>());

    PEGASUS_ASSERT(response != 0);

    // preserve message key
    response->setKey(request->getKey());

    //  Set HTTP method in response from request
    response->setHttpMethod (request->getHttpMethod ());

    // create a handler for this request
    ReferencesResponseHandler handler(request, response);

    // process the request
    try
    {
        Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            "DefaultProviderManager::handleReferencesRequest - Host name: $0  Name space: $1  Class name: $2",
            System::getHostName(),
            request->nameSpace.getString(),
            request->objectName.getClassName().getString());

        // make target object path
        CIMObjectPath objectPath(
            System::getHostName(),
            request->nameSpace,
            request->objectName.getClassName());

        objectPath.setKeyBindings(request->objectName.getKeyBindings());

        ProviderName name(
            objectPath.toString(),
            String::EMPTY,
            String::EMPTY,
            String::EMPTY,
            0);

        // resolve provider name
        name = _resolveProviderName(name);

        // get cached or load new provider module
        OpProviderHolder ph =
            providerManager.getProvider(name.getPhysicalName(), name.getLogicalName(), String::EMPTY);

        // convert arguments
        OperationContext context;

        context.insert(IdentityContainer(request->userName));
        context.insert(AcceptLanguageListContainer(request->acceptLanguages));
        context.insert(ContentLanguageListContainer(request->contentLanguages));

        STAT_GETSTARTTIME;

        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Calling provider.references: " +
            ph.GetProvider().getName());

        pm_service_op_lock op_lock(&ph.GetProvider());

        ph.GetProvider().references(
            context,
            objectPath,
            request->resultClass,
            request->role,
            request->includeQualifiers,
            request->includeClassOrigin,
            request->propertyList.getPropertyNameArray(),
            handler);

        STAT_PMS_PROVIDEREND;
    }
    catch(CIMException & e)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Exception: " + e.getMessage());

        handler.setStatus(e.getCode(), e.getContentLanguages(), e.getMessage()); // l10n
    }
    catch(Exception & e)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Exception: " + e.getMessage());

        handler.setStatus(CIM_ERR_FAILED, e.getContentLanguages(), e.getMessage()); // l10n
    }
    catch(...)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Exception: Unknown");

        handler.setStatus(CIM_ERR_FAILED, "Unknown error.");
    }

    PEG_METHOD_EXIT();

    return(response);
}

Message * DefaultProviderManager::handleReferenceNamesRequest(const Message * message) throw()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "DefaultProviderManager::handleReferenceNamesRequest");

    CIMReferenceNamesRequestMessage * request =
        dynamic_cast<CIMReferenceNamesRequestMessage *>(const_cast<Message *>(message));

    PEGASUS_ASSERT(request != 0);

    CIMReferenceNamesResponseMessage * response =
        new CIMReferenceNamesResponseMessage(
        request->messageId,
        CIMException(),
        request->queueIds.copyAndPop(),
        Array<CIMObjectPath>());

    // preserve message key
    response->setKey(request->getKey());

    //  Set HTTP method in response from request
    response->setHttpMethod (request->getHttpMethod ());

    // create a handler for this request
    ReferenceNamesResponseHandler handler(request, response);

    // process the request
    try
    {
        Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            "DefaultProviderManager::handleReferenceNamesRequest - Host name: $0  Name space: $1  Class name: $2",
            System::getHostName(),
            request->nameSpace.getString(),
            request->objectName.getClassName().getString());

        // make target object path
        CIMObjectPath objectPath(
            System::getHostName(),
            request->nameSpace,
            request->objectName.getClassName());

        objectPath.setKeyBindings(request->objectName.getKeyBindings());

        ProviderName name(
            objectPath.toString(),
            String::EMPTY,
            String::EMPTY,
            String::EMPTY,
            0);

        // resolve provider name
        name = _resolveProviderName(name);

        // get cached or load new provider module
        OpProviderHolder ph =
            providerManager.getProvider(name.getPhysicalName(), name.getLogicalName(), String::EMPTY);

        // convert arguments
        OperationContext context;

        context.insert(IdentityContainer(request->userName));
        context.insert(AcceptLanguageListContainer(request->acceptLanguages));
        context.insert(ContentLanguageListContainer(request->contentLanguages));

        STAT_GETSTARTTIME;

        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Calling provider.referenceNames: " +
            ph.GetProvider().getName());

        pm_service_op_lock op_lock(&ph.GetProvider());

        ph.GetProvider().referenceNames(
            context,
            objectPath,
            request->resultClass,
            request->role,
            handler);

        STAT_PMS_PROVIDEREND;
    }
    catch(CIMException & e)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Exception: " + e.getMessage());

        handler.setStatus(e.getCode(), e.getContentLanguages(), e.getMessage()); // l10n
    }
    catch(Exception & e)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Exception: " + e.getMessage());

        handler.setStatus(CIM_ERR_FAILED, e.getContentLanguages(), e.getMessage()); // l10n
    }
    catch(...)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Exception: Unknown");

        handler.setStatus(CIM_ERR_FAILED, "Unknown error.");
    }

    PEG_METHOD_EXIT();

    return(response);
}

Message * DefaultProviderManager::handleGetPropertyRequest(const Message * message) throw()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "DefaultProviderManager::handleGetPropertyRequest");

    CIMGetPropertyRequestMessage * request =
        dynamic_cast<CIMGetPropertyRequestMessage *>(const_cast<Message *>(message));

    PEGASUS_ASSERT(request != 0);

    // create response message
    CIMGetPropertyResponseMessage * response =
        new CIMGetPropertyResponseMessage(
        request->messageId,
        CIMException(),
        request->queueIds.copyAndPop(),
        CIMValue());

    PEGASUS_ASSERT(response != 0);

    // preserve message key
    response->setKey(request->getKey());

    //  Set HTTP method in response from request
    response->setHttpMethod(request->getHttpMethod());

    GetPropertyResponseHandler handler(request, response);

    try
    {
        Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            "DefaultProviderManager::handleGetPropertyRequest - Host name: $0  Name space: $1  Class name: $2",
            System::getHostName(),
            request->nameSpace.getString(),
            request->instanceName.getClassName().getString());

        // make target object path
        CIMObjectPath objectPath(
            System::getHostName(),
            request->nameSpace,
            request->instanceName.getClassName(),
            request->instanceName.getKeyBindings());

        ProviderName name(
            objectPath.toString(),
            String::EMPTY,
            String::EMPTY,
            String::EMPTY,
            0);

        // resolve provider name
        name = _resolveProviderName(name);

        // get cached or load new provider module
        OpProviderHolder ph =
            providerManager.getProvider(name.getPhysicalName(), name.getLogicalName(), String::EMPTY);

        // convert arguments
        OperationContext context;

        context.insert(IdentityContainer(request->userName));
        context.insert(AcceptLanguageListContainer(request->acceptLanguages));
        context.insert(ContentLanguageListContainer(request->contentLanguages));

        CIMName propertyName = request->propertyName;

        STAT_GETSTARTTIME;

        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Calling provider.getProperty: " +
            ph.GetProvider().getName());

        // forward request
        pm_service_op_lock op_lock(&ph.GetProvider());

        ph.GetProvider().getProperty(
            context,
            objectPath,
            propertyName,
            handler);

        STAT_PMS_PROVIDEREND;
    }
    catch(CIMException & e)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Exception: " + e.getMessage());

        handler.setStatus(e.getCode(), e.getContentLanguages(), e.getMessage()); // l10n
    }
    catch(Exception & e)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Exception: " + e.getMessage());

        handler.setStatus(CIM_ERR_FAILED, e.getContentLanguages(), e.getMessage()); // l10n
    }
    catch(...)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Exception: Unknown");

        handler.setStatus(CIM_ERR_FAILED, "Unknown error.");
    }

    PEG_METHOD_EXIT();

    return(response);
}

Message * DefaultProviderManager::handleSetPropertyRequest(const Message * message) throw()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "DefaultProviderManager::handleSetPropertyRequest");

    CIMSetPropertyRequestMessage * request =
        dynamic_cast<CIMSetPropertyRequestMessage *>(const_cast<Message *>(message));

    PEGASUS_ASSERT(request != 0);

    // create response message
    //l10n
    CIMSetPropertyResponseMessage * response =
        new CIMSetPropertyResponseMessage(
        request->messageId,
        PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED, MessageLoaderParms(
        "ProviderManager.DefaultProviderManager.NOT_IMPLEMENTED",
        "not implemented")),
        request->queueIds.copyAndPop());

    PEGASUS_ASSERT(response != 0);

    // preserve message key
    response->setKey(request->getKey());

    //  Set HTTP method in response from request
    response->setHttpMethod(request->getHttpMethod());

    SetPropertyResponseHandler handler(request, response);

    try
    {
        Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            "DefaultProviderManager::handleSetPropertyRequest - Host name: $0  Name space: $1  Class name: $2",
            System::getHostName(),
            request->nameSpace.getString(),
            request->instanceName.getClassName().getString());

        // make target object path
        CIMObjectPath objectPath(
            System::getHostName(),
            request->nameSpace,
            request->instanceName.getClassName(),
            request->instanceName.getKeyBindings());

        ProviderName name(
            objectPath.toString(),
            String::EMPTY,
            String::EMPTY,
            String::EMPTY,
            0);

        // resolve provider name
        name = _resolveProviderName(name);

        // get cached or load new provider module
        OpProviderHolder ph =
            providerManager.getProvider(name.getPhysicalName(), name.getLogicalName(), String::EMPTY);

        // convert arguments
        OperationContext context;

        context.insert(IdentityContainer(request->userName));
        context.insert(AcceptLanguageListContainer(request->acceptLanguages));
        context.insert(ContentLanguageListContainer(request->contentLanguages));

        CIMName propertyName = request->propertyName;
        CIMValue propertyValue = request->newValue;

        STAT_GETSTARTTIME;

        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Calling provider.setProperty: " +
            ph.GetProvider().getName());

        // forward request
        pm_service_op_lock op_lock(&ph.GetProvider());

        ph.GetProvider().setProperty(
            context,
            objectPath,
            propertyName,
            propertyValue,
            handler);

        STAT_PMS_PROVIDEREND;
    }
    catch(CIMException & e)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Exception: " + e.getMessage());

        handler.setStatus(e.getCode(), e.getContentLanguages(), e.getMessage()); // l10n
    }
    catch(Exception & e)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Exception: " + e.getMessage());

        handler.setStatus(CIM_ERR_FAILED, e.getContentLanguages(), e.getMessage()); // l10n
    }
    catch(...)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Exception: Unknown");

        handler.setStatus(CIM_ERR_FAILED, "Unknown error.");
    }

    PEG_METHOD_EXIT();

    return(response);
}

Message * DefaultProviderManager::handleInvokeMethodRequest(const Message * message) throw()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "DefaultProviderManager::handleInvokeMethodRequest");

    CIMInvokeMethodRequestMessage * request =
        dynamic_cast<CIMInvokeMethodRequestMessage *>(const_cast<Message *>(message));

    PEGASUS_ASSERT(request != 0);

    // create response message
    CIMInvokeMethodResponseMessage * response =
        new CIMInvokeMethodResponseMessage(
        request->messageId,
        CIMException(),
        request->queueIds.copyAndPop(),
        CIMValue(),
        Array<CIMParamValue>(),
        request->methodName);

    PEGASUS_ASSERT(response != 0);

    // propagate message key
    response->setKey(request->getKey());

    //  Set HTTP method in response from request
    response->setHttpMethod (request->getHttpMethod ());

    // create a handler for this request
    InvokeMethodResponseHandler handler(request, response);

    try
    {
        Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            "DefaultProviderManager::handleInvokeMethodRequest - Host name: $0  Name space: $1  Class name: $2",
            System::getHostName(),
            request->nameSpace.getString(),
            request->instanceName.getClassName().getString());

        // make target object path
        CIMObjectPath objectPath(
            System::getHostName(),
            request->nameSpace,
            request->instanceName.getClassName(),
            request->instanceName.getKeyBindings());

        ProviderName name(
            objectPath.toString(),
            String::EMPTY,
            String::EMPTY,
            String::EMPTY,
            0);

        // resolve provider name
        name = _resolveProviderName(name);

        // get cached or load new provider module
        OpProviderHolder ph =
            providerManager.getProvider(name.getPhysicalName(), name.getLogicalName(), String::EMPTY);

        // convert arguments
        OperationContext context;

        context.insert(IdentityContainer(request->userName));
        context.insert(AcceptLanguageListContainer(request->acceptLanguages));
        context.insert(ContentLanguageListContainer(request->contentLanguages));

        CIMObjectPath instanceReference(request->instanceName);

        // ATTN: propagate namespace
        instanceReference.setNameSpace(request->nameSpace);

        // forward request
        STAT_GETSTARTTIME;

        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Calling provider.invokeMethod: " +
            ph.GetProvider().getName());

        pm_service_op_lock op_lock(&ph.GetProvider());

        ph.GetProvider().invokeMethod(
            context,
            instanceReference,
            request->methodName,
            request->inParameters,
            handler);

        STAT_PMS_PROVIDEREND;
    }
    catch(CIMException & e)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Exception: " + e.getMessage());

        handler.setStatus(e.getCode(), e.getContentLanguages(), e.getMessage()); // l10n
    }
    catch(Exception & e)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Exception: " + e.getMessage());

        handler.setStatus(CIM_ERR_FAILED, e.getContentLanguages(), e.getMessage()); // l10n
    }
    catch(...)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Exception: Unknown");

        handler.setStatus(CIM_ERR_FAILED, "Unknown error.");
    }

    PEG_METHOD_EXIT();

    return(response);
}

Message * DefaultProviderManager::handleCreateSubscriptionRequest(const Message * message) throw()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "DefaultProviderManager::handleCreateSubscriptionRequest");

    CIMCreateSubscriptionRequestMessage * request =
        dynamic_cast<CIMCreateSubscriptionRequestMessage *>(const_cast<Message *>(message));

    PEGASUS_ASSERT(request != 0);

    CIMCreateSubscriptionResponseMessage * response =
        new CIMCreateSubscriptionResponseMessage(
        request->messageId,
        CIMException(),
        request->queueIds.copyAndPop());

    PEGASUS_ASSERT(response != 0);

    // preserve message key
    response->setKey(request->getKey());

    //  Set HTTP method in response from request
    response->setHttpMethod (request->getHttpMethod ());

    OperationResponseHandler handler(request, response);

    try
    {
        String temp;

        for(Uint32 i = 0, n = request->classNames.size(); i < n; i++)
        {
            temp.append(request->classNames[i].getString());

            if(i == (n - 1))
            {
                temp.append(", ");
            }
        }

        Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            "DefaultProviderManager::handleCreateSubscriptionRequest - Host name: $0  Name space: $1  Class name(s): $2",
            System::getHostName(),
            request->nameSpace.getString(),
            temp);

        // NOTE: the provider and provider module name are embedded in the request, presumably to reduce
        // the number of provider lookups (but this should be trivial if the data is cached and accessed
        // from the same source). to keep the lookups consistent, use the first class name, combined
        // with the other information in the request, and create an object path that can be used to
        // resolve the provider using the ProviderRegistrar.

        // make target object path
        CIMObjectPath objectPath(
            System::getHostName(),
            request->nameSpace,
            request->classNames[0].getString());

        ProviderName name(
            objectPath.toString(),
            String::EMPTY,
            String::EMPTY,
            String::EMPTY,
            0);

        // resolve provider name
        name = _resolveProviderName(name);

        // get cached or load new provider module
        OpProviderHolder ph =
            providerManager.getProvider(name.getPhysicalName(), name.getLogicalName(), String::EMPTY);

        // convert arguments
        OperationContext context;

        context.insert(IdentityContainer(request->userName));
        context.insert(SubscriptionInstanceContainer
            (request->subscriptionInstance));
        context.insert(SubscriptionFilterConditionContainer
            (request->condition, request->queryLanguage));
        context.insert(SubscriptionLanguageListContainer
            (request->acceptLanguages));
        context.insert(AcceptLanguageListContainer(request->acceptLanguages));
        context.insert(ContentLanguageListContainer(request->contentLanguages));

        CIMObjectPath subscriptionName = request->subscriptionInstance.getPath();

        Array<CIMObjectPath> classNames;

        for(Uint32 i = 0, n = request->classNames.size(); i < n; i++)
        {
            CIMObjectPath className(
                System::getHostName(),
                request->nameSpace,
                request->classNames[i]);

            classNames.append(className);
        }

        CIMPropertyList propertyList = request->propertyList;

        Uint16 repeatNotificationPolicy = request->repeatNotificationPolicy;

        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Calling provider.createSubscription: " +
            ph.GetProvider().getName());

        pm_service_op_lock op_lock(&ph.GetProvider());

        ph.GetProvider().createSubscription(
            context,
            subscriptionName,
            classNames,
            propertyList,
            repeatNotificationPolicy);
    }
    catch(CIMException & e)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Exception: " + e.getMessage());

        handler.setStatus(e.getCode(), e.getContentLanguages(), e.getMessage()); // l10n
    }
    catch(Exception & e)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Exception: " + e.getMessage());

        handler.setStatus(CIM_ERR_FAILED, e.getContentLanguages(), e.getMessage()); // l10n
    }
    catch(...)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Exception: Unknown");

        handler.setStatus(CIM_ERR_FAILED, "Unknown Error");
    }

    PEG_METHOD_EXIT();

    return(response);
}

Message * DefaultProviderManager::handleModifySubscriptionRequest( const Message * message) throw()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "DefaultProviderManager::handleModifySubscriptionRequest");

    CIMModifySubscriptionRequestMessage * request =
        dynamic_cast<CIMModifySubscriptionRequestMessage *>(const_cast<Message *>(message));

    PEGASUS_ASSERT(request != 0);

    CIMModifySubscriptionResponseMessage * response =
        new CIMModifySubscriptionResponseMessage(
        request->messageId,
        CIMException(),
        request->queueIds.copyAndPop());

    PEGASUS_ASSERT(response != 0);

    // preserve message key
    response->setKey(request->getKey());

    //  Set HTTP method in response from request
    response->setHttpMethod (request->getHttpMethod ());

    OperationResponseHandler handler(request, response);

    try
    {
        String temp;

        for(Uint32 i = 0, n = request->classNames.size(); i < n; i++)
        {
            temp.append(request->classNames[i].getString());

            if(i == (n - 1))
            {
                temp.append(", ");
            }
        }

        Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            "DefaultProviderManager::handleModifySubscriptionRequest - Host name: $0  Name space: $1  Class name(s): $2",
            System::getHostName(),
            request->nameSpace.getString(),
            temp);

        // NOTE: the provider and provider module name are embedded in the request, presumably to reduce
        // the number of provider lookups (but this should be trivial if the data is cached and accessed
        // from the same source). to keep the lookups consistent, use the first class name, combined
        // with the other information in the request, and create an object path that can be used to
        // resolve the provider using the ProviderRegistrar.

        // make target object path
        CIMObjectPath objectPath(
            System::getHostName(),
            request->nameSpace,
            request->classNames[0].getString());

        ProviderName name(
            objectPath.toString(),
            String::EMPTY,
            String::EMPTY,
            String::EMPTY,
            0);

        // resolve provider name
        name = _resolveProviderName(name);

        // get cached or load new provider module
        OpProviderHolder ph =
            providerManager.getProvider(name.getPhysicalName(), name.getLogicalName(), String::EMPTY);

        // convert arguments
        OperationContext context;

        context.insert(IdentityContainer(request->userName));
        context.insert(SubscriptionInstanceContainer
            (request->subscriptionInstance));
        context.insert(SubscriptionFilterConditionContainer
            (request->condition, request->queryLanguage));
        context.insert(SubscriptionLanguageListContainer
            (request->acceptLanguages));
        context.insert(AcceptLanguageListContainer(request->acceptLanguages));
        context.insert(ContentLanguageListContainer(request->contentLanguages));

        CIMObjectPath subscriptionName = request->subscriptionInstance.getPath();

        Array<CIMObjectPath> classNames;

        for(Uint32 i = 0, n = request->classNames.size(); i < n; i++)
        {
            CIMObjectPath className(
                System::getHostName(),
                request->nameSpace,
                request->classNames[i]);

            classNames.append(className);
        }

        CIMPropertyList propertyList = request->propertyList;

        Uint16 repeatNotificationPolicy = request->repeatNotificationPolicy;

        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Calling provider.modifySubscription: " +
            ph.GetProvider().getName());

        pm_service_op_lock op_lock(&ph.GetProvider());

        ph.GetProvider().modifySubscription(
            context,
            subscriptionName,
            classNames,
            propertyList,
            repeatNotificationPolicy);
    }
    catch(CIMException & e)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Exception: " + e.getMessage());

        handler.setStatus(e.getCode(), e.getContentLanguages(), e.getMessage()); // l10n
    }
    catch(Exception & e)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Exception: " + e.getMessage());

        handler.setStatus(CIM_ERR_FAILED, e.getContentLanguages(), e.getMessage()); // l10n
    }
    catch(...)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Exception: Unknown");

        handler.setStatus(CIM_ERR_FAILED, "Unknown Error");
    }

    PEG_METHOD_EXIT();

    return(response);
}

Message * DefaultProviderManager::handleDeleteSubscriptionRequest(const Message * message) throw()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "DefaultProviderManager::handleDeleteSubscriptionRequest");

    CIMDeleteSubscriptionRequestMessage * request =
        dynamic_cast<CIMDeleteSubscriptionRequestMessage *>(const_cast<Message *>(message));

    PEGASUS_ASSERT(request != 0);

    CIMDeleteSubscriptionResponseMessage * response =
        new CIMDeleteSubscriptionResponseMessage(
        request->messageId,
        CIMException(),
        request->queueIds.copyAndPop());

    PEGASUS_ASSERT(response != 0);

    // preserve message key
    response->setKey(request->getKey());

    //  Set HTTP method in response from request
    response->setHttpMethod(request->getHttpMethod());

    OperationResponseHandler handler(request, response);

    try
    {
        String temp;

        for(Uint32 i = 0, n = request->classNames.size(); i < n; i++)
        {
            temp.append(request->classNames[i].getString());

            if(i == (n - 1))
            {
                temp.append(", ");
            }
        }

        Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            "DefaultProviderManager::handleDeleteSubscriptionRequest - Host name: $0  Name space: $1  Class name(s): $2",
            System::getHostName(),
            request->nameSpace.getString(),
            temp);

        // NOTE: the provider and provider module name are embedded in the request, presumably to reduce
        // the number of provider lookups (but this should be trivial if the data is cached and accessed
        // from the same source). to keep the lookups consistent, use the first class name, combined
        // with the other information in the request, and create an object path that can be used to
        // resolve the provider using the ProviderRegistrar.

        // make target object path
        CIMObjectPath objectPath(
            System::getHostName(),
            request->nameSpace,
            request->classNames[0].getString());

        ProviderName name(
            objectPath.toString(),
            String::EMPTY,
            String::EMPTY,
            String::EMPTY,
            0);

        // resolve provider name
        name = _resolveProviderName(name);

        // get cached or load new provider module
        OpProviderHolder ph =
            providerManager.getProvider(name.getPhysicalName(), name.getLogicalName(), String::EMPTY);

        // convert arguments
        OperationContext context;

        context.insert(IdentityContainer(request->userName));
        context.insert(SubscriptionInstanceContainer
            (request->subscriptionInstance));
        context.insert(SubscriptionLanguageListContainer
            (request->acceptLanguages));
        context.insert(AcceptLanguageListContainer(request->acceptLanguages));
        context.insert(ContentLanguageListContainer(request->contentLanguages));

        CIMObjectPath subscriptionName = request->subscriptionInstance.getPath();

        Array<CIMObjectPath> classNames;

        for(Uint32 i = 0, n = request->classNames.size(); i < n; i++)
        {
            CIMObjectPath className(
                System::getHostName(),
                request->nameSpace,
                request->classNames[i]);

            classNames.append(className);
        }

        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Calling provider.deleteSubscription: " +
            ph.GetProvider().getName());

        pm_service_op_lock op_lock(&ph.GetProvider());

        ph.GetProvider().deleteSubscription(
            context,
            subscriptionName,
            classNames);
    }
    catch(CIMException & e)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Exception: " + e.getMessage());

        handler.setStatus(e.getCode(), e.getContentLanguages(), e.getMessage()); // l10n
    }
    catch(Exception & e)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Exception: " + e.getMessage());

        handler.setStatus(CIM_ERR_FAILED, e.getContentLanguages(), e.getMessage()); // l10n
    }
    catch(...)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Exception: Unknown");

        handler.setStatus(CIM_ERR_FAILED, "Unknown Error");
    }

    PEG_METHOD_EXIT();

    return(response);
}

Message * DefaultProviderManager::handleEnableIndicationsRequest(const Message * message) throw()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "DefaultProviderManager:: handleEnableIndicationsRequest");

    CIMEnableIndicationsRequestMessage * request =
        dynamic_cast<CIMEnableIndicationsRequestMessage *>(const_cast<Message *>(message));

    PEGASUS_ASSERT(request != 0);

    CIMEnableIndicationsResponseMessage * response =
        new CIMEnableIndicationsResponseMessage(
        request->messageId,
        CIMException(),
        request->queueIds.copyAndPop());

    CIMEnableIndicationsResponseMessage * responseforhandler =
        new CIMEnableIndicationsResponseMessage(
        request->messageId,
        CIMException(),
        request->queueIds.copyAndPop());

    PEGASUS_ASSERT(response != 0);

    // preserve message key
    response->setKey(request->getKey());

    //  Set HTTP method in response from request
    response->setHttpMethod(request->getHttpMethod());

    response->dest = request->queueIds.top();

    /*
    // ATTN: need pointer to Provider Manager Server!
    //EnableIndicationsResponseHandler *handler =
    //    new EnableIndicationsResponseHandler(request, response, this);

    try
    {
        // get the provider file name and logical name
        Triad<String, String, String> triad =
            _getProviderRegPair(request->provider, request->providerModule);

        // get cached or load new provider module
        //Provider provider =
        OpProviderHolder ph =
            providerManager.getProvider(triad.first, triad.second, triad.third);

        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Calling provider.enableIndications: " +
            ph.GetProvider().getName());
        ph.GetProvider().protect();
        ph.GetProvider().enableIndications(*handler);


        // if no exception, store the handler so it is persistent for as
        // long as the provider has indications enabled.
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Storing indication handler for " + ph.GetProvider().getName());

        _insertEntry(ph.GetProvider(), handler);
    }
    catch(CIMException & e)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Exception: " + e.getMessage());
        response->cimException = CIMException(e);
    }
    catch(Exception & e)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Exception: " + e.getMessage());
        //l10n
        //response->cimException = CIMException(CIM_ERR_FAILED, "Internal Error");
        response->cimException = CIMException(CIM_ERR_FAILED, MessageLoaderParms(
            "ProviderManager.DefaultProviderManager.INTERNAL_ERROR",
            "Internal Error"));
    }
    catch(...)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Exception: Unknown");
        //l10n
        //response->cimException = CIMException(CIM_ERR_FAILED, "Unknown Error");
        response->cimException = CIMException(CIM_ERR_FAILED, MessageLoaderParms(
            "ProviderManager.DefaultProviderManager.UNKNOWN_ERROR",
            "Unknown Error"));
    }
    */

    PEG_METHOD_EXIT();

    return(response);
}

Message * DefaultProviderManager::handleDisableIndicationsRequest(const Message * message) throw()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "DefaultProviderManager::handleDisableIndicationsRequest");

    CIMDisableIndicationsRequestMessage * request =
        dynamic_cast<CIMDisableIndicationsRequestMessage *>(const_cast<Message *>(message));

    PEGASUS_ASSERT(request != 0);

    CIMDisableIndicationsResponseMessage * response =
        new CIMDisableIndicationsResponseMessage(
        request->messageId,
        CIMException(),
        request->queueIds.copyAndPop());

    PEGASUS_ASSERT(response != 0);

    // preserve message key
    response->setKey(request->getKey());

    //  Set HTTP method in response from request
    response->setHttpMethod (request->getHttpMethod ());

    OperationResponseHandler handler(request, response);

    try
    {
        ProviderName name(
            String::EMPTY,
            String::EMPTY,
            String::EMPTY,
            String::EMPTY,
            0);
        /*
        ProviderName name(
            String::EMPTY,
            String::EMPTY,
            objectPath.toString());

        // resolve provider name
        name = _resolveProviderName(name);
        */

        // get cached or load new provider module
        OpProviderHolder ph =
            providerManager.getProvider(name.getPhysicalName(), name.getLogicalName(), String::EMPTY);

        /*
        // get the provider file name and logical name
        Triad<String, String, String> triad =
            getProviderRegistrar()->_getProviderRegPair(request->provider, request->providerModule);

        // get cached or load new provider module
        //Provider provider =
        OpProviderHolder ph =
            providerManager.getProvider(triad.first, triad.second, triad.third);
        */

        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Calling provider.disableIndications: " +
            ph.GetProvider().getName());

        ph.GetProvider().disableIndications();

        ph.GetProvider().unprotect();

        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Removing and Destroying indication handler for " +
            ph.GetProvider().getName());

        delete _removeEntry(_generateKey(ph.GetProvider()));
    }

    catch(CIMException & e)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Exception: " + e.getMessage());

        response->cimException = CIMException(e);
    }
    catch(Exception & e)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Exception: " + e.getMessage());
            response->cimException = CIMException(CIM_ERR_FAILED, MessageLoaderParms(
            "ProviderManager.DefaultProviderManager.INTERNAL_ERROR",
            "Internal Error"));
    }
    catch(...)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Exception: Unknown");
            response->cimException = CIMException(CIM_ERR_FAILED, MessageLoaderParms(
            "ProviderManager.DefaultProviderManager.UNKNOWN_ERROR",
            "Unknown Error"));
    }

    PEG_METHOD_EXIT();

    return(response);
}

Message * DefaultProviderManager::handleConsumeIndicationRequest(const Message *message) throw()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "DefaultProviderManager::handlConsumeIndicationRequest");

    CIMConsumeIndicationRequestMessage *request =
        dynamic_cast<CIMConsumeIndicationRequestMessage *>(const_cast<Message *>(message));

    PEGASUS_ASSERT(request != 0);

    CIMResponseMessage * response =
        new CIMResponseMessage(
        CIM_CONSUME_INDICATION_RESPONSE_MESSAGE,
        request->messageId,
        CIMException(),
        request->queueIds.copyAndPop());

    PEGASUS_ASSERT(response != 0);

    response->setKey(request->getKey());

    //  Set HTTP method in response from request
    response->setHttpMethod (request->getHttpMethod ());

    Uint32 type = 3;

    try
    {
        ProviderName name(
            String::EMPTY,
            String::EMPTY,
            String::EMPTY,
            String::EMPTY,
            0);

        /*
        ProviderName name(
            String::EMPTY,
            String::EMPTY,
            objectPath.toString());

        // resolve provider name
        name = _resolveProviderName(name);
        */

        // get cached or load new provider module
        OpProviderHolder ph =
            providerManager.getProvider(name.getPhysicalName(), name.getLogicalName(), String::EMPTY);

        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Calling provider.: " +
            ph.GetProvider().getName());

        OperationContext context;

        //l10n
        // ATTN-CEC 06/04/03 NOTE: I can't find where the consume msg is sent.  This
        // does not appear to be hooked-up.  When it is added, need to
        // make sure that Content-Language is set in the consume msg.
        // NOTE: A-L is not needed to be set in the consume msg.
        // add the langs to the context
        context.insert(ContentLanguageListContainer(request->contentLanguages));

        CIMInstance indication_copy = request->indicationInstance;

        SimpleIndicationResponseHandler handler;

        ph.GetProvider().consumeIndication(context,
            "",
            indication_copy);
    }
    catch(CIMException & e)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Exception: " + e.getMessage());

        response->cimException = CIMException(e);
    }
    catch(Exception & e)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Exception: " + e.getMessage());
        //l10n
        //response->cimException = CIMException(CIM_ERR_FAILED, "Internal Error");
        response->cimException = CIMException(CIM_ERR_FAILED, MessageLoaderParms(
            "ProviderManager.DefaultProviderManager.INTERNAL_ERROR",
            "Internal Error"));
    }
    catch(...)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Exception: Unknown");
        //l10n
        //response->cimException = CIMException(CIM_ERR_FAILED, "Unknown Error");
        response->cimException = CIMException(CIM_ERR_FAILED, MessageLoaderParms(
            "ProviderManager.DefaultProviderManager.UNKNOWN_ERROR",
            "Unknown Error"));
    }

    PEG_METHOD_EXIT();

    return(response);
}

//
// This function disables a provider module if disableProviderOnly is not true,
// otherwise, disables a provider. Disable provider module means that
// block all the providers which contain in the module and unload the
// providers.
// Disable provider means unload the provider and the provider is not blocked.
//
// ATTN-YZ-P2-20030519: Provider needs to be blocked when disable a provider.
//
Message * DefaultProviderManager::handleDisableModuleRequest(const Message * message) throw()
{
    // HACK
    ProviderRegistrationManager * _providerRegistrationManager = GetProviderRegistrationManager();

    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "DefaultProviderManager::handleDisableModuleRequest");

    CIMDisableModuleRequestMessage * request =
        dynamic_cast<CIMDisableModuleRequestMessage *>(const_cast<Message *>(message));

    PEGASUS_ASSERT(request != 0);

    // get provider module name
    String moduleName;
    CIMInstance mInstance = request->providerModule;
    Uint32 pos = mInstance.findProperty(CIMName ("Name"));

    if(pos != PEG_NOT_FOUND)
    {
        mInstance.getProperty(pos).getValue().get(moduleName);
    }

    Boolean disableProviderOnly = request->disableProviderOnly;

    //
    // get operational status
    //
    Array<Uint16> operationalStatus;

    if(!disableProviderOnly)
    {
        Uint32 pos2 = mInstance.findProperty(CIMName ("OperationalStatus"));

        if(pos2 != PEG_NOT_FOUND)
        {
            //
            //  ATTN-CAKG-P2-20020821: Check for null status?
            //
            mInstance.getProperty(pos2).getValue().get(operationalStatus);
        }

        //
        // update module status from OK to Stopping
        //
        for(Uint32 i=0, n = operationalStatus.size(); i < n; i++)
        {
            if(operationalStatus[i] == _MODULE_OK)
            {
                operationalStatus.remove(i);
            }
        }
        operationalStatus.append(_MODULE_STOPPING);

        if(_providerRegistrationManager->setProviderModuleStatus
            (moduleName, operationalStatus) == false)
        {
            //l10n
            //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "set module status failed.");
            throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED, MessageLoaderParms(
                "ProviderManager.DefaultProviderManager.SET_MODULE_STATUS_FAILED",
                "set module status failed."));
        }
    }

    // Unload providers
    Array<CIMInstance> _pInstances = request->providers;

    for(Uint32 i = 0, n = _pInstances.size(); i < n; i++)
    {
        /* temp disabled by Chip
        // get the provider file name and logical name
        Triad<String, String, String> triad =
            getProviderRegistrar()->_getProviderRegPair(_pInstances[i], mInstance);

        providerManager.unloadProvider(triad.first, triad.second);
        */
    }

    if(!disableProviderOnly)
    {
        // update module status from Stopping to Stopped
        for(Uint32 i=0, n = operationalStatus.size(); i < n; i++)
        {
            if(operationalStatus[i] == _MODULE_STOPPING)
            {
                operationalStatus.remove(i);
            }
        }

        operationalStatus.append(_MODULE_STOPPED);

        if(_providerRegistrationManager->setProviderModuleStatus
            (moduleName, operationalStatus) == false)
        {
            //l10n
            //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
            //"set module status failed.");
            throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED, MessageLoaderParms(
                "ProviderManager.DefaultProviderManager.SET_MODULE_STATUS_FAILED",
                "set module status failed."));
        }
    }

    CIMDisableModuleResponseMessage * response =
        new CIMDisableModuleResponseMessage(
        request->messageId,
        CIMException(),
        request->queueIds.copyAndPop(),
        operationalStatus);

    PEGASUS_ASSERT(response != 0);

    // preserve message key
    response->setKey(request->getKey());

    //
    //  Set HTTP method in response from request
    //
    response->setHttpMethod (request->getHttpMethod ());

    PEG_METHOD_EXIT();

    return(response);
}

Message * DefaultProviderManager::handleEnableModuleRequest(const Message * message) throw()
{
    // HACK
    ProviderRegistrationManager * _providerRegistrationManager = GetProviderRegistrationManager();

    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "DefaultProviderManager::handleEnableModuleRequest");

    CIMEnableModuleRequestMessage * request =
        dynamic_cast<CIMEnableModuleRequestMessage *>(const_cast<Message *>(message));

    PEGASUS_ASSERT(request != 0);

    //
    // get module status
    //
    CIMInstance mInstance = request->providerModule;
    Array<Uint16> operationalStatus;
    Uint32 pos = mInstance.findProperty(CIMName ("OperationalStatus"));

    if(pos != PEG_NOT_FOUND)
    {
        //
        //  ATTN-CAKG-P2-20020821: Check for null status?
        //
        mInstance.getProperty(pos).getValue().get(operationalStatus);
    }

    // update module status from Stopped to OK
    for(Uint32 i=0, n = operationalStatus.size(); i < n; i++)
    {
        if(operationalStatus[i] == _MODULE_STOPPED)
        {
            operationalStatus.remove(i);
        }
    }

    operationalStatus.append(_MODULE_OK);

    //
    // get module name
    //
    String moduleName;

    Uint32 pos2 = mInstance.findProperty(CIMName ("Name"));

    if(pos2 != PEG_NOT_FOUND)
    {
        mInstance.getProperty(pos2).getValue().get(moduleName);
    }

    if(_providerRegistrationManager->setProviderModuleStatus
        (moduleName, operationalStatus) == false)
    {
        //l10n
        //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "set module status failed.");
        throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED, MessageLoaderParms(
            "ProviderManager.DefaultProviderManager.SET_MODULE_STATUS_FAILED",
            "set module status failed."));
    }

    CIMEnableModuleResponseMessage * response =
        new CIMEnableModuleResponseMessage(
        request->messageId,
        CIMException(),
        request->queueIds.copyAndPop(),
        operationalStatus);

    PEGASUS_ASSERT(response != 0);

    // preserve message key
    response->setKey(request->getKey());

    //  Set HTTP method in response from request
    response->setHttpMethod (request->getHttpMethod ());

    PEG_METHOD_EXIT();

    return(response);
}

Message * DefaultProviderManager::handleStopAllProvidersRequest(const Message * message) throw()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "DefaultProviderManager::handleStopAllProvidersRequest");

    CIMStopAllProvidersRequestMessage * request =
        dynamic_cast<CIMStopAllProvidersRequestMessage *>(const_cast<Message *>(message));

    PEGASUS_ASSERT(request != 0);

    CIMStopAllProvidersResponseMessage * response =
        new CIMStopAllProvidersResponseMessage(
        request->messageId,
        CIMException(),
        request->queueIds.copyAndPop());

    PEGASUS_ASSERT(response != 0);

    // preserve message key
    response->setKey(request->getKey());

    //  Set HTTP method in response from request
    response->setHttpMethod (request->getHttpMethod ());

    // tell the provider manager to shutdown all the providers
    providerManager.shutdownAllProviders();

    PEG_METHOD_EXIT();

    return(response);
}

void DefaultProviderManager::_insertEntry (
    const Provider & provider,
    const EnableIndicationsResponseHandler *handler)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "DefaultProviderManager::_insertEntry");

    String tableKey = _generateKey
        (provider);

    _responseTable.insert (tableKey, const_cast<EnableIndicationsResponseHandler *>(handler));

    PEG_METHOD_EXIT();
}

EnableIndicationsResponseHandler * DefaultProviderManager::_removeEntry(
    const String & key)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "DefaultProviderManager::_removeEntry");
    EnableIndicationsResponseHandler *ret = 0;

    _responseTable.lookup(key, ret);

    PEG_METHOD_EXIT();

    return(ret);
}

String DefaultProviderManager::_generateKey (
    const Provider & provider)
{
    String tableKey;

    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
        "DefaultProviderManager::_generateKey");

    //
    //  Append provider key values to key
    //
    String providerName = provider.getName();
    String providerFileName = provider.getModule()->getFileName();
    tableKey.append (providerName);
    tableKey.append (providerFileName);

    PEG_METHOD_EXIT();

    return(tableKey);
}

ProviderName DefaultProviderManager::_resolveProviderName(const ProviderName & providerName)
{
    ProviderName temp = findProvider(providerName);

    String physicalName = temp.getPhysicalName();

    // fully qualify physical provider name (module), if not already done so.
    #if defined(PEGASUS_PLATFORM_WIN32_IX86_MSVC)
    physicalName = physicalName + String(".dll");
    #elif defined(PEGASUS_PLATFORM_LINUX_IX86_GNU) || defined(PEGASUS_PLATFORM_LINUX_IA86_GNU)
    String root = ConfigManager::getHomedPath(ConfigManager::getInstance()->getCurrentValue("providerDir"));
    physicalName = root + String("/lib") + physicalName + String(".so");
    #elif defined(PEGASUS_OS_HPUX)
    String root = ConfigManager::getHomedPath(ConfigManager::getInstance()->getCurrentValue("providerDir"));
    physicalName = root + String("/lib") + moduleLocation + String(".sl");
    #elif defined(PEGASUS_OS_OS400)
    // do nothing
    #else
    String root = ConfigManager::getHomedPath(ConfigManager::getInstance()->getCurrentValue("providerDir"));
    physicalName = root + String("/lib") + physicalName + String(".so");

    #endif

    temp.setPhysicalName(physicalName);

    return(temp);
}

PEGASUS_NAMESPACE_END
