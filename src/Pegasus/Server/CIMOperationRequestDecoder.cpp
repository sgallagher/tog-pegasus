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
// Modified By: Yi Zhou, Hewlett-Packard Company (yi_zhou@hp.com)
//              Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//              Nag Boranna, Hewlett-Packard Company (nagaraja_boranna@hp.com)
//              Jenny Yu, Hewlett-Packard Company (jenny_yu@hp.com)
//              Sushma Fernandes, Hewlett-Packard Company 
//              (sushma_fernandes@hp.com)
//              Arthur Pichlkostner (via Markus: sedgewick_de@yahoo.de)
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                  (carolann_graves@hp.com)
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
#include <Pegasus/Common/XmlConstants.h>
#include <Pegasus/Common/Logger.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/StatisticalData.h>
#include "CIMOperationRequestDecoder.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

CIMOperationRequestDecoder::CIMOperationRequestDecoder(
   MessageQueueService* outputQueue,
   Uint32 returnQueueId)
   :  Base(PEGASUS_QUEUENAME_OPREQDECODER),
      _outputQueue(outputQueue),
      _returnQueueId(returnQueueId),
      _serverTerminating(false)

{
}

CIMOperationRequestDecoder::~CIMOperationRequestDecoder()
{

}

void CIMOperationRequestDecoder::sendResponse(
   Uint32 queueId, 
   Array<Sint8>& message)
{
   MessageQueue* queue = MessageQueue::lookup(queueId);

   if (queue)
   {
      HTTPMessage* httpMessage = new HTTPMessage(message);
      queue->enqueue(httpMessage);
   }
}

void CIMOperationRequestDecoder::sendIMethodError(
   Uint32 queueId, 
   const String& messageId,
   const String& iMethodName,
   const CIMException& cimException)
{
    Array<Sint8> message;
    message = XmlWriter::formatSimpleIMethodErrorRspMessage(
        iMethodName,
        messageId,
        cimException);

    sendResponse(queueId, message);
}

void CIMOperationRequestDecoder::sendMethodError(
   Uint32 queueId, 
   const String& messageId,
   const String& methodName,
   const CIMException& cimException)
{
    Array<Sint8> message;
    message = XmlWriter::formatSimpleMethodErrorRspMessage(
        methodName,
        messageId,
        cimException);
    
    sendResponse(queueId, message);
}

void CIMOperationRequestDecoder::sendHttpError(
   Uint32 queueId, 
   const String& status,
   const String& cimError,
   const String& pegasusError) 
{
    Array<Sint8> message;
    message = XmlWriter::formatHttpErrorRspMessage(
        status,
        cimError,
        pegasusError);
    
    sendResponse(queueId, message);
}

void CIMOperationRequestDecoder::handleEnqueue(Message *message)
{

   if (!message)
      return;

   switch (message->getType())
   {
      case HTTP_MESSAGE:
	 handleHTTPMessage((HTTPMessage*)message);
	 break;
   }

   delete message;
}


void CIMOperationRequestDecoder::handleEnqueue()
{
   Message* message = dequeue();
   if(message)
      handleEnqueue(message);
}

//------------------------------------------------------------------------------
//
// From the HTTP/1.1 Specification (RFC 2626):
//
// Both types of message consist of a start-line, zero or more header fields 
// (also known as "headers"), an empty line (i.e., a line with nothing 
// preceding the CRLF) indicating the end of the header fields, and possibly 
// a message-body.
//
// Example CIM request:
//
//     M-POST /cimom HTTP/1.1 
//     HOST: www.erewhon.com 
//     Content-Type: application/xml; charset="utf-8" 
//     Content-Length: xxxx 
//     Man: http://www.dmtf.org/cim/operation ; ns=73 
//     73-CIMOperation: MethodCall 
//     73-CIMMethod: EnumerateInstances 
//     73-CIMObject: root/cimv2 
// 
//------------------------------------------------------------------------------

void CIMOperationRequestDecoder::handleHTTPMessage(HTTPMessage* httpMessage)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDecoder::handleHTTPMessage()");

   // Save queueId:

   Uint32 queueId = httpMessage->queueId;

   // Save userName and authType:

   String userName;
   String authType = String::EMPTY;

   if ( httpMessage->authInfo->isAuthenticated() )
   {
      userName = httpMessage->authInfo->getAuthenticatedUser();
      authType = httpMessage->authInfo->getAuthType();
   }

   // Parse the HTTP message:

   String startLine;
   Array<HTTPHeader> headers;
   Sint8* content;
   Uint32 contentLength;

   httpMessage->parse(startLine, headers, contentLength);

   // Parse the request line:

   String methodName;
   String requestUri;
   String httpVersion;

   Tracer::trace(TRC_XML_IO, Tracer::LEVEL2, "%s",
		 httpMessage->message.getData());

   HTTPMessage::parseRequestLine(
      startLine, methodName, requestUri, httpVersion);

   // Unsupported methods are caught in the HTTPAuthenticatorDelegator
   PEGASUS_ASSERT(methodName == "M-POST" || methodName == "POST");

   // Process M-POST and POST messages:

   String cimOperation;
   String cimBatch;
   Boolean cimBatchFlag;
   String cimProtocolVersion;
   String cimMethod;
   String cimObject;

   // Validate the "CIMOperation" header:

   Boolean operationHeaderFound = HTTPMessage::lookupHeader(
      headers, "CIMOperation", cimOperation, true);
   // If the CIMOperation header was missing, the HTTPAuthenticatorDelegator
   // would not have passed the message to us.
   PEGASUS_ASSERT(operationHeaderFound);

   if (!String::equalNoCase(cimOperation, "MethodCall"))
   {
      // The Specification for CIM Operations over HTTP reads:
      //     3.3.4. CIMOperation
      //     If a CIM Server receives CIM Operation request with this
      //     [CIMOperation] header, but with a missing value or a value
      //     that is not "MethodCall", then it MUST fail the request with
      //     status "400 Bad Request". The CIM Server MUST include a
      //     CIMError header in the response with a value of
      //     unsupported-operation.
      sendHttpError(queueId, HTTP_STATUS_BADREQUEST, "unsupported-operation",
                    String("CIMOperation value \"") + cimOperation +
                        "\" is not supported.");
      PEG_METHOD_EXIT();
      return;
   }

   // Validate the "CIMBatch" header:

   cimBatchFlag = HTTPMessage::lookupHeader(
       headers, "CIMBatch", cimBatch, true);
   if (cimBatchFlag)
   {
      // The Specification for CIM Operations over HTTP reads:
      //     3.3.9. CIMBatch
      //     If a CIM Server receives CIM Operation Request for which the
      //     CIMBatch header is present, but the Server does not support
      //     Multiple Operations, then it MUST fail the request and
      //     return a status of "501 Not Implemented".
      sendHttpError(queueId,
                    HTTP_STATUS_NOTIMPLEMENTED,
                    "multiple-requests-unsupported");
      PEG_METHOD_EXIT();
      return;
   }

   // Save these headers for later checking

   if (!HTTPMessage::lookupHeader(
       headers, "CIMProtocolVersion", cimProtocolVersion, true))
   {
      // Mandated by the Specification for CIM Operations over HTTP
      cimProtocolVersion.assign("1.0");
   }

   if (HTTPMessage::lookupHeader(headers, "CIMMethod", cimMethod, true))
   {
      if (cimMethod == String::EMPTY)
      {
         // This is not a valid value, and we use EMPTY to mean "absent"
         sendHttpError(queueId, HTTP_STATUS_BADREQUEST, "header-mismatch",
                       "Empty CIMMethod value.");
         PEG_METHOD_EXIT();
         return;
      }
   }
   if (HTTPMessage::lookupHeader(headers, "CIMObject", cimObject, true))
   {
      if (cimObject == String::EMPTY)
      {
         // This is not a valid value, and we use EMPTY to mean "absent"
         sendHttpError(queueId, HTTP_STATUS_BADREQUEST, "header-mismatch",
                       "Empty CIMObject value.");
         PEG_METHOD_EXIT();
         return;
      }
   }

   // Zero-terminate the message:

   httpMessage->message.append('\0');

   // Calculate the beginning of the content from the message size and
   // the content length.  Subtract 1 to take into account the null
   // character we just added to the end of the message.

   content = (Sint8*) httpMessage->message.getData() +
   httpMessage->message.size() - contentLength - 1;

   // If it is a method call, then dispatch it to be handled:

   handleMethodCall(queueId, content, contentLength, 
                    cimProtocolVersion, cimMethod,
                    cimObject, authType, userName);
    
   PEG_METHOD_EXIT();
}


void CIMOperationRequestDecoder::handleMethodCall(
   Uint32 queueId,
   Sint8* content,
   Uint32 contentLength,    // used for statistics only
   const String& cimProtocolVersionInHeader,
   const String& cimMethodInHeader,
   const String& cimObjectInHeader,
   String authType,
   String userName)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDecoder::handleMethodCall()");

   //
   // If CIMOM is shutting down, return "Service Unavailable" response
   //
   if (_serverTerminating)
   {
       sendHttpError(queueId, HTTP_STATUS_SERVICEUNAVAILABLE,
                     String::EMPTY,
                     "CIM Server is shutting down.");
       PEG_METHOD_EXIT();
       return;
   }

   // Create a parser:

   XmlParser parser(content);
   XmlEntry entry;
   String messageId;
   const char* cimMethodName = "";
   Message* request;

   try
   {
      //
      // Process <?xml ... >
      //

      // These values are currently unused
      const char* xmlVersion = 0;
      const char* xmlEncoding = 0;

      XmlReader::getXmlDeclaration(parser, xmlVersion, xmlEncoding);

      // Expect <CIM ...>

      const char* cimVersion = 0;
      const char* dtdVersion = 0;

      XmlReader::getCimStartTag(parser, cimVersion, dtdVersion);

      if (strcmp(cimVersion, "2.0") != 0)
      {
         sendHttpError(queueId,
                       HTTP_STATUS_NOTIMPLEMENTED,
                       "unsupported-cim-version",
                       String("CIM version \"") + cimVersion +
                           "\" is not supported.");
         PEG_METHOD_EXIT();
         return;
      }

      if (strcmp(dtdVersion, "2.0") != 0)
      {
         sendHttpError(queueId,
                       HTTP_STATUS_NOTIMPLEMENTED,
                       "unsupported-dtd-version",
                       String("DTD version \"") + dtdVersion +
                           "\" is not supported.");
         PEG_METHOD_EXIT();
         return;
      }

      // Expect <MESSAGE ...>

      String protocolVersion;

      if (!XmlReader::getMessageStartTag(
	     parser, messageId, protocolVersion))
      {
	 throw XmlValidationError(
	    parser.getLine(), "expected MESSAGE element");
      }

      // Validate that the protocol version in the header matches the XML

      if (!String::equalNoCase(protocolVersion, cimProtocolVersionInHeader))
      {
         sendHttpError(queueId, HTTP_STATUS_BADREQUEST, "header-mismatch",
                       String("CIMProtocolVersion value \"") +
                           cimProtocolVersionInHeader + "\" does not " +
                           "match CIM request protocol version \"" +
                           protocolVersion + "\".");
         PEG_METHOD_EXIT();
         return;
      }

      // We only support protocol version 1.0

      if (!String::equalNoCase(protocolVersion, "1.0"))
      {
         // See Specification for CIM Operations over HTTP section 4.3
         sendHttpError(queueId,
                       HTTP_STATUS_NOTIMPLEMENTED,
                       "unsupported-protocol-version",
                       String("CIMProtocolVersion \"") + protocolVersion +
                           "\" is not supported.");
         PEG_METHOD_EXIT();
         return;
      }

      if (XmlReader::testStartTag(parser, entry, "MULTIREQ"))
      {
         // We wouldn't have gotten here if CIMBatch header was specified,
         // so this must be indicative of a header mismatch
         sendHttpError(queueId, HTTP_STATUS_BADREQUEST, "header-mismatch",
                       "Multi-request is missing CIMBatch HTTP header");
         PEG_METHOD_EXIT();
         return;
         // Future: When MULTIREQ is supported, must ensure CIMMethod and
         // CIMObject headers are absent, and CIMBatch header is present.
      }

      // Expect <SIMPLEREQ ...>

      XmlReader::expectStartTag(parser, entry, "SIMPLEREQ");

      // Check for <IMETHODCALL ...>

      if (XmlReader::getIMethodCallStartTag(parser, cimMethodName))
      {
         // The Specification for CIM Operations over HTTP reads:
         //     3.3.6. CIMMethod
         //
         //     This header MUST be present in any CIM Operation Request
         //     message that contains a Simple Operation Request.  
         //
         //     It MUST NOT be present in any CIM Operation Response message,
         //     nor in any CIM Operation Request message that is not a
         //     Simple Operation Request.
         //
         //     The name of the CIM method within a Simple Operation Request
         //     is defined to be the value of the NAME attribute of the
         //     <METHODCALL> or <IMETHODCALL> element.
         //
         //     If a CIM Server receives a CIM Operation Request for which
         //     either:
         //
         //     - The CIMMethod header is present but has an invalid value, or;
         //     - The CIMMethod header is not present but the Operation
         //       Request Message is a Simple Operation Request, or; 
         //     - The CIMMethod header is present but the Operation Request
         //       Message is not a Simple Operation Request, or; 
         //     - The CIMMethod header is present, the Operation Request
         //       Message is a Simple Operation Request, but the CIMIdentifier
         //       value (when unencoded) does not match the unique method name
         //       within the Simple Operation Request,
         //
         //     then it MUST fail the request and return a status of
         //     "400 Bad Request" (and MUST include a CIMError header in the
         //     response with a value of header-mismatch), subject to the
         //     considerations specified in Errors.
         if (!String::equalNoCase(cimMethodName, cimMethodInHeader))
         {
            // ATTN-RK-P3-20020304: How to decode cimMethodInHeader?
            if (cimMethodInHeader == String::EMPTY)
            {
               sendHttpError(queueId, HTTP_STATUS_BADREQUEST, "header-mismatch",
                             "Missing CIMMethod HTTP header.");
            }
            else
            {
               sendHttpError(queueId, HTTP_STATUS_BADREQUEST, "header-mismatch",
                             String("CIMMethod value \"") + cimMethodInHeader +
                                 "\" does not match CIM request method \"" +
                                 cimMethodName + "\".");
            }
            PEG_METHOD_EXIT();
            return;
         }

	 // Expect <LOCALNAMESPACEPATH ...>

	 String nameSpace;

	 if (!XmlReader::getLocalNameSpacePathElement(parser, nameSpace))
	 {
	    throw XmlValidationError(parser.getLine(), 
				     "expected LOCALNAMESPACEPATH element");
	 }

         // The Specification for CIM Operations over HTTP reads:
         //     3.3.7. CIMObject
         //
         //     This header MUST be present in any CIM Operation Request
         //     message that contains a Simple Operation Request.  
         //
         //     It MUST NOT be present in any CIM Operation Response message,
         //     nor in any CIM Operation Request message that is not a
         //     Simple Operation Request.
         //
         //     The header identifies the CIM object (which MUST be a Class
         //     or Instance for an extrinsic method, or a Namespace for an
         //     intrinsic method) on which the method is to be invoked, using
         //     a CIM object path encoded in an HTTP-safe representation.
         //
         //     If a CIM Server receives a CIM Operation Request for which
         //     either:
         //
         //     - The CIMObject header is present but has an invalid value, or;
         //     - The CIMObject header is not present but the Operation
         //       Request Message is a Simple Operation Request, or; 
         //     - The CIMObject header is present but the Operation Request
         //       Message is not a Simple Operation Request, or; 
         //     - The CIMObject header is present, the Operation Request
         //       Message is a Simple Operation Request, but the ObjectPath
         //       value does not match (where match is defined in the section
         //       section on Encoding CIM Object Paths) the Operation Request
         //       Message,
         //
         //     then it MUST fail the request and return a status of
         //     "400 Bad Request" (and MUST include a CIMError header in the
         //     response with a value of header-mismatch), subject to the
         //     considerations specified in Errors.
         if (!String::equalNoCase(nameSpace, cimObjectInHeader))
         {
            if (cimObjectInHeader == String::EMPTY)
            {
               sendHttpError(queueId, HTTP_STATUS_BADREQUEST, "header-mismatch",
                             "Missing CIMObject HTTP header.");
            }
            else
            {
               sendHttpError(queueId, HTTP_STATUS_BADREQUEST, "header-mismatch",
                             String("CIMObject value \"") + cimObjectInHeader +
                                 "\" does not match CIM request object \"" +
                                 nameSpace + "\".");
            }
            PEG_METHOD_EXIT();
            return;
         }

         // This try block only catches CIMExceptions, because they must be
         // responded to with a proper IMETHODRESPONSE.  Other exceptions are
         // caught in the outer try block.
         try
         {
	    // Delegate to appropriate method to handle:

            if (CompareNoCase(cimMethodName, "GetClass") == 0)
               request = decodeGetClassRequest(
                  queueId, parser, messageId, nameSpace, authType, userName);
            else if (CompareNoCase(cimMethodName, "GetInstance") == 0)
               request = decodeGetInstanceRequest(
                  queueId, parser, messageId, nameSpace, authType, userName);
            else if (CompareNoCase(cimMethodName, "EnumerateClassNames") == 0)
               request = decodeEnumerateClassNamesRequest(
                  queueId, parser, messageId, nameSpace, authType, userName);
            else if (CompareNoCase(cimMethodName, "References") == 0)
               request = decodeReferencesRequest(
                  queueId, parser, messageId, nameSpace, authType, userName);
            else if (CompareNoCase(cimMethodName, "ReferenceNames") == 0)
               request = decodeReferenceNamesRequest(
                  queueId, parser, messageId, nameSpace, authType, userName);
            else if (CompareNoCase(cimMethodName, "AssociatorNames") == 0)
               request = decodeAssociatorNamesRequest(
                  queueId, parser, messageId, nameSpace, authType, userName);
            else if (CompareNoCase(cimMethodName, "Associators") == 0)
               request = decodeAssociatorsRequest(
                  queueId, parser, messageId, nameSpace, authType, userName);
            else if (CompareNoCase(cimMethodName, "CreateInstance") == 0)
               request = decodeCreateInstanceRequest(
                  queueId, parser, messageId, nameSpace, authType, userName);
            else if (CompareNoCase(cimMethodName, "EnumerateInstanceNames")==0)
               request = decodeEnumerateInstanceNamesRequest(
                  queueId, parser, messageId, nameSpace, authType, userName);
            else if (CompareNoCase(cimMethodName, "DeleteQualifier") == 0)
               request = decodeDeleteQualifierRequest(
                  queueId, parser, messageId, nameSpace, authType, userName);
            else if (CompareNoCase(cimMethodName, "GetQualifier") == 0)
               request = decodeGetQualifierRequest(
                  queueId, parser, messageId, nameSpace, authType, userName);
            else if (CompareNoCase(cimMethodName, "SetQualifier") == 0)
               request = decodeSetQualifierRequest(
                  queueId, parser, messageId, nameSpace, authType, userName);
            else if (CompareNoCase(cimMethodName, "EnumerateQualifiers") == 0)
               request = decodeEnumerateQualifiersRequest(
                  queueId, parser, messageId, nameSpace, authType, userName);
            else if (CompareNoCase(cimMethodName, "EnumerateClasses") == 0)
               request = decodeEnumerateClassesRequest(
                  queueId, parser, messageId, nameSpace, authType, userName);
            else if (CompareNoCase(cimMethodName, "EnumerateInstances") == 0)
               request = decodeEnumerateInstancesRequest(
                  queueId, parser, messageId, nameSpace, authType, userName);
            else if (CompareNoCase(cimMethodName, "CreateClass") == 0)
               request = decodeCreateClassRequest(
                  queueId, parser, messageId, nameSpace, authType, userName);
            else if (CompareNoCase(cimMethodName, "ModifyClass") == 0)
               request = decodeModifyClassRequest(
                  queueId, parser, messageId, nameSpace, authType, userName);
            else if (CompareNoCase(cimMethodName, "ModifyInstance") == 0)
               request = decodeModifyInstanceRequest(
                  queueId, parser, messageId, nameSpace, authType, userName);
            else if (CompareNoCase(cimMethodName, "DeleteClass") == 0)
               request = decodeDeleteClassRequest(
                  queueId, parser, messageId, nameSpace, authType, userName);
            else if (CompareNoCase(cimMethodName, "DeleteInstance") == 0)
               request = decodeDeleteInstanceRequest(
                  queueId, parser, messageId, nameSpace, authType, userName);
            else if (CompareNoCase(cimMethodName, "GetProperty") == 0)
               request = decodeGetPropertyRequest(
                  queueId, parser, messageId, nameSpace, authType, userName);
            else if (CompareNoCase(cimMethodName, "SetProperty") == 0)
               request = decodeSetPropertyRequest(
                  queueId, parser, messageId, nameSpace, authType, userName);
            else if (CompareNoCase(cimMethodName, "ExecQuery") == 0)
               request = decodeExecQueryRequest(
                  queueId, parser, messageId, nameSpace, authType, userName);
            else
            {
               throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED,
                  String("Unrecognized intrinsic method: ") + cimMethodName);
            }
         }
         catch (CIMException& e)
         {
            sendIMethodError(
               queueId, 
               messageId,
               cimMethodName,
               e);

            PEG_METHOD_EXIT();
            return;
         }

	 // Expect </IMETHODCALL>

	 XmlReader::expectEndTag(parser, "IMETHODCALL");
      }
      // Expect <METHODCALL ...>
      else if (XmlReader::getMethodCallStartTag(parser, cimMethodName))
      {
	 CIMObjectPath reference;
	 XmlEntry        entry;

         // The Specification for CIM Operations over HTTP reads:
         //     3.3.6. CIMMethod
         //
         //     This header MUST be present in any CIM Operation Request
         //     message that contains a Simple Operation Request.  
         //
         //     It MUST NOT be present in any CIM Operation Response message,
         //     nor in any CIM Operation Request message that is not a
         //     Simple Operation Request.
         //
         //     The name of the CIM method within a Simple Operation Request
         //     is defined to be the value of the NAME attribute of the
         //     <METHODCALL> or <IMETHODCALL> element.
         //
         //     If a CIM Server receives a CIM Operation Request for which
         //     either:
         //
         //     - The CIMMethod header is present but has an invalid value, or;
         //     - The CIMMethod header is not present but the Operation
         //       Request Message is a Simple Operation Request, or; 
         //     - The CIMMethod header is present but the Operation Request
         //       Message is not a Simple Operation Request, or; 
         //     - The CIMMethod header is present, the Operation Request
         //       Message is a Simple Operation Request, but the CIMIdentifier
         //       value (when unencoded) does not match the unique method name
         //       within the Simple Operation Request,
         //
         //     then it MUST fail the request and return a status of
         //     "400 Bad Request" (and MUST include a CIMError header in the
         //     response with a value of header-mismatch), subject to the
         //     considerations specified in Errors.
         if (!String::equalNoCase(cimMethodName, cimMethodInHeader))
         {
            // ATTN-RK-P3-20020304: How to decode cimMethodInHeader?
            if (cimMethodInHeader == String::EMPTY)
            {
               sendHttpError(queueId, HTTP_STATUS_BADREQUEST, "header-mismatch",
                             "Missing CIMMethod HTTP header.");
            }
            else
            {
               sendHttpError(queueId, HTTP_STATUS_BADREQUEST, "header-mismatch",
                             String("CIMMethod value \"") + cimMethodInHeader +
                                 "\" does not match CIM request method \"" +
                                 cimMethodName + "\".");
            }
            PEG_METHOD_EXIT();
            return;
         }

	 //
	 // Check for <LOCALINSTANCEPATHELEMENT ...>
	 //
	 if ( XmlReader::testStartTag (parser, entry,
				       XML_ELEMENT_LOCALINSTANCEPATH))
	 {
	    parser.putBack(entry);
	    if (!XmlReader::getLocalInstancePathElement(parser, reference))
	    {
	       throw XmlValidationError(parser.getLine(),
					"expected LOCALINSTANCEPATH element");
	    }
	 }
	 //
	 // Check for <LOCALCLASSPATHELEMENT ...>
	 //
	 else if ( XmlReader::testStartTag( parser, entry,
					    XML_ELEMENT_LOCALCLASSPATH))
	 {
	    parser.putBack(entry);
	    if (!XmlReader::getLocalClassPathElement(parser, reference))
	    {
	       throw XmlValidationError(parser.getLine(),
					"expected LOCALCLASSPATH element");
	    }
	 }
	 else
	 {
	    throw XmlValidationError(parser.getLine(),
				     MISSING_ELEMENT_LOCALPATH);
	 }

         // The Specification for CIM Operations over HTTP reads:
         //     3.3.7. CIMObject
         //
         //     This header MUST be present in any CIM Operation Request
         //     message that contains a Simple Operation Request.  
         //
         //     It MUST NOT be present in any CIM Operation Response message,
         //     nor in any CIM Operation Request message that is not a
         //     Simple Operation Request.
         //
         //     The header identifies the CIM object (which MUST be a Class
         //     or Instance for an extrinsic method, or a Namespace for an
         //     intrinsic method) on which the method is to be invoked, using
         //     a CIM object path encoded in an HTTP-safe representation.
         //
         //     If a CIM Server receives a CIM Operation Request for which
         //     either:
         //
         //     - The CIMObject header is present but has an invalid value, or;
         //     - The CIMObject header is not present but the Operation
         //       Request Message is a Simple Operation Request, or; 
         //     - The CIMObject header is present but the Operation Request
         //       Message is not a Simple Operation Request, or; 
         //     - The CIMObject header is present, the Operation Request
         //       Message is a Simple Operation Request, but the ObjectPath
         //       value does not match (where match is defined in the section
         //       section on Encoding CIM Object Paths) the Operation Request
         //       Message,
         //
         //     then it MUST fail the request and return a status of
         //     "400 Bad Request" (and MUST include a CIMError header in the
         //     response with a value of header-mismatch), subject to the
         //     considerations specified in Errors.
         if (cimObjectInHeader == String::EMPTY)
         {
            sendHttpError(queueId, HTTP_STATUS_BADREQUEST, "header-mismatch",
                          "Missing CIMObject HTTP header.");
            PEG_METHOD_EXIT();
            return;
         }

         CIMObjectPath headerObjectReference;
         try
         {
             headerObjectReference.set(cimObjectInHeader);
         }
         catch (Exception e)
         {
            sendHttpError(queueId, HTTP_STATUS_BADREQUEST, "header-mismatch",
                          String("Could not parse CIMObject value \"") +
                              cimObjectInHeader + "\".");
            PEG_METHOD_EXIT();
            return;
         }

         if (!reference.identical(headerObjectReference))
         {
            sendHttpError(queueId, HTTP_STATUS_BADREQUEST, "header-mismatch",
                          String("CIMObject value \"") + cimObjectInHeader +
                              "\" does not match CIM request object \"" +
                              reference.toString() + "\".");
            PEG_METHOD_EXIT();
            return;
         }

         // This try block only catches CIMExceptions, because they must be
         // responded to with a proper METHODRESPONSE.  Other exceptions are
         // caught in the outer try block.
         try
         {
	    // Delegate to appropriate method to handle:

            request = decodeInvokeMethodRequest(
               queueId, 
               parser, 
               messageId, 
               reference, 
               cimMethodName,
               authType,
               userName);
         }
         catch (CIMException& e)
         {
            sendMethodError(
               queueId, 
               messageId,
               cimMethodName,
               e);

            PEG_METHOD_EXIT();
            return;
         }

         // Expect </METHODCALL>

         XmlReader::expectEndTag(parser, "METHODCALL");
      }
      else
      {
	 throw XmlValidationError(parser.getLine(), 
				  "expected IMETHODCALL or METHODCALL element");
      }

      // Expect </SIMPLEREQ>

      XmlReader::expectEndTag(parser, "SIMPLEREQ");

      // Expect </MESSAGE>

      XmlReader::expectEndTag(parser, "MESSAGE");

      // Expect </CIM>

      XmlReader::expectEndTag(parser, "CIM");
   }
   catch (XmlValidationError& e)
   {
      sendHttpError(queueId, HTTP_STATUS_BADREQUEST, "request-not-valid",
                    e.getMessage());
      PEG_METHOD_EXIT();
      return;
   }
   catch (XmlSemanticError& e)
   {
      // ATTN-RK-P2-20020404: Is this the correct response for these errors?
      sendHttpError(queueId, HTTP_STATUS_BADREQUEST, "request-not-valid",
                    e.getMessage());
      PEG_METHOD_EXIT();
      return;
   }
   catch (XmlException& e)
   {
      sendHttpError(queueId, HTTP_STATUS_BADREQUEST, "request-not-well-formed",
                    e.getMessage());
      PEG_METHOD_EXIT();
      return;
   }
   catch (Exception& e)
   {
      // Don't know why I got this exception.  Seems like a bad thing.
      // Any exceptions we're expecting should be caught separately and
      // dealt with appropriately.  This is a last resort.
      sendHttpError(queueId, HTTP_STATUS_INTERNALSERVERERROR, String::EMPTY,
                    e.getMessage());
      PEG_METHOD_EXIT();
      return;
   }
   catch (...)
   {
      // Don't know why I got whatever this is.  Seems like a bad thing.
      // Any exceptions we're expecting should be caught separately and
      // dealt with appropriately.  This is a last resort.
      sendHttpError(queueId, HTTP_STATUS_INTERNALSERVERERROR);
      PEG_METHOD_EXIT();
      return;
   }

   STAT_BYTESREAD

   _outputQueue->enqueue(request);
   PEG_METHOD_EXIT();
}

