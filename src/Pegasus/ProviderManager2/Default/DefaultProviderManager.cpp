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
//              Yi Zhou, Hewlett-Packard Company (yi_zhou@hp.com)
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//				Seema Gupta (gseema@in.ibm.com) for PEP135
//				Willis White (whiwill@us.ibm.com)
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
#include <Pegasus/Common/Constants.h>

#include <Pegasus/Common/QueryExpression.h>
#include <Pegasus/ProviderManager2/QueryExpressionFactory.h>

#include <Pegasus/ProviderManager2/Default/Provider.h>
#include <Pegasus/ProviderManager2/OperationResponseHandler.h>

#include <Pegasus/ProviderManager2/ProviderManagerService.h>
#include <Pegasus/ProviderManager2/ProviderType.h>

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
// Default Provider Manager
//
DefaultProviderManager::DefaultProviderManager(void)
{
}

DefaultProviderManager::~DefaultProviderManager(void)
{
}

Message * DefaultProviderManager::processMessage(Message * request)
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
        response = handleExecQueryRequest(request);

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
    case CIM_EXPORT_INDICATION_REQUEST_MESSAGE:
        response = handleExportIndicationRequest(request);
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
    case CIM_INITIALIZE_PROVIDER_REQUEST_MESSAGE:
	response = handleInitializeProviderRequest(request);

	break;
    default:
        response = handleUnsupportedRequest(request);

        break;
    }

    PEG_METHOD_EXIT();

    return(response);
}

Message * DefaultProviderManager::handleUnsupportedRequest(const Message * message)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "DefaultProviderManager::handleUnsupportedRequest");

    PEG_METHOD_EXIT();

    // a null response implies unsupported or unknown operation
    return(0);
}

Message * DefaultProviderManager::handleInitializeProviderRequest(
    const Message * message)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, 
	"DefaultProviderManager::handleInitializeProviderRequest");

    CIMInitializeProviderRequestMessage * request =
        dynamic_cast<CIMInitializeProviderRequestMessage *>
	    (const_cast<Message *>(message));

    PEGASUS_ASSERT(request != 0);

    CIMInitializeProviderResponseMessage * response =
        new CIMInitializeProviderResponseMessage(
        request->messageId,
        CIMException(),
        request->queueIds.copyAndPop());

    // preserve message key
    response->setKey(request->getKey());

    //  Set HTTP method in response from request
    response->setHttpMethod(request->getHttpMethod());

    OperationResponseHandler handler(request, response);

    try
    {
        // resolve provider name
	ProviderName name = _resolveProviderName(
	    request->operationContext.get(ProviderIdContainer::NAME));

        // get cached or load new provider module
        OpProviderHolder ph =
            providerManager.getProvider(name.getPhysicalName(), 
		name.getLogicalName(), String::EMPTY);

    }
    catch(CIMException & e)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL3,
            "CIMException: " + e.getMessage());

        handler.setStatus(e.getCode(), e.getContentLanguages(), e.getMessage());
    }
    catch(Exception & e)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL3,
            "Exception: " + e.getMessage());

        handler.setStatus(CIM_ERR_FAILED, e.getContentLanguages(), e.getMessage());
    }
    catch(...)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL3,
            "Exception: Unknown");

        handler.setStatus(CIM_ERR_FAILED, "Unknown error.");
    }

    PEG_METHOD_EXIT();

    return(response);
}

Message * DefaultProviderManager::handleGetInstanceRequest(const Message * message)
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

        // resolve provider name
        ProviderName name = _resolveProviderName(
            request->operationContext.get(ProviderIdContainer::NAME));

        // get cached or load new provider module
        OpProviderHolder ph =
            providerManager.getProvider(name.getPhysicalName(), name.getLogicalName(), String::EMPTY);

        // convert arguments
        OperationContext context;

		context.insert(request->operationContext.get(IdentityContainer::NAME));
		context.insert(request->operationContext.get(AcceptLanguageListContainer::NAME)); 
	    context.insert(request->operationContext.get(ContentLanguageListContainer::NAME)); 

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

