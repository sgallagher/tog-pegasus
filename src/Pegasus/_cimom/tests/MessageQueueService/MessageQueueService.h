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

#ifndef MessageQueueService_test_h
#define MessageQueueService_test_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/MessageQueue.h>
#include <Pegasus/Common/DQueue.h>
#include <Pegasus/Common/Thread.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/AsyncOpNode.h>
#include <Pegasus/_cimom/CimomMessage.h>
#include <Pegasus/_cimom/Cimom.h>
#include <Pegasus/_cimom/MessageQueueService.h>


PEGASUS_NAMESPACE_BEGIN

class test_request : public AsyncRequest
{
  
   public:
      test_request(Uint32 routing, 
		   AsyncOpNode *op, 
		   Uint32 destination, 
		   Uint32 response,
		   char *message)
	 : AsyncRequest(0x04100000,
			Message::getNextKey(), 
			routing,
			0, 
			op, 
			destination, 
			response, 
			true),
	   greeting(message) {   }

      virtual ~test_request(void);
      String greeting;
};

class test_response : public AsyncReply
{
   public:
      test_response(Uint32 key, 
		    Uint32 routing,
		    AsyncOpNode *op, 
		    Uint32 result,
		    Uint32 destination, 
		    char *message)
	 : AsyncReply(0x04200000,
		      key, 
		      routing, 
		      0, 
		      op, 
		      result, 
		      destination,
		      true), 
	   greeting(message) {  }
      
      virtual ~test_response(void);
      String greeting;
};

class MessageQueueServer : public MessageQueueService
{
   public:
      typedef MessageQueueService Base;
      
      MessageQueueServer(const char *name)
	 : Base(name, MessageQueue::getNextQueueId(), 
		0, 
		(message_mask::type_cimom | 
		 message_mask::type_service | 
		 message_mask::ha_request | 
		 message_mask::ha_reply | 
		 message_mask::ha_async )), _die(false) {  }
      
      virtual ~MessageQueueServer(void);
      
      virtual Boolean messageOK(const Message *msg);
      void handle_test_request(AsyncRequest *msg);
      virtual void handle_CimServiceStop(CimServiceStop *req);

   protected:
      virtual void _handle_async_msg(AsyncMessage *msg);
   private:
      Boolean _die;
};


class MessageQueueClient : public MessageQueueService
{
      
   public:
      typedef MessageQueueService Base;
      
      MessageQueueClient(const char *name)
	 : Base(name, MessageQueue::getNextQueueId(), 
		0, 
		(message_mask::type_cimom | 
		 message_mask::type_service | 
		 message_mask::ha_request | 
		 message_mask::ha_reply | 
		 message_mask::ha_async )), 
	   xid(1)
      {  }
      
      virtual ~MessageQueueClient(void);
      
      virtual Boolean messageOK(const Message *msg);

      void send_test_request(char *greeting, Uint32 qid);

      Uint32 _capabilities;
      Uint32 _mask;
      Uint32 get_qid(void) 
      {
	 return _queueId;
      }
      
      
   protected:
      virtual void _handle_async_msg(AsyncMessage *msg);
      AtomicInt xid;
};

PEGASUS_NAMESPACE_END

#endif 


