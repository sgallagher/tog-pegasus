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
// Modified By:  Nag Boranna, Hewlett-Packard Company (nagaraja_boranna@hp.com)
//               Chip Vincent (cvincent@us.ibm.com)
//               Yi Zhou, Hewlett-Packard Company (yi_zhou@hp.com)
//               Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
//               Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//               Mike Day (mdday@us.ibm.com)
//               Carol Ann Krug Graves, Hewlett-Packard Company
//                   (carolann_graves@hp.com)
//               Arthur Pichlkostner (via Markus: sedgewick_de@yahoo.de)
//               Jenny Yu, Hewlett-Packard Company (jenny_yu@hp.com)
//               Karl Schopmeyer (k.schopmeyer@opengroup.org)
//
//%/////////////////////////////////////////////////////////////////////////////

#include "CIMOperationRequestDispatcher.h"
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/XmlReader.h> // stringToValue(), stringArrayToValue()
#include <Pegasus/Common/StatisticalData.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/Formatter.h>

PEGASUS_NAMESPACE_BEGIN

PEGASUS_USING_STD;
//#define LIMIT_ENUM_TO_ONE_LEVEL


CIMOperationRequestDispatcher::CIMOperationRequestDispatcher(
    CIMRepository* repository,
    ProviderRegistrationManager* providerRegistrationManager)
      :
      Base(PEGASUS_QUEUENAME_OPREQDISPATCHER),
      _repository(repository),
      _providerRegistrationManager(providerRegistrationManager)
{

   PEG_METHOD_ENTER(TRC_DISPATCHER,
         "CIMOperationRequestDispatcher::CIMOperationRequestDispatcher");

   // Check whether or not AssociationTraversal is supported.
   //
   ConfigManager* configManager = ConfigManager::getInstance();
   _enableAssociationTraversal = String::equal(
        configManager->getCurrentValue("enableAssociationTraversal"), "true");
   _enableIndicationService = String::equal(
        configManager->getCurrentValue("enableIndicationService"), "true");

   PEG_METHOD_EXIT();
}

CIMOperationRequestDispatcher::~CIMOperationRequestDispatcher(void)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDispatcher::~CIMOperationRequestDispatcher");
   _dying = 1;
   PEG_METHOD_EXIT();
}

void CIMOperationRequestDispatcher::_handle_async_request(AsyncRequest *req)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDispatcher::_handle_async_request");

   	// pass legacy operations to handleEnqueue
	if(req->getType() == async_messages::ASYNC_LEGACY_OP_START)
	{
	   req->op->processing();

	   Message * message = (static_cast<AsyncLegacyOperationStart *>(req)->get_action());

	   handleEnqueue(message);

	   PEG_METHOD_EXIT();
	   return;
	}

	// pass all other operations to the default handler
	Base::_handle_async_request(req);
	PEG_METHOD_EXIT();
}

Boolean CIMOperationRequestDispatcher::_lookupInternalProvider(
   const String& nameSpace,
   const String& className,
   String& service,
   String& provider)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::_lookupInternalProvider");
    // Clear the strings since used as test later. Poor code but true now
    service =  String::EMPTY;
    provider = String::EMPTY;

    if (String::equalNoCase(className, PEGASUS_CLASSNAME_CONFIGSETTING) &&
        String::equalNoCase(nameSpace, PEGASUS_NAMESPACENAME_CONFIG))
    {
        service = PEGASUS_QUEUENAME_CONTROLSERVICE;
        provider = PEGASUS_MODULENAME_CONFIGPROVIDER;
	PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL4,
	    "Internal provider  Service = " + service + " provider " + provider + " found.");
        PEG_METHOD_EXIT();
        return true;
    }
    if ((String::equalNoCase(className, PEGASUS_CLASSNAME_AUTHORIZATION) &&
         String::equalNoCase(nameSpace, PEGASUS_NAMESPACENAME_AUTHORIZATION)) ||
        (String::equalNoCase(className, PEGASUS_CLASSNAME_USER) &&
         String::equalNoCase(nameSpace, PEGASUS_NAMESPACENAME_USER)))
    {
        service = PEGASUS_QUEUENAME_CONTROLSERVICE;
        provider = PEGASUS_MODULENAME_USERAUTHPROVIDER;
	PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL4,
	    "Internal provider  Service = " + service + " provider " + provider + " found.");
        PEG_METHOD_EXIT();
        return true;
    }
    if (String::equalNoCase(className, PEGASUS_CLASSNAME_SHUTDOWN) &&
        String::equalNoCase(nameSpace, PEGASUS_NAMESPACENAME_SHUTDOWN))
    {
        service = PEGASUS_QUEUENAME_CONTROLSERVICE;
        provider = PEGASUS_MODULENAME_SHUTDOWNPROVIDER;
	PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL4,
	    "Internal provider  Service = " + service + " provider " + provider + " found.");
        PEG_METHOD_EXIT();
        return true;
    }
    if (String::equalNoCase(className, PEGASUS_CLASSNAME___NAMESPACE) ||
	String::equalNoCase(className, PEGASUS_CLASSNAME_NAMESPACE))
    {
        service = PEGASUS_QUEUENAME_CONTROLSERVICE;
        provider = PEGASUS_MODULENAME_NAMESPACEPROVIDER;
	PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL4,
	    "Internal provider  Service = " + service + " provider " + provider + " found.");
        PEG_METHOD_EXIT();
        return true;
    }

    if ((String::equalNoCase(className, PEGASUS_CLASSNAME_PROVIDERMODULE) ||
         String::equalNoCase(className, PEGASUS_CLASSNAME_PROVIDER) ||
         String::equalNoCase(className, PEGASUS_CLASSNAME_PROVIDERCAPABILITIES)) &&
        String::equalNoCase(nameSpace, PEGASUS_NAMESPACENAME_PROVIDERREG))
    {
        service = PEGASUS_QUEUENAME_CONTROLSERVICE;
        provider = PEGASUS_MODULENAME_PROVREGPROVIDER;
	PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL4,
	    "Internal provider  Service = " + service + " provider " + provider + " found.");
        PEG_METHOD_EXIT();
        return true;
    }
    if (_enableIndicationService &&
        (String::equalNoCase(className, PEGASUS_CLASSNAME_INDSUBSCRIPTION) ||
         String::equalNoCase(className, PEGASUS_CLASSNAME_INDHANDLER) ||
         String::equalNoCase(className, PEGASUS_CLASSNAME_INDHANDLER_CIMXML) ||
         String::equalNoCase(className, PEGASUS_CLASSNAME_INDHANDLER_SNMP) ||
         String::equalNoCase(className, PEGASUS_CLASSNAME_INDFILTER)))
    {
        service = PEGASUS_QUEUENAME_INDICATIONSERVICE;
        provider = String::EMPTY;
	PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL4,
	    "Internal provider  Service = "
	     + service + " provider " + provider + " found.");
        PEG_METHOD_EXIT();
        return true;
    }
    PEG_METHOD_EXIT();
    return false;
}

/* _getSubClassNames - Gets the names of all subclasses of the defined
    class (including the class) and returns it in an array of strings. Uses a similar
    function in the repository class to get the names.  Note that this prepends
    the result with the input classname.
    @param namespace
    @param className
    @return Array of strings with class names.  Note that there should be at least
    one classname in the array (the input name)
    Note that there is a special exception to this function, the __namespace class
    which does not have any representation in the class repository.
    @exception CIMException(CIM_ERR_INVALID_CLASS)
*/
Array<String> CIMOperationRequestDispatcher::_getSubClassNames(
		    String& nameSpace,
		    String& className) throw(CIMException)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
       "CIMOperationRequestDispatcher::_getSubClassNames");

    Array<String> subClassNames;
    //
    // Get names of descendent classes:
    //
    if(!String::equalNoCase(className, PEGASUS_CLASSNAME___NAMESPACE))
    {
    	try
    	{
    	    // Get the complete list of subclass names
    	    _repository->getSubClassNames(nameSpace,
    			 className, true, subClassNames);
    	}
    	catch(CIMException& e)
    	{
    	    // Gets exception back from the getSubClasses if class does not exist
    	    PEG_METHOD_EXIT();
    	    throw e;
    	}
    }
    // Prepend the array with the classname that formed the array.
    subClassNames.prepend(className);
    return subClassNames;
}


/* _lookupInstanceProvider - Looks up the instance provider for the
    classname and namespace.
    Returns the name of the provider.
    ATTN KS: P0 8 May 2002QUESTION: Why are we getting the name of the provider.  
    ATTN: This needs to be combined with lookupnewinstanceprovider to form a single
    function
    be satisfied with the existance and not need the name.
    Shouldn't we be able to change this to a binary return???
*/
String CIMOperationRequestDispatcher::_lookupInstanceProvider(
   const String& nameSpace,
   const String& className)
{
    CIMInstance pInstance;
    CIMInstance pmInstance;
    String providerName;

    PEG_METHOD_ENTER(TRC_DISPATCHER,
                     "CIMOperationRequestDispatcher::_lookupInstanceProvider");
    if (_providerRegistrationManager->lookupInstanceProvider(
	nameSpace, className, pInstance, pmInstance, false))
    {
	// get the provder name
	Uint32 pos = pInstance.findProperty("Name");

	if ( pos != PEG_NOT_FOUND )
	{
	    pInstance.getProperty(pos).getValue().get(providerName);

            PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL4,
                             "providerName = " + providerName + " found.");
            PEG_METHOD_EXIT();
	    return (providerName);
	}
	else
	{
            PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL4,
                   "providerName = " + providerName + ". Provider name not found.");
            PEG_METHOD_EXIT();
   	    return(String::EMPTY);
	}
    }
    else
    {
        PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL4,
                   "Provider for " + className + " not found.");
        PEG_METHOD_EXIT();
   	return(String::EMPTY);
    }
}

/* _lookupNewInstanceProvider - Looks up the internal and/or instance provider
    for the defined namespace and class and returns the serviceName and
    control provider name if a provider is found
    @return true if an service, control provider, or instance provider is found
    for the defined class and namespace.
    This should be combined with the lookupInstanceProvider code eventually but
    the goal now was to simplify the handlers.
    ATTN: KS P3 20 May 2002 Merge this with lookupinstnaceprovider
*/
Boolean CIMOperationRequestDispatcher::_lookupNewInstanceProvider(
				 const String& nameSpace,
                                 const String& className,
				 String& serviceName,
				 String& controlProviderName)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
                     "CIMOperationRequestDispatcher::_lookupNewInstanceProvider");
   //cout << "KSTEST lookupnewinstanceprovider "
   //    << " namespace " << nameSpace
   //    << " class " << className << endl;
   Boolean hasProvider = false;
   String providerName = String::EMPTY;

   // Check for class provided by an internal provider
   if (_lookupInternalProvider(nameSpace, className, serviceName,
           controlProviderName))
       hasProvider = true;
   else
   {
       // get provider for class
       providerName = _lookupInstanceProvider(nameSpace, className);
   }

   if(providerName != String::EMPTY)
   {
       
       serviceName = PEGASUS_QUEUENAME_PROVIDERMANAGER_CPP;
       hasProvider = true;
   }
   PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL4,
       "Lookup Provider = "
	+ serviceName + " provider " + providerName + " found."
        + " return= " + (hasProvider? "true" : "false"));

   PEG_METHOD_EXIT();

   return hasProvider;

}

String CIMOperationRequestDispatcher::_lookupMethodProvider(
   const String& nameSpace,
   const String& className,
   const String& methodName)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::_lookupMethodProvider");

    CIMInstance pInstance;
    CIMInstance pmInstance;
    String providerName;

    if (_providerRegistrationManager->lookupMethodProvider(
	nameSpace, className, methodName, pInstance, pmInstance))
    {
	// get the provder name
	Uint32 pos = pInstance.findProperty("Name");

	if ( pos != PEG_NOT_FOUND )
	{
	    pInstance.getProperty(pos).getValue().get(providerName);

	    PEG_METHOD_EXIT();
	    return (providerName);
	}
	else
	{
	    PEG_METHOD_EXIT();
   	    return(String::EMPTY);
	}
    }
    else
    {
	PEG_METHOD_EXIT();
	return(String::EMPTY);
    }
}

// ATTN-YZ-P1-20020305: Implement this interface
//
// ATTN Markus:
// algorithm: enumerateClassnames, find all association providers
// registered for classes and give back this list

