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
// Author: Mike Day (mdday@us.ibm.com)
//
// Modified By: David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
//              Jim Wunderlich (Jim_Wunderlich@prodigy.net)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_MessageQueue_Service_h
#define Pegasus_MessageQueue_Service_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Message.h>
#include <Pegasus/Common/InternalException.h>
#include <Pegasus/Common/IPC.h>
#include <Pegasus/Common/Thread.h>
#include <Pegasus/Common/AsyncOpNode.h>
#include <Pegasus/Common/Cimom.h>
#include <Pegasus/Common/CimomMessage.h>
#include <Pegasus/Common/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

extern const Uint32 CIMOM_Q_ID;

class message_module;

class PEGASUS_COMMON_LINKAGE MessageQueueService : public MessageQueue
{
public:

    typedef MessageQueue Base;

    MessageQueueService(const char *name,
            Uint32 queueID = MessageQueue::getNextQueueId(),
            Uint32 capabilities = 0,
            Uint32 mask = message_mask::type_cimom |
            message_mask::type_service |
            message_mask::ha_request |
            message_mask::ha_reply |
            message_mask::ha_async );

    virtual ~MessageQueueService();

    virtual Boolean isAsync() const {  return true;  }

    // enqueue may throw an IPCException
    virtual void enqueue(Message *);

    AsyncReply *SendWait(AsyncRequest *request);
    Boolean SendAsync(AsyncOpNode *op,
            Uint32 destination,
            void (*callback)(AsyncOpNode *, MessageQueue *, void *),
            MessageQueue *callback_q,
            void *callback_ptr);

    Boolean SendAsync(Message *msg,
            Uint32 destination,
            void (*callback)(Message *response, void *handle, void *parameter),
            void *handle,
            void *parameter);

    Boolean SendForget(Message *msg);
    Boolean ForwardOp(AsyncOpNode *, Uint32 destination);


    Boolean register_service(String name, Uint32 capabilities, Uint32 mask);
    Boolean update_service(Uint32 capabilities, Uint32 mask);
    Boolean deregister_service();
    virtual void _shutdown_incoming_queue();

    void find_services(String name,
            Uint32 capabilities,
            Uint32 mask,
            Array<Uint32> *results);
    void enumerate_service(Uint32 queue, message_module *result);
    Uint32 get_next_xid();
    static AsyncOpNode *get_op();
    void return_op(AsyncOpNode *op);

    Boolean operator ==(const MessageQueueService & svce) const
    {
        return operator==((const void *)&svce);
    }

    Boolean operator ==(const void *svce) const
    {
        if((const void *)this == svce)
            return true;
        return false;
    }

    static PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL polling_routine(void *);
    static PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL kill_idle_threads(void *);
    static ThreadPool *get_thread_pool();

    Uint32 _mask;
    AtomicInt _die;
	AtomicInt _threads;
        Uint32 getIncomingCount() {return _incoming.count(); }

protected:
    virtual Boolean accept_async(AsyncOpNode *op);
    virtual Boolean messageOK(const Message *msg);
    virtual void handleEnqueue() = 0;
    virtual void handleEnqueue(Message *) = 0;
    Boolean _enqueueResponse(Message *, Message *);
//      virtual void _handle_incoming_operation(AsyncOpNode *operation, Thread *thread, MessageQueue *queue);
    virtual void _handle_incoming_operation(AsyncOpNode *);

    virtual void _handle_async_request(AsyncRequest *req);
    virtual void _handle_async_callback(AsyncOpNode *operation);
    virtual void _make_response(Message *req, Uint32 code);

    virtual void handle_heartbeat_request(AsyncRequest *req);
    virtual void handle_heartbeat_reply(AsyncReply *rep);

    virtual void handle_AsyncIoctl(AsyncIoctl *req);
    virtual void handle_CimServiceStart(CimServiceStart *req);
    virtual void handle_CimServiceStop(CimServiceStop *req);
    virtual void handle_CimServicePause(CimServicePause *req);
    virtual void handle_CimServiceResume(CimServiceResume *req);

    virtual void handle_AsyncOperationStart(AsyncOperationStart *req);
    virtual void handle_AsyncOperationResult(AsyncOperationResult *rep);
    virtual void handle_AsyncLegacyOperationStart(AsyncLegacyOperationStart *req);
    virtual void handle_AsyncLegacyOperationResult(AsyncLegacyOperationResult *rep);

    void _completeAsyncResponse(AsyncRequest *request,
                AsyncReply *reply,
                Uint32 state,
                Uint32 flag);
    void _complete_op_node(AsyncOpNode *, Uint32, Uint32, Uint32);

    static cimom *_meta_dispatcher;
    static AtomicInt _service_count;
    static Mutex _meta_dispatcher_mutex;

private:
    AsyncDQueue<AsyncOpNode> _incoming;
    static Thread* _polling_thread;
    static Semaphore _polling_sem;
    static AtomicInt _stop_polling;
    static AtomicInt _check_idle_flag;

    static DQueue<MessageQueueService> _polling_list;

    static PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL _req_proc(void *);

    static void _sendwait_callback(AsyncOpNode *, MessageQueue *, void *);

    AtomicInt _incoming_queue_shutdown;

protected:
    static ThreadPool *_thread_pool;

private:
    struct timeval _default_op_timeout;
    static AtomicInt _xid;
    friend class cimom;
    friend class CIMServer;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_MessageQueue_Service_h */
