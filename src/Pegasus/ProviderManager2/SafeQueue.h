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
// Author: Chip Vincent (cvincent@us.ibm.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_SafeQueue_h
#define Pegasus_SafeQueue_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Queue.h>
#include <Pegasus/Common/IPC.h>

PEGASUS_NAMESPACE_BEGIN

template<class T>
class SafeQueue
{
public:
    SafeQueue(void);
    virtual ~SafeQueue(void);

    void enqueue(const T & O);
    T dequeue(void);

    T & front(void);
    const T & front(void) const;

    T & back(void);
    const T & back(void) const;

    Uint32 size(void) const;

protected:
    mutable Mutex _mutex;
    Queue<T> _queue;

};

template<class T>
SafeQueue<T>::SafeQueue(void)
{
}

template<class T>
SafeQueue<T>::~SafeQueue(void)
{
}

template<class T>
void SafeQueue<T>::enqueue(const T & O)
{
    AutoMutex lock(_mutex);

    _queue.enqueue(O);
}

template<class T>
T SafeQueue<T>::dequeue(void)
{
    AutoMutex lock(_mutex);

    T O = _queue.front();

    _queue.dequeue();

    return(O);
}

template<class T>
T & SafeQueue<T>::front(void)
{
    AutoMutex lock(_mutex);

    return(_queue.front());
}

template<class T>
const T & SafeQueue<T>::front(void) const
{
    AutoMutex lock(_mutex);

    return(_queue.front());
}

template<class T>
T & SafeQueue<T>::back(void)
{
    AutoMutex lock(_mutex);

    return(_queue.back());
}

template<class T>
const T & SafeQueue<T>::back(void) const
{
    AutoMutex lock(_mutex);

    return(_queue.back());
}

template<class T>
Uint32 SafeQueue<T>::size(void) const
{
    AutoMutex lock(_mutex);

    return(_queue.size());
}

PEGASUS_NAMESPACE_END

#endif
