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
#include "MessageQueueService.h"
PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

typedef HashTable<Uint32, MessageQueue*, EqualFunc<Uint32>, HashFunc<Uint32> >
    QueueTable;

static QueueTable _queueTable(128);
static Mutex q_table_mut ;

Uint32 MessageQueue::getNextQueueId() throw(IPCException)
{
   static Uint32 _nextQueueId = 2;

   //
   // Lock mutex:
   //

   static Mutex _id_mut ;
   _id_mut.lock(pegasus_thread_self());

   // Assign next queue id. Handle wrap around and never assign zero as
   // a queue id:

   if (_nextQueueId == 0)
      _nextQueueId = 2;

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
   : _queueId(queueId), _capabilities(0), _count(0), _front(0), _back(0), _async(async)
{
    //
    // Copy the name:
    //

   PEG_METHOD_ENTER(TRC_MESSAGEQUEUESERVICE,"MessageQueue::MessageQueue()");

    if (!name)
	name = ""; 

    _name = new char[strlen(name) + 1];
    strcpy(_name, name);

    Tracer::trace(TRC_MESSAGEQUEUESERVICE, Tracer::LEVEL3,
        "MessageQueue::MessageQueue  name = %s, queueId = %i", name, queueId);

    //
    // Insert into queue table:
    //

    q_table_mut.lock(pegasus_thread_self());

    while (!_queueTable.insert(_queueId, this))
       ;

    q_table_mut.unlock();

    
   PEG_METHOD_EXIT();
}

MessageQueue::~MessageQueue()
{
    // ATTN-A: thread safety!

    PEG_METHOD_ENTER(TRC_MESSAGEQUEUESERVICE,"MessageQueue::~MessageQueue()");

    Tracer::trace(TRC_MESSAGEQUEUESERVICE, Tracer::LEVEL3,
        "MessageQueue::~MessageQueue queueId = %i, name = %s", _queueId, _name);

    q_table_mut.lock(pegasus_thread_self());

    _queueTable.remove(_queueId);
    q_table_mut.unlock();
	
    // Free the name:
    
    delete [] _name;

    PEG_METHOD_EXIT();
}

void MessageQueue::enqueue(Message* message) throw(IPCException)
{
    PEG_METHOD_ENTER(TRC_MESSAGEQUEUESERVICE,"MessageQueue::enqueue()");

    if (!message) 
    {
       Tracer::trace(TRC_MESSAGEQUEUESERVICE, Tracer::LEVEL3,
        "MessageQueue::enqueue failure");
       PEG_METHOD_EXIT();
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
    Tracer::trace(TRC_MESSAGEQUEUESERVICE, Tracer::LEVEL4,
		  "MessageQueue::enqueue _queueId = %d, _count = %d", _queueId, _count);
       
    _mut.unlock();
    
    handleEnqueue();
    PEG_METHOD_EXIT();
}

Message* MessageQueue::dequeue() throw(IPCException)
{
    PEG_METHOD_ENTER(TRC_MESSAGEQUEUESERVICE,"MessageQueue::dequeue()");

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
        Tracer::trace(TRC_MESSAGEQUEUESERVICE, Tracer::LEVEL4,
            "MessageQueue::dequeue _queueId = %d, _count = %d", 
            _queueId, _count);

	_mut.unlock();
	message->_next = 0;
	message->_prev = 0;
	message->_owner = 0;

        PEG_METHOD_EXIT();
	return message;
    }
    _mut.unlock();

    PEG_METHOD_EXIT();
    return 0;
}
;


void MessageQueue::remove(Message* message) throw(IPCException)
{
    PEG_METHOD_ENTER(TRC_MESSAGEQUEUESERVICE,"MessageQueue::remove()");

    if (!message)
    {
        PEG_METHOD_EXIT();
	throw NullPointer();
    }

    if (message->_owner != this)
    {
        PEG_METHOD_EXIT();
	throw NoSuchMessageOnQueue();
    }

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
    Tracer::trace(TRC_MESSAGEQUEUESERVICE, Tracer::LEVEL4,
       "MessageQueue::remove _count = %d", _count);

    _mut.unlock();

    message->_prev = 0;
    message->_next = 0;
    message->_owner = 0;

    PEG_METHOD_EXIT();
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
   return _name;
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

    Tracer::trace(TRC_MESSAGEQUEUESERVICE, Tracer::LEVEL3,
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
      if(! strcmp( ((MessageQueue *)i.value())->getQueueName(), name) )
      {
	 q_table_mut.unlock();
	 return( (MessageQueue *)i.value());
      }

   }
   q_table_mut.unlock();

   Tracer::trace(TRC_MESSAGEQUEUESERVICE, Tracer::LEVEL3,
        "MessageQueue::lookup failure - name = %s", name);

   return 0;
}


void MessageQueue::handleEnqueue()
{

}

PEGASUS_NAMESPACE_END