Message * DefaultProviderManager::handleEnumerateInstancesRequest(const Message * message)
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

        // resolve provider name
        ProviderName name = _resolveProviderName(
            request->operationContext.get(ProviderIdContainer::NAME));

        // get cached or load new provider module
        OpProviderHolder ph = providerManager.getProvider(name.getPhysicalName(),
                name.getLogicalName(), String::EMPTY);

        // convert arguments
        OperationContext context;

		context.insert(request->operationContext.get(IdentityContainer::NAME));
		context.insert(request->operationContext.get(AcceptLanguageListContainer::NAME)); 
	    context.insert(request->operationContext.get(ContentLanguageListContainer::NAME)); 

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

Message * DefaultProviderManager::handleEnumerateInstanceNamesRequest(const Message * message)
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

        // resolve provider name
        ProviderName name = _resolveProviderName(
            request->operationContext.get(ProviderIdContainer::NAME));

        // get cached or load new provider module
        OpProviderHolder ph =
            providerManager.getProvider(name.getPhysicalName(), name.getLogicalName());

        // convert arguments
        OperationContext context;

		context.insert(request->operationContext.get(IdentityContainer::NAME));
		context.insert(request->operationContext.get(AcceptLanguageListContainer::NAME)); 
	    context.insert(request->operationContext.get(ContentLanguageListContainer::NAME)); 

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

Message * DefaultProviderManager::handleCreateInstanceRequest(const Message * message)
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

        // resolve provider name
        ProviderName name = _resolveProviderName(
            request->operationContext.get(ProviderIdContainer::NAME));

        // get cached or load new provider module
        OpProviderHolder ph =
            providerManager.getProvider(name.getPhysicalName(), name.getLogicalName(), String::EMPTY);

        // convert arguments
        OperationContext context;

		context.insert(request->operationContext.get(IdentityContainer::NAME));
		context.insert(request->operationContext.get(AcceptLanguageListContainer::NAME)); 
	    context.insert(request->operationContext.get(ContentLanguageListContainer::NAME)); 

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

Message * DefaultProviderManager::handleModifyInstanceRequest(const Message * message)
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

        // resolve provider name
        ProviderName name = _resolveProviderName(
            request->operationContext.get(ProviderIdContainer::NAME));

        // get cached or load new provider module
        OpProviderHolder ph =
            providerManager.getProvider(name.getPhysicalName(), name.getLogicalName(), String::EMPTY);

        // convert arguments
        OperationContext context;

		context.insert(request->operationContext.get(IdentityContainer::NAME));
		context.insert(request->operationContext.get(AcceptLanguageListContainer::NAME)); 
	    context.insert(request->operationContext.get(ContentLanguageListContainer::NAME)); 

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

Message * DefaultProviderManager::handleDeleteInstanceRequest(const Message * message)
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

        // resolve provider name
        ProviderName name = _resolveProviderName(
            request->operationContext.get(ProviderIdContainer::NAME));

        // get cached or load new provider module
        OpProviderHolder ph =
            providerManager.getProvider(name.getPhysicalName(), name.getLogicalName(), String::EMPTY);

        // convert arguments
        OperationContext context;

		context.insert(request->operationContext.get(IdentityContainer::NAME));
		context.insert(request->operationContext.get(AcceptLanguageListContainer::NAME)); 
	    context.insert(request->operationContext.get(ContentLanguageListContainer::NAME)); 

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

