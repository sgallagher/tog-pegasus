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
// Author: Mike Day (mdday@us.ibm.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_CimomMessage_h
#define Pegasus_CimomMessage_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/MessageQueue.h>
#include <Pegasus/Common/AsyncOpNode.h>

PEGASUS_NAMESPACE_BEGIN
//
// This identifier is the queue id for CIMOM queue. It is initialized in
// CimomMessage.cpp by calling MessageQueue::getNextQueueId(). Note that
// this value is passed in the constructor for the CIMOM queue.
//
extern const Uint32 CIMOM_Q_ID;

class AsyncOpNode;

class cimom_results
{
   public:
      static const Uint32 OK;
      static const Uint32 PARAMETER_ERROR;
      static const Uint32 MODULE_ALREADY_REGISTERED;
      static const Uint32 MODULE_NOT_FOUND;
      static const Uint32 INTERNAL_ERROR;

      static const Uint32 ASYNC_STARTED;
      static const Uint32 ASYNC_PROCESSING;
      static const Uint32 ASYNC_COMPLETE;
      static const Uint32 ASYNC_CANCELLED;
      static const Uint32 ASYNC_PAUSED;
      static const Uint32 ASYNC_RESUMED;

      static const Uint32 CIM_SERVICE_STARTED;
      static const Uint32 CIM_SERVICE_STOPPED;
      static const Uint32 CIM_SERVICE_PAUSED;

      static const Uint32 CIM_SERVICE_RESUMED;
      static const Uint32 CIM_NAK;

      static const Uint32 ASYNC_PHASE_COMPLETE;
      static const Uint32 ASYNC_CHILD_COMPLETE;
      static const Uint32 ASYNC_PHASE_STARTED;
      static const Uint32 ASYNC_CHILD_STARTED;
};

// messages handled by the cimom
class cimom_messages
{
   public:
      static const Uint32 HEARTBEAT;
      static const Uint32 REPLY;
      static const Uint32 REGISTER_CIM_SERVICE;
      static const Uint32 DEREGISTER_CIM_SERVICE;
      static const Uint32 UPDATE_CIM_SERVICE;
      static const Uint32 IOCTL;
      static const Uint32 ASYNC_OP_REPLY;
      static const Uint32 ASYNC_LEGACY_OP_REPLY;

      static const Uint32 FIND_SERVICE_Q;
      static const Uint32 ENUMERATE_SERVICE;
};

// messages handled by services (modules)
class service_messages
{
   public:
      static const Uint32 HEARTBEAT;
      static const Uint32 REPLY;
      static const Uint32 START;
      static const Uint32 STOP;
      static const Uint32 PAUSE;
      static const Uint32 RESUME;
      static const Uint32 IOCTL;
      static const Uint32 ASYNC_OP_START;
      static const Uint32 ASYNC_OP_CANCEL;
      static const Uint32 ASYNC_OP_PAUSE;
      static const Uint32 ASYNC_OP_RESUME;
      static const Uint32 ASYNC_LEGACY_OP_START;

      static const Uint32 FIND_SERVICE_Q_RESPONSE;
      static const Uint32 ENUMERATE_SERVICE_RESPONSE;
};


class PEGASUS_CIMOM_LINKAGE Request : public Message
{
   public:
      Request(Uint32 type,
	      Uint32 key,
	      QueueIdStack queue_ids,
	      Uint32 mask,
	      Uint32 routing = 0)
	 : Message(type, key, routing, mask | message_mask::ha_request), 
	   queues(queue_ids) {   }

      virtual ~Request(void) { }

      QueueIdStack queues;
} ;

class PEGASUS_CIMOM_LINKAGE Reply : public Message
{
   public:
      Reply(Uint32 type,
	    Uint32 key,
	    Uint32 result_code,
	    Uint32 mask,
	    Uint32 routing = 0)
	 : Message(type, key, routing, mask | message_mask::ha_reply),
	   result(result_code) {  }

      virtual ~Reply(void) { };

      Uint32 result;
} ;


class PEGASUS_CIMOM_LINKAGE AsyncRequest : public Request
{
   public:
      AsyncRequest(Uint32 type, 
		   Uint32 key, 
		   QueueIdStack queue_ids,
		   Uint32 mask,
		   AsyncOpNode *opnode,
		   Uint32 destination_queue = 0,
		   Uint32 routing = 0) 
	 : Request(type, key, queue_ids, mask | message_mask::ha_async, routing),
	   dest_q(destination_queue),
	   op(opnode) {  }
      virtual ~AsyncRequest(void);

