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
// Author: Mike Day (mdday@us.ibm.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_CimomMessage_h
#define Pegasus_CimomMessage_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/InternalException.h>
#include <Pegasus/Common/MessageQueue.h>
#include <Pegasus/Common/AsyncOpNode.h>
#include <Pegasus/Common/Linkage.h>

PEGASUS_NAMESPACE_BEGIN
//
// This identifier is the queue id for CIMOM queue. It is initialized in
// CimomMessage.cpp by calling MessageQueue::getNextQueueId(). Note that
// this value is passed in the constructor for the CIMOM queue.
//
extern const Uint32 CIMOM_Q_ID;

class AsyncOpNode;

class PEGASUS_COMMON_LINKAGE async_results
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
      static const Uint32 CIM_PAUSED;
      static const Uint32 CIM_STOPPED;
};


class PEGASUS_COMMON_LINKAGE async_messages
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
      static const Uint32 PROVIDERS_STOP;

      static const Uint32 ASYNC_OP_START;
      static const Uint32 ASYNC_OP_RESULT;
      static const Uint32 ASYNC_LEGACY_OP_START;
      static const Uint32 ASYNC_LEGACY_OP_RESULT;

      static const Uint32 FIND_SERVICE_Q;
      static const Uint32 FIND_SERVICE_Q_RESULT;
      static const Uint32 ENUMERATE_SERVICE;
      static const Uint32 ENUMERATE_SERVICE_RESULT;

      static const Uint32 REGISTERED_MODULE;
      static const Uint32 DEREGISTERED_MODULE;
      static const Uint32 FIND_MODULE_IN_SERVICE;
      static const Uint32 FIND_MODULE_IN_SERVICE_RESPONSE;

      static const Uint32 ASYNC_MODULE_OP_START;
      static const Uint32 ASYNC_MODULE_OP_RESULT;
};


class PEGASUS_COMMON_LINKAGE AsyncMessage : public Message
{
   public:
      AsyncMessage(Uint32 type, 
		   Uint32 destination,
		   Uint32 key, 
		   Uint32 routing,
		   Uint32 mask,
		   AsyncOpNode *operation);
           
      virtual ~AsyncMessage(void);
      
      Boolean operator ==(const void *key);
      Boolean operator ==(const AsyncMessage& msg);
      
      AsyncOpNode *op;
//      Thread *_myself;
//      MessageQueue *_service;
};


inline Boolean AsyncMessage::operator ==(const void *key)
{
   if( key == reinterpret_cast<void *>(this))
      return true;
   return false;
}

inline Boolean AsyncMessage::operator ==(const AsyncMessage& msg)
{
   return this->operator==(reinterpret_cast<void *>(const_cast<AsyncMessage *>(&msg)));
}


class PEGASUS_COMMON_LINKAGE AsyncRequest : public AsyncMessage
{
   public:
      AsyncRequest(Uint32 type, 
		   Uint32 key, 
		   Uint32 routing,
		   Uint32 mask,
		   AsyncOpNode *operation,
		   Uint32 destination,
		   Uint32 response,
		   Boolean blocking);
      
      
      virtual ~AsyncRequest(void);
            
      Uint32 resp;
      Boolean block;
};

class PEGASUS_COMMON_LINKAGE AsyncReply : public AsyncMessage
{
   public:
      AsyncReply(Uint32 type, 
		 Uint32 key, 
		 Uint32 routing, 
		 Uint32 mask,
		 AsyncOpNode *operation,
		 Uint32 result_code,
		 Uint32 destination,
		 Boolean blocking);
      
      
      virtual ~AsyncReply(void) { }
            
      Uint32 result;
      Boolean block;
};



class PEGASUS_COMMON_LINKAGE RegisterCimService : public AsyncRequest
{
   public: 
      RegisterCimService(Uint32 routing, 
			 AsyncOpNode *operation,
			 Boolean blocking,
			 String service_name,
			 Uint32 service_capabilities, 
			 Uint32 service_mask,
			 Uint32 service_queue);
      
      virtual ~RegisterCimService(void) 
      {

      }
      
      String name;
      Uint32 capabilities;
      Uint32 mask;
      Uint32 queue;
};

class PEGASUS_COMMON_LINKAGE DeRegisterCimService : public AsyncRequest
{
   public:
      DeRegisterCimService(Uint32 routing, 
			   AsyncOpNode *operation,
			   Boolean blocking, 
			   Uint32 service_queue);
      
      
      virtual ~DeRegisterCimService(void)
      {

      }
      
      Uint32 queue;
} ;

