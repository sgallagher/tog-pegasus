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

#define CDEBUG(X) PEGASUS_STD(cout) << "CIMOpReqDsptchr " << X << PEGASUS_STD(endl)
//#define CDEBUG(X)
//#define DEBUG(X) Logger::put (Logger::DEBUG_LOG, "CIMOpReqDsptchr", Logger::INFORMATION, "$0", X)

//#define LIMIT_ENUM_TO_ONE_LEVEL

// Variable to control whether we do search or simply single provider for reference and
// associatior lookups.
static bool singleProviderType = true;

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

   String maxEnumBreadthOption = configManager->getCurrentValue("maximumEnumerationBreadth");
   
  _maximumEnumerateBreadth = atol(maxEnumBreadthOption.getCString());

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
   const CIMNamespaceName& nameSpace,
   const CIMName& className,
   String& service,
   String& provider)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::_lookupInternalProvider");
    // Clear the strings since used as test later. Poor code but true now
    service =  String::EMPTY;
    provider = String::EMPTY;

    if (className.equal (PEGASUS_CLASSNAME_CONFIGSETTING) &&
        nameSpace.equal (PEGASUS_NAMESPACENAME_CONFIG))
    {
        service = PEGASUS_QUEUENAME_CONTROLSERVICE;
        provider = PEGASUS_MODULENAME_CONFIGPROVIDER;
	PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL4,
	    "Internal provider  Service = " + service + " provider " + provider + " found.");
        PEG_METHOD_EXIT();
        return true;
    }
    if ((className.equal (PEGASUS_CLASSNAME_AUTHORIZATION) &&
         nameSpace.equal (PEGASUS_NAMESPACENAME_AUTHORIZATION)) ||
        (className.equal (PEGASUS_CLASSNAME_USER) &&
         nameSpace.equal (PEGASUS_NAMESPACENAME_USER)))
    {
        service = PEGASUS_QUEUENAME_CONTROLSERVICE;
        provider = PEGASUS_MODULENAME_USERAUTHPROVIDER;
	PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL4,
	    "Internal provider  Service = " + service + " provider " + provider + " found.");
        PEG_METHOD_EXIT();
        return true;
    }
    if (className.equal (PEGASUS_CLASSNAME_SHUTDOWN) &&
        nameSpace.equal (PEGASUS_NAMESPACENAME_SHUTDOWN))
    {
        service = PEGASUS_QUEUENAME_CONTROLSERVICE;
        provider = PEGASUS_MODULENAME_SHUTDOWNPROVIDER;
	PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL4,
	    "Internal provider  Service = " + service + " provider " + provider + " found.");
        PEG_METHOD_EXIT();
        return true;
    }
    if (className.equal (PEGASUS_CLASSNAME___NAMESPACE) ||
	className.equal (PEGASUS_CLASSNAME_NAMESPACE))
    {
        service = PEGASUS_QUEUENAME_CONTROLSERVICE;
        provider = PEGASUS_MODULENAME_NAMESPACEPROVIDER;
	PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL4,
	    "Internal provider  Service = " + service + " provider " + provider + " found.");
        PEG_METHOD_EXIT();
        return true;
    }

    if ((className.equal (PEGASUS_CLASSNAME_PROVIDERMODULE) ||
         className.equal (PEGASUS_CLASSNAME_PROVIDER) ||
         className.equal (PEGASUS_CLASSNAME_PROVIDERCAPABILITIES)) &&
         nameSpace.equal (PEGASUS_NAMESPACENAME_PROVIDERREG))
    {
        service = PEGASUS_QUEUENAME_CONTROLSERVICE;
        provider = PEGASUS_MODULENAME_PROVREGPROVIDER;
	PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL4,
	    "Internal provider  Service = " + service + " provider " + provider + " found.");
        PEG_METHOD_EXIT();
        return true;
    }
    if (_enableIndicationService &&
        (className.equal (PEGASUS_CLASSNAME_INDSUBSCRIPTION) ||
         className.equal (PEGASUS_CLASSNAME_INDHANDLER) ||
         className.equal (PEGASUS_CLASSNAME_INDHANDLER_CIMXML) ||
         className.equal (PEGASUS_CLASSNAME_INDHANDLER_SNMP) ||
         className.equal (PEGASUS_CLASSNAME_INDFILTER)))
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
Array<CIMName> CIMOperationRequestDispatcher::_getSubClassNames(
		    CIMNamespaceName& nameSpace,
		    CIMName& className) throw(CIMException)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
       "CIMOperationRequestDispatcher::_getSubClassNames");

    Array<CIMName> subClassNames;
    //
    // Get names of descendent classes:
    //
    if(!className.equal (PEGASUS_CLASSNAME___NAMESPACE))
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
   const CIMNamespaceName& nameSpace,
   const CIMName& className)
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
    	Uint32 pos = pInstance.findProperty(CIMName ("Name"));
    
    	if ( pos != PEG_NOT_FOUND )
    	{
    	    pInstance.getProperty(pos).getValue().get(providerName);
    
            PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL4,
                             "providerName = " + providerName + " found.");
    	}
    	else
    	{
            PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL4,
                   "providerName = " + providerName + ". Provider name not found.");
            PEG_METHOD_EXIT();
       	    providerName = String::EMPTY;
    	}
        PEG_METHOD_EXIT();
        return (providerName);
    }
    else
    {
        PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL4,
                   "Provider for " + className.getString() + " not found.");
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
				 const CIMNamespaceName& nameSpace,
                                 const CIMName& className,
				 String& serviceName,
				 String& controlProviderName)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
                     "CIMOperationRequestDispatcher::_lookupNewInstanceProvider");
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
   const CIMNamespaceName& nameSpace,
   const CIMName& className,
   const CIMName& methodName)
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
	Uint32 pos = pInstance.findProperty(CIMName ("Name"));

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


/* _lookupAllAssociation Providers for Class
    Returns all of the association providers that exist
    for the defined class name.  Used the referencenames
    function to get list of classes for which providers
    required and then looks up the providers for each
    class
    @param nameSpace
    @param className
    @returns List of ProviderInfo
    @exception - TBD
    */ 
 Array<ProviderInfo> CIMOperationRequestDispatcher::_lookupAllAssociationProviders(
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& objectName)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
           "CIMOperationRequestDispatcher::_lookupAllAssociationProviders");
    //Array<String> classNames;
    //CDEBUG("Lookup Assoc providers for class = " << objectName.getClassName());
    Array<ProviderInfo> providerInfoList;

    Array<CIMName> classNames;
    CIMName className = objectName.getClassName();
    CIMException cimException;
    try
    {
    _repository->getSuperClassNames(nameSpace,className, classNames);
    }
    catch(CIMException& e)
    {
        // Gets exception back from the getsuperClasses if class does not exist
        PEG_METHOD_EXIT();
        throw e;
    }
    //CDEBUG("__Lookupallass rtned superclasses = " << classNames.size());
    Array<CIMName> subClassNames;
    //CDEBUG("now try to get subclassnames");
    classNames.append(className);
    try
    {
        // Get the complete list of subclass names
        _repository->getSubClassNames(nameSpace,className, true, classNames);
    }
    catch(CIMException& e)
    {
        // Gets exception back from the getSubClasses if class does not exist
        PEG_METHOD_EXIT();
        throw e;
    }
    //CDEBUG("Now have class list of size " << classNames.size());
    //for (Uint32 i = 0; i < classNames.size(); i++)
    //{
    //    CDEBUG(" i " << i << "_LookupAllAssocProv classes rtned " << classNames[i]);
    //}
    // Need to do simething with these exceptions if they occur.
    // Isolate the classname.
    // ATTN: What should we do with remote associations if there are any
    for (Uint32 i = 0; i < classNames.size(); i++)
    {
        ProviderInfo providerI;
        String serviceName = String::EMPTY;
        String controlProviderName = String::EMPTY;
        ProviderInfo pi(classNames[i]);

        if(_lookupNewAssociationProvider(nameSpace, classNames[i], serviceName, controlProviderName))
        {
            //CDEBUG("LookupNew return. Class = " <<   classNames[i]);
            pi._serviceName = serviceName;
            pi._controlProviderName = controlProviderName;
            pi._hasProvider = true;
        }
        else
        {
            pi._serviceName = String::EMPTY;
            pi._controlProviderName = String::EMPTY;
            pi._hasProvider = false;
        }
        providerInfoList.append(pi);
    }
    PEG_METHOD_EXIT();
    //CDEBUG("_lookupAllAQssocProvider rtns count = " << providerInfoList.size()); 
    return (providerInfoList);
}

