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
//               Dave Rosckes (rosckes@us.ibm.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include "CIMOperationRequestDispatcher.h"
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/XmlReader.h> // stringToValue(), stringArrayToValue()
#include <Pegasus/Common/ContentLanguages.h> // l10n
#include <Pegasus/Common/StatisticalData.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/Formatter.h>
#include <Pegasus/Server/reg_table.h>

// l10n
#include <Pegasus/Common/MessageLoader.h>

PEGASUS_NAMESPACE_BEGIN

PEGASUS_USING_STD;

//#define CDEBUG(X) PEGASUS_STD(cout) << "CIMOpReqDsptchr " << X << PEGASUS_STD(endl)
#define CDEBUG(X)
//#define DEBUG(X) Logger::put (Logger::DEBUG_LOG, "CIMOpReqDsptchr", Logger::INFORMATION, "$0", X)

// Test tool to limit enumerations to a single level.  This is not production and is used only to
// debug special problems in the requests that issue multiple provider operations.
//#define LIMIT_ENUM_TO_ONE_LEVEL

static DynamicRoutingTable _routing_table;

// Variable to control whether we do search or simply single provider for reference and
// associatior lookups.
// ATTN: KS 5 April 2003 This is to be removed ATTN:
// static bool singleProviderType = true;

// Local save for host name. save host name here.  NOTE: Problem if hostname changes.
// Set by object init. Used by aggregator.
String cimAggregationLocalHost;

// static counter for aggretation serial numbers.
// can be used to determine lost aggregations.
static Uint64 cimOperationAggregationSN;


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

   //
   //  Get the maximum breadth of enum parameter from config if it exists.
   //
   // ATTN: KS 20030602 Bypass param acquistion until we get it into the config manager
   // Config param removed for Pegasus 2.2 and until PEP 66 resolved.  In place,
   // we simply allow anything through by setting the breadth to large number.
// #define  MAXENUMBREADTHTESTENABLED
#ifdef MAXENUMBREADTHTESTENABLED
   String maxEnumBreadthOption = String::EMPTY;;
   try
   {
   maxEnumBreadthOption = configManager->getCurrentValue("maximumEnumerationBreadth");
    if (maxEnumBreadthOption != String::EMPTY)
       _maximumEnumerateBreadth = atol(maxEnumBreadthOption.getCString());  }
   catch(...)
   {
   _maximumEnumerateBreadth=50;
   }
#else
   // As part of disabling the test for now, we set to very large number.
   _maximumEnumerateBreadth = 1000;
#endif

   _routing_table = DynamicRoutingTable::get_rw_routing_table();

  cimAggregationLocalHost = System::getHostName();

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

   static AtomicInt _initialized = 0;
   static Mutex _monitor;

   PEG_METHOD_ENTER(TRC_DISPATCHER,
		    "CIMOperationRequestDispatcher::_lookupInternalProvider");
   // Clear the strings since used as test later. Poor code but true now

   service =  String::EMPTY;
   provider = String::EMPTY;
   CIMNamespaceName _wild;
   _wild.clear();
   if(_initialized == 0)
   {
      _monitor.lock(pegasus_thread_self());
      if(_initialized.value() == 0 )
      {
	 _routing_table.insert_record(PEGASUS_CLASSNAME_CONFIGSETTING,
				      PEGASUS_NAMESPACENAME_CONFIG,
				      DynamicRoutingTable::INTERNAL,
				      0,
				      static_cast<MessageQueueService *>
				      (MessageQueue::lookup(PEGASUS_QUEUENAME_CONTROLSERVICE)),
				      PEGASUS_MODULENAME_CONFIGPROVIDER,
				      PEGASUS_QUEUENAME_CONTROLSERVICE);


	 _routing_table.insert_record(PEGASUS_CLASSNAME_AUTHORIZATION,
				      PEGASUS_NAMESPACENAME_AUTHORIZATION,
				      DynamicRoutingTable::INTERNAL,
				      0,
				      static_cast<MessageQueueService *>
				      (MessageQueue::lookup(PEGASUS_QUEUENAME_CONTROLSERVICE)),
				      PEGASUS_MODULENAME_USERAUTHPROVIDER,
				      service = PEGASUS_QUEUENAME_CONTROLSERVICE);

	 _routing_table.insert_record(PEGASUS_CLASSNAME_USER,
				      PEGASUS_NAMESPACENAME_USER,
				      DynamicRoutingTable::INTERNAL,
				      0,
				      static_cast<MessageQueueService *>
				      (MessageQueue::lookup(PEGASUS_QUEUENAME_CONTROLSERVICE)),
				      PEGASUS_MODULENAME_USERAUTHPROVIDER,
				      service = PEGASUS_QUEUENAME_CONTROLSERVICE);

	 _routing_table.insert_record(PEGASUS_CLASSNAME_SHUTDOWN,
				      PEGASUS_NAMESPACENAME_SHUTDOWN,
				      DynamicRoutingTable::INTERNAL,
				      0,
				      static_cast<MessageQueueService *>
				      (MessageQueue::lookup(PEGASUS_QUEUENAME_CONTROLSERVICE)),
				      PEGASUS_MODULENAME_SHUTDOWNPROVIDER,
				      PEGASUS_QUEUENAME_CONTROLSERVICE);


	 _routing_table.insert_record(PEGASUS_CLASSNAME___NAMESPACE,
				      _wild,
				      DynamicRoutingTable::INTERNAL,
				      0,
				      static_cast<MessageQueueService *>
				      (MessageQueue::lookup(PEGASUS_QUEUENAME_CONTROLSERVICE)),
				      PEGASUS_MODULENAME_NAMESPACEPROVIDER,
				      PEGASUS_QUEUENAME_CONTROLSERVICE);

	 _routing_table.insert_record(PEGASUS_CLASSNAME_NAMESPACE,
				      _wild,
				      DynamicRoutingTable::INTERNAL,
				      0,
				      static_cast<MessageQueueService *>
				      (MessageQueue::lookup(PEGASUS_QUEUENAME_CONTROLSERVICE)),
				      PEGASUS_MODULENAME_NAMESPACEPROVIDER,
				      PEGASUS_QUEUENAME_CONTROLSERVICE);


	 _routing_table.insert_record(PEGASUS_CLASSNAME_PROVIDERMODULE,
				      PEGASUS_NAMESPACENAME_PROVIDERREG,
				      DynamicRoutingTable::INTERNAL,
				      0,
				      static_cast<MessageQueueService *>
				      (MessageQueue::lookup(PEGASUS_QUEUENAME_CONTROLSERVICE)),
				      PEGASUS_MODULENAME_PROVREGPROVIDER,
				      PEGASUS_QUEUENAME_CONTROLSERVICE);


	 _routing_table.insert_record(PEGASUS_CLASSNAME_PROVIDER,
				      PEGASUS_NAMESPACENAME_PROVIDERREG,
				      DynamicRoutingTable::INTERNAL,
				      0,
				      static_cast<MessageQueueService *>
				      (MessageQueue::lookup(PEGASUS_QUEUENAME_CONTROLSERVICE)),
				      PEGASUS_MODULENAME_PROVREGPROVIDER,
				      PEGASUS_QUEUENAME_CONTROLSERVICE);

	 _routing_table.insert_record(PEGASUS_CLASSNAME_PROVIDERCAPABILITIES,
				      PEGASUS_NAMESPACENAME_PROVIDERREG,
				      DynamicRoutingTable::INTERNAL,
				      0,
				      static_cast<MessageQueueService *>
				      (MessageQueue::lookup(PEGASUS_QUEUENAME_CONTROLSERVICE)),
				      PEGASUS_MODULENAME_PROVREGPROVIDER,
				      PEGASUS_QUEUENAME_CONTROLSERVICE);

	 _routing_table.insert_record(PEGASUS_CLASSNAME_CONSUMERCAPABILITIES,
				      PEGASUS_NAMESPACENAME_PROVIDERREG,
				      DynamicRoutingTable::INTERNAL,
				      0,
				      static_cast<MessageQueueService *>
				      (MessageQueue::lookup(PEGASUS_QUEUENAME_CONTROLSERVICE)),
				      PEGASUS_MODULENAME_PROVREGPROVIDER,
				      PEGASUS_QUEUENAME_CONTROLSERVICE);


	 if (_enableIndicationService)
	 {

	    _routing_table.insert_record(PEGASUS_CLASSNAME_INDSUBSCRIPTION,
					 _wild,
					 DynamicRoutingTable::INTERNAL,
					 0,
					 static_cast<MessageQueueService *>
					 (MessageQueue::lookup(PEGASUS_QUEUENAME_INDICATIONSERVICE)),
					 String::EMPTY,
					 PEGASUS_QUEUENAME_INDICATIONSERVICE);


	    _routing_table.insert_record(PEGASUS_CLASSNAME_INDHANDLER,
					 _wild,
					 DynamicRoutingTable::INTERNAL,
					 0,
					 static_cast<MessageQueueService *>
					 (MessageQueue::lookup(PEGASUS_QUEUENAME_INDICATIONSERVICE)),
					 String::EMPTY,
					 PEGASUS_QUEUENAME_INDICATIONSERVICE);

	    _routing_table.insert_record(PEGASUS_CLASSNAME_INDHANDLER_CIMXML,
					 _wild,
					 DynamicRoutingTable::INTERNAL,
					 0,
					 static_cast<MessageQueueService *>
					 (MessageQueue::lookup(PEGASUS_QUEUENAME_INDICATIONSERVICE)),
					 String::EMPTY,
					 PEGASUS_QUEUENAME_INDICATIONSERVICE);

	    _routing_table.insert_record(PEGASUS_CLASSNAME_INDHANDLER_SNMP,
					 _wild,
					 DynamicRoutingTable::INTERNAL,
					 0,
					 static_cast<MessageQueueService *>
					 (MessageQueue::lookup(PEGASUS_QUEUENAME_INDICATIONSERVICE)),
					 String::EMPTY,
					 PEGASUS_QUEUENAME_INDICATIONSERVICE);

	    _routing_table.insert_record(PEGASUS_CLASSNAME_INDFILTER,
					 _wild,
					 DynamicRoutingTable::INTERNAL,
					 0,
					 static_cast<MessageQueueService *>
					 (MessageQueue::lookup(PEGASUS_QUEUENAME_INDICATIONSERVICE)),
					 String::EMPTY,
					 PEGASUS_QUEUENAME_INDICATIONSERVICE);

	 }
	 _initialized = 1;
      }

      _monitor.unlock();
   }

   MessageQueueService *router =
      _routing_table.get_routing(className,
				 nameSpace,
				 DynamicRoutingTable::INTERNAL,
				 0,
				 provider,
				 service);
   PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL4,
		    "Internal provider  Service = " + service + " provider " + provider + " found.");

   PEG_METHOD_EXIT();
   if(router)
      return true;
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
		    const CIMNamespaceName& nameSpace,
		    const CIMName& className) throw(CIMException)
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
	    Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
			"CIMOperationRequestDispatcher::_getSubClassNames - Name Space: $0  Class name: $1",
			nameSpace.getString(),
			className.getString());
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

