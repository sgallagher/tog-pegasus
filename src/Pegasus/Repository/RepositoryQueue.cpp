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
// Author: Chip Vincent (cvincent@us.ibm.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include "RepositoryQueue.h"

PEGASUS_NAMESPACE_BEGIN

RepositoryQueue::RepositoryQueue(void)
   Base("RepositoryQueue", MessageQueue::getNextQueueId())
{

}

RepositoryQueue::~RepositoryQueue(void)
{
}


void RepositoryQueue::handleEnqueue(Message *message)
{
   if( ! message )
      return;
   
   switch(message->getType())
   {
      case CIM_GET_CLASS_REQUEST_MESSAGE:
	 handleGetClassRequest(message);

	 break;
      case CIM_ENUMERATE_CLASSES_REQUEST_MESSAGE:
	 handleEnumerateClassesRequest(message);

	 break;
      case CIM_ENUMERATE_CLASS_NAMES_REQUEST_MESSAGE:
	 handleEnumerateClassNamesRequest(message);

	 break;
      case CIM_CREATE_CLASS_REQUEST_MESSAGE:
	 handleCreateClassRequest(message);

	 break;
      case CIM_MODIFY_CLASS_REQUEST_MESSAGE:
	 handleModifyClassRequest(message);

	 break;
      case CIM_DELETE_CLASS_REQUEST_MESSAGE:
	 handleDeleteClassRequest(message);

	 break;
      case CIM_GET_INSTANCE_REQUEST_MESSAGE:
	 handleGetInstanceRequest(message);

	 break;
      case CIM_ENUMERATE_INSTANCES_REQUEST_MESSAGE:
	 handleEnumerateInstancesRequest(message);

	 break;
      case CIM_ENUMERATE_INSTANCE_NAMES_REQUEST_MESSAGE:
	 handleEnumerateInstanceNamesRequest(message);

	 break;
      case CIM_CREATE_INSTANCE_REQUEST_MESSAGE:
	 handleCreateInstanceRequest(message);

	 break;
      case CIM_MODIFY_INSTANCE_REQUEST_MESSAGE:
	 handleModifyInstanceRequest(message);

	 break;
      case CIM_DELETE_INSTANCE_REQUEST_MESSAGE:
	 handleDeleteInstanceRequest(message);

	 break;
      case CIM_EXEC_QUERY_REQUEST_MESSAGE:
	 handleExecQueryRequest(message);

	 break;
      case CIM_ASSOCIATORS_REQUEST_MESSAGE:
	 handleAssociatorsRequest(message);

	 break;
      case CIM_ASSOCIATOR_NAMES_REQUEST_MESSAGE:
	 handleAssociatorNamesRequest(message);

	 break;
      case CIM_REFERENCES_REQUEST_MESSAGE:
	 handleReferencesRequest(message);

	 break;
      case CIM_REFERENCE_NAMES_REQUEST_MESSAGE:
	 handleRefereceNamesRequest(message);

	 break;
      case CIM_GET_PROPERTY_REQUEST_MESSAGE:
	 handleGetPropertyRequest(message);

	 break;
      case CIM_SET_PROPERTY_REQUEST_MESSAGE:
	 handleSetPropertyRequest(message);

	 break;
      case CIM_GET_QUALIFIER_REQUEST_MESSAGE:
	 handleGetQualifierRequest(message);

	 break;
      case CIM_SET_QUALIFIER_REQUEST_MESSAGE:
	 handleSetQualifierRequest(message);

	 break;
      case CIM_DELETE_QUALIFIER_REQUEST_MESSAGE:
	 handleDeleteQualifierRequest(message);

	 break;
      case CIM_ENUMERATE_QUALIFIERS_REQUEST_MESSAGE:
	 handleEnumerateQualifiersRequest(message);

	 break;
      case CIM_INVOKE_METHOD_REQUEST_MESSAGE:
	 handleInvokeMethodRequest(message);

	 break;
      default:
	 break;
   }

   delete message;
}


void RepositoryQueue::handleEnqueue(void)
{
   Message * message = dequeue();

   PEGASUS_ASSERT(message != 0);
   if(message)
      handleEnqueue(message);
	

}

void RepositoryQueue::handleGetClassRequest(const Message * message)
{
}

void RepositoryQueue::handleEnumerateClassesRequest(const Message * message)
{
}

void RepositoryQueue::handleEnumerateClassNamesRequest(const Message * message)
{
}

void RepositoryQueue::handleCreateClassRequest(const Message * message)
{
}

void RepositoryQueue::handleModifyClassRequest(const Message * message)
{
}

void RepositoryQueue::handleDeleteClassRequest(const Message * message)
{
}

void RepositoryQueue::handleGetInstanceRequest(const Message * message)
{
}

void RepositoryQueue::handleEnumerateInstancesRequest(const Message * message)
{
}

void RepositoryQueue::handleEnumerateInstanceNamesRequest(const Message * message)
{
}

void RepositoryQueue::handleCreateInstanceRequest(const Message * message)
{
}

void RepositoryQueue::handleModifyInstanceRequest(const Message * message)
{
}

void RepositoryQueue::handleDeleteInstanceRequest(const Message * message)
{
}

void RepositoryQueue::handleGetPropertyRequest(const Message * message)
{
}

void RepositoryQueue::handleSetPropertyRequest(const Message * message)
{
}

void RepositoryQueue::handleAssociatorsRequest(const Message * message)
{
}

void RepositoryQueue::handleAssociatorNamesRequest(const Message * message)
{
}

void RepositoryQueue::handleReferencesRequest(const Message * message)
{
}

void RepositoryQueue::handleReferenceNamesRequest(const Message * message)
{
}

void RepositoryQueue::handleGetQualifier(const Message * message)
{
}

void RepositoryQueue::handleEnumerateQualifiers(const Message * message)
{
}

void RepositoryQueue::handleSetQualifier(const Message * message)
{
}

void RepositoryQueue::handleEnumerateNameSpacesRequest(const Message * message)
{
}

void RepositoryQueue::handleCreateNameSpaceRequest(const Message * message)
{
}

void RepositoryQueue::handleModifyNameSpaceRequest(const Message * message)
{
}

void RepositoryQueue::handleDeleteNameSpaceRequest(const Message * message)
{
}

PEGASUS_NAMESPACE_END