Message * DefaultProviderManager::handleExecQueryRequest(const Message * message)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "DefaultProviderManager::handleExecQueryRequest");

    CIMExecQueryRequestMessage * request =
        dynamic_cast<CIMExecQueryRequestMessage *>(const_cast<Message *>(message));

    PEGASUS_ASSERT(request != 0);

    CIMExecQueryResponseMessage * response =
        new CIMExecQueryResponseMessage(
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
    ExecQueryResponseHandler handler(request, response);

    try
    {
        Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            "DefaultProviderManager::handleExecQueryRequest - Host name: $0  Name space: $1  Class name: $2",
            System::getHostName(),
            request->nameSpace.getString(),
            request->className.getString());

        // make target object path
        CIMObjectPath objectPath(
            System::getHostName(),
            request->nameSpace,
            request->className);

        // resolve provider name
        ProviderName name = _resolveProviderName(
            request->operationContext.get(ProviderIdContainer::NAME));

        // get cached or load new provider module
        OpProviderHolder ph =
            providerManager.getProvider(name.getPhysicalName(),
                                        name.getLogicalName(), String::EMPTY);

        if (dynamic_cast<CIMInstanceQueryProvider*>(ph.GetCIMProvider()) == 0) {
           String errorString = " instance provider is registered supporting execQuery "
                                "but is not a CIMQueryInstanceProvider subclass.";
           throw CIMException(CIM_ERR_FAILED,"ProviderLoadFailure (" + name.getPhysicalName() + ":" +
                            name.getLogicalName() + "):" + errorString);
        }

        // convert arguments
        OperationContext context;

		context.insert(request->operationContext.get(IdentityContainer::NAME));
		context.insert(request->operationContext.get(AcceptLanguageListContainer::NAME)); 
	    context.insert(request->operationContext.get(ContentLanguageListContainer::NAME)); 

        QueryExpression qx(QueryExpressionFactory::routeBuildQueryExpressionRep
           (request->queryLanguage,request->query));

        // forward request
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Calling provider.executeQueryRequest: " +
            ph.GetProvider().getName());

        pm_service_op_lock op_lock(&ph.GetProvider());

        STAT_GETSTARTTIME;

        ph.GetProvider().execQuery(
            context,
            objectPath,
            qx,
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

Message * DefaultProviderManager::handleAssociatorsRequest(const Message * message)
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

        CIMObjectPath assocPath(
            System::getHostName(),
            request->nameSpace,
            request->assocClass.getString());

        // resolve provider name
        ProviderName name = _resolveProviderName(
            request->operationContext.get(ProviderIdContainer::NAME));

        // get cached or load new provider module
        OpProviderHolder ph =
            providerManager.getProvider(name.getPhysicalName(), name.getLogicalName(), String::EMPTY);

        // convert arguments
        OperationContext context;

		context.insert(request->operationContext.get(IdentityContainer::NAME));
		context.insert(request->operationContext.get(AcceptLanguageListContainer::NAME)); 
	    context.insert(request->operationContext.get(ContentLanguageListContainer::NAME)); 

        STAT_GETSTARTTIME;
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

Message * DefaultProviderManager::handleAssociatorNamesRequest(const Message * message)
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

        CIMObjectPath assocPath(
            System::getHostName(),
            request->nameSpace,
            request->assocClass.getString());

        // resolve provider name
        ProviderName name = _resolveProviderName(
            request->operationContext.get(ProviderIdContainer::NAME));

        // get cached or load new provider module
        OpProviderHolder ph =
            providerManager.getProvider(name.getPhysicalName(), name.getLogicalName(), String::EMPTY);

        // convert arguments
        OperationContext context;

		context.insert(request->operationContext.get(IdentityContainer::NAME));
		context.insert(request->operationContext.get(AcceptLanguageListContainer::NAME)); 
	    context.insert(request->operationContext.get(ContentLanguageListContainer::NAME)); 

		STAT_GETSTARTTIME;
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

Message * DefaultProviderManager::handleReferencesRequest(const Message * message)
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

        CIMObjectPath resultPath(
            System::getHostName(),
            request->nameSpace,
            request->resultClass.getString());

        // resolve provider name
        ProviderName name = _resolveProviderName(
            request->operationContext.get(ProviderIdContainer::NAME));

        // get cached or load new provider module
        OpProviderHolder ph =
            providerManager.getProvider(name.getPhysicalName(), name.getLogicalName(), String::EMPTY);

        // convert arguments
        OperationContext context;

		context.insert(request->operationContext.get(IdentityContainer::NAME));
		context.insert(request->operationContext.get(AcceptLanguageListContainer::NAME)); 
	    context.insert(request->operationContext.get(ContentLanguageListContainer::NAME)); 

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

Message * DefaultProviderManager::handleReferenceNamesRequest(const Message * message)
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

        CIMObjectPath resultPath(
            System::getHostName(),
            request->nameSpace,
            request->resultClass.getString());

        // resolve provider name
        ProviderName name = _resolveProviderName(
            request->operationContext.get(ProviderIdContainer::NAME));

        // get cached or load new provider module
        OpProviderHolder ph =
            providerManager.getProvider(name.getPhysicalName(), name.getLogicalName(), String::EMPTY);

        // convert arguments
        OperationContext context;

		context.insert(request->operationContext.get(IdentityContainer::NAME));
		context.insert(request->operationContext.get(AcceptLanguageListContainer::NAME)); 
	    context.insert(request->operationContext.get(ContentLanguageListContainer::NAME)); 

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

Message * DefaultProviderManager::handleGetPropertyRequest(const Message * message)
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

        // resolve provider name
        ProviderName name = _resolveProviderName(
            request->operationContext.get(ProviderIdContainer::NAME));

        // get cached or load new provider module
        OpProviderHolder ph =
            providerManager.getProvider(name.getPhysicalName(), name.getLogicalName(), String::EMPTY);

        // convert arguments
        OperationContext context;

		context.insert(request->operationContext.get(IdentityContainer::NAME));
		context.insert(request->operationContext.get(AcceptLanguageListContainer::NAME)); 
	    context.insert(request->operationContext.get(ContentLanguageListContainer::NAME)); 

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

Message * DefaultProviderManager::handleSetPropertyRequest(const Message * message)
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

        // resolve provider name
        ProviderName name = _resolveProviderName(
            request->operationContext.get(ProviderIdContainer::NAME));

        // get cached or load new provider module
        OpProviderHolder ph =
            providerManager.getProvider(name.getPhysicalName(), name.getLogicalName(), String::EMPTY);

        // convert arguments
        OperationContext context;

		context.insert(request->operationContext.get(IdentityContainer::NAME));
		context.insert(request->operationContext.get(AcceptLanguageListContainer::NAME)); 
	    context.insert(request->operationContext.get(ContentLanguageListContainer::NAME)); 

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

Message * DefaultProviderManager::handleInvokeMethodRequest(const Message * message)
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

        // resolve provider name
        ProviderName name = _resolveProviderName(
            request->operationContext.get(ProviderIdContainer::NAME));

        // get cached or load new provider module
        OpProviderHolder ph =
            providerManager.getProvider(name.getPhysicalName(), name.getLogicalName(), String::EMPTY);

        // convert arguments
        OperationContext context;

		context.insert(request->operationContext.get(IdentityContainer::NAME));
		context.insert(request->operationContext.get(AcceptLanguageListContainer::NAME)); 
	    context.insert(request->operationContext.get(ContentLanguageListContainer::NAME));  

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

Message * DefaultProviderManager::handleCreateSubscriptionRequest(const Message * message)
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

		CIMInstance req_provider, req_providerModule;
		ProviderIdContainer pidc = (ProviderIdContainer)request->operationContext.get(ProviderIdContainer::NAME);
		req_provider = pidc.getProvider();
		req_providerModule = pidc.getModule();

        String physicalName=_resolvePhysicalName( req_providerModule.getProperty(
                                                  req_providerModule.findProperty("Location")).getValue().toString());

        ProviderName name(req_provider.getProperty(req_provider.findProperty("Name")).getValue ().toString (),
                                  physicalName,
                                  req_providerModule.getProperty(req_providerModule.findProperty
                                  ("InterfaceType")).getValue().toString(),
                                   0);
		// get cached or load new provider module
        OpProviderHolder ph =
            providerManager.getProvider(name.getPhysicalName(), name.getLogicalName(), String::EMPTY);

        // convert arguments
        OperationContext context;

		context.insert(request->operationContext.get(IdentityContainer::NAME));
		context.insert(request->operationContext.get(AcceptLanguageListContainer::NAME)); 
	    context.insert(request->operationContext.get(ContentLanguageListContainer::NAME)); 
	    context.insert(request->operationContext.get(SubscriptionInstanceContainer::NAME));
	    context.insert(request->operationContext.get(SubscriptionLanguageListContainer::NAME));
	    context.insert(request->operationContext.get(SubscriptionFilterConditionContainer::NAME));

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

Message * DefaultProviderManager::handleModifySubscriptionRequest( const Message * message)
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
            "DefaultProviderManager::handleCreateSubscriptionRequest - Host name: $0  Name space: $1  Class name(s): $2",
            System::getHostName(),
            request->nameSpace.getString(),
            temp);
			
		CIMInstance req_provider, req_providerModule;
		ProviderIdContainer pidc = (ProviderIdContainer)request->operationContext.get(ProviderIdContainer::NAME);
		req_provider = pidc.getProvider();
		req_providerModule = pidc.getModule();

        String physicalName=_resolvePhysicalName( req_providerModule.getProperty(
                                                  req_providerModule.findProperty("Location")).getValue().toString());

        ProviderName name(req_provider.getProperty(req_provider.findProperty("Name")).getValue ().toString (),
                                  physicalName,
                                  req_providerModule.getProperty(req_providerModule.findProperty
                                  ("InterfaceType")).getValue().toString(),
                                   0);

        // get cached or load new provider module
        OpProviderHolder ph =
            providerManager.getProvider(name.getPhysicalName(), name.getLogicalName(), String::EMPTY);

        // convert arguments
        OperationContext context;
		context.insert(request->operationContext.get(IdentityContainer::NAME));
		context.insert(request->operationContext.get(AcceptLanguageListContainer::NAME)); 
	    context.insert(request->operationContext.get(ContentLanguageListContainer::NAME)); 
	    context.insert(request->operationContext.get(SubscriptionInstanceContainer::NAME));
	    context.insert(request->operationContext.get(SubscriptionLanguageListContainer::NAME));
	    context.insert(request->operationContext.get(SubscriptionFilterConditionContainer::NAME));

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

