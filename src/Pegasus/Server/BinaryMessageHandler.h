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
//%/////////////////////////////////////////////////////////////////////////////
#ifndef Pegasus_BinaryMessageHandler_h
#define Pegasus_BinaryMessageHandler_h
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/AsyncOpNode.h>
#include <Pegasus/Common/DQueue.h>
#include <Pegasus/Common/MessageQueueService.h>
#include <Pegasus/Common/Message.h>
#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/TraceComponents.h>
#include <Pegasus/Common/IPC.h>
#include <Pegasus/Common/Thread.h>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Server/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

/** This class provides an in-process binary client interface, 
    both for requests and responses. 
*/

class PEGASUS_SERVER_LINKAGE BinaryMessageHandler : public MessageQueueService
{
   public: 
      typedef MessageQueueService Base;

      BinaryMessageHandler(MessageQueueService *outputQueue);
      ~BinaryMessageHandler();
      
      virtual Boolean messageOK(const Message * message);
      virtual void handleEnqueue(void);
      virtual void handleEnqueue(Message * message);
      virtual void _handle_async_request(AsyncRequest * request);
   protected:
      static PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL handle_binary_message(void *);

      // **** Request Messages **** //
      
      void handleCreateClassRequest(
	 AsyncOpNode *, 
	 CIMCreateClassRequestMessage *) throw();
      
      void handleGetClassRequest(
	 AsyncOpNode *,
	 CIMGetClassRequestMessage *) throw();
      
      void handleModifyClassRequest(
	 AsyncOpNode *, 
	 CIMModifyClassRequestMessage *) throw();
      
      void handleEnumerateClassNamesRequest(
	 AsyncOpNode *,
	 CIMEnumerateClassNamesRequestMessage *) throw();

      void handleEnumerateClassesRequest(
	 AsyncOpNode *,
	 CIMEnumerateClassesRequestMessage *) throw();

      void handleDeleteClassRequest(
	 AsyncOpNode *,
	 CIMDeleteClassRequestMessage *) throw();

      void handleCreateInstanceRequest(
	 AsyncOpNode *,
	 CIMCreateInstanceRequestMessage *) throw();

      void handleGetInstanceRequest(
	 AsyncOpNode *,
	 CIMGetInstanceRequestMessage *) throw();

      void handleModifyInstanceRequest(
	 AsyncOpNode *,
	 CIMModifyInstanceRequestMessage *) throw();

      void handleEnumerateInstanceNamesRequest(
	 AsyncOpNode *,
	 CIMEnumerateInstanceNamesRequestMessage *) throw();

      void handleEnumerateInstancesRequest(
	 AsyncOpNode *,
	 CIMEnumerateInstancesRequestMessage *) throw();

      void handleDeleteInstanceRequest(
	 AsyncOpNode *,
	 CIMDeleteInstanceRequestMessage *) throw();
      
      void handleSetQualifierRequest(
	 AsyncOpNode *,
	 CIMSetQualifierRequestMessage *) throw();
      
      void handleGetQualifierRequest(
	 AsyncOpNode *,
	 CIMGetQualifierRequestMessage *) throw();
      
      void handleEnumerateQualifiersRequest(
	 AsyncOpNode *,
	 CIMEnumerateQualifiersRequestMessage *) throw();
      
      void handleDeleteQualifiersRequest(
	 AsyncOpNode *,
	 CIMDeleteQualifierRequestMessage *) throw(); 
      
      void handleReferenceNamesRequest(
	 AsyncOpNode *,
	 CIMReferenceNamesRequestMessage *) throw(); 

      void handleReferencesRequest(
	 AsyncOpNode *,
	 CIMReferencesRequestMessage *) throw(); 

      void handleAssociatorNamesRequest(
	 AsyncOpNode *,
	 CIMAssociatorNamesRequestMessage *) throw(); 

      void handleAssociatorsRequest(
	 AsyncOpNode *,
	 CIMAssociatorsRequestMessage *) throw();  

      void handleGetPropertyRequest(
	 AsyncOpNode *,
	 CIMGetPropertyRequestMessage *) throw(); 

      void handleSetPropertyRequest(
	 AsyncOpNode *,
	 CIMSetPropertyRequestMessage *) throw();  

