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

#include "CimomMessage.h"

PEGASUS_NAMESPACE_BEGIN



const Uint32 async_results::OK =                        0x00000000;
const Uint32 async_results::PARAMETER_ERROR =           0x00000001;
const Uint32 async_results::MODULE_ALREADY_REGISTERED = 0x00000002;
const Uint32 async_results::MODULE_NOT_FOUND =          0x00000003;
const Uint32 async_results::INTERNAL_ERROR =            0x00000004;


const Uint32 async_results::ASYNC_STARTED =             0x00000005;
const Uint32 async_results::ASYNC_PROCESSING =          0x00000006;
const Uint32 async_results::ASYNC_COMPLETE =            0x00000007;
const Uint32 async_results::ASYNC_CANCELLED =           0x00000008;
const Uint32 async_results::ASYNC_PAUSED =              0x00000009;
const Uint32 async_results::ASYNC_RESUMED =             0x0000000a;

const Uint32 async_results::CIM_SERVICE_STARTED =       0x0000000b;
const Uint32 async_results::CIM_SERVICE_STOPPED  =      0x0000000c;

const Uint32 async_results::CIM_SERVICE_PAUSED  =       0x0000000d;
const Uint32 async_results::CIM_SERVICE_RESUMED =       0x0000000e;
const Uint32 async_results::CIM_NAK =                   0x0000000f;

const Uint32 async_results::ASYNC_PHASE_COMPLETE =      0x00000010;
const Uint32 async_results::ASYNC_CHILD_COMPLETE =      0x00000011;
const Uint32 async_results::ASYNC_PHASE_STARTED =       0x00000012;
const Uint32 async_results::ASYNC_CHILD_STARTED =       0x00000013;
const Uint32 async_results::CIM_PAUSED =                0x00000014;
const Uint32 async_results::CIM_STOPPED =               0x00000015;


const Uint32 async_messages::HEARTBEAT =                DUMMY_MESSAGE;
const Uint32 async_messages::REPLY =                    DUMMY_MESSAGE;
const Uint32 async_messages::REGISTER_CIM_SERVICE =     ASYNC_REGISTER_CIM_SERVICE;
const Uint32 async_messages::DEREGISTER_CIM_SERVICE =   ASYNC_DEREGISTER_CIM_SERVICE;
const Uint32 async_messages::UPDATE_CIM_SERVICE =       ASYNC_UPDATE_CIM_SERVICE;
const Uint32 async_messages::IOCTL =                    ASYNC_IOCTL;
const Uint32 async_messages::CIMSERVICE_START =         ASYNC_CIMSERVICE_START;
const Uint32 async_messages::CIMSERVICE_STOP =          ASYNC_CIMSERVICE_STOP;
const Uint32 async_messages::CIMSERVICE_PAUSE =         ASYNC_CIMSERVICE_PAUSE;
const Uint32 async_messages::CIMSERVICE_RESUME =        ASYNC_CIMSERVICE_RESUME;

const Uint32 async_messages::ASYNC_OP_START =           ASYNC_ASYNC_OP_START;
const Uint32 async_messages::ASYNC_OP_RESULT =          ASYNC_ASYNC_OP_RESULT;
const Uint32 async_messages::ASYNC_LEGACY_OP_START =    ASYNC_ASYNC_LEGACY_OP_START;
const Uint32 async_messages::ASYNC_LEGACY_OP_RESULT =   ASYNC_ASYNC_LEGACY_OP_RESULT;

const Uint32 async_messages::FIND_SERVICE_Q =           ASYNC_FIND_SERVICE_Q;
const Uint32 async_messages::FIND_SERVICE_Q_RESULT =    ASYNC_FIND_SERVICE_Q_RESULT;
const Uint32 async_messages::ENUMERATE_SERVICE =        ASYNC_ENUMERATE_SERVICE;
const Uint32 async_messages::ENUMERATE_SERVICE_RESULT = ASYNC_ENUMERATE_SERVICE_RESULT;

const Uint32 async_messages::REGISTERED_MODULE =        ASYNC_REGISTERED_MODULE;
const Uint32 async_messages::DEREGISTERED_MODULE =      ASYNC_DEREGISTERED_MODULE;
const Uint32 async_messages::FIND_MODULE_IN_SERVICE =   ASYNC_FIND_MODULE_IN_SERVICE;
const Uint32 async_messages::FIND_MODULE_IN_SERVICE_RESPONSE = ASYNC_FIND_MODULE_IN_SERVICE_RESPONSE;
const Uint32 async_messages::ASYNC_MODULE_OP_START =    ASYNC_ASYNC_MODULE_OP_START;
const Uint32 async_messages::ASYNC_MODULE_OP_RESULT  =  ASYNC_ASYNC_MODULE_OP_RESULT;



AsyncMessage::AsyncMessage(Uint32 type, 
			   Uint32 destination,
			   Uint32 key, 
			   Uint32 routing,
			   Uint32 mask,
			   AsyncOpNode *operation)
   : Message(type, destination, key, routing, mask | message_mask::ha_async),
     op(operation)
{  
   
}

