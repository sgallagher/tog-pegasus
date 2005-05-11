//%2005////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
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
// Author: Mike Day (mdday@us.ibm.com) <<< Wed Mar 13 20:49:40 2002 mdd >>>
//
// Modified By: Amit K Arora, IBM (amita@in.ibm.com)
//              David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Module_Controller_h
#define Pegasus_Module_Controller_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/Message.h>
#include <Pegasus/Common/InternalException.h>
#include <Pegasus/Common/IPC.h>
#include <Pegasus/Common/Thread.h>
#include <Pegasus/Common/AsyncOpNode.h>
#include <Pegasus/Common/Cimom.h>
#include <Pegasus/Common/CimomMessage.h>
#include <Pegasus/Common/MessageQueueService.h>
#include <Pegasus/Common/peg_authorization.h>
#include <Pegasus/Common/Linkage.h>
#include <Pegasus/Common/AutoPtr.h>


PEGASUS_NAMESPACE_BEGIN

class ModuleController;


class PEGASUS_COMMON_LINKAGE pegasus_module
{
private:
    class module_rep : public pegasus_auth_handle
    {
    public:
        typedef pegasus_auth_handle Base;

        module_rep(ModuleController *controller,
               const String & name,
               void *module_address,
               Message * (*receive_message)(Message *, void *),
               void (*async_callback)(Uint32, Message *, void *),
               void (*shutdown_notify)(Uint32 code, void *));

        ~module_rep();

        Boolean operator==(const module_rep *rep) const
        {
            return(rep == this);
        }

        Boolean operator==(const module_rep &rep) const
        {
            return(rep == *this);
        }

        Boolean operator==(void *rep) const
        {
            return(reinterpret_cast<const void *>(this) == rep);
        }

        void reference()
        {
            _reference_count++;
        }

        void dereference()
        {
            _reference_count--;
        }

        Uint32 reference_count() const
        {
            return _reference_count.value();
        }

        const String& get_name() const throw()
        {
            return _name;
        }

        void *get_module_address() const
        {
            return _module_address;
        }

        Message * module_receive_message(Message *msg);

        void _send_async_callback(Uint32 msg_handle, Message *msg, void *parm);

        void _send_shutdown_notify();

        void lock()
        {
            _thread_safety.lock(pegasus_thread_self());
        }

        void unlock()
        {
            _thread_safety.unlock();
        }

        Boolean authorized();
        Boolean authorized(Uint32);
        Boolean authorized(Uint32, Uint32);

    private:
        module_rep();
        module_rep(const module_rep&);
        module_rep & operator=(const module_rep & rep);

        Mutex _thread_safety;
        // Don't make this an AutoPtr. Refer to bug 3502
        ModuleController* _controller;
        String _name;
        AtomicInt _reference_count;
        AtomicInt _shutting_down;

        void *_module_address;
        Message * (*_receive_message)(Message *, void *);
        void (*_async_callback)(Uint32, Message *, void *);
        void (*_shutdown_notify)(Uint32 code, void *);

        static Message * default_receive_message(Message *msg, void *inst)
        {
            throw NotImplemented("Module Receive");
            return 0;
        }

        static void default_async_callback(Uint32 handle, Message *msg, void *inst)
        {
            throw NotImplemented("Module Async Receive");
        }

        static void default_shutdown_notify(Uint32 code, void *inst)
        {
            // Intentionally left blank
        }

        static Message * closed_receive_message(Message *msg, void *inst)
        {
            throw ModuleClosed();
            return 0;
        }

        static void closed_async_callback(Uint32 handle, Message *msg, void *inst)
        {
            throw ModuleClosed();
        }

        friend class ModuleController;
    };

public:
    pegasus_module(ModuleController *controller,
             const String &id,
             void *module_address,
             Message * (*receive_message)(Message *, void *),
             void (*async_callback)(Uint32, Message *, void *),
             void (*shutdown_notify)(Uint32 code, void *));

    pegasus_module(const pegasus_module & mod);
    pegasus_module & operator= (const pegasus_module & mod);

    virtual ~pegasus_module();

    virtual Boolean authorized(Uint32 operation);
    virtual Boolean authorized();

    Boolean operator == (const pegasus_module & mod) const;
    Boolean operator == (const String &  mod) const;
    Boolean operator == (const void *mod) const;

    const String & get_name() const;

    // introspection interface
    Boolean query_interface(const String & class_id, void **object_ptr) const;

private:

    AutoPtr<module_rep> _rep;//PEP101

    pegasus_module()
    {
    }

    Boolean _rcv_msg(Message *);
    Message * _receive_message(Message *msg);
    void _send_async_callback(Uint32 msg_handle, Message *msg, void *);
    void _send_shutdown_notify();
    Boolean _shutdown();
    PEGASUS_STD(bitset<32>) _allowed_operations;

    void reference()
    {
        _rep->reference();
    }

    void dereference()
    {
        _rep->dereference();
    }

    friend class ModuleController;
};


