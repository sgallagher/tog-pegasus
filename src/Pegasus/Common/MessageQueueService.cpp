//%////-*-c++-*-////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 The Open group, BMC Software, Tivoli Systems, IBM
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
//
//%/////////////////////////////////////////////////////////////////////////////

#include "MessageQueueService.h"

PEGASUS_NAMESPACE_BEGIN


PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL MessageQueueService::_req_proc(void * parm)
{
   Thread *myself = reinterpret_cast<Thread *>(parm);
   MessageQueueService *service = reinterpret_cast<MessageQueueService *>(myself->get_parm());

   // pull messages off the incoming queue and dispatch them. then 
   // check pending messages that are non-blocking

   while ( service->_die.value() < 1 )
   {

      AsyncOpNode *operation = service->_incoming.remove_first_wait();
      service->_handle_incoming_operation(operation);
   }
   
   myself->exit_self( (PEGASUS_THREAD_RETURN) 1 );
   return(0);
}

PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL MessageQueueService::_rpl_proc(void * parm)
{
   Thread *myself = reinterpret_cast<Thread *>(parm);
   MessageQueueService *service = reinterpret_cast<MessageQueueService *>(myself->get_parm());

   // pull messages off the _pending queue and complete them. 
   // this loop is not optimized because every time we complete an operation 
   // we start again to traverse the loop at the beginning. 
   Uint32 loop = 1;
   Boolean unlinked = false;
   
   
   while ( service->_die.value() < 1 )
   {
      service->_pending.wait_for_node();
      AsyncOpNode *operation = service->_pending.next(0);
//      operation = service->_pending.remove_no_lock(operation);
      while( operation != 0 )
      {
	 if ( operation->_user_data != loop )
	 {
	    Uint32 state = operation->read_state();
	    
	    if ( state  & ASYNC_OPSTATE_COMPLETE )
	    {
	       service->_pending.remove_no_lock(operation) ;
	       operation->_client_sem.signal();
	       operation = service->_pending.next(0);
	       unlinked = true;
	       continue;
	    }
	    operation->_user_data = loop;
	 }
	 operation = service->_pending.next(operation);
      }
      service->_pending.unlock();
      loop++;
      if ( unlinked == true)
      {
	 pegasus_yield();
	 unlinked = false;
      }
   }
   
   myself->exit_self( (PEGASUS_THREAD_RETURN) 1 );
   return(0);
}


void MessageQueueService::_handle_incoming_operation(AsyncOpNode *operation)
{
   if ( operation != 0 )
   {
      Message *rq = operation->get_request();
      PEGASUS_ASSERT(rq != 0 );
      PEGASUS_ASSERT(rq->getMask() & message_mask::ha_async );
      PEGASUS_ASSERT(rq->getMask() & message_mask::ha_request);
      _handle_async_request(static_cast<AsyncRequest *>(rq));
   }
   
   return;
   
}


Boolean MessageQueueService::messageOK(const Message *msg)
{
   if ( msg != 0 )
   {
      Uint32 mask = msg->getMask();
      if ( mask & message_mask::ha_async)
	 if ( mask & message_mask::ha_request)
	    return true;
   }
   return false;
}


MessageQueueService::MessageQueueService(const char *name, 
					 Uint32 queueID, 
					 Uint32 capabilities, 
					 Uint32 mask) 
   : Base(name, false,  queueID),
     _capabilities(capabilities),
     _mask(mask),
     _die(0),
     _pending(true, 1000), 
     _incoming(true, 1000),
     _req_thread(_req_proc, this, false),
     _rpl_thread(_rpl_proc, this, false)
{ 
   _default_op_timeout.tv_sec = 30;
   _default_op_timeout.tv_usec = 100;
   _meta_dispatcher = static_cast<cimom *>(Base::lookup(CIMOM_Q_ID));
   if(_meta_dispatcher == 0 )
      throw NullPointer();
   _req_thread.run();
   _rpl_thread.run();
   
}


MessageQueueService::~MessageQueueService(void)
{
   _die = 1;
   _pending.shutdown_queue();
   _incoming.shutdown_queue();
   _req_thread.join();
   _rpl_thread.join();
}

AtomicInt MessageQueueService::_xid(1);


Boolean MessageQueueService::accept_async(AsyncOpNode *op) throw(IPCException)
{
   // incoming async request message
   
   PEGASUS_ASSERT(op != 0 );
   
   Message *rq = op->get_request();
   if ( true == messageOK(rq) )
   {
      try 
      {
	 _incoming.insert_first_wait(op);
      }
      catch ( IPCException& e)
      {
	 return false ;
      }
      return true;
   }
   return false;
}


