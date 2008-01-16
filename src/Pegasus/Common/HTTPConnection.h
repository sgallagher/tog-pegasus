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

#ifndef Pegasus_HTTPConnection_h
#define Pegasus_HTTPConnection_h

#include <iostream>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/MessageQueue.h>
#include <Pegasus/Common/Pair.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Message.h>
#include <Pegasus/Common/ArrayInternal.h>
#include <Pegasus/Common/Monitor.h>
#include <Pegasus/Common/AuthenticationInfo.h>
#include <Pegasus/Common/TLS.h>
#include <Pegasus/Common/HTTPAcceptor.h>
#include <Pegasus/Common/Linkage.h>
#include <Pegasus/Common/SharedPtr.h>
#include <Pegasus/Common/ContentLanguageList.h>
#include <Pegasus/Common/Buffer.h>

PEGASUS_NAMESPACE_BEGIN

class HTTPConnector;

class MessageQueueService;

struct HTTPConnectionRep;

/** This message is sent from a connection to its owner (so that the
    owner can do any necessary cleanup).
*/
class CloseConnectionMessage : public Message
{
public:

    CloseConnectionMessage(SocketHandle socket_)
        : Message(CLOSE_CONNECTION_MESSAGE), socket(socket_) { }

    SocketHandle socket;
};

/** This class represents an HTTP listener.
*/
class Monitor;

class PEGASUS_COMMON_LINKAGE HTTPConnection : public MessageQueue
{
public:
    typedef MessageQueue Base;

    /** Constructor. */
    HTTPConnection(
        Monitor* monitor,
        SharedPtr<MP_Socket>& socket,
        const String& ipAddress,
        MessageQueue * ownerMessageQueue,
        MessageQueue * outputMessageQueue);

    /** Destructor. */
    ~HTTPConnection();

    virtual void enqueue(Message *);

    /** This method is called whenever a SocketMessage is enqueued
        on the input queue of the HTTPConnection object.
    */
    virtual void handleEnqueue(Message *);

    virtual void handleEnqueue();

    /** Return socket this connection is using. */
    SocketHandle getSocket() { return _socket->getSocket();}

    MP_Socket& getMPSocket() { return *_socket;}

    /** Indicates whether this connection has an outstanding response.
    */
    Boolean isResponsePending();

    Boolean run(Uint32 milliseconds);

    MessageQueue& get_owner()
    {
        return *_ownerMessageQueue;
    }

    // was the request for chunking ?
    Boolean isChunkRequested();

    void setSocketWriteTimeout(Uint32 socketWriteTimeout);

    Boolean closeConnectionOnTimeout(struct timeval* timeNow);

    // ATTN-RK-P1-20020521: This is a major hack, required to get the CIM
    // server and tests to run successfully.  The problem is that the
    // HTTPAcceptor is deleting an HTTPConnection before all the threads
    // that are queued up to run in that HTTPConnection instance have had
    // a chance to finish.  This hack makes the HTTPAcceptor spin until
    // the HTTPConnection event threads have completed, before deleting
    // the HTTPConnection.  Note that this fix is not perfect, because
    // there is a window between when the HTTPConnection queue lookup is
    // done and when the refcount is incremented.  If the HTTPAcceptor
    // deletes the HTTPConnection in that window, the soon-to-be-launched
    // HTTPConnection event thread will fail (hard).
    AtomicInt refcount;

    CIMException cimException;

    // list of content languages
    ContentLanguageList contentLanguages;

private:

    void _clearIncoming();

    /**
        @exception  Exception   Indicates an error occurred.
    */
    void _getContentLengthAndContentOffset();

    void _closeConnection();

    void _handleReadEvent();

    Boolean _handleWriteEvent(Message &message);

    void _handleReadEventFailure(const String& httpStatusWithDetail,
                                 const String& cimError = String());
    void _handleReadEventTransferEncoding();
    Boolean _isClient();

    Monitor* _monitor;

    SharedPtr<MP_Socket> _socket;
    String _ipAddress;
    MessageQueue* _ownerMessageQueue;
    MessageQueue* _outputMessageQueue;

    Sint32 _contentOffset;
    Sint32 _contentLength;
    Buffer _incomingBuffer;
    SharedPtr<AuthenticationInfo> _authInfo;

    // _connectionRequestCount contains the number of
    // requests that have been received on this connection.
    Uint32 _connectionRequestCount;

    /**
        The _responsePending flag indicates whether a request has been
        received on the connection and is awaiting a response.  It is set to
        true when a request is received on the connection and set to false
        when a response is sent.  The connection object must not be destructed
        while a response is pending, because the CIM Server must route the
        response to the connection object when it becomes available.
    */
    Boolean _responsePending;

    Mutex _connection_mut;

    // The _connectionClosePending flag will be set to true if
    // the connection receives a close connection socket message.
    // This flag is used to set the connection status to
    // either Monitor::IDLE (_connectionClosePending == false)
    // or Monitor::DYING (_connectionClosePending == true) when
    // returning control of the connection to the Monitor.
    Boolean _connectionClosePending;

    // The _acceptPending flag is set to true if a server-side
    // connection is accepted but an SSL handshake has not been
    // completed.
    Boolean _acceptPending;

    // The _firstRead flag is set to false if the first bytes of a
    // request was read and validated that it is a supported 
    // HTTP method "POST" or "M-POST".
    Boolean _firstRead;

    // Holds time since the accept pending condition was detected.
    struct timeval _acceptPendingStartTime;

    int _entry_index;

    // When used by the client, it is an offset (from start of http message)
    // representing last NON completely parsed chunk of a transfer encoding.
    // When used by the server, it is the message index that comes down
    // from the providers/repository representing each message chunk.
    // WARNING: Due to the duel use of this member variable, modifying code
    // that uses this variable should be done very carefully. Accidental bugs
    // introduced could easily break interoperability with wbem
    // clients/servers that transfer and/or receive data via HTTP chunking.
    Uint32 _transferEncodingChunkOffset;

    // list of transfer encoding values from sender
    Array<String> _transferEncodingValues;

    // list of TE values from client
    Array<String> _transferEncodingTEValues;

    // 2 digit prefix on http header if mpost was used
    String _mpostPrefix;

    // Holds time since this connection is idle.
    struct timeval _idleStartTime;

    // Idle connection timeout in seconds specified by Config property
    // idleConnectionTimeout.
    Uint32 _idleConnectionTimeoutSeconds;

    friend class Monitor;
    friend class HTTPAcceptor;
    friend class HTTPConnector;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_HTTPConnection_h */
