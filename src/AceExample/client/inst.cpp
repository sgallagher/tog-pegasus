#include <ace/SOCK_Connector.h>
#include <ace/INET_Addr.h>
#include <ace/Reactor.h>
#include <ace/WFMO_Reactor.h>
#include "handler.h"

#if defined (ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION)
 template class ACE_Connector<Handler, ACE_SOCK_CONNECTOR>;
 template class ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_SYNCH>;
 template class ACE_Svc_Tuple<Handler>;
 template class ACE_Map_Entry<ACE_HANDLE, ACE_Svc_Tuple<Handler> *>;
 template class ACE_Map_Iterator_Base<
    ACE_HANDLE, ACE_Svc_Tuple<Handler> *, ACE_SYNCH_RW_MUTEX>;
 template class ACE_Map_Iterator<
    ACE_HANDLE, ACE_Svc_Tuple<Handler> *, ACE_SYNCH_RW_MUTEX>;
 template class ACE_Map_Reverse_Iterator<
    ACE_HANDLE, ACE_Svc_Tuple<Handler> *, ACE_SYNCH_RW_MUTEX>;
 template class ACE_Map_Manager<
    ACE_HANDLE, ACE_Svc_Tuple<Handler> *, ACE_SYNCH_RW_MUTEX>;
#elif defined (ACE_HAS_TEMPLATE_INSTANTIATION_PRAGMA)
# pragma instantiate ACE_Connector<Handler, ACE_SOCK_CONNECTOR>
# pragma instantiate ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_SYNCH>
# pragma instantiate ACE_Svc_Tuple<Handler>
# pragma instantiate ACE_Map_Entry<ACE_HANDLE, ACE_Svc_Tuple<Handler> *>
# pragma instantiate ACE_Map_Iterator_Base<
    ACE_HANDLE, ACE_Svc_Tuple<Handler> *, ACE_SYNCH_RW_MUTEX>
# pragma instantiate ACE_Map_Iterator<
    ACE_HANDLE, ACE_Svc_Tuple<Handler> *, ACE_SYNCH_RW_MUTEX>
# pragma instantiate ACE_Map_Reverse_Iterator<
    ACE_HANDLE, ACE_Svc_Tuple<Handler> *, ACE_SYNCH_RW_MUTEX>
# pragma instantiate ACE_Map_Manager<
    ACE_HANDLE, ACE_Svc_Tuple<Handler> *, ACE_SYNCH_RW_MUTEX>
#endif /* ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION */
