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
// Author:  Nag Boranna, Hewlett-Packard Company (nagaraja_boranna@hp.com)
//
// Modified By: Sushma Fernandes, Hewlett-Packard Company
//		(sushma_fernandes@hp.com)
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                  (carolann_graves@hp.com)
//              Yi Zhou, Hewlett-Packard Company (yi_zhou@hp.com)
//              Amit K Arora, IBM (amita@in.ibm.com) for PEP#101
//				Seema Gupta (gseema@in.ibm.com) for PEP135
//              David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Security/UserManager/UserManager.h>
#include <Pegasus/Common/HTTPMessage.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/Tracer.h>
#include "CIMOperationRequestAuthorizer.h"

// l10n
#include <Pegasus/Common/MessageLoader.h>

PEGASUS_NAMESPACE_BEGIN

PEGASUS_USING_STD;

//
// Set group name separator
//
const char CIMOperationRequestAuthorizer::_GROUPNAME_SEPARATOR = ',';

CIMOperationRequestAuthorizer::CIMOperationRequestAuthorizer(
   MessageQueueService* outputQueue)
   :
   Base(PEGASUS_QUEUENAME_OPREQAUTHORIZER),
   _outputQueue(outputQueue),
   _serverTerminating(false)
{
   PEG_METHOD_ENTER(TRC_SERVER, "CIMOperationRequestAuthorizer::"
                    "CIMOperationRequestAuthorizer");

#ifdef PEGASUS_ENABLE_USERGROUP_AUTHORIZATION
   _authorizedUserGroups = _getAuthorizedUserGroups();
#endif

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
   Array<char>& message)
{
   PEG_METHOD_ENTER(TRC_SERVER, "CIMOperationRequestAuthorizer::sendResponse");

   MessageQueue* queue = MessageQueue::lookup(queueId);

   if (queue)
   {
      AutoPtr<HTTPMessage> httpMessage(new HTTPMessage(message));

      queue->enqueue(httpMessage.release());
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

    Array<char> message;
    message = XmlWriter::formatSimpleIMethodErrorRspMessage(
        iMethodName,
        messageId,
        httpMethod,
        cimException);

    sendResponse(queueId, message);

    PEG_METHOD_EXIT();
}

// Code is duplicated in CIMOperationRequestDecoder
void CIMOperationRequestAuthorizer::sendMethodError(
   Uint32 queueId,
   HttpMethod httpMethod,
   const String& messageId,
   const CIMName& methodName,
   const CIMException& cimException)
{
    PEG_METHOD_ENTER(TRC_SERVER,
                     "CIMOperationRequestAuthorizer::sendMethodError");

    Array<char> message;
    message = XmlWriter::formatSimpleMethodErrorRspMessage(
        methodName,
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

   AutoPtr<CIMOperationRequestMessage> 
         req(dynamic_cast<CIMOperationRequestMessage *>(request));

   PEGASUS_ASSERT(req.get());

   //
   // Get the HTTPConnection queue id
   //
   QueueIdStack qis = req->queueIds.copyAndPop();

   Uint32 queueId = qis.top();

// l10n
   // Set the client's requested language into this service thread.
   // This will allow functions in this service to return messages
   // in the correct language.
   if (req->thread_changed())
   {
       AutoPtr<AcceptLanguages> langs(new AcceptLanguages(((AcceptLanguageListContainer)req->operationContext.get
	   (AcceptLanguageListContainer:: NAME)).getLanguages()));	
       Thread::setLanguages(langs.release());   		
   }

   //
   // If CIMOM is shutting down, return "Service Unavailable" response
   //
   if (_serverTerminating)
   {
       Array<char> message;
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

   // Set the username and namespace.
   nameSpace = req->nameSpace;
   userName = ((IdentityContainer)(req->operationContext.get
			(IdentityContainer::NAME))).getUserName();

   switch (req->getType())
   {
      case CIM_GET_CLASS_REQUEST_MESSAGE:
	 authType = 
	    ((CIMGetClassRequestMessage*)req.get())->authType;
	 cimMethodName = "GetClass";
	 break;

      case CIM_GET_INSTANCE_REQUEST_MESSAGE:
	 authType = 
	    ((CIMGetInstanceRequestMessage*)req.get())->authType;
	 cimMethodName = "GetInstance";
	 break;

      case CIM_DELETE_CLASS_REQUEST_MESSAGE:
	 authType = 
	    ((CIMDeleteClassRequestMessage*)req.get())->authType;
	 cimMethodName = "DeleteClass";
	 break;

      case CIM_DELETE_INSTANCE_REQUEST_MESSAGE:
	 authType = 
	    ((CIMDeleteInstanceRequestMessage*)req.get())->authType;
	 cimMethodName = "DeleteInstance";
	 break;

      case CIM_CREATE_CLASS_REQUEST_MESSAGE:
	 authType = 
	    ((CIMCreateClassRequestMessage*)req.get())->authType;
	 cimMethodName = "CreateClass";
	 break;

      case CIM_CREATE_INSTANCE_REQUEST_MESSAGE:
	 authType = 
	    ((CIMCreateInstanceRequestMessage*)req.get())->authType;
	 cimMethodName = "CreateInstance";
	 break;

      case CIM_MODIFY_CLASS_REQUEST_MESSAGE:
	 authType = 
	    ((CIMModifyClassRequestMessage*)req.get())->authType;
	 cimMethodName = "ModifyClass";
	 break;

      case CIM_MODIFY_INSTANCE_REQUEST_MESSAGE:
	 authType = 
	    ((CIMModifyInstanceRequestMessage*)req.get())->authType;
	 cimMethodName = "ModifyInstance";
	 break;

      case CIM_ENUMERATE_CLASSES_REQUEST_MESSAGE:
	 authType = ((CIMEnumerateClassesRequestMessage*)req.get())->authType;
	 cimMethodName = "EnumerateClasses";
	 break;

      case CIM_ENUMERATE_CLASS_NAMES_REQUEST_MESSAGE:
	 authType = 
	    ((CIMEnumerateClassNamesRequestMessage*)req.get())->authType;
	 cimMethodName = "EnumerateClassNames";
	 break;

      case CIM_ENUMERATE_INSTANCES_REQUEST_MESSAGE:
	 authType = ((CIMEnumerateInstancesRequestMessage*)req.get())->authType;
	 cimMethodName = "EnumerateInstances";
	 break;

      case CIM_ENUMERATE_INSTANCE_NAMES_REQUEST_MESSAGE:
	 authType = ((CIMEnumerateInstanceNamesRequestMessage*)req.get())->authType;
	 cimMethodName = "EnumerateInstanceNames";
	 break;

      case CIM_EXEC_QUERY_REQUEST_MESSAGE:
	 authType = ((CIMExecQueryRequestMessage*)req.get())->authType;
	 cimMethodName = "ExecQuery";
	 break;

      case CIM_ASSOCIATORS_REQUEST_MESSAGE:
	 authType = ((CIMAssociatorsRequestMessage*)req.get())->authType;
	 cimMethodName = "Associators";
	 break;

      case CIM_ASSOCIATOR_NAMES_REQUEST_MESSAGE:
	 authType = ((CIMAssociatorNamesRequestMessage*)req.get())->authType;
	 cimMethodName = "AssociatorNames";
	 break;

      case CIM_REFERENCES_REQUEST_MESSAGE:
	 authType = ((CIMReferencesRequestMessage*)req.get())->authType;
	 cimMethodName = "References";
	 break;

      case CIM_REFERENCE_NAMES_REQUEST_MESSAGE:
	 authType = ((CIMReferenceNamesRequestMessage*)req.get())->authType;
	 cimMethodName = "ReferenceNames";
	 break;

      case CIM_GET_PROPERTY_REQUEST_MESSAGE:
	 authType = ((CIMGetPropertyRequestMessage*)req.get())->authType;
	 cimMethodName = "GetProperty";
	 break;

      case CIM_SET_PROPERTY_REQUEST_MESSAGE:
	 authType = ((CIMSetPropertyRequestMessage*)req.get())->authType;
	 cimMethodName = "SetProperty";
	 break;

      case CIM_GET_QUALIFIER_REQUEST_MESSAGE:
	 authType = ((CIMGetQualifierRequestMessage*)req.get())->authType;
	 cimMethodName = "GetQualifier";
	 break;

      case CIM_SET_QUALIFIER_REQUEST_MESSAGE:
	 authType = ((CIMSetQualifierRequestMessage*)req.get())->authType;
	 cimMethodName = "SetQualifier";
	 break;

      case CIM_DELETE_QUALIFIER_REQUEST_MESSAGE:
	 authType = ((CIMDeleteQualifierRequestMessage*)req.get())->authType;
	 cimMethodName = "DeleteQualifier";
	 break;

      case CIM_ENUMERATE_QUALIFIERS_REQUEST_MESSAGE:
	 authType = ((CIMEnumerateQualifiersRequestMessage*)req.get())->authType;
	 cimMethodName = "EnumerateQualifiers";
	 break;

      case CIM_INVOKE_METHOD_REQUEST_MESSAGE:
	 authType = ((CIMInvokeMethodRequestMessage*)req.get())->authType;
	 cimMethodName = "InvokeMethod";
	 break;

      default:
         PEGASUS_ASSERT(0);
	 break;
   }

#ifdef PEGASUS_ENABLE_USERGROUP_AUTHORIZATION
   //
   // If the user is not privileged and authorized user group is specified, 
   // then perform the user group authorization check.
   //
   try
   {
       if ( ! System::isPrivilegedUser(userName) )
       {
           Uint32 size = _authorizedUserGroups.size();

           if (size > 0)
           {
               Boolean authorized = false;

               //
               // Check if the user name is in the authorized user groups.
               //
               for (Uint32 i = 0; i < size; i++)
               {
                   //
                   // Check if the user is a member of the group
                   //
                   if ( System::isGroupMember(userName.getCString(),
                            _authorizedUserGroups[i].getCString()) )
                   {
                       authorized = true;
                       break;
                   }
               }

               //
               // If the user is not a member of any of the authorized
               // user groups then generate error response.
               //
               if (!authorized)
               {
                   PEG_TRACE_STRING(TRC_SERVER, Tracer::LEVEL2,
                       "Authorization Failed: User '" + userName +
                       "' is not a member of the authorized groups");

                   MessageLoaderParms msgLoaderParms(
                       "Server.CIMOperationRequestAuthorizer.NOT_IN_AUTHORIZED_GRP",
                       "User '$0' is not authorized to access CIM data.",
                       userName);
                   //
                   // user is not in the authorized user groups, send an
                   // error message to the requesting client.
                   //
                   if (cimMethodName == "InvokeMethod")
                   {
                       // l10n
                       sendMethodError(
                           queueId,
                           req->getHttpMethod(),
                           req->messageId,
                           ((CIMInvokeMethodRequestMessage*)req.get())->methodName,
                           PEGASUS_CIM_EXCEPTION_L(CIM_ERR_ACCESS_DENIED, 
                                                   msgLoaderParms));
                       PEG_METHOD_EXIT();
                       return;
                   }
                   else
                   {
                       // l10n
                       sendIMethodError(
                           queueId,
                           req->getHttpMethod(),
                           req->messageId,
                           cimMethodName,
                           PEGASUS_CIM_EXCEPTION_L(CIM_ERR_ACCESS_DENIED, 
                                                   msgLoaderParms));
                       PEG_METHOD_EXIT();
                       return;
                   }
               }
           }
       }
   }
   catch (InternalSystemError &ise)
   {
       sendIMethodError(
               queueId,
               req->getHttpMethod(),
               req->messageId,
               cimMethodName,
               PEGASUS_CIM_EXCEPTION(CIM_ERR_ACCESS_DENIED, ise.getMessage()));
       PEG_METHOD_EXIT();
       return;
   }
#endif  // #ifdef PEGASUS_ENABLE_USERGROUP_AUTHORIZATION

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

	   // l10n
	   
           // String description = "Not authorized to run ";
           // description.append(cimMethodName);
           // description.append(" in the namespace ");
           // description.append(nameSpace.getString());

            if (cimMethodName == "InvokeMethod")
            {
	      // l10n
	      sendMethodError(
                  queueId,
                  req->getHttpMethod(),
                  req->messageId,
                  ((CIMInvokeMethodRequestMessage*)req.get())->methodName,
                  PEGASUS_CIM_EXCEPTION_L(CIM_ERR_ACCESS_DENIED, 
			 MessageLoaderParms(
			 "Server.CIMOperationRequestAuthorizer.NOT_AUTHORIZED", 
			 "Not authorized to run $0 in the namespace $1", 
			   cimMethodName, nameSpace.getString())));
            }
            else
            {
	      // l10n
	      sendIMethodError(
		       queueId,
		       req->getHttpMethod(),
		       req->messageId,
		       cimMethodName,
		       PEGASUS_CIM_EXCEPTION_L(CIM_ERR_ACCESS_DENIED, 
		       MessageLoaderParms(
		       "Server.CIMOperationRequestAuthorizer.NOT_AUTHORIZED", 
		 	"Not authorized to run $0 in the namespace $1", 
	    		cimMethodName, nameSpace.getString())));
            }

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

      if (cimMethodName == "InvokeMethod")
      {

	// l10n

         sendMethodError(
            queueId,
            req->getHttpMethod(),
            req->messageId,
            ((CIMInvokeMethodRequestMessage*)req.get())->methodName,
           PEGASUS_CIM_EXCEPTION_L(CIM_ERR_ACCESS_DENIED, MessageLoaderParms(
                   "Server.CIMOperationRequestAuthorizer.REMOTE_NOT_ENABLED", 
                   "Remote privileged user access is not enabled.")));
      }
      else
      {
	// l10n

         sendIMethodError(
            queueId,
            req->getHttpMethod(),
            req->messageId,
            cimMethodName,
           PEGASUS_CIM_EXCEPTION_L(CIM_ERR_ACCESS_DENIED, MessageLoaderParms(
              "Server.CIMOperationRequestAuthorizer.REMOTE_NOT_ENABLED", 
              "Remote privileged user access is not enabled.")));
      }

      PEG_METHOD_EXIT();

      return;
   }

   //
   // Enqueue the request
   //
   _outputQueue->enqueue(req.release());

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

Array<String> CIMOperationRequestAuthorizer::_getAuthorizedUserGroups()
{
   PEG_METHOD_ENTER(TRC_SERVER,
       "CIMOperationRequestAuthorizer::getAuthorizedUserGroups");

   Array<String> authorizedGroups;

   String groupNames = String::EMPTY;

   //
   // Get a config manager instance
   //
   ConfigManager* configManager = ConfigManager::getInstance();

   groupNames = configManager->getCurrentValue("authorizedUserGroups");

   //
   // Check if the group name is empty
   //
   if (groupNames == String::EMPTY || groupNames == "")
   {
       PEG_METHOD_EXIT();
       return authorizedGroups;
   }

   //
   // Append _GROUPNAME_SEPARATOR to the end of the groups
   //
   groupNames.append(_GROUPNAME_SEPARATOR);

   Uint32   position = 0;
   String   groupName = String::EMPTY;

   while (groupNames != String::EMPTY)
   {
       //
       // Get a group name from user groups
       // User groups are separated by _GROUPNAME_SEPARATOR
       //
       position = groupNames.find(_GROUPNAME_SEPARATOR);
       groupName = groupNames.subString(0,(position));

       authorizedGroups.append(groupName);

       // Remove the searched group name
       groupNames.remove(0, position + 1);
   }

   PEG_METHOD_EXIT();

   return authorizedGroups;
}
PEGASUS_NAMESPACE_END
