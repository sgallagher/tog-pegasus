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
// Modified By: Josephine Eskaline Joyce (jojustin@in.ibm.com) for PEP#101
//
//%/////////////////////////////////////////////////////////////////////////////

#include "Cimom.h"

#include <iostream>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/MessageLoader.h>
#include <Pegasus/Common/AutoPtr.h>

PEGASUS_NAMESPACE_BEGIN

PEGASUS_USING_STD;

const Uint32 CIMOM_Q_ID = 1;

Uint32 module_capabilities::async =             0x00000001;
Uint32 module_capabilities::remote =            0x00000002;
Uint32 module_capabilities::trusted =           0x00000004;
Uint32 module_capabilities::paused  =           0x00000008;
Uint32 module_capabilities::stopped =           0x00000010;
Uint32 module_capabilities::module_controller = 0x00000020;



const String & message_module::get_name() const { return _name ; }
Uint32 message_module::get_capabilities() const { return _capabilities ; }
Uint32 message_module::get_mask() const { return _mask ; }
Uint32 message_module::get_queue() const { return _q_id ; }
void message_module::put_name(String & name) { _name.clear(); _name = name; }
void message_module::put_capabilities(Uint32 capabilities) {  _capabilities = capabilities; }
void message_module::put_mask(Uint32 mask) { _mask = mask; }
void message_module::put_queue(Uint32 queue) {  _q_id = queue; }


Boolean message_module::operator ==(Uint32 q) const
{
   if(this->_q_id == q)
      return true;
   return false;
}

Boolean message_module::operator == (const message_module *mm) const
{
   if(this == mm)
      return true;
   return false;
}


Boolean message_module::operator == (const String & name ) const
{
   if(name == this->_name)
      return true;
   return false;

}

Boolean message_module::operator == (const message_module & mm) const
{
   if(this == &mm)
      return true;
   if( _name == mm._name )
      if ( _capabilities == mm._capabilities)
	 if(_mask == mm._mask)
	    if(_q_id == mm._q_id)
	       return true;

   return false;

}

Boolean message_module::operator == (const void *key) const
{
   return operator == ( (*(reinterpret_cast<const message_module *>(key) ) ) );
}

AtomicInt cimom::_xid(0);

Boolean cimom::route_async(AsyncOpNode *op)
{


   if( _die.value()> 0 )
      return false;

   if( _routed_queue_shutdown.value() > 0 )
      return false;
   
   _routed_ops.insert_last_wait(op);
   
   return true;

}

void cimom::_shutdown_routed_queue()
{

   if (_routed_queue_shutdown.value() > 0 )
      return ;
   
   AutoPtr<AsyncIoctl> msg(new AsyncIoctl(get_xid(),
				    0, 
				    CIMOM_Q_ID, 
				    CIMOM_Q_ID, 
				    true, 
				    AsyncIoctl::IO_CLOSE, 
				    0, 
				    0));
   msg->op = get_cached_op();

   msg->op->_flags |= ASYNC_OPFLAGS_FIRE_AND_FORGET;
   msg->op->_flags &= ~(ASYNC_OPFLAGS_CALLBACK | ASYNC_OPFLAGS_SAFE_CALLBACK 
		   | ASYNC_OPFLAGS_SIMPLE_STATUS);
   msg->op->_state &= ~ASYNC_OPSTATE_COMPLETE;
   msg->op->_op_dest = _global_this;
   msg->op->_request.insert_first(msg.get());

   _routed_ops.insert_last_wait(msg->op);
   _routing_thread.join();
   msg.release();
   
}


PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL cimom::_routing_proc(void *parm)
{

   Thread *myself = reinterpret_cast<Thread *>(parm);
   cimom *dispatcher = reinterpret_cast<cimom *>(myself->get_parm());
   AsyncOpNode *op = 0;
   
   while( dispatcher->_die.value()  == 0 )
   {
      try 
      {
	 op = dispatcher->_routed_ops.remove_first_wait();
      }
      catch(ListClosed & )
      {
	 break;
      }
      
      if (op == 0 )
      {
	 break;
      }
      else 
      {
	 Uint32 capabilities = 0;
	 Uint32 code = 0;
	 

//      ATTN: optimization
//      <<< Sun Feb 17 18:26:39 2002 mdd >>>
//      once the opnode is enqueued on the cimom's list, the cimom owns it
//      and no one is allowed to write to it but the cimom. 
//      services are only allowed to read status bits	 
//      this can eliminate the need for the lock/unlock
//      unles reading/writing status bits

	 op->lock();
	 MessageQueue *dest_q = op->_op_dest;
	 Uint32 dest_qid = dest_q->getQueueId();
	 op->unlock();
      
	 Boolean accepted = false;

	 if(dest_qid == CIMOM_Q_ID )
	 {
	    dispatcher->_handle_cimom_op(op, myself, dispatcher);
	    accepted = true;
	 }
	 else
	 {
//          ATTN: optimization
//          <<< Sun Feb 17 18:29:26 2002 mdd >>>
//          this lock/loop/unlock is really just a safety check to ensure 
//          the service is registered with the meta dispatcher. 
//          if speed is an issue we can remove this lookup
//          because we have converted to MessageQueueService from 
//          MessageQueue, and because we register in the constructor, 
//          the safety check is unecessary
//
//   << Tue Feb 19 11:40:37 2002 mdd >>
//   moved the lookup to sendwait/nowait/forget/forward functions. 

	    MessageQueueService *dest_svc = 0;
	    
	    if ( dest_q->get_capabilities()  & module_capabilities::async)
	    {
	        dest_svc= static_cast<MessageQueueService *>(dest_q);
	    }
	    
	    if(dest_svc != 0)
	    {
	       if( dest_svc->get_capabilities() & module_capabilities::paused ||
		   dest_svc->get_capabilities() & module_capabilities::stopped )
	       {
		  // the target is stopped or paused
		  // unless the message is a start or resume
		  // just handle it from here.  
		  op->lock();
		  AsyncRequest *request = static_cast<AsyncRequest *>(op->_request.next(0));
		  op->unlock();		  
		  code = request->getType();
		  
		  if (code != async_messages::CIMSERVICE_START  &&
		      code != async_messages::CIMSERVICE_RESUME )
		  {
		     if ( dest_svc->get_capabilities() & module_capabilities::paused )
			dispatcher->_make_response(request, async_results::CIM_PAUSED);
		     else 
			dispatcher->_make_response(request, async_results::CIM_STOPPED);
		     accepted = true;
		  }
		  else // deliver the start or resume message 
			if (dest_svc->_die.value() == 0)
		     accepted = dest_svc->accept_async(op);
	       }
	       else 
			if (dest_svc->_die.value() == 0)
		  accepted = dest_svc->accept_async(op);
	    }
	    if ( accepted == false )
	    {
	       // set completion code to NAK and flag completed 
	       _complete_op_node(op, ASYNC_OPSTATE_COMPLETE, 
				 ASYNC_OPFLAGS_SIMPLE_STATUS, 
				 async_results::CIM_NAK);
	    }
	 }

      }
   } // loop 

   return(0);
}



cimom::cimom()
   : MessageQueue(PEGASUS_QUEUENAME_METADISPATCHER, true, CIMOM_Q_ID ),
     _modules(true),
     _recycle(true),
     _routed_ops(true, 0), 
     _internal_ops(true),
     _routing_thread( _routing_proc, this, false),
     _die(0), _routed_queue_shutdown(0)
{
   _capabilities |= module_capabilities::async;
   
   _global_this = static_cast<cimom *>(MessageQueue::lookup(CIMOM_Q_ID));
   
   pegasus_gettimeofday(&_last_module_change);
   _default_op_timeout.tv_sec = 30;
   _default_op_timeout.tv_usec = 100;
   ThreadStatus tr = PEGASUS_THREAD_OK;
   while ( (tr = _routing_thread.run()) != PEGASUS_THREAD_OK)
   {
      if (tr == PEGASUS_THREAD_INSUFFICIENT_RESOURCES)
        pegasus_yield();
      else
        throw Exception(MessageLoaderParms("Common.Cimom.NOT_ENOUGH_THREADS",
			"Cannot allocate thread for Cimom class"));
   }

}


