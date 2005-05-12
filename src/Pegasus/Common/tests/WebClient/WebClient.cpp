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
// Modified By: Carol Ann Krug Graves, Hewlett-Packard Company
//                  (carolann_graves@hp.com)
//              Dan Gorey, IBM (djgorey@us.ibm.com)
//              David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
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
     #ifdef PEGASUS_DEBUG
       HTTPMessage* httpMessage = (HTTPMessage*)message;
       httpMessage->printAll(cout);
     #endif
	exit(0);
    }
}

////////////////////////////////////////////////////////////////////////////////
//
// Local routines:
//
////////////////////////////////////////////////////////////////////////////////

void ParseURL(const String& url, String& host, Uint32& portNumber, 
    String& document)
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

    Uint32 index = host.find (':');
    if (index == PEG_NOT_FOUND)
    {
        portNumber = 80;
    }
    else
    {
        host = host.subString (0, index);
        portNumber = 0;
        String portStr = rem.subString (index + 1, slash);
        sscanf (portStr.getCString (), "%u", &portNumber);
    }

    if (slash == PEG_NOT_FOUND)
	document = "/";
    else
    {
	document = rem.subString(slash);
    }
}

void GetDocument(
    HTTPConnection* connection, 
    const String& host,
    const Uint32 portNumber,
    const String& document)
{
    // Build HTTP (request) message:

    char buffer[4096];

    sprintf(buffer, 
	"GET %s HTTP/1.1\r\n"
	"Accept: */*\r\n"
	"Accept-Language: en-us\r\n"
	"Accept-Encoding: gzip, deflate\r\n"
	"User-Agent: Mozilla/4.0 (compatible; MyClient; Windows NT 5.0)\r\n"
	"Host: %s:%u\r\n"
	"Connection: Keep-Alive\r\n"
	"\r\n",
	(const char*)document.getCString(),
        (const char*)host.getCString(),
        portNumber);

    Array<char> message;
    message.append(buffer, strlen(buffer));
    HTTPMessage* httpMessage = new HTTPMessage(message);

    // Enqueue message on the connection's queue (so that it will be sent
    // to the server it is conneted to:

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
    Uint32 portNumber;
    String document;
    ParseURL(url, host, portNumber, document);

    // Connect to server and get the document:

    try
    {
 WebClientQueue* webClientQueue = new WebClientQueue;
  Monitor* monitor = new Monitor;
  HTTPConnector* httpConnector = new HTTPConnector(monitor);
  HTTPConnection* connection
	    = httpConnector->connect(host, portNumber, webClientQueue);

	GetDocument(connection, host, portNumber, document);

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
