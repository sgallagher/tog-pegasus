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

#ifndef Pegasus_MessageQueue_Service_h
#define Pegasus_MessageQueue_Service_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Message.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/IPC.h>
#include <Pegasus/Common/Thread.h>
#include <Pegasus/Common/AsyncOpNode.h>
#include <Pegasus/_cimom/Cimom.h>
#include <Pegasus/_cimom/CimomMessage.h>

PEGASUS_NAMESPACE_BEGIN


class PEGASUS_COMMON_LINKAGE MessageQueueService : public MessageQueue
{
   public:

      typedef MessageQueue Base;
      
      MessageQueueService(const char *name, Uint32 queueID, Uint32 capabilities, Uint32 mask) 
	 : Base(name, true, queueID),
	   _capabilities(capabilities),
	   _mask(mask),
	   _die(0)
      {
	 _default_op_timeout.tv_sec = 30;
	 _default_op_timeout.tv_usec = 100;
	 _meta_dispatcher = static_cast<cimom *>(Base::lookup(CIMOM_Q_ID));
	 if(_meta_dispatcher == 0 )
	    throw NullPointer();
      }

      virtual ~MessageQueueService(void);
      
      virtual void handleEnqueue();
//      virtual Boolean accept_async(Message *message) throw(IPCException);
      virtual Message *openEnvelope(Message *msg);
      
      void SendWait(AsyncRequest *request, unlocked_dq<AsyncMessage>& reply_list);
      Boolean SendAsync(AsyncMessage *msg);
      void _enqueueAsyncResponse(AsyncRequest *request, 
				 AsyncReply *reply, 
				 Uint32 state, 
				 Uint32 flag);
            
   protected:
      Boolean register_service(String name, Uint32 capabilities, Uint32 mask);
      Boolean update_service(Uint32 capabilities, Uint32 mask);
      Boolean deregister_service(void);
      Uint32 get_next_xid(void);
      
      Uint32 _capabilities;
      Uint32 _mask;
      
   private: 
      AsyncOpNode *_get_op(void);
      void _return_op(AsyncOpNode *op);
      cimom *_meta_dispatcher;
      struct timeval _default_op_timeout;
      AtomicInt _die;
      static AtomicInt _xid;
};

inline Uint32 MessageQueueService::get_next_xid(void)
{
   _xid++;
   return _xid.value();
}



PEGASUS_NAMESPACE_END

#endif /* Pegasus_MessageQueue_Service_h */


