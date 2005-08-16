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
// Modified By:
//              Amit K Arora, IBM (amita@in.ibm.com) for Bug#1090,#2657
//              Josephine Eskaline Joyce, IBM (jojustin@in.ibm.com) for Bug#3259
//              Jim Wunderlich (Jim_Wunderlich@prodigy.net)
//
//%/////////////////////////////////////////////////////////////////////////////

// #include <iostream.h>
#include "MessageQueueService.h"
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/MessageLoader.h> //l10n

PEGASUS_NAMESPACE_BEGIN

cimom *MessageQueueService::_meta_dispatcher = 0;
AtomicInt MessageQueueService::_service_count = 0;
AtomicInt MessageQueueService::_xid(1);
Mutex MessageQueueService::_meta_dispatcher_mutex;

static struct timeval deallocateWait = {300, 0};

ThreadPool *MessageQueueService::_thread_pool = 0;

DQueue<MessageQueueService> MessageQueueService::_polling_list(true);

Thread* MessageQueueService::_polling_thread = 0;

ThreadPool *MessageQueueService::get_thread_pool()
{
   return _thread_pool;
}
//
// MAX_THREADS_PER_SVC_QUEUE_LIMIT
//
// JR Wunderlich Jun 6, 2005
//

#define MAX_THREADS_PER_SVC_QUEUE_LIMIT 5000 
#define MAX_THREADS_PER_SVC_QUEUE_DEFAULT 5

Uint32 max_threads_per_svc_queue;

PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL
MessageQueueService::kill_idle_threads(void *parm)
{

   static struct timeval now, last = {0,0};
   gettimeofday(&now, NULL);
   int dead_threads = 0;

   if (now.tv_sec - last.tv_sec > 120)
   {
      gettimeofday(&last, NULL);
      try
      {
         dead_threads = MessageQueueService::_thread_pool->cleanupIdleThreads();
      }
      catch(...)
      {

      }
   }

#ifdef PEGASUS_POINTER_64BIT
   return (PEGASUS_THREAD_RETURN)(Uint64)dead_threads;
#elif PEGASUS_PLATFORM_AIX_RS_IBMCXX
   return (PEGASUS_THREAD_RETURN)(unsigned long)dead_threads;
#else
   return (PEGASUS_THREAD_RETURN)(Uint32)dead_threads;
#endif
}

PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL MessageQueueService::polling_routine(void *parm)
{
   Thread *myself = reinterpret_cast<Thread *>(parm);
   DQueue<MessageQueueService> *list = reinterpret_cast<DQueue<MessageQueueService> *>(myself->get_parm());
   while (_stop_polling.value()  == 0)
   {
      _polling_sem.wait();

      if (_stop_polling.value() != 0)
      {
         break;
      }

      // The polling_routine thread must hold the lock on the
      // _polling_thread list while processing incoming messages.
      // This lock is used to give this thread ownership of
      // services on the _polling_routine list.

      // This is necessary to avoid confict with other threads
      // processing the _polling_list
      // (e.g., MessageQueueServer::~MessageQueueService).

      list->lock();
      MessageQueueService *service = list->next(0);
      ThreadStatus rtn = PEGASUS_THREAD_OK;
      while (service != NULL)
      {
          if ((service->_incoming.count() > 0) &&
              (service->_die.value() == 0) &&
              (service->_threads < max_threads_per_svc_queue))
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
                 Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
                    "Not enough threads to process this request. Skipping.");

                 Tracer::trace(TRC_MESSAGEQUEUESERVICE, Tracer::LEVEL2,
                    "Could not allocate thread for %s. " \
                    "Queue has %d messages waiting and %d threads servicing." \
                    "Skipping the service for right now. ",
                    service->getQueueName(),
                    service->_incoming.count(),
                    service->_threads.value());

                 pegasus_yield();
                 service = NULL;
              } 
          }
          if (service != NULL) 
          {
             service = list->next(service);
          }
      }
      list->unlock();

      if (_check_idle_flag.value() != 0)
      {
         _check_idle_flag = 0;
         // try to do idle thread clean up processing when system is not busy
         // if system is busy there may not be a thread available to allocate 
         // so nothing will be done and that is OK. 

         if ( _thread_pool->allocate_and_awaken(service, kill_idle_threads, &_polling_sem) != PEGASUS_THREAD_OK)
         {
                Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
                        "Not enough threads to kill idle threads. What an irony.");

                Tracer::trace(TRC_MESSAGEQUEUESERVICE, Tracer::LEVEL2,
                        "Could not allocate thread to kill idle threads." \
                        "Skipping. ");
         }

         
      }
   }
   myself->exit_self( (PEGASUS_THREAD_RETURN) 1 );
   return(0);
}


Semaphore MessageQueueService::_polling_sem(0);
AtomicInt MessageQueueService::_stop_polling(0);
AtomicInt MessageQueueService::_check_idle_flag(0);


MessageQueueService::MessageQueueService(
   const char *name,
   Uint32 queueID,
   Uint32 capabilities,
   Uint32 mask)
   : Base(name, true,  queueID),
     _mask(mask),
     _die(0),
     _threads(0),
     _incoming(true, 0),
     _incoming_queue_shutdown(0)
{

   _capabilities = (capabilities | module_capabilities::async);

   _default_op_timeout.tv_sec = 30;
   _default_op_timeout.tv_usec = 100;

   max_threads_per_svc_queue = MAX_THREADS_PER_SVC_QUEUE;

   // if requested threads gt MAX_THREADS_PER_SVC_QUEUE_LIMIT
   // then set to MAX_THREADS_PER_SVC_QUEUE_LIMIT

   if (max_threads_per_svc_queue > MAX_THREADS_PER_SVC_QUEUE_LIMIT)
     {
       max_threads_per_svc_queue = MAX_THREADS_PER_SVC_QUEUE_LIMIT;
     }

   // if requested threads eq 0 (unlimited) 
   // then set to MAX_THREADS_PER_SVC_QUEUE_LIMIT

   if (max_threads_per_svc_queue == 0)
     {
       max_threads_per_svc_queue = MAX_THREADS_PER_SVC_QUEUE_DEFAULT;
     }

   // cout << "MAX_THREADS_PER_SVC_QUEUE = " << MAX_THREADS_PER_SVC_QUEUE << endl;
   // cout << "max_threads_per_svc_queue set to = " << max_threads_per_svc_queue << endl;
   

   AutoMutex autoMut(_meta_dispatcher_mutex);

   if (_meta_dispatcher == 0)
   {
      _stop_polling = 0;
      PEGASUS_ASSERT(_service_count.value() == 0);
      _meta_dispatcher = new cimom();
      if (_meta_dispatcher == NULL)
      {
         throw NullPointer();
      }
      //  _thread_pool = new ThreadPool(initial_cnt, "MessageQueueService",
      //   minimum_cnt, maximum_cnt, deallocateWait); 
      //
      _thread_pool =
          new ThreadPool(0, "MessageQueueService", 0, 0, deallocateWait);
   }
   _service_count++;

   if (false == register_service(name, _capabilities, _mask))
   {
      //l10n
      //throw BindFailedException("MessageQueueService Base Unable to register with  Meta Dispatcher");
      MessageLoaderParms parms("Common.MessageQueueService.UNABLE_TO_REGISTER",
         "MessageQueueService Base Unable to register with  Meta Dispatcher");

      throw BindFailedException(parms);
   }

   _polling_list.insert_last(this);

}


MessageQueueService::~MessageQueueService()
{
   _die = 1;

   // The polling_routine locks the _polling_list while
   // processing the incoming messages for services on the 
   // list.  Deleting the service from the _polling_list
   // prior to processing, avoids synchronization issues
   // with the _polling_routine.

   _polling_list.remove(this);

   // ATTN: The code for closing the _incoming queue
   // is not working correctly. In OpenPegasus 2.5,
   // execution of the following code is very timing
   // dependent. This needs to be fix.
   // See Bug 4079 for details. 
   if (_incoming_queue_shutdown.value() == 0)
   {
       _shutdown_incoming_queue();
   }

   // Wait until all threads processing the messages
   // for this service have completed.

   while (_threads.value() > 0)
   {
      pegasus_yield();
   }

   {
     AutoMutex autoMut(_meta_dispatcher_mutex);
     _service_count--;
     if (_service_count.value() == 0)
     {

      _stop_polling++;
      _polling_sem.signal();
      if (_polling_thread) {
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
   // Clean up in case there are extra stuff on the queue.
  while (_incoming.count())
  {
    try { 
      delete _incoming.remove_first();
    } catch (const ListClosed &e)
    {
      // If the list is closed, there is nothing we can do. 
      break;
    }
  }
}

void MessageQueueService::_shutdown_incoming_queue()
{
   if (_incoming_queue_shutdown.value() > 0)
      return;

   AsyncIoctl *msg = new AsyncIoctl(
      get_next_xid(),
      0,
      _queueId,
      _queueId,
      true,
      AsyncIoctl::IO_CLOSE,
      0,
      0);

   msg->op = get_op();
   msg->op->_flags |= ASYNC_OPFLAGS_FIRE_AND_FORGET;
   msg->op->_flags &= ~(ASYNC_OPFLAGS_CALLBACK | ASYNC_OPFLAGS_SAFE_CALLBACK
       | ASYNC_OPFLAGS_SIMPLE_STATUS);
   msg->op->_state &= ~ASYNC_OPSTATE_COMPLETE;

   msg->op->_op_dest = this;
   msg->op->_request.insert_first(msg);
   try {
     _incoming.insert_last_wait(msg->op);
     _polling_sem.signal();
   } catch (const ListClosed &) 
   { 
        // This means the queue has already been shut-down (happens  when there
    // are two AsyncIoctrl::IO_CLOSE messages generated and one got first
    // processed.
     delete msg; 
   }
   catch (const Permission &)
   {
     delete msg;
   }
}



void MessageQueueService::enqueue(Message *msg)
{
   PEG_METHOD_ENTER(TRC_MESSAGEQUEUESERVICE, "MessageQueueService::enqueue()");

   Base::enqueue(msg);

   PEG_METHOD_EXIT();
}


PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL MessageQueueService::_req_proc(
    void * parm)
{
    MessageQueueService* service =
            reinterpret_cast<MessageQueueService*>(parm);
    PEGASUS_ASSERT(service != 0);
    try
    {

        if (service->_die.value() != 0)
        {
            service->_threads--;
            return (0);
        }
        // pull messages off the incoming queue and dispatch them. then
        // check pending messages that are non-blocking
        AsyncOpNode *operation = 0;

        // many operations may have been queued.
        do
        {
            try
            {
                operation = service->_incoming.remove_first();
            }
            catch (ListClosed &)
            {
                // ATTN: This appears to be a common loop exit path.
                //PEG_TRACE_STRING(TRC_DISCARDED_DATA, Tracer::LEVEL2,
                //    "Caught ListClosed exception.  Exiting _req_proc.");
                break;
            }

            if (operation)
            {
               operation->_service_ptr = service;
               service->_handle_incoming_operation(operation);
            }
        } while (operation);
    }
    catch (const Exception& e)
    {
        PEG_TRACE_STRING(TRC_DISCARDED_DATA, Tracer::LEVEL2,
            String("Caught exception: \"") + e.getMessage() +
                "\".  Exiting _req_proc.");
    }
    catch (...)
    {
        PEG_TRACE_STRING(TRC_DISCARDED_DATA, Tracer::LEVEL2,
            "Caught unrecognized exception.  Exiting _req_proc.");
    }
    service->_threads--;
    return(0);
}


void MessageQueueService::_sendwait_callback(
    AsyncOpNode *op,
    MessageQueue *q,
    void *parm)
{
   op->_client_sem.signal();
}


// callback function is responsible for cleaning up all resources
// including op, op->_callback_node, and op->_callback_ptr
void MessageQueueService::_handle_async_callback(AsyncOpNode *op)
{
   if (op->_flags & ASYNC_OPFLAGS_SAFE_CALLBACK)
   {

      Message *msg = op->get_request();
      if (msg && (msg->getMask() & message_mask::ha_async))
      {
         if (msg->getType() == async_messages::ASYNC_LEGACY_OP_START)
         {
            AsyncLegacyOperationStart *wrapper =
               static_cast<AsyncLegacyOperationStart *>(msg);
            msg = wrapper->get_action();
            delete wrapper;
         }
         else if (msg->getType() == async_messages::ASYNC_MODULE_OP_START)
         {
            AsyncModuleOperationStart *wrapper =
               static_cast<AsyncModuleOperationStart *>(msg);
            msg = wrapper->get_action();
            delete wrapper;
         }
         else if (msg->getType() == async_messages::ASYNC_OP_START)
         {
            AsyncOperationStart *wrapper =
               static_cast<AsyncOperationStart *>(msg);
            msg = wrapper->get_action();
            delete wrapper;
         }
         delete msg;
      }

      msg = op->get_response();
      if (msg && (msg->getMask() & message_mask::ha_async))
      {
         if (msg->getType() == async_messages::ASYNC_LEGACY_OP_RESULT)
         {
            AsyncLegacyOperationResult *wrapper =
               static_cast<AsyncLegacyOperationResult *>(msg);
            msg = wrapper->get_result();
            delete wrapper;
         }
         else if (msg->getType() == async_messages::ASYNC_MODULE_OP_RESULT)
         {
            AsyncModuleOperationResult *wrapper =
               static_cast<AsyncModuleOperationResult *>(msg);
            msg = wrapper->get_result();
            delete wrapper;
         }
      }
      void (*callback)(Message *, void *, void *) = op->__async_callback;
      void *handle = op->_callback_handle;
      void *parm = op->_callback_parameter;
      op->release();
      return_op(op);
      callback(msg, handle, parm);
   }
   else if (op->_flags & ASYNC_OPFLAGS_CALLBACK)
   {
      // note that _callback_node may be different from op
      // op->_callback_response_q is a "this" pointer we can use for
      // static callback methods
      op->_async_callback(op->_callback_node, op->_callback_response_q, op->_callback_ptr);
   }
}


void MessageQueueService::_handle_incoming_operation(AsyncOpNode *operation)
{
   if (operation != 0)
   {

// ATTN: optimization
// << Tue Feb 19 14:10:38 2002 mdd >>
      operation->lock();

      Message *rq = operation->_request.next(0);

// optimization <<< Thu Mar  7 21:04:05 2002 mdd >>>
// move this to the bottom of the loop when the majority of
// messages become async messages.

      // divert legacy messages to handleEnqueue
      if ((rq != 0) && (!(rq->getMask() & message_mask::ha_async)))
      {
         rq = operation->_request.remove_first() ;
         operation->unlock();
         // delete the op node
         operation->release();
         return_op(operation);

         handleEnqueue(rq);
         return;
      }

      if ((operation->_flags & ASYNC_OPFLAGS_CALLBACK ||
           operation->_flags & ASYNC_OPFLAGS_SAFE_CALLBACK) &&
          (operation->_state & ASYNC_OPSTATE_COMPLETE))
      {
         operation->unlock();
         _handle_async_callback(operation);
      }
      else
      {
         PEGASUS_ASSERT(rq != 0);
         operation->unlock();
         _handle_async_request(static_cast<AsyncRequest *>(rq));
      }
   }
   return;
}

void MessageQueueService::_handle_async_request(AsyncRequest *req)
{
   if (req != 0)
   {
      req->op->processing();

      Uint32 type = req->getType();
      if (type == async_messages::HEARTBEAT)
         handle_heartbeat_request(req);
      else if (type == async_messages::IOCTL)
         handle_AsyncIoctl(static_cast<AsyncIoctl *>(req));
      else if (type == async_messages::CIMSERVICE_START)
         handle_CimServiceStart(static_cast<CimServiceStart *>(req));
      else if (type == async_messages::CIMSERVICE_STOP)
         handle_CimServiceStop(static_cast<CimServiceStop *>(req));
      else if (type == async_messages::CIMSERVICE_PAUSE)
         handle_CimServicePause(static_cast<CimServicePause *>(req));
      else if (type == async_messages::CIMSERVICE_RESUME)
         handle_CimServiceResume(static_cast<CimServiceResume *>(req));
      else if (type == async_messages::ASYNC_OP_START)
         handle_AsyncOperationStart(static_cast<AsyncOperationStart *>(req));
      else
      {
         // we don't handle this request message
         _make_response(req, async_results::CIM_NAK);
      }
   }
}


Boolean MessageQueueService::_enqueueResponse(
   Message* request,
   Message* response)
{

  STAT_COPYDISPATCHER

   PEG_METHOD_ENTER(TRC_MESSAGEQUEUESERVICE,
                    "MessageQueueService::_enqueueResponse");

   if (request->getMask() & message_mask::ha_async)
   {
      if (response->getMask() & message_mask::ha_async)
      {
         _completeAsyncResponse(static_cast<AsyncRequest *>(request),
                                static_cast<AsyncReply *>(response),
                                ASYNC_OPSTATE_COMPLETE, 0);
         PEG_METHOD_EXIT();
         return true;
      }
   }

   if (request->_async != 0)
   {
      Uint32 mask = request->_async->getMask();
      PEGASUS_ASSERT(mask & (message_mask::ha_async | message_mask::ha_request));

      AsyncRequest *async = static_cast<AsyncRequest *>(request->_async);
      AsyncOpNode *op = async->op;
      request->_async = 0;
      // the legacy request is going to be deleted by its handler
      // remove it from the op node

      static_cast<AsyncLegacyOperationStart *>(async)->get_action();

      AsyncLegacyOperationResult *async_result =
         new AsyncLegacyOperationResult(
            async->getKey(),
            async->getRouting(),
            op,
            response);
      _completeAsyncResponse(
         async,
         async_result,
         ASYNC_OPSTATE_COMPLETE,
         0);
      PEG_METHOD_EXIT();
      return true;
   }

   // ensure that the destination queue is in response->dest
   PEG_METHOD_EXIT();
   return SendForget(response);

}

void MessageQueueService::_make_response(Message *req, Uint32 code)
{
   cimom::_make_response(req, code);
}


void MessageQueueService::_completeAsyncResponse(
    AsyncRequest *request,
    AsyncReply *reply,
    Uint32 state,
    Uint32 flag)
{
   PEG_METHOD_ENTER(TRC_MESSAGEQUEUESERVICE,
                    "MessageQueueService::_completeAsyncResponse");

   cimom::_completeAsyncResponse(request, reply, state, flag);

   PEG_METHOD_EXIT();
}


void MessageQueueService::_complete_op_node(
    AsyncOpNode *op,
    Uint32 state,
    Uint32 flag,
    Uint32 code)
{
   cimom::_complete_op_node(op, state, flag, code);
}


Boolean MessageQueueService::accept_async(AsyncOpNode *op)
{
   if (_incoming_queue_shutdown.value() > 0)
      return false;
   if (_polling_thread == NULL)
   {
      _polling_thread = new Thread(
          polling_routine,
          reinterpret_cast<void *>(&_polling_list),
          false);
      ThreadStatus tr = PEGASUS_THREAD_OK;
      while ( (tr =_polling_thread->run()) != PEGASUS_THREAD_OK)
      {
        if (tr == PEGASUS_THREAD_INSUFFICIENT_RESOURCES)
           pegasus_yield();
        else
           throw Exception(MessageLoaderParms("Common.MessageQueueService.NOT_ENOUGH_THREAD",
                        "Could not allocate thread for the polling thread."));
      }
   }
// ATTN optimization remove the message checking altogether in the base
// << Mon Feb 18 14:02:20 2002 mdd >>
   op->lock();
   Message *rq = op->_request.next(0);
   Message *rp = op->_response.next(0);
   op->unlock();

   if ((rq != 0 && (true == messageOK(rq))) ||
       (rp != 0 && (true == messageOK(rp))) && _die.value() == 0)
   {
      _incoming.insert_last_wait(op);
      _polling_sem.signal();
      return true;
   }
   return false;
}

Boolean MessageQueueService::messageOK(const Message *msg)
{
   if (_incoming_queue_shutdown.value() > 0)
      return false;
   return true;
}

void MessageQueueService::handle_heartbeat_request(AsyncRequest *req)
{
   // default action is to echo a heartbeat response

   AsyncReply *reply = new AsyncReply(
      async_messages::HEARTBEAT,
      req->getKey(),
      req->getRouting(),
      0,
      req->op,
      async_results::OK,
      req->resp,
      false);
   _completeAsyncResponse(req, reply, ASYNC_OPSTATE_COMPLETE, 0);
}


void MessageQueueService::handle_heartbeat_reply(AsyncReply *rep)
{
}

void MessageQueueService::handle_AsyncIoctl(AsyncIoctl *req)
{
   switch (req->ctl)
   {
      case AsyncIoctl::IO_CLOSE:
      {
         MessageQueueService *service = static_cast<MessageQueueService *>(req->op->_service_ptr);

#ifdef MESSAGEQUEUESERVICE_DEBUG
         PEGASUS_STD(cout) << service->getQueueName() << " Received AsyncIoctl::IO_CLOSE " << PEGASUS_STD(endl);
#endif

         // respond to this message. this is fire and forget, so we don't need to delete anything.
         // this takes care of two problems that were being found
         // << Thu Oct  9 10:52:48 2003 mdd >>
          _make_response(req, async_results::OK);
         // ensure we do not accept any further messages

         // ensure we don't recurse on IO_CLOSE
         if (_incoming_queue_shutdown.value() > 0)
            break;

         // set the closing flag
         service->_incoming_queue_shutdown = 1;
         // empty out the queue
         while (1)
         {
            AsyncOpNode *operation;
            try
            {
               operation = service->_incoming.remove_first();
            }
            catch(IPCException &)
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

         // shutdown the AsyncDQueue
         service->_incoming.shutdown_queue();
         return;
      }

      default:
         _make_response(req, async_results::CIM_NAK);
   }
}

void MessageQueueService::handle_CimServiceStart(CimServiceStart *req)
{

#ifdef MESSAGEQUEUESERVICE_DEBUG
   PEGASUS_STD(cout) << getQueueName() << "received START" << PEGASUS_STD(endl);
#endif

   // clear the stoped bit and update
   _capabilities &= (~(module_capabilities::stopped));
   _make_response(req, async_results::OK);
   // now tell the meta dispatcher we are stopped
   update_service(_capabilities, _mask);

}
void MessageQueueService::handle_CimServiceStop(CimServiceStop *req)
{
#ifdef MESSAGEQUEUESERVICE_DEBUG
   PEGASUS_STD(cout) << getQueueName() << "received STOP" << PEGASUS_STD(endl);
#endif
   // set the stopeed bit and update
   _capabilities |= module_capabilities::stopped;
   _make_response(req, async_results::CIM_STOPPED);
   // now tell the meta dispatcher we are stopped
   update_service(_capabilities, _mask);
}

void MessageQueueService::handle_CimServicePause(CimServicePause *req)
{
   // set the paused bit and update
   _capabilities |= module_capabilities::paused;
   update_service(_capabilities, _mask);
   _make_response(req, async_results::CIM_PAUSED);
   // now tell the meta dispatcher we are stopped
}

void MessageQueueService::handle_CimServiceResume(CimServiceResume *req)
{
   // clear the paused  bit and update
   _capabilities &= (~(module_capabilities::paused));
   update_service(_capabilities, _mask);
   _make_response(req, async_results::OK);
   // now tell the meta dispatcher we are stopped
}

void MessageQueueService::handle_AsyncOperationStart(AsyncOperationStart *req)
{
   _make_response(req, async_results::CIM_NAK);
}

void MessageQueueService::handle_AsyncOperationResult(AsyncOperationResult *req)
{
   ;
}


void MessageQueueService::handle_AsyncLegacyOperationStart(AsyncLegacyOperationStart *req)
{
   // remove the legacy message from the request and enqueue it to its destination
   Uint32 result = async_results::CIM_NAK;

   Message *legacy = req->_act;
   if (legacy != 0)
   {
      MessageQueue* queue = MessageQueue::lookup(req->_legacy_destination);
      if (queue != 0)
      {
         if (queue->isAsync() == true)
         {
            (static_cast<MessageQueueService *>(queue))->handleEnqueue(legacy);
         }
         else
         {
            // Enqueue the response:
            queue->enqueue(req->get_action());
         }

         result = async_results::OK;
      }
   }
   _make_response(req, result);
}

void MessageQueueService::handle_AsyncLegacyOperationResult(AsyncLegacyOperationResult *rep)
{
   ;
}

AsyncOpNode *MessageQueueService::get_op()
{
   AsyncOpNode *op = new AsyncOpNode();

   op->_state = ASYNC_OPSTATE_UNKNOWN;
   op->_flags = ASYNC_OPFLAGS_SINGLE | ASYNC_OPFLAGS_NORMAL;

   return op;
}

void MessageQueueService::return_op(AsyncOpNode *op)
{
   PEGASUS_ASSERT(op->read_state() & ASYNC_OPSTATE_RELEASED);
   delete op;
}


Boolean MessageQueueService::ForwardOp(
    AsyncOpNode *op,
    Uint32 destination)
{
   PEGASUS_ASSERT(op != 0);
   op->lock();
   op->_op_dest = MessageQueue::lookup(destination);
   op->_flags |= (ASYNC_OPFLAGS_FIRE_AND_FORGET | ASYNC_OPFLAGS_FORWARD);
   op->_flags &= ~(ASYNC_OPFLAGS_CALLBACK);
   op->unlock();
   if (op->_op_dest == 0)
      return false;

   return  _meta_dispatcher->route_async(op);
}


Boolean MessageQueueService::SendAsync(
    AsyncOpNode *op,
    Uint32 destination,
    void (*callback)(AsyncOpNode *, MessageQueue *, void *),
    MessageQueue *callback_response_q,
    void *callback_ptr)
{
   PEGASUS_ASSERT(op != 0 && callback != 0);

   // get the queue handle for the destination

   op->lock();
   op->_op_dest = MessageQueue::lookup(destination); // destination of this message
   op->_flags |= ASYNC_OPFLAGS_CALLBACK;
   op->_flags &= ~(ASYNC_OPFLAGS_FIRE_AND_FORGET);
   // initialize the callback data
   op->_async_callback = callback;   // callback function to be executed by recpt. of response
   op->_callback_node = op;          // the op node
   op->_callback_response_q = callback_response_q;  // the queue that will receive the response
   op->_callback_ptr = callback_ptr;   // user data for callback
   op->_callback_request_q = this;     // I am the originator of this request

   op->unlock();
   if (op->_op_dest == 0)
      return false;

   return  _meta_dispatcher->route_async(op);
}


Boolean MessageQueueService::SendAsync(
    Message *msg,
    Uint32 destination,
    void (*callback)(Message *response, void *handle, void *parameter),
    void *handle,
    void *parameter)
{
   if (msg == NULL)
      return false;
   if (callback == NULL)
      return SendForget(msg);
   AsyncOpNode *op = get_op();
   msg->dest = destination;
   if (NULL == (op->_op_dest = MessageQueue::lookup(msg->dest)))
   {
      op->release();
      return_op(op);
      return false;
   }
   op->_flags |= ASYNC_OPFLAGS_SAFE_CALLBACK;
   op->_flags &= ~(ASYNC_OPFLAGS_FIRE_AND_FORGET);
   op->_state &= ~ASYNC_OPSTATE_COMPLETE;
   op->__async_callback = callback;
   op->_callback_node = op;
   op->_callback_handle = handle;
   op->_callback_parameter = parameter;
   op->_callback_response_q = this;

   if (!(msg->getMask() & message_mask::ha_async))
   {
      AsyncLegacyOperationStart *wrapper = new AsyncLegacyOperationStart(
         get_next_xid(),
         op,
         destination,
         msg,
         destination);
   }
   else
   {
      op->_request.insert_first(msg);
      (static_cast<AsyncMessage *>(msg))->op = op;
   }
   return _meta_dispatcher->route_async(op);
}


Boolean MessageQueueService::SendForget(Message *msg)
{
   AsyncOpNode *op = 0;
   Uint32 mask = msg->getMask();

   if (mask & message_mask::ha_async)
   {
      op = (static_cast<AsyncMessage *>(msg))->op ;
   }

   if (op == 0)
   {
      op = get_op();
      op->_request.insert_first(msg);
      if (mask & message_mask::ha_async)
      {
         (static_cast<AsyncMessage *>(msg))->op = op;
      }
   }
   op->_op_dest = MessageQueue::lookup(msg->dest);
   op->_flags |= ASYNC_OPFLAGS_FIRE_AND_FORGET;
   op->_flags &= ~(ASYNC_OPFLAGS_CALLBACK | ASYNC_OPFLAGS_SAFE_CALLBACK
       | ASYNC_OPFLAGS_SIMPLE_STATUS);
   op->_state &= ~ASYNC_OPSTATE_COMPLETE;
   if (op->_op_dest == 0)
   {
      op->release();
      return_op(op);
      return false;
   }

   // now see if the meta dispatcher will take it
   return  _meta_dispatcher->route_async(op);
}


AsyncReply *MessageQueueService::SendWait(AsyncRequest *request)
{
   if (request == 0)
      return 0 ;

   Boolean destroy_op = false;

   if (request->op == 0)
   {
      request->op = get_op();
      request->op->_request.insert_first(request);
      destroy_op = true;
   }

   request->block = false;
   request->op->_flags |= ASYNC_OPFLAGS_PSEUDO_CALLBACK;
   SendAsync(
      request->op,
      request->dest,
      _sendwait_callback,
      this,
      (void *)0);

   request->op->_client_sem.wait();

   request->op->lock();
   AsyncReply * rpl = static_cast<AsyncReply *>(request->op->_response.remove_first());
   rpl->op = 0;
   request->op->unlock();

   if (destroy_op == true)
   {
      request->op->lock();
      request->op->_request.remove(request);
      request->op->_state |= ASYNC_OPSTATE_RELEASED;
      request->op->unlock();
      return_op(request->op);
      request->op = 0;
   }
   return rpl;
}


Boolean MessageQueueService::register_service(
    String name,
    Uint32 capabilities,
    Uint32 mask)
{
   RegisterCimService *msg = new RegisterCimService(
      get_next_xid(),
      0,
      true,
      name,
      capabilities,
      mask,
      _queueId);
   msg->dest = CIMOM_Q_ID;

   Boolean registered = false;
   AsyncReply *reply = static_cast<AsyncReply *>(SendWait(msg));

   if (reply != 0)
   {
      if (reply->getMask() & message_mask::ha_async)
      {
         if (reply->getMask() & message_mask::ha_reply)
         {
            if (reply->result == async_results::OK ||
                reply->result == async_results::MODULE_ALREADY_REGISTERED)
            {
                registered = true;
            }
         }
      }

      delete reply;
   }
   delete msg;
   return registered;
}

Boolean MessageQueueService::update_service(Uint32 capabilities, Uint32 mask)
{
   UpdateCimService *msg = new UpdateCimService(
      get_next_xid(),
      0,
      true,
      _queueId,
      _capabilities,
      _mask);
   Boolean registered = false;

   AsyncMessage *reply = SendWait(msg);
   if (reply)
   {
      if (reply->getMask() & message_mask::ha_async)
      {
         if (reply->getMask() & message_mask::ha_reply)
         {
            if (static_cast<AsyncReply *>(reply)->result == async_results::OK)
            {
               registered = true;
            }
         }
      }
      delete reply;
   }
   delete msg;
   return registered;
}


Boolean MessageQueueService::deregister_service()
{

   _meta_dispatcher->deregister_module(_queueId);
   return true;
}


void MessageQueueService::find_services(
    String name,
    Uint32 capabilities,
    Uint32 mask,
    Array<Uint32> *results)
{
   if (results == 0)
   {
      throw NullPointer();
   }

   results->clear();

   FindServiceQueue *req = new FindServiceQueue(
      get_next_xid(),
      0,
      _queueId,
      true,
      name,
      capabilities,
      mask);

   req->dest = CIMOM_Q_ID;

   AsyncMessage *reply = SendWait(req);
   if (reply)
   {
      if (reply->getMask() & message_mask::ha_async)
      {
         if (reply->getMask() & message_mask::ha_reply)
         {
            if (reply->getType() == async_messages::FIND_SERVICE_Q_RESULT)
            {
               if ((static_cast<FindServiceQueueResult *>(reply))->result == async_results::OK)
                  *results = (static_cast<FindServiceQueueResult *>(reply))->qids;
            }
         }
      }
      delete reply;
   }
   delete req;
   return ;
}

void MessageQueueService::enumerate_service(Uint32 queue, message_module *result)
{
   if (result == 0)
   {
      throw NullPointer();
   }

   EnumerateService *req = new EnumerateService(
      get_next_xid(),
      0,
      _queueId,
      true,
      queue);

   AsyncMessage *reply = SendWait(req);

   if (reply)
   {
      Boolean found = false;

      if (reply->getMask() & message_mask::ha_async)
      {
         if (reply->getMask() & message_mask::ha_reply)
         {
            if (reply->getType() == async_messages::ENUMERATE_SERVICE_RESULT)
            {
               if ((static_cast<EnumerateServiceResponse *>(reply))->result == async_results::OK)
               {
                  if (found == false)
                  {
                     found = true;

                     result->put_name((static_cast<EnumerateServiceResponse *>(reply))->name);
                     result->put_capabilities((static_cast<EnumerateServiceResponse *>(reply))->capabilities);
                     result->put_mask((static_cast<EnumerateServiceResponse *>(reply))->mask);
                     result->put_queue((static_cast<EnumerateServiceResponse *>(reply))->qid);
                  }
               }
            }
         }
      }
      delete reply;
   }
   delete req;

   return;
}

Uint32 MessageQueueService::get_next_xid()
{
   static Mutex _monitor;
   Uint32 value;
   AutoMutex autoMut(_monitor);
   _xid++;
   value =  _xid.value();
   return value;

}

PEGASUS_NAMESPACE_END
