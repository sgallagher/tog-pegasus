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
   
   _new_ops.empty_list();
   _new_ops.shutdown_queue();
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





// << Thu Dec 27 15:51:47 2001 mdd >>
// get rid of the new list, just use the pending list
// in handle_enqueue:
// if an async message, link msg->op to pending queue; call accept_async() for each recipient
//  accept_async is guaranteed to return without calling handleEnqueue()
// else (not async)
//   create an async_op message
//   create an asyncopnode, link to async_op message
//    link original synchronous message to asyncopnode
//    link to op node to pending queue
//    handle in separate thread by calling enqueue() for the recipient 
//    recipient extracts opnode from async_op messsage
//    recipient deletes async_op message, extracts original message 
//    from opnode, handles message synchronously, updates op node, deletes message
//
//

PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL cimom::_new_proc(void *parm)
{
   Thread *myself = reinterpret_cast<Thread *>(parm);
   cimom *cim_manager = reinterpret_cast<cimom *>(myself->get_parm());
   Uint32 *module_ids;
   Uint32 id_buffer_size  = 5 + cim_manager->getModuleCount();
   module_ids = new Uint32[ id_buffer_size ] ;


   // all of this code is just a way to cache the q ids of registered modules
   // so we don't need to lock the module list every time we need a queue ID

   Uint32 module_count = cim_manager->getModuleIDs(module_ids, id_buffer_size );
   
   if( module_count > id_buffer_size )
   {
      delete module_ids;
      id_buffer_size = 3 + cim_manager->getModuleCount() ;
      module_ids = new Uint32[ id_buffer_size ];
      module_count = cim_manager->getModuleIDs(module_ids, id_buffer_size );
   }
   
   while( 0 == cim_manager->_die.value() )
   {

      struct timeval now;
      pegasus_gettimeofday(&now);
      
      // always refresh our array of q ids for registered modules, it 
      // could be different each time through the loop !
      module_count = cim_manager->getModuleIDs(module_ids, id_buffer_size );
      if( module_count > id_buffer_size )
      {
	 delete module_ids;
	 id_buffer_size = 1 + cim_manager->getModuleCount() ;
	 module_ids = new Uint32[ id_buffer_size ];
	 module_count = cim_manager->getModuleIDs(module_ids, id_buffer_size );
      }
      
      AsyncOpNode *work ;
            
      // try to pull an async op node of the new  list 
      work = cim_manager->_new_ops.remove_first();
      while  (work != 0 )
      {
	 Message *msg = const_cast<Message *>(work->get_request());
	 if(msg != 0)
	 {
	    work->_start.tv_sec = now.tv_sec;
	    work->_start.tv_usec = now.tv_usec;
	    
	    MessageQueue *dst_q;
	    Uint32 i;
	    if( msg->getMask() & (message_mask::type_service | message_mask::type_broadcast ))
	    {
	       // contains a message to one of the registered modules
	       // use mask to determine modules that may handle this 
	       // message. Give each eligible module the opportunity to 
	       // handle the message. More than one module may choose to do so. 
	       
	       
	       for( i = 2; i < id_buffer_size ; i++)
	       {
		  dst_q = MessageQueue::lookup( module_ids[i] );
		  if( dst_q != 0 )
		  {
		     work->_state |= ASYNC_OPSTATE_OFFERED;
		     if( true == dst_q->accept_async(msg)) 
			work->_state |= ASYNC_OPSTATE_ACCEPTED;
		  }
		  else
		     break;
	       }
	    }
	    else if( msg->getMask() & message_mask::type_legacy)
	    {
	       // one of the CIM-related messages in Pegasus
	       // start an asynchronous operation by creating and enqueuing a start_async message
	       // ATTN << Thu Dec 27 16:17:49 2001 mdd >>
	       ServiceAsyncOpStart *async_msg = 
		  new ServiceAsyncOpStart( Message::getNextKey(),
					   QueueIdStack( MessageQueue::_CIMOM_Q_ID, MessageQueue::_CIMOM_Q_ID ),
					   work );
	       
	       work->_state |= ASYNC_OPSTATE_OFFERED;
	       if(true  == cim_manager->_cim_dispatcher->accept_async(async_msg))
		  work->_state |= ASYNC_OPSTATE_ACCEPTED;
	       else
	       {
		  work->_state |= ASYNC_OPSTATE_CANCELLED ;
		  delete async_msg;
	       }
	    }
	 }
	 else 
	    work->_state |= ASYNC_OPSTATE_CANCELLED;
	 if( ! (work->_state & ASYNC_OPSTATE_ACCEPTED) )
	    work->_state |= ASYNC_OPSTATE_CANCELLED;
	 
	 if(work->_state & ASYNC_OPSTATE_CANCELLED)
	 {
	    cim_manager->_completed_ops.insert_last_wait(work);
	 }
	 work = cim_manager->_new_ops.remove_first();
      } // while work on the new ops list

      // now that all the work is dispatched, handle internal messages 
      work = cim_manager->_internal_ops.remove_first();
      while(work != 0)
      {
	 cim_manager->handle_internal(work);
	 work = cim_manager->_internal_ops.remove_first();
      }
   } // while alive 
   
   myself->exit_self( (PEGASUS_THREAD_RETURN) 1 );
   return(0);
}
   



PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL cimom::_pending_proc(void *parm)
{
   Thread *myself = reinterpret_cast<Thread *>(parm);
   cimom *cim_manager = reinterpret_cast<cimom *>(myself->get_parm());
   Uint32 *module_ids;
   Uint32 id_buffer_size  = 5 + cim_manager->getModuleCount();
   module_ids = new Uint32[ id_buffer_size ] ;


   // all of this code is just a way to cache the q ids of registered modules
   // so we don't need to lock the module list every time we need a queue ID

   Uint32 module_count = cim_manager->getModuleIDs(module_ids, id_buffer_size );
   
   if( module_count > id_buffer_size )
   {
      delete module_ids;
      id_buffer_size = 3 + cim_manager->getModuleCount() ;
      module_ids = new Uint32[ id_buffer_size ];
      module_count = cim_manager->getModuleIDs(module_ids, id_buffer_size );
   }
   
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
   Uint32 *module_ids;
   Uint32 id_buffer_size  = 5 + cim_manager->getModuleCount();
   module_ids = new Uint32[ id_buffer_size ] ;


   // all of this code is just a way to cache the q ids of registered modules
   // so we don't need to lock the module list every time we need a queue ID

   Uint32 module_count = cim_manager->getModuleIDs(module_ids, id_buffer_size );
   
   if( module_count > id_buffer_size )
   {
      delete module_ids;
      id_buffer_size = 3 + cim_manager->getModuleCount() ;
      module_ids = new Uint32[ id_buffer_size ];
      module_count = cim_manager->getModuleIDs(module_ids, id_buffer_size );
   }
   
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
    
    
   // at a gross level, look at the message and decide if it is for the cimom or
   // for another module
    Uint32 mask = request->getMask();
    if(mask & message_mask::ha_async)
    {
       AsyncOpNode *op = (static_cast<AsyncRequest *>(request))->op;
       if (mask & (message_mask::type_cimom | message_mask::type_broadcast))
       {
	  op->write_state( ASYNC_OPSTATE_UNKNOWN ) ;
	  op->write_flags( ASYNC_OPFLAGS_NORMAL | ASYNC_OPFLAGS_SINGLE);
	  _internal_ops.insert_last(op);
       } // control message for the cimom
       else 
       {
	  op->write_state( ASYNC_OPSTATE_UNKNOWN ) ;
	  op->write_flags( ASYNC_OPFLAGS_NORMAL | ASYNC_OPFLAGS_SINGLE);
	  _new_ops.insert_last_wait(op);
       }
    }


    else if ( mask & message_mask::type_legacy )
    {
       
//       _new_ops.insert_last_wait( op_node ) ;
       
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



