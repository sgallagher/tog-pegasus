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

#include "ModuleController.h"
#include <Pegasus/Common/MessageLoader.h>
#include <Pegasus/Common/InternalException.h>

PEGASUS_NAMESPACE_BEGIN

PEGASUS_USING_STD;

RegisteredModuleHandle::RegisteredModuleHandle(
    const String& name,
    void* module_address,
    Message* (*receive_message)(Message *, void *),
    void (*async_callback)(Uint32, Message *, void *))
    : _name(name),
      _module_address(module_address),
      _module_receive_message(receive_message),
      _async_callback(async_callback)
{
    PEGASUS_ASSERT(_module_receive_message != 0);
}

RegisteredModuleHandle::~RegisteredModuleHandle()
{
}

const String & RegisteredModuleHandle::get_name() const
{
    return _name;
}

Message* RegisteredModuleHandle::_receive_message(Message* msg)
{
    return _module_receive_message(msg, _module_address);
}

void RegisteredModuleHandle::_send_async_callback(
    Uint32 msg_handle,
    Message* msg,
    void* parm)
{
    // ATTN: Assert this condition?
    if (_async_callback == 0)
    {
        throw NotImplemented("Module Async Receive");
    }

    _async_callback(msg_handle, msg, parm);
}


ModuleController::ModuleController(const char* name)
    : Base(name, MessageQueue::getNextQueueId(),
           module_capabilities::module_controller |
           module_capabilities::async),
      _modules()
{
}

ModuleController::~ModuleController()
{
    RegisteredModuleHandle* module;

    try
    {
        module = _modules.remove_front();
        while (module)
        {
            delete module;
            module = _modules.remove_front();
        }
    }
    catch (...)
    {
    }
}

// called by a module to register itself, returns a handle to the controller
ModuleController& ModuleController::register_module(
    const String& controller_name,
    const String& module_name,
    void* module_address,
    Message* (*receive_message)(Message *, void *),
    void (*async_callback)(Uint32, Message *, void *),
    RegisteredModuleHandle** instance)
{
    RegisteredModuleHandle *module;
    ModuleController *controller;

    Array<Uint32> services;

    MessageQueue *message_queue =
        MessageQueue::lookup(controller_name.getCString());

    if ((message_queue == NULL) || (!message_queue->isAsync()))
    {
        throw IncompatibleTypesException();
    }

    MessageQueueService *service =
        static_cast<MessageQueueService *>(message_queue);
    if ((service == NULL) ||
        !(service->get_capabilities() & module_capabilities::module_controller))
    {
        throw IncompatibleTypesException();
    }

    controller = static_cast<ModuleController *>(service);

    {

        // see if the module already exists in this controller.
        _module_lock lock(&(controller->_modules));

        module = controller->_modules.front();
        while (module != NULL)
        {
            if (module->get_name() == module_name)
            {
                MessageLoaderParms parms(
                    "Common.ModuleController.MODULE",
                    "module \"$0\"",
                    module_name);
                throw AlreadyExistsException(parms);
            }
            module = controller->_modules.next_of(module);
        }
    }

    // now reserve this module name with the meta dispatcher

    Uint32 result = 0;
    AutoPtr<RegisteredModule> request(new RegisteredModule(
        0,
        true,
        controller->getQueueId(),
        module_name));

    request->dest = CIMOM_Q_ID;

    AutoPtr<AsyncReply> response(controller->SendWait(request.get()));
    if (response.get() != NULL)
        result = response->result;

    request.reset();
    response.reset();
    if (result == async_results::MODULE_ALREADY_REGISTERED)
    {
        MessageLoaderParms parms(
            "Common.ModuleController.MODULE",
            "module \"$0\"", module_name);
        throw AlreadyExistsException(parms);
    }

    // the module does not exist, go ahead and create it.
    module = new RegisteredModuleHandle(
        module_name,
        module_address,
        receive_message,
        async_callback);

    controller->_modules.insert_back(module);

    if (instance != NULL)
        *instance = module;

    return *controller;
}


