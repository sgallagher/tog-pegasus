//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software, Hewlett-Packard Company, IBM,
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
//              Arthur Pichlkostner (via Markus: sedgewick_de@yahoo.de)
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                (carolann_graves@hp.com)
//              Sushma Fernandes , Hewlett-Packard Company
//                (sushma_fernandes@hp.com)
//              Dave Rosckes (rosckes@us.ibm.com)
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
#include <Pegasus/Common/StatisticalData.h>
#include "CIMOperationResponseEncoder.h"

#ifdef PEGASUS_CCOVER
# include <ccover.h>
#endif

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

const String CIMOperationResponseEncoder::OUT_OF_MEMORY_MESSAGE  =
   "A System error has occured. Please retry the CIM Operation at a later time.";

CIMOperationResponseEncoder::CIMOperationResponseEncoder()
   : Base(PEGASUS_QUEUENAME_OPRESPENCODER)
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

      Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
		  "CIMOperationResponseEncoder::SendResponse - QueueId: $0  XML content: $1",
		  queue,
		  httpMessage->message.getData());

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
   HttpMethod httpMethod,
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
        httpMethod,
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
      response->getHttpMethod(),
      response->messageId, 
      cimMethodName, 
      response->cimException); 

   PEG_METHOD_EXIT();
}

void CIMOperationResponseEncoder::sendMethodError(
   Uint32 queueId, 
   HttpMethod httpMethod,
   const String& messageId,
   const CIMName& methodName,
   const CIMException& cimException)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
                     "CIMOperationResponseEncoder::sendMethodError()");

    Array<Sint8> message;
    message = XmlWriter::formatSimpleMethodErrorRspMessage(
        methodName,
        messageId,
        httpMethod,
        cimException);

    sendResponse(queueId, message);

    PEG_METHOD_EXIT();
}

void CIMOperationResponseEncoder::sendMethodError(
   CIMResponseMessage* response,
   const CIMName& cimMethodName)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
                    "CIMOperationResponseEncoder::sendMethodError()");

   Uint32 queueId = response->queueIds.top();
   response->queueIds.pop();

   sendMethodError(
      queueId,
      response->getHttpMethod(),
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

void CIMOperationResponseEncoder::encodeCreateClassResponse(
   CIMCreateClassResponseMessage* response)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
                    "CIMOperationResponseEncoder::encodeCreateClassResponse()");

   if (response->cimException.getCode() != CIM_ERR_SUCCESS)
   {
      STAT_SERVEREND_ERROR

      sendIMethodError(response, "CreateClass");
      PEG_METHOD_EXIT();
      return;
   }

   Array<Sint8> body;

   Array<Sint8> message = XmlWriter::formatSimpleIMethodRspMessage(
      CIMName ("CreateClass"), response->messageId, 
      response->getHttpMethod(), body);

   STAT_SERVEREND

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
      STAT_SERVEREND_ERROR

      sendIMethodError(response, "GetClass");
      PEG_METHOD_EXIT();
      return;
   }

   Array<Sint8> body;
   XmlWriter::appendClassElement(body, response->cimClass);

   Array<Sint8> message = XmlWriter::formatSimpleIMethodRspMessage(
      CIMName ("GetClass"), response->messageId, 
      response->getHttpMethod(), body);

   STAT_SERVEREND

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
      STAT_SERVEREND_ERROR

      sendIMethodError(response, "ModifyClass");
      PEG_METHOD_EXIT();
      return;
   }

   Array<Sint8> body;

   Array<Sint8> message = XmlWriter::formatSimpleIMethodRspMessage(
      CIMName ("ModifyClass"), response->messageId, 
      response->getHttpMethod(), body);

   STAT_SERVEREND

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
      STAT_SERVEREND_ERROR

      sendIMethodError(response, "EnumerateClassNames");
      PEG_METHOD_EXIT();
      return;
   }

   Array<Sint8> body;

   for (Uint32 i = 0; i < response->classNames.size(); i++)
      XmlWriter::appendClassNameElement(body, response->classNames[i]);

   Array<Sint8> message = XmlWriter::formatSimpleIMethodRspMessage(
      CIMName ("EnumerateClassNames"), response->messageId, 
      response->getHttpMethod(), body);

   STAT_SERVEREND

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
      STAT_SERVEREND_ERROR

      sendIMethodError(response, "EnumerateClasses");
      PEG_METHOD_EXIT();
      return;
   }

   Array<Sint8> body;
   Array<Sint8> message;

   try
   {
       for (Uint32 i = 0; i < response->cimClasses.size(); i++)
          XmlWriter::appendClassElement(body, response->cimClasses[i]);

       message = XmlWriter::formatSimpleIMethodRspMessage(
          CIMName ("EnumerateClasses"), response->messageId, 
          response->getHttpMethod(), body);

       STAT_SERVEREND
   }
   // This operation may result in a large response. Handle the bad_alloc
   // exception and send an error response.

#ifdef PEGASUS_PLATFORM_WIN32_IX86_MSVC
   catch (std::bad_alloc& be)
#else
   catch (bad_alloc& be)
#endif
   {
       // ATTN-SF-P5-20021004 A message should be logged here
       // indicating the out of memory message.
       CIMException cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
                                      OUT_OF_MEMORY_MESSAGE);
       response->cimException = cimException;
       sendIMethodError(response, "EnumerateClasses");
       PEG_METHOD_EXIT();
       return;
   }

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
      STAT_SERVEREND_ERROR

      sendIMethodError(response, "DeleteClass");
      PEG_METHOD_EXIT();
      return;
   }

   Array<Sint8> body;

   Array<Sint8> message = XmlWriter::formatSimpleIMethodRspMessage(
      CIMName ("DeleteClass"), response->messageId, 
      response->getHttpMethod(), body);

   STAT_SERVEREND

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
      STAT_SERVEREND_ERROR

      sendIMethodError(response, "CreateInstance");
      PEG_METHOD_EXIT();
      return;
   }

   Array<Sint8> body;

   XmlWriter::appendInstanceNameElement(body, response->instanceName);

   Array<Sint8> message = XmlWriter::formatSimpleIMethodRspMessage(
      CIMName ("CreateInstance"), response->messageId, 
      response->getHttpMethod(), body);

   STAT_SERVEREND

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
      STAT_SERVEREND_ERROR

      sendIMethodError(response, "GetInstance");
      PEG_METHOD_EXIT();
      return;
   }

   Array<Sint8> body;
   XmlWriter::appendInstanceElement(body, response->cimInstance);

   Array<Sint8> message = XmlWriter::formatSimpleIMethodRspMessage(
      CIMName ("GetInstance"), response->messageId, 
      response->getHttpMethod(), body);

   STAT_SERVEREND

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
      STAT_SERVEREND_ERROR

      sendIMethodError(response, "ModifyInstance");
      PEG_METHOD_EXIT();
      return;
   }

   Array<Sint8> body;

   Array<Sint8> message = XmlWriter::formatSimpleIMethodRspMessage(
      CIMName ("ModifyInstance"), response->messageId, 
      response->getHttpMethod(), body);

   STAT_SERVEREND

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
      STAT_SERVEREND_ERROR

      sendIMethodError(response, "EnumerateInstances");
      PEG_METHOD_EXIT();
      return;
   }

   Array<Sint8> body;
   Array<Sint8> message;

   try
   {
       for (Uint32 i = 0; i < response->cimNamedInstances.size(); i++)
          XmlWriter::appendValueNamedInstanceElement(
              body, response->cimNamedInstances[i]);

       message = XmlWriter::formatSimpleIMethodRspMessage(
          CIMName ("EnumerateInstances"), response->messageId, 
          response->getHttpMethod(), body);

   STAT_SERVEREND
   }
   // This operation may result in a large response. Handle the bad_alloc
   // exception and send an error response.

#ifdef PEGASUS_PLATFORM_WIN32_IX86_MSVC
   catch (std::bad_alloc& be)
#else
   catch (bad_alloc& be)
#endif
   {
       // ATTN-SF-P5-20021004 A message should be logged here
       // indicating the out of memory message.
       CIMException cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
                                      OUT_OF_MEMORY_MESSAGE);
       response->cimException = cimException;
       sendIMethodError(response, "EnumerateInstances");
       PEG_METHOD_EXIT();
       return;
   }

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
      STAT_SERVEREND_ERROR

      sendIMethodError(response, "EnumerateInstanceNames");
      PEG_METHOD_EXIT();
      return;
   }

   Array<Sint8> body;

   for (Uint32 i = 0; i < response->instanceNames.size(); i++)
      XmlWriter::appendInstanceNameElement(body, response->instanceNames[i]);

   Array<Sint8> message = XmlWriter::formatSimpleIMethodRspMessage(
      CIMName ("EnumerateInstanceNames"), response->messageId, 
      response->getHttpMethod(), body);

   STAT_SERVEREND

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
      STAT_SERVEREND_ERROR

      sendIMethodError(response, "DeleteInstance");
      PEG_METHOD_EXIT();
      return;
   }

   Array<Sint8> body;

   Array<Sint8> message = XmlWriter::formatSimpleIMethodRspMessage(
      CIMName ("DeleteInstance"), response->messageId, 
      response->getHttpMethod(), body);

   STAT_SERVEREND

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
      STAT_SERVEREND_ERROR

      sendIMethodError(response, "GetProperty");
      PEG_METHOD_EXIT();
      return;
   }

   Array<Sint8> body;
   XmlWriter::appendValueElement(body, response->value);

   Array<Sint8> message = XmlWriter::formatSimpleIMethodRspMessage(
      CIMName ("GetProperty"), response->messageId, 
      response->getHttpMethod(), body);

   STAT_SERVEREND

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
      STAT_SERVEREND_ERROR

      sendIMethodError(response, "SetProperty");
      PEG_METHOD_EXIT();
      return;
   }

   Array<Sint8> body;

   Array<Sint8> message = XmlWriter::formatSimpleIMethodRspMessage(
      CIMName ("SetProperty"), response->messageId, 
      response->getHttpMethod(), body);

   STAT_SERVEREND

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
      STAT_SERVEREND_ERROR

      sendIMethodError(response, "SetQualifier");
      PEG_METHOD_EXIT();
      return;
   }

   Array<Sint8> body;

   Array<Sint8> message = XmlWriter::formatSimpleIMethodRspMessage(
      CIMName ("SetQualifier"), response->messageId, 
      response->getHttpMethod(), body);

   STAT_SERVEREND

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
      STAT_SERVEREND_ERROR

      sendIMethodError(response, "GetQualifier");
      PEG_METHOD_EXIT();
      return;
   }

   Array<Sint8> body;
   XmlWriter::appendQualifierDeclElement(body, response->cimQualifierDecl);

   Array<Sint8> message = XmlWriter::formatSimpleIMethodRspMessage(
      CIMName ("GetQualifier"), response->messageId, 
      response->getHttpMethod(), body);

   STAT_SERVEREND

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
      STAT_SERVEREND_ERROR

      sendIMethodError(response, "EnumerateQualifiers");
      PEG_METHOD_EXIT();
      return;
   }

   Array<Sint8> body;

   for (Uint32 i = 0; i < response->qualifierDeclarations.size(); i++)
      XmlWriter::appendQualifierDeclElement(body, response->qualifierDeclarations[i]);

   Array<Sint8> message = XmlWriter::formatSimpleIMethodRspMessage(
      CIMName ("EnumerateQualifiers"), response->messageId, 
      response->getHttpMethod(), body);

   STAT_SERVEREND

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
      STAT_SERVEREND_ERROR

      sendIMethodError(response, "DeleteQualifier");
      PEG_METHOD_EXIT();
      return;
   }

   Array<Sint8> body;

   Array<Sint8> message = XmlWriter::formatSimpleIMethodRspMessage(
      CIMName ("DeleteQualifier"), response->messageId, 
      response->getHttpMethod(), body);

   STAT_SERVEREND

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
      STAT_SERVEREND_ERROR

      sendIMethodError(response, "ReferenceNames");
      PEG_METHOD_EXIT();
      return;
   }

   Array<Sint8> body;

   for (Uint32 i = 0; i < response->objectNames.size(); i++)
   {
      body << "<OBJECTPATH>\n";
      XmlWriter::appendValueReferenceElement(body, response->objectNames[i], false);
      body << "</OBJECTPATH>\n";
   }

   Array<Sint8> message = XmlWriter::formatSimpleIMethodRspMessage(
      CIMName ("ReferenceNames"), response->messageId, 
      response->getHttpMethod(), body);

   STAT_SERVEREND

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
      STAT_SERVEREND_ERROR

      sendIMethodError(response, "References");
      PEG_METHOD_EXIT();
      return;
   }

   Array<Sint8> body;

   for (Uint32 i = 0; i < response->cimObjects.size(); i++)
      XmlWriter::appendValueObjectWithPathElement(body, response->cimObjects[i]);

   Array<Sint8> message = XmlWriter::formatSimpleIMethodRspMessage(
      CIMName ("References"), response->messageId, 
      response->getHttpMethod(), body);

   STAT_SERVEREND

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
      STAT_SERVEREND_ERROR

      sendIMethodError(response, "AssociatorNames");
      PEG_METHOD_EXIT();
      return;
   }

   Array<Sint8> body;

   for (Uint32 i = 0; i < response->objectNames.size(); i++)
   {
      body << "<OBJECTPATH>\n";
      XmlWriter::appendValueReferenceElement(body, response->objectNames[i], false);
      body << "</OBJECTPATH>\n";
   }

   Array<Sint8> message = XmlWriter::formatSimpleIMethodRspMessage(
      CIMName ("AssociatorNames"), response->messageId, 
      response->getHttpMethod(), body);

   STAT_SERVEREND

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
      STAT_SERVEREND_ERROR

      sendIMethodError(response, "Associators");
      PEG_METHOD_EXIT();
      return;
   }

   Array<Sint8> body;

   for (Uint32 i = 0; i < response->cimObjects.size(); i++)
      XmlWriter::appendValueObjectWithPathElement(body, response->cimObjects[i]);

   Array<Sint8> message = XmlWriter::formatSimpleIMethodRspMessage(
      CIMName ("Associators"), response->messageId, 
      response->getHttpMethod(), body);

   STAT_SERVEREND

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
      STAT_SERVEREND_ERROR

      sendIMethodError(response, "ExecQuery");
      PEG_METHOD_EXIT();
      return;
   }

   Array<Sint8> body;

   for (Uint32 i = 0; i < response->cimObjects.size(); i++)
      XmlWriter::appendValueObjectWithPathElement(body, response->cimObjects[i]);

   Array<Sint8> message = XmlWriter::formatSimpleIMethodRspMessage(
      CIMName ("ExecQuery"), response->messageId, 
      response->getHttpMethod(), body);

   STAT_SERVEREND

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
      STAT_SERVEREND_ERROR

      sendMethodError(response, response->methodName);
      PEG_METHOD_EXIT();
      return;
   }

   Array<Sint8> body;

   // ATTN-RK-P3-20020219: Who's job is it to make sure the return value is
   // not an array?
   // Only add the return value if it is not null
   if (!response->retValue.isNull())
   {
      XmlWriter::appendReturnValueElement(body, response->retValue);
   }

   for (Uint32 i=0; i < response->outParameters.size(); i++)
   {
      XmlWriter::appendParamValueElement(body, response->outParameters[i]);
   }

   Array<Sint8> message = XmlWriter::formatSimpleMethodRspMessage(
       response->methodName, response->messageId, 
       response->getHttpMethod(), body);

   STAT_SERVEREND

   sendResponse(response->queueIds.top(), message);
   PEG_METHOD_EXIT();
}

PEGASUS_NAMESPACE_END
