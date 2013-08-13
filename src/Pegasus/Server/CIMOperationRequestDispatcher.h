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
#include <Pegasus/Common/ArrayIterator.h>
#include <Pegasus/Common/Magic.h>

#include <Pegasus/Repository/CIMRepository.h>

#include <Pegasus/Server/CIMServer.h>
#include \
    <Pegasus/Server/ProviderRegistrationManager/ProviderRegistrationManager.h>
#include <Pegasus/Server/Linkage.h>
#include <Pegasus/Server/reg_table.h>

PEGASUS_NAMESPACE_BEGIN
PEGASUS_USING_STD;
//
// Convience Macro to simplify conversion of String to const char*
//
#define CSTRING(ARG) (const char*) ARG.getCString()

/******************************************************************************
**
**  ProviderInfo Class - manage info about classes and providers received
**      from provider lookup functions. Provides info on classes and providers
**      in enumerations and association looksups including provider type,
**      etc. This class is private to the dispatcher.
**
******************************************************************************/

class PEGASUS_SERVER_LINKAGE ProviderInfo
{
public:
    /**
     * Constructor with only className. Used in those cases where
     * the ProviderInfo object is supplied to one of the lookup
     * methods with only classname.  The remaining information (ex.
     * serviceId, controlProviderName, etc. can be added with the
     * addProviderInfo function.
     *
     */
    ProviderInfo(const CIMName& className_)
        : className(className_),
          serviceId(0),
          hasProvider(false),
          hasProviderNormalization(false),
          hasNoQuery(true)
    {
    }
    /**
       Constructor with className, serviceId, and
       controlProviderName. All basic information in the
       constructor.
     */
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
    /**
       Copy constructor
     */
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

    /**
        Method to add serviceID, etc. after the object is constructed.
        Use with the constructor that only adds className to complete
        the basic registration info for the provider.
    */
    void addProviderInfo(Uint32 serviceId_, Boolean hasProvider_,
                         Boolean hasNoQuery_)
    {
        serviceId = serviceId_;
        hasProvider = hasProvider_;
        hasNoQuery = hasNoQuery_;
    }

    CIMName className;
    Uint32 serviceId;
    String controlProviderName;
    Boolean hasProvider;
    Boolean hasProviderNormalization;
    Boolean hasNoQuery;
    AutoPtr<ProviderIdContainer> providerIdContainer;

private:
    // Empty constructor not allowed.
    ProviderInfo()
    {
    }
};
/*
    Container for items of ProviderInfo to manage a list of ProviderInfo
    information objects. The providerCounter
    counts the number of providers in the list not the number of objects
    in the list.  Classes without providers may also exist in the list.
*/
class PEGASUS_SERVER_LINKAGE ProviderInfoList
{
public:
    ProviderInfoList()
    : providerCount(0), _index(0)
    {
    }
    ~ProviderInfoList()
    {
    }
    /** Get size of list. Note This may not match count of providers
    */
    Uint32 size();

    /** Append a class to the list.  This function does NOT
        increment the providerCount. This allows classes that do not
        have providers to be added to the list to be processed by
        the dispatcher.
        @param ProviderInfo defining the class to be appended
     */
    void appendClass(ProviderInfo& x);

    /** append a new Provider to the list.  Note that this function
        increments the ProviderCount.  This count may be different
        than the count of items in the array
        @param x ProviderInfo to append to the list
     */
    void appendProvider(ProviderInfo& x);

    /**  Test to determine if there are more entries in the providerInfoList.
        The getNext() function should be used in conjunction with this to
        get the next entry from the list. When the instance of the class
        is initialized it sets a pointer to the start of the list.  Each
        hasMore() finds the next providerInfo that matches the isProvider
        criteria. When the last entry is processed, it sets the pointer back
        to the start of the list so it can be used again without a formal
        reset function.
        @param isProvider Boolean true if next is request for
        providers or false if it is for classes that do not have a
        provider.
        @return true if more, false if finished

        <p><b>Example:</b>
        
        while (providerInfo.hasMore(x, true)) // loop to return all providers
        or
        while (providerInfo.hasMore(x, false)) // loop to returns classes w/o
            providers
    */
    Boolean hasMore(Boolean isProvider);

