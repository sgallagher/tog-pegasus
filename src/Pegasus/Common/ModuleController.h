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


class PEGASUS_COMMON_LINKAGE service_module
{
   public:

      service_module() 
      {
      }
      
      virtual ~service_module()
      {
      }
      
      Boolean operator == (const service_module *) const;
      Boolean operator == (const String &  ) const ;
      Boolean operator == (const service_module & ) const ;
      Boolean operator == (const void *) const;

      String & get_name(void);
      

   private:

      virtual Boolean _rcv_msg(Message *) = 0;
      virtual void _send_async_callback(Uint32 msg_handle, Message *msg) = 0;
      
      virtual Boolean _shutdown(Uint32) = 0;
      ModuleController *_controller;
      
      String _name;
      
      friend class ModuleController;
}


class PEGASUS_COMMON_LINKAGE ModuleController : public MessageQueueService
{

   public:
      typedef MesageQueueService Base;
      
      ModuleController(const char *name, Uint32 queueID);
      ~ModuleController(void);

      
      
      // module api 
      ModuleController & register_module(service_module *);
      deregister_module(service_module *);
      
      Uint32 find_service(service_module & handle, String & name);
      String & find_service(service_module & handle, Uint32 queue_id);

      Message *ModuleSendWait(service_module & handle,
			      Uint32 destination, 
			      Message *message);

      Message *ModuleSendWait(service_module & handle,
			      String & destination, 
			      Message *message);
      
      Boolean ModuleSendAsync(service_module & handle,
			      Uint32 msg_handle, 
			      Uint32 destination, 
			      Message *message);

      Boolean ModuleSendAsync(service_module & handle,
			      Uint32 msg_handle, 
			      String & destination, 
			      Message *message);

      Uint32 blocking_thread_exec(service_module & handle,
				  PEGASUS_THREAD_RETURN (PEGASUS_THREAD_CDECL *thread_func)(void *));
      Uint32 async_thread_exec(service_module & handle, 
				  PEGASUS_THREAD_RETURN (PEGASUS_THREAD_CDECL *thread_func)(void *));
      

   protected:


      

   private:


      
      

      DQueue<service_module> _modules;
      ThreadPool _thread_pool;
      

}

PEGASUS_NAMESPACE_END


#endif // Pegasus_Module_Controller_h
