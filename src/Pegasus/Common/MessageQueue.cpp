//%/////////////////////////////////////////////////////////////////////////////
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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/HashTable.h>
#include <Pegasus/Common/IPC.h>
#include <Pegasus/Common/Tracer.h>
#include "MessageQueue.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

typedef HashTable<Uint32, MessageQueue*, EqualFunc<Uint32>, HashFunc<Uint32> >
    QueueTable;

static QueueTable _queueTable(128);
static Mutex q_table_mut = Mutex();

Uint32 MessageQueue::getNextQueueId() throw(IPCException)
{
   static Uint32 _nextQueueId = 1;

   //
   // Lock mutex:
   //

   static Mutex _id_mut = Mutex();
   _id_mut.lock(pegasus_thread_self());

   // Assign next queue id. Handle wrap around and never assign zero as
   // a queue id:

   if (_nextQueueId == 0)
      _nextQueueId = 1;

   Uint32 queueId = _nextQueueId++;

   //
   // Unlock mutex:
   //

   _id_mut.unlock();

   return queueId;
}

MessageQueue::MessageQueue(
    const char* name, 
    Boolean async,
    Uint32 queueId)
    : 
    _queueId(queueId), _count(0), _front(0), _back(0), _async(async), 
    _workThread(MessageQueue::workThread, this, false), _workSemaphore(0)
{
    //
    // Copy the name:
    //

   PEG_FUNC_ENTER(TRC_DISPATCHER,"MessageQueue::MessageQueue()");

    if (!name)
	name = "";

    _name = new char[strlen(name) + 1];
    strcpy(_name, name);

    Tracer::trace(TRC_DISPATCHER, Tracer::LEVEL3,
        "MessageQueue::MessageQueue  name = %s, queueId = %i", name, queueId);

    //
    // Insert into queue table:
    //

    q_table_mut.lock(pegasus_thread_self());

    while (!_queueTable.insert(_queueId, this))
       ;

    q_table_mut.unlock();

    
    if(_async == true)
       _workThread.run();

   PEG_FUNC_EXIT(TRC_DISPATCHER,"MessageQueue::MessageQueue()");
}

MessageQueue::~MessageQueue()
{
    // ATTN-A: thread safety!

    PEG_FUNC_ENTER(TRC_DISPATCHER,"MessageQueue::~MessageQueue()");

    Tracer::trace(TRC_DISPATCHER, Tracer::LEVEL3,
        "MessageQueue::~MessageQueue queueId = %i, name = %s", _queueId, _name);

    q_table_mut.lock(pegasus_thread_self());

    _queueTable.remove(_queueId);
    q_table_mut.unlock();
	
    if(_async == true)
    {
       _workThread.cancel();	// cancel thread
       _workSemaphore.signal(); // wake thread
       _workThread.join();      // wait for thread to complete
    }

    // Free the name:
    
    delete [] _name;

    PEG_FUNC_EXIT(TRC_DISPATCHER,"MessageQueue::~MessageQueue()");
}


PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL MessageQueue::workThread(void * arg)
{

	PEG_FUNC_ENTER(TRC_DISPATCHER,"MessageQueue::workThread()");

	// get thread from argument
	Thread * thread = (Thread *)arg;

	PEGASUS_ASSERT(thread != 0);

	// get message queue from thread
	MessageQueue * queue = (MessageQueue *)thread->get_parm();
	
	PEGASUS_ASSERT(queue != 0);

	while(true)
	{
	   if(thread->is_cancelled())
	   {
	      break;
	   }
	   
	   // wait for work
	   queue->_workSemaphore.wait();
	   
	   // stop the thread when the message queue has been destroyed.
	   // ATTN: should check the thread cancel flag that is not yet exposed!
	   if(MessageQueue::lookup(queue->_queueId) == 0)
	   {
             Tracer::trace(TRC_DISPATCHER, Tracer::LEVEL3,
                     "MessageQueue::workThread - ", 
                     "Message queue, %i, for thread no longer exists",
                     queue->_queueId);
	      break;
	   }
	   
	   // ensure the queue has a message before dispatching
	   if(queue->_count != 0)
	   {
	      queue->handleEnqueue();
	   }
	}

	PEG_FUNC_EXIT(TRC_DISPATCHER,"MessageQueue::workThread()");

	thread->exit_self(PEGASUS_THREAD_RETURN(0));
	
	return(0);
}