    /** Get the current providerInfo item in the  ProviderInfoList
        array. This function must follow the hasMore() function that
        determines if there are more entries in the array.
        while (providerInfoList.hasMore(true))
        {
            providerInfo pI = providerInfoList.getNext()
            ....
        }
        @exception generates IndexOutOfBound exception if the
        request is made after haveMore returns false or possibly
        without executing the hasMore function.
    */
    ProviderInfo& getNext();

    /** get the index for the current entry in the providerInfoList.
        Use this after getNext() to retrieve an index. Used only as
        a counter for displays today.
        @return Uint32
     */
    Uint32 getIndex();

// The following are purely debug functionality and are normally not
// compiled.  To compile them for testing of the results of the
// lookup providers functionality enable the following define.
#define PROVIDER_INFO_DISPLAY
#ifdef PROVIDER_INFO_DISPLAY
    void trace(const CIMNamespaceName& nameSpace, const CIMName& className,
               Boolean assoc = false, const CIMName& assocClass = CIMName());

    // display the classname and hasprovider info
    void display();

#endif

    // Count of providers found. This may differ from size if classes were
    // found that do not have providers.
    Uint32 providerCount;

private:
    Array<ProviderInfo> _array;
    Uint32 _index;
};

// inline methods for ProviderInfoList
inline Uint32 ProviderInfoList::size()
{
    return _array.size();
}

inline void ProviderInfoList::appendClass(ProviderInfo& x)
{
    _array.append(x);
}

inline void ProviderInfoList::appendProvider(ProviderInfo& x)
{
    _array.append(x);
    providerCount++;
}

inline ProviderInfo& ProviderInfoList::getNext()
{
    return _array[_index++];
}
inline Uint32 ProviderInfoList::getIndex()
{
    return _index - 1;
}

#ifdef PROVIDER_INFO_DISPLAY
inline void ProviderInfoList::trace(const CIMNamespaceName& nameSpace,
    const CIMName& className, Boolean assoc, const CIMName& assocClass)
{
    String display;
    for (Uint32 i = 0; i < size() ; i++)
    {
        display.append(_array[i].className.getString());
        display.append(_array[i].hasProvider? "+ ":"- ");
    }
    PEG_TRACE((TRC_DISPATCHER,Tracer::LEVEL1,
        "providerList namespace %s className %s %s%s"
            "  count %u provider count %u %s",
        (const char *)nameSpace.getString().getCString(),
        (const char *)className.getString().getCString(),
        (assoc? "assoc:" : ""),
        (assoc? (const char *)assocClass.getString().getCString() : ""),
        size(), providerCount,
        (const char *)display.getCString()));
}
inline void ProviderInfoList::display()
    {
        for (Uint32 i = 0 ; i < _array.size() ; i++)
        {
            ProviderInfo& pi = _array[i];
            cout << i << " " <<  pi.className.getString() << " "
                 << (pi.hasProvider? "true" : "false") << endl;
        }
    }
#endif
/******************************************************************************
**
**  OperationAggregate Class
**
******************************************************************************/
/* Class to manage the aggregation of data required by post processors. This
    class is private to the dispatcher. An instance is created by the operation
    dispatcher to aggregate request and response information and used by the
    post processor to aggregate responses together. It is not released until
    the request is complete.
    Includes all data from the request used by the dispatcher aggregator
    functions.
*/
class PEGASUS_SERVER_LINKAGE OperationAggregate
{
    friend class CIMOperationRequestDispatcher;
public:
    /** Operation Aggregate constructor.  Builds an aggregate object
        object.
        @param request - CIMOprationRequestMessage containing copy
                       of the original request
        @param className Cimname Classname for the request
        @param nameSpace CIMNamespace Namespace defined for the
                         request
        @param totalIssued Uint32 Sets number of responses expected
        @param requiresHostnameCompletion Boolean flag for
                                          aggregator to execute host
                                          name completion
        @param hasPropList - tells aggregator to move propertylist
                           back to responses.  Required because
                           issue getting responses back from OOP
                           with propertylist entact. (Optional
                           Default = false)
        @param query QueryExpressionRep* The query Statement. Used
        only by CQL & WQL OperationRequestDispatchers (Optional)
        @param String queryLanguage (Optional)
                                              */
    OperationAggregate(CIMOperationRequestMessage* request,
        CIMName className,
        CIMNamespaceName nameSpace,
        Uint32 totalIssued,
        Boolean requiresHostnameCompletion,
        Boolean hasPropList = false,
        QueryExpressionRep* query = 0,
        String queryLanguage = String::EMPTY);