/* _lookupNewAssociationProvider - Looks up the internal and/or instance provider
    for the defined namespace and class and returns the serviceName and
    control provider name if a provider is found
    @return true if an service, control provider, or instance provider is found
    for the defined class and namespace.
    This should be combined with the lookupInstanceProvider code eventually but
    the goal now was to simplify the handlers.
    ATTN: KS P3 20 May 2002 Merge this with lookupAssociationprovider
*/
Boolean CIMOperationRequestDispatcher::_lookupNewAssociationProvider(
				 const CIMNamespaceName& nameSpace,
                 const CIMName& className,
				 String& serviceName,
				 String& controlProviderName)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
                     "CIMOperationRequestDispatcher::_lookupNewAssociationProvider");
   Boolean hasProvider = false;
   String providerName = String::EMPTY;

   // Check for class provided by an internal provider
   if (_lookupInternalProvider(nameSpace, className, serviceName,
           controlProviderName))
       hasProvider = true;
   else
   {
       // get provider for class. Note that we reduce it from
       // Multiple possible class names to a single one here.
       // This is a hack.  Clean up later
       // ATTN: KS - Clean up this multiple to single provider stuff.
       Array<String> tmp;
       //CDEBUG( "LookupnewAssoc No Internal Provider class = " << className);
       CIMException cimException;
       try
       {
           tmp = _lookupAssociationProvider(nameSpace, className);
       }
       catch(CIMException& exception)
       {
          PEG_METHOD_EXIT();
          throw exception;
          //CDEBUG("New exception 1 " << exception.getMessage());
       }
       catch(Exception& exception)
       {
          //CDEBUG("new exception 2 " << exception.getMessage());
          cimException =
             PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
       }
       catch(...)
       {
          //CDEBUG("New exception 3 ");
          cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
       }
       //CDEBUG( "LookupnewAssoc after lookupasscal class = " << className << " tmp " << tmp.size());

       if (tmp.size() > 0)
       {
           providerName = tmp[0];
       }
       //CDEBUG("_LookupAllAssoc after lookupAssoc = Rtns " << tmp.size());
   }
   //CDEBUG("_LookupNewAssocProv hasProvider " << ((hasProvider)? "true" : "false") << " svcnme " << serviceName);
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
   //CDEBUG("Exit _lookupNewAssociation. SvcName = "<< serviceName << " hasProvider " << (hasProvider? "true" : "false"));
   return hasProvider;

}

// Lookup the Association provider(s) for this class and convert the
// result to an array of providernames.
// NOTE: The code allows for multiple providers but there is no reason
// to have this until we really support multiple providers per class.
// The multiple provider code was here because there was some code in to
// do the lookup in provider registration which was removed. Some day we
// will support multiple providers per class so it was logical to just leave
// the multiple provider code in place.
// NOTE: assocClass and resultClass are optional
//
Array<String> CIMOperationRequestDispatcher::_lookupAssociationProvider(
   const CIMNamespaceName& nameSpace,
   const CIMName& className,
   const CIMName& assocClass,
   const CIMName& resultClass)
{
    // instances of the provider class and provider module class for the response
    Array<CIMInstance> pInstances; // Providers
    Array<CIMInstance> pmInstances; // ProviderModules

    Array<String> providerNames;
    String providerName;

    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::_lookupAssociationProvider");
    //CDEBUG("_lookupAssociationProvider");
    // Isolate the provider names from the response and return
    // just a list of provider names.
    Boolean returnValue = false;
    CIMException cimException;
    //CDEBUG("Calling REG->LookupAssocProv with class = " << className);
    try
    {
        returnValue = _providerRegistrationManager->lookupAssociationProvider(
                nameSpace, className, assocClass, resultClass,
                    pInstances, pmInstances);
    }
    catch(CIMException& exception)
    {
       cimException = exception;
       //CDEBUG("exception 1 " << exception.getMessage());
    }
    catch(Exception& exception)
    {
       //DEBUG("exception 2 " << exception.getMessage());
       cimException =
          PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
    }
    catch(...)
    {
       //CDEBUG("exception 3 ");
       cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
    }
    //CDEBUG("Rtn from REG->LookupAssocProv with class = " << className);
    if (returnValue)
    {

        for(Uint32 i=0,n=pInstances.size(); i<n; i++)
        {
            // get the provider name
            Uint32 pos = pInstances[i].findProperty(CIMName ("Name"));

            if ( pos != PEG_NOT_FOUND )
            {
                pInstances[i].getProperty(pos).getValue().get(providerName);

                PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL4,
                             "Association providerName = " + providerName + " found."
                            + " for Class " + className.getString());
                providerNames.append(providerName);
            }
        }
    }
    //CDEBUG("_lookupAssociationProvider. Rtns from lookupAssoc..." << pInstances.size());

    if (providerNames.size() == 0)
    {
        PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL4,
            "Association Provider NOT found for Class " + className.getString()
             + " in nameSpace " + nameSpace.getString());
    }
    PEG_METHOD_EXIT();
    //CDEBUG("_lookupAssocProvider rtns for classname = " << className.getString());
    return providerNames;
}

/*  Common Dispatcher callback for response aggregation.
*/
void CIMOperationRequestDispatcher::_forwardForAggregationCallback(
    AsyncOpNode *op,
    MessageQueue *q,
    void *userParameter)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::_forwardForAggregationCallback");

    CIMOperationRequestDispatcher *service =
        static_cast<CIMOperationRequestDispatcher *>(q);

    AsyncRequest *asyncRequest = static_cast<AsyncRequest *>(op->get_request());
    AsyncReply *asyncReply = static_cast<AsyncReply *>(op->get_response());

    OperationAggregate* poA = reinterpret_cast<OperationAggregate*>(userParameter);
    PEGASUS_ASSERT(poA != 0);

    // Verify that the aggregator is valid.
    PEGASUS_ASSERT(poA->valid());
    //CDEBUG("_ForwardForAggregationCallback ");
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
    //CDEBUG("AggregationCallback 2");

    PEGASUS_ASSERT(response != 0);
    PEGASUS_ASSERT(response->messageId == poA->_messageId);


    delete asyncRequest;
    delete asyncReply;
    op->release();
    service->return_op(op);

    Boolean isDoneAggregation = poA->appendResponse(response);
    //CDEBUG("Callback counts responses = " << poA->numberResponses() << " Issued " << poA->totalIssued() );
    // If all responses received, call the postProcessor
    //CDEBUG("Aggreg Is response done? = " << ((isDoneAggregation)? "true" : "false"));
    if (isDoneAggregation)
    {
        //CDEBUG("AggregationCallback 8");
        
        service->handleOperationResponseAggregation(poA);
    }

    PEG_METHOD_EXIT();
}


/*  Common Dispatcher callback.
*/
void CIMOperationRequestDispatcher::_forwardRequestCallback(
    AsyncOpNode *op,
    MessageQueue *q,
    void *userParameter)
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
    response->dest = (Uint64)userParameter;
#elif PEGASUS_PLATFORM_AIX_RS_IBMCXX
    response->dest = (unsigned long)userParameter;   //Cast to size 32/64 bit safe
#else
    response->dest = (Uint32)userParameter;
#endif

    PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL3, 
        "Forwarding " + String(MessageTypeToString(response->getType())) + 
        " via Callback to " + 
        ((MessageQueue::lookup(response->dest)) ? 
         String( ((MessageQueue::lookup(response->dest))->getQueueName()) ) : 
         String("BAD queue name")));

    if(userParameter != 0 )
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
    //CDEBUG("ForwardRequestForAggregation");
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
    const CIMName& className,        // only for diagnostic
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
            " on class " + className.getString() + " to service " + serviceName +
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
            " on class " + className.getString() + " to service " + serviceName +
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
//   Return Aggregated responses back to the Correct Aggregator
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


// Aggregate the responses for reference names into a single response
//
void CIMOperationRequestDispatcher::handleAssociatorNamesResponseAggregation(
    OperationAggregate* poA)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "CIMOperationRequestDispatcher::handleReferenceNamesResponseAggregation");
    CIMAssociatorNamesResponseMessage * toResponse = 
	(CIMAssociatorNamesResponseMessage *) poA->getResponse(0);

    // Work backward and delete each response off the end of the array
    //CDEBUG("AssociatorNames aggregating responses. Count =  " << poA->numberResponses());
    for(Uint32 i = poA->numberResponses() - 1; i > 0; i--)
    {
    	CIMAssociatorNamesResponseMessage *fromResponse = 
    	    (CIMAssociatorNamesResponseMessage *)poA->getResponse(i);
        //CDEBUG("AssociatorNames aggregation from " << i << "contains " << fromResponse->objectNames.size());
    	for (Uint32 j = 0; j < fromResponse->objectNames.size(); j++)
    	{
    	    // Duplicate test goes here. Today we pass dupliats through
            toResponse->objectNames.append(fromResponse->objectNames[j]);
    	}
    	poA->deleteResponse(i);

    }
    //CDEBUG("AssociatorNames aggregation. Returns responce count " << toResponse->objectNames.size());
    PEG_METHOD_EXIT();
}

// Aggregate the responses for Associators into a single response

void CIMOperationRequestDispatcher::handleAssociatorsResponseAggregation(
    OperationAggregate* poA)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "CIMOperationRequestDispatcher::handleReferencesResponseAggregation");
    
    CIMAssociatorsResponseMessage * toResponse = 
	(CIMAssociatorsResponseMessage *) poA->getResponse(0);

    // Work backward and delete each response off the end of the array
    for(Uint32 i = poA->numberResponses() - 1; i > 0; i--)
    {
    	CIMAssociatorsResponseMessage *fromResponse = 
    	    (CIMAssociatorsResponseMessage *)poA->getResponse(i);
    
    	for (Uint32 j = 0; j < fromResponse->cimObjects.size(); j++)
    	{
    	    toResponse->cimObjects.append(fromResponse->cimObjects[j]);
    	}
    	poA->deleteResponse(i);
    }
    PEG_METHOD_EXIT();
}

// Aggregate the responses for References into a single response

void CIMOperationRequestDispatcher::handleReferencesResponseAggregation(
    OperationAggregate* poA)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "CIMOperationRequestDispatcher::handleReferencesResponseAggregation");
    
    CIMReferencesResponseMessage * toResponse = 
	(CIMReferencesResponseMessage *) poA->getResponse(0);

    // Work backward and delete each response off the end of the array
    for(Uint32 i = poA->numberResponses() - 1; i > 0; i--)
    {
    	CIMReferencesResponseMessage *fromResponse = 
    	    (CIMReferencesResponseMessage *)poA->getResponse(i);
    
    	for (Uint32 j = 0; j < fromResponse->cimObjects.size(); j++)
    	{
    	    toResponse->cimObjects.append(fromResponse->cimObjects[j]);
    	}
    	poA->deleteResponse(i);
    }
    PEG_METHOD_EXIT();
}

// Aggregate the responses for reference names into a single response
//
void CIMOperationRequestDispatcher::handleReferenceNamesResponseAggregation(
    OperationAggregate* poA)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "CIMOperationRequestDispatcher::handleReferenceNamesResponseAggregation");
    CIMReferenceNamesResponseMessage * toResponse = 
	(CIMReferenceNamesResponseMessage *) poA->getResponse(0);
    //CDEBUG("aggregating referenceName responses Number = " << poA->numberResponses());
    // Work backward and delete each response off the end of the array
    // adding it to the toResponse, which is really the first response.
    //CDEBUG("Aggregating to response with count = " << toResponse->objectNames.size());
    for(Uint32 i = poA->numberResponses() - 1; i > 0; i--)
    {
    	CIMReferenceNamesResponseMessage *fromResponse = 
    	    (CIMReferenceNamesResponseMessage *)poA->getResponse(i);
    
        // Move each object name
        //CDEBUG(" Aggregating responses i = " << i << " fromResponse has " << fromResponse->objectNames.size());
        for (Uint32 j = 0; j < fromResponse->objectNames.size(); j++)
    	{
            toResponse->objectNames.append(fromResponse->objectNames[j]);
            //CDEBUG("ReferenceNameAggregation Append response num = " << j << " " << fromResponse->objectNames[j].toString());
    	}
    	poA->deleteResponse(i);
    }
    //CDEBUG("Aggregated to response total count = " << toResponse->objectNames.size());

    PEG_METHOD_EXIT();
}

/* aggregate the responses for enumerateinstancenames into a single response
*/
void CIMOperationRequestDispatcher::handleEnumerateInstanceNamesResponseAggregation(
    OperationAggregate* poA)
{
    PEG_METHOD_ENTER(TRC_PROVIDERMANAGER,
        "CIMOperationRequestDispatcher::handleEnumerateInstanceNamesResponseAggregation");
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
void CIMOperationRequestDispatcher::handleEnumerateInstancesResponseAggregation(OperationAggregate* poA)
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
   //CDEBUG("Aggregation of responses");
   PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL4, 
            Formatter::format(" Response Aggregation  type $1 responses $0",
               totalResponses,
               String(MessageTypeToString(poA->getRequestType()))));
    
   //If there was only one response, simply forward it on.
   // Add the destination queue ID to the first response.

   CIMResponseMessage* response = poA->getResponse(0);

   response->dest = poA->_dest;

   if(totalResponses == 1)
   {
       SendForget(poA->getResponse(0));
       //CDEBUG("Aggregation, Only one response");
       delete poA;

       PEG_METHOD_EXIT();
       return;
   }

    /* Determine if there any "good" responses. If all responses are error
       we return CIMException.
    */
   //CDEBUG("Aggregation. Preparing for Errors test. Num responses = " << totalResponses);
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
    //CDEBUG("Aggregation error count = " << errorCount << " total Responses = " << totalResponses );
   
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
   //CDEBUG("ResponseAggregator num responses before error removal = " << poA->numberResponses());
   // ATTN: KS 20030306 We can be more efficient if we just build this into the following loop rather
   // than removing the responses from the list.
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
    //CDEBUG("ResponseAggregator total Responses " << totalResponses << " total Good Responses " << poA->numberResponses());
    // If more than one response, go to proper aggregation function
    if(poA->numberResponses() > 1)
    {
    // Multiple responses. Merge them by appending the response components
    // to the first entry
        CDEBUG("Aggregator. Calls appropriate aggration function");
        // Call the appropriate function for merging responses
        if (poA->getRequestType() == CIM_ENUMERATE_INSTANCE_NAMES_REQUEST_MESSAGE)
        {
            handleEnumerateInstanceNamesResponseAggregation(poA);
        }
        if (poA->getRequestType() == CIM_ENUMERATE_INSTANCES_REQUEST_MESSAGE)
        {
            handleEnumerateInstancesResponseAggregation(poA);
        }
        if (poA->getRequestType() == CIM_ASSOCIATORS_REQUEST_MESSAGE)
        {
            handleAssociatorsResponseAggregation(poA);
        }
        if (poA->getRequestType() == CIM_ASSOCIATOR_NAMES_REQUEST_MESSAGE)
        {
            handleAssociatorNamesResponseAggregation(poA);
        }
        if (poA->getRequestType() == CIM_REFERENCES_REQUEST_MESSAGE)
        {
            handleReferencesResponseAggregation(poA);
        }
        if (poA->getRequestType() == CIM_REFERENCE_NAMES_REQUEST_MESSAGE)
        {
            handleReferenceNamesResponseAggregation(poA);
        }
        /// ATTN: KS: need trap if not found.

    }

    // Send the remaining response and delete the aggregator.
    response = poA->getResponse(0);
    response->dest = poA->_dest;
    SendForget(response);
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

   //
   //  Set HTTP method in response from request
   //
   response->setHttpMethod (request->getHttpMethod ());

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
   CIMName className = request->instanceName.getClassName();

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
      STAT_COPYDISPATCHER_REP

      _enqueueResponse(request, response);
      PEG_METHOD_EXIT();
      return;
   }

   //CIMNamespaceName NameSpace = request->nameSpace;
   // ATTNKSDELETE CIMResponseMessage * response;
   String serviceName = String::EMPTY;
   String controlProviderName = String::EMPTY;
   String providerName = String::EMPTY;

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
   CIMName className = request->instanceName.getClassName();
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

      STAT_COPYDISPATCHER_REP
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
   CIMName className = request->newInstance.getClassName();
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

      STAT_COPYDISPATCHER_REP
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
   CIMName className = request->modifiedInstance.getClassName();
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

      STAT_COPYDISPATCHER_REP
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

   Array<CIMName> classNames;

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
/**$*******************************************************
    handleEnumerateInstancesRequest
**********************************************************/

