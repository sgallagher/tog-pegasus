//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
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
//%/////////////////////////////////////////////////////////////////////////////


#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Condition.h>
#include <Pegasus/Common/Mutex.h>
#include <Pegasus/Common/Thread.h>

PEGASUS_USING_PEGASUS;

class ConditionQueue
{
public:

    ConditionQueue(size_t max_size);

    ~ConditionQueue();

    void enqueue(void* entry);

    void* dequeue();

private:

    ConditionQueue(const ConditionQueue&);
    ConditionQueue& operator=(const ConditionQueue&);

    enum { MAX_AUTO_ELEMENTS = 8 };

    void** _data;
    size_t _size;
    size_t _head;
    size_t _tail;
    size_t _max_size;

    Mutex _lock;
    Condition _not_empty;
    Condition _not_full;
};

ConditionQueue::ConditionQueue(size_t max_size) : 
    _size(0), 
    _head(0), 
    _tail(0), 
    _max_size(max_size),
    _lock(Mutex::NON_RECURSIVE)
{
    _data = new void*[max_size];
}

ConditionQueue::~ConditionQueue()
{
    delete [] _data;
}

void ConditionQueue::enqueue(void* entry)
{
    AutoMutex am(_lock);

    while (_size == _max_size)
    {
        _not_full.wait(_lock);
    }

    _data[_tail++] = entry;

    if (_tail == _max_size)
        _tail = 0;

    _size++;

    _not_empty.signal();
}

void* ConditionQueue::dequeue()
{
    AutoMutex am(_lock);

    while (_size == 0)
    {
        _not_empty.wait(_lock);
    }

    void* entry = _data[_head++];

    if (_head == _max_size)
        _head = 0;

    _size--;

    _not_full.signal();

    return entry;
}

static ThreadReturnType PEGASUS_THREAD_CDECL _reader(void* self_)
{
    Thread* self = (Thread*)self_;
    ConditionQueue* queue = (ConditionQueue*)self->get_parm();

    for (size_t i = 0; i < 100000; i++)
    {
        char buf[22];
        sprintf(buf, "%d", i);
        char* msg = (char*)queue->dequeue();
        assert(strcmp(msg, buf) == 0);
        free(msg);
    }

    return 0;
}

static ThreadReturnType PEGASUS_THREAD_CDECL _writer(void* self_)
{
    Thread* self = (Thread*)self_;
    ConditionQueue* queue = (ConditionQueue*)self->get_parm();

    for (int i = 0; i < 100000; i++)
    {
        char buf[22];
        sprintf(buf, "%d", i);
        queue->enqueue(strdup(buf));
    }

    return 0;
}

int main()
{
    ConditionQueue* queue = new ConditionQueue(10);

    Thread reader(_reader, queue, false);
    Thread writer(_writer, queue, false);

    reader.run();
    writer.run();

    reader.join();
    writer.join();

    delete queue;

    printf("+++++ passed all tests\n");
}