Boolean ModuleController::deregister_module(const String& module_name)
{
    AutoPtr<DeRegisteredModule> request(new DeRegisteredModule(
        0,
        true,
        getQueueId(),
        module_name));
    request->dest = _meta_dispatcher->getQueueId();

    AutoPtr<AsyncReply> response(SendWait(request.get()));

    request.reset();
    response.reset();

    RegisteredModuleHandle* module;

    _module_lock lock(&_modules);
    module = _modules.front();
    while (module != NULL)
    {
        if (module->get_name() == module_name)
        {
            _modules.remove(module);
            return true;
        }
        module = _modules.next_of(module);
    }
    return false;
}

void ModuleController::verify_handle(RegisteredModuleHandle* handle)
{
    RegisteredModuleHandle *module;

    if (handle->_module_address == (void *)this)
        return;

    _module_lock lock(&_modules);

    module = _modules.front();
    while (module != NULL)
    {
        if ( module == handle)
        {
            return;
        }
        module = _modules.next_of(module);
    }

    throw IPCException(Threads::self());
}

// given a name, find a service's queue id
Uint32 ModuleController::find_service(
    const RegisteredModuleHandle& handle,
    const String& name)
{
    verify_handle(const_cast<RegisteredModuleHandle *>(&handle));
    Array<Uint32> services;
    Base::find_services(name, 0, 0, &services);
    return services[0];
}


// returns the queue ID of the service hosting the named module,
// zero otherwise

Uint32 ModuleController::find_module_in_service(
    const RegisteredModuleHandle& handle,
    const String& name)
{
    verify_handle(const_cast<RegisteredModuleHandle *>(&handle));

    Uint32 result = 0;

    AutoPtr<FindModuleInService> request(new FindModuleInService(
        0,
        true,
        _meta_dispatcher->getQueueId(),
        name));
    request->dest = _meta_dispatcher->getQueueId();
    AutoPtr<FindModuleInServiceResponse> response(
        static_cast<FindModuleInServiceResponse *>(SendWait(request.get())));
    if (response.get() != NULL)
        result = response->_module_service_queue;

    return result;
}


AsyncReply* ModuleController::_send_wait(
    Uint32 destination_q,
    AsyncRequest* request)
{
    request->dest = destination_q;
    AsyncReply* reply = Base::SendWait(request);
    return reply;
}


// sendwait to another service
AsyncReply* ModuleController::ModuleSendWait(
    const RegisteredModuleHandle& handle,
    Uint32 destination_q,
    AsyncRequest* request)
{
    verify_handle(const_cast<RegisteredModuleHandle *>(&handle));

    return _send_wait(destination_q, request);
}

AsyncReply* ModuleController::_send_wait(
    Uint32 destination_q,
    const String& destination_module,
    AsyncRequest* message)
{
    AutoPtr<AsyncModuleOperationStart> request(new AsyncModuleOperationStart(
        0,
        destination_q,
        getQueueId(),
        true,
        destination_module,
        message));

    request->dest = destination_q;
    AutoPtr<AsyncModuleOperationResult> response(
        static_cast<AsyncModuleOperationResult *>(SendWait(request.get())));

    AsyncReply *ret = 0;

    if (response.get() != NULL &&
        response->getType() == ASYNC_ASYNC_MODULE_OP_RESULT)
    {
        // clear the request out of the envelope so it can be deleted by the
        // module
        ret = static_cast<AsyncReply *>(response->get_result());
    }
    request->get_action();
    return ret;
}


// sendwait to another module controlled by another service.
AsyncReply* ModuleController::ModuleSendWait(
    const RegisteredModuleHandle& handle,
    Uint32 destination_q,
    const String& destination_module,
    AsyncRequest* message)
{
    verify_handle(const_cast<RegisteredModuleHandle *>(&handle));

    return _send_wait(destination_q, destination_module, message);
}

