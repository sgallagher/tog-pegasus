//%2005////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
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
// Author: Mike Day (mdday@us.ibm.com)
//
// Modified By:  Amit K Arora, IBM (amita@in.ibm.com) for Bug#1188
//               David Dillard, VERITAS Software Corp.
//                   (david.dillard@veritas.com)
//		 Sean Keenan (sean.keenan@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/AsyncOpNode.h>
#include <Pegasus/Common/InternalException.h>

#if defined(PEGASUS_OS_VMS)
#include <stdio.h>
#endif

PEGASUS_NAMESPACE_BEGIN

#if defined(PEGASUS_PLATFORM_WIN32_IX86_MSVC)
#define snprintf _snprintf
#endif

// AsyncOpNode * AsyncOpNode::_headOfFreeList;
// const int AsyncOpNode::BLOCK_SIZE = 200;
// Mutex AsyncOpNode::_alloc_mut;

// void * AsyncOpNode::operator new(size_t size)
// {
//    if(size != sizeof(AsyncOpNode))
//       return :: operator new(size);

//    _alloc_mut.lock(pegasus_thread_self());

//    AsyncOpNode *node = _headOfFreeList;
//    if(node)
//       _headOfFreeList = node->_parent;
//    else
//    {
//       AsyncOpNode * newBlock =
// 	 reinterpret_cast<AsyncOpNode *>(::operator new( BLOCK_SIZE * sizeof(AsyncOpNode)));
//       int i;
//       for( i = 1; i < BLOCK_SIZE - 1; ++i)
// 	 newBlock[i]._parent = &newBlock[i + 1];
//       newBlock[BLOCK_SIZE - 1]._parent = NULL;
//       node = newBlock;
//       _headOfFreeList = &newBlock[1];
//    }
//    _alloc_mut.unlock();
//    return node;
// }

// void AsyncOpNode::operator delete(void *dead, size_t size)
// {

//    if(dead == 0)
//       return;
//    if(size != sizeof(AsyncOpNode))
//    {
//       ::operator delete(dead);
//       return;
//    }
//    AsyncOpNode *node = reinterpret_cast<AsyncOpNode *>(dead);
//    _alloc_mut.lock(pegasus_thread_self());
//    node->_parent = _headOfFreeList;
//    _headOfFreeList = node;
//    _alloc_mut.unlock();
// }


AsyncOpNode::AsyncOpNode(void)
   : _client_sem(0), _request(true), _response(true),
     _state(0), _flags(0), _offered_count(0), _total_ops(0), _completed_ops(0),
     _user_data(0), _completion_code(0), _op_dest(0),
     _parent(0), _children(true), _async_callback(0),__async_callback(0),
     _callback_node(0), _callback_response_q(0),
     _callback_ptr(0), _callback_parameter(0),
     _callback_handle(0), _callback_notify(0), _callback_request_q(0)
{
   gettimeofday(&_start, NULL);
   memset(&_lifetime, 0x00, sizeof(struct timeval));
   memset(&_updated, 0x00, sizeof(struct timeval));
    _timeout_interval.tv_sec = 60;
   _timeout_interval.tv_usec = 100;
}

AsyncOpNode::~AsyncOpNode(void)
{
   _request.empty_list();
   _response.empty_list();
}

void AsyncOpNode::_reset(unlocked_dq<AsyncOpNode> *dst_q)
{
   AsyncOpNode *child = _children.remove_first();
   while( child != 0 )
   {
      child->_reset(dst_q);
      child = _children.remove_first();
   }

   _parent = 0;
   _request.empty_list();
   _response.empty_list();

   //_operation_list.reset();
   _state = 0;
   _flags = 0;
   _offered_count = 0;
   _total_ops = 0;
   _completed_ops = 0;
   _user_data = 0;
   _completion_code = 0;
   _op_dest = 0;
   _async_callback = 0;
   __async_callback = 0;
   _callback_node =0;
   _callback_response_q = 0;
   _callback_ptr=0;
   _callback_parameter = 0;
   _callback_handle = 0;
   _callback_notify = 0;
   _callback_request_q = 0;
   dst_q->insert_first(this);
   while ( _client_sem.count() )
      _client_sem.wait();
   PEGASUS_ASSERT( _client_sem.count() == 0 );

   return;
}

void AsyncOpNode::print_to_buffer(char **buf)
{
   if(buf == NULL)
      return;
   
#if !defined(PEGASUS_PLATFORM_ZOS_ZSERIES_IBM)
   char work_buf[512];
   snprintf(work_buf, sizeof(work_buf), "AsyncOpNode %p\n" \
	    "\trq %d; rp %d; state %d; flags %d; op_dst q %p\n" \
	    "\tcallback node %p; callback rp q %p; callback ptr %p\n" \
	    "\tcallback parm %p; callback handle %p; callback notify %p\n" \
	    "\tcallback rq q %p; service %p; thread %p\n\n",
	    this, _request.count(), _response.count(), 
	    _state, _flags, _op_dest, _callback_node, _callback_response_q,
	    _callback_ptr, _callback_parameter, _callback_handle, _callback_notify,
	    _callback_request_q, _service_ptr, _thread_ptr);
   *buf = strdup(work_buf);
#endif
   return;
   
}

String AsyncOpNode::print_to_string(void)
{
   char work_buf[512];
#if !defined(PEGASUS_PLATFORM_ZOS_ZSERIES_IBM)
   snprintf(work_buf, sizeof(work_buf), "AsyncOpNode %p\n" \
	    "\trq %d; rp %d; state %d; flags %d; op_dst q %p\n" \
	    "\tcallback node %p; callback rp q %p; callback ptr %p\n" \
	    "\tcallback parm %p; callback handle %p; callback notify %p\n" \
	    "\tcallback rq q %p; service %p; thread %p\n\n",
	    this, _request.count(), _response.count(), 
	    _state, _flags, _op_dest, _callback_node, _callback_response_q,
	    _callback_ptr, _callback_parameter, _callback_handle, _callback_notify,
	    _callback_request_q, _service_ptr, _thread_ptr);
#else
   work_buf[0] = '\0';
#endif
    
   return String(work_buf);
}

PEGASUS_NAMESPACE_END
