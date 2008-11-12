//%2006/////////////////////////////////////////////////////////////////////////
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

#include "Cimom.h"

#include <iostream>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/MessageLoader.h>
#include <Pegasus/Common/AutoPtr.h>

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

const Uint32 CIMOM_Q_ID = 1;

Boolean cimom::route_async(AsyncOpNode *op)
{
    if (_die.get() > 0)
        return false;

    if (_routed_queue_shutdown.get() > 0)
        return false;

    return _routed_ops.enqueue(op);
}

ThreadReturnType PEGASUS_THREAD_CDECL cimom::_routing_proc(void *parm)
{
    Thread* myself = reinterpret_cast<Thread *>(parm);
    cimom* dispatcher = reinterpret_cast<cimom *>(myself->get_parm());
    AsyncOpNode *op = 0;

    while (dispatcher->_die.get() == 0)
    {
        op = dispatcher->_routed_ops.dequeue_wait();

        if (op == 0)
        {
            break;
        }
        else
        {
            MessageQueue *dest_q = op->_op_dest;
            Uint32 dest_qid = dest_q->getQueueId();

            Boolean accepted = false;

            if (dest_qid == CIMOM_Q_ID)
            {
               dispatcher->_handle_cimom_op(op);
               accepted = true;
            }
            else
            {
                //<< Tue Feb 19 11:40:37 2002 mdd >>
                // moved the lookup to sendwait/nowait/forget/forward functions.

                MessageQueueService *dest_svc = 0;
                // ATTN: We should only get async queues for message dispatch.
                PEGASUS_ASSERT(dest_q->isAsync());
                dest_svc= static_cast<MessageQueueService *>(dest_q);

                if (!dest_svc->isRunning())
                {
                    // the target is stopped, unless the message is a start
                    // just handle it from here.
                    AsyncRequest *request =
                        static_cast<AsyncRequest *>(op->_request.get());
                    MessageType messageType = request->getType();

                    if (messageType != ASYNC_CIMSERVICE_START)
                    {
                       dispatcher->_make_response(
                           request, async_results::CIM_SERVICE_STOPPED);
                       accepted = true;
                    }
                    else
                    {
                        // deliver the start message
                        if (dest_svc->_die.get() == 0)
                        {
                            accepted = dest_svc->accept_async(op);
                        }
                    }
                }
                else if (dest_svc->_die.get() == 0)
                {
                    accepted = dest_svc->accept_async(op);
                }

                if (accepted == false)
                {
                   // set completion code to NAK and flag completed
                    _make_response(op->_request.get(), async_results::CIM_NAK);
                }
            }
        }
    } // loop

    return 0;
}


cimom::cimom()
    : MessageQueue(PEGASUS_QUEUENAME_METADISPATCHER, true, CIMOM_Q_ID),
      _routed_ops(),
      _routing_thread(_routing_proc, this, false),
      _die(0),
      _routed_queue_shutdown(0)
{
    _global_this = static_cast<cimom *>(MessageQueue::lookup(CIMOM_Q_ID));

    ThreadStatus tr = PEGASUS_THREAD_OK;
    while ((tr = _routing_thread.run()) != PEGASUS_THREAD_OK)
    {
        if (tr == PEGASUS_THREAD_INSUFFICIENT_RESOURCES)
            Threads::yield();
        else
            throw Exception(
                MessageLoaderParms("Common.Cimom.NOT_ENOUGH_THREADS",
                    "Cannot allocate thread for Cimom class"));
    }
}


cimom::~cimom()
{
    PEGASUS_ASSERT(_routed_queue_shutdown.get() == 0);

    AsyncIoClose *msg = new AsyncIoClose(
        0,
        CIMOM_Q_ID,
        CIMOM_Q_ID,
        true);

    msg->op = get_cached_op();
    msg->op->_flags = ASYNC_OPFLAGS_FIRE_AND_FORGET;
    msg->op->_op_dest = _global_this;
    msg->op->_request.reset(msg);

    Boolean done = _routed_ops.enqueue(msg->op);
    PEGASUS_ASSERT(done);

    _routing_thread.join(); 

    PEGASUS_ASSERT(_routed_queue_shutdown.get());
    PEGASUS_ASSERT(_die.get());
}

