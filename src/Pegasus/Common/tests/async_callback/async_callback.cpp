//%////-*-c++-*-////////////////////////////////////////////////////////////////
//   
// Copyright (c) 2000, 2001 The Open group, BMC Software, Tivoli Systems, IBM
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
////%/////////////////////////////////////////////////////////////////////////////
 

#include "async_callback.h"

PEGASUS_USING_STD;
PEGASUS_USING_PEGASUS;
  
 

async_start::async_start(AsyncOpNode *op,
			 Uint32 start_q, 
			 Uint32 completion_q,
			 Message *op_data)
   : Base(getNextKey(),
	  op, 
	  start_q, 
	  completion_q,
	  false,
	  op_data)
{
      
}
      
async_complete::async_complete(const async_start & start,   
			       Uint32 result,  
			       Message *result_data)
   : Base(start.getKey(), 
	  start.getRouting(),
	  start.op,
	  result,
	  start.resp,
	  false),
     _result_data(result_data)
{
   start.op->put_response(this);
}

 
Message *async_complete::get_result_data(void)
{
   Message *ret = _result_data;
   _result_data = 0;
   ret->put_async(0);
   return ret;
}


AtomicInt test_async_queue::msg_count(0);

test_async_queue::test_async_queue(ROLE role)
   : Base( ( role == CLIENT ) ? "client" : "server" , getNextQueueId()),
     _die_now(0),
     _role(role)
{
   
}




Boolean test_async_queue::messageOK(const Message * msg)
{
   if (msg->getMask() & message_mask::ha_async)
   {
      if(_role == CLIENT )
      {
      if(msg->getType() == async_messages::ASYNC_OP_RESULT)
	 return true;
      }
      else 
      {
	 if(msg->getType() == async_messages::ASYNC_OP_START)
	    return true;
	 if(msg->getType() == async_messages::CIMSERVICE_STOP)
	    return true;
      }
   }
   return false;
}


void test_async_queue::_handle_async_request(AsyncRequest *rq)
{

   if( rq->getType() == async_messages::ASYNC_OP_START)
   {
      PEGASUS_ASSERT(_role == SERVER);
      Message *response_data = new Message(CIM_GET_INSTANCE_RESPONSE_MESSAGE);
      async_complete *response_msg = 
	 new async_complete(static_cast<async_start &>(*rq),
			    async_results::OK,
			    response_data);
      _complete_op_node(rq->op, 0, 0, 0);
   }
   else if( rq->getType() == async_messages::CIMSERVICE_STOP )
   {
      PEGASUS_ASSERT(_role == SERVER);
      _handle_stop((CimServiceStop *)rq);
   }
   else 
      Base::_handle_async_request(rq);
}


void test_async_queue::_handle_async_callback(AsyncOpNode *op)
{
   PEGASUS_ASSERT(_role == CLIENT);
   
   PEGASUS_ASSERT( op->read_state() & ASYNC_OPSTATE_COMPLETE  );

   Base::_handle_async_callback(op);
}

void test_async_queue::async_handleEnqueue(AsyncOpNode *op, 
					   MessageQueue *q, 
					   void *parm)
{

   // I am static, get a pointer to my object 
   test_async_queue *myself = static_cast<test_async_queue *>(q);
   
   async_start *rq = static_cast<async_start *>(op->get_request());
   async_complete *rp = static_cast<async_complete *>(op->get_response());
   
   if ( (rq->getType() == async_messages::ASYNC_OP_START ) &&
	(rp->getType() == async_messages::ASYNC_OP_RESULT) )
   {
      Message *cim_rq = rq->get_action();
      Message *cim_rp = rp->get_result_data();
       
      PEGASUS_ASSERT(cim_rq->getType() == CIM_GET_INSTANCE_REQUEST_MESSAGE);
      PEGASUS_ASSERT(cim_rp->getType() == CIM_GET_INSTANCE_RESPONSE_MESSAGE);
      test_async_queue::msg_count++;
      
      delete cim_rp;
      delete cim_rq;
      delete rp;
      delete rq;
      delete op;
   }
}


