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

class async_results
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


class async_messages
{
   public:
      static const Uint32 HEARTBEAT;
      static const Uint32 REPLY;
      static const Uint32 REGISTER_CIM_SERVICE;
      static const Uint32 DEREGISTER_CIM_SERVICE;
      static const Uint32 UPDATE_CIM_SERVICE;
      static const Uint32 IOCTL;
      static const Uint32 CIMSERVICE_START;
      static const Uint32 CIMSERVICE_STOP;
      static const Uint32 CIMSERVICE_PAUSE;
      static const Uint32 CIMSERVICE_RESUME;

      static const Uint32 ASYNC_OP_START;
      static const Uint32 ASYNC_OP_RESULT;
      static const Uint32 ASYNC_LEGACY_OP_START;
      static const Uint32 ASYNC_LEGACY_OP_RESULT;

      static const Uint32 FIND_SERVICE_Q;
      static const Uint32 FIND_SERVICE_Q_RESULT;
      static const Uint32 ENUMERATE_SERVICE;
      static const Uint32 ENUMERATE_SERVICE_RESULT;
};


class PEGASUS_CIMOM_LINKAGE AsyncMessage : public Message
{
   public:
      AsyncMessage(Uint32 type, 
		   Uint32 key, 
		   Uint32 routing,
		   Uint32 mask,
		   AsyncOpNode *operation)
	 : Message(type, key, routing, mask | message_mask::ha_async),
	   op(operation) {  }
      virtual ~AsyncMessage(void);

      Boolean operator ==(void *key);
      Boolean operator ==(const AsyncMessage& msg);
      
      AsyncOpNode *op;
};


inline Boolean AsyncMessage::operator ==(void *key)
{
   if( key == reinterpret_cast<void *>(this))
      return true;
   return false;
}

inline Boolean AsyncMessage::operator ==(const AsyncMessage& msg)
{
   return this->operator==(reinterpret_cast<void *>(const_cast<AsyncMessage *>(&msg)));
}



class PEGASUS_CIMOM_LINKAGE AsyncRequest : public AsyncMessage
{
   public:
      AsyncRequest(Uint32 type, 
		   Uint32 key, 
		   Uint32 routing,
		   Uint32 mask,
		   AsyncOpNode *operation,
		   Uint32 destination,
		   Uint32 response,
		   Boolean blocking)
	 : AsyncMessage(type, key, routing, mask | message_mask::ha_request, operation),
	   dest(destination),
	   resp(response),
	   block(blocking) 
      {  
	 op->put_request(this);
      }

      virtual ~AsyncRequest(void);
      
      Uint32 dest;
      Uint32 resp;
      Boolean block;
};

class PEGASUS_CIMOM_LINKAGE AsyncReply : public AsyncMessage
{
   public:
      AsyncReply(Uint32 type, 
		 Uint32 key, 
		 Uint32 routing, 
		 Uint32 mask,
		 AsyncOpNode *operation,
		 Uint32 result_code,
		 Uint32 destination,
		 Boolean blocking)
	 : AsyncMessage(type, key, routing, mask | message_mask::ha_reply, operation),
	   result(result_code),
	   dest(destination),
	   block(blocking) 
      {  
	 op->put_response(this);
      }
      virtual ~AsyncReply(void);
      
      Uint32 result;
      Uint32 dest;
      Boolean block;
};

class PEGASUS_CIMOM_LINKAGE RegisterCimService : public AsyncRequest
{
   public: 
      RegisterCimService(Uint32 routing, 
			 AsyncOpNode *operation,
			 Boolean blocking,
			 String service_name,
			 Uint32 service_capabilities, 
			 Uint32 service_mask,
			 Uint32 service_queue)
	 : AsyncRequest( async_messages::REGISTER_CIM_SERVICE,
			 Message::getNextKey(),
			 routing, 0, operation, CIMOM_Q_ID,
			 service_queue, blocking),
	   name(service_name),
	   capabilities(service_capabilities),
	   mask(service_mask),
	   queue(service_queue) {   }

      virtual ~RegisterCimService(void);
      
      String name;
      Uint32 capabilities;
      Uint32 mask;
      Uint32 queue;
};

class PEGASUS_CIMOM_LINKAGE DeRegisterCimService : public AsyncRequest
{
   public:
      DeRegisterCimService(Uint32 routing, 
			   AsyncOpNode *operation,
			   Boolean blocking, 
			   Uint32 service_queue)
	 : AsyncRequest( async_messages::DEREGISTER_CIM_SERVICE,
			 Message::getNextKey(), 
			 routing, 0, operation, CIMOM_Q_ID, 
			 service_queue, blocking),
	   queue(service_queue) {   }