Array<String> CIMOperationRequestDispatcher::_lookupAssociationProvider(
   const String& nameSpace,
   const String& className,
   const String& assocClassName,
   const String& resultClassName)
{
    Array<CIMInstance> pInstances; // Provider
    Array<CIMInstance> pmInstances; // ProviderModule

    Array<String> providerNames;
    String providerName;

    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::_lookupAssociationProvider");

        // assume assocClassName is empty
    // algorithm: enumerateClassnames, find all association providers
    // registered for classes and give back this list

    if (_providerRegistrationManager->lookupAssociationProvider(
            nameSpace, className, assocClassName, resultClassName,
                pInstances, pmInstances))
    {

        for(Uint32 i=0,n=pInstances.size(); i<n; i++)
        {
            // get the provider name
            Uint32 pos = pInstances[i].findProperty("Name");

            if ( pos != PEG_NOT_FOUND )
            {
                pInstances[i].getProperty(pos).getValue().get(providerName);

                PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL4,
                             "providerName = " + providerName + " found.");
                providerNames.append(providerName);
            }
        }
    }

    PEG_METHOD_EXIT();

    return providerNames;
}

/*  Common Dispatcher callback for response aggregation.
*/
void CIMOperationRequestDispatcher::_forwardForAggregationCallback(
    AsyncOpNode *op,
    MessageQueue *q,
    void *parm)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::_forwardForAggregationCallback");

    CIMOperationRequestDispatcher *service =
        static_cast<CIMOperationRequestDispatcher *>(q);

    AsyncRequest *asyncRequest = static_cast<AsyncRequest *>(op->get_request());
    AsyncReply *asyncReply = static_cast<AsyncReply *>(op->get_response());

    OperationAggregate* poA = reinterpret_cast<OperationAggregate*>(parm);
    PEGASUS_ASSERT(poA != 0);

    // Verify that the aggregator is valid.
    PEGASUS_ASSERT(poA->valid());

    CIMResponseMessage *response;

    Uint32 msgType = asyncReply->getType();

    if (msgType == async_messages::ASYNC_LEGACY_OP_RESULT)
    {
        response = reinterpret_cast<CIMResponseMessage *>
            ((static_cast<AsyncLegacyOperationResult *>(asyncReply))->get_result());
    }
    else if (msgType == async_messages::ASYNC_MODULE_OP_RESULT)
    {
        response = reinterpret_cast<CIMResponseMessage *>
            ((static_cast<AsyncModuleOperationResult *>(asyncReply))->get_result());
    }
    else
    {
        // Error
    }

    PEGASUS_ASSERT(response != 0);
    PEGASUS_ASSERT(response->messageId == poA->_messageId);

    poA->appendResponse(response);
    /*cout << "KSTEST counts " 
          << "total issued = " << poA->totalIssued()
          << "number responses = " <<  poA->numberResponses()
          << " Total Operations outstanding = " << size
          << endl;*/

    delete asyncRequest;
    delete asyncReply;
    op->release();
    service->return_op(op);

    // If all responses received, call the postProcessor
    if (poA->totalIssued() == poA->numberResponses())
    {
        service->handleOperationResponseAggregation(poA);
    }

    PEG_METHOD_EXIT();
}


/*  Common Dispatcher callback.
*/
void CIMOperationRequestDispatcher::_forwardRequestCallback(
    AsyncOpNode *op,
    MessageQueue *q,
    void *parm)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::_forwardRequestCallback");

    CIMOperationRequestDispatcher *service =
        static_cast<CIMOperationRequestDispatcher *>(q);

    AsyncRequest *asyncRequest = static_cast<AsyncRequest *>(op->get_request());
    AsyncReply *asyncReply = static_cast<AsyncReply *>(op->get_response());

    CIMResponseMessage *response;

    Uint32 msgType =  asyncReply->getType();

    if(msgType == async_messages::ASYNC_LEGACY_OP_RESULT)
    {
        response = reinterpret_cast<CIMResponseMessage *>
            ((static_cast<AsyncLegacyOperationResult *>(asyncReply))->get_result());
    }
    else if(msgType == async_messages::ASYNC_MODULE_OP_RESULT)
    {
        response = reinterpret_cast<CIMResponseMessage *>
            ((static_cast<AsyncModuleOperationResult *>(asyncReply))->get_result());
    }
    else
    {
        // Error
    }

    PEGASUS_ASSERT(response != 0);
      
    // ensure that the destination queue is in response->dest
#ifdef PEGASUS_ARCHITECTURE_IA64
    response->dest = (Uint64)parm;
#else
    response->dest = (Uint32)parm;
#endif

    PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL3, 
        "Forwarding " + String(MessageTypeToString(response->getType())) + 
        " via Callback to " + 
        ((MessageQueue::lookup(response->dest)) ? 
         String( ((MessageQueue::lookup(response->dest))->getQueueName()) ) : 
         String("BAD queue name")));

    if(parm != 0 )
        service->SendForget(response);
    else 
        delete response;

    delete asyncRequest;
    delete asyncReply;
    op->release();
    service->return_op(op);

    PEG_METHOD_EXIT();
}


void CIMOperationRequestDispatcher::_forwardRequestToService(
    const String& serviceName,
    CIMRequestMessage* request,
    CIMResponseMessage*& response)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::_forwardRequestToService");

    Array<Uint32> serviceIds;
    find_services(serviceName, 0, 0, &serviceIds);
    PEGASUS_ASSERT(serviceIds.size() != 0);

    AsyncOpNode * op = this->get_op();

    AsyncLegacyOperationStart * asyncRequest =
        new AsyncLegacyOperationStart(
	    get_next_xid(),
	    op,
	    serviceIds[0],
	    request,
	    this->getQueueId());

    asyncRequest->dest = serviceIds[0];
    
    PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL3, 
        "Forwarding " + String(MessageTypeToString(request->getType())) + 
        " to service " + serviceName + ". Response should go to queue " + 
        ((MessageQueue::lookup(request->queueIds.top())) ? 
         String( ((MessageQueue::lookup(request->queueIds.top()))->getQueueName()) ) : 
         String("BAD queue name")));

    SendAsync(op, 
	      serviceIds[0],
	      CIMOperationRequestDispatcher::_forwardRequestCallback,
	      this,
	      (void *)request->queueIds.top());

    PEG_METHOD_EXIT();
}

/* Send a OperationsRequest message to a Control provider - Forwards the message
   defined in request to the Control Provider defined in controlProviderName.
   This is an internal function.
*/
void CIMOperationRequestDispatcher::_forwardRequestToControlProvider(
    const String& serviceName,
    const String& controlProviderName,
    CIMRequestMessage* request,
    CIMResponseMessage*& response)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::_forwardRequestToControlProvider");

    Array<Uint32> serviceIds;
    find_services(serviceName, 0, 0, &serviceIds);
    PEGASUS_ASSERT(serviceIds.size() != 0);

    AsyncOpNode * op = this->get_op();

    AsyncModuleOperationStart * moduleControllerRequest =
        new AsyncModuleOperationStart(
	    get_next_xid(),
	    op,
	    serviceIds[0],
	    this->getQueueId(),
	    true,
	    controlProviderName,
	    request);
    
    PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL3, 
        "Forwarding " + String(MessageTypeToString(request->getType())) + 
        " to service " + serviceName +
        ", control provider " + controlProviderName +
        ". Response should go to queue " + 
        ((MessageQueue::lookup(request->queueIds.top())) ? 
         String( ((MessageQueue::lookup(request->queueIds.top()))->getQueueName()) ) : 
         String("BAD queue name")));

    // Send to the Control provider
    SendAsync(op, 
 	      serviceIds[0],
 	      CIMOperationRequestDispatcher::_forwardRequestCallback,
 	      this,
 	      (void *)request->queueIds.top());

    PEG_METHOD_EXIT();
}


/* This function simply decides based on the controlProviderNameField
    whether to forward to Service or ControlProvider.
    If controlProviderName String empty, ToService, else toControlProvider.
    This function specifically forwards requests for response aggregation.
*/
void CIMOperationRequestDispatcher::_forwardRequestForAggregation(
    const String& serviceName,
    const String& controlProviderName,
    CIMRequestMessage* request,
    OperationAggregate* poA)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "CIMOperationRequestDispatcher::_forwardRequestForAggregation");

    Array<Uint32> serviceIds;
    find_services(serviceName, 0, 0, &serviceIds);
    PEGASUS_ASSERT(serviceIds.size() != 0);

    AsyncOpNode * op = this->get_op();

    // If ControlProviderName empty, forward to service.
    if (controlProviderName == String::EMPTY)
    {
	AsyncLegacyOperationStart * asyncRequest =
	    new AsyncLegacyOperationStart(
		get_next_xid(),
		op,
		serviceIds[0],
		request,
		this->getQueueId());

        PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL3, 
            "Forwarding " + String(MessageTypeToString(request->getType())) + 
            " to service " + serviceName + ". Response should go to queue " + 
            ((MessageQueue::lookup(request->queueIds.top())) ? 
             String( ((MessageQueue::lookup(request->queueIds.top()))->getQueueName()) ) : 
             String("BAD queue name")));

	SendAsync(op, 
		  serviceIds[0],
		  CIMOperationRequestDispatcher::_forwardForAggregationCallback,
		  this,
		  poA);
    }
    else
    {
       AsyncModuleOperationStart * moduleControllerRequest =
	   new AsyncModuleOperationStart(
	       get_next_xid(),
	       op,
	       serviceIds[0],
	       this->getQueueId(),
	       true,
	       controlProviderName,
	       request);

        PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL3, 
            "Forwarding " + String(MessageTypeToString(request->getType())) + 
            " to service " + serviceName +
            ", control provider " + controlProviderName +
            ". Response should go to queue " + 
            ((MessageQueue::lookup(request->queueIds.top())) ? 
             String( ((MessageQueue::lookup(request->queueIds.top()))->getQueueName()) ) : 
             String("BAD queue name")));

       SendAsync(op, 
		 serviceIds[0],
		 CIMOperationRequestDispatcher::_forwardForAggregationCallback,
		 this,
		 poA);
    }

    PEG_METHOD_EXIT();
}

/* This function simply decides based on the controlProviderNameField
    whether to forward to Service or ControlProvider.
    If controlProviderName String empty, ToService, else toControlProvider
    Convience coding to simply other functions.
    ATTN:  KS 28 MayWe want to move to use this call exclusively but with the other problems
    I did not make the change universal. 
*/
void CIMOperationRequestDispatcher::_forwardRequest(
    const String& className,        // only for diagnostic
    const String& serviceName,
    const String& controlProviderName,
    CIMRequestMessage* request)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "CIMOperationRequestDispatcher::_forwardRequest");

    Array<Uint32> serviceIds;
    find_services(serviceName, 0, 0, &serviceIds);
    PEGASUS_ASSERT(serviceIds.size() != 0);

    AsyncOpNode * op = this->get_op();

    // If ControlProviderName empty, forward to service.
    if (controlProviderName == String::EMPTY)
    {
	AsyncLegacyOperationStart * asyncRequest =
	    new AsyncLegacyOperationStart(
		get_next_xid(),
		op,
		serviceIds[0],
		request,
		this->getQueueId());

	asyncRequest->dest = serviceIds[0];

        PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL3, 
            "Forwarding " + String(MessageTypeToString(request->getType())) + 
            " on class " + className + " to service " + serviceName +
            ". Response should go to queue " + 
            ((MessageQueue::lookup(request->queueIds.top())) ? 
             String( ((MessageQueue::lookup(request->queueIds.top()))->getQueueName()) ) : 
             String("BAD queue name")));

	SendAsync(op, 
		  serviceIds[0],
		  CIMOperationRequestDispatcher::_forwardRequestCallback,
		  this,
		  (void *)request->queueIds.top());
    }
    else
    {
       AsyncModuleOperationStart * moduleControllerRequest =
	   new AsyncModuleOperationStart(
	       get_next_xid(),
	       op,
	       serviceIds[0],
	       this->getQueueId(),
	       true,
	       controlProviderName,
	       request);

        PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL3, 
            "Forwarding " + String(MessageTypeToString(request->getType())) + 
            " on class " + className + " to service " + serviceName +
            ", control provider " + controlProviderName +
            ". Response should go to queue " + 
            ((MessageQueue::lookup(request->queueIds.top())) ? 
             String( ((MessageQueue::lookup(request->queueIds.top()))->getQueueName()) ) : 
             String("BAD queue name")));

       // Send to the Control provider
       SendAsync(op, 
		 serviceIds[0],
		 CIMOperationRequestDispatcher::_forwardRequestCallback,
		 this,
		 (void *)request->queueIds.top());
    }

    PEG_METHOD_EXIT();
}

/*********************************************************************/
//
//   Temp hack to get responses back to post processors. KS
//   ATTN: This was temporary to isolate the aggregation processing.
//   We need to combine this with the other callbacks to create a single
//   set of functions
//
//   The aggregator includes an aggregation object that is used to 
//   accumulate responses.  It is attached to each request sent and
//   received back as part of the response call back in the "parm"
//   Responses are aggregated until the count reaches the sent count and
//   then the aggregation code is called to create a single response from
//   the accumulated responses.
//
/*********************************************************************/

