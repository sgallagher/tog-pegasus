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

#ifndef Pegasus_ModuleController_h
#define Pegasus_ModuleController_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Linkage.h>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/List.h>
#include <Pegasus/Common/Message.h>
#include <Pegasus/Common/MessageQueueService.h>
#include <Pegasus/Common/AsyncOpNode.h>
#include <Pegasus/Common/AutoPtr.h>

PEGASUS_NAMESPACE_BEGIN

class PEGASUS_COMMON_LINKAGE RegisteredModuleHandle : public Linkable
{
public:
    RegisteredModuleHandle(
        const String& name,
        void* module_address,
        Message* (*receive_message)(Message *, void *),
        void (*async_callback)(Uint32, Message *, void *));

    virtual ~RegisteredModuleHandle();

    const String& get_name() const;

private:

    RegisteredModuleHandle();
    RegisteredModuleHandle(const RegisteredModuleHandle&);
    RegisteredModuleHandle& operator=(const RegisteredModuleHandle&);

    Message* _receive_message(Message* msg);
    void _send_async_callback(Uint32 msg_handle, Message* msg, void* parm);

    String _name;
    void* _module_address;
    Message* (*_module_receive_message)(Message *, void *);
    void (*_async_callback)(Uint32, Message *, void *);

    friend class ModuleController;
};


class PEGASUS_COMMON_LINKAGE ModuleController : public MessageQueueService
{
public:
    typedef MessageQueueService Base;

    class callback_handle
    {
    public:
        callback_handle(RegisteredModuleHandle* module, void* parm)
           : _module(module), _parm(parm)
        {
        }

        ~callback_handle()
        {
            if (_module->get_name() == String(PEGASUS_MODULENAME_TEMP))
                _module.reset();
        }

        AutoPtr<RegisteredModuleHandle> _module;
        void* _parm;
    };

public:
    ModuleController(const char *name);

    ~ModuleController();

    // module api
    // @exception AlreadyExistsException
    // @exception IncompatibleTypesException
    static ModuleController& register_module(
        const String & controller_name,
        const String& module_name,
        void* module_address,
        Message* (*receive_message)(Message *, void *),
        void (*async_callback)(Uint32, Message *, void *),
        RegisteredModuleHandle** instance = 0);

    Boolean deregister_module(const String& module_name);

    Uint32 find_service(
        const RegisteredModuleHandle& handle,
        const String& name);

    Uint32 find_module_in_service(
        const RegisteredModuleHandle& handle,
        const String& module_name);

    // send a message to another service
    AsyncReply* ModuleSendWait(
        const RegisteredModuleHandle& handle,
        Uint32 destination_q,
        AsyncRequest* request);

    // send a message to another module via another service
    AsyncReply* ModuleSendWait(
        const RegisteredModuleHandle& handle,
        Uint32 destination_q,
        const String& destination_module,
        AsyncRequest* message);

    // send an async message to another service
    Boolean ModuleSendAsync(
        const RegisteredModuleHandle& handle,
        Uint32 msg_handle,
        Uint32 destination_q,
        AsyncRequest* message,
        void* callback_parm);

    // send an async message to another module via another service
    Boolean ModuleSendAsync(
        const RegisteredModuleHandle& handle,
        Uint32 msg_handle,
        Uint32 destination_q,
        const String& destination_module,
        AsyncRequest* message,
        void* callback_parm);

    Boolean ModuleSendForget(
        const RegisteredModuleHandle& handle,
        Uint32 destination_q,
        AsyncRequest* message);

    Boolean ModuleSendForget(
        const RegisteredModuleHandle & handle,
        Uint32 destination_q,
        const String & destination_module,
        AsyncRequest* message);

    void verify_handle(RegisteredModuleHandle *);

    static ModuleController* getModuleController();

    // send a message to another service
    AsyncReply* ClientSendWait(
        Uint32 destination_q,
        AsyncRequest* request);

    // send a message to another module via another service
    AsyncReply* ClientSendWait(
        Uint32 destination_q,
        String& destination_module,
        AsyncRequest* message);

    // send an async message to another service
    Boolean ClientSendAsync(
        Uint32 msg_handle,
        Uint32 destination_q,
        AsyncRequest* message,
        void (*async_callback)(Uint32, Message *, void *),
        void* callback_parm);

    // send an async message to another module via another service
    Boolean ClientSendAsync(
        Uint32 msg_handle,
        Uint32 destination_q,
        const String& destination_module,
        AsyncRequest* message,
        void (*async_callback)(Uint32, Message *, void *),
        void* callback_parm);

    Boolean ClientSendForget(
        Uint32 destination_q,
        AsyncRequest* message);

    Boolean ClientSendForget(
        Uint32 destination_q,
        const String& destination_module,
        AsyncRequest* message);

protected:
    // ATTN-RK-P2-20010322:  These methods are pure virtual in superclass
    virtual void handleEnqueue() {}
    virtual void handleEnqueue(Message *) {}
    virtual void _handle_async_request(AsyncRequest *rq);
    virtual void _handle_async_callback(AsyncOpNode *op);

private:
    class _module_lock
    {
    public:
        _module_lock(List<RegisteredModuleHandle, Mutex> * list)
           :_list(list)
        {
           _list->lock();
        }

        ~_module_lock()
        {
           _list->unlock();
        }

    private:
        _module_lock();
        List<RegisteredModuleHandle, Mutex> * _list;
    };


    static void _async_handleEnqueue(
        AsyncOpNode* h,
        MessageQueue* q,
        void* parm);

    List<RegisteredModuleHandle, Mutex> _modules;
    AsyncReply *_send_wait(Uint32, AsyncRequest *);
    AsyncReply *_send_wait(Uint32, const String &, AsyncRequest *);

    Boolean _send_forget(Uint32, AsyncRequest *);

    Boolean _send_forget(Uint32, const String &, AsyncRequest *);
};

PEGASUS_NAMESPACE_END

#endif // Pegasus_ModuleController_h
