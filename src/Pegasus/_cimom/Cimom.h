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
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/AsyncOpNode.h>
#include <Pegasus/_cimom/CimomMessage.h>
#include <Pegasus/Server/CIMOperationRequestDispatcher.h>
#include <Pegasus/Server/CIMOperationResponseEncoder.h>
#include <Pegasus/Server/CIMOperationRequestDecoder.h>

PEGASUS_NAMESPACE_BEGIN


class PEGASUS_CIMOM_LINKAGE module_capabilities 
{
   public:
      static Uint32 async;
      static Uint32 remote;
      static Uint32 trusted;
} ;

class PEGASUS_CIMOM_LINKAGE cimom;

class PEGASUS_CIMOM_LINKAGE message_module
{
   public:
      message_module(const String & name,
		     Uint32 capabilities,
		     Uint32 mask,
		     Uint32 queue)
      
	 : _name(name), _capabilities(capabilities),
	   _mask(mask), _q_id(queue)  { }
      
      Boolean operator == (const message_module *mm) const;
      Boolean operator == (const String & name ) const ;
      Boolean operator == (const message_module & mm ) const ;
      Boolean operator == (const void *) const;
      Boolean operator == (Uint32) const;
      
      
   private:
      String _name;
      Uint32 _capabilities;
      Uint32 _mask;
      
      Uint32 _q_id;
      friend class cimom;
};


class PEGASUS_CIMOM_LINKAGE cimom : public MessageQueue
{
  
   public : cimom(void)
      : MessageQueue("cimom"), _modules(true), 
	_internal_ops(true ), 	_pending_ops(true, 100), 
	_completed_ops(true, 100),
	_pending_thread( _pending_proc, this, false),
	_completed_thread( _completed_proc, this, false),
	_die(0)  
      { 
	 pegasus_gettimeofday(&_last_module_change);
      }
            
      ~cimom(void) ;
            
      Boolean moduleChange(struct timeval last);
      
      Uint32 getModuleCount(void);
      Uint32 getModuleIDs(Uint32 *ids, Uint32 count) throw(IPCException);
      
      virtual void handleEnqueue();
      void handle_internal(AsyncOpNode *internal_op);
      void register_module(CimomRegisterService *msg);
      void deregister_module(CimomDeregisterService *msg) ;
      

   protected:
      Uint32 get_module_q(const String & name);
      void _enqueueResponse(Request *req, Reply *rep);
      
   private:
      struct timeval _last_module_change;
      DQueue<message_module> _modules;
      DQueue<Message> _internal_ops; 

      AsyncDQueue<AsyncOpNode> _new_ops;
      AsyncDQueue<AsyncOpNode> _pending_ops;
      AsyncDQueue<AsyncOpNode> _completed_ops;
      
      static PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL _pending_proc(void *);
      static PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL _completed_proc(void *);
      Thread _pending_thread;
      Thread _completed_thread;
      AtomicInt _die;
      CIMOperationRequestDispatcher *_cim_dispatcher;
      CIMOperationResponseEncoder *_cim_encoder;
      CIMOperationRequestDecoder *_cim_decoder;
      CIMRepository *_repository;
      
};



inline Boolean message_module::operator ==(Uint32 q) const
{
   if(this->_q_id == q)
      return true;
   return false;
}

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
	 if(_mask == mm._mask)
	    if(_q_id == mm._q_id)
	       return true;
   
   return false;
   
}

inline Boolean message_module::operator == (const void *key) const
{
   return operator == ( (*(reinterpret_cast<const message_module *>(key) ) ) );
}


// returns true if the list of registered modules changes since the parameter
inline Boolean cimom::moduleChange(struct timeval last)
{
   if( (last.tv_sec >= _last_module_change.tv_sec)) 
      if(last.tv_usec >_last_module_change.tv_usec )
	 return false;
   return true;
}


inline Uint32 cimom::getModuleCount(void) 
{
   return _modules.count();
}

inline Uint32 cimom::getModuleIDs(Uint32 *ids, Uint32 count) throw(IPCException)
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






PEGASUS_NAMESPACE_END

#endif // CIMOM_include
