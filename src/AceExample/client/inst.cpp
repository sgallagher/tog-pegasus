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
