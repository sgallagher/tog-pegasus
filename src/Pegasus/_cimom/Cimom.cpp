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


void cimom::_enqueueResponse(
    Request* request,
    Message* response)
{
    // Use the same key as used in the request:

    response->setKey(request->getKey());
    response->setRouting(request->getRouting());
    
    // Lookup the message queue:

    Uint32 dst_q = request->queues.top();
    if(dst_q == CIMOM_Q_ID )
    {
       // somehow I got this response message which is for me. 
       _internal_ops.insert_last(request);
    }
    
    MessageQueue* queue = MessageQueue::lookup(request->queues.top());
    if(queue != 0)
    {
       // Enqueue the response
       

       
       if(false == queue->accept_async(response))
       {
	  delete response;
       }
    }
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
   
   AsyncOpNode *current_op , *temp;
   unlocked_dq<AsyncOpNode> completed(true);
 
   while( 0 == cim_manager->_die.value() )
   {
      // the next call always returns with a lock on the list
      cim_manager->_pending_ops.wait_for_node();
      // list is now locked 
      current_op = cim_manager->_pending_ops.next(0);
      while(current_op != 0 )
      {
	 if( current_op->_state & ASYNC_OPSTATE_COMPLETE ) 
	 {
	    // this operation is complete - remove it from the pending list
	    temp = current_op;
	    current_op = cim_manager->_pending_ops.next(current_op);
	    temp = cim_manager->_pending_ops.remove_no_lock(temp);
	    completed.insert_last(temp);
	    continue;
	 }
	 
	 // check for a timeout
	 if( true == current_op->timeout() )
	 {
	    // this operation has timed out 
	    temp = current_op;
	    current_op = cim_manager->_pending_ops.next(current_op);
	    
	    temp = cim_manager->_pending_ops.remove_no_lock(temp);
	    
	    temp->_state |= ASYNC_OPSTATE_TIMEOUT;
	    // insert it on our temporary holding list
	    if(temp != 0)
	       completed.insert_last(temp);
	    continue;
	 }
	 current_op = cim_manager->_pending_ops.next(current_op);
      }
      // unlock the pending list 
      cim_manager->_pending_ops.unlock();

      // now go through our temporary holding list and insert those nodes
      // on the completed list
      while(completed.count() > 0 )
      {
	 temp = completed.remove_first();
	 cim_manager->_completed_ops.insert_last_wait(temp);
      }
   }
   
   myself->exit_self( (PEGASUS_THREAD_RETURN) 1 );
   return(0);
}

PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL cimom::_completed_proc(void *parm)
{
   Thread *myself = reinterpret_cast<Thread *>(parm);
   cimom *cim_manager = reinterpret_cast<cimom *>(myself->get_parm());

   AsyncOpNode *current_op , *temp;
   unlocked_dq<AsyncOpNode> reply_list(true);
   unlocked_dq<AsyncOpNode> destroy_list(true);
   unlocked_dq<AsyncOpNode> recycle_list(true);
         
   
   // check the status. if timed out, delete the request & recycle 
   // the op node.
   
   // if complete, check for a response message. if a response, 
   // detach it from the opnode and enqueue it to its destination
   // (check to see if it is for the cimom first)

   while( 0 == cim_manager->_die.value() )
   {

      // the next call always returns with a lock on the list
      cim_manager->_completed_ops.wait_for_node();
      // list is now locked 
      current_op = cim_manager->_completed_ops.next(0);
      while(current_op != 0 )
      {
	 if( current_op->_state & ASYNC_OPSTATE_COMPLETE )
	 {
	    temp = current_op;
	    current_op = cim_manager->_completed_ops.next(current_op);
	    temp = cim_manager->_completed_ops.remove_no_lock(temp);
	    reply_list.insert_last(temp);
	    continue;
	 }
	 else 
	 {
	    temp = current_op;
	    current_op = cim_manager->_completed_ops.next(current_op);
	    temp = cim_manager->_completed_ops.remove_no_lock(temp);
	    destroy_list.insert_last(temp);
	    continue;
	 }
      }
      // unlock the completed list
      cim_manager->_completed_ops.unlock();
      
      // now that the list is unlocked process the operations 
      current_op = reply_list.remove_first();
      while(current_op != 0 )
      {
	 Request *rq = static_cast<Request *>(const_cast<Message *>(current_op->get_request()));
	 // there could be many reply messages for this one request 
	 Message *reply = const_cast<Message *>(current_op->get_response());
	 while(reply != 0 )
	 {
	    // enqueue each response message 
	    // this will either pass the response to another q or delete it 
	    cim_manager->_enqueueResponse(rq, reply);
	    reply = const_cast<Message *>(current_op->get_response());
	 }
	 // recycle the async op node. This clears the op nodes and piles them 
	 // up on a list that we can use for recycling. It also clears any children of 
	 // the op node and flattens the entire tree out into a list. 
	 current_op->_reset(&recycle_list);
	 // get the next op node
	 current_op = reply_list.remove_first();
      }
      
      current_op = destroy_list.remove_first();
      while(current_op != 0 )
      {
	 current_op->_reset(&recycle_list);
	 // get the next op node
	 current_op = destroy_list.remove_first();
      }
      // ok, now I need a place to put the recycled op nodes
      // this seems like a lot of extra work. resetting the op node 
      // puts it on an unlocked list. Now we have to go and remove
      // all the op nodes from the unlocked list and put them onto the 
      // cimom's locked recycle list. All of these machinations are for 
      // reducing the amount of time times we have to lock the recycle list.
      
      if( recycle_list.count() > 0 )
      {
	 cim_manager->_recycle.lock();
	 current_op = recycle_list.remove_first();
	 while(current_op != 0 )
	 {
	    cim_manager->_recycle.insert_last_no_lock(current_op);
	    current_op = recycle_list.remove_first();
	 }
	 cim_manager->_recycle.unlock();
      }
   }

   myself->exit_self( (PEGASUS_THREAD_RETURN) 1 );
   return(0);
}