void ModuleController::_async_handleEnqueue(
    AsyncOpNode* op,
    MessageQueue* q,
    void* parm)
{
    ModuleController* myself = static_cast<ModuleController *>(q);
    Message* request = op->removeRequest();
    Message* response = op->removeResponse();

    if (request && (!(request->getMask() & MessageMask::ha_async)))
        throw TypeMismatchException();

    if (response && (!(response->getMask() & MessageMask::ha_async)))
        throw TypeMismatchException();

    op->release();
    myself->return_op(op);

    // get rid of the module wrapper
    if (request && request->getType() == ASYNC_ASYNC_MODULE_OP_START)
    {
        (static_cast<AsyncMessage *>(request))->op = NULL;
        AsyncModuleOperationStart *rq =
            static_cast<AsyncModuleOperationStart *>(request);
        request = rq->get_action();
        delete rq;
    }

    // get rid of the module wrapper
    if (response &&
        response->getType() == ASYNC_ASYNC_MODULE_OP_RESULT)
    {
        (static_cast<AsyncMessage *>(response))->op = NULL;
        AsyncModuleOperationResult *rp =
            static_cast<AsyncModuleOperationResult *>(response);
        response = rp->get_result();
        delete rp;
    }

    callback_handle *cb = reinterpret_cast<callback_handle *>(parm);

    cb->_module->_send_async_callback(0, response, cb->_parm);
    delete cb;
}


// send an async message to a service asynchronously
Boolean ModuleController::ModuleSendAsync(
    const RegisteredModuleHandle& handle,
    Uint32 msg_handle,
    Uint32 destination_q,
    AsyncRequest* message,
    void* callback_parm)
{
    verify_handle(const_cast<RegisteredModuleHandle *>(&handle));

    if (message->op == NULL)
    {
        message->op = get_op();
        message->op->setRequest(message);
    }


    callback_handle *cb = new callback_handle(
        const_cast<RegisteredModuleHandle *>(&handle),
        callback_parm);

    message->resp = getQueueId();
    message->block = false;
    message->dest = destination_q;
    return SendAsync(
        message->op,
        destination_q,
        _async_handleEnqueue,
        this,
        cb);
}

// send a message to a module within another service asynchronously
Boolean ModuleController::ModuleSendAsync(
    const RegisteredModuleHandle& handle,
    Uint32 msg_handle,
    Uint32 destination_q,
    const String& destination_module,
    AsyncRequest* message,
    void* callback_parm)
{
    verify_handle(const_cast<RegisteredModuleHandle *>(&handle));

    AsyncOpNode *op = get_op();
    AsyncModuleOperationStart *request = new AsyncModuleOperationStart(
        op,
        destination_q,
        getQueueId(),
        true,
        destination_module,
        message);
    request->dest = destination_q;
    callback_handle* cb = new callback_handle(
        const_cast<RegisteredModuleHandle *>(&handle),
        callback_parm);
    return SendAsync(
        op,
        destination_q,
        _async_handleEnqueue,
        this,
        cb);
}


Boolean ModuleController::_send_forget(
    Uint32 destination_q,
    AsyncRequest *message)
{
    message->dest = destination_q;
    return SendForget(message);
}

Boolean ModuleController::_send_forget(
    Uint32 destination_q,
    const String& destination_module,
    AsyncRequest* message)
{
    AsyncOpNode* op = get_op();
    message->dest = destination_q;
    AsyncModuleOperationStart* request = new AsyncModuleOperationStart(
        op,
        destination_q,
        getQueueId(),
        true,
        destination_module,
        message);
    return SendForget(request);
}


Boolean ModuleController::ModuleSendForget(
    const RegisteredModuleHandle& handle,
    Uint32 destination_q,
    AsyncRequest* message)
{
    verify_handle(const_cast<RegisteredModuleHandle *>(&handle));

    return _send_forget(destination_q, message);
}

Boolean ModuleController::ModuleSendForget(
    const RegisteredModuleHandle& handle,
    Uint32 destination_q,
    const String& destination_module,
    AsyncRequest* message)
{
    verify_handle(const_cast<RegisteredModuleHandle *>(&handle));
    return _send_forget(destination_q, destination_module, message);
}