Uint32 cimom::get_xid()
{
   _xid++;
   return _xid.value();
}

cimom::~cimom()
{

// send STOP messages to all modules
// shutdown legacy queues; e.g., cim operation dispatcher etc.
   _die++;
   if (_routed_queue_shutdown.value() == 0 )
      _routed_ops.shutdown_queue();
   _routing_thread.join();

   while (_modules.count())
   {
      delete _modules.remove_first();
   }
   _modules.empty_list();
   
   return;
}

void cimom::_make_response(Message *req, Uint32 code)
{
   
   if ( ! (req->getMask() & message_mask::ha_async) )
   {
      // legacy message, just delete
      delete req;
      return;
   }
   
   if( (static_cast<AsyncRequest *>(req))->op->_flags & ASYNC_OPFLAGS_FIRE_AND_FORGET )
   {
      // destructor empties request list 
      delete (static_cast<AsyncRequest *>(req))->op;
      return;
   }

   AutoPtr<AsyncReply> reply ;
   if ( ! ((static_cast<AsyncRequest *>(req))->op->_flags & ASYNC_OPFLAGS_SIMPLE_STATUS) )
   {
      reply.reset(new AsyncReply(async_messages::REPLY,
			     req->getKey(),
			     req->getRouting(),
			     0,
			     (static_cast<AsyncRequest *>(req))->op, 
			     code, 
			     (static_cast<AsyncRequest *>(req))->resp,
			     false));
   }
   else       
      (static_cast<AsyncRequest *>(req))->op->_completion_code = code;
             // sender does not want a reply message, just the 
             // _completion_code field in the AsyncOpNode.


   _completeAsyncResponse(static_cast<AsyncRequest *>(req), reply.get(), ASYNC_OPSTATE_COMPLETE, 0 );
   reply.release();
}

void cimom::_completeAsyncResponse(AsyncRequest *request,
				   AsyncReply *reply,
				   Uint32 state,
				   Uint32 flag)
{
   PEG_METHOD_ENTER(TRC_MESSAGEQUEUESERVICE,
                    "cimom::_completeAsyncResponse");

   PEGASUS_ASSERT(request != 0);

   Boolean haveLock = false;

   AsyncOpNode *op = request->op;
   op->lock();
   haveLock = true;

  if ( (op->_flags & ASYNC_OPFLAGS_CALLBACK ||
	op->_flags & ASYNC_OPFLAGS_SAFE_CALLBACK ) && 
       (! (op->_flags & ASYNC_OPFLAGS_PSEUDO_CALLBACK)) )
  {
     op->unlock();
     haveLock = false;
     if ( (reply != 0) && (false == op->_response.exists(reinterpret_cast<void *>(reply))) )
	 op->_response.insert_last(reply);
     _complete_op_node(op, state, flag, (reply ? reply->result : 0 ));
     return;
  }
   

   if( op->_flags & ASYNC_OPFLAGS_FIRE_AND_FORGET )
   {
      
      // destructor empties request list 
      op->unlock();
      haveLock = false;
      
      op->release();
      _global_this->cache_op(op);
      
      PEG_METHOD_EXIT();
      return;
   }
   
   op->_state |= (state | ASYNC_OPSTATE_COMPLETE);
   op->_flags |= flag;
   gettimeofday(&(op->_updated), NULL);
   if ( op->_flags & ASYNC_OPFLAGS_SIMPLE_STATUS )
   { 
      PEGASUS_ASSERT(reply != 0 );
      
      op->_completion_code = reply->result;
      PEG_METHOD_EXIT();
      delete reply;
   }
   else
   {
      if ( (reply != 0) && (false == op->_response.exists(reinterpret_cast<void *>(reply))) )
	 op->_response.insert_last(reply);
   }
   
   if (haveLock)
   {
      op->unlock();
      haveLock = false;
   }
   op->_client_sem.signal();
   PEG_METHOD_EXIT();
}