void MessageQueueService::_completeAsyncResponse(AsyncRequest *request, 
						AsyncReply *reply, 
						Uint32 state, 
						Uint32 flag)
{
   PEGASUS_ASSERT(request != 0  && reply != 0 );
   
   AsyncOpNode *op = request->op;
   op->lock();
//   if (false == op->_response.exists(reply))
//      op->_response.insert_last(reply);
   op->_response = reply;
   
   op->_state |= state ;
   op->_flags |= flag;
   gettimeofday(&(op->_updated), NULL);
   op->unlock();
}

void MessageQueueService::handleEnqueue(void)
{
   Message *msg = dequeue();
   if( msg )
   {
      delete msg;
   }
}

void MessageQueueService::_handle_async_request(AsyncRequest *req)
{
   if ( req != 0 )
   {
      req->op->processing();
   
      Uint32 type = req->getType();
      if( type == async_messages::HEARTBEAT )
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
      else if ( type == async_messages::ASYNC_OP_START)
	 handle_AsyncOperationStart(static_cast<AsyncOperationStart *>(req));
      else 
      {
	 // we don't handle this request message 
	 _make_response(req, async_results::CIM_NAK );
      }
   }
}

void MessageQueueService::_handle_async_reply(AsyncReply *rep)
{

   if (rep->op != 0 )
      rep->op->processing();
   
   Uint32 type = rep->getType();
   
   if ( type == async_messages::ASYNC_OP_RESULT )
      handle_AsyncOperationResult(static_cast<AsyncOperationResult *>(rep));
   else 
   {
      // we don't handle this reply
      ;
   }

   if( rep->op != 0 )
      rep->op->release();
}

void MessageQueueService::_make_response(AsyncRequest *req, Uint32 code)
{
   AsyncReply *reply = 
      new AsyncReply(async_messages::REPLY,
		     req->getKey(),
		     req->getRouting(),
		     0,
		     req->op, 
		     code, 
		     req->resp,
		     false);
   _completeAsyncResponse(req, reply, ASYNC_OPSTATE_COMPLETE, 0 );
}


void MessageQueueService::handle_heartbeat_request(AsyncRequest *req)
{
   // default action is to echo a heartbeat response 
   
   AsyncReply *reply = 
      new AsyncReply(async_messages::HEARTBEAT,
		     req->getKey(),
		     req->getRouting(),
		     0,
		     req->op, 
		     async_results::OK, 
		     req->resp,
		     false);
   _completeAsyncResponse(req, reply, ASYNC_OPSTATE_COMPLETE, 0 );
}


void MessageQueueService::handle_heartbeat_reply(AsyncReply *rep)
{ 
   ;
}
      
void MessageQueueService::handle_AsyncIoctl(AsyncIoctl *req)
{
   _make_response(req, async_results::OK);
}
void MessageQueueService::handle_CimServiceStart(CimServiceStart *req)
{
   _make_response(req, async_results::CIM_NAK);
}
void MessageQueueService::handle_CimServiceStop(CimServiceStop *req)
{
   _make_response(req, async_results::CIM_NAK);
}
void MessageQueueService::handle_CimServicePause(CimServicePause *req)
{
   _make_response(req, async_results::CIM_NAK);
}
void MessageQueueService::handle_CimServiceResume(CimServiceResume *req)
{
   _make_response(req, async_results::CIM_NAK);
}
      
void MessageQueueService::handle_AsyncOperationStart(AsyncOperationStart *req)
{
   _make_response(req, async_results::CIM_NAK);

}

void MessageQueueService::handle_AsyncOperationResult(AsyncOperationResult *req)
{
   ;
}

AsyncOpNode *MessageQueueService::get_op(void)
{
   AsyncOpNode *op = new AsyncOpNode();
   
   op->write_state(ASYNC_OPSTATE_UNKNOWN);
   op->write_flags(ASYNC_OPFLAGS_SINGLE | ASYNC_OPFLAGS_NORMAL );
   
   return op;
}

void MessageQueueService::return_op(AsyncOpNode *op)
{
   PEGASUS_ASSERT(op->read_state() & ASYNC_OPSTATE_RELEASED );
   delete op;
}



AsyncReply *MessageQueueService::SendWait(AsyncRequest *request)
{
   if ( request == 0 )
      return 0 ;
   PEGASUS_ASSERT( request->op != 0 );
   
   request->block = true;
   request->op->_state &= ~ASYNC_OPSTATE_COMPLETE;
   request->op->put_response(0);
   
   // first link it on our pending list
   _pending.insert_last_wait(request->op);
   
   // now see if the meta dispatcher will take it

   if (true == _meta_dispatcher->route_async(request->op))
   {
      request->op->_client_sem.wait();
   }

   return static_cast<AsyncReply *>(request->op->get_response());
   
}


