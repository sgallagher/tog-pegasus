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
   : Base("CIMOpResponseEncoder", MessageQueue::getNextQueueId())
{

}

CIMOperationResponseEncoder::~CIMOperationResponseEncoder()
{

}

void CIMOperationResponseEncoder::sendResponse(
   Uint32 queueId, 
   Array<Sint8>& message)
{
   PEG_FUNC_ENTER(TRC_DISPATCHER,"CIMOperationResponseEncoder::sendResponse()");
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

   PEG_FUNC_EXIT(TRC_DISPATCHER,"CIMOperationResponseEncoder::sendResponse()");
}

// Code is duplicated in CIMOperationRequestDecoder
void CIMOperationResponseEncoder::sendIMethodError(
   Uint32 queueId, 
   const String& messageId,
   const String& iMethodName,
   CIMStatusCode code,
   const String& description) 
{
    PEG_FUNC_ENTER(TRC_DISPATCHER,"CIMOperationResponseEncoder::"
		   "sendIMethodError()");

    Array<Sint8> message;
    message = XmlWriter::formatSimpleIMethodErrorRspMessage(
        iMethodName,
        messageId,
        code,
        description);

    sendResponse(queueId, message);

    PEG_FUNC_EXIT(TRC_DISPATCHER,"CIMOperationResponseEncoder::"
		  "sendIMethodError()");
}

void CIMOperationResponseEncoder::sendIMethodError(
   CIMResponseMessage* response,
   const String& cimMethodName)
{
   PEG_FUNC_ENTER(TRC_DISPATCHER,"CIMOperationResponseEncoder::"
		  "sendIMethodError()");

   Uint32 queueId = response->queueIds.top();
   response->queueIds.pop();

   sendIMethodError(
      queueId,
      response->messageId, 
      cimMethodName, 
      response->errorCode, 
      response->errorDescription);

   PEG_FUNC_EXIT(TRC_DISPATCHER,"CIMOperationResponseEncoder::"
		 "sendIMethodError()");
}

void CIMOperationResponseEncoder::sendMethodError(
   Uint32 queueId, 
   const String& messageId,
   const String& methodName,
   CIMStatusCode code,
   const String& description) 
{
    PEG_FUNC_ENTER(TRC_DISPATCHER,"CIMOperationResponseEncoder::"
		   "sendMethodError()");

    Array<Sint8> message;
    message = XmlWriter::formatSimpleMethodErrorRspMessage(
        methodName,
        messageId,
        code,
        description);

    sendResponse(queueId, message);

    PEG_FUNC_EXIT(TRC_DISPATCHER,"CIMOperationResponseEncoder::"
		  "sendMethodError()");
}

void CIMOperationResponseEncoder::sendMethodError(
   CIMResponseMessage* response,
   const String& cimMethodName)
{
   PEG_FUNC_ENTER(TRC_DISPATCHER,"CIMOperationResponseEncoder::"
		  "sendMethodError()");

   Uint32 queueId = response->queueIds.top();
   response->queueIds.pop();

   sendMethodError(
      queueId,
      response->messageId, 
      cimMethodName, 
      response->errorCode, 
      response->errorDescription);
   PEG_FUNC_EXIT(TRC_DISPATCHER,"CIMOperationResponseEncoder::"
		 "sendMethodError()");
}

void CIMOperationResponseEncoder::handleEnqueue(Message *message)
{
   if (!message)
   {
      PEG_FUNC_EXIT(TRC_DISPATCHER,"CIMOperationResponseEncoder::"
		    "handleEnqueue()");
      return;
   }
   PEG_FUNC_ENTER(TRC_DISPATCHER,"CIMOperationResponseEncoder::"
		  "handleEnqueue()");
   
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

	 // ATTN: implement this!
      case CIM_EXEC_QUERY_RESPONSE_MESSAGE:
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

   PEG_FUNC_EXIT(TRC_DISPATCHER,"CIMOperationResponseEncoder::"
		 "handleEnqueue()");
    
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
   return "CIMOperationResponseEncoder";
}

void CIMOperationResponseEncoder::encodeCreateClassResponse(
   CIMCreateClassResponseMessage* response)
{
   PEG_FUNC_ENTER(TRC_DISPATCHER,"CIMOperationResponseEncoder::"
		  "encodeCreateClassResponse()");

   if (response->errorCode != CIM_ERR_SUCCESS)
   {
      sendIMethodError(response, "CreateClass");
      PEG_FUNC_EXIT(TRC_DISPATCHER,"CIMOperationResponseEncoder::"
		    "encodeCreateClassResponse()");
      return;
   }

   Array<Sint8> body;

   Array<Sint8> message = XmlWriter::formatSimpleIMethodRspMessage(
      "CreateClass", response->messageId, body);

   sendResponse(response->queueIds.top(), message);

   PEG_FUNC_EXIT(TRC_DISPATCHER,"CIMOperationResponseEncoder::"
		 "encodeCreateClassResponse()");
}

void CIMOperationResponseEncoder::encodeGetClassResponse(
   CIMGetClassResponseMessage* response)
{
   PEG_FUNC_ENTER(TRC_DISPATCHER,"CIMOperationResponseEncoder::"
		  "encodeGetClassResponse()");

   if (response->errorCode != CIM_ERR_SUCCESS)
   {
      sendIMethodError(response, "GetClass");
      PEG_FUNC_EXIT(TRC_DISPATCHER,"CIMOperationResponseEncoder::"
		    "encodeGetClassResponse()");
      return;
   }

   Array<Sint8> body;
   response->cimClass.toXml(body);

   Array<Sint8> message = XmlWriter::formatSimpleIMethodRspMessage(
      "GetClass", response->messageId, body);

   sendResponse(response->queueIds.top(), message);

   PEG_FUNC_EXIT(TRC_DISPATCHER,"CIMOperationResponseEncoder::"
		 "encodeGetClassResponse()");
}

void CIMOperationResponseEncoder::encodeModifyClassResponse(
   CIMModifyClassResponseMessage* response)
{
   PEG_FUNC_ENTER(TRC_DISPATCHER,"CIMOperationResponseEncoder::"
		  "encodeModifyClassResponse()");

   if (response->errorCode != CIM_ERR_SUCCESS)
   {
      sendIMethodError(response, "ModifyClass");
      PEG_FUNC_EXIT(TRC_DISPATCHER,"CIMOperationResponseEncoder::"
		    "encodeModifyClassResponse()");
      return;
   }

   Array<Sint8> body;

   Array<Sint8> message = XmlWriter::formatSimpleIMethodRspMessage(
      "ModifyClass", response->messageId, body);

   sendResponse(response->queueIds.top(), message);

   PEG_FUNC_EXIT(TRC_DISPATCHER,"CIMOperationResponseEncoder::"
		 "encodeModifyClassResponse()");
}

void CIMOperationResponseEncoder::encodeEnumerateClassNamesResponse(
   CIMEnumerateClassNamesResponseMessage* response)
{
   PEG_FUNC_ENTER(TRC_DISPATCHER,"CIMOperationResponseEncoder::"
		  "encodeEnumerateClassNamesResponse()");

   if (response->errorCode != CIM_ERR_SUCCESS)
   {
      sendIMethodError(response, "EnumerateClassNames");
      PEG_FUNC_EXIT(TRC_DISPATCHER,"CIMOperationResponseEncoder::"
		    "encodeEnumerateClassNamesResponse()");
      return;
   }

   Array<Sint8> body;

   for (Uint32 i = 0; i < response->classNames.size(); i++)
      XmlWriter::appendClassNameElement(body, response->classNames[i]);

   Array<Sint8> message = XmlWriter::formatSimpleIMethodRspMessage(
      "EnumerateClassNames", response->messageId, body);

   sendResponse(response->queueIds.top(), message);

   PEG_FUNC_EXIT(TRC_DISPATCHER,"CIMOperationResponseEncoder::"
		 "encodeEnumerateClassNamesResponse()");
}

void CIMOperationResponseEncoder::encodeEnumerateClassesResponse(
   CIMEnumerateClassesResponseMessage* response)
{
   PEG_FUNC_ENTER(TRC_DISPATCHER,"CIMOperationResponseEncoder::"
		  "encodeEnumerateClassesResponse()");

   if (response->errorCode != CIM_ERR_SUCCESS)
   {
      sendIMethodError(response, "EnumerateClasses");
      PEG_FUNC_EXIT(TRC_DISPATCHER,"CIMOperationResponseEncoder::"
		    "encodeEnumerateClassesResponse()");
      return;
   }

   Array<Sint8> body;

   for (Uint32 i = 0; i < response->cimClasses.size(); i++)
      response->cimClasses[i].toXml(body);

   Array<Sint8> message = XmlWriter::formatSimpleIMethodRspMessage(
      "EnumerateClasses", response->messageId, body);

   sendResponse(response->queueIds.top(), message);
   PEG_FUNC_EXIT(TRC_DISPATCHER,"CIMOperationResponseEncoder::"
		 "encodeEnumerateClassesResponse()");
}