cimom *cimom::_global_this;


void cimom::_default_callback(AsyncOpNode *op, MessageQueue *q, void *ptr)
{
   PEGASUS_ASSERT(op != 0 && q != 0);
   return;
}


void cimom::_complete_op_node(AsyncOpNode *op, Uint32 state, Uint32 flag, Uint32 code)
{
   
   Uint32 flags;
   
   op->lock();
   
   op->_completion_code = code;
   op->_state |= (state | ASYNC_OPSTATE_COMPLETE);
   flags = (op->_flags |= flag);
   op->unlock();
   if ( flags & ASYNC_OPFLAGS_FIRE_AND_FORGET )
   {
      delete op;
      return;
   }
   
   if ( (flags & ASYNC_OPFLAGS_CALLBACK)  && (! (flags & ASYNC_OPFLAGS_PSEUDO_CALLBACK)))
   {

      // << Wed Oct  8 12:29:32 2003 mdd >>
      // check to see if the response queue is stopped or paused
      if( op->_callback_response_q == 0 || 
	  op->_callback_response_q->get_capabilities() & module_capabilities::paused ||
	  op->_callback_response_q->get_capabilities() & module_capabilities::stopped )
      {
	 // delete, respondent is paused or stopped
	 delete op;
	 return;
      }

      // send this node to the response queue
      op->_op_dest = op->_callback_response_q; 
      _global_this->route_async(op);
      return;
   }
   if((flags & ASYNC_OPFLAGS_SAFE_CALLBACK ) && (! (flags & ASYNC_OPFLAGS_PSEUDO_CALLBACK)))
   {
      op->_op_dest = op->_callback_response_q; 
      _global_this->route_async(op);
      return;
   }
   op->_client_sem.signal();
   return;
}


void cimom::handleEnqueue()
{

   Message* msg = dequeue();

   if (!msg)
      return;

   return;
}


void cimom::_handle_cimom_op(AsyncOpNode *op, Thread *thread, MessageQueue *queue)
{
   if(op == 0)
      return;

   // ATTN: optimization << Tue Feb 19 11:33:21 2002 mdd >>
   // do away with the lock/unlock 
   op->lock();
   Message *msg = op->_request.next(0);
   op->unlock();
   
   if ( msg == 0 )
      return;

   Boolean accepted = false;

   Uint32 mask = msg->getMask();
   Uint32 type = msg->getType();
   if ( ! (mask & message_mask::ha_async) )
   {
      _make_response(msg, async_results::CIM_NAK);
   }
   op->_thread_ptr = thread;
   op->_service_ptr = queue;
   
   if( mask & message_mask::ha_request)
   {
      op->processing();
      accepted = true;

      if( type == async_messages::REGISTER_CIM_SERVICE )
	 register_module(static_cast<RegisterCimService *>(msg));
      else if ( type == async_messages::UPDATE_CIM_SERVICE )
	 update_module(static_cast<UpdateCimService *>(msg ));
      else if ( type == async_messages::IOCTL )
	 ioctl(static_cast<AsyncIoctl *>(msg));
      else if ( type == async_messages::FIND_SERVICE_Q )
	 find_service_q(static_cast<FindServiceQueue *>(msg));
      else if (type == async_messages::ENUMERATE_SERVICE)
	 enumerate_service(static_cast<EnumerateService *>(msg));
      else if (type == async_messages::FIND_MODULE_IN_SERVICE)
	 _find_module_in_service(static_cast<FindModuleInService *>(msg));
      else if (type == async_messages::REGISTERED_MODULE)
	 _registered_module_in_service(static_cast<RegisteredModule *>(msg));
      else if (type == async_messages::DEREGISTERED_MODULE)
	 _deregistered_module_in_service(static_cast<DeRegisteredModule *>(msg));
   }
   if ( accepted == false )
   {
      _make_response(msg, async_results::CIM_NAK);
   }

}


