//BEGIN_LICENSE
//
// Copyright (c) 2000 The Open Group, BMC Software, Tivoli Systems, IBM
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
//END_LICENSE
//BEGIN_HISTORY
//
// Author:
//
// $Log: Handler.cpp,v $
// Revision 1.4  2001/04/12 09:57:40  mike
// Post Channel Port to Linux
//
// Revision 1.1.1.1  2001/01/14 19:53:51  mike
// Pegasus import
//
//
//END_HISTORY

#include <iostream>
#include <cctype>
#include <cassert>
#include <cstdlib>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/XmlParser.h>
#include <Pegasus/Common/XmlWriter.h>
#include "Handler.h"

using namespace std;

PEGASUS_NAMESPACE_BEGIN

#define D(X) // X

Handler::Handler()
{
    D( cout << "Handler::Handler()" << endl; )
}

Handler::~Handler()
{
    D( cout << "Handler::~Handler()" << endl; )
}

Boolean Handler::handleOpen(Channel* channel)
{
    D( cout << "Handler::handleOpen()" << endl; )

    channel->disableBlocking();
    clear();
    return true;
}

void Handler::clear()
{
    _state = WAITING;
    _message.clear();
    _lines.clear();
    _contentOffset = 0;
    _contentLength = 0;
}

void Handler::skipWhitespace(const char*& p)
{
    while (isspace(*p))
	p++;
}

const char* Handler::getFieldValue(const char* fieldName) const
{
    const char* message = _message.getData();
    Uint32 fieldNameLength = strlen(fieldName);

    for (Uint32 i = 1; i < _lines.getSize(); i++)
    {
	const char* line = &message[_lines[i]];

	if (strncmp(line, fieldName, fieldNameLength) == 0 &&
	    line[fieldNameLength] == ':')
	{
	    const char* p = line + fieldNameLength + 1;

	    while (isspace(*p))
		p++;

	    return p;
	}
    }

    return 0;
}

const char* Handler::getFieldValueSubString(const char* str) const
{
    const char* message = _message.getData();

    for (Uint32 i = 1; i < _lines.getSize(); i++)
    {
	const char* line = &message[_lines[i]];

	if (strstr(line, str))
	{
	    const char* p = strchr(line, ':');

	    if (!p)
		return 0;

	    p++;
	    
	    while (isspace(*p))
		p++;

	    return p;
	}
    }

    return 0;
}

const char* Handler::getContent() const 
{
    return _message.getData() + _contentOffset;
}

Uint32 Handler::getContentLength() const
{
    const char* p = getFieldValue("Content-Length");

    if (!p)
	return Uint32(-1);

    return atoi(p);
}

void Handler::print() const
{
    // Print out the header:

    const char* message = _message.getData();

    for (Uint32 i = 0; i < _lines.getSize(); i++)
    {
	cout << &message[_lines[i]] << "\r\n";
    }

    cout << "\r\n";

    // Print out the content:

    const char* content = _message.getData() + _contentOffset;
    const char* end = content + _contentLength;

    ((Array<Sint8>&)_message).append('\0');
    XmlWriter::indentedPrint(cout, content);
    ((Array<Sint8>&)_message).remove(_message.getSize() - 1);
}

static char* _FindTerminator(const char* data, Uint32 size)
{
    const char* p = data;
    const char* end = p + size;

    while (p != end)
    {
	if (*p == '\r')
	{
	    Uint32 n = end - p;

	    if (n >= 4 && p[1] == '\n' && p[2] == '\r' && p[3] == '\n')
		return (char*)p;
	}

	p++;
    }

    return 0;
}

