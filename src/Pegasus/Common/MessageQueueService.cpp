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

#include "MessageQueueService.h"
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/MessageLoader.h>

PEGASUS_NAMESPACE_BEGIN

cimom *MessageQueueService::_meta_dispatcher = 0;
AtomicInt MessageQueueService::_service_count(0);
Mutex MessageQueueService::_meta_dispatcher_mutex;

static struct timeval deallocateWait = {300, 0};

ThreadPool *MessageQueueService::_thread_pool = 0;

MessageQueueService::PollingList* MessageQueueService::_polling_list;
Mutex MessageQueueService::_polling_list_mutex;

Thread* MessageQueueService::_polling_thread = 0;

ThreadPool *MessageQueueService::get_thread_pool()
{
   return _thread_pool;
}

//
// MAX_THREADS_PER_SVC_QUEUE
//
// JR Wunderlich Jun 6, 2005
//

#define MAX_THREADS_PER_SVC_QUEUE_LIMIT 5000
#define MAX_THREADS_PER_SVC_QUEUE_DEFAULT 5

#ifndef MAX_THREADS_PER_SVC_QUEUE
# define MAX_THREADS_PER_SVC_QUEUE MAX_THREADS_PER_SVC_QUEUE_DEFAULT
#endif

Uint32 max_threads_per_svc_queue;

ThreadReturnType PEGASUS_THREAD_CDECL MessageQueueService::polling_routine(
    void* parm)
{
    Thread *myself = reinterpret_cast<Thread *>(parm);
    List<MessageQueueService, Mutex> *list =
        reinterpret_cast<List<MessageQueueService, Mutex>*>(myself->get_parm());

    while (_stop_polling.get()  == 0)
    {
        _polling_sem.wait();

        if (_stop_polling.get() != 0)
        {
            break;
        }

        // The polling_routine thread must hold the lock on the
        // _polling_list while processing incoming messages.
        // This lock is used to give this thread ownership of
        // services on the _polling_routine list.

        // This is necessary to avoid confict with other threads
        // processing the _polling_list
        // (e.g., MessageQueueServer::~MessageQueueService).

        list->lock();
        MessageQueueService *service = list->front();
        ThreadStatus rtn = PEGASUS_THREAD_OK;
        while (service != NULL)
        {
            if ((service->_incoming.count() > 0) &&
                (service->_die.get() == 0) &&
                (service->_threads.get() < max_threads_per_svc_queue))
            {
                // The _threads count is used to track the
                // number of active threads that have been allocated
                // to process messages for this service.

                // The _threads count MUST be incremented while
                // the polling_routine owns the _polling_thread
                // lock and has ownership of the service object.

                service->_threads++;
                try
                {
                    rtn = _thread_pool->allocate_and_awaken(
                        service, _req_proc, &_polling_sem);
                }
                catch (...)
                {
                    service->_threads--;

                    // allocate_and_awaken should never generate an exception.
                    PEGASUS_ASSERT(0);
                }
                // if no more threads available, break from processing loop
                if (rtn != PEGASUS_THREAD_OK )
                {
                    service->_threads--;
                    PEG_TRACE((TRC_MESSAGEQUEUESERVICE, Tracer::LEVEL1,
                        "Could not allocate thread for %s.  Queue has %d "
                            "messages waiting and %d threads servicing."
                            "Skipping the service for right now. ",
                        service->getQueueName(),
                        service->_incoming.count(),
                        service->_threads.get()));

                    Threads::yield();
                    service = NULL;
                }
            }
            if (service != NULL)
            {
                service = list->next_of(service);
            }
        }
        list->unlock();
    }
    return ThreadReturnType(0);
}


Semaphore MessageQueueService::_polling_sem(0);
AtomicInt MessageQueueService::_stop_polling(0);


