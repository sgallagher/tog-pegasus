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
// Modified By: Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
//              Mike Day (mdday@us.ibm.com)
//              Yi Zhou (yi_zhou@hp.com)
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                  (carolann_graves@hp.com)
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//		        Karl Schopmeyer (k.schopmeyer@opengroup.org)
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

#include <Pegasus/Server/ProviderRegistrationManager/ProviderRegistrationManager.h>
#include <Pegasus/Server/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

// Class to aggregate and manage the information about classes and providers
// this simply masks some of the confusion of control providers, etc. today.

class PEGASUS_SERVER_LINKAGE ProviderInfo
{
public:
    ProviderInfo()
    {
    }
    ProviderInfo(CIMName& className)
            : _className(className), 
              _magicNumber(54321)
    {
    }
    ProviderInfo(CIMName className, String& serviceName, String& controlProviderName)
        :   _className(className), _serviceName(serviceName),
            _controlProviderName(controlProviderName),
            _magicNumber(54321)
    {
    }
    ~ProviderInfo()
    {
    }
    void setClassName(CIMName className)
    {
    _className = className;
    }
    CIMName getClassName()
    {
        return _className;
    }
    void setServiceName(String& serviceName)
    {
        _serviceName = serviceName; 
    }
    String getServiceName()
    {
        return _serviceName;
    }
String _controlProviderName;
String _serviceName;
CIMName _className;
Boolean _hasProvider;
CIMNamespaceName _nameSpace;

private:
Uint32 _magicNumber;
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
        
        NOTE: This is the version we will generally use
    */

    OperationAggregate(CIMRequestMessage* request,
                                Uint32 msgRequestType,
                                String messageId,
                                Uint32 dest,
                                CIMName className)
    : _messageId(messageId), _msgRequestType(msgRequestType),
      _request(request), _totalIssued(0), _magicNumber(12345),
      _dest(dest), _className(className)
    {}
    /* Constructor with nameSpace object also.
    */
    OperationAggregate(CIMRequestMessage* request,
                                Uint32 msgRequestType,
                                String messageId,
                                Uint32 dest,
                                CIMName className,
                                CIMNamespaceName nameSpace)
    : _messageId(messageId), _msgRequestType(msgRequestType),
      _request(request), _totalIssued(0), _magicNumber(12345),
      _dest(dest), _className(className),_nameSpace(nameSpace)
    {}


    ~OperationAggregate()
    {
        _magicNumber = 0;
        delete _request;
    }
    // Tests validity by checking the magic number we put into the
    // packet.
    Boolean valid() {return(_magicNumber == 12345)? true: false; }
    
    // Returns count of total responses collected into this aggregation
    Uint32 totalIssued() { return _totalIssued; }
    
    // increment the count of requests issued
    void incIssued() { _totalIssued++;}

    // Sets the total issued to the input parameter
    void setTotalIssued(Uint32 i) {_totalIssued = i; }

    // Append a new entry to the response list.  Return value indicates
    // whether this response is the last one expected
    Boolean appendResponse(CIMResponseMessage* response)
    {
        _appendResponseMutex.lock(pegasus_thread_self());
        _responseList.append(response);
        Boolean returnValue = (totalIssued() == numberResponses());
        _appendResponseMutex.unlock();
        return returnValue;
    }


    Uint32 numberResponses() { return _responseList.size(); }

    CIMResponseMessage* getResponse(const Uint32& pos)
    {
	return _responseList[pos];
    }

    void deleteResponse(const Uint32&pos)
    {
        delete _responseList[pos];
        _responseList.remove(pos);
    }

    Uint32 getRequestType(){ return _msgRequestType;}
    
    String _messageId;
    Uint32 _msgRequestType;
    Uint32 _dest;
    CIMNamespaceName _nameSpace;
    CIMName _className;

    Array<CIMName> classes;
    Array<String> serviceNames;
    Array<String> controlProviderNames;
    Array<String> propertyList;

    Uint64 _aggregationSN;
private:
    /** Hidden (unimplemented) copy constructor */
    OperationAggregate(const OperationAggregate& x){ }

    Array<CIMResponseMessage*> _responseList;
    Mutex _appendResponseMutex;
    CIMRequestMessage* _request;
    Uint32 _totalIssued;
    Uint32 _magicNumber;
};
class PEGASUS_SERVER_LINKAGE CIMOperationRequestDispatcher : public MessageQueueService
{
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

      static void handleReferencesResponseAggregation(OperationAggregate* poA);

      static void handleReferenceNamesResponseAggregation(OperationAggregate* poA);

      static void handleAssociatorsResponseAggregation(OperationAggregate* poA);

      static void handleAssociatorNamesResponseAggregation(OperationAggregate* poA);
      
      static void handleEnumerateInstancesResponseAggregation(OperationAggregate* poA);

      static void handleEnumerateInstanceNamesResponseAggregation(OperationAggregate* poA);


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
        const CIMName& className) throw(CIMException);
    
    Boolean _lookupInternalProvider(
        const CIMNamespaceName& nameSpace,
        const CIMName& className,
        String& service,
        String& provider);
    
    Boolean _lookupNewInstanceProvider(
        const CIMNamespaceName& nameSpace, 
        const CIMName& className,
        String& serviceName,
        String& controlProviderName);

    String _lookupInstanceProvider(
        const CIMNamespaceName& nameSpace,
        const CIMName& className);
    
    Array<ProviderInfo> _lookupAllInstanceProviders(
        const CIMNamespaceName& nameSpace,
        const CIMName& className,
        Uint32& providerCount)  throw(CIMException);
    
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
        String& controlProviderName);
    
    Array<String> _lookupAssociationProvider(
        const CIMNamespaceName& nameSpace,
        const CIMName& assocClass);

      String _lookupMethodProvider(const CIMNamespaceName& nameSpace,
    	const CIMName& className, const CIMName& methodName);

      void _forwardRequestToService(
        const String& serviceName,
        CIMRequestMessage* request,
        CIMResponseMessage*& response);

      void _forwardRequestToControlProvider(
        const String& serviceName,
        const String& controlProviderName,
        CIMRequestMessage* request,
        CIMResponseMessage*& response);

      void _forwardRequestForAggregation(
        const String& serviceName,
        const String& controlProviderName,
        CIMRequestMessage* request,
        OperationAggregate* poA);

      void _forwardRequestToProviderManager(
        const CIMName& className,
        const String& serviceName,
        const String& controlProviderName,
        CIMRequestMessage* request);

      void _enqueueResponse(
          CIMRequestMessage* request, CIMResponseMessage* response);
      
      CIMValue _convertValueType(const CIMValue& value, CIMType type);

      void _fixInvokeMethodParameterTypes(CIMInvokeMethodRequestMessage* request);

      void _fixSetPropertyValueType(CIMSetPropertyRequestMessage* request);

      void _checkExistenceOfClass(
          const CIMNamespaceName& nameSpace,
	  const CIMName& className,
	  CIMException& cimException);

      CIMRepository * _repository;

      ProviderRegistrationManager* _providerRegistrationManager;

      AtomicInt _dying;

      Boolean _enableAssociationTraversal;
      Boolean _enableIndicationService;
      Uint32 _maximumEnumerateBreadth;

      // << Tue Feb 12 08:48:09 2002 mdd >> meta dispatcher integration
      virtual void _handle_async_request(AsyncRequest *req);
   private:
      static void _handle_enqueue_callback(AsyncOpNode *, MessageQueue *, void *);

};

PEGASUS_NAMESPACE_END

#endif /* PegasusDispatcher_Dispatcher_h */
