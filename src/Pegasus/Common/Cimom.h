//%2005////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
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
// Modified By: David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef CIMOM_include
#define CIMOM_include

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/InternalException.h>
#include <Pegasus/Common/MessageQueue.h>
#include <Pegasus/Common/DQueue.h>
#include <Pegasus/Common/Thread.h>
#include <Pegasus/Common/ArrayInternal.h>
#include <Pegasus/Common/AsyncOpNode.h>
#include <Pegasus/Common/CimomMessage.h>
#include <Pegasus/Common/MessageQueueService.h>
//#include <Pegasus/Server/CIMOperationRequestDispatcher.h>
//#include <Pegasus/Server/CIMOperationResponseEncoder.h>
//#include <Pegasus/Server/CIMOperationRequestDecoder.h>
#include <Pegasus/Common/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

extern const Uint32 CIMOM_Q_ID;

class PEGASUS_COMMON_LINKAGE module_capabilities
{
   public:
      static Uint32 async;
      static Uint32 remote;
      static Uint32 trusted;
      static Uint32 paused;
      static Uint32 stopped;
      static Uint32 module_controller;

} ;

class PEGASUS_COMMON_LINKAGE cimom;

class PEGASUS_COMMON_LINKAGE message_module
{
   public:
      message_module()
	 : _name(), _capabilities(0),
	   _mask(0), _q_id(0) { }

      message_module(const String & name,
		     Uint32 capabilities,
		     Uint32 mask,
		     Uint32 queue)
      	 : _name(name), _modules(), _capabilities(capabilities),
	   _mask(mask), _q_id(queue)  { }

      Boolean operator == (const message_module *mm) const;
      Boolean operator == (const String & name ) const ;
      Boolean operator == (const message_module & mm ) const ;
      Boolean operator == (const void *) const;
      Boolean operator == (Uint32) const;
      const String & get_name() const ;
      Uint32 get_capabilities() const ;
      Uint32 get_mask() const ;
      Uint32 get_queue() const ;

      void put_name(String & name);
      void put_capabilities(Uint32 capabilities);
      void put_mask(Uint32 mask);
      void put_queue(Uint32 queue) ;

   private:
      String _name;
      Array<String> _modules;
      Uint32 _capabilities;
      Uint32 _mask;
      struct timeval _heartbeat;


      Uint32 _q_id;
      friend class cimom;
};

class MessageQueueService;


class PEGASUS_COMMON_LINKAGE cimom : public MessageQueue
{
   public :
      cimom();

      virtual ~cimom() ;

      Boolean moduleChange(struct timeval last);

      Uint32 getModuleCount();
      Uint32 getModuleIDs(Uint32 *ids, Uint32 count);

      AsyncOpNode *get_cached_op();
      void cache_op(AsyncOpNode *op);

      void set_default_op_timeout(const struct timeval *buffer);
      void get_default_op_timeout(struct timeval *timeout) const ;



   protected:
      Uint32 get_module_q(const String & name);
      static void _make_response(Message *req, Uint32 code);
      static void _completeAsyncResponse(AsyncRequest *request,
				  AsyncReply *reply,
				  Uint32 state,
				  Uint32 flag);
      static void _complete_op_node(AsyncOpNode *op, Uint32 state, Uint32 flag, Uint32 code);
      static void _default_callback(AsyncOpNode *, MessageQueue *, void *);

   private:
      struct timeval _default_op_timeout;
      struct timeval _last_module_change;
      DQueue<message_module> _modules;

      DQueue<AsyncOpNode> _recycle;

      AsyncDQueue<AsyncOpNode> _routed_ops;
      DQueue<AsyncOpNode> _internal_ops;

      static PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL _routing_proc(void *);

      Thread _routing_thread;

      static Uint32 get_xid();
      void _handle_cimom_op(AsyncOpNode *op, Thread *thread, MessageQueue *queue);
      Uint32 _ioctl(Uint32, Uint32, void *);

      virtual void handleEnqueue();
      void register_module(RegisterCimService *msg);
      void deregister_module(Uint32 quid);
      void update_module(UpdateCimService *msg );
      void ioctl(AsyncIoctl *msg );

      void find_service_q(FindServiceQueue *msg );
      void enumerate_service(EnumerateService *msg );
      Boolean route_async(AsyncOpNode *operation);
      void _shutdown_routed_queue();

      void _registered_module_in_service(RegisteredModule *msg);
      void _deregistered_module_in_service(DeRegisteredModule *msg);
      void _find_module_in_service(FindModuleInService *msg);


      AtomicInt _die;
      AtomicInt _routed_queue_shutdown;

      static AtomicInt _xid;
      static cimom *_global_this;

      friend class MessageQueueService;
};





PEGASUS_NAMESPACE_END

#endif // CIMOM_include
