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

#ifndef Pegasus_RepositoryQueue_h
#define Pegasus_RepositoryQueue_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/MessageQueueService.h>
#include <Pegasus/Repository/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

class PEGASUS_REPOSITORY_LINKAGE RepositoryQueue : public MessageQueueService
{
   public:
      typedef MessageQueueService Base;
      RepositoryQueue(void);
      virtual ~RepositoryQueue(void);
      
      virtual void handleEnqueue(Message *);
      virtual void handleEnqueue(void);

   protected:
      void handleGetClassRequest(const Message * message);
      void handleEnumerateClassesRequest(const Message * message);
      void handleEnumerateClassNamesRequest(const Message * message);
      void handleCreateClassRequest(const Message * message);
      void handleModifyClassRequest(const Message * message);
      void handleDeleteClassRequest(const Message * message);
	
      void handleGetInstanceRequest(const Message * message);
      void handleEnumerateInstancesRequest(const Message * message);
      void handleEnumerateInstanceNamesRequest(const Message * message);
      void handleCreateInstanceRequest(const Message * message);
      void handleModifyInstanceRequest(const Message * message);
      void handleDeleteInstanceRequest(const Message * message);
	
      void handleGetPropertyRequest(const Message * message);
      void handleSetPropertyRequest(const Message * message);

      void handleAssociatorsRequest(const Message * message);
      void handleAssociatorNamesRequest(const Message * message);
      void handleReferencesRequest(const Message * message);
      void handleReferenceNamesRequest(const Message * message);
	
      void handleGetQualifier(const Message * message);
      void handleEnumerateQualifiers(const Message * message);
      void handleSetQualifier(const Message * message);

      void handleEnumerateNameSpacesRequest(const Message * message);
      void handleCreateNameSpaceRequest(const Message * message);
      void handleModifyNameSpaceRequest(const Message * message);
      void handleDeleteNameSpaceRequest(const Message * message);

   protected:

};

PEGASUS_NAMESPACE_END

#endif
