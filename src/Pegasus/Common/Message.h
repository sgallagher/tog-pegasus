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

#ifndef Pegasus_Message_h
#define Pegasus_Message_h

#include <iostream>
#include <Pegasus/Common/Config.h>

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
class Message
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

    virtual void print(
	PEGASUS_STD(ostream)& os, 
	Boolean printHeader = true) const;

private:
    Uint32 _type;
    Uint32 _key;
    Message* _next;
    Message* _prev;
    MessageQueue* _owner;
    static Uint32 _nextKey;
    friend class MessageQueue;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Message_h */