void ModuleController::_handle_async_request(AsyncRequest* rq)
{
    if (rq->getType() == ASYNC_ASYNC_MODULE_OP_START)
    {
        // find the target module
        RegisteredModuleHandle* target;
        Message* module_result = NULL;

        {
            _module_lock lock(&_modules);
            target = _modules.front();
            while (target != NULL)
            {
                if (target->get_name() ==
                        static_cast<AsyncModuleOperationStart *>(rq)->
                            _target_module)
                {
                    break;
                }

                target = _modules.next_of(target);
            }
        }

        if (target)
        {
            // ATTN: This statement was taken out of the _module_lock block
            // above because that caused all requests to control providers to
            // be serialized.  There is now a risk that the control provider
            // module may be deleted after the lookup and before this call.
            // See Bugzilla 3120.
            module_result = target->_receive_message(
                static_cast<AsyncModuleOperationStart *>(rq)->_act);
        }

        if (module_result == NULL)
        {
            module_result = new AsyncReply(
                ASYNC_REPLY,
                MessageMask::ha_async | MessageMask::ha_reply,
                rq->op,
                async_results::CIM_NAK,
                rq->resp,
                false);
        }

        AsyncModuleOperationResult *result = new AsyncModuleOperationResult(
            rq->op,
            async_results::OK,
            static_cast<AsyncModuleOperationStart *>(rq)->resp,
            false,
            static_cast<AsyncModuleOperationStart *>(rq)->_target_module,
            module_result);
        _complete_op_node(rq->op, 0, 0, 0);
    }
    else
        Base::_handle_async_request(rq);
}

void ModuleController::_handle_async_callback(AsyncOpNode* op)
{
    Base::_handle_async_callback(op);
}

ModuleController* ModuleController::getModuleController()
{
    MessageQueue* messageQueue =
        MessageQueue::lookup(PEGASUS_QUEUENAME_CONTROLSERVICE);
    PEGASUS_ASSERT(messageQueue != 0);
    PEGASUS_ASSERT(messageQueue->isAsync());

    MessageQueueService* service =
        dynamic_cast<MessageQueueService*>(messageQueue);
    PEGASUS_ASSERT(service != 0);
    PEGASUS_ASSERT(
        service->get_capabilities() & module_capabilities::module_controller);

    return static_cast<ModuleController*>(service);
}


// send a message to another service
AsyncReply* ModuleController::ClientSendWait(
    Uint32 destination_q,
    AsyncRequest* request)
{
    return _send_wait(destination_q, request);
}


// send a message to another module via another service
AsyncReply *ModuleController::ClientSendWait(
    Uint32 destination_q,
    String& destination_module,
    AsyncRequest* request)
{
    return _send_wait(destination_q, destination_module, request);
}


// send an async message to another service
Boolean ModuleController::ClientSendAsync(
    Uint32 msg_handle,
    Uint32 destination_q,
    AsyncRequest* message,
    void (*async_callback)(Uint32, Message *, void *),
    void* callback_parm)
{
    RegisteredModuleHandle* temp = new RegisteredModuleHandle(
        String(PEGASUS_MODULENAME_TEMP),
        this,
        0,
        async_callback);
   return ModuleSendAsync(
        *temp,
        msg_handle,
        destination_q,
        message,
        callback_parm);
}


// send an async message to another module via another service
Boolean ModuleController::ClientSendAsync(
    Uint32 msg_handle,
    Uint32 destination_q,
    const String& destination_module,
    AsyncRequest* message,
    void (*async_callback)(Uint32, Message *, void *),
    void* callback_parm)
{
    RegisteredModuleHandle* temp = new RegisteredModuleHandle(
        String(PEGASUS_MODULENAME_TEMP),
        this,
        0,
        async_callback);
   return ModuleSendAsync(
        *temp,
        msg_handle,
        destination_q,
        destination_module,
        message,
        callback_parm);
}


Boolean ModuleController::ClientSendForget(
    Uint32 destination_q,
    AsyncRequest* message)
{
    return _send_forget(destination_q, message);
}


Boolean ModuleController::ClientSendForget(
    Uint32 destination_q,
    const String& destination_module,
    AsyncRequest* message)
{
    return _send_forget(destination_q, destination_module, message);
}

PEGASUS_NAMESPACE_END
