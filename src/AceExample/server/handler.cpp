#include "handler.h"
#include "iostream.h"

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

    return BaseClass::open(argument);
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

cout << "read " << n << " bytes" << endl;
    
    while (n > 0)
    {
	cout << "[" << buffer << "]" << endl;
	peer().send(buffer, n);
	n = peer().recv(buffer, sizeof(buffer));
    }

    return 0;
}

int Handler::handle_close(ACE_HANDLE handle, ACE_Reactor_Mask mask)
{
    cout << "Handler::handle_close()" << endl;

    // Invoke handle_close() in base class; this object will be
    // destructed.

    return BaseClass::handle_close(handle, mask);
}