void CIMOperationResponseEncoder::encodeDeleteClassResponse(
   CIMDeleteClassResponseMessage* response)
{
   PEG_FUNC_ENTER(TRC_DISPATCHER,"CIMOperationResponseEncoder::"
		  "encodeDeleteClassResponse()");

   if (response->errorCode != CIM_ERR_SUCCESS)
   {
      sendIMethodError(response, "DeleteClass");
      PEG_FUNC_EXIT(TRC_DISPATCHER,"CIMOperationResponseEncoder::"
		    "encodeDeleteClassResponse()");
      return;
   }

   Array<Sint8> body;

   Array<Sint8> message = XmlWriter::formatSimpleIMethodRspMessage(
      "DeleteClass", response->messageId, body);

   sendResponse(response->queueIds.top(), message);
   PEG_FUNC_EXIT(TRC_DISPATCHER,"CIMOperationResponseEncoder::"
		 "encodeDeleteClassResponse()");
}

void CIMOperationResponseEncoder::encodeCreateInstanceResponse(
   CIMCreateInstanceResponseMessage* response)
{
   PEG_FUNC_ENTER(TRC_DISPATCHER,"CIMOperationResponseEncoder::"
		  "encodeCreateInstanceResponse()");

   if (response->errorCode != CIM_ERR_SUCCESS)
   {
      sendIMethodError(response, "CreateInstance");
      PEG_FUNC_EXIT(TRC_DISPATCHER,"CIMOperationResponseEncoder::"
		    "encodeCreateInstanceResponse()");
      return;
   }

   Array<Sint8> body;

   XmlWriter::appendInstanceNameElement(body, response->instanceName);

   Array<Sint8> message = XmlWriter::formatSimpleIMethodRspMessage(
      "CreateInstance", response->messageId, body);

   sendResponse(response->queueIds.top(), message);
   PEG_FUNC_EXIT(TRC_DISPATCHER,"CIMOperationResponseEncoder::"
		 "encodeCreateInstanceResponse()");
}

void CIMOperationResponseEncoder::encodeGetInstanceResponse(
   CIMGetInstanceResponseMessage* response)
{
   PEG_FUNC_ENTER(TRC_DISPATCHER,"CIMOperationResponseEncoder::"
		  "encodeGetInstanceResponse()");

   if (response->errorCode != CIM_ERR_SUCCESS)
   {
      sendIMethodError(response, "GetInstance");
      PEG_FUNC_EXIT(TRC_DISPATCHER,"CIMOperationResponseEncoder::"
		    "encodeGetInstanceResponse()");
      return;
   }

   Array<Sint8> body;
   response->cimInstance.toXml(body);

   Array<Sint8> message = XmlWriter::formatSimpleIMethodRspMessage(
      "GetInstance", response->messageId, body);

   sendResponse(response->queueIds.top(), message);
   PEG_FUNC_EXIT(TRC_DISPATCHER,"CIMOperationResponseEncoder::"
		 "encodeGetInstanceResponse()");
}

void CIMOperationResponseEncoder::encodeModifyInstanceResponse(
   CIMModifyInstanceResponseMessage* response)
{
   PEG_FUNC_ENTER(TRC_DISPATCHER,"CIMOperationResponseEncoder::"
		  "encodeModifyInstanceResponse()");

   if (response->errorCode != CIM_ERR_SUCCESS)
   {
      sendIMethodError(response, "ModifyInstance");
      PEG_FUNC_EXIT(TRC_DISPATCHER,"CIMOperationResponseEncoder::"
		    "encodeModifyInstanceResponse()");
      return;
   }

   Array<Sint8> body;

   Array<Sint8> message = XmlWriter::formatSimpleIMethodRspMessage(
      "ModifyInstance", response->messageId, body);

   sendResponse(response->queueIds.top(), message);
   PEG_FUNC_EXIT(TRC_DISPATCHER,"CIMOperationResponseEncoder::"
		 "encodeModifyInstanceResponse()");
}

