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
// Author:  Nag Boranna, Hewlett-Packard Company (nagaraja_boranna@hp.com)
//
// Modified By: Sushma Fernandes, Hewlett-Packard Company
//		(sushma_fernandes@hp.com)
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                (carolann_graves@hp.com)
//              Yi Zhou, Hewlett-Packard Company (yi_zhou@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Security/UserManager/UserManager.h>
#include <Pegasus/Common/HTTPMessage.h>
#include <Pegasus/Common/Destroyer.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/Tracer.h>
#include "CIMOperationRequestAuthorizer.h"

PEGASUS_NAMESPACE_BEGIN

PEGASUS_USING_STD;


CIMOperationRequestAuthorizer::CIMOperationRequestAuthorizer(
   MessageQueueService* outputQueue)
   :
   Base(PEGASUS_QUEUENAME_OPREQAUTHORIZER),
   _outputQueue(outputQueue),
   _serverTerminating(false)
{
   PEG_METHOD_ENTER(TRC_SERVER, "CIMOperationRequestAuthorizer::"
                    "CIMOperationRequestAuthorizer");

   PEG_METHOD_EXIT();
}

CIMOperationRequestAuthorizer::~CIMOperationRequestAuthorizer()
{
   PEG_METHOD_ENTER(TRC_SERVER, "CIMOperationRequestAuthorizer::"
                    "~CIMOperationRequestAuthorizer");

   PEG_METHOD_EXIT();
}

void CIMOperationRequestAuthorizer::sendResponse(
   Uint32 queueId,
   Array<Sint8>& message)
{
   PEG_METHOD_ENTER(TRC_SERVER, "CIMOperationRequestAuthorizer::sendResponse");

   MessageQueue* queue = MessageQueue::lookup(queueId);

   if (queue)
   {
      HTTPMessage* httpMessage = new HTTPMessage(message);
      queue->enqueue(httpMessage);
   }
   PEG_METHOD_EXIT();
}

// Code is duplicated in CIMOperationRequestDecoder
void CIMOperationRequestAuthorizer::sendIMethodError(
   Uint32 queueId,
   HttpMethod httpMethod,
   const String& messageId,
   const CIMName& iMethodName,
   const CIMException& cimException)
{
    PEG_METHOD_ENTER(TRC_SERVER,
                     "CIMOperationRequestAuthorizer::sendIMethodError");

    Array<Sint8> message;
    message = XmlWriter::formatSimpleIMethodErrorRspMessage(
        iMethodName,
        messageId,
        httpMethod,
        cimException);

    sendResponse(queueId, message);

    PEG_METHOD_EXIT();
}

////////////////////////////////////////////////////////////////////////////////


