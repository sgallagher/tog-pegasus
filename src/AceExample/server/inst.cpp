#include <ace/SOCK_Acceptor.h>
#include <ace/INET_Addr.h>
#include <ace/Acceptor.h>
#include "handler.h"

#if defined (ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION)
template class ACE_Concurrency_Strategy<Handler>;
template class ACE_Acceptor<Handler, ACE_SOCK_ACCEPTOR>;
template class ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_NULL_SYNCH>;
// template class Svc_Handler<ACE_SOCK_STREAM>;
#elif defined (ACE_HAS_TEMPLATE_INSTANTIATION_PRAGMA)
#pragma instantiate ACE_Concurrency_Strategy<Handler>
#pragma instantiate ACE_Acceptor<Handler, ACE_SOCK_ACCEPTOR>
#pragma instantiate ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_NULL_SYNCH>
// #pragma instantiate Svc_Handler<ACE_SOCK_STREAM>
#endif /* ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION */
