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
//              Amit K Arora, IBM (amita@in.ibm.com) for PEP#101
//              Seema Gupta (gseema@in.ibm.com) for PEP135
//         Brian G. Campbell, EMC (campbell_brian@emc.com) - PEP140/phase1
//				Willis White (whiwill@us.ibm.com) PEP 127 and 128
//         Brian G. Campbell, EMC (campbell_brian@emc.com) - PEP140/phase2
//              David Dillard, VERITAS Software Corp.
//                 (david.dillard@veritas.com)
//              John Alex, IBM (johnalex@us.ibm.com) - Bug#2290
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Constants.h>
#include <cctype>
#include <cstdio>
#include <Pegasus/Common/HTTPConnection.h>
#include <Pegasus/Common/XmlParser.h>
#include <Pegasus/Common/XmlReader.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/HTTPMessage.h>
#include <Pegasus/Common/Logger.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/StatisticalData.h>
#include <Pegasus/Common/AutoPtr.h>
#include "CIMOperationResponseEncoder.h"

// l10n
#include <Pegasus/Common/MessageLoader.h>

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

void 
CIMOperationResponseEncoder::sendResponse(CIMResponseMessage* response,
																					const String &name,
																					Boolean isImplicit,
																					Array<char> *bodygiven)
{
	static String funcname = "CIMOperationResponseEncoder::sendResponse: ";
	String funcnameClassS = String(funcname + "for class " + name);
	CString funcnameClass = funcnameClassS.getCString();
	PEG_METHOD_ENTER(TRC_DISPATCHER, funcnameClass);

	if (! response)
	{
		PEG_METHOD_EXIT();
		return;
	}

	Uint32 queueId = response->queueIds.top();
	response->queueIds.pop();

        Boolean closeConnect = response->getCloseConnect();
        Tracer::trace(
            TRC_HTTP,
            Tracer::LEVEL3,
            "CIMOperationResponseEncoder::sendResponse()- response->getCloseConnect() returned %d",
            closeConnect);

	MessageQueue* queue = MessageQueue::lookup(queueId);

	if (!queue)
	{
		Tracer::trace(TRC_DISCARDED_DATA, Tracer::LEVEL2, 
									"ERROR: non-existent queueId = %u, response not sent.", queueId);
		PEG_METHOD_EXIT();
		return;
	}

	HTTPConnection *httpQueue = dynamic_cast<HTTPConnection *>(queue);

	if (! httpQueue)
	{
		Tracer::trace(TRC_DISCARDED_DATA, Tracer::LEVEL2, 
									"ERROR: Unknown queue type. queueId = %u, response not sent.",
									queueId);
		PEG_METHOD_EXIT();
		return;
	}

	Boolean isChunkRequest = httpQueue->isChunkRequested();
	HttpMethod httpMethod = response->getHttpMethod();
	String &messageId = response->messageId;
	CIMException &cimException = response->cimException;
	Array<char> message;

	// Note: the language is ALWAYS passed empty to the xml formatters because
	// it is HTTPConnection that needs to make the decision of whether to add
	// the languages to the HTTP message.
	ContentLanguages contentLanguage;

	CIMName cimName(name);
	Uint32 messageIndex = response->getIndex();
	Boolean isFirst = messageIndex == 0 ? true : false;
	Boolean isLast = response->isComplete();
	Array<char> bodylocal;
	Array<char> &body = bodygiven ? *bodygiven : bodylocal;

    STAT_SERVEREND         // STAT_SERVEREND sets the toServerTime value in the message class
    Uint32 serverTime	= response->totServerTime;


	Array<char> (*formatResponse)(const CIMName& iMethodName,
																 const String& messageId,
																 HttpMethod httpMethod,
																 const ContentLanguages &httpContentLanguages, 
																 const Array<char>& body,
																 Uint32 serverResponseTime,
																 Boolean isFirst,
																 Boolean isLast);

	Array<char> (*formatError)(const CIMName& methodName,
															const String& messageId,
															HttpMethod httpMethod,
															const CIMException& cimException);

	if (isImplicit == false)
	{
		formatResponse = XmlWriter::formatSimpleMethodRspMessage;
		formatError = XmlWriter::formatSimpleMethodErrorRspMessage;
	}
	else
	{
		formatResponse = XmlWriter::formatSimpleIMethodRspMessage;
 		formatError = XmlWriter::formatSimpleIMethodErrorRspMessage;
	}

	if (cimException.getCode() != CIM_ERR_SUCCESS)
	{
		STAT_SERVEREND_ERROR

		// only process the FIRST error
		if (httpQueue->cimException.getCode() == CIM_ERR_SUCCESS)
		{
			// NOTE: even if this error occurs in the middle, HTTPConnection will
			// flush the entire queued message and reformat.
			if (isChunkRequest == false)
				message = formatError(name, messageId, httpMethod, cimException);

			// uri encode the error (for the http header) only when it is 
			// non-chunking or the first error with chunking
			if (isChunkRequest == false || (isChunkRequest == true && isFirst == true))
			{
				String msg = TraceableCIMException(cimException).getDescription();
				String uriEncodedMsg = XmlWriter::encodeURICharacters(msg);
				CIMException cimExceptionUri(cimException.getCode(), uriEncodedMsg);
				cimExceptionUri.setContentLanguages(cimException.getContentLanguages());
				cimException = cimExceptionUri;
			}

		} // if first error in response stream

		// never put the error in chunked response (because it will end up in
		// the trailer), so just use the non-error response formatter to send
		// more data

		if (isChunkRequest == true)
		{
			message = formatResponse(cimName, messageId, httpMethod, contentLanguage,
															 body, serverTime, isFirst, isLast);
		}
	}
	else
	{
		// else non-error condition
	  	try
		{
			message = formatResponse(cimName, messageId, httpMethod, contentLanguage,
															 body, serverTime, isFirst, isLast);
		}

#ifdef PEGASUS_PLATFORM_WIN32_IX86_MSVC
   catch (std::bad_alloc&)
#else
   catch (bad_alloc&)
#endif
	 {
		 Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
								 funcname + OUT_OF_MEMORY_MESSAGE);

		 cimException = PEGASUS_CIM_EXCEPTION_L
			 (CIM_ERR_FAILED,
				MessageLoaderParms("Server.CIMOperationResponseEncoder.OUT_OF_MEMORY", 
													 OUT_OF_MEMORY_MESSAGE));
		 // try again with new error and no body
		 body.clear();
		 sendResponse(response, name, isImplicit);
		 return;
	 }
	 
	 STAT_BYTESSENT

	}

	AutoPtr<HTTPMessage> httpMessage(new HTTPMessage(message, 0, &cimException));
	httpMessage->setComplete(isLast);
	httpMessage->setIndex(messageIndex);

	if (cimException.getCode() != CIM_ERR_SUCCESS)
	{
		httpMessage->contentLanguages = cimException.getContentLanguages();
	}
	else
	{
		const OperationContext::Container &container = 
			response->operationContext.get(ContentLanguageListContainer::NAME);
		const ContentLanguageListContainer &listContainer =
			*dynamic_cast<const ContentLanguageListContainer *>(&container);
		contentLanguage = listContainer.getLanguages();
		httpMessage->contentLanguages = contentLanguage;
	}

        Tracer::traceBuffer(
            TRC_XML_IO,
            Tracer::LEVEL2,
            httpMessage->message.getData(),
            httpMessage->message.size());
	
        Logger::put(
            Logger::STANDARD_LOG,
            System::CIMSERVER,
            Logger::TRACE,
            "CIMOperationResponseEncoder::sendResponse - QueueId: $0  "
            "XML content: $1",queueId,
            String(message.getData(), message.size()));
	
        httpMessage->setCloseConnect(closeConnect);
	queue->enqueue(httpMessage.release());
	
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

   Tracer::trace(
       TRC_HTTP,
       Tracer::LEVEL3,
       "CIMOperationResponseEncoder::handleEnque()- message->getCloseConnect() returned %d",
       message->getCloseConnect());

   
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

