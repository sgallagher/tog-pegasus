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
// Modified By: Carol Ann Krug Graves, Hewlett-Packard Company
//                  (carolann_graves@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include "BinaryMessageHandler.h"
#include <Pegasus/Common/IPC.h>
#include <Pegasus/Common/Thread.h>

PEGASUS_NAMESPACE_BEGIN

BinaryMessageHandler::BinaryMessageHandler(MessageQueueService *output_q)
   : Base(PEGASUS_QUEUENAME_BINARY_HANDLER),
     _outputQueue(output_q),
     _msg_q(true, 0)
{

}

BinaryMessageHandler::~BinaryMessageHandler(void)
{
   
}


Boolean BinaryMessageHandler::messageOK(const Message * msg)
{
   return Base::messageOK(msg);
}

void BinaryMessageHandler::handleEnqueue(void)
{
   Message *msg = dequeue();
   handleEnqueue(msg);
}

void BinaryMessageHandler::handleEnqueue(Message * message)
{
   PEG_METHOD_ENTER(TRC_BINARY_MSG_HANDLER,
		    "BinaryMessageHandler::_handleEnqueue(Message *)");
   PEGASUS_ASSERT(message != 0);

   AsyncRequest * asyncRequest;
   AsyncOpNode * op;
   
   message->_async = 0;
   
   PEG_TRACE_STRING(TRC_BINARY_MSG_HANDLER, Tracer::LEVEL4, 
		    "Converting legacy message to AsyncLegacyOperationStart");
   op = this->get_op();
   asyncRequest = new AsyncLegacyOperationStart(
      get_next_xid(),
      op,
      this->getQueueId(),
      message,
      this->getQueueId());
   op->_flags |= ASYNC_OPFLAGS_FIRE_AND_FORGET;
   

   _handle_async_request(asyncRequest);
   PEG_METHOD_EXIT();
}

void BinaryMessageHandler::_handle_async_request(AsyncRequest * request)
{
   PEG_METHOD_ENTER(TRC_BINARY_MSG_HANDLER,
		    "BinaryMessageHandler::_handle_async_request");

   PEGASUS_ASSERT(request != 0 && request->op != 0 );

   if(request->getType() == async_messages::ASYNC_LEGACY_OP_START || 
      request->getType() == async_messages::ASYNC_LEGACY_OP_RESULT)
   {        
      PEG_TRACE_STRING(TRC_BINARY_MSG_HANDLER, Tracer::LEVEL4,
		       "Processing ASYNC_LEGACY_OP_* Message.");
      request->op->processing();
      try
      {
	 _msg_q.insert_last(request->op);
      }
      catch(ListFull & )
      {
	 PEG_TRACE_STRING(TRC_BINARY_MSG_HANDLER, Tracer::LEVEL4,
			  "Internal DQueue Full.");
	 Base::_handle_async_request(request);
	 PEG_METHOD_EXIT();
	 return;
      }
       
      catch(...)
      {
	 PEG_TRACE_STRING(TRC_BINARY_MSG_HANDLER, Tracer::LEVEL4,
			  "Internal DQueue Error.");
	 Base::_handle_async_request(request);
	 PEG_METHOD_EXIT();
	 return;
      }
       
      PEG_TRACE_STRING(TRC_BINARY_MSG_HANDLER, Tracer::LEVEL4,
		       "Allocating pooled thread to handle binary message.");
      if (_thread_pool->allocate_and_awaken(
	 (void *)this, BinaryMessageHandler::handle_binary_message)!=PEGASUS_THREAD_OK)
	{
 	    Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            	"Not enough threads to handle binary message.");
 
	    Tracer::trace(TRC_BINARY_MSG_HANDLER, Tracer::LEVEL2,
		"Could not allocate thread for %s. " \
 		"Queue has %d messages waiting. ",
                getQueueName(),
 		_msg_q.count());
	}	
   }
   else if(request->getType() == async_messages::CIMSERVICE_STOP)
   {
      PEG_TRACE_STRING(TRC_BINARY_MSG_HANDLER, Tracer::LEVEL4,
		       "Handling CIMServer Stop Message");
      Base::_handle_async_request(request);
   }
   else
   {
      // pass all other operations to the default handler
      PEG_TRACE_STRING(TRC_BINARY_MSG_HANDLER, Tracer::LEVEL4,
		       "Passing message to parent.");
#ifdef BINARYMESSAGEHANDLER_DEBUG
      PEGASUS_STD(cout) << "Unexpected Message: type " << request->getType() << PEGASUS_STD(endl);
#endif

      Base::_handle_async_request(request);
   }

   PEG_METHOD_EXIT();

   return;
}

PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL
BinaryMessageHandler::handle_binary_message(void* parm)
{
   PEG_METHOD_ENTER(TRC_BINARY_MSG_HANDLER,
      "BinaryMessageHandler::_handle_async_request");

   BinaryMessageHandler* myself =
      reinterpret_cast<BinaryMessageHandler*>(parm);
   PEGASUS_ASSERT(myself != 0);

   AsyncOpNode *op;
   try
   {
      op = myself->_msg_q.remove_first();
   }
   catch(...)
   {
      PEG_TRACE_STRING(TRC_BINARY_MSG_HANDLER, Tracer::LEVEL2,
         "Internal DQueue Error.");
      PEG_METHOD_EXIT();
      return(0);
   }
   Uint32 msg_type;

   PEGASUS_ASSERT(op != 0);

   // we only receive ASYNC_LEGACY_OP_START and
   // ASYNC_LEGACY_OP_RESULT messages

   Message *legacy = 0;
   AsyncMessage *msg = 0;

   try
   {
      // if there is a response, dispatch the response
      if(op->_response.count())
      {
         msg = static_cast<AsyncMessage *>(op->_response.next(0));
         legacy = static_cast<AsyncLegacyOperationResult*>(msg)->get_result();
      }
      else
      {
         // there is no response so there has to be a request
         if(op->_request.count() == 0)
         {
            PEG_TRACE_STRING(TRC_BINARY_MSG_HANDLER, Tracer::LEVEL2,
               "Received OpNode with no messages.");
            PEG_METHOD_EXIT();
            return(0);
         }
         // dispatch the request
         msg = static_cast<AsyncMessage *>(op->_request.next(0));
         legacy = static_cast<AsyncLegacyOperationStart *>(msg)->get_action();
      }
      if(msg && legacy)
      {
         legacy->_async = 0;

         switch(legacy->getType())
         {
            case CIM_CREATE_CLASS_REQUEST_MESSAGE:
               myself->handleCreateClassRequest(
                  op, (CIMCreateClassRequestMessage *)legacy);
               break;
            case CIM_CREATE_INSTANCE_REQUEST_MESSAGE:
               myself->handleCreateInstanceRequest(
                  op, (CIMCreateInstanceRequestMessage *)legacy);
               break;
            case CIM_MODIFY_CLASS_REQUEST_MESSAGE:
               myself->handleModifyClassRequest(
                  op, (CIMModifyClassRequestMessage *)legacy);
               break;
            case CIM_MODIFY_INSTANCE_REQUEST_MESSAGE:
               myself->handleModifyInstanceRequest(
                  op, (CIMModifyInstanceRequestMessage *)legacy);
               break;
            case CIM_ENUMERATE_CLASSES_REQUEST_MESSAGE: //10
               myself->handleEnumerateClassesRequest(
                  op, (CIMEnumerateClassesRequestMessage *)legacy);
               break;
            case CIM_ENUMERATE_CLASS_NAMES_REQUEST_MESSAGE:
               myself->handleEnumerateClassNamesRequest(
                  op, (CIMEnumerateClassNamesRequestMessage *)legacy);
               break;
            case CIM_ENUMERATE_INSTANCES_REQUEST_MESSAGE:
               myself->handleEnumerateInstancesRequest(
                  op, (CIMEnumerateInstancesRequestMessage *)legacy);
               break;
            case CIM_ENUMERATE_INSTANCE_NAMES_REQUEST_MESSAGE:
               myself->handleEnumerateInstanceNamesRequest(
                  op, (CIMEnumerateInstanceNamesRequestMessage *)legacy);
               break;
            case CIM_EXEC_QUERY_REQUEST_MESSAGE:
               myself->handleExecQueryRequest(
                  op, (CIMExecQueryRequestMessage *)legacy);
               break;
            case CIM_ASSOCIATORS_REQUEST_MESSAGE:
               myself->handleAssociatorsRequest(
                  op, (CIMAssociatorsRequestMessage *)legacy);
               break;
            case CIM_ASSOCIATOR_NAMES_REQUEST_MESSAGE:
               myself->handleAssociatorNamesRequest(
                  op, (CIMAssociatorNamesRequestMessage *)legacy);
               break;
            case CIM_REFERENCES_REQUEST_MESSAGE:
               myself->handleReferencesRequest(
                  op, (CIMReferencesRequestMessage *)legacy);
               break;
            case CIM_REFERENCE_NAMES_REQUEST_MESSAGE:
               myself->handleReferenceNamesRequest(
                  op, (CIMReferenceNamesRequestMessage *)legacy);
               break;
            case CIM_GET_PROPERTY_REQUEST_MESSAGE:
               myself->handleGetPropertyRequest(
                  op, (CIMGetPropertyRequestMessage *)legacy);
               break;
            case CIM_SET_PROPERTY_REQUEST_MESSAGE: //20
               myself->handleSetPropertyRequest(
                  op, (CIMSetPropertyRequestMessage *)legacy);
               break;
            case CIM_GET_QUALIFIER_REQUEST_MESSAGE:
               myself->handleGetQualifierRequest(
                  op, (CIMGetQualifierRequestMessage *)legacy);
               break;
            case CIM_SET_QUALIFIER_REQUEST_MESSAGE:
               myself->handleSetQualifierRequest(
                  op, (CIMSetQualifierRequestMessage *)legacy);
               break;
            case CIM_DELETE_QUALIFIER_REQUEST_MESSAGE:
               myself->handleDeleteQualifiersRequest(
                  op, (CIMDeleteQualifierRequestMessage *)legacy);
               break;
            case CIM_ENUMERATE_QUALIFIERS_REQUEST_MESSAGE:
               myself->handleEnumerateQualifiersRequest(
                  op, (CIMEnumerateQualifiersRequestMessage *)legacy);
               break;
            case CIM_INVOKE_METHOD_REQUEST_MESSAGE:
               myself->handleInvokeMethodRequest(
                  op, (CIMInvokeMethodRequestMessage *)legacy);
               break;
            //**** response messages ****//
            case CIM_GET_CLASS_RESPONSE_MESSAGE:
               myself->handleGetClassResponse(
                  op, (CIMGetClassResponseMessage *)legacy);
               break;
            case CIM_GET_INSTANCE_RESPONSE_MESSAGE:
               myself->handleGetInstanceResponse(
                  op, (CIMGetInstanceResponseMessage *)legacy);
               break;
            case CIM_DELETE_CLASS_RESPONSE_MESSAGE:
               myself->handleDeleteClassResponse(
                  op, (CIMDeleteClassResponseMessage *)legacy);
               break;
            case CIM_DELETE_INSTANCE_RESPONSE_MESSAGE:
               myself->handleDeleteInstanceResponse(
                  op, (CIMDeleteInstanceResponseMessage *)legacy);
               break;
            case CIM_CREATE_CLASS_RESPONSE_MESSAGE:
               myself->handleCreateClassResponse(
                  op, (CIMCreateClassResponseMessage *)legacy);
               break;
            case CIM_CREATE_INSTANCE_RESPONSE_MESSAGE:
               myself->handleCreateInstanceResponse(
                  op, (CIMCreateInstanceResponseMessage *)legacy);
               break;
            case CIM_MODIFY_CLASS_RESPONSE_MESSAGE:
               myself->handleModifyClassResponse(
                  op, (CIMModifyClassResponseMessage *)legacy);
               break;
            case CIM_MODIFY_INSTANCE_RESPONSE_MESSAGE:
               myself->handleModifyInstanceResponse(
                  op, (CIMModifyInstanceResponseMessage *)legacy);
               break;
            case CIM_ENUMERATE_CLASSES_RESPONSE_MESSAGE:
               myself->handleEnumerateClassesResponse(
                  op, (CIMEnumerateClassesResponseMessage *)legacy);
               break;
            case CIM_ENUMERATE_CLASS_NAMES_RESPONSE_MESSAGE:
               myself->handleEnumerateClassNamesResponse(
                  op, (CIMEnumerateClassNamesResponseMessage *)legacy);
               break;
            case CIM_ENUMERATE_INSTANCES_RESPONSE_MESSAGE:
               myself->handleEnumerateInstancesResponse(
                  op, (CIMEnumerateInstancesResponseMessage *)legacy);
               break;
            case CIM_ENUMERATE_INSTANCE_NAMES_RESPONSE_MESSAGE:
               myself->handleEnumerateInstanceNamesResponse(
                  op, (CIMEnumerateInstanceNamesResponseMessage *)legacy);
               break;
            case CIM_EXEC_QUERY_RESPONSE_MESSAGE:
               myself->handleExecQueryResponse(
                  op, (CIMExecQueryResponseMessage *)legacy);
               break;
            case CIM_ASSOCIATORS_RESPONSE_MESSAGE:
               myself->handleAssociatorsResponse(
                  op, (CIMAssociatorsResponseMessage *)legacy);
               break;
            case CIM_ASSOCIATOR_NAMES_RESPONSE_MESSAGE:
               myself->handleAssociatorNamesResponse(
                  op, (CIMAssociatorNamesResponseMessage *)legacy);
               break;
            case CIM_REFERENCES_RESPONSE_MESSAGE:
               myself->handleReferencesResponse(
                  op, (CIMReferencesResponseMessage *)legacy);
               break;
            case CIM_REFERENCE_NAMES_RESPONSE_MESSAGE:
               myself->handleReferenceNamesResponse(
                  op, (CIMReferenceNamesResponseMessage *)legacy);
               break;
            case CIM_GET_PROPERTY_RESPONSE_MESSAGE:
               myself->handleGetPropertyResponse(
                  op, (CIMGetPropertyResponseMessage *)legacy);
               break;
            case CIM_SET_PROPERTY_RESPONSE_MESSAGE:
               myself->handleSetPropertyResponse(
                  op, (CIMSetPropertyResponseMessage *)legacy);
               break;
            case CIM_GET_QUALIFIER_RESPONSE_MESSAGE:
               myself->handleGetQualifierResponse(
                  op, (CIMGetQualifierResponseMessage *)legacy);
               break;
            case CIM_SET_QUALIFIER_RESPONSE_MESSAGE:
               myself->handleSetQualifierResponse(
                  op, (CIMSetQualifierResponseMessage *)legacy);
               break;
            case CIM_DELETE_QUALIFIER_RESPONSE_MESSAGE:
               myself->handleDeleteQualifierResponse(
                  op, (CIMDeleteQualifierResponseMessage *)legacy);
               break;
            case CIM_ENUMERATE_QUALIFIERS_RESPONSE_MESSAGE:
               myself->handleEnumerateQualifiersResponse(
                  op, (CIMEnumerateQualifiersResponseMessage *)legacy);
               break;
            case CIM_INVOKE_METHOD_RESPONSE_MESSAGE:
               myself->handleInvokeMethodResponse(
                  op, (CIMInvokeMethodResponseMessage *)legacy);
               break;

            // unexpected requests
            case CIM_ENABLE_INDICATION_SUBSCRIPTION_REQUEST_MESSAGE:
            case CIM_MODIFY_INDICATION_SUBSCRIPTION_REQUEST_MESSAGE:
            case CIM_DISABLE_INDICATION_SUBSCRIPTION_REQUEST_MESSAGE:
            case CIM_PROCESS_INDICATION_REQUEST_MESSAGE:
            case CIM_HANDLE_INDICATION_REQUEST_MESSAGE: // 30
            case CIM_NOTIFY_PROVIDER_REGISTRATION_REQUEST_MESSAGE:
            case CIM_NOTIFY_PROVIDER_TERMINATION_REQUEST_MESSAGE:
            case CIM_NOTIFY_PROVIDER_ENABLE_REQUEST_MESSAGE:

            // new
            case CIM_CREATE_SUBSCRIPTION_REQUEST_MESSAGE:
            case CIM_MODIFY_SUBSCRIPTION_REQUEST_MESSAGE:
            case CIM_DELETE_SUBSCRIPTION_REQUEST_MESSAGE:
            case CIM_SUBSCRIPTION_INIT_COMPLETE_REQUEST_MESSAGE:

            // new
            case CIM_DISABLE_MODULE_REQUEST_MESSAGE:
            case CIM_ENABLE_MODULE_REQUEST_MESSAGE:

            case CIM_INITIALIZE_PROVIDER_REQUEST_MESSAGE:

            case CIM_NOTIFY_CONFIG_CHANGE_REQUEST_MESSAGE:

            case CIM_STOP_ALL_PROVIDERS_REQUEST_MESSAGE:
            case CIM_GET_CLASS_REQUEST_MESSAGE:
            case CIM_GET_INSTANCE_REQUEST_MESSAGE:
            case CIM_EXPORT_INDICATION_REQUEST_MESSAGE:
            case CIM_DELETE_CLASS_REQUEST_MESSAGE:
            case CIM_DELETE_INSTANCE_REQUEST_MESSAGE:
               PEG_TRACE_STRING(TRC_BINARY_MSG_HANDLER, Tracer::LEVEL4,
                  "Received Unexpected legacy request message.");
               myself->_handleRequest(op, legacy);
               break;

            // unexpected replies
            case CIM_ENABLE_INDICATION_SUBSCRIPTION_RESPONSE_MESSAGE:
            case CIM_MODIFY_INDICATION_SUBSCRIPTION_RESPONSE_MESSAGE:
            case CIM_DISABLE_INDICATION_SUBSCRIPTION_RESPONSE_MESSAGE:
            case CIM_PROCESS_INDICATION_RESPONSE_MESSAGE:
            case CIM_NOTIFY_PROVIDER_REGISTRATION_RESPONSE_MESSAGE:
            case CIM_NOTIFY_PROVIDER_TERMINATION_RESPONSE_MESSAGE:
            case CIM_HANDLE_INDICATION_RESPONSE_MESSAGE:
            // new
            case CIM_CREATE_SUBSCRIPTION_RESPONSE_MESSAGE:
            case CIM_MODIFY_SUBSCRIPTION_RESPONSE_MESSAGE:
            case CIM_DELETE_SUBSCRIPTION_RESPONSE_MESSAGE:
            case CIM_SUBSCRIPTION_INIT_COMPLETE_RESPONSE_MESSAGE:
            // new
            case CIM_DISABLE_MODULE_RESPONSE_MESSAGE:
            case CIM_ENABLE_MODULE_RESPONSE_MESSAGE:
            case CIM_STOP_ALL_PROVIDERS_RESPONSE_MESSAGE:
            case CIM_NOTIFY_PROVIDER_ENABLE_RESPONSE_MESSAGE:
            case CIM_INITIALIZE_PROVIDER_RESPONSE_MESSAGE:
            case CIM_NOTIFY_CONFIG_CHANGE_RESPONSE_MESSAGE:
               break;
            case CIM_EXPORT_INDICATION_RESPONSE_MESSAGE: // 42
            default:
               PEG_TRACE_STRING(TRC_BINARY_MSG_HANDLER, Tracer::LEVEL4,
                  "Received Unexpected legacy response message.");
               myself->_handleResponse(op, legacy);
               break;
         }
      }
      else
      {
         PEG_TRACE_STRING(TRC_BINARY_MSG_HANDLER, Tracer::LEVEL4,
            "Damaged or uninitialized AsyncOpNode received.");
      }
   }
   catch (const Exception& e)
   {
      PEG_TRACE_STRING(TRC_DISCARDED_DATA, Tracer::LEVEL2,
         "Caught exception: \"" + e.getMessage() +
            "\".  Exiting handle_binary_message.");
   }
   catch (...)
   {
      PEG_TRACE_STRING(TRC_DISCARDED_DATA, Tracer::LEVEL2,
         "Caught unrecognized exception.  Exiting handle_binary_message.");
   }

   PEG_METHOD_EXIT();
   return(0);
}

