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
	    // it must be a timed out operation. 
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
    
    if( mask & message_mask::type_cimom )
    {
       // a message for the cimom 
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
    else if ( mask == message_mask::type_legacy )
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
       

       ServiceAsyncLegacyOpStart *async_request = 
	  new ServiceAsyncLegacyOpStart(Message::getNextKey(),
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
PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL cimom::_internal_proc(void * parm)
{
   Thread *myself = reinterpret_cast<Thread *>(parm);
   cimom *cim_manager = reinterpret_cast<cimom *>(myself->get_parm());
   Message *msg;
   
   while( 0 == cim_manager->_die.value() )
   {
      cim_manager->_internal_ops.wait_for_node();
      // list is locked now
      // get a pointer to the first msg 
      msg = cim_manager->_internal_ops.next(0);
      // remove the first msg
      msg = cim_manager->_internal_ops.remove_no_lock(msg);
      // unlock the list 
      cim_manager->_internal_ops.unlock();
      
      if (msg != 0)
      {
	 Uint32 mask = msg->getMask();
	 if( mask & message_mask::type_cimom ||
	     mask & message_mask::type_service ||
	     mask & message_mask::type_broadcast )
	 {
	    cim_manager->_handle_cimom_msg(msg);
	 }
	 else
	    delete msg;
      }
   }

   myself->exit_self( (PEGASUS_THREAD_RETURN) 1 );
   return(0);
}


void cimom::_handle_cimom_msg(Message *msg)
{
   Uint32 mask = msg->getMask();
   Uint32 type = msg->getType();

   // nothing is locked
   if(mask & message_mask::type_cimom)
   {
      if(type == cimom_messages::HEARTBEAT)
      {
	 // update the heartbeat time for the module 
      }
      else if (type == cimom_messages::ASYNC_OP_REPLY || 
	 type == cimom_messages::ASYNC_LEGACY_OP_REPLY)
      {
	 // a provider, repository, or other service is telling
	 // us that it has completed or update an async operation
	 // we need to find the async op node an handle it accordingly 
	 // look first at the pending Q
	 // look next at the pending Q

	 // Note: right now most completed operations will be handled by the 
	 // completed Q thread routine. We don't need to do anything
	 // right here for completed operations. 

	 // This code block will be put to use when we support phased 
	 // operations, transactional operations, child operations, 
	 // or remote operations. 
	 // <<< Sun Dec 30 20:51:18 2001 mdd >>>
	 
      }
      else if (type == cimom_messages::FIND_SERVICE_Q)
      {
	 find_service_q(static_cast<CimomFindServices *>(msg));
      }
      else if (type == cimom_messages::ENUMERATE_SERVICE)
      {
	 enumerate_service(static_cast<CimomEnumerateService *>(msg));
      }
      else if (type == cimom_messages::REGISTER_CIM_SERVICE)
      {
	 register_module(static_cast<CimomRegisterService *>(msg));
      }
      else if (type == cimom_messages::DEREGISTER_CIM_SERVICE)
      {
	 deregister_module(static_cast<CimomDeregisterService *>(msg));
      }
      else if (type == cimom_messages::UPDATE_CIM_SERVICE)
      {
	 update_module(static_cast<CimomUpdateService *>(msg));
      }
      else if (type == cimom_messages::IOCTL)
      {
	 ioctl(static_cast<CimomIoctl *>(msg));
      }
      delete msg;
   }
   return;
}



// fill an array with queue IDs of as many registered services
// as match the request message parameters
void cimom::find_service_q(CimomFindServices *msg)
{

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

   FindServiceQResponse *reply  = 
      new FindServiceQResponse(msg->getKey(), 
			       cimom_results::OK,
			       found, 
			       msg->getRouting() );
   
   _enqueueResponse(msg, reply);

   return;
}


// given a service Queue ID, return all registation data for 
// that service 
void cimom::enumerate_service(CimomEnumerateService *msg)
{
   _modules.lock();
   message_module *ret = _modules.next( 0 );

   EnumerateServiceResponse *reply = 0;
      
   while( ret != 0 )
   {
      if( ret->_q_id == msg->qid )
      {
	 reply = new EnumerateServiceResponse(msg->getKey(), 
					      cimom_results::OK, 
					      ret->_name, 
					      ret->_capabilities, 
					      ret->_mask, 
					      ret->_q_id, 
					      msg->getRouting() );
	 break;
      }
      
      ret = _modules.next(ret);
   }
   _modules.unlock();
      
   if(reply == 0 )
   {
      reply = new EnumerateServiceResponse(msg->getKey(),
					   cimom_results::MODULE_NOT_FOUND,
					   String(),
					   0, 0, 0, msg->getRouting() );
      
   }
   
   _enqueueResponse(msg, reply);

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

   Reply *reply = new Reply(service_messages::REPLY, msg->getKey(), result,
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

   Reply *reply = new Reply ( service_messages::REPLY, msg->getKey(), result,
			      message_mask::type_service | message_mask::ha_reply,
			      msg->getRouting() );
   _enqueueResponse(msg, reply);
   return;
}


void cimom::update_module(CimomUpdateService *msg)
{
   Uint32 result = cimom_results::MODULE_NOT_FOUND;
   
   _modules.lock();
   message_module *temp = _modules.next(0);
   while( temp != 0 )
   {
      if(temp->_q_id == msg->q_id )
      {
	 temp->_capabilities = msg->capabilities;
	 temp->_mask = msg->mask;
	 gettimeofday(&(temp->_heartbeat), NULL);
	 result = cimom_results::OK;
	 break;
      }
      temp = _modules.next(temp);
   }

   Reply *reply = new Reply ( service_messages::REPLY, msg->getKey(), result,
			      message_mask::type_service | message_mask::ha_reply,
			      msg->getRouting() );

   _enqueueResponse(msg, reply);
}


void cimom::ioctl(CimomIoctl *msg)
{
   Uint32 result = _ioctl(msg->code, msg->uint, msg->pparam);
   Reply *reply = new Reply ( service_messages::REPLY, msg->getKey(), result,
			      message_mask::type_service | message_mask::ha_reply,
			      msg->getRouting() );
   
   _enqueueResponse(msg, reply);
}


Uint32 cimom::_ioctl(Uint32 code, Uint32 int_param, void *pointer_param)
{
   return cimom_results::OK;
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