MessageQueueService::MessageQueueService(
    const char* name,
    Uint32 queueID)
    : Base(name, true,  queueID),
      _die(0),
      _threads(0),
      _incoming(),
      _incoming_queue_shutdown(0)
{
    _isRunning = true;

    max_threads_per_svc_queue = MAX_THREADS_PER_SVC_QUEUE;

    // if requested thread max is out of range, then set to
    // MAX_THREADS_PER_SVC_QUEUE_LIMIT

    if ((max_threads_per_svc_queue < 1) ||
        (max_threads_per_svc_queue > MAX_THREADS_PER_SVC_QUEUE_LIMIT))
    {
        max_threads_per_svc_queue = MAX_THREADS_PER_SVC_QUEUE_LIMIT;
    }

    PEG_TRACE((TRC_MESSAGEQUEUESERVICE, Tracer::LEVEL3,
       "max_threads_per_svc_queue set to %u.", max_threads_per_svc_queue));

    AutoMutex autoMut(_meta_dispatcher_mutex);

    if (_meta_dispatcher == 0)
    {
        _stop_polling = 0;
        PEGASUS_ASSERT(_service_count.get() == 0);
        _meta_dispatcher = new cimom();

        //  _thread_pool = new ThreadPool(initial_cnt, "MessageQueueService",
        //   minimum_cnt, maximum_cnt, deallocateWait);
        //
        _thread_pool =
            new ThreadPool(0, "MessageQueueService", 0, 0, deallocateWait);
    }
    _service_count++;

    _get_polling_list()->insert_back(this);
}


MessageQueueService::~MessageQueueService()
{
    _die = 1;

    // The polling_routine locks the _polling_list while
    // processing the incoming messages for services on the
    // list.  Deleting the service from the _polling_list
    // prior to processing, avoids synchronization issues
    // with the _polling_routine.

    // ATTN: added to prevent assertion in List in which the list does not
    // contain this element.

    if (_get_polling_list()->contains(this))
        _get_polling_list()->remove(this);

    // ATTN: The code for closing the _incoming queue
    // is not working correctly. In OpenPegasus 2.5,
    // execution of the following code is very timing
    // dependent. This needs to be fix.
    // See Bug 4079 for details.
    if (_incoming_queue_shutdown.get() == 0)
    {
        _shutdown_incoming_queue();
    }

    // Wait until all threads processing the messages
    // for this service have completed.

    while (_threads.get() > 0)
    {
        Threads::yield();
    }

    {
        AutoMutex autoMut(_meta_dispatcher_mutex);
        _service_count--;
        if (_service_count.get() == 0)
        {

            _stop_polling++;
            _polling_sem.signal();
            if (_polling_thread)
            {
                _polling_thread->join();
                delete _polling_thread;
                _polling_thread = 0;
            }
            _meta_dispatcher->_shutdown_routed_queue();
            delete _meta_dispatcher;
            _meta_dispatcher = 0;

            delete _thread_pool;
            _thread_pool = 0;
        }
    } // mutex unlocks here

    // Clean up any extra stuff on the queue.
    AsyncOpNode* op = 0;
    while ((op = _incoming.dequeue()))
    {
        delete op;
    }
}

void MessageQueueService::_shutdown_incoming_queue()
{
    if (_incoming_queue_shutdown.get() > 0)
        return;

    AsyncIoctl *msg = new AsyncIoctl(
        0,
        _queueId,
        _queueId,
        true,
        AsyncIoctl::IO_CLOSE,
        0,
        0);

    msg->op = get_op();
    msg->op->_flags = ASYNC_OPFLAGS_FIRE_AND_FORGET;

    msg->op->_op_dest = this;
    msg->op->_request.reset(msg);
    if (_incoming.enqueue(msg->op))
    {
        _polling_sem.signal();
    }
    else
    {
        // This means the queue has already been shut-down (happens  when there
        // are two AsyncIoctrl::IO_CLOSE messages generated and one got first
        // processed.
        delete msg;
    }
}


void MessageQueueService::enqueue(Message* msg)
{
    PEG_METHOD_ENTER(TRC_MESSAGEQUEUESERVICE, "MessageQueueService::enqueue()");

    Base::enqueue(msg);

    PEG_METHOD_EXIT();
}


