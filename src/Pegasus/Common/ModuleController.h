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
      class  module_rep;
      
   public:

      pegasus_module(ModuleController *controller, 
		     const String &id, 
		     void *module_address,
		     void (*_async_callback)(Uint32, Message *)) 
      {
	 
      }
      
      pegasus_module(const pegasus_module & mod)
      {
	 (mod._rep->_reference_count)++;
	 _rep = mod._rep;
      }
      
      virtual ~pegasus_module()
      {
	 if( 0 == (_rep->_reference_count.value()))
	    delete _rep;
	 
      }
      
      pegasus_module & pegasus_module::operator= (const pegasus_module & mod)
      {
	 (mod._rep->_reference_count)++;
	 if ( ((rep->_reference_count)--) == 0 )
	    delete rep;
	 _rep = mod._rep;
	 return *this;
      }
      
      
      Boolean operator == (const pegasus_module *mod) const
      {
	 if(mod->_rep == _rep)
	    return true;
	 return false;
      }
      
      Boolean operator == (const String &  mod) const 
      {
	 if(_rep->_name == mod)
	    return true;
	 return false;
      }
      
      Boolean operator == (const pegasus_module & mod) const 
      {
	 if( mod._rep == _rep )
	    return true;
	 return false;
	 
      }
      
      Boolean operator == (const void *mod) const
      {
	 if ( (reinterpret_cast<pegasus_module *>(mod))->_rep == _rep)
	    return true;
	 return false;
      }
      
      const String & get_name(void);
            
      // introspection interface
      Boolean query_interface(String & class_id, void **object_ptr) ;

   private:

      module_rep *_rep;
      
      pegasus_module(void);
      virtual Boolean _rcv_msg(Message *) = 0;
      void _send_async_callback(Uint32 msg_handle, Message *msg) ;
      
      virtual Boolean _shutdown(Uint32) = 0;

      virtual Uint32 reference(void) { _reference_count++; }
      virtual Uint32 dereference(void)  { _reference_count--; }

      friend class ModuleController;
};


class PEGASUS_COMMON_LINKAGE pegasus_module::module_rep 
{
   public:
      module_rep(ModuleController *controller, 
		 const String & name,
		 void *module_address, 
		 void (*async_callback)(Uint32, Message *))
	 : _controller(controller), 
	   _name(name), 
	   _reference_count(1), 
	   _module_address(module_address),
	   _async_callback(async_callback)
      {

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
      

   private: 
      module_rep(void);
      module_rep(const module_rep & );
      module_rep& operator= (const module_rep & );
      

      ModuleController *_controller;
      String _name;
      AtomicInt _reference_count;
      void *_module_address;
      void (*_async_callback)(Uint32, Message *);
      friend class ModuleController;
};


class PEGASUS_COMMON_LINKAGE ModuleController : public MessageQueueService
{

   public:
      typedef MesageQueueService Base;
      
      ModuleController(const char *name, Uint32 queueID);
      virtual ~ModuleController(void);

      // module api 
      ModuleController & register_module(const String & module_name, 
					 void *module_address, 
					 void (*async_callback)(Uint32, Message *));

      deregister_module(const String & module_name);
      
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


#endif // Pegasus_Module_Controller_h
