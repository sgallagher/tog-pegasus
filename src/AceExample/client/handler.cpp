//%2003////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002  BMC Software, Hewlett-Packard Development
// Company, L. P., IBM Corp., The Open Group, Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L. P.;
// IBM Corp.; EMC Corporation, The Open Group.
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
#include <iostream>
#include "handler.h"

Handler::Handler(ACE_Reactor *reactor)
{
    this->reactor(reactor);
}

Handler::~Handler()
{
    cout << "Handler::~Handler()" << endl;
}

int Handler::open(void* argument)
{
    cout << "Handler::open()" << endl;

    // Enable non-blocking I/O:

    peer().enable(ACE_NONBLOCK);

    // Invoke open() in base class; the object will be registered
    // to receive read events. This could be done here like this:
    //
    //     ACE_Reactor::instance()->register_handler(
    //     this, ACE_Event_Handler::READ_MASK);

    int result = BaseClass::open(argument);

    if (result != 0)
    {
	cerr << "BaseClass::open() failed" << endl;
	return result;
    }

    // Write mesasge:

    const char MESSAGE[] = "Hello World\n";
    peer().send_n(MESSAGE, sizeof(MESSAGE));

    return 0;
}

int Handler::handle_input(ACE_HANDLE)
{
    cout << "Handler::handle_input()" << endl;

    char buffer[1024];
    int n = peer().recv(buffer, sizeof(buffer));

    if (n == 0)
    {
	cout << "Remote side hung up" << endl;
	return -1;
    }
    
    while (n > 0)
    {
	cout << "[" << buffer << "]" << endl;
	n = peer().recv(buffer, sizeof(buffer));
    }

    return 0;
}

int Handler::handle_output(ACE_HANDLE)
{
    cout << "Handler::handle_output()" << endl;
    return 0;
}

int Handler::handle_close(ACE_HANDLE handle, ACE_Reactor_Mask mask)
{
    cout << "Handler::handle_close()" << endl;

    // Invoke handle_close() in base class; this object will be
    // destructed.

    return BaseClass::handle_close(handle, mask);
}