      Uint32 dest_q;
      
      AsyncOpNode *op;
} ;

class PEGASUS_CIMOM_LINKAGE AsyncReply : public Reply
{
   public:
      AsyncReply(Uint32 type, 
		 Uint32 key, 
		 Uint32 result_code, 
		 Uint32 mask, 
		 AsyncOpNode *opnode,
		 Uint32 routing = 0)
	 :Reply(type, key, result_code, mask | message_mask::ha_async, routing),
	  op(opnode) {  }
      virtual ~AsyncReply(void);
      
      AsyncOpNode *op;
} ;


class PEGASUS_CIMOM_LINKAGE Ioctl : public Request
{
   public:
      Ioctl(Uint32 type,
	    Uint32 key,
	    QueueIdStack queue_ids,
	    Uint32 mask,
	    Uint32 control_code,
	    Uint32 uint_param,
	    void * pointer_param,
	    Uint32 routing = 0)
	 : Request(type, key, queue_ids, mask, routing),
	   code(code), uint(uint_param), pparam(pointer_param) { }

      virtual ~Ioctl(void) { };

      Uint32 code;
      Uint32 uint;
      void *pparam;
      
};

// heartbeat TO the cimom from a CIM_SERVICE

class PEGASUS_CIMOM_LINKAGE CimomHeartBeat : public Reply
{
   public:
      CimomHeartBeat(Uint32 key,
		     Uint32 sender,
		     Uint32 routing = 0)
	 : Reply(cimom_messages::HEARTBEAT, key, 0,
		 message_mask::type_cimom | message_mask::ha_reply | message_mask::ha_synchronous,
		 routing ), 
	   sendingQ(sender)
      {
      }

      virtual ~CimomHeartBeat(void) { };
      Uint32 sendingQ;

} ;

// request from a service to the cimom
class PEGASUS_CIMOM_LINKAGE CimomRegisterService : public Request
{
   public:
      CimomRegisterService(Uint32 key,
			   QueueIdStack queue_ids,
			   const String & module_name,
			   Uint32 module_capabilities,
			   Uint32 module_mask,
			   Uint32 module_queue,
			   Uint32 routing = 0)
	 : Request(cimom_messages::REGISTER_CIM_SERVICE, key, queue_ids, 
			message_mask::type_cimom | message_mask::ha_request , 
			routing),
	   name(module_name), capabilities(module_capabilities),
	   mask(module_mask ), q_id(module_queue)  {   }

      virtual ~CimomRegisterService(void) { };

      String name;
      Uint32 capabilities;
      Uint32 mask;
      Uint32 q_id;
};


// request from a service to the cimom
class PEGASUS_CIMOM_LINKAGE CimomDeregisterService : public Request
{
   public:

      CimomDeregisterService(Uint32 key,
			     QueueIdStack queue_ids,
			     Uint32 module_queue,
			     Uint32 routing = 0 )
	 : Request(cimom_messages::DEREGISTER_CIM_SERVICE, key, queue_ids, 
		   (message_mask::type_cimom | message_mask::ha_request),
		   routing),
	   q_id(module_queue) {  }

      virtual ~CimomDeregisterService(void) { };

      Uint32 q_id;
} ;


// modules can unly update their capabilities or their mask, not their queue_id.
// changing a queue_id is equivalent to deregistering and registering.
class PEGASUS_CIMOM_LINKAGE CimomUpdateService : public Request
{
   public:
      CimomUpdateService(Uint32 key,
			 QueueIdStack queue_ids,
			 Uint32 module_capabilities,
			 Uint32 module_mask,
			 Uint32 module_queue,
			 Uint32 routing = 0)
	 : Request(cimom_messages::UPDATE_CIM_SERVICE, key, queue_ids, 
		   message_mask::type_cimom | message_mask::ha_request,
		   routing ) ,
	  capabilities(module_capabilities), mask(module_mask),
	  q_id(module_queue) {  }

      virtual ~CimomUpdateService(void) { };

      Uint32 capabilities;
      Uint32 mask;
      Uint32 q_id;
} ;



// ioctl issued TO the cimom FROM a Service

