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
// Author: Mike Day (mdday@us.ibm.com)
//
// Modified By: Amit K Arora, IBM (amita@in.ibm.com) for PEP#101
//              Alagaraja Ramasubramanian (alags_raj@in.ibm.com) for Bug#1090
//              Amit K Arora, IBM (amita@in.ibm.com) for Bug#2322
//              David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
//              Amit K Arora, IBM (amita@in.ibm.com) for Bug#2960
//
// Reworked By:
//              Mike Brasher (m.brasher@inovadevelopment.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_AsyncQueue_h
#define Pegasus_AsyncQueue_h

#include <Pegasus/Common/IPC.h>
#include <Pegasus/Common/Linkage.h>
#include <Pegasus/Common/List.h>

PEGASUS_NAMESPACE_BEGIN

/** AsyncQueue implementation (formerly AsyncDQueue).
*/
template<class ElemType> 
class AsyncQueue
{
public:

    /** Constructor.
    */
    AsyncQueue(Uint32 capacity = 0);

    /** Destructor.
    */
    virtual ~AsyncQueue();

    /** Shutdownt the queue.
    */
    void shutdown_queue();

    /** Enqueue an element at the back of queue.
    */
    void enqueue(ElemType *element);

    /** Enqueue an element at the back of queue (wait for dequeue by another
	thread).
    */
    void enqueue_wait(ElemType *element);

    /** Dequeue an element from the front of the queue. Return null immediately
	if queue is empty.
    */
    ElemType *dequeue();

    /** Dequeue an element from the front of the queue (if there is no element
	on queue, wait until there is).
    */
    ElemType *dequeue_wait();

    /** Discard all the elements on the list. The size becomes zero afterwards.
    */
    void clear();

    /** Return number of element in queue.
    */
    Uint32 count() const { return _count.get(); }

    /** Return number of element in queue.
    */
    Uint32 size() const { return _count.get(); }

    /** Return the maximum number of elements permitted on queue at once.
    */
    Uint32 capacity() const { return _capacity.get(); }

    /** Return true if queue has reached its capacity.
    */
    Boolean is_full() const { /* never full */ return false; }

    /** Return true is queue is empty (has zero elements).
    */
    Boolean is_empty() const { return size() == 0; }

    /** Return true if the queue has been shutdown (in which case no new
        elements may be enqueued).
    */
    Boolean is_shutdown() const { return _disallow.get() > 0; }

    /** Attempt to lock the queue.
    */
    void try_lock(PEGASUS_THREAD_TYPE myself);

    /** Lock the queue.
    */
    void lock(PEGASUS_THREAD_TYPE myself);

    /** Unlock the queue.
    */
    void unlock(void) { _cond.unlock(); }

private:

    /**
        @exception  IPCException    Indicates an IPC error occurred.
    */
    void _insert_prep();

    /** @exception  IPCException    Indicates an IPC error occurred.
    */
    void _insert_recover();

    /** @exception  IPCException    Indicates an IPC error occurred.
    */
    void _unlink_prep();

    /** @exception  IPCException    Indicates an IPC error occurred.
    */
    void _unlink_recover();

    /** @exception  IPCException    Indicates an IPC error occurred.
    */
    ElemType *_remove_no_lock(const void *key);

    /** @exception  IPCException    Indicates an IPC error occurred.
    */
    ElemType *_remove_no_lock(const ElemType *key);

    static bool _equal_key(const ElemType* elem, const void* key)
    {
        return elem->operator==(key);
    }

    static bool _equal_object(const ElemType* elem, const void* object)
    {
        return elem->operator==(*((ElemType*)object));
    }

    Mutex _cond;
    Condition _slot;
    Condition _node;
    AtomicInt _count;
    AtomicInt _disallow;
    AtomicInt _capacity;
    typedef List<ElemType,NullLock> Rep;
    Rep _rep;
};

template<class ElemType> 
AsyncQueue<ElemType>::AsyncQueue(Uint32 capacity) :
    _slot(_cond), _node(_cond), _capacity(0)
{
}

template<class ElemType> 
AsyncQueue<ElemType>::~AsyncQueue()
{

}

