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
// Author: Mike Day (mdday@us.ibm.com) <<< Wed Mar 13 20:49:40 2002 mdd >>>
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Module_Controller_h
#define Pegasus_Module_Controller_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Message.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/IPC.h>
#include <Pegasus/Common/Thread.h>
#include <Pegasus/Common/AsyncOpNode.h>
#include <Pegasus/Common/Cimom.h>
#include <Pegasus/Common/CimomMessage.h>
#include <Pegasus/Common/MessageQueueService.h>
PEGASUS_NAMESPACE_BEGIN

class ModuleController;



class PEGASUS_COMMON_LINKAGE pegasus_module
{
      class PEGASUS_COMMON_LINKAGE module_rep 
      {
	 public:
	    module_rep(ModuleController *controller, 
		       const String & name,
		       void *module_address, 
		       Boolean (*receive_message)(Message *),
		       void (*async_callback)(Uint32, Message *),
		       void (*shutdown_notify)(Uint32 code))
	       : _thread_safety(),
		 _controller(controller), 
		 _name(name), 
		 _reference_count(1), 
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
      
	    virtual ~module_rep(void) 
	    {

	    }
      

	    Boolean operator == (const module_rep *rep) const
	    {
	       if (rep == this )
		  return true;
	       return false;
	    }
      
	    Boolean operator == (const module_rep &rep) const
	    {
	       if (rep == *this)
		  return true;
	       return false;
	    }
      
	    Boolean operator == (void *rep) const 
	    {
	       if ( (void *)this == rep )
		  return true;
	       return false;
	    }
      
	    void reference(void) { _reference_count++; } 
	    void dereference(void) { _reference_count--; } 
	    Uint32 reference_count(void)  { return _reference_count.value(); }  
	    const String & get_name(void) const { return _name; }
	    void *get_module_address(void) const { return _module_address; }
	    Boolean module_receive_message(Message *msg)
	    {

	       _thread_safety.lock(pegasus_thread_self());
	       try {  _receive_message(msg); }
	       catch(...) { _thread_safety.unlock(); throw; }
	       _thread_safety.unlock();
	    }
	    
	    void _send_async_callback(Uint32 msg_handle, Message *msg)
	    {
	       _thread_safety.lock(pegasus_thread_self());
	       try  { _async_callback(msg_handle, msg); }
	       catch(...) { _thread_safety.unlock(); throw; }
	       
	    }
	    void _send_shutdown_notify(Uint32 code)
	    {
	       _shutdown_notify(code);
	    }
	    void lock(void) 
	    {
	       _thread_safety.lock(pegasus_thread_self());
	    }
	    
	    void unlock(void)
	    {
	       _thread_safety.unlock();
	    }
	    
	 private: 
	    module_rep(void);
	    module_rep(const module_rep & );
	    module_rep& operator= (const module_rep & );
      
	    Mutex _thread_safety;
	    ModuleController *_controller;
	    String _name;
	    AtomicInt _reference_count;
	    void *_module_address;
	    Boolean (*_receive_message)(Message *);
	    void (*_async_callback)(Uint32, Message *);
	    void (*_shutdown_notify)(Uint32 code);

	    static Boolean default_receive_message(Message *msg)
	    {
	       throw NotImplemented("Module Receive");
	    }

	    static void default_async_callback(Uint32 handle, Message *msg)
	    {
	       throw NotImplemented("Module Async Receive");
	    }
	    
	    static void default_shutdown_notify(Uint32 code)
	    {
	       return;
	    }

	    friend class ModuleController;
      };
      
   public:

      pegasus_module(ModuleController *controller, 
		     const String &id, 
		     void *module_address,
		     Boolean (*receive_message)(Message *),
		     void (*async_callback)(Uint32, Message *),
		     void (*shutdown_notify)(Uint32 code)) ;
      
      virtual ~pegasus_module(void)
      {
	 _rep->dereference();
	 if( 0 == _rep->reference_count())
	    delete _rep;
      }
      
      pegasus_module & operator= (const pegasus_module & mod);
      Boolean operator == (const pegasus_module *mod) const;
      Boolean operator == (const pegasus_module & mod) const ; 
      Boolean operator == (const String &  mod) const;
      Boolean operator == (const void *mod) const;

      const String & get_name(void) const;
            
      // introspection interface
      Boolean query_interface(const String & class_id, void **object_ptr) const;

   private:

      module_rep *_rep;
      
      pegasus_module(void);
      pegasus_module(const pegasus_module & mod);
      virtual Boolean _rcv_msg(Message *) ;
      Boolean _receive_message(Message *msg)
      {
	 _rep->module_receive_message(msg);
      }
      
      void _send_async_callback(Uint32 msg_handle, Message *msg) 
      {
	 _rep->_send_async_callback(msg_handle, msg);
      }
      void _send_shutdown_notify(Uint32 code)
      {
	 _rep->_send_shutdown_notify(code);
      }
      
      virtual Boolean _shutdown(Uint32) ;
      virtual void reference(void) { _rep->reference(); }
      virtual void dereference(void)  { _rep->dereference(); }

      friend class ModuleController;
};




class PEGASUS_COMMON_LINKAGE ModuleController : public MessageQueueService
{

   public:
      typedef MessageQueueService Base;
      
      ModuleController(const char *name, Uint32 queueID);
      virtual ~ModuleController(void);

      // module api 
      ModuleController & register_module(const String & module_name, 
					 void *module_address, 
					 void (*async_callback)(Uint32, Message *));

      Boolean deregister_module(const String & module_name);
      
      Uint32 find_service(pegasus_module & handle, String & name);
      String & find_service(pegasus_module & handle, Uint32 queue_id);

      pegasus_module & get_module_reference(pegasus_module & handle, String & name);
      
      // send a message to another service
      Message *ModuleSendWait(pegasus_module & handle,
			      Uint32 destination_q, 
			      Message *message);

      // send a message to another module via another service
      Message *ModuleSendWait(pegasus_module & handle,
			      Uint32 destination_q,
			      String & destination_module,
			      Message *message);

      // send a message to another service
      Boolean ModuleSendAsync(pegasus_module & handle,
			      Uint32 msg_handle, 
			      Uint32 destination_q, 
			      Message *message);

      // send a message to another module via another service
      Boolean ModuleSendAsync(pegasus_module & handle,
			      Uint32 msg_handle, 
			      Uint32 destination_q, 
			      String & destination_module,
			      Message *message);

      Uint32 blocking_thread_exec(pegasus_module & handle,
				  PEGASUS_THREAD_RETURN (PEGASUS_THREAD_CDECL *thread_func)(void *), 
				  void *parm);
      Uint32 async_thread_exec(pegasus_module & handle, 
			       PEGASUS_THREAD_RETURN (PEGASUS_THREAD_CDECL *thread_func)(void *), 
			       void *parm);
      
   protected:

   private:

      DQueue<pegasus_module> _modules;
      ThreadPool _thread_pool;
};







PEGASUS_NAMESPACE_END


#endif // Pegasus_Module_Controller_H
