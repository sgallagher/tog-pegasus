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
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////


#ifndef SERVICE_CONTROL_include
#define SERVICE_CONTROL_include

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/InternalException.h>
#include <Pegasus/Common/MessageQueue.h>
#include <Pegasus/Common/DQueue.h>
#include <Pegasus/Common/Thread.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/AsyncOpNode.h>
#include <Pegasus/Common/CimomMessage.h>
#include <Pegasus/Common/MessageQueueService.h>

PEGASUS_NAMESPACE_BEGIN



// allows us to treat the control service as a regular provider, 
// but also gives the service a handle into the ControlServiceManager. 
// this handle provides the control service with access to important
// communication methods with "the world"
class ControlServiceFacade : public ProviderFacade
{
   public:

      typedef ProviderFacade Base;
      
      ControlServiceFacade(ControlServiceManager *service_handle, 
			   ControlService *control);

      // direct call "ioctl" interface, counterpart of AsyncIoctl message
      // this is a general-purpose function call mechanism
      Uint32 control_ioctl(Uint32 code, Uint32 int_param, void *ptr_param);
      
      virtual ~ControlServiceFacade(void);
      
   private:

      ControlServiceFacade(void);
} ;



// needs a reference count 
class PEGASUS_COMMON_LINKAGE ControlService



class PEGASUS_COMMON_LINKAGE ControlServiceManager : public MessageQueueService
{
   public:

      typedef MessageQueueService Base;
      
      ControlServiceManager(void)
	 : Base("pegasus control service mgr", 
		MessageQueue::getNextQueueId(), 0, 
		message_mask::type_cimom | 
		message_mask::type_service | 
		message_mask::ha_request | 
		message_mask::ha_reply | 
		message_mask::ha_async )
      {

      }
      
      ~ControlServiceManager(void)
      {

      }
      
      void register_control_service(ControlService *service);


      // caller gets a handle that provides direct calls into a control
      // service. calls that are not supported throw a not 
      // supported exception
      ControlServiceFacade *reference_control_service(String name);

      // decrement the reference count so that we know if it 
      // is OK to delete the service 
      void dereference_control_service(String name);
      
      // send an Async message to a control service, get a response
      AsyncResponse *send_to_control_service(AsyncRequest *request, 
					     String service_name);
      
      // for use by control providers  that want to send async messages 
      // to the system
      AsyncResponse *control_send_wait(AsyncRequest *request);
      
      // allow the control provider to run a function on a pooled thread
      Uint32 run_on_thread(PEGASUS_THREAD_RETURN (PEGASUS_THREAD_CDECL *work)(void *));
      
      


      // virtual methods, not used by the control service directly. 
      virtual Boolean accept_async(AsyncOpNode *op);
      virtual Boolean messageOK(const Message *msg) ;

      virtual void _handle_incoming_operation(AsyncOpNode *operation, 
					      Thread *thread, 
					      MessageQueue *queue);
      virtual void _handle_async_request(AsyncRequest *req);

   private:

      DQueue<ControlServiceFacade> _control_services;
      ThreadPool _service_thread_pool;
            
};



// need to define a message for calling into utility providers
// depends upon provider registration so that operations can get 
// routed to the correct control service
// 

PEGASUS_NAMESPACE_END
#endif // SERVICE_CONTROL_include