template<class ElemType> 
void AsyncQueue<ElemType>::_insert_prep()
{
    if(_disallow.get() > 0)
    {
        unlock();
        throw ListClosed();
    }

    _slot.lock_object(pegasus_thread_self());
    while(true == is_full())
    {
        _slot.unlocked_wait(pegasus_thread_self());
        if(_disallow.get() > 0)
        {
            unlock();
            throw ListClosed();
        }
    }
}

template<class ElemType> 
void AsyncQueue<ElemType>::_insert_recover()
{
    _node.unlocked_signal(pegasus_thread_self());
    _count++;
    unlock();
}

template<class ElemType> 
void AsyncQueue<ElemType>::_unlink_prep()
{
    if(_disallow.get() > 0)
    {
        unlock();
        throw ListClosed();
    }
    _node.lock_object(pegasus_thread_self());
    while(true == is_empty())
    {
        _node.unlocked_wait(pegasus_thread_self());
       if(_disallow.get() > 0)
       {
           unlock();
           throw ListClosed();
       }
    }
}

template<class ElemType> 
void AsyncQueue<ElemType>::_unlink_recover()
{
    _slot.unlocked_signal(pegasus_thread_self());
    _count--;
    unlock();
}

template<class ElemType> 
ElemType* AsyncQueue<ElemType>::_remove_no_lock(const void *key)
{
    if(_disallow.get() > 0)
    {
        unlock();
        throw ListClosed();
    }

    if( pegasus_thread_self() != _cond.get_owner())
        throw Permission(pegasus_thread_self());

    return _rep.remove(_equal_key, key);
}

template<class ElemType> 
ElemType *AsyncQueue<ElemType>::_remove_no_lock(const ElemType *key)
{
    if(_disallow.get() > 0)
    {
        unlock();
        throw ListClosed();
    }
    if( pegasus_thread_self() != _cond.get_owner())
        throw Permission(pegasus_thread_self());

    return _rep.remove(_equal_object, key);
}

template<class ElemType> 
void AsyncQueue<ElemType>::shutdown_queue()
{
    try
    {
        lock(pegasus_thread_self());
        _disallow++;
        _node.disallow();
        _node.unlocked_signal(pegasus_thread_self());
        _node.unlocked_signal(pegasus_thread_self());
        _slot.disallow();
        _slot.unlocked_signal(pegasus_thread_self());
        _slot.unlocked_signal(pegasus_thread_self());
        unlock();
    }
    catch(const ListClosed &)
    {
        _disallow++;
    }
}

template<class ElemType> 
void AsyncQueue<ElemType>::try_lock(PEGASUS_THREAD_TYPE myself)
{
    if(_disallow.get() > 0)
    {
        throw ListClosed();
    }

    _cond.try_lock(myself);
}

template<class ElemType> 
void AsyncQueue<ElemType>::lock(PEGASUS_THREAD_TYPE myself)
{
    if(_disallow.get() > 0)
    {
       throw ListClosed();
    }
    _cond.lock(myself);
}

template<class ElemType> 
void AsyncQueue<ElemType>::enqueue(ElemType *element)
{
    if(element != 0)
    {
        lock(pegasus_thread_self());
        if(true == is_full())
        {
            unlock();
            throw ListFull(_capacity.get());
        }
        _rep.insert_back(element);
        _insert_recover();
    }
}

template<class ElemType> 
void AsyncQueue<ElemType>::enqueue_wait(ElemType *element)
{
    if(element != 0)
    {
        _insert_prep();
        _rep.insert_back(element);
        _insert_recover();
    }
}

template<class ElemType> 
void AsyncQueue<ElemType>::clear()
{
    lock(pegasus_thread_self());
    _rep.clear();
    _count = 0;
    _slot.unlocked_signal(pegasus_thread_self());
    unlock();
}

template<class ElemType> 
ElemType *AsyncQueue<ElemType>::dequeue()
{

    lock(pegasus_thread_self());
    ElemType *ret = _rep.remove_front();
    if(ret != 0)
    {
        _slot.unlocked_signal(pegasus_thread_self());
        _count--;
    }
    unlock();
    return ret;
}

template<class ElemType> 
ElemType *AsyncQueue<ElemType>::dequeue_wait()
{
    _unlink_prep();
    ElemType *ret = _rep.remove_front();
    _unlink_recover();
    return ret;
}

PEGASUS_NAMESPACE_END

#endif /* Pegasus_AsyncQueue_h */
