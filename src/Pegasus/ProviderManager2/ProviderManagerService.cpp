//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000 - 2003 BMC Software, Hewlett-Packard Company, IBM,
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
//              Mike Day, IBM (mdday@us.ibm.com)
//              Karl Schopmeyer(k.schopmeyer@opengroup.org) - Fix associators.
//              Yi Zhou, Hewlett-Packard Company (yi_zhou@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include "ProviderManagerService.h"

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/Logger.h>
#include <Pegasus/Common/Destroyer.h>

#include <Pegasus/Config/ConfigManager.h>

PEGASUS_NAMESPACE_BEGIN

// BEGIN TEMP SECTION
class ProviderManagerContainer
{
public:
    ProviderManagerContainer(const String & physicalName, const String & logicalName, const String & interfaceName)
    {
        #if defined(PEGASUS_OS_TYPE_WINDOWS)
        _physicalName = physicalName + String(".dll");
        #elif defined(PEGASUS_OS_HPUX) && defined(PEGASUS_PLATFORM_HPUX_PARISC_ACC)
        _physicalName = ConfigManager::getHomedPath(ConfigManager::getInstance()->getCurrentValue("providerDir"));
        _physicalName.append(String("/lib") + physical + String(".sl"));
        #elif defined(PEGASUS_OS_HPUX) && !defined(PEGASUS_PLATFORM_HPUX_PARISC_ACC)
        _physicalName = ConfigManager::getHomedPath(ConfigManager::getInstance()->getCurrentValue("providerDir"));
        _physicalName.append(String("/lib") + physical + String(".so"));
        #elif defined(PEGASUS_OS_OS400)
        _physicalName = physicalName;
        #else
        _physicalName = ConfigManager::getHomedPath(ConfigManager::getInstance()->getCurrentValue("providerDir"));
        _physicalName.append(String("/lib") + physical + String(".so"));
        #endif

        _logicalName = logicalName;

        _interfaceName = interfaceName;

        _module = ProviderManagerModule(_physicalName);

        _module.load();

        _manager = _module.getProviderManager(_logicalName);
    }

    ~ProviderManagerContainer(void)
    {
        _module.unload();
    }

    ProviderManager & getProviderManager(void)
    {
        return(*_manager);
    }

    String & getPhysicalName(void)
    {
        return(_physicalName);
    }

    String & getLogicalName(void)
    {
        return(_logicalName);
    }

    String & getInterfaceName(void)
    {
        return(_interfaceName);
    }

private:
    String _physicalName;
    String _logicalName;
    String _interfaceName;

    ProviderManagerModule _module;
    ProviderManager * _manager;

};

static Array<ProviderManagerContainer> _providerManagers;
// END TEMP SECTION

inline Boolean _isSupportedRequestType(const Message * message)
{
    // ATTN: needs implementation

    // for now, assume all requests are valid

    return(true);
}

inline Boolean _isSupportedResponseType(const Message * message)
{
    // ATTN: needs implementation

    // for now, assume all responses are invalid

    return(false);
}

ProviderManagerService::ProviderManagerService(void)
    : MessageQueueService(PEGASUS_QUEUENAME_PROVIDERMANAGER_CPP)
{
}

ProviderManagerService::ProviderManagerService(ProviderRegistrationManager * providerRegistrationManager)
    : MessageQueueService(PEGASUS_QUEUENAME_PROVIDERMANAGER_CPP)
{
    SetProviderRegistrationManager(providerRegistrationManager);

    // ATTN: this section is a temporary solution to populate the list of enabled
    // provider managers for a given distribution. it includes another temporary
    // solution for converting a generic file name into a file name useable by
    // each platform.

    // BEGIN TEMP SECTION
    //#if defined(PEGASUS_OS_OS400)
    //_providerManagers.append(ProviderManagerContainer("QSYS/??????????", "INTERNAL", "INTERNAL"));
    //#else
    //_providerManager.append(ProviderManagerContainer("InternalProviderManager", "DEFAULT", "INTERNAL"));
    //#endif

    #if defined(ENABLE_DEFAULT_PROVIDER_MANAGER)
    #if defined(PEGASUS_OS_OS400)
    _providerManagers.append(ProviderManagerContainer("QSYS/QYCMDFTPVM", "DEFAULT", "C++Default"));
    #else
    _providerManagers.append(ProviderManagerContainer("DefaultProviderManager", "DEFAULT", "C++Default"));
    #endif
    #endif

    #if defined(ENABLE_CMPI_PROVIDER_MANAGER)
    #if defined(PEGASUS_OS_OS400)
    _providerManagers.append(ProviderManagerContainer("QSYS/QYCMCMPIPM", "CMPI", "CMPI"));
    #else
    _providerManagers.append(ProviderManagerContainer("CMPIProviderManager", "CMPI", "CMPI"));
    #endif
    #endif
    // END TEMP SECTION
}

