//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software, Hewlett-Packard Company, IBM,
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

#include <Pegasus/Common/Config.h>
#include <cstdio>
#include <cassert>
#include <iostream>
#include <Pegasus/Common/HTTPConnector.h>
#include <Pegasus/Common/HTTPConnection.h>
#include <Pegasus/Common/HTTPMessage.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

////////////////////////////////////////////////////////////////////////////////
//
// WebClientQueue
//
////////////////////////////////////////////////////////////////////////////////

class WebClientQueue : public MessageQueue
{
public:
    WebClientQueue();

    virtual void handleEnqueue();
};

WebClientQueue::WebClientQueue()
    : MessageQueue("WebClientQueue TEST")
{
}

void WebClientQueue::handleEnqueue()
{
    Message* message = dequeue();
    assert(message != 0);

    if (message->getType() == HTTP_MESSAGE)
    {
	HTTPMessage* httpMessage = (HTTPMessage*)message;
	httpMessage->printAll(cout);
	exit(0);
    }
}

////////////////////////////////////////////////////////////////////////////////
//
// Local routines:
//
////////////////////////////////////////////////////////////////////////////////

void ParseURL(const String& url, String& host, String& document)
{
    if (url.find("http://") != 0)
    {
	cerr << "Malformed URL: must begin with \"http://\"" << endl;
	exit(1);
    }

    // Extract the host:

    String rem = url.subString(7);
    Uint32 slash = rem.find('/');
    host = rem.subString(0, slash);

    if (host.find(':') == PEGASUS_NOT_FOUND)
	host += ":80";

    if (slash == PEGASUS_NOT_FOUND)
	document = "/";
    else
    {
	document = rem.subString(slash);
    }
}

void GetDocument(
    HTTPConnection* connection, 
    const String& host,
    const String& document)
{
    // Build HTTP (request) message:

    char buffer[4096];
    char* tmpDocument = document.allocateCString();
    char* tmpHost = host.allocateCString();

    sprintf(buffer, 
	"GET %s HTTP/1.1\r\n"
	"Accept: */*\r\n"
	"Accept-Language: en-us\r\n"
	"Accept-Encoding: gzip, deflate\r\n"
	"User-Agent: Mozilla/4.0 (compatible; MyClient; Windows NT 5.0)\r\n"
	"Host: %s\r\n"
	"Connection: Keep-Alive\r\n"
	"\r\n",
	tmpDocument,
	tmpHost);

    delete [] tmpDocument;
    delete [] tmpHost;

    Array<Sint8> message;
    message.append(buffer, strlen(buffer));
    HTTPMessage* httpMessage = new HTTPMessage(message);

    // Enqueue message on the connection's queue (so that it will be sent
    // to the sever it is conneted to:

    // httpMessage->print(cout);

    connection->enqueue(httpMessage);
}

////////////////////////////////////////////////////////////////////////////////
//
// main()
//
////////////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv)
{
    // Check the arguments:

    if (argc != 2)
    {
	cerr << "Usage: " << argv[0] << " url" << endl;
	exit(1);
    }

    // Parse the URL:

    String url = argv[1];
    String host;
    String document;
    ParseURL(url, host, document);

    // Connect to server and get the document:

    try
    {
	Monitor* monitor = new Monitor;
	WebClientQueue* webClientQueue = new WebClientQueue;
	HTTPConnector* httpConnector = new HTTPConnector(monitor);

	HTTPConnection* connection 
	    = httpConnector->connect(host, webClientQueue);

	GetDocument(connection, host, document);

	for (;;)
	{
	    cout << "Loop..." << endl;
	    monitor->run(5000);
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