void CIMOperationRequestDispatcher::handleEnumerateInstancesRequest(
   CIMEnumerateInstancesRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDispatcher::handleEnumerateInstancesRequest");
 #define NEW_ENUM_CODE
 #ifdef NEW_ENUM_CODE
   // get the class name
   CIMName className = request->className;

   CIMException checkClassException;
   _checkExistenceOfClass(request->nameSpace, className, checkClassException);
   if (checkClassException.getCode() != CIM_ERR_SUCCESS)
   {
      CIMEnumerateInstancesResponseMessage* response =
         new CIMEnumerateInstancesResponseMessage(request->messageId,
            checkClassException,
            request->queueIds.copyAndPop(),
            Array<CIMInstance>());

      STAT_COPYDISPATCHER_REP
      _enqueueResponse(request, response);
      PEG_METHOD_EXIT();
      return;
   }

   //
   // Get names of descendent classes:
   //
   Array<CIMName> classNames;
   CIMException cimException;
   try
   {
       classNames = _getSubClassNames(request->nameSpace,className);
   }
   catch(CIMException& exception)
   {
       // Return exception response if exception from getSubClasses
       cimException = exception;
       CIMEnumerateInstancesResponseMessage* response =
       new CIMEnumerateInstancesResponseMessage(request->messageId,
         cimException,
         request->queueIds.copyAndPop(),
         Array<CIMInstance>());

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
   Array<String> serviceNames;
   Array<String> controlProviderNames;
   Array<Boolean> hasProvider;
   Uint32 providersFound = 0;
   for(Uint32 i = 0; i < classNames.size(); i++)
   {
       String serviceName = String::EMPTY;
       String controlProviderName = String::EMPTY;
    
       // Lookup any instance providers and add to send list
       if(_lookupNewInstanceProvider(request->nameSpace, classNames[i],
           serviceName, controlProviderName))
		{
		   providersFound++;
           // Append the returned values to the list to send.
           hasProvider.append(true);
		   serviceNames.append(serviceName);
		   controlProviderNames.append(controlProviderName);
	       CDEBUG("FoundProvider for class = " << classNames[i].getString());
            PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL4,
               "Provider found for Class = " + classNames[i].getString()
			   + " servicename = " + serviceName
			   + " controlProviderName = " 
               + ((controlProviderName != String::EMPTY)  ? controlProviderName : "None"));
		}
       else
       {
           hasProvider.append(false);
           serviceNames.append(String::EMPTY);
           controlProviderNames.append(String::EMPTY);
           CDEBUG("No provider for class = " << classNames[i].getString());
       }
   }

   Uint32 ps = serviceNames.size();

   // Test for "enumerate to Broad" and if so, execute exception.
   // Simply limits the subclass depth to some max, not number
   // of instances returned.
   // ATTN: add size and limit to the message.
   if(ps > _maximumEnumerateBreadth)
   {
       CIMEnumerateInstancesResponseMessage* response =
         new CIMEnumerateInstancesResponseMessage( request->messageId,
            PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, "Enumerate request too Broad"),
            request->queueIds.copyAndPop(),
            Array<CIMInstance>());

      STAT_COPYDISPATCHER

      _enqueueResponse(request, response);
      PEG_METHOD_EXIT();
      return;
   }
    CDEBUG("Providers Found = " << providersFound );
    if ((providersFound == 0) && (_repository->isDefaultInstanceProvider()))
    {
        CDEBUG( "Going to repository for everything");  
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
    
          _enqueueResponse(request, response);
          PEG_METHOD_EXIT();
          return;
   }
   // No provider is registered and the repository isn't the default
   if ((providersFound == 0) && !(_repository->isDefaultInstanceProvider()))
   {         
       CDEBUG("No providers, no repository");
       PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL4, 
           "No support for " + className.getString() + " and subclasses."); 

       CIMEnumerateInstancesResponseMessage* response =
         new CIMEnumerateInstancesResponseMessage(
            request->messageId,
            PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, String::EMPTY),
            request->queueIds.copyAndPop(),
            Array<CIMInstance>());

      STAT_COPYDISPATCHER

      _enqueueResponse(request, response);
      PEG_METHOD_EXIT();
      return;

   }
   // We have instances for Providers.
   //Set up an aggregate object for the information and save request in it.
   // With the original request message.

   OperationAggregate *poA= new OperationAggregate(
       new CIMEnumerateInstancesRequestMessage(*request),
       request->getType(),
       request->messageId);
   poA->_dest = request->queueIds.top();

   //************* Limit to one response if we have problems with more.
//#ifdef LIMIT_ENUM_TO_ONE_LEVEL
//   ps = 1;
//#endif
   // Simply takes the top level request and ignores the others.
   //*******************/
   poA->setTotalIssued(ps);

   for(Uint32 i = 0; i < ps; i++ )
   {
       poA->classes.append(classNames[i]);
       poA->serviceNames.append(serviceNames[i]);
       poA->controlProviderNames.append(controlProviderNames[i]);
       
       // This should be used for displays, etc for the moment.
       Uint32 current =  poA->classes.size() - 1;
       //request->print(cout, true);


       PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL4, 
           Formatter::format(
           "EnumerateInstances Req. class $0 to svc $1 for control provider $2 No $3 of $4",
           poA->classes[current].getString(), poA->serviceNames[current],
           ((poA->controlProviderNames[current] != String::EMPTY)  ?
                poA->controlProviderNames[current] : "None"), i, ps));
        
       /*cout << "KSTEST send to provider, Class =  " << classNames[current]
           << " servicename = " << poA->serviceNames[current]
           << " control provider name = " << ((poA->controlProviderNames[current] != String::EMPTY)  ?
                poA->controlProviderNames[current] : "None") << " count " << current << endl;

       for(Uint32 j = 0; j <= i; j++ )
       {
           cout << "KSTEST View Class Arrays " << " Class = " <<   poA->classes[j]
                << " Service Name = " << poA->serviceNames[j]
                << " Control Provider = " << ((poA->controlProviderNames[j]
                     != String::EMPTY)  ? poA->controlProviderNames[j] : "None") 
                << " count " << j << endl;       }
        */
       // If this class has a provider
       if (!hasProvider[i])
       {
           if (_repository->isDefaultInstanceProvider())
           {
               Tracer::trace(TRC_DISPATCHER, Tracer::LEVEL4,
              "Repository being used as THE default instance provider");
               CDEBUG("Class for repository = " << classNames[i]);

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
                      false,
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
               Boolean isDoneAggregation;
               isDoneAggregation =  poA->appendResponse(response);
               CDEBUG("ReferenceName Send Is response done? = " << ((isDoneAggregation)? "true" : "false"));
               if (isDoneAggregation)
                   handleOperationResponseAggregation(poA);
           }
       }
       else
       {
           CIMEnumerateInstancesRequestMessage* requestCopy =
             new CIMEnumerateInstancesRequestMessage(*request);

           requestCopy->className = poA->classes[current];

           //request->print(cout, true);
           //requestCopy->print(cout, true);
           CDEBUG("Class to Provider = " << poA->classes[current]);
          _forwardRequestForAggregation(poA->serviceNames[current],
               poA->controlProviderNames[current], requestCopy, poA);
       }
   }
   PEG_METHOD_EXIT();
   return;
 /***************************The following is the old code that we will delete   */
 #else
   // get the class name
   CIMName className = request->className;

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

      STAT_COPYDISPATCHER_REP
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
   Array<CIMName> classProperties;
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
   Array<CIMName> subClassNames;
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
   Array<CIMName> subClassNameList;
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
               "Provider found for Class = " + subClassNames[i].getString() 
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
   if(ps > _maximumEnumerateBreadth)
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
   poA->_dest = request->queueIds.top();

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

/*//#if 0
//*     // if there will be other responses, put this on the response list.
//      // else, simply issue it.
//      if (ps > 0)
//      {
//          poA->appendResponse(response);
          staticInstancesExist = true;
      }
      else
      {
          delete poA;
          _enqueueResponse(request, response);
          PEG_METHOD_EXIT();
          return;
      }\
/* removed this since is is in error and will be replaced when we delete all this code*/
//#else*/
      delete poA;
      _enqueueResponse(request, response);
      PEG_METHOD_EXIT();
      return;
/*#endif*/
   }

   if(ps > 0)
   {
       //************* Limit to one response if we have problems with more.
//#ifdef LIMIT_ENUM_TO_ONE_LEVEL
//       ps = 1;
//#endif
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
               poA->classes[current].getString(), poA->serviceNames[current],
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
           "No provider for " + className.getString() + " and subclasses."); 

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
#endif
/* ***************END OF OLD CODE THAT WILL BE DELTED **************/
}

/**$*******************************************************
    handleEnumerateInstanceNamesRequest
**********************************************************/

