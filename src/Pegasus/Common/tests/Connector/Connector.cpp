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
// Author: Michael E. Brasher
//
//END_HISTORY

#include <cassert>
#include <iostream>
#include <Pegasus/Common/WindowsChannel.h>

using namespace Pegasus;
using namespace std;

int main()
{
    WindowsChannelConnector connector(0);

    Channel* channel = connector.connect("www.bmc.com:80");
    assert(channel);

    channel->enableBlocking();

    char MESSAGE[] = "GET / HTTP/1.0\r\n\r\n";

    Sint32 size = channel->writeN(MESSAGE, strlen(MESSAGE));

    cout << "size=" << size << endl;

    char buffer[17];

    for (;;)
    {
	memset(buffer, 0, sizeof(buffer));

	Sint32 size = channel->readN(buffer, sizeof(buffer)-1);

	cout << buffer;

	if (size <= 0)
	    break;
    }

    return 0;
}
