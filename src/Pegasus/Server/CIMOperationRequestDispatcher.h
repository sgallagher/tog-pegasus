//%///////-*-c++-*-/////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 BMC Software, Hewlett-Packard Company, IBM,
// The Open Group, Tivoli Systems
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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By: Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
//            : Mike Day (mdday@us.ibm.com)
//            : Yi Zhou (yi_zhou@hp.com)
//            : Carol Ann Krug Graves, Hewlett-Packard Company
//              (carolann_graves@hp.com)
//
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef PegasusDispatcher_Dispatcher_h
#define PegasusDispatcher_Dispatcher_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/IPC.h>
#include <Pegasus/Common/DQueue.h>
#include <Pegasus/Common/Thread.h>
#include <Pegasus/Common/MessageQueue.h>
#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Common/CIMObject.h>
#include <Pegasus/Common/AsyncOpNode.h>
#include <Pegasus/Provider2/AsyncResponseHandler.h>
#include <Pegasus/Common/OperationContext.h>
#include <Pegasus/Server/ProviderManager.h>


PEGASUS_NAMESPACE_BEGIN

class CIMRepository;

class PEGASUS_SERVER_LINKAGE CIMOperationRequestDispatcher : public MessageQueue
{
public:
      typedef void (CIMOperationRequestDispatcher::*p_handler)(AsyncOpNode *);
      typedef struct op_async
      {
	    CIMOperationRequestDispatcher *dispatcher;
	    p_handler top_half;
	    p_handler bottom_half;
	    AsyncOpNode *work;
	    op_async(CIMOperationRequestDispatcher *d, 
			       p_handler top, 
			       p_handler bottom,
			       AsyncOpNode *op)
	    {
	       dispatcher = d;
	       top_half = top;
	       bottom_half = bottom;
	       work = op;
	    }
	    
	    inline Boolean operator==(const void *key) const 
	    { 
	       if ( this == key) 
		  return(true); 
	       return(false);
	    } 
	    inline Boolean operator==(const struct op_async & b) const
	    {
	       return(operator==((const void *)this));
	    }
// call (dispatcher->*top)(work_node);
//	(dispatcher->*bottom)(work_node);    

      }CIMOperation_async;
      
      static PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL async_dispatcher(void *parm);
      
      CIMOperationRequestDispatcher(CIMRepository* repository, CIMServer* server);
      
      virtual ~CIMOperationRequestDispatcher();
      
      virtual void handleEnqueue();
      
      virtual const char* getQueueName() const;
      
      void handleGetClassRequest(
	 CIMGetClassRequestMessage* request);
      
      void handleGetInstanceRequest(
	 CIMGetInstanceRequestMessage* request);
      
      void handleDeleteClassRequest(
	 CIMDeleteClassRequestMessage* request);
      
      void handleDeleteInstanceRequest(
	 CIMDeleteInstanceRequestMessage* request);
      
      void handleCreateClassRequest(
	 CIMCreateClassRequestMessage* request);
      
      void handleCreateInstanceRequest(
	 CIMCreateInstanceRequestMessage* request);
      
      void handleModifyClassRequest(
	 
	CIMModifyClassRequestMessage* request);

    void handleModifyInstanceRequest(
	CIMModifyInstanceRequestMessage* request);

    void handleEnumerateClassesRequest(
	CIMEnumerateClassesRequestMessage* request);

    void handleEnumerateClassNamesRequest(
	CIMEnumerateClassNamesRequestMessage* request);

    void handleEnumerateInstancesRequest(
	CIMEnumerateInstancesRequestMessage* request);

    void handleEnumerateInstanceNamesRequest(
	CIMEnumerateInstanceNamesRequestMessage* request);

    void handleAssociatorsRequest(
	CIMAssociatorsRequestMessage* request);

    void handleAssociatorNamesRequest(
	CIMAssociatorNamesRequestMessage* request);

    void handleReferencesRequest(
	CIMReferencesRequestMessage* request);

    void handleReferenceNamesRequest(
	CIMReferenceNamesRequestMessage* request);

    void handleGetPropertyRequest(
	CIMGetPropertyRequestMessage* request);

    void handleSetPropertyRequest(
	CIMSetPropertyRequestMessage* request);

    void handleGetQualifierRequest(
	CIMGetQualifierRequestMessage* request);

    void handleSetQualifierRequest(
	CIMSetQualifierRequestMessage* request);

    void handleDeleteQualifierRequest(
	CIMDeleteQualifierRequestMessage* request);

    void handleEnumerateQualifiersRequest(
	CIMEnumerateQualifiersRequestMessage* request);

    void handleInvokeMethodRequest(
	CIMInvokeMethodRequestMessage* request);

    void handleEnableIndicationSubscriptionRequest(
        CIMEnableIndicationSubscriptionRequestMessage* request);

    void handleModifyIndicationSubscriptionRequest(
        CIMModifyIndicationSubscriptionRequestMessage* request);

    void handleDisableIndicationSubscriptionRequest(
        CIMDisableIndicationSubscriptionRequestMessage* request);

    ProviderManager* getProviderManager(void);

    void loadRegisteredProviders(void);

protected:

    void _enqueueResponse(
	CIMRequestMessage* request,
	CIMResponseMessage* response);

    String _lookupProviderForClass(
	const String& nameSpace,
	const String& className);

      CIMRepository* _repository;
      ProviderManager _providerManager;
      
      Thread _dispatch_thread;
      ThreadPool _dispatch_pool; 
      
      static  PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL _qthread (void *);
           
      DQueue<AsyncOpNode> _opnode_cache;
      AsyncDQueue<AsyncOpNode> _waiting_ops;
      AsyncDQueue<AsyncOpNode> _started_ops;
      AsyncDQueue<AsyncOpNode> _completed_ops;
      
      AtomicInt _dying ;
      friend void async_operation_dispatch(CIMOperationRequestDispatcher *d, 
					   void (CIMOperationRequestDispatcher::*top)(AsyncOpNode *),
					   void (CIMOperationRequestDispatcher::*bottom)(AsyncOpNode *),
					   int rh_type,
					   Message *request) 
	 throw(IPCException) ;
};

PEGASUS_NAMESPACE_END

#endif /* PegasusDispatcher_Dispatcher_h */
