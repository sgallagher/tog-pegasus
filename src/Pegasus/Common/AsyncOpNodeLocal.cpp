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

#include <Pegasus/Common/AsyncOpNodeLocal.h>
#include <Pegasus/Common/AsyncResponseHandler.h>
#include <Pegasus/Common/ResponseHandler.h>

PEGASUS_NAMESPACE_BEGIN
   



AsyncOpNodeLocal::AsyncOpNodeLocal(void) 
   : _mut(), _request(0), _response(0), _req_ctx(0), 
     _proc_ctx(0), _comp_ctx(0), _state(0), _flags(0),
     _total_ops(0), _completed_ops(0), _response_handler(0), 
     _provider(0), _parent(0), _children(true), 
     _err_code(0), _err_description()
      
{
   memset(&_start, 0x00, sizeof(struct timeval));
   memset(&_lifetime, 0x00, sizeof(struct timeval));
   memset(&_updated, 0x00, sizeof(struct timeval));
   memset(&_timeout_interval, 0xff, sizeof(struct timeval));
}

AsyncOpNodeLocal::~AsyncOpNodeLocal(void)
{
   reset();
   delete_rh(_response_handler, _rh_type);
}

void AsyncOpNodeLocal::reset(void) throw(IPCException)
{
   delete _request;
   delete _response;
   delete _req_ctx;
   delete _proc_ctx;
   delete _comp_ctx;
   _children.empty_list();
}


 void AsyncOpNodeLocal::notify(const void *key, 
			       OperationContext *context,
			       Uint32 flag, 
			       Uint32 state,
			       int type)
   throw(IPCException)
{

   // update my own state
   // if I have a parent, call notify pasing my own state
   // this allows a trickle up consolidation of complex
   // operation results

   if(flag & ASYNC_OPFLAGS_COMPLETE)
   {

      ;
   }
   if(flag & ASYNC_OPFLAGS_INDICATION)
   {
      ;
   }
   
   if(flag & ASYNC_OPFLAGS_DELIVER)
   {
      ;
   }
   
   return;
}




PEGASUS_NAMESPACE_END