// requests always go to the output queue 
void 
BinaryMessageHandler::_handleRequest(AsyncOpNode *op, Message *msg) 
   throw()
{
   PEG_METHOD_ENTER(TRC_BINARY_MSG_HANDLER,
      "BinaryMessageHandler::_handleRequest(AsyncOpNode *, Message *)");

   AsyncRequest *async_request = static_cast<AsyncRequest *>(op->get_request());
   
   msg->_async = 0;
   try 
   {
      _outputQueue->enqueue(msg) ;
   }
   catch(...)
   {

   }
   
   delete async_request;
   
   _complete_op_node(op, ASYNC_OPSTATE_COMPLETE, 0, 0);
   PEG_METHOD_EXIT();
}

// responses have their destination queue id in the message
void 
BinaryMessageHandler::_handleResponse(AsyncOpNode *op, Message *msg)
   throw()
{
   PEG_METHOD_ENTER(TRC_BINARY_MSG_HANDLER,
		    "BinaryMessageHandler::_handleResponse(AsyncOpNode *, Message *)");
   AsyncReply *async_reply = static_cast<AsyncReply *>(op->get_response());
   msg->_async = 0;
   delete async_reply;
   
   MessageQueue *dest = MessageQueue::lookup(((CIMRequestMessage *)msg)->queueIds.top());
   if(dest == 0)
   {
      PEG_TRACE_STRING(TRC_BINARY_MSG_HANDLER, Tracer::LEVEL4,
		       "Bad or non-existent Queue ID for desination in legacy message.");
      delete msg;
   }
   else
   {
      try
      {
	 
	 dest->enqueue(msg);
      }
      catch(...)
      {
      }
      
   }
   _complete_op_node(op, ASYNC_OPSTATE_COMPLETE, 0, 0);
   PEG_METHOD_EXIT();
}

void BinaryMessageHandler::handleCreateClassRequest(
   AsyncOpNode *op, 
   CIMCreateClassRequestMessage *msg) throw()
{
   PEG_METHOD_ENTER(TRC_BINARY_MSG_HANDLER,
		    "BinaryMessageHandler::handleCreateClassRequest()");
   _handleRequest(op, msg);
   PEG_METHOD_EXIT();
}

      
void BinaryMessageHandler::handleGetClassRequest(
   AsyncOpNode *op,
   CIMGetClassRequestMessage *msg) throw()
{
   PEG_METHOD_ENTER(TRC_BINARY_MSG_HANDLER,
		    "BinaryMessageHandler::handleGetClassRequest()");
   _handleRequest(op, msg);
   PEG_METHOD_EXIT();
}

      
void BinaryMessageHandler::handleModifyClassRequest(
   AsyncOpNode *op, 
   CIMModifyClassRequestMessage *msg) throw()
{
   PEG_METHOD_ENTER(TRC_BINARY_MSG_HANDLER,
		    "BinaryMessageHandler::handleModifyClassRequest()");
   _handleRequest(op, msg);
   PEG_METHOD_EXIT();
}

void BinaryMessageHandler::handleEnumerateClassNamesRequest(
   AsyncOpNode *op,
   CIMEnumerateClassNamesRequestMessage *msg) throw()
{
   PEG_METHOD_ENTER(TRC_BINARY_MSG_HANDLER,
		    "BinaryMessageHandler::handleEnumerateClassNamesRequest()");
   _handleRequest(op, msg);
   PEG_METHOD_EXIT();
}

