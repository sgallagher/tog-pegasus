//%///-*-c++-*-/////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 BMC Software, Hewlett-Packard Company, IBM,
// The Open Group, Tivoli Systems
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
// Modified By: Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
// Modified By: Carol Ann Krug Graves, Hewlett-Packard Company 
//              (carolann_graves@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Message_h
#define Pegasus_Message_h

#include <iostream>
#include <cstring>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Exception.h>

PEGASUS_NAMESPACE_BEGIN

class MessageQueue;

/** The Message class and derived classes are used to pass messages between 
    modules. Messages are passed between modules using the message queues
    (see MessageQueue class). Derived classes may add their own fields.
    This base class defines two common fields: type, which is the type of
    the message, and key which is a key value whose meaning is defined by
    the derived class. The MessageQueue class provides methods for finding
    messages by both type and key.

    The Message class also provides previous and next pointers which are
    used to place the messages on a queue by the MessageQueue class.
*/
class PEGASUS_COMMON_LINKAGE Message
{
public:

    Message(Uint32 type, Uint32 key = getNextKey()) 
	: _type(type), _key(key), _next(0), _prev(0) { }

    virtual ~Message(); 

    Uint32 getType() const { return _type; }

    void setType(Uint32 type) { _type = type; }

    Uint32 getKey() const { return _key; }

    void setKey(Uint32 key) { _key = key; }

    Message* getNext() { return _next; }

    const Message* getNext() const { return _next; }

    Message* getPrevious() { return _prev; }

    const Message* getPrevious() const { return _prev; }

    static Uint32 getNextKey() { return ++_nextKey; }

    virtual void print(PEGASUS_STD(ostream)& os) const;

private:
    Uint32 _type;
    Uint32 _key;
    Message* _next;
    Message* _prev;
    MessageQueue* _owner;
    static Uint32 _nextKey;
    friend class MessageQueue;
};


// each component needs to support a set of these messgaes and pass that array
// to the dispatcher so the dispatcher can route messages at the first level
// i.e., client will not accept request messages.
// every message should have a response

// dispatcher supports full cim api set (as below)
// repository needs to be a peer to the provider manager
// 

// mkdir _dispatcher
// mkdir _providermanager
// mkdir _server (http incoming, front end)
// mkdir _repositorymanager
//       _subscriptionprocessor
//       _indicationprocessor
//       _configurationmanager 
//       _cimom (loads and links everyone, hooks up queues)

// fundamental messages:

// start, stop, pause, resume
// handshaking: interrogate (as in windows service api)
//              message class support
//              message namespace support ???

enum MessageType
{
    DUMMY_MESSAGE,

    // CIM Message types:

    CIM_GET_CLASS_REQUEST_MESSAGE,
    CIM_GET_INSTANCE_REQUEST_MESSAGE,
    CIM_EXPORT_INDICATION_REQUEST_MESSAGE,
    CIM_DELETE_CLASS_REQUEST_MESSAGE,
    CIM_DELETE_INSTANCE_REQUEST_MESSAGE,
    CIM_CREATE_CLASS_REQUEST_MESSAGE,
    CIM_CREATE_INSTANCE_REQUEST_MESSAGE,
    CIM_MODIFY_CLASS_REQUEST_MESSAGE,
    CIM_MODIFY_INSTANCE_REQUEST_MESSAGE,
    CIM_ENUMERATE_CLASSES_REQUEST_MESSAGE,
    CIM_ENUMERATE_CLASS_NAMES_REQUEST_MESSAGE,
    CIM_ENUMERATE_INSTANCES_REQUEST_MESSAGE,
    CIM_ENUMERATE_INSTANCE_NAMES_REQUEST_MESSAGE,
    CIM_EXEC_QUERY_REQUEST_MESSAGE,
    CIM_ASSOCIATORS_REQUEST_MESSAGE,
    CIM_ASSOCIATOR_NAMES_REQUEST_MESSAGE,
    CIM_REFERENCES_REQUEST_MESSAGE,
    CIM_REFERENCE_NAMES_REQUEST_MESSAGE,
    CIM_GET_PROPERTY_REQUEST_MESSAGE,
    CIM_SET_PROPERTY_REQUEST_MESSAGE,
    CIM_GET_QUALIFIER_REQUEST_MESSAGE,
    CIM_SET_QUALIFIER_REQUEST_MESSAGE,
    CIM_DELETE_QUALIFIER_REQUEST_MESSAGE,
    CIM_ENUMERATE_QUALIFIERS_REQUEST_MESSAGE,
    CIM_INVOKE_METHOD_REQUEST_MESSAGE,
    CIM_ENABLE_INDICATION_SUBSCRIPTION_REQUEST_MESSAGE,
    CIM_MODIFY_INDICATION_SUBSCRIPTION_REQUEST_MESSAGE,
    CIM_DISABLE_INDICATION_SUBSCRIPTION_REQUEST_MESSAGE,
    CIM_GET_CLASS_RESPONSE_MESSAGE,
    CIM_GET_INSTANCE_RESPONSE_MESSAGE,
    CIM_EXPORT_INDICATION_RESPONSE_MESSAGE,
    CIM_DELETE_CLASS_RESPONSE_MESSAGE,
    CIM_DELETE_INSTANCE_RESPONSE_MESSAGE,
    CIM_CREATE_CLASS_RESPONSE_MESSAGE,
    CIM_CREATE_INSTANCE_RESPONSE_MESSAGE,
    CIM_MODIFY_CLASS_RESPONSE_MESSAGE,
    CIM_MODIFY_INSTANCE_RESPONSE_MESSAGE,
    CIM_ENUMERATE_CLASSES_RESPONSE_MESSAGE,
    CIM_ENUMERATE_CLASS_NAMES_RESPONSE_MESSAGE,
    CIM_ENUMERATE_INSTANCES_RESPONSE_MESSAGE,
    CIM_ENUMERATE_INSTANCE_NAMES_RESPONSE_MESSAGE,
    CIM_EXEC_QUERY_RESPONSE_MESSAGE,
    CIM_ASSOCIATORS_RESPONSE_MESSAGE,
    CIM_ASSOCIATOR_NAMES_RESPONSE_MESSAGE,
    CIM_REFERENCES_RESPONSE_MESSAGE,
    CIM_REFERENCE_NAMES_RESPONSE_MESSAGE,
    CIM_GET_PROPERTY_RESPONSE_MESSAGE,
    CIM_SET_PROPERTY_RESPONSE_MESSAGE,
    CIM_GET_QUALIFIER_RESPONSE_MESSAGE,
    CIM_SET_QUALIFIER_RESPONSE_MESSAGE,
    CIM_DELETE_QUALIFIER_RESPONSE_MESSAGE,
    CIM_ENUMERATE_QUALIFIERS_RESPONSE_MESSAGE,
    CIM_INVOKE_METHOD_RESPONSE_MESSAGE,
    CIM_ENABLE_INDICATION_SUBSCRIPTION_RESPONSE_MESSAGE,
    CIM_MODIFY_INDICATION_SUBSCRIPTION_RESPONSE_MESSAGE,
    CIM_DISABLE_INDICATION_SUBSCRIPTION_RESPONSE_MESSAGE,

    // Monitor-related messages:

    SOCKET_MESSAGE,

    // Connection-oriented messages:

    CLOSE_CONNECTION_MESSAGE,

    // HTTP messages:

    HTTP_MESSAGE,

    NUMBER_OF_MESSAGES
};

PEGASUS_COMMON_LINKAGE const char* MessageTypeToString(Uint32 messageType);

/** This class implements a stack of queue-ids. Many messages must keep a
    stack of queue-ids of queues which they must be returned to. This provides
    a light efficient stack for this purpose.
*/
class QueueIdStack
{
public:

    QueueIdStack() : _size(0) 
    { 
    }

    QueueIdStack(const QueueIdStack& x) : _size(x._size) 
    {
	memcpy(_items, x._items, sizeof(_items));
    }

    PEGASUS_EXPLICIT QueueIdStack(Uint32 x) : _size(0) 
    { 
	push(x); 
    }

    PEGASUS_EXPLICIT QueueIdStack(Uint32 x1, Uint32 x2) : _size(0) 
    {
	push(x1); 
	push(x2); 
    }

    ~QueueIdStack() 
    { 
    }

    QueueIdStack& operator=(const QueueIdStack& x) 
    {
	if (this != &x)
	{
	    memcpy(_items, x._items, sizeof(_items));
	    _size = x._size;
	}
	return *this;
    }

    Uint32 size() const 
    { 
	return _size; 
    }

    Boolean isEmpty() const 
    { 
	return _size == 0; 
    }

    void push(Uint32 x) 
    {
	if (_size == MAX_SIZE)
	    throw StackOverflow();

	_items[_size++] = x;
    }

    Uint32& top()
    {
	if (_size == 0)
	    throw StackUnderflow();

	return _items[_size-1];
    }

    Uint32 top() const 
    {
	return ((QueueIdStack*)this)->top(); 
    }

    void pop() 
    {
	if (_size == 0)
	    throw StackUnderflow();

	_size--;
    }

    /** Make a copy of this stack and then pop the top element. */
    QueueIdStack copyAndPop() const
    {
	return QueueIdStack(*this, 0);
    }

private:

    // Copy the given stack but then pop the top element:
    QueueIdStack(const QueueIdStack& x, int) : _size(x._size) 
    {
	memcpy(_items, x._items, sizeof(_items));
	pop();
    }

    enum { MAX_SIZE = 5 };
    Uint32 _items[MAX_SIZE];
    Uint32 _size;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Message_h */
