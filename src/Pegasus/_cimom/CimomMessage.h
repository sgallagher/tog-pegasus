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
#include <Pegasus/Common/Message.h>

PEGASUS_NAMESPACE_BEGIN

class AsyncOpNode;

class PEGASUS_EXPORT CimomMessage : public Message
{
   public:
      CimomMessage(Uint32 type, Uint32 id )
	 : Message(type), message_id(id) { }

      virtual ~CimomMessage();
      
      Uint32 message_id;
};


// used to register a new module with the cimom
// async_control points to the async ioctl routine (like a device driver)
// result_callback is a direct callback to the module to record the result
// of the operation. the result parameter contains the message queue id that
// the new module can use to receive messages. 

class PEGASUS_EXPORT CimomRegistration : public Message
{
   public: CimomRegistration(Uint32 type, 
			     Uint32 id,
			     const String& module_name,
			     Uint32 capabilities
			     Uint32 messages,
			     void (*async_control)(AsyncOpNode *op),
			     void (*result_callback)(Uint32 result)) 
      : CimomMessage(type, id), module(module_name),
	capabilities_mask(capabilities), 
	message_mask(messages), control(async_control), 
	result(result_callback) {  }
      
      virtual ~CimomRegistration(void);

      String module;
      Uint32 capabilities_mask;
      Uint32 message_mask;
      void (*control)(AsyncOpNode *op);
      void (*result)(Uint32 result);
} ;


class PEGASUS_EXPORT CimomRequest : public CimomMessage
{
   public:
      CimomRequest(Uint32 type, Uint32 id, QueueIdStack queues)
	 : CimomMessage(type, id), queue_ids(queues) { }
      
      virtual ~CimomRequest(void);
      
      QueueIdStack queue_ids;
}


class PEGASUS_EXPORT CimomReply : public CimomMessage
{
   public :
      CimomReply(Uint32 type, 
		 Uint32 id, 
		 Uint32 status)
	 : CimomMessage(type, id), error_status(status) { }
      
      virtual ~CimomReply(void);
      
      Uint32 error_status;
};

class PEGASUS_EXPORT CimomGetKnownModulesRequest : public CimomRequest
{
   public:
      CimomGetKnownModulesRequest(Uint32 type, 
				  Uint32 id, 
				  QueueIdStack queues,
				  const String& module_name,
				  Uint32 module_capabilities,
				  Uint32 module_messages)
	 : CimomRequest(type, id, queues), module(module_name),
	   capabilities(module_capabilities), 
	   messages(module_messages) { }

      virtual ~CimomGetKnownModulesRequest(void);
      
      String module;
      Uint32 capabilties;
      Uint32 messages
	 
}


class PEGASUS_EXPORT CimomGetKnownModulesReply : public CimomReply
{
   public:
      CimomGetKnownModulesReply(Uint32 type, 
				Uint32 id, 
				Uint32 status,
				const Array<String>& module_names,
				const Array<Uint32>& module_capabilities,
				const Array<Uint32>& module_messages )
	 : names(module_names), capabilities(module_capabilities),
	   messages(module_messages) { }

      virtual ~CimomGetKnownModulesReply(void);
      
      Array<String> names;
      Array<Uint32> capabilities;
      Array<Uint32> messages;
};



PEGASUS_NAMESPACE_END

#endif // CIMOM_MESSAGE_include
