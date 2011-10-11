//%LICENSE////////////////////////////////////////////////////////////////
//
// Licensed to The Open Group (TOG) under one or more contributor license
// agreements.  Refer to the OpenPegasusNOTICE.txt file distributed with
// this work for additional information regarding copyright ownership.
// Each contributor licenses this file to you under the OpenPegasus Open
// Source License; you may not use this file except in compliance with the
// License.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//////////////////////////////////////////////////////////////////////////
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef PegasusDispatcher_Dispatcher_h
#define PegasusDispatcher_Dispatcher_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Thread.h>
#include <Pegasus/Common/MessageQueue.h>
#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Common/CIMObject.h>
#include <Pegasus/Common/OperationContextInternal.h>
#include <Pegasus/Common/QueryExpressionRep.h>
#include <Pegasus/Common/AutoPtr.h>
#include <Pegasus/Common/NumericArg.h>
#include <Pegasus/Common/Magic.h>

#include <Pegasus/Repository/CIMRepository.h>

#include <Pegasus/Server/CIMServer.h>
#include \
    <Pegasus/Server/ProviderRegistrationManager/ProviderRegistrationManager.h>
#include <Pegasus/Server/Linkage.h>
#include <Pegasus/Server/reg_table.h>

PEGASUS_NAMESPACE_BEGIN

/****************************************************************************** 
** 
**  ProviderInfo Clas - manage info about classes and providers received
**      from provider lookup functions. Provides info on classes and providers
**      in enumerations and association looksups including provider type,
**      etc. This class is private to the dispatcher.
**
******************************************************************************/ 

class PEGASUS_SERVER_LINKAGE ProviderInfo
{
public:
    // constructor
    ProviderInfo(const CIMName& className_)
        : className(className_),
          serviceId(0),
          hasProvider(false),
          hasProviderNormalization(false),
          hasNoQuery(true)
    {
    }
    //constructor
    ProviderInfo(
        const CIMName& className_,
        Uint32 serviceId_,
        const String& controlProviderName_)
        : className(className_),
          serviceId(serviceId_),
          controlProviderName(controlProviderName_),
          hasProvider(false),
          hasProviderNormalization(false),
          hasNoQuery(true)
    {
    }
    // copy constructor
    ProviderInfo(const ProviderInfo& providerInfo)
        : className(providerInfo.className),
          serviceId(providerInfo.serviceId),
          controlProviderName(providerInfo.controlProviderName),
          hasProvider(providerInfo.hasProvider),
          hasProviderNormalization(providerInfo.hasProviderNormalization),
          hasNoQuery(providerInfo.hasNoQuery)
    {
        if (providerInfo.providerIdContainer.get() != 0)
        {
            providerIdContainer.reset(
                new ProviderIdContainer(*providerInfo.providerIdContainer));
        }
    }

    ProviderInfo& operator=(const ProviderInfo& providerInfo)
    {
        if (&providerInfo != this)
        {
            className = providerInfo.className;
            serviceId = providerInfo.serviceId;
            controlProviderName = providerInfo.controlProviderName;
            hasProvider = providerInfo.hasProvider;
            hasProviderNormalization = providerInfo.hasProviderNormalization;
            hasNoQuery = providerInfo.hasNoQuery;

            providerIdContainer.reset();

            if (providerInfo.providerIdContainer.get() != 0)
            {
                providerIdContainer.reset(new ProviderIdContainer(
                    *providerInfo.providerIdContainer.get()));
            }
        }

        return *this;
    }

    CIMName className;
    Uint32 serviceId;
    String controlProviderName;
    Boolean hasProvider;
    Boolean hasProviderNormalization;
    Boolean hasNoQuery;
    AutoPtr<ProviderIdContainer> providerIdContainer;

private:
    ProviderInfo()
    {
    }
};
/*
    Manage a list of ProviderInfo information objects
*/
class PEGASUS_SERVER_LINKAGE ProviderInfoListStruct
{
public:
    Array<ProviderInfo> array;
    Uint32 providerCount;