void cimom::register_module(RegisterCimService *msg)
{
   // first see if the module is already registered
   Uint32 result = async_results::OK;


   if( 0 != get_module_q(msg->name))
      result = async_results::MODULE_ALREADY_REGISTERED;
   else
   {

      AutoPtr<message_module> new_mod(new message_module(msg->name,
						   msg->capabilities,
						   msg->mask,
						   msg->queue));
      if(new_mod.get() == 0 )
	 result = async_results::INTERNAL_ERROR;
      else
      {
	 try
	 {
	    _modules.insert_first(new_mod.get());
	 }
	 catch(IPCException&)
	 {
	    result = async_results::INTERNAL_ERROR;
	    new_mod.reset();
	 }
      }
     new_mod.release();
   }

   AutoPtr<AsyncReply> reply(new AsyncReply(async_messages::REPLY,
				      msg->getKey(),
				      msg->getRouting(),
				      0,
				      msg->op,
				      result,
				      msg->resp,
				      msg->block));

   _completeAsyncResponse(static_cast<AsyncRequest *>(msg),
			  reply.get(),
			  ASYNC_OPSTATE_COMPLETE,
			  0);
   reply.release();
   return;
}


void cimom::deregister_module(Uint32 quid)
{

   _modules.lock();

   message_module *temp = _modules.next(0);
   while( temp != 0 )
   {
      if (temp->_q_id == quid)
      {
	 _modules.remove_no_lock(temp);
	 break;
      }
      temp = _modules.next(temp);
   }
   _modules.unlock();
}


void cimom::update_module(UpdateCimService *msg )
{
   Uint32 result = async_results::MODULE_NOT_FOUND;

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
   _modules.unlock();

   AutoPtr<AsyncReply> reply(new AsyncReply(async_messages::REPLY,
				      msg->getKey(),
				      msg->getRouting(),
				      0,
				      msg->op,
				      result,
				      msg->resp,
				      msg->block));
   _completeAsyncResponse(static_cast<AsyncRequest *>(msg),
			  reply.get(),
			  ASYNC_OPSTATE_COMPLETE,
			  0);
    reply.release();
   return;
}


void cimom::ioctl(AsyncIoctl *msg)
{

   switch(msg->ctl)
   {
      case AsyncIoctl::IO_CLOSE:
      {
	 // save my bearings 
	 Thread *myself = msg->op->_thread_ptr;
	 cimom *service = static_cast<cimom *>(msg->op->_service_ptr);
	 
	 // respond to this message.
	 AutoPtr<AsyncReply> reply(new AsyncReply( async_messages::REPLY,
					     msg->getKey(),
					     msg->getRouting(),
					     0,
					     msg->op,
					     async_results::OK,
					     msg->resp,
					     msg->block));
	 _completeAsyncResponse(static_cast<AsyncRequest *>(msg),
				reply.get(),
				ASYNC_OPSTATE_COMPLETE,
				0);
     reply.release();
	 // ensure we do not accept any further messages

	 // ensure we don't recurse on IO_CLOSE
	 if( _routed_queue_shutdown.value() > 0 )
	    break;
	 
	 // set the closing flag 
	 service->_routed_queue_shutdown = 1;
	 // empty out the queue
	 while( 1 )
	 {
	    AsyncOpNode *operation;
	    try 
	    {
	       operation = service->_routed_ops.remove_first();
	    }
	    catch(IPCException & )
	    {
	       break;
	    }
	    if( operation )
	    {
	       service->_handle_cimom_op(operation, myself, service);
	    }
	    else
	       break;
	 } // message processing loop

	 // shutdown the AsyncDQueue
	 service->_routed_ops.shutdown_queue();
	 // exit the thread ! 
	 _die++;
	 return;
      }

      default:
      {
	 Uint32 result = _ioctl(msg->ctl, msg->intp, msg->voidp);
	 AutoPtr<AsyncReply> reply(new AsyncReply( async_messages::REPLY,
					     msg->getKey(),
					     msg->getRouting(),
					     0,
					     msg->op,
					     result,
					     msg->resp,
					     msg->block));
	 _completeAsyncResponse(static_cast<AsyncRequest *>(msg),
				reply.get(),
				ASYNC_OPSTATE_COMPLETE,
				0);
     reply.release();
      }
   }
}