// l10n - added content language support below

void CIMOperationResponseEncoder::encodeCreateClassResponse(
   CIMCreateClassResponseMessage* response)
{
	sendResponse(response, "CreateClass", true);
}

void CIMOperationResponseEncoder::encodeGetClassResponse(
   CIMGetClassResponseMessage* response)
{
	Array<char> body;
	if (response->cimException.getCode() == CIM_ERR_SUCCESS)
		XmlWriter::appendClassElement(body, response->cimClass);
	sendResponse(response, "GetClass", true, &body);
}

void CIMOperationResponseEncoder::encodeModifyClassResponse(
   CIMModifyClassResponseMessage* response)
{
	sendResponse(response, "ModifyClass", true);
}

void CIMOperationResponseEncoder::encodeEnumerateClassNamesResponse(
   CIMEnumerateClassNamesResponseMessage* response)
{
	Array<char> body;
	if (response->cimException.getCode() == CIM_ERR_SUCCESS)
		for (Uint32 i = 0, n = response->classNames.size(); i < n; i++)
			XmlWriter::appendClassNameElement(body, response->classNames[i]);
	sendResponse(response, "EnumerateClassNames", true, &body);
}

void CIMOperationResponseEncoder::encodeEnumerateClassesResponse(
   CIMEnumerateClassesResponseMessage* response)
{
	Array<char> body;
	if (response->cimException.getCode() == CIM_ERR_SUCCESS)
		for (Uint32 i = 0, n= response->cimClasses.size(); i < n; i++)
			XmlWriter::appendClassElement(body, response->cimClasses[i]);
	sendResponse(response, "EnumerateClasses", true, &body);
}

