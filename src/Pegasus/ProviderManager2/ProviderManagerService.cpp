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
//		        Yi Zhou, Hewlett-Packard Company (yi_zhou@hp.com)
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

// ATTN: this section is a temporary solution to populate the list of enabled
// provider managers for a given distribution. it includes another temporary
// solution for converting a generic file name into a file name useable by
// each platform.

// BEGIN TEMP SECTION
String _resolveFileName(const String & fileName)
{
    String temp;

    #if defined(PEGASUS_OS_TYPE_WINDOWS)
    temp = fileName + String(".dll");
    #elif defined(PEGASUS_OS_HPUX) && defined(PEGASUS_PLATFORM_HPUX_PARISC_ACC)
    temp = ConfigManager::getHomedPath(ConfigManager::getInstance()->getCurrentValue("providerDir"));
    temp.append(String("/lib") + fileName + String(".sl"));
    #elif defined(PEGASUS_OS_HPUX) && !defined(PEGASUS_PLATFORM_HPUX_PARISC_ACC)
    temp = ConfigManager::getHomedPath(ConfigManager::getInstance()->getCurrentValue("providerDir"));
    temp.append(String("/lib") + fileName + String(".so"));
    #elif defined(PEGASUS_OS_OS400)
    temp = fileName;
    #else
    temp = ConfigManager::getHomedPath(ConfigManager::getInstance()->getCurrentValue("providerDir"));
    temp.append(String("/lib") + fileName + String(".so"));
    #endif

    return(temp);
}

Array<Pair<String, String> > _initializeFileNames(void)
{
    Array<Pair<String, String> > temp;

    #if defined(ENABLE_DEFAULT_PROVIDER_MANAGER)
    #if defined(PEGASUS_OS_OS400)
    temp.append(Pair<String, String>(_resolveFileName("QSYS/QYCMDFTPVM"), String("DEFAULT")));
    #else
    temp.append(Pair<String, String>(_resolveFileName("DefaultProviderManager"), String("DEFAULT")));
    #endif
    #endif

    #if defined(ENABLE_CMPI_PROVIDER_MANAGER)
    #if defined(PEGASUS_OS_OS400)
    temp.append(Pair<String, String>(_resolveFileName("QSYS/QYCMCMPIPM"), String("CMPI")));
    #else
    temp.append(Pair<String, String>(_resolveFileName("CMPIProviderManager"), String("CMPI")));
    #endif
    #endif

    return(temp);
}

static const Array<Pair<String,String> > _fileNames = _initializeFileNames();
// END TEMP SECTION

inline Boolean _isSupportedRequestType(const Message * message)
{
    Boolean rc = false;

    if(message == 0)
    {
        return(rc);
    }

    /*
    // ATTN : need to determine valid request message types
    // before enabling.

    switch(message->getType())
    {
    case CIM_GET_INSTANCE_REQUEST_MESSAGE:
    case CIM_ENUMERATE_INSTANCES_REQUEST_MESSAGE:
    case CIM_ENUMERATE_INSTANCE_NAMES_REQUEST_MESSAGE:
    case CIM_CREATE_INSTANCE_REQUEST_MESSAGE:
    case CIM_MODIFY_INSTANCE_REQUEST_MESSAGE:
    case CIM_DELETE_INSTANCE_REQUEST_MESSAGE:
    case CIM_EXEC_QUERY_REQUEST_MESSAGE:
    case CIM_ASSOCIATORS_REQUEST_MESSAGE:
    case CIM_ASSOCIATOR_NAMES_REQUEST_MESSAGE:
    case CIM_REFERENCES_REQUEST_MESSAGE:
    case CIM_REFERENCE_NAMES_REQUEST_MESSAGE:
    case CIM_GET_PROPERTY_REQUEST_MESSAGE:
    case CIM_SET_PROPERTY_REQUEST_MESSAGE:
    case CIM_INVOKE_METHOD_REQUEST_MESSAGE:
    case CIM_CREATE_SUBSCRIPTION_REQUEST_MESSAGE:
    case CIM_MODIFY_SUBSCRIPTION_REQUEST_MESSAGE:
    case CIM_DELETE_SUBSCRIPTION_REQUEST_MESSAGE:
    case CIM_ENABLE_INDICATIONS_REQUEST_MESSAGE:
    case CIM_DISABLE_INDICATIONS_REQUEST_MESSAGE:
    case CIM_DISABLE_MODULE_REQUEST_MESSAGE:
    case CIM_ENABLE_MODULE_REQUEST_MESSAGE:
    case CIM_STOP_ALL_PROVIDERS_REQUEST_MESSAGE:
    case CIM_CONSUME_INDICATION_REQUEST_MESSAGE:
        rc = true;

        break;
    default:
        rc = false;

        break;
    }
    */

    rc = true;

    return(rc);
}

inline Boolean _isSupportedResponseType(const Message * message)
{
    Boolean rc = false;

    return(rc);
}

ProviderManagerService::ProviderManagerService(void)
    : MessageQueueService(PEGASUS_QUEUENAME_PROVIDERMANAGER_CPP)
{
}

ProviderManagerService::ProviderManagerService(ProviderRegistrationManager * providerRegistrationManager)
    : MessageQueueService(PEGASUS_QUEUENAME_PROVIDERMANAGER_CPP)
{
    for(Uint32 i = 0, n = _fileNames.size(); i < n; i++)
    {
        String message;

        message = "ProviderManagerService::ProviderManagerService() loading " +
            _fileNames[i].first + "(" + _fileNames[i].second + ")";

        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4, message);

        try
        {
            ProviderManagerModule module(_fileNames[i].first);

            if(module.load() == false)
            {
                throw 0;    // ATTN: inefficient
            }

            ProviderManager * manager = module.getProviderManager(_fileNames[i].second);

            if(manager == 0)
            {
                throw 0;    // ATTN: inefficient
            }

            // ATTN: only set the hacked/cached provider registration manager pointer after the
            // DEFAULT provider manager is loaded.
            if(String::equalNoCase(_fileNames[i].second, "DEFAULT"))
            {
                manager->setProviderRegistrationManager(providerRegistrationManager);
            }

            _providerManagers.append(Pair<ProviderManager *, ProviderManagerModule>(manager, module));
        }
        catch(...)
        {
            message = "ProviderManagerService::ProviderManagerService() exception loading " +
                _fileNames[i].first + "(" + _fileNames[i].second + ")";

            PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4, message);
        }
    }
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

    // find provider manager
    // ATTN: implement efficient lookup
    ProviderManager * manager = _providerManagers[0].first;

    try
    {
        PEG_TRACE_STRING(TRC_PROVIDERMANAGER, Tracer::LEVEL4,
            "ProviderManagerService::handleCimRequest() passing control to provider manager.");

        // forward request
        response = manager->processMessage(request);
    }
    catch(...)
    {
        // ATTN: create response with error message
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