CIMCreateClassRequestMessage* CIMOperationRequestDecoder::decodeCreateClassRequest(
   Uint32 queueId,
   XmlParser& parser, 
   const String& messageId,
   const String& nameSpace,
   const String& authType,
   const String& userName)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDecoder::decodeCreateClassRequest()");

   STAT_GETSTARTTIME

   CIMClass newClass;
   Boolean duplicateParameter = false;
   Boolean gotClass = false;

   for (const char* name; XmlReader::getIParamValueTag(parser, name);)
   {
      if (CompareNoCase(name, "NewClass") == 0)
      {
	 XmlReader::getClassElement(parser, newClass);
	 duplicateParameter = gotClass;
	 gotClass = true;
      }
      else
      {
         PEG_METHOD_EXIT();
	 throw CIMException(CIM_ERR_NOT_SUPPORTED);
      }

      XmlReader::expectEndTag(parser, "IPARAMVALUE");

      if (duplicateParameter)
      {
         PEG_METHOD_EXIT();
	 throw CIMException(CIM_ERR_INVALID_PARAMETER);
      }
   }

   if (!gotClass)
   {
      PEG_METHOD_EXIT();
      throw CIMException(CIM_ERR_INVALID_PARAMETER);
   }
 
   CIMCreateClassRequestMessage* request = new CIMCreateClassRequestMessage(
      messageId,
      nameSpace,
      newClass,
      QueueIdStack(queueId, _returnQueueId),
      authType,
      userName);

   STAT_SERVERSTART

   PEG_METHOD_EXIT();
   return(request);
}

CIMGetClassRequestMessage* CIMOperationRequestDecoder::decodeGetClassRequest(
   Uint32 queueId,
   XmlParser& parser, 
   const String& messageId,
   const String& nameSpace,
   const String& authType,
   const String& userName)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDecoder::decodeGetClassRequest()");

   STAT_GETSTARTTIME

   String className;
   Boolean localOnly = true;
   Boolean includeQualifiers = true;
   Boolean includeClassOrigin = false;
   CIMPropertyList propertyList;
   Boolean duplicateParameter = false;
   Boolean gotClassName = false;
   Boolean gotLocalOnly = false;
   Boolean gotIncludeQualifiers = false;
   Boolean gotIncludeClassOrigin = false;
   Boolean gotPropertyList = false;

   for (const char* name; XmlReader::getIParamValueTag(parser, name);)
   {
      if (CompareNoCase(name, "ClassName") == 0)
      {
	 XmlReader::getClassNameElement(parser, className, true);
	 duplicateParameter = gotClassName;
	 gotClassName = true;
      }
      else if (CompareNoCase(name, "LocalOnly") == 0)
      {
	 XmlReader::getBooleanValueElement(parser, localOnly, true);
	 duplicateParameter = gotLocalOnly;
	 gotLocalOnly = true;
      }
      else if (CompareNoCase(name, "IncludeQualifiers") == 0)
      {
	 XmlReader::getBooleanValueElement(parser, includeQualifiers, true);
	 duplicateParameter = gotIncludeQualifiers;
	 gotIncludeQualifiers = true;
      }
      else if (CompareNoCase(name, "IncludeClassOrigin") == 0)
      {
	 XmlReader::getBooleanValueElement(parser, includeClassOrigin, true);
	 duplicateParameter = gotIncludeClassOrigin;
	 gotIncludeClassOrigin = true;
      }
      else if (CompareNoCase(name, "PropertyList") == 0)
      {
	 CIMValue pl;
	 if (XmlReader::getValueArrayElement(parser, CIMTYPE_STRING, pl))
	 {
	    Array<String> propertyListArray;
	    pl.get(propertyListArray);
            Array<CIMName> cimNameArray;
            for (Uint32 i = 0; i < propertyListArray.size(); i++)
            {
                cimNameArray.append(propertyListArray[i]);
            }
	    propertyList.set(cimNameArray);
	 }
	 duplicateParameter = gotPropertyList;
	 gotPropertyList = true;
      }
      else
      {
         PEG_METHOD_EXIT();
	 throw CIMException(CIM_ERR_NOT_SUPPORTED);
      }

      XmlReader::expectEndTag(parser, "IPARAMVALUE");

      if (duplicateParameter)
      {
         PEG_METHOD_EXIT();
	 throw CIMException(CIM_ERR_INVALID_PARAMETER);
      }
   }

   if (!gotClassName)
   {
      PEG_METHOD_EXIT();
      throw CIMException(CIM_ERR_INVALID_PARAMETER);
   }

   CIMGetClassRequestMessage* request = new CIMGetClassRequestMessage(
      messageId,
      nameSpace,
      className,
      localOnly,
      includeQualifiers,
      includeClassOrigin,
      propertyList,
      QueueIdStack(queueId, _returnQueueId),
      authType,
      userName);

   STAT_SERVERSTART

   PEG_METHOD_EXIT();
   return(request);
}

CIMModifyClassRequestMessage* CIMOperationRequestDecoder::decodeModifyClassRequest(
   Uint32 queueId,
   XmlParser& parser, 
   const String& messageId,
   const String& nameSpace,
   const String& authType,
   const String& userName)
{
   STAT_GETSTARTTIME
  
   CIMClass modifiedClass;
   Boolean duplicateParameter = false;
   Boolean gotClass = false;

   for (const char* name; XmlReader::getIParamValueTag(parser, name);)
   {
      if (CompareNoCase(name, "ModifiedClass") == 0)
      {
	 XmlReader::getClassElement(parser, modifiedClass);
	 duplicateParameter = gotClass;
	 gotClass = true;
      }
      else
      {
	 throw CIMException(CIM_ERR_NOT_SUPPORTED);
      }

      XmlReader::expectEndTag(parser, "IPARAMVALUE");

      if (duplicateParameter)
      {
	 throw CIMException(CIM_ERR_INVALID_PARAMETER);
      }
   }

   if (!gotClass)
   {
      throw CIMException(CIM_ERR_INVALID_PARAMETER);
   }

   CIMModifyClassRequestMessage* request = 
      new CIMModifyClassRequestMessage(
	 messageId,
	 nameSpace,
	 modifiedClass,
	 QueueIdStack(queueId, _returnQueueId),
	 authType,
	 userName);

   STAT_SERVERSTART

   return(request);
}

CIMEnumerateClassNamesRequestMessage* CIMOperationRequestDecoder::decodeEnumerateClassNamesRequest(
   Uint32 queueId,
   XmlParser& parser, 
   const String& messageId,
   const String& nameSpace,
   const String& authType,
   const String& userName)
{
   STAT_GETSTARTTIME

   String className;
   Boolean deepInheritance = false;
   Boolean duplicateParameter = false;
   Boolean gotClassName = false;
   Boolean gotDeepInheritance = false;

   for (const char* name; XmlReader::getIParamValueTag(parser, name);)
   {
      if (CompareNoCase(name, "ClassName") == 0)
      {
	 XmlReader::getClassNameElement(parser, className, true);
	 duplicateParameter = gotClassName;
	 gotClassName = true;
      }
      else if (CompareNoCase(name, "DeepInheritance") == 0)
      {
	 XmlReader::getBooleanValueElement(parser, deepInheritance, true);
	 duplicateParameter = gotDeepInheritance;
	 gotDeepInheritance = true;
      }
      else
      {
	 throw CIMException(CIM_ERR_NOT_SUPPORTED);
      }

      XmlReader::expectEndTag(parser, "IPARAMVALUE");

      if (duplicateParameter)
      {
	 throw CIMException(CIM_ERR_INVALID_PARAMETER);
      }
   }

   CIMEnumerateClassNamesRequestMessage* request = 
      new CIMEnumerateClassNamesRequestMessage(
	 messageId,
	 nameSpace,
	 className,
	 deepInheritance,
	 QueueIdStack(queueId, _returnQueueId),
	 authType,
	 userName);

   STAT_SERVERSTART

   return(request);
}

CIMEnumerateClassesRequestMessage* CIMOperationRequestDecoder::decodeEnumerateClassesRequest(
   Uint32 queueId,
   XmlParser& parser, 
   const String& messageId,
   const String& nameSpace,
   const String& authType,
   const String& userName)
{
   STAT_GETSTARTTIME

   String className;
   Boolean deepInheritance = false;
   Boolean localOnly = true;
   Boolean includeQualifiers = true;
   Boolean includeClassOrigin = false;
   Boolean duplicateParameter = false;
   Boolean gotClassName = false;
   Boolean gotDeepInheritance = false;
   Boolean gotLocalOnly = false;
   Boolean gotIncludeQualifiers = false;
   Boolean gotIncludeClassOrigin = false;

   for (const char* name; XmlReader::getIParamValueTag(parser, name);)
   {
      if (CompareNoCase(name, "ClassName") == 0)
      {
	 XmlReader::getClassNameElement(parser, className, true);
	 duplicateParameter = gotClassName;
	 gotClassName = true;
      }
      else if (CompareNoCase(name, "DeepInheritance") == 0)
      {
	 XmlReader::getBooleanValueElement(parser, deepInheritance, true);
	 duplicateParameter = gotDeepInheritance;
	 gotDeepInheritance = true;
      }
      else if (CompareNoCase(name, "LocalOnly") == 0)
      {
	 XmlReader::getBooleanValueElement(parser, localOnly, true);
	 duplicateParameter = gotLocalOnly;
	 gotLocalOnly = true;
      }
      else if (CompareNoCase(name, "IncludeQualifiers") == 0)
      {
	 XmlReader::getBooleanValueElement(parser, includeQualifiers, true);
	 duplicateParameter = gotIncludeQualifiers;
	 gotIncludeQualifiers = true;
      }
      else if (CompareNoCase(name, "IncludeClassOrigin") == 0)
      {
	 XmlReader::getBooleanValueElement(parser, includeClassOrigin, true);
	 duplicateParameter = gotIncludeClassOrigin;
	 gotIncludeClassOrigin = true;
      }
      else
      {
	 throw CIMException(CIM_ERR_NOT_SUPPORTED);
      }

      XmlReader::expectEndTag(parser, "IPARAMVALUE");

      if (duplicateParameter)
      {
	 throw CIMException(CIM_ERR_INVALID_PARAMETER);
      }
   }

   CIMEnumerateClassesRequestMessage* request = 
      new CIMEnumerateClassesRequestMessage(
	 messageId,
	 nameSpace,
	 className,
	 deepInheritance,
	 localOnly,
	 includeQualifiers,
	 includeClassOrigin,
	 QueueIdStack(queueId, _returnQueueId),
	 authType,
	 userName);

   STAT_SERVERSTART

   return(request);
}

