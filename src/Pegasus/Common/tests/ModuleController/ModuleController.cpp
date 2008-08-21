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
#if defined(PEGASUS_PLATFORM_WIN64_IA64_MSVC) || \
    defined(PEGASUS_PLATFORM_WIN64_X86_64_MSVC) || \
    defined(PEGASUS_PLATFORM_WIN32_IX86_MSVC)
#else
#include <unistd.h>
#endif 
#include <Pegasus/Common/PegasusAssert.h>
#include <Pegasus/Common/Threads.h>
#include <iostream>
#include <stdio.h>
#include <string.h>

#include <Pegasus/Common/ModuleController.h>

PEGASUS_USING_STD;
PEGASUS_USING_PEGASUS;

static char * verbose;

#define SERVICE_NAME "peg_test_service"
#define CONTROLLER_NAME "peg_test_module_controller"


class test_request : public AsyncRequest
{
  
   public:
      typedef AsyncRequest Base;
      
      test_request(
           AsyncOpNode *op, 
           Uint32 destination, 
           Uint32 response,
           const char *message)
     : Base(CIM_GET_CLASS_REQUEST_MESSAGE,
        0, 
        op, 
        destination, 
        response, 
        true),
       greeting(message) 
      {   
      }
      
      virtual ~test_request() 
      {
      }
      
      String greeting;
};


class test_response : public AsyncReply
{
   public:
      typedef AsyncReply Base;
      

      test_response(
            AsyncOpNode *op, 
            Uint32 result,
            Uint32 destination, 
            const char *message)
     : Base(CIM_GET_CLASS_RESPONSE_MESSAGE,
        0, 
        op, 
        result, 
        destination,
        true), 
       greeting(message) 
      {  
     
      }
      
      virtual ~test_response()
      {
     
      }
      
      String greeting;
};


class test_module 
{
   public:
      test_module(const char *name, const char *controller_name);
      ~test_module();
      
      static Message *receive_msg(Message *msg, void *parm);
      static void async_callback(Uint32 msg_id, Message *msg, void *parm);
      static ThreadReturnType PEGASUS_THREAD_CDECL thread_func(void *);
      ModuleController *get_controller();
      RegisteredModuleHandle *get_mod_handle();

   private:
      test_module();
      test_module(const test_module &);
      test_module & operator =(const test_module &);
      ModuleController *_controller;
      RegisteredModuleHandle *_module_handle;
      AtomicInt _msg_rx;
      AtomicInt _msg_tx;
      AtomicInt _thread_ex;
      String _name;
      String _controller_name;
};


test_module::test_module(const char *name, const char *controller_name)
   : _controller(0), _module_handle(0),
     _msg_rx(0), _msg_tx(0), _thread_ex(0), _name(name), 
     _controller_name(controller_name)
{
}

test_module::~test_module()
{
   if(_controller)
   {
       _controller->deregister_module(_name);
   }
}


Message *test_module::receive_msg(Message *msg, void *parm)
{
   test_module *myself = reinterpret_cast<test_module *>(parm);
   if (msg && msg->getType() == CIM_GET_CLASS_REQUEST_MESSAGE)
   {
      cout << "received msg from peer " << endl;
      
      myself->_msg_rx++;
      return new test_response(
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
   if (msg &&
       (msg->getType() == CIM_GET_CLASS_RESPONSE_MESSAGE ||
        msg->getType() == CIM_GET_CLASS_REQUEST_MESSAGE))
   {
      cout << "module async callback " << endl;
      
      (myself->_msg_rx)++;
      delete msg;
   }
}

ThreadReturnType PEGASUS_THREAD_CDECL test_module::thread_func(void *parm)
{
   test_module *myself = reinterpret_cast<test_module *>(parm);
   (myself->_thread_ex)++;
   return 0;
   
}


ModuleController *test_module::get_controller()
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
                               &_module_handle));
      }
      catch(AlreadyExistsException &)
      {
     ;
      }
      
   }
   return _controller;
}