    ProviderInfoListStruct()
    : providerCount(0)
    {
    }
    // get size of list
    Uint32 size()
    {
        return array.size();
    }
    // append to the list
    void append(ProviderInfo& x)
    {
        array.append(x);
    }
    // get item from array
    ProviderInfo& operator[](Uint32 index)
    {
        return array[index];
    }

};
typedef  ProviderInfoListStruct ProviderInfoList;

/****************************************************************************** 
** 
**  OperationAggregate Class
**
******************************************************************************/ 
/* Class to manage the aggregation of data required by post processors. This
    class is private to the dispatcher. An instance is created by the operation
    dispatcher to aggregate request and response information and used by the
    post processor to aggregate responses from providers.
*/
class PEGASUS_SERVER_LINKAGE OperationAggregate
{
    friend class CIMOperationRequestDispatcher;
public:
    /** Operation Aggregate constructor.  Builds an aggregate
        object.
        @param request
        @param msgRequestType
        @param messageId
        @param dest
        @param className
    */
    OperationAggregate(CIMRequestMessage* request,
        MessageType msgRequestType,
        String messageId,
        Uint32 dest,
        CIMName className,
        CIMNamespaceName nameSpace = CIMNamespaceName(),
        QueryExpressionRep* query = 0,
        String queryLanguage = String::EMPTY);

    virtual ~OperationAggregate();

    // Tests validity by checking the magic number we put into the
    // packet.

    Boolean valid() const;

    // Sets the total Operation Requests issued parameter. 

    void setTotalIssued(Uint32 i);

    void incTotalIssued();

    //KS_TODO The total issued must be atomic.  It is not now.

    // Append a new entry to the response list.  Return value indicates
    // whether this response is the last one expected

    Boolean appendResponse(CIMResponseMessage* response);

    // KS_TODO - Think this can be removed.
//  void prependResponse(CIMResponseMessage* response);

    Uint32 numberResponses();

    CIMRequestMessage* getRequest();

    void setRequest(CIMRequestMessage* request);

//  // KS_TODO Think this can be removed
//  Boolean isInitialResponseGenerated();
//
//  // KS_TODO Think this can be removed.
//  void initialResponseGenerated(Boolean x);

    CIMResponseMessage* getResponse(const Uint32& pos);

    // allow dispatcher to remove the response so it doesn't become
    // destroyed when the poA is destroyed.

    CIMResponseMessage* removeResponse(const Uint32& pos);

    void deleteResponse(const Uint32&pos);

    void incObjectCount();

    void decObjectCount();

    MessageType getRequestType() const;

    void resequenceResponse(CIMResponseMessage& response);

    // Diagnostic only.  We keep serialnumber of aggregation objects and
    // this gets current serial number.
    Uint64 getAggregationSN();

    /** sets the parameters required for pull operations into a new
     *  operation aggregate that was created as part of an Open...
     *  Operation.
     * 
     * @param enContext EnumerationContext defined for this sequence
     * @param contextString String representing the operation 
     *      Context defined for this sequence
     * @param nameSpace Namespace for this sequence of Operations
     */
    void setPullOperation(const void* enContext,
        const String& contextString,
        const CIMNamespaceName& nameSpace);

    String _messageId;
    MessageType _msgRequestType;
    Uint32 _dest;
    CIMNamespaceName _nameSpace;
    CIMName _className;
    Array<String> propertyList;
    QueryExpressionRep* _query;
    String _queryLanguage;
    Uint64 _objectCount;
    Boolean _pullOperation;
    Boolean _enumerationFinished;
    Boolean _closeReceived;
    // KS_PULL extensions KS_TODO - Do not need to maintain both name & ptr
    void* _enumerationContext;
    String _enumerationContextName;

    // Serial number for aggregation packets.  Diagnostic only.
    Uint64 _aggregationSN;
    static Uint64 _operationAggregationSNAccumulator;

private:
    /** Hidden (unimplemented) copy constructor */
    OperationAggregate(const OperationAggregate& x);

    Array<CIMResponseMessage*> _responseList;
    Mutex _appendResponseMutex;
    Mutex _enqueueResponseMutex;
    Mutex _enqueuePullResponseMutex;
    CIMRequestMessage* _request;
    Uint32 _totalIssued;
    Uint32 _magicNumber;
    Uint32 _totalReceived;
    Uint32 _totalReceivedComplete;
    Uint32 _totalReceivedExpected;
    Uint32 _totalReceivedErrors;
    Uint32 _totalReceivedNotSupported;
    //Boolean _initialResponseGenerated;
    Magic<0xC531B144> _magic;
};