///////////////////////////////////////////////////////////////////////////
// Provider Lookup Functions
///////////////////////////////////////////////////////////
/* _lookupAllInstanceProviders - Returns the list of all subclasses of this
   class along with information about whether it is an instance provider.
   Today this is hacked up because of the different provider types.
*/
Array<ProviderInfo> CIMOperationRequestDispatcher::_lookupAllInstanceProviders(
                    const CIMNamespaceName& nameSpace,
                    const CIMName& className,
                    Uint32& providerCount)  throw(CIMException)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
           "CIMOperationRequestDispatcher::_lookupAllInstanceProviders");

    Array<CIMName> classNames = _getSubClassNames(nameSpace,className);

    Array<ProviderInfo> providerInfoList;
    providerCount = 0;

    // Loop for all classNames found
    for(Uint32 i = 0; i < classNames.size(); i++)
   {
       String serviceName = String::EMPTY;
       String controlProviderName = String::EMPTY;

       ProviderInfo pi(classNames[i]);

       // Lookup any instance providers and add to send list
       if(_lookupNewInstanceProvider(nameSpace, classNames[i],
                        serviceName, controlProviderName))
		{
           // Append the returned values to the list to send.
           pi._serviceName = serviceName;
           pi._controlProviderName = controlProviderName;
           pi._hasProvider = true;
           providerCount++;
           CDEBUG("FoundProvider for class = " << classNames[i].getString());
            PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL4,
               "Provider found for Class = " + classNames[i].getString()
			   + " servicename = " + serviceName
			   + " controlProviderName = "
               + ((controlProviderName != String::EMPTY)  ? controlProviderName : "None"));
		}
       else
       {
           pi._serviceName = String::EMPTY;
           pi._controlProviderName = String::EMPTY;
           pi._hasProvider = false;
           CDEBUG("No provider for class = " << classNames[i].getString());
       }
       providerInfoList.append(pi);
   }
   PEG_METHOD_EXIT();
   return (providerInfoList);
}


/* _lookupInstanceProvider - Looks up the instance provider for the
    classname and namespace.
    Returns the name of the provider.
    NOTE: This is the low level lookup for compatibility with
    the external lookup in provider registration manager.
    Added value is simply to get the name property from the returned
    Instance.

*/
String CIMOperationRequestDispatcher::_lookupInstanceProvider(
   const CIMNamespaceName& nameSpace,
   const CIMName& className)
{
    CIMInstance pInstance;
    CIMInstance pmInstance;
    String providerName;
    String serviceName;

    PEG_METHOD_ENTER(TRC_DISPATCHER,
                     "CIMOperationRequestDispatcher::_lookupInstanceProvider");

    MessageQueueService *router =
       _routing_table.get_routing(className,
				  nameSpace,
				  DynamicRoutingTable::INSTANCE,
				  0,
				  providerName,
				  serviceName);
    if(router)
    {

       PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL4,
			"providerName = " + providerName + " found.");
       PEG_METHOD_EXIT();
       return (providerName);
    }


    if (_providerRegistrationManager->lookupInstanceProvider(
	nameSpace, className, pInstance, pmInstance, false))
    {
    	// get the provder name
    	Uint32 pos = pInstance.findProperty(CIMName ("Name"));

    	if ( pos != PEG_NOT_FOUND )
    	{
    	    pInstance.getProperty(pos).getValue().get(providerName);

            PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL4,
                             "Provider for class " + className.getString()+ " provider " + providerName + " found.");
            PEG_METHOD_EXIT();
            return (providerName);
    	}
    }
    PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL4,
               "Provider for " + className.getString() + " not found.");
    PEG_METHOD_EXIT();
    return(String::EMPTY);
}

/* _lookupNewInstanceProvider - Looks up the internal and/or instance provider
    for the defined namespace and class and returns the serviceName and
    control provider name if a provider is found.
    @return true if a service, control provider, or instance provider is found
    for the defined class and namespace.
    This should be combined with the lookupInstanceProvider code eventually but
    the goal now was to simplify the handlers.
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
   if (_lookupInternalProvider(nameSpace, className, serviceName,controlProviderName))
   {
       hasProvider = true;

       Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
		   "CIMOperationRequestDispatcher::_lookupNewInstanceProvider - Name Space: $0  Class Name: $1  Service Name: $2  Provider Name: $3",
		   nameSpace.getString(),
		   className.getString(),
		   serviceName,
		   controlProviderName);
   }
   else
   {
       // get provider for class
       providerName = _lookupInstanceProvider(nameSpace, className);

       Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
		   "CIMOperationRequestDispatcher::_lookupNewInstanceProvider - Name Space: $0  Class Name: $1  Provider Name: $2",
		   nameSpace.getString(),
		   className.getString(),
		   providerName);
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
    String serviceName;

    MessageQueueService *router =
       _routing_table.get_routing(className,
				  nameSpace,
				  DynamicRoutingTable::METHOD,
				  0,
				  providerName,
				  serviceName);
    if(router)
    {

       PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL4,
			"providerName = " + providerName + " found.");
       PEG_METHOD_EXIT();
       return (providerName);
    }



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
    @param objectName
    @param - referenced parameter that is modified in this funciton.  This is the
    count of actual providers found, not the count of classes.  This differs from
    the count of classes in that the providerInfo list is all classes including those
    that would go to the repository.
    @returns List of ProviderInfo
    @exception - Exceptions From the Repository
    */
 Array<ProviderInfo> CIMOperationRequestDispatcher::_lookupAllAssociationProviders(
    const CIMNamespaceName& nameSpace,
    const CIMObjectPath& objectName,
    const CIMName& assocClass,
    const String& role,
    Uint32& providerCount)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
           "CIMOperationRequestDispatcher::_lookupAllAssociationProviders");

    providerCount = 0;
    Array<ProviderInfo> providerInfoList;

    Array<CIMName> classNames;
    CIMName className = objectName.getClassName();
    PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL4,
        "Association Class Lookup for Class " + className.getString());

// The association class is the basis
// for association registration.  When an association class request is received by the CIMOM
// the target class is the endpoint class or instance.  Prevously we also called
// providers registered against this class.  Thus, typically this would be the same
// class as the instance class.  For example ClassA with Assoc1.  We would register
// an instance and Association provider against Class A.  With this change, we register
// the Instance class as before, but register the association class with the association
// itself, i.e. register it against the class Assoc1.
// In this case, the list of potential classes for providers is the same as the
// reference names, i.e the association classes that have associations pointing back
// to the target class.
// ATTN: KS 20030515. After we test and complete the move to using this option, lets go back and
// change the call to avoid the double conversion to and from CIM Object path.
    CDEBUG("_LookupALLAssocProvider Calling referenceNames. class " << className);
    Array<CIMObjectPath> tmp;
    // Note: we use assoc class here because this is the association function
    // the referencname calls must put the result class here.
    tmp = _repository->referenceNames(
            nameSpace,
            CIMObjectPath(String::EMPTY, CIMNamespaceName(), className),
            assocClass,
            role);
   // returns the list of possible association classes for this target.
    // Convert to classnames
   for (Uint32 i = 0 ; i < tmp.size() ; i++)
   {
       classNames.append(tmp[i].getClassName());
       CDEBUG("Reference Lookup returnsclass " << tmp[i].getClassName());
   }

    CDEBUG("_lookup all assoc Classes Returned class list of size " << classNames.size() << " className= " << className.getString() << " assocClass= " << assocClass);
    for (Uint32 i = 0; i < classNames.size(); i++)
    {
        CDEBUG(" Count i " << i << "Class rtned " << classNames[i].getString());
    }
    PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL4,
             Formatter::format(" Association Lookup $0 classes found",
                classNames.size()));

    // ATTN: KS P2 20030420  What should we do with remote associations if there are any
    for (Uint32 i = 0; i < classNames.size(); i++)
    {
        String serviceName = String::EMPTY;
        String controlProviderName = String::EMPTY;
        ProviderInfo pi(classNames[i]);

        // We use the returned classname for the association classname
        // under the assumption that the registration is for the 
        // association class, not the target class
        if(_lookupNewAssociationProvider(nameSpace, classNames[i],
            serviceName, controlProviderName))
        {
            //CDEBUG("LookupNew return. Class = " <<   classNames[i]);
            pi._serviceName = serviceName;
            pi._controlProviderName = controlProviderName;
            pi._hasProvider = true;
            providerCount++;
            CDEBUG("Found Association Provider for class = " << classNames[i].getString());

            // ATTN: Do not need this trace.  The _LookupNewAssoc Function should
            // handle it.
            PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL4,
                "Provider found for Class = " + classNames[i].getString()
                + " servicename = " + serviceName
                + " controlProviderName = "
                + ((controlProviderName != String::EMPTY)  ? controlProviderName : "None"));
            CDEBUG("Found Assoc wo Provider for class= "  << classNames[i].getString());
        }
        else
        {
            pi._hasProvider = false;
        }
        providerInfoList.append(pi);
    }
    CDEBUG("_lookupAllAssocProvider rtns count = " << providerInfoList.size() << " providerCount= " << providerCount);
    PEG_METHOD_EXIT();
    return (providerInfoList);
}