    virtual ~OperationAggregate();

    // Tests validity by checking the magic number we put into the
    // packet.

    Boolean valid() const;

    // Increment the total Operation Requests issued parameter by 1
    void incTotalIssued();

    Uint32 getTotalIssued();

    // Append a new entry to the response list.  Return value indicates
    // whether this response is the last one expected

    Boolean appendResponse(CIMResponseMessage* response);

    Uint32 numberResponses() const;

    CIMOperationRequestMessage* getRequest();

    CIMResponseMessage* getResponse(const Uint32& pos);

    // allow dispatcher to remove the response so it doesn't become
    // destroyed when the poA is destroyed.

    CIMResponseMessage* removeResponse(const Uint32& pos);

    void deleteResponse(const Uint32&pos);

    MessageType getRequestType() const;

    void resequenceResponse(CIMResponseMessage& response);

    String _messageId;
    MessageType _msgRequestType;
    Uint32 _dest;
    CIMName _className;
    CIMNamespaceName _nameSpace;

    // Set upon creation of OA object and used by handleAggregation
    Boolean _requiresHostnameCompletion;
    Boolean _hasPropList;

    Array<String> propertyList;
    QueryExpressionRep* _query;
    String _queryLanguage;

private:
    /** Hidden (unimplemented) copy constructor */
    OperationAggregate(const OperationAggregate& x);

    Array<CIMResponseMessage*> _responseList;
    Mutex _appendResponseMutex;
    Mutex _enqueueResponseMutex;
    CIMOperationRequestMessage* _request;
    Uint32 _totalIssued;
    Uint32 _totalReceived;
    Uint32 _totalReceivedComplete;
    Uint32 _totalReceivedExpected;
    Uint32 _totalReceivedErrors;
    Uint32 _totalReceivedNotSupported;

    Magic<0xC531B144> _magic;
};

inline Uint32 OperationAggregate::getTotalIssued()
{
    return _totalIssued;
}


inline void OperationAggregate::incTotalIssued()
{
    _totalIssued++;
}

inline CIMOperationRequestMessage* OperationAggregate::getRequest()
{
    return _request;
}

inline MessageType OperationAggregate::getRequestType() const
{
    return _msgRequestType;
}

