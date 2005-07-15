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


#include "async_callback.h"
#include <Pegasus/Common/Tracer.h>
PEGASUS_USING_STD;
PEGASUS_USING_PEGASUS;


static char *verbose;

async_start::async_start (AsyncOpNode * op, Uint32 start_q, Uint32 completion_q, Message * op_data):Base (getNextKey (),
      op,
      start_q, completion_q, false, op_data)
{

}

async_complete::async_complete (const async_start & start,
                                Uint32 result, Message * result_data):
Base (start.getKey (),
      start.getRouting (), start.op, result, start.resp, false),
_result_data (result_data)
{
  start.op->put_response (this);
}


Message *
async_complete::get_result_data (void)
{
  Message *ret = _result_data;
  _result_data = 0;
  ret->put_async (0);
  return ret;
}


AtomicInt test_async_queue::msg_count (0);

test_async_queue::test_async_queue (ROLE role):Base ((role == CLIENT) ? "client" : "server", getNextQueueId ()),
_die_now (0), _role (role)
{

}




Boolean test_async_queue::messageOK (const Message * msg)
{
  if (msg->getMask () & message_mask::ha_async)
    {
      if (_role == CLIENT)
        {
          if (msg->getType () == async_messages::ASYNC_OP_RESULT)
            return true;
        }
      else
        {
          if (msg->getType () == async_messages::ASYNC_OP_START)
            return true;
          if (msg->getType () == async_messages::CIMSERVICE_STOP)
            return true;
        }
    }
  return true;
}


void
test_async_queue::_handle_async_request (AsyncRequest * rq)
{
  if ((rq->getType () == async_messages::ASYNC_OP_START) ||
      (rq->getType () == async_messages::ASYNC_LEGACY_OP_START))
    {  
     try {
      PEGASUS_ASSERT (_role == SERVER);
      Message *response_data =
        new Message (CIM_GET_INSTANCE_RESPONSE_MESSAGE);
      async_complete *response_msg =
        new async_complete (static_cast < async_start & >(*rq),
                            async_results::OK,
                            response_data);
      _complete_op_node (rq->op, 0, 0, async_results::ASYNC_COMPLETE);
    } catch (const PEGASUS_STD(bad_alloc) &)
	{
		cerr <<" Out of memory!" << endl;
	}
    }
  else if (rq->getType () == async_messages::CIMSERVICE_STOP)
    {
      PEGASUS_ASSERT (_role == SERVER);
      _handle_stop ((CimServiceStop *) rq);
    }
  else
    Base::_handle_async_request (rq);
}


void
test_async_queue::_handle_async_callback (AsyncOpNode * op)
{
  PEGASUS_ASSERT (_role == CLIENT);

  PEGASUS_ASSERT (op->read_state () & ASYNC_OPSTATE_COMPLETE);

  Base::_handle_async_callback (op);
}

void
test_async_queue::async_handleEnqueue (AsyncOpNode * op,
                                       MessageQueue * q, void *parm)
{

  // I am static, get a pointer to my object 
  test_async_queue *myself = static_cast < test_async_queue * >(q);

  async_start *rq = static_cast < async_start * >(op->get_request ());
  async_complete *rp = static_cast < async_complete * >(op->get_response ());
  if ((rq->getType () == async_messages::ASYNC_OP_START) &&
      (rp->getType () == async_messages::ASYNC_OP_RESULT))
    {
      Message *cim_rq = rq->get_action ();
      Message *cim_rp = rp->get_result_data ();

      PEGASUS_ASSERT (cim_rq->getType () == CIM_GET_INSTANCE_REQUEST_MESSAGE);
      PEGASUS_ASSERT (cim_rp->getType () ==
                      CIM_GET_INSTANCE_RESPONSE_MESSAGE);
      test_async_queue::msg_count++;

      delete cim_rp;
      delete cim_rq;
      delete rp;
      delete rq;
      delete op;
    }
}


void
test_async_queue::async_handleSafeEnqueue (Message * msg,
                                           void *handle, void *parameter)
{
  test_async_queue::msg_count++;
  delete msg;
}

void
test_async_queue::_handle_stop (CimServiceStop * stop)
{
  try { 
  AsyncReply *resp = new AsyncReply (async_messages::REPLY,
                                     stop->getKey (),
                                     stop->getRouting (),
                                     0,
                                     stop->op,
                                     async_results::CIM_SERVICE_STOPPED,
                                     stop->resp,
                                     stop->block);
  _completeAsyncResponse (stop, resp, ASYNC_OPSTATE_COMPLETE, 0);
  _die_now = 1;
  } catch (const PEGASUS_STD(bad_alloc) &) {
	cerr << "Out of memory in _handle_stop.	Continuing tests .. " << endl;
  }
}

PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL client_func (void *parm);
PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL server_func (void *parm);

int
main (int argc, char **argv)
{
  verbose = getenv ("PEGASUS_TEST_VERBOSE");

#ifdef PEGASUS_DEBUG
  if (verbose)
  {
    Tracer::setTraceLevel(4);
    Tracer::setTraceComponents("DiscardedData");
    Tracer::setTraceFile("async_callback.trc");
  }
#endif
  try
  {
    Thread client (client_func, (void *) 0, false);

    Thread server (server_func, (void *) 0, false);

    server.run ();
    client.run ();


    client.join ();
    server.join ();
  } catch (const Exception & e)
  {
    cout << "Exception: " << e.getMessage () << endl;

  }
  cout << argv[0] << " +++++ passed all tests" << endl;

  return (0);
}


PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL
client_func (void *parm)
{
  Thread *myself = reinterpret_cast < Thread * >(parm);

  test_async_queue *client = new test_async_queue (test_async_queue::CLIENT);

  // find the server 
  Array < Uint32 > services;
  while (services.size () == 0)
    {
      client->find_services (String ("server"), 0, 0, &services);
      // It is a good idea to yield to other threads. You should do this,
      // but this test-case stresses situations in which does not happend.
      //pegasus_yield ();
    }

  AtomicInt rq_count;
  if (verbose)
    cout << "testing low-level async send " << endl;
  do
    {
      // The problem on multi-processor machines is that if we make it continue on
      // sending the messages, and the MessageQueueService does not get to pickup
      // the messages, the machine can crawl to halt with about 300-400 threads
      // and ever-continuing number of them created. This is an evil stress test
      // so lets leave it behind.
        try {
          Message *cim_rq = new Message (CIM_GET_INSTANCE_REQUEST_MESSAGE);

          AsyncOpNode *op = client->get_op ();
          AsyncOperationStart *async_rq =
            new AsyncOperationStart (client->get_next_xid (),
                                     op,
                                     services[0],
                                     client->getQueueId (),
                                     false,
                                     cim_rq);
          client->SendAsync (op,
                             services[0],
                             test_async_queue::async_handleEnqueue,
                             client, (void *) 0);

        } catch (const PEGASUS_STD(bad_alloc) &)
	{
		cerr <<" Out of memory! Continuing tests." << endl;
	}
      rq_count++;
      // You really ought to allow other threads to their job (like picking up
      // all of these messages, but we  want to stress test unfair circumstances. 
      //pegasus_yield ();
      if (verbose)
        {
          if (test_async_queue::msg_count.value () % 100 == 0)
            cout << (test_async_queue::msg_count.value () /
                     10) << "%% complete" << endl;
        }
    }
  while (test_async_queue::msg_count.value () < 1000);

  if (verbose)
   cout << "Waiting until all messages are flushed. " << endl;
  while (test_async_queue::msg_count.value() != rq_count.value())
  {
	pegasus_yield();
  }
  test_async_queue::msg_count = 0;
  rq_count = 0; 
  if (verbose)
    cout << "testing fast safe async send " << endl;
  do
    {
      // The problem on multi-processor machines is that if we make it continue on
      // sending the messages, and the MessageQueueService does not get to pickup
      // the messages, the machine can crawl to halt with about 300-400 threads
      // and ever-continuing number of them created. 
      try 
        {
          Message *cim_rq = new Message (CIM_GET_INSTANCE_REQUEST_MESSAGE);

          client->SendAsync (cim_rq,
                             services[0],
                             test_async_queue::async_handleSafeEnqueue,
                             client, (void *) NULL);
        } catch (const PEGASUS_STD(bad_alloc) &) {
		cerr <<" Out of memory! Continuing tests." << endl;
	}
      rq_count++;
      //pegasus_yield ();
      if (verbose)
        {
          if (test_async_queue::msg_count.value () % 100 == 0)
            cout << (test_async_queue::msg_count.value () /
                     10) << "%% complete" << endl;
        }

    }
  while (test_async_queue::msg_count.value () < 1000);
  if (verbose)
   cout << "Waiting until all messages are flushed. " << endl;
  while (test_async_queue::msg_count.value() != rq_count.value())
  {
	pegasus_yield();
  }
  if (verbose)
    cout << "sending stop to server " << endl;
  try {
  	CimServiceStop *stop = new CimServiceStop (client->get_next_xid (),
                                             0,
                                             services[0],
                                             client->getQueueId (),
                                             true);

  	AsyncMessage *reply = client->SendWait (stop);
  	delete stop;
  	delete reply;
  } catch (const PEGASUS_STD(bad_alloc) &)
  {
	cerr <<" Out of memory! Continuing tests." << endl;
  }
  // wait for the server to shut down 
  while (services.size () > 0)
    {
      client->find_services (String ("server"), 0, 0, &services);
      pegasus_yield ();
    }


  if (verbose)
    cout << "shutting down client" << endl;

  client->deregister_service ();
  client->_shutdown_incoming_queue ();

  delete client;
  myself->exit_self ((PEGASUS_THREAD_RETURN) 1);
  return (0);
}


PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL
server_func (void *parm)
{
  Thread *myself = reinterpret_cast < Thread * >(parm);

  test_async_queue *server = new test_async_queue (test_async_queue::SERVER);

  while (server->_die_now.value () < 1)
    {
      pegasus_yield ();
    }
  if (verbose)
    cout << "server shutting down" << endl;

  server->deregister_service ();
  server->_shutdown_incoming_queue ();


  delete server;

  myself->exit_self ((PEGASUS_THREAD_RETURN) 1);
  return (0);
}