CIMDeleteClassRequestMessage* CIMOperationRequestDecoder::decodeDeleteClassRequest(
   Uint32 queueId,
   XmlParser& parser, 
   const String& messageId,
   const String& nameSpace,
   const String& authType,
   const String& userName)
{
   STAT_GETSTARTTIME

   String className;
   Boolean duplicateParameter = false;
   Boolean gotClassName = false;

   for (const char* name; XmlReader::getIParamValueTag(parser, name);)
   {
      if (CompareNoCase(name, "ClassName") == 0)
      {
	 XmlReader::getClassNameElement(parser, className);
	 duplicateParameter = gotClassName;
	 gotClassName = true;
      }
      else
      {
	 throw CIMException(CIM_ERR_NOT_SUPPORTED);
      }

      XmlReader::expectEndTag(parser, "IPARAMVALUE");

      if (duplicateParameter)
      {
	 throw CIMException(CIM_ERR_INVALID_PARAMETER);
      }
   }

   if (!gotClassName)
   {
      throw CIMException(CIM_ERR_INVALID_PARAMETER);
   }

   CIMDeleteClassRequestMessage* request = new CIMDeleteClassRequestMessage(
      messageId,
      nameSpace,
      className,
      QueueIdStack(queueId, _returnQueueId),
      authType,
      userName);

   STAT_SERVERSTART

   return(request);
}

CIMCreateInstanceRequestMessage* CIMOperationRequestDecoder::decodeCreateInstanceRequest(
   Uint32 queueId,
   XmlParser& parser, 
   const String& messageId,
   const String& nameSpace,
   const String& authType,
   const String& userName)
{
   STAT_GETSTARTTIME

   CIMInstance newInstance;
   Boolean duplicateParameter = false;
   Boolean gotInstance = false;

   for (const char* name; XmlReader::getIParamValueTag(parser, name);)
   {
      if (CompareNoCase(name, "NewInstance") == 0)
      {
	 XmlReader::getInstanceElement(parser, newInstance);
	 duplicateParameter = gotInstance;
	 gotInstance = true;
      }
      else
      {
	 throw CIMException(CIM_ERR_NOT_SUPPORTED);
      }

      XmlReader::expectEndTag(parser, "IPARAMVALUE");

      if (duplicateParameter)
      {
	 throw CIMException(CIM_ERR_INVALID_PARAMETER);
      }
   }

   if (!gotInstance)
   {
      throw CIMException(CIM_ERR_INVALID_PARAMETER);
   }

   CIMCreateInstanceRequestMessage* request = 
      new CIMCreateInstanceRequestMessage(
	 messageId,
	 nameSpace,
	 newInstance,
	 QueueIdStack(queueId, _returnQueueId),
	 authType,
	 userName);

   STAT_SERVERSTART

   return(request);
}

CIMGetInstanceRequestMessage* CIMOperationRequestDecoder::decodeGetInstanceRequest(
   Uint32 queueId,
   XmlParser& parser, 
   const String& messageId,
   const String& nameSpace,
   const String& authType,
   const String& userName)
{
   STAT_GETSTARTTIME

   CIMObjectPath instanceName;
   Boolean localOnly = true;
   Boolean includeQualifiers = false;
   Boolean includeClassOrigin = false;
   CIMPropertyList propertyList;
   Boolean duplicateParameter = false;
   Boolean gotInstanceName = false;
   Boolean gotLocalOnly = false;
   Boolean gotIncludeQualifiers = false;
   Boolean gotIncludeClassOrigin = false;
   Boolean gotPropertyList = false;

   for (const char* name; XmlReader::getIParamValueTag(parser, name);)
   {
      if (CompareNoCase(name, "InstanceName") == 0)
      {
	 XmlReader::getInstanceNameElement(parser, instanceName);
	 duplicateParameter = gotInstanceName;
	 gotInstanceName = true;
      }
      else if (CompareNoCase(name, "LocalOnly") == 0)
      {
	 XmlReader::getBooleanValueElement(parser, localOnly, true);
	 duplicateParameter = gotLocalOnly;
	 gotLocalOnly = true;
      }
      else if (CompareNoCase(name, "IncludeQualifiers") == 0)
      {
	 XmlReader::getBooleanValueElement(parser, includeQualifiers, true);
	 duplicateParameter = gotIncludeQualifiers;
	 gotIncludeQualifiers = true;
      }
      else if (CompareNoCase(name, "IncludeClassOrigin") == 0)
      {
	 XmlReader::getBooleanValueElement(parser, includeClassOrigin, true);
	 duplicateParameter = gotIncludeClassOrigin;
	 gotIncludeClassOrigin = true;
      }
      else if (CompareNoCase(name, "PropertyList") == 0)
      {
	 CIMValue pl;
	 if (XmlReader::getValueArrayElement(parser, CIMTYPE_STRING, pl))
	 {
	    Array<String> propertyListArray;
	    pl.get(propertyListArray);
            Array<CIMName> cimNameArray;
            for (Uint32 i = 0; i < propertyListArray.size(); i++)
            {
                cimNameArray.append(propertyListArray[i]);
            }
	    propertyList.set(cimNameArray);
	 }
	 duplicateParameter = gotPropertyList;
	 gotPropertyList = true;
      }
      else
      {
	 throw CIMException(CIM_ERR_NOT_SUPPORTED);
      }

      XmlReader::expectEndTag(parser, "IPARAMVALUE");

      if (duplicateParameter)
      {
	 throw CIMException(CIM_ERR_INVALID_PARAMETER);
      }
   }

   if (!gotInstanceName)
   {
      throw CIMException(CIM_ERR_INVALID_PARAMETER);
   }

   CIMGetInstanceRequestMessage* request = new CIMGetInstanceRequestMessage(
      messageId,
      nameSpace,
      instanceName,
      localOnly,
      includeQualifiers,
      includeClassOrigin,
      propertyList,
      QueueIdStack(queueId, _returnQueueId),
      authType,
      userName);

   STAT_SERVERSTART

   return(request);
}

CIMModifyInstanceRequestMessage* CIMOperationRequestDecoder::decodeModifyInstanceRequest(
   Uint32 queueId,
   XmlParser& parser, 
   const String& messageId,
   const String& nameSpace,
   const String& authType,
   const String& userName)
{
   STAT_GETSTARTTIME

   CIMInstance modifiedInstance;
   Boolean includeQualifiers = true;
   CIMPropertyList propertyList;
   Boolean duplicateParameter = false;
   Boolean gotInstance = false;
   Boolean gotIncludeQualifiers = false;
   Boolean gotPropertyList = false;

   for (const char* name; XmlReader::getIParamValueTag(parser, name);)
   {
      if (CompareNoCase(name, "ModifiedInstance") == 0)
      {
	 XmlReader::getNamedInstanceElement(parser, modifiedInstance);
	 duplicateParameter = gotInstance;
	 gotInstance = true;
      }
      else if (CompareNoCase(name, "IncludeQualifiers") == 0)
      {
	 XmlReader::getBooleanValueElement(parser, includeQualifiers, true);
	 duplicateParameter = gotIncludeQualifiers;
	 gotIncludeQualifiers = true;
      }
      else if (CompareNoCase(name, "PropertyList") == 0)
      {
	 CIMValue pl;
	 if (XmlReader::getValueArrayElement(parser, CIMTYPE_STRING, pl))
	 {
	    Array<String> propertyListArray;
	    pl.get(propertyListArray);
            Array<CIMName> cimNameArray;
            for (Uint32 i = 0; i < propertyListArray.size(); i++)
            {
                cimNameArray.append(propertyListArray[i]);
            }
	    propertyList.set(cimNameArray);
	 }
	 duplicateParameter = gotPropertyList;
	 gotPropertyList = true;
      }
      else
      {
	 throw CIMException(CIM_ERR_NOT_SUPPORTED);
      }

      XmlReader::expectEndTag(parser, "IPARAMVALUE");

      if (duplicateParameter)
      {
	 throw CIMException(CIM_ERR_INVALID_PARAMETER);
      }
   }

   if (!gotInstance)
   {
      throw CIMException(CIM_ERR_INVALID_PARAMETER);
   }

   CIMModifyInstanceRequestMessage* request = 
      new CIMModifyInstanceRequestMessage(
	 messageId,
	 nameSpace,
	 modifiedInstance,
	 includeQualifiers,
	 propertyList,
	 QueueIdStack(queueId, _returnQueueId),
	 authType,
	 userName);

   STAT_SERVERSTART

   return(request);
}