ThreadReturnType PEGASUS_THREAD_CDECL MessageQueueService::_req_proc(
    void* parm)
{
    MessageQueueService* service =
        reinterpret_cast<MessageQueueService*>(parm);
    PEGASUS_ASSERT(service != 0);
    try
    {
        if (service->_die.get() != 0)
        {
            service->_threads--;
            return 0;
        }
        // pull messages off the incoming queue and dispatch them. then
        // check pending messages that are non-blocking
        AsyncOpNode *operation = 0;

        // many operations may have been queued.
        do
        {
            operation = service->_incoming.dequeue();

            if (operation)
            {
               operation->_service_ptr = service;
               service->_handle_incoming_operation(operation);
            }
        } while (operation);
    }
    catch (const Exception& e)
    {
        PEG_TRACE((TRC_DISCARDED_DATA, Tracer::LEVEL1,
            "Caught exception: \"%s\".  Exiting _req_proc.",
            (const char*)e.getMessage().getCString()));
    }
    catch (...)
    {
        PEG_TRACE_CSTRING(TRC_DISCARDED_DATA, Tracer::LEVEL1,
            "Caught unrecognized exception.  Exiting _req_proc.");
    }
    service->_threads--;
    return 0;
}


void MessageQueueService::_sendwait_callback(
    AsyncOpNode* op,
    MessageQueue* q,
    void *parm)
{
    op->_client_sem.signal();
}


// callback function is responsible for cleaning up all resources
// including op, op->_callback_node, and op->_callback_ptr
void MessageQueueService::_handle_async_callback(AsyncOpNode* op)
{
    PEGASUS_ASSERT(op->_flags == ASYNC_OPFLAGS_CALLBACK);
    // note that _callback_node may be different from op
    // op->_callback_response_q is a "this" pointer we can use for
    // static callback methods
    op->_async_callback(
        op->_callback_node, op->_callback_response_q, op->_callback_ptr);
}


void MessageQueueService::_handle_incoming_operation(AsyncOpNode* operation)
{
    if (operation != 0)
    {
        Message *rq = operation->_request.get();

// optimization <<< Thu Mar  7 21:04:05 2002 mdd >>>
// move this to the bottom of the loop when the majority of
// messages become async messages.

        // divert legacy messages to handleEnqueue
        if ((rq != 0) && (!(rq->getMask() & MessageMask::ha_async)))
        {
            operation->_request.release();
            // delete the op node
            return_op(operation);
            handleEnqueue(rq);
            return;
        }

        if ((operation->_flags & ASYNC_OPFLAGS_CALLBACK) &&
            (operation->_state & ASYNC_OPSTATE_COMPLETE))
        {
            _handle_async_callback(operation);
        }
        else
        {
            PEGASUS_ASSERT(rq != 0);
            _handle_async_request(static_cast<AsyncRequest *>(rq));
        }
    }
    return;
}

void MessageQueueService::_handle_async_request(AsyncRequest *req)
{
    MessageType type = req->getType();
    if (type == ASYNC_IOCTL)
    {
        handle_AsyncIoctl(static_cast<AsyncIoctl *>(req));
    }
    else if (type == ASYNC_CIMSERVICE_START)
    {
        handle_CimServiceStart(static_cast<CimServiceStart *>(req));
    }
    else if (type == ASYNC_CIMSERVICE_STOP)
    {
        handle_CimServiceStop(static_cast<CimServiceStop *>(req));
    }
    else
    {
        // we don't handle this request message
        _make_response(req, async_results::CIM_NAK);
    }
}

Boolean MessageQueueService::_enqueueResponse(
    Message* request,
    Message* response)
{
    PEG_METHOD_ENTER(TRC_MESSAGEQUEUESERVICE,
        "MessageQueueService::_enqueueResponse");

    if (request->getMask() & MessageMask::ha_async)
    {
        if (response->getMask() & MessageMask::ha_async)
        {
            _completeAsyncResponse(
                static_cast<AsyncRequest *>(request),
                static_cast<AsyncReply *>(response));

            PEG_METHOD_EXIT();
            return true;
        }
    }

    AsyncRequest* asyncRequest =
        static_cast<AsyncRequest*>(request->get_async());

    if (asyncRequest != 0)
    {
        PEGASUS_ASSERT(asyncRequest->getMask() &
            (MessageMask::ha_async | MessageMask::ha_request));

        AsyncOpNode* op = asyncRequest->op;

        // the legacy request is going to be deleted by its handler
        // remove it from the op node

        static_cast<AsyncLegacyOperationStart *>(asyncRequest)->get_action();

        AsyncLegacyOperationResult *async_result =
            new AsyncLegacyOperationResult(
                op,
                response);
        _completeAsyncResponse(
            asyncRequest,
            async_result);

        PEG_METHOD_EXIT();
        return true;
    }

    // ensure that the destination queue is in response->dest
    PEG_METHOD_EXIT();
    return SendForget(response);
}

