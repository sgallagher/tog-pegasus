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

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

const Uint32 module_capabilities::async =   0x00000001;
const Uint32 module_capabilities::remote =  0x00000002;
const Uint32 module_capabilities::trusted = 0x00000004; 

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

PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL cimom::_proc(void *parm)
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


// the cimom's mutex is unlocked upon entry into this routine 

void cimom::handleEnqueue(void)
{
    Message* request = dequeue();

    if (!request)
       return;
   // at a gross level, look at the message and decide if it is for the cimom or
   // for another module
    Uint32 mask = request->getMask(); 
    
    if( mask == message_mask::type_legacy)
    {
       // an existing (pre-asynchronous) message type
       // create an op node, contain the message within the op 
       // node. link the op node to the starting ops queue and put
       // the caller to sleep by waiting on the op node's semaphore.

       // when awakened, pull the response message out of the async op node
       // and enqueue the response back to the caller

    }
    else if (mask & message_mask::type_cimom)
    {
       // a message that must be handled by the cimom 
       if(mask & message_mask::type_control )
       {
	  // a message that we can handle synchronously on the caller's thread
	  switch(request->getType())
	  {
	     case CIMOM_REGISTER_SERVICE:
		register_module(static_cast<CimomRegisterService *>(request));
		break;

		
	     default:
		break;
	  }
	  delete request;
       }
    }
}


void cimom::register_module(CimomRegisterService *msg)
{
   // first see if the module is already registered
   Uint32 result = OK;
   
   if( _modules.exists( reinterpret_cast<void *>(&(msg->name)) ) )
      result = MODULE_ALREADY_REGISTERED;
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
	 result = INTERNAL_ERROR;
      }
   }
   
   Reply *reply = new Reply(msg->getType(), msg->getKey(), result,
			    message_mask::type_cimom | message_mask::ha_reply,
			    msg->getRouting() );
   _enqueueResponse(msg, reply);
   return;
   
}

void cimom::deregister_module(CimomDeregisterService *msg)
{
   
   Uint32 result = OK;
   
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
      result = MODULE_NOT_FOUND;
   else
      delete temp;
      
   Reply *reply = new Reply ( msg->getType(), msg->getKey(), result, 
			      message_mask::type_cimom | message_mask::ha_reply,
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
