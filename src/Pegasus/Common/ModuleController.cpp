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
// Author: Mike Day (mdday@us.ibm.com) << Tue Mar 19 13:19:24 2002 mdd >>
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include "ModuleController.h"
 

PEGASUS_NAMESPACE_BEGIN

PEGASUS_USING_STD;
 

pegasus_module::module_rep::module_rep(ModuleController *controller, 
				       const String & name,
				       void *module_address, 
				       Message * (*receive_message)(Message *, void *),
				       void (*async_callback)(Uint32, Message *, void *),
				       void (*shutdown_notify)(Uint32 code, void *))
   : Base ( pegasus_internal_identity(peg_credential_types::MODULE) ),
     _thread_safety(),
     _controller(controller), 
     _name(name), 
     _reference_count(1), 
     _shutting_down(0),
     _module_address(module_address)

{
   if(receive_message != NULL)
      _receive_message = receive_message;
   else
      _receive_message = default_receive_message;
   if(async_callback != NULL)
      _async_callback = async_callback;
   else
      _async_callback = default_async_callback;
   if(shutdown_notify != NULL)
      _shutdown_notify = shutdown_notify;
   else
      _shutdown_notify = default_shutdown_notify;
}


pegasus_module::module_rep::~module_rep(void)
{
   _send_shutdown_notify();
}

Message * pegasus_module::module_rep::module_receive_message(Message *msg)
{
   Message * ret;
   _thread_safety.lock(pegasus_thread_self());
   try {  ret = _receive_message(msg, _module_address); }
   catch(...) { _thread_safety.unlock(); throw; }
   _thread_safety.unlock();
   return ret;
}

void pegasus_module::module_rep::_send_async_callback(Uint32 msg_handle, Message *msg, void *parm)
{
   _thread_safety.lock(pegasus_thread_self());
   try  { _async_callback(msg_handle, msg, parm); }
   catch(...) { _thread_safety.unlock(); throw; }
}
 
void pegasus_module::module_rep::_send_shutdown_notify(void)
{
   _thread_safety.lock(pegasus_thread_self());
   if(_reference_count.value() == 0 )
   {
      if( _shutting_down == 0 )
      {
	 _shutting_down++;
	 _shutdown_notify(_reference_count.value(), _module_address);
	 _async_callback = closed_async_callback;
	 _receive_message = closed_receive_message;
      }
   }
   _thread_safety.unlock();
}


Boolean pegasus_module::module_rep::authorized()
{
   return true;
}

Boolean pegasus_module::module_rep::authorized(Uint32 operation)
{
   return true;
}

Boolean pegasus_module::module_rep::authorized(Uint32 index, Uint32 operation)
{
   return true;
}


pegasus_module::pegasus_module(ModuleController *controller, 
			       const String &id, 
 			       void *module_address,
			       Message * (*receive_message)(Message *, void *),
			       void (*async_callback)(Uint32, Message *, void *),
			       void (*shutdown_notify)(Uint32 code, void *))
{
   _rep = new module_rep(controller, 
			 id, 
			 module_address, 
			 receive_message,
			 async_callback,
			 shutdown_notify);
   _allowed_operations = ModuleController::GET_CLIENT_HANDLE |
                         ModuleController::REGISTER_MODULE |
                         ModuleController::DEREGISTER_MODULE | 
                         ModuleController::FIND_SERVICE |
                         ModuleController::FIND_MODULE_IN_SERVICE | 
                         ModuleController::GET_MODULE_REFERENCE | 
                         ModuleController::MODULE_SEND_WAIT | 
                         ModuleController::MODULE_SEND_WAIT_MODULE | 
                         ModuleController::MODULE_SEND_ASYNC | 
                         ModuleController::MODULE_SEND_ASYNC_MODULE | 
                         ModuleController::BLOCKING_THREAD_EXEC | 
                         ModuleController::ASYNC_THREAD_EXEC;
}

pegasus_module::pegasus_module(const pegasus_module & mod)
{
   mod._rep->reference();
   _rep = mod._rep;
}

