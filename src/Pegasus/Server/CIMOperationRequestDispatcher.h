//%///////-*-c++-*-/////////////////////////////////////////////////////////////
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
// Modified By: Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
//            : Mike Day (mdday@us.ibm.com)
//            : Yi Zhou (yi_zhou@hp.com)
//            : Carol Ann Krug Graves, Hewlett-Packard Company
//              (carolann_graves@hp.com)
//
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef PegasusDispatcher_Dispatcher_h
#define PegasusDispatcher_Dispatcher_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/IPC.h>
#include <Pegasus/Common/DQueue.h>
#include <Pegasus/Common/Thread.h>
#include <Pegasus/Common/MessageQueue.h>
#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Common/CIMObject.h>
#include <Pegasus/Common/OperationContext.h>

#include <Pegasus/Server/CIMServer.h>

#include <Pegasus/Repository/CIMRepository.h>

#include <Pegasus/Server/ServiceCIMOMHandle.h>
#include <Pegasus/Server/ConfigurationManager/ConfigurationManagerQueue.h>
#include <Pegasus/Server/ProviderRegistrationManager/ProviderRegistrationManager.h>

PEGASUS_NAMESPACE_BEGIN

class PEGASUS_SERVER_LINKAGE CIMOperationRequestDispatcher : public MessageQueueService
{
public:

      typedef MessageQueueService Base;

      CIMOperationRequestDispatcher(
	 CIMRepository* repository,
	 ProviderRegistrationManager* providerRegistrationManager,
	 CIMServer* server);

      virtual ~CIMOperationRequestDispatcher();

      virtual void handleEnqueue(Message *);

      virtual void handleEnqueue();

      virtual const char* getQueueName() const;

      void handleGetClassRequest(
	 CIMGetClassRequestMessage* request);

      void handleGetInstanceRequest(
	 CIMGetInstanceRequestMessage* request);

      void handleDeleteClassRequest(
	 CIMDeleteClassRequestMessage* request);

      void handleDeleteInstanceRequest(
	 CIMDeleteInstanceRequestMessage* request);

      void handleCreateClassRequest(
	 CIMCreateClassRequestMessage* request);

      void handleCreateInstanceRequest(
	 CIMCreateInstanceRequestMessage* request);

      void handleModifyClassRequest(
	 CIMModifyClassRequestMessage* request);

      void handleModifyInstanceRequest(
	 CIMModifyInstanceRequestMessage* request);

      void handleEnumerateClassesRequest(
	 CIMEnumerateClassesRequestMessage* request);

      void handleEnumerateClassNamesRequest(
	 CIMEnumerateClassNamesRequestMessage* request);

      void handleEnumerateInstancesRequest(
	 CIMEnumerateInstancesRequestMessage* request);

      void handleEnumerateInstanceNamesRequest(
	 CIMEnumerateInstanceNamesRequestMessage* request);

      void handleAssociatorsRequest(
	 CIMAssociatorsRequestMessage* request);

      void handleAssociatorNamesRequest(
	 CIMAssociatorNamesRequestMessage* request);

      void handleReferencesRequest(
	 CIMReferencesRequestMessage* request);

      void handleReferenceNamesRequest(
	 CIMReferenceNamesRequestMessage* request);

      void handleGetPropertyRequest(
	 CIMGetPropertyRequestMessage* request);

      void handleSetPropertyRequest(
	 CIMSetPropertyRequestMessage* request);

      void handleGetQualifierRequest(
	 CIMGetQualifierRequestMessage* request);

      void handleSetQualifierRequest(
	 CIMSetQualifierRequestMessage* request);

      void handleDeleteQualifierRequest(
	 CIMDeleteQualifierRequestMessage* request);

      void handleEnumerateQualifiersRequest(
	 CIMEnumerateQualifiersRequestMessage* request);

      void handleInvokeMethodRequest(
	 CIMInvokeMethodRequestMessage* request);

      void handleEnableIndicationSubscriptionRequest(
	 CIMEnableIndicationSubscriptionRequestMessage* request);

      void handleModifyIndicationSubscriptionRequest(
	 CIMModifyIndicationSubscriptionRequestMessage* request);

      void handleDisableIndicationSubscriptionRequest(
	 CIMDisableIndicationSubscriptionRequestMessage* request);

      void handleProcessIndicationRequest(
	 CIMProcessIndicationRequestMessage* request);

      ProviderRegistrationManager* getProviderRegistrationManager(void);

protected:

      String _lookupInstanceProvider(
	const String& nameSpace, const String& className);

      String _lookupAssociationProvider(
	const String& nameSpace, const String& className);

      String _lookupMethodProvider(const String& nameSpace,
	const String& className, const String& methodName);

      String _lookupIndicationProvider(
	const String& nameSpace, const String& className);

      void _enqueueResponse(
	 CIMRequestMessage* request, CIMResponseMessage* response);

      CIMValue _convertValueType(const CIMValue& value, CIMType type);

      void _fixInvokeMethodParameterTypes(CIMInvokeMethodRequestMessage* request);

      void _fixSetPropertyValueType(CIMSetPropertyRequestMessage* request);

      CIMRepository * _repository;

      ProviderRegistrationManager* _providerRegistrationManager;

      ServiceCIMOMHandle _cimom;

      ConfigurationManagerQueue _configurationManager;

      AtomicInt _dying;

      // << Tue Feb 12 08:48:09 2002 mdd >> meta dispatcher integration
      virtual void _handle_async_request(AsyncRequest *req);
   private:
      static void _handle_enqueue_callback(AsyncOpNode *, MessageQueue *, void *);

};

PEGASUS_NAMESPACE_END

#endif /* PegasusDispatcher_Dispatcher_h */
