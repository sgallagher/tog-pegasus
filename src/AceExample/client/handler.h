#ifndef _handler_h
#define _handler_h

#include <ace/Service_Config.h>
#include <ace/Connector.h>
#include <ace/SOCK_Stream.h>
#include <ace/OS.h>

class Handler : public ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_NULL_SYNCH>
{
public:

    Handler(ACE_Reactor* reactor = 0);

    virtual ~Handler();

    virtual int open(void* = 0);

    virtual int handle_input(ACE_HANDLE);

    virtual int handle_output(ACE_HANDLE);

    virtual int handle_close(ACE_HANDLE, ACE_Reactor_Mask mask);

    typedef ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_NULL_SYNCH> BaseClass;
};

#endif /* _handler_h */
