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
//		Yi Zhou, Hewlett-Packard Company (yi_zhou@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include "ProviderManagerService.h"

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Common/OperationContext.h>
#include <Pegasus/Common/Destroyer.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/StatisticalData.h>
#include <Pegasus/Common/Logger.h>
#include <Pegasus/Common/MessageLoader.h> //l10n

#include <Pegasus/ProviderManager/ProviderManager.h>
#include <Pegasus/ProviderManager/Provider.h>
#include <Pegasus/ProviderManager/OperationResponseHandler.h>

#include <Pegasus/Config/ConfigManager.h>

#include <Pegasus/Server/ProviderRegistrationManager/ProviderRegistrationManager.h>

PEGASUS_NAMESPACE_BEGIN



// auto variable to protect provider during operations

class pm_service_op_lock 
{
   private:
      pm_service_op_lock(void);
      
   public:
      pm_service_op_lock(Provider *provider)
	 : _provider(provider)
      {
	 _provider->protect();
      }
      
      ~pm_service_op_lock(void)
      {
	 _provider->unprotect();
      }
      
      Provider *_provider;
};



//
// Provider module status
//
static const Uint16 _MODULE_OK       = 2;
static const Uint16 _MODULE_STOPPING = 9;
static const Uint16 _MODULE_STOPPED  = 10;


// provider manager

ProviderManager providerManager;
ProviderManager * ProviderManagerService::getProviderManager(void)
{
   return &providerManager;
}

ProviderManagerService::ProviderManagerService(
    ProviderRegistrationManager * providerRegistrationManager)
    : MessageQueueService(PEGASUS_QUEUENAME_PROVIDERMANAGER_CPP),
    _providerRegistrationManager(providerRegistrationManager)
{
}

ProviderManagerService::~ProviderManagerService(void)
{
}

Triad<String, String, String> _getProviderRegPair(
    const CIMInstance& pInstance, const CIMInstance& pmInstance)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "_getProviderRegPair");

    String providerName;
    String location;
    String interfaceName;

    Array<Uint16> operationalStatus;

    // get the OperationalStatus from the provider module instance
    Uint32 pos = pmInstance.findProperty(CIMName ("OperationalStatus"));

    if(pos == PEG_NOT_FOUND)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "OperationalStatus not found.");

        PEG_METHOD_EXIT();
	//l10n
        //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "provider lookup failed.");
        throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED, MessageLoaderParms(
        					"ProviderManager.ProviderManagerService.PROVIDER_LOOKUP_FAILED",
        					"provider lookup failed."));
    }

    //
    //  ATTN-CAKG-P2-20020821: Check for null status?
    //
    pmInstance.getProperty(pos).getValue().get(operationalStatus);

    for(Uint32 i = 0; i < operationalStatus.size(); i++)
    {
        if(operationalStatus[i] == _MODULE_STOPPED ||
	   operationalStatus[i] == _MODULE_STOPPING)
        {
            PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                "Provider blocked.");

            PEG_METHOD_EXIT();
			//l10n
            //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_ACCESS_DENIED, "provider blocked.");
            throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_ACCESS_DENIED, MessageLoaderParms(
            			"ProviderManager.ProviderManagerService.PROVIDER_BLOCKED",
            			"provider blocked."));
        }
    }

    // get the provider name from the provider instance
    pos = pInstance.findProperty(CIMName ("Name"));

    if(pos == PEG_NOT_FOUND)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Provider name not found.");

        PEG_METHOD_EXIT();
		//l10n
        //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "provider lookup failed.");
        throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED, MessageLoaderParms(
        					"ProviderManager.ProviderManagerService.PROVIDER_LOOKUP_FAILED",
        					"provider lookup failed."));
    }

    pInstance.getProperty(pos).getValue().get(providerName);

    PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
        "providerName = " + providerName + " found.");

    // get the provider location from the provider module instance
    pos = pmInstance.findProperty(CIMName ("Location"));

    if(pos == PEG_NOT_FOUND)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Provider location not found.");

        PEG_METHOD_EXIT();
		//l10n
        //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "provider lookup failed.");
        throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED, MessageLoaderParms(
        					"ProviderManager.ProviderManagerService.PROVIDER_LOOKUP_FAILED",
        					"provider lookup failed."));
    }

    pmInstance.getProperty(pos).getValue().get(location);

    // get the provider location from the provider module instance
    pos = pmInstance.findProperty(CIMName ("InterfaceType"));

    if (pos != PEG_NOT_FOUND)
    {
        pmInstance.getProperty(pos).getValue().get(interfaceName);

        if (String::equal(interfaceName,"C++Default") )
            interfaceName = String::EMPTY;
    }

    PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
        "location = " + location + " found.");

    String fileName;

#ifdef PEGASUS_OS_TYPE_WINDOWS
    fileName = location + String(".dll");
#elif defined(PEGASUS_OS_HPUX)
    fileName = ConfigManager::getHomedPath(ConfigManager::getInstance()->getCurrentValue("providerDir"));
# ifdef PEGASUS_PLATFORM_HPUX_PARISC_ACC
    fileName.append(String("/lib") + location + String(".sl"));
# else
    fileName.append(String("/lib") + location + String(".so"));
# endif
#elif defined(PEGASUS_OS_OS400)
    fileName = location;
#else
    fileName = ConfigManager::getHomedPath(ConfigManager::getInstance()->getCurrentValue("providerDir"));
    fileName.append(String("/lib") + location + String(".so"));
#endif

    PEG_METHOD_EXIT();

    return(Triad<String, String, String>(fileName, providerName,interfaceName));
}

void ProviderManagerService::_lookupProviderForAssocClass(
    const CIMNamespaceName& nameSpace, 
    const CIMName& assocClassName,
    Array<String>& Locations, 
    Array<String>& providerNames,
    Array<String>& interfaceNames)
{
    Array<CIMInstance> pInstances;
    Array<CIMInstance> pmInstances;

    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "ProviderManagerService::_lookupProviderForAssocClass");

    PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
        "nameSpace = " + nameSpace.getString() + 
        "; className = " + assocClassName.getString());

    // get the provider and provider module instance from the registration manager
    if(_providerRegistrationManager->lookupAssociationProvider(
        nameSpace, assocClassName, pInstances, pmInstances) == false)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Provider registration not found for " + nameSpace.getString() +
            " className " + assocClassName.getString());

        PEG_METHOD_EXIT();
		//l10n
        //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "provider lookup failed.");
        throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED, MessageLoaderParms(
        					"ProviderManager.ProviderManagerService.PROVIDER_LOOKUP_FAILED",
        					"provider lookup failed."));
    }

    for(Uint32 i=0,n=pInstances.size(); i<n; i++)
    {
        // get the provider name from the provider instance
        Uint32 pos = pInstances[i].findProperty(CIMName ("Name"));

        String providerName, Location, interfaceName;

        if(pos == PEG_NOT_FOUND)
        {
            PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                "Provider name not found.");
        }

        pInstances[i].getProperty(pos).getValue().get(providerName);

        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "providerName = " + providerName + " found.");

        // get the provider location from the provider module instance
        pos = pmInstances[i].findProperty(CIMName ("Location"));

        if(pos == PEG_NOT_FOUND)
        {
            PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                "Provider location not found.");
        }

        pmInstances[i].getProperty(pos).getValue().get(Location);

        // get the provider location from the provider module instance
        pos = pmInstances[i].findProperty(CIMName ("InterfaceType"));

        if (pos != PEG_NOT_FOUND)
        {
            pmInstances[i].getProperty(pos).getValue().get(interfaceName);

            if (String::equal(interfaceName,"C++Default") )
                interfaceName = String::EMPTY;
        }

        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "location = " + Location + " found.");

        String fileName;

#ifdef PEGASUS_OS_TYPE_WINDOWS
        fileName = Location + String(".dll");
#elif defined(PEGASUS_OS_HPUX)
        fileName = ConfigManager::getHomedPath(ConfigManager::getInstance()->getCurrentValue("providerDir"));
# ifdef PEGASUS_PLATFORM_HPUX_PARISC_ACC
        fileName.append(String("/lib") + Location + String(".sl"));
# else
        fileName.append(String("/lib") + Location + String(".so"));
# endif
#elif defined(PEGASUS_OS_OS400)
        fileName = Location;
#else
        fileName = ConfigManager::getHomedPath(ConfigManager::getInstance()->getCurrentValue("providerDir"));
        fileName.append(String("/lib") + Location + String(".so"));
#endif

        providerNames.append(providerName);
        Locations.append(fileName);
        interfaceNames.append(interfaceName);
    }

    PEG_METHOD_EXIT();

    return;
}

Triad<String, String, String> ProviderManagerService::_lookupMethodProviderForClass(
    const CIMObjectPath & objectPath,
    const CIMName & methodName)
{
    CIMInstance pInstance;
    CIMInstance pmInstance;

    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "ProviderManagerService::_lookupMethodProviderForClass");

    PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
        "nameSpace = " + objectPath.getNameSpace().getString() + 
        "; className = " + objectPath.getClassName().getString() + 
        "; methodName = " + methodName.getString());

    // get the provider and provider module instance from the registration manager
    if(_providerRegistrationManager->lookupMethodProvider(
        objectPath.getNameSpace(), objectPath.getClassName(), methodName,
        pInstance, pmInstance) == false)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Provider registration not found.");

        PEG_METHOD_EXIT();
		//l10n
        //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "provider lookup failed.");
        throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED, MessageLoaderParms(
        					"ProviderManager.ProviderManagerService.PROVIDER_LOOKUP_FAILED",
        					"provider lookup failed."));
    }

    Triad<String, String, String> triad;

    triad = _getProviderRegPair(pInstance, pmInstance);

    PEG_METHOD_EXIT();

    return(triad);
}

Triad<String, String, String> ProviderManagerService::_lookupConsumerProvider(
   const String & destinationPath)
{
   CIMInstance pInstance;
   CIMInstance pmInstance;

   PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "ProviderManagerService::_lookupConsumerProvider");

   PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
		    "destinationPath = " + destinationPath);
   
   //
   // get the provider and provider module instance from the
   // provider registration manager
   //
   if(_providerRegistrationManager->lookupIndicationConsumer(
        destinationPath, pInstance, pmInstance) == false)
   {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Provider registration not found.");

        PEG_METHOD_EXIT();

   	throw CIMException(CIM_ERR_FAILED, MessageLoaderParms(
       		"ProviderManager.ProviderManagerService.PROVIDER_LOOKUP_FAILED",
        					"provider lookup failed."));
   }

   Triad<String, String, String> triad;
   triad = _getProviderRegPair(pInstance, pmInstance);
   
   PEG_METHOD_EXIT();
   
   return(triad);
}

Triad<String, String, String> ProviderManagerService::_lookupProviderForClass(
    const CIMObjectPath & objectPath)
{
    CIMInstance pInstance;
    CIMInstance pmInstance;

    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "ProviderManagerService::_lookupProviderForClass");

    PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
        "nameSpace = " + objectPath.getNameSpace().getString() + 
        "; className = " + objectPath.getClassName().getString());

    // get the provider and provider module instance from the registration manager
    if(_providerRegistrationManager->lookupInstanceProvider(
        objectPath.getNameSpace(), objectPath.getClassName(),
        pInstance, pmInstance) == false)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "Provider registration not found.");

        PEG_METHOD_EXIT();
		//l10n
        //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "provider lookup failed.");
        throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED, MessageLoaderParms(
        					"ProviderManager.ProviderManagerService.PROVIDER_LOOKUP_FAILED",
        					"provider lookup failed."));
    }

    Triad<String, String, String> triad;

    triad = _getProviderRegPair(pInstance, pmInstance);

    PEG_METHOD_EXIT();

    return(triad);
}

Boolean ProviderManagerService::messageOK(const Message * message)
{
    PEGASUS_ASSERT(message != 0);

    /*
    Boolean rc = false;

    switch(message->getType())
    {
    case CIM_GET_INSTANCE_REQUEST_MESSAGE:
    case CIM_ENUMERATE_INSTANCES_REQUEST_MESSAGE:
    case CIM_ENUMERATE_INSTANCE_NAMES_REQUEST_MESSAGE:
    case CIM_CREATE_INSTANCE_REQUEST_MESSAGE:
    case CIM_MODIFY_INSTANCE_REQUEST_MESSAGE:
    case CIM_DELETE_INSTANCE_REQUEST_MESSAGE:
    case CIM_GET_PROPERTY_REQUEST_MESSAGE:
    case CIM_SET_PROPERTY_REQUEST_MESSAGE:
    case CIM_INVOKE_METHOD_REQUEST_MESSAGE:
    case CIM_ENABLE_INDICATION_SUBSCRIPTION_REQUEST_MESSAGE:
    case CIM_MODIFY_INDICATION_SUBSCRIPTION_REQUEST_MESSAGE:
    case CIM_DISABLE_INDICATION_SUBSCRIPTION_REQUEST_MESSAGE:
    rc = true;

    break;
    default:
    rc = false;

    break;
    }

    return(rc);
    */

    return(MessageQueueService::messageOK(message));
}

void ProviderManagerService::handleEnqueue(void)
{
    Message * message = dequeue();

    handleEnqueue(message);
}

void ProviderManagerService::handleEnqueue(Message * message)
{
    PEGASUS_ASSERT(message != 0);

    //*FIXME* Markus
    // catch response messages that should never appear here

    //    if (message->getType() == CIM_ENUMERATE_INSTANCE_NAMES_RESPONSE_MESSAGE)
    //        abort(); // handle double provider callback !

    AsyncLegacyOperationStart * asyncRequest;

    if(message->_async != NULL)
    {
        asyncRequest = static_cast<AsyncLegacyOperationStart *>(message->_async);
    }
    else
    {
        asyncRequest = new AsyncLegacyOperationStart(
            get_next_xid(),
            0,
            this->getQueueId(),
            message,
            this->getQueueId());
    }

    _handle_async_request(asyncRequest);
}

void ProviderManagerService::_handle_async_request(AsyncRequest * request)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "ProviderManagerService::_handle_async_request");

    PEGASUS_ASSERT(request != 0 && request->op != 0 );

    if(request->getType() == async_messages::ASYNC_LEGACY_OP_START)
    {
        request->op->processing();
        _incomingQueue.enqueue(request->op);

        _thread_pool->allocate_and_awaken((void *)this, ProviderManagerService::handleCimOperation);
    }
    else
    {
        // pass all other operations to the default handler
        MessageQueueService::_handle_async_request(request);
    }

    PEG_METHOD_EXIT();

    return;
}

/*
PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL ProviderManagerService::handleServiceOperation(void * arg) throw()
{
    // get the service from argument
    ProviderManagerService * service = reinterpret_cast<ProviderManagerService *>(arg);

    PEGASUS_ASSERT(service != 0);

    // get message from service queue
    Message * message = service->_incomingQueue.dequeue();

    PEGASUS_ASSERT(message != 0);

    return(0);
}
*/

PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL ProviderManagerService::handleCimOperation(void * arg) throw()
{
   PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "ProviderManagerService::handleCimOperation");
    // get the service from argument
    ProviderManagerService * service = reinterpret_cast<ProviderManagerService *>(arg);

    PEGASUS_ASSERT(service != 0);

    if(service->_incomingQueue.size() == 0)
    {
       PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4, 
			"ProviderManagerService::handleCimOperation() called with no op node in queue" );
        // thread started with no message in queue.
        return(PEGASUS_THREAD_RETURN(1));
    }

    AsyncOpNode * op = service->_incomingQueue.dequeue();

    PEGASUS_ASSERT(op != 0 );

    if(op->_request.count() == 0)
    {
        MessageQueue * queue = MessageQueue::lookup(op->_source_queue);

        PEGASUS_ASSERT(queue != 0);
        // no request in op node
        return(PEGASUS_THREAD_RETURN(1));
    }

    AsyncRequest * request = static_cast<AsyncRequest *>(op->_request.next(0));

    PEGASUS_ASSERT(request != 0);

    if(request->getType() == async_messages::ASYNC_LEGACY_OP_START)
    {
        Message * legacy = static_cast<AsyncLegacyOperationStart *>(request)->get_action();

        if(legacy != 0)
        {
            Destroyer<Message> xmessage(legacy);

	    // Set the client's requested language into this service thread.
	    // This will allow functions in this service to return messages
	    // in the correct language.
// l10n    
 	    CIMMessage * msg = dynamic_cast<CIMMessage *>(legacy);
	    if (msg != NULL)
 	    {
			AcceptLanguages *langs = 
   					new AcceptLanguages(msg->acceptLanguages);	
			Thread::setLanguages(langs);   		
	    }
	    else
	    {
			Thread::clearLanguages();
	    }		           

            // pass the request message to a handler method based on message type
            switch(legacy->getType())
            {
            case CIM_GET_CLASS_REQUEST_MESSAGE:
            case CIM_ENUMERATE_CLASSES_REQUEST_MESSAGE:
            case CIM_ENUMERATE_CLASS_NAMES_REQUEST_MESSAGE:
            case CIM_CREATE_CLASS_REQUEST_MESSAGE:
            case CIM_MODIFY_CLASS_REQUEST_MESSAGE:
            case CIM_DELETE_CLASS_REQUEST_MESSAGE:
                break;
            case CIM_GET_INSTANCE_REQUEST_MESSAGE:
                service->handleGetInstanceRequest(op, legacy);

                break;
            case CIM_ENUMERATE_INSTANCES_REQUEST_MESSAGE:
                service->handleEnumerateInstancesRequest(op, legacy);

                break;
            case CIM_ENUMERATE_INSTANCE_NAMES_REQUEST_MESSAGE:
                service->handleEnumerateInstanceNamesRequest(op, legacy);

                break;
            case CIM_CREATE_INSTANCE_REQUEST_MESSAGE:
                service->handleCreateInstanceRequest(op, legacy);

                break;
            case CIM_MODIFY_INSTANCE_REQUEST_MESSAGE:
                service->handleModifyInstanceRequest(op, legacy);

                break;
            case CIM_DELETE_INSTANCE_REQUEST_MESSAGE:
                service->handleDeleteInstanceRequest(op, legacy);

                break;
            case CIM_EXEC_QUERY_REQUEST_MESSAGE:
                service->handleExecuteQueryRequest(op, legacy);

                break;
            case CIM_ASSOCIATORS_REQUEST_MESSAGE:
                service->handleAssociatorsRequest(op, legacy);

                break;
            case CIM_ASSOCIATOR_NAMES_REQUEST_MESSAGE:
                service->handleAssociatorNamesRequest(op, legacy);

                break;
            case CIM_REFERENCES_REQUEST_MESSAGE:
                service->handleReferencesRequest(op, legacy);

                break;
            case CIM_REFERENCE_NAMES_REQUEST_MESSAGE:
                service->handleReferenceNamesRequest(op, legacy);

                break;
            case CIM_GET_PROPERTY_REQUEST_MESSAGE:
                service->handleGetPropertyRequest(op, legacy);

                break;
            case CIM_SET_PROPERTY_REQUEST_MESSAGE:
                service->handleSetPropertyRequest(op, legacy);

                break;
            case CIM_INVOKE_METHOD_REQUEST_MESSAGE:
                service->handleInvokeMethodRequest(op, legacy);

                break;
            case CIM_CREATE_SUBSCRIPTION_REQUEST_MESSAGE:
                service->handleCreateSubscriptionRequest(op, legacy);

                break;
            case CIM_MODIFY_SUBSCRIPTION_REQUEST_MESSAGE:
                service->handleModifySubscriptionRequest(op, legacy);

                break;
            case CIM_DELETE_SUBSCRIPTION_REQUEST_MESSAGE:
                service->handleDeleteSubscriptionRequest(op, legacy);

                break;
            case CIM_ENABLE_INDICATIONS_REQUEST_MESSAGE:
                service->handleEnableIndicationsRequest(op, legacy);

                break;
            case CIM_DISABLE_INDICATIONS_REQUEST_MESSAGE:
                service->handleDisableIndicationsRequest(op, legacy);

                break;
            case CIM_DISABLE_MODULE_REQUEST_MESSAGE:
                service->handleDisableModuleRequest(op, legacy);

                break;
            case CIM_ENABLE_MODULE_REQUEST_MESSAGE:
                service->handleEnableModuleRequest(op, legacy);

                break;
            case CIM_STOP_ALL_PROVIDERS_REQUEST_MESSAGE:
                service->handleStopAllProvidersRequest(op, legacy);

                break;
	    case CIM_EXPORT_INDICATION_REQUEST_MESSAGE:
	       service->handleExportIndicationRequest(op, legacy);
	       break;

            default:
                // unsupported messages are ignored
                break;
            }
        }
    }
    else
    {
        // reply with a NAK
    }
    PEG_METHOD_EXIT();
    return(0);
}

void ProviderManagerService::handleGetInstanceRequest(AsyncOpNode *op, const Message *message) throw()
{
   PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "ProviderManagerService::handleGetInstanceRequest");
    CIMGetInstanceRequestMessage * request =
        dynamic_cast<CIMGetInstanceRequestMessage *>(const_cast<Message *>(message));

    AsyncRequest * async = static_cast<AsyncRequest *>(op->_request.next(0));

    PEGASUS_ASSERT(request != 0 && async != 0 );

    CIMGetInstanceResponseMessage * response =
        new CIMGetInstanceResponseMessage(
        request->messageId,
        CIMException(),
        request->queueIds.copyAndPop(),
        CIMInstance());

    PEGASUS_ASSERT(response != 0);

    // preserve message key
    //response->setKey(request->getKey());
    
    response->synch_response(request);
    
    //
    //  Set HTTP method in response from request
    //
    response->setHttpMethod (request->getHttpMethod ());

    // create a handler for this request
    GetInstanceResponseHandler handler(request, response);

    try
    {
	Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
	            "ProviderManagerService::handleGetInstanceRequest - Host name: $0  Name space: $1  Class name: $2",
	            System::getHostName(),
	   	    request->nameSpace.getString(),
 	            request->instanceName.getClassName().getString());	
	
        // make target object path
        CIMObjectPath objectPath(
            System::getHostName(),
            request->nameSpace,
            request->instanceName.getClassName(),
            request->instanceName.getKeyBindings());

        // get the provider file name and logical name
        Triad<String, String, String> triad =
                _lookupProviderForClass(objectPath);

        // get cached or load new provider module
        //Provider provider =
        OpProviderHolder ph = 
                providerManager.getProvider(triad.first, triad.second, triad.third);

        // convert arguments
        OperationContext context;

        // add the user name to the context
        context.insert(IdentityContainer(request->userName));

//l10n
	// add the langs to the context
	context.insert(AcceptLanguageListContainer(request->acceptLanguages));
	context.insert(ContentLanguageListContainer(request->contentLanguages));	 

        CIMPropertyList propertyList(request->propertyList);

        STAT_GETSTARTTIME;

	PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4, 
			 "Calling provider.getInstance: " + 
			 ph.GetProvider().getName());
	
        // forward request
	pm_service_op_lock op_lock(&ph.GetProvider());
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

    AsyncLegacyOperationResult *async_result =
        new AsyncLegacyOperationResult(
        async->getKey(),
        async->getRouting(),
        op,
        response);

    _complete_op_node(op, ASYNC_OPSTATE_COMPLETE, 0, 0);

    PEG_METHOD_EXIT();
}

void ProviderManagerService::handleEnumerateInstancesRequest(AsyncOpNode *op, const Message * message) throw()
{
   PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "ProviderManagerService::handleEnumerateInstanceRequest");
    CIMEnumerateInstancesRequestMessage * request =
        dynamic_cast<CIMEnumerateInstancesRequestMessage *>(const_cast<Message *>(message));

    AsyncRequest * async = static_cast<AsyncRequest *>(op->_request.next(0));

    PEGASUS_ASSERT(request != 0 && async != 0 );

    CIMEnumerateInstancesResponseMessage * response =
        new CIMEnumerateInstancesResponseMessage(
        request->messageId,
        CIMException(),
        request->queueIds.copyAndPop(),
        Array<CIMInstance>());

    PEGASUS_ASSERT(response != 0);

    // preserve message key
    //response->setKey(request->getKey());
    response->synch_response(request);
    //
    //  Set HTTP method in response from request
    //
    response->setHttpMethod (request->getHttpMethod ());

    // create a handler for this request
    EnumerateInstancesResponseHandler handler(request, response);

    try
    {
	Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
	            "ProviderManagerService::handleEnumerateInstancesRequest - Host name: $0  Name space: $1  Class name: $2",
	            System::getHostName(),
	   	    request->nameSpace.getString(),
 	            request->className.getString());	
        // make target object path
        CIMObjectPath objectPath(
            System::getHostName(),
            request->nameSpace,
            request->className);

        // get the provider file name and logical name
        Triad<String, String, String> triad =
                _lookupProviderForClass(objectPath);

        // get cached or load new provider module
        //Provider provider =
        OpProviderHolder ph = 
                providerManager.getProvider(triad.first, triad.second, triad.third);

        // convert arguments
        OperationContext context;

        // add the user name to the context
        context.insert(IdentityContainer(request->userName));

//l10n
	// add the langs to the context
	context.insert(AcceptLanguageListContainer(request->acceptLanguages));
	context.insert(ContentLanguageListContainer(request->contentLanguages));	 

        CIMPropertyList propertyList(request->propertyList);

        STAT_GETSTARTTIME;

	PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4, 
			 "Calling provider.enumerateInstances: " + 
			 ph.GetProvider().getName());
	pm_service_op_lock op_lock(&ph.GetProvider());
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

    AsyncLegacyOperationResult *async_result =
        new AsyncLegacyOperationResult(
        async->getKey(),
        async->getRouting(),
        op,
        response);

    _complete_op_node(op, ASYNC_OPSTATE_COMPLETE, 0, 0);
    PEG_METHOD_EXIT();
}

void ProviderManagerService::handleEnumerateInstanceNamesRequest(AsyncOpNode *op, const Message * message) throw()
{
   PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "ProviderManagerService::handleEnumerateInstanceNamesRequest");
    CIMEnumerateInstanceNamesRequestMessage * request =
        dynamic_cast<CIMEnumerateInstanceNamesRequestMessage *>(const_cast<Message *>(message));

    AsyncRequest *async = static_cast<AsyncRequest *>(op->_request.next(0));

    PEGASUS_ASSERT(request != 0 && async != 0 );

    CIMEnumerateInstanceNamesResponseMessage * response =
        new CIMEnumerateInstanceNamesResponseMessage(
        request->messageId,
        CIMException(),
        request->queueIds.copyAndPop(),
        Array<CIMObjectPath>());

    PEGASUS_ASSERT(response != 0);

    // preserve message key
    response->synch_response(request);

	//set HTTP method in response from request

	response->setHttpMethod(request->getHttpMethod());;

    // create a handler for this request
    EnumerateInstanceNamesResponseHandler handler(request, response);

    // process the request
    try
    {
	Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
	            "ProviderManagerService::handleEnumerateInstanceNamesRequest - Host name: $0  Name space: $1  Class name: $2",
	            System::getHostName(),
	   	    request->nameSpace.getString(),
 	            request->className.getString());	

        // make target object path
        CIMObjectPath objectPath(
            System::getHostName(),
            request->nameSpace,
            request->className);

        // get the provider file name and logical name
        Triad<String, String, String> triad =
                _lookupProviderForClass(objectPath);

        // get cached or load new provider module
        //Provider provider =
        OpProviderHolder ph = 
                providerManager.getProvider(triad.first, triad.second, triad.third);

        // convert arguments
        OperationContext context;

        // add the user name to the context
        context.insert(IdentityContainer(request->userName));

//l10n
	// add the langs to the context
	context.insert(AcceptLanguageListContainer(request->acceptLanguages));
	context.insert(ContentLanguageListContainer(request->contentLanguages));	 

        STAT_GETSTARTTIME;
	PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4, 
			 "Calling provider.enumerateInstanceNames: " + 
			 ph.GetProvider().getName());
	pm_service_op_lock op_lock(&ph.GetProvider());
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

    AsyncLegacyOperationResult *async_result =
        new AsyncLegacyOperationResult(
        async->getKey(),
        async->getRouting(),
        op,
        response);

    _complete_op_node(op, ASYNC_OPSTATE_COMPLETE, 0, 0);
    PEG_METHOD_EXIT();
}

void ProviderManagerService::handleCreateInstanceRequest(AsyncOpNode *op, const Message * message) throw()
{
   PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "ProviderManagerService::handleCreateInstanceRequest");
    CIMCreateInstanceRequestMessage * request =
        dynamic_cast<CIMCreateInstanceRequestMessage *>(const_cast<Message *>(message));
 
    AsyncRequest *async = static_cast<AsyncRequest *>(op->_request.next(0));

    PEGASUS_ASSERT(request != 0 && async != 0 );

    // create response message
    CIMCreateInstanceResponseMessage * response =
        new CIMCreateInstanceResponseMessage(
        request->messageId,
        CIMException(),
        request->queueIds.copyAndPop(),
        CIMObjectPath());

    PEGASUS_ASSERT(response != 0);

    // preserve message key
    response->synch_response(request);

    //
    //  Set HTTP method in response from request
    //
    response->setHttpMethod (request->getHttpMethod ());

    // create a handler for this request
    CreateInstanceResponseHandler handler(request, response);

    try
    {
	Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
	            "ProviderManagerService::handleCreateInstanceRequest - Host name: $0  Name space: $1  Class name: $2",
	            System::getHostName(),
	   	    request->nameSpace.getString(),
 	            request->newInstance.getPath().getClassName().getString());	

        // make target object path
        CIMObjectPath objectPath(
            System::getHostName(),
            request->nameSpace,
            request->newInstance.getPath().getClassName(),
            request->newInstance.getPath().getKeyBindings());

        // get the provider file name and logical name
        Triad<String, String, String> triad =
                _lookupProviderForClass(objectPath);

        // get cached or load new provider module
        //Provider provider =
        OpProviderHolder ph = 
                providerManager.getProvider(triad.first, triad.second, triad.third);

        // convert arguments
        OperationContext context;

        // add the user name to the context
        context.insert(IdentityContainer(request->userName));

//l10n
	// add the langs to the context
	context.insert(AcceptLanguageListContainer(request->acceptLanguages));
	context.insert(ContentLanguageListContainer(request->contentLanguages));	 

        // forward request

        STAT_GETSTARTTIME;
	PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4, 
			 "Calling provider.createInstance: " + 
			 ph.GetProvider().getName());
	pm_service_op_lock op_lock(&ph.GetProvider());
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

    AsyncLegacyOperationResult *async_result =
        new AsyncLegacyOperationResult(
        async->getKey(),
        async->getRouting(),
        op,
        response);

    _complete_op_node(op, ASYNC_OPSTATE_COMPLETE, 0, 0);
    PEG_METHOD_EXIT();
}

void ProviderManagerService::handleModifyInstanceRequest(AsyncOpNode *op, const Message * message) throw()
{
   PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "ProviderManagerService::handleModifyInstanceRequest");
    CIMModifyInstanceRequestMessage * request =
        dynamic_cast<CIMModifyInstanceRequestMessage *>(const_cast<Message *>(message));

    AsyncRequest *async = static_cast<AsyncRequest *>(op->_request.next(0));
    PEGASUS_ASSERT(request != 0 && async != 0 );

    // create response message
    CIMModifyInstanceResponseMessage * response =
        new CIMModifyInstanceResponseMessage(
        request->messageId,
        CIMException(),
        request->queueIds.copyAndPop());

    PEGASUS_ASSERT(response != 0);

    // preserve message key
    response->synch_response(request);

    //
    //  Set HTTP method in response from request
    //
    response->setHttpMethod (request->getHttpMethod ());

    // create a handler for this request
    ModifyInstanceResponseHandler handler(request, response);

    try
    {
	Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
	            "ProviderManagerService::handleModifyInstanceRequest - Host name: $0  Name space: $1  Class name: $2",
	            System::getHostName(),
	   	    request->nameSpace.getString(),
 	            request->modifiedInstance.getPath().getClassName().getString());	

        // make target object path
        CIMObjectPath objectPath(
            System::getHostName(),
            request->nameSpace,
            request->modifiedInstance.getPath ().getClassName(),
            request->modifiedInstance.getPath ().getKeyBindings());

        // get the provider file name and logical name
        Triad<String, String, String> triad =
                _lookupProviderForClass(objectPath);

        // get cached or load new provider module
        //Provider provider =
        OpProviderHolder ph = 
                providerManager.getProvider(triad.first, triad.second, triad.third);

        // convert arguments
        OperationContext context;

        // add the user name to the context
        context.insert(IdentityContainer(request->userName));

//l10n
	// add the langs to the context
	context.insert(AcceptLanguageListContainer(request->acceptLanguages));
	context.insert(ContentLanguageListContainer(request->contentLanguages));	 

        CIMPropertyList propertyList(request->propertyList);

        // forward request
        STAT_GETSTARTTIME;
	PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4, 
			 "Calling provider.modifyInstance: " + 
			 ph.GetProvider().getName());
	pm_service_op_lock op_lock(&ph.GetProvider());
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

    AsyncLegacyOperationResult *async_result =
        new AsyncLegacyOperationResult(
        async->getKey(),
        async->getRouting(),
        op,
        response);

    _complete_op_node(op, ASYNC_OPSTATE_COMPLETE, 0, 0);
    PEG_METHOD_EXIT();
}

void ProviderManagerService::handleDeleteInstanceRequest(AsyncOpNode *op, const Message * message) throw()
{
   PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "ProviderManagerService::handleDeleteInstanceRequest");
    CIMDeleteInstanceRequestMessage * request =
        dynamic_cast<CIMDeleteInstanceRequestMessage *>(const_cast<Message *>(message));

    AsyncRequest *async = static_cast<AsyncRequest *>(op->_request.next(0));

    PEGASUS_ASSERT(request != 0 && async != 0 );

    // create response message
    CIMDeleteInstanceResponseMessage * response =
        new CIMDeleteInstanceResponseMessage(
        request->messageId,
        CIMException(),
        request->queueIds.copyAndPop());

    PEGASUS_ASSERT(response != 0);

    // preserve message key
    response->synch_response(request);

    //
    //  Set HTTP method in response from request
    //
    response->setHttpMethod (request->getHttpMethod ());

    // create a handler for this request
    DeleteInstanceResponseHandler handler(request, response);

    try
    {
	Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
	            "ProviderManagerService::handleDeleteInstanceRequest - Host name: $0  Name space: $1  Class name: $2",
	            System::getHostName(),
	   	    request->nameSpace.getString(),
 	            request->instanceName.getClassName().getString());	

        // make target object path
        CIMObjectPath objectPath(
            System::getHostName(),
            request->nameSpace,
            request->instanceName.getClassName(),
            request->instanceName.getKeyBindings());

        // get the provider file name and logical name
        Triad<String, String, String> triad =
                _lookupProviderForClass(objectPath);

        // get cached or load new provider module
        //Provider provider =
        OpProviderHolder ph = 
            providerManager.getProvider(triad.first, triad.second, triad.third);

        // convert arguments
        OperationContext context;

        context.insert(IdentityContainer(request->userName));

//l10n
	// add the langs to the context
	context.insert(AcceptLanguageListContainer(request->acceptLanguages));
	context.insert(ContentLanguageListContainer(request->contentLanguages));	 

        // forward request
        STAT_GETSTARTTIME;
	PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4, 
			 "Calling provider.deleteInstance: " +
			 ph.GetProvider().getName());

	pm_service_op_lock op_lock(&ph.GetProvider());
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

    AsyncLegacyOperationResult *async_result =
        new AsyncLegacyOperationResult(
        async->getKey(),
        async->getRouting(),
        op,
        response);

    _complete_op_node(op, ASYNC_OPSTATE_COMPLETE, 0, 0);
    PEG_METHOD_EXIT();
}

void ProviderManagerService::handleExecuteQueryRequest(AsyncOpNode *op, const Message * message) throw()
{
   PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "ProviderManagerService::handleExecuteQueryRequest");
    CIMExecQueryRequestMessage * request =
        dynamic_cast<CIMExecQueryRequestMessage *>(const_cast<Message *>(message));

    AsyncRequest *async = static_cast<AsyncRequest *>(op->_request.next(0));

    PEGASUS_ASSERT(request != 0 && async != 0 );

    Array<CIMObject> cimObjects;

//l10n
    //CIMExecQueryResponseMessage * response =
        //new CIMExecQueryResponseMessage(
        //request->messageId,
        //PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "not implemented"),
        //request->queueIds.copyAndPop(),
        //cimObjects);
    CIMExecQueryResponseMessage * response =
        new CIMExecQueryResponseMessage(
        request->messageId,
        PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED, MessageLoaderParms(
        					"ProviderManager.ProviderManagerService.NOT_IMPLEMENTED",
        					"not implemented")),
        request->queueIds.copyAndPop(),
        cimObjects);

    PEGASUS_ASSERT(response != 0);

    // preserve message key
    response->synch_response(request);


// l10n
// ATTN: when this is implemented, need to add the language containers to the 
// OperationContext.  See how the other requests do it.


// l10n
// ATTN: when this is implemented, need to add the language containers to the 
// OperationContext.  See how the other requests do it.


// l10n
// ATTN: when this is implemented, need to add the language containers to the 
// OperationContext.  See how the other requests do it.


// l10n
// ATTN: when this is implemented, need to add the language containers to the 
// OperationContext.  See how the other requests do it.

    //
    //  Set HTTP method in response from request
    //
    response->setHttpMethod (request->getHttpMethod ());

    AsyncLegacyOperationResult *async_result =
        new AsyncLegacyOperationResult(
        async->getKey(),
        async->getRouting(),
        op,
        response);

    _complete_op_node(op, ASYNC_OPSTATE_COMPLETE, 0, 0);
    PEG_METHOD_EXIT();
}

void ProviderManagerService::handleAssociatorsRequest(AsyncOpNode *op, const Message * message) throw()
{
   PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "ProviderManagerService::handleAssociatorsRequest");
   CIMAssociatorsRequestMessage * request =
      dynamic_cast<CIMAssociatorsRequestMessage *>(const_cast<Message *>(message));

    AsyncRequest *async = static_cast<AsyncRequest *>(op->_request.next(0));

    PEGASUS_ASSERT(request != 0 && async != 0);

    Array<CIMObject> cimObjects;
       
    CIMAssociatorsResponseMessage * response =
           new CIMAssociatorsResponseMessage(
           request->messageId,
           CIMException(),
           request->queueIds.copyAndPop(),
           cimObjects);

       PEGASUS_ASSERT(response != 0);

       // preserve message key
       response->synch_response(request);

       // create a handler for this request
       AssociatorsResponseHandler handler(request, response);

       // process the request
       try
       {
           Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
                    "ProviderManagerService::handleAssociatorsRequest - Host name: $0  Name space: $1  Class name: $2",
                    System::getHostName(),
                    request->nameSpace.getString(),
                    request->objectName.getClassName().getString());	

           // make target object path
           CIMObjectPath objectPath(
               System::getHostName(),
               request->nameSpace,
               request->objectName.getClassName());

           objectPath.setKeyBindings(request->objectName.getKeyBindings());

           // get the provider file name and logical name
           Array<String> first;
           Array<String> second;
           Array<String> third;

           _lookupProviderForAssocClass(request->nameSpace,
                                        request->assocClass,
                                        first, second, third);

           for(Uint32 i=0,n=first.size(); i<n; i++)
           {
               // get cached or load new provider module
               //Provider provider =
               OpProviderHolder ph = 
                  providerManager.getProvider(first[i], second[i], third[i]);

               // convert arguments
               OperationContext context;

               // add the user name to the context
               context.insert(IdentityContainer(request->userName));

//l10n
               // add the langs to the context
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
                   request->propertyList,
                   handler);

               STAT_PMS_PROVIDEREND;

           } // end for loop
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

    AsyncLegacyOperationResult *async_result =
        new AsyncLegacyOperationResult(
        async->getKey(),
        async->getRouting(),
        op,
        response);

    _complete_op_node(op, ASYNC_OPSTATE_COMPLETE, 0, 0);
    PEG_METHOD_EXIT();

}

void ProviderManagerService::handleAssociatorNamesRequest(AsyncOpNode *op, const Message * message) throw()
{
   PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "ProviderManagerService::handleAssociatorNamesRequest");
    CIMAssociatorNamesRequestMessage * request =
        dynamic_cast<CIMAssociatorNamesRequestMessage *>(const_cast<Message *>(message));

    AsyncRequest *async = static_cast<AsyncRequest *>(op->_request.next(0));

    PEGASUS_ASSERT(request != 0 && async != 0 );

    Array<CIMObjectPath> cimReferences;

    CIMAssociatorNamesResponseMessage * response =
        new CIMAssociatorNamesResponseMessage(
        request->messageId,
        CIMException(),
        request->queueIds.copyAndPop(),
        cimReferences);

    PEGASUS_ASSERT(response != 0);

    // preserve message key
    response->synch_response(request);

    //
    //  Set HTTP method in response from request
    //
    response->setHttpMethod (request->getHttpMethod ());

    // create a handler for this request
    AssociatorNamesResponseHandler handler(request, response);

    // process the request
    try
    {
	Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
	            "ProviderManagerService::handleAssociationNamesRequest - Host name: $0  Name space: $1  Class name: $2",
	            System::getHostName(),
	   	    request->nameSpace.getString(),
 	            request->objectName.getClassName().getString());	

        // make target object path
        CIMObjectPath objectPath(
            System::getHostName(),
            request->nameSpace,
            request->objectName.getClassName());

        objectPath.setKeyBindings(request->objectName.getKeyBindings());

        // get the provider file name and logical name
        Array<String> first;
        Array<String> second;
        Array<String> third;

        _lookupProviderForAssocClass(request->nameSpace,
                                     request->assocClass,
                                     first, second, third);

        for(Uint32 i=0,n=first.size(); i<n; i++)
        {
            // get cached or load new provider module
            //Provider provider =
            OpProviderHolder ph = 
               providerManager.getProvider(first[i], second[i], third[i]);

            // convert arguments
            OperationContext context;

            // add the user name to the context
            context.insert(IdentityContainer(request->userName));

//l10n
            // add the langs to the context
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

        } // end for loop
    }
    catch(CIMException & e)
    {
        handler.setStatus(e.getCode(), e.getContentLanguages(), e.getMessage()); // l10n
    }
    catch(Exception & e)
    {
        handler.setStatus(CIM_ERR_FAILED, e.getContentLanguages(), e.getMessage()); // l10n
    }
    catch(...)
    {
       PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4, 
			"Exception: Unknown");
        handler.setStatus(CIM_ERR_FAILED, "Unknown error.");
    }

    AsyncLegacyOperationResult *async_result =
        new AsyncLegacyOperationResult(
        async->getKey(),
        async->getRouting(),
        op,
        response);

    _complete_op_node(op, ASYNC_OPSTATE_COMPLETE, 0, 0);
    PEG_METHOD_EXIT();
}

void ProviderManagerService::handleReferencesRequest(AsyncOpNode *op, const Message * message) throw()
{
   PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "ProviderManagerService::handleReferencesRequest");
   CIMReferencesRequestMessage * request =
        dynamic_cast<CIMReferencesRequestMessage *>(const_cast<Message *>(message));

    AsyncRequest *async = static_cast<AsyncRequest *>(op->_request.next(0));

    PEGASUS_ASSERT(request != 0 && async != 0 );

    Array<CIMObject> cimObjects;

    CIMReferencesResponseMessage * response =
        new CIMReferencesResponseMessage(
        request->messageId,
        CIMException(),
        request->queueIds.copyAndPop(),
        cimObjects);

    PEGASUS_ASSERT(response != 0);

    // preserve message key
    response->synch_response(request);

    //
    //  Set HTTP method in response from request
    //
    response->setHttpMethod (request->getHttpMethod ());

    // create a handler for this request
    ReferencesResponseHandler handler(request, response);

    // process the request
    try
    {
	Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
	            "ProviderManagerService::handleReferencesRequest - Host name: $0  Name space: $1  Class name: $2",
                System::getHostName(),
                request->nameSpace.getString(),
                request->objectName.getClassName().getString());	

        // make target object path
        CIMObjectPath objectPath(
            System::getHostName(),
            request->nameSpace,
            request->objectName.getClassName());

        objectPath.setKeyBindings(request->objectName.getKeyBindings());

        // get the provider file name and logical name
        Array<String> first;
        Array<String> second;
        Array<String> third;

        _lookupProviderForAssocClass(request->nameSpace,
                                     request->resultClass,
                                     first, second, third);

        for(Uint32 i=0,n=first.size(); i<n; i++)
        {
            // get cached or load new provider module
            //Provider provider =
            OpProviderHolder ph = 
               providerManager.getProvider(first[i], second[i], third[i]);

            // convert arguments
            OperationContext context;

            // add the user name to the context
            context.insert(IdentityContainer(request->userName));

//l10n
            // add the langs to the context
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
                request->propertyList,
                handler);
            STAT_PMS_PROVIDEREND;

        } // end for loop
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

    AsyncLegacyOperationResult *async_result =
        new AsyncLegacyOperationResult(
        async->getKey(),
        async->getRouting(),
        op,
        response);

    _complete_op_node(op, ASYNC_OPSTATE_COMPLETE, 0, 0);
    PEG_METHOD_EXIT();
}

void ProviderManagerService::handleReferenceNamesRequest(AsyncOpNode *op, const Message * message) throw()
{
   PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "ProviderManagerService::handleReferenceNamesRequest");
    CIMReferenceNamesRequestMessage * request =
        dynamic_cast<CIMReferenceNamesRequestMessage *>(const_cast<Message *>(message));

    AsyncRequest *async = static_cast<AsyncRequest *>(op->_request.next(0));

    PEGASUS_ASSERT(request != 0 && async != 0 );

    Array<CIMObjectPath> cimReferences;

    CIMReferenceNamesResponseMessage * response =
        new CIMReferenceNamesResponseMessage(
        request->messageId,
        CIMException(),
        request->queueIds.copyAndPop(),
        cimReferences);

    // preserve message key
    response->synch_response(request);

    //
    //  Set HTTP method in response from request
    //
    response->setHttpMethod (request->getHttpMethod ());

    // create a handler for this request
    ReferenceNamesResponseHandler handler(request, response);

    // process the request
    try
    {
	Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
	            "ProviderManagerService::handleReferenceNamesRequest - Host name: $0  Name space: $1  Class name: $2",
	            System::getHostName(),
	   	    request->nameSpace.getString(),
 	            request->objectName.getClassName().getString());	

        // make target object path
        CIMObjectPath objectPath(
            System::getHostName(),
            request->nameSpace,
            request->objectName.getClassName());

        objectPath.setKeyBindings(request->objectName.getKeyBindings());

        // get the provider file name and logical name
        Array<String> first;
        Array<String> second;
        Array<String> third;

        _lookupProviderForAssocClass(request->nameSpace,
                                     request->resultClass,
                                     first, second, third);

        for(Uint32 i=0,n=first.size(); i<n; i++)
        {
            // get cached or load new provider module
            //Provider provider =
            OpProviderHolder ph = 
               providerManager.getProvider(first[i], second[i], third[i]);

            // convert arguments
            OperationContext context;

            // add the user name to the context
            context.insert(IdentityContainer(request->userName));

//l10n
            // add the langs to the context
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

        } // end for loop
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

    AsyncLegacyOperationResult *async_result =
        new AsyncLegacyOperationResult(
        async->getKey(),
        async->getRouting(),
        op,
        response);

    _complete_op_node(op, ASYNC_OPSTATE_COMPLETE, 0, 0);
    PEG_METHOD_EXIT();
}

