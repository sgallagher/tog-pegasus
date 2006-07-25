//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
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
// Modified By: Amit K Arora, IBM (amita@in.ibm.com) for PEP101 and Bug#1090
//              David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include "ModuleController.h"
#include <Pegasus/Common/MessageLoader.h> //l10n
#include <Pegasus/Common/Tracer.h>
PEGASUS_NAMESPACE_BEGIN

PEGASUS_USING_STD;

pegasus_module::module_rep::module_rep(ModuleController *controller,
				       const String & name,
				       void *module_address,
				       Message * (*receive_message)(Message *, void *),
				       void (*async_callback)(Uint32, Message *, void *),
				       void (*shutdown_notify)(Uint32 code, void *))
   : _thread_safety(),
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


pegasus_module::module_rep::~module_rep()
{
   _send_shutdown_notify();
}

Message * pegasus_module::module_rep::module_receive_message(Message *msg)
{
   Message * ret;
   // ATTN: This Mutex serializes requests to this Module (Control Provider)
   //AutoMutex autoMut(_thread_safety);
   ret = _receive_message(msg, _module_address);
   return ret;
}

void pegasus_module::module_rep::_send_async_callback(Uint32 msg_handle, Message *msg, void *parm)
{
   AutoMutex autoMut(_thread_safety);
   _async_callback(msg_handle, msg, parm);
}

void pegasus_module::module_rep::_send_shutdown_notify()
{
   AutoMutex autoMut(_thread_safety);
   if(_reference_count.get() == 0 )
   {
      if( _shutting_down.get() == 0 )
      {
	 _shutting_down++;
	 _shutdown_notify(_reference_count.get(), _module_address);
	 _async_callback = closed_async_callback;
	 _receive_message = closed_receive_message;
      }
   }
}


pegasus_module::pegasus_module(ModuleController *controller,
			       const String &id,
 			       void *module_address,
			       Message * (*receive_message)(Message *, void *),
			       void (*async_callback)(Uint32, Message *, void *),
                   void (*shutdown_notify)(Uint32 code, void *)) :
             _rep(new module_rep(controller, id, module_address,
                          receive_message, async_callback, shutdown_notify))
{
}

pegasus_module::pegasus_module(const pegasus_module & mod) : Linkable()
{
   mod._rep->reference();
   _rep.reset(mod._rep.get());
}

pegasus_module::~pegasus_module()
{
   _rep->dereference();
   _send_shutdown_notify();
   if( 0 == _rep->reference_count())
        _rep.reset();
}

pegasus_module & pegasus_module::operator= (const pegasus_module & mod)
{
   if( this != &mod)
   {
      if ( _rep->reference_count() == 0 )
	              _rep.reset();
      _rep.reset(mod._rep.get());
   }
   return *this;
}

Boolean pegasus_module::operator == (const String &  mod) const
{
   if(_rep->get_name() == mod)
      return true;
   return false;
}

const String & pegasus_module::get_name() const
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

void pegasus_module::_send_shutdown_notify()
{
   _rep->_send_shutdown_notify();
}

Boolean pegasus_module::_shutdown()
{
   _send_shutdown_notify();
   return true;
}


// NOTE: "destroy" is defined in <memory> on HP-UX and must not be redefined
static struct timeval createTime = {0, 50000};
static struct timeval destroyTime = {15, 0};

ModuleController::ModuleController(const char *name )
   :Base(name, MessageQueue::getNextQueueId(),
	 module_capabilities::module_controller |
	 module_capabilities::async),
    _modules()
{
}

// ModuleController::ModuleController(const char *name ,
// 				   Sint16 min_threads,
// 				   Sint16 max_threads,
// 				   struct timeval & create_thread,
// 				   struct timeval & destroy_thread)
//    :Base(name, MessageQueue::getNextQueueId(),
// 	 module_capabilities::module_controller |
// 	 module_capabilities::async),
//    _modules(true),
//     _thread_pool(min_threads + 1,
// 		 name, min_threads,
// 		 max_threads,
// 		 create_thread,
// 		 destroy_thread)
// {
// }

ModuleController::~ModuleController()
{

   pegasus_module *module;

   try
   {
      module = _modules.remove_front();
      while(module)
      {
	 delete module;
	 module = _modules.remove_front();
      }

   }
   catch(...)
   {
   }
}

// called by a module to register itself, returns a handle to the controller
ModuleController & ModuleController::register_module(const String & controller_name,
						     const String & module_name,
						     void *module_address,
						     Message * (*receive_message)(Message *, void *),
						     void (*async_callback)(Uint32, Message *, void *),
						     void (*shutdown_notify)(Uint32, void *),
						     pegasus_module **instance)
{

   pegasus_module *module ;
   ModuleController *controller;


   Array<Uint32> services;

   MessageQueue *message_queue = MessageQueue::lookup(controller_name.getCString());

   if ((message_queue == NULL) || ( false == message_queue->isAsync() ))
   {
      throw IncompatibleTypesException();
   }

   MessageQueueService *service = static_cast<MessageQueueService *>(message_queue);
   if( (service == NULL) ||  ! ( service->get_capabilities() & module_capabilities::module_controller ))
   {
      throw IncompatibleTypesException();
   }

   controller = static_cast<ModuleController *>(service);

   {

   // see if the module already exists in this controller.
   _module_lock lock(&(controller->_modules));

   module = controller->_modules.front();
   while(module != NULL )
   {
      if(module->get_name() == module_name )
      {
	 //l10n
	 //throw AlreadyExistsException("module \"" + module_name + "\"");
	 MessageLoaderParms parms("Common.ModuleController.MODULE",
	 						  "module \"$0\"",
	 						  module_name);
	 throw AlreadyExistsException(parms);
      }
      module = controller->_modules.next_of(module);
   }

   }

   // now reserve this module name with the meta dispatcher

   Uint32 result = 0 ;
   AutoPtr<RegisteredModule> request(new RegisteredModule(
			   0,
			   true,
			   controller->getQueueId(),
			   module_name));

   request->dest = CIMOM_Q_ID;

   AutoPtr<AsyncReply> response(controller->SendWait(request.get()));
   if( response.get() != NULL)
      result  = response->result;

   request.reset();
   response.reset();
   if ( result == async_results::MODULE_ALREADY_REGISTERED){
   	//l10n
      //throw AlreadyExistsException("module \"" + module_name + "\"");
      MessageLoaderParms parms("Common.ModuleController.MODULE",
	 						  "module \"$0\"",
	 						  module_name);
	 throw AlreadyExistsException(parms);

   }

   // the module does not exist, go ahead and create it.
   module = new pegasus_module(controller,
			       module_name,
			       module_address,
			       receive_message,
			       async_callback,
			       shutdown_notify);

   controller->_modules.insert_back(module);

   if(instance != NULL)
      *instance = module;

   return *controller;
}


Boolean ModuleController::deregister_module(const String & module_name)
{
   AutoPtr<DeRegisteredModule> request(new DeRegisteredModule(
			     0,
			     true,
			     getQueueId(),
			     module_name));
   request->dest = _meta_dispatcher->getQueueId();

   AutoPtr<AsyncReply> response(SendWait(request.get()));

   request.reset();
   response.reset();

   pegasus_module *module;

   _module_lock lock(&_modules);
   module = _modules.front();
   while(module != NULL )
   {
      if( module->get_name() == module_name)
      {
	 _modules.remove(module);
	 return true;
      }
      module = _modules.next_of(module);
   }
   return false;
}

Boolean ModuleController::verify_handle(pegasus_module *handle)
{
   pegasus_module *module;

   // ATTN change to use authorization and the pegasus_id class
   // << Fri Apr  5 12:43:19 2002 mdd >>
   if( handle->_rep->_module_address == (void *)this)
      return true;

   _module_lock lock(&_modules);

   module = _modules.front();
   while(module != NULL)
   {
      if ( module == handle)
      {
	 return true;
      }
      module = _modules.next_of(module);
   }
   return false;
}

// given a name, find a service's queue id
Uint32 ModuleController::find_service(const pegasus_module & handle,
				      const String & name)
{

   if ( false == verify_handle(const_cast<pegasus_module *>(&handle)) )
      throw Permission(pegasus_thread_self());
   Array<Uint32> services;
   Base::find_services(name, 0, 0, &services);
   return( services[0]);
}


// returns the queue ID of the service hosting the named module,
// zero otherwise

