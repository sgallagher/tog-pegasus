//%////-*-c++-*-////////////////////////////////////////////////////////////////
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

#include "MessageQueueService.h"

PEGASUS_NAMESPACE_BEGIN

AtomicInt MessageQueueService::_xid(1);

// mutex is UNLOCKED
void MessageQueueService::handleEnqueue(void)
{

   
}


Boolean MessageQueueService::register_service(String name, Uint32 capabilities, Uint32 mask)
{
   
   CimomRegisterService *msg = 
      new CimomRegisterService(Message::getNextKey(), 
			       QueueIdStack(_queueId, _queueId),
			       0,
			       _name, 
			       _capabilities, 
			       _mask,
			       _queueId, 
			       get_next_xid());
   
   Boolean accepted = _meta_dispatcher->accept_async(static_cast<Message *>(msg));
   if (false == accepted )
      delete msg;
   
   return accepted;
}

Boolean MessageQueueService::update_service(Uint32 capabilities, Uint32 mask)
{
   _capabilities = capabilities;
   _mask = mask;
   
   CimomUpdateService *msg = 
      new CimomUpdateService( Message::getNextKey(), 
			      QueueIdStack(_queueId, _queueId), 
			      0, 
			      capabilities, 
			      mask, 
			      get_next_xid());
   
   Boolean accepted = _meta_dispatcher->accept_async(static_cast<Message *>(msg));
   if (false == accepted )
      delete msg;
   
   return accepted;
   
}

Boolean MessageQueueService::SendMessage(Message *msg, Uint32 dst_queue)
{
   
   ServiceAsyncOpStart *envelope = 
      new ServiceAsyncOpStart(Message::getNextKey(),
			      QueueIdStack(_queueId, _queueId),
			      0,
			      dst_queue, 
			      get_next_xid());
   
   Boolean accepted = _meta_dispatcher->accept_async(static_cast<Message *>(envelope));
   if (accepted == false)
      delete msg;
   return accepted;
}





PEGASUS_NAMESPACE_END