pegasus_module::~pegasus_module(void)
{
   _rep->dereference();
   _send_shutdown_notify();
   if( 0 == _rep->reference_count())
      delete _rep;
}

Boolean pegasus_module::authorized(Uint32 operation)
{
   return true;
}

Boolean pegasus_module::authorized()
{
   return true;
}


pegasus_module & pegasus_module::operator= (const pegasus_module & mod)
{   
   if( this != &mod)
   {
      if ( _rep->reference_count() == 0 )
	 delete _rep;
      _rep = mod._rep;
   }
   return *this;
}

Boolean pegasus_module::operator== (const pegasus_module & mod) const 
{
   if( mod._rep == _rep )
      return true;
   return false;
   
}
      
Boolean pegasus_module::operator == (const String &  mod) const
{
   if(_rep->get_name() == mod)
      return true; 
   return false;
}


Boolean pegasus_module::operator == (const void *mod) const
{
   if ( (reinterpret_cast<const pegasus_module *>(mod))->_rep == _rep)
      return true;
   return false;
}

const String & pegasus_module::get_name(void) const
{
   return _rep->get_name();
}


Boolean pegasus_module::query_interface(const String & class_id,  
					void **object_ptr) const 
{
   PEGASUS_ASSERT(object_ptr != NULL);
   if( class_id == _rep->get_name())
   {
      *object_ptr = _rep->get_module_address();
      return true;
   }
   *object_ptr = NULL;
   return false;
}

Message * pegasus_module::_receive_message(Message *msg)
{
   return _rep->module_receive_message(msg);
}

void pegasus_module::_send_async_callback(Uint32 msg_handle, Message *msg, void *parm) 
{
   _rep->_send_async_callback(msg_handle, msg, parm);
}

void pegasus_module::_send_shutdown_notify(void)
{
   _rep->_send_shutdown_notify();
}

Boolean pegasus_module::_shutdown(void) 
{
   _send_shutdown_notify(); 
   return true; 
} 



const Uint32 ModuleController::GET_CLIENT_HANDLE =          0x00000001;
const Uint32 ModuleController::REGISTER_MODULE =            0x00000002;
const Uint32 ModuleController::DEREGISTER_MODULE =          0x00000004;
const Uint32 ModuleController::FIND_SERVICE =               0x00000008;
const Uint32 ModuleController::FIND_MODULE_IN_SERVICE =     0x00000010;
const Uint32 ModuleController::GET_MODULE_REFERENCE =       0x00000020;
const Uint32 ModuleController::MODULE_SEND_WAIT =           0x00000040;
const Uint32 ModuleController::MODULE_SEND_WAIT_MODULE =    0x00000040;
const Uint32 ModuleController::MODULE_SEND_ASYNC =          0x00000080;
const Uint32 ModuleController::MODULE_SEND_ASYNC_MODULE =   0x00000080;
const Uint32 ModuleController::BLOCKING_THREAD_EXEC =       0x00000100;
const Uint32 ModuleController::ASYNC_THREAD_EXEC =          0x00000200;
const Uint32 ModuleController::CLIENT_SEND_WAIT =           0x00000400;
const Uint32 ModuleController::CLIENT_SEND_WAIT_MODULE =    0x00000400;
const Uint32 ModuleController::CLIENT_SEND_ASYNC =          0x00000800;
const Uint32 ModuleController::CLIENT_SEND_ASYNC_MODULE =   0x00000800;
const Uint32 ModuleController::CLIENT_BLOCKING_THREAD_EXEC =0x00001000;
const Uint32 ModuleController::CLIENT_ASYNC_THREAD_EXEC =   0x00001000;


Boolean ModuleController::client_handle::authorized()
{
   return true;
}

Boolean ModuleController::client_handle::authorized(Uint32 operation)
{
   return true;
}
 

Boolean ModuleController::client_handle::authorized(Uint32 index, Uint32 operation)
{
   return true;
}
 

// NOTE: "destroy" is defined in <memory> on HP-UX and must not be redefined
static struct timeval createTime = {0, 50000};
static struct timeval destroyTime = {15, 0};
static struct timeval deadlockTime = {5, 0};

ModuleController::ModuleController(const char *name )
   :Base(name, MessageQueue::getNextQueueId(), 
	 module_capabilities::module_controller |
	 module_capabilities::async),
    _modules(true),
   _thread_pool(2, "Module Controller",  1, 10,
                createTime, destroyTime, deadlockTime),
    _internal_module(this, String("INTERNAL"), this, NULL, NULL, NULL)
{ 

}

ModuleController::ModuleController(const char *name ,
				   Sint16 min_threads, 
				   Sint16 max_threads,
				   struct timeval & create_thread,
				   struct timeval & destroy_thread,
				   struct timeval & deadlock)
   :Base(name, MessageQueue::getNextQueueId(),
	 module_capabilities::module_controller |
	 module_capabilities::async),
   _modules(true),
    _thread_pool(min_threads + 1,  
		 name, min_threads, 
		 max_threads, 
		 create_thread, 
		 destroy_thread, 
		 deadlock)   
{ 

}

ModuleController::~ModuleController()
{

   // deregister all modules 
   pegasus_module *module;
   _modules.lock();
    
   module = _modules.next(0);
   while(module != NULL )
   {
      String name = module->get_name();
      _modules.unlock();
      deregister_module(name);
      delete module;
      _modules.lock();
      module = _modules.next(0);
   }
   _modules.unlock();
}

// called by a module to register itself, returns a handle to the controller 
ModuleController & ModuleController::register_module(const String & controller_name,
						     const String & module_name, 
						     void *module_address, 
						     Message * (*receive_message)(Message *, void *),
						     void (*async_callback)(Uint32, Message *, void *),
						     void (*shutdown_notify)(Uint32, void *),
						     pegasus_module **instance) 
   throw(AlreadyExists, IncompatibleTypes)
{

   pegasus_module *module ;
   ModuleController *controller;
   

   Array<Uint32> services;
   char *temp = controller_name.allocateCString(0, true);
   
   MessageQueue *message_queue = MessageQueue::lookup(temp);
   delete [] temp;
   
   if ((message_queue == NULL) || ( false == message_queue->isAsync() ))
   {
      throw IncompatibleTypes();
   }

   MessageQueueService *service = static_cast<MessageQueueService *>(message_queue);
   if( (service == NULL) ||  ! ( service->get_capabilities() & module_capabilities::module_controller ))
   {
      throw IncompatibleTypes();
   }

   controller = static_cast<ModuleController *>(service);
   
   // see if the module already exists in this controller.
   controller->_modules.lock();
   module = controller->_modules.next(0);
   while(module != NULL )
   {
      if(module->get_name() == module_name )
      {
	 controller->_modules.unlock();
	 throw AlreadyExists();
      }
      module = controller->_modules.next(module);
   }
   controller->_modules.unlock();
   
   // now reserve this module name with the meta dispatcher

   Uint32 result = 0 ;
   RegisteredModule *request = 
      new RegisteredModule(controller->get_next_xid(),
			   0, 
			   true, 
			   controller->_meta_dispatcher->getQueueId(),
			   module_name);
   request->dest = controller->_meta_dispatcher->getQueueId();
   AsyncReply * response = controller->SendWait(request);
   if( response != NULL)
      result  = response->result;
   
   delete request; 
   delete response;
   if ( result == async_results::MODULE_ALREADY_REGISTERED)
      throw AlreadyExists();
   
   // the module does not exist, go ahead and create it. 
   module = new pegasus_module(controller, 
			       module_name, 
			       module_address, 
			       receive_message, 
			       async_callback, 
			       shutdown_notify);
   
   controller->_modules.insert_last(module);
   
   if(instance != NULL)
      *instance = module;
   
   return *controller;
}


Boolean ModuleController::deregister_module(const String & module_name)
   throw (Permission)