/* _lookupNewAssociationProvider - Looks up the internal and/or instance provider
    for the defined namespace and class and returns the serviceName and
    control provider name if a provider is found.
    @param nameSpace
    @param assocClass
    @param serviceName
    @param controlProviderName
    @return true if an service, control provider, or instance provider is found
    for the defined class and namespace.
    This should be combined with the lookupInstanceProvider code eventually but
    the goal now was to simplify the handlers.
*/
Boolean CIMOperationRequestDispatcher::_lookupNewAssociationProvider(
            const CIMNamespaceName& nameSpace,
            const CIMName& assocClass,
            String& serviceName,
            String& controlProviderName)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::_lookupNewAssociationProvider");
   PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL4,
       "assocClass = " + assocClass.getString());

   Boolean hasProvider = false;
   String providerName = String::EMPTY;
   // Check for class provided by an internal provider
   if (_lookupInternalProvider(nameSpace, assocClass, serviceName,
           controlProviderName))
       hasProvider = true;
   else
   {
       // get provider for class. Note that we reduce it from
       // Multiple possible class names to a single one here.
       // This is a hack.  Clean up later
       Array<String> tmp;
       CIMException cimException;
       try
       {
           tmp = _lookupAssociationProvider(nameSpace, assocClass);
       }
       catch(CIMException& exception)
       {
          PEG_METHOD_EXIT();
          throw exception;
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

       if (tmp.size() > 0)
       {
           providerName = tmp[0];
       }
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
   const CIMName& assocClass)
{
    // instances of the provider class and provider module class for the response
    Array<CIMInstance> pInstances; // Providers
    Array<CIMInstance> pmInstances; // ProviderModules

    Array<String> providerNames;
    String providerName;

    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::_lookupAssociationProvider");
    // Isolate the provider names from the response and return list of providers
    Boolean returnValue = false;
    CIMException cimException;
    try
    {
        CDEBUG("_lookupAssociationProvider. assocClass= " << assocClass.getString());
        returnValue = _providerRegistrationManager->lookupAssociationProvider(
                nameSpace, assocClass, pInstances, pmInstances);
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
                            + " for Class " + assocClass.getString());
                providerNames.append(providerName);
            }
        }
    }

    if (providerNames.size() == 0)
    {
        PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL4,
            "Association Provider NOT found for Class " + assocClass.getString()
             + " in nameSpace " + nameSpace.getString());
    }
    PEG_METHOD_EXIT();
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
    PEG_METHOD_ENTER(TRC_DISPATCHER,
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

/** _forwardRequestToProviderManager
    This function decides based on the controlProviderNameField
    whether to forward to Service or ControlProvider.
    If controlProviderName String empty,
           ForwardToService,
       else
           ForwardtoControlProvider
    Convience coding to simplify other functions.
*/
void CIMOperationRequestDispatcher::_forwardRequestToProviderManager(
    const CIMName& className,        // only for diagnostic
    const String& serviceName,
    const String& controlProviderName,
    CIMRequestMessage* request)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::_forwardRequestToProviderManager");

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
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::handleReferenceNamesResponseAggregation");
    CIMAssociatorNamesResponseMessage * toResponse =
	(CIMAssociatorNamesResponseMessage *) poA->getResponse(0);
    Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
        "CIMOperationRequestDispatcher::ReferenceNames Response - Name Space: $0  Class name: $1 Response Count: poA->numberResponses",
        poA->_nameSpace.getString(),
        poA->_className.getString(),
        poA->numberResponses());

    // Aggregate the multiple responses into one response.  This moves the
    // individual objects from response 1 ... n to the first response
    // working backward and deleting each response off the end of the array
    //CDEBUG("AssociatorNames aggregating responses. Count =  " << poA->numberResponses());
    for(Uint32 i = poA->numberResponses() - 1; i > 0; i--)
    {
    	CIMAssociatorNamesResponseMessage *fromResponse =
    	    (CIMAssociatorNamesResponseMessage *)poA->getResponse(i);
        // For this response, move the objects to the zeroth response.
        //CDEBUG("AssociatorNames aggregation from " << i << "contains " << fromResponse->objectNames.size());
    	for (Uint32 j = 0; j < fromResponse->objectNames.size(); j++)
    	{
            // Insert path information if it is not already installed
            if (fromResponse->objectNames[j].getHost().size() == 0)
                fromResponse->objectNames[j].setHost(cimAggregationLocalHost);

            if (fromResponse->objectNames[j].getNameSpace().isNull())
                fromResponse->objectNames[j].setNameSpace(poA->_nameSpace);

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
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::handleReferencesResponseAggregation");

    CIMAssociatorsResponseMessage * toResponse =
	(CIMAssociatorsResponseMessage *) poA->getResponse(0);

    Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
        "CIMOperationRequestDispatcher::Associators Response - Name Space: $0  Class name: $1 Response Count: poA->numberResponses",
        poA->_nameSpace.getString(),
        poA->_className.getString(),
        poA->numberResponses());

    // Work backward and delete each response off the end of the array
    for(Uint32 i = poA->numberResponses() - 1; i > 0; i--)
    {
    	CIMAssociatorsResponseMessage *fromResponse =
    	    (CIMAssociatorsResponseMessage *)poA->getResponse(i);

    	for (Uint32 j = 0; j < fromResponse->cimObjects.size(); j++)
    	{
    	    // Test and complete path if necessary.  Required because
            // XML not correct without full path.
            CIMObjectPath p = fromResponse->cimObjects[j].getPath();
            Boolean isChanged = false;
            if (p.getHost().size() ==0)
            {
                isChanged = true;
                p.setHost(cimAggregationLocalHost);
            }
            if (p.getNameSpace().isNull())
            {
                isChanged = true;
                p.setNameSpace(poA->_nameSpace);
            }
            if (isChanged)
            {
                fromResponse->cimObjects[j].setPath(p);
            }
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
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::handleReferencesResponseAggregation");

    CIMReferencesResponseMessage * toResponse =
	(CIMReferencesResponseMessage *) poA->getResponse(0);

    Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
        "CIMOperationRequestDispatcher::References Response - Name Space: $0  Class name: $1 Response Count: poA->numberResponses",
        poA->_nameSpace.getString(),
        poA->_className.getString(),
        poA->numberResponses());

    // Work backward copying the names to the first response
    // and delete each response off the end of the array
    for(Uint32 i = poA->numberResponses() - 1; i > 0; i--)
    {
    	CIMReferencesResponseMessage *fromResponse =
    	    (CIMReferencesResponseMessage *)poA->getResponse(i);

        // for each response, move the objects to the first response
    	for (Uint32 j = 0; j < fromResponse->cimObjects.size(); j++)
    	{
    	    
            // Test for existence of Namespace and host and if not
            // in the path component, add them.
    	    CIMObjectPath p = fromResponse->cimObjects[j].getPath();
            Boolean isChanged = false;
            if (p.getHost().size() ==0)
            {
                isChanged = true;
                p.setHost(cimAggregationLocalHost);
            }
            if (p.getNameSpace().isNull())
            {
                isChanged = true;
                p.setNameSpace(poA->_nameSpace);
            }
            if (isChanged)
            {
                fromResponse->cimObjects[j].setPath(p);
            }
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
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::handleReferenceNamesResponseAggregation");
    CIMReferenceNamesResponseMessage * toResponse =
	(CIMReferenceNamesResponseMessage *) poA->getResponse(0);
    
    // Work backward and delete each response off the end of the array
    // adding it to the toResponse, which is really the first response.
    Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
        "CIMOperationRequestDispatcher::Referencenames Response - Name Space: $0  Class name: $1 Response Count: poA->numberResponses",
        poA->_nameSpace.getString(),
        poA->_className.getString(),
        poA->numberResponses());

    // Aggregate all names into the first response, deleting the others.
    // Work backwards for efficiency.
    // For responses 1 ... n, move the names to the zeroth response
    for(Uint32 i = poA->numberResponses() - 1; i > 0; i--)
    {
    	CIMReferenceNamesResponseMessage *fromResponse =
    	    (CIMReferenceNamesResponseMessage *)poA->getResponse(i);

        // Move each object name to the zeroth response.
        for (Uint32 j = 0; j < fromResponse->objectNames.size(); j++)
    	{
            // Insert path information if it is not already in
            if (fromResponse->objectNames[j].getHost().size() == 0)
                fromResponse->objectNames[j].setHost(cimAggregationLocalHost);

            if (fromResponse->objectNames[j].getNameSpace().isNull())
                fromResponse->objectNames[j].setNameSpace(poA->_nameSpace);

            toResponse->objectNames.append(fromResponse->objectNames[j]);
    	}
    	poA->deleteResponse(i);
    }

    PEG_METHOD_EXIT();
}

/* aggregate the responses for enumerateinstancenames into a single response
*/
void CIMOperationRequestDispatcher::handleEnumerateInstanceNamesResponseAggregation(
    OperationAggregate* poA)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::handleEnumerateInstanceNamesResponseAggregation");
    CIMEnumerateInstanceNamesResponseMessage * toResponse =
	(CIMEnumerateInstanceNamesResponseMessage *) poA->getResponse(0);

    Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
        "CIMOperationRequestDispatcher::EnumerateInstanceNames Response - Name Space: $0  Class name: $1 Response Count: poA->numberResponses",
        poA->_nameSpace.getString(),
        poA->_className.getString(),
        poA->numberResponses());

    // Work backward and delete each response off the end of the array
    for(Uint32 i = poA->numberResponses() - 1; i > 0; i--)
    {
    	CIMEnumerateInstanceNamesResponseMessage *fromResponse =
    	    (CIMEnumerateInstanceNamesResponseMessage *)poA->getResponse(i);

    	for (Uint32 j = 0; j < fromResponse->instanceNames.size(); j++)
    	{
    	    // Duplicate test goes here if we decide to eliminate dups in the future.
            toResponse->instanceNames.append(fromResponse->instanceNames[j]);
    	}
    	poA->deleteResponse(i);
    }
    PEG_METHOD_EXIT();
}

