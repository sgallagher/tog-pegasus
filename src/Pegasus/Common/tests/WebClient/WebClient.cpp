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

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <Pegasus/Common/TCPChannel.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

static String globalDocument;
static String globalHost;

class MyChannelHandler : public ChannelHandler
{
public:

    MyChannelHandler()
    {
    }

    virtual ~MyChannelHandler()
    {
    }

    virtual Boolean handleOpen(Channel* channel)
    {
	char buffer[4096];
	char* tmp = globalDocument.allocateCString();
	char* tmpHost = globalHost.allocateCString();

	sprintf(buffer, 
	    "GET %s HTTP/1.1\r\n"
	    "Accept: */*\r\n"
	    "Accept-Language: en-us\r\n"
	    "Accept-Encoding: gzip, deflate\r\n"
	    "User-Agent: Mozilla/4.0 (compatible; MyClient; Windows NT 5.0)\r\n"
	    "Host: %s\r\n"
	    "Connection: Keep-Alive\r\n"
	    "\r\n",
	    tmp,
	    tmpHost);

	delete [] tmp;
	delete [] tmpHost;

	assert(channel->write(buffer, strlen(buffer)) == strlen(buffer));
	return true;
    }

    virtual Boolean handleInput(Channel* channel)
    {
	char buffer[1024];

	Sint32 size = channel->read(buffer, sizeof(buffer));

	if (size <= 0)
	    return false;

	for (Uint32 i = 0; i < size; i++)
	    cout << buffer[i];
	cout << flush;

	return true;
    }

    virtual Boolean handleOutput(Channel* channel)
    {
	return true;
    }

    virtual void handleClose(Channel* channel)
    {
	exit(0);
    }

private:

    Array<char> _received;
};

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

int main(int argc, char** argv)
{
    // Check the arguments:

    if (argc != 2)
    {
	cerr << "Usage: " << argv[0] << " url" << endl;
	exit(1);
    }

    // Parse the URL:

    ParseURL(argv[1], globalHost, globalDocument);

    ChannelHandlerFactory* factory 
	= new DefaultChannelHandlerFactory<MyChannelHandler>;

    Selector* selector = new Selector;

    TCPChannelConnector connector(factory, selector);

    char* tmpHost = globalHost.allocateCString();
    Channel* channel = connector.connect(tmpHost);
    delete [] tmpHost;

    if (channel == 0)
    {
	cerr << "Failed to connect to " << globalHost << endl;
	exit(1);
    }

    for (;;)
	selector->select(5000);

    PEGASUS_UNREACHABLE( return 0; )
}