/* aggregate the responses for enumerateinstancenames into a single response
*/
void CIMOperationRequestDispatcher::handleEnumerateInstanceNamesResponse(OperationAggregate* poA)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "CIMOperationRequestDispatcher::handleEnumerateInstanceNamesResponse");
    CIMEnumerateInstanceNamesResponseMessage * toResponse = 
	(CIMEnumerateInstanceNamesResponseMessage *) poA->getResponse(0);

    // Work backward and delete each response off the end of the array
    for(Uint32 i = poA->numberResponses() - 1; i > 0; i--)
    {
    	CIMEnumerateInstanceNamesResponseMessage *fromResponse = 
    	    (CIMEnumerateInstanceNamesResponseMessage *)poA->getResponse(i);
    
    	for (Uint32 j = 0; j < fromResponse->instanceNames.size(); j++)
    	{
    	    // Duplicate test goes here. 
            // If the from response already contains the name, do not put it.
            /* ATTN: KS 28 May 2002 - Temporarily disable the duplicate delete code.
            if (!Contains( toResponse->instanceNames, fromResponse->instanceNames[j]))
            {
                toResponse->instanceNames.append(fromResponse->instanceNames[j]);
            }
            */
            toResponse->instanceNames.append(fromResponse->instanceNames[j]);
    	}
    	poA->deleteResponse(i);
    }

    toResponse->dest = poA->dest;
    PEG_METHOD_EXIT();
}

/* The function aggregates individual EnumerateInstance Responses into a single response
   for return to the client. It simply aggregates the responses into the
   first response (0).  
   ATTN: KS 28 May 2002 - At this time we do not do the following:
   1. eliminate duplicates.
   2. Order so we eliminate duplicates from top down
   3. prune the properties if localOnly or deepInheritance are set.
   This function does not send any responses. 
*/
void CIMOperationRequestDispatcher::handleEnumerateInstancesResponse(OperationAggregate* poA)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "CIMOperationRequestDispatcher::handleEnumerateInstancesResponse");
    
    CIMEnumerateInstancesResponseMessage * toResponse = 
	(CIMEnumerateInstancesResponseMessage *) poA->getResponse(0);

    // Work backward and delete each response off the end of the array
    for(Uint32 i = poA->numberResponses() - 1; i > 0; i--)
    {
    	CIMEnumerateInstancesResponseMessage *fromResponse = 
    	    (CIMEnumerateInstancesResponseMessage *)poA->getResponse(i);
    
    	for (Uint32 j = 0; j < fromResponse->cimNamedInstances.size(); j++)
    	{
    	    toResponse->cimNamedInstances.append(fromResponse->cimNamedInstances[j]);
    	}
    	poA->deleteResponse(i);
    }
    PEG_METHOD_EXIT();
}

/* handleOperationResponseAggregation - handles all of the general functions of
   aggregation including:
   1. checking for good responses and eliminating any error responses
   2. issuing an error if all responses are bad.
   3. calling the proper function for merging
   4. Issuing the single merged response.
*/
void CIMOperationRequestDispatcher::handleOperationResponseAggregation(
   OperationAggregate* poA)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::handleOperationResponseAggregation");

   Uint32 totalResponses = poA->numberResponses();

   PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL4, 
            Formatter::format(" Response Aggregation  type $1 responses $0",
               totalResponses,
               String(MessageTypeToString(poA->getRequestType()))));
    
   /* If there was only one response, simply forward it on.
   */
   // Add the destination queue ID to the first response.

   CIMResponseMessage* response = poA->getResponse(0);

   response->dest = poA->dest;

   if(totalResponses == 1)
   {
       SendForget(poA->getResponse(0));

       delete poA;

       PEG_METHOD_EXIT();
       return;
   }

    /* Determine if there any "good" responses. If all responses are error
       we return CIMException.
    */
   Uint32 errorCount = 0;
   for(Uint32 i = 0; i < totalResponses; i++)
   {
       CIMResponseMessage *response = poA->getResponse(i);
       if (response->cimException.getCode() != CIM_ERR_SUCCESS)
	   errorCount++;
   }

   PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL4,
            Formatter::format("Post Processor $0 total responses $1 errors"
                              , totalResponses, errorCount));

   //cout << "KSTEST error count = " << errorCount 
   //     << " total Responses = " << totalResponses << endl;
   
   // If all responses are in error
   if(errorCount == totalResponses)
   {
	// Here we need to send some other response error message because
	// which one we pick is a crapshoot.  They could all be different
	// ATTN: For the moment, simply send the first and delete all others.
        SendForget(poA->getResponse(0));
        for(Uint32 j = totalResponses - 1; j > 0; j--)
        {
            poA->deleteResponse(j);
        }
        delete poA;

        PEG_METHOD_EXIT();
        return;
   }

    /* We have at least one good response.  Now delete the error responses.  We will
	not pass them back to the client.
	We remove them from the array. Note that this means that the
	size must be repeatedly recalculated.
    */
   if(errorCount > 0)
   {
       for(Uint32 j = 0; j < poA->numberResponses(); j++)
       {
    	   CIMResponseMessage* response = poA->getResponse(j);
    	   if (response->cimException.getCode() != CIM_ERR_SUCCESS)
    	   {
    	       poA->deleteResponse(j);
               j--;
           }
       }
   }
   /* Merge the responses into a single CIMEnumerateInstanceNamesResponse
   */
    //cout << "KSTEST total Responses " << totalResponses 
    //    << " total Good Responses " << poA->numberResponses()<< endl;

    // If more than one response, go to proper aggregation function
    if(poA->numberResponses() > 1)
    {
    // Multiple responses. Merge them by appending the response components
    // to the first entry

        // Call the appropriate function for merging.
        if (poA->getRequestType() == CIM_ENUMERATE_INSTANCE_NAMES_REQUEST_MESSAGE)
        {
            handleEnumerateInstanceNamesResponse(poA);
        }
        else if (poA->getRequestType() == CIM_ENUMERATE_INSTANCES_REQUEST_MESSAGE)
        {
            handleEnumerateInstancesResponse(poA);
        }
    }

    // Send the remaining response and delete the aggregator.
    SendForget(poA->getResponse(0));
    delete poA;
 
    PEG_METHOD_EXIT();
}
/*******End of the functions hacked for aggregation***************************/

void CIMOperationRequestDispatcher::_enqueueResponse(
   CIMRequestMessage* request,
   CIMResponseMessage* response)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDispatcher::_enqueueResponse");

   // Use the same key as used in the request:

   response->setKey(request->getKey());
   response->dest = request->queueIds.top();

   if( true == Base::_enqueueResponse(request, response))
   {
      PEG_METHOD_EXIT();
      return;
   }

   MessageQueue * queue = MessageQueue::lookup(request->queueIds.top());
   PEGASUS_ASSERT(queue != 0 );

   queue->enqueue(response);

   PEG_METHOD_EXIT();
}

void CIMOperationRequestDispatcher::handleEnqueue(Message *request)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDispatcher::handleEnqueue(Message *request)");
   if(!request)
   {
      PEG_METHOD_EXIT();
      return;
   }

   switch(request->getType())
   {

      case CIM_GET_CLASS_REQUEST_MESSAGE:
	 handleGetClassRequest((CIMGetClassRequestMessage*)request);
	 break;

      case CIM_GET_INSTANCE_REQUEST_MESSAGE:
	 handleGetInstanceRequest((CIMGetInstanceRequestMessage*)request);
	 break;

      case CIM_DELETE_CLASS_REQUEST_MESSAGE:
	 handleDeleteClassRequest(
	    (CIMDeleteClassRequestMessage*)request);
	 break;

      case CIM_DELETE_INSTANCE_REQUEST_MESSAGE:
	 handleDeleteInstanceRequest(
	    (CIMDeleteInstanceRequestMessage*)request);
	 break;

      case CIM_CREATE_CLASS_REQUEST_MESSAGE:
	 handleCreateClassRequest((CIMCreateClassRequestMessage*)request);
	 break;

      case CIM_CREATE_INSTANCE_REQUEST_MESSAGE:
	 handleCreateInstanceRequest(
	    (CIMCreateInstanceRequestMessage*)request);
	 break;

      case CIM_MODIFY_CLASS_REQUEST_MESSAGE:
	 handleModifyClassRequest((CIMModifyClassRequestMessage*)request);
	 break;

      case CIM_MODIFY_INSTANCE_REQUEST_MESSAGE:
	 handleModifyInstanceRequest(
	    (CIMModifyInstanceRequestMessage*)request);
	 break;

      case CIM_ENUMERATE_CLASSES_REQUEST_MESSAGE:
	 handleEnumerateClassesRequest(
	    (CIMEnumerateClassesRequestMessage*)request);
	 break;

      case CIM_ENUMERATE_CLASS_NAMES_REQUEST_MESSAGE:
	 handleEnumerateClassNamesRequest(
	    (CIMEnumerateClassNamesRequestMessage*)request);
	 break;

      case CIM_ENUMERATE_INSTANCES_REQUEST_MESSAGE:
	 handleEnumerateInstancesRequest(
	    (CIMEnumerateInstancesRequestMessage*)request);
	 break;

      case CIM_ENUMERATE_INSTANCE_NAMES_REQUEST_MESSAGE:
	 handleEnumerateInstanceNamesRequest(
	    (CIMEnumerateInstanceNamesRequestMessage*)request);
	 break;

      case CIM_EXEC_QUERY_REQUEST_MESSAGE:
	 handleExecQueryRequest(
	    (CIMExecQueryRequestMessage*)request);
	 break;

      case CIM_ASSOCIATORS_REQUEST_MESSAGE:
	 handleAssociatorsRequest((CIMAssociatorsRequestMessage*)request);
	 break;

      case CIM_ASSOCIATOR_NAMES_REQUEST_MESSAGE:
	 handleAssociatorNamesRequest(
	    (CIMAssociatorNamesRequestMessage*)request);
	 break;

      case CIM_REFERENCES_REQUEST_MESSAGE:
	 handleReferencesRequest((CIMReferencesRequestMessage*)request);
	 break;

      case CIM_REFERENCE_NAMES_REQUEST_MESSAGE:
	 handleReferenceNamesRequest(
	    (CIMReferenceNamesRequestMessage*)request);
	 break;

      case CIM_GET_PROPERTY_REQUEST_MESSAGE:
	 handleGetPropertyRequest(
	    (CIMGetPropertyRequestMessage*)request);
	 break;

      case CIM_SET_PROPERTY_REQUEST_MESSAGE:
	 handleSetPropertyRequest(
	    (CIMSetPropertyRequestMessage*)request);
	 break;

      case CIM_GET_QUALIFIER_REQUEST_MESSAGE:
	 handleGetQualifierRequest((CIMGetQualifierRequestMessage*)request);
	 break;

      case CIM_SET_QUALIFIER_REQUEST_MESSAGE:
	 handleSetQualifierRequest((CIMSetQualifierRequestMessage*)request);
	 break;

      case CIM_DELETE_QUALIFIER_REQUEST_MESSAGE:
	 handleDeleteQualifierRequest(
	    (CIMDeleteQualifierRequestMessage*)request);
	 break;

      case CIM_ENUMERATE_QUALIFIERS_REQUEST_MESSAGE:
	 handleEnumerateQualifiersRequest(
	    (CIMEnumerateQualifiersRequestMessage*)request);
	 break;

      case CIM_INVOKE_METHOD_REQUEST_MESSAGE:
	 handleInvokeMethodRequest(
	    (CIMInvokeMethodRequestMessage*)request);
	 break;
   }

   delete request;

}

// allocate a CIM Operation_async,  opnode, context, and response handler
// initialize with pointers to async top and async bottom
// link to the waiting q
void CIMOperationRequestDispatcher::handleEnqueue()
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDispatcher::handleEnqueue");

   Message* request = dequeue();

   if(request)
      handleEnqueue(request);

   PEG_METHOD_EXIT();
}

void CIMOperationRequestDispatcher::handleGetClassRequest(
   CIMGetClassRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDispatcher::handleGetClassRequest");

   STAT_PROVIDERSTART

   // ATTN: Need code here to expand partial class!

   CIMException cimException;
   CIMClass cimClass;

   _repository->read_lock();

   try
   {
      cimClass = _repository->getClass(
    	 request->nameSpace,
    	 request->className,
    	 request->localOnly,
    	 request->includeQualifiers,
    	 request->includeClassOrigin,
    	 request->propertyList.getPropertyNameArray());
   }
   catch(CIMException& exception)
   {
      cimException = exception;
   }
   catch(Exception& exception)
   {
      cimException =
         PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
   }
   catch(...)
   {
      cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
   }

   _repository->read_unlock();

   STAT_PROVIDEREND

   CIMGetClassResponseMessage* response = new CIMGetClassResponseMessage(
      request->messageId,
      cimException,
      request->queueIds.copyAndPop(),
      cimClass);

   STAT_COPYDISPATCHER_REP

   _enqueueResponse(request, response);
   PEG_METHOD_EXIT();
}


void CIMOperationRequestDispatcher::handleGetInstanceRequest(
   CIMGetInstanceRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDispatcher::handleGetInstanceRequest");

   // ATTN: Need code here to expand partial instance!

   // get the class name
   String className = request->instanceName.getClassName();

   CIMException checkClassException;
   _checkExistenceOfClass(request->nameSpace, className, checkClassException);
   if (checkClassException.getCode() != CIM_ERR_SUCCESS)
   {
      CIMGetInstanceResponseMessage* response =
         new CIMGetInstanceResponseMessage(
            request->messageId,
            checkClassException,
            request->queueIds.copyAndPop(),
            CIMInstance());

      _enqueueResponse(request, response);
      PEG_METHOD_EXIT();
      return;
   }

   //String NameSpace = request->nameSpace;
   // ATTNKSDELETE CIMResponseMessage * response;
   String serviceName = String::EMPTY;
   String controlProviderName = String::EMPTY;
   String providerName = String::EMPTY;
// Temp to save old code until we confirm everything runs 7 May ks ATTN P0
#define NEWTEST
#ifdef NEWTEST
   if(_lookupNewInstanceProvider(request->nameSpace, className, serviceName,
	    controlProviderName))
    {
	CIMGetInstanceRequestMessage* requestCopy =
	    new CIMGetInstanceRequestMessage(*request);

	_forwardRequest(className, serviceName, controlProviderName,
	    requestCopy);
	PEG_METHOD_EXIT();
	return;
    }
#else

   // Check for class provided by an internal provider
   if (_lookupInternalProvider(request->nameSpace, className, serviceName,
           controlProviderName))
       hasProvider = true;
   else
   {
       // get provider for class
       providerName = _lookupInstanceProvider(request->nameSpace, className);
   }

   if(providerName != String::EMPTY)
   {
       serviceName = PEGASUS_QUEUENAME_PROVIDERMANAGER_CPP;
       hasProvider = true;
   }
   if(hasProvider)
   {
      CIMGetInstanceRequestMessage* requestCopy =
          new CIMGetInstanceRequestMessage(*request);

           _forwardRequest(className, serviceName, controlProviderName,
               requestCopy, response);

      PEG_METHOD_EXIT();
      return;
   }
#endif

   // not internal or found provider, go to default
   if (_repository->isDefaultInstanceProvider())
   {
      CIMException cimException;
      CIMInstance cimInstance;

      STAT_PROVIDERSTART

      _repository->read_lock();

      try
      {
         cimInstance = _repository->getInstance(
	    request->nameSpace,
	    request->instanceName,
	    request->localOnly,
	    request->includeQualifiers,
	    request->includeClassOrigin,
	    request->propertyList.getPropertyNameArray());
      }
      catch(CIMException& exception)
      {
         cimException = exception;
      }
      catch(Exception& exception)
      {
         cimException =
            PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
      }
      catch(...)
      {
         cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
      }

      _repository->read_unlock();

      STAT_PROVIDEREND

      CIMGetInstanceResponseMessage* response =
         new CIMGetInstanceResponseMessage(
            request->messageId,
            cimException,
            request->queueIds.copyAndPop(),
            cimInstance);

      STAT_COPYDISPATCHER_REP

      _enqueueResponse(request, response);
   }
   else // No provider is registered and the repository isn't the default
   {
      CIMGetInstanceResponseMessage* response =
         new CIMGetInstanceResponseMessage(
            request->messageId,
            PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, String::EMPTY),
            request->queueIds.copyAndPop(),
            CIMInstance());

      STAT_COPYDISPATCHER

      _enqueueResponse(request, response);
   }
   PEG_METHOD_EXIT();
}

void CIMOperationRequestDispatcher::handleDeleteClassRequest(
   CIMDeleteClassRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDispatcher::handleDeleteClassRequest");

   STAT_PROVIDERSTART

   CIMException cimException;

   _repository->write_lock();

   try
   {
      _repository->deleteClass(
	 request->nameSpace,
	 request->className);
   }

   catch(CIMException& exception)
   {
      cimException = exception;
   }
   catch(Exception& exception)
   {
      cimException =
         PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
   }
   catch(...)
   {
      cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
   }

   _repository->write_unlock();

   STAT_PROVIDEREND

   CIMDeleteClassResponseMessage* response =
      new CIMDeleteClassResponseMessage(
	 request->messageId,
	 cimException,
	 request->queueIds.copyAndPop());

   STAT_COPYDISPATCHER_REP

   _enqueueResponse(request, response);

   PEG_METHOD_EXIT();
}

void CIMOperationRequestDispatcher::handleDeleteInstanceRequest(
   CIMDeleteInstanceRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDispatcher::handleDeleteInstanceRequest");

   // get the class name
   String className = request->instanceName.getClassName();
   CIMResponseMessage * response;
   
   CIMException checkClassException;
   _checkExistenceOfClass(request->nameSpace, className, checkClassException);
   if (checkClassException.getCode() != CIM_ERR_SUCCESS)
   {
      CIMDeleteInstanceResponseMessage* response =
         new CIMDeleteInstanceResponseMessage(
            request->messageId,
            checkClassException,
            request->queueIds.copyAndPop());

      _enqueueResponse(request, response);
      PEG_METHOD_EXIT();
      return;
   }

   String serviceName = String::EMPTY;
   String controlProviderName = String::EMPTY;

   // Check for class provided by an internal provider
   if (_lookupInternalProvider(request->nameSpace, className, serviceName,
           controlProviderName))
   {
      CIMDeleteInstanceRequestMessage* requestCopy =
         new CIMDeleteInstanceRequestMessage(*request);

      _forwardRequest(className,serviceName, controlProviderName,
          requestCopy);

      PEG_METHOD_EXIT();
      return;
   }

   String providerName = _lookupInstanceProvider(request->nameSpace, className);

   if(providerName.size() != 0)
   {
      CIMDeleteInstanceRequestMessage* requestCopy =
          new CIMDeleteInstanceRequestMessage(*request);

      _forwardRequestToService(
          PEGASUS_QUEUENAME_PROVIDERMANAGER_CPP, requestCopy, response);


      PEG_METHOD_EXIT();
      return;
   }
   else if (_repository->isDefaultInstanceProvider())
   {
      CIMException cimException;

      STAT_PROVIDERSTART

      _repository->write_lock();

      try
      {
         _repository->deleteInstance(
	    request->nameSpace,
	    request->instanceName);
      }
      catch(CIMException& exception)
      {
         cimException = exception;
      }
      catch(Exception& exception)
      {
         cimException =
            PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
      }
      catch(...)
      {
         cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
      }

      _repository->write_unlock();

      STAT_PROVIDEREND

      CIMDeleteInstanceResponseMessage* response =
         new CIMDeleteInstanceResponseMessage(
	    request->messageId,
	    cimException,
	    request->queueIds.copyAndPop());

      STAT_COPYDISPATCHER_REP

      _enqueueResponse(request, response);
   }
   else // No provider is registered and the repository isn't the default
   {
      CIMDeleteInstanceResponseMessage* response =
         new CIMDeleteInstanceResponseMessage(
            request->messageId,
            PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, String::EMPTY),
            request->queueIds.copyAndPop());

      STAT_COPYDISPATCHER

      _enqueueResponse(request, response);
   }

   PEG_METHOD_EXIT();
}

void CIMOperationRequestDispatcher::handleCreateClassRequest(
   CIMCreateClassRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDispatcher::handleCreateClassRequest");

   CIMException cimException;

   STAT_PROVIDERSTART

   _repository->write_lock();

   try
   {
      _repository->createClass(
	 request->nameSpace,
	 request->newClass);
   }

   catch(CIMException& exception)
   {
      cimException = exception;
   }
   catch(Exception& exception)
   {
      cimException =
         PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
   }
   catch(...)
   {
      cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
   }

   _repository->write_unlock();

   STAT_PROVIDERSTART

   CIMCreateClassResponseMessage* response =
      new CIMCreateClassResponseMessage(
	 request->messageId,
	 cimException,
	 request->queueIds.copyAndPop());

   STAT_COPYDISPATCHER_REP

   _enqueueResponse(request, response);

   PEG_METHOD_EXIT();
}

void CIMOperationRequestDispatcher::handleCreateInstanceRequest(
   CIMCreateInstanceRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDispatcher::handleCreateInstanceRequest()");

   // get the class name
   String className = request->newInstance.getClassName();
   CIMResponseMessage * response;

   CIMException checkClassException;
   _checkExistenceOfClass(request->nameSpace, className, checkClassException);
   if (checkClassException.getCode() != CIM_ERR_SUCCESS)
   {
      CIMCreateInstanceResponseMessage* response =
         new CIMCreateInstanceResponseMessage(
            request->messageId,
            checkClassException,
            request->queueIds.copyAndPop(),
            CIMObjectPath());

      _enqueueResponse(request, response);
      PEG_METHOD_EXIT();
      return;
   }

   String serviceName = String::EMPTY;
   String controlProviderName = String::EMPTY;

   // Check for class provided by an internal provider
   if (_lookupInternalProvider(request->nameSpace, className, serviceName,
           controlProviderName))
   {
      CIMCreateInstanceRequestMessage* requestCopy =
         new CIMCreateInstanceRequestMessage(*request);

      _forwardRequest(className,serviceName, controlProviderName, requestCopy);

      PEG_METHOD_EXIT();
      return;
   }

   String providerName = _lookupInstanceProvider(request->nameSpace, className);

   if(providerName.size() != 0)
   {
      CIMCreateInstanceRequestMessage* requestCopy =
          new CIMCreateInstanceRequestMessage(*request);

      _forwardRequestToService(
          PEGASUS_QUEUENAME_PROVIDERMANAGER_CPP, requestCopy, response);


      PEG_METHOD_EXIT();
      return;
   }
   else if (_repository->isDefaultInstanceProvider())
   {
      CIMException cimException;
      CIMObjectPath instanceName;

      STAT_PROVIDERSTART

      _repository->write_lock();

      try
      {
         instanceName = _repository->createInstance(
	    request->nameSpace,
	    request->newInstance);
      }
      catch(CIMException& exception)
      {
         cimException = exception;
      }
      catch(Exception& exception)
      {
         cimException =
            PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
      }
      catch(...)
      {
         cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
      }

      _repository->write_unlock();

      STAT_PROVIDEREND

      CIMCreateInstanceResponseMessage* response =
         new CIMCreateInstanceResponseMessage(
	    request->messageId,
	    cimException,
	    request->queueIds.copyAndPop(),
	    instanceName);

      STAT_COPYDISPATCHER_REP

      _enqueueResponse(request, response);
   }
   else // No provider is registered and the repository isn't the default
   {
      CIMCreateInstanceResponseMessage* response =
         new CIMCreateInstanceResponseMessage(
            request->messageId,
            PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, String::EMPTY),
            request->queueIds.copyAndPop(),
            CIMObjectPath());

      STAT_COPYDISPATCHER

      _enqueueResponse(request, response);
   }

   PEG_METHOD_EXIT();
}

void CIMOperationRequestDispatcher::handleModifyClassRequest(
   CIMModifyClassRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDispatcher::handleModifyClassRequest");

   CIMException cimException;

   STAT_PROVIDERSTART

   _repository->write_lock();

   try
   {
      _repository->modifyClass(
	 request->nameSpace,
	 request->modifiedClass);
   }

   catch(CIMException& exception)
   {
      cimException = exception;
   }
   catch(Exception& exception)
   {
      cimException =
         PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
   }
   catch(...)
   {
      cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
   }

   _repository->write_unlock();

   STAT_PROVIDEREND

   CIMModifyClassResponseMessage* response =
      new CIMModifyClassResponseMessage(
	 request->messageId,
	 cimException,
	 request->queueIds.copyAndPop());

   STAT_COPYDISPATCHER_REP

   _enqueueResponse(request, response);

   PEG_METHOD_EXIT();
}