/* The function aggregates individual EnumerateInstance Responses into a single response
   for return to the client. It aggregates the responses into the
   first response (0).
   ATTN: KS 28 May 2002 - At this time we do not do the following:
   1. eliminate duplicates.
   2. prune the properties if localOnly or deepInheritance are set.
   This function does not send any responses.
*/
void CIMOperationRequestDispatcher::handleEnumerateInstancesResponseAggregation(OperationAggregate* poA)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::handleEnumerateInstancesResponse");

    CIMEnumerateInstancesResponseMessage * toResponse =
	(CIMEnumerateInstancesResponseMessage *) poA->getResponse(0);

    Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
        "CIMOperationRequestDispatcher::EnumerateInstances Response - Name Space: $0  Class name: $1 Response Count: poA->numberResponses",
        poA->_nameSpace.getString(),
        poA->_className.getString(),
        poA->numberResponses());

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
   3. determining the language for the aggregated response  l10n
   4. calling the proper function for merging
   5. Issuing the single merged response.
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

   //If there was only one response, simply forward it on.
   // Add the destination queue ID to the first response.

   CIMResponseMessage* response = poA->getResponse(0);

   response->dest = poA->_dest;

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
            Formatter::format("Aggregation Processor $0 total responses $1 errors"
                              , totalResponses, errorCount));

   // Cover the case where we have responses and "some" errors.
   // ATTN: Should we cover the others also?
   // ATTN: Need tool to show what errors we received.
   if (errorCount != 0)
   {
   Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
       "CIMOperationRequestDispatcher::Operation Response Aggregation included error responses - Name Space: $0  Class name: $1 Response Count: poA->numberResponses",
       poA->_nameSpace.getString(),
       poA->_className.getString(),
       totalResponses,
       errorCount);
   }

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
   // ATTN: KS 20030306 We can be more efficient if we just build this into the following loop rather
   // than removing the responses from the list.
   if(errorCount > 0)
   {
       for(Uint32 j = 0; j < poA->numberResponses(); j++)
       {
    	   CIMResponseMessage* response = poA->getResponse(j);
    	   if (response->cimException.getCode() != CIM_ERR_SUCCESS)
    	   {
               PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL4,
                        Formatter::format("Aggregation Processor Error Discard. Error $0"
                                          , response->cimException.getCode()));
    	       poA->deleteResponse(j);
               j--;
           }
       }
   }

    // Used to determine if all the languages in the responses match
    Boolean langMismatch = false;   // l10n

   // Merge the responses into a single CIMEnumerateInstanceNamesResponse
    // If more than one response, go to proper aggregation function
    if(poA->numberResponses() > 1)
    {
    // Multiple responses. Merge them by appending the response components to the first entry
	// l10n start
    	// Aggregate the content langs.  If the language of all the responses are the
        // same, then use that language as the language of the aggregated response.
        // Otherwise, don't set any language in the aggregated response.

	// Get the language of the first response  
    	ContentLanguages firstLang = poA->getResponse(0)->contentLanguages;
        PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL4,
                Formatter::format("Aggregation Processor First Lang = $0"
                                  , firstLang.toString()));    	
  	
	// Loop through the rest of the langs, checking for a mismatch
        // to the lang of the first response
	if (firstLang.size() > 0)
	{
	    // The first language is set.  Scan the rest of the langs.
	    for(Uint32 j = 1; j < poA->numberResponses(); j++)
       	    {
    	        CIMResponseMessage* response = poA->getResponse(j);
    	    
    	        if (response->contentLanguages != firstLang)
    	        {
	           // Found a mismatch.  Set the flag and end the loop	
                   PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL4,
                        Formatter::format("Aggregation Processor Lang Mismatch.  Mismatching lang is: $0"
                                          , response->contentLanguages.toString()));
	           langMismatch = true;                                     
                   break;				    
                }
            }
        }
        else
        {   
             // The first lang is empty.  That guarantees a mismatch.
             langMismatch = true;
        }    
        // l10n -end

        switch( poA->getRequestType())
        {
            case CIM_ENUMERATE_INSTANCE_NAMES_REQUEST_MESSAGE :
                    handleEnumerateInstanceNamesResponseAggregation(poA);
                    break;
            case CIM_ENUMERATE_INSTANCES_REQUEST_MESSAGE :
                    handleEnumerateInstancesResponseAggregation(poA);
                    break;
            case CIM_ASSOCIATORS_REQUEST_MESSAGE :
                    handleAssociatorsResponseAggregation(poA);
                    break;
            case CIM_ASSOCIATOR_NAMES_REQUEST_MESSAGE :
                    handleAssociatorNamesResponseAggregation(poA);
                    break;
            case CIM_REFERENCES_REQUEST_MESSAGE :
                    handleReferencesResponseAggregation(poA);
                    break;
            case CIM_REFERENCE_NAMES_REQUEST_MESSAGE :
                    handleReferenceNamesResponseAggregation(poA);
                    break;
            default :
                    PEGASUS_ASSERT(0);
                    break;
                /// This needs exception rtn attachment.
        }
    }

    // Send the remaining response and delete the aggregator.
    response = poA->getResponse(0);
    response->dest = poA->_dest;
    // l10n
    // If all the langs didn't match, then send no language in the aggregated response.
    if (langMismatch == true)
    {
    	response->contentLanguages.clear(); 
    }
    SendForget(response);
    delete poA;

    PEG_METHOD_EXIT();
}
/*******End of the functions for aggregation***************************/

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

   Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
   "CIMOperationRequestDispatcher::handleEnqueue - Case: $0",
	       request->getType());

// l10n
   // Set the client's requested language into this service thread.
   // This will allow functions in this service to return messages
   // in the correct language.
   CIMMessage * req = dynamic_cast<CIMMessage *>(request);
   if (req != NULL)
   {
	if (req->thread_changed())
        {
	   Thread::setLanguages(new AcceptLanguages(req->acceptLanguages));   		
        }
   } 
   else
   {
	Thread::clearLanguages();
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

     default :

       // l10n

	 Logger::put_l(Logger::ERROR_LOG, System::CIMSERVER, Logger::SEVERE,
		       "Server.CIMOperationRequestDispatcher.HANDLE_ENQUEUE",
		       "$0 - Case: $1 not valid",
		       "CIMOperationRequestDispatcher::handleEnqueue",
		       request->getType());

       //Logger::put(Logger::ERROR_LOG, System::CIMSERVER, Logger::SEVERE,
       //"CIMOperationRequestDispatcher::handleEnqueue - Case: $0 not valid",
       //     request->getType());
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

/**$*******************************************************
    handleGetClassRequest
**********************************************************/

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
    	 request->propertyList);

      Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
		  "CIMOperationRequestDispatcher::handleGetClassRequest - Name Space: $0  Class name: $1",
		  request->nameSpace.getString(),
		  request->className.getString());
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

/**$*******************************************************
    handleGetInstanceRequest
**********************************************************/

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
       Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
		   "CIMOperationRequestDispatcher::handleGetInstanceRequest - CIM exist exception has occurred.  Name Space: $0  Class Name: $1",
		   request->nameSpace.getString(),
		   className.getString());

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

   String serviceName = String::EMPTY;
   String controlProviderName = String::EMPTY;
   // ATTN: KS String providerName = String::EMPTY;

   if(_lookupNewInstanceProvider(request->nameSpace, className, serviceName,
	    controlProviderName))
    {
    	CIMGetInstanceRequestMessage* requestCopy =
    	    new CIMGetInstanceRequestMessage(*request);

    	_forwardRequestToProviderManager(className, serviceName,
            controlProviderName, requestCopy);
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
    	    request->propertyList);
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

/**$*******************************************************
    handleDeleteClassRequest
**********************************************************/

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

      Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
		  "CIMOperationRequestDispatcher::handleDeleteClassRequest - Name Space: $0  Class Name: $1",
		  request->nameSpace.getString(),
		  request->className.getString());

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

/**$*******************************************************
    handleDeleteInstanceRequest
**********************************************************/

void CIMOperationRequestDispatcher::handleDeleteInstanceRequest(
   CIMDeleteInstanceRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDispatcher::handleDeleteInstanceRequest");

   // get the class name
   CIMName className = request->instanceName.getClassName();
   // ATTN: KS CIMResponseMessage * response;

   CIMException checkClassException;
   _checkExistenceOfClass(request->nameSpace, className, checkClassException);
   if (checkClassException.getCode() != CIM_ERR_SUCCESS)
   {
       Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
		   "CIMOperationRequestDispatcher::handleDeleteInstanceRequest - CIM exist exception has occurred.  Name Space: $0  Class Name: $1",
		   request->nameSpace.getString(),
		   className.getString());

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

   if(_lookupNewInstanceProvider(request->nameSpace, className, serviceName,
	    controlProviderName))
    {
    	CIMDeleteInstanceRequestMessage* requestCopy =
    	    new CIMDeleteInstanceRequestMessage(*request);

    	_forwardRequestToProviderManager(className, serviceName, controlProviderName,
    	    requestCopy);
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

	 Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
		     "CIMOperationRequestDispatcher::handleDeleteInstanceRequest - Name Space: $0  Instance Name: $1",
		     request->nameSpace.getString(),
		     request->instanceName.toString());
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

/**$*******************************************************
    handleCreateClassRequest
**********************************************************/

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

      Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
		  "CIMOperationRequestDispatcher::handleCreateClassRequest - Name Space: $0  Class Name: $1",
		  request->nameSpace.getString(),
		  request->newClass.getClassName().getString());
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

/**$*******************************************************
    handleCreateInstanceRequest
**********************************************************/

void CIMOperationRequestDispatcher::handleCreateInstanceRequest(
   CIMCreateInstanceRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDispatcher::handleCreateInstanceRequest()");

   // get the class name
   CIMName className = request->newInstance.getClassName();

   CIMException checkClassException;
   _checkExistenceOfClass(request->nameSpace, className, checkClassException);
   if (checkClassException.getCode() != CIM_ERR_SUCCESS)
   {
       Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
		   "CIMOperationRequestDispatcher::handleGetInstanceRequest - CIM exist exception has occurred.  Name Space: $0  Class Name: $1",
		   request->nameSpace.getString(),
		   className.getString());
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

   if(_lookupNewInstanceProvider(request->nameSpace, className, serviceName,
	    controlProviderName))
    {
    	CIMCreateInstanceRequestMessage* requestCopy =
    	    new CIMCreateInstanceRequestMessage(*request);

    	_forwardRequestToProviderManager(className, serviceName, controlProviderName,
    	    requestCopy);
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

	 Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
		     "CIMOperationRequestDispatcher::handleCreateInstanceRequest - Name Space: $0  Instance name: $1",
		     request->nameSpace.getString(),
		     request->newInstance.getClassName().getString());
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

/**$*******************************************************
    handleModifyClassRequest
    This request is analyzed and if correct, is passed directly to the
    repository
**********************************************************/
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

/**$*******************************************************
    handleModifyInstanceRequest
    ATTN: FIX LOOKUP
**********************************************************/

void CIMOperationRequestDispatcher::handleModifyInstanceRequest(
   CIMModifyInstanceRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDispatcher::handleModifyInstanceRequest");

   // ATTN: Who makes sure the instance name and the instance match?
   // ATTN: KS May 28. Change following to reflect new instancelookup
   // get the class name
   CIMName className = request->modifiedInstance.getClassName();
   // ATTN: KS CIMResponseMessage * response;

   CIMException checkClassException;
   _checkExistenceOfClass(request->nameSpace, className, checkClassException);
   if (checkClassException.getCode() != CIM_ERR_SUCCESS)
   {
       Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
		   "CIMOperationRequestDispatcher::handleModifyInstanceRequest - CIM exist exception has occurred.  Name Space: $0  Class Name: $1",
		   request->nameSpace.getString(),
		   className.getString());
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

   if(_lookupNewInstanceProvider(request->nameSpace, className, serviceName,
	    controlProviderName))
    {
    	CIMModifyInstanceRequestMessage* requestCopy =
    	    new CIMModifyInstanceRequestMessage(*request);

    	_forwardRequestToProviderManager(className, serviceName, controlProviderName,
    	    requestCopy);
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

	 Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
		     "CIMOperationRequestDispatcher::handleModifiedInstanceRequest - Name Space: $0  Instance name: $1",
		     request->nameSpace.getString(),
		     request->modifiedInstance.getClassName().getString());
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

/**$*******************************************************
    handleEnumerateClassesRequest
**********************************************************/

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

      Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
		  "CIMOperationRequestDispatcher::handleEnumerateClassesRequest - Name Space: $0  Class name: $1",
		  request->nameSpace.getString(),
		  request->className.getString());
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

/**$*******************************************************
    handleEnumerateInstanceNamesRequest
    Passed directly to the CIMRepository
**********************************************************/
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

      Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
		  "CIMOperationRequestDispatcher::handleEnumerateClassNamesRequest - Name Space: $0  Class name: $1",
		  request->nameSpace.getString(),
		  request->className.getString());
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
/**$*******EnumerateInstancesRequest*********************************
    handleEnumerateInstancesRequest
    If !validClassName
        generate exception response
        Return
    Get all subclasses to target class
    For all classes
        Get Provider for Class
    If number of providers > BreadthLimit
        generate excetpion
        Return
    if (no Proiders found) and (repository is default)
        Get instances from repository for class and sublclasses
        Generate response
        Return
    if (no providers found) and NOT(repository is default)
        generate CIM_Not Supported response
        Return
    For all targetclass and subclasses
        if class has a provider
            Copy request
            Substitute current class name
            Forward request to provider
       else
            Issue request for this class to repository
            put response on aggregate list.

    Return
**********************************************************/