ProviderManagerService::~ProviderManagerService(void)
{
}

Boolean ProviderManagerService::messageOK(const Message * message)
{
    PEGASUS_ASSERT(message != 0);

    if(_isSupportedRequestType(message))
    {
        return(MessageQueueService::messageOK(message));
    }

    return(false);
}

void ProviderManagerService::handleEnqueue(void)
{
    Message * message = dequeue();

    handleEnqueue(message);
}

void ProviderManagerService::handleEnqueue(Message * message)
{
    PEGASUS_ASSERT(message != 0);

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

    PEGASUS_ASSERT((request != 0) && (request->op != 0));

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

    if(service->_incomingQueue.size() == 0)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "ProviderManagerService::handleCimOperation() called with no op node in queue" );

        PEG_METHOD_EXIT();

        // thread started with no message in queue.
        return(PEGASUS_THREAD_RETURN(1));
    }

    AsyncOpNode * op = service->_incomingQueue.dequeue();

    PEGASUS_ASSERT(op != 0 );

    if(op->_request.count() == 0)
    {
        MessageQueue * queue = MessageQueue::lookup(op->_source_queue);

        PEGASUS_ASSERT(queue != 0);

        PEG_METHOD_EXIT();

        // no request in op node
        return(PEGASUS_THREAD_RETURN(1));
    }

    return(0);
}
*/

PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL ProviderManagerService::handleCimOperation(void * arg) throw()
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "ProviderManagerService::handleCimOperation");

    if(arg == 0)
    {
        // thread started with invalid argument.
        return(PEGASUS_THREAD_RETURN(1));
    }

    // get the service from argument
    ProviderManagerService * service = reinterpret_cast<ProviderManagerService *>(arg);

    if(service->_incomingQueue.size() == 0)
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "ProviderManagerService::handleCimOperation() called with no op node in queue" );

        PEG_METHOD_EXIT();

        // thread started with no message in queue.
        return(PEGASUS_THREAD_RETURN(1));
    }

    AsyncOpNode * op = service->_incomingQueue.dequeue();

    if((op == 0) || (op->_request.count() == 0))
    {
        MessageQueue * queue = MessageQueue::lookup(op->_source_queue);

        PEGASUS_ASSERT(queue != 0);

        PEG_METHOD_EXIT();

        // no request in op node
        return(PEGASUS_THREAD_RETURN(1));
    }

    AsyncRequest * request = static_cast<AsyncRequest *>(op->_request.next(0));

    if((request == 0) || (request->getType() != async_messages::ASYNC_LEGACY_OP_START))
    {
        // reply with NAK

        PEG_METHOD_EXIT();

        return(PEGASUS_THREAD_RETURN(0));
    }

    try
    {
        Message * legacy = static_cast<AsyncLegacyOperationStart *>(request)->get_action();

        if(_isSupportedRequestType(legacy))
        {
            Destroyer<Message> xmessage(legacy);

            // Set the client's requested language into this service thread.
            // This will allow functions in this service to return messages
            // in the correct language.
            CIMMessage * msg = dynamic_cast<CIMMessage *>(legacy);

            if(msg != 0)
            {
                AcceptLanguages * langs = new AcceptLanguages(msg->acceptLanguages);

                Thread::setLanguages(langs);
            }
            else
            {
                Thread::clearLanguages();
            }

            service->handleCimRequest(op, legacy);
        }
    }
    catch(...)
    {
        // ATTN: log error
    }

    PEG_METHOD_EXIT();

    return(PEGASUS_THREAD_RETURN(0));
}