CIMEnumerateInstancesRequestMessage* CIMOperationRequestDecoder::decodeEnumerateInstancesRequest(
   Uint32 queueId,
   XmlParser& parser, 
   const String& messageId,
   const String& nameSpace,
   const String& authType,
   const String& userName)
{
   STAT_GETSTARTTIME

   String className;
   Boolean deepInheritance = false;
   Boolean localOnly = true;
   Boolean includeQualifiers = true;
   Boolean includeClassOrigin = false;
   CIMPropertyList propertyList;
   Boolean duplicateParameter = false;
   Boolean gotClassName = false;
   Boolean gotDeepInheritance = false;
   Boolean gotLocalOnly = false;
   Boolean gotIncludeQualifiers = false;
   Boolean gotIncludeClassOrigin = false;
   Boolean gotPropertyList = false;

   for (const char* name; XmlReader::getIParamValueTag(parser, name);)
   {
      if (CompareNoCase(name, "ClassName") == 0)
      {
	 XmlReader::getClassNameElement(parser, className, true);
	 duplicateParameter = gotClassName;
	 gotClassName = true;
      }
      else if (CompareNoCase(name, "DeepInheritance") == 0)
      {
	 XmlReader::getBooleanValueElement(parser, deepInheritance, true);
	 duplicateParameter = gotDeepInheritance;
	 gotDeepInheritance = true;
      }
      else if (CompareNoCase(name, "LocalOnly") == 0)
      {
	 XmlReader::getBooleanValueElement(parser, localOnly, true);
	 duplicateParameter = gotLocalOnly;
	 gotLocalOnly = true;
      }
      else if (CompareNoCase(name, "IncludeQualifiers") == 0)
      {
	 XmlReader::getBooleanValueElement(parser, includeQualifiers, true);
	 duplicateParameter = gotIncludeQualifiers;
	 gotIncludeQualifiers = true;
      }
      else if (CompareNoCase(name, "IncludeClassOrigin") == 0)
      {
	 XmlReader::getBooleanValueElement(parser, includeClassOrigin, true);
	 duplicateParameter = gotIncludeClassOrigin;
	 gotIncludeClassOrigin = true;
      }
      else if (CompareNoCase(name, "PropertyList") == 0)
      {
	 CIMValue pl;
	 if (XmlReader::getValueArrayElement(parser, CIMTYPE_STRING, pl))
	 {
	    Array<String> propertyListArray;
	    pl.get(propertyListArray);
            Array<CIMName> cimNameArray;
            for (Uint32 i = 0; i < propertyListArray.size(); i++)
            {
                cimNameArray.append(propertyListArray[i]);
            }
	    propertyList.set(cimNameArray);
	 }
	 duplicateParameter = gotPropertyList;
	 gotPropertyList = true;
      }
      else
      {
	 throw CIMException(CIM_ERR_NOT_SUPPORTED);
      }

      XmlReader::expectEndTag(parser, "IPARAMVALUE");

      if (duplicateParameter)
      {
	 throw CIMException(CIM_ERR_INVALID_PARAMETER);
      }
   }

   if (!gotClassName)
   {
      throw CIMException(CIM_ERR_INVALID_PARAMETER);
   }

   CIMEnumerateInstancesRequestMessage* request = 
      new CIMEnumerateInstancesRequestMessage(
	 messageId,
	 nameSpace,
	 className,
	 deepInheritance,
	 localOnly,
	 includeQualifiers,
	 includeClassOrigin,
	 propertyList,
	 QueueIdStack(queueId, _returnQueueId),
	 authType,
	 userName);

   STAT_SERVERSTART

   return(request);
}

CIMEnumerateInstanceNamesRequestMessage* CIMOperationRequestDecoder::decodeEnumerateInstanceNamesRequest(
   Uint32 queueId,
   XmlParser& parser, 
   const String& messageId,
   const String& nameSpace,
   const String& authType,
   const String& userName)
{
   STAT_GETSTARTTIME

   String className;
   Boolean duplicateParameter = false;
   Boolean gotClassName = false;

   for (const char* name; XmlReader::getIParamValueTag(parser, name);)
   {
      if (CompareNoCase(name, "ClassName") == 0)
      {
	 XmlReader::getClassNameElement(parser, className, true);
	 duplicateParameter = gotClassName;
	 gotClassName = true;
      }
      else
      {
	 throw CIMException(CIM_ERR_NOT_SUPPORTED);
      }

      XmlReader::expectEndTag(parser, "IPARAMVALUE");

      if (duplicateParameter)
      {
	 throw CIMException(CIM_ERR_INVALID_PARAMETER);
      }
   }

   if (!gotClassName)
   {
      throw CIMException(CIM_ERR_INVALID_PARAMETER);
   }

   CIMEnumerateInstanceNamesRequestMessage* request = 
      new CIMEnumerateInstanceNamesRequestMessage(
	 messageId,
	 nameSpace,
	 className,
	 QueueIdStack(queueId, _returnQueueId),
	 authType,
	 userName);

   STAT_SERVERSTART

   return(request);
}

CIMDeleteInstanceRequestMessage* CIMOperationRequestDecoder::decodeDeleteInstanceRequest(
   Uint32 queueId,
   XmlParser& parser, 
   const String& messageId,
   const String& nameSpace,
   const String& authType,
   const String& userName)
{
   STAT_GETSTARTTIME

   CIMObjectPath instanceName;
   Boolean duplicateParameter = false;
   Boolean gotInstanceName = false;

   for (const char* name; XmlReader::getIParamValueTag(parser, name);)
   {
      if (CompareNoCase(name, "InstanceName") == 0)
      {
	 XmlReader::getInstanceNameElement(parser, instanceName);
	 duplicateParameter = gotInstanceName;
	 gotInstanceName = true;
      }
      else
      {
	 throw CIMException(CIM_ERR_NOT_SUPPORTED);
      }

      XmlReader::expectEndTag(parser, "IPARAMVALUE");

      if (duplicateParameter)
      {
	 throw CIMException(CIM_ERR_INVALID_PARAMETER);
      }
   }

   if (!gotInstanceName)
   {
      throw CIMException(CIM_ERR_INVALID_PARAMETER);
   }

   CIMDeleteInstanceRequestMessage* request = new CIMDeleteInstanceRequestMessage(
      messageId,
      nameSpace,
      instanceName,
      QueueIdStack(queueId, _returnQueueId),
      authType,
      userName);

   STAT_SERVERSTART

   return(request);
}

CIMSetQualifierRequestMessage* CIMOperationRequestDecoder::decodeSetQualifierRequest(
   Uint32 queueId,
   XmlParser& parser, 
   const String& messageId,
   const String& nameSpace,
   const String& authType,
   const String& userName)
{
   STAT_GETSTARTTIME

   CIMQualifierDecl qualifierDeclaration;
   Boolean duplicateParameter = false;
   Boolean gotQualifierDeclaration = false;

   for (const char* name; XmlReader::getIParamValueTag(parser, name);)
   {
      if (CompareNoCase(name, "QualifierDeclaration") == 0)
      {
	 XmlReader::getQualifierDeclElement(parser, qualifierDeclaration);
	 duplicateParameter = gotQualifierDeclaration;
	 gotQualifierDeclaration = true;
      }
      else
      {
	 throw CIMException(CIM_ERR_NOT_SUPPORTED);
      }

      XmlReader::expectEndTag(parser, "IPARAMVALUE");

      if (duplicateParameter)
      {
	 throw CIMException(CIM_ERR_INVALID_PARAMETER);
      }
   }

   if (!gotQualifierDeclaration)
   {
      throw CIMException(CIM_ERR_INVALID_PARAMETER);
   }

   CIMSetQualifierRequestMessage* request = 
      new CIMSetQualifierRequestMessage(
	 messageId,
	 nameSpace,
	 qualifierDeclaration,
	 QueueIdStack(queueId, _returnQueueId),
	 authType,
	 userName);

   STAT_SERVERSTART

   return(request);
}

CIMGetQualifierRequestMessage* CIMOperationRequestDecoder::decodeGetQualifierRequest(
   Uint32 queueId,
   XmlParser& parser, 
   const String& messageId,
   const String& nameSpace,
   const String& authType,
   const String& userName)
{
   STAT_GETSTARTTIME

   String qualifierName;
   Boolean duplicateParameter = false;
   Boolean gotQualifierName = false;

   for (const char* name; XmlReader::getIParamValueTag(parser, name);)
   {
      if (CompareNoCase(name, "QualifierName") == 0)
      {
	 XmlReader::getClassNameElement(parser, qualifierName, true);
	 duplicateParameter = gotQualifierName;
	 gotQualifierName = true;
      }
      else
      {
	 throw CIMException(CIM_ERR_NOT_SUPPORTED);
      }

      XmlReader::expectEndTag(parser, "IPARAMVALUE");

      if (duplicateParameter)
      {
	 throw CIMException(CIM_ERR_INVALID_PARAMETER);
      }
   }

   if (!gotQualifierName)
   {
      throw CIMException(CIM_ERR_INVALID_PARAMETER);
   }

   CIMGetQualifierRequestMessage* request = 
      new CIMGetQualifierRequestMessage(
	 messageId,
	 nameSpace,
	 qualifierName,
	 QueueIdStack(queueId, _returnQueueId),
	 authType,
	 userName);

   STAT_SERVERSTART

   return(request);
}

CIMEnumerateQualifiersRequestMessage* CIMOperationRequestDecoder::decodeEnumerateQualifiersRequest(
   Uint32 queueId,
   XmlParser& parser, 
   const String& messageId,
   const String& nameSpace,
   const String& authType,
   const String& userName)
{
   STAT_GETSTARTTIME

   for (const char* name; XmlReader::getIParamValueTag(parser, name);)
   {
      // No IPARAMVALUEs are defined for this operation
      throw CIMException(CIM_ERR_NOT_SUPPORTED);
   }

   CIMEnumerateQualifiersRequestMessage* request = 
      new CIMEnumerateQualifiersRequestMessage(
	 messageId,
	 nameSpace,
	 QueueIdStack(queueId, _returnQueueId),
	 authType,
	 userName);

   STAT_SERVERSTART

   return(request);
}

