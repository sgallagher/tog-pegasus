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


#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/IPC.h>
#include <Pegasus/Common/DQueue.h>
#include <Pegasus/Common/Thread.h>
#include <Pegasus/_cimom/Cimom.h>
#include "MessageQueueService.h"

#include <sys/types.h>
#if defined(PEGASUS_PLATFORM_WIN32_IX86_MSVC)
#else
#include <unistd.h>
#endif 
#include <cassert>
#include <iostream>
#include <stdio.h>
#include <string.h>


PEGASUS_NAMESPACE_BEGIN




void MessageQueueServer::handleEnqueue()
{
   Message *msg = dequeue();
   if ( msg == 0 )
      return;
   
   if( msg->getMask() & message_mask::ha_async )
   {
      handle_test_request(static_cast<AsyncMessage *>(msg));
   }
   else
      delete msg;
}

Boolean MessageQueueServer::messageOK(const Message *msg)
{
   if(msg->getMask() & message_mask::ha_async)
   {
      if(msg->getType() == 0x04100000)
	 return true;
   }
   return false;
}

void MessageQueueServer::handle_test_request(AsyncMessage *msg)
{
   if( msg->getType() == 0x04100000 )
   {
      test_response *resp = 
	 new test_response(msg->getKey(),
			   msg->getRouting(),
			   msg->op, 
			   async_results::OK, 
			   msg->dest, 
			   msg->greeting);
      _enqueueAsyncResponse(msg, resp, ASYNC_OPSTATE_PROCESSING, 0);
   }
   
}


Boolean MessageQueueClient::handleEnqueue()
{
   Message *msg = dequeue();
   if ( msg == 0 )
      return;
   
   if( msg->getMask() & message_mask::ha_async )
   {
      if(msg->getType() == 0x04200000)
      {
	 msg_count++;
      }
   }
   else
      delete msg;
}


Boolean MessageQueueClient::messageOK(const Message *msg)
{
   if(msg->getMask() & message_mask::ha_async)
   {
      if(msg->getType() == 0x04200000)
	 return true;
   }
   return false;
}


void MessageQueueClient::send_test_request(String *greeting, Uint32 qid)
{
   test_request *req = 
      new test_request(get_next_xid(),
		       get_op(),
		       qid, 
		       _queueId,
		       String("message queue service test client "));

   unlocked_dq<AsyncMessage> replies(true);
   SendWait(test_request, replies);
   while( replies.count() )
   { 
      delete static_cast<test_response *>(replies.remove_first());
   }
}


PEGASUS_NAMESPACE_END

PEGASUS_USING_STD;
PEGASUS_USING_PEGASUS;

AtomicInt msg_count;

PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL client_func(void *parm);
PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL server_func(void *parm);




int main(int argc, char **argv)
{
   Thread client(client_func, (void *)&msg_count, false);
   Thread server(server_func, (void *)&msg_count, false);
   
   cimom Q_server();
   
   client->run();
   server->run();
   while( msg_count.value() < 1000 )
   {
      pegasus_sleep(1);
   }
   client->join();
   server->join();
   
   return(1);
}


PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL client_func(void *parm)
{
   Thread *my_handle = reinterpret_cast<Thread *>(parm);
   AtomicInt & count = *(reinterpret_cast<AtomicInt *>(my_handle->get_parm()));
   
   MessageQueueClient q_client("test client");
   q_client.register_service("test client", _capabilities, _mask);
   
   Uint32 test_server = 

   
}

