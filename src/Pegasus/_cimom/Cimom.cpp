//%///-*-c++-*-/////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 BMC Software, Hewlett-Packard Company, IBM,
// The Open Group, Tivoli Systems
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

#include "Cimom.h"


PEGASUS_NAMESPACE_BEGIN

Uint32 module_capabilities::async =   0x00000001;
Uint32 module_capabilities::remote =  0x00000002;
Uint32 module_capabilities::trusted = 0x00000004;

AtomicInt cimom::_xid(0);

Uint32 cimom::get_xid(void)
{
   _xid++;
   return _xid.value();
}

cimom::~cimom(void)
{
   _die++;
   
   _pending_ops.empty_list();
   _pending_ops.shutdown_queue();
   _completed_ops.empty_list();
   _completed_ops.shutdown_queue(); 


// send STOP messages to all modules
// shutdown legacy queues; e.g., cim operation dispatcher etc.
   return;
   
}

void cimom::_enqueueResponse(AsyncOpNode *op)
{

   op->lock();
   
   // ATTN: make this a loop to enqueue a list of responses 
   AsyncRequest *request = static_cast<AsyncRequest *>(op->_request.next(0));
   AsyncReply *reply = static_cast<AsyncReply *>(op->_response.next(0));
   
   PEGASUS_ASSERT(request->getMask() & message_mask::ha_async);
   PEGASUS_ASSERT(reply->getMask() & message_mask::ha_async);
   
   // Use the same key as used in the request
   
   reply->setKey(request->getKey());
   reply->setRouting(request->getRouting());
   reply->dest = request->resp;
   PEGASUS_ASSERT(reply->op->_state & ASYNC_OPSTATE_COMPLETE );
   PEGASUS_ASSERT(reinterpret_cast<void *>(reply->op) == reinterpret_cast<void *>(request->op));
   
   // ensure that the released bit is cleared so we give the 
   // recipient a chance to take the message 
   reply->op->_state &= (~ASYNC_OPSTATE_RELEASED);
   
   // Lookup the message queue:
   
   
   if(reply->dest == CIMOM_Q_ID )
   {
      // ATTN:
      // somehow I got this response message which is for me. 
      
   }
   
   MessageQueue* queue = MessageQueue::lookup(reply->dest);
   if(queue != 0)
   {
      // Enqueue the response
      if(true == queue->accept_async(reply))
      {
	 if(reply->block == true)
	 {
	    reply->op->_client_sem.signal();
	 }
      }
      else
      {
	 // just set the released bit and we will clean it up automatically
	 reply->op->_state |= ASYNC_OPSTATE_RELEASED;;
      }
   }
   op->unlock();
   
   return;
}



/** Thread that monitors the progress of asynchronous operations. Will
    perform timeout analysis and will kill operations that are not 
    proceeding according to their timeout interval.
*/
PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL cimom::_pending_proc(void *parm)
{
   Thread *myself = reinterpret_cast<Thread *>(parm);
   cimom *cim_manager = reinterpret_cast<cimom *>(myself->get_parm());
   AsyncOpNode *operation ;
   
   // use these unlocked containers as temporary storage bins 
   // so we can run through the pending list as quicly as possible
   // without keeping it locked too long. 

   // another benefit is that we only need to keep one list 
   // locked at a time. 
   unlocked_dq<AsyncOpNode> completed(true);
   unlocked_dq<AsyncOpNode> recycle(true);
   
   while( 0 == cim_manager->_die.value() )
   {
      cim_manager->_pending_ops.wait_for_node();
      // list is locked 
      // get a reference to the first node
      operation = cim_manager->_pending_ops.next(0);
      // remove the first node 
      while( operation != 0 )
      {
	 // if the op is completed, put it on the completed list. 
	 // otherwise, check for a timeout. If timed out, flag it and recycle it

	 // don't call the read_state method because it updates the time stamp, 
	 // which will interfere with the timeout evaluation 
	 operation->lock();
	 Uint32 state = operation->_state;
	 operation->unlock();
	 
	 if(state & ASYNC_OPSTATE_COMPLETE)
	 {
	    AsyncOpNode *temp = operation;
	    operation = cim_manager->_pending_ops.next(operation);
	    
	    cim_manager->_pending_ops.remove_no_lock(temp);
	    completed.insert_first(temp);
	    continue;
	 }
	 else if( true == operation->timeout() )
	 {
	    AsyncOpNode *temp = operation;
	    operation = cim_manager->_pending_ops.next(operation);
	    
	    cim_manager->_pending_ops.remove_no_lock(temp);
	    recycle.insert_first(temp);
	    continue;
	 }
	 operation = cim_manager->_pending_ops.next(operation);
      } // traversing the list 
      cim_manager->_pending_ops.unlock();
      
      // now get the completed nodes out of the temporary bin and into the 
      // completed list 
      
      while (completed.count())
      {
	 // enqueue response messages from each op node 
	 operation = completed.remove_first();
	 cim_manager->_enqueueResponse(operation);
	 
	 cim_manager->_completed_ops.insert_first(operation);
      }
      
      // recycle the dead operations 
      while( recycle.count() )
      {
	 cim_manager->cache_op( recycle.remove_first() );
      }
   } // while alive 
   
   myself->exit_self( (PEGASUS_THREAD_RETURN) 1 );
   return(0);
}

PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL cimom::_completed_proc(void *parm)
{
   Thread *myself = reinterpret_cast<Thread *>(parm);
   cimom *cim_manager = reinterpret_cast<cimom *>(myself->get_parm());
   AsyncOpNode *operation ;
   
   unlocked_dq<AsyncOpNode> recycle(true);
      
   while( 0 == cim_manager->_die.value() )
   {
      cim_manager->_completed_ops.wait_for_node();
      // list is locked 
      // get a reference to the first node
      operation = cim_manager->_completed_ops.next(0);
      while( operation != 0 )
      {
	 operation->lock();
	 Uint32 state = operation->_state;
	 operation->unlock();
	 if( state & ASYNC_OPSTATE_RELEASED )
	 {
	    AsyncOpNode *temp = operation;
	    operation = cim_manager->_completed_ops.next(operation);
	    
	    cim_manager->_completed_ops.remove_no_lock(temp);
	    recycle.insert_first(temp);
	    continue;
	 }
	 else if( true == operation->timeout() )
	 {
	    AsyncOpNode *temp = operation;
	    operation = cim_manager->_completed_ops.next(operation);
	    
	    cim_manager->_completed_ops.remove_no_lock(temp);
	    recycle.insert_first(temp);
	    continue;
	 }
	 operation = cim_manager->_completed_ops.next(operation);
      } // traversing the list 
      cim_manager->_completed_ops.unlock();
   }
   // recycle the dead operations 
   while( recycle.count() )
   {
      cim_manager->cache_op( recycle.remove_first() );
   }

   myself->exit_self( (PEGASUS_THREAD_RETURN) 1 );
   return(0);
}



