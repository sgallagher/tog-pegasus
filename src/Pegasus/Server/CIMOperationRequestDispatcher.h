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
// Modified By: Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
//              Mike Day (mdday@us.ibm.com)
//              Yi Zhou (yi_zhou@hp.com)
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                  (carolann_graves@hp.com)
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//              Karl Schopmeyer (k.schopmeyer@opengroup.org)
//              Adrian Schuur (schuur@de.ibm.com)
//              Seema Gupta (gseema@in.ibm.com for PEP135)
//              Chip Vincent (cvincent@us.ibm.com)
//              Alagaraja Ramasubramanian (alags_raj@in.ibm.com) for Bug#1090
//         Brian G. Campbell, EMC (campbell_brian@emc.com) - PEP140/phase2
//              David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
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
#include <Pegasus/Common/OperationContextInternal.h>
#include <Pegasus/Common/QueryExpressionRep.h>
#include <Pegasus/Common/AutoPtr.h>

#include <Pegasus/Repository/CIMRepository.h>

#include <Pegasus/Server/CIMServer.h>
#include <Pegasus/Server/ProviderRegistrationManager/ProviderRegistrationManager.h>
#include <Pegasus/Server/Linkage.h>

#include <Pegasus/Common/QueryExpressionRep.h>

PEGASUS_NAMESPACE_BEGIN

// Class to aggregate and manage the information about classes and providers
// this simply masks some of the confusion of control providers, etc. today.

class PEGASUS_SERVER_LINKAGE ProviderInfo
{
public:
    ProviderInfo(const CIMName & className)
        : className(className),
        hasProvider(false),
        hasProviderNormalization(false),
        hasNoQuery(true)
    {
    }

    ProviderInfo(
        const CIMName & className,
        const String & serviceName,
        const String & controlProviderName)
        : className(className),
        serviceName(serviceName),
        controlProviderName(controlProviderName),
        hasProvider(false),
        hasProviderNormalization(false),
	    hasNoQuery(true)
    {
    }

    ProviderInfo(const ProviderInfo & providerInfo)
        : className(providerInfo.className),
        serviceName(providerInfo.serviceName),
        controlProviderName(providerInfo.controlProviderName),
        hasProvider(providerInfo.hasProvider),
        hasProviderNormalization(false),
        hasNoQuery(providerInfo.hasNoQuery)
    {
        if (providerInfo.providerIdContainer.get() != 0)
        {
            providerIdContainer.reset(
                new ProviderIdContainer(*providerInfo.providerIdContainer));
        }
    }

    ProviderInfo & operator=(const ProviderInfo & providerInfo)
    {
        if (&providerInfo != this)
        {
            className = providerInfo.className;
            serviceName = providerInfo.serviceName;
            controlProviderName = providerInfo.controlProviderName;
            hasProvider = providerInfo.hasProvider;
            hasProviderNormalization = providerInfo.hasProviderNormalization;
            hasNoQuery = providerInfo.hasNoQuery;

            providerIdContainer.reset();

            if (providerInfo.providerIdContainer.get() != 0)
            {
                providerIdContainer.reset(
                    new ProviderIdContainer(*providerInfo.providerIdContainer.get()));
            }
        }

        return(*this);
    }

    CIMName className;
    String serviceName;
    String controlProviderName;
    Boolean hasProvider;
    Boolean hasProviderNormalization;
    Boolean hasNoQuery;
    AutoPtr<ProviderIdContainer> providerIdContainer;

private:
    ProviderInfo(void)
    {
    }

};

/* Class to manage the aggregation of data required by post processors. This
    class is private to the dispatcher. An instance is created by the operation
    dispatcher to aggregate request and response information and used by the
    post processor to aggregate responses together.
*/
class PEGASUS_SERVER_LINKAGE OperationAggregate
{
public:
    /* Operation Aggregate constructor.  Builds an aggregate
        object.
        @param request
        @param msgRequestType
        @param messageId
        @param dest
				@param className
    */

    OperationAggregate(CIMRequestMessage* request,
											 Uint32 msgRequestType,
											 String messageId,
											 Uint32 dest,
											 CIMName className,
											 CIMNamespaceName nameSpace = CIMNamespaceName(),
											 QueryExpressionRep *query = 0,
											 String queryLanguage = String::EMPTY);