void CIMOperationRequestDispatcher::handleModifyInstanceRequest(
   CIMModifyInstanceRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDispatcher::handleModifyInstanceRequest");

   // ATTN: Who makes sure the instance name and the instance match?
   // ATTN: KS May 28. Change following to reflect new instancelookup
   // get the class name
   String className = request->modifiedInstance.getClassName();
   CIMResponseMessage * response;

   CIMException checkClassException;
   _checkExistenceOfClass(request->nameSpace, className, checkClassException);
   if (checkClassException.getCode() != CIM_ERR_SUCCESS)
   {
      CIMModifyInstanceResponseMessage* response =
         new CIMModifyInstanceResponseMessage(
            request->messageId,
            checkClassException,
            request->queueIds.copyAndPop());

      _enqueueResponse(request, response);
      PEG_METHOD_EXIT();
      return;
   }

   String serviceName = String::EMPTY;
   String controlProviderName = String::EMPTY;

   // Check for class provided by an internal provider
   if (_lookupInternalProvider(request->nameSpace, className, serviceName,
           controlProviderName))
   {
      CIMModifyInstanceRequestMessage* requestCopy =
         new CIMModifyInstanceRequestMessage(*request);

      _forwardRequest(className, serviceName, controlProviderName,
          requestCopy);

      PEG_METHOD_EXIT();
      return;
   }

   // check the class name for an "external provider"
   String providerName = _lookupInstanceProvider(request->nameSpace, className);

   if(providerName.size() != 0)
   {
      CIMModifyInstanceRequestMessage* requestCopy =
          new CIMModifyInstanceRequestMessage(*request);

      _forwardRequestToService(
          PEGASUS_QUEUENAME_PROVIDERMANAGER_CPP, requestCopy, response);

      PEG_METHOD_EXIT();
      return;
   }
   else if (_repository->isDefaultInstanceProvider())
   {
      // translate and forward request to repository
      CIMException cimException;

      STAT_PROVIDERSTART

      _repository->write_lock();

      try
      {
         _repository->modifyInstance(
    	    request->nameSpace,
    	    request->modifiedInstance,
    	    request->includeQualifiers,request->propertyList);
      }
      catch(CIMException& exception)
      {
         cimException = exception;
      }
      catch(Exception& exception)
      {
         cimException =
            PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
      }
      catch(...)
      {
         cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
      }

      _repository->write_unlock();

      STAT_PROVIDEREND

      CIMModifyInstanceResponseMessage* response =
         new CIMModifyInstanceResponseMessage(
	    request->messageId,
	    cimException,
	    request->queueIds.copyAndPop());

      STAT_COPYDISPATCHER_REP

      _enqueueResponse(request, response);
   }
   else // No provider is registered and the repository isn't the default
   {
      CIMModifyInstanceResponseMessage* response =
         new CIMModifyInstanceResponseMessage(
	    request->messageId,
            PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, String::EMPTY),
	    request->queueIds.copyAndPop());

      STAT_COPYDISPATCHER

      _enqueueResponse(request, response);
   }

   PEG_METHOD_EXIT();
}

void CIMOperationRequestDispatcher::handleEnumerateClassesRequest(
   CIMEnumerateClassesRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDispatcher::handleEnumerateClassesRequest");

   CIMException cimException;

   STAT_PROVIDERSTART

   Array<CIMClass> cimClasses;

   _repository->read_lock();

   try
   {
      cimClasses = _repository->enumerateClasses(
    	 request->nameSpace,
    	 request->className,
    	 request->deepInheritance,
    	 request->localOnly,
    	 request->includeQualifiers,
    	 request->includeClassOrigin);
   }

   catch(CIMException& exception)
   {
      cimException = exception;
   }
   catch(Exception& exception)
   {
      cimException =
         PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
   }
   catch(...)
   {
      cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
   }

   _repository->read_unlock();

   STAT_PROVIDEREND

   CIMEnumerateClassesResponseMessage* response =
      new CIMEnumerateClassesResponseMessage(
	 request->messageId,
	 cimException,
	 request->queueIds.copyAndPop(),
	 cimClasses);

   STAT_COPYDISPATCHER_REP

   _enqueueResponse(request, response);

   PEG_METHOD_EXIT();
}

void CIMOperationRequestDispatcher::handleEnumerateClassNamesRequest(
   CIMEnumerateClassNamesRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDispatcher::handleEnumerateClassNamesRequest");

   CIMException cimException;

   STAT_PROVIDERSTART

   Array<String> classNames;

   _repository->read_lock();

   try
   {
      classNames = _repository->enumerateClassNames(
	 request->nameSpace,
	 request->className,
	 request->deepInheritance);
   }

   catch(CIMException& exception)
   {
      cimException = exception;
   }
   catch(Exception& exception)
   {
      cimException =
         PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
   }
   catch(...)
   {
      cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
   }

   _repository->read_unlock();

   STAT_PROVIDEREND

   CIMEnumerateClassNamesResponseMessage* response =
      new CIMEnumerateClassNamesResponseMessage(
	 request->messageId,
	 cimException,
	 request->queueIds.copyAndPop(),
	 classNames);

   STAT_COPYDISPATCHER_REP

   _enqueueResponse(request, response);

   PEG_METHOD_EXIT();
}

void CIMOperationRequestDispatcher::handleEnumerateInstancesRequest(
   CIMEnumerateInstancesRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDispatcher::handleEnumerateInstancesRequest");

   // get the class name
   String className = request->className;

   CIMException checkClassException;
   _checkExistenceOfClass(request->nameSpace, className, checkClassException);
   if (checkClassException.getCode() != CIM_ERR_SUCCESS)
   {
      CIMEnumerateInstancesResponseMessage* response =
         new CIMEnumerateInstancesResponseMessage(
            request->messageId,
            checkClassException,
            request->queueIds.copyAndPop(),
            Array<CIMInstance>());

      _enqueueResponse(request, response);
      PEG_METHOD_EXIT();
      return;
   }

   /***** ATTN: KS 28 May 2002 - localonly and deepinheritance processing
        temporarily removed until testing complete.
   // If localonly or deepinheritance, build propertylist.
   // unless there is specific propertylist. Assume that
   // specific property list has priority over that generated
   // from localOnly
   Array<String> classProperties;
   if (!request->deepInheritance || request->localOnly)
   {
       if (request->propertyList.isNull())
       {
           CIMClass cl;
           // add try here
           // get the class.
           cl = _repository->getClass(
               request->nameSpace,
               request->className,
               true, false, false);
           for (Uint32 i = 0; i < cl.getPropertyCount(); i++)
           {
                CIMProperty p = cl.getProperty(i);
                if (request->localOnly)
                {
                    if (!p.getPropagated())
                        classProperties.append(p.getName());
                }
                else
                    classProperties.append(p.getName());
           }
        // We now have property list and can either send it to 
        // the provider or save for review of the responses.
        }
      
   }
   NOTE: See the piece below that puts it into the PoA
   **************************************************/
   //
   // Get names of descendent classes:
   //
   Array<String> subClassNames;
   CIMException cimException;
   try
   {
       subClassNames = _getSubClassNames(request->nameSpace,className);
   }
   catch(CIMException& cimException)
   {
       // Return exception response if exception from getSubClasses
       CIMEnumerateInstancesResponseMessage* response =
          new CIMEnumerateInstancesResponseMessage(
             request->messageId,
             cimException,
             request->queueIds.copyAndPop(),
             Array<CIMInstance>());
       _enqueueResponse(request, response);
       PEG_METHOD_EXIT();
       return;
   }
   STAT_PROVIDERSTART
    
   //
   // Find all providers for these classes and modify the subclass list.
   // The following arrays represent the list of subclasses with
   // valid providers
   Array<String> subClassNameList;
   Array<String> serviceNames;
   Array<String> controlProviderNames;

   for(Uint32 i = 0; i < subClassNames.size(); i++)
   {
       String serviceName = String::EMPTY;
       String controlProviderName = String::EMPTY;
    
       // Lookup any instance providers and add to send list
       if(_lookupNewInstanceProvider(request->nameSpace, subClassNames[i],
           serviceName, controlProviderName))
       {
           // Append the returned values to the list to send.
           subClassNameList.append(subClassNames[i]); 
           serviceNames.append(serviceName);
           controlProviderNames.append(controlProviderName);

          //  cout << "KSTEST class list Output. Class = " << subClassNames[i] 
//            << " servicename = " << serviceName
//            << " controlProviderName = " 
//            << ((controlProviderName != String::EMPTY)  ? controlProviderName : "None") 
//            << " i = " << i << endl;

           PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL4,
               "Provider found for Class = " + subClassNames[i] 
               + " servicename = " + serviceName
               + " controlProviderName = " 
               + ((controlProviderName != String::EMPTY)  ? controlProviderName : "None"));
       }
   }

   Uint32 ps = serviceNames.size();

   // Test for "enumerate to Broad" and if so, execute exception.
   // Simply limits the subclass depth to some max, not number
   // of instances returned.
   // ATTN: KS P3 May 28 2002add size and limit to this message.
   if(ps > MAX_ENUMERATE_BREADTH)
   {
       CIMEnumerateInstancesResponseMessage* response =
         new CIMEnumerateInstancesResponseMessage(
            request->messageId,
            PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, "Enumerate request too broad"),
            request->queueIds.copyAndPop(),
            Array<CIMInstance>());

      STAT_COPYDISPATCHER

      _enqueueResponse(request, response);
      PEG_METHOD_EXIT();
      return;
   }
   // Set up an aggregate object for the information and save request in it.
   // With the original request message.
   OperationAggregate *poA= new OperationAggregate(
       new CIMEnumerateInstancesRequestMessage(*request),
       request->getType(),
       request->messageId);
   poA->dest = request->queueIds.top();

   Boolean staticInstancesExist = false;
   // ATTN: KS 28 May 2002 P3 - This code to process property lists and
   // localonly, etc. has not been tested yet. Temporarily removed.
   /************************* Removed along with rest of property list code above
   if (classProperties.size() != 0)
   {
       poA->propertyList = classProperties;
   }
   **********************************/
   if ((ps == 0) && (_repository->isDefaultInstanceProvider()))
   {
      Tracer::trace(TRC_DISPATCHER, Tracer::LEVEL4,
	 "Repository being used as THE default instance provider");
      CIMException cimException;

      STAT_PROVIDERSTART

      Array<CIMInstance> cimNamedInstances;

      _repository->read_lock();

      try
      {
         cimNamedInstances = _repository->enumerateInstancesForClass(
    	    request->nameSpace,
    	    request->className,
    	    request->deepInheritance,
    	    request->localOnly,
    	    request->includeQualifiers,
    	    request->includeClassOrigin,
    	    true,
    	    request->propertyList.getPropertyNameArray());
      }
      catch(CIMException& exception)
      {
         cimException = exception;
      }
      catch(Exception& exception)
      {
         cimException =
            PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
      }
      catch(...)
      {
         cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
      }

      _repository->read_unlock();

      STAT_PROVIDEREND

      CIMEnumerateInstancesResponseMessage* response =
        new CIMEnumerateInstancesResponseMessage(
        request->messageId,
        cimException,
        request->queueIds.copyAndPop(),
        cimNamedInstances);

      STAT_COPYDISPATCHER_REP

#if 0
      // if there will be other responses, put this on the response list.
      // else, simply issue it.
      if (ps > 0)
      {
          poA->appendResponse(response);
          staticInstancesExist = true;
      }
      else
      {
          delete poA;
          _enqueueResponse(request, response);
          PEG_METHOD_EXIT();
          return;
      }
#else
      delete poA;
      _enqueueResponse(request, response);
      PEG_METHOD_EXIT();
      return;
#endif
   }

   if(ps > 0)
   {
       //************* Limit to one response if we have problems with more.
#ifdef LIMIT_ENUM_TO_ONE_LEVEL
       ps = 1;
#endif
       // Simply takes the top level request and ignores the others.
       // decomment the ps = 1 to generate only one request.
       //*******************/
       poA->setTotalIssued(ps);

       for(Uint32 i = 0; i < ps; i++ )
       {
           poA->classes.append(subClassNameList[i]);
           poA->serviceNames.append(serviceNames[i]);
           poA->controlProviderNames.append(controlProviderNames[i]);

           // Note that current may not be same as i if there is a response
           // from the repository.
           Uint32 current = poA->classes.size() - 1;

           CIMEnumerateInstancesRequestMessage* requestCopy =
    	     new CIMEnumerateInstancesRequestMessage(*request);
           requestCopy->className = poA->classes[current];

    	   PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL4, 
               Formatter::format(
               "Enumerate instance Req. class $0 to svc $1 for control provider $2 No $3 of $4",
               poA->classes[current], poA->serviceNames[current],
               ((poA->controlProviderNames[current] != String::EMPTY)  ?
                    poA->controlProviderNames[current] : "None"),
               i, ps));

           _forwardRequestForAggregation(poA->serviceNames[current],
                poA->controlProviderNames[current], requestCopy, poA);
       }
       PEG_METHOD_EXIT();
       return;
   }
   // No provider is registered and the repository isn't the default. Error response
   // ATTN: KS 28 May 2002 - Rethink this error. What if there are simply no instances
   // Is this what we generate?
   if ((ps == 0) && !(_repository->isDefaultInstanceProvider()))
   {         
       PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL4, 
                        "No provider for " + className + " and subclasses."); 

       CIMEnumerateInstancesResponseMessage* response =
          new CIMEnumerateInstancesResponseMessage(
             request->messageId,
             PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, String::EMPTY),
             request->queueIds.copyAndPop(),
             Array<CIMInstance>());

      STAT_COPYDISPATCHER

      delete poA;
      _enqueueResponse(request, response);
   }

   PEG_METHOD_EXIT();
}