void MessageQueueService::_make_response(Message* req, Uint32 code)
{
    cimom::_make_response(req, code);
}


void MessageQueueService::_completeAsyncResponse(
    AsyncRequest* request,
    AsyncReply* reply)
{
    PEG_METHOD_ENTER(TRC_MESSAGEQUEUESERVICE,
        "MessageQueueService::_completeAsyncResponse");

    cimom::_completeAsyncResponse(request, reply);

    PEG_METHOD_EXIT();
}


void MessageQueueService::_complete_op_node(
    AsyncOpNode* op)
{
    cimom::_complete_op_node(op);
}


Boolean MessageQueueService::accept_async(AsyncOpNode* op)
{
    if (_incoming_queue_shutdown.get() > 0)
        return false;
    if (_polling_thread == NULL)
    {
        _polling_thread = new Thread(
            polling_routine,
            reinterpret_cast<void *>(_get_polling_list()),
            false);
        ThreadStatus tr = PEGASUS_THREAD_OK;
        while ( (tr =_polling_thread->run()) != PEGASUS_THREAD_OK)
        {
            if (tr == PEGASUS_THREAD_INSUFFICIENT_RESOURCES)
                Threads::yield();
            else
                throw Exception(MessageLoaderParms(
                    "Common.MessageQueueService.NOT_ENOUGH_THREAD",
                    "Could not allocate thread for the polling thread."));
        }
    }
    if (_die.get() == 0)
    {
        if (_incoming.enqueue(op))
        {
            _polling_sem.signal();
            return true;
        }
    }
    return false;
}

void MessageQueueService::handle_AsyncIoctl(AsyncIoctl* req)
{
    switch (req->ctl)
    {
        case AsyncIoctl::IO_CLOSE:
        {
            MessageQueueService *service =
                static_cast<MessageQueueService *>(req->op->_service_ptr);

#ifdef MESSAGEQUEUESERVICE_DEBUG
            PEGASUS_STD(cout) << service->getQueueName() <<
                " Received AsyncIoctl::IO_CLOSE " << PEGASUS_STD(endl);
#endif

            // respond to this message. this is fire and forget, so we
            // don't need to delete anything.
            // this takes care of two problems that were being found
            // << Thu Oct  9 10:52:48 2003 mdd >>
            _make_response(req, async_results::OK);
            // ensure we do not accept any further messages

            // ensure we don't recurse on IO_CLOSE
            if (_incoming_queue_shutdown.get() > 0)
                break;

            // set the closing flag
            service->_incoming_queue_shutdown = 1;
            // empty out the queue
            while (1)
            {
                AsyncOpNode* operation = 0;
                try
                {
                    operation = service->_incoming.dequeue();
                }
                catch (...)
                {
                    break;
                }
                if (operation)
                {
                    operation->_service_ptr = service;
                    service->_handle_incoming_operation(operation);
                }
                else
                    break;
            } // message processing loop

            // shutdown the AsyncQueue
            service->_incoming.close();
            return;
        }

        default:
            _make_response(req, async_results::CIM_NAK);
    }
}

void MessageQueueService::handle_CimServiceStart(CimServiceStart* req)
{
#ifdef MESSAGEQUEUESERVICE_DEBUG
    PEGASUS_STD(cout) << getQueueName() << "received START" <<
        PEGASUS_STD(endl);
#endif
    PEGASUS_ASSERT(!_isRunning);
    _isRunning = true;
    _make_response(req, async_results::OK);
}

void MessageQueueService::handle_CimServiceStop(CimServiceStop* req)
{
#ifdef MESSAGEQUEUESERVICE_DEBUG
    PEGASUS_STD(cout) << getQueueName() << "received STOP" << PEGASUS_STD(endl);
#endif
    PEGASUS_ASSERT(_isRunning);
    _isRunning = false;
    _make_response(req, async_results::CIM_SERVICE_STOPPED);
}