CIMDeleteQualifierRequestMessage* CIMOperationRequestDecoder::decodeDeleteQualifierRequest(
   Uint32 queueId,
   XmlParser& parser, 
   const String& messageId,
   const String& nameSpace,
   const String& authType,
   const String& userName)
{
   STAT_GETSTARTTIME

   String qualifierName;
   Boolean duplicateParameter = false;
   Boolean gotQualifierName = false;

   for (const char* name; XmlReader::getIParamValueTag(parser, name);)
   {
      if (CompareNoCase(name, "QualifierName") == 0)
      {
	 XmlReader::getClassNameElement(parser, qualifierName, true);
	 duplicateParameter = gotQualifierName;
	 gotQualifierName = true;
      }
      else
      {
	 throw CIMException(CIM_ERR_NOT_SUPPORTED);
      }

      XmlReader::expectEndTag(parser, "IPARAMVALUE");

      if (duplicateParameter)
      {
	 throw CIMException(CIM_ERR_INVALID_PARAMETER);
      }
   }

   if (!gotQualifierName)
   {
      throw CIMException(CIM_ERR_INVALID_PARAMETER);
   }

   CIMDeleteQualifierRequestMessage* request = 
      new CIMDeleteQualifierRequestMessage(
	 messageId,
	 nameSpace,
	 qualifierName,
	 QueueIdStack(queueId, _returnQueueId),
	 authType,
	 userName);

   STAT_SERVERSTART

   return(request);
}

CIMReferenceNamesRequestMessage* CIMOperationRequestDecoder::decodeReferenceNamesRequest(
   Uint32 queueId,
   XmlParser& parser, 
   const String& messageId,
   const String& nameSpace,
   const String& authType,
   const String& userName)
{
   STAT_GETSTARTTIME

   CIMObjectPath objectName;
   String resultClass;
   String role;
   Boolean duplicateParameter = false;
   Boolean gotObjectName = false;
   Boolean gotResultClass = false;
   Boolean gotRole = false;

   for (const char* name; XmlReader::getIParamValueTag(parser, name);)
   {
      if (CompareNoCase(name, "ObjectName") == 0)
      {
	 XmlReader::getObjectNameElement(parser, objectName);
	 duplicateParameter = gotObjectName;
	 gotObjectName = true;
      }
      else if (CompareNoCase(name, "ResultClass") == 0)
      {
	 XmlReader::getClassNameElement(parser, resultClass, true);
	 duplicateParameter = gotResultClass;
	 gotResultClass = true;
      }
      else if (CompareNoCase(name, "Role") == 0)
      {
	 XmlReader::getStringValueElement(parser, role, true);
	 duplicateParameter = gotRole;
	 gotRole = true;
      }
      else
      {
	 throw CIMException(CIM_ERR_NOT_SUPPORTED);
      }

      XmlReader::expectEndTag(parser, "IPARAMVALUE");

      if (duplicateParameter)
      {
	 throw CIMException(CIM_ERR_INVALID_PARAMETER);
      }
   }

   if (!gotObjectName)
   {
      throw CIMException(CIM_ERR_INVALID_PARAMETER);
   }

   CIMReferenceNamesRequestMessage* request = 
      new CIMReferenceNamesRequestMessage(
	 messageId,
	 nameSpace,
	 objectName,
	 resultClass,
	 role,
	 QueueIdStack(queueId, _returnQueueId),
	 authType,
	 userName);

   STAT_SERVERSTART

   return(request);
}

CIMReferencesRequestMessage* CIMOperationRequestDecoder::decodeReferencesRequest(
   Uint32 queueId,
   XmlParser& parser, 
   const String& messageId,
   const String& nameSpace,
   const String& authType,
   const String& userName)
{
   STAT_GETSTARTTIME

   CIMObjectPath objectName;
   String resultClass;
   String role;
   Boolean includeQualifiers = false;
   Boolean includeClassOrigin = false;
   CIMPropertyList propertyList;
   Boolean duplicateParameter = false;
   Boolean gotObjectName = false;
   Boolean gotResultClass = false;
   Boolean gotRole = false;
   Boolean gotIncludeQualifiers = false;
   Boolean gotIncludeClassOrigin = false;
   Boolean gotPropertyList = false;

   for (const char* name; XmlReader::getIParamValueTag(parser, name);)
   {
      if (CompareNoCase(name, "ObjectName") == 0)
      {
	 XmlReader::getObjectNameElement(parser, objectName);
	 duplicateParameter = gotObjectName;
	 gotObjectName = true;
      }
      else if (CompareNoCase(name, "ResultClass") == 0)
      {
	 XmlReader::getClassNameElement(parser, resultClass, true);
	 duplicateParameter = gotResultClass;
	 gotResultClass = true;
      }
      else if (CompareNoCase(name, "Role") == 0)
      {
	 XmlReader::getStringValueElement(parser, role, true);
	 duplicateParameter = gotRole;
	 gotRole = true;
      }
      else if (CompareNoCase(name, "IncludeQualifiers") == 0)
      {
	 XmlReader::getBooleanValueElement(parser, includeQualifiers, true);
	 duplicateParameter = gotIncludeQualifiers;
	 gotIncludeQualifiers = true;
      }
      else if (CompareNoCase(name, "IncludeClassOrigin") == 0)
      {
	 XmlReader::getBooleanValueElement(parser, includeClassOrigin, true);
	 duplicateParameter = gotIncludeClassOrigin;
	 gotIncludeClassOrigin = true;
      }
      else if (CompareNoCase(name, "PropertyList") == 0)
      {
	 CIMValue pl;
	 if (XmlReader::getValueArrayElement(parser, CIMTYPE_STRING, pl))
	 {
	    Array<String> propertyListArray;
	    pl.get(propertyListArray);
            Array<CIMName> cimNameArray;
            for (Uint32 i = 0; i < propertyListArray.size(); i++)
            {
                cimNameArray.append(propertyListArray[i]);
            }
	    propertyList.set(cimNameArray);
	 }
	 duplicateParameter = gotPropertyList;
	 gotPropertyList = true;
      }
      else
      {
	 throw CIMException(CIM_ERR_NOT_SUPPORTED);
      }

      XmlReader::expectEndTag(parser, "IPARAMVALUE");

      if (duplicateParameter)
      {
	 throw CIMException(CIM_ERR_INVALID_PARAMETER);
      }
   }

   if (!gotObjectName)
   {
      throw CIMException(CIM_ERR_INVALID_PARAMETER);
   }

   CIMReferencesRequestMessage* request = 
      new CIMReferencesRequestMessage(
	 messageId,
	 nameSpace,
	 objectName,
	 resultClass,
	 role,
	 includeQualifiers,
	 includeClassOrigin,
	 propertyList,
	 QueueIdStack(queueId, _returnQueueId),
	 authType,
	 userName);

   STAT_SERVERSTART

   return(request);
}

CIMAssociatorNamesRequestMessage* CIMOperationRequestDecoder::decodeAssociatorNamesRequest(
   Uint32 queueId,
   XmlParser& parser, 
   const String& messageId,
   const String& nameSpace,
   const String& authType,
   const String& userName)
{
   STAT_GETSTARTTIME

   CIMObjectPath objectName;
   String assocClass;
   String resultClass;
   String role;
   String resultRole;
   Boolean duplicateParameter = false;
   Boolean gotObjectName = false;
   Boolean gotAssocClass = false;
   Boolean gotResultClass = false;
   Boolean gotRole = false;
   Boolean gotResultRole = false;

   for (const char* name; XmlReader::getIParamValueTag(parser, name);)
   {
      if (CompareNoCase(name, "ObjectName") == 0)
      {
	 XmlReader::getObjectNameElement(parser, objectName);
	 duplicateParameter = gotObjectName;
	 gotObjectName = true;
      }
      else if (CompareNoCase(name, "AssocClass") == 0)
      {
	 XmlReader::getClassNameElement(parser, assocClass, true);
	 duplicateParameter = gotAssocClass;
	 gotAssocClass = true;
      }
      else if (CompareNoCase(name, "ResultClass") == 0)
      {
	 XmlReader::getClassNameElement(parser, resultClass, true);
	 duplicateParameter = gotResultClass;
	 gotResultClass = true;
      }
      else if (CompareNoCase(name, "Role") == 0)
      {
	 XmlReader::getStringValueElement(parser, role, true);
	 duplicateParameter = gotRole;
	 gotRole = true;
      }
      else if (CompareNoCase(name, "ResultRole") == 0)
      {
	 XmlReader::getStringValueElement(parser, resultRole, true);
	 duplicateParameter = gotResultRole;
	 gotResultRole = true;
      }
      else
      {
	 throw CIMException(CIM_ERR_NOT_SUPPORTED);
      }

      XmlReader::expectEndTag(parser, "IPARAMVALUE");

      if (duplicateParameter)
      {
	 throw CIMException(CIM_ERR_INVALID_PARAMETER);
      }
   }

   if (!gotObjectName)
   {
      throw CIMException(CIM_ERR_INVALID_PARAMETER);
   }

   CIMAssociatorNamesRequestMessage* request = 
      new CIMAssociatorNamesRequestMessage(
	 messageId,
	 nameSpace,
	 objectName,
	 assocClass,
	 resultClass,
	 role,
	 resultRole,
	 QueueIdStack(queueId, _returnQueueId),
	 authType,
	 userName);

   STAT_SERVERSTART

   return(request);
}