/****************************************************************************** 
** 
**  CIMOperationRequestDispatcher Class
**
******************************************************************************/ 
class PEGASUS_SERVER_LINKAGE CIMOperationRequestDispatcher :
    public MessageQueueService
{
    friend class QuerySupportRouter;
    friend class ProviderRequests;

public:

    typedef MessageQueueService Base;

    CIMOperationRequestDispatcher(
        CIMRepository* repository,
        ProviderRegistrationManager* providerRegistrationManager);

    virtual ~CIMOperationRequestDispatcher();

    virtual void handleEnqueue(Message*);

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
 // EXP_PULL_BEGIN

    void handleOpenEnumerateInstancesRequest(
        CIMOpenEnumerateInstancesRequestMessage* request);

    void handleOpenEnumerateInstancePathsRequest(
        CIMOpenEnumerateInstancePathsRequestMessage* request);

    void handleOpenReferenceInstancesRequest(
        CIMOpenReferenceInstancesRequestMessage* request);

    void handleOpenReferenceInstancePathsRequest(
        CIMOpenReferenceInstancePathsRequestMessage* request);

    void handleOpenAssociatorInstancesRequest(
        CIMOpenAssociatorInstancesRequestMessage* request);

    void handleOpenAssociatorInstancePathsRequest(
        CIMOpenAssociatorInstancePathsRequestMessage* request);

    void handlePullInstancesWithPath(
        CIMPullInstancesWithPathRequestMessage* request);

    void handlePullInstancePaths(
        CIMPullInstancePathsRequestMessage* request);

    void handleCloseEnumeration(
        CIMCloseEnumerationRequestMessage* request);

    void handleEnumerationCount(
        CIMEnumerationCountRequestMessage* request);
// EXP_PULL END

    static void _forwardForAggregationCallback(
        AsyncOpNode*,
        MessageQueue*,
        void*);

// EXP_PULL_BEGIN
    static void _forwardForPullAggregationCallback(
        AsyncOpNode*,
        MessageQueue*,
        void*);
// EXP_PULL_END

    static void _forwardRequestCallback(
        AsyncOpNode*,
        MessageQueue*,
        void*);

    // Response Handler functions

    void handleOperationResponseAggregation(OperationAggregate* poA);

    void handleReferencesResponseAggregation(OperationAggregate* poA);

    void handleReferenceNamesResponseAggregation(OperationAggregate* poA);

    void handleAssociatorsResponseAggregation(OperationAggregate* poA);

    void handleAssociatorNamesResponseAggregation(OperationAggregate* poA);

    void handleEnumerateInstancesResponseAggregation(
        OperationAggregate* poA,
        bool hasPropList);

    void handleEnumerateInstanceNamesResponseAggregation(
        OperationAggregate* poA);

    void handleExecQueryResponseAggregation(OperationAggregate* poA);

    // Issue RequestToProvider Functions
    void issueRequestsToProviders(CIMEnumerateInstancesRequestMessage* request,
        Uint32 numberOfClasses,
        ProviderInfoList& providerInfos,
        OperationAggregate* poA);
//  KS_PULL_END

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
        Uint32 &serviceId,
        String& provider);

    /* Boolean _lookupNewQueryProvider(
        const CIMNamespaceName& nameSpace,
        const CIMName& className,
        String& serviceName,
        String& controlProviderName,
        Boolean* notQueryProvider); */

    ProviderInfo _lookupNewInstanceProvider(
        const CIMNamespaceName& nameSpace,
        const CIMName& className);

    /* String _lookupQueryProvider(
        const CIMNamespaceName& nameSpace,
        const CIMName& className,
        Boolean* notQueryProvider); */

    ProviderInfo _lookupInstanceProvider(
        const CIMNamespaceName& nameSpace,
        const CIMName& className);

    /* Array<ProviderInfo> _lookupAllQueryProviders(
        const CIMNamespaceName& nameSpace,
        const CIMName& className,
        Uint32& providerCount); */

    // @exception CIMException
    ProviderInfoList _lookupAllInstanceProviders(
        const CIMNamespaceName& nameSpace,
        const CIMName& className);

    ProviderInfoList _lookupAllAssociationProviders(
        const CIMNamespaceName& nameSpace,
        const CIMObjectPath& objectName,
        const CIMName& assocClass,
        const String& role);

    Boolean _lookupNewAssociationProvider(
        const CIMNamespaceName& nameSpace,
        const CIMName& assocClass,
        Uint32 &serviceId,
        String& controlProviderName,
        ProviderIdContainer** container);

    Array<String> _lookupAssociationProvider(
        const CIMNamespaceName& nameSpace,
        const CIMName& assocClass,
        ProviderIdContainer** container);

    String _lookupMethodProvider(
        const CIMNamespaceName& nameSpace,
        const CIMName& className,
        const CIMName& methodName,
        ProviderIdContainer** providerIdContainer);

    void _forwardRequestToService(
        Uint32 serviceId,
        CIMRequestMessage* request,
        CIMRequestMessage* requestCopy);

    void _forwardRequestForAggregation(
        Uint32 serviceId,
        const String& controlProviderName,
        CIMRequestMessage* request,
        OperationAggregate* poA,
        CIMResponseMessage* response = 0);

