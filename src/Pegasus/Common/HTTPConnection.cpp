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
//         Nag Boranna, Hewlett-Packard Company(nagaraja_boranna@hp.com)
//         Jenny Yu, Hewlett-Packard Company (jenny_yu@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>

#ifdef PEGASUS_PLATFORM_LINUX_IX86_GNU
#include <Pegasus/Common/Signal.h>
#endif

#include <iostream>
#include <cctype>
#include <cassert>
#include <cstdlib>
#include "Socket.h"
#include "TLS.h"
#include "HTTPConnection.h"
#include "MessageQueue.h"
#include "Monitor.h"
#include "HTTPMessage.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
//
// Local routines:
//
////////////////////////////////////////////////////////////////////////////////

static inline Uint32 _Min(Uint32 x, Uint32 y) 
{
    return x < y ? x : y; 
}

static char* _FindSeparator(const char* data, Uint32 size)
{
    const char* p = data;
    const char* end = p + size;

    while (p != end)
    {
	if (*p == '\r')
	{
	    Uint32 n = end - p;

	    if (n >= 2 && p[1] == '\n')
		return (char*)p;
	}
	else if (*p == '\n')
	    return (char*)p;

	p++;
    }

    return 0;
}

////////////////////////////////////////////////////////////////////////////////
//
// HTTPConnection
//
////////////////////////////////////////////////////////////////////////////////

HTTPConnection::HTTPConnection(
    Monitor* monitor,
    //Sint32 socket, 
    MP_Socket* socket, 
    MessageQueue* ownerMessageQueue,
    MessageQueue* outputMessageQueue)
    : 
    _monitor(monitor),
    _socket(socket), 
    _ownerMessageQueue(ownerMessageQueue),
    _outputMessageQueue(outputMessageQueue),
    _contentOffset(-1),
    _contentLength(-1)
{
    //Socket::disableBlocking(_socket);
    _socket->disableBlocking();
    _authInfo = new AuthenticationInfo();

    //
    // if ownerMessageQueue is HTTPAcceptor, get CIMServerState instance
    //
    HTTPAcceptor* acceptor = dynamic_cast<HTTPAcceptor*>(_ownerMessageQueue);
    if (acceptor)
    {
        _serverState = CIMServerState::getInstance();
    }
    else
    {
        _serverState = NULL;
    }
}

HTTPConnection::~HTTPConnection()
{
    _socket->close();
    delete _socket;
    delete _authInfo;
}

void HTTPConnection::handleEnqueue()
{
    Message* message = dequeue();

    if (!message)
        return;

    switch (message->getType())
    {
	case SOCKET_MESSAGE:
	{
	    SocketMessage* socketMessage = (SocketMessage*)message;

	    if (socketMessage->events & SocketMessage::READ)
		_handleReadEvent();

	    break;
	}

	case HTTP_MESSAGE:
	{
	    HTTPMessage* httpMessage = (HTTPMessage*)message;

	    // ATTN: convert over to asynchronous write scheme:

	    // Send response message to the client (use synchronous I/O for now:

            _socket->enableBlocking();

	    const Array<Sint8>& buffer = httpMessage->message;
	    const Uint32 CHUNK_SIZE = 16 * 1024;

#ifdef PEGASUS_PLATFORM_LINUX_IX86_GNU
            SignalHandler::ignore(SIGPIPE);

            //getSigHandle()->registerHandler(SIGSEGV,sig_act);
            //getSigHandle()->activate(SIGSEGV);
            // use the next two lines to test the SIGSEGV handler
            //Thread t(::segmentation_faulter,NULL,false);
            //t.run();
#endif
	    for (Uint32 bytesRemaining = buffer.size(); bytesRemaining > 0; )
	    {
		Uint32 bytesToWrite = _Min(bytesRemaining, CHUNK_SIZE);

		Sint32 bytesWritten = _socket->write(
		    buffer.getData() + buffer.size() - bytesRemaining, 
		    bytesToWrite);

		if (bytesWritten < 0)
		    break;
                    //throw ConnectionBroken();

		bytesRemaining -= bytesWritten;
	    }
            //
            // decrement request count
            //
            HTTPAcceptor* acceptor = 
                 dynamic_cast<HTTPAcceptor*>(_ownerMessageQueue);

            if (acceptor)
            {
                _serverState->decrementRequestCount();
            }

            _socket->disableBlocking();
	}

	default:
	    // ATTN: need unexpected message error!
	    break;
    };

    delete message;
}