// the cimom's mutex is UNLOCKED upon entry into this routine
void cimom::handleEnqueue(void)
{

    Message* request = dequeue();

    if (!request)
       return;

    if( _die.value() != 0 )
    {
       delete request;
       return;
    }

    // local list to store recycled op nodes 
    unlocked_dq<AsyncOpNode> recycled(true);

    //----- PREPROCESSING -----//
    // at a gross level, look at the message and decide if it is for the cimom or
    // for another module
    Uint32 mask = request->getMask();
    AsyncOpNode *op = 0;
    
    if((mask & message_mask::type_cimom) || (mask & message_mask::ha_reply))
    {
       // should be almost all reply messages 
       _internal_ops.insert_last(request);
       return;
    }
    else if(mask & message_mask::ha_async)
    {
       // an async request 
       op = (static_cast<AsyncRequest *>(request))->op;
              
       if(op == 0)
       {
	  // something is wrong, just drop this message
	  delete request;
	  return;
       }
       // note the reference map:
       // request->opnode
       // we enqueue the request on the appropriate message queue,
       // and save the op node on our list of new operations.
       // it is the responsibility of the message queue handling the
       // message to use the opnode to update the cimom on the 
       // status of the message's asynchronous operation
    }
    else if ( mask & message_mask::type_legacy )
    {
       // create an asynchronous "envelope" for this message
       // first try to get a recycled async op node. If that fails, 
       // allocate a new one 
       op = _recycle.remove_first();
       if (op == 0 )
	  op = new AsyncOpNode();
       if(op == 0 )
	  throw NullPointer();
       
       op->_flags = (ASYNC_OPFLAGS_NORMAL | ASYNC_OPFLAGS_SINGLE );
       op->_state = ASYNC_OPFLAGS_UNKNOWN;

       // place the original (legacy) request within the "envelope"
       op->put_request(request);
       

       ServiceAsyncOpStart *async_request = 
	  new ServiceAsyncOpStart(Message::getNextKey(),
				  QueueIdStack(CIMOM_Q_ID, CIMOM_Q_ID),
				  op);

       // redirect the message pointer to point the the "envelope"
       request = static_cast<Message *>(async_request);
       // reinitialize the mask to reflect the envelope
       mask = request->getMask();
       
       // NOTE: 
       // the reference map is as follows:
       // request->opnode->original_request 
    }
    

    //----- ROUTING -----//
    if(mask & message_mask::ha_request)
    {
       if( op == 0 || request == 0 )
	  throw NullPointer();
       
       // now give each registered module a chance to handle this request message
       // for now we bail after one module has accepted the message. 
       // in the future, multiple modules can concurrently handle the
       // same message

       MessageQueue *dst_q;
       
       // set the operation timeout to the cimom's default timeout interval
       // the module performing the operation can change the timeout interval
       // if it needs to. 
       op->set_timeout_interval(&_default_op_timeout);
       _modules.lock();
       message_module *module = _modules.next(0);
       while(module != 0)
       {
	  dst_q = MessageQueue::lookup(module->_q_id);
	  if (dst_q != 0 )
	  {
	     if( true == dst_q->accept_async(request))
	     {
		op->_state |= ASYNC_OPSTATE_ACCEPTED;
		break;
	     }
	     
	  }
	  module = _modules.next(module);
       }
       _modules.unlock();
       
       if(op->_state & ASYNC_OPSTATE_ACCEPTED)
       {
	  _pending_ops.insert_last(op);
       }
       else
       {
	  op->_reset(&recycled);
       }
    }
    else 
    {
       if( op != 0 )
	  op->_reset(&recycled);
       else
	  delete request;
    }
    
    if( recycled.count() > 0 )
    {
       _recycle.lock();
       op = recycled.remove_first();
       while(op != 0 )
       {
	  _recycle.insert_last_no_lock(op);
	  op = recycled.remove_first();
       }
       _recycle.unlock();
    }
    
    return;
}


// handles internal control messages and responses to 
// async operation requests
void cimom::handle_internal(AsyncOpNode *internal_op)
{
   return;
}


void cimom::register_module(CimomRegisterService *msg)
{
   // first see if the module is already registered
   Uint32 result = cimom_results::OK;

   if( _modules.exists( reinterpret_cast<void *>(&(msg->name))))
      result = cimom_results::MODULE_ALREADY_REGISTERED;
   else
   {
      message_module *new_mod =  new message_module(msg->name,
						    msg->capabilities,
						    msg->mask,
						    msg->q_id);
      try
      {
	 _modules.insert_first(new_mod);
      }
      catch(IPCException&)
      {
	 result = cimom_results::INTERNAL_ERROR;
      }
   }

   Reply *reply = new Reply(msg->getType(), msg->getKey(), result,
			    message_mask::type_service | message_mask::ha_reply,
			    msg->getRouting() );
   _enqueueResponse(msg, reply);
   return;

}

void cimom::deregister_module(CimomDeregisterService *msg)
{

   Uint32 result = cimom_results::OK;

   _modules.lock();
   message_module *temp = _modules.next(0);
   while( temp != 0 )
   {
      if (temp->_q_id == msg->q_id)
	 break;
      temp = _modules.next(temp);
   }
   _modules.remove_no_lock((void *)temp);
   _modules.unlock();

   if(temp == 0)
      result = cimom_results::MODULE_NOT_FOUND;
   else
      delete temp;

   Reply *reply = new Reply ( msg->getType(), msg->getKey(), result,
			      message_mask::type_service | message_mask::ha_reply,
			      msg->getRouting() );
   _enqueueResponse(msg, reply);
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



