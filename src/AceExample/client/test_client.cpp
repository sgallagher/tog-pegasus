#include <ace/SOCK_Connector.h>
#include <ace/INET_Addr.h>
#include <ace/Reactor.h>
#include <ace/WFMO_Reactor.h>
#include "handler.h"

typedef ACE_Connector<Handler, ACE_SOCK_CONNECTOR> Connector;

int main (int argc, char *argv[])
{
    ACE_INET_Addr addr(8888, "localhost");

    Handler* handler = new Handler;

    Connector connector;

    if (connector.connect(handler, addr) == -1)
    {
	cerr << argv[0] << ": failed to connect" << endl;
	exit(1);
    }

    for (;;)
	ACE_Reactor::instance()->handle_events();

    return 0;
}