      void handleExecQueryRequest(
	 AsyncOpNode *,
	 CIMExecQueryRequestMessage *) throw();  

      void handleInvokeMethodRequest(
	 AsyncOpNode *,
	 CIMInvokeMethodRequestMessage *) throw(); 
      
      // **** Response Messages **** //
      
      void handleCreateClassResponse(
	 AsyncOpNode *,
	 CIMCreateClassResponseMessage *) throw(); 

      void handleGetClassResponse(
	 AsyncOpNode *,
	 CIMGetClassResponseMessage *) throw(); 

      void handleModifyClassResponse(
	 AsyncOpNode *,
	 CIMModifyClassResponseMessage *) throw(); 
      
      void handleEnumerateClassNamesResponse(
	 AsyncOpNode *,
	 CIMEnumerateClassNamesResponseMessage *) throw(); 

      void handleEnumerateClassesResponse(
	 AsyncOpNode *,
	 CIMEnumerateClassesResponseMessage *) throw(); 

      void handleDeleteClassResponse(
	 AsyncOpNode *,
	 CIMDeleteClassResponseMessage *) throw(); 

      void handleCreateInstanceResponse(
	 AsyncOpNode *,
	 CIMCreateInstanceResponseMessage *) throw(); 

      void handleGetInstanceResponse(
	 AsyncOpNode *,
	 CIMGetInstanceResponseMessage *) throw(); 

      void handleModifyInstanceResponse(
	 AsyncOpNode *,
	 CIMModifyInstanceResponseMessage *) throw(); 

      void handleEnumerateInstanceNamesResponse(
	 AsyncOpNode *,
	 CIMEnumerateInstanceNamesResponseMessage *) throw(); 

      void handleEnumerateInstancesResponse(
	 AsyncOpNode *,
	 CIMEnumerateInstancesResponseMessage *) throw(); 

      void handleDeleteInstanceResponse(
	 AsyncOpNode *,
	 CIMDeleteInstanceResponseMessage *) throw(); 

      void handleGetPropertyResponse(
	 AsyncOpNode *,
	 CIMGetPropertyResponseMessage *) throw(); 

      void handleSetPropertyResponse(
	 AsyncOpNode *,
	 CIMSetPropertyResponseMessage *) throw(); 
      
      void handleSetQualifierResponse(
	 AsyncOpNode *,
	 CIMSetQualifierResponseMessage *) throw(); 

      void handleGetQualifierResponse(
	 AsyncOpNode *,
	 CIMGetQualifierResponseMessage *) throw(); 
      
      void handleEnumerateQualifiersResponse(
	 AsyncOpNode *,
	 CIMEnumerateQualifiersResponseMessage *) throw(); 

      void handleDeleteQualifierResponse(
	 AsyncOpNode *,
	 CIMDeleteQualifierResponseMessage *) throw(); 

      void handleReferenceNamesResponse(
	 AsyncOpNode *,
	 CIMReferenceNamesResponseMessage *) throw(); 

      void handleReferencesResponse(
	 AsyncOpNode *,
	 CIMReferencesResponseMessage *) throw(); 

      void handleAssociatorNamesResponse(
	 AsyncOpNode *,
	 CIMAssociatorNamesResponseMessage *) throw();  

      void handleAssociatorsResponse(
	 AsyncOpNode *,
	 CIMAssociatorsResponseMessage *) throw();

      void handleExecQueryResponse(
	 AsyncOpNode *,
	 CIMExecQueryResponseMessage *) throw();
      
      void handleInvokeMethodResponse(
	 AsyncOpNode *,
	 CIMInvokeMethodResponseMessage *) throw();
      
   private:
      BinaryMessageHandler(void);
      BinaryMessageHandler & operator = (const BinaryMessageHandler & ) const;
      void _handleRequest(AsyncOpNode *, Message *) throw();
      void _handleResponse(AsyncOpNode *, Message *) throw();
      

      MessageQueueService* _outputQueue;

      // Flag to indicate whether or not the CIMServer is shutting down.
      AsyncDQueue<AsyncOpNode> _msg_q;
      
};


PEGASUS_NAMESPACE_END

#endif // Pegasus_BinaryMessageHandler_h