void BinaryMessageHandler::handleEnumerateClassesRequest(
   AsyncOpNode *op,
   CIMEnumerateClassesRequestMessage *msg) throw()
{
   PEG_METHOD_ENTER(TRC_BINARY_MSG_HANDLER,
		    "BinaryMessageHandler::handleEnumerateClassRequest()");
   _handleRequest(op, msg);
   PEG_METHOD_EXIT();
}


void BinaryMessageHandler::handleDeleteClassRequest(
   AsyncOpNode *op,
   CIMDeleteClassRequestMessage *msg) throw()
{
   PEG_METHOD_ENTER(TRC_BINARY_MSG_HANDLER,
		    "BinaryMessageHandler::handleDeleteClassRequest()");
   _handleRequest(op, msg);
   PEG_METHOD_EXIT();
}

void BinaryMessageHandler::handleCreateInstanceRequest(
   AsyncOpNode *op,
   CIMCreateInstanceRequestMessage *msg) throw()
{
   PEG_METHOD_ENTER(TRC_BINARY_MSG_HANDLER,
		    "BinaryMessageHandler::handleCreateInstanceRequest()");
   _handleRequest(op, msg);
   PEG_METHOD_EXIT();
}

void BinaryMessageHandler::handleGetInstanceRequest(
   AsyncOpNode *op,
   CIMGetInstanceRequestMessage *msg) throw()
{
   PEG_METHOD_ENTER(TRC_BINARY_MSG_HANDLER,
		    "BinaryMessageHandler::handleGetInstanceRequest()");
   _handleRequest(op, msg);
   PEG_METHOD_EXIT();
}

void BinaryMessageHandler::handleModifyInstanceRequest(
   AsyncOpNode *op,
   CIMModifyInstanceRequestMessage *msg) throw()
{
   PEG_METHOD_ENTER(TRC_BINARY_MSG_HANDLER,
		    "BinaryMessageHandler::handleModifyInstanceRequest()");
   _handleRequest(op, msg);
   PEG_METHOD_EXIT();
}