void cimom::handleEnqueue(void)
{

    Message* msg = dequeue();
    
    if (!msg)
       return;
    
    // if the message is not an AsyncMessage, put it in an AsyncMessage envelope. 

    if( msg->getMask() == message_mask::type_legacy )
    {

       // ATTN: use a heuristic to determine the destination queue 
       AsyncLegacyOperationStart *async_msg = 
	  new AsyncLegacyOperationStart(get_xid(),
					get_cached_op(), 
					0,
					msg);
       async_msg->op->_flags = ASYNC_OPFLAGS_NORMAL | ASYNC_OPFLAGS_SINGLE;
       async_msg->op->_state = ASYNC_OPSTATE_UNKNOWN ;
       msg = static_cast<Message *>(async_msg);
       
    }
    
    if( ! (msg->getMask() & message_mask::ha_async) )
    {
       // ATTN: use a heuristic to determine the destination queue 
       AsyncOperationStart *async_msg = 
	  new AsyncOperationStart(get_xid(), 
				  get_cached_op(),
				  0, 
				  CIMOM_Q_ID, 
				  false, 
				  msg);
       async_msg->op->_flags = ASYNC_OPFLAGS_NORMAL | ASYNC_OPFLAGS_SINGLE;
       async_msg->op->_state = ASYNC_OPSTATE_UNKNOWN ;
       msg = static_cast<Message *>(async_msg);
    }
    
    // if an async request, enqueue it on someone's list. flag it accepted
    // and put the op node on the pending list . 
    
    if( msg->getMask() & message_mask::ha_request )
    {
       Boolean accepted = false;
       MessageQueue *dest_queue;
       AsyncRequest *async_msg = static_cast<AsyncRequest *>(msg);
       
       if(async_msg->dest != 0 )
       {
	  dest_queue = MessageQueue::lookup(async_msg->dest);
	  if( dest_queue != 0 )
	  {
	     accepted = dest_queue->accept_async(msg);
	  }
       }
       else 
       {
	  // ATTN: this code offers the message to each registered module and 
	  // breaks when one module accepts the message. In the future we will 
	  // use this code to broadcast or share messages among modules. 
	  message_module *module;
	  // offer this request to each module 
	  _modules.lock();
	  module = _modules.next(0);
	  while(module != 0 )
	  {
	     dest_queue = MessageQueue::lookup(module->_q_id);
	     if (dest_queue != 0 )
	     {
		if ( true == (accepted = dest_queue->accept_async(msg)))
		   break;
	     }
	     module = _modules.next(module);
	  }
	  _modules.unlock();
       }

       // if the request is not accepted, recycle it 
       // if it is accepted, put it on the pending list 
       if(accepted == true )
	  _pending_ops.insert_last(async_msg->op);
       else
	  cache_op(async_msg->op);
       return;
    }
    
    // if the message is an AsyncResponse, ensure that the op node is flagged complete
    // and that it is on the completed list. Normally we shouldn't get a response
    // on our queue.

    if( msg->getMask() & message_mask::ha_reply )
    {
       PEGASUS_ASSERT(msg->getMask() & message_mask::ha_async );
       AsyncReply *async_msg = static_cast<AsyncReply *>(msg);
       
       // see if we can remove this reply's op node from the pending list 
       if(0 !=  _pending_ops.remove(async_msg->op) )
       {
	  // ok, flag it as completed and put it on the completed list 
	  async_msg->op->complete();
	  _completed_ops.insert_last( async_msg->op );
       }
       else
       {
	  // recycle this response. He is confused. 
	  cache_op(async_msg->op);
       }
    }
    
    return;
}

void cimom::register_module(RegisterCimService *msg)
{
   // first see if the module is already registered
   Uint32 result = async_results::OK;

   msg->op->processing();
   
   if( 0 != get_module_q(msg->name))
      result = async_results::MODULE_ALREADY_REGISTERED;
   else 
   {
      
      message_module *new_mod = new message_module(msg->name, 
						   msg->capabilities, 
						   msg->mask, 
						   msg->queue);
      if(new_mod == 0 )
	 result = async_results::INTERNAL_ERROR;
      else
      {
	 try
	 {
	    _modules.insert_first(new_mod);
	 }
	 catch(IPCException&)
	 {
	    result = async_results::INTERNAL_ERROR;
	    delete new_mod;
	 }
      }
   }
   
   AsyncReply *reply = new AsyncReply(async_messages::REPLY, 
				      msg->getKey(),
				      msg->getRouting(),
				      0, 
				      msg->op,
				      result, 
				      msg->resp, 
				      msg->block);
   
   // flag this guy as complete 
   reply->op->complete();
   return;
}


void cimom::deregister_module(DeRegisterCimService *msg)
{

   Uint32 result = async_results::OK;

   msg->op->processing();
   
   _modules.lock();
   message_module *temp = _modules.next(0);
   while( temp != 0 )
   {
      if (temp->_q_id == msg->queue)
      {
	 _modules.remove_no_lock(reinterpret_cast<void *>(temp));
	 break;
      }
      
      temp = _modules.next(temp);
   }
   _modules.unlock();

   if(temp == 0)
      result = async_results::MODULE_NOT_FOUND;
   else
      delete temp;
   
   AsyncReply *reply = new AsyncReply(async_messages::REPLY, 
				      msg->getKey(),
				      msg->getRouting(),
				      0, 
				      msg->op,
				      result, 
				      msg->resp, 
				      msg->block);
   reply->op->complete();
   return;
}