void ProviderManagerService::handleGetPropertyRequest(AsyncOpNode *op, const Message * message) throw()
{
   PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "ProviderManagerService::handleGetPropertyRequest");
    CIMGetPropertyRequestMessage * request =
        dynamic_cast<CIMGetPropertyRequestMessage *>(const_cast<Message *>(message));

    AsyncRequest *async = static_cast<AsyncRequest *>(op->_request.next(0));

    PEGASUS_ASSERT(request != 0 && async != 0 );

    CIMValue cimValue;

    // create response message
    CIMGetPropertyResponseMessage * response =
        new CIMGetPropertyResponseMessage(
        request->messageId,
        CIMException(),
        request->queueIds.copyAndPop(),
        cimValue);

    PEGASUS_ASSERT(response != 0);

    // preserve message key
    response->synch_response(request);

    //
    //  Set HTTP method in response from request
    //
    response->setHttpMethod (request->getHttpMethod ());

    GetPropertyResponseHandler handler(request, response);

    try
    {
	Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
	            "ProviderManagerService::handleGetPropertyRequest - Host name: $0  Name space: $1  Class name: $2",
	            System::getHostName(),
	   	    request->nameSpace.getString(),
 	            request->instanceName.getClassName().getString());	

    	// make target object path
    	CIMObjectPath objectPath(
    	    System::getHostName(),
    	    request->nameSpace,
    	    request->instanceName.getClassName(),
    	    request->instanceName.getKeyBindings());

    	// get the provider file name and logical name
    	Triad<String, String, String> triad =
                _lookupProviderForClass(objectPath);

    	// get cached or load new provider module
        //Provider provider =
        OpProviderHolder ph = 
                providerManager.getProvider(triad.first, triad.second, triad.third);

    	// convert arguments
    	OperationContext context;

    	// add the user name to the context
    	context.insert(IdentityContainer(request->userName));

//l10n
        // add the langs to the context
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

    AsyncLegacyOperationResult *async_result =
        new AsyncLegacyOperationResult(
        async->getKey(),
        async->getRouting(),
        op,
        response);

    _complete_op_node(op, ASYNC_OPSTATE_COMPLETE, 0, 0);
    PEG_METHOD_EXIT();
}

void ProviderManagerService::handleSetPropertyRequest(AsyncOpNode *op, const Message * message) throw()
{
   PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "ProviderManagerService::handleSetPropertyRequest");
    CIMSetPropertyRequestMessage * request =
        dynamic_cast<CIMSetPropertyRequestMessage *>(const_cast<Message *>(message));

    AsyncRequest *async = static_cast<AsyncRequest *>(op->_request.next(0));

    PEGASUS_ASSERT(request != 0 && async != 0);

    // create response message
    CIMSetPropertyResponseMessage * response =
        new CIMSetPropertyResponseMessage(
        request->messageId,
	CIMException(),
        request->queueIds.copyAndPop());


    PEGASUS_ASSERT(response != 0);

    // preserve message key
    response->synch_response(request);

    //
    //  Set HTTP method in response from request
    //
    response->setHttpMethod (request->getHttpMethod ());

    SetPropertyResponseHandler handler(request, response);

    try
    {
	Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
	            "ProviderManagerService::handleSetPropertyRequest - Host name: $0  Name space: $1  Class name: $2",
	            System::getHostName(),
	   	    request->nameSpace.getString(),
 	            request->instanceName.getClassName().getString());	

    	// make target object path
    	CIMObjectPath objectPath(
    	    System::getHostName(),
    	    request->nameSpace,
    	    request->instanceName.getClassName(),
    	    request->instanceName.getKeyBindings());

    	// get the provider file name and logical name
    	Triad<String, String, String> triad =
                _lookupProviderForClass(objectPath);

    	// get cached or load new provider module
        //Provider provider =
        OpProviderHolder ph = 
                providerManager.getProvider(triad.first, triad.second, triad.third);

    	// convert arguments
    	OperationContext context;

    	// add the user name to the context
    	context.insert(IdentityContainer(request->userName));

//l10n
        // add the langs to the context
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

    AsyncLegacyOperationResult *async_result =
        new AsyncLegacyOperationResult(
        async->getKey(),
        async->getRouting(),
        op,
        response);

    _complete_op_node(op, ASYNC_OPSTATE_COMPLETE, 0, 0);
    PEG_METHOD_EXIT();
}

void ProviderManagerService::handleInvokeMethodRequest(AsyncOpNode *op, const Message * message) throw()
{
   PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "ProviderManagerService::handleInvokeMethodRequest");

    CIMInvokeMethodRequestMessage * request =
        dynamic_cast<CIMInvokeMethodRequestMessage *>(const_cast<Message *>(message));

    AsyncRequest *async = static_cast<AsyncRequest *>(op->_request.next(0));

    PEGASUS_ASSERT(request != 0 && async != 0 );

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
    response->synch_response(request);

    //
    //  Set HTTP method in response from request
    //
    response->setHttpMethod (request->getHttpMethod ());

    // create a handler for this request
    InvokeMethodResponseHandler handler(request, response);

    try
    {
	Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
	            "ProviderManagerService::handleInvokeMethodRequest - Host name: $0  Name space: $1  Class name: $2",
	            System::getHostName(),
	   	    request->nameSpace.getString(),
 	            request->instanceName.getClassName().getString());	

        // make target object path
        CIMObjectPath objectPath(
            System::getHostName(),
            request->nameSpace,
            request->instanceName.getClassName(),
            request->instanceName.getKeyBindings());

        // get the provider file name and logical name
        Triad<String, String, String> triad =
            _lookupMethodProviderForClass(objectPath, request->methodName);

	// get cached or load new provider module
        //Provider provider =
        OpProviderHolder ph = 
            providerManager.getProvider(triad.first, triad.second, triad.third);

        // convert arguments
        OperationContext context;

        // add the user name to the context
        context.insert(IdentityContainer(request->userName));

//l10n
        // add the langs to the context
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

    AsyncLegacyOperationResult *async_result =
        new AsyncLegacyOperationResult(
        async->getKey(),
        async->getRouting(),
        op,
        response);

    _complete_op_node(op, ASYNC_OPSTATE_COMPLETE, 0, 0);

    PEG_METHOD_EXIT();
}

void ProviderManagerService::handleCreateSubscriptionRequest(AsyncOpNode *op, const Message * message) throw()
{
   PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "ProviderManagerService::handleCreateSubscriptionRequest");
    CIMCreateSubscriptionRequestMessage * request =
        dynamic_cast<CIMCreateSubscriptionRequestMessage *>(const_cast<Message *>(message));

    AsyncRequest *async = static_cast<AsyncRequest *>(op->_request.next(0));

    PEGASUS_ASSERT(request != 0 && async != 0 );

    CIMCreateSubscriptionResponseMessage * response =
        new CIMCreateSubscriptionResponseMessage(
        request->messageId,
        CIMException(),
        request->queueIds.copyAndPop());

    PEGASUS_ASSERT(response != 0);

    // preserve message key
    response->synch_response(request);

    OperationResponseHandler handler(request, response);

    try
    {
	// get the provider file name and logical name
	Triad<String, String, String> triad =
	    _getProviderRegPair(request->provider, request->providerModule);
	
	// get cached or load new provider module
	   
        //Provider provider =
	OpProviderHolder ph =
	    providerManager.getProvider(triad.first, triad.second, triad.third);

	// convert arguments
	OperationContext context;

	context.insert(IdentityContainer(request->userName));
	       context.insert(SubscriptionInstanceContainer
            (request->subscriptionInstance));
        context.insert(SubscriptionFilterConditionContainer
            (request->condition, request->queryLanguage));

// l10n
        context.insert(SubscriptionLanguageListContainer
	    (request->acceptLanguages));
	    
//l10n
	// add the langs to the context
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

	    Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
	                "ProviderManagerService::handleCreateSubscriptionRequest - Host name: $0  Name space: $1  Class name: $2",
	                System::getHostName(),
	   	        request->nameSpace.getString(),
 	                className.toString());

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

    AsyncLegacyOperationResult *async_result =
        new AsyncLegacyOperationResult(
        async->getKey(),
        async->getRouting(),
        op,
        response);

    _complete_op_node(op, ASYNC_OPSTATE_COMPLETE, 0, 0);
    PEG_METHOD_EXIT();
}

void ProviderManagerService::handleModifySubscriptionRequest(AsyncOpNode *op, const Message * message) throw()
{
   PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "ProviderManagerService::handleModifySubscriptionRequest");
    CIMModifySubscriptionRequestMessage * request =
        dynamic_cast<CIMModifySubscriptionRequestMessage *>(const_cast<Message *>(message));

    AsyncRequest *async = static_cast<AsyncRequest *>(op->_request.next(0));

    PEGASUS_ASSERT(request != 0 && async != 0 );

    CIMModifySubscriptionResponseMessage * response =
        new CIMModifySubscriptionResponseMessage(
        request->messageId,
        CIMException(),
        request->queueIds.copyAndPop());

    PEGASUS_ASSERT(response != 0);

    // preserve message key
    response->synch_response(request);

    OperationResponseHandler handler(request, response);

    try
    {
	// get the provider file name and logical name
	Triad<String, String, String> triad =
	    _getProviderRegPair(request->provider, request->providerModule);

	// get cached or load new provider module
        //Provider provider =
	OpProviderHolder ph =
            providerManager.getProvider(triad.first, triad.second, triad.third);

        // convert arguments
        OperationContext context;

        context.insert(IdentityContainer(request->userName));

        context.insert(SubscriptionInstanceContainer
            (request->subscriptionInstance));
        context.insert(SubscriptionFilterConditionContainer
            (request->condition, request->queryLanguage));

// l10n
        context.insert(SubscriptionLanguageListContainer
	    (request->acceptLanguages));    

//l10n
	// add the langs to the context
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

	    Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
	                "ProviderManagerService::handleGetInstanceRequest - Host name: $0  Name space: $1  Class name: $2",
	                System::getHostName(),
	   	        request->nameSpace.getString(),
 	                className.toString());

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

    AsyncLegacyOperationResult *async_result =
        new AsyncLegacyOperationResult(
        async->getKey(),
        async->getRouting(),
        op,
        response);

    _complete_op_node(op, ASYNC_OPSTATE_COMPLETE, 0, 0);
    PEG_METHOD_EXIT();
}