Message * DefaultProviderManager::handleDeleteSubscriptionRequest(const Message * message)
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

		CIMInstance req_provider, req_providerModule;
		ProviderIdContainer pidc = (ProviderIdContainer)request->operationContext.get(ProviderIdContainer::NAME);

		req_provider = pidc.getProvider();
		req_providerModule = pidc.getModule();
        
              String physicalName=_resolvePhysicalName( req_providerModule.getProperty(
                                                  req_providerModule.findProperty("Location")).getValue().toString());

              ProviderName name(req_provider.getProperty(req_provider.findProperty("Name")).getValue ().toString (),
                                  physicalName,
                                  req_providerModule.getProperty(req_providerModule.findProperty
                                  ("InterfaceType")).getValue().toString(),
                                   0);

        // get cached or load new provider module
        OpProviderHolder ph =
            providerManager.getProvider(name.getPhysicalName(), name.getLogicalName(), String::EMPTY);

        // convert arguments
        OperationContext context;

		context.insert(request->operationContext.get(IdentityContainer::NAME));
		context.insert(request->operationContext.get(AcceptLanguageListContainer::NAME)); 
	    context.insert(request->operationContext.get(ContentLanguageListContainer::NAME)); 
	    context.insert(request->operationContext.get(SubscriptionInstanceContainer::NAME));
	    context.insert(request->operationContext.get(SubscriptionLanguageListContainer::NAME));

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