void CIMOperationRequestDispatcher::handleEnumerateInstancesRequest(
   CIMEnumerateInstancesRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDispatcher::handleEnumerateInstancesRequest");

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
   CIMException cimException;
   Array<ProviderInfo> providerInfos;

   // Note that we modify this in the function.
   Uint32 providerCount;

   try
   {
       providerInfos = _lookupAllInstanceProviders(request->nameSpace,
                                                  className,
                                                  providerCount);
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
   Uint32 toIssueCount = providerInfos.size();

   STAT_PROVIDERSTART

   // Test for "enumerate to Broad" and if so, execute exception.
   // Simply limits the subclass depth to some max, not number
   // of instances returned.
   if(providerCount > _maximumEnumerateBreadth)
   {
       Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
		   "CIMOperationRequestDispatcher::enumerateInstanceNamesRequest - Request-to-broad exception.  Name Space: $0  Class Name: $1 Limit= $2",
		   request->nameSpace.getString(),
		   className.getString(), _maximumEnumerateBreadth, providerCount);

       PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL4,
           Formatter::format("ERROR Enumerate to Broad for  class $0. Limit = $1. Request = $2",
               className.getString(), _maximumEnumerateBreadth, providerCount));

       // l10n

       CIMEnumerateInstancesResponseMessage* response =
         new CIMEnumerateInstancesResponseMessage( request->messageId,
            PEGASUS_CIM_EXCEPTION_L(CIM_ERR_NOT_SUPPORTED, 
                       MessageLoaderParms("Server.CIMOperationRequestDispatcher.ENUM_REQ_TOO_BROAD","Enumerate request too Broad")),
            request->queueIds.copyAndPop(),
            Array<CIMInstance>());

       // CIMEnumerateInstancesResponseMessage* response =
       // new CIMEnumerateInstancesResponseMessage( request->messageId,
       //  PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, "Enumerate request too Broad"),
       //   request->queueIds.copyAndPop(),
       //   Array<CIMInstance>());

      STAT_COPYDISPATCHER

      _enqueueResponse(request, response);
      PEG_METHOD_EXIT();
      return;
   }
    if ((providerCount == 0) && (_repository->isDefaultInstanceProvider()))
    {
        PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL4,
    	 "Repository is default instance provider for Class " + className.getString());

        // If there are no proivders, issue one repository request.
          CIMException cimException;
          STAT_PROVIDERSTART
          Array<CIMInstance> cimNamedInstances;
          _repository->read_lock();
          try
          {
              // Enumerate complete hiearchy from request class.
              cimNamedInstances = _repository->enumerateInstancesForClass(
                 request->nameSpace,
                 request->className,
                 request->deepInheritance,
                 request->localOnly,
                 request->includeQualifiers,
                 request->includeClassOrigin,
                 true,
                 request->propertyList);
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
   if ((providerCount == 0) && !(_repository->isDefaultInstanceProvider()))
   {
       // ATTN: KS 20030425 - Is CIM_ERR_NOT_SUPPORTED correct here
       CDEBUG("No providers, no repository");
       PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL4,
           "CIM_ERROR_NOT_SUPPORTED for " + className.getString() + " and subclasses.");

       CIMEnumerateInstancesResponseMessage* response =
         new CIMEnumerateInstancesResponseMessage( request->messageId,
            PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, String::EMPTY),
            request->queueIds.copyAndPop(),
            Array<CIMInstance>());

      STAT_COPYDISPATCHER

      _enqueueResponse(request, response);
      PEG_METHOD_EXIT();
      return;

   }
   // We have instances for Providers and possibly repository.
   //Set up an aggregate object for the information and save request in it.
   // With the original request message.

   OperationAggregate *poA= new OperationAggregate(
       new CIMEnumerateInstancesRequestMessage(*request),
       request->getType(),
       request->messageId,
       request->queueIds.top(),
       className);
   poA->_aggregationSN = cimOperationAggregationSN++;

   //************* Limit to one response if we have problems with more.
#ifdef LIMIT_ENUM_TO_ONE_LEVEL
   toIssueCount = 1;
#endif
   // Takes the top level request and ignores the others.
   //*******************/
   poA->setTotalIssued(toIssueCount);

   for(Uint32 i = 0; i < toIssueCount; i++ )
   {
       poA->classes.append(providerInfos[i]._className);
       poA->serviceNames.append(providerInfos[i]._serviceName);
       poA->controlProviderNames.append(providerInfos[i]._controlProviderName);

       // This should be used for displays, etc for the moment.
       Uint32 current =  poA->classes.size() - 1;
       //request->print(cout, true);

      STAT_PROVIDERSTART

       PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL4,
           Formatter::format(
           "EnumerateInstances Req. class $0 to svc $1 for control provider $2 No $3 of $4, SN $5",
                poA->classes[current].getString(),
                poA->serviceNames[current],
                ((poA->controlProviderNames[current] != String::EMPTY)  ?
                        poA->controlProviderNames[current] : "None"),
                i, providerCount,
                poA->_aggregationSN));

       // If this class has a provider
       if (!providerInfos[i]._hasProvider)
       {
           if (_repository->isDefaultInstanceProvider())
           {
               PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL4,
                   Formatter::format("Call Repository for class $0 SN $1",
                        providerInfos[i]._className.getString(), poA->_aggregationSN));

               CDEBUG("Class for repository = " << providerInfos[i]._className.getString());

               CIMException cimException;
               STAT_PROVIDERSTART
               Array<CIMInstance> cimNamedInstances;
               _repository->read_lock();
               try
               {
                   // Enumerate instances only for this class
                   cimNamedInstances = _repository->enumerateInstancesForClass(
                      request->nameSpace,
                      providerInfos[i]._className.getString(),
                      request->deepInheritance,
                      request->localOnly,
                      request->includeQualifiers,
                      request->includeClassOrigin,
                      false,
                      request->propertyList);
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

           requestCopy->className = providerInfos[i]._className;

           CDEBUG("Class to Provider = " << providerInfos[i]._className.getString());
           PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL4,
               Formatter::format("Call Provider for class $0 SN $1",
                   poA->classes[current].getString(),
                   poA->_aggregationSN));

           //request->print(cout, true);
          _forwardRequestForAggregation(poA->serviceNames[current],
               poA->controlProviderNames[current], requestCopy, poA);
       }
   }
   PEG_METHOD_EXIT();
   return;
}

