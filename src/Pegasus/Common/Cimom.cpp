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


const String & message_module::get_name(void) const { return _name ; }
Uint32 message_module::get_capabilities(void) const { return _capabilities ; }
Uint32 message_module::get_mask(void) const { return _mask ; }
Uint32 message_module::get_queue(void) const { return _q_id ; }
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
   if(*this == mm)
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


cimom::cimom(void)
   : MessageQueue("cimom", true, CIMOM_Q_ID),
     _modules(true), 
     _recycle(true),
     _pending_ops(true, 100000), 
     _completed_ops(true, 100000),
     _pending_thread( _pending_proc, this, false),
     _completed_thread( _completed_proc, this, false),
     _die(0)
{ 
   pegasus_gettimeofday(&_last_module_change);
   _default_op_timeout.tv_sec = 30;
   _default_op_timeout.tv_usec = 100;
//   _pending_thread.run();
//   _completed_thread.run();
   
}


Uint32 cimom::get_xid(void)
{
   _xid++;
   return _xid.value();
}

cimom::~cimom(void)
{

// send STOP messages to all modules
// shutdown legacy queues; e.g., cim operation dispatcher etc.
   _die++;
   
   _pending_thread.join();
   _completed_thread.join();
   
   _pending_ops.empty_list();
   _pending_ops.shutdown_queue();
   _completed_ops.empty_list();
   _completed_ops.shutdown_queue(); 

   return;
   
}

void cimom::_enqueueResponse(AsyncOpNode *op)
{

   op->lock();
   
   // ATTN: make this a loop to enqueue a list of responses 
//   AsyncRequest *request = static_cast<AsyncRequest *>(op->_request.next(0));
   AsyncRequest *request = static_cast<AsyncRequest *>(op->_request);
   AsyncReply *reply = static_cast<AsyncReply *>(op->_response);
   PEGASUS_ASSERT(request->getMask() & message_mask::ha_async);
   PEGASUS_ASSERT(reply->getMask() & message_mask::ha_async);

   
   // Use the same key as used in the request
   
   reply->setKey(request->getKey());
   reply->setRouting(request->getRouting());
   reply->dest = request->resp;
   PEGASUS_ASSERT(reply->op == request->op);
   PEGASUS_ASSERT(reply->op->_state & ASYNC_OPSTATE_COMPLETE );
   PEGASUS_ASSERT(reinterpret_cast<void *>(reply->op) == reinterpret_cast<void *>(request->op));
   
   // ensure that the released bit is cleared so we give the 
   // recipient a chance to take the message 
   reply->op->_state &= (~ASYNC_OPSTATE_RELEASED);
   
   // Lookup the message queue:
   
   if(reply->dest == CIMOM_Q_ID )
   {
      // somehow I got this response message which is for me.
      _handle_cimom_msg(reply);
      return;
   }
   
   MessageQueue* queue = MessageQueue::lookup(reply->dest);
   if(queue != 0)
   {
      // if the destination is blocking, don't enqueue the response
      if(reply->block == true)
      {
	 reply->op->_client_sem.signal();
      }
      // destination is not blocking, enqueue the response
      else if(false == queue->accept_async(reply))
      {
	 // just set the released bit and we will clean it up automatically
	 reply->op->_state |= ASYNC_OPSTATE_RELEASED;;
      }
   }
   else 
   {
      // just set the released bit and we will clean it up automatically
      reply->op->_state |= ASYNC_OPSTATE_RELEASED;;
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
   AtomicInt loop(1);
   
   
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
      loop++;
      
      // list is locked 
      // get a reference to the first node
      operation = cim_manager->_pending_ops.next(0);
      // remove the first node 
      while( operation != 0 )
      {

	 // stamp this op node with our loop counter so we exit when we
	 // have traversed the entire list 
	 if ( operation->_user_data == loop.value() )
	    break;
	 operation->_user_data = loop.value();

	 operation->lock();
	 Uint32 state = operation->_state;
	 operation->unlock();
	 
	 if(state & ASYNC_OPSTATE_COMPLETE)
	 {
	    AsyncOpNode *temp = operation;
 	    operation = cim_manager->_completed_ops.next(operation);
	    
 	    cim_manager->_completed_ops.remove_no_lock(temp);
	    cim_manager->_enqueueResponse(temp);
	    continue;
	 }
	 operation = cim_manager->_pending_ops.next(operation);
      } // traversing the list 
      cim_manager->_pending_ops.unlock();
            
      pegasus_sleep(10);
   }
   
   myself->exit_self( (PEGASUS_THREAD_RETURN) 1 );
   return(0);
}

PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL cimom::_completed_proc(void *parm)
{
   Thread *myself = reinterpret_cast<Thread *>(parm);
   cimom *cim_manager = reinterpret_cast<cimom *>(myself->get_parm());
   AsyncOpNode *operation ;
   AtomicInt loop(1);
   
   unlocked_dq<AsyncOpNode> recycle(true);
      
   while( 0 == cim_manager->_die.value() )
   {
      while(cim_manager->_completed_ops.count() < 1 )
	 pegasus_yield();
      
      cim_manager->_completed_ops.wait_for_node();
      loop++;
      
      // list is locked 
      // get a reference to the first node
      operation = cim_manager->_completed_ops.next(0);
      while( operation != 0 )
      {
	 operation->lock();
	 if ( operation->_user_data == loop.value() )
	    break;
	 
	 operation->_user_data = loop.value();
	 
	 Uint32 state = operation->_state;
	 operation->unlock();

	 if( true == operation->timeout() )
	 {
 
	    // we have enqueued a response but the recipient has not released this op node
	    // to reliably handle this error we need to hearbeat the service and
	    // see if he responds. If not, we need to unlink him from 
	    // the service module list and keep this op node on an orphaned list. 

// 	    AsyncOpNode *temp = operation;
// 	    operation = cim_manager->_completed_ops.next(operation);
	    
// 	    cim_manager->_completed_ops.remove_no_lock(temp);
// 	    recycle.insert_first(temp);
// 	    continue;
 	 }
	 operation = cim_manager->_completed_ops.next(operation);
      } // traversing the list 
      cim_manager->_completed_ops.unlock();

      // recycle the dead operations 
      while( recycle.count() )
      {
	 cim_manager->cache_op( recycle.remove_first() );
      }
      pegasus_yield();
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
	 if ( async_msg->dest == CIMOM_Q_ID )
	 {
	    _handle_cimom_msg(msg);
	    accepted = true;
	 }
	 else 
	 {
	    dest_queue = MessageQueue::lookup(async_msg->dest);
	    if( dest_queue != 0 )
	    {
	       accepted = dest_queue->accept_async(msg);
	    }
	 }
      }
      else 
      {
	 // ATTN: this code offers the message to each registered module and 
	 // breaks out of the loop when one module accepts the message. In the 
	 // future we will use this code to broadcast or share messages among 
	 // modules. 
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
      // link the op node to the pending list so we can manage 
      // the result 
      // _pending_ops.insert_last_wait(async_msg->op);

      // if the request was not accepted, send a NAK response 
      if(accepted == false )
      {
	 if ( msg->getMask() & message_mask::ha_request )
	 {
	     
	    cout << " meta generating NAK " << async_msg->dest << endl;
	    
	    AsyncReply *reply = new AsyncReply(async_messages::REPLY, 
					       msg->getKey(),
					       msg->getRouting(),
					       0, 
					       async_msg->op,
					       async_results::CIM_NAK, 
					       (static_cast<AsyncRequest *>(msg))->resp, 
					       (static_cast<AsyncRequest *>(msg))->block);
	    // flag this guy as complete 
	    reply->op->complete();
	    reply->op->_client_sem.signal();
	 }
      }
      return;
   }
    
   // if the message is an AsyncResponse, ensure that the op node is flagged complete
   // and that it is on the completed list. 

   if( msg->getMask() & message_mask::ha_reply )
   {
      PEGASUS_ASSERT(msg->getMask() & message_mask::ha_async );
      AsyncReply *async_msg = static_cast<AsyncReply *>(msg);
       
      async_msg->op->complete();
      async_msg->op->_client_sem.signal();
   }
    
   return;
}

      
void cimom::_handle_cimom_msg(Message *msg)
{
   if(msg == 0)
      return;
   
   Uint32 mask = msg->getMask();
   Uint32 type = msg->getType();
   
   if ( mask & message_mask::ha_async )
   {
      if( mask & message_mask::ha_request)
      {
	 if( type == async_messages::REGISTER_CIM_SERVICE )
	    register_module(static_cast<RegisterCimService *>(msg));
	 else if ( type == async_messages::DEREGISTER_CIM_SERVICE )
 	    deregister_module(static_cast<DeRegisterCimService *>(msg));
	 else if ( type == async_messages::UPDATE_CIM_SERVICE )
	    update_module(static_cast<UpdateCimService *>(msg ));
	 else if ( type == async_messages::IOCTL )
	    ioctl(static_cast<AsyncIoctl *>(msg));
	 else if ( type == async_messages::FIND_SERVICE_Q )
	    find_service_q(static_cast<FindServiceQueue *>(msg));
	 else if (type == async_messages::ENUMERATE_SERVICE)
	    enumerate_service(static_cast<EnumerateService *>(msg));
	 else 
	 {
	    // we don't handle this message, reply with a NAK 
	    AsyncReply *reply = new AsyncReply(async_messages::REPLY, 
					       msg->getKey(),
					       msg->getRouting(),
					       0, 
					       (static_cast<AsyncRequest *>(msg))->op,
					       async_results::CIM_NAK, 
					       (static_cast<AsyncRequest *>(msg))->resp, 
					       (static_cast<AsyncRequest *>(msg))->block);
	 }
	 
      } // an async request 
      static_cast<AsyncMessage *>(msg)->op->complete();
      static_cast<AsyncMessage *>(msg)->op->_client_sem.signal();
      
   }
   else
      delete msg;
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
	 
	 _modules.remove_no_lock(temp);
	 
	 break;
      }
      temp = _modules.next(temp);
   }

   _modules.unlock();

   if(temp == 0)
      result = async_results::MODULE_NOT_FOUND;
   else
   {
      
      delete temp;
   }
   
   
   AsyncReply *reply = new AsyncReply(async_messages::REPLY, 
				      msg->getKey(),
				      msg->getRouting(),
				      0, 
				      msg->op,
				      result, 
				      msg->resp, 
				      msg->block);
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
   return;
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


Uint32 cimom::getModuleCount(void) 
{
   return _modules.count();
}

Uint32 cimom::getModuleIDs(Uint32 *ids, Uint32 count) throw(IPCException)
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

AsyncOpNode *cimom::get_cached_op(void) throw(IPCException)
{
   AsyncOpNode *op;
   op = new AsyncOpNode(); 
   op->_flags = ASYNC_OPFLAGS_META_DISPATCHER;
   return op;
   
//    op = _recycle.remove_first();
//    if(op == 0)
//       op = new AsyncOpNode();
//    if ( op == 0 )
//       throw NullPointer();
   

//    return op;
   
}

void cimom::cache_op(AsyncOpNode *op) throw(IPCException)
{
   PEGASUS_ASSERT(op->read_state() & ASYNC_OPSTATE_RELEASED );
   delete op;
   return;
       

   unlocked_dq<AsyncOpNode> recycle;
   op->_reset(&recycle);
   while( recycle.count() )
   {
      _recycle.insert_last(recycle.remove_first());
   }
   
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

PEGASUS_NAMESPACE_END