    virtual ~OperationAggregate();

    // Tests validity by checking the magic number we put into the
    // packet.

    Boolean valid() const;

		// Sets the total issued to the input parameter

    void setTotalIssued(Uint32 i);

    // Append a new entry to the response list.  Return value indicates
    // whether this response is the last one expected

    Boolean appendResponse(CIMResponseMessage* response);

    Uint32 numberResponses() const;

    CIMRequestMessage* getRequest();

    CIMResponseMessage* getResponse(const Uint32& pos);

    // allow dispatcher to remove the response so it doesn't become
    // destroyed when the poA is destroyed.

    CIMResponseMessage* removeResponse(const Uint32& pos);

    void deleteResponse(const Uint32&pos);

    Uint32 getRequestType() const;

		void resequenceResponse(CIMResponseMessage& response);

    String _messageId;
    Uint32 _msgRequestType;
    Uint32 _dest;
    CIMNamespaceName _nameSpace;
    CIMName _className;
    Array<String> propertyList;
    Uint64 _aggregationSN;
    QueryExpressionRep* _query;
    String _queryLanguage;

private:
    /** Hidden (unimplemented) copy constructor */
    OperationAggregate(const OperationAggregate& x) : _query(NULL) {}

    Array<CIMResponseMessage*> _responseList;
    Mutex _appendResponseMutex;
    CIMRequestMessage* _request;
    Uint32 _totalIssued;
    Uint32 _magicNumber;
		Uint32 _totalReceived;
		Uint32 _totalReceivedComplete;
		Uint32 _totalReceivedExpected;
		Uint32 _totalReceivedErrors;
    Uint32 _totalReceivedNotSupported;
};

class PEGASUS_SERVER_LINKAGE CIMOperationRequestDispatcher : public MessageQueueService
{
   friend class QuerySupportRouter;
   public:

      typedef MessageQueueService Base;

      CIMOperationRequestDispatcher(
    	 CIMRepository* repository,
         ProviderRegistrationManager* providerRegistrationManager);

      virtual ~CIMOperationRequestDispatcher();

      virtual void handleEnqueue(Message *);

      virtual void handleEnqueue();

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

      void handleExecQueryRequest(
    	 CIMExecQueryRequestMessage* request);

      void handleInvokeMethodRequest(
    	 CIMInvokeMethodRequestMessage* request);

      static void _forwardForAggregationCallback(AsyncOpNode *,
					   MessageQueue *,
					   void *);
      static void _forwardRequestCallback(AsyncOpNode *,
					   MessageQueue *,
					   void *);

      // Response Handler functions

      void handleOperationResponseAggregation(OperationAggregate* poA);

      void handleReferencesResponseAggregation(OperationAggregate* poA);

      void handleReferenceNamesResponseAggregation(OperationAggregate* poA);

      void handleAssociatorsResponseAggregation(OperationAggregate* poA);

      void handleAssociatorNamesResponseAggregation(OperationAggregate* poA);

      void handleEnumerateInstancesResponseAggregation(OperationAggregate* poA);

      void handleEnumerateInstanceNamesResponseAggregation(OperationAggregate* poA);

      void handleExecQueryResponseAggregation(OperationAggregate* poA);

   protected:

	/** _getSubClassNames - Gets the names of all subclasses of the defined
	    class (including the class) and returns it in an array of strings. Uses
	    a similar function in the repository class to get the names.
	    @param namespace
	    @param className
	    @return Array of strings with class names.  Note that there should be
	    at least one classname in the array (the input name)
	    Note that there is a special exception to this function, the __namespace
	    class which does not have any representation in the class repository.
	    @exception CIMException(CIM_ERR_INVALID_CLASS)
	*/
    Array<CIMName> _getSubClassNames(
        const CIMNamespaceName& nameSpace,
        const CIMName& className);

    Boolean _lookupInternalProvider(
        const CIMNamespaceName& nameSpace,
        const CIMName& className,
        String& service,
        String& provider);

/*    Boolean _lookupNewQueryProvider(
        const CIMNamespaceName& nameSpace,
        const CIMName& className,
        String& serviceName,
        String& controlProviderName,
	Boolean* notQueryProvider); */