Message * DefaultProviderManager::handleEnableIndicationsRequest(const Message * message)
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

    PEGASUS_ASSERT(response != 0);

    // preserve message key
    response->setKey(request->getKey());

    //  Set HTTP method in response from request
    response->setHttpMethod(request->getHttpMethod());

    response->dest = request->queueIds.top();

	CIMInstance req_provider, req_providerModule;
	ProviderIdContainer pidc = (ProviderIdContainer)request->operationContext.get(ProviderIdContainer::NAME);

	req_provider = pidc.getProvider();
	req_providerModule = pidc.getModule();

    EnableIndicationsResponseHandler *handler =
        new EnableIndicationsResponseHandler(
            request, response, req_provider, _indicationCallback);



    try
    {
          String physicalName=_resolvePhysicalName( req_providerModule.getProperty(
                                                    req_providerModule.findProperty("Location")).getValue().toString());

          ProviderName name(req_provider.getProperty(req_provider.findProperty("Name")).getValue ().toString (),
                                  physicalName,
                                  req_providerModule.getProperty(req_providerModule.findProperty
                                  ("InterfaceType")).getValue().toString(),
                                   0);

        // get cached or load new provider module
        OpProviderHolder ph =
            providerManager.getProvider(name.getPhysicalName(), name.getLogicalName(), String::EMPTY);

        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Calling provider.enableIndications: " +
            ph.GetProvider().getName());

        pm_service_op_lock op_lock(&ph.GetProvider());
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