/**$*******************************************************
    handleEnumerateInstanceNamesRequest
    Returns instance names from the target class and all
    descendant classes in the hiearchy.
    Looks both in the repository and providers for instances.

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
   // Get names of descendent classes:
   //
   CIMException cimException;
   Array<ProviderInfo> providerInfos;

   // Note that we modify this in the function.
   Uint32 providerCount;

   try
   {
       providerInfos = _lookupAllInstanceProviders(request->nameSpace,
                                                  className,
                                                  providerCount);
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
   Uint32 toIssueCount = providerInfos.size();
   STAT_PROVIDERSTART

   //
   //Find all providers for these classes and modify the subclass list.
   //
   // The following arrays represent the list of subclasses with
   // valid providers
   //Array<String> serviceNames;
   //Array<String> controlProviderNames;
   //Array<Boolean> hasProvider;
   //Uint32 providersFound = 0;

   // Test for "enumerate to Broad" and if so, execute exception.
   // Limits the subclass depth to some max, not number
   // of instances returned.
   if(providerCount > _maximumEnumerateBreadth)
   {
       Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
		   "CIMOperationRequestDispatcher::enumerateInstanceNamesRequest - Request-to-broad exception.  Name Space: $0  Class Name: $1 Limit= $2",
		   request->nameSpace.getString(),
		   className.getString(), _maximumEnumerateBreadth, providerCount);

       PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL4,
           Formatter::format(
           "ERROR Enumerate to Broad for  class $0. Limit = $1. Request = $2",
               className.getString(), _maximumEnumerateBreadth, providerCount));


       // l10n

       CIMEnumerateInstanceNamesResponseMessage* response =
         new CIMEnumerateInstanceNamesResponseMessage( request->messageId,
            PEGASUS_CIM_EXCEPTION_L(CIM_ERR_NOT_SUPPORTED, MessageLoaderParms("Server.CIMOperationRequestDispatcher.ENUM_REQ_TOO_BROAD","Enumerate request too Broad")),
            request->queueIds.copyAndPop(),
            Array<CIMObjectPath>());

       // CIMEnumerateInstanceNamesResponseMessage* response =
       // new CIMEnumerateInstanceNamesResponseMessage( request->messageId,
       //  PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, "Enumerate request too Broad"),
       //   request->queueIds.copyAndPop(),
       //   Array<CIMObjectPath>());

      STAT_COPYDISPATCHER

      _enqueueResponse(request, response);
      PEG_METHOD_EXIT();
      return;
   }
    CDEBUG("Providers Found = " << providerCount );
    if ((providerCount == 0) && (_repository->isDefaultInstanceProvider()))
    {
        CDEBUG( "Going to repository for everything" << className.getString());
        PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL4,
            "Repository is default instance provider for Class " + className.getString());

        // If there are no proivders, issue one repository request.
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
   if ((providerCount == 0) && !(_repository->isDefaultInstanceProvider()))
   {
       CDEBUG("No providers, no repository");
       PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL4,
           "CIM_ERROR_NOT_SUPPORTED for " + className.getString() + " and subclasses.");

       CIMEnumerateInstanceNamesResponseMessage* response =
         new CIMEnumerateInstanceNamesResponseMessage( request->messageId,
            PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, String::EMPTY),
            request->queueIds.copyAndPop(),
            Array<CIMObjectPath>());

      STAT_COPYDISPATCHER

      _enqueueResponse(request, response);
      PEG_METHOD_EXIT();
      return;

   }
   // We have instances for Providers and possibly repository.
   //Set up an aggregate object for the information and save request in it.
   // With the original request message.
   OperationAggregate *poA= new OperationAggregate(
       new CIMEnumerateInstanceNamesRequestMessage(*request),
       request->getType(),
       request->messageId,
       request->queueIds.top(),
       className);
   poA->_aggregationSN = cimOperationAggregationSN++;

   //************* Limit to one response if we have problems with more.
#ifdef LIMIT_ENUM_TO_ONE_LEVEL
   toIssueCount = 1;
#endif
   // Takes the top level request and ignores the others.
   //*******************/
   poA->setTotalIssued(toIssueCount);

   for(Uint32 i = 0; i < toIssueCount; i++ )
   {
       // save for diagnostics.
       poA->classes.append(providerInfos[i]._className);
       poA->serviceNames.append(providerInfos[i]._serviceName);
       poA->controlProviderNames.append(providerInfos[i]._controlProviderName);

       // This should be used for displays, etc for the moment.
       Uint32 current =  poA->classes.size() - 1;
       //request->print(cout, true);


       PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL4,
           Formatter::format(
           "EnumerateNames Req. class $0 to svc $1 for control provider $2 No $3 of $4, SN $5",
               poA->classes[current].getString(), poA->serviceNames[current],
               ((poA->controlProviderNames[current] != String::EMPTY)  ?
                    poA->controlProviderNames[current] : "None"),
               i, toIssueCount,
               poA->_aggregationSN));

       // If this class has a provider
       if (!providerInfos[i]._hasProvider)
       {
           if (_repository->isDefaultInstanceProvider())
           {
               PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL4,
                   Formatter::format("Call Repository for class $0 SN $1",
                        providerInfos[i]._className.getString(), poA->_aggregationSN));

               CDEBUG("Class for repository = " << providerInfos[i]._className.getString());

               CIMException cimException;
               STAT_PROVIDERSTART
               Array<CIMObjectPath> instanceNames;
               _repository->read_lock();
               try
               {
                  instanceNames = _repository->enumerateInstanceNamesForClass(
                             request->nameSpace,
                             providerInfos[i]._className,
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

           requestCopy->className = providerInfos[i]._className;

           //request->print(cout, true);
           CDEBUG("Class to Provider = " << providerInfos[i]._className);
           PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL4,
               Formatter::format("Call Provider for class $0 SN $1",
                   poA->classes[current].getString(),
                   poA->_aggregationSN));

          _forwardRequestForAggregation(poA->serviceNames[current],
               poA->controlProviderNames[current], requestCopy, poA);
       }
   }
   PEG_METHOD_EXIT();
   return;

}

/**$*******************************************************
    handleAssociatorsRequest
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
    _checkExistenceOfClass(request->nameSpace,
                           request->objectName.getClassName(),
                           checkClassException);
    if (checkClassException.getCode() != CIM_ERR_SUCCESS)
    {
        CIMAssociatorsResponseMessage* response =
            new CIMAssociatorsResponseMessage(
                request->messageId,
                checkClassException,
                request->queueIds.copyAndPop(),
                Array<CIMObject>());

        STAT_COPYDISPATCHER

        _enqueueResponse(request, response);

        PEG_METHOD_EXIT();
        return;
    }

    Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
        "CIMOperationRequestDispatcher::handleAssociators - "
            "Name Space: $0  Class name: $1",
        request->nameSpace.getString(),
        request->objectName.toString());

    //  ATTN-CAKG-P2-20020726:  The following condition does not correctly
    //  distinguish instanceNames from classNames in every case
    //  The instanceName of a singleton instance of a keyless class also
    //  has no key bindings
    Boolean isClassRequest =
        (request->objectName.getKeyBindings().size() == 0) ? true : false;

    if (isClassRequest)
    {
        //
        // For Class requests, get the results from the repository
        //

        Tracer::trace(TRC_DISPATCHER, Tracer::LEVEL4,
                      "Associators executing Class request");

        CIMException cimException;
        Array<CIMObject> cimObjects;

        STAT_PROVIDERSTART

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
                request->propertyList);
        }
        catch(CIMException& exception)
        {
            cimException = exception;
        }
        catch(Exception& exception)
        {
            cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
                                                 exception.getMessage());
        }
        catch(...)
        {
            cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
                                                 String::EMPTY);
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
    else
    {
        //
        // For Instance requests, get results from providers and the repository
        //

        //
        // Determine list of providers for this request
        //

        Array<ProviderInfo> providerInfo;
        Uint32 providerCount;
        try
        {
            providerInfo = _lookupAllAssociationProviders(
                request->nameSpace,
                request->objectName,
                request->assocClass,
                String::EMPTY,
                providerCount);
        }
        catch(CIMException& cimException)
        {
            CIMAssociatorsResponseMessage* response =
                new CIMAssociatorsResponseMessage(
                    request->messageId,
                    cimException,
                    request->queueIds.copyAndPop(),
                    Array<CIMObject>());

            STAT_COPYDISPATCHER

            _enqueueResponse(request, response);
            PEG_METHOD_EXIT();
            return;
        }
        Tracer::trace(TRC_DISPATCHER, Tracer::LEVEL4,
                      "providerCount = %u.", providerCount);

        //
        // Get the instances from the repository, as necessary
        //

        Array<CIMObject> cimObjects;
        CIMException cimException;

        if (_repository->isDefaultInstanceProvider())
        {
            STAT_PROVIDERSTART

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
                    request->propertyList);
            }
            catch(CIMException& exception)
            {
                cimException = exception;
            }
            catch(Exception& exception)
            {
                cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
                                                     exception.getMessage());
            }
            catch(...)
            {
                cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
                                                     String::EMPTY);
            }

            _repository->read_unlock();

            STAT_PROVIDEREND

            Tracer::trace(TRC_DISPATCHER, Tracer::LEVEL4,
                "Associators repository access: class = %s, count = %u.",
                    (const char*)request->objectName.toString().getCString(),
                    cimObjects.size());
        }

        // Store the repository results in a response message.
        // Note: if not using the repository, this response has no instances.
        CIMAssociatorsResponseMessage* response =
            new CIMAssociatorsResponseMessage(
                request->messageId,
                cimException,
                request->queueIds.copyAndPop(),
                cimObjects);

        STAT_COPYDISPATCHER_REP

        //
        // If we have no providers to call, just return what we've got
        //

        if (providerCount == 0)
        {
             _enqueueResponse(request, response);
             PEG_METHOD_EXIT();
             return;
        }

        //
        // Set up an aggregate object and save the original request message
        //

        OperationAggregate *poA = new OperationAggregate(
            new CIMAssociatorsRequestMessage(*request),
            request->getType(),
            request->messageId,
            request->queueIds.top(),
            request->objectName.getClassName(),
            request->nameSpace);

        poA->_aggregationSN = cimOperationAggregationSN++;
        poA->setTotalIssued(providerCount+1);
        poA->appendResponse(response);  // Save the repository's results

        for (Uint32 i = 0; i < providerInfo.size(); i++)
        {
            if (providerInfo[i]._hasProvider)
            {
                CIMAssociatorsRequestMessage* requestCopy =
                    new CIMAssociatorsRequestMessage(*request);
                // Insert the association class name to limit the provider
                // to this class.
                requestCopy->assocClass = providerInfo[i]._className;

                PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL4,
                    "Forwarding to provider for class " +
                    providerInfo[i]._className.getString());
                _forwardRequestForAggregation(providerInfo[i]._serviceName,
                    providerInfo[i]._controlProviderName, requestCopy, poA);
                // Note: poA must not be referenced after last "forwardRequest"
            }
        }
    }  // End of instance processing

    PEG_METHOD_EXIT();
    return;
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
            new CIMAssociatorNamesResponseMessage(request->messageId,
                cimException,
                request->queueIds.copyAndPop(),
                Array<CIMObjectPath>());

        STAT_COPYDISPATCHER

        _enqueueResponse(request, response);

        PEG_METHOD_EXIT();
        return;
    }

    CIMException checkClassException;
    _checkExistenceOfClass(request->nameSpace,
                           request->objectName.getClassName(),
                           checkClassException);
    if (checkClassException.getCode() != CIM_ERR_SUCCESS)
    {
        CIMAssociatorNamesResponseMessage* response =
            new CIMAssociatorNamesResponseMessage(
                request->messageId,
                checkClassException,
                request->queueIds.copyAndPop(),
                Array<CIMObjectPath>());

        STAT_COPYDISPATCHER

        _enqueueResponse(request, response);

        PEG_METHOD_EXIT();
        return;
    }

    Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
        "CIMOperationRequestDispatcher::handleAssociatorNames - "
            "Name Space: $0  Class name: $1",
        request->nameSpace.getString(),
        request->objectName.toString());

    //  ATTN-CAKG-P2-20020726:  The following condition does not correctly
    //  distinguish instanceNames from classNames in every case
    //  The instanceName of a singleton instance of a keyless class also
    //  has no key bindings
    Boolean isClassRequest =
        (request->objectName.getKeyBindings().size() == 0) ? true : false;

    if (isClassRequest)
    {
        //
        // For Class requests, get the results from the repository
        //

        Tracer::trace(TRC_DISPATCHER, Tracer::LEVEL4,
                      "AssociatorNames executing Class request");

        CIMException cimException;
        Array<CIMObjectPath> cimObjectPaths;

        STAT_PROVIDERSTART

        _repository->read_lock();

        try
        {
            cimObjectPaths = _repository->associatorNames(
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
            cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
                                                 exception.getMessage());
        }
        catch(...)
        {
            cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
                                                 String::EMPTY);
        }

        _repository->read_unlock();

        STAT_PROVIDEREND

        CIMAssociatorNamesResponseMessage* response =
            new CIMAssociatorNamesResponseMessage(
                request->messageId,
                cimException,
                request->queueIds.copyAndPop(),
                cimObjectPaths);

        STAT_COPYDISPATCHER_REP

        _enqueueResponse(request, response);
    }
    else
    {
        //
        // For Instance requests, get results from providers and the repository
        //

        //
        // Determine list of providers for this request
        //

        Array<ProviderInfo> providerInfo;
        Uint32 providerCount;
        try
        {
            providerInfo = _lookupAllAssociationProviders(
                request->nameSpace,
                request->objectName,
                request->assocClass,
                String::EMPTY,
                providerCount);
        }
        catch(CIMException& cimException)
        {
            CIMAssociatorNamesResponseMessage* response =
                new CIMAssociatorNamesResponseMessage(
                    request->messageId,
                    cimException,
                    request->queueIds.copyAndPop(),
                    Array<CIMObjectPath>());

            STAT_COPYDISPATCHER

            _enqueueResponse(request, response);
            PEG_METHOD_EXIT();
            return;
        }
        Tracer::trace(TRC_DISPATCHER, Tracer::LEVEL4,
                      "providerCount = %u.", providerCount);

        //
        // Get the instances from the repository, as necessary
        //

        Array<CIMObjectPath> cimObjectPaths;
        CIMException cimException;

        if (_repository->isDefaultInstanceProvider())
        {
            STAT_PROVIDERSTART

            _repository->read_lock();

            try
            {
                cimObjectPaths = _repository->associatorNames(
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
                cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
                                                     exception.getMessage());
            }
            catch(...)
            {
                cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
                                                     String::EMPTY);
            }

            _repository->read_unlock();

            STAT_PROVIDEREND

            Tracer::trace(TRC_DISPATCHER, Tracer::LEVEL4,
                "AssociatorNames repository access: class = %s, count = %u.",
                    (const char*)request->objectName.toString().getCString(),
                    cimObjectPaths.size());
        }

        // Store the repository results in a response message.
        // Note: if not using the repository, this response has no instances.
        CIMAssociatorNamesResponseMessage* response =
            new CIMAssociatorNamesResponseMessage(
                request->messageId,
                cimException,
                request->queueIds.copyAndPop(),
                cimObjectPaths);

        STAT_COPYDISPATCHER_REP

        //
        // If we have no providers to call, just return what we've got
        //

        if (providerCount == 0)
        {
             _enqueueResponse(request, response);
             PEG_METHOD_EXIT();
             return;
        }

        //
        // Set up an aggregate object and save the original request message
        //

        OperationAggregate *poA = new OperationAggregate(
            new CIMAssociatorNamesRequestMessage(*request),
            request->getType(),
            request->messageId,
            request->queueIds.top(),
            request->objectName.getClassName(),
            request->nameSpace);

        poA->_aggregationSN = cimOperationAggregationSN++;
        poA->setTotalIssued(providerCount+1);
        poA->appendResponse(response);  // Save the repository's results

        for (Uint32 i = 0; i < providerInfo.size(); i++)
        {
            if (providerInfo[i]._hasProvider)
            {
                CIMAssociatorNamesRequestMessage* requestCopy =
                    new CIMAssociatorNamesRequestMessage(*request);
                // Insert the association class name to limit the provider
                // to this class.
                requestCopy->assocClass = providerInfo[i]._className;

                PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL4,
                    "Forwarding to provider for class " +
                    providerInfo[i]._className.getString());
                _forwardRequestForAggregation(providerInfo[i]._serviceName,
                    providerInfo[i]._controlProviderName, requestCopy, poA);
                // Note: poA must not be referenced after last "forwardRequest"
            }
        }
    }  // End of instance processing

    PEG_METHOD_EXIT();
    return;
}

/**$*******************************************************
    handleReferencesRequest
**********************************************************/