class PEGASUS_CIMOM_LINKAGE CimomIoctl : public Ioctl
{
   public:
      CimomIoctl(Uint32 key,
		 QueueIdStack queue_ids,
		 Uint32 code,
		 Uint32 uint_param,
		 void *pointer_param,
		 Uint32 routing = 0)
	 : Ioctl(cimom_messages::IOCTL,
		 key,
		 queue_ids,
		 message_mask::type_cimom | message_mask::ha_request,
		 code,
		 uint_param,
		 pointer_param,
		 routing) { }
      
      virtual ~CimomIoctl(void) { };
};

class PEGASUS_CIMOM_LINKAGE CimomAsyncOpReply : public AsyncReply
{
   public:
      CimomAsyncOpReply(Uint32 key,
			Uint32 result_code,
			AsyncOpNode *operation,
			Uint32 routing = 0)
	 : AsyncReply(cimom_messages::ASYNC_OP_REPLY, 
		      key, 
		      result_code,
		      message_mask::type_cimom | message_mask::ha_reply, 
		      operation,
		      routing) {  }

	  virtual ~CimomAsyncOpReply(void) { };

};

class PEGASUS_CIMOM_LINKAGE CimomAsyncLegacyOpReply: public AsyncReply
{
   public:
      CimomAsyncLegacyOpReply(Uint32 key, 
			      Uint32 result_code,
			      AsyncOpNode *operation,
			      Uint32 routing = 0)
	 : AsyncReply(cimom_messages::ASYNC_LEGACY_OP_REPLY, 
		      key, 
		      result_code,
		      message_mask::type_cimom | message_mask::ha_reply,
		      operation,
		      routing) {   }
      virtual ~CimomAsyncLegacyOpReply(void);
};


class PEGASUS_CIMOM_LINKAGE CimomFindServices: public Request
{
   public:
      CimomFindServices(Uint32 key,
			QueueIdStack queue_ids, 
			String service_name,
			Uint32 service_capabilities,
			Uint32 service_mask,
			Uint32 routing = 0)
	 : Request(cimom_messages::FIND_SERVICE_Q,
		   key,
		   queue_ids,
		   message_mask::type_cimom | message_mask::ha_request,
		   routing),
	   name(service_name),
	   capabilities(service_capabilities),
	   mask(service_mask)  { }
      virtual ~CimomFindServices(void);
      
      String name;
      Uint32 capabilities;
      Uint32 mask;
};


class PEGASUS_CIMOM_LINKAGE CimomEnumerateService : public Request
{
   public:
      CimomEnumerateService(Uint32 key,
			    QueueIdStack queue_ids, 
			    Uint32 serviceQ,
			    Uint32 routing = 0)
	 : Request(cimom_messages::ENUMERATE_SERVICE, 
		   key, 
		   queue_ids, 
		   message_mask::type_cimom | message_mask::ha_request,
		   routing),
	   qid(serviceQ) {  }
      virtual ~CimomEnumerateService(void);
      
      Uint32 qid;
};

// async operation issued TO a service FROM the Cimom
class PEGASUS_CIMOM_LINKAGE ServiceAsyncReq : public AsyncRequest
{
   public:
      ServiceAsyncReq(Uint32 type,
		      Uint32 key,
		      QueueIdStack queue_ids,
		      AsyncOpNode *op,
		      Uint32 destination_queue = 0,
		      Uint32 routing = 0)
	 : AsyncRequest(type, key, queue_ids, 
			( message_mask::type_service | message_mask::ha_request),
			op, routing) {  }
      
      virtual ~ServiceAsyncReq(void) ;
      
      Uint32 destination;
} ;


class PEGASUS_CIMOM_LINKAGE ServiceAsyncOpStart : public ServiceAsyncReq
{
   public:
      ServiceAsyncOpStart(Uint32 key,
			  QueueIdStack queue_ids,
			  AsyncOpNode *operation,
			  Uint32 routing = 0)
	 : ServiceAsyncReq(service_messages::ASYNC_OP_START, key, queue_ids, operation, routing)  {  }
      
      virtual ~ServiceAsyncOpStart(void) ;
};


class PEGASUS_CIMOM_LINKAGE ServiceAsyncLegacyOpStart : public AsyncRequest
{
   public:
      ServiceAsyncLegacyOpStart(Uint32 key, 
				QueueIdStack queue_ids, 
				AsyncOpNode *operation,
				Uint32 routing = 0)
	 : AsyncRequest(service_messages::ASYNC_LEGACY_OP_START,
		    key, queue_ids, 
		    message_mask::type_service | message_mask::ha_request, 
		    operation, routing) {    }
      virtual ~ServiceAsyncLegacyOpStart(void) ;
};