void CIMOperationResponseEncoder::encodeDeleteClassResponse(
   CIMDeleteClassResponseMessage* response)
{
	sendResponse(response, "DeleteClass", true);
}

void CIMOperationResponseEncoder::encodeCreateInstanceResponse(
   CIMCreateInstanceResponseMessage* response)
{
   Array<char> body;
	 if (response->cimException.getCode() == CIM_ERR_SUCCESS)
		 XmlWriter::appendInstanceNameElement(body, response->instanceName);
	 sendResponse(response, "CreateInstance", true, &body);
}

void CIMOperationResponseEncoder::encodeGetInstanceResponse(
   CIMGetInstanceResponseMessage* response)
{
   Array<char> body;
	 if (response->cimException.getCode() == CIM_ERR_SUCCESS)
		 XmlWriter::appendInstanceElement(body, response->cimInstance);
	 sendResponse(response, "GetInstance", true, &body);
}

void CIMOperationResponseEncoder::encodeModifyInstanceResponse(
   CIMModifyInstanceResponseMessage* response)
{
	sendResponse(response, "ModifyInstance", true);
}

void CIMOperationResponseEncoder::encodeEnumerateInstancesResponse(
   CIMEnumerateInstancesResponseMessage* response)
{
	Array<char> body;
	if (response->cimException.getCode() == CIM_ERR_SUCCESS)
		for (Uint32 i = 0, n = response->cimNamedInstances.size(); i < n; i++)
			XmlWriter::appendValueNamedInstanceElement(body, response->cimNamedInstances[i]);
	sendResponse(response, "EnumerateInstances", true, &body);
}

void CIMOperationResponseEncoder::encodeEnumerateInstanceNamesResponse(
   CIMEnumerateInstanceNamesResponseMessage* response)
{
   Array<char> body;
	 if (response->cimException.getCode() == CIM_ERR_SUCCESS)
		 for (Uint32 i = 0, n = response->instanceNames.size(); i < n; i++)
			 XmlWriter::appendInstanceNameElement(body, response->instanceNames[i]);
	 sendResponse(response, "EnumerateInstanceNames", true, &body);
}

void CIMOperationResponseEncoder::encodeDeleteInstanceResponse(
   CIMDeleteInstanceResponseMessage* response)
{
	sendResponse(response, "DeleteInstance", true);
}

void CIMOperationResponseEncoder::encodeGetPropertyResponse(
   CIMGetPropertyResponseMessage* response)
{
	Array<char> body;
	if (response->cimException.getCode() == CIM_ERR_SUCCESS)
		XmlWriter::appendValueElement(body, response->value);
	sendResponse(response, "GetProperty", true, &body);
}

void CIMOperationResponseEncoder::encodeSetPropertyResponse(
   CIMSetPropertyResponseMessage* response)
{
	sendResponse(response, "SetProperty", true);
}

void CIMOperationResponseEncoder::encodeSetQualifierResponse(
   CIMSetQualifierResponseMessage* response)
{
	sendResponse(response, "SetQualifier", true);
}