void CIMOperationRequestDispatcher::handleReferencesRequest(
    CIMReferencesRequestMessage* request)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::handleReferencesRequest");

    if (!_enableAssociationTraversal)
    {
        CIMException cimException =
            PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, "References");

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
    _checkExistenceOfClass(request->nameSpace,
                           request->objectName.getClassName(),
                           checkClassException);
    if (checkClassException.getCode() != CIM_ERR_SUCCESS)
    {
        CIMReferencesResponseMessage* response =
            new CIMReferencesResponseMessage(
                request->messageId,
                checkClassException,
                request->queueIds.copyAndPop(),
                Array<CIMObject>());

        STAT_COPYDISPATCHER

        _enqueueResponse(request, response);

        PEG_METHOD_EXIT();
        return;
    }

    Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
        "CIMOperationRequestDispatcher::handleReferences - "
            "Name Space: $0  Class name: $1",
        request->nameSpace.getString(),
        request->objectName.toString());

    //  ATTN-CAKG-P2-20020726:  The following condition does not correctly
    //  distinguish instanceNames from classNames in every case
    //  The instanceName of a singleton instance of a keyless class also
    //  has no key bindings
    Boolean isClassRequest =
        (request->objectName.getKeyBindings().size() == 0) ? true : false;

    if (isClassRequest)
    {
        //
        // For Class requests, get the results from the repository
        //

        Tracer::trace(TRC_DISPATCHER, Tracer::LEVEL4,
                      "References executing Class request");

        CIMException cimException;
        Array<CIMObject> cimObjects;

        STAT_PROVIDERSTART

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
                request->propertyList);
        }
        catch(CIMException& exception)
        {
            cimException = exception;
        }
        catch(Exception& exception)
        {
            cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
                                                 exception.getMessage());
        }
        catch(...)
        {
            cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
                                                 String::EMPTY);
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
    else
    {
        //
        // For Instance requests, get results from providers and the repository
        //

        //
        // Determine list of providers for this request
        //

        Array<ProviderInfo> providerInfo;
        Uint32 providerCount;
        try
        {
            providerInfo = _lookupAllAssociationProviders(
                request->nameSpace,
                request->objectName,
                request->resultClass,
                String::EMPTY,
                providerCount);
        }
        catch(CIMException& cimException)
        {
            CIMReferencesResponseMessage* response =
                new CIMReferencesResponseMessage(
                    request->messageId,
                    cimException,
                    request->queueIds.copyAndPop(),
                    Array<CIMObject>());

            STAT_COPYDISPATCHER

            _enqueueResponse(request, response);
            PEG_METHOD_EXIT();
            return;
        }
        Tracer::trace(TRC_DISPATCHER, Tracer::LEVEL4,
                      "providerCount = %u.", providerCount);

        //
        // Get the instances from the repository, as necessary
        //

        Array<CIMObject> cimObjects;
        CIMException cimException;

        if (_repository->isDefaultInstanceProvider())
        {
            STAT_PROVIDERSTART

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
                    request->propertyList);
            }
            catch(CIMException& exception)
            {
                cimException = exception;
            }
            catch(Exception& exception)
            {
                cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
                                                     exception.getMessage());
            }
            catch(...)
            {
                cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
                                                     String::EMPTY);
            }

            _repository->read_unlock();

            STAT_PROVIDEREND

            Tracer::trace(TRC_DISPATCHER, Tracer::LEVEL4,
                "References repository access: class = %s, count = %u.",
                    (const char*)request->objectName.toString().getCString(),
                    cimObjects.size());
        }

        // Store the repository results in a response message.
        // Note: if not using the repository, this response has no instances.
        CIMReferencesResponseMessage* response =
            new CIMReferencesResponseMessage(
                request->messageId,
                cimException,
                request->queueIds.copyAndPop(),
                cimObjects);

        STAT_COPYDISPATCHER_REP

        //
        // If we have no providers to call, just return what we've got
        //

        if (providerCount == 0)
        {
             _enqueueResponse(request, response);
             PEG_METHOD_EXIT();
             return;
        }

        //
        // Set up an aggregate object and save the original request message
        //

        OperationAggregate *poA = new OperationAggregate(
            new CIMReferencesRequestMessage(*request),
            request->getType(),
            request->messageId,
            request->queueIds.top(),
            request->objectName.getClassName(),
            request->nameSpace);

        poA->_aggregationSN = cimOperationAggregationSN++;
        poA->setTotalIssued(providerCount+1);
        poA->appendResponse(response);  // Save the repository's results

        for (Uint32 i = 0; i < providerInfo.size(); i++)
        {
            if (providerInfo[i]._hasProvider)
            {
                CIMReferencesRequestMessage* requestCopy =
                    new CIMReferencesRequestMessage(*request);
                // Insert the association class name to limit the provider
                // to this class.
                requestCopy->resultClass = providerInfo[i]._className;

                PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL4,
                    "Forwarding to provider for class " +
                    providerInfo[i]._className.getString());
                _forwardRequestForAggregation(providerInfo[i]._serviceName,
                    providerInfo[i]._controlProviderName, requestCopy, poA);
                // Note: poA must not be referenced after last "forwardRequest"
            }
        }
    }  // End of instance processing

    PEG_METHOD_EXIT();
    return;
}

/**$*******************************************************
    handleReferenceNamesRequest
**********************************************************/