Message * DefaultProviderManager::handleDisableIndicationsRequest(const Message * message)
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

    // preserve message key
    response->setKey(request->getKey());

    //  Set HTTP method in response from request
    response->setHttpMethod (request->getHttpMethod ());

    OperationResponseHandler handler(request, response);

	CIMInstance req_provider, req_providerModule;
	ProviderIdContainer pidc = (ProviderIdContainer)request->operationContext.get(ProviderIdContainer::NAME);

	req_provider = pidc.getProvider();
	req_providerModule = pidc.getModule();
     
    try
    {
       String physicalName=_resolvePhysicalName(
              req_providerModule.getProperty(
                req_providerModule.findProperty("Location")).getValue().toString());

       ProviderName name(
               req_provider.getProperty(req_provider.findProperty
                   ("Name")).getValue ().toString (),
               physicalName,
                req_providerModule.getProperty(req_providerModule.findProperty
                    ("InterfaceType")).getValue().toString(),
            0);
        // get cached or load new provider module
        OpProviderHolder ph =
            providerManager.getProvider(name.getPhysicalName(), name.getLogicalName(), String::EMPTY);

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

Message *DefaultProviderManager::handleExportIndicationRequest(const Message *message)
{
   PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "DefaultProviderManagerService::handlExportIndicationRequest");

    CIMExportIndicationRequestMessage * request =
        dynamic_cast<CIMExportIndicationRequestMessage *>(const_cast<Message *>(message));

    PEGASUS_ASSERT(request != 0);

    CIMExportIndicationResponseMessage * response =
        new CIMExportIndicationResponseMessage(
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
        // resolve provider name
        ProviderName name = _resolveProviderName(
            request->operationContext.get(ProviderIdContainer::NAME));

        // get cached or load new provider module
        OpProviderHolder ph =
            providerManager.getProvider(name.getPhysicalName(), name.getLogicalName(), String::EMPTY);

        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                       "Calling provider.: " +
                       ph.GetProvider().getName());

        OperationContext context;

//L10N_TODO
//l10n
// ATTN-CEC 06/04/03 NOTE: I can't find where the consume msg is sent.  This
// does not appear to be hooked-up.  When it is added, need to
// make sure that Content-Language is set in the consume msg.
// NOTE: A-L is not needed to be set in the consume msg.
      // add the langs to the context

      context.insert(request->operationContext.get(ContentLanguageListContainer::NAME)); 

      CIMInstance indication_copy = request->indicationInstance;
      pm_service_op_lock op_lock(&ph.GetProvider());

      ph.GetProvider().consumeIndication(context,
                                request->destinationPath,
                                indication_copy);

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




//
// This function disables a provider module if disableProviderOnly is not true,
// otherwise, disables a provider. Disable provider module means that
// block all the providers which contain in the module and unload the
// providers.
// Disable provider means unload the provider and the provider is not blocked.
//
// ATTN-YZ-P2-20030519: Provider needs to be blocked when disable a provider.
//
Message * DefaultProviderManager::handleDisableModuleRequest(const Message * message)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "DefaultProviderManager::handleDisableModuleRequest");

    CIMDisableModuleRequestMessage * request =
        dynamic_cast<CIMDisableModuleRequestMessage *>(const_cast<Message *>(message));

    PEGASUS_ASSERT(request != 0);

    Array<Uint16> operationalStatus;
    CIMException cimException;

    try
    {
        // get provider module name
        String moduleName;
        CIMInstance mInstance = request->providerModule;
        Uint32 pos = mInstance.findProperty(CIMName ("Name"));
        PEGASUS_ASSERT(pos != PEG_NOT_FOUND);
        mInstance.getProperty(pos).getValue().get(moduleName);

        Boolean disableProviderOnly = request->disableProviderOnly;

        //
        // Unload providers
        //
        Array<CIMInstance> _pInstances = request->providers;
        Array<Boolean> _indicationProviders = request->indicationProviders;

        String physicalName=_resolvePhysicalName(
           mInstance.getProperty(
              mInstance.findProperty("Location")).getValue().toString());

        for(Uint32 i = 0, n = _pInstances.size(); i < n; i++)
        {
            String pName(_pInstances[i].getProperty(
               _pInstances[i].findProperty("Name")).getValue().toString());

            Sint16 ret_value = providerManager.disableProvider(physicalName,pName);

            if (ret_value == 0)
            {
                // disable failed since there are pending requests,
                // stop trying to disable other providers in this module.
                operationalStatus.append(CIM_MSE_OPSTATUS_VALUE_OK);
                break;
            }
            else if (ret_value == 1)  // Success
            {
                // if It is an indication provider
                // remove the entry from the table since the
                // provider has been disabled
                if (_indicationProviders[i])
                {
                    delete _removeEntry(_generateKey(pName,physicalName));
                }
            }
            else
            {
                // disable failed for other reason, throw exception
                throw PEGASUS_CIM_EXCEPTION_L(
                    CIM_ERR_FAILED,
                    MessageLoaderParms(
                        "ProviderManager.ProviderManagerService."
                            "DISABLE_PROVIDER_FAILED",
                        "Failed to disable the provider."));
            }
        }
    }
    catch(CIMException & e)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                         "Exception: " + e.getMessage());
        cimException = e;
    }
    catch(Exception & e)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                         "Exception: " + e.getMessage());
        cimException = CIMException(CIM_ERR_FAILED, e.getMessage());
    }
    catch(...)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                         "Exception: Unknown");
        //l10n
        //response->cimException = CIMException(CIM_ERR_FAILED, "Unknown Error");
        cimException = PEGASUS_CIM_EXCEPTION_L(
            CIM_ERR_FAILED,
            MessageLoaderParms(
                "ProviderManager.ProviderManagerService.UNKNOWN_ERROR",
                "Unknown Error"));
    }

    if (cimException.getCode() == CIM_ERR_SUCCESS)
    {
        // Status is set to OK if a provider was busy
        if (operationalStatus.size() == 0)
        {
            operationalStatus.append(CIM_MSE_OPSTATUS_VALUE_STOPPED);
        }
    }
    else
    {
        // If exception occurs, module is not stopped
        operationalStatus.append(CIM_MSE_OPSTATUS_VALUE_OK);
    }

    CIMDisableModuleResponseMessage * response =
        new CIMDisableModuleResponseMessage(
            request->messageId,
            CIMException(),
            request->queueIds.copyAndPop(),
            operationalStatus);

    // preserve message key
    response->setKey(request->getKey());

    //  Set HTTP method in response from request
    response->setHttpMethod (request->getHttpMethod ());

    PEG_METHOD_EXIT();

    return(response);
}