AsyncRequest::AsyncRequest(Uint32 type, 
			   Uint32 key, 
			   Uint32 routing,
			   Uint32 mask,
			   AsyncOpNode *operation,
			   Uint32 destination,
			   Uint32 response,
			   Boolean blocking) 
   : AsyncMessage(type, destination, key, routing, mask | message_mask::ha_request, operation),
     resp(response),
     block(blocking) 
{  
   if( op != 0 )
      op->put_request(this);
}

AsyncReply::AsyncReply(Uint32 type, 
		       Uint32 key, 
		       Uint32 routing, 
		       Uint32 mask,
		       AsyncOpNode *operation,
		       Uint32 result_code,
		       Uint32 destination,
		       Boolean blocking) 
   : AsyncMessage(type, destination, key, routing, mask | message_mask::ha_reply, operation),
     result(result_code),
     block(blocking) 
{  
   if( op != 0 )
      op->put_response(this);
}




RegisterCimService::RegisterCimService(Uint32 routing, 
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
     queue(service_queue) 
{   
   
}
      

DeRegisterCimService::DeRegisterCimService(Uint32 routing, 
					   AsyncOpNode *operation,
					   Boolean blocking, 
					   Uint32 service_queue)
   : AsyncRequest( async_messages::DEREGISTER_CIM_SERVICE,
		   Message::getNextKey(), 
		   routing, 0, operation, CIMOM_Q_ID, 
		   service_queue, blocking),
     queue(service_queue) 
{   
	 
}



UpdateCimService::UpdateCimService(Uint32 routing, 
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
     mask(service_mask) 
{   
   
}

RegisteredModule::RegisteredModule(Uint32 routing, 
				   AsyncOpNode *operation,
				   Boolean blocking, 
				   Uint32 service_queue, 
				   String new_module)
   : AsyncRequest( async_messages::REGISTERED_MODULE, 
		   Message::getNextKey(),
		   routing, 
		   0, 
		   operation, 
		   CIMOM_Q_ID, 
		   service_queue, 
		   blocking),
     _module(new_module)
{

}

DeRegisteredModule::DeRegisteredModule(Uint32 routing, 
				     AsyncOpNode *operation,
				     Boolean blocking, 
				     Uint32 service_queue, 
				     String removed_module)
   : AsyncRequest( async_messages::DEREGISTERED_MODULE, 
		   Message::getNextKey(),
		   routing, 
		   0, 
		   operation, 
		   CIMOM_Q_ID, 
		   service_queue, 
		   blocking),
     _module(removed_module)
{

}


FindModuleInService::FindModuleInService(Uint32 routing, 
					 AsyncOpNode *operation, 
					 Boolean blocking,
					 Uint32 response_queue,
					 String module)
   : AsyncRequest(async_messages::FIND_MODULE_IN_SERVICE, 
		  Message::getNextKey(),
		  routing, 
		  0, 
		  operation, 
		  CIMOM_Q_ID,
		  response_queue,
		  blocking),
     _module(module)
{

}

FindModuleInServiceResponse::FindModuleInServiceResponse(Uint32 routing,
							 Uint32 key,
							 AsyncOpNode *operation,
							 Uint32 result_code, 
							 Uint32 destination, 
							 Uint32 blocking, 
							 Uint32 module_service_queue)

   : AsyncReply(async_messages::FIND_MODULE_IN_SERVICE_RESPONSE,
		key, 
		routing, 
		0, 
		operation, 
		result_code,
		destination, 
		blocking),
     _module_service_queue(module_service_queue)
{

}

AsyncIoctl::AsyncIoctl(Uint32 routing, 
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
     voidp(p_param)
{  
	 
}


CimServiceStart::CimServiceStart(Uint32 routing, 
				 AsyncOpNode *operation, 
				 Uint32 destination, 
				 Uint32 response, 
				 Boolean blocking)
   : AsyncRequest(async_messages::CIMSERVICE_START,
		  Message::getNextKey(), routing, 
		  0, operation, destination, 
		  response, blocking) 
{  
	 
}


CimServiceStop::CimServiceStop(Uint32 routing, 
			       AsyncOpNode *operation, 
			       Uint32 destination, 
			       Uint32 response, 
			       Boolean blocking)
   : AsyncRequest(async_messages::CIMSERVICE_STOP,
		  Message::getNextKey(), routing, 
		  0, operation, destination, 
		  response, blocking) 
{  

}



CimServicePause::CimServicePause(Uint32 routing, 
				 AsyncOpNode *operation, 
				 Uint32 destination, 
				 Uint32 response, 
				 Boolean blocking)
   : AsyncRequest(async_messages::CIMSERVICE_PAUSE,
		  Message::getNextKey(), routing, 
		  0, operation, destination, 
		  response, blocking) 
{  
	 
}


CimServiceResume::CimServiceResume(Uint32 routing, 
				   AsyncOpNode *operation, 
				   Uint32 destination, 
				   Uint32 response, 
				   Boolean blocking)
   : AsyncRequest(async_messages::CIMSERVICE_RESUME,
		  Message::getNextKey(), routing, 
		  0, operation, destination, 
		  response, blocking) 
{  
	 
}

AsyncOperationStart::AsyncOperationStart(Uint32 routing, 
					 AsyncOpNode *operation, 
					 Uint32 destination, 
					 Uint32 response, 
					 Boolean blocking, 
					 Message *action)
   : AsyncRequest(async_messages::ASYNC_OP_START, 
		  Message::getNextKey(), routing, 0,
		  operation, 
		  destination, response, blocking),
     _act(action) 
{  

}


Message * AsyncOperationStart::get_action(void)
{
   Message *ret = _act;
   _act = 0;
   ret->put_async(0);
   return ret;
   
}


AsyncOperationResult::AsyncOperationResult(Uint32 key, 
					   Uint32 routing, 
					   AsyncOpNode *operation,
					   Uint32 result_code, 
					   Uint32 destination,
					   Uint32 blocking)
   : AsyncReply(async_messages::ASYNC_OP_RESULT, 
		key, routing, 0,
		operation, result_code, destination, 
		blocking) 
{   
	 
}


AsyncModuleOperationStart::AsyncModuleOperationStart(Uint32 routing, 
						     AsyncOpNode *operation, 
						     Uint32 destination, 
						     Uint32 response, 
						     Boolean blocking, 
						     String target_module,
						     Message *action)
   : AsyncRequest(async_messages::ASYNC_MODULE_OP_START, 
		  Message::getNextKey(), routing, 0,
		  operation, 
		  destination, response, blocking),
     _target_module(target_module),
     _act(action) 
{  
   _act->put_async(this);
   
}


Message * AsyncModuleOperationStart::get_action(void)
{
   Message *ret = _act;
   _act = 0;
   ret->put_async(0);
   return ret;
   
}

AsyncModuleOperationResult::AsyncModuleOperationResult(Uint32 key, 
						       Uint32 routing, 
						       AsyncOpNode *operation,
						       Uint32 result_code, 
						       Uint32 destination,
						       Uint32 blocking,
						       String target_module,
						       Message *result)
   : AsyncReply(async_messages::ASYNC_MODULE_OP_RESULT, 
		key, routing, 0,
		operation, result_code, destination, 
		blocking),
     _target_module(target_module),
     _res(result)
{   
   _res->put_async(this);
}

Message * AsyncModuleOperationResult::get_result(void)
{
   Message *ret = _res;
   _res = 0;
   ret->put_async(0);
   return ret;
}


AsyncLegacyOperationStart::AsyncLegacyOperationStart(Uint32 routing, 
						     AsyncOpNode *operation, 
						     Uint32 destination, 
						     Message *action,
						     Uint32 action_destination)
   : AsyncRequest(async_messages::ASYNC_LEGACY_OP_START, 
		  Message::getNextKey(), routing, 0,
		  operation, destination, CIMOM_Q_ID, false),
     _act(action) , _legacy_destination(action_destination)
{  
   _act->put_async(this);
}


Message * AsyncLegacyOperationStart::get_action(void)
{
   Message *ret = _act;
   _act = 0;
//   ret->put_async(0);
   return ret;
   
}

AsyncLegacyOperationResult::AsyncLegacyOperationResult(Uint32 key, 
						       Uint32 routing, 
						       AsyncOpNode *operation,
						       Message *result)
   : AsyncReply(async_messages::ASYNC_LEGACY_OP_RESULT, 
		key, routing, 0, operation, 
		0, CIMOM_Q_ID, false),
     _res(result)
{   
   _res->put_async(this);
}
      
Message *AsyncLegacyOperationResult::get_result(void)
{
   Message *ret = _res;
   _res = 0;
//   ret->put_async(0);
   return ret;
}

FindServiceQueue::FindServiceQueue(Uint32 routing, 
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
     mask(service_mask) 
{ 
	 
}

 
FindServiceQueueResult::FindServiceQueueResult(Uint32 key, 
					       Uint32 routing, 
					       AsyncOpNode *operation, 
					       Uint32 result_code, 
					       Uint32 destination, 
					       Boolean blocking, 
					       Array<Uint32> queue_ids)
   : AsyncReply(async_messages::FIND_SERVICE_Q_RESULT, 
		key, routing, 0, operation, 
		result_code, destination, blocking),
     qids(queue_ids) 
{  
	 
}

EnumerateService::EnumerateService(Uint32 routing, 
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
     qid(queue_id) 
{  
	 
}

EnumerateServiceResponse::EnumerateServiceResponse(Uint32 key, 
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
     qid(service_qid) 
{
	 
}

AsyncMessage::~AsyncMessage(void) 
{
}

AsyncRequest::~AsyncRequest(void)
{
}

PEGASUS_NAMESPACE_END
