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

class MyChannelHandler : public ChannelHandler
{
public:

    MyChannelHandler()
    {
	cout << "MyChannelHandler::MyChannelHandler()" << endl;
    }

    virtual ~MyChannelHandler()
    {
	cout << "MyChannelHandler::~MyChannelHandler()" << endl;
    }

    virtual Boolean handleOpen(Channel* channel)
    {
	cout << "MyChannelHandler::handleOpen()" << endl;
	return true;
    }

    virtual Boolean handleInput(Channel* channel)
    {
	cout << "MyChannelHandler::handleInput()" << endl;

	char buffer[1024];

	for (;;)
	{
	    Sint32 size = channel->read(buffer, sizeof(buffer));

	    if (size == 0)
		return false;

#if 0
	    for (Uint32 i = 0; i < size; i++)
		cout << buffer[i];
#endif
	}

	return true;
    }

    virtual Boolean handleOutput(Channel* channel)
    {
	cout << "MyChannelHandler::handleOutput()" << endl;
	return true;
    }

    virtual void handleClose(Channel* channel)
    {
	cout << "MyChannelHandler::handleClose()" << endl;
    }
};

int main()
{
    ChannelHandlerFactory* factory 
	= new DefaultChannelHandlerFactory<MyChannelHandler>;

    Selector* selector = Selector::create();

    WindowsChannelConnector connector(factory, selector);

    Channel* channel = connector.connect("www.bmc.com:80");
    assert(channel);

    channel->enableBlocking();

    char MESSAGE[] = "GET / HTTP/1.0\r\n\r\n";

    Sint32 size = channel->writeN(MESSAGE, strlen(MESSAGE));
    assert(size == strlen(MESSAGE));

    while (true)
    {
	cout << "Loop..." << endl;
	selector->select(5000);
    }

#if 0

    char buffer[17];

    for (;;)
    {
	memset(buffer, 0, sizeof(buffer));

	Sint32 size = channel->readN(buffer, sizeof(buffer)-1);

	cout << buffer;

	if (size <= 0)
	    break;
    }
#endif

    return 0;
}
