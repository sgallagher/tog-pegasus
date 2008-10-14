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

#ifndef Pegasus_MessageQueue_Service_h
#define Pegasus_MessageQueue_Service_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Message.h>
#include <Pegasus/Common/InternalException.h>
#include <Pegasus/Common/Thread.h>
#include <Pegasus/Common/ThreadPool.h>
#include <Pegasus/Common/AsyncOpNode.h>
#include <Pegasus/Common/Cimom.h>
#include <Pegasus/Common/Mutex.h>
#include <Pegasus/Common/CimomMessage.h>
#include <Pegasus/Common/Linkage.h>
#include <Pegasus/Common/Mutex.h>

PEGASUS_NAMESPACE_BEGIN

extern const Uint32 CIMOM_Q_ID;

class message_module;
class cimom;


class PEGASUS_COMMON_LINKAGE MessageQueueService :
    public Linkable, public MessageQueue
{
public:

    typedef MessageQueue Base;

    MessageQueueService(
        const char* name,
        Uint32 queueID = MessageQueue::getNextQueueId(),
        Uint32 capabilities = 0,
        Uint32 mask = MessageMask::ha_request |
            MessageMask::ha_reply |
            MessageMask::ha_async);

    virtual ~MessageQueueService();

    virtual Boolean isAsync() const {  return true;  }

    virtual void enqueue(Message *);

    AsyncReply* SendWait(AsyncRequest* request);
    Boolean SendAsync(AsyncOpNode* op,
        Uint32 destination,
        void (*callback)(AsyncOpNode*, MessageQueue*, void*),
        MessageQueue* callback_q,
        void* callback_ptr);

    Boolean SendForget(Message* msg);

    Boolean update_service(Uint32 capabilities, Uint32 mask);
    Boolean deregister_service();

    Uint32 find_service_qid(const String &name);
    static AsyncOpNode* get_op();
    void return_op(AsyncOpNode* op);

    static ThreadPool* get_thread_pool();

    Uint32 _mask;
    AtomicInt _die;
    AtomicInt _threads;
    Uint32 getIncomingCount() {return _incoming.count(); }

protected:
    virtual void _shutdown_incoming_queue();
    virtual Boolean accept_async(AsyncOpNode* op);
    virtual Boolean messageOK(const Message* msg);
    virtual void handleEnqueue() = 0;
    virtual void handleEnqueue(Message *) = 0;
    Boolean _enqueueResponse(Message *, Message *);
    virtual void _handle_incoming_operation(AsyncOpNode *);

    virtual void _handle_async_request(AsyncRequest* req);
    virtual void _handle_async_callback(AsyncOpNode* operation);
    virtual void _make_response(Message* req, Uint32 code);

    virtual void handle_AsyncIoctl(AsyncIoctl* req);
    virtual void handle_CimServiceStart(CimServiceStart* req);
    virtual void handle_CimServiceStop(CimServiceStop* req);

    void _completeAsyncResponse(
        AsyncRequest* request,
        AsyncReply* reply,
        Uint32 state,
        Uint32 flag);
    void _complete_op_node(AsyncOpNode *, Uint32, Uint32, Uint32);

    static cimom* _meta_dispatcher;
    static AtomicInt _service_count;
    static Mutex _meta_dispatcher_mutex;
    static ThreadPool* _thread_pool;

private:
    static ThreadReturnType PEGASUS_THREAD_CDECL polling_routine(void *);

    AsyncQueue<AsyncOpNode> _incoming;
    static Thread* _polling_thread;
    static Semaphore _polling_sem;
    static AtomicInt _stop_polling;

    typedef List<MessageQueueService, Mutex> PollingList;
    static PollingList* _polling_list;
    static Mutex _polling_list_mutex;

    PollingList* _get_polling_list();

    static ThreadReturnType PEGASUS_THREAD_CDECL _req_proc(void *);

    static void _sendwait_callback(AsyncOpNode *, MessageQueue *, void *);

    AtomicInt _incoming_queue_shutdown;

    friend class cimom;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_MessageQueue_Service_h */
