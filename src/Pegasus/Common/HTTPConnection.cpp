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

#include <iostream>
#include <cctype>
#include <cassert>
#include <cstdlib>
#include "HTTPConnection.h"
#include "MessageQueue.h"
#include "Socket.h"
#include "Monitor.h"

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
// HTTPMessage
//
////////////////////////////////////////////////////////////////////////////////

HTTPMessage::HTTPMessage(
    const Array<Sint8>& message_, 
    Uint32 returnQueueId_)
    :
    Message(HTTP_MESSAGE), 
    message(message_), 
    returnQueueId(returnQueueId_)
{

}

void HTTPMessage::parse(
    String& firstLine,
    Array<HTTPHeader>& headers,
    Sint8*& content,
    Uint32& contentLength) const
{
    firstLine.clear();
    headers.clear();
    content = "";
    contentLength = 0;

    char* data = (char*)message.getData();
    Uint32 size = message.size();
    char* line = (char*)data;
    char* sep;
    Boolean firstTime = true;

    while ((sep = _FindSeparator(line, size - (line - data))))
    {
	// Look for double separator which terminates the header?

	if (line == sep)
	{
	    // Establish pointer to content (account for "\n" and "\r\n").

	    content = line + ((*sep == '\r') ? 2 : 1);

	    // Determine length of content:

	    contentLength = message.size() - (content - data);
	    break;
	}

	Uint32 lineLength = sep - line;

	if (firstTime)
	    firstLine.assign(line, lineLength);
	else
	{
	    // Find the colon:

	    Sint8* colon = 0;

	    for (Uint32 i = 0; i < lineLength; i++)
	    {
		if (line[i] == ':')
		{
		    colon = &line[i];
		    break;
		}
	    }

	    // This should always be true:

	    if (colon)
	    {
		// Get the name part:

		Sint8* end;

		for (end = colon - 1; end > line && isspace(*end); end--)
		    ;

		end++;

		String name(line, end - line);

		// Get the value part:

		Sint8* start;

		for (start = colon + 1; start < sep && isspace(*start); start++)
		    ;

		String value(start, sep - start);

		headers.append(HTTPHeader(name, value));
	    }
	}

	line = sep + ((*sep == '\r') ? 2 : 1);
	firstTime = false;
    }
}

void HTTPMessage::print(ostream& os) const
{
    Message::print(os);

    String firstLine;
    Array<HTTPHeader> headers;
    Sint8* content;
    Uint32 contentLength;
    parse(firstLine, headers, content, contentLength);

    // Print the first line:

    cout << firstLine << endl;

    // Print the headers:

    Boolean image = false;

    for (Uint32 i = 0; i < headers.size(); i++)
    {
	cout << headers[i].first << ": " << headers[i].second << endl;

	if (String::equalNoCase(headers[i].first, "content-type"))
	{
	    if (headers[i].second.find("image/") == 0)
		image = true;
	}
    }

    cout << endl;

    // Print the content:

    for (Uint32 i = 0; i < contentLength; i++)
    {
	Sint8 c = content[i];

	if (image)
	{
	    if ((i % 60) == 0)
		cout << endl;

	    Sint8 c = content[i];

	    if (c >= ' ' && c < '~')
		cout << c;
	    else
		cout << '.';
	}
	else
	    cout << c;
    }

    cout << endl;
}

////////////////////////////////////////////////////////////////////////////////
//
// HTTPConnection
//
////////////////////////////////////////////////////////////////////////////////

HTTPConnection::HTTPConnection(
    Monitor* monitor,
    Sint32 socket, 
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
    Socket::disableBlocking(_socket);
}

HTTPConnection::~HTTPConnection()
{
    Socket::close(_socket);
}

void HTTPConnection::handleEnqueue()
{
    // cout << "HTTPConnection::handleEnqueue()" << endl;

    Message* message = dequeue();

    if (!message)
        return;

    if (getenv("PEGASUS_TRACE"))
        message->print(cout);

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

	    Socket::enableBlocking(_socket);

	    const Array<Sint8>& buffer = httpMessage->message;
	    const Uint32 CHUNK_SIZE = 16 * 1024;

	    for (Uint32 bytesRemaining = buffer.size(); bytesRemaining > 0; )
	    {
		Uint32 bytesToWrite = _Min(bytesRemaining, CHUNK_SIZE);

		Uint32 bytesWritten = Socket::write(
		    _socket, 
		    buffer.getData() + buffer.size() - bytesRemaining, 
		    bytesToWrite);

		if (bytesWritten < 0)
		    break;

		bytesRemaining -= bytesWritten;
	    }

	    Socket::disableBlocking(_socket);
	}

	default:
	    // ATTN: need unexpected message error!
	    break;
    };

    delete message;
}

Boolean _IsBodylessMessage(const char* line)
{
    const char* METHOD_NAMES[] =
    {
	"GET"
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
    Message* message = new CloseConnectionMessage(_socket);
    _ownerMessageQueue->enqueue(message);
}

void HTTPConnection::_handleReadEvent()
{
    // -- Append all data waiting on socket to incoming buffer:

    Sint32 bytesRead = 0;

    for (;;)
    {
	char buffer[4096];
	Sint32 n = Socket::read(_socket, buffer, sizeof(buffer));

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
	(_incomingBuffer.size() >= _contentLength + _contentOffset))
    {
	HTTPMessage* message = new HTTPMessage(_incomingBuffer, getQueueId());
	_outputMessageQueue->enqueue(message);
	_clearIncoming();

	if (bytesRead == 0)
	{
	    _closeConnection();
	    return;
	}
    }
}

PEGASUS_NAMESPACE_END