// KS_PULL_BEGIN
// KS_PULL_TODO - This and other aggregator
// can be pulled together.
    void _forwardRequestForPullAggregation(
        Uint32 serviceId,
        const String& controlProviderName,
        CIMRequestMessage* request,
        OperationAggregate* poA,
        CIMResponseMessage* response = 0);
//KS_PULL_END
    void _forwardRequestToProviderManager(
        const CIMName& className,
        Uint32 serviceId,
        const String& controlProviderName,
        CIMRequestMessage* request,
        CIMRequestMessage* requestCopy);

    void _getProviderName(
          const OperationContext& context,
          String& moduleName,
          String& providerName);

    void _logOperation(
        const CIMRequestMessage* request,
        const CIMResponseMessage* response);

    void _enqueueExceptionResponse(
        CIMRequestMessage* request,
        CIMException& exception);

    void _enqueueExceptionResponse(
        CIMRequestMessage* request,
        TraceableCIMException& exception);

    Boolean _enqueueAggregateResponse(
        OperationAggregate*& poA,
        CIMResponseMessage*& response);
// KS_PULL_BEGIN
    Boolean _enqueuePullAggregateResponse(
        OperationAggregate*& poA,
        CIMResponseMessage*& response);
//KS_PULL_END
    void _enqueueResponse(
        CIMRequestMessage* request,
        CIMResponseMessage* response);

    CIMValue _convertValueType(const CIMValue& value, CIMType type);

    void _fixInvokeMethodParameterTypes(CIMInvokeMethodRequestMessage* request);

    void _fixSetPropertyValueType(CIMSetPropertyRequestMessage* request);


    // Error Generating Functions - The following functions test for
    // particular operations parameters, etc. and if the tests fail
    // generate error response messages.  The all follow the same common
    // pattern of returning false if the test fails so that the main
    // function must test the result and return.  This allows putting
    // the trace method return into the CIMOperationRequestDispatcher main.

    Boolean _rejectAssociationTraversalDisabled(CIMRequestMessage* request,
        const String& opName);

    Boolean _rejectInvalidRoleParameter(CIMRequestMessage* request,
        const String& roleParameter,
        const String& parameterName);

    Boolean _rejectInvalidObjectPathParameter(
        CIMRequestMessage* request,
        const CIMObjectPath& path);

    Boolean _rejectIfContinueOnError(CIMRequestMessage* request,
        Boolean continueOnError);

    Boolean _rejectInvalidFilterParameters(CIMRequestMessage* request,
        const String& filterQueryLanguageParam,
        const String& filterQueryParam);

    Boolean _rejectInvalidMaxObjectCountParam(CIMRequestMessage* request,
        const Uint32Arg& maxObjectCountParam,
        Boolean requiredParameter,
        Uint32& value,
        const Uint32 defaultValue);

    Boolean _rejectInvalidClassParameter(CIMRequestMessage* request,
        const CIMName& className,
        const CIMNamespaceName& nameSpace,
        CIMConstClass& cimClass);

    Boolean _rejectInvalidOperationTimeout(CIMRequestMessage* request, 
        const Uint32Arg& operationTimeout);

    Boolean _rejectInValidEnumerationContext(CIMRequestMessage* request,
        void* enumerationContext);

    Boolean _rejectIfContextTimedOut(CIMRequestMessage* request,
        Boolean isTimedOut);

    Boolean _rejectInvalidPullRequest(CIMRequestMessage* request,
        Boolean valid);

    Boolean _rejectIfEnumerationContextActive(CIMRequestMessage* request,
        Boolean active);

    /**
        Checks whether the specified class is defined in the specified
        namespace.
        @param nameSpace The namespace to check for className.
        @param className The name of the class to check for in nameSpace.
        @return True if the specified class is defined in the specified
            namespace, false otherwise.
    */
    Boolean _checkExistenceOfClass(
        const CIMNamespaceName& nameSpace,
        const CIMName& className);

    CIMConstClass _getClass(
        const CIMNamespaceName& nameSpace,
        const CIMName& className,
        CIMException& cimException);

    // KS TODO Bring all the error functions into a common place and with
    // common naming (ex. check...) common return mechanism.
