//%/////////////////////////////////////////////////////////////////////////////
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
// Modified By: Yi Zhou (yi_zhou@hp.com)
//              Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Constants.h>
#include <cctype>
#include <cstdio>
#include <Pegasus/Common/XmlParser.h>
#include <Pegasus/Common/XmlReader.h>
#include <Pegasus/Common/Destroyer.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/HTTPMessage.h>
#include <Pegasus/Common/Logger.h>
#include <Pegasus/Common/Tracer.h>
#include "CIMOperationResponseEncoder.h"

#ifdef PEGASUS_CCOVER
# include <ccover.h>
#endif

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

CIMOperationResponseEncoder::CIMOperationResponseEncoder()
   : Base(PEGASUS_SERVICENAME_CIMOPREQENCODER, MessageQueue::getNextQueueId())
{

}

CIMOperationResponseEncoder::~CIMOperationResponseEncoder()
{

}

void CIMOperationResponseEncoder::sendResponse(
   Uint32 queueId, 
   Array<Sint8>& message)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
                    "CIMOperationResponseEncoder::sendResponse()");
   MessageQueue* queue = MessageQueue::lookup(queueId);

   if (queue)

   {
      HTTPMessage* httpMessage = new HTTPMessage(message);
      Tracer::traceBuffer(TRC_XML_IO, Tracer::LEVEL2, 
			  httpMessage->message.getData(), httpMessage->message.size());

      queue->enqueue(httpMessage);

#ifdef PEGASUS_CCOVER
      cov_write();
#endif
   }
   else
   {
      Tracer::trace(TRC_DISPATCHER, Tracer::LEVEL3, 
		    "Invalid queueId = %i, response not sent.", queueId);
   }

   PEG_METHOD_EXIT();
}

// Code is duplicated in CIMOperationRequestDecoder
void CIMOperationResponseEncoder::sendIMethodError(
   Uint32 queueId, 
   const String& messageId,
   const String& iMethodName,
   const CIMException& cimException)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
                     "CIMOperationResponseEncoder::sendIMethodError()");

    Array<Sint8> message;
    message = XmlWriter::formatSimpleIMethodErrorRspMessage(
        iMethodName,
        messageId,
        cimException);

    sendResponse(queueId, message);

    PEG_METHOD_EXIT();
}

void CIMOperationResponseEncoder::sendIMethodError(
   CIMResponseMessage* response,
   const String& cimMethodName)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
                    "CIMOperationResponseEncoder::sendIMethodError()");

   Uint32 queueId = response->queueIds.top();
   response->queueIds.pop();

   sendIMethodError(
      queueId,
      response->messageId, 
      cimMethodName, 
      response->cimException); 

   PEG_METHOD_EXIT();
}

void CIMOperationResponseEncoder::sendMethodError(
   Uint32 queueId, 
   const String& messageId,
   const String& methodName,
   const CIMException& cimException)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
                     "CIMOperationResponseEncoder::sendMethodError()");

    Array<Sint8> message;
    message = XmlWriter::formatSimpleMethodErrorRspMessage(
        methodName,
        messageId,
        cimException);

    sendResponse(queueId, message);

    PEG_METHOD_EXIT();
}

void CIMOperationResponseEncoder::sendMethodError(
   CIMResponseMessage* response,
   const String& cimMethodName)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
                    "CIMOperationResponseEncoder::sendMethodError()");

   Uint32 queueId = response->queueIds.top();
   response->queueIds.pop();

   sendMethodError(
      queueId,
      response->messageId, 
      cimMethodName, 
      response->cimException); 
   PEG_METHOD_EXIT();
}

