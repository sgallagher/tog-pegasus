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
#include "MessageQueue.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

typedef HashTable<Uint32, MessageQueue*, EqualFunc<Uint32>, HashFunc<Uint32> >
    QueueTable;

static QueueTable _queueTable(128);

static Uint32 _GetNextQueueId()
{
   static Mutex _mut;
   
   static Uint32 _queueId = 2;

   _mut.lock(pegasus_thread_self());
   
   // Handle wrap-around!
   if (_queueId == 0)
      _queueId = MessageQueue::_CIMOM_Q_ID;
   Uint32 ret = _queueId++;
   _mut.unlock();
   
   return ret;
}

Uint32 MessageQueue::_CIMOM_Q_ID = 1;

MessageQueue::MessageQueue() : _count(0), _front(0), _back(0)
{
    // ATTN-A: thread safety!

   memset(_name, 0x00, 16);
    while (!_queueTable.insert(_queueId = _GetNextQueueId(), this))
	;
}

MessageQueue::MessageQueue(char *name) : _count(0), _front(0), _back(0)
{
   if(name != NULL)
   {
      strncpy(_name, name, 15);
      _name[15] = 0x00;
   }
   
   else
      memset(_name, 0x00,16);
    while (!_queueTable.insert(_queueId = _GetNextQueueId(), this))
       ;
}


MessageQueue::~MessageQueue()
{
    // ATTN-A: thread safety!

    _queueTable.remove(_queueId);
}

void MessageQueue::enqueue(Message* message)
{
    if (!message)
	throw NullPointer();

    if (getenv("PEGASUS_TRACE"))
    {
	cout << "===== " << getQueueName() << ": ";
	message->print(cout);
    }

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

    handleEnqueue();
}

Message* MessageQueue::dequeue()
{
    if (_front)
    {
	Message* message = _front;
	_front = _front->_next;
	if (_front)
	    _front->_prev = 0;

	if (_back == message)
	    _back = 0;
	
	message->_next = 0;
	message->_prev = 0;
	message->_owner = 0;
	_count--;

	return message;
    }
    return 0;
}

void MessageQueue::remove(Message* message)
{
    if (!message)
	throw NullPointer();

    if (message->_owner != this)
	throw NoSuchMessageOnQueue();

    if (message->_next)
	message->_next->_prev = message->_prev;
    else
	_back = message->_prev;

    if (message->_prev)
	message->_prev->_next = message->_next;
    else
	_front = message->_next;

    message->_prev = 0;
    message->_next = 0;
    message->_owner = 0;
    _count--;
}

Message* MessageQueue::findByType(Uint32 type)
{
    for (Message* m = front(); m; m = m->getNext())
    {
	if (m->getType() == type)
	    return m;
    }

    return 0;
}

Message* MessageQueue::findByKey(Uint32 key)
{
    for (Message* m = front(); m; m = m->getNext())
    {
	if (m->getKey() == key)
	    return m;
    }

    return 0;
}

void MessageQueue::print(ostream& os) const
{
    for (const Message* m = front(); m; m = m->getNext())
	m->print(os);
}

Message* MessageQueue::find(Uint32 type, Uint32 key)
{
    for (Message* m = front(); m; m = m->getNext())
    {
	if (m->getType() == type && m->getKey() == key)
	    return m;
    }

    return 0;
}

void MessageQueue::lock()
{

}

void MessageQueue::unlock()
{

}

const char* MessageQueue::getQueueName() const
{
   if(_name[0] != 0x00)
      return _name;
   return "unknown";
}

MessageQueue* MessageQueue::lookup(Uint32 queueId)
{
    MessageQueue* queue = 0;

    if (_queueTable.lookup(queueId, queue))
	return queue;

    // Not found!
    return 0;
}


MessageQueue* MessageQueue::lookup(const char *name)
{
   if(name == NULL)
      throw NullPointer();
   for(QueueTable::Iterator i = _queueTable.start(); i; i++)
   {
        // ATTN: Need to decide how many characters to compare in queue names
	if(! strncmp( ((MessageQueue *)i.value())->getQueueName(), name, 25) )
	    return( (MessageQueue *)i.value());
   }
   return 0;
}


void MessageQueue::handleEnqueue()
{

}

PEGASUS_NAMESPACE_END