void CIMOperationRequestDispatcher::handleEnumerateInstanceNamesRequest(
   CIMEnumerateInstanceNamesRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDispatcher::handleEnumerateInstanceNamesRequest");

   // get the class name
   String className = request->className;

   CIMException checkClassException;
   _checkExistenceOfClass(request->nameSpace, className, checkClassException);
   if (checkClassException.getCode() != CIM_ERR_SUCCESS)
   {
      CIMEnumerateInstanceNamesResponseMessage* response =
         new CIMEnumerateInstanceNamesResponseMessage(
            request->messageId,
            checkClassException,
            request->queueIds.copyAndPop(),
            Array<CIMObjectPath>());

      _enqueueResponse(request, response);
      PEG_METHOD_EXIT();
      return;
   }

   //
   // Get names of descendent classes:
   //
   Array<String> subClassNames;
   CIMException cimException;
   try
   {
       subClassNames = _getSubClassNames(request->nameSpace,className);
   }
   catch(CIMException& exception)
   {
       // Return exception response if exception from getSubClasses
       cimException = exception;
       Array<CIMObjectPath> instanceNames;
       CIMEnumerateInstanceNamesResponseMessage* response =
       new CIMEnumerateInstanceNamesResponseMessage(
	     request->messageId,
	     cimException,
	     request->queueIds.copyAndPop(),
	     instanceNames);
       _enqueueResponse(request, response);
       PEG_METHOD_EXIT();
       return;
   }

   STAT_PROVIDERSTART
    
   //
   //Find all providers for these classes and modify the subclass list.
   //
   // The following arrays represent the list of subclasses with
   // valid providers
   Array<String> subClassNameList;
   Array<String> serviceNames;
   Array<String> controlProviderNames;

   for(Uint32 i = 0; i < subClassNames.size(); i++)
   {
       String serviceName = String::EMPTY;
       String controlProviderName = String::EMPTY;
    
       // Lookup any instance providers and add to send list
       if(_lookupNewInstanceProvider(request->nameSpace, subClassNames[i],
           serviceName, controlProviderName))
		{
		   // Append the returned values to the list to send.
		   subClassNameList.append(subClassNames[i]); 
		   serviceNames.append(serviceName);
		   controlProviderNames.append(controlProviderName);
	
            PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL4,
               "Provider found for Class = " + subClassNames[i] 
			   + " servicename = " + serviceName
			   + " controlProviderName = " 
               + ((controlProviderName != String::EMPTY)  ? controlProviderName : "None"));
		}
   }

   Uint32 ps = serviceNames.size();

   // Test for "enumerate to Broad" and if so, execute exception.
   // Simply limits the subclass depth to some max, not number
   // of instances returned.
   // ATTN: add size and limit to the message.
   if(ps > MAX_ENUMERATE_BREADTH)
   {
       CIMEnumerateInstanceNamesResponseMessage* response =
         new CIMEnumerateInstanceNamesResponseMessage(
            request->messageId,
            PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, "Enumerate request too Broad"),
            request->queueIds.copyAndPop(),
            Array<CIMObjectPath>());

      STAT_COPYDISPATCHER

      _enqueueResponse(request, response);
      PEG_METHOD_EXIT();
      return;
       
   }
   // Set up an aggregate object for the information and save request in it.
   // With the original request message.

   OperationAggregate *poA= new OperationAggregate(
       new CIMEnumerateInstanceNamesRequestMessage(*request),
       request->getType(),
       request->messageId);
   poA->dest = request->queueIds.top();

   Boolean staticInstancesExist = false;

   if ((ps == 0) && (_repository->isDefaultInstanceProvider()))
   {
      Tracer::trace(TRC_DISPATCHER, Tracer::LEVEL4,
	 "Repository being used as THE default instance provider");
      CIMException cimException;
      STAT_PROVIDERSTART
      Array<CIMObjectPath> instanceNames;
      _repository->read_lock();
      try
      {
         instanceNames = _repository->enumerateInstanceNamesForClass(
            request->nameSpace,
            request->className,
            true);
      }
      catch(CIMException& exception)
      {
         cimException = exception;
      }
      catch(Exception& exception)
      {
         cimException =
            PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
      }
      catch(...)
      {
         cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
      }
      _repository->read_unlock();

      STAT_PROVIDEREND

      CIMEnumerateInstanceNamesResponseMessage* response =
        new CIMEnumerateInstanceNamesResponseMessage(
        request->messageId,
        cimException,
        request->queueIds.copyAndPop(),
        instanceNames);

      STAT_COPYDISPATCHER_REP

#if 0
//ATTN-DME-P1-20020529: If providers exist then the repository should not
//     be called.
      // If there will be other responses, put this on the response list.
      // else, simply issue it.
      if (ps > 0)
      {
          poA->appendResponse(response);
          staticInstancesExist = true;
      }
      else
      {
          delete poA;
          _enqueueResponse(request, response);
          PEG_METHOD_EXIT();
          return;
      }
#else
      delete poA;
      _enqueueResponse(request, response);
      PEG_METHOD_EXIT();
      return;
#endif
   }

   if(ps > 0)
   {
       //************* Limit to one response if we have problems with more.
#ifdef LIMIT_ENUM_TO_ONE_LEVEL
       ps = 1;
#endif
       // Simply takes the top level request and ignores the others.
       //*******************/
       poA->setTotalIssued(ps);

       for(Uint32 i = 0; i < ps; i++ )
       {
           poA->classes.append(subClassNameList[i]);
           poA->serviceNames.append(serviceNames[i]);
           poA->controlProviderNames.append(controlProviderNames[i]);
           
           Uint32 current =  poA->classes.size() - 1;
           //request->print(cout, true);

           CIMEnumerateInstanceNamesRequestMessage* requestCopy =
    	     new CIMEnumerateInstanceNamesRequestMessage(*request);

           requestCopy->className = poA->classes[current];

           //request->print(cout, true);
           //requestCopy->print(cout, true);

    	   PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL4, 
               Formatter::format(
               "EnumerateNames Req. class $0 to svc $1 for control provider $2 No $3 of $4",
               poA->classes[current], poA->serviceNames[current],
               ((poA->controlProviderNames[current] != String::EMPTY)  ?
                    poA->controlProviderNames[current] : "None"),
               i, ps));
            
    	   /*cout << "KSTEST send to provider, Class =  " << classes[current]
               << " servicename = " << poA->serviceNames[current]
               << " control provider name = " 
               << ((poA->controlProviderNames[current] != String::EMPTY)  ?
                    poA->controlProviderNames[current] : "None")
    	       << " count " << current << endl;

           for(Uint32 j = 0; j <= i; j++ )
           {
               cout << "KSTEST View Class Arrays "
                    << " Class = " <<   poA->classes[j]
                    << " Service Name = " << poA->serviceNames[j]
                    << " Control Provider = " << ((poA->controlProviderNames[j]
                         != String::EMPTY)  ?
                           poA->controlProviderNames[j] : "None") 
    	            << " count " << j << endl;
           }*/
           
    	  _forwardRequestForAggregation(poA->serviceNames[current],
    	       poA->controlProviderNames[current], requestCopy, poA);
       }
       PEG_METHOD_EXIT();
       return;
   }
   // No provider is registered and the repository isn't the default
   if ((ps == 0) && !(_repository->isDefaultInstanceProvider()))
   {         
       PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL4, 
                        "No provider for " + className + " and subclasses."); 

       CIMEnumerateInstanceNamesResponseMessage* response =
         new CIMEnumerateInstanceNamesResponseMessage(
            request->messageId,
            PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, String::EMPTY),
            request->queueIds.copyAndPop(),
            Array<CIMObjectPath>());

      STAT_COPYDISPATCHER

      delete poA;
      _enqueueResponse(request, response);
   }

   PEG_METHOD_EXIT();
}

void CIMOperationRequestDispatcher::handleAssociatorsRequest(
   CIMAssociatorsRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDispatcher::handleAssociatorsRequest");

   if (!_enableAssociationTraversal)
   {
       CIMException cimException =
           PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, "Associators");
       Array<CIMObject> cimObjects;

       CIMAssociatorsResponseMessage* response =
           new CIMAssociatorsResponseMessage(
               request->messageId,
	       cimException,
	       request->queueIds.copyAndPop(),
	       cimObjects);

       STAT_COPYDISPATCHER

       _enqueueResponse(request, response);

       PEG_METHOD_EXIT();
       return;
   }

   String className = request->objectName.getClassName();
   String assocClassName = request->assocClass;
   String resultClassName = request->resultClass;

   CIMResponseMessage * response;

   // check the class name for an "external provider"
   Array<String> providerNames = _lookupAssociationProvider(request->nameSpace, className);

   if(providerNames.size() != 0)
   {
      CIMAssociatorsRequestMessage* requestCopy =
          new CIMAssociatorsRequestMessage(*request);

      _forwardRequestToService(
          PEGASUS_QUEUENAME_PROVIDERMANAGER_CPP, requestCopy, response);

      PEG_METHOD_EXIT();
      return;
   }
   else if (_repository->isDefaultInstanceProvider())
   {
      CIMException cimException;

      STAT_PROVIDERSTART

      Array<CIMObject> cimObjects;

      _repository->read_lock();

      try
      {
         cimObjects = _repository->associators(
    	    request->nameSpace,
    	    request->objectName,
    	    request->assocClass,
    	    request->resultClass,
    	    request->role,
    	    request->resultRole,
    	    request->includeQualifiers,
    	    request->includeClassOrigin,
    	    request->propertyList.getPropertyNameArray());
      }
      catch(CIMException& exception)
      {
         cimException = exception;
      }
      catch(Exception& exception)
      {
         cimException =
            PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
      }
      catch(...)
      {
         cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
      }

      _repository->read_unlock();

      STAT_PROVIDEREND

      CIMAssociatorsResponseMessage* response =
         new CIMAssociatorsResponseMessage(
	    request->messageId,
	    cimException,
	    request->queueIds.copyAndPop(),
	    cimObjects);

      STAT_COPYDISPATCHER_REP

      _enqueueResponse(request, response);
   }
   else // No provider is registered and the repository isn't the default
   {
      CIMAssociatorsResponseMessage* response =
         new CIMAssociatorsResponseMessage(
            request->messageId,
            PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, String::EMPTY),
            request->queueIds.copyAndPop(),
            Array<CIMObject>());

      STAT_COPYDISPATCHER

      _enqueueResponse(request, response);
   }

   PEG_METHOD_EXIT();
}

void CIMOperationRequestDispatcher::handleAssociatorNamesRequest(
   CIMAssociatorNamesRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDispatcher::handleAssociatorNamesRequest");

   if (!_enableAssociationTraversal)
   {
       CIMException cimException =
           PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, "AssociatorNames");
       Array<CIMObjectPath> cimObjects;

       CIMAssociatorNamesResponseMessage* response =
           new CIMAssociatorNamesResponseMessage(
               request->messageId,
	       cimException,
	       request->queueIds.copyAndPop(),
	       cimObjects);

       STAT_COPYDISPATCHER

       _enqueueResponse(request, response);

       PEG_METHOD_EXIT();
       return;
   }

   String className = request->objectName.getClassName();
   String resultClassName = request->resultClass;
   CIMResponseMessage * response;

   // check the class name for an "external provider"
   Array<String> providerNames = _lookupAssociationProvider(request->nameSpace, className);

   if(providerNames.size() != 0)
   {
      CIMAssociatorNamesRequestMessage* requestCopy =
          new CIMAssociatorNamesRequestMessage(*request);

      _forwardRequestToService(
          PEGASUS_QUEUENAME_PROVIDERMANAGER_CPP, requestCopy, response);

      PEG_METHOD_EXIT();
      return;
   }
   else if (_repository->isDefaultInstanceProvider())
   {
      CIMException cimException;

      STAT_PROVIDERSTART

      Array<CIMObjectPath> objectNames;

      _repository->read_lock();

      try
      {
         objectNames = _repository->associatorNames(
	    request->nameSpace,
	    request->objectName,
	    request->assocClass,
	    request->resultClass,
	    request->role,
	    request->resultRole);
      }
      catch(CIMException& exception)
      {
         cimException = exception;
      }
      catch(Exception& exception)
      {
         cimException =
            PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
      }
      catch(...)
      {
         cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
      }

      _repository->read_unlock();

      STAT_PROVIDEREND

      CIMAssociatorNamesResponseMessage* response =
         new CIMAssociatorNamesResponseMessage(
	    request->messageId,
	    cimException,
	    request->queueIds.copyAndPop(),
	    objectNames);

      STAT_COPYDISPATCHER_REP

      _enqueueResponse(request, response);
   }
   else // No provider is registered and the repository isn't the default
   {
      CIMAssociatorNamesResponseMessage* response =
         new CIMAssociatorNamesResponseMessage(
            request->messageId,
            PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, String::EMPTY),
            request->queueIds.copyAndPop(),
            Array<CIMObjectPath>());

      STAT_COPYDISPATCHER

      _enqueueResponse(request, response);
   }

   PEG_METHOD_EXIT();
}