CIMAssociatorsRequestMessage* CIMOperationRequestDecoder::decodeAssociatorsRequest(
   Uint32 queueId,
   XmlParser& parser, 
   const String& messageId,
   const String& nameSpace,
   const String& authType,
   const String& userName)
{
   STAT_GETSTARTTIME

   CIMObjectPath objectName;
   String assocClass;
   String resultClass;
   String role;
   String resultRole;
   Boolean includeQualifiers = false;
   Boolean includeClassOrigin = false;
   CIMPropertyList propertyList;
   Boolean duplicateParameter = false;
   Boolean gotObjectName = false;
   Boolean gotAssocClass = false;
   Boolean gotResultClass = false;
   Boolean gotRole = false;
   Boolean gotResultRole = false;
   Boolean gotIncludeQualifiers = false;
   Boolean gotIncludeClassOrigin = false;
   Boolean gotPropertyList = false;

   for (const char* name; XmlReader::getIParamValueTag(parser, name);)
   {
      if (CompareNoCase(name, "ObjectName") == 0)
      {
	 XmlReader::getObjectNameElement(parser, objectName);
	 duplicateParameter = gotObjectName;
	 gotObjectName = true;
      }
      else if (CompareNoCase(name, "AssocClass") == 0)
      {
	 XmlReader::getClassNameElement(parser, assocClass, true);
	 duplicateParameter = gotAssocClass;
	 gotAssocClass = true;
      }
      else if (CompareNoCase(name, "ResultClass") == 0)
      {
	 XmlReader::getClassNameElement(parser, resultClass, true);
	 duplicateParameter = gotResultClass;
	 gotResultClass = true;
      }
      else if (CompareNoCase(name, "Role") == 0)
      {
	 XmlReader::getStringValueElement(parser, role, true);
	 duplicateParameter = gotRole;
	 gotRole = true;
      }
      else if (CompareNoCase(name, "ResultRole") == 0)
      {
	 XmlReader::getStringValueElement(parser, resultRole, true);
	 duplicateParameter = gotResultRole;
	 gotResultRole = true;
      }
      else if (CompareNoCase(name, "IncludeQualifiers") == 0)
      {
	 XmlReader::getBooleanValueElement(parser, includeQualifiers, true);
	 duplicateParameter = gotIncludeQualifiers;
	 gotIncludeQualifiers = true;
      }
      else if (CompareNoCase(name, "IncludeClassOrigin") == 0)
      {
	 XmlReader::getBooleanValueElement(parser, includeClassOrigin, true);
	 duplicateParameter = gotIncludeClassOrigin;
	 gotIncludeClassOrigin = true;
      }
      else if (CompareNoCase(name, "PropertyList") == 0)
      {
	 CIMValue pl;
	 if (XmlReader::getValueArrayElement(parser, CIMTYPE_STRING, pl))
	 {
	    Array<String> propertyListArray;
	    pl.get(propertyListArray);
            Array<CIMName> cimNameArray;
            for (Uint32 i = 0; i < propertyListArray.size(); i++)
            {
                cimNameArray.append(propertyListArray[i]);
            }
	    propertyList.set(cimNameArray);
	 }
	 duplicateParameter = gotPropertyList;
	 gotPropertyList = true;
      }
      else
      {
	 throw CIMException(CIM_ERR_NOT_SUPPORTED);
      }

      XmlReader::expectEndTag(parser, "IPARAMVALUE");

      if (duplicateParameter)
      {
	 throw CIMException(CIM_ERR_INVALID_PARAMETER);
      }
   }

   if (!gotObjectName)
   {
      throw CIMException(CIM_ERR_INVALID_PARAMETER);
   }

   CIMAssociatorsRequestMessage* request = 
      new CIMAssociatorsRequestMessage(
	 messageId,
	 nameSpace,
	 objectName,
	 assocClass,
	 resultClass,
	 role,
	 resultRole,
	 includeQualifiers,
	 includeClassOrigin,
	 propertyList,
	 QueueIdStack(queueId, _returnQueueId),
	 authType,
	 userName);

   STAT_SERVERSTART

   return(request);
}

CIMGetPropertyRequestMessage* CIMOperationRequestDecoder::decodeGetPropertyRequest(
   Uint32 queueId,
   XmlParser& parser, 
   const String& messageId,
   const String& nameSpace,
   const String& authType,
   const String& userName)
{
   STAT_GETSTARTTIME

   CIMObjectPath instanceName;
   String propertyName;
   Boolean duplicateParameter = false;
   Boolean gotInstanceName = false;
   Boolean gotPropertyName = false;

   for (const char* name; XmlReader::getIParamValueTag(parser, name);)
   {
      if (CompareNoCase(name, "InstanceName") == 0)
      {
	 XmlReader::getInstanceNameElement(parser, instanceName);
	 duplicateParameter = gotInstanceName;
	 gotInstanceName = true;
      }
      else if (CompareNoCase(name, "PropertyName") == 0)
      {
	 XmlReader::getStringValueElement(parser, propertyName, true);
	 duplicateParameter = gotPropertyName;
	 gotPropertyName = true;
      }
      else
      {
	 throw CIMException(CIM_ERR_NOT_SUPPORTED);
      }

      XmlReader::expectEndTag(parser, "IPARAMVALUE");

      if (duplicateParameter)
      {
	 throw CIMException(CIM_ERR_INVALID_PARAMETER);
      }
   }

   if (!gotInstanceName || !gotPropertyName)
   {
      throw CIMException(CIM_ERR_INVALID_PARAMETER);
   }

   CIMGetPropertyRequestMessage* request = new CIMGetPropertyRequestMessage(
      messageId,
      nameSpace,
      instanceName,
      propertyName,
      QueueIdStack(queueId, _returnQueueId),
      authType,
      userName);

   STAT_SERVERSTART

   return(request);
}

CIMSetPropertyRequestMessage* CIMOperationRequestDecoder::decodeSetPropertyRequest(
   Uint32 queueId,
   XmlParser& parser, 
   const String& messageId,
   const String& nameSpace,
   const String& authType,
   const String& userName)
{
   STAT_GETSTARTTIME

   CIMObjectPath instanceName;
   String propertyName;
   CIMValue propertyValue;
   Boolean duplicateParameter = false;
   Boolean gotInstanceName = false;
   Boolean gotPropertyName = false;
   Boolean gotNewValue = false;

   for (const char* name; XmlReader::getIParamValueTag(parser, name);)
   {
      if (CompareNoCase(name, "InstanceName") == 0)
      {
	 XmlReader::getInstanceNameElement(parser, instanceName);
	 duplicateParameter = gotInstanceName;
	 gotInstanceName = true;
      }
      else if (CompareNoCase(name, "PropertyName") == 0)
      {
	 XmlReader::getStringValueElement(parser, propertyName, true);
	 duplicateParameter = gotPropertyName;
	 gotPropertyName = true;
      }
      else if (CompareNoCase(name, "NewValue") == 0)
      {
	 if (!XmlReader::getPropertyValue(parser, propertyValue))
	 {
	    propertyValue.setNullValue(CIMTYPE_STRING, false);
	 }
	 duplicateParameter = gotNewValue;
	 gotNewValue = true;
      }
      else
      {
	 throw CIMException(CIM_ERR_NOT_SUPPORTED);
      }

      XmlReader::expectEndTag(parser, "IPARAMVALUE");

      if (duplicateParameter)
      {
	 throw CIMException(CIM_ERR_INVALID_PARAMETER);
      }
   }

   if (!gotInstanceName || !gotPropertyName)
   {
      throw CIMException(CIM_ERR_INVALID_PARAMETER);
   }

   CIMSetPropertyRequestMessage* request = new CIMSetPropertyRequestMessage(
      messageId,
      nameSpace,
      instanceName,
      propertyName,
      propertyValue,
      QueueIdStack(queueId, _returnQueueId),
      authType,
      userName);

   STAT_SERVERSTART

   return(request);
}

CIMExecQueryRequestMessage* CIMOperationRequestDecoder::decodeExecQueryRequest(
   Uint32 queueId,
   XmlParser& parser, 
   const String& messageId,
   const String& nameSpace,
   const String& authType,
   const String& userName)
{
   STAT_GETSTARTTIME

   String queryLanguage;
   String query;
   Boolean duplicateParameter = false;
   Boolean gotQueryLanguage = false;
   Boolean gotQuery = false;

   for (const char* name; XmlReader::getIParamValueTag(parser, name);)
   {
      if (CompareNoCase(name, "QueryLanguage") == 0)
      {
	 XmlReader::getStringValueElement(parser, queryLanguage, true);
	 duplicateParameter = gotQueryLanguage;
	 gotQueryLanguage = true;
      }
      else if (CompareNoCase(name, "Query") == 0)
      {
	 XmlReader::getStringValueElement(parser, query, true);
	 duplicateParameter = gotQuery;
	 gotQuery = true;
      }
      else
      {
	 throw CIMException(CIM_ERR_NOT_SUPPORTED);
      }

      XmlReader::expectEndTag(parser, "IPARAMVALUE");

      if (duplicateParameter)
      {
	 throw CIMException(CIM_ERR_INVALID_PARAMETER);
      }
   }

   if (!gotQueryLanguage || !gotQuery)
   {
      throw CIMException(CIM_ERR_INVALID_PARAMETER);
   }

   CIMExecQueryRequestMessage* request = 
      new CIMExecQueryRequestMessage(
	 messageId,
	 nameSpace,
	 queryLanguage,
	 query,
	 QueueIdStack(queueId, _returnQueueId),
	 authType,
	 userName);

   STAT_SERVERSTART

   return(request);
}

CIMInvokeMethodRequestMessage* CIMOperationRequestDecoder::decodeInvokeMethodRequest(
   Uint32 queueId,
   XmlParser& parser, 
   const String& messageId,
   const CIMObjectPath& reference,
   const String& cimMethodName,
   const String& authType,
   const String& userName)
{
   STAT_GETSTARTTIME

   CIMParamValue paramValue;
   Array<CIMParamValue> inParameters;
    
   while (XmlReader::getParamValueElement(parser, paramValue))
   {
      inParameters.append(paramValue);
   }

   CIMInvokeMethodRequestMessage* request =     
      new CIMInvokeMethodRequestMessage(
	 messageId, 
	 reference.getNameSpace(), 
	 reference, 
	 cimMethodName,
	 inParameters,
	 QueueIdStack(queueId, _returnQueueId),
	 authType,
	 userName);

   STAT_SERVERSTART

   return(request);
}

void CIMOperationRequestDecoder::setServerTerminating(Boolean flag)
{
   _serverTerminating = flag;
}

PEGASUS_NAMESPACE_END