class PEGASUS_CIMOM_LINKAGE ServiceAsyncOpCancel : public ServiceAsyncReq
{
   public:
      ServiceAsyncOpCancel(Uint32 key,
			   QueueIdStack queue_ids,
			   AsyncOpNode *operation,
			   Uint32 routing = 0)
	 : ServiceAsyncReq(service_messages::ASYNC_OP_CANCEL, 
			   key, 
			   queue_ids, 
			   operation, 
			   routing) {  }
      
      virtual ~ServiceAsyncOpCancel(void) ;
      
};

class PEGASUS_CIMOM_LINKAGE ServiceAsyncOpPause : public ServiceAsyncReq
{
   public:
      ServiceAsyncOpPause(Uint32 key,
			  QueueIdStack queue_ids,
			  AsyncOpNode *operation,
			  Uint32 routing = 0)
	 : ServiceAsyncReq(service_messages::ASYNC_OP_PAUSE, 
			   key, 
			   queue_ids, 
			   operation, 
			   routing) {  }
      
      virtual ~ServiceAsyncOpPause(void) ;
      
};

class PEGASUS_CIMOM_LINKAGE ServiceAsyncOpResume : public ServiceAsyncReq
{
   public:
      ServiceAsyncOpResume(Uint32 key,
			   QueueIdStack queue_ids,
			   AsyncOpNode *operation,
			   Uint32 routing = 0)
	 : ServiceAsyncReq(service_messages::ASYNC_OP_RESUME, 
			   key, 
			   queue_ids, 
			   operation, 
			   routing) {  }
      
      virtual ~ServiceAsyncOpResume(void) ;
      
};

class PEGASUS_CIMOM_LINKAGE ServiceIoctl : public Ioctl
{
   public:
      ServiceIoctl(Uint32 key,
		   QueueIdStack queue_ids,
		   Uint32 code,
		   Uint32 uint_param,
		   void *pointer_param,
		   AsyncOpNode *operation_node,
		   Uint32 routing = 0)
	 : Ioctl(service_messages::IOCTL,
		 key,
		 queue_ids,
		 message_mask::type_service | message_mask::ha_request,
		 code,
		 uint_param,
		 pointer_param,
		 routing),
	   op(operation_node) { }
      
      virtual ~ServiceIoctl(void) ;
      
      AsyncOpNode *op;
      
};

class PEGASUS_CIMOM_LINKAGE ServiceHeartbeat : public Request
{
   public:
      ServiceHeartbeat(Uint32 key, 
		       QueueIdStack queue_ids,
		       Uint32 routing = 0)
	 :Request(service_messages::HEARTBEAT, 
		  key, 
		  queue_ids, 
		  message_mask::type_service | message_mask::ha_request | message_mask::ha_synchronous,
		  routing) {  }
      virtual ~ServiceHeartbeat(void) ;
};

class PEGASUS_CIMOM_LINKAGE ServiceStart : public AsyncRequest
{
   public:
      ServiceStart(Uint32 key, 
		   QueueIdStack queue_ids,
		   AsyncOpNode *op,
		   Uint32 routing = 0)
	 :AsyncRequest(service_messages::START, 
		  key, 
		  queue_ids, 
		  message_mask::type_service | message_mask::ha_request,
		  op, routing) {  }
      virtual ~ServiceStart(void) ;
};

class PEGASUS_CIMOM_LINKAGE ServiceStop : public AsyncRequest
{
   public:
      ServiceStop(Uint32 key, 
		  QueueIdStack queue_ids,
		  AsyncOpNode *op,
		  Uint32 routing = 0)
	 :AsyncRequest(service_messages::STOP, 
		       key, 
		       queue_ids, 
		       message_mask::type_service | message_mask::ha_request,
		       op, 
		       routing) {  }
      virtual ~ServiceStop(void) ;
};

class PEGASUS_CIMOM_LINKAGE ServicePause : public AsyncRequest
{
   public:
      ServicePause(Uint32 key, 
		   QueueIdStack queue_ids,
		   AsyncOpNode *op,
		   Uint32 routing = 0)
	 :AsyncRequest(service_messages::PAUSE, 
		  key, 
		  queue_ids, 
		  message_mask::type_service | message_mask::ha_request,
		  op,
		  routing) {  }
      virtual ~ServicePause(void) ;
};

