//%//////////////////////////////////////////////////////////////////////////
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
// Author: Mike Day (mdday@us.ibm.com)
//
// Modified By: 
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/AsyncOpNode.h>

PEGASUS_NAMESPACE_BEGIN
   
AsyncOpNode::AsyncOpNode(void) 
   : _client_sem(1), _mut(), _request(0), _response(0), 
     _state(0), _flags(0), _total_ops(0), _completed_ops(0), 
     _parent(0), _children(true)
{
   gettimeofday(&_start, NULL);
   memset(&_lifetime, 0x00, sizeof(struct timeval));
   memset(&_updated, 0x00, sizeof(struct timeval));
   memset(&_timeout_interval, 0xff, sizeof(struct timeval));
}

AsyncOpNode::~AsyncOpNode(void)
{
   delete _request;
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
   delete _request;
   _request = 0;
   _response.empty_list();
   _operation_list.reset();
   _state = 0;
   _flags = 0;
   _total_ops = 0;
   _completed_ops = 0;
   dst_q->insert_first(this);
   while ( _client_sem.count() )
      _client_sem.wait();
   PEGASUS_ASSERT( _client_sem.count() == 0 );
   
   return;
}

PEGASUS_NAMESPACE_END