void CIMOperationResponseEncoder::handleEnqueue(Message *message)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
                    "CIMOperationResponseEncoder::handleEnqueue()");

   if (!message)
   {
      PEG_METHOD_EXIT();
      return;
   }
   
   switch (message->getType())
   {
      case CIM_GET_CLASS_RESPONSE_MESSAGE:
	 encodeGetClassResponse(
	    (CIMGetClassResponseMessage*)message);
	 break;

      case CIM_GET_INSTANCE_RESPONSE_MESSAGE:
	 encodeGetInstanceResponse(
	    (CIMGetInstanceResponseMessage*)message);
	 break;

      case CIM_DELETE_CLASS_RESPONSE_MESSAGE:
	 encodeDeleteClassResponse(
	    (CIMDeleteClassResponseMessage*)message);
	 break;

      case CIM_DELETE_INSTANCE_RESPONSE_MESSAGE:
	 encodeDeleteInstanceResponse(
	    (CIMDeleteInstanceResponseMessage*)message);
	 break;

      case CIM_CREATE_CLASS_RESPONSE_MESSAGE:
	 encodeCreateClassResponse(
	    (CIMCreateClassResponseMessage*)message);
	 break;

      case CIM_CREATE_INSTANCE_RESPONSE_MESSAGE:
	 encodeCreateInstanceResponse(
	    (CIMCreateInstanceResponseMessage*)message);
	 break;

      case CIM_MODIFY_CLASS_RESPONSE_MESSAGE:
	 encodeModifyClassResponse(
	    (CIMModifyClassResponseMessage*)message);
	 break;

      case CIM_MODIFY_INSTANCE_RESPONSE_MESSAGE:
	 encodeModifyInstanceResponse(
	    (CIMModifyInstanceResponseMessage*)message);
	 break;

      case CIM_ENUMERATE_CLASSES_RESPONSE_MESSAGE:
	 encodeEnumerateClassesResponse(
	    (CIMEnumerateClassesResponseMessage*)message);
	 break;

      case CIM_ENUMERATE_CLASS_NAMES_RESPONSE_MESSAGE:
	 encodeEnumerateClassNamesResponse(
	    (CIMEnumerateClassNamesResponseMessage*)message);
	 break;

      case CIM_ENUMERATE_INSTANCES_RESPONSE_MESSAGE:
	 encodeEnumerateInstancesResponse(
	    (CIMEnumerateInstancesResponseMessage*)message);
	 break;

      case CIM_ENUMERATE_INSTANCE_NAMES_RESPONSE_MESSAGE:
	 encodeEnumerateInstanceNamesResponse(
	    (CIMEnumerateInstanceNamesResponseMessage*)message);
	 break;

      case CIM_EXEC_QUERY_RESPONSE_MESSAGE:
	 encodeExecQueryResponse(
	    (CIMExecQueryResponseMessage*)message);
	 break;

      case CIM_ASSOCIATORS_RESPONSE_MESSAGE:
	 encodeAssociatorsResponse(
	    (CIMAssociatorsResponseMessage*)message);
	 break;

      case CIM_ASSOCIATOR_NAMES_RESPONSE_MESSAGE:
	 encodeAssociatorNamesResponse(
	    (CIMAssociatorNamesResponseMessage*)message);
	 break;

      case CIM_REFERENCES_RESPONSE_MESSAGE:
	 encodeReferencesResponse(
	    (CIMReferencesResponseMessage*)message);
	 break;

      case CIM_REFERENCE_NAMES_RESPONSE_MESSAGE:
	 encodeReferenceNamesResponse(
	    (CIMReferenceNamesResponseMessage*)message);
	 break;

      case CIM_GET_PROPERTY_RESPONSE_MESSAGE:
	 encodeGetPropertyResponse(
	    (CIMGetPropertyResponseMessage*)message);
	 break;

      case CIM_SET_PROPERTY_RESPONSE_MESSAGE:
	 encodeSetPropertyResponse(
	    (CIMSetPropertyResponseMessage*)message);
	 break;

      case CIM_GET_QUALIFIER_RESPONSE_MESSAGE:
	 encodeGetQualifierResponse(
	    (CIMGetQualifierResponseMessage*)message);
	 break;

      case CIM_SET_QUALIFIER_RESPONSE_MESSAGE:
	 encodeSetQualifierResponse(
	    (CIMSetQualifierResponseMessage*)message);
	 break;

      case CIM_DELETE_QUALIFIER_RESPONSE_MESSAGE:
	 encodeDeleteQualifierResponse(
	    (CIMDeleteQualifierResponseMessage*)message);
	 break;

      case CIM_ENUMERATE_QUALIFIERS_RESPONSE_MESSAGE:
	 encodeEnumerateQualifiersResponse(
	    (CIMEnumerateQualifiersResponseMessage*)message);
	 break;

      case CIM_INVOKE_METHOD_RESPONSE_MESSAGE:
	 encodeInvokeMethodResponse(
	    (CIMInvokeMethodResponseMessage*)message); 
	 break;
   }

   delete message;

   PEG_METHOD_EXIT();
    
   return;
    
}