const char* HTTPConnection::getQueueName() const
{
    return "HTTPConnection";
}

Boolean _IsBodylessMessage(const char* line)
{
    //ATTN: Make sure this is the right place to check for HTTP/1.1 and
    //      HTTP/1.0 that is part of the authentication challenge header.
    //
    const char* METHOD_NAMES[] =
    {
        "GET",
        "HTTP/1.1 401",
        "HTTP/1.0 401"
    };

    const Uint32 METHOD_NAMES_SIZE = sizeof(METHOD_NAMES) / sizeof(char*);

    for (Uint32 i = 0; i < METHOD_NAMES_SIZE; i++)
    {
	Uint32 n = strlen(METHOD_NAMES[i]);

	if (strncmp(line, METHOD_NAMES[i], n) == 0 && isspace(line[n]))
	    return true;
    }

    return false;
}

void HTTPConnection::_getContentLengthAndContentOffset()
{
    char* data = (char*)_incomingBuffer.getData();
    Uint32 size = _incomingBuffer.size();
    char* line = (char*)data;
    char* sep;
    Uint32 lineNum = 0;
    Boolean bodylessMessage = false;

    while ((sep = _FindSeparator(line, size - (line - data))))
    {
	char save = *sep;
	*sep = '\0';

	// Did we find the double separator which terminates the headers?

	if (line == sep)
	{
	    *sep = save;
	    line = sep + ((save == '\r') ? 2 : 1);
	    _contentOffset = line - _incomingBuffer.getData();
	    break;
	}

	// If this is one of the bodyless methods, then we can assume the
	// message is complete when the "\r\n\r\n" is encountered.

	if (lineNum == 0 && _IsBodylessMessage(line))
	    bodylessMessage = true;

	// Look for the content-length if not already found:

	char* colon = strchr(line, ':');

	if (colon)
	{
	    *colon  = '\0';

	    if (String::compareNoCase(line, "content-length") == 0)
		_contentLength = atoi(colon + 1);

	    *colon = ':';
	}

	*sep = save;
	line = sep + ((save == '\r') ? 2 : 1);
	lineNum++;
    }

    if (_contentOffset != -1 && bodylessMessage)
	_contentLength = 0;
}

void HTTPConnection::_clearIncoming()
{
    _contentOffset = -1;
    _contentLength = -1;
    _incomingBuffer.clear();
}

void HTTPConnection::_closeConnection()
{
    Message* message= new CloseConnectionMessage(_socket->getSocket());
    _ownerMessageQueue->enqueue(message);
}

void HTTPConnection::_handleReadEvent()
{
    // -- Append all data waiting on socket to incoming buffer:

    Sint32 bytesRead = 0;

    for (;;)
    {
	char buffer[4096];
        Sint32 n = _socket->read(buffer, sizeof(buffer));

	if (n <= 0)
	    break;

	_incomingBuffer.append(buffer, n);
	bytesRead += n;
    }

    // -- If still waiting for beginning of content!

    if (_contentOffset == -1)
	_getContentLengthAndContentOffset();

    // -- See if the end of the message was reached (some peers signal end of 
    // -- the message by closing the connection; others use the content length
    // -- HTTP header and then there are those messages which have no bodies
    // -- at all).

    if (bytesRead == 0 || 
	_contentLength != -1 && 
	(Sint32(_incomingBuffer.size()) >= _contentLength + _contentOffset))
    {
	HTTPMessage* message = new HTTPMessage(_incomingBuffer, getQueueId());
        message->authInfo = _authInfo;

        //
        // increment request count
        //
        HTTPAcceptor* acceptor = dynamic_cast<HTTPAcceptor*>(_ownerMessageQueue);

        if (acceptor)
        {
            _serverState->incrementRequestCount();
        }

	_outputMessageQueue->enqueue(message);
	_clearIncoming();

	if (bytesRead == 0)
	{
	    _closeConnection();

            //
            // decrement request count
            //
            if (acceptor)
            {
                _serverState->decrementRequestCount();
            }

	    return;
	}
    }
}

PEGASUS_NAMESPACE_END
