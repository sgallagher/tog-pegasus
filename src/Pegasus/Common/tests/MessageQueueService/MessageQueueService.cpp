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


#include <sys/types.h>
#if defined(PEGASUS_PLATFORM_WIN32_IX86_MSVC)
#else
#include <unistd.h>
#endif 
#include <cassert>
#include <iostream>
#include <stdio.h>
#include <string.h>



#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/MessageQueue.h>
#include <Pegasus/Common/DQueue.h>
#include <Pegasus/Common/Thread.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/AsyncOpNode.h>
#include <Pegasus/Common/CimomMessage.h>
#include <Pegasus/Common/Cimom.h>
#include <Pegasus/Common/MessageQueueService.h>

PEGASUS_USING_STD;
PEGASUS_USING_PEGASUS;


class test_request : public AsyncRequest
{
  
   public:
      typedef AsyncRequest Base;
      
      test_request(Uint32 routing, 
		   AsyncOpNode *op, 
		   Uint32 destination, 
		   Uint32 response,
		   char *message)
	 : Base(0x04100000,
		Message::getNextKey(), 
		routing,
		0, 
		op, 
		destination, 
		response, 
		true),
	   greeting(message) 
      {   
	 
      }
      
      virtual ~test_request(void) 
      {

      }
      
      String greeting;
};


class test_response : public AsyncReply
{
   public:
      typedef AsyncReply Base;
      

      test_response(Uint32 key, 
		    Uint32 routing,
		    AsyncOpNode *op, 
		    Uint32 result,
		    Uint32 destination, 
		    char *message)
	 : Base(0x04200000,
		key, 
		routing, 
		0, 
		op, 
		result, 
		destination,
		true), 
	   greeting(message) 
      {  
	 
      }
      
      virtual ~test_response(void)
      {
	 
      }
      
      String greeting;
};


class MessageQueueServer : public MessageQueueService
{
   public:
      typedef MessageQueueService Base;
      MessageQueueServer(char *name)
	 : Base(name, MessageQueue::getNextQueueId(), 0, 
		message_mask::type_cimom | 
		message_mask::type_service | 
		message_mask::ha_request | 
		message_mask::ha_reply | 
		message_mask::ha_async ) ,
	   dienow(0)
      {
	 
      }
      

      virtual ~MessageQueueServer(void) 
      {
      }
      
      
      virtual Boolean messageOK(const Message *msg);
      void handle_test_request(AsyncRequest *msg);
      virtual void handle_CimServiceStop(CimServiceStop *req);

      
      virtual void _handle_async_msg(AsyncMessage *msg);
      AtomicInt dienow;
      

};


class MessageQueueClient : public MessageQueueService
{
      
   public:
      typedef MessageQueueService Base;
      
      MessageQueueClient(char *name)
	 : Base(name, MessageQueue::getNextQueueId(), 0, 
		message_mask::type_cimom | 
		message_mask::type_service | 
		message_mask::ha_request | 
		message_mask::ha_reply | 
		message_mask::ha_async ),
	   client_xid(1)
      {  
	 _client_capabilities = Base::_capabilities;
	 _client_mask = Base::_mask;
      }
            
      virtual ~MessageQueueClient(void) 
      {
      }
      
      
      virtual Boolean messageOK(const Message *msg);

      void send_test_request(char *greeting, Uint32 qid);
      Uint32 get_qid(void);
      
      Uint32 _client_capabilities;
      Uint32 _client_mask;
      
      
      virtual void _handle_async_msg(AsyncMessage *msg);
      AtomicInt client_xid;
};

AtomicInt msg_count;


 
Uint32 MessageQueueClient::get_qid(void) 
{
   return _queueId;
}

void MessageQueueServer::_handle_async_msg(AsyncMessage *msg)
{
   if (msg->getType() == 0x04100000 )
   {
      msg->op->processing();
      handle_test_request(static_cast<AsyncRequest *>(msg));
   }
   else if ( msg->getType() == async_messages::CIMSERVICE_STOP )
   {
      msg->op->processing();
      handle_CimServiceStop(static_cast<CimServiceStop *>(msg));
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

   AsyncReply *resp =  
      new AsyncReply(async_messages::REPLY, 
		     req->getKey(), 
		     req->getRouting(), 
		     0, 
		     req->op, 
		     async_results::CIM_SERVICE_STOPPED, 
		     req->resp, 
		     req->block);
   _enqueueAsyncResponse(req, resp, ASYNC_OPSTATE_COMPLETE, 0 );
   
   dienow = 1;
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
   
   cimom *Q_server = new cimom();
   

//   server.run();
   client.run();
   while( msg_count.value() < 10 )
   {
      pegasus_sleep(10);
   }

   client.join();
   server.join();
   
   delete Q_server;
   
   return(1);
}


PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL client_func(void *parm)
{
   Thread *my_handle = reinterpret_cast<Thread *>(parm);
   AtomicInt & count = *(reinterpret_cast<AtomicInt *>(my_handle->get_parm()));
   
   MessageQueueClient *q_client = new MessageQueueClient("test client");
   q_client->register_service("test client", q_client->_client_capabilities, q_client->_client_mask);
   Array<Uint32> services;
   while( services.size() == 0 )
   {
      q_client->find_services(String("test server"), 0, 0, &services);
      my_handle->sleep(10); 
   }
   
   while (msg_count.value() < 10 )
   {
      q_client->send_test_request("i am the test client" , services[0]);

   }
   // now that we have sent and received all of our responses, tell 
   // the server thread to stop 

   CimServiceStop *stop = 
      new CimServiceStop(q_client->get_next_xid(),
			 q_client->get_op(), 
			 services[0], 
			 q_client->get_qid(),
			 true);

   unlocked_dq<AsyncMessage> replies(true);
   q_client->SendWait(stop, &replies);
   q_client->deregister_service();

   delete q_client;
   my_handle->exit_self( (PEGASUS_THREAD_RETURN) 1 );
   return(0);
}


PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL server_func(void *parm)
{
   Thread *my_handle = reinterpret_cast<Thread *>(parm);

   MessageQueueServer *q_server = new MessageQueueServer("test server");
   q_server->register_service("test server", q_server->_capabilities, q_server->_mask);
   
//   while( q_server->dienow.value()  < 1  )
   while(1)
   {
      my_handle->sleep(10);
     
   }

   q_server->deregister_service();
   delete q_server;
   
   my_handle->exit_self( (PEGASUS_THREAD_RETURN) 1 );
   return(0);
   
}
