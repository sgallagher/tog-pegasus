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

#include <Pegasus/Common/ModuleController.h>

PEGASUS_USING_STD;
PEGASUS_USING_PEGASUS;


#define SERVICE_NAME "peg_test_service"
#define CONTROLLER_NAME "peg_test_module_controller"


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


class test_module 
{
   public:
      test_module(char *name, char *controller_name);
      ~test_module(void);
      
      static Message *receive_msg(Message *msg, void *parm);
      static void async_callback(Uint32 msg_id, Message *msg, void *parm);
      static void shutdown_notify(Uint32 code, void *parm);
      static PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL thread_func(void *);
      ModuleController *get_controller(void);
      pegasus_module *get_mod_handle(void);
      ModuleController::client_handle *get_client_handle(void);

   private:
      test_module(void);
      test_module(const test_module &);
      test_module & operator =(const test_module &);
      ModuleController *_controller;
      pegasus_module *_module_handle;
      ModuleController::client_handle *_client_handle;
      AtomicInt _msg_rx;
      AtomicInt _msg_tx;
      AtomicInt _thread_ex;
      String _name;
      String _controller_name;
      pegasus_internal_identity _id;
      
};


test_module::test_module(char *name, char *controller_name)
   : _controller(0), _module_handle(0), _client_handle(0),
     _msg_rx(0), _msg_tx(0), _thread_ex(0), _name(name), 
     _controller_name(controller_name), _id(peg_credential_types::MODULE)
{

      
   
}

test_module::~test_module(void)
{
   if(_controller)
   {
      try 
      {
	 _controller->deregister_module(_name);
      }
      catch(Permission &)
      {
	 ;
      }
      if(_client_handle)
	 _controller->return_client_handle(_client_handle);
   }
}


Message *test_module::receive_msg(Message *msg, void *parm)
{
   test_module *myself = reinterpret_cast<test_module *>(parm);
   if(msg && msg->getType() == 0x04100000 )
   {
      cout << "received msg from peer " << endl;
      
      myself->_msg_rx++;
      return new test_response(msg->getKey(),
			   msg->getRouting(),
			   static_cast<AsyncRequest *>(msg)->op, 
			   async_results::OK,  
			   msg->dest, 
			   "i am a test response"); 
   }
   return NULL;
}


void test_module::async_callback(Uint32 id, Message *msg, void *parm)
{

   test_module *myself = reinterpret_cast<test_module *>(parm);
   if(msg && (msg->getType() == 0x04200000 || msg->getType() == 0x04100000))
   {
      cout << "module async callback " << endl;
      
      (myself->_msg_rx)++;
      delete msg;
   }
}

void test_module::shutdown_notify(Uint32 code, void *parm)
{
   test_module *myself = reinterpret_cast<test_module *>(parm);
   cout << "module received shutdown notification" << endl;
}

PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL test_module::thread_func(void *parm)
{
   test_module *myself = reinterpret_cast<test_module *>(parm);
   (myself->_thread_ex)++;
   return 0;
   
}


ModuleController *test_module::get_controller(void)
{
   if(_controller == NULL)
   {
      try 
      {
	 
	 _controller = &(ModuleController::register_module(_controller_name, 
							   _name,
							   this,
							   receive_msg,
							   async_callback,
							   shutdown_notify,
							   &_module_handle));
      }
      catch(AlreadyExistsException &)
      {
	 ;
      }
      
   }
   return _controller;
}

pegasus_module *test_module::get_mod_handle(void)
{
   if(_controller == NULL)
   {
      try 
      {
	 
	 _controller = &(ModuleController::register_module(_controller_name, 
							   _name,
							   this,
							   receive_msg,
							   async_callback,
							   shutdown_notify,
							   &_module_handle));
      }
      catch(AlreadyExistsException &)
      {
	 ;
      }
      
   }
   return _module_handle;
}


ModuleController::client_handle *test_module::get_client_handle(void)
{
   if(_client_handle == NULL)
   {
      CString temp = _controller_name.getCString();
      if( temp )
      {
	 try 
	 {
	    _controller = &(ModuleController::get_client_handle(temp, _id, &_client_handle));
	 }
	 catch(IncompatibleTypesException &)
	 {
	    ;
	 }
	 delete temp;
      }
   }
   
   return _client_handle;
}


class test_service : public MessageQueueService
{
   public:
      typedef MessageQueueService Base;

      test_service(char *name);

      virtual ~test_service(void) ;