      virtual ~DeRegisterCimService(void);

      Uint32 queue;
} ;

class PEGASUS_CIMOM_LINKAGE UpdateCimService : public AsyncRequest
{
   public:
      UpdateCimService(Uint32 routing, 
		       AsyncOpNode *operation,
		       Boolean blocking, 
		       Uint32 service_queue, 
		       Uint32 service_capabilities, 
		       Uint32 service_mask) 
	 : AsyncRequest( async_messages::UPDATE_CIM_SERVICE, 
			 Message::getNextKey(), 
			 routing, 0, operation, CIMOM_Q_ID, 
			 service_queue, blocking),
	   queue(service_queue),
	   capabilities(service_capabilities),
	   mask(service_mask) {   }

      virtual ~UpdateCimService(void);

      Uint32 queue;
      Uint32 capabilities;
      Uint32 mask;

      
};


class PEGASUS_CIMOM_LINKAGE AsyncIoctl : public AsyncRequest
{
   public:
      AsyncIoctl(Uint32 routing, 
		 AsyncOpNode *operation, 
		 Uint32 destination, 
		 Uint32 response,
		 Boolean blocking,
		 Uint32 code, 
		 Uint32 int_param,
		 void *p_param)
	 : AsyncRequest( async_messages::IOCTL, 
			 Message::getNextKey(), 
			 routing, 0, operation, 
			 destination, response, blocking),
	   ctl(code), 
	   intp(int_param),
	   voidp(p_param) {  }
      
      virtual ~AsyncIoctl(void);
      
      Uint32 ctl;
      Uint32 intp;
      void *voidp;
};

class PEGASUS_CIMOM_LINKAGE CimServiceStart : public AsyncRequest
{
   public:
      CimServiceStart(Uint32 routing, 
		      AsyncOpNode *operation, 
		      Uint32 destination, 
		      Uint32 response, 
		      Boolean blocking)
	 : AsyncRequest(async_messages::CIMSERVICE_START,
			Message::getNextKey(), routing, 
			0, operation, destination, 
			response, blocking) {  }

      virtual ~CimServiceStart(void);
      
};


class PEGASUS_CIMOM_LINKAGE CimServiceStop : public AsyncRequest
{
   public:
      CimServiceStop(Uint32 routing, 
		     AsyncOpNode *operation, 
		     Uint32 destination, 
		     Uint32 response, 
		     Boolean blocking)
	 : AsyncRequest(async_messages::CIMSERVICE_STOP,
			Message::getNextKey(), routing, 
			0, operation, destination, 
			response, blocking) {  }
      
      virtual ~CimServiceStop(void);
};

class PEGASUS_CIMOM_LINKAGE CimServicePause : public AsyncRequest
{
   public:
      CimServicePause(Uint32 routing, 
		      AsyncOpNode *operation, 
		      Uint32 destination, 
		      Uint32 response, 
		      Boolean blocking)
	 : AsyncRequest(async_messages::CIMSERVICE_PAUSE,
			Message::getNextKey(), routing, 
			0, operation, destination, 
			response, blocking) {  }
      
      virtual ~CimServicePause(void);
};

class PEGASUS_CIMOM_LINKAGE CimServiceResume : public AsyncRequest
{
   public:
      CimServiceResume(Uint32 routing, 
		       AsyncOpNode *operation, 
		       Uint32 destination, 
		       Uint32 response, 
		       Boolean blocking)
	 : AsyncRequest(async_messages::CIMSERVICE_RESUME,
			Message::getNextKey(), routing, 
			0, operation, destination, 
			response, blocking) {  }
      
      virtual ~CimServiceResume(void);
};

class PEGASUS_CIMOM_LINKAGE AsyncOperationStart : public AsyncRequest
{
   public:
      AsyncOperationStart(Uint32 routing, 
			  AsyncOpNode *operation, 
			  Uint32 destination, 
			  Uint32 response, 
			  Boolean blocking, 
			  Message *action)
	 : AsyncRequest(async_messages::ASYNC_OP_START, 
			Message::getNextKey(), routing, 0,
			operation, 
			destination, response, blocking),
	   act(action) 
      {  
	 op->put_request(act);
      }
      
      virtual ~AsyncOperationStart(void);
      
      Message *act;
};

