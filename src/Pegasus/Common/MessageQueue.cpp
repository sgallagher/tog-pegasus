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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By: Amit K Arora, IBM (amita@in.ibm.com) for Bug#1090
//              Josephine Eskaline Joyce, IBM (jojustin@in.ibm.com) for Bug#2076
//              David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
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

static QueueTable _queueTable(256);
static Mutex q_table_mut ;

void MessageQueue::remove_myself(Uint32 qid)
{
    AutoMutex autoMut(q_table_mut);
    _queueTable.remove(qid);
}


Uint32 MessageQueue::getNextQueueId()
{
    static Uint32 _nextQueueId = 2;

    //
    // Lock mutex:
    //
    static Mutex _id_mut ;
    AutoMutex autoMut(_id_mut);

    Uint32 queueId;

    // Assign the next queue ID that is not already in use
    do
    {
        // Handle wrap around and never assign zero or one as a queue id:
        if (_nextQueueId == 0)
        {
            _nextQueueId = 2;
        }

        queueId = _nextQueueId++;
    } while (lookup(queueId) != 0);

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
        "MessageQueue::MessageQueue  name = %s, queueId = %u", name, queueId);

    //
    // Insert into queue table:
    //
    AutoMutex autoMut(q_table_mut);
    while (!_queueTable.insert(_queueId, this))
        ;

    PEG_METHOD_EXIT();
}

MessageQueue::~MessageQueue()
{
    // ATTN-A: thread safety!
    PEG_METHOD_ENTER(TRC_MESSAGEQUEUESERVICE,"MessageQueue::~MessageQueue()");
    Tracer::trace(TRC_MESSAGEQUEUESERVICE, Tracer::LEVEL3,
        "MessageQueue::~MessageQueue queueId = %i, name = %s", _queueId, _name);

    {
        AutoMutex autoMut(q_table_mut);
        _queueTable.remove(_queueId);
    } // mutex unlocks here

    // Free the name:

    delete [] _name;

    while(_front)
    {
       Message* tmp = _front;
       _front = _front->_next;
       delete tmp;
    }

    PEG_METHOD_EXIT();
}

void MessageQueue::enqueue(Message* message)
{
    PEG_METHOD_ENTER(TRC_MESSAGEQUEUESERVICE,"MessageQueue::enqueue()");

    if (!message)
    {
        Tracer::trace(TRC_MESSAGEQUEUESERVICE, Tracer::LEVEL3,
                    "MessageQueue::enqueue failure");
        PEG_METHOD_EXIT();
        throw NullPointer();
    }

    PEG_TRACE_STRING( TRC_MESSAGEQUEUESERVICE, Tracer::LEVEL3,
                      String("Queue name: ") + getQueueName() ) ;
    Tracer::trace   ( TRC_MESSAGEQUEUESERVICE,
                      Tracer::LEVEL3,
                      "Message: [%s, %d]",
                      MessageTypeToString(message->getType()),
                      message->getKey() );

    {
    AutoMutex autoMut(_mut);
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

    } // mutex unlocks here

    handleEnqueue();
    PEG_METHOD_EXIT();
}

Message* MessageQueue::dequeue()
{
    PEG_METHOD_ENTER(TRC_MESSAGEQUEUESERVICE,"MessageQueue::dequeue()");

    AutoMutex autoMut(_mut);
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

        message->_next = 0;
        message->_prev = 0;
        message->_owner = 0;

        PEG_METHOD_EXIT();
        return message;
    }

    PEG_METHOD_EXIT();
    return 0;
}



void MessageQueue::remove(Message* message)
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

    {
    AutoMutex autoMut(_mut);

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

    } // mutex unlocks here

    message->_prev = 0;
    message->_next = 0;
    message->_owner = 0;

    PEG_METHOD_EXIT();
}

Message* MessageQueue::findByType(Uint32 type)
{
    AutoMutex autoMut(_mut);

    for (Message* m = front(); m; m = m->getNext())
    {
        if (m->getType() == type)
        {
            return m;
        }
    }

    return 0;
}

Message* MessageQueue::findByKey(Uint32 key)
{
    AutoMutex autoMut(_mut);

    for (Message* m = front(); m; m = m->getNext())
    {
       if (m->getKey() == key)
       {
          return m;
       }

    }

    return 0;
}

#ifdef PEGASUS_DEBUG
void MessageQueue::print(ostream& os) const
{
    AutoMutex autoMut(const_cast<MessageQueue *>(this)->_mut);

    for (const Message* m = front(); m; m = m->getNext())
        m->print(os);
}
#endif

Message* MessageQueue::find(Uint32 type, Uint32 key)
{
    AutoMutex autoMut(_mut);

    for (Message* m = front(); m; m = m->getNext())
    {
        if (m->getType() == type && m->getKey() == key)
        {
            return m;
        }
    }

    return 0;
}

const char* MessageQueue::getQueueName() const
{
    return _name;
}

MessageQueue* MessageQueue::lookup(Uint32 queueId)
{

    MessageQueue* queue = 0;
    AutoMutex autoMut(q_table_mut);

    if (_queueTable.lookup(queueId, queue))
    {
        return queue;
    }

    // Not found!

    Tracer::trace(TRC_MESSAGEQUEUESERVICE, Tracer::LEVEL3,
        "MessageQueue::lookup failure queueId = %u", queueId);

    return 0;
}


MessageQueue* MessageQueue::lookup(const char *name)
{

    if(name == NULL)
        throw NullPointer();

    AutoMutex autoMut(q_table_mut);
   for(QueueTable::Iterator i = _queueTable.start(); i; i++)
   {
        // ATTN: Need to decide how many characters to compare in queue names
        if(! strcmp( ((MessageQueue *)i.value())->getQueueName(), name) )
        {
            return( (MessageQueue *)i.value());
        }
    }

    Tracer::trace(TRC_MESSAGEQUEUESERVICE, Tracer::LEVEL3,
                    "MessageQueue::lookup failure - name = %s", name);

    return 0;
}


void MessageQueue::handleEnqueue()
{

}

PEGASUS_NAMESPACE_END
