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

MessageQueueService::MessageQueueService(const char *name, 
					 Uint32 queueID, 
					 Uint32 capabilities, 
					 Uint32 mask) 
   : Base(name, false, queueID),
     _capabilities(capabilities),
     _mask(mask),
     _die(0)
{ 
   _default_op_timeout.tv_sec = 30;
   _default_op_timeout.tv_usec = 100;
   _meta_dispatcher = static_cast<cimom *>(Base::lookup(CIMOM_Q_ID));
   if(_meta_dispatcher == 0 )
      throw NullPointer();
}

MessageQueueService::~MessageQueueService(void)
{
   _die = 1;
   
}


AtomicInt MessageQueueService::_xid(1);

// mutex is UNLOCKED
void MessageQueueService::handleEnqueue(void)
{
   Message *msg = dequeue();
   if( msg )
   {
      if(msg->getMask() & message_mask::ha_async)
      {
	 _handle_async_msg(static_cast<AsyncMessage *>(msg));
      }
      else
	 delete msg;
   }
}


void MessageQueueService::_enqueueAsyncResponse(AsyncRequest *request, 
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
   op->_client_sem.signal();
   
}

// may be overriden by derived classes
void MessageQueueService::_handle_async_msg(AsyncMessage *msg)
{
   if( msg == 0 )
      return;
   
   Uint32 mask = msg->getMask();
   Uint32 type = msg->getType();
   
   if (mask & message_mask::ha_async)
   {
      if (mask & message_mask::ha_request)
	 _handle_async_request(static_cast<AsyncRequest *>(msg));
      else 
	 _handle_async_reply(static_cast<AsyncReply *>(msg));
   }
   else
      delete msg;
}

void MessageQueueService::_handle_async_request(AsyncRequest *req)
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
   _enqueueAsyncResponse(req, reply, ASYNC_OPSTATE_COMPLETE, 0 );
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
   _enqueueAsyncResponse(req, reply, ASYNC_OPSTATE_COMPLETE, 0 );
   
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
   AsyncOpNode *op = _meta_dispatcher->get_cached_op();
   if(op == 0 )
      throw NullPointer();
   
   op->write_state(ASYNC_OPSTATE_UNKNOWN);
   op->write_flags(ASYNC_OPFLAGS_SINGLE | 
		   ASYNC_OPFLAGS_NORMAL | 
		   ASYNC_OPFLAGS_META_DISPATCHER);
   return op;
}

void MessageQueueService::return_op(AsyncOpNode *op)
{
   PEGASUS_ASSERT(op->read_state() & ASYNC_OPSTATE_RELEASED );
   
   if(op->read_state() & ASYNC_OPFLAGS_META_DISPATCHER )
   {
      _meta_dispatcher->cache_op(op);
   }
   else
      delete op;
}

AsyncMessage *MessageQueueService::SendWait(AsyncRequest *request)
{
   if (request == 0 )
      throw NullPointer();
   AsyncMessage *ret_msg = 0;
   
   AsyncOpNode *op = request->op;
   
   if(op == 0 )
      return 0;
   //     ATTN: debugging 
   op->put_response(0);
   if(true == _meta_dispatcher->accept_async(static_cast<Message *>(request)))
   {
      op->_client_sem.wait();
      ret_msg = static_cast<AsyncMessage *>(op->_response);
      op->_response = 0;
            
      
//      op->lock();
//       while( op->_response.count() )
//       {
// 	 AsyncMessage *rply = static_cast<AsyncMessage *>(op->_response.remove_last());
// 	 if (rply != 0 )
// 	 {
// 	    rply->op = 0;
//      reply_list->insert_first( static_cast<AsyncMessage *>(op->_response) );
// 	 }
//       }
      // release the opnode, the meta-dispatcher will recycle it for us
//      op->_state |= ASYNC_OPSTATE_RELEASED ;
//      op->unlock();
   }
   return ret_msg;
   
}


void MessageQueueService::SendWait(AsyncRequest *request, unlocked_dq<AsyncMessage> *reply_list)
{
   if (request == 0 || reply_list == 0 )
      throw NullPointer();
   
   AsyncOpNode *op = request->op;
   
   if(op == 0 )
      return;
   //     ATTN: debugging 
   op->put_response(0);
   if(true == _meta_dispatcher->accept_async(static_cast<Message *>(request)))
   {

      
      op->_client_sem.wait();
      op->lock();
      
      AsyncMessage *response = static_cast<AsyncMessage *>(op->_response);
      reply_list->insert_first(response);
      op->unlock();
      
      op->release();
      
//      op->lock();
//       while( op->_response.count() )
//       {
// 	 AsyncMessage *rply = static_cast<AsyncMessage *>(op->_response.remove_last());
// 	 if (rply != 0 )
// 	 {
// 	    rply->op = 0;
//      reply_list->insert_first( static_cast<AsyncMessage *>(op->_response) );
// 	 }
//       }
      // release the opnode, the meta-dispatcher will recycle it for us
//      op->_state |= ASYNC_OPSTATE_RELEASED ;
//      op->unlock();
   }
   else
   {
      // manually free the opnode and message
      op->release();
      return_op(op);
   }
}

// Boolean MessageQueueService::SendAsync(AsyncMessage *msg)
// {
//    return _meta_dispatcher->accept_async(static_cast<Message *>(msg));
// }


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
	    cout << " service registered " << _queueId << endl;
	    
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