void CIMOperationRequestAuthorizer::handleEnqueue(Message *request)
{

   PEG_METHOD_ENTER(TRC_SERVER, "CIMOperationRequestAuthorizer::handleEnqueue");

   if (!request)
   {
      PEG_METHOD_EXIT();
      return;
   }

   //
   // Get the HTTPConnection queue id
   //
   QueueIdStack qis = ((CIMRequestMessage*)request)->queueIds.copyAndPop();

   Uint32 queueId = qis.top();

   //
   // If CIMOM is shutting down, return "Service Unavailable" response
   //
   if (_serverTerminating)
   {
       Array<Sint8> message;
       message = XmlWriter::formatHttpErrorRspMessage(
           HTTP_STATUS_SERVICEUNAVAILABLE,
           String::EMPTY,
           "CIM Server is shutting down.");

       sendResponse(queueId, message);
       PEG_METHOD_EXIT();
       return;
   }

   String userName = String::EMPTY;
   String authType = String::EMPTY;
   CIMNamespaceName nameSpace;
   String cimMethodName = String::EMPTY;

   switch (request->getType())
   {
      case CIM_GET_CLASS_REQUEST_MESSAGE:
	 userName = ((CIMGetClassRequestMessage*)request)->userName;
	 authType = 
	    ((CIMGetClassRequestMessage*)request)->authType;
	 nameSpace = ((CIMGetClassRequestMessage*)request)->nameSpace;
	 cimMethodName = "GetClass";
	 break;

      case CIM_GET_INSTANCE_REQUEST_MESSAGE:
	 userName = ((CIMGetInstanceRequestMessage*)request)->userName;
	 authType = 
	    ((CIMGetInstanceRequestMessage*)request)->authType;
	 nameSpace = ((CIMGetInstanceRequestMessage*)request)->nameSpace;
	 cimMethodName = "GetInstance";
	 break;

      case CIM_DELETE_CLASS_REQUEST_MESSAGE:
	 userName = ((CIMDeleteClassRequestMessage*)request)->userName;
	 authType = 
	    ((CIMDeleteClassRequestMessage*)request)->authType;
	 nameSpace = ((CIMDeleteClassRequestMessage*)request)->nameSpace;
	 cimMethodName = "DeleteClass";
	 break;

      case CIM_DELETE_INSTANCE_REQUEST_MESSAGE:
	 userName = ((CIMDeleteInstanceRequestMessage*)request)->userName;
	 authType = 
	    ((CIMDeleteInstanceRequestMessage*)request)->authType;
	 nameSpace = ((CIMDeleteInstanceRequestMessage*)request)->nameSpace;
	 cimMethodName = "DeleteInstance";
	 break;

      case CIM_CREATE_CLASS_REQUEST_MESSAGE:
	 userName = ((CIMCreateClassRequestMessage*)request)->userName;
	 authType = 
	    ((CIMCreateClassRequestMessage*)request)->authType;
	 nameSpace = ((CIMCreateClassRequestMessage*)request)->nameSpace;
	 cimMethodName = "CreateClass";
	 break;

      case CIM_CREATE_INSTANCE_REQUEST_MESSAGE:
	 userName = ((CIMCreateInstanceRequestMessage*)request)->userName;
	 authType = 
	    ((CIMCreateInstanceRequestMessage*)request)->authType;
	 nameSpace = ((CIMCreateInstanceRequestMessage*)request)->nameSpace;
	 cimMethodName = "CreateInstance";
	 break;

      case CIM_MODIFY_CLASS_REQUEST_MESSAGE:
	 userName = ((CIMModifyClassRequestMessage*)request)->userName;
	 authType = 
	    ((CIMModifyClassRequestMessage*)request)->authType;
	 nameSpace = ((CIMModifyClassRequestMessage*)request)->nameSpace;
	 cimMethodName = "ModifyClass";
	 break;

      case CIM_MODIFY_INSTANCE_REQUEST_MESSAGE:
	 userName = ((CIMModifyInstanceRequestMessage*)request)->userName;
	 authType = 
	    ((CIMModifyInstanceRequestMessage*)request)->authType;
	 nameSpace = ((CIMModifyInstanceRequestMessage*)request)->nameSpace;
	 cimMethodName = "ModifyInstance";
	 break;

      case CIM_ENUMERATE_CLASSES_REQUEST_MESSAGE:
	 userName = ((CIMEnumerateClassesRequestMessage*)request)->userName;
	 authType = ((CIMEnumerateClassesRequestMessage*)request)->authType;
	 nameSpace = ((CIMEnumerateClassesRequestMessage*)request)->nameSpace;
	 cimMethodName = "EnumerateClasses";
	 break;

      case CIM_ENUMERATE_CLASS_NAMES_REQUEST_MESSAGE:
	 userName = ((CIMEnumerateClassNamesRequestMessage*)request)->userName;
	 authType = 
	    ((CIMEnumerateClassNamesRequestMessage*)request)->authType;
	 nameSpace = ((CIMEnumerateClassNamesRequestMessage*)request)->nameSpace;
	 cimMethodName = "EnumerateClassNames";
	 break;

      case CIM_ENUMERATE_INSTANCES_REQUEST_MESSAGE:
	 userName = ((CIMEnumerateInstancesRequestMessage*)request)->userName;
	 authType = ((CIMEnumerateInstancesRequestMessage*)request)->authType;
	 nameSpace = ((CIMEnumerateInstancesRequestMessage*)request)->nameSpace;
	 cimMethodName = "EnumerateInstances";
	 break;

      case CIM_ENUMERATE_INSTANCE_NAMES_REQUEST_MESSAGE:
	 userName = ((CIMEnumerateInstanceNamesRequestMessage*)request)->userName;
	 authType = ((CIMEnumerateInstanceNamesRequestMessage*)request)->authType;
	 nameSpace = ((CIMEnumerateInstanceNamesRequestMessage*)request)->nameSpace;
	 cimMethodName = "EnumerateInstanceNames";
	 break;

      case CIM_EXEC_QUERY_REQUEST_MESSAGE:
	 userName = ((CIMExecQueryRequestMessage*)request)->userName;
	 authType = ((CIMExecQueryRequestMessage*)request)->authType;
	 nameSpace = ((CIMExecQueryRequestMessage*)request)->nameSpace;
	 cimMethodName = "ExecQuery";
	 break;

      case CIM_ASSOCIATORS_REQUEST_MESSAGE:
	 userName = ((CIMAssociatorsRequestMessage*)request)->userName;
	 authType = ((CIMAssociatorsRequestMessage*)request)->authType;
	 nameSpace = ((CIMAssociatorsRequestMessage*)request)->nameSpace;
	 cimMethodName = "Associators";
	 break;

      case CIM_ASSOCIATOR_NAMES_REQUEST_MESSAGE:
	 userName = ((CIMAssociatorNamesRequestMessage*)request)->userName;
	 authType = ((CIMAssociatorNamesRequestMessage*)request)->authType;
	 nameSpace = ((CIMAssociatorNamesRequestMessage*)request)->nameSpace;
	 cimMethodName = "AssociatorNames";
	 break;

      case CIM_REFERENCES_REQUEST_MESSAGE:
	 userName = ((CIMReferencesRequestMessage*)request)->userName;
	 authType = ((CIMReferencesRequestMessage*)request)->authType;
	 nameSpace = ((CIMReferencesRequestMessage*)request)->nameSpace;
	 cimMethodName = "References";
	 break;

      case CIM_REFERENCE_NAMES_REQUEST_MESSAGE:
	 userName = ((CIMReferenceNamesRequestMessage*)request)->userName;
	 authType = ((CIMReferenceNamesRequestMessage*)request)->authType;
	 nameSpace = ((CIMReferenceNamesRequestMessage*)request)->nameSpace;
	 cimMethodName = "ReferenceNames";
	 break;

      case CIM_GET_PROPERTY_REQUEST_MESSAGE:
	 userName = ((CIMGetPropertyRequestMessage*)request)->userName;
	 authType = ((CIMGetPropertyRequestMessage*)request)->authType;
	 nameSpace = ((CIMGetPropertyRequestMessage*)request)->nameSpace;
	 cimMethodName = "GetProperty";
	 break;

      case CIM_SET_PROPERTY_REQUEST_MESSAGE:
	 userName = ((CIMSetPropertyRequestMessage*)request)->userName;
	 authType = ((CIMSetPropertyRequestMessage*)request)->authType;
	 nameSpace = ((CIMSetPropertyRequestMessage*)request)->nameSpace;
	 cimMethodName = "SetProperty";
	 break;

      case CIM_GET_QUALIFIER_REQUEST_MESSAGE:
	 userName = ((CIMGetQualifierRequestMessage*)request)->userName;
	 authType = ((CIMGetQualifierRequestMessage*)request)->authType;
	 nameSpace = ((CIMGetQualifierRequestMessage*)request)->nameSpace;
	 cimMethodName = "GetQualifier";
	 break;

      case CIM_SET_QUALIFIER_REQUEST_MESSAGE:
	 userName = ((CIMSetQualifierRequestMessage*)request)->userName;
	 authType = ((CIMSetQualifierRequestMessage*)request)->authType;
	 nameSpace = ((CIMSetQualifierRequestMessage*)request)->nameSpace;
	 cimMethodName = "SetQualifier";
	 break;

      case CIM_DELETE_QUALIFIER_REQUEST_MESSAGE:
	 userName = ((CIMDeleteQualifierRequestMessage*)request)->userName;
	 authType = ((CIMDeleteQualifierRequestMessage*)request)->authType;
	 nameSpace = ((CIMDeleteQualifierRequestMessage*)request)->nameSpace;
	 cimMethodName = "DeleteQualifier";
	 break;

      case CIM_ENUMERATE_QUALIFIERS_REQUEST_MESSAGE:
	 userName = ((CIMEnumerateQualifiersRequestMessage*)request)->userName;
	 authType = ((CIMEnumerateQualifiersRequestMessage*)request)->authType;
	 nameSpace = ((CIMEnumerateQualifiersRequestMessage*)request)->nameSpace;
	 cimMethodName = "EnumerateQualifiers";
	 break;

      case CIM_INVOKE_METHOD_REQUEST_MESSAGE:
	 userName = ((CIMInvokeMethodRequestMessage*)request)->userName;
	 authType = ((CIMInvokeMethodRequestMessage*)request)->authType;
	 nameSpace = ((CIMInvokeMethodRequestMessage*)request)->nameSpace;
	 cimMethodName = "InvokeMethod";
	 break;

      default:
	 break;
   }

   //
   // Get a config manager instance
   //
   ConfigManager* configManager = ConfigManager::getInstance();

   //
   // Do namespace authorization verification
   //
   if (String::equalNoCase(
          configManager->getCurrentValue("enableNamespaceAuthorization"),
          "true"))
   {
      //
      // If the user is not privileged, perform the authorization check.
      //
#if !defined(PEGASUS_PLATFORM_OS400_ISERIES_IBM)
      if ( ! System::isPrivilegedUser(userName) )
#else
      // On OS/400, always check authorization if remote user.
      // Always allow local privileged users through.
      // Check authorization for local non-privileged users.
      // (User authorization to providers are checked downstream from here).
      if ( ! String::equalNoCase(authType,"Local") ||
           ! System::isPrivilegedUser(userName) )
#endif
      {
         UserManager* userManager = UserManager::getInstance();

         if ( !userManager || !userManager->verifyAuthorization(
                 userName, nameSpace, cimMethodName) )
         {
            String description = "Not authorized to run ";
            description.append(cimMethodName);
            description.append(" in the namespace ");
            description.append(nameSpace.getString());

            sendIMethodError(
               queueId,
               request->getHttpMethod(),
               ((CIMRequestMessage*)request)->messageId,
               cimMethodName,
               PEGASUS_CIM_EXCEPTION(CIM_ERR_ACCESS_DENIED, description));

            PEG_METHOD_EXIT();

            return;
         }
      }
   }

   //
   // If the user is privileged, and remote privileged user access is not 
   // enabled and the auth type is not Local then reject access.
   //
   if ( System::isPrivilegedUser(userName) &&
        !String::equalNoCase(authType, "Local") &&
        !String::equalNoCase(
           configManager->getCurrentValue("enableRemotePrivilegedUserAccess"),
           "true") )
   {
      String description =
	 "Remote privileged user access is not enabled.";

      sendIMethodError(
	 queueId,
         request->getHttpMethod(),
	 ((CIMRequestMessage*)request)->messageId,
	 cimMethodName,
	 PEGASUS_CIM_EXCEPTION(CIM_ERR_ACCESS_DENIED, description));

      PEG_METHOD_EXIT();

      return;
   }

   //
   // Enqueue the request
   //
   _outputQueue->enqueue(request);

   PEG_METHOD_EXIT();

}


void CIMOperationRequestAuthorizer::handleEnqueue()
{
   PEG_METHOD_ENTER(TRC_SERVER, "CIMOperationRequestAuthorizer::handleEnqueue");

   Message* request = dequeue();
   if( request )
      handleEnqueue(request);

   PEG_METHOD_EXIT();
}

void CIMOperationRequestAuthorizer::setServerTerminating(Boolean flag)
{
   PEG_METHOD_ENTER(TRC_SERVER,
                    "CIMOperationRequestAuthorizer::setServerTerminating");

   _serverTerminating = flag;

   PEG_METHOD_EXIT();
}

PEGASUS_NAMESPACE_END
