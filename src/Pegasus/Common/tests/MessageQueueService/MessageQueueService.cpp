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
      
      virtual void _handle_incoming_operation(AsyncOpNode *operation);
      virtual Boolean messageOK(const Message *msg);
      void handle_test_request(AsyncRequest *msg);
      virtual void handle_CimServiceStop(CimServiceStop *req);
      virtual void _handle_async_request(AsyncRequest *req);
      
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
      
      virtual void _handle_async_request(AsyncRequest *req);
      AtomicInt client_xid;
};

AtomicInt msg_count;


 
Uint32 MessageQueueClient::get_qid(void) 
{
   return _queueId;
}


void MessageQueueServer::_handle_incoming_operation(AsyncOpNode *operation)
{
   if ( operation != 0 )
   {
      Message *rq = operation->get_request();
      PEGASUS_ASSERT(rq != 0 );
      PEGASUS_ASSERT(rq->getMask() & message_mask::ha_async );
      PEGASUS_ASSERT(rq->getMask() & message_mask::ha_request);
      _handle_async_request(static_cast<AsyncRequest *>(rq));
   }
     
   return;
   
}

void MessageQueueServer::_handle_async_request(AsyncRequest *req)
{
   if (req->getType() == 0x04100000 )
   {
      req->op->processing();
      handle_test_request(req);
   }
   else if ( req->getType() == async_messages::CIMSERVICE_STOP )
   {
      req->op->processing();
      handle_CimServiceStop(static_cast<CimServiceStop *>(req));
   }
   
   else
      Base::_handle_async_request(req);
}

Boolean MessageQueueServer::messageOK(const Message *msg)
{
   if(msg->getMask() & message_mask::ha_async)
   {
      if( msg->getType() == 0x04100000 ||
	  msg->getType() == async_messages::CIMSERVICE_STOP )
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
      _completeAsyncResponse(msg, resp, ASYNC_OPSTATE_COMPLETE, 0);
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
   _completeAsyncResponse(req, resp, ASYNC_OPSTATE_COMPLETE, 0 );
   
   cout << "recieved STOP from test client" << endl;
   
   dienow++;
}


void MessageQueueClient::_handle_async_request(AsyncRequest *req)
{
   Base::_handle_async_request(req);
}
 

Boolean MessageQueueClient::messageOK(const Message *msg)
{ 
   if(msg->getMask() & message_mask::ha_async)
   {
      if (msg->getType() == 0x04200000)
      return true;
   }
   return false;
}


void MessageQueueClient::send_test_request(char *greeting, Uint32 qid)
{
   test_request *req = 
      new test_request(Base::get_next_xid(),
		       0,
		       qid, 
		       _queueId,
		       greeting);


   AsyncMessage *response = SendWait(req);
   if( response != 0  )
   {
      msg_count++; 
      cout << "rcv test msg " << msg_count.value() << endl;
      delete response;
   }
   delete req;
}


PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL client_func(void *parm);
PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL server_func(void *parm);

int main(int argc, char **argv)
{
   Thread client(client_func, (void *)&msg_count, false);
   Thread another(client_func, (void *)&msg_count, false);
   Thread a_third(client_func, (void *)&msg_count, false);
   
   Thread server(server_func, (void *)&msg_count, false);
   
   cimom *Q_server = new cimom();
   

   server.run();
   client.run();
   another.run();
   a_third.run();
   
   
   while( msg_count.value() < 1500 ) 
   {
      pegasus_sleep(10);
   }
   a_third.join();
   another.join();
   client.join();
   server.join();
   cout << " delete meta dispatcher" << endl;
   
   Q_server->_shutdown_routed_queue();
    
   delete Q_server;  
   
   return(0);
}



PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL client_func(void *parm)
{
   Thread *my_handle = reinterpret_cast<Thread *>(parm);
   AtomicInt & count = *(reinterpret_cast<AtomicInt *>(my_handle->get_parm()));
   
   MessageQueueClient *q_client = new MessageQueueClient("test client");
   q_client->register_service("test client", q_client->_client_capabilities, q_client->_client_mask);
   cout << " client registered " << endl;
   
   Array<Uint32> services; 
   while( services.size() == 0 )
   {
      q_client->find_services(String("test server"), 0, 0, &services); 
      my_handle->sleep(10);  
   }
   
   cout << "found server at " << services[0] << endl;
   
   while (msg_count.value() < 1500 )
   {
      q_client->send_test_request("i am the test client" , services[0]);
   }
   // now that we have sent and received all of our responses, tell 
   // the server thread to stop 

   cout << "sending STOP to test server" << endl;
   
   CimServiceStop *stop =   
      new CimServiceStop(q_client->get_next_xid(),
			 0, 
			 services[0], 
			 q_client->get_qid(),
			 true);

   q_client->SendWait(stop ); 
   delete stop;
   
   cout << "deregistering client qid " << q_client->getQueueId() << endl;
 
   q_client->deregister_service(); 
 
   cout << "closing service queue" << endl;
   
   q_client->_shutdown_incoming_queue();
   
   cout << " deleting client " << endl ;

   delete q_client;

   cout << " exiting " << endl;
   
   my_handle->exit_self( (PEGASUS_THREAD_RETURN) 1 );
   return(0);
}


PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL server_func(void *parm)
{
   Thread *my_handle = reinterpret_cast<Thread *>(parm);

   MessageQueueServer *q_server = new MessageQueueServer("test server") ;
 
   q_server->register_service("test server", q_server->_capabilities, q_server->_mask);
   
   cout << "test server registered" << endl;
   
   while( q_server->dienow.value()  < 3  )
   {
      my_handle->sleep(10);
   }
 
   cout << "deregistering server qid " << q_server->getQueueId() << endl;
   q_server->deregister_service();

   cout << "closing server queue" << endl;
   q_server->_shutdown_incoming_queue();
   

   cout << " deleting server " << endl;
   
   delete q_server; 
   
   cout << "exiting server " << endl;
   
   my_handle->exit_self( (PEGASUS_THREAD_RETURN) 1 );
   return(0);

}
