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
// Author: Mike Day (mdday@us.ibm.com
//
// Modified By: 
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef CIMOM_include
#define CIMOM_include

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/MessageQueue.h>
#include <Pegasus/Common/DQueue.h>
#include <Pegasus/Common/Thread.h>
#include <Pegasus/Common/AsyncOpNode.h>
#include <Pegasus/_cimom/CimomMessage.h>


PEGASUS_NAMESPACE_BEGIN


class PEGASUS_CIMOM_LINKAGE module_capabilities 
{
   public:
      static const Uint32 async;
      static const Uint32 remote;
      static const Uint32 trusted;
} ;

class PEGASUS_CIMOM_LINKAGE message_module
{
   public:
      message_module(const String & name,
		     Uint32 capabilities,
		     Uint32 messages,
		     Uint32 queue)
      
	 : _name(name), _capabilities(capabilities),
	   _messages(messages), _q_id(queue)  { }
      
      Boolean operator == (const message_module *mm) const;
      Boolean operator == (const String & name ) const ;
      Boolean operator == (const message_module & mm ) const ;
      Boolean operator == (const void *) const;
      
      
   private:
      String _name;
      Uint32 _capabilities;
      Uint32 _messages;
      Uint32 _q_id;
};


class PEGASUS_CIMOM_LINKAGE cimom : public MessageQueue
{
  
   public : cimom(void)
      : MessageQueue("cimom"), _modules(true ), 
	_internal_ops(true ), _new_ops(true, 20), 
	_pending_ops(true, 100), _completed_ops(true, 100),
	_thread( _proc, this, false), _die(0)
      
      
      {  }
            
      virtual void handleEnqueue();
      void cimom::register_module(ModuleRegister *);
      

   protected:
      Uint32 get_module_q(const String & name);
      void _enqueueResponse(CimomRequest *req, CimomReply *rep);
      
   private:
      DQueue<message_module> _modules;
      DQueue<AsyncOpNode> _internal_ops; // a place to put messages to me that I
                                         // need to process asynchronously
      AsyncDQueue<AsyncOpNode> _new_ops;
      AsyncDQueue<AsyncOpNode> _pending_ops;
      AsyncDQueue<AsyncOpNode> _completed_ops;
      
      static PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL _proc(void *);
      
      Thread _thread;
      AtomicInt _die;
      
};


inline Boolean message_module::operator == (const message_module *mm) const 
{
   if(this == mm)
      return true;
   return false;
}


inline Boolean message_module::operator == (const String & name ) const 
{
   if(name == this->_name)
      return true;
   return false;
   
}

inline Boolean message_module::operator == (const message_module & mm) const
{
   if(*this == mm)
      return true;
   if( _name == mm._name )
      if ( _capabilities == mm._capabilities)
	 if(_messages == mm._messages)
	    if(_q_id == mm._q_id)
	       return true;
   
   return false;
   
}

inline Boolean message_module::operator == (const void *key) const
{
   return this->operator == (reinterpret_cast<const String &>(*key));
      
}

PEGASUS_NAMESPACE_END

#endif // CIMOM_include