      virtual void _handle_incoming_operation(AsyncOpNode *operation);
      virtual Boolean messageOK(const Message *msg);
      void handle_test_request(AsyncRequest *msg);
      virtual void _handle_async_request(AsyncRequest *req);
      virtual void handleEnqueue(void)
      {
      }
      virtual void handleEnqueue(Message *msg)
      {
      }
      
      
   private:
      test_service(void);
      test_service(const test_service &);
      test_service & operator =(const test_service &);
};


test_service::test_service(char *name)
      	 : Base(name, MessageQueue::getNextQueueId(), 0, 
		message_mask::type_cimom | 
		message_mask::type_service | 
		message_mask::ha_request | 
		message_mask::ha_reply | 
 		message_mask::ha_async ) 
{
   
}

test_service::~test_service(void)
{

}

void test_service::_handle_incoming_operation(AsyncOpNode *operation)
{
   if ( operation != 0 )
   {
      Message *rq = operation->get_request();

      PEGASUS_ASSERT(rq != 0 );
      if ( rq && (rq->getMask() & message_mask::ha_async))
      {
	 _handle_async_request(static_cast<AsyncRequest *>(rq));
      }
      else 
      {
	 delete rq;
	 operation->release();
	 return_op(operation);
      }
   }
   return;
}


Boolean test_service::messageOK(const Message *msg)
{
   return true;
}