class PEGASUS_CIMOM_LINKAGE ServiceResume : public AsyncRequest
{
   public:
      ServiceResume(Uint32 key, 
		    QueueIdStack queue_ids,
		    AsyncOpNode *op,
		    Uint32 routing = 0)
	 :AsyncRequest(service_messages::RESUME, 
		  key, 
		  queue_ids, 
		  message_mask::type_service | message_mask::ha_request,
		  op,
		  routing) {  }
      virtual ~ServiceResume(void) ;
};

class PEGASUS_CIMOM_LINKAGE FindServiceQResponse : public Reply
{
   public:
      FindServiceQResponse(Uint32 key, 
			   Uint32 result_code,
			   Array<Uint32> service_queues, 
			   Uint32 routing = 0)
	 : Reply(service_messages::FIND_SERVICE_Q_RESPONSE,
		 key, 
		 result_code, 
		 message_mask::type_service | message_mask::ha_reply,
		 routing) ,
	   qids(service_queues) {   }
      
      virtual ~FindServiceQResponse(void);
      Array<Uint32> qids;
} ;


class PEGASUS_CIMOM_LINKAGE EnumerateServiceResponse : public Reply
{
   public:
      EnumerateServiceResponse(Uint32 key, 
			       Uint32 result_code, 
			       String service_name,
			       Uint32 service_capabilities, 
			       Uint32 service_mask, 
			       Uint32 service_queue, 
			       Uint32 routing = 0)
	 : Reply( service_messages::ENUMERATE_SERVICE_RESPONSE, 
		  key,
		  result_code, 
		  message_mask::type_service | message_mask::ha_reply, 
		  routing), 
	   name(service_name), 
	   capabilities(service_capabilities), 
	   mask(service_mask),
	   qid(service_queue) {  }
      virtual ~EnumerateServiceResponse(void);
      
      String name;
      Uint32 capabilities;
      Uint32 mask;
      Uint32 qid;
      
};



// void handleEnqueue(void )
// {

//    Message* message = dequeue();

//     if (!message)
// 	return;

//     switch (message->getType())
//     {
//        case CIM_SERVICE_ASYNC_OP:
//        {
// 	  // pass the async op node to the handler
// 	  handleAsyncOpMessage(  static_cast<ServiceAsyncOp *>(message) );
	
// 	  break;
//        }

//     }

//     delete message;
// }


// void handleAsyncOpMessage(ServiceAsyncOp *request)
// {

//    // let the cimom know that you have started processing the op node
//    request->op->processing();

//    // retrieve the original CIM Operation Message from the AsyncOpNode
//    Message *encapsulatedMessage = node->get_request();

//    // now you can process the encapsulated message
//    // there are several different ways to do this, below is just one
//    switch( encapsulatedMessage->getType() )
//    {
//       case CIM_GET_PROPERTY_MESSAGE:
// 	 handleGetPropertyRequest(request->op, (CIMGetPropertyRequestMessage *)encpasulatedMessage) ;
// 	 break;
//    }

//    // construct a reply to the Cimom

//    Reply *reply = new CimomAsyncReply(request->getKey, ASYNC_STARTED, request->op);


//    MessageQueue *queue = MessageQueue::lookup(CIMOM_Q_ID);
//    queue->enqueue(reply);
// }


// void handleGetPropertyRequest(AsyncOpNode *op, CIMGetPropertyRequestMessage *request)
// {
//    AsyncResponseHandler<CIMValue> *rh = new AsyncResponseHandler<CIMValue>( );

//    rh->set_parent(op);

//    ProviderHandle * provider = getProvider( ... );



//    // see the header for the AsyncResponseHandler -- its interfaces
// automatically call into the AsyncOpNode to keep the cimom
// aware of the processing progress of this operation

//    CIMValue value = provider->getProperty( op->get_context(),
// 					   request->nameSpace,
// 					   request->instanceName,
// 					   request->propertyName,
// 					   rh);

//     CIMGetPropertyResponseMessage* response =
//        new CIMGetPropertyResponseMessage(
// 	  request->messageId,
// 	  errorCode,
// 	  errorDescription,
// 	  request->queueIds.copyAndPop(),
// 	  value);
//     // DO NOT enqueue the response message

//     op->put_response(response);
//     rh->complete();


//    // do not delete the request message !!!!

//    // update the time stamp on the async op node

//    op->update();
// }


PEGASUS_NAMESPACE_END

#endif // CIMOM_MESSAGE_include