/*
    Check to determine if we have any providers or repository for this
    class.
    NOTE: There may be issues with this test in general since it is use
    for enumerates, etc. and the goal there is to never exception for
    things like this, simply return no objects.  On the other hand
    This is a real boundary condition for the server where the
    repository is not to be used and there are NO providers registered.
    TODO - Check if this is really valid
*/
    Boolean _checkNoProvidersOrRepository(CIMRequestMessage* request,
        Uint32 providerCount, const CIMName& className);

    /**
        Checks whether the number of providers required to complete an
        operation is greater than the maximum allowed.
        @param nameSpace The target namespace of the operation.
        @param className The name of the class specified in the request.
        @param providerCount The number of providers required to complete the
            operation.
        @exception CIMException if the providerCount is greater than the
            maximum allowed.
    */
    void _checkEnumerateTooBroad(const CIMNamespaceName& nameSpace,
        const CIMName& className, Uint32 providerCount);

    CIMRepository* _repository;

    ProviderRegistrationManager* _providerRegistrationManager;

    // Determines if association requests are allowed. If false, all
    // association and reference requests will be refused.
    Boolean _enableAssociationTraversal;
    Boolean _enableIndicationService;

    // Defines the maximum number of class that is the maximum that 
    // will be allowed on input.  This allows the system to limit the
    // size of enumeration responses, at least in terms of number of
    // classes that can be included in a enumeration.
    Uint32 _maximumEnumerateBreadth;

    // Defines the maximum number of objects that the system will accept
    // for pull operation input parameter.
    Uint32 _systemMaxPullOperationObjectCount;

    // Defines whether the system will accept the value zero as a valid
    // pull interoperation timeout. Since the value of zero disables the
    // timer this would mean that the system operates with no timeout
    // between pull operations. This should be somewhere externally as
    // a configuration parameter
    Boolean _rejectZeroOperationTimeoutValue;

    // KS_TBD - Think this one can go away.
    Uint32 _systemMaxOperationTimeout;

    static Uint64 cimOperationAggregationSN;
    Uint32 _providerManagerServiceId;
#ifdef PEGASUS_ENABLE_OBJECT_NORMALIZATION
    Array<String> _excludeModulesFromNormalization;
#endif

    virtual void _handle_async_request(AsyncRequest* req);

    // the following two methods enable specific query language implementations

    /* void handleQueryRequest(
        CIMExecQueryRequestMessage* request);

    void handleQueryResponseAggregation(
        OperationAggregate* poA);

    void applyQueryToEnumeration(CIMResponseMessage* msg,
        QueryExpressionRep* query);
    */

private:
    static void _handle_enqueue_callback(AsyncOpNode*, MessageQueue*, void*);

    DynamicRoutingTable *_routing_table;
};

PEGASUS_NAMESPACE_END

#endif /* PegasusDispatcher_Dispatcher_h */