void test_service::handle_test_request(AsyncRequest *msg)
{

   cout << "service received test request" << endl;
   
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

void test_service::_handle_async_request(AsyncRequest *req)
{
   if (req->getType() == 0x04100000 )
   {
      req->op->processing();
      handle_test_request(req);
   }
   
   else
      Base::_handle_async_request(req);
}


typedef struct _test_module_parms 
{
      _test_module_parms(char *controller, char *peer, char *me)
      {
	 _controller = strdup(controller);
	 _peer = strdup(peer);
	 _me = strdup(me);
      }
      
      char *_controller;
      char *_peer;
      char *_me;
      ~_test_module_parms(void)
      {
	 delete _controller;
	 delete _peer;
	 delete _me;
      }
      
} MODULE_PARMS;


PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL module_func(void *parm)
{
   Thread *myself = reinterpret_cast<Thread *>(parm);
   MODULE_PARMS *parms = reinterpret_cast<MODULE_PARMS *>(myself->get_parm());
      
   test_module *my_module = new test_module(parms->_me, parms->_controller);
   my_module->get_controller();
   my_module->get_mod_handle();
   my_module->get_client_handle();
   
   Uint32 svc_qid = 
      my_module->get_controller()->find_service(*(my_module->get_mod_handle()), SERVICE_NAME);
   MessageQueue *svce = MessageQueue::lookup(svc_qid);
   if(svce)
      cout << "Found Service " << SERVICE_NAME << " at " << svce << endl;
   Uint32 peer_qid;
   do 
   {  
      peer_qid = 
	 my_module->get_controller()->find_module_in_service(*(my_module->get_mod_handle()), String(parms->_peer));

      if((svce = MessageQueue::lookup(peer_qid)) == NULL)
	 pegasus_sleep(1);
   } while( svce == NULL);
   
   cout << "Found Peer Module " << parms->_peer << " at " << svce << endl;
   
   pegasus_sleep(1);
   
   test_request *req = 
      new test_request(my_module->get_controller()->get_next_xid(),
		       MessageQueueService::get_op(), 
		       svc_qid,
		       my_module->get_controller()->getQueueId(),
		       "hello");

   AsyncMessage *response = my_module->get_controller()->ModuleSendWait(*(my_module->get_mod_handle()),
									svc_qid,
									req);
   if(response && response->getType() == 0x04200000)
      cout << " ModuleSendWait to service successfull " << endl;
   
   delete req;
   delete response;
   
   pegasus_sleep(1);

   req = 
      new test_request(my_module->get_controller()->get_next_xid(),
		       MessageQueueService::get_op(), 
		       peer_qid,
		       my_module->get_controller()->getQueueId(),
		       "hello");
   
 
   response = my_module->get_controller()->ModuleSendWait(*(my_module->get_mod_handle()),
							  peer_qid,
							  String(parms->_peer),
							  req);
   
   delete req; 
   delete response;
   pegasus_sleep(1);
   cout << " ModuleSendWait to module  successfull " << endl;
   
   req = 
      new test_request(my_module->get_controller()->get_next_xid(),
		       MessageQueueService::get_op(), 
		       svc_qid,
		       my_module->get_controller()->getQueueId(),
		       "hello");

   Boolean success = my_module->get_controller()->ModuleSendAsync( (*my_module->get_mod_handle()),
								   0, 
								   svc_qid,
								   req,
								   my_module);
   if(success == true )
      cout << "SendAsync to service successful" << endl;

   delete req;
   pegasus_sleep(1);
   req = 
      new test_request(my_module->get_controller()->get_next_xid(),
		       MessageQueueService::get_op(), 
		       peer_qid,
		       my_module->get_controller()->getQueueId(),
		       "hello");

   
   success = my_module->get_controller()->ModuleSendAsync( (*my_module->get_mod_handle()),
							   0,  
							   peer_qid,
							   String(parms->_peer),
							   req,
							   my_module);
   
   delete req;
   
   if(success == true )
      cout << "SendAsync to module successful" << endl;


   req = 
      new test_request(my_module->get_controller()->get_next_xid(),
		       MessageQueueService::get_op(), 
		       peer_qid,
		       my_module->get_controller()->getQueueId(),
		       "hello");

   success = my_module->get_controller()->ClientSendAsync( (*my_module->get_client_handle()),
							   0, 
							   svc_qid,
							   req,
							   test_module::async_callback,
							   my_module);
   
   delete req;
   if(success == true )
      cout << "ClientSendAsync to service successful" << endl;

   req = 
      new test_request(my_module->get_controller()->get_next_xid(),
		       MessageQueueService::get_op(), 
		       peer_qid,
		       my_module->get_controller()->getQueueId(),
		       "hello");
   
   success = my_module->get_controller()->ClientSendAsync( (*my_module->get_client_handle()),
							   0, 
							   peer_qid,
							   String(parms->_peer),
							   req,
							   test_module::async_callback,
							   my_module);
   
   delete req;
   if(success == true )
      cout << "ClientSendAsync to module successful" << endl;




   req = 
      new test_request(my_module->get_controller()->get_next_xid(),
		       MessageQueueService::get_op(), 
		       peer_qid,
		       my_module->get_controller()->getQueueId(),
		       "hello");
   
   success = my_module->get_controller()->ModuleSendForget( (*my_module->get_mod_handle()),
							    svc_qid,
							    req);
   
   if(success == true )
      cout << "ModuleSendForget to service successful" << endl;



   req = 
      new test_request(my_module->get_controller()->get_next_xid(),
		       MessageQueueService::get_op(), 
		       peer_qid,
		       my_module->get_controller()->getQueueId(),
		       "hello");
   
   success = my_module->get_controller()->ModuleSendForget( (*my_module->get_mod_handle()),
							    peer_qid,
							    String(parms->_peer),
							    req);
   
   if(success == true )
      cout << "ModuleSendForget to module successful" << endl;





   req = 
      new test_request(my_module->get_controller()->get_next_xid(),
		       MessageQueueService::get_op(), 
		       peer_qid,
		       my_module->get_controller()->getQueueId(),
		       "hello");
   
   success = my_module->get_controller()->ClientSendForget( (*my_module->get_client_handle()),
							    svc_qid,
							    req);
   
   if(success == true )
      cout << "ClientSendForget to service successful" << endl;



   req = 
      new test_request(my_module->get_controller()->get_next_xid(),
		       MessageQueueService::get_op(), 
		       peer_qid, 
		       my_module->get_controller()->getQueueId(),
		       "hello");
   
   success = my_module->get_controller()->ClientSendForget( (*my_module->get_client_handle()),
							    peer_qid,
							    String(parms->_peer),
							    req);
   
   if(success == true )
      cout << "ClientSendForget to module successful" << endl;

   pegasus_sleep(1000);
   delete my_module;
   cout << "module deleted" << endl;
   
   return 0;
   
}




ModuleController internal_controller(CONTROLLER_NAME);
test_service internal_service(SERVICE_NAME);

int main(int argc, char **argv)
{

   cout << "module controller at " << internal_controller.getQueueId() << endl;
   cout << "service at " << internal_service.getQueueId() << endl;

   MODULE_PARMS parms_one(CONTROLLER_NAME, "two", "one");
   MODULE_PARMS parms_two(CONTROLLER_NAME, "one", "two");
   
   Thread one(module_func, (void *)&parms_one, false);
   Thread two(module_func, (void *)&parms_two, false);
   two.run();
   one.run();

   
   one.join();
   two.join(); 
   cout << " joined" << endl;
   return(0);
}

