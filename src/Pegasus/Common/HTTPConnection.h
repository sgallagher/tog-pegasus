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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By:
//         Nag Boranna, Hewlett-Packard Company(nagaraja_boranna@hp.com)
//         Jenny Yu, Hewlett-Packard Company (jenny_yu@hp.com)
//         Brian G. Campbell, EMC (campbell_brian@emc.com) - PEP140/phase1
//         Amit K Arora, IBM (amita@in.ibm.com) for Bug#1097, #2541
//         David Dillard, VERITAS Software Corp.  (david.dillard@veritas.com)
//         Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
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
#include <Pegasus/Common/AutoPtr.h>
#include <Pegasus/Common/ContentLanguages.h>

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

    CloseConnectionMessage(Sint32 socket_)
        : Message(CLOSE_CONNECTION_MESSAGE), socket(socket_) { }

    Sint32 socket;
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
        AutoPtr<MP_Socket>& socket,
        MessageQueue * ownerMessageQueue,
        MessageQueue * outputMessageQueue,
        Boolean exportConnection);


    /** Destructor. */
    ~HTTPConnection();

    /** This method is called whenever a SocketMessage is enqueued
        on the input queue of the HTTPConnection object.
    */
    virtual void handleEnqueue(Message *);

    virtual void handleEnqueue();

    /** Return socket this connection is using. */
    Sint32 getSocket() { return _socket->getSocket();}

    MP_Socket& getMPSocket() { return *_socket;}

    /** Return the number of outstanding requests for all HTTPConnection
        instances.
    */
    Uint32 getRequestCount();

    Boolean run(Uint32 milliseconds);

    MessageQueue & get_owner(void)
    {
        return *_ownerMessageQueue;
    }

    // was the request for chunking ?
    Boolean isChunkRequested();

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
    ContentLanguages contentLanguages;

private:

    void _clearIncoming();

    /**
        @exception  Exception   Indicates an error occurred.
    */
    void _getContentLengthAndContentOffset();

    void _closeConnection();

    void _handleReadEvent();

    Boolean _handleWriteEvent(Message &message);

    void _handleReadEventFailure(String &httpStatusWithDetail,
                                 String cimError = String());
    void _handleReadEventTransferEncoding();
    Boolean _isClient();

    Monitor* _monitor;

    AutoPtr<MP_Socket> _socket;
    MessageQueue* _ownerMessageQueue;
    MessageQueue* _outputMessageQueue;

    Sint32 _contentOffset;
    Sint32 _contentLength;
    Array<char> _incomingBuffer;
    AutoPtr<AuthenticationInfo> _authInfo;
    static AtomicInt _requestCount;

    // _connectionRequestCount contains the number of
    // requests that have been received on this connection.
    Uint32 _connectionRequestCount;

    // The _responsePending flag has been added to help
    // isolate "client connection" problems. When the
    // HTTPConnection object is created, this flag is
    // initialized to false.  It is set to true when a
    // request is received on the connection and set to
    // false when a response is sent. If _responsePending
    // is true when a close connection request is processed,
    // a "DISCARDED_DATA" trace entry will be written.
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

    int _entry_index;

    // When used by the client, it is an offset (from start of http message)
    // representing last NON completely parsed chunk of a transfer encoding.
    // When used by the server, it is the message index that comes down
    // from the providers/repository representing each message chunk
    Uint32 _transferEncodingChunkOffset;

    // list of transfer encoding values from sender
    Array<String> _transferEncodingValues;

    // list of TE values from client
    Array<String> _transferEncodingTEValues;

    // 2 digit prefix on http header if mpost was used
    String _mpostPrefix;

    friend class Monitor;
    friend class HTTPAcceptor;
    friend class HTTPConnector;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_HTTPConnection_h */