void cimom::update_module(UpdateCimService *msg )
{
   Uint32 result = async_results::MODULE_NOT_FOUND;
   msg->op->processing();
   
   _modules.lock();
   message_module *temp = _modules.next(0);
   while( temp != 0 )
   {
      if(temp->_q_id == msg->queue )
      {
	 temp->_capabilities = msg->capabilities;
	 temp->_mask = msg->mask;
	 gettimeofday(&(temp->_heartbeat), NULL);
	 result = async_results::OK;
	 break;
      }
      temp = _modules.next(temp);
   }

   AsyncReply *reply = new AsyncReply(async_messages::REPLY, 
				      msg->getKey(), 
				      msg->getRouting(),
				      0, 
				      msg->op, 
				      result, 
				      msg->resp,
				      msg->block);
   reply->op->complete();
}


void cimom::ioctl(AsyncIoctl *msg)
{

   msg->op->processing();
   
   Uint32 result = _ioctl(msg->ctl, msg->intp, msg->voidp);
   AsyncReply *reply = new AsyncReply( async_messages::REPLY, 
				       msg->getKey(), 
				       msg->getRouting(),
				       0, 
				       msg->op, 
				       result, 
				       msg->resp, 
				       msg->block);
   
   reply->op->complete();
}


Uint32 cimom::_ioctl(Uint32 code, Uint32 int_param, void *pointer_param)
{
   return async_results::OK;
}

// fill an array with queue IDs of as many registered services
// as match the request message parameters
void cimom::find_service_q(FindServiceQueue  *msg)
{

   msg->op->processing();
   
   Array<Uint32> found;
      
   _modules.lock();
   message_module *ret = _modules.next( 0 );
   while( ret != 0 )
   {
      if( msg->name.size() > 0 )
      {
	 if( msg->name != ret->_name )
	 {
	    ret = _modules.next(ret);
	    continue;
	 }
      }

      if(msg->capabilities != 0 )
      {
	 if (! msg->capabilities & ret->_capabilities)
	 {
	    ret = _modules.next(ret);
	    continue;
	 }
      }
      if(msg->mask != 0 )
      {
	 if ( ! msg->mask & ret->_mask )
	 {
	    ret = _modules.next(ret);
	    continue;
	 }
      }
      
      // if we get to here, we "found" this service 

      found.append(ret->_q_id);
      ret = _modules.next(ret);
   }
   _modules.unlock();

   FindServiceQueueResult *reply  = 
      new FindServiceQueueResult( msg->getKey(), 
				  msg->getRouting(), 
				  msg->op, 
				  async_results::OK, 
				  msg->resp, 
				  msg->block, 
				  found);
   	 
   reply->op->complete();

   return;
}


// given a service Queue ID, return all registation data for 
// that service 
void cimom::enumerate_service(EnumerateService *msg)
{

   msg->op->processing();
   EnumerateServiceResponse *reply = 0;
   _modules.lock();
   message_module *ret = _modules.next( 0 );
      
   while( ret != 0 )
   {
      if( ret->_q_id == msg->qid )
      {
	 reply = new EnumerateServiceResponse(msg->getKey(),
					      msg->getRouting(), 
					      msg->op, 
					      async_results::OK, 
					      msg->resp, 
					      msg->block, 
					      ret->_name, 
					      ret->_capabilities, 
					      ret->_mask, 
					      ret->_q_id);
	 break;
      }
      ret = _modules.next(ret);
   }
   _modules.unlock();
      
   if(reply == 0 )
   {
      reply = new EnumerateServiceResponse(msg->getKey(), 
					   msg->getRouting(), 
					   msg->op, 
					   async_results::MODULE_NOT_FOUND, 
					   msg->resp, 
					   msg->block, 
					   String(), 
					   0, 0, 0);
      
   }
   reply->op->complete();
   
   return;
}

Uint32 cimom::get_module_q(const String & name)
{
   _modules.lock();
   message_module *ret = _modules.next( 0 );
   while( ret != 0 )
   {
      if (ret->_name == name)
	 break;
      ret = _modules.next(ret);
   }

   _modules.unlock();
   if(ret != 0 )
      return ret->_q_id;
   else
      return 0 ;
}

PEGASUS_NAMESPACE_END