class PEGASUS_COMMON_LINKAGE ModuleController : public MessageQueueService
{
public:
    typedef MessageQueueService Base;

    static const Uint32 GET_CLIENT_HANDLE;
    static const Uint32 REGISTER_MODULE;
    static const Uint32 DEREGISTER_MODULE;
    static const Uint32 FIND_SERVICE;
    static const Uint32 FIND_MODULE_IN_SERVICE;
    static const Uint32 GET_MODULE_REFERENCE;
    static const Uint32 MODULE_SEND_WAIT;
    static const Uint32 MODULE_SEND_WAIT_MODULE;
    static const Uint32 MODULE_SEND_ASYNC;
    static const Uint32 MODULE_SEND_ASYNC_MODULE;
    static const Uint32 BLOCKING_THREAD_EXEC;
    static const Uint32 ASYNC_THREAD_EXEC;
    static const Uint32 CLIENT_SEND_WAIT;
    static const Uint32 CLIENT_SEND_WAIT_MODULE;
    static const Uint32 CLIENT_SEND_ASYNC;
    static const Uint32 CLIENT_SEND_ASYNC_MODULE;
    static const Uint32 CLIENT_BLOCKING_THREAD_EXEC;
    static const Uint32 CLIENT_ASYNC_THREAD_EXEC;
    static const Uint32 CLIENT_SEND_FORGET;
    static const Uint32 CLIENT_SEND_FORGET_MODULE;
    static const Uint32 MODULE_SEND_FORGET;
    static const Uint32 MODULE_SEND_FORGET_MODULE;

// ATTN-DME-P2-20020406 Removed private declaration.  client_handle is
//          currently used in Pegasus/Provider/CIMOMHandle.cpp

//   private:
    class client_handle : public pegasus_auth_handle
    {
    public:
        typedef pegasus_auth_handle Base;

        client_handle(const pegasus_identity & id)
           :Base(id) ,
        allowed_operations( GET_CLIENT_HANDLE |
                    FIND_SERVICE |
                    FIND_MODULE_IN_SERVICE |
                    GET_MODULE_REFERENCE |
                    CLIENT_SEND_WAIT |
                    CLIENT_SEND_WAIT_MODULE |
                    CLIENT_SEND_ASYNC |
                    CLIENT_SEND_ASYNC_MODULE |
                    CLIENT_BLOCKING_THREAD_EXEC |
                    CLIENT_ASYNC_THREAD_EXEC),
        reference_count(1)
        {
        }

        ~client_handle()
        {
        }

        client_handle & operator=(const client_handle & handle)
        {
            if (this == &handle)
                return *this;
           reference_count++;
           return *this;
        }


        virtual Boolean authorized(Uint32, Uint32);
        virtual Boolean authorized(Uint32 operation);
        virtual Boolean authorized();
        PEGASUS_STD(bitset<32>) allowed_operations;
        AtomicInt reference_count;
    };

    class callback_handle
    {
        public:
        static void * operator new(size_t );
        static void operator delete(void *, size_t);
    private:
        static callback_handle *_head;
        static const int BLOCK_SIZE;
        static Mutex _alloc_mut;

    public:
        callback_handle(pegasus_module * module, void *parm)
           : _module(module), _parm(parm)
        {
        }

        ~callback_handle()
        {
            if( _module->get_name() == String(PEGASUS_MODULENAME_TEMP) )
                // delete _module;
                _module.reset();
        }

        AutoPtr<pegasus_module> _module;//PEP101
        void *_parm;
    };

public:
    ModuleController(const char *name);
/*       ModuleController(const char *name,  */
/*             Sint16 min_threads,  */
/*             Sint16 max_threads, */
/*             struct timeval & create_thread, */
/*             struct timeval & destroy_thread); */

    ~ModuleController();

    // module api
    // @exception AlreadyExistsException
    // @exception IncompatibleTypesException
    static ModuleController & register_module(const String & controller_name,
                    const String & module_name,
                    void *module_address,
                    Message * (*receive_message)(Message *, void *),
                    void (*async_callback)(Uint32, Message *, void *),
                    void (*shutdown_notify)(Uint32, void *),
                    pegasus_module **instance = NULL);

    // @exception Permission
    Boolean deregister_module(const String & module_name);

    // @exception Permission
    Uint32 find_service(const pegasus_module & handle, const String & name);

    // @exception Permission
    // @exception IPCException
    Uint32 find_module_in_service(const pegasus_module & handle,
                    const String & module_name);

    // @exception Permission
    pegasus_module * get_module_reference(const pegasus_module & my_handle,
                    const String & module_name);

    // send a message to another service
    // @exception Permission
    // @exception IPCException
    AsyncReply *ModuleSendWait(const pegasus_module & handle,
                    Uint32 destination_q,
                    AsyncRequest *request);

    // send a message to another module via another service
    // @exception Permission
    // @exception DeadLock
    // @exception IPCException
    AsyncReply *ModuleSendWait(const pegasus_module & handle,
                    Uint32 destination_q,
                    const String & destination_module,
                    AsyncRequest *message);

