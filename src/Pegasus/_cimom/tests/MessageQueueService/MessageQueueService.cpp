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


#include <sys/types.h>
#if defined(PEGASUS_PLATFORM_WIN32_IX86_MSVC)
#else
#include <unistd.h>
#endif 
#include <cassert>
#include <iostream>
#include <stdio.h>
#include <string.h>


#include "MessageQueueService.h"

PEGASUS_USING_STD;
PEGASUS_USING_PEGASUS;

AtomicInt msg_count;

void MessageQueueServer::_handle_async_msg(AsyncMessage *msg)
{
   if (msg->getType() == 0x04100000 )
   {
      msg->op->processing();
      handle_test_request(static_cast<AsyncRequest *>(msg));
   }
   else
      Base::_handle_async_msg(msg);
}

Boolean MessageQueueServer::messageOK(const Message *msg)
{
   if(msg->getMask() & message_mask::ha_async)
   {
      return true;
   }
   return false;
}

void MessageQueueServer::handle_test_request(AsyncRequest *msg)
{
   if( msg->getType() == 0x04100000 )
   {
      test_response *resp = 
	 new test_response(msg->getKey(),
			   msg->getRouting(),
			   msg->op, 
			   async_results::OK,
			   msg->dest, 
			   "i am a test response");
      _enqueueAsyncResponse(msg, resp, ASYNC_OPSTATE_COMPLETE, 0);
   }
}


void MessageQueueServer::handle_CimServiceStop(CimServiceStop *req)
{
   
}



void MessageQueueClient::_handle_async_msg(AsyncMessage *msg)
{
   Base::_handle_async_msg(msg);
}


Boolean MessageQueueClient::messageOK(const Message *msg)
{
   if(msg->getMask() & message_mask::ha_async)
   {
      return true;
   }
   return false;
}


void MessageQueueClient::send_test_request(char *greeting, Uint32 qid)
{
   test_request *req = 
      new test_request(Base::get_next_xid(),
		       get_op(),
		       qid, 
		       _queueId,
		       greeting);

   unlocked_dq<AsyncMessage> replies(true);
   SendWait(req, &replies);
   while( replies.count() )
   { 
      msg_count++;
      delete static_cast<test_response *>(replies.remove_first());
   }


}

PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL client_func(void *parm);
PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL server_func(void *parm);

int main(int argc, char **argv)
{
   Thread client(client_func, (void *)&msg_count, false);
   Thread server(server_func, (void *)&msg_count, false);
   
   cimom Q_server();
   

   server.run();
   client.run();
   while( msg_count.value() < 1000 )
   {
      pegasus_sleep(1);
   }
   client.join();
   server.join();
   
   return(1);
}


PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL client_func(void *parm)
{
   Thread *my_handle = reinterpret_cast<Thread *>(parm);
   AtomicInt & count = *(reinterpret_cast<AtomicInt *>(my_handle->get_parm()));
   
   MessageQueueClient q_client("test client");
   q_client.register_service("test client", q_client._capabilities, q_client._mask);
   Array<Uint32> services;
   while( services.size() == 0 )
   {
      q_client.find_services(String("test server"), 0, 0, &services);
      my_handle->sleep(1);
   }
   
   while (msg_count.value() < 1000 )
   {
      q_client.send_test_request("i am the test client" , services[0]);

   }
   CimServiceStop *stop = 
      new CimServiceStop(q_client.get_next_xid(),
			 q_client.get_op(), 
			 services[0], 
			 q_client.get_qid(),
			 true);
   unlocked_dq<AsyncMessage> replies(true);
   q_client.SendWait(stop, &replies);
   my_handle->exit_self( (PEGASUS_THREAD_RETURN) 1 );

   return(0);
   
}