void test_async_queue::async_handleSafeEnqueue(Message *msg, 
					       void *handle, 
					       void *parameter)
{

   async_complete *rp = static_cast<async_complete *>(msg);
   PEGASUS_ASSERT(msg->getType() == async_messages::ASYNC_OP_RESULT);
   msg = rp->get_result_data();
   
   test_async_queue::msg_count++;
   delete msg;
   delete rp;
   
}

void test_async_queue::_handle_stop(CimServiceStop *stop)
{
   
   AsyncReply *resp =  
      new AsyncReply(async_messages::REPLY, 
		     stop->getKey(), 
		     stop->getRouting(), 
		     0, 
		     stop->op, 
		     async_results::CIM_SERVICE_STOPPED, 
		     stop->resp, 
		     stop->block);
   _completeAsyncResponse(stop, resp, ASYNC_OPSTATE_COMPLETE, 0 );
   _die_now = 1;
}

PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL client_func(void *parm);
PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL server_func(void *parm);  

int main(int argc, char **argv)
{
   Thread client(client_func,(void *)0, false); 
   
   Thread server(server_func, (void *)0, false); 
   
   server.run();
   client.run();
   
   
   client.join();
   server.join();
   cout << "+++++ passed all tests " << endl;
   
   return(0);
}


PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL client_func(void *parm)
{
   Thread *myself = reinterpret_cast<Thread *>(parm);
   
   test_async_queue *client = new test_async_queue(test_async_queue::CLIENT);
   
   // find the server 
   Array<Uint32> services; 
   while( services.size() == 0 )
   {
      client->find_services(String("server"), 0, 0, &services );
      pegasus_yield();
   }
   
   AtomicInt rq_count;
   cout << "testing low-level async send " << endl;
   do 
   {
      
      Message *cim_rq = new Message(CIM_GET_INSTANCE_REQUEST_MESSAGE); 
      
      AsyncOpNode *op = client->get_op();
      AsyncOperationStart *async_rq = new AsyncOperationStart(client->get_next_xid(),
							      op,
							      services[0],
							      client->getQueueId(),
							      false,
							      cim_rq);
      client->SendAsync(op, 
			services[0], 
			test_async_queue::async_handleEnqueue,
			client,
			(void *)0);
      rq_count++; 
      pegasus_yield();  

   } while( test_async_queue::msg_count.value() < 10000 );
   
   test_async_queue::msg_count = 0 ;
   rq_count = 0;
   cout << "testing fast safe async send " << endl;
   do   
   {
      Message *cim_rq = new Message(CIM_GET_INSTANCE_REQUEST_MESSAGE);
      
      AsyncOperationStart *async_rq = new AsyncOperationStart(client->get_next_xid(),
							      0,
							      services[0],
							      client->getQueueId(),
							      false,
							      cim_rq);
      client->SendAsync(async_rq, 
			services[0], 
			test_async_queue::async_handleSafeEnqueue,
			client, 
			(void *)NULL);
      rq_count++;
      pegasus_yield();

   } while( test_async_queue::msg_count.value() < 10000 );

   cout << "sending stop to server " << endl;
   
   CimServiceStop *stop =   
      new CimServiceStop(client->get_next_xid(),
			 0, 
			 services[0], 
			 client->getQueueId(),
			 true);

   AsyncMessage *reply = client->SendWait( stop );
   delete stop;
   delete reply;
   
   // wait for the server to shut down 
   while( services.size() > 0 )
   {
      client->find_services(String("server"), 0, 0, &services );
      pegasus_yield();
   }

   cout << "shutting down client" << endl;
   
   client->deregister_service();
   client->_shutdown_incoming_queue();


   cout << "client waiting for lingering callbacks..." << endl;
   
   pegasus_sleep(1000);
   delete client;
   myself->exit_self( (PEGASUS_THREAD_RETURN) 1 );
   return(0);
}


PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL server_func(void *parm)
{
   Thread *myself = reinterpret_cast<Thread *>(parm);
   
   test_async_queue *server = new test_async_queue(test_async_queue::SERVER);
   
   while( server->_die_now.value() < 1 ) 
   {
      pegasus_yield();
   }
   cout << "server shutting down" << endl;
   
   server->deregister_service();
   server->_shutdown_incoming_queue();
   cout << "server waiting for lingering callbacks..." << endl;
   pegasus_sleep(1000);
   delete server;
   
   myself->exit_self( (PEGASUS_THREAD_RETURN) 1 );
   return(0);
}
