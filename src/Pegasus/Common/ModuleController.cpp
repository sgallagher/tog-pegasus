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
			       Boolean (*receive_message)(Message *),
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
   :Base(name, MessageQueue::getNextQueueId()),
    _modules(true),
   _thread_pool(2, "test pool ",  1, 10, create, destroy, deadlock)   
{ 

}

ModuleController::ModuleController(const char *name ,
				   Sint16 min_threads, 
				   Sint16 max_threads,
				   struct timeval & create_thread,
				   struct timeval & destroy_thread,
				   struct timeval & deadlock)
   :Base(name, MessageQueue::getNextQueueId()),
   _modules(true),
    _thread_pool(min_threads + 1, 
		 name, min_threads, 
		 max_threads, 
		 create_thread, 
		 destroy_thread, 
		 deadlock)   
{ 

}



ModuleController & ModuleController::register_module(const String & module_name, 
						     void *module_address, 
						     Boolean (*receive_message)(Message *),
						     void (*async_callback)(Uint32, Message *),
						     void (*shutdown_notify)(Uint32)) throw(AlreadyExists)
{
   return *this;
}




PEGASUS_NAMESPACE_END