Uint32 ModuleController::find_module_in_service(const pegasus_module & handle,
						const String & name)
{
   if ( false == verify_handle(const_cast<pegasus_module *>(&handle)))
      throw(Permission(pegasus_thread_self()));

   Uint32 result = 0 ;

   AutoPtr<FindModuleInService> request(new FindModuleInService(
			      0,
			      true,
			      _meta_dispatcher->getQueueId(),
			      name));
   request->dest = _meta_dispatcher->getQueueId();
   AutoPtr<FindModuleInServiceResponse>
       response(static_cast<FindModuleInServiceResponse *>(SendWait(request.get())));
   if( response.get() != NULL)
      result = response->_module_service_queue;

   return result;
}


pegasus_module * ModuleController::get_module_reference(const pegasus_module & my_handle,
							const String & module_name)
{
   if ( false == verify_handle(const_cast<pegasus_module *>(&my_handle)))
      throw(Permission(pegasus_thread_self()));

   pegasus_module *module, *ref = NULL;
   _module_lock lock(&_modules);
   module = _modules.front();
   while(module != NULL)
   {
      if(module->get_name() == module_name)
      {
	 ref = new pegasus_module(*module);
	 break;
      }
      module = _modules.next_of(module);
   }
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
AsyncReply * ModuleController::ModuleSendWait(const pegasus_module & handle,
					      Uint32 destination_q,
					      AsyncRequest *request)
{
   if ( false == verify_handle(const_cast<pegasus_module *>(&handle)))
      throw(Permission(pegasus_thread_self()));

   return _send_wait(destination_q, request);
}

AsyncReply *ModuleController::_send_wait(Uint32 destination_q,
					 const String & destination_module,
					 AsyncRequest *message)
{
   AutoPtr<AsyncModuleOperationStart> request(new AsyncModuleOperationStart(
				    0,
				    destination_q,
				    getQueueId(),
				    true,
				    destination_module,
				    message));

   request->dest = destination_q;
   AutoPtr<AsyncModuleOperationResult>
      response(static_cast<AsyncModuleOperationResult *>(SendWait(request.get())));

   AsyncReply *ret = 0;

   if (response.get() != NULL && response->getType() == async_messages::ASYNC_MODULE_OP_RESULT )
   {
      ret = static_cast<AsyncReply *>(response->get_result());
      //clear the request out of the envelope so it can be deleted by the module
   }
   request->get_action();
   return ret;
}


// sendwait to another module controlled by another service.
// throws Deadlock() if destination_q is this->queue_id
AsyncReply * ModuleController::ModuleSendWait(const pegasus_module & handle,
					      Uint32 destination_q,
					      const String & destination_module,
					      AsyncRequest *message)
{
   if ( false == verify_handle(const_cast<pegasus_module *>(&handle)))
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

   if( request && (! (request->getMask() & message_mask::ha_async)))
      throw TypeMismatchException();

   if( response && (! (response->getMask() & message_mask::ha_async) ))
      throw TypeMismatchException();

   op->release();
   myself->return_op(op);

   // get rid of the module wrapper
   if( request && request->getType() == async_messages::ASYNC_MODULE_OP_START )
   {
      (static_cast<AsyncMessage *>(request))->op = NULL;
      AsyncModuleOperationStart *rq = static_cast<AsyncModuleOperationStart *>(request);
      request = rq->get_action();
      delete rq;
   }

   // get rid of the module wrapper
   if(response && response->getType() == async_messages::ASYNC_MODULE_OP_RESULT )
   {
      (static_cast<AsyncMessage *>(response))->op = NULL;
      AsyncModuleOperationResult *rp = static_cast<AsyncModuleOperationResult *>(response);
      response = rp->get_result();
      delete rp;
   }

   callback_handle *cb = reinterpret_cast<callback_handle *>(parm);

   cb->_module->_send_async_callback(0, response, cb->_parm);
   delete cb;

   return;
}


// send an async message to a service asynchronously
Boolean ModuleController::ModuleSendAsync(const pegasus_module & handle,
					  Uint32 msg_handle,
					  Uint32 destination_q,
					  AsyncRequest *message,
					  void *callback_parm)
{
   //printf("verifying handle %p, controller at %p \n", &handle, this);

   if ( false == verify_handle(const_cast<pegasus_module *>(&handle)))
      throw(Permission(pegasus_thread_self()));

   if (message->op == NULL)
   {
      message->op = get_op();
      message->op->put_request(message);
   }


   callback_handle *cb = new callback_handle(const_cast<pegasus_module *>(&handle),
					     callback_parm);

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
Boolean ModuleController::ModuleSendAsync(const pegasus_module & handle,
					  Uint32 msg_handle,
					  Uint32 destination_q,
					  const String & destination_module,
					  AsyncRequest *message,
					  void *callback_parm)
{

   if ( false == verify_handle(const_cast<pegasus_module *>(&handle)))
      throw(Permission(pegasus_thread_self()));

   AsyncOpNode *op = get_op();
   AsyncModuleOperationStart *request =
      new AsyncModuleOperationStart(
				    op,
				    destination_q,
				    getQueueId(),
				    true,
				    destination_module,
				    message);
   request->dest = destination_q;
   callback_handle *cb = new callback_handle(const_cast<pegasus_module *>(&handle), callback_parm);
   return SendAsync(op,
		    destination_q,
		    _async_handleEnqueue,
		    this,
		    cb);
}


Boolean ModuleController::_send_forget(Uint32 destination_q, AsyncRequest *message)
{
   message->dest = destination_q;
   return SendForget(message);
}

Boolean ModuleController::_send_forget(Uint32 destination_q,
				       const String & destination_module,
				       AsyncRequest *message)
{
   AsyncOpNode *op = get_op();
   message->dest = destination_q;
   AsyncModuleOperationStart *request =
      new AsyncModuleOperationStart(
				    op,
				    destination_q,
				    getQueueId(),
				    true,
				    destination_module,
				    message);
   return SendForget(request);
}



Boolean ModuleController::ModuleSendForget(const pegasus_module & handle,
					   Uint32 destination_q,
					   AsyncRequest *message)
{
   if(false == verify_handle(const_cast<pegasus_module *>( &handle)))
      throw(Permission(pegasus_thread_self()));

   return _send_forget(destination_q, message);
}

Boolean ModuleController::ModuleSendForget(const pegasus_module & handle,
					   Uint32 destination_q,
					   const String & destination_module,
					   AsyncRequest *message)
{
   if(false == verify_handle(const_cast<pegasus_module *>( &handle)))
      throw(Permission(pegasus_thread_self()));
   return _send_forget(destination_q,
		       destination_module,
		       message);
}



void ModuleController::_blocking_thread_exec(
   PEGASUS_THREAD_RETURN (PEGASUS_THREAD_CDECL *thread_func)(void *),
   void *parm)
{
   AutoPtr<Semaphore> blocking_sem(new Semaphore(0));
   ThreadStatus rc = PEGASUS_THREAD_OK;
   while ((rc=_thread_pool->allocate_and_awaken(parm, thread_func, blocking_sem.get())) != PEGASUS_THREAD_OK)
   {
      if (rc == PEGASUS_THREAD_INSUFFICIENT_RESOURCES)
      	pegasus_yield();
      else
      {
 	Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            	"Not enough threads for the client's blocking thread function.");
	// ATTN: There is no category for the 'ModuleController' trace.
 	Tracer::trace(TRC_DISCARDED_DATA, Tracer::LEVEL2,
                "Could not allocate for %s a client's blocking thread.",
                getQueueName());
	break;
      }
   }
   blocking_sem->wait();
}


void ModuleController::blocking_thread_exec(
   const pegasus_module & handle,
   PEGASUS_THREAD_RETURN (PEGASUS_THREAD_CDECL *thread_func)(void *),
   void *parm)
{
   if ( false == verify_handle(const_cast<pegasus_module *>(&handle)))
      throw(Permission(pegasus_thread_self()));
   _blocking_thread_exec(thread_func, parm);

}


void ModuleController::_async_thread_exec(
   PEGASUS_THREAD_RETURN (PEGASUS_THREAD_CDECL *thread_func)(void *),
   void *parm)
{
   ThreadStatus rc = PEGASUS_THREAD_OK;
   while ((rc=_thread_pool->allocate_and_awaken(parm, thread_func)) != PEGASUS_THREAD_OK)
   {
      if (rc == PEGASUS_THREAD_INSUFFICIENT_RESOURCES)
      	pegasus_yield();
      else
      {
 	Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            	"Not enough threads for the client's asynchronous thread function.");
	// ATTN:There is no category for 'ModuleController' traceing.
 	Tracer::trace(TRC_DISCARDED_DATA, Tracer::LEVEL2,
                "Could not allocate for %s a client's asynchronous thread.",
                getQueueName());
	break;
      }
   }
}



