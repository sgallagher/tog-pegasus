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
// Author: Mike Day (mdday@us.ibm.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_MessageQueue_Service_h
#define Pegasus_MessageQueue_Service_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Message.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/IPC.h>
#include <Pegasus/Common/Thread.h>
#include <Pegasus/Common/AsyncOpNode.h>
#include <Pegasus/Common/Cimom.h>
#include <Pegasus/Common/CimomMessage.h>

PEGASUS_NAMESPACE_BEGIN

extern const Uint32 CIMOM_Q_ID;

class message_module;

class PEGASUS_COMMON_LINKAGE MessageQueueService : public MessageQueue
{
   public:

      typedef MessageQueue Base;
      
      MessageQueueService(const char *name, Uint32 queueID, 
			  Uint32 capabilities = 0, 
			  Uint32 mask = message_mask::type_cimom | 
			  message_mask::type_service | 
			  message_mask::ha_request | 
			  message_mask::ha_reply | 
			  message_mask::ha_async ) ;
      
      virtual ~MessageQueueService(void);
      
      virtual void handle_heartbeat_request(AsyncRequest *req);
      virtual void handle_heartbeat_reply(AsyncReply *rep);
      
      virtual void handle_AsyncIoctl(AsyncIoctl *req);
      virtual void handle_CimServiceStart(CimServiceStart *req);
      virtual void handle_CimServiceStop(CimServiceStop *req);
      virtual void handle_CimServicePause(CimServicePause *req);
      virtual void handle_CimServiceResume(CimServiceResume *req);
      
      virtual void handle_AsyncOperationStart(AsyncOperationStart *req);
      virtual void handle_AsyncOperationResult(AsyncOperationResult *rep);
      virtual void handle_AsyncLegacyOperationStart(AsyncLegacyOperationStart *req);
      virtual void handle_AsyncLegacyOperationResult(AsyncLegacyOperationResult *rep);
      
      virtual Boolean isAsync(void) {  return true;  }
      
      virtual Boolean accept_async(AsyncOpNode *op);
      virtual Boolean messageOK(const Message *msg) ;
      
      virtual void enqueue(Message *) throw(IPCException);
      
      AsyncReply *SendWait(AsyncRequest *request);
      Boolean SendAsync(AsyncOpNode *op, 
			Uint32 destination,
			void (*callback)(AsyncOpNode *, MessageQueue *, void *));
      Boolean  SendForget(Message *msg);

      void _completeAsyncResponse(AsyncRequest *request, 
				 AsyncReply *reply, 
				 Uint32 state, 
				 Uint32 flag);
      Boolean register_service(String name, Uint32 capabilities, Uint32 mask);
      Boolean update_service(Uint32 capabilities, Uint32 mask);
      Boolean deregister_service(void);
      virtual void _shutdown_incoming_queue(void);
      void find_services(String name,
			 Uint32 capabilities, 
			 Uint32 mask, 
			 Array<Uint32> *results);
      void enumerate_service(Uint32 queue, message_module *result);
      Uint32 get_next_xid(void);
      AsyncOpNode *get_op(void);
      void return_op(AsyncOpNode *op);

      Uint32 _mask;
      AtomicInt _die;
   protected:
      virtual void handleEnqueue(void);
      virtual void handleEnqueue(Message *);
      virtual Boolean _enqueueResponse(Message *, Message *);
      virtual void _handle_incoming_operation(AsyncOpNode *operation, Thread *thread, MessageQueue *queue);
      virtual void _handle_async_request(AsyncRequest *req);
      virtual void _handle_async_callback(AsyncOpNode *operation);     
      virtual void _make_response(Message *req, Uint32 code);
      
      static cimom *_meta_dispatcher;
      static AtomicInt _service_count;
      static Mutex _meta_dispatcher_mutex;
      

   private: 
      
      DQueue<AsyncOpNode> _pending;
      AsyncDQueue<AsyncOpNode> _incoming;
      
      static PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL _req_proc(void *);
      AtomicInt _incoming_queue_shutdown;
      
      Thread _req_thread;
      
      struct timeval _default_op_timeout;

      static AtomicInt _xid;
      



};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_MessageQueue_Service_h */


