//%/////////////////////////////////////////////////////////////////////////////
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
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_HTTPConnection_h
#define Pegasus_HTTPConnection_h

#include <iostream>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/MessageQueue.h>
#include <Pegasus/Common/Pair.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Message.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/Monitor.h>

PEGASUS_NAMESPACE_BEGIN

class MessageQueue;
struct HTTPConnectionRep;

/** This message is sent from a connection to its owner (so that the
    owner can do any necessary cleanup).
*/
class CloseConnectionMessage : public Message
{
public:

    CloseConnectionMessage(Sint32 socket_) 
	: Message(CLOSE_CONNECTION_MESSAGE), socket(socket_) { }

    Sint32 socket;
};

/** This message is sent from a connection to its output queue when
    a complete HTTP message is received.
*/
class PEGASUS_COMMON_LINKAGE HTTPMessage : public Message
{
public:

    HTTPMessage(const Array<Sint8>& message_, Uint32 returnQueueId_ = 0);

    Array<Sint8> message;
    Uint32 returnQueueId;

    typedef Pair<String, String> HTTPHeader;

    void parse(
	String& firstLine,
	Array<HTTPHeader>& headers,
	Sint8*& content,
	Uint32& contentLength) const;

    void print(PEGASUS_STD(ostream)& os) const;
};

/** This class represents an HTTP listener.
*/
class PEGASUS_COMMON_LINKAGE HTTPConnection : public MessageQueue
{
public:

    /** Constructor. */
    HTTPConnection(
	Monitor* monitor,
	Sint32 socket, 
	MessageQueue* ownerMessageQueue,
	MessageQueue* outputMessageQueue);

    /** Destructor. */
    ~HTTPConnection();

    /** This method is called whenever a SocketMessage is enqueued
	on the input queue of the HTTPConnection object.
    */ 
    virtual void handleEnqueue();

    /** Return socket this connection is using. */
    Sint32 getSocket() const { return _socket; }

private:

    void _clearIncoming();

    void _getContentLengthAndContentOffset();

    void _closeConnection();

    void _handleReadEvent();

    Monitor* _monitor;
    Sint32 _socket;
    MessageQueue* _ownerMessageQueue;
    MessageQueue* _outputMessageQueue;

    Sint32 _contentOffset;
    Sint32 _contentLength;
    Array<Sint8> _incomingBuffer;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_HTTPConnection_h */