void BinaryMessageHandler::handleEnumerateInstanceNamesRequest(
   AsyncOpNode *op,
   CIMEnumerateInstanceNamesRequestMessage *msg) throw()
{
   PEG_METHOD_ENTER(TRC_BINARY_MSG_HANDLER,
		    "BinaryMessageHandler::handleEnumerateInstanceNamesRequest()");
   _handleRequest(op, msg);
   PEG_METHOD_EXIT();
}

void BinaryMessageHandler::handleEnumerateInstancesRequest(
   AsyncOpNode *op,
   CIMEnumerateInstancesRequestMessage *msg) throw()
{
   PEG_METHOD_ENTER(TRC_BINARY_MSG_HANDLER,
		    "BinaryMessageHandler::handleEnumerateInstancesRequest()");
   _handleRequest(op, msg);
   PEG_METHOD_EXIT();
}

void BinaryMessageHandler::handleDeleteInstanceRequest(
   AsyncOpNode *op,
   CIMDeleteInstanceRequestMessage *msg) throw()
{
   PEG_METHOD_ENTER(TRC_BINARY_MSG_HANDLER,
		    "BinaryMessageHandler::handleDeleteInstanceRequest()");
   _handleRequest(op, msg);
   PEG_METHOD_EXIT();
}
      
void BinaryMessageHandler::handleSetQualifierRequest(
   AsyncOpNode *op,
   CIMSetQualifierRequestMessage *msg) throw()
{
   PEG_METHOD_ENTER(TRC_BINARY_MSG_HANDLER,
		    "BinaryMessageHandler::handleSetQualifierRequest()");
   _handleRequest(op, msg);
   PEG_METHOD_EXIT();
}
      
void BinaryMessageHandler::handleGetQualifierRequest(
   AsyncOpNode *op,
   CIMGetQualifierRequestMessage *msg) throw()
{
   PEG_METHOD_ENTER(TRC_BINARY_MSG_HANDLER,
		    "BinaryMessageHandler::handleGetQualifierRequest()");
   _handleRequest(op, msg);
   PEG_METHOD_EXIT();
}
      
void BinaryMessageHandler::handleEnumerateQualifiersRequest(
   AsyncOpNode *op,
   CIMEnumerateQualifiersRequestMessage *msg) throw()
{
   PEG_METHOD_ENTER(TRC_BINARY_MSG_HANDLER,
		    "BinaryMessageHandler::handleEnumerateQualifiersRequest()");
   _handleRequest(op, msg);
   PEG_METHOD_EXIT();
}
      
void BinaryMessageHandler::handleDeleteQualifiersRequest(
   AsyncOpNode *op,
   CIMDeleteQualifierRequestMessage *msg) throw()
{
   PEG_METHOD_ENTER(TRC_BINARY_MSG_HANDLER,
		    "BinaryMessageHandler::handleDeleteQualifiersRequest()");
   _handleRequest(op, msg);
   PEG_METHOD_EXIT();
}
      
void BinaryMessageHandler::handleReferenceNamesRequest(
   AsyncOpNode *op,
   CIMReferenceNamesRequestMessage *msg) throw()
{
   PEG_METHOD_ENTER(TRC_BINARY_MSG_HANDLER,
		    "BinaryMessageHandler::handleReferenceNamesRequest()");
   _handleRequest(op, msg);
   PEG_METHOD_EXIT();
}

void BinaryMessageHandler::handleReferencesRequest(
   AsyncOpNode *op,
   CIMReferencesRequestMessage *msg) throw()
{
   PEG_METHOD_ENTER(TRC_BINARY_MSG_HANDLER,
		    "BinaryMessageHandler::handleReferencesRequest()");
   _handleRequest(op, msg);
   PEG_METHOD_EXIT();
}

void BinaryMessageHandler::handleAssociatorNamesRequest(
   AsyncOpNode *op,
   CIMAssociatorNamesRequestMessage *msg) throw()
{
   PEG_METHOD_ENTER(TRC_BINARY_MSG_HANDLER,
		    "BinaryMessageHandler::handleAssociatorNamesRequest()");
   _handleRequest(op, msg);
   PEG_METHOD_EXIT();
}

void BinaryMessageHandler::handleAssociatorsRequest(
   AsyncOpNode *op,
   CIMAssociatorsRequestMessage *msg) throw()
{
   PEG_METHOD_ENTER(TRC_BINARY_MSG_HANDLER,
		    "BinaryMessageHandler::handleAssociatorsRequest()");
   _handleRequest(op, msg);
   PEG_METHOD_EXIT();
}

void BinaryMessageHandler::handleGetPropertyRequest(
   AsyncOpNode *op,
   CIMGetPropertyRequestMessage *msg) throw()
{
   PEG_METHOD_ENTER(TRC_BINARY_MSG_HANDLER,
		    "BinaryMessageHandler::handleGetPropertyRequest()");
   _handleRequest(op, msg);
   PEG_METHOD_EXIT();
}

void BinaryMessageHandler::handleSetPropertyRequest(
   AsyncOpNode *op,
   CIMSetPropertyRequestMessage *msg) throw()
{
   PEG_METHOD_ENTER(TRC_BINARY_MSG_HANDLER,
		    "BinaryMessageHandler::handleSetPropertyRequest()");
   _handleRequest(op, msg);
   PEG_METHOD_EXIT();
}