void CIMOperationResponseEncoder::encodeEnumerateInstancesResponse(
   CIMEnumerateInstancesResponseMessage* response)
{
   PEG_FUNC_ENTER(TRC_DISPATCHER,"CIMOperationResponseEncoder::"
		  "encodeEnumerateInstancesResponse()");

   if (response->errorCode != CIM_ERR_SUCCESS)
   {
      sendIMethodError(response, "EnumerateInstances");
      PEG_FUNC_EXIT(TRC_DISPATCHER,"CIMOperationResponseEncoder::"
		    "encodeEnumerateInstanceResponse()");
      return;
   }

   Array<Sint8> body;

   for (Uint32 i = 0; i < response->cimNamedInstances.size(); i++)
      response->cimNamedInstances[i].toXml(body);

   Array<Sint8> message = XmlWriter::formatSimpleIMethodRspMessage(
      "EnumerateInstances", response->messageId, body);

   sendResponse(response->queueIds.top(), message);
   PEG_FUNC_EXIT(TRC_DISPATCHER,"CIMOperationResponseEncoder::"
		 "encodeEnumerateInstanceResponse()");
}

void CIMOperationResponseEncoder::encodeEnumerateInstanceNamesResponse(
   CIMEnumerateInstanceNamesResponseMessage* response)
{
   PEG_FUNC_ENTER(TRC_DISPATCHER,"CIMOperationResponseEncoder::"
		  "encodeEnumerateInstanceNamesResponse()");

   if (response->errorCode != CIM_ERR_SUCCESS)
   {
      sendIMethodError(response, "EnumerateInstanceNames");
      PEG_FUNC_EXIT(TRC_DISPATCHER,"CIMOperationResponseEncoder::"
		    "encodeEnumerateInstanceNamesResponse()");
      return;
   }

   Array<Sint8> body;

   for (Uint32 i = 0; i < response->instanceNames.size(); i++)
      XmlWriter::appendInstanceNameElement(body, response->instanceNames[i]);

   Array<Sint8> message = XmlWriter::formatSimpleIMethodRspMessage(
      "EnumerateInstanceNames", response->messageId, body);

   sendResponse(response->queueIds.top(), message);
   PEG_FUNC_EXIT(TRC_DISPATCHER,"CIMOperationResponseEncoder::"
		 "encodeEnumerateInstanceNamesResponse()");
}

void CIMOperationResponseEncoder::encodeDeleteInstanceResponse(
   CIMDeleteInstanceResponseMessage* response)
{
   PEG_FUNC_ENTER(TRC_DISPATCHER,"CIMOperationResponseEncoder::"
		  "encodeDeleteInstanceResponse()");

   if (response->errorCode != CIM_ERR_SUCCESS)
   {
      sendIMethodError(response, "DeleteInstance");
      PEG_FUNC_EXIT(TRC_DISPATCHER,"CIMOperationResponseEncoder::"
		    "encodeDeleteInstanceResponse()");
      return;
   }

   Array<Sint8> body;

   Array<Sint8> message = XmlWriter::formatSimpleIMethodRspMessage(
      "DeleteInstance", response->messageId, body);

   sendResponse(response->queueIds.top(), message);
   PEG_FUNC_EXIT(TRC_DISPATCHER,"CIMOperationResponseEncoder::"
		 "encodeDeleteInstanceResponse()");
}

void CIMOperationResponseEncoder::encodeGetPropertyResponse(
   CIMGetPropertyResponseMessage* response)
{
   PEG_FUNC_ENTER(TRC_DISPATCHER,"CIMOperationResponseEncoder::"
		  "encodeGetPropertyResponse()");

   if (response->errorCode != CIM_ERR_SUCCESS)
   {
      sendIMethodError(response, "GetProperty");
      PEG_FUNC_EXIT(TRC_DISPATCHER,"CIMOperationResponseEncoder::"
		    "encodeGetPropertyResponse()");
      return;
   }

   Array<Sint8> body;
   response->value.toXml(body);

   Array<Sint8> message = XmlWriter::formatSimpleIMethodRspMessage(
      "GetProperty", response->messageId, body);

   sendResponse(response->queueIds.top(), message);
   PEG_FUNC_EXIT(TRC_DISPATCHER,"CIMOperationResponseEncoder::"
		 "encodeGetPropertyResponse()");
}

void CIMOperationResponseEncoder::encodeSetPropertyResponse(
   CIMSetPropertyResponseMessage* response)
{
   PEG_FUNC_ENTER(TRC_DISPATCHER,"CIMOperationResponseEncoder::"
		  "encodeSetPropertyResponse()");

   if (response->errorCode != CIM_ERR_SUCCESS)
   {
      sendIMethodError(response, "SetProperty");
      PEG_FUNC_EXIT(TRC_DISPATCHER,"CIMOperationResponseEncoder::"
		    "encodeSetPropertyResponse()");
      return;
   }

   Array<Sint8> body;

   Array<Sint8> message = XmlWriter::formatSimpleIMethodRspMessage(
      "SetProperty", response->messageId, body);

   sendResponse(response->queueIds.top(), message);
   PEG_FUNC_EXIT(TRC_DISPATCHER,"CIMOperationResponseEncoder::"
		 "encodeSetPropertyResponse()");
}

void CIMOperationResponseEncoder::encodeSetQualifierResponse(
   CIMSetQualifierResponseMessage* response)
{
   PEG_FUNC_ENTER(TRC_DISPATCHER,"CIMOperationResponseEncoder::"
		  "encodeSetQualifierResponse()");

   if (response->errorCode != CIM_ERR_SUCCESS)
   {
      sendIMethodError(response, "SetQualifier");
      PEG_FUNC_EXIT(TRC_DISPATCHER,"CIMOperationResponseEncoder::"
		    "encodeSetQualifierResponse()");
      return;
   }

   Array<Sint8> body;

   Array<Sint8> message = XmlWriter::formatSimpleIMethodRspMessage(
      "SetQualifier", response->messageId, body);

   sendResponse(response->queueIds.top(), message);
   PEG_FUNC_EXIT(TRC_DISPATCHER,"CIMOperationResponseEncoder::"
		 "encodeSetQualifierResponse()");
}

void CIMOperationResponseEncoder::encodeGetQualifierResponse(
   CIMGetQualifierResponseMessage* response)
{
   PEG_FUNC_ENTER(TRC_DISPATCHER,"CIMOperationResponseEncoder::"
		  "encodeGetQualifierResponse()");

   if (response->errorCode != CIM_ERR_SUCCESS)
   {
      sendIMethodError(response, "GetQualifier");
      PEG_FUNC_EXIT(TRC_DISPATCHER,"CIMOperationResponseEncoder::"
		    "encodeGetQualifierResponse()");
      return;
   }

   Array<Sint8> body;
   response->cimQualifierDecl.toXml(body);

   Array<Sint8> message = XmlWriter::formatSimpleIMethodRspMessage(
      "GetQualifier", response->messageId, body);

   sendResponse(response->queueIds.top(), message);
   PEG_FUNC_EXIT(TRC_DISPATCHER,"CIMOperationResponseEncoder::"
		 "encodeGetQualifierResponse()");
}

void CIMOperationResponseEncoder::encodeEnumerateQualifiersResponse(
   CIMEnumerateQualifiersResponseMessage* response)
{
   PEG_FUNC_ENTER(TRC_DISPATCHER,"CIMOperationResponseEncoder::"
		  "encodeEnumerateQualifierResponse()");

   if (response->errorCode != CIM_ERR_SUCCESS)
   {
      sendIMethodError(response, "EnumerateQualifiers");
      PEG_FUNC_EXIT(TRC_DISPATCHER,"CIMOperationResponseEncoder::"
		    "encodeEnumerateQualifierResponse()");
      return;
   }

   Array<Sint8> body;

   for (Uint32 i = 0; i < response->qualifierDeclarations.size(); i++)
      response->qualifierDeclarations[i].toXml(body);

   Array<Sint8> message = XmlWriter::formatSimpleIMethodRspMessage(
      "EnumerateQualifiers", response->messageId, body);

   sendResponse(response->queueIds.top(), message);
   PEG_FUNC_EXIT(TRC_DISPATCHER,"CIMOperationResponseEncoder::"
		 "encodeEnumerateQualifierResponse()");
}

void CIMOperationResponseEncoder::encodeDeleteQualifierResponse(
   CIMDeleteQualifierResponseMessage* response)
{
   PEG_FUNC_ENTER(TRC_DISPATCHER,"CIMOperationResponseEncoder::"
		  "encodeDeleteQualifierResponse()");

   if (response->errorCode != CIM_ERR_SUCCESS)
   {
      sendIMethodError(response, "DeleteQualifier");
      PEG_FUNC_EXIT(TRC_DISPATCHER,"CIMOperationResponseEncoder::"
		    "encodeDeleteQualifierResponse()");
      return;
   }

   Array<Sint8> body;

   Array<Sint8> message = XmlWriter::formatSimpleIMethodRspMessage(
      "DeleteQualifier", response->messageId, body);

   sendResponse(response->queueIds.top(), message);
   PEG_FUNC_EXIT(TRC_DISPATCHER,"CIMOperationResponseEncoder::"
		 "encodeDeleteQualifierResponse()");
}