void ProviderManagerService::handleDeleteSubscriptionRequest(AsyncOpNode *op, const Message * message) throw()
{
   PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "ProviderManagerService::handleDeleteSubscriptionRequest");
   CIMDeleteSubscriptionRequestMessage * request =
        dynamic_cast<CIMDeleteSubscriptionRequestMessage *>(const_cast<Message *>(message));

    AsyncRequest *async = static_cast<AsyncRequest *>(op->_request.next(0));

    PEGASUS_ASSERT(request != 0 && async != 0 );

    CIMDeleteSubscriptionResponseMessage * response =
        new CIMDeleteSubscriptionResponseMessage(
        request->messageId,
        CIMException(),
        request->queueIds.copyAndPop());

    PEGASUS_ASSERT(response != 0);

    // preserve message key
    response->synch_response(request);

    OperationResponseHandler handler(request, response);

    try
    {
	// get the provider file name and logical name
	Triad<String, String, String> triad =
	    _getProviderRegPair(request->provider, request->providerModule);

	// get cached or load new provider module
        //Provider provider =
	OpProviderHolder ph =
            providerManager.getProvider(triad.first, triad.second, triad.third);

        // convert arguments
        OperationContext context;

        context.insert(IdentityContainer(request->userName));
        context.insert(SubscriptionInstanceContainer
            (request->subscriptionInstance));

// l10n
        context.insert(SubscriptionLanguageListContainer
	    (request->acceptLanguages));
		    
//l10n
	// add the langs to the context
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

	    Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
	                "ProviderManagerService::handleGetInstanceRequest - Host name: $0  Name space: $1  Class name: $2",
	                System::getHostName(),
	   	        request->nameSpace.getString(),
 	                className.toString());

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

    AsyncLegacyOperationResult *async_result =
        new AsyncLegacyOperationResult(
        async->getKey(),
        async->getRouting(),
        op,
        response);

    _complete_op_node(op, ASYNC_OPSTATE_COMPLETE, 0, 0);
    PEG_METHOD_EXIT();
}

void ProviderManagerService::handleEnableIndicationsRequest(AsyncOpNode *op, const Message * message) throw()
{
   PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "ProviderManagerService:: handleEnableIndicationsRequest");
    CIMEnableIndicationsRequestMessage * request =
        dynamic_cast<CIMEnableIndicationsRequestMessage *>(const_cast<Message *>(message));

    AsyncRequest *async = static_cast<AsyncRequest *>(op->_request.next(0));

    PEGASUS_ASSERT(request != 0 && async != 0 );

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
    response->synch_response(request);

    response->dest = request->queueIds.top();


    EnableIndicationsResponseHandler *handler = 
       new EnableIndicationsResponseHandler(request, response, 
           request->provider, this);

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
       											"ProviderManager.ProviderManagerService.INTERNAL_ERROR",
       											"Internal Error"));
    }
    catch(...)
    {
       PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4, 
			"Exception: Unknown");
		//l10n
       //response->cimException = CIMException(CIM_ERR_FAILED, "Unknown Error");
       response->cimException = CIMException(CIM_ERR_FAILED, MessageLoaderParms(
       											"ProviderManager.ProviderManagerService.UNKNOWN_ERROR",
       											"Unknown Error"));
    }
       
    AsyncLegacyOperationResult *async_result =
        new AsyncLegacyOperationResult(
        async->getKey(),
        async->getRouting(),
        op,
        response);

    _complete_op_node(op, ASYNC_OPSTATE_COMPLETE, 0, 0);
    PEG_METHOD_EXIT();
}

