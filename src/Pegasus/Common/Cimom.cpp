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

Boolean cimom::route_async(AsyncOpNode *op)
{

   if( op == 0 )
      return false;
   
   _routed_ops.insert_last_wait(op);
   
   return true;
   
}

PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL cimom::_internal_proc(void *parm)
{
   Thread *myself = reinterpret_cast<Thread *>(parm);
   cimom *dispatcher = reinterpret_cast<cimom *>(myself->get_parm());

   while( dispatcher->_die.value() < 1 )
   {
      dispatcher->_internal_ops.wait_for_node();
      AsyncOpNode *op = dispatcher->_internal_ops.next(0);
      op = dispatcher->_internal_ops.remove_no_lock(op);
      dispatcher->_internal_ops.unlock();
      cout << " cimom internal " << op->get_request()->getRouting() << endl;
      
      dispatcher->_handle_cimom_op(op);
      
   } // loop
   
   myself->exit_self( (PEGASUS_THREAD_RETURN) 1 );
   return(0);
}


PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL cimom::_routing_proc(void *parm)
{

   Thread *myself = reinterpret_cast<Thread *>(parm);
   cimom *dispatcher = reinterpret_cast<cimom *>(myself->get_parm());

   while( dispatcher->_die.value() < 1 )
   {
      dispatcher->_routed_ops.wait_for_node();
      AsyncOpNode *op = dispatcher->_routed_ops.next(0);
      op = dispatcher->_routed_ops.remove_no_lock(op);
      dispatcher->_routed_ops.unlock();
      cout << " cimom routing " << op->get_request()->getRouting() << endl;
      AsyncRequest *request = static_cast<AsyncRequest *>(op->_request);
      PEGASUS_ASSERT(request->getMask() & message_mask::ha_async);

      Uint32 dest = request->dest;

      Boolean accepted = false;
      
      if(dest == CIMOM_Q_ID )
      {
        // put the op  on our internal list 
	 dispatcher->_internal_ops.insert_last_wait(op);
	 accepted = true;
      }
      else 
      {
	 MessageQueueService *svce = 0;
	 
	 message_module *temp = 0;
	 dispatcher->_modules.lock();
	 temp = dispatcher->_modules.next(temp);
	 while( temp != 0 )
	 {
	    if ( temp->_q_id == dest )
	    {
	       svce = static_cast<MessageQueueService *>(MessageQueue::lookup(dest));
	       accepted = true;
	       break;
	    }
	    temp = dispatcher->_modules.next(temp);
	 }
	 dispatcher->_modules.unlock();
	 if(svce != 0)
	 {
	    accepted = svce->accept_async(op);
	 }
      }

      if ( accepted == false )
      {
	 // make a NAK and flag completed 
	 AsyncReply *reply = new AsyncReply(async_messages::REPLY, 
					    request->getKey(),
					    request->getRouting(),
					    0, 
					    request->op,
					    async_results::CIM_NAK, 
					    request->resp, 
					    request->block);
	 dispatcher->_completeAsyncResponse(request, 
					     reply, 
					     ASYNC_OPSTATE_COMPLETE, 
					     0);
      }
   } // loop 
   
   myself->exit_self( (PEGASUS_THREAD_RETURN) 1 );
   return(0);
}



cimom::cimom(void)
   : MessageQueue("cimom", false, CIMOM_Q_ID),
     _modules(true), 
     _recycle(true),
     _routed_ops(true, 1000), 
     _internal_ops(true, 1000),
     _routing_thread( _routing_proc, this, false),
     _internal_thread( _internal_proc, this, false),
     _die(0)
{ 
   pegasus_gettimeofday(&_last_module_change);
   _default_op_timeout.tv_sec = 30;
   _default_op_timeout.tv_usec = 100;
   _routing_thread.run();
   _internal_thread.run();
   
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
   
   _routed_ops.shutdown_queue();
   _internal_ops.shutdown_queue(); 

   _routing_thread.join();
   _internal_thread.join();
   return;
   
}


void cimom::_completeAsyncResponse(AsyncRequest *request, 
				   AsyncReply *reply, 
				   Uint32 state, 
				   Uint32 flag)
{
   PEGASUS_ASSERT(request != 0  && reply != 0 );
   
   AsyncOpNode *op = request->op;
   cout << " cimom COMPLETE  " << op->get_request()->getRouting() << endl;
   op->lock();
//   if (false == op->_response.exists(reply))
//      op->_response.insert_last(reply);
   op->_response = reply;
   
   op->_state |= state ;
   op->_flags |= flag;
   gettimeofday(&(op->_updated), NULL);
   op->unlock();
}


void cimom::handleEnqueue(void)
{

   Message* msg = dequeue();
    
   if (!msg)
      return;
    
   return;
}

      
void cimom::_handle_cimom_op(AsyncOpNode *op)
{
   if(op == 0)
      return;
   
   Message *msg = op->get_request();
   if ( msg == 0 )
      return;
   
   Boolean accepted = false;
   
   Uint32 mask = msg->getMask();
   Uint32 type = msg->getType();
   

   if( mask & message_mask::ha_request)
   {
      accepted = true;
      
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
      cout << " cimom accepted " << op->get_request()->getRouting() << endl;
   }
   if ( accepted == false )
   {
      // we don't handle this message, reply with a NAK 
      AsyncReply *reply = new AsyncReply(async_messages::REPLY, 
					 msg->getKey(),
					 msg->getRouting(),
					 0, 
					 op,
					 async_results::CIM_NAK, 
					 (static_cast<AsyncRequest *>(msg))->resp, 
					 (static_cast<AsyncRequest *>(msg))->block);
      _completeAsyncResponse(static_cast<AsyncRequest *>(msg), 
			     reply, 
			     ASYNC_OPSTATE_COMPLETE, 
			     0);
      cout << " cimom NAK " << op->get_request()->getRouting() << endl;
   }
   
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
   
   _completeAsyncResponse(static_cast<AsyncRequest *>(msg), 
			  reply, 
			  ASYNC_OPSTATE_COMPLETE, 
			  0);
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
   _completeAsyncResponse(static_cast<AsyncRequest *>(msg), 
			  reply, 
			  ASYNC_OPSTATE_COMPLETE, 
			  0);
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
   _completeAsyncResponse(static_cast<AsyncRequest *>(msg), 
			  reply, 
			  ASYNC_OPSTATE_COMPLETE, 
			  0);
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
   _completeAsyncResponse(static_cast<AsyncRequest *>(msg), 
			  reply, 
			  ASYNC_OPSTATE_COMPLETE, 
			  0);
   
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

   _completeAsyncResponse(static_cast<AsyncRequest *>(msg), 
			  reply, 
			  ASYNC_OPSTATE_COMPLETE, 
			  0);
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
   
   _completeAsyncResponse(static_cast<AsyncRequest *>(msg), 
			  reply, 
			  ASYNC_OPSTATE_COMPLETE, 
			  0);

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
   
}

void cimom::cache_op(AsyncOpNode *op) throw(IPCException)
{
   PEGASUS_ASSERT(op->read_state() & ASYNC_OPSTATE_RELEASED );
   delete op;
   return;
       
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



