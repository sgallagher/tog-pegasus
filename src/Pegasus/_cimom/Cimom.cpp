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

#include "Cimom.h"


PEGASUS_NAMESPACE_BEGIN

Uint32 module_capabilities::async =   0x00000001;
Uint32 module_capabilities::remote =  0x00000002;
Uint32 module_capabilities::trusted = 0x00000004;


cimom::~cimom(void)
{
   _die++;
   
   _pending_ops.empty_list();
   _pending_ops.shutdown_queue();
   _completed_ops.empty_list();
   _completed_ops.shutdown_queue(); 


// send STOP messages to all modules
// shutdown legacy queues; e.g., cim operation dispatcher etc.
   return;
   
}


void cimom::_enqueueResponse(
    Request* request,
    Reply* response)
{
    // Use the same key as used in the request:

    response->setKey(request->getKey());
    // Lookup the message queue:

    MessageQueue* queue = MessageQueue::lookup(request->queues.top());
    PEGASUS_ASSERT(queue != 0);

    // Enqueue the response:

    queue->enqueue(response);
}



PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL cimom::_pending_proc(void *parm)
{
   Thread *myself = reinterpret_cast<Thread *>(parm);
   cimom *cim_manager = reinterpret_cast<cimom *>(myself->get_parm());
   
   while( 0 == cim_manager->_die.value() )
   {
      myself->sleep(1);
   }
   
   myself->exit_self( (PEGASUS_THREAD_RETURN) 1 );
   return(0);
}

PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL cimom::_completed_proc(void *parm)
{
   Thread *myself = reinterpret_cast<Thread *>(parm);
   cimom *cim_manager = reinterpret_cast<cimom *>(myself->get_parm());
   while( 0 == cim_manager->_die.value() )
   {
      myself->sleep(1);
   }
   
   myself->exit_self( (PEGASUS_THREAD_RETURN) 1 );
   return(0);
}



// the cimom's mutex is UNLOCKED upon entry into this routine
void cimom::handleEnqueue(void)
{

    Message* request = dequeue();

    if (!request)
       return;

    if( _die.value() != 0 )
    {
       delete request;
       return;
    }
    

    //----- PREPROCESSING -----//
    // at a gross level, look at the message and decide if it is for the cimom or
    // for another module
    Uint32 mask = request->getMask();
    AsyncOpNode *op;
    
    if(mask & message_mask::type_cimom)
    {
       // should be almost all reply messages 
       _internal_ops.insert_last(request);
       return;
    }
    else if(mask & message_mask::ha_async)
    {
       // an async request or reply 
       op = (static_cast<AsyncRequest *>(request))->op;
       if(op == 0)
       {
	  // something is wrong, just drop this message
	  delete request;
	  return;
       }

       // if this is a request, ensure that the opnode and
       // message are mutually referrent
       if(mask & message_mask::ha_request)
	  op->put_request(request);
    }
    else if ( mask & message_mask::type_legacy )
    {
       // create an asynchronous "envelope" for this message
       op = new AsyncOpNode();
       
       op->_flags = (ASYNC_OPFLAGS_NORMAL | ASYNC_OPFLAGS_SINGLE );
       op->_state = ASYNC_OPFLAGS_UNKNOWN;

       // place the original (legacy) request within the "envelope"
       op->put_request(request);
       

       ServiceAsyncOpStart *async_request = 
	  new ServiceAsyncOpStart(Message::getNextKey(),
				  QueueIdStack(_CIMOM_Q_ID, _CIMOM_Q_ID),
				  op);

       // redirect the message pointer to point the the "envelope"
       request = static_cast<Message *>(async_request);
       // reinitialize the mask to reflect the envelope
       mask = request->getMask();
       
       // NOTE: there is no mutual reference between the request and the opnode:
       // the reference map is as follows:
       // request->op->legacy_request 
    }
    

    //----- ROUTING -----//
    if(mask & message_mask::ha_request)
    {
       // now give each registered module a chance to handle this request message
       // for now we bail after one module has accepted the message. 
       // in the future, multiple modules can concurrently handle the
       // same message

       MessageQueue *dst_q;
       
       _modules.lock();
       message_module *module = _modules.next(0);
       while(module != 0)
       {
	  dst_q = MessageQueue::lookup(module->_q_id);
	  if (dst_q != 0 )
	  {
	     if( true == dst_q->accept_async(request))
	     {
		op->_state |= ASYNC_OPSTATE_ACCEPTED;
		break;
	     }
	     
	  }
	  module = _modules.next(module);
       }
       _modules.unlock();
       
       if(op->_state & ASYNC_OPSTATE_ACCEPTED)
	  _pending_ops.insert_last(op);
       else
       {
	  delete request;
	  return;
       }
    }
    else if(mask & message_mask::ha_reply)
    {
       // replies should get handled by the cimom on its internal ops queue 
       _internal_ops.insert_last(request);
       return;
    }
    else 
    {
       delete request;
    }
    
    return;
}


void cimom::handle_internal(AsyncOpNode *internal_op)
{
   return;
}


void cimom::register_module(CimomRegisterService *msg)
{
   // first see if the module is already registered
   Uint32 result = cimom_results::OK;

   if( _modules.exists( reinterpret_cast<void *>(&(msg->name))))
      result = cimom_results::MODULE_ALREADY_REGISTERED;
   else
   {
      message_module *new_mod =  new message_module(msg->name,
						    msg->capabilities,
						    msg->mask,
						    msg->q_id);
      try
      {
	 _modules.insert_first(new_mod);
      }
      catch(IPCException& e)
      {
	 result = cimom_results::INTERNAL_ERROR;
      }
   }

   Reply *reply = new Reply(msg->getType(), msg->getKey(), result,
			    message_mask::type_service | message_mask::ha_reply,
			    msg->getRouting() );
   _enqueueResponse(msg, reply);
   return;

}

void cimom::deregister_module(CimomDeregisterService *msg)
{

   Uint32 result = cimom_results::OK;

   _modules.lock();
   message_module *temp = _modules.next(0);
   while( temp != 0 )
   {
      if (temp->_q_id == msg->q_id)
	 break;
      temp = _modules.next(temp);
   }
   _modules.remove_no_lock((void *)temp);
   _modules.unlock();

   if(temp == 0)
      result = cimom_results::MODULE_NOT_FOUND;
   else
      delete temp;

   Reply *reply = new Reply ( msg->getType(), msg->getKey(), result,
			      message_mask::type_service | message_mask::ha_reply,
			      msg->getRouting() );
   _enqueueResponse(msg, reply);
   return;
}

Uint32 cimom::get_module_q(const String & name)
{
   _modules.lock();
   message_module *ret = _modules.next( 0 );
   while( ret != 0 )
   {
      if (ret == name )
	 break;
      ret = _modules.next(ret);
   }

   _modules.unlock();
   if(ret != 0 )
      return ret->_q_id;
   else
      return 0 ;
}

PEGASUS_NAMESPACE_END