RegisteredModuleHandle *test_module::get_mod_handle()
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
                               &_module_handle));
      }
      catch(AlreadyExistsException &)
      {
     ;
      }
      
   }
   return _module_handle;
}


class test_service : public MessageQueueService
{
   public:
      typedef MessageQueueService Base;

      test_service(const char *name);

      virtual ~test_service();

      virtual void _handle_incoming_operation(AsyncOpNode *operation);
      virtual Boolean messageOK(const Message *msg);
      void handle_test_request(AsyncRequest *msg);
      virtual void _handle_async_request(AsyncRequest *req);
      virtual void handleEnqueue()
      {
      }
      virtual void handleEnqueue(Message *msg)
      {
      }
      
      
   private:
      test_service();
      test_service(const test_service &);
      test_service & operator =(const test_service &);
};


test_service::test_service(const char *name)
         : Base(name, 0, 
        MessageMask::ha_request | 
        MessageMask::ha_reply | 
        MessageMask::ha_async ) 
{
}

test_service::~test_service()
{
}

void test_service::_handle_incoming_operation(AsyncOpNode *operation)
{
   if ( operation != 0 )
   {
      Message* rq = operation->getRequest();

      PEGASUS_TEST_ASSERT(rq != 0 );
      if ( rq && (rq->getMask() & MessageMask::ha_async))
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
   
   if (msg->getType() == CIM_GET_CLASS_REQUEST_MESSAGE)
   {
       test_response *resp = 
           new test_response(
               msg->op, 
               async_results::OK,  
               msg->dest, 
               "i am a test response");
       _completeAsyncResponse(msg, resp, ASYNC_OPSTATE_COMPLETE, 0);
   }
}

void test_service::_handle_async_request(AsyncRequest *req)
{
   if (req->getType() == CIM_GET_CLASS_REQUEST_MESSAGE)
   {
      req->op->processing();
      handle_test_request(req);
   }
   else
      Base::_handle_async_request(req);
}


typedef struct _test_module_parms 
{
      _test_module_parms(
          const char *controller,
          const char *peer,
          const char *me)
      {
     _controller = strdup(controller);
     _peer = strdup(peer);
     _me = strdup(me);
      }
      
      char *_controller;
      char *_peer;
      char *_me;
      ~_test_module_parms()
      {
     delete _controller;
     delete _peer;
     delete _me;
      }
      
} MODULE_PARMS;


ThreadReturnType PEGASUS_THREAD_CDECL module_func(void *parm)
{
   Thread *myself = reinterpret_cast<Thread *>(parm);
   MODULE_PARMS *parms = reinterpret_cast<MODULE_PARMS *>(myself->get_parm());
      
   test_module *my_module = new test_module(parms->_me, parms->_controller);
   my_module->get_controller();
   my_module->get_mod_handle();
   
   Uint32 svc_qid = 
      my_module->get_controller()->find_service(*(my_module->get_mod_handle())
                                                , SERVICE_NAME);
   MessageQueue *svce = MessageQueue::lookup(svc_qid);
   if(svce)
      cout << "Found Service " << SERVICE_NAME << " at " << svce << endl;
   Uint32 peer_qid;
   do 
   {  
      peer_qid = 
     my_module->get_controller()->find_module_in_service(
         *(my_module->get_mod_handle()), String(parms->_peer));

      if((svce = MessageQueue::lookup(peer_qid)) == NULL)
     Threads::sleep(1);
   } while( svce == NULL);
   
   cout << "Found Peer Module " << parms->_peer << " at " << svce << endl;
   
   Threads::sleep(1);
   
   test_request *req;
   Boolean success;

   req = 
      new test_request(
               0, //MessageQueueService::get_op(), 
               svc_qid,
               my_module->get_controller()->getQueueId(),
               "hello");

   AsyncMessage *response = 
       my_module->get_controller()->ModuleSendWait(
                                *(my_module->get_mod_handle()),
                                    svc_qid,
                                    req);
   if (response && response->getType() == CIM_GET_CLASS_RESPONSE_MESSAGE)
       cout << " ModuleSendWait to service successful" << endl;
   
   delete req;
   delete response;
   
   Threads::sleep(1);

   req = 
      new test_request(
               0, //MessageQueueService::get_op(), 
               peer_qid,
               my_module->get_controller()->getQueueId(),
               "hello");
   
 
   response = my_module->get_controller()->
                    ModuleSendWait(*(my_module->get_mod_handle()),
                              peer_qid,
                              String(parms->_peer),
                              req);
   
   delete req; 
   delete response;
   Threads::sleep(1);
   cout << " ModuleSendWait to module  successfull " << endl;
   
   req = 
      new test_request(
               0, //MessageQueueService::get_op(), 
               svc_qid,
               my_module->get_controller()->getQueueId(),
               "hello");

   success = my_module->get_controller()->
                    ModuleSendAsync( (*my_module->get_mod_handle()),
                                   0, 
                                   svc_qid,
                                   req,
                                   my_module);
   if(success == true )
      cout << "SendAsync to service successful" << endl;

   delete req;
   Threads::sleep(1);
   req = 
      new test_request(
               0, //MessageQueueService::get_op(), 
               peer_qid,
               my_module->get_controller()->getQueueId(),
               "hello");

   
   success = my_module->get_controller()->
                        ModuleSendAsync( (*my_module->get_mod_handle()),
                               0,  
                               peer_qid,
                               String(parms->_peer),
                               req,
                               my_module);
   
   delete req;
   
   if(success == true )
      cout << "SendAsync to module successful" << endl;


   req = 
      new test_request(
               0, //MessageQueueService::get_op(), 
               peer_qid,
               my_module->get_controller()->getQueueId(),
               "hello");

   success = my_module->get_controller()->ClientSendAsync(
                               0, 
                               svc_qid,
                               req,
                               test_module::async_callback,
                               my_module);
   
   delete req;
   if(success == true )
      cout << "ClientSendAsync to service successful" << endl;

   req = 
      new test_request(
               0, //MessageQueueService::get_op(), 
               peer_qid,
               my_module->get_controller()->getQueueId(),
               "hello");
   
   success = my_module->get_controller()->ClientSendAsync(
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
      new test_request(
               0, //MessageQueueService::get_op(), 
               peer_qid,
               my_module->get_controller()->getQueueId(),
               "hello");
   
   success = my_module->get_controller()->
                        ModuleSendForget( (*my_module->get_mod_handle()),
                                svc_qid,
                                req);
   
   if(success == true )
      cout << "ModuleSendForget to service successful" << endl;



   req = 
      new test_request(
               0, //MessageQueueService::get_op(), 
               peer_qid,
               my_module->get_controller()->getQueueId(),
               "hello");
   
   success = my_module->get_controller()->ModuleSendForget(
                        (*my_module->get_mod_handle()),
                                peer_qid,
                                String(parms->_peer),
                                req);
   
   if(success == true )
      cout << "ModuleSendForget to module successful" << endl;



   req = 
      new test_request(
               0, //MessageQueueService::get_op(), 
               peer_qid,
               my_module->get_controller()->getQueueId(),
               "hello");
   
   success = my_module->get_controller()->ClientSendForget(
                                svc_qid,
                                req);
   
   if(success == true )
      cout << "ClientSendForget to service successful" << endl;



   req = 
      new test_request(
               0, //MessageQueueService::get_op(), 
               peer_qid, 
               my_module->get_controller()->getQueueId(),
               "hello");
   
   success = my_module->get_controller()->ClientSendForget(
                                peer_qid,
                                String(parms->_peer),
                                req);
   
   if(success == true )
      cout << "ClientSendForget to module successful" << endl;

   Threads::sleep(1000);
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

