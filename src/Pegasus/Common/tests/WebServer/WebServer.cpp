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

////////////////////////////////////////////////////////////////////////////////
//
// TODO:
//
//     Handle different document types (other than just HTML).
//     Generate no such document error.
//     Generate no such method error (not implemented).
//
////////////////////////////////////////////////////////////////////////////////

#include <cassert>
#include <iostream>
#include <Pegasus/Common/HTTPAcceptor.h>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/HTTPConnection.h>

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

    virtual void handleEnqueue();

    static Message* handleHTTPMessage(HTTPMessage* requestMessage);
};

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
		requestHTTPMessage->returnQueueId);

	    assert(queue != 0);

	    queue->enqueue(responseHTTPMessage);
	}
    }

    delete message;
}

Message* WebServerQueue::handleHTTPMessage(HTTPMessage* requestMessage)
{
    String firstLine;
    Array<HTTPMessage::HTTPHeader> headers;
    Sint8* content;
    Uint32 contentLength;

    requestMessage->parse(firstLine, headers, content, contentLength);

    requestMessage->print(cout);

    // Split the first line about the method name and the rest:

    Uint32 space = firstLine.find(' ');
    String methodName = firstLine.subString(0, space);
    String remainder = firstLine.subString(space + 1);

    // Handle GET requests:

    if (methodName == "GET")
    {
	// Extract and expand document name:

	String documentName = remainder.subString(0, remainder.find(' '));

	if (documentName == "/")
	    documentName = "index.html";

	String fullDocumentName = "htdocs/";
	fullDocumentName += documentName;

PEGASUS_OUT(fullDocumentName);

	// Load the document into memory:

	const char HEADER[] =
	    "HTTP/1.1 200 OK\r\n"
	    "Server: Pegasus 1.0\r\n"
	    "Content-Length: %d\r\n"
	    "Content-Type: %s \r\n\r\n";

	Array<Sint8> content;

	try
	{
	    FileSystem::loadFileToMemory(content, fullDocumentName);
	}
	catch (CannotOpenFile& e)
	{
	    cout << "Error: " << e.getMessage() << endl;
	    return 0;
	}

	// Resolve the document type:

	char docuentType[32];
	strcpy(docuentType, "text/plain");

	Uint32 dot = fullDocumentName.reverseFind('.');

	if (dot != PEGASUS_NOT_FOUND)
	{
	    String ext = fullDocumentName.subString(dot + 1);
	    PEGASUS_OUT(ext);

	    if (ext == ".jpg")
		strcpy(docuentType, "image/jpeg");
	    else if (ext == ".gif")
		strcpy(docuentType, "image/jpeg");
	    else if (ext == ".html")
		strcpy(docuentType, "text/html");
	}

PEGASUS_OUT(docuentType);

	char header[sizeof(HEADER) + 32];
	sprintf(header, HEADER, content.size(), docuentType);

	Array<Sint8> buffer;
	buffer.append(header, strlen(header));
	buffer.append(content.getData(), content.size());
	return new HTTPMessage(buffer);
    }

    return 0;

#if 0


#endif
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

	WebServerQueue* webServerQueueueue = new WebServerQueue;

	// Create an acceptor to wait for and accept connections on the
	// server port.

	HTTPAcceptor* httpAcceptor 
	    = new HTTPAcceptor(monitor, webServerQueueueue);

	// Bind the acceptor to listen on the given port:

	const Uint32 PORT_NUMBER = 7777;
	httpAcceptor->bind(PORT_NUMBER);

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