void CIMOperationResponseEncoder::encodeGetQualifierResponse(
   CIMGetQualifierResponseMessage* response)
{
	Array<char> body;
	if (response->cimException.getCode() == CIM_ERR_SUCCESS)
		XmlWriter::appendQualifierDeclElement(body, response->cimQualifierDecl);
	sendResponse(response, "GetQualifier", true, &body);
}

void CIMOperationResponseEncoder::encodeEnumerateQualifiersResponse(
   CIMEnumerateQualifiersResponseMessage* response)
{
   Array<char> body;
	 if (response->cimException.getCode() == CIM_ERR_SUCCESS)
		 for (Uint32 i = 0, n = response->qualifierDeclarations.size(); i < n;i++)
			 XmlWriter::appendQualifierDeclElement(body, response->qualifierDeclarations[i]);
	 sendResponse(response, "EnumerateQualifiers", true, &body);
}

void CIMOperationResponseEncoder::encodeDeleteQualifierResponse(
   CIMDeleteQualifierResponseMessage* response)
{
	sendResponse(response, "DeleteQualifier", true);
}

void CIMOperationResponseEncoder::encodeReferenceNamesResponse(
   CIMReferenceNamesResponseMessage* response)
{
   Array<char> body;
	 if (response->cimException.getCode() == CIM_ERR_SUCCESS)
		 for (Uint32 i = 0, n = response->objectNames.size(); i < n; i++)
		 {
			 body << "<OBJECTPATH>\n";
			 XmlWriter::appendValueReferenceElement(body, response->objectNames[i],
																							false);
			 body << "</OBJECTPATH>\n";
		 }
	 sendResponse(response, "ReferenceNames", true, &body);
}

void CIMOperationResponseEncoder::encodeReferencesResponse(
   CIMReferencesResponseMessage* response)
{
   Array<char> body;
	 if (response->cimException.getCode() == CIM_ERR_SUCCESS)
		 for (Uint32 i = 0, n = response->cimObjects.size(); i < n;i++)
			 XmlWriter::appendValueObjectWithPathElement(body, response->cimObjects[i]);
	 sendResponse(response, "References", true, &body);
}

void CIMOperationResponseEncoder::encodeAssociatorNamesResponse(
   CIMAssociatorNamesResponseMessage* response)
{
   Array<char> body;
	 if (response->cimException.getCode() == CIM_ERR_SUCCESS)
		 for (Uint32 i = 0, n = response->objectNames.size(); i < n; i++)
		 {
			 body << "<OBJECTPATH>\n";
			 XmlWriter::appendValueReferenceElement(body, response->objectNames[i], false);
			 body << "</OBJECTPATH>\n";
		 }
	 sendResponse(response, "AssociatorNames", true, &body);
}

void CIMOperationResponseEncoder::encodeAssociatorsResponse(
   CIMAssociatorsResponseMessage* response)
{
   Array<char> body;
	 if (response->cimException.getCode() == CIM_ERR_SUCCESS)
		 for (Uint32 i = 0, n = response->cimObjects.size(); i < n; i++)
			 XmlWriter::appendValueObjectWithPathElement(body, response->cimObjects[i]);
	 sendResponse(response, "Associators", true, &body);
}

void CIMOperationResponseEncoder::encodeExecQueryResponse(
   CIMExecQueryResponseMessage* response)
{
   Array<char> body;
	 if (response->cimException.getCode() == CIM_ERR_SUCCESS)
		 for (Uint32 i = 0; i < response->cimObjects.size(); i++)
			 XmlWriter::appendValueObjectWithPathElement(body, response->cimObjects[i]);
	 sendResponse(response, "ExecQuery", true, &body);
}

void CIMOperationResponseEncoder::encodeInvokeMethodResponse(
    CIMInvokeMethodResponseMessage* response)
{
   Array<char> body;

   // ATTN-RK-P3-20020219: Who's job is it to make sure the return value is
   // not an array?
   // Only add the return value if it is not null

	 if (response->cimException.getCode() == CIM_ERR_SUCCESS)
	 {
		 if (!response->retValue.isNull())
			 XmlWriter::appendReturnValueElement(body, response->retValue);
		 
		 for (Uint32 i=0, n = response->outParameters.size(); i < n; i++)
			 XmlWriter::appendParamValueElement(body, response->outParameters[i]);
	 }
	 sendResponse(response, response->methodName.getString(), false, &body);
}

PEGASUS_NAMESPACE_END