AsyncOpNode* MessageQueueService::get_op()
{
   AsyncOpNode* op = new AsyncOpNode();

   op->_state = ASYNC_OPSTATE_UNKNOWN;
   op->_flags = ASYNC_OPFLAGS_UNKNOWN;

   return op;
}

void MessageQueueService::return_op(AsyncOpNode* op)
{
    delete op;
}


Boolean MessageQueueService::SendAsync(
    AsyncOpNode* op,
    Uint32 destination,
    void (*callback)(AsyncOpNode*, MessageQueue*, void*),
    MessageQueue* callback_response_q,
    void* callback_ptr)
{
    return _sendAsync(
        op,
        destination,
        callback,
        callback_response_q,
        callback_ptr,
        ASYNC_OPFLAGS_CALLBACK);

}

Boolean MessageQueueService::_sendAsync(
    AsyncOpNode* op,
    Uint32 destination,
    void (*callback)(AsyncOpNode*, MessageQueue*, void*),
    MessageQueue* callback_response_q,
    void* callback_ptr,
    Uint32 flags)
{
    PEGASUS_ASSERT(op != 0 && callback != 0);

    // destination of this message
    op->_op_dest = MessageQueue::lookup(destination);
    if (op->_op_dest == 0)
    {
        return false;
    }
    op->_flags = flags;
    // initialize the callback data
    // callback function to be executed by recpt. of response
    op->_async_callback = callback;
    // the op node
    op->_callback_node = op;
    // the queue that will receive the response
    op->_callback_response_q = callback_response_q;
    // user data for callback
    op->_callback_ptr = callback_ptr;
    // I am the originator of this request
    op->_callback_request_q = this;

    return  _meta_dispatcher->route_async(op);
}

Boolean MessageQueueService::SendForget(Message* msg)
{
    AsyncOpNode* op = 0;
    Uint32 mask = msg->getMask();

    if (mask & MessageMask::ha_async)
    {
        op = (static_cast<AsyncMessage *>(msg))->op;
    }

    if (op == 0)
    {
        op = get_op();
        op->_request.reset(msg);
        if (mask & MessageMask::ha_async)
        {
            (static_cast<AsyncMessage *>(msg))->op = op;
        }
    }

    PEGASUS_ASSERT(op->_flags == ASYNC_OPFLAGS_UNKNOWN);
    PEGASUS_ASSERT(op->_state == ASYNC_OPSTATE_UNKNOWN);
    op->_op_dest = MessageQueue::lookup(msg->dest);
    if (op->_op_dest == 0)
    {
        return_op(op);
        return false;
    }

    op->_flags = ASYNC_OPFLAGS_FIRE_AND_FORGET;

    // now see if the meta dispatcher will take it
    return  _meta_dispatcher->route_async(op);
}


AsyncReply *MessageQueueService::SendWait(AsyncRequest* request)
{
    if (request == 0)
        return 0;

    Boolean destroy_op = false;

    if (request->op == 0)
    {
        request->op = get_op();
        request->op->_request.reset(request);
        destroy_op = true;
    }
    
    PEGASUS_ASSERT(request->op->_flags == ASYNC_OPFLAGS_UNKNOWN);
    PEGASUS_ASSERT(request->op->_state == ASYNC_OPSTATE_UNKNOWN);

    request->block = false;
    _sendAsync(
        request->op,
        request->dest,
        _sendwait_callback,
        this,
        (void *)0,
        ASYNC_OPFLAGS_PSEUDO_CALLBACK);

    request->op->_client_sem.wait();

    AsyncReply* rpl = static_cast<AsyncReply *>(request->op->removeResponse());
    rpl->op = 0;

    if (destroy_op == true)
    {
        request->op->_request.release();
        return_op(request->op);
        request->op = 0;
    }
    return rpl;
}

Uint32 MessageQueueService::find_service_qid(const String &name)
{
    MessageQueue *queue = MessageQueue::lookup((const char*)name.getCString());
    PEGASUS_ASSERT(queue);
    return queue->getQueueId();
}

MessageQueueService::PollingList* MessageQueueService::_get_polling_list()
{
    _polling_list_mutex.lock();

    if (!_polling_list)
        _polling_list = new PollingList;

    _polling_list_mutex.unlock();

    return _polling_list;
}

PEGASUS_NAMESPACE_END