Message * DefaultProviderManager::handleEnableModuleRequest(const Message * message)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "DefaultProviderManager::handleEnableModuleRequest");

    CIMEnableModuleRequestMessage * request =
        dynamic_cast<CIMEnableModuleRequestMessage *>(const_cast<Message *>(message));

    PEGASUS_ASSERT(request != 0);

    Array<Uint16> operationalStatus;
    operationalStatus.append(CIM_MSE_OPSTATUS_VALUE_OK);

    CIMEnableModuleResponseMessage * response =
        new CIMEnableModuleResponseMessage(
            request->messageId,
            CIMException(),
            request->queueIds.copyAndPop(),
            operationalStatus);

    // preserve message key
    response->setKey(request->getKey());

    //  Set HTTP method in response from request
    response->setHttpMethod (request->getHttpMethod ());

    PEG_METHOD_EXIT();
    return(response);
}

Message * DefaultProviderManager::handleStopAllProvidersRequest(const Message * message)
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
    _responseTable.remove(key);         // why is this needed ? - we get killed when removed...

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

String DefaultProviderManager::_generateKey (
    const String & providerName,
    const String & providerFileName)
{
    String tableKey;

    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE,
                      "DefaultProviderManagerService::_generateKey");

    //
    //  Append providerName and providerFileName to key
    //
    tableKey.append (providerName);
    tableKey.append (providerFileName);

    PEG_METHOD_EXIT ();
    return tableKey;
}

ProviderName DefaultProviderManager::_resolveProviderName(
    const ProviderIdContainer & providerId)
{
    String providerName;
    String fileName;
    String interfaceName;
    CIMValue genericValue;

    genericValue = providerId.getProvider().getProperty(
        providerId.getProvider().findProperty("Name")).getValue();
    genericValue.get(providerName);

    genericValue = providerId.getModule().getProperty(
        providerId.getModule().findProperty("Location")).getValue();
    genericValue.get(fileName);
    fileName = _resolvePhysicalName(fileName);

    // ATTN: This attribute is probably not required
    genericValue = providerId.getModule().getProperty(
        providerId.getModule().findProperty("InterfaceType")).getValue();
    genericValue.get(interfaceName);

    return ProviderName(providerName, fileName, interfaceName, 0);
}

Boolean DefaultProviderManager::hasActiveProviders()
{
    return providerManager.hasActiveProviders();
}

void DefaultProviderManager::unloadIdleProviders()
{
    providerManager.unloadIdleProviders();
}

PEGASUS_NAMESPACE_END