void CIMOperationResponseEncoder::encodeReferenceNamesResponse(
   CIMReferenceNamesResponseMessage* response)
{
   PEG_FUNC_ENTER(TRC_DISPATCHER,"CIMOperationResponseEncoder::"
		  "encodeReferenceNamesResponse()");

   if (response->errorCode != CIM_ERR_SUCCESS)
   {
      sendIMethodError(response, "ReferenceNames");
      PEG_FUNC_EXIT(TRC_DISPATCHER,"CIMOperationResponseEncoder::"
		    "encodeReferenceNamesResponse()");
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
   PEG_FUNC_EXIT(TRC_DISPATCHER,"CIMOperationResponseEncoder::"
		 "encodeReferenceNamesResponse()");
}

void CIMOperationResponseEncoder::encodeReferencesResponse(
   CIMReferencesResponseMessage* response)
{
   PEG_FUNC_ENTER(TRC_DISPATCHER,"CIMOperationResponseEncoder::"
		  "encodeReferencesResponse()");

   if (response->errorCode != CIM_ERR_SUCCESS)
   {
      sendIMethodError(response, "References");
      PEG_FUNC_EXIT(TRC_DISPATCHER,"CIMOperationResponseEncoder::"
		    "encodeReferencesResponse()");
      return;
   }

   Array<Sint8> body;

   for (Uint32 i = 0; i < response->cimObjects.size(); i++)
      response->cimObjects[i].toXml(body);

   Array<Sint8> message = XmlWriter::formatSimpleIMethodRspMessage(
      "References", response->messageId, body);

   sendResponse(response->queueIds.top(), message);
   PEG_FUNC_EXIT(TRC_DISPATCHER,"CIMOperationResponseEncoder::"
		 "encodeReferencesResponse()");
}

void CIMOperationResponseEncoder::encodeAssociatorNamesResponse(
   CIMAssociatorNamesResponseMessage* response)
{
   PEG_FUNC_ENTER(TRC_DISPATCHER,"CIMOperationResponseEncoder::"
		  "encodeAssociatorNamesResponse()");

   if (response->errorCode != CIM_ERR_SUCCESS)
   {
      sendIMethodError(response, "AssociatorNames");
      PEG_FUNC_EXIT(TRC_DISPATCHER,"CIMOperationResponseEncoder::"
		    "encodeAssociatorNamesResponse()");
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
   PEG_FUNC_EXIT(TRC_DISPATCHER,"CIMOperationResponseEncoder::"
		 "encodeAssociatorNamesResponse()");
}

void CIMOperationResponseEncoder::encodeAssociatorsResponse(
   CIMAssociatorsResponseMessage* response)
{
   PEG_FUNC_ENTER(TRC_DISPATCHER,"CIMOperationResponseEncoder::"
		  "encodeAssociatorsResponse()");

   if (response->errorCode != CIM_ERR_SUCCESS)
   {
      sendIMethodError(response, "Associators");
      PEG_FUNC_EXIT(TRC_DISPATCHER,"CIMOperationResponseEncoder::"
		    "encodeAssociatorsResponse()");
      return;
   }

   Array<Sint8> body;

   for (Uint32 i = 0; i < response->cimObjects.size(); i++)
      response->cimObjects[i].toXml(body);

   Array<Sint8> message = XmlWriter::formatSimpleIMethodRspMessage(
      "Associators", response->messageId, body);

   sendResponse(response->queueIds.top(), message);
   PEG_FUNC_EXIT(TRC_DISPATCHER,"CIMOperationResponseEncoder::"
		 "encodeAssociatorsResponse()");
}

void CIMOperationResponseEncoder::encodeInvokeMethodResponse(
   CIMInvokeMethodResponseMessage* response)
{
   PEG_FUNC_ENTER(TRC_DISPATCHER,"CIMOperationResponseEncoder::"
		  "encodeInvokeMethodResponse()");

   if (response->errorCode != CIM_ERR_SUCCESS)
   {
      sendMethodError(response, response->methodName);
      PEG_FUNC_EXIT(TRC_DISPATCHER,"CIMOperationResponseEncoder::"
		    "encodeInvokeMethodResponse()");
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
   PEG_FUNC_EXIT(TRC_DISPATCHER,"CIMOperationResponseEncoder::"
		 "encodeInvokeMethodResponse()");
}

PEGASUS_NAMESPACE_END