void MessageQueue::enqueue(Message* message) throw(IPCException)
{

    if (!message) 
    {
       Tracer::trace(TRC_DISPATCHER, Tracer::LEVEL3,
        "MessageQueue::enqueue failure");
       throw NullPointer();
    }

    if (getenv("PEGASUS_TRACE"))
    {
       cout << "===== " << getQueueName() << ": ";
       message->print(cout);
    }

    _mut.lock(pegasus_thread_self());
    if (_back)
    {
       _back->_next = message;
       message->_prev = _back;
       message->_next = 0;
       _back = message;
    }
    else
    {
       _front = message;
       _back = message;
       message->_prev = 0;
       message->_next = 0;
    }
    message->_owner = this;
    _count++;
    if( _async == true )
    {
       _workSemaphore.signal();
       
    }
       
    _mut.unlock();

    if(_async == false )
       handleEnqueue();

}


Message* MessageQueue::dequeue() throw(IPCException)
{
   _mut.lock(pegasus_thread_self());
    if (_front)
    {
	Message* message = _front;
	_front = _front->_next;
	if (_front)
	    _front->_prev = 0;

	if (_back == message)
	    _back = 0;
	_count--;
	_mut.unlock();
	message->_next = 0;
	message->_prev = 0;
	message->_owner = 0;
	return message;
    }
    _mut.unlock();
    return 0;
}

void MessageQueue::remove(Message* message) throw(IPCException)
{
    if (!message)
	throw NullPointer();

    if (message->_owner != this)
	throw NoSuchMessageOnQueue();

    _mut.lock(pegasus_thread_self());

    if (message->_next)
	message->_next->_prev = message->_prev;
    else
	_back = message->_prev;

    if (message->_prev)
	message->_prev->_next = message->_next;
    else
	_front = message->_next;

    _count--;
    _mut.unlock();

    message->_prev = 0;
    message->_next = 0;
    message->_owner = 0;
}

Message* MessageQueue::findByType(Uint32 type) throw(IPCException)
{
   _mut.lock(pegasus_thread_self());

    for (Message* m = front(); m; m = m->getNext())
    {
       if (m->getType() == type)
       {
	  _mut.unlock();
	  return m;
       }
    }
    _mut.unlock();
    return 0;
}

Message* MessageQueue::findByKey(Uint32 key) throw(IPCException)
{
   _mut.lock(pegasus_thread_self());

    for (Message* m = front(); m; m = m->getNext())
    {
       if (m->getKey() == key)
       {
	  _mut.unlock();
	  return m;
       }

    }
    _mut.unlock();
    return 0;
}

void MessageQueue::print(ostream& os) const throw(IPCException)
{
   const_cast<MessageQueue *>(this)->_mut.lock(pegasus_thread_self());

   for (const Message* m = front(); m; m = m->getNext())
	m->print(os);
   const_cast<MessageQueue *>(this)->_mut.unlock();
}

Message* MessageQueue::find(Uint32 type, Uint32 key) throw(IPCException)
{
   _mut.lock(pegasus_thread_self());

    for (Message* m = front(); m; m = m->getNext())
    {
       if (m->getType() == type && m->getKey() == key)
       {
	  _mut.unlock();
	  return m;
       }
    }
    _mut.unlock();

    return 0;
}

void MessageQueue::lock() throw(IPCException)
{
   _mut.lock(pegasus_thread_self());
}

void MessageQueue::unlock()
{
   _mut.unlock();
}

const char* MessageQueue::getQueueName() const
{
   if(_name[0] != 0x00)
      return _name;
   return "unknown";
}

MessageQueue* MessageQueue::lookup(Uint32 queueId) throw(IPCException)
{

    MessageQueue* queue = 0;
    q_table_mut.lock(pegasus_thread_self());

    if (_queueTable.lookup(queueId, queue))
    {
       q_table_mut.unlock();
       return queue;
    }

    // Not found!

    q_table_mut.unlock();

    Tracer::trace(TRC_DISPATCHER, Tracer::LEVEL3,
        "MessageQueue::lookup failure queueId = %i", queueId);

    return 0;
}


MessageQueue* MessageQueue::lookup(const char *name) throw(IPCException)
{

   if(name == NULL)
      throw NullPointer();
   q_table_mut.lock(pegasus_thread_self());

   for(QueueTable::Iterator i = _queueTable.start(); i; i++)
   {
        // ATTN: Need to decide how many characters to compare in queue names
      if(! strncmp( ((MessageQueue *)i.value())->getQueueName(), name, 25) )
      {
	 q_table_mut.unlock();
	 return( (MessageQueue *)i.value());
      }

   }
   q_table_mut.unlock();

   Tracer::trace(TRC_DISPATCHER, Tracer::LEVEL3,
        "MessageQueue::lookup failure - name = %s", name);

   return 0;
}


void MessageQueue::handleEnqueue()
{

}

PEGASUS_NAMESPACE_END
