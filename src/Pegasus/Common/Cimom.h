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
#include <Pegasus/Common/CimomMessage.h>
//#include <Pegasus/Server/CIMOperationRequestDispatcher.h>
//#include <Pegasus/Server/CIMOperationResponseEncoder.h>
//#include <Pegasus/Server/CIMOperationRequestDecoder.h>

PEGASUS_NAMESPACE_BEGIN


class PEGASUS_COMMON_LINKAGE module_capabilities 
{
   public:
      static Uint32 async;
      static Uint32 remote;
      static Uint32 trusted;
} ;

class PEGASUS_COMMON_LINKAGE cimom;

class PEGASUS_COMMON_LINKAGE message_module
{
   public:
      message_module(void) 
	 : _name(), _capabilities(0), 
	   _mask(0), _q_id(0) { }
      
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
      
      const String & get_name(void) const ;
      Uint32 get_capabilities(void) const ; 
      Uint32 get_mask(void) const ; 
      Uint32 get_queue(void) const ; 

      void put_name(String & name);
      void put_capabilities(Uint32 capabilities);
      void put_mask(Uint32 mask);
      void put_queue(Uint32 queue) ;

   private:
      String _name;
      Uint32 _capabilities;
      Uint32 _mask;
      struct timeval _heartbeat;
      

      Uint32 _q_id;
      friend class cimom;
};


class PEGASUS_COMMON_LINKAGE cimom : public MessageQueue
{
   public : 
      cimom(void);
      
      virtual ~cimom(void) ;
            
      Boolean moduleChange(struct timeval last);
      
      Uint32 getModuleCount(void);
      Uint32 getModuleIDs(Uint32 *ids, Uint32 count) throw(IPCException);

      AsyncOpNode *get_cached_op(void) throw(IPCException);
      void cache_op(AsyncOpNode *op) throw(IPCException);
            
      void set_default_op_timeout(const struct timeval *buffer);
      void get_default_op_timeout(struct timeval *timeout) const ;

      virtual void handleEnqueue();
      void register_module(RegisterCimService *msg);
      void deregister_module(Uint32 quid);
      void deregister_module(DeRegisterCimService *msg) ;
      void update_module(UpdateCimService *msg );
      void ioctl(AsyncIoctl *msg );

      void find_service_q(FindServiceQueue *msg );
      void enumerate_service(EnumerateService *msg );
      
            
   protected:
      Uint32 get_module_q(const String & name);
      void _enqueueResponse(AsyncOpNode *op);
      
   private:
      struct timeval _default_op_timeout;
      struct timeval _last_module_change;
      DQueue<message_module> _modules;

      DQueue<AsyncOpNode> _recycle;
      
      // accepted - put on the pending q
      // complete - put on the completed q
      // released - processed by reply destination, put op node on the recycle queue

      AsyncDQueue<AsyncOpNode> _pending_ops;
      AsyncDQueue<AsyncOpNode> _completed_ops;
      
      static PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL _pending_proc(void *);
      static PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL _completed_proc(void *);

      static Uint32 get_xid(void);
      
      void _handle_cimom_msg(Message *msg);
      Uint32 _ioctl(Uint32, Uint32, void *);
      Thread _pending_thread;
      Thread _completed_thread;

      AtomicInt _die;
      static AtomicInt _xid;
      
//       CIMOperationRequestDispatcher *_cim_dispatcher;
//       CIMOperationResponseEncoder *_cim_encoder;
//       CIMOperationRequestDecoder *_cim_decoder;
//       CIMRepository *_repository;
      
};





PEGASUS_NAMESPACE_END

#endif // CIMOM_include