Uint32 cimom::_ioctl(Uint32 code, Uint32 int_param, void *pointer_param)
{
   return async_results::OK;
}

// fill an array with queue IDs of as many registered services
// as match the request message parameters
void cimom::find_service_q(FindServiceQueue  *msg)
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

   AutoPtr<FindServiceQueueResult> reply(
      new FindServiceQueueResult( msg->getKey(),
				  msg->getRouting(),
				  msg->op,
				  async_results::OK,
				  msg->resp,
				  msg->block,
				  found));

   _completeAsyncResponse(static_cast<AsyncRequest *>(msg),
			  reply.get(),
			  ASYNC_OPSTATE_COMPLETE,
			  0);
    reply.release();
   return;
}


// given a service Queue ID, return all registation data for
// that service
void cimom::enumerate_service(EnumerateService *msg)
{

   AutoPtr<EnumerateServiceResponse> reply;
   _modules.lock();
   message_module *ret = _modules.next( 0 );

   while( ret != 0 )
   {
      if( ret->_q_id == msg->qid )
      {
	 reply.reset(new EnumerateServiceResponse(msg->getKey(),
					      msg->getRouting(),
					      msg->op,
					      async_results::OK,
					      msg->resp,
					      msg->block,
					      ret->_name,
					      ret->_capabilities,
					      ret->_mask,
					      ret->_q_id));
	 break;
      }
      ret = _modules.next(ret);
   }
   _modules.unlock();

   if(reply.get() == 0 )
   {
      reply.reset(new EnumerateServiceResponse(msg->getKey(),
					   msg->getRouting(),
					   msg->op,
					   async_results::MODULE_NOT_FOUND,
					   msg->resp,
					   msg->block,
					   String(),
					   0, 0, 0));
   }

   _completeAsyncResponse(static_cast<AsyncRequest *>(msg),
			  reply.get(),
			  ASYNC_OPSTATE_COMPLETE,
			  0);
    reply.release();

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



// returns true if the list of registered modules changes since the parameter
Boolean cimom::moduleChange(struct timeval last)
{
   if( (last.tv_sec >= _last_module_change.tv_sec))
      if(last.tv_usec >= _last_module_change.tv_usec )
	 return false;
   return true;
}


Uint32 cimom::getModuleCount()
{
   return _modules.count();
}

Uint32 cimom::getModuleIDs(Uint32 *ids, Uint32 count)
{

   if(ids == 0)
      return 0;

   message_module *temp = 0;
   _modules.lock();
   temp = _modules.next(temp);
   while( temp != 0 && count > 0 )
   {
      *ids = temp->_q_id;
      ids++;
      count--;
      temp = _modules.next(temp);
   }
   _modules.unlock();

   while( count > 0 )
   {
      *ids = 0;
      ids++;
      count--;
   }

   return _modules.count();
}

AsyncOpNode *cimom::get_cached_op()
{

   AutoPtr<AsyncOpNode> op(new AsyncOpNode());
   
   op->_state = ASYNC_OPSTATE_UNKNOWN;
   op->_flags = ASYNC_OPFLAGS_SINGLE | ASYNC_OPFLAGS_NORMAL | ASYNC_OPFLAGS_META_DISPATCHER;
   
   return op.release();
}

void cimom::cache_op(AsyncOpNode *op)
{
   PEGASUS_ASSERT(op->read_state() & ASYNC_OPSTATE_RELEASED );
   delete op;
}

void cimom::set_default_op_timeout(const struct timeval *buffer)
{
   if (buffer != 0)
   {
      _default_op_timeout.tv_sec = buffer->tv_sec;
      _default_op_timeout.tv_usec = buffer->tv_usec;
   }
   return;
}

void cimom::get_default_op_timeout(struct timeval *timeout) const
{
   if (timeout != 0)
   {
      timeout->tv_sec = _default_op_timeout.tv_sec;
      timeout->tv_usec = _default_op_timeout.tv_usec;
   }
   return;
}

void cimom::_registered_module_in_service(RegisteredModule *msg)
{
   Uint32 result = async_results::MODULE_NOT_FOUND;
   
   _modules.lock();
   message_module *ret = _modules.next( 0 );
   while( ret != 0 )
   {
      if (ret->_q_id == msg->resp)
      {
	 // see if the module is already registered
	 Uint32 i = 0;
	 for( ; i < ret->_modules.size() ; i++  )
	 {
	    if( ret->_modules[i] == msg->_module ) 
	    {
	       result = async_results::MODULE_ALREADY_REGISTERED;;
	       break;
	    }
	 }
	 if ( result != async_results::MODULE_ALREADY_REGISTERED)
	 {
	    ret->_modules.append(msg->_module);
	    result = async_results::OK;
	 }
	 break;
      }
      ret = _modules.next(ret);
   }
   _modules.unlock();
   _make_response(msg, result);
}

void cimom::_deregistered_module_in_service(DeRegisteredModule *msg)
{
   Uint32 result = async_results::MODULE_NOT_FOUND;
   
   _modules.lock();
   message_module *ret = _modules.next( 0 );
   while( ret != 0 )
   {
      if (ret->_q_id == msg->resp)
      {
	 Uint32 i = 0;
	 for( ; i < ret->_modules.size() ; i++  )
	 {
	    if( ret->_modules[i] == msg->_module )
	    {
	       ret->_modules.remove(i);
	       result = async_results::OK;
	       break;
	    }
	 }
      }
      ret = _modules.next(ret);
   }
   _modules.unlock();
   _make_response(msg, result);
}

void cimom::_find_module_in_service(FindModuleInService *msg)
{
   Uint32 result = async_results::MODULE_NOT_FOUND;
   Uint32 q_id = 0;
   
   _modules.lock();
   message_module *ret = _modules.next( 0 );
   while( ret != 0 )
   {
      if ( ret->get_capabilities() & module_capabilities::module_controller)
      {
	 // see if the module is in this service 
	 Uint32 i = 0;
	 for( ; i < ret->_modules.size() ; i++  )
	 {
	    if( ret->_modules[i] == msg->_module )
	    {
	       result = async_results::OK;
	       q_id = ret->_q_id;
	    break;
	    }
	 }
      }
      ret = _modules.next(ret);
   }
   _modules.unlock();

   FindModuleInServiceResponse *response = 
      new FindModuleInServiceResponse(msg->getRouting(), 
				      msg->getKey(),
				      msg->op, 
				      result,
				      msg->resp,
				      msg->block,
				      q_id);
	    
   _complete_op_node(msg->op, 
		     ASYNC_OPSTATE_COMPLETE, 
		     0, 
		     result);
}



PEGASUS_NAMESPACE_END