    ProviderInfo _lookupNewInstanceProvider(
        const CIMNamespaceName& nameSpace,
        const CIMName& className);

/*    String _lookupQueryProvider(
        const CIMNamespaceName& nameSpace,
        const CIMName& className,
	Boolean* notQueryProvider); */

    ProviderInfo _lookupInstanceProvider(
        const CIMNamespaceName& nameSpace,
        const CIMName& className);

/*   Array<ProviderInfo> _lookupAllQueryProviders(
        const CIMNamespaceName& nameSpace,
        const CIMName& className,
        Uint32& providerCount); */

    // @exception CIMException
    Array<ProviderInfo> _lookupAllInstanceProviders(
        const CIMNamespaceName& nameSpace,
        const CIMName& className,
        Uint32& providerCount);

    Array<ProviderInfo> _lookupAllAssociationProviders(
        const CIMNamespaceName& nameSpace,
        const CIMObjectPath& objectName,
        const CIMName& assocClass,
        const String& role,
        Uint32& providerCount);

    Boolean _lookupNewAssociationProvider(
        const CIMNamespaceName& nameSpace,
        const CIMName& assocClass,
        String& serviceName,
        String& controlProviderName,
		ProviderIdContainer **container);

    Array<String> _lookupAssociationProvider(
        const CIMNamespaceName& nameSpace,
        const CIMName& assocClass,
		ProviderIdContainer **container);

      String _lookupMethodProvider(const CIMNamespaceName& nameSpace,
    	const CIMName& className, const CIMName& methodName,
		ProviderIdContainer **providerIdContainer);

      void _forwardRequestToService(
        const String& serviceName,
        CIMRequestMessage* request,
        CIMResponseMessage*& response);

      void _forwardRequestForAggregation(
        const String& serviceName,
        const String& controlProviderName,
        CIMRequestMessage* request,
        OperationAggregate* poA,
				CIMResponseMessage *response = 0);

      void _forwardRequestToProviderManager(
        const CIMName& className,
        const String& serviceName,
        const String& controlProviderName,
        CIMRequestMessage* request);

			Boolean _enqueueResponse(OperationAggregate *&poA,
															 CIMResponseMessage *&response);

      void _enqueueResponse(
          CIMRequestMessage* request, CIMResponseMessage* response);

      CIMValue _convertValueType(const CIMValue& value, CIMType type);

      void _fixInvokeMethodParameterTypes(CIMInvokeMethodRequestMessage* request);

      void _fixSetPropertyValueType(CIMSetPropertyRequestMessage* request);

      void _checkExistenceOfClass(const CIMNamespaceName& nameSpace,
		                          const CIMName& className,
		                          CIMException& cimException);

	  CIMClass _getClass(const CIMNamespaceName& nameSpace,
		                 const CIMName& className,
		                 CIMException& cimException);


      CIMRepository * _repository;

      ProviderRegistrationManager* _providerRegistrationManager;


      Boolean _enableAssociationTraversal;
      Boolean _enableIndicationService;
      Uint32 _maximumEnumerateBreadth;
      static Uint64 cimOperationAggregationSN;

      #ifdef PEGASUS_ENABLE_OBJECT_NORMALIZATION
      Boolean _enableNormalization;
      Array<String> _excludeModulesFromNormalization;
      #endif

      // << Tue Feb 12 08:48:09 2002 mdd >> meta dispatcher integration
      virtual void _handle_async_request(AsyncRequest *req);

      // the following two methods enable specific query language implementations

 /*     void handleQueryRequest(
    	 CIMExecQueryRequestMessage* request);

      void handleQueryResponseAggregation(
         OperationAggregate* poA);

      void applyQueryToEnumeration(CIMResponseMessage* msg,
         QueryExpressionRep* query);
*/
   private:
      static void _handle_enqueue_callback(AsyncOpNode *, MessageQueue *, void *);

};

PEGASUS_NAMESPACE_END

#endif /* PegasusDispatcher_Dispatcher_h */