void CIMOperationRequestDispatcher::handleEnumerateInstanceNamesRequest(
   CIMEnumerateInstanceNamesRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDispatcher::handleEnumerateInstanceNamesRequest");

   // get the class name
   CIMName className = request->className;

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

      STAT_COPYDISPATCHER_REP
      _enqueueResponse(request, response);
      PEG_METHOD_EXIT();
      return;
   }

   //
   // Get names of descendent classes:
   //
   Array<CIMName> classNames;
   CIMException cimException;
   try
   {
       classNames = _getSubClassNames(request->nameSpace,className);
   }
   catch(CIMException& exception)
   {
       // Return exception response if exception from getSubClasses
       cimException = exception;
       CIMEnumerateInstanceNamesResponseMessage* response =
       new CIMEnumerateInstanceNamesResponseMessage(
         request->messageId,
         cimException,
         request->queueIds.copyAndPop(),
         Array<CIMObjectPath>());
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
   Array<String> serviceNames;
   Array<String> controlProviderNames;
   Array<Boolean> hasProvider;
   Uint32 providersFound = 0;
   for(Uint32 i = 0; i < classNames.size(); i++)
   {
       String serviceName = String::EMPTY;
       String controlProviderName = String::EMPTY;
    
       // Lookup any instance providers and add to send list
       if(_lookupNewInstanceProvider(request->nameSpace, classNames[i],
           serviceName, controlProviderName))
		{
		   providersFound++;
           // Append the returned values to the list to send.
           hasProvider.append(true);
		   serviceNames.append(serviceName);
		   controlProviderNames.append(controlProviderName);
	       CDEBUG("FoundProvider for class = " << classNames[i].getString());
            PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL4,
               "Provider found for Class = " + classNames[i].getString()
			   + " servicename = " + serviceName
			   + " controlProviderName = " 
               + ((controlProviderName != String::EMPTY)  ? controlProviderName : "None"));
		}
       else
       {
           hasProvider.append(false);
           serviceNames.append(String::EMPTY);
           controlProviderNames.append(String::EMPTY);
           CDEBUG("No provider for class = " << classNames[i].getString());
       }
   }

   Uint32 ps = serviceNames.size();

   // Test for "enumerate to Broad" and if so, execute exception.
   // Simply limits the subclass depth to some max, not number
   // of instances returned.
   // ATTN: add size and limit to the message.
   if(ps > _maximumEnumerateBreadth)
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
    CDEBUG("Providers Found = " << providersFound );
    if ((providersFound == 0) && (_repository->isDefaultInstanceProvider()))
    {
        CDEBUG( "Going to repository for everything");  
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
    
          _enqueueResponse(request, response);
          PEG_METHOD_EXIT();
          return;
   }
   // No provider is registered and the repository isn't the default
   if ((providersFound == 0) && !(_repository->isDefaultInstanceProvider()))
   {         
       CDEBUG("No providers, no repository");
       PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL4, 
           "No support for " + className.getString() + " and subclasses."); 

       CIMEnumerateInstanceNamesResponseMessage* response =
         new CIMEnumerateInstanceNamesResponseMessage(
            request->messageId,
            PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, String::EMPTY),
            request->queueIds.copyAndPop(),
            Array<CIMObjectPath>());

      STAT_COPYDISPATCHER

      _enqueueResponse(request, response);
      PEG_METHOD_EXIT();
      return;

   }
   // We have instances for Providers.
   //Set up an aggregate object for the information and save request in it.
   // With the original request message.

   OperationAggregate *poA= new OperationAggregate(
       new CIMEnumerateInstanceNamesRequestMessage(*request),
       request->getType(),
       request->messageId);
   poA->_dest = request->queueIds.top();

   //************* Limit to one response if we have problems with more.
#ifdef LIMIT_ENUM_TO_ONE_LEVEL
   ps = 1;
#endif
   // Simply takes the top level request and ignores the others.
   //*******************/
   poA->setTotalIssued(ps);

   for(Uint32 i = 0; i < ps; i++ )
   {
       poA->classes.append(classNames[i]);
       poA->serviceNames.append(serviceNames[i]);
       poA->controlProviderNames.append(controlProviderNames[i]);
       
       // This should be used for displays, etc for the moment.
       Uint32 current =  poA->classes.size() - 1;
       //request->print(cout, true);


       PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL4, 
           Formatter::format(
           "EnumerateNames Req. class $0 to svc $1 for control provider $2 No $3 of $4",
           poA->classes[current].getString(), poA->serviceNames[current],
           ((poA->controlProviderNames[current] != String::EMPTY)  ?
                poA->controlProviderNames[current] : "None"), i, ps));
        
       /*cout << "KSTEST send to provider, Class =  " << classNames[current]
           << " servicename = " << poA->serviceNames[current]
           << " control provider name = " << ((poA->controlProviderNames[current] != String::EMPTY)  ?
                poA->controlProviderNames[current] : "None") << " count " << current << endl;

       for(Uint32 j = 0; j <= i; j++ )
       {
           cout << "KSTEST View Class Arrays " << " Class = " <<   poA->classes[j]
                << " Service Name = " << poA->serviceNames[j]
                << " Control Provider = " << ((poA->controlProviderNames[j]
                     != String::EMPTY)  ? poA->controlProviderNames[j] : "None") 
                << " count " << j << endl;       }
        */
       // If this class has a provider
       if (!hasProvider[i])
       {
           if (_repository->isDefaultInstanceProvider())
           {
               Tracer::trace(TRC_DISPATCHER, Tracer::LEVEL4,
              "Repository being used as THE default instance provider");
               CDEBUG("Class for repository = " << classNames[i]);

               CIMException cimException;
               STAT_PROVIDERSTART
               Array<CIMObjectPath> instanceNames;
               _repository->read_lock();
               try
               {
                  instanceNames = _repository->enumerateInstanceNamesForClass(
                     request->nameSpace,
                     classNames[i],
                     false);
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
               Boolean isDoneAggregation;
               isDoneAggregation =  poA->appendResponse(response);
               CDEBUG("ReferenceName Send Is response done? = " << ((isDoneAggregation)? "true" : "false"));
               if (isDoneAggregation)
                   handleOperationResponseAggregation(poA);
           }
       }
       else
       {
           CIMEnumerateInstanceNamesRequestMessage* requestCopy =
             new CIMEnumerateInstanceNamesRequestMessage(*request);

           requestCopy->className = poA->classes[current];

           //request->print(cout, true);
           //requestCopy->print(cout, true);
           CDEBUG("Class to Provider = " << poA->classes[current]);
          _forwardRequestForAggregation(poA->serviceNames[current],
               poA->controlProviderNames[current], requestCopy, poA);
       }
   }
   PEG_METHOD_EXIT();
   return;

}
/**$*******************************************************
    handleAssociatorssRequest
**********************************************************/

void CIMOperationRequestDispatcher::handleAssociatorsRequest(
   CIMAssociatorsRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDispatcher::handleAssociatorsRequest");

   if (!_enableAssociationTraversal)
   {
       CIMException cimException =
           PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, "Associators");
       ////////////Array<CIMObject> cimObjects;

       CIMAssociatorsResponseMessage* response =
           new CIMAssociatorsResponseMessage(request->messageId,
	       cimException,
	       request->queueIds.copyAndPop(),
	       Array<CIMObject>());

       STAT_COPYDISPATCHER

       _enqueueResponse(request, response);

       PEG_METHOD_EXIT();
       return;
   }
   CIMException checkClassException;
   _checkExistenceOfClass(request->nameSpace, request->objectName.getClassName(), checkClassException);
   if (checkClassException.getCode() != CIM_ERR_SUCCESS)
   {
       CIMAssociatorsResponseMessage* response =
           new CIMAssociatorsResponseMessage(request->messageId,
	       checkClassException,
	       request->queueIds.copyAndPop(),
	       Array<CIMObject>());

       STAT_COPYDISPATCHER_REP
      _enqueueResponse(request, response);
      PEG_METHOD_EXIT();
      return;
   }

   //  ATTN-CAKG-P2-20020726:  The following condition does not correctly
   //  distinguish instanceNames from classNames in every case
   //  The instanceName of a singleton instance of a keyless class also
   //  has no key bindings
   //
   // if this is a class request, simply go to the Repository

   Boolean isClassRequest = (request->objectName.getKeyBindings ().size () == 0)? true: false;