void CIMOperationResponseEncoder::handleEnqueue()
{

   Message* message = dequeue();
   if(message)
      handleEnqueue(message);
}

const char* CIMOperationResponseEncoder::getQueueName() const
{
   return PEGASUS_SERVICENAME_CIMOPREQENCODER;
}

void CIMOperationResponseEncoder::encodeCreateClassResponse(
   CIMCreateClassResponseMessage* response)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
                    "CIMOperationResponseEncoder::encodeCreateClassResponse()");

   if (response->cimException.getCode() != CIM_ERR_SUCCESS)
   {
      sendIMethodError(response, "CreateClass");
      PEG_METHOD_EXIT();
      return;
   }

   Array<Sint8> body;

   Array<Sint8> message = XmlWriter::formatSimpleIMethodRspMessage(
      "CreateClass", response->messageId, body);

   sendResponse(response->queueIds.top(), message);

   PEG_METHOD_EXIT();
}

void CIMOperationResponseEncoder::encodeGetClassResponse(
   CIMGetClassResponseMessage* response)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
                    "CIMOperationResponseEncoder::encodeGetClassResponse()");

   if (response->cimException.getCode() != CIM_ERR_SUCCESS)
   {
      sendIMethodError(response, "GetClass");
      PEG_METHOD_EXIT();
      return;
   }

   Array<Sint8> body;
   response->cimClass.toXml(body);

   Array<Sint8> message = XmlWriter::formatSimpleIMethodRspMessage(
      "GetClass", response->messageId, body);

   sendResponse(response->queueIds.top(), message);

   PEG_METHOD_EXIT();
}

void CIMOperationResponseEncoder::encodeModifyClassResponse(
   CIMModifyClassResponseMessage* response)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
                    "CIMOperationResponseEncoder::encodeModifyClassResponse()");

   if (response->cimException.getCode() != CIM_ERR_SUCCESS)
   {
      sendIMethodError(response, "ModifyClass");
      PEG_METHOD_EXIT();
      return;
   }

   Array<Sint8> body;

   Array<Sint8> message = XmlWriter::formatSimpleIMethodRspMessage(
      "ModifyClass", response->messageId, body);

   sendResponse(response->queueIds.top(), message);

   PEG_METHOD_EXIT();
}

void CIMOperationResponseEncoder::encodeEnumerateClassNamesResponse(
   CIMEnumerateClassNamesResponseMessage* response)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER, "CIMOperationResponseEncoder::"
		    "encodeEnumerateClassNamesResponse()");

   if (response->cimException.getCode() != CIM_ERR_SUCCESS)
   {
      sendIMethodError(response, "EnumerateClassNames");
      PEG_METHOD_EXIT();
      return;
   }

   Array<Sint8> body;

   for (Uint32 i = 0; i < response->classNames.size(); i++)
      XmlWriter::appendClassNameElement(body, response->classNames[i]);

   Array<Sint8> message = XmlWriter::formatSimpleIMethodRspMessage(
      "EnumerateClassNames", response->messageId, body);

   sendResponse(response->queueIds.top(), message);

   PEG_METHOD_EXIT();
}

void CIMOperationResponseEncoder::encodeEnumerateClassesResponse(
   CIMEnumerateClassesResponseMessage* response)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER, "CIMOperationResponseEncoder::"
		    "encodeEnumerateClassesResponse()");

   if (response->cimException.getCode() != CIM_ERR_SUCCESS)
   {
      sendIMethodError(response, "EnumerateClasses");
      PEG_METHOD_EXIT();
      return;
   }

   Array<Sint8> body;

   for (Uint32 i = 0; i < response->cimClasses.size(); i++)
      response->cimClasses[i].toXml(body);

   Array<Sint8> message = XmlWriter::formatSimpleIMethodRspMessage(
      "EnumerateClasses", response->messageId, body);

   sendResponse(response->queueIds.top(), message);
   PEG_METHOD_EXIT();
}

void CIMOperationResponseEncoder::encodeDeleteClassResponse(
   CIMDeleteClassResponseMessage* response)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
                    "CIMOperationResponseEncoder::encodeDeleteClassResponse()");

   if (response->cimException.getCode() != CIM_ERR_SUCCESS)
   {
      sendIMethodError(response, "DeleteClass");
      PEG_METHOD_EXIT();
      return;
   }

   Array<Sint8> body;

   Array<Sint8> message = XmlWriter::formatSimpleIMethodRspMessage(
      "DeleteClass", response->messageId, body);

   sendResponse(response->queueIds.top(), message);
   PEG_METHOD_EXIT();
}

void CIMOperationResponseEncoder::encodeCreateInstanceResponse(
   CIMCreateInstanceResponseMessage* response)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER, "CIMOperationResponseEncoder::"
		    "encodeCreateInstanceResponse()");

   if (response->cimException.getCode() != CIM_ERR_SUCCESS)
   {
      sendIMethodError(response, "CreateInstance");
      PEG_METHOD_EXIT();
      return;
   }

   Array<Sint8> body;

   XmlWriter::appendInstanceNameElement(body, response->instanceName);

   Array<Sint8> message = XmlWriter::formatSimpleIMethodRspMessage(
      "CreateInstance", response->messageId, body);

   sendResponse(response->queueIds.top(), message);
   PEG_METHOD_EXIT();
}

void CIMOperationResponseEncoder::encodeGetInstanceResponse(
   CIMGetInstanceResponseMessage* response)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER, "CIMOperationResponseEncoder::"
		    "encodeGetInstanceResponse()");

   if (response->cimException.getCode() != CIM_ERR_SUCCESS)
   {
      sendIMethodError(response, "GetInstance");
      PEG_METHOD_EXIT();
      return;
   }

   Array<Sint8> body;
   response->cimInstance.toXml(body);

   Array<Sint8> message = XmlWriter::formatSimpleIMethodRspMessage(
      "GetInstance", response->messageId, body);

   sendResponse(response->queueIds.top(), message);
   PEG_METHOD_EXIT();
}

void CIMOperationResponseEncoder::encodeModifyInstanceResponse(
   CIMModifyInstanceResponseMessage* response)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER, "CIMOperationResponseEncoder::"
		    "encodeModifyInstanceResponse()");

   if (response->cimException.getCode() != CIM_ERR_SUCCESS)
   {
      sendIMethodError(response, "ModifyInstance");
      PEG_METHOD_EXIT();
      return;
   }

   Array<Sint8> body;

   Array<Sint8> message = XmlWriter::formatSimpleIMethodRspMessage(
      "ModifyInstance", response->messageId, body);

   sendResponse(response->queueIds.top(), message);
   PEG_METHOD_EXIT();
}

void CIMOperationResponseEncoder::encodeEnumerateInstancesResponse(
   CIMEnumerateInstancesResponseMessage* response)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER, "CIMOperationResponseEncoder::"
		    "encodeEnumerateInstancesResponse()");

   if (response->cimException.getCode() != CIM_ERR_SUCCESS)
   {
      sendIMethodError(response, "EnumerateInstances");
      PEG_METHOD_EXIT();
      return;
   }

   Array<Sint8> body;

   for (Uint32 i = 0; i < response->cimNamedInstances.size(); i++)
      response->cimNamedInstances[i].toXml(body);

   Array<Sint8> message = XmlWriter::formatSimpleIMethodRspMessage(
      "EnumerateInstances", response->messageId, body);

   sendResponse(response->queueIds.top(), message);
   PEG_METHOD_EXIT();
}

void CIMOperationResponseEncoder::encodeEnumerateInstanceNamesResponse(
   CIMEnumerateInstanceNamesResponseMessage* response)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER, "CIMOperationResponseEncoder::"
		    "encodeEnumerateInstanceNamesResponse()");

   if (response->cimException.getCode() != CIM_ERR_SUCCESS)
   {
      sendIMethodError(response, "EnumerateInstanceNames");
      PEG_METHOD_EXIT();
      return;
   }

   Array<Sint8> body;

   for (Uint32 i = 0; i < response->instanceNames.size(); i++)
      XmlWriter::appendInstanceNameElement(body, response->instanceNames[i]);

   Array<Sint8> message = XmlWriter::formatSimpleIMethodRspMessage(
      "EnumerateInstanceNames", response->messageId, body);

   sendResponse(response->queueIds.top(), message);
   PEG_METHOD_EXIT();
}

void CIMOperationResponseEncoder::encodeDeleteInstanceResponse(
   CIMDeleteInstanceResponseMessage* response)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER, "CIMOperationResponseEncoder::"
		    "encodeDeleteInstanceResponse()");

   if (response->cimException.getCode() != CIM_ERR_SUCCESS)
   {
      sendIMethodError(response, "DeleteInstance");
      PEG_METHOD_EXIT();
      return;
   }

   Array<Sint8> body;

   Array<Sint8> message = XmlWriter::formatSimpleIMethodRspMessage(
      "DeleteInstance", response->messageId, body);

   sendResponse(response->queueIds.top(), message);
   PEG_METHOD_EXIT();
}

void CIMOperationResponseEncoder::encodeGetPropertyResponse(
   CIMGetPropertyResponseMessage* response)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
                    "CIMOperationResponseEncoder::encodeGetPropertyResponse()");

   if (response->cimException.getCode() != CIM_ERR_SUCCESS)
   {
      sendIMethodError(response, "GetProperty");
      PEG_METHOD_EXIT();
      return;
   }

   Array<Sint8> body;
   // Put the value in body. Note, if Null but out the value tags
   // ATTN: P1 KS NULL testing.  Not sure if this is correct. Should
   // we be putting the value tags out or not for null?
   response->value.toXml(body, true);

   Array<Sint8> message = XmlWriter::formatSimpleIMethodRspMessage(
      "GetProperty", response->messageId, body);

   sendResponse(response->queueIds.top(), message);
   PEG_METHOD_EXIT();
}

void CIMOperationResponseEncoder::encodeSetPropertyResponse(
   CIMSetPropertyResponseMessage* response)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
                    "CIMOperationResponseEncoder::encodeSetPropertyResponse()");

   if (response->cimException.getCode() != CIM_ERR_SUCCESS)
   {
      sendIMethodError(response, "SetProperty");
      PEG_METHOD_EXIT();
      return;
   }

   Array<Sint8> body;

   Array<Sint8> message = XmlWriter::formatSimpleIMethodRspMessage(
      "SetProperty", response->messageId, body);

   sendResponse(response->queueIds.top(), message);
   PEG_METHOD_EXIT();
}