class PEGASUS_SERVER_LINKAGE CIMOperationRequestDispatcher :
    public MessageQueueService
{
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

    static void _forwardForAggregationCallback(
        AsyncOpNode*,
        MessageQueue*,
        void*);

    static void _forwardRequestCallback(
        AsyncOpNode*,
        MessageQueue*,
        void*);

    // Response Handler functions

    void handleOperationResponseAggregation(
        OperationAggregate* poA);

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

    ProviderIdContainer* _updateProviderContainer(
        const CIMNamespaceName& nameSpace,
        const CIMInstance& pInstance,
        const CIMInstance& pmInstance);

    // @exception CIMException
    ProviderInfoList _lookupAllInstanceProviders(
        const CIMNamespaceName& nameSpace,
        const CIMName& className);

    ProviderInfoList _lookupAllAssociationProviders(
        const CIMNamespaceName& nameSpace,
        const CIMObjectPath& objectName,
        const CIMName& assocClass,
        const String& role);

    ProviderInfo _lookupInstanceProvider(
        const CIMNamespaceName& nameSpace,
        const CIMName& className);

    String _lookupMethodProvider(
        const CIMNamespaceName& nameSpace,
        const CIMName& className,
        const CIMName& methodName,
        ProviderIdContainer** providerIdContainer);

    /*Forward the response defined for aggregation processing
      and queue for output.  Note this function is called
      when a response that should be processed through the
      aggregator already exists.
    */
    void _forwardResponseForAggregation(
        CIMOperationRequestMessage* request,
        OperationAggregate* poA,
        CIMResponseMessage* response);

    /*
        Forward a request for aggregation.  This is the path used
        to forward aggregationoperation requests to providers,
        control providers, and services
    */
    void _forwardRequestForAggregation(
        const ProviderInfo& providerInfo,
        CIMOperationRequestMessage* request,
        OperationAggregate* poA);

    /*  Commmon aggregating function used by both _forwardResponseForAggregation
        and _forwardRequestForAggregation above.

    */
    void _forwardRequestForAggregation(
        Uint32 serviceId,
        const String& controlProviderName,
        CIMOperationRequestMessage* request,
        OperationAggregate* poA,
        CIMResponseMessage* response = 0);

    void _forwardRequestToProvider(
        const ProviderInfo& providerInfo,
        CIMOperationRequestMessage* request,
        CIMOperationRequestMessage* requestCopy);

    void _getProviderName(
          const OperationContext& context,
          String& moduleName,
          String& providerName);

    void _logOperation(
        const CIMOperationRequestMessage* request,
        const CIMResponseMessage* response);


    void _enqueueExceptionResponse(
        CIMOperationRequestMessage* request,
        CIMException& exception);

    void _enqueueExceptionResponse(
        CIMOperationRequestMessage* request,
        TraceableCIMException& exception);

    void _enqueueExceptionResponse(
        CIMOperationRequestMessage* request,
        CIMStatusCode code,
        const String& ExtraInfo);

    /** Send the defined response synchronously using the data in
        the OperationAggragate to define destination.  This function
        is used to deliver parts of aggregated responses where each
        response may not be a complete message (i.e chunks). This
        function is controlled by a mutex so that only one chunk may
        be delivered at a time.
        @param poA OperationAggregate* that defines the current
                   state of the aggregated response
        @param response CIMResponseMessage defining the current
                        response segment

        @return Boolean Returns true if the response is complete
                with this segment.
     */
    Boolean _enqueueResponse(
        OperationAggregate*& poA,
        CIMResponseMessage*& response);

    /** Enqueue a simple response aschronously.  Destination is
        determied by the request.
        @param request CIMOperationReqeustMessage that defines
                       destination
        @param response CIMResponseMessage that contains response
                        data
     */
    void _enqueueResponse(
        CIMOperationRequestMessage* request,
        CIMResponseMessage* response);

    CIMValue _convertValueType(const CIMValue& value, CIMType type);

    void _fixInvokeMethodParameterTypes(CIMInvokeMethodRequestMessage* request);

    void _fixSetPropertyValueType(CIMSetPropertyRequestMessage* request);

    // Request Error Response Functions - The following functions test for
    // particular operations parameters, etc. and if the tests fail
    // generate error response messages.  They all follow the same common
    // pattern of returning true if the test fails so that the main
    // function must test the result and return.  This allows putting
    // the trace method return into the CIMOperationRequestDispatcher main.

    Boolean _rejectAssociationTraversalDisabled(
        CIMOperationRequestMessage* request,
        const String& opName);

    Boolean _rejectInvalidRoleParameter(CIMOperationRequestMessage* request,
        const String& roleParameter,
        const String& parameterName);

    Boolean _rejectInvalidObjectPathParameter(
        CIMOperationRequestMessage* request,
        const CIMObjectPath& path);

    Boolean _rejectInvalidClassParameter(CIMOperationRequestMessage* request,
        const CIMNamespaceName& nameSpace,
        const CIMObjectPath& objectName);

    Boolean _rejectInvalidClassParameter(CIMOperationRequestMessage* request,
        const CIMNamespaceName& nameSpace,
        const CIMName& className,
        CIMConstClass& targetClass);
    /**
        Reject if no providers or repository for this class
    */
    Boolean _rejectNoProvidersOrRepository(CIMOperationRequestMessage* request,
        const ProviderInfoList&,
        const CIMName& className);

    /**
        Checks whether the number of providers required to complete an
        operation is greater than the maximum allowed.
        @param nameSpace The target namespace of the operation.
        @param className The name of the class specified in the request.
        @param providerInfoList Contains count of the number of
            providers required to complete the operation.
        @exception CIMException if the providerCount is greater than the
            maximum allowed.
    */
    void _rejectEnumerateTooBroad(
        const CIMNamespaceName& nameSpace,
        const CIMName& className,
        Uint32 providerCount);

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

////  void enumerateInstancesFromRepository(
////      CIMEnumerateInstancesResponseMessage *response,
////      OperationAggregate* poA,
////      const CIMNamespaceName& nameSpace,
////      const CIMName& className,
////      Boolean includeQualifiers = false,
////      Boolean includeClassOrigin = false,
////      const CIMPropertyList& propertyList = CIMPropertyList());

    Boolean _forwardEnumerationToProvider(
        ProviderInfo &providerInfo,
        OperationAggregate* poA,
        CIMOperationRequestMessage* request);

    CIMRepository* _repository;

    ProviderRegistrationManager* _providerRegistrationManager;
    //
    // Enable particular Operations or services
    //
    Boolean _enableAssociationTraversal;
    Boolean _enableIndicationService;

    // Define the maximum number of classes that system will enumerate
    // Allows the system to limit the size of enumeration responses,
    // at least in terms of number of classes that can be included in
    // an enumeration. Does not limit number of objects in enumeration
    // response.
    Uint32 _maximumEnumerateBreadth;

    Uint32 _providerManagerServiceId;
#ifdef PEGASUS_ENABLE_OBJECT_NORMALIZATION
    Array<String> _excludeModulesFromNormalization;
#endif

    // meta dispatcher integration
    virtual void _handle_async_request(AsyncRequest* req);

    Boolean _enumerateFromRepository(
        CIMOperationRequestMessage* request,
        OperationAggregate* poA,
        ProviderInfoList& providerInfos);

private:
    static void _handle_enqueue_callback(AsyncOpNode*, MessageQueue*, void*);


    Boolean _lookupAssociationProvider(
        const CIMNamespaceName& nameSpace,
        const CIMName& assocClass,
        ProviderInfo& providerInfo );

    Array<String> _lookupRegisteredAssociationProvider(
        const CIMNamespaceName& nameSpace,
        const CIMName& assocClass,
        ProviderIdContainer** container);

    Boolean _lookupInternalProvider(
        const CIMNamespaceName& nameSpace,
        const CIMName& className,
        ProviderInfo& providerInfo);

    Boolean _getAssocFromRepository(
        CIMOperationRequestMessage* request,
        OperationAggregate* poA,
        ProviderInfoList& providerInfos,
        CIMResponseMessage* response);

    // Pointer to internal RoutingTable for Control Providers and Services
    DynamicRoutingTable *_routing_table;
};

// Forward response to aggregrator
inline void CIMOperationRequestDispatcher::_forwardResponseForAggregation(
    CIMOperationRequestMessage* request,
    OperationAggregate* poA,
    CIMResponseMessage* response)
{
    _forwardRequestForAggregation(
        getQueueId(),
        String(),
        request,
        poA,
        response);
}

inline void CIMOperationRequestDispatcher::_forwardRequestForAggregation(
    const ProviderInfo& providerInfo,
    CIMOperationRequestMessage* request,
    OperationAggregate* poA)
{
    _forwardRequestForAggregation(
        providerInfo.serviceId,
        providerInfo.controlProviderName,
        request,
        poA);
}

PEGASUS_NAMESPACE_END

#endif /* PegasusDispatcher_Dispatcher_h */
