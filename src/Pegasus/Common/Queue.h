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
// Author: Karl Schopmeyer (k.schopmeyer@attglobal.net)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Queue_h
#define Pegasus_Queue_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Exception.h>

PEGASUS_NAMESPACE_BEGIN

/** The Queue class provides a simple FIFO Queue implementation.
    This class provides a Queue implementation which is based on the Array<>
    template class. It allows enqueing, dequeing and size determination.
*/
template<class T>
class Queue
{
public:

    /** */
    Queue() { }

    /** */
    Queue(const Queue<T>& x) : _rep(x._rep) { }

    /** */
    ~Queue() { }

    /** */
    Queue<T>& operator=(const Queue<T>& x) { _rep = x._rep; return *this; }

    /** */
    Boolean isEmpty() const { return _rep.size() == 0; }

    /** Enqueue - Adds a new item to the end of the queue*/
    void enqueue(const T& x) { _rep.append(x); }

    /** dequeue - Removes the first entry from the queue. Note that this does 
	not return the dequeued item to the user.
	The normal approach to use this is to first look at the first item
	with the front method and then dequeue with the dequeue
    */
    void dequeue();
    
    /** */
    T& front();

    /** */
    const T& front() const { return ((Queue<T>*)this)->front(); }

    /** */
    T& back();

    /** */
    const T& back() const { return ((Queue<T>*)this)->back(); }


    /** */
    Uint32 size() const { return _rep.size(); }

    /** The [] operator allows you to treat the queue as an indexed array
        and look at individual items on the queue.
    */
    T& operator[](Uint32 i) { return _rep[i]; }

    /** The [] operator allows you to treat the queue as an indexed array
        and look at individual items on the queue.
    */
    const T& operator[](Uint32 i) const { return _rep[i]; }

private:

    Array<T> _rep;
};

template<class T>
T& Queue<T>::front()
{
    if (!isEmpty())
	return _rep[0];
    else
    {
	static T dummy = T();
	return dummy;
    }
}
template<class T>
T& Queue<T>::back()
{
    if (!isEmpty())
	return _rep[_rep.size() - 1];
    else
    {
	static T dummy = T();
	return dummy;
    }
}

template<class T>
void Queue<T>::dequeue()
{
    if (_rep.size() == 0)
	throw QueueUnderflow();

    _rep.remove(0);
}

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Queue_h */
