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
// Author: Markus Mueller (sedgewick_de@yahoo.de)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/IPC.h>
#include <Pegasus/Common/HashTable.h>
#include <Pegasus/Common/MessageQueue.h>
#include "MT_MessageQueue.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

typedef HashTable<Uint32, MessageQueue*, EqualFunc<Uint32>, HashFunc<Uint32> >
    QueueTable;

static Mutex _MT_queueTable_lock;
static QueueTable _MT_queueTable(128);

MT_MessageQueue::MT_MessageQueue()
{
    _MT_queueTable_lock.lock(pegasus_thread_self());

    // has to be rewritten to use the lockable QueueTable !!!
    _mqueue = new MessageQueue();

    // the message queue MAY only be changed, if both locks held !
    _full = new Condition();
    _empty= new Condition();

    _lowWaterMark = PEGASUS_DEFAULT_LOWWATERMARK;
    _highWaterMark = PEGASUS_DEFAULT_HIGHWATERMARK;

    _preferReader = false; //default

    _MT_queueTable_lock.unlock();
}

MT_MessageQueue::~MT_MessageQueue()
{
    _MT_queueTable_lock.lock(pegasus_thread_self());

    // has to be rewritten to use the lockable QueueTable
    delete _full;
    delete _empty;
    delete _mqueue;

    _MT_queueTable_lock.unlock();
}

void MT_MessageQueue::enqueue(Message* message)
{
    Uint32 count;

    if (!message)
	throw NullPointer();

    while (true) 
    {
        _full->getMutex()->lock(pegasus_thread_self());
 
        while(_mqueue->getCount() >= _highWaterMark) 
	  {
	  try
	    {
	      _full->unlocked_timed_wait(500, pegasus_thread_self());
	    }
	  catch(TimeOut& to)
	    {
	      ;
	    }
	  }

        if (_preferReader)
        {
            // if you cannot get the 2nd mutex,
            // just release all locks and try again
	  try 
	    { 
	      _empty->getMutex()->try_lock(pegasus_thread_self()) ;
	    }
	  catch ( AlreadyLocked& al ) 
	    { 
	      _full->getMutex()->unlock(); 
	      break;
	    }
        }
        else {
            _empty->getMutex()->lock(pegasus_thread_self());
            break;
        }
    } /* end while */

    _mqueue->enqueue(message);

    _empty->unlocked_signal(pegasus_thread_self());

    _empty->getMutex()->unlock();

    _full->getMutex()->unlock();

    handleEnqueue();
}

Message* MT_MessageQueue::dequeue()
{
    Message * message; 

    while (true) 
    {
        _empty->getMutex()->lock(pegasus_thread_self());

        while (_mqueue->getCount() <= _lowWaterMark) 
	  {
	    try 
	      {
		_empty->unlocked_timed_wait(500, pegasus_thread_self());
	      }
	    catch(TimeOut& to)
	      {
		;
	      }
	  }

        if (!_preferReader)
        {
            // if you cannot get the 2nd mutex,
            // just release all locks and try again
	  try 
	    {
	      _full->getMutex()->try_lock(pegasus_thread_self());
	      break;
	    }
	  catch (AlreadyLocked& al)
	    {
	      _empty->getMutex()->unlock();
	    }
        }
        else {
            _full->getMutex()->lock(pegasus_thread_self());
            break;
        }
    } /* end while */

    message = _mqueue->dequeue();

    _full->unlocked_signal(pegasus_thread_self());

    _empty->getMutex()->unlock();

    _full->getMutex()->unlock();

    return message;
}

void MT_MessageQueue::remove(Message* message)
{
}

Message* MT_MessageQueue::findByType(Uint32 type)
{
    return NULL;
}

Message* MT_MessageQueue::findByKey(Uint32 key)
{
    return NULL;
}

void MT_MessageQueue::print(ostream& os) const
{
}

Message* MT_MessageQueue::find(Uint32 type, Uint32 key)
{
    return NULL;
}

void MT_MessageQueue::lock()
{
}

void MT_MessageQueue::unlock()
{
    _full->getMutex()->unlock();
    _empty->getMutex()->unlock();
}

MessageQueue* MT_MessageQueue::lookup(Uint32 queueId)
{
    MessageQueue* queue = 0;

    _MT_queueTable_lock.lock(pegasus_thread_self());
    if (_MT_queueTable.lookup(queueId, queue))
    {
        _MT_queueTable_lock.unlock();
	return queue;
    }

    // Not found!
    _MT_queueTable_lock.unlock();
    return 0;
}

void MT_MessageQueue::handleEnqueue()
{
    _mqueue->handleEnqueue();
}

PEGASUS_NAMESPACE_END