{
   DeRegisteredModule *request = 
      new DeRegisteredModule(get_next_xid(),
			     0,
			     true,
			     getQueueId(),
			     module_name);
   request->dest = _meta_dispatcher->getQueueId();
   
   AsyncReply * response = SendWait(request);

   delete request;
   delete response;
   
   Boolean ret = false;
   pegasus_module *module;
   _modules.lock();
   module = _modules.next(0);
   while(module != NULL )
   {
      if( module->get_name() == module_name)
      {
	 _modules.remove(module);
	 ret = true;
	 break;
      }
      module = _modules.next(0);
   }
   _modules.unlock();
   return ret;
}

Boolean ModuleController::verify_handle(pegasus_module *handle)
{
   pegasus_module *module;
   Boolean ret = false;
   
   // ATTN change to use authorization and the pegasus_id class
   // << Fri Apr  5 12:43:19 2002 mdd >>
   if( handle->_rep->_module_address == (void *)this)
      return true;
   
   _modules.lock();
   module = _modules.next(0);
   while(module != NULL)
   {
      if ( module == handle)
      {
	 ret = true;
	 break;
      }
      module = _modules.next(module);
   }
   _modules.unlock();
   return ret;
}

// given a name, find a service's queue id
Uint32 ModuleController::find_service(pegasus_module & handle, 
				      const String & name) throw(Permission)
{

   if ( false == verify_handle(&handle) )
      throw Permission(pegasus_thread_self());
   Array<Uint32> services;
   Base::find_services(name, 0, 0, &services);
   return( services[0]);
}


// returns the queue ID of the service hosting the named module, 
// zero otherwise

Uint32 ModuleController::find_module_in_service(pegasus_module & handle, 
						const String & name)
   throw(Permission, IPCException)
{
   if ( false == verify_handle(&handle))
      throw(Permission(pegasus_thread_self()));
   
   Uint32 result = 0 ;
   

   FindModuleInService *request = 
      new FindModuleInService(get_next_xid(),
			      0, 
			      true, 
			      _meta_dispatcher->getQueueId(),
			      name);
   request->dest = _meta_dispatcher->getQueueId();
   FindModuleInServiceResponse * response =
      static_cast<FindModuleInServiceResponse *>(SendWait(request));
   if( response != NULL)
      result = response->_module_service_queue;

   delete request;
   delete response;
   
   return result;
}


pegasus_module * ModuleController::get_module_reference(pegasus_module & my_handle, 
							const String & module_name)
   throw(Permission)
{
   if ( false == verify_handle(&my_handle))
      throw(Permission(pegasus_thread_self()));

   pegasus_module *module, *ref = NULL;
   _modules.lock();
   module = _modules.next(0);
   while(module != NULL)
   {
      if(module->get_name() == module_name)
      {
	 ref = new pegasus_module(*module);
	 break;
      }
      module = _modules.next(module);
   }
   _modules.unlock();
   return ref;
}


AsyncReply *ModuleController::_send_wait(Uint32 destination_q, 
					 AsyncRequest *request)
{
   request->dest = destination_q;
   AsyncReply *reply = Base::SendWait(request);
   return reply;
}


// sendwait to another service
AsyncReply * ModuleController::ModuleSendWait(pegasus_module & handle, 
					      Uint32 destination_q,
					      AsyncRequest *request)
   throw(Permission, IPCException)
{
   if ( false == verify_handle(&handle))
      throw(Permission(pegasus_thread_self()));

   return _send_wait(destination_q, request);
}

AsyncReply *ModuleController::_send_wait(Uint32 destination_q, 
					 String & destination_module, 
					 AsyncRequest *message)
{
   AsyncModuleOperationStart *request = 
      new AsyncModuleOperationStart(get_next_xid(),
				    0, 
				    destination_q,
				    getQueueId(),
				    true, 
				    destination_module,
				    message);
   
   request->dest = destination_q;
   AsyncModuleOperationResult *response = 
      static_cast<AsyncModuleOperationResult *>(SendWait(request));
   
   AsyncReply *ret = 0;
   
   if (response != NULL)
   {
      ret = static_cast<AsyncReply *>(response->get_result());
      //clear the request out of the envelope so it can be deleted by the module
      request->get_action();
   }
   delete request; 
   delete response;
   return ret;
}