void ProviderManagerService::handleCimRequest(AsyncOpNode * op, const Message * message)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER, "ProviderManagerService::handleCimRequest");

    // ATTN: ensure message is a request???
    CIMMessage * request = dynamic_cast<CIMMessage *>(const_cast<Message *>(message));

    // get request from op node
    AsyncRequest * async = static_cast<AsyncRequest *>(op->_request.next(0));

    PEGASUS_ASSERT((request != 0) && (async != 0));

    Message * response = 0;

    // get namespace and class name from message
    String nameSpace;
    String className;

    switch(message->getType())
    {
    case CIM_GET_CLASS_REQUEST_MESSAGE:
        {
            const CIMGetClassRequestMessage * p = dynamic_cast<const CIMGetClassRequestMessage *>(message);

            PEGASUS_ASSERT(p != 0);

            nameSpace = p->nameSpace.getString();
            className = p->className.getString();
        }

        break;
    case CIM_ENUMERATE_CLASSES_REQUEST_MESSAGE:
        {
            const CIMEnumerateClassesRequestMessage * p = dynamic_cast<const CIMEnumerateClassesRequestMessage *>(message);

            PEGASUS_ASSERT(p != 0);

            nameSpace = p->nameSpace.getString();
            className = p->className.getString();
        }

        break;
    case CIM_ENUMERATE_CLASS_NAMES_REQUEST_MESSAGE:
        {
            const CIMEnumerateClassNamesRequestMessage * p = dynamic_cast<const CIMEnumerateClassNamesRequestMessage *>(message);

            PEGASUS_ASSERT(p != 0);

            nameSpace = p->nameSpace.getString();
            className = p->className.getString();
        }

        break;
    case CIM_CREATE_CLASS_REQUEST_MESSAGE:
        {
            const CIMCreateClassRequestMessage * p = dynamic_cast<const CIMCreateClassRequestMessage *>(message);

            PEGASUS_ASSERT(p != 0);

            nameSpace = p->nameSpace.getString();
            //className = p->className.getString();
        }

        break;
    case CIM_MODIFY_CLASS_REQUEST_MESSAGE:
        {
            const CIMModifyClassRequestMessage * p = dynamic_cast<const CIMModifyClassRequestMessage *>(message);

            PEGASUS_ASSERT(p != 0);

            nameSpace = p->nameSpace.getString();
            //className = p->className.getString();
        }

        break;
    case CIM_DELETE_CLASS_REQUEST_MESSAGE:
        {
            const CIMDeleteClassRequestMessage * p = dynamic_cast<const CIMDeleteClassRequestMessage *>(message);

            PEGASUS_ASSERT(p != 0);

            nameSpace = p->nameSpace.getString();
            className = p->className.getString();
        }

        break;
    case CIM_GET_INSTANCE_REQUEST_MESSAGE:
        {
            const CIMGetInstanceRequestMessage * p = dynamic_cast<const CIMGetInstanceRequestMessage *>(message);

            PEGASUS_ASSERT(p != 0);

            nameSpace = p->nameSpace.getString();
            //className = p->className.getString();
        }

        break;
    case CIM_ENUMERATE_INSTANCES_REQUEST_MESSAGE:
        {
            const CIMEnumerateInstancesRequestMessage * p = dynamic_cast<const CIMEnumerateInstancesRequestMessage *>(message);

            PEGASUS_ASSERT(p != 0);

            nameSpace = p->nameSpace.getString();
            className = p->className.getString();
        }

        break;
    case CIM_ENUMERATE_INSTANCE_NAMES_REQUEST_MESSAGE:
        {
            const CIMEnumerateInstanceNamesRequestMessage * p = dynamic_cast<const CIMEnumerateInstanceNamesRequestMessage *>(message);

            PEGASUS_ASSERT(p != 0);

            nameSpace = p->nameSpace.getString();
            className = p->className.getString();
        }

        break;
    case CIM_CREATE_INSTANCE_REQUEST_MESSAGE:
        {
            const CIMCreateInstanceRequestMessage * p = dynamic_cast<const CIMCreateInstanceRequestMessage *>(message);

            PEGASUS_ASSERT(p != 0);

            nameSpace = p->nameSpace.getString();
            //className = p->className.getString();
        }

        break;
    case CIM_MODIFY_INSTANCE_REQUEST_MESSAGE:
        {
            const CIMModifyInstanceRequestMessage * p = dynamic_cast<const CIMModifyInstanceRequestMessage *>(message);

            PEGASUS_ASSERT(p != 0);

            nameSpace = p->nameSpace.getString();
            //className = p->className.getString();
        }

        break;
    case CIM_DELETE_INSTANCE_REQUEST_MESSAGE:
        {
            const CIMDeleteInstanceRequestMessage * p = dynamic_cast<const CIMDeleteInstanceRequestMessage *>(message);

            PEGASUS_ASSERT(p != 0);

            nameSpace = p->nameSpace.getString();
            //className = p->className.getString();
        }

        break;
    case CIM_EXEC_QUERY_REQUEST_MESSAGE:
        break;
    case CIM_ASSOCIATORS_REQUEST_MESSAGE:
        {
            const CIMAssociatorsRequestMessage * p = dynamic_cast<const CIMAssociatorsRequestMessage *>(message);

            PEGASUS_ASSERT(p != 0);

            nameSpace = p->nameSpace.getString();
            //className = p->className.getString();
        }

        break;
    case CIM_ASSOCIATOR_NAMES_REQUEST_MESSAGE:
        {
            const CIMAssociatorNamesRequestMessage * p = dynamic_cast<const CIMAssociatorNamesRequestMessage *>(message);

            PEGASUS_ASSERT(p != 0);

            nameSpace = p->nameSpace.getString();
            //className = p->className.getString();
        }

        break;
    case CIM_REFERENCES_REQUEST_MESSAGE:
        {
            const CIMReferencesRequestMessage * p = dynamic_cast<const CIMReferencesRequestMessage *>(message);

            PEGASUS_ASSERT(p != 0);

            nameSpace = p->nameSpace.getString();
            //className = p->className.getString();
        }

        break;
    case CIM_REFERENCE_NAMES_REQUEST_MESSAGE:
        {
            const CIMReferenceNamesRequestMessage * p = dynamic_cast<const CIMReferenceNamesRequestMessage *>(message);

            PEGASUS_ASSERT(p != 0);

            nameSpace = p->nameSpace.getString();
            //className = p->className.getString();
        }

        break;
    case CIM_GET_PROPERTY_REQUEST_MESSAGE:
        {
            const CIMGetPropertyRequestMessage * p = dynamic_cast<const CIMGetPropertyRequestMessage *>(message);

            PEGASUS_ASSERT(p != 0);

            nameSpace = p->nameSpace.getString();
            //className = p->className.getString();
        }

        break;
    case CIM_SET_PROPERTY_REQUEST_MESSAGE:
        {
            const CIMSetPropertyRequestMessage * p = dynamic_cast<const CIMSetPropertyRequestMessage *>(message);

            PEGASUS_ASSERT(p != 0);

            nameSpace = p->nameSpace.getString();
            //className = p->className.getString();
        }

        break;
    case CIM_INVOKE_METHOD_REQUEST_MESSAGE:
        {
            const CIMInvokeMethodRequestMessage * p = dynamic_cast<const CIMInvokeMethodRequestMessage *>(message);

            PEGASUS_ASSERT(p != 0);

            nameSpace = p->nameSpace.getString();
            //className = p->className.getString();
        }

        break;
    /*
    case CIM_ENABLE_INDICATION_SUBSCRIPTION_REQUEST_MESSAGE:
        {
            const CIMEnableIndicationsSubscriptionRequestMessage * p = dynamic_cast<const CIMEnableIndicationsRequestMessage *>(message);

            PEGASUS_ASSERT(p != 0);

            nameSpace = p->nameSpace.getString();
            className = p->className.getString();
        }

        break;
    case CIM_MODIFY_INDICATION_SUBSCRIPTION_REQUEST_MESSAGE:
        {
            const CIMModifyIndicationsRequestMessage * p = dynamic_cast<const CIMModifyIndicationsRequestMessage *>(message);

            PEGASUS_ASSERT(p != 0);

            nameSpace = p->nameSpace.getString();
            className = p->className.getString();
        }

        break;
    case CIM_DISABLE_INDICATION_SUBSCRIPTION_REQUEST_MESSAGE:
        {
            const CIMGetClassRequestMessage * p = dynamic_cast<const CIMGetClassRequestMessage *>(message);

            PEGASUS_ASSERT(p != 0);

            nameSpace = p->nameSpace.getString();
            className = p->className.getString();
        }

        break;
    */
    default:
        break;
    }

    ProviderName name(
        CIMObjectPath(String::EMPTY, nameSpace, className).toString(),
        String::EMPTY,
        String::EMPTY,
        String::EMPTY,
        0);

    // find provider manager
    name = ProviderRegistrar().findProvider(name);

    // find provider manager for provider interface
    for(Uint32 i = 0, n = _providerManagers.size(); i < n; i++)
    {
        if(String::equalNoCase(name.getInterfaceName(), _providerManagers[i].getInterfaceName()))
        {
            try
            {
                PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
                    "ProviderManagerService::handleCimRequest() passing control to provider manager.");

                // forward request
                response = _providerManagers[0].getProviderManager().processMessage(request);
            }
            catch(...)
            {
                // ATTN: create response with error message
            }

            break;
        }
    }

    // preserve message key
    response->setKey(request->getKey());

    // set HTTP method in response from request
    response->setHttpMethod(request->getHttpMethod());

    AsyncLegacyOperationResult * async_result =
        new AsyncLegacyOperationResult(
        async->getKey(),
        async->getRouting(),
        op,
        response);

    _complete_op_node(op, ASYNC_OPSTATE_COMPLETE, 0, 0);

    PEG_METHOD_EXIT();
}

void ProviderManagerService::unload_idle_providers(void)
{
}

PEGASUS_NAMESPACE_END