void CIMOperationRequestDispatcher::handleReferencesRequest(
   CIMReferencesRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDispatcher::handleReferencesRequest");

   if (!_enableAssociationTraversal)
   {
       CIMException cimException =
           PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, "References");
       Array<CIMObject> cimObjects;

       CIMReferencesResponseMessage* response =
           new CIMReferencesResponseMessage(
               request->messageId,
	       cimException,
	       request->queueIds.copyAndPop(),
	       cimObjects);

       STAT_COPYDISPATCHER

       _enqueueResponse(request, response);

       PEG_METHOD_EXIT();
       return;
   }

   String className = request->objectName.getClassName();
   CIMResponseMessage * response;

   // check the class name for an "external provider"
   Array<String> providerNames = _lookupAssociationProvider(request->nameSpace, className);

   if(providerNames.size() != 0)
   {
      CIMReferencesRequestMessage* requestCopy =
          new CIMReferencesRequestMessage(*request);

      _forwardRequestToService(
          PEGASUS_QUEUENAME_PROVIDERMANAGER_CPP, requestCopy, response);

      PEG_METHOD_EXIT();
      return;
   }
   else if (_repository->isDefaultInstanceProvider())
   {
      CIMException cimException;

      STAT_PROVIDERSTART

      Array<CIMObject> cimObjects;

      _repository->read_lock();

      try
      {
         cimObjects = _repository->references(
	    request->nameSpace,
	    request->objectName,
	    request->resultClass,
	    request->role,
	    request->includeQualifiers,
	    request->includeClassOrigin,
	    request->propertyList.getPropertyNameArray());
      }
      catch(CIMException& exception)
      {
         cimException = exception;
      }
      catch(Exception& exception)
      {
         cimException =
            PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
      }
      catch(...)
      {
         cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
      }

      _repository->read_unlock();

      STAT_PROVIDEREND

      CIMReferencesResponseMessage* response =
         new CIMReferencesResponseMessage(
	    request->messageId,
	    cimException,
	    request->queueIds.copyAndPop(),
	    cimObjects);

      STAT_COPYDISPATCHER_REP

      _enqueueResponse(request, response);
   }
   else // No provider is registered and the repository isn't the default
   {
      CIMReferencesResponseMessage* response =
         new CIMReferencesResponseMessage(
            request->messageId,
            PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, String::EMPTY),
            request->queueIds.copyAndPop(),
            Array<CIMObject>());

      STAT_COPYDISPATCHER

      _enqueueResponse(request, response);
   }

   PEG_METHOD_EXIT();
}

void CIMOperationRequestDispatcher::handleReferenceNamesRequest(
   CIMReferenceNamesRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDispatcher::handleReferenceNamesRequest");

   if (!_enableAssociationTraversal)
   {
       CIMException cimException =
           PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, "ReferenceNames");
       Array<CIMObjectPath> cimObjects;

       CIMReferenceNamesResponseMessage* response =
           new CIMReferenceNamesResponseMessage(
               request->messageId,
	       cimException,
	       request->queueIds.copyAndPop(),
	       cimObjects);

       STAT_COPYDISPATCHER

       _enqueueResponse(request, response);

       PEG_METHOD_EXIT();
       return;
   }

   String className = request->objectName.getClassName();
   CIMResponseMessage * response;

   // check the class name for an "external provider"
   Array<String> providerNames = _lookupAssociationProvider(request->nameSpace, className);

   if(providerNames.size() != 0)
   {
      CIMReferenceNamesRequestMessage* requestCopy =
          new CIMReferenceNamesRequestMessage(*request);

      _forwardRequestToService(
          PEGASUS_QUEUENAME_PROVIDERMANAGER_CPP, requestCopy, response);

      PEG_METHOD_EXIT();
      return;
   }
   else if (_repository->isDefaultInstanceProvider())
   {
      CIMException cimException;

      STAT_PROVIDERSTART

      Array<CIMObjectPath> objectNames;

      _repository->read_lock();

      try
      {
         objectNames = _repository->referenceNames(
	    request->nameSpace,
	    request->objectName,
	    request->resultClass,
	    request->role);
      }
      catch(CIMException& exception)
      {
         cimException = exception;
      }
      catch(Exception& exception)
      {
         cimException =
            PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
      }
      catch(...)
      {
         cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
      }

      _repository->read_unlock();

      STAT_PROVIDEREND

      CIMReferenceNamesResponseMessage* response =
         new CIMReferenceNamesResponseMessage(
	    request->messageId,
	    cimException,
	    request->queueIds.copyAndPop(),
	    objectNames);

      STAT_COPYDISPATCHER_REP

      _enqueueResponse(request, response);
   }
   else // No provider is registered and the repository isn't the default
   {
      CIMReferenceNamesResponseMessage* response =
         new CIMReferenceNamesResponseMessage(
            request->messageId,
            PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, String::EMPTY),
            request->queueIds.copyAndPop(),
            Array<CIMObjectPath>());

      STAT_COPYDISPATCHER

      _enqueueResponse(request, response);
   }
   PEG_METHOD_EXIT();
}

void CIMOperationRequestDispatcher::handleGetPropertyRequest(
   CIMGetPropertyRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDispatcher::handleGetPropertyRequest");

   String className = request->instanceName.getClassName();
   CIMResponseMessage * response;

   // check the class name for an "external provider"
   String providerName = _lookupInstanceProvider(request->nameSpace, className);

   if(providerName.size() != 0)
   {
      CIMGetPropertyRequestMessage* requestCopy =
          new CIMGetPropertyRequestMessage(*request);

      _forwardRequestToService(
          PEGASUS_QUEUENAME_PROVIDERMANAGER_CPP, requestCopy, response);

      PEG_METHOD_EXIT();
      return;
   }
   else if (_repository->isDefaultInstanceProvider())
   {
      CIMException cimException;

      STAT_PROVIDERSTART

      CIMValue value;

      _repository->read_lock();

      try
      {
         value = _repository->getProperty(
	    request->nameSpace,
	    request->instanceName,
	    request->propertyName);
      }
      catch(CIMException& exception)
      {
         cimException = exception;
      }
      catch(Exception& exception)
      {
         cimException =
            PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
      }
      catch(...)
      {
         cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
      }

      _repository->read_unlock();

      STAT_PROVIDEREND

      CIMGetPropertyResponseMessage* response =
         new CIMGetPropertyResponseMessage(
	    request->messageId,
	    cimException,
	    request->queueIds.copyAndPop(),
	    value);

      STAT_COPYDISPATCHER_REP

      _enqueueResponse(request, response);
   }
   else // No provider is registered and the repository isn't the default
   {
      CIMGetPropertyResponseMessage* response =
         new CIMGetPropertyResponseMessage(
            request->messageId,
            PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, String::EMPTY),
            request->queueIds.copyAndPop(),
            CIMValue());

      STAT_COPYDISPATCHER

      _enqueueResponse(request, response);
   }
   PEG_METHOD_EXIT();
}

void CIMOperationRequestDispatcher::handleSetPropertyRequest(
   CIMSetPropertyRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDispatcher::handleSetPropertyRequest");

   {
      CIMException cimException;
      try
      {
         _fixSetPropertyValueType(request);
      }
      catch (CIMException& exception)
      {
         cimException = exception;
      }
      catch(Exception& exception)
      {
         cimException =
            PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
      }
      catch(...)
      {
         cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
      }

      if (cimException.getCode() != CIM_ERR_SUCCESS)
      {
         CIMSetPropertyResponseMessage* response =
            new CIMSetPropertyResponseMessage(
               request->messageId,
               cimException,
               request->queueIds.copyAndPop());

         STAT_COPYDISPATCHER

         _enqueueResponse(request, response);

         PEG_METHOD_EXIT();
         return;
      }
   }

   String className = request->instanceName.getClassName();
   CIMResponseMessage * response;

   // check the class name for an "external provider"
   String providerName = _lookupInstanceProvider(request->nameSpace, className);

   if(providerName.size() != 0)
   {
      CIMSetPropertyRequestMessage* requestCopy =
          new CIMSetPropertyRequestMessage(*request);

      _forwardRequestToService(
          PEGASUS_QUEUENAME_PROVIDERMANAGER_CPP, requestCopy, response);

      PEG_METHOD_EXIT();
      return;
   }
   else if (_repository->isDefaultInstanceProvider())
   {
      CIMException cimException;

      STAT_PROVIDERSTART

      _repository->write_lock();

      try
      {
         _repository->setProperty(
	    request->nameSpace,
	    request->instanceName,
	    request->propertyName,
	    request->newValue);
      }
      catch(CIMException& exception)
      {
         cimException = exception;
      }
      catch(Exception& exception)
      {
         cimException =
            PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
      }
      catch(...)
      {
         cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
      }

      _repository->write_unlock();

      STAT_PROVIDEREND

      CIMSetPropertyResponseMessage* response =
         new CIMSetPropertyResponseMessage(
	    request->messageId,
	    cimException,
	    request->queueIds.copyAndPop());

      STAT_COPYDISPATCHER_REP

      _enqueueResponse(request, response);
   }
   else // No provider is registered and the repository isn't the default
   {
      CIMSetPropertyResponseMessage* response =
         new CIMSetPropertyResponseMessage(
            request->messageId,
            PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, String::EMPTY),
            request->queueIds.copyAndPop());

      STAT_COPYDISPATCHER

      _enqueueResponse(request, response);
   }

   PEG_METHOD_EXIT();
}

void CIMOperationRequestDispatcher::handleGetQualifierRequest(
   CIMGetQualifierRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDispatcher::handleGetQualifierRequest");

   STAT_PROVIDERSTART

   CIMException cimException;
   CIMQualifierDecl cimQualifierDecl;

   _repository->read_lock();

   try
   {
      cimQualifierDecl = _repository->getQualifier(
	 request->nameSpace,
	 request->qualifierName);
   }
   catch(CIMException& exception)
   {
      cimException = exception;
   }
   catch(Exception& exception)
   {
      cimException =
         PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
   }
   catch(...)
   {
      cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
   }

   _repository->read_unlock();

   STAT_PROVIDEREND

   CIMGetQualifierResponseMessage* response =
      new CIMGetQualifierResponseMessage(
	 request->messageId,
	 cimException,
	 request->queueIds.copyAndPop(),
	 cimQualifierDecl);

   STAT_COPYDISPATCHER_REP

   _enqueueResponse(request, response);

   PEG_METHOD_EXIT();
}

void CIMOperationRequestDispatcher::handleSetQualifierRequest(
   CIMSetQualifierRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDispatcher::handleSetQualifierRequest");

   STAT_PROVIDERSTART

   CIMException cimException;

   _repository->write_lock();

   try
   {
      _repository->setQualifier(
	 request->nameSpace,
	 request->qualifierDeclaration);
   }
   catch(CIMException& exception)
   {
      cimException = exception;
   }
   catch(Exception& exception)
   {
      cimException =
         PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
   }
   catch(...)
   {
      cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
   }

   _repository->write_unlock();

   STAT_PROVIDEREND

   CIMSetQualifierResponseMessage* response =
      new CIMSetQualifierResponseMessage(
	 request->messageId,
	 cimException,
	 request->queueIds.copyAndPop());

   STAT_COPYDISPATCHER_REP

   _enqueueResponse(request, response);

   PEG_METHOD_EXIT();
}

void CIMOperationRequestDispatcher::handleDeleteQualifierRequest(
   CIMDeleteQualifierRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDispatcher::handleDeleteQualifierRequest");

   STAT_PROVIDERSTART

   CIMException cimException;

   _repository->write_lock();

   try
   {
      _repository->deleteQualifier(
	 request->nameSpace,
	 request->qualifierName);
   }

   catch(CIMException& exception)
   {
      cimException = exception;
   }
   catch(Exception& exception)
   {
      cimException =
         PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
   }
   catch(...)
   {
      cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
   }

   _repository->write_unlock();

   STAT_PROVIDEREND

   CIMDeleteQualifierResponseMessage* response =
      new CIMDeleteQualifierResponseMessage(
	 request->messageId,
	 cimException,
	 request->queueIds.copyAndPop());

   STAT_COPYDISPATCHER_REP

   _enqueueResponse(request, response);

   PEG_METHOD_EXIT();
}

