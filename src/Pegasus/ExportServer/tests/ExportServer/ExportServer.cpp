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
// Modified By:   Dan Gorey, IBM (djgorey@us.ibm.com)
//                Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <cstdio>
#include <cassert>
#include <iostream>
#include <Pegasus/Common/HTTPAcceptor.h>
#include <Pegasus/Common/HTTPConnection.h>
#include <Pegasus/Common/HTTPMessage.h>
#include <Pegasus/Repository/CIMRepository.h>
#include <Pegasus/ExportServer/CIMExportRequestDecoder.h>
#include <Pegasus/ExportServer/CIMExportResponseEncoder.h>
#include <Pegasus/ExportServer/CIMExportRequestDispatcher.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

////////////////////////////////////////////////////////////////////////////////
//
// ListenerQueue
//
////////////////////////////////////////////////////////////////////////////////

class ListenerQueue : public MessageQueue
{
public:

    virtual void handleEnqueue();

    virtual const char* getQueueName() const { return "ListenerQueue"; }

    static Message* handleHTTPMessage(HTTPMessage* requestMessage);
};

void ListenerQueue::handleEnqueue()
{
    Message* message = dequeue();
    assert(message != 0);

    if (message->getType() == HTTP_MESSAGE)
    {
	// Down-cast the message to its derived type:

	HTTPMessage* requestHTTPMessage = (HTTPMessage*)message;

	// Print the incoming message:

	requestHTTPMessage->printAll(cout);
    }

    delete message;
}

////////////////////////////////////////////////////////////////////////////////
//
// main()
//
////////////////////////////////////////////////////////////////////////////////

int main()
{
    try
    {
	// Create a monitor to watch for activity on sockets:

        Monitor* monitor = new Monitor;

	CIMExportRequestDispatcher* dispatcher
	    = new CIMExportRequestDispatcher();

	CIMExportResponseEncoder* encoder
	    = new CIMExportResponseEncoder;

	CIMExportRequestDecoder* decoder = new CIMExportRequestDecoder(
	    dispatcher,
	    encoder->getQueueId());

	// Create an acceptor to wait for and accept connections on the
	// server port.

        HTTPAcceptor* acceptor = new HTTPAcceptor(monitor, decoder);

	// Bind the acceptor to listen on the given port:

	const Uint32 PORT_NUMBER = 8888;
	cout << "Binding to port " << PORT_NUMBER << "..." << endl;
	acceptor->bind(PORT_NUMBER);

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
