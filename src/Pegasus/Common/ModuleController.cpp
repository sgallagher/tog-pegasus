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
 
pegasus_module::pegasus_module(ModuleController *controller, 
			       const String &id, 
 			       void *module_address,
			       Message * (*receive_message)(Message *),
			       void (*async_callback)(Uint32, Message *),
			       void (*shutdown_notify)(Uint32 code))
{
   _rep = new module_rep(controller, 
			 id, 
			 module_address, 
			 receive_message,
			 async_callback,
			 shutdown_notify);
}

pegasus_module::pegasus_module(const pegasus_module & mod)
{
   mod._rep->reference();
   _rep = mod._rep;
}

pegasus_module & pegasus_module::operator= (const pegasus_module & mod)
{   
   (mod._rep->reference());
   if ( _rep->reference_count() == 0 )
      delete _rep;
   _rep = mod._rep;
   return *this;
}

Boolean pegasus_module::operator== (const pegasus_module *mod) const
{
   if(mod->_rep == _rep)
      return true;
   return false;
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

static struct timeval create = {0, 50000};
static struct timeval destroy = {15, 0};
static struct timeval deadlock = {5, 0};

ModuleController::ModuleController(const char *name )
   :Base(name, MessageQueue::getNextQueueId(), 
	 module_capabilities::module_controller |
	 module_capabilities::async),
    _modules(true),
   _thread_pool(2, "Module Controller",  1, 10, create, destroy, deadlock)   
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


// called by a module to register itself, returns a handle to the controller 
ModuleController & ModuleController::register_module(const String & module_name, 
						     void *module_address, 
						     Message * (*receive_message)(Message *),
						     void (*async_callback)(Uint32, Message *),
						     void (*shutdown_notify)(Uint32)) throw(AlreadyExists)
{

   pegasus_module *module ;
   
   // see if the module already exists in this controller.
   _modules.lock();
   module = _modules.next(0);
   while(module != NULL )
   {
      if(module->get_name() == module_name )
      {
	 _modules.unlock();
	 throw AlreadyExists();
      }
      module = _modules.next(module);
   }
   _modules.unlock();
   
   // now reserve this module name with the meta dispatcher

   Uint32 result = 0 ;
   RegisteredModule *request = 
      new RegisteredModule(get_next_xid(),
			   0, 
			   true, 
			   _meta_dispatcher->getQueueId(),
			   module_name);
   request->dest = _meta_dispatcher->getQueueId();
   AsyncReply * response = SendWait(request);
   if( response != NULL)
      result  = response->result;
   
   delete request; 
   delete response;
   if ( result == async_results::MODULE_ALREADY_REGISTERED)
      throw AlreadyExists();
   
   // the module does not exist, go ahead and create it. 
   module = new pegasus_module(this, 
			       module_name, 
			       module_address, 
			       receive_message, 
			       async_callback, 
			       shutdown_notify);
   
   _modules.insert_last(module);
   
   // now tell the cimom about this module

   return *this;
}


Boolean ModuleController::deregister_module(const String & module_name)
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


// sendwait to another service
AsyncReply * ModuleController::ModuleSendWait(pegasus_module & handle, 
					      Uint32 destination_q,
					      AsyncRequest *request)
   throw(Permission, IPCException)
{
   if ( false == verify_handle(&handle))
      throw(Permission(pegasus_thread_self()));

   request->dest = destination_q;
   AsyncReply *reply = Base::SendWait(request);
   return reply;
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

void ModuleController::_async_handleEnqueue(AsyncOpNode *op, 
					    MessageQueue *q, 
					    void *parm)
{
   ModuleController *myself = static_cast<ModuleController *>(q);
   Message *request = op->get_request();
   Message *response = op->get_response();
   pegasus_module *mod = reinterpret_cast<pegasus_module *>(parm);
   delete op;
   mod->_send_async_callback(request->getRouting(), response);
   return;
}


// send an async message to a service asynchronously
Boolean ModuleController::ModuleSendAsync(pegasus_module & handle,
					  Uint32 msg_handle, 
					  Uint32 destination_q, 
					  AsyncMessage *message) 
   throw(Permission, IPCException)
{
   
   if ( false == verify_handle(&handle))
      throw(Permission(pegasus_thread_self()));
   
   AsyncOpNode *op = get_op();
   AsyncOperationStart *request = 
      new AsyncOperationStart(get_next_xid(),
			      op,
			      destination_q,
			      getQueueId(),
			      true, 
			      message);
   request->dest = destination_q;
   return SendAsync(op, 
		    destination_q,
		    _async_handleEnqueue,
		    this,
		    &handle);
}

// send a message to a module within another service asynchronously 
Boolean ModuleController::ModuleSendAsync(pegasus_module & handle,
					  Uint32 msg_handle, 
					  Uint32 destination_q, 
					  String & destination_module,
					  AsyncMessage *message) 
   throw(Permission, IPCException)
{
   
   if ( false == verify_handle(&handle))
      throw(Permission(pegasus_thread_self()));
   
   AsyncOpNode *op = get_op();
   AsyncModuleOperationStart *request = 
      new AsyncModuleOperationStart(get_next_xid(),
				    op,
				    destination_q,
				    getQueueId(),
				    true, 
				    destination_module,
				    message);
   request->dest = destination_q;
   return SendAsync(op, 
		    destination_q,
		    _async_handleEnqueue,
		    this,
		    &handle);
}

void ModuleController::blocking_thread_exec(pegasus_module & handle, 
					    PEGASUS_THREAD_RETURN (PEGASUS_THREAD_CDECL *thread_func)(void *), 
					    void *parm) throw(Permission, Deadlock, IPCException)
{
   if ( false == verify_handle(&handle))
      throw(Permission(pegasus_thread_self()));
   Semaphore *blocking_sem = new Semaphore(0);
   _thread_pool.allocate_and_awaken(parm, thread_func, blocking_sem);
   blocking_sem->wait();
   return;
}

void ModuleController::async_thread_exec(pegasus_module & handle, 
					    PEGASUS_THREAD_RETURN (PEGASUS_THREAD_CDECL *thread_func)(void *), 
					    void *parm) throw(Permission, Deadlock, IPCException)
{
   if ( false == verify_handle(&handle))
      throw(Permission(pegasus_thread_self()));
   _thread_pool.allocate_and_awaken(parm, thread_func);
   return;
}



PEGASUS_NAMESPACE_END