void ProviderManagerService::handleDisableIndicationsRequest(AsyncOpNode *op, const Message * message) throw()
{
   PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "ProviderManagerService::handleDisableIndicationsRequest");
    CIMDisableIndicationsRequestMessage * request =
        dynamic_cast<CIMDisableIndicationsRequestMessage *>(const_cast<Message *>(message));

    AsyncRequest *async = static_cast<AsyncRequest *>(op->_request.next(0));

    PEGASUS_ASSERT(request != 0 && async != 0 );

    CIMDisableIndicationsResponseMessage * response =
        new CIMDisableIndicationsResponseMessage(
        request->messageId,
        CIMException(),
        request->queueIds.copyAndPop());

    PEGASUS_ASSERT(response != 0);

    // preserve message key
    response->synch_response(request);

    OperationResponseHandler handler(request, response);

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
			 "Calling provider.disableIndications: " + 
			 ph.GetProvider().getName());
	
        ph.GetProvider().disableIndications();
	ph.GetProvider().unprotect();
	PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4, 
			 "Removing and Destroying indication handler for " + 
			 ph.GetProvider().getName());
	
	_removeEntry(_generateKey(ph.GetProvider()));
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
       											"ProviderManager.ProviderManagerService.INTERNAL_ERROR",
       											"Internal Error"));
    }
    catch(...)
    {
       PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4, 
			"Exception: Unknown");
			//l10n
       //response->cimException = CIMException(CIM_ERR_FAILED, "Unknown Error");
       response->cimException = CIMException(CIM_ERR_FAILED, MessageLoaderParms(
       											"ProviderManager.ProviderManagerService.UNKNOWN_ERROR",
       											"Unknown Error"));
    }

    AsyncLegacyOperationResult *async_result =
        new AsyncLegacyOperationResult(
        async->getKey(),
        async->getRouting(),
        op,
        response);

    _complete_op_node(op, ASYNC_OPSTATE_COMPLETE, 0, 0);
    PEG_METHOD_EXIT();
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
void ProviderManagerService::handleDisableModuleRequest(AsyncOpNode *op, const Message * message) throw()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
                     "ProviderManagerService::handleDisableModuleRequest");
    CIMDisableModuleRequestMessage * request =
        dynamic_cast<CIMDisableModuleRequestMessage *>(
            const_cast<Message *>(message));

    AsyncRequest *async = static_cast<AsyncRequest *>(op->_request.next(0));

    PEGASUS_ASSERT(request != 0 && async != 0);

    CIMException cimException;
    Array<Uint16> operationalStatus;

    try
    {
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
        if (!disableProviderOnly)
        {
            Uint32 pos2 = mInstance.findProperty(CIMName ("OperationalStatus"));
            if (pos2 != PEG_NOT_FOUND)
            {
                //
                //  ATTN-CAKG-P2-20020821: Check for null status?
                //
                mInstance.getProperty(pos2).getValue().get(operationalStatus);
            }

            //
            // update module status from OK to Stopping
            //
            for (Uint32 i=0, n = operationalStatus.size(); i < n; i++)
            {
                if (operationalStatus[i] == _MODULE_OK)
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
                throw PEGASUS_CIM_EXCEPTION_L(
                    CIM_ERR_FAILED,
                    MessageLoaderParms(
                        "ProviderManager.ProviderManagerService."
                            "SET_MODULE_STATUS_FAILED",
                        "set module status failed."));
            }
        }

        // Unload providers
        Array<CIMInstance> _pInstances = request->providers;
        Array<Boolean> _indicationProviders = request->indicationProviders;

        for(Uint32 i = 0, n = _pInstances.size(); i < n; i++)
        {
            // get the provider file name and logical name
            Triad<String, String, String> triad =
                _getProviderRegPair(_pInstances[i], mInstance);

            // It is an indication provider
            if (_indicationProviders[i])
            {
                Sint16 ret_value = providerManager.disableIndicationProvider(
                                      triad.first, triad.second);
                if (ret_value == 1)
                {
                    // remove the entry from the table since the indication
                    // provider has been disabled
                    _removeEntry(_generateKey(triad.second, triad.first));
                }
                else if (ret_value == 0)
                {
                    // disable failed since there are pending requests, 
                    // update module status from Stopping to OK
                    {
                        if (operationalStatus[i] == _MODULE_STOPPING)
                        {
                            operationalStatus.remove(i);
                        }
                    }

                    operationalStatus.append(_MODULE_OK);

                    if(_providerRegistrationManager->setProviderModuleStatus
                        (moduleName, operationalStatus) == false)
                    {
                        throw PEGASUS_CIM_EXCEPTION_L(
                            CIM_ERR_FAILED,
                            MessageLoaderParms(
                                "ProviderManager.ProviderManagerService."
                                    "SET_MODULE_STATUS_FAILED",
                                "set module status failed."));
                    }
                }
                else // disable failed for other reason, throw exception
                {
// L10N TODO
                    throw PEGASUS_CIM_EXCEPTION(
                        CIM_ERR_FAILED,
                        "Disable Indication Provider Failed.");
                }
            }
            else // not an indication provider
            {
                if (providerManager.disableProvider(
                        triad.first, triad.second) == 0)
                {
                    // disable failed since there are pending requests
                    // update module status from Stopping to OK
                    for(Uint32 i=0, n = operationalStatus.size(); i < n; i++)
                    {
                        if (operationalStatus[i] == _MODULE_STOPPING)
                        {
                            operationalStatus.remove(i);
                        }
                    }
  
                    operationalStatus.append(_MODULE_OK);

                    if(_providerRegistrationManager->setProviderModuleStatus
                        (moduleName, operationalStatus) == false)
                    {
                        //l10n
                        //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, 
                        //"set module status failed.");
                        throw PEGASUS_CIM_EXCEPTION_L(
                            CIM_ERR_FAILED,
                            MessageLoaderParms(
                                "ProviderManager.ProviderManagerService."
                                    "SET_MODULE_STATUS_FAILED",
                                "set module status failed."));
                    }
                }
            }
        }

        if (!disableProviderOnly)
        {
            // update module status from Stopping to Stopped
            for(Uint32 i=0, n = operationalStatus.size(); i < n; i++)
            {
                if (operationalStatus[i] == _MODULE_STOPPING)
                {
                    operationalStatus.remove(i);
                    operationalStatus.append(_MODULE_STOPPED);
                }
            }

            if(_providerRegistrationManager->setProviderModuleStatus
                (moduleName, operationalStatus) == false)
            {
                //l10n
                //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, 
                //"set module status failed.");
                throw PEGASUS_CIM_EXCEPTION_L(
                    CIM_ERR_FAILED,
                    MessageLoaderParms(
                        "ProviderManager.ProviderManagerService."
                            "SET_MODULE_STATUS_FAILED",
                        "set module status failed."));
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

    CIMDisableModuleResponseMessage * response =
        new CIMDisableModuleResponseMessage(
        request->messageId,
        cimException,
        request->queueIds.copyAndPop(),
        operationalStatus);

    PEGASUS_ASSERT(response != 0);

    // preserve message key
    response->synch_response(request);

    AsyncLegacyOperationResult *async_result =
        new AsyncLegacyOperationResult(
        async->getKey(),
        async->getRouting(),
        op,
        response);

    _complete_op_node(op, ASYNC_OPSTATE_COMPLETE, 0, 0);
    PEG_METHOD_EXIT();
}

void ProviderManagerService::handleEnableModuleRequest(AsyncOpNode *op, const Message * message) throw()
{
   PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "ProviderManagerService::handleEnableModuleRequest");
    CIMEnableModuleRequestMessage * request =
        dynamic_cast<CIMEnableModuleRequestMessage *>(const_cast<Message *>(message));

    AsyncRequest * async = static_cast<AsyncRequest *>(op->_request.next(0));

    PEGASUS_ASSERT(request != 0 && async != 0 );

    CIMException cimException;
    Array<Uint16> operationalStatus;

    try
    {
        //
        // get module status
        //
        CIMInstance mInstance = request->providerModule;
        Uint32 pos = mInstance.findProperty(CIMName ("OperationalStatus"));

        if (pos != PEG_NOT_FOUND)
        {
            //
            //  ATTN-CAKG-P2-20020821: Check for null status?
            //
            mInstance.getProperty(pos).getValue().get(operationalStatus);
        }

        // update module status from Stopped to OK
        for(Uint32 i=0, n = operationalStatus.size(); i < n; i++)
        {
            if (operationalStatus[i] == _MODULE_STOPPED)
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
        if (pos2 != PEG_NOT_FOUND)
        {
	    mInstance.getProperty(pos2).getValue().get(moduleName);
        }

        if(_providerRegistrationManager->setProviderModuleStatus
            (moduleName, operationalStatus) == false)
        {
    	    //l10n
            //throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "set module status failed.");
            throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED, MessageLoaderParms(
            				"ProviderManager.ProviderManagerService.SET_MODULE_STATUS_FAILED",
            				"set module status failed."));
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

    CIMEnableModuleResponseMessage * response =
        new CIMEnableModuleResponseMessage(
        request->messageId,
        cimException,
        request->queueIds.copyAndPop(),
        operationalStatus);

    PEGASUS_ASSERT(response != 0);

    // preserve message key
    response->synch_response(request);

    AsyncLegacyOperationResult *async_result =
        new AsyncLegacyOperationResult(
        async->getKey(),
        async->getRouting(),
        op,
        response);

    _complete_op_node(op, ASYNC_OPSTATE_COMPLETE, 0, 0);
    PEG_METHOD_EXIT();
}

void ProviderManagerService::handleStopAllProvidersRequest(AsyncOpNode *op, const
    Message * message) throw()
{
   PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "ProviderManagerService::handleStopAllProvidersRequest");
    CIMStopAllProvidersRequestMessage * request =
        dynamic_cast<CIMStopAllProvidersRequestMessage *>(const_cast<Message *>(message));

    AsyncRequest * async = static_cast<AsyncRequest *>(op->_request.next(0));

    PEGASUS_ASSERT(request != 0 && async != 0);

    //
    // tell the provider manager to shutdown all the providers
    //
    providerManager.shutdownAllProviders();

    CIMStopAllProvidersResponseMessage * response =
        new CIMStopAllProvidersResponseMessage(
        request->messageId,
        CIMException(),
        request->queueIds.copyAndPop());

    PEGASUS_ASSERT(response != 0);

    // preserve message key
    response->synch_response(request);

    AsyncLegacyOperationResult *async_result =
       new AsyncLegacyOperationResult(
          async->getKey(),
          async->getRouting(),
          op,
          response);

    _complete_op_node(op, ASYNC_OPSTATE_COMPLETE, 0, 0);
    PEG_METHOD_EXIT();
}

void ProviderManagerService::handleExportIndicationRequest(AsyncOpNode *op, 
				    const Message *message) throw()
{
   PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "ProviderManagerService::handlExportIndicationRequest");

   CIMExportIndicationRequestMessage *request = 
      dynamic_cast<CIMExportIndicationRequestMessage *>(const_cast<Message *>(message));
   
   AsyncRequest *async = static_cast<AsyncRequest *>(op->_request.next(0));
   
   PEGASUS_ASSERT(request != 0 && async != 0);
   
   CIMResponseMessage * response = 
      new CIMExportIndicationResponseMessage(
	 request->messageId,
	 CIMException(), 
	 request->queueIds.copyAndPop());
   
   PEGASUS_ASSERT(response != 0);
   
   response->synch_response(request);

   try
   {
      // get the provider file name and logical name
      Triad<String, String, String> triad =
	 _lookupConsumerProvider(request->destinationPath);
      
      // get cached or load new provider module
      OpProviderHolder ph = 
	 providerManager.getProvider(triad.first, triad.second, triad.third);

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
      context.insert(ContentLanguageListContainer(request->contentLanguages));	  

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
      response->cimException = CIMException(e);
   }
   catch(Exception & e)
   {
      PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4, 
		       "Exception: " + e.getMessage());
		       //l10n
      //response->cimException = CIMException(CIM_ERR_FAILED, "Internal Error");
      response->cimException = CIMException(CIM_ERR_FAILED, MessageLoaderParms(
      			"ProviderManager.ProviderManagerService.INTERNAL_ERROR",
      			"Internal Error"));
      
   }
   catch(...)
   {
      PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4, 
		       "Exception: Unknown");
		       //l10n
      //response->cimException = CIMException(CIM_ERR_FAILED, "Unknown Error");
      response->cimException = CIMException(CIM_ERR_FAILED, MessageLoaderParms(
      			"ProviderManager.ProviderManagerService.UNKNOWN_ERROR",
      			"Unknown Error"));
   }

   AsyncLegacyOperationResult *async_result = 
      new AsyncLegacyOperationResult(
	 async->getKey(), 
	 async->getRouting(), 
	 op, 
	 response);

   _complete_op_node(op, ASYNC_OPSTATE_COMPLETE, 0, 0);   
   PEG_METHOD_EXIT();
}


void ProviderManagerService::_insertEntry (
    const Provider & provider,
    const EnableIndicationsResponseHandler *handler)
{
    PEG_METHOD_ENTER (TRC_PROVIDERMANAGER,
                      "ProviderManagerService::_insertEntry");

    String tableKey = _generateKey 
        (provider);
    
    _responseTable.insert (tableKey, const_cast<EnableIndicationsResponseHandler *>(handler));

    PEG_METHOD_EXIT ();
}


void ProviderManagerService::_removeEntry(
   const String & key)
{
   PEG_METHOD_ENTER (TRC_PROVIDERMANAGER,
		     "ProviderManagerService::_removeEntry");
   EnableIndicationsResponseHandler *ret = 0;
   
   if (_responseTable.lookup(key, ret))
   {
	delete ret;
	_responseTable.remove(key);
   }
   PEG_METHOD_EXIT ();
}


String ProviderManagerService::_generateKey (
    const Provider & provider)
{
    String tableKey;

    PEG_METHOD_ENTER (TRC_PROVIDERMANAGER,
                      "ProviderManagerService::_generateKey");

    //
    //  Append provider key values to key
    //
    String providerName = provider.getName();
    String providerFileName = provider.getModule()->getFileName();
    tableKey.append (providerName);
    tableKey.append (providerFileName);

    PEG_METHOD_EXIT ();
    return tableKey;
}

String ProviderManagerService::_generateKey (
    const String & providerName,
    const String & providerFileName)
{
    String tableKey;

    PEG_METHOD_ENTER (TRC_PROVIDERMANAGER,
                      "ProviderManagerService::_generateKey");

    //
    //  Append providerName and providerFileName to key
    //
    tableKey.append (providerName);
    tableKey.append (providerFileName);

    PEG_METHOD_EXIT ();
    return tableKey;
}



PEGASUS_NAMESPACE_END
