#include <ace/SOCK_Acceptor.h>
#include <ace/Acceptor.h>
#include <ace/INET_Addr.h>
#include "handler.h"

typedef ACE_Acceptor<Handler, ACE_SOCK_ACCEPTOR> Acceptor;

int main (int argc, char *argv[])
{
    ACE_INET_Addr addr(8888);

    Acceptor acceptor(addr, ACE_Reactor::instance());

    for (;;)
	ACE_Reactor::instance()->handle_events();

    return 0;
}