#define ASSOCIATORS_NEW_CODE
#ifdef ASSOCIATORS_NEW_CODE
   // if is Class request get from repository
   if (isClassRequest)
   {
      CIMException cimException;
      STAT_PROVIDERSTART
      CDEBUG("References executing Class Request");
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
         cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
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

   
    else   // instance request- get instances from providers and repository  
        {
        // Set up an aggregate object for the information and save request in it.
        // With the original request message.
        OperationAggregate *poA = new OperationAggregate(
            new CIMAssociatorsRequestMessage(*request),
            request->getType(),
            request->messageId,
            request->queueIds.top());
       
        CIMName className = request->objectName.getClassName();
        CIMResponseMessage * response;
        //Array<CIMName> classNames;
        //Array<String> serviceNames;
        //Array<String> controlProviderNames;

        Array<ProviderInfo> pI;

        CDEBUG("Reference Provider Lookup");
        // Determine list of providers for this request inc repository
        CIMException cimException;
        try
        {
            pI = _lookupAllAssociationProviders(
                request->nameSpace,
                request->objectName);
        }
        catch(CIMException& exception)
        {
           cimException = exception;
           CIMAssociatorsResponseMessage* response =
             new CIMAssociatorsResponseMessage(request->messageId,
                cimException,
                request->queueIds.copyAndPop(),
                Array<CIMObject>());

          STAT_COPYDISPATCHER

          _enqueueResponse(request, response);
          return;
        }

        if (pI.size() != 0)
        {
            CDEBUG ("references found count to send =  " << pI.size());
            Uint32 ps = pI.size();
            // Set the total issued since loop below will issue this number of responses
            poA->setTotalIssued(ps);
    
            for(Uint32 i = 0; i < ps; i++ )
            {
                // ATTN: KS This should be removed I think. Do not need.
                poA->classes.append(pI[i]._className);
                poA->serviceNames.append(pI[i]._serviceName);
                poA->controlProviderNames.append(pI[i]._controlProviderName);
                Uint32 current =  poA->classes.size() - 1;
    
                if (pI[i]._hasProvider)
                {
                    CDEBUG("Associator send to provider. Class =  " << poA->classes[current].getString()); 
                    CIMAssociatorsRequestMessage* requestCopy =
                      new CIMAssociatorsRequestMessage(*request);
                    requestCopy->objectName.setClassName(pI[i]._className);
                    CDEBUG("Forward for aggreg " << pI[i].getClassName());
                    _forwardRequestForAggregation(poA->serviceNames[current],
                         poA->controlProviderNames[current], requestCopy, poA);
                }
                else  // Does not have provider.
                {
                    CIMException cimException;
                    STAT_PROVIDERSTART
                    CDEBUG("Associators executing Instance Request to Repository" << poA->classes[current].getString());
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
                       cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
                    }
                    catch(...)
                    {
                       cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
                    }

                    _repository->read_unlock();

                    STAT_PROVIDEREND
                    CDEBUG("Associators individual repository access. class= " << poA->classes[current].getString() );
                    CIMAssociatorsResponseMessage* response =
                       new CIMAssociatorsResponseMessage(
                          request->messageId,
                          cimException,
                          request->queueIds.copyAndPop(),
                          cimObjects);

                    STAT_COPYDISPATCHER_REP
                    Boolean isDoneAggregation;
                    isDoneAggregation =  poA->appendResponse(response);
                    CDEBUG("Associator rep return Is response done? = " << ((isDoneAggregation)? "true" : "false"));
                    if (isDoneAggregation)
                        handleOperationResponseAggregation(poA);
                }
            }
            CDEBUG("Associators Rtn false from Lookup");
            }
        else // No provider is registered and the repository isn't the default
        {
            CDEBUG("Associators No Providers Found");
            CIMAssociatorsResponseMessage* response =
              new CIMAssociatorsResponseMessage(request->messageId,
                 PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, String::EMPTY),
                 request->queueIds.copyAndPop(),
                 Array<CIMObject>());
        
           STAT_COPYDISPATCHER
        
           _enqueueResponse(request, response);
        }

   }

   PEG_METHOD_EXIT();
   return;

#else
   if (!isClassRequest)
   {
       CIMName className = request->objectName.getClassName();
       CIMName assocClassName = request->assocClass;
       CIMName resultClassName = request->resultClass;
    
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
   }
   if (isClassRequest || _repository->isDefaultInstanceProvider() )
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
#endif
/*   ********************END OF OLD ASSOC CODE ******************/
}
/**$*******************************************************
    handleAssociatorNamesRequest
**********************************************************/