void CIMOperationRequestDispatcher::handleReferenceNamesRequest(
    CIMReferenceNamesRequestMessage* request)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::handleReferenceNamesRequest");

    if (!_enableAssociationTraversal)
    {
        CIMException cimException =
            PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, "ReferenceNames");

        CIMReferenceNamesResponseMessage* response =
            new CIMReferenceNamesResponseMessage(request->messageId,
                cimException,
                request->queueIds.copyAndPop(),
                Array<CIMObjectPath>());

        STAT_COPYDISPATCHER

        _enqueueResponse(request, response);

        PEG_METHOD_EXIT();
        return;
    }

    CIMException checkClassException;
    _checkExistenceOfClass(request->nameSpace,
                           request->objectName.getClassName(),
                           checkClassException);
    if (checkClassException.getCode() != CIM_ERR_SUCCESS)
    {
        CIMReferenceNamesResponseMessage* response =
            new CIMReferenceNamesResponseMessage(
                request->messageId,
                checkClassException,
                request->queueIds.copyAndPop(),
                Array<CIMObjectPath>());

        STAT_COPYDISPATCHER

        _enqueueResponse(request, response);

        PEG_METHOD_EXIT();
        return;
    }

    Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
        "CIMOperationRequestDispatcher::handleReferenceNames - "
            "Name Space: $0  Class name: $1",
        request->nameSpace.getString(),
        request->objectName.toString());

    //  ATTN-CAKG-P2-20020726:  The following condition does not correctly
    //  distinguish instanceNames from classNames in every case
    //  The instanceName of a singleton instance of a keyless class also
    //  has no key bindings
    Boolean isClassRequest =
        (request->objectName.getKeyBindings().size() == 0) ? true : false;

    if (isClassRequest)
    {
        //
        // For Class requests, get the results from the repository
        //

        Tracer::trace(TRC_DISPATCHER, Tracer::LEVEL4,
                      "ReferenceNames executing Class request");

        CIMException cimException;
        Array<CIMObjectPath> cimObjectPaths;

        STAT_PROVIDERSTART

        _repository->read_lock();

        try
        {
            cimObjectPaths = _repository->referenceNames(
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
            cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
                                                 exception.getMessage());
        }
        catch(...)
        {
            cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
                                                 String::EMPTY);
        }

        _repository->read_unlock();

        STAT_PROVIDEREND

        CIMReferenceNamesResponseMessage* response =
            new CIMReferenceNamesResponseMessage(
                request->messageId,
                cimException,
                request->queueIds.copyAndPop(),
                cimObjectPaths);

        STAT_COPYDISPATCHER_REP

        _enqueueResponse(request, response);
    }
    else
    {
        //
        // For Instance requests, get results from providers and the repository
        //

        //
        // Determine list of providers for this request
        //

        Array<ProviderInfo> providerInfo;
        Uint32 providerCount;
        try
        {
            providerInfo = _lookupAllAssociationProviders(
                request->nameSpace,
                request->objectName,
                request->resultClass,
                String::EMPTY,
                providerCount);
        }
        catch(CIMException& cimException)
        {
            CIMReferenceNamesResponseMessage* response =
                new CIMReferenceNamesResponseMessage(
                    request->messageId,
                    cimException,
                    request->queueIds.copyAndPop(),
                    Array<CIMObjectPath>());

            STAT_COPYDISPATCHER

            _enqueueResponse(request, response);
            PEG_METHOD_EXIT();
            return;
        }
        Tracer::trace(TRC_DISPATCHER, Tracer::LEVEL4,
                      "providerCount = %u.", providerCount);

        //
        // Get the instances from the repository, as necessary
        //

        Array<CIMObjectPath> cimObjectPaths;
        CIMException cimException;

        if (_repository->isDefaultInstanceProvider())
        {
            STAT_PROVIDERSTART

            _repository->read_lock();

            try
            {
                cimObjectPaths = _repository->referenceNames(
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
                cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
                                                     exception.getMessage());
            }
            catch(...)
            {
                cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
                                                     String::EMPTY);
            }

            _repository->read_unlock();

            STAT_PROVIDEREND

            Tracer::trace(TRC_DISPATCHER, Tracer::LEVEL4,
                "ReferenceNames repository access: class = %s, count = %u.",
                    (const char*)request->objectName.toString().getCString(),
                    cimObjectPaths.size());
        }

        // Store the repository results in a response message.
        // Note: if not using the repository, this response has no instances.
        CIMReferenceNamesResponseMessage* response =
            new CIMReferenceNamesResponseMessage(
                request->messageId,
                cimException,
                request->queueIds.copyAndPop(),
                cimObjectPaths);

        STAT_COPYDISPATCHER_REP

        //
        // If we have no providers to call, just return what we've got
        //

        if (providerCount == 0)
        {
             _enqueueResponse(request, response);
             PEG_METHOD_EXIT();
             return;
        }

        //
        // Set up an aggregate object and save the original request message
        //

        OperationAggregate *poA = new OperationAggregate(
            new CIMReferenceNamesRequestMessage(*request),
            request->getType(),
            request->messageId,
            request->queueIds.top(),
            request->objectName.getClassName(),
            request->nameSpace);

        poA->_aggregationSN = cimOperationAggregationSN++;
        poA->setTotalIssued(providerCount+1);
        poA->appendResponse(response);  // Save the repository's results

        for (Uint32 i = 0; i < providerInfo.size(); i++)
        {
            if (providerInfo[i]._hasProvider)
            {
                CIMReferenceNamesRequestMessage* requestCopy =
                    new CIMReferenceNamesRequestMessage(*request);
                // Insert the association class name to limit the provider
                // to this class.
                requestCopy->resultClass = providerInfo[i]._className;

                PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL4,
                    "Forwarding to provider for class " +
                    providerInfo[i]._className.getString());
                _forwardRequestForAggregation(providerInfo[i]._serviceName,
                    providerInfo[i]._controlProviderName, requestCopy, poA);
                // Note: poA must not be referenced after last "forwardRequest"
            }
        }
    }  // End of instance processing

    PEG_METHOD_EXIT();
    return;
}

/**$*******************************************************
    handleGetPropertyRequest
    ATTN: FIX LOOKUP
**********************************************************/

void CIMOperationRequestDispatcher::handleGetPropertyRequest(
   CIMGetPropertyRequestMessage* request)
{
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDispatcher::handleGetPropertyRequest");

   CIMName className = request->instanceName.getClassName();
   CIMResponseMessage * response;

   // check the class name for an "external provider"
   // Assumption here is that there are no "internal" property requests.
   // teATTN: KS 20030402 - This needs cleanup along with the setproperty.
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
    ATTN: FIX LOOKUP
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
	  Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
		      "CIMOperationRequestDispatcher::handleSetPropertyRequest - CIM exception has occurred.");
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

	 Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
		     "CIMOperationRequestDispatcher::handleSetPropertyRequest - Name Space: $0  Instance Name: $1  Property Name: $2  New Value: $3",
		     request->nameSpace.getString(),
		     request->instanceName.getClassName().getString(),
		     request->propertyName.getString(),
		     request->newValue.toString());
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

/**$*******************************************************
    handleGetQualifierRequest
**********************************************************/

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

      Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
		  "CIMOperationRequestDispatcher::handleGetQualifierRequest - Name Space: $0  Qualifier Name: $1",
		  request->nameSpace.getString(),
		  request->qualifierName.getString());
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

/**$*******************************************************
    handleSetQualifierRequest
**********************************************************/

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

 Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
		  "CIMOperationRequestDispatcher::handleSetQualifierRequest - Name Space: $0  Qualifier Name: $1",
		  request->nameSpace.getString(),
		  request->qualifierDeclaration.getName().getString());
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

/**$*******************************************************
    handleDeleteQualifierRequest
**********************************************************/

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


      Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
		  "CIMOperationRequestDispatcher::handleDeleteQualifierRequest - Name Space: $0  Qualifier Name: $1",
		  request->nameSpace.getString(),
		  request->qualifierName.getString());
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

/**$*******************************************************
    handleEnumerateQualifiersRequest
**********************************************************/

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

      Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
		  "CIMOperationRequestDispatcher::handleEnumerateQualifiersRequest - Name Space: $0",
		  request->nameSpace.getString());
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

/**$*******************************************************
    handleExecQueryRequest
**********************************************************/

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

/**$*******************************************************
    handleInvokeMethodRequest
**********************************************************/

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
	  Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
		      "CIMOperationRequestDispatcher::handleInvokeMethodRequest - CIM exception has occurred.");
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
       // map CIM_ERR_INVALID_CLASS to CIM_ERR_NOT_FOUND
       if (checkClassException.getCode() == CIM_ERR_INVALID_CLASS)
       {
           checkClassException = PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_FOUND,
                                                       className.getString());
       }

       Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
		   "CIMOperationRequestDispatcher::handleInvokeMethodRequest - CIM exist exception has occurred.  Name Space: $0  Class Name: $1",
		   request->nameSpace.getString(),
		   className.getString());
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

      _forwardRequestToProviderManager(className, serviceName, controlProviderName,
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

   // l10n
   
   CIMException cimException =
     PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED, MessageLoaderParms("Server.CIMOperationRequestDispatcher.PROVIDER_NOT_AVAILABLE","Provider not available"));

   // CIMException cimException =
   // PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, "Provider not available");

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

	 // l10n 

	 // throw PEGASUS_CIM_EXCEPTION(
	 // CIM_ERR_INVALID_PARAMETER,
	 // String("Malformed ") + cimTypeToString (type) + " value");

	 throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_INVALID_PARAMETER, MessageLoaderParms("Server.CIMOperationRequestDispatcher.CIM_ERR_INVALID_PARAMETER",
									"Malformed $0 value", cimTypeToString (type)));

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

	 // l10n

	 // throw PEGASUS_CIM_EXCEPTION(
	 // CIM_ERR_INVALID_PARAMETER,
	 // String("Malformed ") + cimTypeToString (type) + " value");

	 throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_INVALID_PARAMETER, MessageLoaderParms("Server.CIMOperationRequestDispatcher.CIM_ERR_INVALID_PARAMETER",
			       "Malformed $0 value", cimTypeToString (type)));

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

		    Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
				"CIMOperationRequestDispatcher::_fixInvokeMethodParameterTypes - Name Space: $0  Class Name: $1",
				request->nameSpace.getString(),
				request->instanceName.getClassName().getString());
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

      Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
		  "CIMOperationRequestDispatcher::_fixSetPropertyValueType - Name Space: $0  Class Name: $1",
		  request->nameSpace.getString(),
		  request->instanceName.getClassName().getString());
   }
   catch (CIMException& exception)
   {
      _repository->read_unlock();
      PEG_METHOD_EXIT();

      // map CIM_ERR_NOT_FOUND to CIM_ERR_INVALID_CLASS
      if (exception.getCode() == CIM_ERR_NOT_FOUND)
      {
         throw PEGASUS_CIM_EXCEPTION(CIM_ERR_INVALID_CLASS,
                  request->instanceName.getClassName().getString());
      }
      else
      {
         throw exception;
      }
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

      Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
		  "CIMOperationRequestDispatcher::_checkExistenceOfClass - Name Space: $0  Class Name: $1",
		  nameSpace.getString(),
		  className.getString());
   }
   catch(CIMException& exception)
   {
      // map CIM_ERR_NOT_FOUND to CIM_ERR_INVALID_CLASS
      if (exception.getCode() == CIM_ERR_NOT_FOUND)
      {
         cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_INVALID_CLASS,
                                              className.getString());
      }
      else
      {
         cimException = exception;
      }
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