void CIMOperationResponseEncoder::encodeSetQualifierResponse(
   CIMSetQualifierResponseMessage* response)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER, "CIMOperationResponseEncoder::"
		    "encodeSetQualifierResponse()");

   if (response->cimException.getCode() != CIM_ERR_SUCCESS)
   {
      sendIMethodError(response, "SetQualifier");
      PEG_METHOD_EXIT();
      return;
   }

   Array<Sint8> body;

   Array<Sint8> message = XmlWriter::formatSimpleIMethodRspMessage(
      "SetQualifier", response->messageId, body);

   sendResponse(response->queueIds.top(), message);
   PEG_METHOD_EXIT();
}

void CIMOperationResponseEncoder::encodeGetQualifierResponse(
   CIMGetQualifierResponseMessage* response)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER, "CIMOperationResponseEncoder::"
		    "encodeGetQualifierResponse()");

   if (response->cimException.getCode() != CIM_ERR_SUCCESS)
   {
      sendIMethodError(response, "GetQualifier");
      PEG_METHOD_EXIT();
      return;
   }

   Array<Sint8> body;
   response->cimQualifierDecl.toXml(body);

   Array<Sint8> message = XmlWriter::formatSimpleIMethodRspMessage(
      "GetQualifier", response->messageId, body);

   sendResponse(response->queueIds.top(), message);
   PEG_METHOD_EXIT();
}

void CIMOperationResponseEncoder::encodeEnumerateQualifiersResponse(
   CIMEnumerateQualifiersResponseMessage* response)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER, "CIMOperationResponseEncoder::"
		    "encodeEnumerateQualifierResponse()");

   if (response->cimException.getCode() != CIM_ERR_SUCCESS)
   {
      sendIMethodError(response, "EnumerateQualifiers");
      PEG_METHOD_EXIT();
      return;
   }

   Array<Sint8> body;

   for (Uint32 i = 0; i < response->qualifierDeclarations.size(); i++)
      response->qualifierDeclarations[i].toXml(body);

   Array<Sint8> message = XmlWriter::formatSimpleIMethodRspMessage(
      "EnumerateQualifiers", response->messageId, body);

   sendResponse(response->queueIds.top(), message);
   PEG_METHOD_EXIT();
}

void CIMOperationResponseEncoder::encodeDeleteQualifierResponse(
   CIMDeleteQualifierResponseMessage* response)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER, "CIMOperationResponseEncoder::"
		    "encodeDeleteQualifierResponse()");

   if (response->cimException.getCode() != CIM_ERR_SUCCESS)
   {
      sendIMethodError(response, "DeleteQualifier");
      PEG_METHOD_EXIT();
      return;
   }

   Array<Sint8> body;

   Array<Sint8> message = XmlWriter::formatSimpleIMethodRspMessage(
      "DeleteQualifier", response->messageId, body);

   sendResponse(response->queueIds.top(), message);
   PEG_METHOD_EXIT();
}

void CIMOperationResponseEncoder::encodeReferenceNamesResponse(
   CIMReferenceNamesResponseMessage* response)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER, "CIMOperationResponseEncoder::"
		    "encodeReferenceNamesResponse()");

   if (response->cimException.getCode() != CIM_ERR_SUCCESS)
   {
      sendIMethodError(response, "ReferenceNames");
      PEG_METHOD_EXIT();
      return;
   }

   Array<Sint8> body;

   for (Uint32 i = 0; i < response->objectNames.size(); i++)
   {
      body << "<OBJECTPATH>\n";
      response->objectNames[i].toXml(body, false);
      body << "</OBJECTPATH>\n";
   }

   Array<Sint8> message = XmlWriter::formatSimpleIMethodRspMessage(
      "ReferenceNames", response->messageId, body);

   sendResponse(response->queueIds.top(), message);
   PEG_METHOD_EXIT();
}

void CIMOperationResponseEncoder::encodeReferencesResponse(
   CIMReferencesResponseMessage* response)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
                    "CIMOperationResponseEncoder::encodeReferencesResponse()");

   if (response->cimException.getCode() != CIM_ERR_SUCCESS)
   {
      sendIMethodError(response, "References");
      PEG_METHOD_EXIT();
      return;
   }

   Array<Sint8> body;

   for (Uint32 i = 0; i < response->cimObjects.size(); i++)
      response->cimObjects[i].toXml(body);

   Array<Sint8> message = XmlWriter::formatSimpleIMethodRspMessage(
      "References", response->messageId, body);

   sendResponse(response->queueIds.top(), message);
   PEG_METHOD_EXIT();
}

void CIMOperationResponseEncoder::encodeAssociatorNamesResponse(
   CIMAssociatorNamesResponseMessage* response)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER, "CIMOperationResponseEncoder::"
		    "encodeAssociatorNamesResponse()");

   if (response->cimException.getCode() != CIM_ERR_SUCCESS)
   {
      sendIMethodError(response, "AssociatorNames");
      PEG_METHOD_EXIT();
      return;
   }

   Array<Sint8> body;

   for (Uint32 i = 0; i < response->objectNames.size(); i++)
   {
      body << "<OBJECTPATH>\n";
      response->objectNames[i].toXml(body, false);
      body << "</OBJECTPATH>\n";
   }

   Array<Sint8> message = XmlWriter::formatSimpleIMethodRspMessage(
      "AssociatorNames", response->messageId, body);

   sendResponse(response->queueIds.top(), message);
   PEG_METHOD_EXIT();
}

void CIMOperationResponseEncoder::encodeAssociatorsResponse(
   CIMAssociatorsResponseMessage* response)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
                    "CIMOperationResponseEncoder::encodeAssociatorsResponse()");

   if (response->cimException.getCode() != CIM_ERR_SUCCESS)
   {
      sendIMethodError(response, "Associators");
      PEG_METHOD_EXIT();
      return;
   }

   Array<Sint8> body;

   for (Uint32 i = 0; i < response->cimObjects.size(); i++)
      response->cimObjects[i].toXml(body);

   Array<Sint8> message = XmlWriter::formatSimpleIMethodRspMessage(
      "Associators", response->messageId, body);

   sendResponse(response->queueIds.top(), message);
   PEG_METHOD_EXIT();
}

void CIMOperationResponseEncoder::encodeExecQueryResponse(
   CIMExecQueryResponseMessage* response)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
                    "CIMOperationResponseEncoder::encodeExecQueryResponse()");

   if (response->cimException.getCode() != CIM_ERR_SUCCESS)
   {
      sendIMethodError(response, "ExecQuery");
      PEG_METHOD_EXIT();
      return;
   }

   Array<Sint8> body;

   for (Uint32 i = 0; i < response->cimObjects.size(); i++)
      response->cimObjects[i].toXml(body);

   Array<Sint8> message = XmlWriter::formatSimpleIMethodRspMessage(
      "ExecQuery", response->messageId, body);

   sendResponse(response->queueIds.top(), message);
   PEG_METHOD_EXIT();
}

void CIMOperationResponseEncoder::encodeInvokeMethodResponse(
   CIMInvokeMethodResponseMessage* response)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER, "CIMOperationResponseEncoder::"
		    "encodeInvokeMethodResponse()");

   if (response->cimException.getCode() != CIM_ERR_SUCCESS)
   {
      sendMethodError(response, response->methodName);
      PEG_METHOD_EXIT();
      return;
   }

   Array<Sint8> body;

   // ATTN-RK-P3-20020219: Who's job is it to make sure the return value is
   // not an array?
   XmlWriter::appendReturnValueElement(body, response->retValue);

   for (Uint32 i=0; i < response->outParameters.size(); i++)
   {
      response->outParameters[i].toXml(body);
   }

   Array<Sint8> message = XmlWriter::formatSimpleMethodRspMessage(
      _CString(response->methodName), response->messageId, body);

   sendResponse(response->queueIds.top(), message);
   PEG_METHOD_EXIT();
}

PEGASUS_NAMESPACE_END