Boolean Handler::handleInput(Channel* channel)
{
    D( cout << "Handler::handleInput()" << endl; )

    //--------------------------------------------------------------------------
    // If in the waiting state, set start state to header state.
    //--------------------------------------------------------------------------

    if (_state == WAITING) 
	_state = LINES;

    //--------------------------------------------------------------------------
    // Read all incoming data (non-blocking):
    //--------------------------------------------------------------------------

    char buffer[4096];
    Uint32 totalBytesRead = 0;
    int bytesRead;

    while ((bytesRead = channel->read(buffer, sizeof(buffer))) > 0)
    {
	_message.append(buffer, bytesRead);
	totalBytesRead += bytesRead;
    }

    if (totalBytesRead == 0)
    {
	// We check here for end of message body because some
	// implementations signal the end of the message by closing
	// the connection rather than using a Content-Length field.
	// If the content length field was encountered, then the handler
	// has already been called and the state already set back to
	// waiting.

	if (_state == CONTENT)
	{
	    _state = DONE;
	    _contentLength = _message.getSize() - _contentOffset;
	    D( cout << "handleMessage(); closed connection" << endl; )

	    if (handleMessage() != 0)
		return false;

	    clear();
	    _state = WAITING;
	}

	return false;
    }

    //--------------------------------------------------------------------------
    // Process what we have so far:
    //--------------------------------------------------------------------------

    switch (_state)
    {
	case LINES:
	{
	    char* m = (char*)_message.getData();
	    Uint32 mSize = _message.getSize();
	    char* term = _FindTerminator(m, mSize);

	    if (term)
	    {
		*term = '\0';

		for (char* p = strtok(m, "\r\n"); p; p = strtok(NULL, "\r\n"))
		    _lines.append(p - m);

		_contentOffset = (term + 4) - m;
		_contentLength = 0;

		// If this is a GET operation, go straight to done state
		// (GET operations don't have bodies).

		if (strncmp(m, "GET", 3) == 0 && isspace(m[3]))
		{
		    _state = DONE;
		    D( cout << "handleMessage(); GET" << endl; )

		    if (handleMessage() != 0)
			return false;

		    clear();
		    _state = WAITING;
		    break;
		}

		// Now we move into content fetching state:

		_state = CONTENT;

		// Attempt to extract the content-length (if it returns
		// -1, then the content is finished when the connection
		// is closed.

		_contentLength = getContentLength();

		// Fall through to next case!
	    }
	}

	case CONTENT:
	{
	    Uint32 currentContentLength = _message.getSize() - _contentOffset;

	    if (_contentLength != Uint32(-1)
		&& currentContentLength == _contentLength)
	    {
		_state = DONE;

		D(cout << "handleMessage(); content-length" << endl;)

		// Null terminate the content:

		_message.append('\0');

		if (handleMessage() != 0)
		    return false;

		clear();
		_state = WAITING;
	    }

	    break;
	}

	default:
	    assert(0);
	    break;
    }

    return true;
}

Boolean Handler::handleOutput(Channel* channel)
{
    return true;
}

void Handler::handleClose(Channel* channel)
{
    D( cout << "Handler::handleClose()" << endl; )
}

int Handler::handleMessage()
{
    D( cout << "Handler::handleMessage()" << endl; )

    if (getenv("PEGASUS_TRACE_PROTOCOL"))
    {
	cout << "========== RECEIVED ==========" << endl; 
	print();
    }

    // printMessage(cout, _message);

    return 0;
}

void Handler::printMessage(ostream& os, const Array<Sint8>& message)
{
    // Find separator between HTTP headers and content:

    const char* p = message.getData();
    const char* separator = 0;

    for (; *p; p++)
    {
	if (p[0] == '\r' && p[1] == '\n' && p[2] == '\r' && p[3] == '\n')
	{
	    p += 4;
	    separator = p - 1;
	    break;
	}
    }

    if (!separator)
	return;

    *((char*)separator) = '\0';

    // Print HTTP Headers:

    os << message.getData() << endl;

    // Print the body:

    ((Array<Sint8>&)message).append('\0');

    XmlWriter::indentedPrint(os, p);

    *((char*)separator) = '\n';
    ((Array<Sint8>&)message).remove(message.getSize() - 1);
}

PEGASUS_NAMESPACE_END