class PEGASUS_COMMON_LINKAGE UpdateCimService : public AsyncRequest
{
   public:
      UpdateCimService(Uint32 routing, 
		       AsyncOpNode *operation,
		       Boolean blocking, 
		       Uint32 service_queue, 
		       Uint32 service_capabilities, 
		       Uint32 service_mask);

      virtual ~UpdateCimService(void) 
      {

      }
      
      Uint32 queue;
      Uint32 capabilities;
      Uint32 mask;
};


class PEGASUS_COMMON_LINKAGE RegisteredModule : public AsyncRequest
{
   public:
      RegisteredModule(Uint32 routing, 
		       AsyncOpNode *operation,
		       Boolean blocking, 
		       Uint32 service_queue,
		       String new_module );

      virtual ~RegisteredModule(void)
      {

      }
      
      String _module;
};


class PEGASUS_COMMON_LINKAGE DeRegisteredModule : public AsyncRequest
{
   public:
      DeRegisteredModule(Uint32 routing, 
		       AsyncOpNode *operation,
		       Boolean blocking, 
		       Uint32 service_queue,
		       String removed_module );

      virtual ~DeRegisteredModule(void)
      {
      }
      
      String _module;
};


class PEGASUS_COMMON_LINKAGE FindModuleInService : public AsyncRequest
{
   public:
      FindModuleInService(Uint32 routing, 
			  AsyncOpNode *operation, 
			  Boolean blocking, 
			  Uint32 response_queue,
			  String module);
      virtual ~FindModuleInService(void)
      {
      }
      
      String _module;
      
};

class PEGASUS_COMMON_LINKAGE FindModuleInServiceResponse : public AsyncReply
{
   public:
      FindModuleInServiceResponse(Uint32 routing, 
				  Uint32 key,
				  AsyncOpNode *operation,
				  Uint32 result_code, 
				  Uint32 destination, 
				  Uint32 blocking, 
				  Uint32 module_service_queue);

      virtual ~FindModuleInServiceResponse(void)
      {
      }
      
      Uint32 _module_service_queue;
};

class PEGASUS_COMMON_LINKAGE AsyncIoctl : public AsyncRequest
{
   public:
      AsyncIoctl(Uint32 routing, 
		 AsyncOpNode *operation, 
		 Uint32 destination, 
		 Uint32 response,
		 Boolean blocking,
		 Uint32 code, 
		 Uint32 int_param,
		 void *p_param);

      virtual ~AsyncIoctl(void)
      {

      }
      
      enum 
      {
	 IO_CLOSE,
	 IO_OPEN,
	 IO_SOURCE_QUENCH,
	 IO_SERVICE_DEFINED, 
	 IO_IDLE_CONTROL
      };
      
      

      Uint32 ctl;
      Uint32 intp;
      void *voidp;

};

class PEGASUS_COMMON_LINKAGE CimServiceStart : public AsyncRequest
{
   public:
      CimServiceStart(Uint32 routing, 
		      AsyncOpNode *operation, 
		      Uint32 destination, 
		      Uint32 response, 
		      Boolean blocking);
      
      virtual ~CimServiceStart(void) 
      {
	 
      }
};


class PEGASUS_COMMON_LINKAGE CimServiceStop : public AsyncRequest
{
   public:
      CimServiceStop(Uint32 routing, 
		     AsyncOpNode *operation, 
		     Uint32 destination, 
		     Uint32 response, 
		     Boolean blocking);
            
      virtual ~CimServiceStop(void) 
      {

      }
};

class PEGASUS_COMMON_LINKAGE CimServicePause : public AsyncRequest
{
   public:
      CimServicePause(Uint32 routing, 
		      AsyncOpNode *operation, 
		      Uint32 destination, 
		      Uint32 response, 
		      Boolean blocking);
      
      
      virtual ~CimServicePause(void)
      {

      }
};

class PEGASUS_COMMON_LINKAGE CimServiceResume : public AsyncRequest
{
   public:
      CimServiceResume(Uint32 routing, 
		       AsyncOpNode *operation, 
		       Uint32 destination, 
		       Uint32 response, 
		       Boolean blocking);
      
      
      virtual ~CimServiceResume(void)
      {

      }
};

class PEGASUS_COMMON_LINKAGE CimProvidersStop : public AsyncRequest
{
   public:
      CimProvidersStop(Uint32 routing, 
		       AsyncOpNode *operation, 
		       Uint32 destination, 
		       Uint32 response, 
		       Boolean blocking);
            
      virtual ~CimProvidersStop(void) 
      {

      }
};

class PEGASUS_COMMON_LINKAGE AsyncOperationStart : public AsyncRequest
{
   public:
      AsyncOperationStart(Uint32 routing, 
			  AsyncOpNode *operation, 
			  Uint32 destination, 
			  Uint32 response, 
			  Boolean blocking, 
			  Message *action);
      