void ModuleController::async_thread_exec(const pegasus_module & handle,
					 PEGASUS_THREAD_RETURN (PEGASUS_THREAD_CDECL *thread_func)(void *),
					 void *parm)
{
   if ( false == verify_handle(const_cast<pegasus_module *>(&handle)))
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

      {
	 _module_lock lock(&_modules);
	 target = _modules.front();
	 while(target != NULL)
	 {
	    if(target->get_name() == static_cast<AsyncModuleOperationStart *>(rq)->_target_module)
	    {
	       break;
	    }

	    target = _modules.next_of(target);
	 }
      }

      if (target)
      {
          // ATTN: This statement was taken out of the _module_lock block
          // above because that caused all requests to control providers to
          // be serialized.  There is now a risk that the control provider
          // module may be deleted after the lookup and before this call.
          // See Bugzilla 3120.
          module_result = target->_receive_message(
              static_cast<AsyncModuleOperationStart *>(rq)->_act);
      }

      if(module_result == NULL)
      {
	 module_result = new AsyncReply(async_messages::REPLY,
					message_mask::ha_async | message_mask::ha_reply,
					rq->op,
					async_results::CIM_NAK,
					rq->resp,
					false);
      }

      AsyncModuleOperationResult *result =
	 new AsyncModuleOperationResult(
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

ModuleController* ModuleController::getModuleController()
{
   MessageQueue *messageQueue =
      MessageQueue::lookup(PEGASUS_QUEUENAME_CONTROLSERVICE);
   PEGASUS_ASSERT(messageQueue != 0);
   PEGASUS_ASSERT(messageQueue->isAsync());

   MessageQueueService* service =
      dynamic_cast<MessageQueueService*>(messageQueue);
   PEGASUS_ASSERT(service != 0);
   PEGASUS_ASSERT(
      service->get_capabilities() & module_capabilities::module_controller);

   return static_cast<ModuleController*>(service);
}


// send a message to another service
AsyncReply *ModuleController::ClientSendWait(
					     Uint32 destination_q,
					     AsyncRequest *request)
{
   return _send_wait(destination_q, request);
}


// send a message to another module via another service
AsyncReply *ModuleController::ClientSendWait(
					     Uint32 destination_q,
					     String & destination_module,
					     AsyncRequest *request)
{
   return _send_wait(destination_q, destination_module, request);
}


// send an async message to another service
Boolean ModuleController::ClientSendAsync(
					  Uint32 msg_handle,
					  Uint32 destination_q,
					  AsyncRequest *message,
					  void (*async_callback)(Uint32, Message *, void *) ,
					  void *callback_parm)
{
   pegasus_module *temp = new pegasus_module(this,
					     String(PEGASUS_MODULENAME_TEMP),
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
Boolean ModuleController::ClientSendAsync(
					  Uint32 msg_handle,
					  Uint32 destination_q,
					  const String & destination_module,
					  AsyncRequest *message,
					  void (*async_callback)(Uint32, Message *, void *),
					  void *callback_parm)
{
   pegasus_module *temp = new pegasus_module(this,
					     String(PEGASUS_MODULENAME_TEMP),
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



Boolean ModuleController::ClientSendForget(
					   Uint32 destination_q,
					   AsyncRequest *message)
{
   return _send_forget(destination_q, message);
}


Boolean ModuleController::ClientSendForget(
					   Uint32 destination_q,
					   const String & destination_module,
					   AsyncRequest *message)
{
   return _send_forget(destination_q, destination_module, message);
}

void ModuleController::client_blocking_thread_exec(
   PEGASUS_THREAD_RETURN (PEGASUS_THREAD_CDECL *thread_func)(void *),
   void *parm)
{
   _blocking_thread_exec(thread_func, parm);
}

void ModuleController::client_async_thread_exec(
   PEGASUS_THREAD_RETURN (PEGASUS_THREAD_CDECL *thread_func)(void *),
   void *parm)
{
   _async_thread_exec(thread_func, parm);
}


PEGASUS_NAMESPACE_END