    // send an async message to another service
    // @exception Permission
    // @exception DeadLock
    // @exception IPCException
    Boolean ModuleSendAsync(const pegasus_module & handle,
                    Uint32 msg_handle,
                    Uint32 destination_q,
                    AsyncRequest *message,
                    void *callback_parm);

    // send an async message to another module via another service
    // @exception Permission
    // @exception IPCException
    Boolean ModuleSendAsync(const pegasus_module & handle,
                    Uint32 msg_handle,
                    Uint32 destination_q,
                    const String & destination_module,
                    AsyncRequest *message,
                    void *callback_parm);

    // @exception Permission
    // @exception IPCException
    Boolean ModuleSendForget(const pegasus_module & handle,
                    Uint32 destination_q,
                    AsyncRequest *message);

    // @exception Permission
    // @exception IPCException
    Boolean ModuleSendForget(const pegasus_module & handle,
                    Uint32 destination_q,
                    const String & destination_module,
                    AsyncRequest *message);

    // @exception Permission
    // @exception Deadlock
    // @exception IPCException
    void blocking_thread_exec(const pegasus_module & handle,
                    PEGASUS_THREAD_RETURN (PEGASUS_THREAD_CDECL *thread_func)(void *),
                    void *parm);

    // @exception Permission
    // @exception Deadlock
    // @exception IPCException
    void async_thread_exec(const pegasus_module & handle,
                    PEGASUS_THREAD_RETURN (PEGASUS_THREAD_CDECL *thread_func)(void *),
                    void *parm);

    Boolean verify_handle(pegasus_module *);

    // @exception IncompatibleTypesException
    static ModuleController & get_client_handle(const pegasus_identity & id,
                    client_handle **handle);

    // @exception IncompatibleTypesException
    static ModuleController & get_client_handle(const char *controller,
                    const pegasus_identity & id,
                    client_handle **handle);


    void return_client_handle(client_handle *handle);

    // send a message to another service
    // @exception Permission
    // @exception IPCException
    AsyncReply *ClientSendWait(const client_handle & handle,
                    Uint32 destination_q, AsyncRequest *request);

    // send a message to another module via another service
    // @exception Permission
    // @exception Deadlock
    // @exception IPCException
    AsyncReply *ClientSendWait(const client_handle & handle,
                 Uint32 destination_q,
                 String & destination_module,
                 AsyncRequest *message);

    // send an async message to another service
    // @exception Permission
    // @exception IPCException
    Boolean ClientSendAsync(const client_handle & handle,
                Uint32 msg_handle,
                Uint32 destination_q,
                AsyncRequest *message,
                void (*async_callback)(Uint32, Message *, void *) ,
                void *callback_parm);

    // send an async message to another module via another service
    // @exception Permission
    // @exception IPCException
    Boolean ClientSendAsync(const client_handle & handle,
                Uint32 msg_handle,
                Uint32 destination_q,
                const String & destination_module,
                AsyncRequest *message,
                void (*async_callback)(Uint32, Message *, void *),
                void *callback_parm);

    // @exception Permission
    // @exception IPCException
    Boolean ClientSendForget(const client_handle & handle,
                Uint32 destination_q,
                AsyncRequest *message);

    // @exception Permission
    // @exception IPCException
    Boolean ClientSendForget(const client_handle & handle,
                Uint32 destination_q,
                const String & destination_module,
                AsyncRequest *message);

    // @exception Permission
    // @exception Deadlock
    // @exception IPCException
    void client_blocking_thread_exec(const client_handle & handle,
                PEGASUS_THREAD_RETURN (PEGASUS_THREAD_CDECL *thread_func)(void *),
                void *parm);

    // @exception Permission
    // @exception Deadlock
    // @exception IPCException
    void client_async_thread_exec(const client_handle & handle,
                PEGASUS_THREAD_RETURN (PEGASUS_THREAD_CDECL *thread_func)(void *),
                void *parm);

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
        _module_lock(DQueue<pegasus_module> * list)
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
        DQueue<pegasus_module> * _list;
    };



    static void _async_handleEnqueue(AsyncOpNode *h, MessageQueue *q, void *parm);
    DQueue<pegasus_module> _modules;
    AsyncReply *_send_wait(Uint32, AsyncRequest *);
    AsyncReply *_send_wait(Uint32, const String &, AsyncRequest *);

    // @exception IPCException
    Boolean _send_forget(Uint32, AsyncRequest *);

    // @exception IPCException
    Boolean _send_forget(Uint32, const String &, AsyncRequest *);

    void _blocking_thread_exec(
                PEGASUS_THREAD_RETURN (PEGASUS_THREAD_CDECL *thread_func)(void *),
                void *parm);

    void _async_thread_exec(
                PEGASUS_THREAD_RETURN (PEGASUS_THREAD_CDECL *thread_func)(void *),
                void *parm);
};

PEGASUS_NAMESPACE_END

#endif // Pegasus_Module_Controller_H