void CIMOperationRequestDispatcher::handleEnumerateQualifiersRequest(
   CIMEnumerateQualifiersRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDispatcher::handleEnumerateQualifiersRequest");

   CIMException cimException;

   STAT_PROVIDERSTART

   Array<CIMQualifierDecl> qualifierDeclarations;

   _repository->read_lock();

   try
   {
      qualifierDeclarations = _repository->enumerateQualifiers(
	 request->nameSpace);
   }

   catch(CIMException& exception)
   {
      cimException = exception;
   }
   catch(Exception& exception)
   {
      cimException =
         PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
   }
   catch(...)
   {
      cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
   }

   _repository->read_unlock();

   STAT_PROVIDEREND

   CIMEnumerateQualifiersResponseMessage* response =
      new CIMEnumerateQualifiersResponseMessage(
	 request->messageId,
	 cimException,
	 request->queueIds.copyAndPop(),
	 qualifierDeclarations);

   _enqueueResponse(request, response);

   STAT_COPYDISPATCHER_REP

   PEG_METHOD_EXIT();
}

void CIMOperationRequestDispatcher::handleExecQueryRequest(
   CIMExecQueryRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDispatcher::handleExecQueryRequest");

   CIMException cimException =
       PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, "ExecQuery");
   Array<CIMObject> cimObjects;

   CIMExecQueryResponseMessage* response =
      new CIMExecQueryResponseMessage(
	 request->messageId,
	 cimException,
	 request->queueIds.copyAndPop(),
	 cimObjects);

   STAT_COPYDISPATCHER

   _enqueueResponse(request, response);

   PEG_METHOD_EXIT();
}

void CIMOperationRequestDispatcher::handleInvokeMethodRequest(
   CIMInvokeMethodRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDispatcher::handleInvokeMethodRequest");

   CIMResponseMessage * response;

   {
      CIMException cimException;
      try
      {
         _fixInvokeMethodParameterTypes(request);
      }
      catch (CIMException& exception)
      {
         cimException = exception;
      }
      catch(Exception& exception)
      {
         cimException =
            PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
      }
      catch(...)
      {
         cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
      }

      if (cimException.getCode() != CIM_ERR_SUCCESS)
      {
         response =
            new CIMInvokeMethodResponseMessage(
               request->messageId,
               cimException,
               request->queueIds.copyAndPop(),
               CIMValue(),
               Array<CIMParamValue>(),
               request->methodName);

         STAT_COPYDISPATCHER

         _enqueueResponse(request, response);

         PEG_METHOD_EXIT();
         return;
      }
   }


   String className = request->instanceName.getClassName();

   CIMException checkClassException;
   _checkExistenceOfClass(request->nameSpace, className, checkClassException);
   if (checkClassException.getCode() != CIM_ERR_SUCCESS)
   {
      CIMInvokeMethodResponseMessage* response =
         new CIMInvokeMethodResponseMessage(
            request->messageId,
            checkClassException,
            request->queueIds.copyAndPop(),
            CIMValue(),
            Array<CIMParamValue>(),
            request->methodName);

      _enqueueResponse(request, response);
      PEG_METHOD_EXIT();
      return;
   }

   String serviceName = String::EMPTY;
   String controlProviderName = String::EMPTY;

   // Check for class provided by an internal provider
   if (_lookupInternalProvider(request->nameSpace, className, serviceName,
           controlProviderName))
   {
      CIMInvokeMethodRequestMessage* requestCopy =
         new CIMInvokeMethodRequestMessage(*request);

      _forwardRequest(className, serviceName, controlProviderName,
          requestCopy);

      PEG_METHOD_EXIT();
      return;
   }

   // check the class name for an "external provider"
   String providerName = _lookupMethodProvider(request->nameSpace,
			 className, request->methodName);

   if(providerName.size() != 0)
   {
      CIMInvokeMethodRequestMessage* requestCopy =
          new CIMInvokeMethodRequestMessage(*request);

      _forwardRequestToService(
          PEGASUS_QUEUENAME_PROVIDERMANAGER_CPP, requestCopy, response);

      PEG_METHOD_EXIT();
      return;
   }

   CIMException cimException =
       PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "Provider not available");
   CIMValue retValue(1);
   Array<CIMParamValue> outParameters;

   response =
      new CIMInvokeMethodResponseMessage(
	 request->messageId,
	 cimException,
	 request->queueIds.copyAndPop(),
	 retValue,
	 outParameters,
	 request->methodName);

   STAT_COPYDISPATCHER

   _enqueueResponse(request, response);

   PEG_METHOD_EXIT();
}

/**
   Convert the specified CIMValue to the specified type, and return it in
   a new CIMValue.
*/
CIMValue CIMOperationRequestDispatcher::_convertValueType(
   const CIMValue& value,
   CIMType type)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDispatcher::_convertValueType");

   CIMValue newValue;

   if (value.isArray())
   {
      Array<String> stringArray;
      Array<char*> charPtrArray;
      Array<const char*> constCharPtrArray;

      //
      // Convert the value to Array<const char*> to send to conversion method
      //
      // ATTN-RK-P3-20020221: Deal with TypeMismatchException
      // (Shouldn't really ever get that exception)
      value.get(stringArray);

      for (Uint32 k=0; k<stringArray.size(); k++)
      {
	 // Need to build an Array<const char*> to send to the conversion
	 // routine, but also need to keep track of them pointers as char*
	 // because Windows won't let me delete a const char*.
	 char* charPtr = stringArray[k].allocateCString();
	 charPtrArray.append(charPtr);
	 constCharPtrArray.append(charPtr);
      }

      //
      // Convert the value to the specified type
      //
      try
      {
	 newValue = XmlReader::stringArrayToValue(0, constCharPtrArray, type);
      }
      catch (XmlSemanticError e)
      {
         PEG_METHOD_EXIT();
	 throw PEGASUS_CIM_EXCEPTION(
	    CIM_ERR_INVALID_PARAMETER,
	    String("Malformed ") + cimTypeToString (type) + " value");
      }

      for (Uint32 k=0; k<charPtrArray.size(); k++)
      {
	 delete charPtrArray[k];
      }
   }
   else
   {
      String stringValue;

      // ATTN-RK-P3-20020221: Deal with TypeMismatchException
      // (Shouldn't really ever get that exception)
      value.get(stringValue);

      try
      {
	 newValue = XmlReader::stringToValue(0, _CString(stringValue), type);
      }
      catch (XmlSemanticError e)
      {
         PEG_METHOD_EXIT();
	 throw PEGASUS_CIM_EXCEPTION(
	    CIM_ERR_INVALID_PARAMETER,
	    String("Malformed ") + cimTypeToString (type) + " value");
      }
   }

   PEG_METHOD_EXIT();
   return newValue;
}

/**
   Find the CIMParamValues in the InvokeMethod request whose types were
   not specified in the XML encoding, and convert them to the types
   specified in the method schema.
*/
void CIMOperationRequestDispatcher::_fixInvokeMethodParameterTypes(
   CIMInvokeMethodRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDispatcher::_fixInvokeMethodParameterTypes");

   Boolean gotMethodDefinition = false;
   CIMMethod method;

    //
    // Cycle through the input parameters, converting the untyped ones.
    //
    Array<CIMParamValue> inParameters = request->inParameters;
    Uint32 numInParamValues = inParameters.size();
    for (Uint32 i=0; i<numInParamValues; i++)
    {
        if (!inParameters[i].isTyped())
        {
            //
            // Retrieve the method definition, if we haven't already done so
            // (only look up the method if we have an untyped parameter value)
            //
            if (!gotMethodDefinition)
            {
                //
                // Get the class definition for this method
                //
                CIMClass cimClass;
                _repository->read_lock();
                try
                {
                    cimClass = _repository->getClass(
                        request->nameSpace,
                        request->instanceName.getClassName(),
                        false, //localOnly,
                        false, //includeQualifiers,
                        false, //includeClassOrigin,
                        CIMPropertyList());
                }
                catch (CIMException& exception)
                {
                    _repository->read_unlock();
                    PEG_METHOD_EXIT();
                    throw exception;
                }
                catch (Exception& exception)
                {
                    _repository->read_unlock();
                    PEG_METHOD_EXIT();
                    throw exception;
                }
                catch (...)
                {
                    _repository->read_unlock();
                    PEG_METHOD_EXIT();
                    throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
                }
                _repository->read_unlock();

                //
                // Get the method definition from the class
                //
                Uint32 methodPos = cimClass.findMethod(request->methodName);
                if (methodPos == PEG_NOT_FOUND)
                {
                    throw PEGASUS_CIM_EXCEPTION(CIM_ERR_METHOD_NOT_FOUND,
                                                String::EMPTY);
                }
                method = cimClass.getMethod(methodPos);

                gotMethodDefinition = true;
            }

            //
            // Find the parameter definition for this input parameter
            //
            String paramName = inParameters[i].getParameterName();
            Uint32 numParams = method.getParameterCount();
            for (Uint32 j=0; j<numParams; j++)
            {
                CIMParameter param = method.getParameter(j);
                if (paramName == param.getName())
                {
                    //
                    // Retype the input parameter value according to the
                    // type defined in the class/method schema
                    //
                    CIMType paramType = param.getType();
                    CIMValue newValue;

                    if (inParameters[i].getValue().isNull())
                    {
                        newValue.setNullValue(param.getType(), param.isArray());
                    }
                    else if (inParameters[i].getValue().isArray() !=
                                 param.isArray())
                    {
                        // ATTN-RK-P1-20020222: Who catches this?  They aren't.
                        PEG_METHOD_EXIT();
                        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_TYPE_MISMATCH,
                                                    String::EMPTY);
                    }
                    else
                    {
                        newValue = _convertValueType(inParameters[i].getValue(),
                            paramType);
                    }

                    inParameters[i].setValue(newValue);
                    inParameters[i].setIsTyped(true);
                    break;
                }
            }
        }
    }

    PEG_METHOD_EXIT();
}

/**
   Convert the CIMValue given in a SetProperty request to the correct
   type according to the schema, because it is not possible to specify
   the property type in the XML encoding.
*/
void CIMOperationRequestDispatcher::_fixSetPropertyValueType(
   CIMSetPropertyRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDispatcher::_fixSetPropertyValueType");

   CIMValue inValue = request->newValue;

   //
   // Only do the conversion if the type is not already set
   //
   if ((inValue.getType() != CIMTYPE_STRING))
   {
      PEG_METHOD_EXIT();
      return;
   }

   //
   // Get the class definition for this property
   //
   CIMClass cimClass;
   _repository->read_lock();
   try
   {
      cimClass = _repository->getClass(
         request->nameSpace,
         request->instanceName.getClassName(),
         false, //localOnly,
         false, //includeQualifiers,
         false, //includeClassOrigin,
         CIMPropertyList());
   }
   catch (CIMException& exception)
   {
      _repository->read_unlock();
      PEG_METHOD_EXIT();
      throw exception;
   }
   catch (Exception& exception)
   {
      _repository->read_unlock();
      PEG_METHOD_EXIT();
      throw exception;
   }
   catch (...)
   {
      _repository->read_unlock();
      PEG_METHOD_EXIT();
      throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
   }
   _repository->read_unlock();

   //
   // Get the property definition from the class
   //
   Uint32 propertyPos = cimClass.findProperty(request->propertyName);
   if (propertyPos == PEG_NOT_FOUND)
   {
      PEG_METHOD_EXIT();
      throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NO_SUCH_PROPERTY, String::EMPTY);
   }
   CIMProperty property = cimClass.getProperty(propertyPos);

   //
   // Retype the input property value according to the
   // type defined in the schema
   //
   CIMValue newValue;

   if (inValue.isNull())
   {
      newValue.setNullValue(property.getType(), property.isArray());
   }
   else if (inValue.isArray() != property.isArray())
   {
      // ATTN-RK-P1-20020222: Who catches this?  They aren't.
      PEG_METHOD_EXIT();
      throw PEGASUS_CIM_EXCEPTION(CIM_ERR_TYPE_MISMATCH, String::EMPTY);
   }
   else
   {
      newValue = _convertValueType(inValue, property.getType());
   }

   //
   // Put the retyped value back into the message
   //
   request->newValue = newValue;

   PEG_METHOD_EXIT();
}

void CIMOperationRequestDispatcher::_checkExistenceOfClass(
   const String& nameSpace,
   const String& className,
   CIMException& cimException)
{
   if (String::equalNoCase(className,"__Namespace"))
   {
      return;
   }

   CIMClass cimClass;

   _repository->read_lock();

   try
   {
      cimClass = _repository->getClass(
         nameSpace,
         className,
         true,
         false,
         false,
         CIMPropertyList());
   }
   catch(CIMException& exception)
   {
      cimException = exception;
   }
   catch(Exception& exception)
   {
      cimException =
         PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
   }
   catch(...)
   {
      cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
   }

   _repository->read_unlock();
}

PEGASUS_NAMESPACE_END