void BinaryMessageHandler::handleExecQueryRequest(
   AsyncOpNode *op,
   CIMExecQueryRequestMessage *msg) throw()
{
   PEG_METHOD_ENTER(TRC_BINARY_MSG_HANDLER,
		    "BinaryMessageHandler::handleExecQueryRequest()");
   _handleRequest(op, msg);
   PEG_METHOD_EXIT();
}
 
void BinaryMessageHandler::handleInvokeMethodRequest(
   AsyncOpNode *op,
   CIMInvokeMethodRequestMessage *msg) throw()
{
   PEG_METHOD_ENTER(TRC_BINARY_MSG_HANDLER,
		    "BinaryMessageHandler::handleInvokeMethodRequest()");
   _handleRequest(op, msg);
   PEG_METHOD_EXIT();
}
      
// **** Response Messages **** //
      
void BinaryMessageHandler::handleCreateClassResponse(
   AsyncOpNode *op,
   CIMCreateClassResponseMessage *msg) throw()
{
   PEG_METHOD_ENTER(TRC_BINARY_MSG_HANDLER,
		    "BinaryMessageHandler::handleCreateClassResponse()");
   _handleResponse(op, msg);
   PEG_METHOD_EXIT();
}

void BinaryMessageHandler::handleGetClassResponse(
   AsyncOpNode *op,
   CIMGetClassResponseMessage *msg) throw()
{
   PEG_METHOD_ENTER(TRC_BINARY_MSG_HANDLER,
		    "BinaryMessageHandler::handleGetClassResponse()");
   _handleResponse(op, msg);
   PEG_METHOD_EXIT();
}

void BinaryMessageHandler::handleModifyClassResponse(
   AsyncOpNode *op,
   CIMModifyClassResponseMessage *msg) throw()
{
   PEG_METHOD_ENTER(TRC_BINARY_MSG_HANDLER,
		    "BinaryMessageHandler::handleModifyClassResponse()");
   _handleResponse(op, msg);
   PEG_METHOD_EXIT();
}
      
void BinaryMessageHandler::handleEnumerateClassNamesResponse(
   AsyncOpNode *op,
   CIMEnumerateClassNamesResponseMessage *msg) throw()
{
   PEG_METHOD_ENTER(TRC_BINARY_MSG_HANDLER,
		    "BinaryMessageHandler::handleEnumerateClassNamesResponse()");
   _handleResponse(op, msg);
   PEG_METHOD_EXIT();
}

void BinaryMessageHandler::handleEnumerateClassesResponse(
   AsyncOpNode *op,
   CIMEnumerateClassesResponseMessage *msg) throw()
{
   PEG_METHOD_ENTER(TRC_BINARY_MSG_HANDLER,
		    "BinaryMessageHandler::handleEnumerateClassesResponse()");
   _handleResponse(op, msg);
   PEG_METHOD_EXIT();
}

void BinaryMessageHandler::handleDeleteClassResponse(
   AsyncOpNode *op,
   CIMDeleteClassResponseMessage *msg) throw()
{
   PEG_METHOD_ENTER(TRC_BINARY_MSG_HANDLER,
		    "BinaryMessageHandler::handleDeleteClassResponse()");
   _handleResponse(op, msg);
   PEG_METHOD_EXIT();
}

void BinaryMessageHandler::handleCreateInstanceResponse(
   AsyncOpNode *op,
   CIMCreateInstanceResponseMessage *msg) throw()
{
   PEG_METHOD_ENTER(TRC_BINARY_MSG_HANDLER,
		    "BinaryMessageHandler::handleCreateInstanceResponse()");
   _handleResponse(op, msg);
   PEG_METHOD_EXIT();
}

void BinaryMessageHandler::handleGetInstanceResponse(
   AsyncOpNode *op,
   CIMGetInstanceResponseMessage *msg) throw()
{
   PEG_METHOD_ENTER(TRC_BINARY_MSG_HANDLER,
		    "BinaryMessageHandler::handleGetInstanceResponse()");
   _handleResponse(op, msg);
   PEG_METHOD_EXIT();
}

void BinaryMessageHandler::handleModifyInstanceResponse(
   AsyncOpNode *op,
   CIMModifyInstanceResponseMessage *msg) throw()
{
   PEG_METHOD_ENTER(TRC_BINARY_MSG_HANDLER,
		    "BinaryMessageHandler::handleModifyInstanceResponse()");
   _handleResponse(op, msg);
   PEG_METHOD_EXIT();
}

void BinaryMessageHandler::handleEnumerateInstanceNamesResponse(
   AsyncOpNode *op,
   CIMEnumerateInstanceNamesResponseMessage *msg) throw()
{
   PEG_METHOD_ENTER(TRC_BINARY_MSG_HANDLER,
		    "BinaryMessageHandler::handleEnumerateInstanceNamesResponse()");
   _handleResponse(op, msg);
   PEG_METHOD_EXIT();
}

void BinaryMessageHandler::handleEnumerateInstancesResponse(
   AsyncOpNode *op,
   CIMEnumerateInstancesResponseMessage *msg) throw()
{
   PEG_METHOD_ENTER(TRC_BINARY_MSG_HANDLER,
		    "BinaryMessageHandler::handleEnumerateInstancesResponse()");
   _handleResponse(op, msg);
   PEG_METHOD_EXIT();
}