      virtual ~AsyncOperationStart(void)
      {
	 delete _act;
      }
      

      Message *get_action(void ) ;
      
      
   private:
      friend class MessageQueueService;
      friend class cimom;
      Message *_act;
};

class PEGASUS_COMMON_LINKAGE AsyncOperationResult : public AsyncReply
{
   public:
      AsyncOperationResult(Uint32 key, 
			   Uint32 routing, 
			   AsyncOpNode *operation,
			   Uint32 result_code, 
			   Uint32 destination,
			   Uint32 blocking);
      

      virtual ~AsyncOperationResult(void)
      {

      }
};


class PEGASUS_COMMON_LINKAGE AsyncModuleOperationStart : public AsyncRequest
{
   public:
      AsyncModuleOperationStart(Uint32 routing, 
				AsyncOpNode *operation,
				Uint32 destination, 
				Uint32 response, 
				Boolean blocking, 
				String target_module,
				Message *action);

      virtual ~AsyncModuleOperationStart(void) 
      {
	 delete _act;
      }
      
      Message *get_action(void);

   private:
      friend class MessageQueueService;
      friend class cimom;
      friend class ModuleController;
      String _target_module;
      Message *_act;
};

class PEGASUS_COMMON_LINKAGE AsyncModuleOperationResult : public AsyncReply
{
   public:
      AsyncModuleOperationResult(Uint32 key, 
				 Uint32 routing, 
				 AsyncOpNode *operation,
				 Uint32 result_code, 
				 Uint32 destination,
				 Uint32 blocking, 
				 String target_module, 
				 Message *action);

      virtual ~AsyncModuleOperationResult(void)
      {
	 delete _res;
      }
      
      Message *get_result(void);
   private:
      friend class MessageQueueService;
      friend class cimom;
      friend class ModuleController;
      String _target_module;
      Message *_res;
};




class PEGASUS_COMMON_LINKAGE AsyncLegacyOperationStart : public AsyncRequest
{
   public:
      AsyncLegacyOperationStart(Uint32 routing, 
				AsyncOpNode *operation, 
				Uint32 destination, 
				Message *action, 
				Uint32 action_destination);
      
      
      virtual ~AsyncLegacyOperationStart(void)
      {
	 delete _act;
      }

      Message *get_action(void);

   private:
      friend class MessageQueueService;
      friend class cimom;
      Message *_act;
      Uint32 _legacy_destination;
      
};

class PEGASUS_COMMON_LINKAGE AsyncLegacyOperationResult : public AsyncReply
{
   public:
      AsyncLegacyOperationResult(Uint32 key, 
				 Uint32 routing, 
				 AsyncOpNode *operation,
				 Message *result);
      
      virtual ~AsyncLegacyOperationResult(void)
      {
	 delete _res;
      }

      Message *get_result(void);
      

   private:
      friend class MessageQueueService;
      friend class cimom;
      Message *_res;
};


class PEGASUS_COMMON_LINKAGE FindServiceQueue : public AsyncRequest
{
   public:
      FindServiceQueue(Uint32 routing, 
		       AsyncOpNode *operation, 
		       Uint32 response,
		       Boolean blocking, 
		       String service_name, 
		       Uint32 service_capabilities, 
		       Uint32 service_mask);
      
      virtual ~FindServiceQueue(void)
      {

      }
      
      String name;
      Uint32 capabilities;
      Uint32 mask;
} ;

class PEGASUS_COMMON_LINKAGE FindServiceQueueResult : public AsyncReply
{
   public:
      FindServiceQueueResult(Uint32 key, 
			     Uint32 routing, 
			     AsyncOpNode *operation, 
			     Uint32 result_code, 
			     Uint32 destination, 
			     Boolean blocking, 
			     Array<Uint32> queue_ids);
      
      
      virtual ~FindServiceQueueResult(void)
      {

      }
      
      Array<Uint32> qids;
} ;

class PEGASUS_COMMON_LINKAGE EnumerateService : public AsyncRequest
{
   public:
      EnumerateService(Uint32 routing, 
		       AsyncOpNode *operation, 
		       Uint32 response, 
		       Boolean blocking, 
		       Uint32 queue_id);
      
      
      virtual ~EnumerateService(void)
      {

      }
      
      Uint32 qid;
};

class PEGASUS_COMMON_LINKAGE EnumerateServiceResponse : public AsyncReply
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
			       Uint32 service_qid);
      
      
      virtual ~EnumerateServiceResponse(void)
      {

      }
      
      String name;
      Uint32 capabilities;
      Uint32 mask;
      Uint32 qid;
};


PEGASUS_NAMESPACE_END

#endif // CIMOM_MESSAGE_include