class PEGASUS_CIMOM_LINKAGE AsyncOperationResult : public AsyncReply
{
   public:
      AsyncOperationResult(Uint32 key, 
			   Uint32 routing, 
			   AsyncOpNode *operation,
			   Uint32 result_code, 
			   Uint32 destination,
			   Uint32 blocking)
	 : AsyncReply(async_messages::ASYNC_OP_RESULT, 
		      key, routing, 0,
		      operation, result_code, destination, 
		      blocking) {   }
      virtual ~AsyncOperationResult(void);
      
};


class PEGASUS_CIMOM_LINKAGE AsyncLegacyOperationStart : public AsyncRequest
{
   public:
      AsyncLegacyOperationStart(Uint32 routing, 
				AsyncOpNode *operation, 
				Uint32 destination, 
				Message *action)
	 : AsyncRequest(async_messages::ASYNC_LEGACY_OP_START, 
			Message::getNextKey(), routing, 0,
			operation, destination, CIMOM_Q_ID, false),
	   act(action) 
      {  
	 op->put_request(act);
      }
      
      virtual ~AsyncLegacyOperationStart(void);
      
      Message *act;
};

class PEGASUS_CIMOM_LINKAGE AsyncLegacyOperationResult : public AsyncReply
{
   public:
      AsyncLegacyOperationResult(Uint32 key, 
				 Uint32 routing, 
				 AsyncOpNode *operation,
				 Message *result)
	 : AsyncReply(async_messages::ASYNC_LEGACY_OP_RESULT, 
		      key, routing, 0, operation, 
		      0, CIMOM_Q_ID, false),
	   res(result)
      {   
	 op->put_response(res);
      }

      virtual ~AsyncLegacyOperationResult(void);

      Message *res;
};


class PEGASUS_CIMOM_LINKAGE FindServiceQueue : public AsyncRequest
{
   public:
      FindServiceQueue(Uint32 routing, 
		       AsyncOpNode *operation, 
		       Uint32 response,
		       Boolean blocking, 
		       String service_name, 
		       Uint32 service_capabilities, 
		       Uint32 service_mask)
	 : AsyncRequest(async_messages::FIND_SERVICE_Q, 
			Message::getNextKey(),
			routing, 0, operation, 
			CIMOM_Q_ID, 
			response, 
			blocking),
	   name(service_name),
	   capabilities(service_capabilities),
	   mask(service_mask) { }
      
      virtual ~FindServiceQueue(void);
      
      String name;
      Uint32 capabilities;
      Uint32 mask;
		       
} ;

class PEGASUS_CIMOM_LINKAGE FindServiceQueueResult : public AsyncReply
{
   public:
      FindServiceQueueResult(Uint32 key, 
			     Uint32 routing, 
			     AsyncOpNode *operation, 
			     Uint32 result_code, 
			     Uint32 destination, 
			     Boolean blocking, 
			     Array<Uint32> queue_ids)
	 : AsyncReply(async_messages::FIND_SERVICE_Q_RESULT, 
		      key, routing, 0, operation, 
		      result_code, destination, blocking),
	   qids(queue_ids) {  }

      virtual ~FindServiceQueueResult(void);
      
      Array<Uint32> qids;
} ;

class PEGASUS_CIMOM_LINKAGE EnumerateService : public AsyncRequest
{
   public:
      EnumerateService(Uint32 routing, 
		       AsyncOpNode *operation, 
		       Uint32 response, 
		       Boolean blocking, 
		       Uint32 queue_id)
	 : AsyncRequest(async_messages::ENUMERATE_SERVICE, 
			Message::getNextKey(),
			routing, 0, 
			operation, 
			CIMOM_Q_ID, 
			response, 
			blocking),
	   qid(queue_id) {  }
      
      virtual ~EnumerateService(void);
      
      Uint32 qid;
};

class PEGASUS_CIMOM_LINKAGE EnumerateServiceResponse : public AsyncReply
{
   public:
      EnumerateServiceResponse(Uint32 key, 
			       Uint32 routing, 
			       AsyncOpNode *operation, 
			       Uint32 result_code, 
			       Uint32 response, 
			       Boolean blocking,
			       String service_name, 
			       Uint32 service_capabilities, 
			       Uint32 service_mask, 
			       Uint32 service_qid)
	 : AsyncReply(async_messages::ENUMERATE_SERVICE_RESULT, 
		      key, 
		      routing, 0,
		      operation,
		      result_code,
		      response, 
		      blocking),
	   name(service_name),
	   capabilities(service_capabilities),
	   mask(service_mask),
	   qid(service_qid) {  }
      
      virtual ~EnumerateServiceResponse(void);
      
      String name;
      Uint32 capabilities;
      Uint32 mask;
      Uint32 qid;
};


PEGASUS_NAMESPACE_END

#endif // CIMOM_MESSAGE_include
