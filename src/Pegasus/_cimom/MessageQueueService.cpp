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

AtomicInt MessageQueueService::_xid(1);

// mutex is UNLOCKED
void MessageQueueService::handleEnqueue(void)
{
   Message *msg = dequeue();
   if( msg )
   {
      if(msg->getMask() & message_mask::ha_async)
      {
	 (static_cast<AsyncMessage *>(msg))->op->release();
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
   AsyncOpNode *op = request->op;
   op->lock();
   if (false == op->_response.exists(reply))
      op->_response.insert_last(reply);
   
   op->_state |= state;
   op->_flags |= flag;
   gettimeofday(&(op->_updated), NULL);
   op->unlock();
}


Message *MessageQueueService::openEnvelope(Message *msg)
{
   Uint32 mask = msg->getMask();
   if( mask & message_mask::ha_async )
   {
      AsyncOpNode *op = (static_cast<AsyncMessage *>(msg))->op;
      if(op == 0 )
	 throw NullPointer();
      // ATTN
      // start pulling the last message
      // when we reach the envelope return null

   }
   return 0;
}

AsyncOpNode *MessageQueueService::_get_op(void)
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

void MessageQueueService::_return_op(AsyncOpNode *op)
{
   PEGASUS_ASSERT(op->read_state() & ASYNC_OPSTATE_RELEASED );
   
   if(op->read_state() & ASYNC_OPFLAGS_META_DISPATCHER )
   {
      _meta_dispatcher->cache_op(op);
   }
   else
      delete op;
}


void MessageQueueService::SendWait(AsyncRequest *request, unlocked_dq<AsyncMessage>& reply_list)
{
   AsyncOpNode *op = request->op;
   if(op == 0 )
      return;
   
   if(true == _meta_dispatcher->accept_async(static_cast<Message *>(request)))
   {
      op->_client_sem.wait();
      op->lock();
      while( op->_response.count() )
      {
	 AsyncMessage *rply = static_cast<AsyncMessage *>(op->_response.remove_last());
	 if (rply != 0 )
	 {
	    rply->op = 0;
	    reply_list.insert_first( rply );
	 }
      }
      // release the opnode, the meta-dispatcher will recycle it for us
      op->_state |= ASYNC_OPSTATE_RELEASED ;
      op->unlock();
   }
   else
   {
      // manually free the opnode and message
      op->release();
      _return_op(op);
   }
}

Boolean MessageQueueService::SendAsync(AsyncMessage *msg)
{
   return _meta_dispatcher->accept_async(static_cast<Message *>(msg));
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
   unlocked_dq<AsyncMessage> reply_list;
   SendWait(msg, reply_list);
   Boolean registered = false;
   
   AsyncReply *reply = static_cast<AsyncReply *>(reply_list.remove_first());
   while(reply)
   {
      if(reply->getMask() & message_mask:: ha_async)
      {
	 if(reply->getMask() & message_mask::ha_reply)
	 {
	    if(reply->result == async_results::OK)
	       registered = true;
	 }
      }
      
      delete reply;
      reply = static_cast<AsyncReply *>(reply_list.remove_first());
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
   unlocked_dq<AsyncMessage> reply_list;
   SendWait(msg, reply_list);
   Boolean registered = false;
   AsyncReply *reply = static_cast<AsyncReply *>(reply_list.remove_first());
   while(reply)
   {
      if(reply->getMask() & message_mask:: ha_async)
      {
	 if(reply->getMask() & message_mask::ha_reply)
	 {
	    if(reply->result == async_results::OK)
	       registered = true;
	 }
      }
      
      delete reply;
      reply = static_cast<AsyncReply *>(reply_list.remove_first());
   }
   return registered;
}

PEGASUS_NAMESPACE_END