// sendwait to another module controlled by another service. 
// throws Deadlock() if destination_q is this->queue_id
AsyncReply * ModuleController::ModuleSendWait(pegasus_module & handle, 
					      Uint32 destination_q, 
					      String & destination_module,
					      AsyncRequest *message)
   throw(Permission, Deadlock, IPCException)
{
   if ( false == verify_handle(&handle))
      throw(Permission(pegasus_thread_self()));
   
   return _send_wait(destination_q, destination_module, message);
}

void ModuleController::_async_handleEnqueue(AsyncOpNode *op, 
					    MessageQueue *q, 
					    void *parm)
{
   ModuleController *myself = static_cast<ModuleController *>(q);
   Message *request = op->get_request();
   Message *response = op->get_response();

   if( ! (request->getMask() & message_mask::ha_async) )
      throw TypeMismatch();
   if( ! (response->getMask() & message_mask::ha_async) )
      throw TypeMismatch();
   (static_cast<AsyncMessage *>(request))->op = NULL;
   (static_cast<AsyncMessage *>(response))->op = NULL;

   myself->return_op(op);

   // get rid of the module wrapper 
   if( request->getType() == async_messages::ASYNC_MODULE_OP_START )
   {
      AsyncModuleOperationStart *rq = static_cast<AsyncModuleOperationStart *>(request);
      request = rq->get_action();
      request->setRouting(rq->getRouting());
      delete rq;
   }
   
   // get rid of the module wrapper 
   if(response->getType() == async_messages::ASYNC_MODULE_OP_RESULT )
   {
      AsyncModuleOperationResult *rp = static_cast<AsyncModuleOperationResult *>(response);
      response = rp->get_result();
      response->setRouting(rp->getRouting());
      delete rp;
   }


   callback_handle *cb = reinterpret_cast<callback_handle *>(parm);
   
   cb->_module->_send_async_callback(request->getRouting(), response, cb->_parm);
   delete cb;
   
   return;
}


// send an async message to a service asynchronously
Boolean ModuleController::ModuleSendAsync(pegasus_module & handle,
					  Uint32 msg_handle, 
					  Uint32 destination_q, 
					  AsyncRequest *message, 
					  void *callback_parm) 
   throw(Permission, IPCException)
{
   
   if ( false == verify_handle(&handle))
      throw(Permission(pegasus_thread_self()));

   if (message->op == NULL)
   {
      message->op = get_op();
      message->op->put_request(message);
   }

   callback_handle *cb = new callback_handle(&handle, callback_parm);
   
   message->setRouting(msg_handle);
   message->resp = getQueueId();
   message->block = false;
   message->dest = destination_q;
   return SendAsync(message->op, 
		    destination_q,
		    _async_handleEnqueue,
		    this,
		    cb);
}

// send a message to a module within another service asynchronously 
Boolean ModuleController::ModuleSendAsync(pegasus_module & handle,
					  Uint32 msg_handle, 
					  Uint32 destination_q, 
					  String & destination_module,
					  AsyncRequest *message, 
					  void *callback_parm) 
   throw(Permission, IPCException)
{
   
   if ( false == verify_handle(&handle))
      throw(Permission(pegasus_thread_self()));
   
   AsyncOpNode *op = get_op();
   AsyncModuleOperationStart *request = 
      new AsyncModuleOperationStart(msg_handle,
				    op,
				    destination_q,
				    getQueueId(),
				    true, 
				    destination_module,
				    message);
   request->dest = destination_q;
   callback_handle *cb = new callback_handle(&handle, callback_parm); 
   return SendAsync(op, 
		    destination_q,
		    _async_handleEnqueue,
		    this,
		    &cb);
}

