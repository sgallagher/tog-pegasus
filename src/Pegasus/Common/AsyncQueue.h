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

#ifndef Pegasus_AsyncQueue_h
#define Pegasus_AsyncQueue_h

#include <Pegasus/Common/Linkage.h>
#include <Pegasus/Common/List.h>
#include <Pegasus/Common/Condition.h>

PEGASUS_NAMESPACE_BEGIN

/** AsyncQueue implementation (formerly AsyncDQueue).
*/
template<class ElemType>
class AsyncQueue
{
public:

    /** Constructor.
    */
    AsyncQueue();

    /** Destructor.
    */
    virtual ~AsyncQueue();

    /** Close the queue so that subsequent enqueue() and dequeue() requests
        result in ListClosed() exceptions.
    */
    void close();

    /** Enqueue an element at the back of queue.
    */
    void enqueue(ElemType *element);

    /** Dequeue an element from the front of the queue. Return null immediately
        if queue is empty.
    */
    ElemType *dequeue();

    /** Dequeue an element from the front of the queue (wait if the queue is
        empty).
    */
    ElemType *dequeue_wait();

    /** Discard all the elements on the list. The size becomes zero afterwards.
    */
    void clear();

    /** Return number of element in queue.
    */
    Uint32 count() const { return _rep.size(); }

    /** Return true is queue is empty (has zero elements).
    */
    Boolean is_empty() const { return _rep.size() == 0; }

    /** Return true if the queue has been closed (in which case no new
        elements may be enqueued).
    */
    Boolean is_closed() const { return _closed.get(); }

private:

    Mutex _mutex;
    Condition _not_empty;
    AtomicInt _closed;
    typedef List<ElemType,NullLock> Rep;
    Rep _rep;
};

template<class ElemType>
AsyncQueue<ElemType>::AsyncQueue() :
    _mutex(Mutex::NON_RECURSIVE)
{
}

template<class ElemType>
AsyncQueue<ElemType>::~AsyncQueue()
{
}

template<class ElemType>
void AsyncQueue<ElemType>::close()
{
    AutoMutex auto_mutex(_mutex);

    if (!is_closed())
    {
        _closed++;
        _not_empty.signal();
    }
}

template<class ElemType>
void AsyncQueue<ElemType>::enqueue(ElemType *element)
{
    if (element)
    {
        AutoMutex auto_mutex(_mutex);

        if (is_closed())
            throw ListClosed();

        _rep.insert_back(element);
        _not_empty.signal();
    }
}

template<class ElemType>
void AsyncQueue<ElemType>::clear()
{
    AutoMutex auto_mutex(_mutex);
    _rep.clear();
}

template<class ElemType>
ElemType* AsyncQueue<ElemType>::dequeue()
{
    AutoMutex auto_mutex(_mutex);

    if (is_closed())
        throw ListClosed();

    return _rep.remove_front();
}

template<class ElemType>
ElemType* AsyncQueue<ElemType>::dequeue_wait()
{
    AutoMutex auto_mutex(_mutex);

    while (is_empty())
    {
        if (is_closed())
            throw ListClosed();

        _not_empty.wait(_mutex);
    }

    if (is_closed())
        throw ListClosed();

    ElemType* p = _rep.remove_front();
    PEGASUS_DEBUG_ASSERT(p != 0);

    return p;
}

PEGASUS_NAMESPACE_END

#endif /* Pegasus_AsyncQueue_h */
