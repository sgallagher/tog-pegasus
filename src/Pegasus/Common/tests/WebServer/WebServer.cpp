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
// Modified By:    Dan Gorey, IBM (djgorey@us.ibm.com)
//                 Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <cstdio>
#include <cassert>
#include <iostream>
#include <Pegasus/Common/HTTPAcceptor.h>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/HTTPConnection.h>
#include <Pegasus/Common/HTTPMessage.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

////////////////////////////////////////////////////////////////////////////////
//
// WebServerQueue
//
////////////////////////////////////////////////////////////////////////////////

class WebServerQueue : public MessageQueue
{
public:

    WebServerQueue();

    virtual void handleEnqueue();

    static Message* handleHTTPMessage(HTTPMessage* requestMessage);
};

WebServerQueue::WebServerQueue()
    : MessageQueue("WebServer")
{
}

void WebServerQueue::handleEnqueue()
{
    Message* message = dequeue();
    assert(message != 0);

    if (message->getType() == HTTP_MESSAGE)
    {
	// Down-cast the message to its derived type:

	HTTPMessage* requestHTTPMessage = (HTTPMessage*)message;

	// Form the response message:

	Message* responseHTTPMessage 
	    = handleHTTPMessage(requestHTTPMessage);

	// Put the response back on the indicated queue:

	if (responseHTTPMessage)
	{
	    MessageQueue* queue = MessageQueue::lookup(
		requestHTTPMessage->queueId);

	    assert(queue != 0);

	    queue->enqueue(responseHTTPMessage);
	}
    }

    delete message;
}

Message* WebServerQueue::handleHTTPMessage(HTTPMessage* requestMessage)
{
    String startLine;
    Array<HTTPHeader> headers;
    Uint32 contentLength;

    requestMessage->parse(startLine, headers, contentLength);

    // requestMessage->print(cout);

    // Parse the request-line:

    String methodName;
    String requestUri;
    String httpVersion;

    HTTPMessage::parseRequestLine(
	startLine, methodName, requestUri, httpVersion);

    // Handle GET requests:

    if (methodName == "GET")
    {
	// Extract and expand document name:

	if (requestUri == "/")
	    requestUri = "index.html";

	String fullDocumentName = "htdocs/";
	fullDocumentName.append(requestUri);

	// Load the document into memory:

	Array<char> content;

	try
	{
	    FileSystem::loadFileToMemory(content, fullDocumentName);
	}
	catch (CannotOpenFile&)
	{
	    const char HEADER[] =
		"HTTP/1.1 404 Object Not Found\r\n"
		"Server: WebServer/1.0.0\r\n"
		"Content-Length: %d\r\n"
		"Content-Type: text/html\r\n"
		"\r\n";

	    const char CONTENT[] =
		"<html>\n"
		"  <head>\n"
		"    <title>Not Found</title>\n"
		"  </head>\n"
		"  <body>\n"
		"  <h1>404: Not Found</h1>\n"
		"  </body>\n"
		"</html>\n";

	    char header[256];
	    sprintf(header, HEADER, strlen(CONTENT));

	    Array<char> buffer;
	    buffer.append(header, strlen(header));
	    buffer.append(CONTENT, strlen(CONTENT));

	    return new HTTPMessage(buffer);
	}

	// Resolve the document type:

	char documentType[32];
	strcpy(documentType, "text/plain");

	Uint32 dot = fullDocumentName.reverseFind('.');

	if (dot != PEG_NOT_FOUND)
	{
	    String ext = fullDocumentName.subString(dot);

	    if (ext == ".jpg")
		strcpy(documentType, "image/jpeg");
	    else if (ext == ".gif")
		strcpy(documentType, "image/gif");
	    else if (ext == ".html")
		strcpy(documentType, "text/html");
	}

	const char HEADER[] =
	    "HTTP/1.1 200 OK\r\n"
	    "Server: Pegasus 1.0\r\n"
	    "Content-Length: %d\r\n"
	    "Content-Type: %s \r\n\r\n";

	char header[sizeof(HEADER) + 32];
	sprintf(header, HEADER, content.size(), documentType);

	Array<char> buffer;
	buffer.append(header, strlen(header));
	buffer.append(content.getData(), content.size());
	return new HTTPMessage(buffer);
    }

    return 0;
}

////////////////////////////////////////////////////////////////////////////////
//
// main()
//
////////////////////////////////////////////////////////////////////////////////

int main()
{
    if (!FileSystem::isDirectory("./htdocs"))
    {
	cerr << "Failed to open directory \"./htdocs\"" << endl;
	exit(1);
    }

    try
    {
	// Create a monitor to watch for activity on sockets:

        Monitor* monitor = new Monitor;

	// Create a queue to receive incoming HTTP messages:

	WebServerQueue* webServerQueue = new WebServerQueue;

	// Create an acceptor to wait for and accept connections on the
	// server port.

	const Uint32 PORT_NUMBER = 7777;
        HTTPAcceptor* httpAcceptor = new HTTPAcceptor(
            monitor, webServerQueue, false, PORT_NUMBER, 0, false);

	// Bind the acceptor to listen on the given port:

	httpAcceptor->bind();

	cout << "Binding to port " << PORT_NUMBER << endl;

	// Run the main loop (timeout after five seconds):

	const Uint32 FIVE_SECONDS_MSEC = 5 * 1000;

	for (;;)
	{
	    // cout << "Loop..." << endl;
	    monitor->run(FIVE_SECONDS_MSEC);
	}
    }
    catch (Exception& e)
    {
	cerr << e.getMessage() << endl;
	exit(1);
    }

    exit(0);
    PEGASUS_UNREACHABLE( return 0; )
}