void CIMOperationRequestDispatcher::handleAssociatorNamesRequest(
   CIMAssociatorNamesRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDispatcher::handleAssociatorNamesRequest");

   if (!_enableAssociationTraversal)
   {
       CIMException cimException =
           PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, "AssociatorNames");

       CIMAssociatorNamesResponseMessage* response =
           new CIMAssociatorNamesResponseMessage( request->messageId,
	       cimException,
	       request->queueIds.copyAndPop(),
	       Array<CIMObjectPath>());

       STAT_COPYDISPATCHER

       _enqueueResponse(request, response);

       PEG_METHOD_EXIT();
       return;
   }

   CIMException checkClassException;
   _checkExistenceOfClass(request->nameSpace, request->objectName.getClassName(), checkClassException);
   if (checkClassException.getCode() != CIM_ERR_SUCCESS)
   {
       CIMAssociatorNamesResponseMessage* response =
           new CIMAssociatorNamesResponseMessage( request->messageId,
           checkClassException,
           request->queueIds.copyAndPop(),
           Array<CIMObjectPath>());
       STAT_COPYDISPATCHER
    
       _enqueueResponse(request, response);
    
       PEG_METHOD_EXIT();
       return;
   }
   //  ATTN-CAKG-P2-20020726:  The following condition does not correctly
   //  distinguish instanceNames from classNames in every case
   //  The instanceName of a singleton instance of a keyless class also
   //  has no key bindings
   //
   // if this is a class request, simply go to the Repository
   
   Boolean isClassRequest = (request->objectName.getKeyBindings ().size () == 0)? true: false;

 #define NEW_ASSOC_NAME_CODE
 #ifdef NEW_ASSOC_NAME_CODE
   // if is Class request get from repository
   if (isClassRequest)
   {
      CIMException cimException;
      STAT_PROVIDERSTART
      CDEBUG("AssociatorNames executing Class Request");
      Array<CIMObjectPath> objectPaths;

      _repository->read_lock();

      try
      {
          objectPaths = _repository->associatorNames(
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
         cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
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
    	    objectPaths);

      STAT_COPYDISPATCHER_REP

      _enqueueResponse(request, response);
   }

   
    else   // instance request- get instances from providers and repository  
        {
        // Set up an aggregate object for the information and save request in it.
        // With the original request message.
        OperationAggregate *poA = new OperationAggregate(
            new CIMAssociatorNamesRequestMessage(*request),
            request->getType(),
            request->messageId,
            request->queueIds.top());

        CIMName className = request->objectName.getClassName();
        CIMResponseMessage * response;

        Array<ProviderInfo> pI;

        CDEBUG("REference Provider Lookup");
        // Determine list of providers for this request inc repository
        CIMException cimException;
        try
        {
            pI = _lookupAllAssociationProviders(
                request->nameSpace,
                request->objectName);
        }
        catch(CIMException& exception)
        {
           cimException = exception;
           CIMAssociatorNamesResponseMessage* response =
             new CIMAssociatorNamesResponseMessage(request->messageId,
                cimException,
                request->queueIds.copyAndPop(),
                Array<CIMObjectPath>());

          STAT_COPYDISPATCHER

          _enqueueResponse(request, response);
          return;
        }

        if (pI.size() != 0)
        {
            CDEBUG ("AssociatorNames found count to send =  " << pI.size());
            Uint32 ps = pI.size();
            // Set the total issued since loop below will issue this number of responses
            poA->setTotalIssued(ps);
    
            for(Uint32 i = 0; i < ps; i++ )
            {
                // ATTN: KS This should be removed I think. Do not need.
                poA->classes.append(pI[i]._className);
                poA->serviceNames.append(pI[i]._serviceName);
                poA->controlProviderNames.append(pI[i]._controlProviderName);
                Uint32 current =  poA->classes.size() - 1;
    
                if (pI[i]._hasProvider)
                {
                    CDEBUG("AssociatorName send to provider. Class =  " << poA->classes[current].getString()); 
                    CIMAssociatorNamesRequestMessage* requestCopy =
                      new CIMAssociatorNamesRequestMessage(*request);
                    // What do I do about the object name??
                    requestCopy->objectName.setClassName(pI[i]._className);
                    //requestCopy->resultClass = request->resultClass;
                    //requestCopy->role = request->role;
                    CDEBUG("Forward association for aggregggregation " << pI[i].getClassName());
                    _forwardRequestForAggregation(poA->serviceNames[current],
                         poA->controlProviderNames[current], requestCopy, poA);
                }
                else  // Does not have provider.
                {
                    CIMException cimException;
                    STAT_PROVIDERSTART
                    CDEBUG("AssociatorNames executing Instance Request to Repository" << poA->classes[current].getString());
                    Array<CIMObjectPath> objectPaths;

                    _repository->read_lock();

                    try
                    {
                        objectPaths = _repository->associatorNames(
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
                       cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
                    }
                    catch(...)
                    {
                       cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
                    }

                    _repository->read_unlock();

                    STAT_PROVIDEREND
                    CDEBUG("AssociatorNames individual repository access. class= " 
                        << poA->classes[current].getString() 
                        << "Got responses = " << objectPaths.size());
                    CIMAssociatorNamesResponseMessage* response =
                       new CIMAssociatorNamesResponseMessage(
                          request->messageId,
                          cimException,
                          request->queueIds.copyAndPop(),
                          objectPaths);

                    STAT_COPYDISPATCHER_REP
                    Boolean isDoneAggregation;
                    isDoneAggregation =  poA->appendResponse(response);
                    CDEBUG("AssociatorName rep return Is response done? = " << ((isDoneAggregation)? "true" : "false"));
                    if (isDoneAggregation)
                        handleOperationResponseAggregation(poA);
                }
            }
            CDEBUG("AssociatorNames Rtn false from Lookup");
            }
        else // No provider is registered and the repository isn't the default
        {
            CDEBUG("AssociatorName No Providers Found");
            CIMAssociatorNamesResponseMessage* response =
              new CIMAssociatorNamesResponseMessage(request->messageId,
                 PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, String::EMPTY),
                 request->queueIds.copyAndPop(),
                 Array<CIMObjectPath>());
        
           STAT_COPYDISPATCHER
        
           _enqueueResponse(request, response);
        }

   }

   PEG_METHOD_EXIT();
   return;
 
 #else
 
   if (!isClassRequest)
   {
       CIMName className = request->objectName.getClassName();
       CIMName resultClassName = request->resultClass;
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
   }
   if (isClassRequest || _repository->isDefaultInstanceProvider())
   {
      CIMException cimException;

      STAT_PROVIDERSTART

      Array<CIMObjectPath> objectPaths;

      _repository->read_lock();

      try
      {
        objectPaths = _repository->associatorNames(
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
#endif
/*****************END OLD ASSOCNAME CODE - KS DLETE THIS****/
}
/**$*******************************************************
    handleReferencesRequest
**********************************************************/

void CIMOperationRequestDispatcher::handleReferencesRequest(
   CIMReferencesRequestMessage* request)
{
    Uint32 ps = 0;
    PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDispatcher::handleReferencesRequest");

   if (!_enableAssociationTraversal)
   {
       CIMException cimException =
           PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, "References");
       ///////Array<CIMObject> cimObjects;

       CIMReferencesResponseMessage* response =
           new CIMReferencesResponseMessage(request->messageId,
    	       cimException,
    	       request->queueIds.copyAndPop(),
    	       Array<CIMObject>());

       STAT_COPYDISPATCHER

       _enqueueResponse(request, response);

       PEG_METHOD_EXIT();
       return;
   }
   CIMException checkClassException;
   _checkExistenceOfClass(request->nameSpace, request->objectName.getClassName(), checkClassException);
   if (checkClassException.getCode() != CIM_ERR_SUCCESS)
   {
       CIMReferencesResponseMessage* response =
           new CIMReferencesResponseMessage(request->messageId,
               checkClassException,
               request->queueIds.copyAndPop(),
               Array<CIMObject>());
    
       STAT_COPYDISPATCHER
    
       _enqueueResponse(request, response);
    
       PEG_METHOD_EXIT();
       return;
   }

   //  ATTN-CAKG-P2-20020726:  The following condition does not correctly
   //  distinguish instanceNames from classNames in every case
   //  The instanceName of a singleton instance of a keyless class also
   //  has no key bindings
   //
   // if this is a class request, simply go to the Repository
   Boolean isClassRequest = (request->objectName.getKeyBindings ().size () == 0)? true: false;

#define NEW_REFERENCES_CODE
#ifdef NEW_REFERENCES_CODE
   // if is Class request get from repository
   if (isClassRequest)
   {
      CIMException cimException;
      STAT_PROVIDERSTART
      CDEBUG("References executing Class Request");
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
         cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
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

   
    else   // instance request- get instances from providers and repository  
        {
        // Set up an aggregate object for the information and save request in it.
        // With the original request message.
        OperationAggregate *poA = new OperationAggregate(
            new CIMReferencesRequestMessage(*request),
            request->getType(),
            request->messageId,
            request->queueIds.top());

//////        poA->_dest = ;
        
        CIMName className = request->objectName.getClassName();
        CIMResponseMessage * response;
        //Array<CIMName> classNames;
        //Array<String> serviceNames;
        //Array<String> controlProviderNames;

        Array<ProviderInfo> pI;

        CDEBUG("REference Provider Lookup");
        // Determine list of providers for this request inc repository
        CIMException cimException;
        try
        {
            pI = _lookupAllAssociationProviders(
                request->nameSpace,
                request->objectName);
        }
        catch(CIMException& exception)
        {
           cimException = exception;
           CIMReferencesResponseMessage* response =
             new CIMReferencesResponseMessage(request->messageId,
                cimException,
                request->queueIds.copyAndPop(),
                Array<CIMObject>());

          STAT_COPYDISPATCHER

          _enqueueResponse(request, response);
          return;
        }

        if (pI.size() != 0)
        {
            CDEBUG ("references found count to send =  " << pI.size());
            Uint32 ps = pI.size();
            // Set the total issued since loop below will issue this number of responses
            poA->setTotalIssued(ps);
    
            for(Uint32 i = 0; i < ps; i++ )
            {
                // ATTN: KS This should be removed I think. Do not need.
                poA->classes.append(pI[i]._className);
                poA->serviceNames.append(pI[i]._serviceName);
                poA->controlProviderNames.append(pI[i]._controlProviderName);
                Uint32 current =  poA->classes.size() - 1;
    
                if (pI[i]._hasProvider)
                {
                    CDEBUG("ReferenceName send to provider. Class =  " << poA->classes[current].getString()); 
                    CIMReferencesRequestMessage* requestCopy =
                      new CIMReferencesRequestMessage(*request);
                    requestCopy->objectName.setClassName(pI[i]._className);
                    CDEBUG("Forward for aggreg " << pI[i].getClassName());
                    _forwardRequestForAggregation(poA->serviceNames[current],
                         poA->controlProviderNames[current], requestCopy, poA);
                }
                else  // Does not have provider.
                {
                    CIMException cimException;
                    STAT_PROVIDERSTART
                    CDEBUG("References executing Instance Request to Repository" << poA->classes[current].getString());
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
                       cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
                    }
                    catch(...)
                    {
                       cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
                    }

                    _repository->read_unlock();

                    STAT_PROVIDEREND
                    CDEBUG("References individual repository access. class= " << poA->classes[current].getString() );
                    CIMReferencesResponseMessage* response =
                       new CIMReferencesResponseMessage(
                          request->messageId,
                          cimException,
                          request->queueIds.copyAndPop(),
                          cimObjects);

                    STAT_COPYDISPATCHER_REP
                    Boolean isDoneAggregation;
                    isDoneAggregation =  poA->appendResponse(response);
                    CDEBUG("ReferenceName rep return Is response done? = " << ((isDoneAggregation)? "true" : "false"));
                    if (isDoneAggregation)
                        handleOperationResponseAggregation(poA);
                }
            }
            CDEBUG("References Rtn false from Lookup");
            }
        else // No provider is registered and the repository isn't the default
        {
            CDEBUG("ReferenceName No Providers Found");
            CIMReferencesResponseMessage* response =
              new CIMReferencesResponseMessage(request->messageId,
                 PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, String::EMPTY),
                 request->queueIds.copyAndPop(),
                 Array<CIMObject>());
        
           STAT_COPYDISPATCHER
        
           _enqueueResponse(request, response);
        }

   }

   PEG_METHOD_EXIT();
   return;



/* ********* FOLLOWING IS OLD CODE THAT EILL BE DELETED KS *************************/
#else
   
   if (!isClassRequest)
   {
           CIMName className = request->objectName.getClassName();
           CIMResponseMessage * response;
        
           cout << "KSTEST Dispatcher References ClassName = " << className.getString() << endl;
        
           // Get the list of classes we want to process.
    
           // check the class name for an "external provider"
           Array<String> providerNames = _lookupAssociationProvider(request->nameSpace, className);
        
           // Who did we get back in providerName array.
           if (providerNames.size() > 0)
               for (Uint32 i = 0; i < providerNames.size(); i++)
                   cout << "KSTEST Dispatcher, AssocProviderNames = " << i << " : "<< providerNames[i] << endl;
           else
               cout << "KSTEST DispatcherNo AssocProvider Names" << endl;
        
           if(providerNames.size() != 0)
           {
              CIMReferencesRequestMessage* requestCopy =
                  new CIMReferencesRequestMessage(*request);
        
              _forwardRequestToService(
                  PEGASUS_QUEUENAME_PROVIDERMANAGER_CPP, requestCopy, response);
        
              PEG_METHOD_EXIT();
              ps = 1;
              return;
       }
   }
   if ( isClassRequest || _repository->isDefaultInstanceProvider())
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
         cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
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

      PEG_METHOD_EXIT();
      return;
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
#endif
/* *********************** END OF REFERNCES OLD CODE ****************/
}
/**$*******************************************************
    handleReferenceNamessRequest
**********************************************************/

void CIMOperationRequestDispatcher::handleReferenceNamesRequest(
   CIMReferenceNamesRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDispatcher::handleReferenceNamesRequest");

   // Generate exception of Association capability disabled.
   if (!_enableAssociationTraversal)
   {
       CIMException cimException =
           PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, "ReferenceNames");
       // KSTEST - DELTETE THIS Array<CIMObjectPath> objectPaths;

       CIMReferenceNamesResponseMessage* response =
           new CIMReferenceNamesResponseMessage( request->messageId,
	       cimException, request->queueIds.copyAndPop(),
	       Array<CIMObjectPath>());

       STAT_COPYDISPATCHER

       _enqueueResponse(request, response);

       PEG_METHOD_EXIT();
       return;
   }
   CIMException checkClassException;
   _checkExistenceOfClass(request->nameSpace, request->objectName.getClassName(), checkClassException);
   if (checkClassException.getCode() != CIM_ERR_SUCCESS)
   {
       CIMReferenceNamesResponseMessage* response =
           new CIMReferenceNamesResponseMessage( request->messageId,
	       checkClassException, request->queueIds.copyAndPop(),
	       Array<CIMObjectPath>());
    
       STAT_COPYDISPATCHER
    
       _enqueueResponse(request, response);
    
       PEG_METHOD_EXIT();
       return;
   }

   //  ATTN-CAKG-P2-20020726:  The following condition does not correctly
   //  distinguish instanceNames from classNames in every case
   //  The instanceName of a singleton instance of a keyless class also
   //  has no key bindings
   //
   // if this is a class request, simply go to the Repository
   Boolean isClassRequest = (request->objectName.getKeyBindings ().size () == 0)? true: false;
   
   // if is Class request get from repository
   if (isClassRequest)
   {
      CIMException cimException;
      STAT_PROVIDERSTART
      CDEBUG("ReferenceNames executing Class Request");
      Array<CIMObjectPath> objectPaths;

      _repository->read_lock();

      try
      {
         objectPaths = _repository->referenceNames(
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
         cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
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
    	    objectPaths);

      STAT_COPYDISPATCHER_REP

      _enqueueResponse(request, response);
   }

   
    else   // instance request- get instances from providers and repository  
        {
        // Set up an aggregate object for the information and save request in it.
        // With the original request message.
        OperationAggregate *poA = new OperationAggregate(
            new CIMReferenceNamesRequestMessage(*request),
            request->getType(),
            request->messageId);

        poA->_dest = request->queueIds.top();
        
        CIMName className = request->objectName.getClassName();
        CIMResponseMessage * response;
        //Array<CIMName> classNames;
        //Array<String> serviceNames;
        //Array<String> controlProviderNames;

        Array<ProviderInfo> pI;

        CDEBUG("REference Provider Lookup");
        // Determine list of providers for this request inc repository
        CIMException cimException;
        try
        {
            pI = _lookupAllAssociationProviders(
                request->nameSpace,
                request->objectName);
        }
        catch(CIMException& exception)
        {
           cimException = exception;
           CIMReferenceNamesResponseMessage* response =
             new CIMReferenceNamesResponseMessage(request->messageId,
                cimException,
                request->queueIds.copyAndPop(),
                Array<CIMObjectPath>());

          STAT_COPYDISPATCHER

          _enqueueResponse(request, response);
          return;
        }

        if (pI.size() != 0)
        {
            CDEBUG ("referenceNames found count to send =  " << pI.size());
            Uint32 ps = pI.size();
            // Set the total issued since loop below will issue this number of responses
            poA->setTotalIssued(ps);
    
            for(Uint32 i = 0; i < ps; i++ )
            {
                // ATTN: KS This should be removed I think. Do not need.
                poA->classes.append(pI[i]._className);
                poA->serviceNames.append(pI[i]._serviceName);
                poA->controlProviderNames.append(pI[i]._controlProviderName);
                Uint32 current =  poA->classes.size() - 1;
    
                if (pI[i]._hasProvider)
                {
                    CDEBUG("ReferenceName send to provider. Class =  " << poA->classes[current].getString()); 
                    CIMReferenceNamesRequestMessage* requestCopy =
                      new CIMReferenceNamesRequestMessage(*request);
                    requestCopy->objectName.setClassName(pI[i]._className);
                    CDEBUG("Forward for aggreg " << pI[i].getClassName());
                    _forwardRequestForAggregation(poA->serviceNames[current],
                         poA->controlProviderNames[current], requestCopy, poA);
                }
                else  // Does not have provider.
                {
                    CIMException cimException;
                    STAT_PROVIDERSTART
                    CDEBUG("ReferenceNames executing Instance Request to Repository" << poA->classes[current].getString());
                    Array<CIMObjectPath> objectPaths;

                    _repository->read_lock();

                    try
                    {
                       objectPaths = _repository->referenceNames(
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
                       cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, exception.getMessage());
                    }
                    catch(...)
                    {
                       cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, String::EMPTY);
                    }

                    _repository->read_unlock();

                    STAT_PROVIDEREND
                    CDEBUG("ReferenceNames individual repository access. class= " 
                        << poA->classes[current].getString() 
                        << "Got responses = " << objectPaths.size());
                    CIMReferenceNamesResponseMessage* response =
                       new CIMReferenceNamesResponseMessage(
                          request->messageId,
                          cimException,
                          request->queueIds.copyAndPop(),
                          objectPaths);

                    STAT_COPYDISPATCHER_REP
                    Boolean isDoneAggregation;
                    isDoneAggregation =  poA->appendResponse(response);
                    CDEBUG("ReferenceName rep return Is response done? = " << ((isDoneAggregation)? "true" : "false"));
                    if (isDoneAggregation)
                        handleOperationResponseAggregation(poA);
                }
            }
            CDEBUG("ReferenceNames Rtn false from Lookup");
            }
        else // No provider is registered and the repository isn't the default
        {
            CDEBUG("ReferenceName No Providers Found");
            CIMReferenceNamesResponseMessage* response =
              new CIMReferenceNamesResponseMessage(request->messageId,
                 PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, String::EMPTY),
                 request->queueIds.copyAndPop(),
                 Array<CIMObjectPath>());
        
           STAT_COPYDISPATCHER
        
           _enqueueResponse(request, response);
        }

   }

   PEG_METHOD_EXIT();
   return;
}
/**$*******************************************************
    handleGetPropertyRequest
**********************************************************/

void CIMOperationRequestDispatcher::handleGetPropertyRequest(
   CIMGetPropertyRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDispatcher::handleGetPropertyRequest");

   CIMName className = request->instanceName.getClassName();
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

/**$*******************************************************
    handleSetPropertyRequest
**********************************************************/

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

   CIMName className = request->instanceName.getClassName();
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


   CIMName className = request->instanceName.getClassName();

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
      STAT_COPYDISPATCHER

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
	 char* charPtr = strdup(stringArray[k].getCString());
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
	 newValue = XmlReader::stringToValue(0, stringValue.getCString(), type);
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
            CIMName paramName = inParameters[i].getParameterName();
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
   const CIMNamespaceName& nameSpace,
   const CIMName& className,
   CIMException& cimException)
{
   if (className.equal (CIMName (PEGASUS_CLASSNAME___NAMESPACE)))
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