void BinaryMessageHandler::handleDeleteInstanceResponse(
   AsyncOpNode *op,
   CIMDeleteInstanceResponseMessage *msg) throw()
{
   PEG_METHOD_ENTER(TRC_BINARY_MSG_HANDLER,
		    "BinaryMessageHandler::handleDeleteInstanceResponse()");
   _handleResponse(op, msg);
   PEG_METHOD_EXIT();
}

void BinaryMessageHandler::handleGetPropertyResponse(
   AsyncOpNode *op,
   CIMGetPropertyResponseMessage *msg) throw()
{
   PEG_METHOD_ENTER(TRC_BINARY_MSG_HANDLER,
		    "BinaryMessageHandler::handleGetPropertyResponse()");
   _handleResponse(op, msg);
   PEG_METHOD_EXIT();
}

void BinaryMessageHandler::handleSetPropertyResponse(
   AsyncOpNode *op,
   CIMSetPropertyResponseMessage *msg) throw()
{
   PEG_METHOD_ENTER(TRC_BINARY_MSG_HANDLER,
		    "BinaryMessageHandler::handleSetPropertyResponse()");
   _handleResponse(op, msg);
   PEG_METHOD_EXIT();
}


void BinaryMessageHandler::handleSetQualifierResponse(
   AsyncOpNode *op,
   CIMSetQualifierResponseMessage *msg) throw()
{
   PEG_METHOD_ENTER(TRC_BINARY_MSG_HANDLER,
		    "BinaryMessageHandler::handleSetQualifierResponse()");
   _handleResponse(op, msg);
   PEG_METHOD_EXIT();
}

void BinaryMessageHandler::handleGetQualifierResponse(
   AsyncOpNode *op,
   CIMGetQualifierResponseMessage *msg) throw()
{
   PEG_METHOD_ENTER(TRC_BINARY_MSG_HANDLER,
		    "BinaryMessageHandler::handleGetQualifierResponse()");
   _handleResponse(op, msg);
   PEG_METHOD_EXIT();
}
      
void BinaryMessageHandler::handleEnumerateQualifiersResponse(
   AsyncOpNode *op,
   CIMEnumerateQualifiersResponseMessage *msg) throw()
{
   PEG_METHOD_ENTER(TRC_BINARY_MSG_HANDLER,
		    "BinaryMessageHandler::handleEnumerateQualifiersResponse()");
   _handleResponse(op, msg);
   PEG_METHOD_EXIT();
}

void BinaryMessageHandler::handleDeleteQualifierResponse(
   AsyncOpNode *op,
   CIMDeleteQualifierResponseMessage *msg) throw()
{
   PEG_METHOD_ENTER(TRC_BINARY_MSG_HANDLER,
		    "BinaryMessageHandler::handleDeleteQualifierResponse()");
   _handleResponse(op, msg);
   PEG_METHOD_EXIT();
}

void BinaryMessageHandler::handleReferenceNamesResponse(
   AsyncOpNode *op,
   CIMReferenceNamesResponseMessage *msg) throw()
{
   PEG_METHOD_ENTER(TRC_BINARY_MSG_HANDLER,
		    "BinaryMessageHandler::handleReferenceNamesResponse()");
   _handleResponse(op, msg);
   PEG_METHOD_EXIT();
}

void BinaryMessageHandler::handleReferencesResponse(
   AsyncOpNode *op,
   CIMReferencesResponseMessage *msg) throw()
{
   PEG_METHOD_ENTER(TRC_BINARY_MSG_HANDLER,
		    "BinaryMessageHandler::handleReferencesResponse()");
   _handleResponse(op, msg);
   PEG_METHOD_EXIT();
}

void BinaryMessageHandler::handleAssociatorNamesResponse(
   AsyncOpNode *op,
   CIMAssociatorNamesResponseMessage *msg) throw()
{
   PEG_METHOD_ENTER(TRC_BINARY_MSG_HANDLER,
		    "BinaryMessageHandler::handleAssociatorNamesResponse()");
   _handleResponse(op, msg);
   PEG_METHOD_EXIT();
}

void BinaryMessageHandler::handleAssociatorsResponse(
   AsyncOpNode *op,
   CIMAssociatorsResponseMessage *msg) throw()
{
   PEG_METHOD_ENTER(TRC_BINARY_MSG_HANDLER,
		    "BinaryMessageHandler::handleAssociatorsResponse()");
   _handleResponse(op, msg);
   PEG_METHOD_EXIT();
}

void BinaryMessageHandler::handleExecQueryResponse(
   AsyncOpNode *op,
   CIMExecQueryResponseMessage *msg) throw()
{
   PEG_METHOD_ENTER(TRC_BINARY_MSG_HANDLER,
		    "BinaryMessageHandler::handleExecQueryResponse()");
   _handleResponse(op, msg);
   PEG_METHOD_EXIT();
}


void BinaryMessageHandler::handleInvokeMethodResponse(
   AsyncOpNode *op,
   CIMInvokeMethodResponseMessage *msg) throw()
{
   PEG_METHOD_ENTER(TRC_BINARY_MSG_HANDLER,
		    "BinaryMessageHandler::handleInvokeMethodResponse()");
   _handleResponse(op, msg);
   PEG_METHOD_EXIT();
}

PEGASUS_NAMESPACE_END