Boolean MessageQueueService::register_service(String name, 
					      Uint32 capabilities, 
					      Uint32 mask)

{
   AsyncOpNode *op = _meta_dispatcher->get_cached_op();
   
   op->_state |= ASYNC_OPSTATE_UNKNOWN;
   op->_flags |= ASYNC_OPFLAGS_SINGLE | ASYNC_OPFLAGS_NORMAL;
   
   RegisterCimService *msg = new RegisterCimService(get_next_xid(),
						    op, 
						    true, 
						    name, 
						    capabilities, 
						    mask,
						    _queueId);
   Boolean registered = false;
   AsyncMessage *reply = SendWait( msg );
   
   if ( reply != 0 )
   {
      if(reply->getMask() & message_mask:: ha_async)
      {
	 if(reply->getMask() & message_mask::ha_reply)
	 {
	    if((static_cast<AsyncReply *>(reply))->result == async_results::OK)
	       registered = true;
	    
	 }
      }
      
      delete reply;
   }
   return registered;
}

Boolean MessageQueueService::update_service(Uint32 capabilities, Uint32 mask)
{
   
   AsyncOpNode *op = _meta_dispatcher->get_cached_op();
   op->_state |= ASYNC_OPSTATE_UNKNOWN;
   op->_flags |= ASYNC_OPFLAGS_SINGLE | ASYNC_OPFLAGS_NORMAL;
   
   UpdateCimService *msg = new UpdateCimService(get_next_xid(), 
						op, 
						true, 
						_queueId,
						_capabilities, 
						_mask);
   Boolean registered = false;


   AsyncMessage *reply = SendWait(msg);
   if (reply)
   {
      if(reply->getMask() & message_mask:: ha_async)
      {
	 if(reply->getMask() & message_mask::ha_reply)
	 {
	    if(static_cast<AsyncReply *>(reply)->result == async_results::OK)
	       registered = true;
	 }
      }
      delete reply;
   }
   return registered;
}


Boolean MessageQueueService::deregister_service(void)
{

//   _meta_dispatcher->deregister_module(_queueId);
//   return true;
   

   AsyncOpNode *op = _meta_dispatcher->get_cached_op();
   op->_state |= ASYNC_OPSTATE_UNKNOWN;
   op->_flags |= ASYNC_OPFLAGS_SINGLE | ASYNC_OPFLAGS_NORMAL;
   
   DeRegisterCimService *msg = new DeRegisterCimService(get_next_xid(), 
							op, 
							true, 
							_queueId);
   Boolean deregistered = false;



   return _meta_dispatcher->accept_async(static_cast<Message *>(msg));
}


void MessageQueueService::find_services(String name, 
					Uint32 capabilities, 
					Uint32 mask, 
					Array<Uint32> *results)
{
   
   if( results == 0 )
      throw NullPointer();
   
   AsyncOpNode *op = get_op();
   results->clear();
   
   FindServiceQueue *req = 
      new FindServiceQueue(get_next_xid(), 
			   op, 
			   _queueId, 
			   true, 
			   name, 
			   capabilities, 
			   mask);
   
   AsyncMessage *reply = SendWait(req); 
   if(reply)
   {
      if( reply->getMask() & message_mask::ha_async)
      {
	 if(reply->getMask() & message_mask::ha_reply)
	 {
	    if(reply->getType() == async_messages::FIND_SERVICE_Q_RESULT)
	    {
	       if( (static_cast<FindServiceQueueResult *>(reply))->result == async_results::OK )
		  *results = (static_cast<FindServiceQueueResult *>(reply))->qids;
	    }
	 }
      }
      delete reply;
   }
   return ;
}

void MessageQueueService::enumerate_service(Uint32 queue, message_module *result)
{
   if(result == 0)
      throw NullPointer();
   
   AsyncOpNode *op = get_op();
   
   EnumerateService *req 
      = new EnumerateService(get_next_xid(),
			     op, 
			     _queueId, 
			     true, 
			     queue);
   
   AsyncMessage *reply = SendWait(req);
   
   if (reply)
   {
      Boolean found = false;
      
      if( reply->getMask() & message_mask::ha_async)
      {
	 if(reply->getMask() & message_mask::ha_reply)
	 {
	    if(reply->getType() == async_messages::ENUMERATE_SERVICE_RESULT)
	    {
	       if( (static_cast<EnumerateServiceResponse *>(reply))->result == async_results::OK )
	       {
		  if( found == false)
		  {
		     found = true;
		     
		     result->put_name( (static_cast<EnumerateServiceResponse *>(reply))->name);
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
   return;
}

Uint32 MessageQueueService::get_next_xid(void)
{
   _xid++;
   return _xid.value();
}

PEGASUS_NAMESPACE_END