void ModuleController::_blocking_thread_exec(
   PEGASUS_THREAD_RETURN (PEGASUS_THREAD_CDECL *thread_func)(void *), 
   void *parm) 
{
   Semaphore *blocking_sem = new Semaphore(0);
   _thread_pool.allocate_and_awaken(parm, thread_func, blocking_sem);
   blocking_sem->wait();
   delete blocking_sem;
}


void ModuleController::blocking_thread_exec(
   pegasus_module & handle, 
   PEGASUS_THREAD_RETURN (PEGASUS_THREAD_CDECL *thread_func)(void *), 
   void *parm) throw(Permission, Deadlock, IPCException)
{
   if ( false == verify_handle(&handle))
      throw(Permission(pegasus_thread_self()));
   _blocking_thread_exec(thread_func, parm);
   
}


void ModuleController::_async_thread_exec(
   PEGASUS_THREAD_RETURN (PEGASUS_THREAD_CDECL *thread_func)(void *), 
   void *parm) 
{
   _thread_pool.allocate_and_awaken(parm, thread_func);
}



void ModuleController::async_thread_exec(pegasus_module & handle, 
					    PEGASUS_THREAD_RETURN (PEGASUS_THREAD_CDECL *thread_func)(void *), 
					    void *parm) throw(Permission, Deadlock, IPCException)
{
   if ( false == verify_handle(&handle))
      throw(Permission(pegasus_thread_self()));
   _async_thread_exec(thread_func, parm);
}

void ModuleController::_handle_async_request(AsyncRequest *rq)
{

   if( rq->getType() == async_messages::ASYNC_MODULE_OP_START)
   {
      // find the target module
      pegasus_module *target;
      Message *module_result = NULL;
      
      _modules.lock();
      target = _modules.next(0);
      while(target != NULL)
      {
	 if(target->get_name() == static_cast<AsyncModuleOperationStart *>(rq)->_target_module)
	 {
	    module_result = target->_receive_message(static_cast<AsyncModuleOperationStart *>(rq)->_act);
	    break;
	 }
	 
	 target = _modules.next(target);
      }
      _modules.unlock();
      
      if(module_result == NULL)
      {
	 module_result = new AsyncReply(async_messages::REPLY, 
					static_cast<AsyncModuleOperationStart *>(rq)->_act->getKey(),
					static_cast<AsyncModuleOperationStart *>(rq)->_act->getRouting(),
					message_mask::ha_async | message_mask::ha_reply,
					rq->op,
					async_results::CIM_NAK,
					rq->resp,
					false);
      }
      
      AsyncModuleOperationResult *result = 
	 new AsyncModuleOperationResult(rq->getKey(),
					rq->getRouting(),
					rq->op,
					async_results::OK,
					static_cast<AsyncModuleOperationStart *>(rq)->resp,
					false, 
					static_cast<AsyncModuleOperationStart *>(rq)->_target_module,
					module_result);
      _complete_op_node(rq->op, 0, 0, 0);
   }
   else
      Base::_handle_async_request(rq);
}

void ModuleController::_handle_async_callback(AsyncOpNode *op)
{
   Base::_handle_async_callback(op);
   
}


// called by a non-module client to get the interface and authorization to use the controller

ModuleController & ModuleController::get_client_handle(const pegasus_identity & id, 
						       client_handle **handle) 
   throw(IncompatibleTypes)
{
   
   if(handle == NULL)
      throw NullPointer();
      
   Array<Uint32> services;
   
   MessageQueue *message_queue = MessageQueue::lookup(PEGASUS_SERVICENAME_CONTROLSERVICE);
   
   if ((message_queue == NULL) || ( false == message_queue->isAsync() ))
   {
      throw IncompatibleTypes();
   }

   MessageQueueService *service = static_cast<MessageQueueService *>(message_queue);
   if( (service == NULL) ||  ! ( service->get_capabilities() & module_capabilities::module_controller ))
   {
      throw IncompatibleTypes();
   }

   ModuleController *controller = static_cast<ModuleController *>(service);
   if(true == const_cast<pegasus_identity &>(id).authenticate())
      *handle = new client_handle(id);
   else
      *handle = NULL;
   
   return *controller;
}