void cimom::_make_response(Message *req, Uint32 code)
{
    if (!(req->getMask() & MessageMask::ha_async))
    {
        // legacy message, just delete
        delete req;
        return;
    }

    Uint32 flags = static_cast<AsyncRequest *>(req)->op->_flags;

    if (flags == ASYNC_OPFLAGS_FIRE_AND_FORGET)
    {
        _global_this->cache_op(static_cast<AsyncRequest *>(req)->op);
        return;
    }

    AutoPtr<AsyncReply> reply;
    reply.reset(new AsyncReply(
        ASYNC_REPLY,
        0,
        (static_cast<AsyncRequest *>(req))->op,
        code,
        (static_cast<AsyncRequest *>(req))->resp,
        false));

    _completeAsyncResponse(
        static_cast<AsyncRequest*>(req),
        reply.get());
    reply.release();
}

void cimom::_completeAsyncResponse(
    AsyncRequest *request,
    AsyncReply *reply)
{
    PEG_METHOD_ENTER(TRC_MESSAGEQUEUESERVICE, "cimom::_completeAsyncResponse");
    PEGASUS_ASSERT(request != 0);

    AsyncOpNode *op = request->op;
    if (op->_flags == ASYNC_OPFLAGS_CALLBACK)
    {
        if (reply != 0)
        {
            op->_response.reset(reply);
        }
        _complete_op_node(op);
        return;
    }
    else if (op->_flags == ASYNC_OPFLAGS_FIRE_AND_FORGET)
    {
        PEGASUS_ASSERT(op->_state == ASYNC_OPSTATE_UNKNOWN);
        _global_this->cache_op(op);
    }
    else
    {
        PEGASUS_ASSERT (op->_flags == ASYNC_OPFLAGS_PSEUDO_CALLBACK);
        PEGASUS_ASSERT(op->_state == ASYNC_OPSTATE_UNKNOWN);
        op->_state = ASYNC_OPSTATE_COMPLETE;
        op->_client_sem.signal();
    }
    PEG_METHOD_EXIT();
}

cimom *cimom::_global_this;


void cimom::_default_callback(AsyncOpNode *op, MessageQueue *q, void *ptr)
{
    PEGASUS_ASSERT(op != 0 && q != 0);
    return;
}


void cimom::_complete_op_node(
    AsyncOpNode *op)
{
    Uint32 flags = op->_flags;

    PEGASUS_ASSERT(op->_state == ASYNC_OPSTATE_UNKNOWN);

    op->_state = ASYNC_OPSTATE_COMPLETE;
    if (flags ==  ASYNC_OPFLAGS_FIRE_AND_FORGET )
    {
        _global_this->cache_op(op);
        return;
    }

    if (flags ==  ASYNC_OPFLAGS_PSEUDO_CALLBACK)
    {
        op->_client_sem.signal();
        return;
    }

    PEGASUS_ASSERT(flags == ASYNC_OPFLAGS_CALLBACK);

    // << Wed Oct  8 12:29:32 2003 mdd >>
    // check to see if the response queue is stopped
    if (op->_callback_response_q == 0 ||
        !(static_cast<MessageQueueService*> (
            op->_callback_response_q))->isRunning())
    {
        // delete, respondent is stopped
        _global_this->cache_op(op);
    }
    else
    {
        // send this node to the response queue
        op->_op_dest = op->_callback_response_q;
        _global_this->route_async(op);
    }
}


void cimom::handleEnqueue()
{
    //ATTN: We should never get legacy messages directly.
    PEGASUS_ASSERT(0);
}


void cimom::_handle_cimom_op(AsyncOpNode *op)
{
    Message* msg = op->getRequest();

    // We handle only one message at present.
    PEGASUS_ASSERT( msg->getType() ==  ASYNC_IOCLOSE);
    _global_this->_routed_queue_shutdown = 1;
    _make_response(msg, async_results::OK);
    // All services are shutdown, empty out the queue
    for(;;)
    {
        AsyncOpNode* operation = 0;
        try
        {
            operation = _global_this->_routed_ops.dequeue();
            if (operation)
            {
                _global_this->cache_op(operation);
            }
            else
            {
                break;
            }
        }
        catch (...)
        {
             break;
        }
    }
    // shutdown the AsyncQueue
    _global_this->_routed_ops.close();
    // exit the routing thread.
    _die++;
}

AsyncOpNode* cimom::get_cached_op()
{
    AutoPtr<AsyncOpNode> op(new AsyncOpNode());

    op->_state = ASYNC_OPSTATE_UNKNOWN;
    op->_flags = ASYNC_OPFLAGS_UNKNOWN;

    return op.release();
}

void cimom::cache_op(AsyncOpNode* op)
{
    delete op;
}

PEGASUS_NAMESPACE_END
