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

#ifndef CIMOM_MESSAGE_include
#define CIMOM_MESSAGE_include

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/MessageQueue.h>
#include <Pegasus/Common/AsyncOpNode.h>


PEGASUS_NAMESPACE_BEGIN

class AsyncOpNode;

enum cimom_results
{
   OK,
   PARAMETER_ERROR,
   MODULE_ALREADY_REGISTERED,
   MODULE_NOT_FOUND,
   INTERNAL_ERROR,

   NUMBER_RESULTS
};

enum cimom_messages
{
   HEARTBEAT,
   MODULE_REGISTER,
   MODULE_DEREGISTER,
   ASYNC_OP,
   
   NUMBER_MESSAGES
};


class PEGASUS_CIMOM_LINKAGE CimomRequest : public Message
{
   public:
      CimomRequest(Uint32 type, 
		   Uint32 key,
		   QueueIdStack queue_ids = QueueIdStack(MessageQueue::_CIMOM_Q_ID),
		   Uint32 mask = message_mask::type_cimom | message_mask::type_request)
	 : Message(type, key, mask), queues(queue_ids) {   }
      
      virtual ~CimomRequest(void);
      QueueIdStack queues;
} ;

class PEGASUS_CIMOM_LINKAGE CimomReply : public Message
{
   public:
      CimomReply(Uint32 type,
		 Uint32 key, 
		 Uint32 result_code,
		 QueueIdStack queue_ids,
		 Uint32 mask = message_mask::type_cimom | message_mask::type_reply)
	 : Message(type, key, mask), result(result_code), queues(queue_ids)  {   }

      virtual ~CimomReply(void);
      
      Uint32 result;
      QueueIdStack queues;
};


class PEGASUS_CIMOM_LINKAGE ModuleRegister : public CimomRequest 
{
   public:
      ModuleRegister(Uint32 key, 
		     QueueIdStack queue_ids,
		     const String & module_name,
		     Uint32 module_capabilities,
		     Uint32 module_messages,
		     Uint32 module_queue)
	 : CimomRequest(MODULE_REGISTER, key, queue_ids, 
			message_mask::type_cimom | 
			message_mask::type_request | 
			message_mask::type_control),
	   name(module_name), capabilities(module_capabilities),
	   msg_mask( ), q_id(module_queue)  {   }
      
      virtual ~ModuleRegister(void);
      
      String name;
      Uint32 capabilities;
      Uint32 msg_mask;
      Uint32 q_id;
};


class PEGASUS_CIMOM_LINKAGE ModuleDeregister : public CimomRequest
{
   public:
      ModuleDeregister(Uint32 key,
		       QueueIdStack queue_ids, 
		       Uint32 module_queue)
	 : CimomRequest(MODULE_DEREGISTER, key, queue_ids, 
			message_mask::type_cimom | 
			message_mask::type_request | 
			message_mask::type_control),
	   q_id(module_queue) {  }
      virtual ~ModuleDeregister();
      

      Uint32 q_id;
} ;



class PEGASUS_CIMOM_LINKAGE AsyncDispatch : public CimomRequest
{
   public:
      AsyncDispatch(Uint32 type,
		    Uint32 key, 
		    Uint32 mask,
		    AsyncOpNode *operation)
	 :CimomRequest(ASYNC_OP, key, 
		       QueueIdStack(MessageQueue::_CIMOM_Q_ID)),
	  op(operation) {  }
      
      AsyncOpNode *op;
};




PEGASUS_NAMESPACE_END

#endif // CIMOM_MESSAGE_include
