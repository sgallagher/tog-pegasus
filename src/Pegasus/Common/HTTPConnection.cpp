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

#include "HTTPConnection.h"
#include "MessageQueue.h"
#include "Socket.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
//
// HTTPConnection
//
////////////////////////////////////////////////////////////////////////////////

HTTPConnection::HTTPConnection(Sint32 socket, MessageQueue* ownerMessageQueue) 
    : _socket(socket), _ownerMessageQueue(ownerMessageQueue)
{

}

HTTPConnection::~HTTPConnection()
{
    Socket::close(_socket);
}

void HTTPConnection::handleEnqueue()
{
    cout << "HTTPConnection::handleEnqueue()" << endl;

    Message* message = dequeue();

    if (!message)
        return;

    if (getenv("PEGASUS_TRACE"))
        message->print(cout);

    switch (message->getType())
    {
	case SOCKET_MESSAGE:
	{
	    break;
	}

	default:
	    // ATTN: need unexpected message error!
	    break;
    };

    delete message;
}

PEGASUS_NAMESPACE_END