void ModuleController::return_client_handle(client_handle *handle)
{
   delete handle;
}


// send a message to another service
AsyncReply *ModuleController::ClientSendWait(const client_handle & handle,
					     Uint32 destination_q, 
					     AsyncRequest *request) 
   throw(Permission, IPCException)
{
   if( false == const_cast<client_handle &>(handle).authorized(CLIENT_SEND_WAIT)) 
      throw Permission(pegasus_thread_self());
   return _send_wait(destination_q, request);
}


// send a message to another module via another service
AsyncReply *ModuleController::ClientSendWait(const client_handle & handle,
					     Uint32 destination_q,
					     String & destination_module,
					     AsyncRequest *request) 
   throw(Permission, Deadlock, IPCException)
{
   if( false == const_cast<client_handle &>(handle).authorized(CLIENT_SEND_WAIT_MODULE)) 
      throw Permission(pegasus_thread_self());
   return _send_wait(destination_q, destination_module, request);
}


// send an async message to another service
Boolean ModuleController::ClientSendAsync(const client_handle & handle,
					  Uint32 msg_handle, 
					  Uint32 destination_q, 
					  AsyncRequest *message,
					  void (*async_callback)(Uint32, Message *, void *) ,
					  void *callback_parm)
   throw(Permission, IPCException)
{
   if( false == const_cast<client_handle &>(handle).authorized(CLIENT_SEND_ASYNC)) 
      throw Permission(pegasus_thread_self());
   pegasus_module *temp = new pegasus_module(this, 
					     String(PEGASUS_CONTROL_TEMP_MODULE),
					     this, 
					     0, 
					     async_callback, 
					     0);
   return ModuleSendAsync( *temp, 
			   msg_handle, 
			   destination_q, 
			   message, 
			   callback_parm);
}


// send an async message to another module via another service
Boolean ModuleController::ClientSendAsync(const client_handle & handle,
					  Uint32 msg_handle, 
					  Uint32 destination_q, 
					  String & destination_module,
					  AsyncRequest *message, 
					  void (*async_callback)(Uint32, Message *, void *),
					  void *callback_parm)
   throw(Permission, IPCException)
{
   if( false == const_cast<client_handle &>(handle).authorized(CLIENT_SEND_ASYNC_MODULE)) 
      throw Permission(pegasus_thread_self());
   
   pegasus_module *temp = new pegasus_module(this, 
					     String(PEGASUS_CONTROL_TEMP_MODULE),
					     this, 
					     0, 
					     async_callback, 
					     0);
   return ModuleSendAsync(*temp, 
			  msg_handle, 
			  destination_q, 
			  destination_module,
			  message,
			  callback_parm);
}


void ModuleController::client_blocking_thread_exec(
   const client_handle & handle,
   PEGASUS_THREAD_RETURN (PEGASUS_THREAD_CDECL *thread_func)(void *), 
   void *parm) throw(Permission, Deadlock, IPCException)
{
   if( false == const_cast<client_handle &>(handle).authorized(CLIENT_BLOCKING_THREAD_EXEC)) 
      throw Permission(pegasus_thread_self());
   _blocking_thread_exec(thread_func, parm);
   
}

void ModuleController::client_async_thread_exec(
   const client_handle & handle, 
   PEGASUS_THREAD_RETURN (PEGASUS_THREAD_CDECL *thread_func)(void *), 
   void *parm)    throw(Permission, Deadlock, IPCException)
{

   if( false == const_cast<client_handle &>(handle).authorized(CLIENT_ASYNC_THREAD_EXEC)) 
      throw Permission(pegasus_thread_self());
   _async_thread_exec(thread_func, parm);
}


PEGASUS_NAMESPACE_END
