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
//		Yi Zhou, Hewlett-Packard Company (yi_zhou@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include "ProviderManagerService.h"

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/Logger.h>

#include <Pegasus/Common/Destroyer.h>

PEGASUS_NAMESPACE_BEGIN

ProviderManagerService::ProviderManagerService(void)
    : MessageQueueService(PEGASUS_QUEUENAME_PROVIDERMANAGER_CPP)
{
}

ProviderManagerService::ProviderManagerService(ProviderRegistrationManager * providerRegistrationManager)
    : MessageQueueService(PEGASUS_QUEUENAME_PROVIDERMANAGER_CPP)
{
    try
    {
        ProviderManagerModule module("DefaultProviderManager");

        // ATTN: ensure module loaded
        module.load();

        // ATTN: ensure entry point returned valid response
        ProviderManager * manager = module.getProviderManager("Default");

        // ATTN: only set the hacked/cached provider registration manager pointer after the module
        // has loaded.
        manager->setProviderRegistrationManager(providerRegistrationManager);

        _providerManagers.append(Pair<ProviderManager *, ProviderManagerModule>(manager, module));
    }
    catch(...)
    {
    }
}

ProviderManagerService::~ProviderManagerService(void)
{
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

    // get the service from argument
    ProviderManagerService * service = reinterpret_cast<ProviderManagerService *>(arg);

    PEGASUS_ASSERT(service != 0);

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

    AsyncRequest * request = static_cast<AsyncRequest *>(op->_request.next(0));

    PEGASUS_ASSERT(request != 0);

    if(request->getType() != async_messages::ASYNC_LEGACY_OP_START)
    {
        // reply with NAK

        PEG_METHOD_EXIT();

        return(PEGASUS_THREAD_RETURN(0));
    }

    Message * legacy = static_cast<AsyncLegacyOperationStart *>(request)->get_action();

    if(legacy != 0)
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

        // only pass valid message types to provider managers
        switch(legacy->getType())
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
            service->handleCimRequest(op, legacy);
            break;

        default:
            // unsupported messages are ignored
            break;
        }
    }

    PEG_METHOD_EXIT();

    return(PEGASUS_THREAD_RETURN(0));
}

void ProviderManagerService::handleCimRequest(AsyncOpNode * op, const Message * message) throw()
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
