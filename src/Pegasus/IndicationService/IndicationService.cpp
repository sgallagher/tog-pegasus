//%/////////////////////////////////////////////////////////////////////////////
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
// Author: Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
//
// Modified By:  Carol Ann Krug Graves, Hewlett-Packard Company 
//               (carolann_graves@hp.com)
//
// Modified By:  Ben Heilbronn, Hewlett-Packard Company
//               (ben_heilbronn@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMDateTime.h>
#include <Pegasus/Common/CIMProperty.h>
#include <Pegasus/Common/MessageQueue.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Repository/CIMRepository.h>
#include <Pegasus/Provider/OperationFlag.h>
#include <Pegasus/WQL/WQLParser.h>
#include <Pegasus/WQL/WQLSelectStatement.h>

#include "IndicationService.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

/**
    The name of the indication subscription class
 */
const char   IndicationService::_CLASS_SUBSCRIPTION []  = 
                 "CIM_IndicationSubscription";

/**
    The name of the indication filter class
 */
const char   IndicationService::_CLASS_FILTER []        = 
                 "CIM_IndicationFilter";

/**
    The name of the indication handler class
 */
const char   IndicationService::_CLASS_HANDLER []       = 
                 "CIM_IndicationHandler";

/**
    The name of the CIMXML Indication Handler class
 */
const char   IndicationService::_CLASS_HANDLERCIMXML [] = 
                 "CIM_IndicationHandlerCIMXML";

/**
    The name of the SNMP Indication Handler class
 */
const char   IndicationService::_CLASS_HANDLERSNMP []   = 
                 "CIM_IndicationHandlerSNMP";

/**
    The name of the Indication class
 */
const char   IndicationService::_CLASS_INDICATION []    = "CIM_Indication";

/**
    The name of the CIMOM Shutdown alert indication class
 */
//
//  ATTN: Update once CimomShutdownAlertIndication has been defined
//
const char   IndicationService::_CLASS_CIMOM_SHUTDOWN_ALERT [] =
                 "CIM_AlertIndication";

/**
    The name of the No Provider alert indication class
 */
//
//  ATTN: Update once NoProviderAlertIndication has been defined
//
const char   IndicationService::_CLASS_NO_PROVIDER_ALERT [] =
                 "CIM_AlertIndication";

/**
    The name of the CIMOM shutdown alert indication class
 */
//
//  ATTN: Update once ProviderTerminatedAlertIndication has been defined
//
const char   IndicationService::_CLASS_PROVIDER_TERMINATED_ALERT [] =
                 "CIM_AlertIndication";

/**
    The name of the Provider Capabilities class
 */
const char   IndicationService::_CLASS_PROVIDER_CAPABILITIES [] = 
                 "CIM_ProviderCapabilities";

/**
    The name of the filter reference property for indication subscription class
 */
const char   IndicationService::_PROPERTY_FILTER []     = "Filter";

/**
    The name of the handler reference property for indication subscription class
 */
const char   IndicationService::_PROPERTY_HANDLER []     = "Handler";

/**
    The name of the subscription state property for indication subscription 
    class
 */
const char   IndicationService::_PROPERTY_STATE []      = 
                 "SubscriptionState";

/**
    The name of the Other Subscription State property for Indication 
    Subscription class
 */
const char   IndicationService::_PROPERTY_OTHERSTATE [] = 
                 "OtherSubscriptionState";

/**
    The name of the repeat notification policy property for indication
    subscription class
 */
const char   IndicationService::_PROPERTY_REPEATNOTIFICATIONPOLICY [] =
                 "RepeatNotificationPolicy";

/**
    The name of the other repeat notification policy property for
    indication subscription class
 */
const char   IndicationService::_PROPERTY_OTHERREPEATNOTIFICATIONPOLICY []
                 = "OtherRepeatNotificationPolicy";

/**
    The name of the repeat notification interval property for indication
    subscription class
 */
const char   IndicationService::_PROPERTY_REPEATNOTIFICATIONINTERVAL [] =
                 "RepeatNotificationInterval";

/**
    The name of the repeat notification gap property for indication
    subscription class
 */
const char   IndicationService::_PROPERTY_REPEATNOTIFICATIONGAP [] =
                 "RepeatNotificationGap";

/**
    The name of the repeat notification count property for indication
    subscription class
 */
const char   IndicationService::_PROPERTY_REPEATNOTIFICATIONCOUNT [] =
                 "RepeatNotificationCount";

/**
    The name of the On Fatal Error Policy property for Indication Subscription 
    class
 */
const char   IndicationService::_PROPERTY_ONFATALERRORPOLICY [] = 
                 "OnFatalErrorPolicy";

/**
    The name of the Other On Fatal Error Policy property for Indication 
    Subscription class
 */
const char   IndicationService::_PROPERTY_OTHERONFATALERRORPOLICY [] = 
                 "OtherOnFatalErrorPolicy";

/**
    The name of the query property for indication filter class
 */
const char   IndicationService::_PROPERTY_QUERY []      = "Query";

/**
    The name of the query language property for indication filter class
 */
const char   IndicationService::_PROPERTY_QUERYLANGUAGE [] = 
                 "QueryLanguage";

/**
    The name of the Source Namespace property for indication filter class
 */
const char   IndicationService::_PROPERTY_SOURCENAMESPACE [] = 
                 "SourceNamespace";

/**
    The name of the name property for indication filter and indications handler     classes
 */
const char   IndicationService::_PROPERTY_NAME []       = "Name";

/**
    The name of the creation class name property for indication filter and 
    indications handler classes
 */
const char   IndicationService::_PROPERTY_CREATIONCLASSNAME [] = 
             "CreationClassName";

/**
    The name of the system name property for indication filter and indications 
    handler classes
 */
const char   IndicationService::_PROPERTY_SYSTEMNAME [] = "SystemName";

/**
    The name of the system creation class name property for indication filter 
    and indications handler classes
 */
const char   IndicationService::_PROPERTY_SYSTEMCREATIONCLASSNAME [] = 
             "SystemCreationClassName";

/**
    The name of the Persistence Type property for Indication Handler class
 */
const char   IndicationService::_PROPERTY_PERSISTENCETYPE [] = 
                 "PersistenceType";

/**
    The name of the Other Persistence Type property for Indication Handler 
    class
 */
const char   IndicationService::_PROPERTY_OTHERPERSISTENCETYPE [] = 
                 "OtherPersistenceType";

/**
    The name of the Destination property for Indication Handler subclasses
 */
const char   IndicationService::_PROPERTY_DESTINATION [] = "Destination";

/**
    The name of the SNMP Type property for SNMP Indication Handler class
 */
const char   IndicationService::_PROPERTY_SNMPTYPE [] = "snmpType";

/**
    The name of the Alert Type property for Alert Indication class
 */
const char   IndicationService::_PROPERTY_ALERTTYPE [] = "AlertType";

/**
    The name of the Other Alert Type property for Alert Indication class
 */
const char   IndicationService::_PROPERTY_OTHERALERTTYPE [] = 
             "OtherAlertType";

/**
    The name of the Perceived Severity property for Alert Indication class
 */
const char   IndicationService::_PROPERTY_PERCEIVEDSEVERITY [] = 
             "PerceivedSeverity";

/**
    The name of the Probable Cause property for Alert Indication class
 */
const char   IndicationService::_PROPERTY_PROBABLECAUSE [] = 
             "ProbableCause";

/**
    The name of the Provider Name property for Provider Capabilities class
 */
const char   IndicationService::_PROPERTY_PROVIDER_NAME [] =
             "ProviderName";

/**
    The name of the Class Name property for Provider Capabilities class
 */
const char   IndicationService::_PROPERTY_CLASS_NAME [] =
             "ClassName";

/**
    The name of the Supported Properties property for Provider Capabilities
    class
 */
const char   IndicationService::_PROPERTY_SUPPORTED_PROPERTIES [] =
             "SupportedProperties";

/**
    The name of the Provider Type property for Provider Capabilities class
 */
const char   IndicationService::_PROPERTY_PROVIDER_TYPE [] =
             "ProviderType";

/**
    The name of the Creator property for a class
 */
const char   IndicationService::_PROPERTY_CREATOR [] = 
             "Creator";

/**
    The integer representing the Indication value for the Provider Type
    property of the Provider Capabilities class
 */
const Uint16 IndicationService::_VALUE_INDICATION       = 4;

/**
    The WHERE keyword in WQL
 */
const char   IndicationService::_QUERY_WHERE []         = "WHERE";

/**
    The string representing the asterisk all properties symbol in WQL
 */
const char   IndicationService::_QUERY_ALLPROPERTIES [] = "*";

/**
    The string identifying the service name of the Provider Manager Service
 */
const char IndicationService::_SERVICE_PROVIDERMANAGER [] = 
               "Server::ProviderManagerService";

/**
    The string identifying the service name of the Repository Service 
 */
const char IndicationService::_SERVICE_REPOSITORY [] = 
               "Server::RepositoryService";

/**
    The string identifying the service name of the Handler Manager Service 
 */
const char IndicationService::_SERVICE_HANDLERMANAGER [] = 
               "IndicationHandlerService";


IndicationService::IndicationService (CIMRepository* repository)
    : Base ("Server::IndicationService", MessageQueue::getNextQueueId ()),
         _repository (repository)
{
    //
    //  Initialize
    //
    _initialize ();
}

IndicationService::~IndicationService (void)
{
}

void IndicationService::_handle_async_request(AsyncRequest *req)
{
    if ( req->getType() == async_messages::CIMSERVICE_STOP )
    {
        req->op->processing();

        //
        //  Call _terminate
        //
        _terminate ();

        handle_CimServiceStop(static_cast<CimServiceStop *>(req));
    }
    if (req->getType () == async_messages::CIMSERVICE_START)
    {
        cout << "CIMSERVICE_START message received" << endl;
        req->op->processing ();

        //
        //  Call _initialize ();
        //
        _initialize ();

        handle_CimServiceStart (static_cast <CimServiceStart *> (req));
    }
    else if ( req->getType() == async_messages::ASYNC_LEGACY_OP_START )
    {
        req->op->processing();
        Message *legacy = (static_cast<AsyncLegacyOperationStart *>(req)->act);
        if (false == handleEnqueue(legacy))
            _make_response(req, async_results::CIM_NAK);
        return;
    }
    else
        Base::_handle_async_request(req);
}

Boolean IndicationService::handleEnqueue(Message* message)
{
    Boolean ret = true;

    switch(message->getType())
    {
	case CIM_GET_INSTANCE_REQUEST_MESSAGE:
	    _handleGetInstanceRequest(message);
	    break;

	case CIM_ENUMERATE_INSTANCES_REQUEST_MESSAGE:
	    _handleEnumerateInstancesRequest(message);
	    break;

	case CIM_ENUMERATE_INSTANCE_NAMES_REQUEST_MESSAGE:
	    _handleEnumerateInstanceNamesRequest(message);
	    break;

	case CIM_CREATE_INSTANCE_REQUEST_MESSAGE:
	    _handleCreateInstanceRequest(message);
	    break;

	case CIM_MODIFY_INSTANCE_REQUEST_MESSAGE:
	    _handleModifyInstanceRequest(message);
	    break;

	case CIM_DELETE_INSTANCE_REQUEST_MESSAGE:
	    _handleDeleteInstanceRequest(message);
	    break;

        case CIM_PROCESS_INDICATION_REQUEST_MESSAGE:
            _handleProcessIndicationRequest(message);
            break;

	case CIM_NOTIFY_PROVIDER_REGISTRATION_REQUEST_MESSAGE:
	    _handleNotifyProviderRegistrationRequest(message);    
	    break;

	case CIM_NOTIFY_PROVIDER_TERMINATION_REQUEST_MESSAGE:
	    _handleNotifyProviderTerminationRequest(message);    
	    break;

	default:
	    // do nothing, unsupported message?
            ret = false;
	    break;
    }

    delete message;
    return ret;
}

void IndicationService::handleEnqueue(void)
{
    Message * message = dequeue();

    PEGASUS_ASSERT(message != 0);
    handleEnqueue(message);
}

void IndicationService::_initialize (void)
{
    Array <struct SubscriptionRef> activeSubscriptions;
    String filterQuery;
    WQLSelectStatement selectStatement;
    String indicationClassName;
    String condition;
    String queryLanguage;
    Array <String> indicationSubclasses;
    CIMPropertyList propertyList;
    Array <struct ProviderClassList> indicationProviders;
    Array <struct SubscriptionRef> noProviderSubscriptions;
    Array <String> startProviders;
    Boolean duplicate;
    CIMInstance indicationInstance;

    const char METHOD_NAME [] = "IndicationService::_initialize";

    PEG_FUNC_ENTER (TRC_INDICATION_SERVICE, METHOD_NAME);


    //
    //  Get list of namespaces in repository
    //
    Array <String> nameSpaceNames = _repository->enumerateNameSpaces ();

    //
    //  Check for subscriptions in each namespace in the repository
    //
    for (Uint8 i = 0; i < nameSpaceNames.size (); i++)
    {
        //
        //  Make sure subscription classes include Creator property
        //
        try
        {
            CIMClass subscriptionClass = _repository->getClass 
                (nameSpaceNames [i], _CLASS_SUBSCRIPTION);
            if (!subscriptionClass.existsProperty (_PROPERTY_CREATOR))
            {
                subscriptionClass.addProperty (CIMProperty (_PROPERTY_CREATOR,
                    CIMValue (String::EMPTY)));
                _repository->modifyClass (nameSpaceNames [i], 
                    subscriptionClass);
            }
            CIMClass filterClass = _repository->getClass (nameSpaceNames [i], 
                _CLASS_FILTER);
            if (!filterClass.existsProperty (_PROPERTY_CREATOR))
            {
                filterClass.addProperty (CIMProperty (_PROPERTY_CREATOR,
                    CIMValue (String::EMPTY)));
                _repository->modifyClass (nameSpaceNames [i], filterClass);
            }
            CIMClass cimxmlHandlerClass = _repository->getClass 
                (nameSpaceNames [i], _CLASS_HANDLERCIMXML);
            if (!cimxmlHandlerClass.existsProperty (_PROPERTY_CREATOR))
            {
                cimxmlHandlerClass.addProperty (CIMProperty (_PROPERTY_CREATOR,
                    CIMValue (String::EMPTY)));
                _repository->modifyClass (nameSpaceNames [i], 
                    cimxmlHandlerClass);
            }
            CIMClass snmpHandlerClass = _repository->getClass 
                (nameSpaceNames [i], _CLASS_HANDLERSNMP);
            if (!snmpHandlerClass.existsProperty (_PROPERTY_CREATOR))
            {
                snmpHandlerClass.addProperty (CIMProperty (_PROPERTY_CREATOR,
                    CIMValue (String::EMPTY)));
                _repository->modifyClass (nameSpaceNames [i], snmpHandlerClass);
            }
        }
        catch (CIMException e)
        {
            //
            //  Some namespaces may not include the subscription class
            //  In that case, just continue with the next namespace
            //
            if (e.getCode () == CIM_ERR_INVALID_CLASS)
            {
                continue;
            }
            else
            {
                PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
                throw e;
            }
        }
    }

    //
    //  Get existing active subscriptions from each namespace in the repository
    //
    activeSubscriptions = _getActiveSubscriptions ();
    noProviderSubscriptions.clear ();

    for (Uint8 i = 0; i < activeSubscriptions.size (); i++)
    {
        //
        //  Get filter query
        //
        filterQuery = _getFilterQuery (activeSubscriptions [i].subscription,
            activeSubscriptions [i].nameSpaceName);
        selectStatement = _getSelectStatement (filterQuery);
    
        //
        //  Get indication class name from filter query (FROM clause)
        //
        indicationClassName = _getIndicationClassName (selectStatement,
            activeSubscriptions [i].nameSpaceName);
    
        //
        //  Get list of subclass names for indication class
        //
        indicationSubclasses = _repository->enumerateClassNames 
            (activeSubscriptions [i].nameSpaceName, indicationClassName, true);
        indicationSubclasses.append (indicationClassName);
    
        //
        //  Get property list from filter query (FROM and WHERE 
        //  clauses)
        //
        if ((selectStatement.getSelectPropertyNameCount () > 0) ||
            (selectStatement.getWherePropertyNameCount () > 0))
        {
            propertyList = _getPropertyList (selectStatement);
        }

        //
        //  Get indication provider class lists
        //
        indicationProviders = _getIndicationProviders 
            (activeSubscriptions [i].nameSpaceName, indicationClassName,
             indicationSubclasses, propertyList);

        if (indicationProviders.size () == 0)
        {
            //
            //  There are no providers that can support this subscription
            //  Append this subscription to no provider list
            //
            noProviderSubscriptions.append (activeSubscriptions [i]);
            continue;
        }
    
        //
        //  Get condition from filter query (WHERE clause)
        //
        if (selectStatement.hasWhereClause ())
        {
            condition = _getCondition (filterQuery);
        }

        //
        //  Get filter query language
        //
        queryLanguage = _getFilterQueryLanguage 
            (activeSubscriptions [i].subscription,
             activeSubscriptions [i].nameSpaceName);
    
        //
        //  Send enable request message to each provider
        //
        _sendEnableRequests (indicationProviders, 
            activeSubscriptions [i].nameSpaceName,
            propertyList, condition, queryLanguage,
            activeSubscriptions [i].subscription);

        //
        //  Merge provider list into list of unique providers to start
        //
        for (Uint8 j = 0; j < indicationProviders.size (); j++)
        {
            duplicate = false;
            for (Uint8 k = 0; k < startProviders.size () && !duplicate; k++)
            {
                if (indicationProviders [j].providerName == startProviders [k])
                {
                    duplicate = true;
                }
            }
    
            if (!duplicate)
            {
                startProviders.append (indicationProviders [j].providerName);
            }
        }
    }  // for each subscription

    //
    //  Send alerts for any subscriptions for which there is no longer any
    //  provider
    //
    if (noProviderSubscriptions.size () > 0)
    {
        //
        //  Send NoProviderAlertIndication to handler instances
        //  ATTN: NoProviderAlertIndication must be defined
        //
        indicationInstance = _createAlertInstance
            (_CLASS_NO_PROVIDER_ALERT, noProviderSubscriptions);

        _sendAlerts (noProviderSubscriptions, indicationInstance);
    }

    //
    //  Send start message to each provider
    //
    for (Uint8 m = 0; m < startProviders.size (); m++)
    {
        //
        //  ATTN: start message has not yet been defined
        //
    }

    //
    //  Find required services
    //
    Array <Uint32> pmservices;
    find_services (_SERVICE_PROVIDERMANAGER, 0, 0, &pmservices);
    pegasus_yield ();
    cout << "Found " << pmservices.size () << " Provider Manager Services" 
         << endl;
    if (pmservices.size () > 0)
    {
        _providerManager = pmservices [0];
        cout << "Found Provider Manager Service at: " << pmservices [0] << endl;
    }
    else
    {
        cout << "Provider Manager Service not found" << endl;
    }

    //Array <Uint32> rservices;
    //find_services (_SERVICE_REPOSITORY, 0, 0, &rservices);
    //pegasus_yield ();
    //_repository = rservices [0];
    //cout << "Found Repository Service at: " << rservices [0] << endl;

    Array <Uint32> hmservices;
    find_services (_SERVICE_HANDLERMANAGER, 0, 0, &hmservices);
    pegasus_yield ();
    cout << "Found " << hmservices.size () << " Handler Manager Services" 
         << endl;
    if (hmservices.size () > 0)
    {
        _handlerService = hmservices [0];
        cout << "Found Handler Manager Service at: " << hmservices [0] << endl;
    }
    else
    {
        cout << "Handler Manager Service not found" << endl;
    }

    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
}

void IndicationService::_terminate (void)
{
    Array <struct SubscriptionRef> activeSubscriptions;
    CIMInstance indicationInstance;

    const char METHOD_NAME [] = "IndicationService::_terminate";

    PEG_FUNC_ENTER (TRC_INDICATION_SERVICE, METHOD_NAME);

    //
    //  Get existing active subscriptions from each namespace in the repository
    //
    activeSubscriptions = _getActiveSubscriptions ();

    //
    //  Create CimomShutdownAlertIndication instance
    //  ATTN: CimomShutdownAlertIndication must be defined
    //
    indicationInstance = _createAlertInstance (_CLASS_CIMOM_SHUTDOWN_ALERT,
        activeSubscriptions);

    //
    //  Send CimomShutdownAlertIndication to each unique handler instance
    //
    _sendAlerts (activeSubscriptions, indicationInstance);

    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
}

void IndicationService::_handleCreateInstanceRequest (const Message * message)
{
    const char METHOD_NAME [] = 
	"IndicationService::_handleCreateInstanceRequest";

    PEG_FUNC_ENTER (TRC_INDICATION_SERVICE, METHOD_NAME);

    CIMCreateInstanceRequestMessage* request = 
	(CIMCreateInstanceRequestMessage*) message;

    CIMStatusCode errorCode = CIM_ERR_SUCCESS;
    String errorDescription;

    CIMReference instanceRef;

    CIMValue subscriptionStateValue;
    Uint16 subscriptionState;
    CIMValue repeatPolicyValue;
    Uint16 repeatNotificationPolicy;
    CIMValue errorPolicyValue;
    Uint16 onFatalErrorPolicy;
    CIMValue persistenceValue;
    Uint16 persistenceType;

    CIMInstance instance = request->newInstance.clone ();

    try
    {
        // REVIEW: Derived classes of CIM_IndicationSubscription not
        // handled. It is reasonable for a user to derive from this
        // class and add extra properties.

        // REVIEW: how does the provider manager know to forward
        // requests to this service? Is it by class name? If so,
        // shouldn't the provider use an is-a operator on the new
        // class?

        //
        //  Check all required properties exist
        //  For a property that has a default value, if it does not exist,
        //  add property with default value
        //
        if (instance.getClassName () == _CLASS_SUBSCRIPTION)
        {
            //
            //  Filter and Handler are key properties for Subscription
            //  No other properties are required
            //
            if (!instance.existsProperty (_PROPERTY_FILTER))
            {
                PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
                throw CIMException (CIM_ERR_INVALID_PARAMETER);
            }
    
            if (!instance.existsProperty (_PROPERTY_HANDLER))
            {
                PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
                throw CIMException (CIM_ERR_INVALID_PARAMETER);
            }

            //
            //  Default value for Subscription State is Enabled
            //
            if (!instance.existsProperty (_PROPERTY_STATE))
            {
                instance.addProperty (CIMProperty (_PROPERTY_STATE,
                    CIMValue ((Uint16) _STATE_ENABLED)));
            }
            else
            {
                //
                //  Get subscription state
                //
                subscriptionStateValue = instance.getProperty
                    (instance.findProperty (_PROPERTY_STATE)).getValue ();
                subscriptionStateValue.get (subscriptionState);

                //
                //  If Subscription State is Other, Other Subscription State
                //  property must exist
                //
                if ((subscriptionState == _STATE_OTHER) &&
                    (!instance.existsProperty (_PROPERTY_OTHERSTATE)))
                {
                    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
                    throw CIMException (CIM_ERR_INVALID_PARAMETER);
                }

                //
                //  If Subscription State is not Other, 
                //  Other Subscription State property must not exist
                //
                else if (instance.existsProperty (_PROPERTY_OTHERSTATE))
                {
                    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
                    throw CIMException (CIM_ERR_INVALID_PARAMETER);
                }
            }

            //
            //  Default value for Repeat Notification Policy is None
            //
            if (!instance.existsProperty (_PROPERTY_REPEATNOTIFICATIONPOLICY))
            {
                instance.addProperty (CIMProperty 
                    (_PROPERTY_REPEATNOTIFICATIONPOLICY, 
                    CIMValue ((Uint16) _POLICY_NONE)));
            }

            else
            {
                //
                //  Get Repeat Notification Policy
                //
                repeatPolicyValue = instance.getProperty (instance.findProperty
                    (_PROPERTY_REPEATNOTIFICATIONPOLICY)).getValue ();
                repeatPolicyValue.get (repeatNotificationPolicy);

                //
                //  If Repeat Notification Policy is Other, 
                //  Other Repeat Notification Policy property must exist
                //
                if ((repeatNotificationPolicy == _POLICY_OTHER) &&
                    (!instance.existsProperty 
                    (_PROPERTY_OTHERREPEATNOTIFICATIONPOLICY)))
                {
                    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
                    throw CIMException (CIM_ERR_INVALID_PARAMETER);
                }

                //
                //  If Repeat Notification Policy is not Other, 
                //  Other Repeat Notification Policy property must not exist
                //
                else if (instance.existsProperty 
                    (_PROPERTY_OTHERREPEATNOTIFICATIONPOLICY))
                {
                    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
                    throw CIMException (CIM_ERR_INVALID_PARAMETER);
                }
            }

            //
            //  Default value for On Fatal Error Policy is Ignore
            //
            if (!instance.existsProperty (_PROPERTY_ONFATALERRORPOLICY))
            {
                instance.addProperty (CIMProperty 
                    (_PROPERTY_ONFATALERRORPOLICY, 
                    CIMValue ((Uint16) _ERRORPOLICY_IGNORE)));
            }
            else
            {
                //
                //  Get On Fatal Error Policy
                //
                errorPolicyValue = instance.getProperty (instance.findProperty 
                    (_PROPERTY_ONFATALERRORPOLICY)).getValue ();
                errorPolicyValue.get (onFatalErrorPolicy);

                //
                //  If On Fatal Error Policy is Other, Other On Fatal Error 
                //  Policy property must exist
                //
                if ((onFatalErrorPolicy == _ERRORPOLICY_OTHER) &&
                    (!instance.existsProperty 
                    (_PROPERTY_OTHERONFATALERRORPOLICY)))
                {
                    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
                    throw CIMException (CIM_ERR_INVALID_PARAMETER);
                }

                //
                //  If On Fatal Error Policy is not Other, 
                //  Other On Fatal Error Policy property must not exist
                //
                else if (instance.existsProperty 
                    (_PROPERTY_OTHERONFATALERRORPOLICY))
                {
                    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
                    throw CIMException (CIM_ERR_INVALID_PARAMETER);
                }
            }
        } 
        else  // Filter or Handler
        {
            //
            //  Name, CreationClassName, SystemName, and SystemCreationClassName
            //  are key properties for Filter and Handler  
            //  CreationClassName and Name must exist
            //  If others do not exist, add and set to default
            //
            if (!instance.existsProperty (_PROPERTY_NAME))
            {
                PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
                throw CIMException (CIM_ERR_INVALID_PARAMETER);
            }
    
            if (!instance.existsProperty (_PROPERTY_CREATIONCLASSNAME))
            {
                PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
                throw CIMException (CIM_ERR_INVALID_PARAMETER);
            }
    
            if (!instance.existsProperty (_PROPERTY_SYSTEMNAME))
            {
                instance.addProperty (CIMProperty (_PROPERTY_SYSTEMNAME,
                    System::getHostName ()));
            }
    
            if (!instance.existsProperty (_PROPERTY_SYSTEMCREATIONCLASSNAME))
            {
                //
                //  ATTN: how is System Creation Class Name determined?
                //
                instance.addProperty (CIMProperty 
                    (_PROPERTY_SYSTEMCREATIONCLASSNAME, 
                    "CIM_UnitaryComputerSystem"));
            }
    
            if (instance.getClassName () == _CLASS_FILTER)
            {
                //
                //  Query and QueryLanguage properties are required for Filter
                //
                if (!instance.existsProperty (_PROPERTY_QUERY))
                {
                    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
                    throw CIMException (CIM_ERR_INVALID_PARAMETER);
                }
                if (!instance.existsProperty (_PROPERTY_QUERYLANGUAGE))
                {
                    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
                    throw CIMException (CIM_ERR_INVALID_PARAMETER);
                }

                //
                //  Default value for Source Namespace is the namespace of the
                //  Filter registration
                //
                if (!instance.existsProperty (_PROPERTY_SOURCENAMESPACE))
                {
                    instance.addProperty (CIMProperty 
                        (_PROPERTY_SOURCENAMESPACE, request->nameSpace));
                }
            }

            //
            //  Currently only two direct subclasses of Indication handler 
            //  class are supported -- further subclassing is not currently 
            //  supported
            //
            else if ((instance.getClassName () == _CLASS_HANDLERCIMXML) ||
                     (instance.getClassName () == _CLASS_HANDLERSNMP))
            {
                //
                //  Default value for Persistence Type is Permanent
                //
                if (!instance.existsProperty (_PROPERTY_PERSISTENCETYPE))
                {
                    instance.addProperty (CIMProperty 
                        (_PROPERTY_PERSISTENCETYPE, 
                        CIMValue ((Uint16) _PERSISTENCE_PERMANENT)));
                }
                else
                {
                    //
                    //  Get Persistence Type
                    //
                    persistenceValue = instance.getProperty 
                        (instance.findProperty 
                        (_PROPERTY_PERSISTENCETYPE)).getValue ();
                    persistenceValue.get (persistenceType);
    
                    //
                    //  If Persistence Type is Other, Other Persistence Type
                    //  property must exist
                    //
                    if ((persistenceType == _PERSISTENCE_OTHER) &&
                        (!instance.existsProperty 
                        (_PROPERTY_OTHERPERSISTENCETYPE)))
                    {
                        PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
                        throw CIMException (CIM_ERR_INVALID_PARAMETER);
                    }
    
                    //
                    //  If Persistence Type is not Other, 
                    //  Other Persistence Type property must not exist
                    //
                    else if (instance.existsProperty 
                        (_PROPERTY_OTHERPERSISTENCETYPE))
                    {
                        PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
                        throw CIMException (CIM_ERR_INVALID_PARAMETER);
                    }
                }

                //
                //  Destination property is required for CIMXML and SNMP 
                //  Handler subclasses
                //
                if (!instance.existsProperty (_PROPERTY_DESTINATION))
                {
                    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
                    throw CIMException (CIM_ERR_INVALID_PARAMETER);
                }

                if (instance.getClassName () == _CLASS_HANDLERSNMP)
                {
                    //
                    //  SNMP Type property is required for SNMP Handler
                    //
                    if (!instance.existsProperty (_PROPERTY_SNMPTYPE))
                    {
                        PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
                        throw CIMException (CIM_ERR_INVALID_PARAMETER);
                    }
                }
            }

            else
            {
                //
                //  A class not currently served by the Indication Service
                //
                PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
                throw CIMException (CIM_ERR_INVALID_PARAMETER);
            }
        }
    
        //
        //  Add creator property to Instance
        //  ATTN: how is current user obtained?
        //
        String currentUser = String::EMPTY;
        instance.addProperty (CIMProperty (_PROPERTY_CREATOR, currentUser));
    
        //
        //  Create instance in repository
        //
        _repository->write_lock ();
    
        instanceRef = _repository->createInstance (request->nameSpace, 
            instance);
    
        _repository->write_unlock ();
    
        //
        //  If the instance is of the CIM_IndicationSubscription class
        //  and subscription state is enabled, send enable request to 
        //  indication providers
        //
        if (instance.getClassName () == _CLASS_SUBSCRIPTION)
        {
            if ((subscriptionState == _STATE_ENABLED) ||
                (subscriptionState == _STATE_ENABLEDDEGRADED))
            {
                String filterQuery;
                WQLSelectStatement selectStatement;
                String indicationClassName;
                Array <String> indicationSubclasses;
                Array <struct ProviderClassList> indicationProviders;
        
                //
                //  Get filter query
                //
                filterQuery = _getFilterQuery (instance, request->nameSpace);
                selectStatement = _getSelectStatement (filterQuery);
            
                //
                //  Get indication class name from filter query (FROM clause)
                //
                indicationClassName = _getIndicationClassName (selectStatement,
                    request->nameSpace);
    
                //
                //  Get list of subclass names for indication class
                //
                indicationSubclasses = _repository->enumerateClassNames 
                    (request->nameSpace, indicationClassName, true);
                indicationSubclasses.append (indicationClassName);
            
                CIMPropertyList propertyList;
                String condition;
                String queryLanguage;
        
                //
                //  Get property list from filter query (FROM and WHERE 
                //  clauses)
                //
                if ((selectStatement.getSelectPropertyNameCount () > 0) ||
                    (selectStatement.getWherePropertyNameCount () > 0))
                {
                    propertyList = _getPropertyList (selectStatement);
                }
        
                //
                //  Get indication provider class lists
                //
                indicationProviders = _getIndicationProviders 
                    (request->nameSpace, indicationClassName, 
                    indicationSubclasses, propertyList);
        
                if (indicationProviders.size () == 0)
                {
                    //
                    //  There are no providers that can support this 
                    //  subscription
                    //
                    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
                    throw CIMException (CIM_ERR_NOT_SUPPORTED);
                }

                //
                //  Get condition from filter query (WHERE clause)
                //
                if (selectStatement.hasWhereClause ())
                {
                    condition = _getCondition (filterQuery);
                }
        
                //
                //  Get filter query language
                //
                queryLanguage = _getFilterQueryLanguage (instance, 
                    request->nameSpace);
            
                //
                //  Send enable request message to each provider
                //
                _sendEnableRequests (indicationProviders, request->nameSpace, 
                    propertyList, condition, queryLanguage, instance);
        
                //
                //  Send start message to each provider
                //
                for (Uint8 i = 0; i < indicationProviders.size (); i++)
                {
                    //
                    //  ATTN: start message has not yet been defined
                    //
                }
            }
        }
    }
    catch (CIMException & exception)
    {
        errorCode = exception.getCode ();
        errorDescription = exception.getMessage ();
    }
    catch (Exception & exception)
    {
        errorCode = CIM_ERR_FAILED;
        errorDescription = exception.getMessage ();
    }

    CIMCreateInstanceResponseMessage* response =
        new CIMCreateInstanceResponseMessage(
            request->messageId,
            errorCode,
            errorDescription,
            request->queueIds.copyAndPop(),
            instanceRef);

    _enqueueResponse(request, response);

    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
}

void IndicationService::_handleGetInstanceRequest (const Message* message)
{
    const char METHOD_NAME [] = 
	"IndicationService::_handleGetInstanceRequest";

    PEG_FUNC_ENTER (TRC_INDICATION_SERVICE, METHOD_NAME);

    CIMGetInstanceRequestMessage* request = 
	(CIMGetInstanceRequestMessage*) message;

    CIMStatusCode errorCode = CIM_ERR_SUCCESS;
    String errorDescription;
    CIMInstance instance;

    try
    {
	//
	//  Get instance from repository
	//
	instance = _repository->getInstance (request->nameSpace, 
            request->instanceName, request->localOnly, 
            request->includeQualifiers, request->includeClassOrigin, 
            request->propertyList);

	//
	//  Remove Creator property from instance before returning
	//
	instance.removeProperty (instance.findProperty (_PROPERTY_CREATOR));
    }
    catch (CIMException& exception)
    {
	errorCode = exception.getCode ();
	errorDescription = exception.getMessage ();
    }
    catch (Exception& exception)
    {
	errorCode = CIM_ERR_FAILED;
	errorDescription = exception.getMessage ();
    }

    CIMGetInstanceResponseMessage* response = new CIMGetInstanceResponseMessage
        (request->messageId,
	errorCode,
	errorDescription,
	request->queueIds.copyAndPop(),
	instance);

    _enqueueResponse(request, response);

    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
}

void IndicationService::_handleEnumerateInstancesRequest(const Message* message)
{
    const char METHOD_NAME [] = 
	"IndicationService::_handleEnumerateInstancesRequest";

    PEG_FUNC_ENTER (TRC_INDICATION_SERVICE, METHOD_NAME);

    CIMEnumerateInstancesRequestMessage* request = 
	(CIMEnumerateInstancesRequestMessage*) message;

    Array <CIMNamedInstance> enumInstances;

    CIMStatusCode errorCode = CIM_ERR_SUCCESS;
    String errorDescription;
    CIMInstance cimInstance;

    try
    {
	enumInstances = _repository->enumerateInstances (request->nameSpace, 
            request->className, request->deepInheritance, request->localOnly, 
            request->includeQualifiers, request->includeClassOrigin, 
            request->propertyList);
	
        //
        //  Remove Creator property from instances before returning
        //
        for (Uint8 i = 0; i < enumInstances.size (); i++)
        {
            enumInstances [i].getInstance ().removeProperty 
                (enumInstances [i].getInstance ().findProperty 
                (_PROPERTY_CREATOR));
        }
    }
    catch (CIMException& exception)
    {
	errorCode = exception.getCode ();
	errorDescription = exception.getMessage ();
//cout << "CIMException: " << exception.getMessage () << endl;
    }
    catch (Exception& exception)
    {
	errorCode = CIM_ERR_FAILED;
	errorDescription = exception.getMessage ();
//cout << "Exception: " << exception.getMessage () << endl;
    }

    CIMEnumerateInstancesResponseMessage* response = 
	new CIMEnumerateInstancesResponseMessage(
	    request->messageId,
	    errorCode,
	    errorDescription,
	    request->queueIds.copyAndPop(),
	    enumInstances);

    _enqueueResponse(request, response);

    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
}

void IndicationService::_handleEnumerateInstanceNamesRequest
    (const Message* message)
{
    PEG_METHOD_ENTER (TRC_INDICATION_SERVICE, 
            "IndicationService::_handleEnumerateInstanceNamesRequest");

    CIMEnumerateInstanceNamesRequestMessage* request =
	(CIMEnumerateInstanceNamesRequestMessage*) message;

    Array<CIMReference> enumInstanceNames;

    CIMStatusCode errorCode = CIM_ERR_SUCCESS;
    String errorDescription;

    try
    {
	enumInstanceNames = _repository->enumerateInstanceNames 
            (request->nameSpace, request->className);
    }
    catch (CIMException& exception)
    {
	errorCode = exception.getCode ();
	errorDescription = exception.getMessage ();
    }
    catch (Exception& exception)
    {
	errorCode = CIM_ERR_FAILED;
	errorDescription = exception.getMessage ();
    }

    CIMEnumerateInstanceNamesResponseMessage* response =
	new CIMEnumerateInstanceNamesResponseMessage(
	    request->messageId,
	    errorCode,
	    errorDescription,
	    request->queueIds.copyAndPop(),
	    enumInstanceNames);

    // preserve message key
    response->setKey(request->getKey());

    // lookup the message queue
    MessageQueue * queue = MessageQueue::lookup(request->queueIds.top());

    PEGASUS_ASSERT(queue != 0);

    // enqueue the response
    queue->enqueue(response);

//    _enqueueResponse(request, response);

    PEG_METHOD_EXIT ();
}

void IndicationService::_handleModifyInstanceRequest (const Message* message)
{
    const char METHOD_NAME [] = 
	"IndicationService::_handleModifyInstanceRequest";

    PEG_FUNC_ENTER (TRC_INDICATION_SERVICE, METHOD_NAME);

    CIMModifyInstanceRequestMessage* request = 
	(CIMModifyInstanceRequestMessage*) message;

    CIMStatusCode errorCode = CIM_ERR_SUCCESS;
    String errorDescription;
    
    try
    {
	//
	//  Get the instance name
	//
	CIMReference instanceReference = 
            request->modifiedInstance.getInstanceName ();
	
	//
	//  Currently, only modification allowed is of Subscription State 
        //  property in Subscription class
	//
	if (instanceReference.getClassName () != _CLASS_SUBSCRIPTION)
	{
	    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
	    throw CIMException (CIM_ERR_NOT_SUPPORTED);
	}

        //
        //  ATTN:  Need to get value of PARTIAL_INSTANCE flag from 
        //  operation context
        //
	/*if (!(flags & OperationFlag::PARTIAL_INSTANCE))
	{
	    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
	    throw CIMException (CIM_ERR_NOT_SUPPORTED);
	}*/

	if (request->includeQualifiers)
	{
	    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
	    throw CIMException (CIM_ERR_NOT_SUPPORTED);
	}

        //
        //  Request is invalid if property list is null, meaning all properties
        //  are to be updated
        //
	if (request->propertyList.isNull ())
	{
	    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
	    throw CIMException (CIM_ERR_NOT_SUPPORTED);
	}

        //
        //  Request is invalid if more than one property is specified
        //
	else if (request->propertyList.getNumProperties () > 1)
	{
	    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
	    throw CIMException (CIM_ERR_NOT_SUPPORTED);
	}

        //
        //  For request to be valid, exactly one property must be specified,
        //  and it must be the Subscription State property
        //
	else if ((request->propertyList.getNumProperties () == 1) &&
                 (request->propertyList.getPropertyName (0) != _PROPERTY_STATE))
	{
	    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
	    throw CIMException (CIM_ERR_NOT_SUPPORTED);
	}

	//
	//  Get creator from instance
	//
	CIMInstance instance = _repository->getInstance
	    (request->nameSpace, instanceReference);

	String creator = instance.getProperty (instance.findProperty 
	    (_PROPERTY_CREATOR)).getValue ().toString ();

//cout << "Creator property not found/removed" << endl;
	//
	//  ATTN: how is current user obtained?
	//
	String currentUser = String::EMPTY;

	//
	//  Current user must be privileged user or instance Creator to modify
	//
	if (/*(!System::isPrivilegedUser (currentUser)) && */ 
            (currentUser != creator))
	{
	    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
	    throw CIMException (CIM_ERR_ACCESS_DENIED);
	}

	if (request->propertyList.getNumProperties () > 0)
	{
	    //
	    //  Get current state from instance
	    //
	    CIMValue subscriptionStateValue;
	    Uint16 currentState;
	    subscriptionStateValue = instance.getProperty 
                (instance.findProperty (_PROPERTY_STATE)).getValue ();
	    subscriptionStateValue.get (currentState);
    
	    //
	    //  Modify the instance in the repository
	    //
            _repository->write_lock ();

	    _repository->modifyInstance (request->nameSpace,
                CIMNamedInstance (instanceReference, 
                request->modifiedInstance.getInstance ()), 
                request->includeQualifiers, request->propertyList);

           _repository->write_unlock ();

	    //
	    //  Get new state 
	    //
            Uint16 newState;
            subscriptionStateValue = 
                request->modifiedInstance.getInstance ().getProperty
                (request->modifiedInstance.getInstance ().findProperty
                (_PROPERTY_STATE)).getValue ();

            subscriptionStateValue.get (newState);
    
            if (((newState == _STATE_ENABLED) || 
                 (newState == _STATE_ENABLEDDEGRADED))
                && ((currentState != _STATE_ENABLED) && 
                    (currentState != _STATE_ENABLEDDEGRADED)))
            {
		//
		//  Subscription was previously not enabled but is now to be 
                //  enabled
		//
		String filterQuery;
		WQLSelectStatement selectStatement;
		String indicationClassName;
		Array <String> indicationSubclasses;
		Array <struct ProviderClassList> indicationProviders;
    
		//
		//  Get filter query
		//
		filterQuery = _getFilterQuery 
                    (instance, request->nameSpace);
		selectStatement = _getSelectStatement (filterQuery);
        
		//
		//  Get indication class name from filter query (FROM clause)
		//
		indicationClassName = _getIndicationClassName (selectStatement,
		    request->nameSpace);
        
		//
		//  Get list of subclass names for indication class
		//
		indicationSubclasses = _repository->enumerateClassNames 
		    (request->nameSpace, indicationClassName, true);
                indicationSubclasses.append (indicationClassName);
        
		CIMPropertyList properties;
		String condition;
		String queryLanguage;
    
		//
		//  Get property list from filter query (FROM and WHERE 
		//  clauses)
		//
		if ((selectStatement.getSelectPropertyNameCount () > 0) ||
		    (selectStatement.getWherePropertyNameCount () > 0))
		{
		    properties = _getPropertyList (selectStatement);
		}
    
		//
		//  Get indication provider class lists
		//
		indicationProviders = _getIndicationProviders 
		    (request->nameSpace, indicationClassName, 
                    indicationSubclasses, properties);
    
		if (indicationProviders.size () == 0)
		{
		    //
		    //  There are no providers that can support this 
                    //  subscription
		    //
		    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
		    throw CIMException (CIM_ERR_NOT_SUPPORTED);
		}
        
		//
		//  Get condition from filter query (WHERE clause)
		//
		if (selectStatement.hasWhereClause ())
		{
		    condition = _getCondition (filterQuery);
		}
    
		//
		//  Get filter query language
		//
		queryLanguage = _getFilterQueryLanguage (instance, 
                    request->nameSpace);
        
		_sendEnableRequests (indicationProviders, request->nameSpace, 
                    properties, condition, queryLanguage, instance);

		//
		//  Send start message to each provider
		//
		for (Uint8 i = 0; i < indicationProviders.size (); i++)
		{
		    //
		    //  ATTN: start message has not yet been defined
		    //
		}
	    }
	    else if ((newState == _STATE_DISABLED) &&
		     ((currentState == _STATE_ENABLED) ||
		      (currentState == _STATE_ENABLEDDEGRADED)))
	    {
		//
		//  Subscription was previously enabled but is now to be 
                //  disabled
		//
		String filterQuery;
		WQLSelectStatement selectStatement;
		String indicationClassName;
		Array <String> indicationSubclasses;
		Array <struct ProviderClassList> indicationProviders;

		//
		//  Get filter query
		//
		filterQuery = _getFilterQuery (instance, request->nameSpace);
		selectStatement = _getSelectStatement (filterQuery);
        
		//
		//  Get indication class name from filter query (FROM clause)
		//
		indicationClassName = _getIndicationClassName (selectStatement,
		    request->nameSpace);
        
		//
		//  Get list of subclass names for indication class
		//
		indicationSubclasses = _repository->enumerateClassNames 
		    (request->nameSpace, indicationClassName, true);
                indicationSubclasses.append (indicationClassName);
        
		CIMPropertyList properties;
    
		//
		//  Get property list from filter query (FROM and WHERE 
		//  clauses)
		//
		if ((selectStatement.getSelectPropertyNameCount () > 0) ||
		    (selectStatement.getWherePropertyNameCount () > 0))
		{
		    properties = _getPropertyList (selectStatement);
		}
    
		//
		//  Get indication provider class lists
		//
		indicationProviders = _getIndicationProviders 
		    (request->nameSpace, indicationClassName, 
                     indicationSubclasses, properties);
    
		//
		//  Send disable requests
		//
                if (indicationProviders.size () > 0)
                {
		    _sendDisableRequests (indicationProviders, 
                        request->nameSpace, instance);
                }
	    }
	}
    }
    catch (CIMException& exception)
    {
	errorCode = exception.getCode ();
	errorDescription = exception.getMessage ();
    }
    catch (Exception& exception)
    {
	errorCode = CIM_ERR_FAILED;
	errorDescription = exception.getMessage ();
    }

    CIMModifyInstanceResponseMessage* response =
	new CIMModifyInstanceResponseMessage(
	    request->messageId,
	    errorCode,
	    errorDescription,
	    request->queueIds.copyAndPop());

    _enqueueResponse(request, response);

    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
}

void IndicationService::_handleDeleteInstanceRequest (const Message* message)
{
    const char METHOD_NAME [] = 
	"IndicationService::_handleDeleteInstanceRequest";

    PEG_FUNC_ENTER (TRC_INDICATION_SERVICE, METHOD_NAME);

    CIMDeleteInstanceRequestMessage* request = 
	(CIMDeleteInstanceRequestMessage*) message;

    CIMStatusCode errorCode = CIM_ERR_SUCCESS;
    String errorDescription;

    try
    {
	//
	//  Get creator from instance
	//
	CIMInstance instance = _repository->getInstance
	    (request->nameSpace, request->instanceName);

	String creator = instance.getProperty (instance.findProperty 
	    (_PROPERTY_CREATOR)).getValue ().toString ();

	//
	//  ATTN: how is current user obtained?
	//
	String currentUser = String::EMPTY;

	//
	//  Current user must be privileged user or instance Creator to delete
	//
	if (/*(!System::isPrivilegedUser (currentUser) && */ 
            (currentUser != creator))
	{
	    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
	    throw CIMException (CIM_ERR_ACCESS_DENIED);
	}

	//
	//  Check if instance may be deleted -- a filter or handler instance 
	//  referenced by a subscription instance may not be deleted
	//
	if (_canDelete (request->instanceName, request->nameSpace))
	{
	    if (request->instanceName.getClassName () == _CLASS_SUBSCRIPTION)
	    {
		CIMInstance subscriptionInstance;
		String filterQuery;
		WQLSelectStatement selectStatement;
		String indicationClassName;
		Array <String> indicationSubclasses;
		Array <struct ProviderClassList> indicationProviders;

		subscriptionInstance = _repository->getInstance 
		    (request->nameSpace, request->instanceName);

		//
		//  Get filter query
		//
		filterQuery = _getFilterQuery (subscriptionInstance,
		    request->nameSpace);
		selectStatement = _getSelectStatement (filterQuery);
        
		//
		//  Get indication class name from filter query (FROM clause)
		//
		indicationClassName = _getIndicationClassName (selectStatement,
		    request->nameSpace);
        
		//
		//  Get list of subclass names for indication class
		//
		indicationSubclasses = _repository->enumerateClassNames 
		    (request->nameSpace, indicationClassName, true);
                indicationSubclasses.append (indicationClassName);

		CIMPropertyList properties;

		//
		//  Get property list from filter query (FROM and WHERE
		//  clauses)
		//
		if ((selectStatement.getSelectPropertyNameCount () > 0) ||
		    (selectStatement.getWherePropertyNameCount () > 0))
		{
		    properties = _getPropertyList (selectStatement);
		}
        
		//
		//  Get indication provider class lists
		//
		indicationProviders = _getIndicationProviders 
		    (request->nameSpace, indicationClassName, 
                     indicationSubclasses, properties);
    
		//
		//  Send disable requests
		//
		_sendDisableRequests (indicationProviders,
		    request->nameSpace, subscriptionInstance);
	    }

	    //
	    //  Delete instance from repository
	    //
            _repository->write_lock ();

	    _repository->deleteInstance (request->nameSpace, 
                request->instanceName);

            _repository->write_unlock ();
	}
	else
	{
	    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
	    throw CIMException(CIM_ERR_NOT_SUPPORTED);
	}
    }
    catch (CIMException& exception)
    {
	errorCode = exception.getCode ();
	errorDescription = exception.getMessage ();
    }
    catch (Exception& exception)
    {
	errorCode = CIM_ERR_FAILED;
	errorDescription = exception.getMessage ();
    }

    CIMDeleteInstanceResponseMessage* response =
	new CIMDeleteInstanceResponseMessage(
	    request->messageId,
	    errorCode,
	    errorDescription,
	    request->queueIds.copyAndPop());

    _enqueueResponse(request, response);

    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
}

void IndicationService::_handleProcessIndicationRequest (const Message* message)
{
    const char METHOD_NAME [] =
        "IndicationService::_handleProcessIndicationRequest";

    PEG_FUNC_ENTER (TRC_INDICATION_SERVICE, METHOD_NAME);

    CIMProcessIndicationRequestMessage* request =
        (CIMProcessIndicationRequestMessage*) message;

    CIMStatusCode errorCode = CIM_ERR_SUCCESS;
    String errorDescription;

    CIMHandleIndicationResponseMessage* response =
        new CIMHandleIndicationResponseMessage(
            request->messageId,
            errorCode,
            errorDescription,
            request->queueIds.copyAndPop());

    String filterQuery;
    Array<String> propertyList;
    Boolean match;

    Array <struct SubscriptionRef> matchedSubscriptions;
    struct HandlerRef handlerRef;

    WQLSelectStatement selectStatement;

    CIMInstance handler;
    CIMInstance indication = request->indicationInstance;
    
    try
    {
        WQLSimplePropertySource propertySource = _getPropertySourceFromInstance(
            indication);

        for (Uint8 i = 0; i < indication.getPropertyCount(); i++)
            propertyList.append(indication.getProperty(i).getName());

        matchedSubscriptions = _getMatchingSubscriptions(
            indication.getClassName(), CIMPropertyList(propertyList));

        for (Uint8 i = 0; i < matchedSubscriptions.size(); i++)
        {
            match = true;

            filterQuery = _getFilterQuery(
                matchedSubscriptions[i].subscription,
                matchedSubscriptions[i].nameSpaceName);

            selectStatement = _getSelectStatement (filterQuery);

            if (selectStatement.hasWhereClause())
            {
                if (!selectStatement.evaluateWhereClause(&propertySource))
                {
                    match = false;
                }
            }

            if (match)
            {
                 handlerRef = _getHandlerRef(matchedSubscriptions[i]);

                 CIMRequestMessage * handler_request =
                     new CIMHandleIndicationRequestMessage (
                         "1234",
                         request->nameSpace,
                         handlerRef.handler,
                         indication,
                         QueueIdStack(_handlerService, getQueueId()));
                
                 AsyncOpNode* op = this->get_op();

                 AsyncLegacyOperationStart *async_req = 
                    new AsyncLegacyOperationStart(
                     get_next_xid(),
                     op,
                     _handlerService,
                     handler_request,
                     _queueId);

                 AsyncReply *async_reply = SendWait(async_req);
                 response = reinterpret_cast<CIMHandleIndicationResponseMessage *>
                     ((static_cast<AsyncLegacyOperationResult *>(async_reply))->res);

                 delete async_req;
                 delete async_reply;
            }
        }
    }
    catch (CIMException& exception)
    {
        response->errorCode = exception.getCode();
        response->errorDescription = exception.getMessage();
    }
    catch (Exception& exception)
    {
        response->errorCode = CIM_ERR_FAILED;
        response->errorDescription = exception.getMessage();
    }

    _enqueueResponse(request, response);

    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
}

void IndicationService::_handleNotifyProviderRegistrationRequest
    (const Message* message)
{
    const char METHOD_NAME [] = 
        "IndicationService::_handleNotifyProviderRegistrationRequest";

    PEG_FUNC_ENTER (TRC_INDICATION_SERVICE, METHOD_NAME);
//cout << "IndicationService::_handleNotifyProviderRegistrationRequest" << endl;

    CIMNotifyProviderRegistrationRequestMessage* request = 
	(CIMNotifyProviderRegistrationRequestMessage*) message;

    CIMStatusCode errorCode = CIM_ERR_SUCCESS;
    String errorDescription;

    String providerName = request->providerName;
    String className = request->className;
    CIMPropertyList newPropertyNames = request->newPropertyNames;
    CIMPropertyList oldPropertyNames = request->oldPropertyNames;

    Array <struct SubscriptionRef> newSubscriptions;
    Array <struct SubscriptionRef> formerSubscriptions;
    Array <struct ProviderClassList> indicationProviders;
    struct ProviderClassList indicationProvider;

    newSubscriptions.clear ();
    formerSubscriptions.clear ();

    //
    //  Construct provider class list from input provider and class name
    //
    indicationProvider.providerName = providerName;
    indicationProvider.classList.append (className);
    indicationProviders.append (indicationProvider);

    switch (request->operation)
    {
        case OP_CREATE:
        {
//cout << "OP_CREATE" << endl;
            //
            //  Get matching subscriptions
            //
            newSubscriptions = _getMatchingSubscriptions (className, 
                newPropertyNames);

            break;
        }

        case OP_DELETE:
        {
//cout << "OP_DELETE" << endl;
            //
            //  Get matching subscriptions
            //
            formerSubscriptions = _getMatchingSubscriptions (className, 
                oldPropertyNames);

            break;
        }

        case OP_MODIFY:
        {
//cout << "OP_MODIFY" << endl;
            //
            //  Get lists of affected subscriptions
            //
            _getModifiedSubscriptions (className, newPropertyNames, 
                oldPropertyNames, newSubscriptions, formerSubscriptions);

            break;
        }
        default:
            //
            //  Error condition: operation not supported
            //
            PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
            throw CIMException (CIM_ERR_NOT_SUPPORTED);
            break;
    }  // switch

    if (newSubscriptions.size () > 0)
    {
        CIMPropertyList propertyList;
        String filterQuery;
        WQLSelectStatement selectStatement;
        String condition;
        String queryLanguage;

//cout << "newSubscriptions.size (): " << newSubscriptions.size () << endl;
        //
        //  Send enable request for each subscription that can newly be
        //  supported
        //
        for (Uint8 i = 0; i < newSubscriptions.size (); i++)
        {
            //
            //  Get filter query
            //
            filterQuery = _getFilterQuery (newSubscriptions [i].subscription,
                 newSubscriptions [i].nameSpaceName);
            selectStatement = _getSelectStatement (filterQuery);

            //
            //  Get propertyList
            //
            if ((selectStatement.getSelectPropertyNameCount () > 0) ||
                (selectStatement.getWherePropertyNameCount () > 0))
            {
                propertyList = _getPropertyList (selectStatement);
            }

            //
            //  Get condition from filter query (WHERE clause)
            //
            if (selectStatement.hasWhereClause ())
            {
                condition = _getCondition (filterQuery);
            }
                        
            //
            //  Get filter query language
            //
            queryLanguage = _getFilterQueryLanguage
                (newSubscriptions [i].subscription,
                 newSubscriptions [i].nameSpaceName);

            //
            //  Send enable request
            //
            _sendEnableRequests (indicationProviders,
                newSubscriptions [i].nameSpaceName, propertyList, condition, 
                queryLanguage, newSubscriptions [i].subscription);

            //
            //  Send start message to each provider
            //
            for (Uint8 j = 0; j < indicationProviders.size (); j++)
            {
                //
                //  ATTN: start message has not yet been defined
                //
            }
        }
    }

    if (formerSubscriptions.size () > 0)
    {
//cout << "formerSubscriptions.size (): " << formerSubscriptions.size () << endl;
        CIMInstance indicationInstance;

        //
        //  Send disable request for each subscription that can no longer 
        //  be supported
        //
        for (Uint8 i = 0; i < formerSubscriptions.size (); i++)
        {
            _sendDisableRequests (indicationProviders,
                formerSubscriptions [i].nameSpaceName,
                formerSubscriptions [i].subscription);
        }

        //
        //  ATTN: Should alert always be sent, or only in the case 
        //  that there are no other providers that can satisfy any
        //  of the subscription indication subclasses??
        //

        //
        //  Create NoProviderAlertIndication instance
        //  ATTN: NoProviderAlertIndication must be defined
        //
        indicationInstance = _createAlertInstance 
            (_CLASS_NO_PROVIDER_ALERT, formerSubscriptions);
    
        //
        //  Send NoProviderAlertIndication to each unique handler instance
        //
        _sendAlerts (formerSubscriptions, indicationInstance);
    }

    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
}

void IndicationService::_handleNotifyProviderTerminationRequest
    (const Message * message)
{
    Array <struct SubscriptionRef> providerSubscriptions;
    CIMInstance indicationInstance;

    const char METHOD_NAME [] = 
        "IndicationService::_handleNotifyProviderTermination";

    PEG_FUNC_ENTER (TRC_INDICATION_SERVICE, METHOD_NAME);

    CIMNotifyProviderTerminationRequestMessage* request = 
	(CIMNotifyProviderTerminationRequestMessage*) message;

    String providerName = request->providerName;

    //
    //  Get list of affected subscriptions
    //
    providerSubscriptions.clear ();
    providerSubscriptions = _getProviderSubscriptions (providerName);

    //
    //  ATTN: Should alert always be sent, or only in the case 
    //  that there are no other providers that can satisfy any
    //  of the subscription indication subclasses??
    //

    //
    //  Create ProviderTerminatedAlertIndication instance
    //  ATTN: ProviderTerminatedAlertIndication must be defined
    //
    indicationInstance = _createAlertInstance 
        (_CLASS_PROVIDER_TERMINATED_ALERT, providerSubscriptions);

    //
    //  Send ProviderTerminatedAlertIndication to each unique handler instance
    //
    _sendAlerts (providerSubscriptions, indicationInstance);

    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
}


Boolean IndicationService::_canDelete (
    const CIMReference & instanceReference,
    const String & nameSpace)
{
    String superClass;
    String propName;

    const char METHOD_NAME [] = "IndicationService::_canDelete";

    PEG_FUNC_ENTER (TRC_INDICATION_SERVICE, METHOD_NAME);

    //
    //  Get the class and superclass of the instance to be deleted
    //
    CIMClass refClass = _repository->getClass (nameSpace,
        instanceReference.getClassName());

    superClass = refClass.getSuperClassName();

    //
    //  If the class or superclass is Filter or Handler, check for 
    //  subscription instances referring to the instance to be deleted
    //
    if ((superClass == _CLASS_FILTER) ||
        (superClass == _CLASS_HANDLER) ||
        (instanceReference.getClassName() == _CLASS_FILTER) ||
        (instanceReference.getClassName() == _CLASS_HANDLER))
    {
        if ((superClass == _CLASS_FILTER) ||
            (instanceReference.getClassName() == _CLASS_FILTER))
        {
            propName = _PROPERTY_FILTER;
        }
        else if (superClass == _CLASS_HANDLER)
        {
            propName = _PROPERTY_HANDLER;
        }

        //
        //  Get the instance to be deleted from the respository
        //
        CIMInstance instance = _repository->getInstance (nameSpace, 
            instanceReference);

        //
        //  Get all the subscriptions from the respository
        //
        Array <CIMNamedInstance> instanceObjects = 
            _repository->enumerateInstances (nameSpace, _CLASS_SUBSCRIPTION);

        CIMValue propValue;

        //
        //  Check each subscription for a reference to the instance to be 
        //  deleted
        //
        for (Uint8 i = 0; i < instanceObjects.size(); i++)
        {
            //
            //  Get the subscription Filter or Handler property value
            //
            propValue = instanceObjects[i].getInstance().getProperty
                (instanceObjects[i].getInstance().findProperty
                (propName)).getValue();
            
            CIMReference ref;
            propValue.get (ref);

            //
            //  If the current subscription Filter or Handler is the instance 
            //  to be deleted, it may not be deleted
            //
            // ATTN: Can namespaces in the references cause comparison failure?
            //
            if (instanceReference == ref)
            {
                PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
                return false;
            }
        }
    }

    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
    return true;
}


Array <SubscriptionRef> IndicationService::_getActiveSubscriptions () const
{
    Array <SubscriptionRef> activeSubscriptions;
    Array <String> nameSpaceNames;
    Array <CIMNamedInstance> subscriptions;
    CIMValue subscriptionStateValue;
    Uint16 subscriptionState;
    struct SubscriptionRef current;

    const char METHOD_NAME [] = 
        "IndicationService::_getActiveSubscriptions";

    PEG_FUNC_ENTER (TRC_INDICATION_SERVICE, METHOD_NAME);

    //
    //  Get list of namespaces in repository
    //
    nameSpaceNames = _repository->enumerateNameSpaces ();

    //
    //  Get existing subscriptions from each namespace in the repository
    //
    for (Uint8 i = 0; i < nameSpaceNames.size (); i++)
    {

        //
        //  Get existing subscriptions in current namespace
        //
        try
        {
        subscriptions = _repository->enumerateInstances (nameSpaceNames [i], 
            _CLASS_SUBSCRIPTION);
        }
        catch (CIMException e)
        {
            //
            //  Some namespaces may not include the subscription class
            //  In that case, just continue with the next namespace
            //
            if (e.getCode () == CIM_ERR_INVALID_CLASS)
            {
                continue;
            }
            else
            {
                PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
                throw e;
            }
        }
    
        //
        //  Process each subscription
        //
        for (Uint8 j = 0; j < subscriptions.size (); j++)
        {
            //
            //  Get subscription state
            //
            subscriptionStateValue = 
                subscriptions [j].getInstance ().getProperty
                (subscriptions [j].getInstance ().findProperty 
                (_PROPERTY_STATE)).getValue ();
            subscriptionStateValue.get (subscriptionState);

    
            //
            //  Process each enabled subscription
            //
            if ((subscriptionState == _STATE_ENABLED) ||
                (subscriptionState == _STATE_ENABLEDDEGRADED))
            {
                current.subscription = subscriptions [j].getInstance ();
                current.nameSpaceName = nameSpaceNames [i];

                activeSubscriptions.append (current);

            }  // if subscription is enabled
        }  // for each subscription
    }  // for each namespace

    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
    return (activeSubscriptions);
}


Array <SubscriptionRef> IndicationService::_getMatchingSubscriptions (
    const String & targetClass,
    const CIMPropertyList & targetProperties) const 
{
    Array <SubscriptionRef> matchingSubscriptions;
    Array <String> nameSpaceNames;
    Array <CIMNamedInstance> subscriptions;
    CIMValue subscriptionStateValue;
    Uint16 subscriptionState;
    String filterQuery;
    WQLSelectStatement selectStatement;
    String indicationClassName;
    Array <String> indicationSubclasses;
    CIMPropertyList propertyList;
    Boolean match;
    struct SubscriptionRef current;

    const char METHOD_NAME [] = 
        "IndicationService::_getMatchingSubscriptions";

    PEG_FUNC_ENTER (TRC_INDICATION_SERVICE, METHOD_NAME);

    //
    //  Get list of namespaces in repository
    //
    nameSpaceNames = _repository->enumerateNameSpaces ();

    //
    //  Get existing subscriptions from each namespace in the repository
    //
    for (Uint8 i = 0; i < nameSpaceNames.size (); i++)
    {

        //
        //  Get existing subscriptions in current namespace
        //
        try
        {
        subscriptions = _repository->enumerateInstances (nameSpaceNames [i], 
            _CLASS_SUBSCRIPTION);
        }
        catch (CIMException e)
        {
            //
            //  Some namespaces may not include the subscription class
            //  In that case, just continue with the next namespace
            //
            if (e.getCode () == CIM_ERR_INVALID_CLASS)
            {
                continue;
            }
            else
            {
                PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
                throw e;
            }
        }
    
        //
        //  Process each subscription
        //
        for (Uint8 j = 0; j < subscriptions.size (); j++)
        {
            //
            //  Get subscription state
            //
            subscriptionStateValue = 
                subscriptions [j].getInstance ().getProperty
                (subscriptions [j].getInstance ().findProperty 
                (_PROPERTY_STATE)).getValue ();
            subscriptionStateValue.get (subscriptionState);
    
            //
            //  Process each enabled subscription
            //
            if ((subscriptionState == _STATE_ENABLED) ||
                (subscriptionState == _STATE_ENABLEDDEGRADED))
            {
                //
                //  Get filter query
                //
                filterQuery = _getFilterQuery (subscriptions [j].getInstance (),
                    nameSpaceNames [i]);
                selectStatement = _getSelectStatement (filterQuery);
            
                //
                //  Get indication class name from filter query (FROM clause)
                //
                indicationClassName = _getIndicationClassName (selectStatement,
                    nameSpaceNames [i]);
            
                //
                //  Get list of subclass names for indication class
                //
                indicationSubclasses = _repository->enumerateClassNames 
                    (nameSpaceNames [i], indicationClassName, true);
                indicationSubclasses.append (indicationClassName);

                //
                //  Does current subscription include target class?
                //
                if (Contains (indicationSubclasses, targetClass))
                {
                    match = true;

                    //
                    //  If target properties is null (all properties)
                    //  the subscription can be supported
                    //
                    if (!targetProperties.isNull ())
                    {
                        //
                        //  Get property list from filter query (FROM and 
                        //  WHERE clauses)
                        //
                        if ((selectStatement.getSelectPropertyNameCount () > 0)
                            ||
                            (selectStatement.getWherePropertyNameCount () > 0))
                        {
                            propertyList = _getPropertyList (selectStatement);
                        }
                
                        //
                        //  If the subscription requires all properties,
                        //  but target does not include all properties, 
                        //  the subscription cannot be supported
                        //
                        if (propertyList.isNull ())
                        {
                            match = false;
                        }
                        else 
                        {
                            //
                            //  Compare subscription property list
                            //  with target property list
                            //
                            for (Uint8 k = 0; 
                                 k < propertyList.getNumProperties () && match; 
                                 k++)
                            {
                                if (!Contains 
                                    (targetProperties.getPropertyNameArray (), 
                                    propertyList.getPropertyName (k)))
                                {
                                    match = false;
                                }
                            }
                        }
                    }

                    //
                    //  Add current subscription ref to list
                    //
                    if (match)
                    {
                        current.subscription = subscriptions [j].getInstance ();
                        current.nameSpaceName = nameSpaceNames [i];
                        matchingSubscriptions.append (current);
                    }

                }  // if subscription includes target class
            }  // if subscription is enabled
        }  // for each subscription
    }  // for each namespace

    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
    return (matchingSubscriptions);
}

void IndicationService::_getModifiedSubscriptions (
    const String & targetClass,
    const CIMPropertyList & newProperties,
    const CIMPropertyList & oldProperties,
    Array <struct SubscriptionRef> & newSubscriptions,
    Array <struct SubscriptionRef> & formerSubscriptions)
{
    Array <String> nameSpaceNames;
    Array <CIMNamedInstance> subscriptions;
    CIMValue subscriptionStateValue;
    Uint16 subscriptionState;
    String filterQuery;
    WQLSelectStatement selectStatement;
    String indicationClassName;
    Array <String> indicationSubclasses;
    CIMPropertyList propertyList;
    Boolean newMatch;
    Boolean formerMatch;
    struct SubscriptionRef current;

    const char METHOD_NAME [] = 
        "IndicationService::_getModifiedSubscriptions";

    PEG_FUNC_ENTER (TRC_INDICATION_SERVICE, METHOD_NAME);

    newSubscriptions.clear ();
    formerSubscriptions.clear ();

    //
    //  Get list of namespaces in repository
    //
    nameSpaceNames = _repository->enumerateNameSpaces ();

    //
    //  Get existing subscriptions from each namespace in the repository
    //
    for (Uint8 i = 0; i < nameSpaceNames.size (); i++)
    {

        //
        //  Get existing subscriptions in current namespace
        //
        try
        {
        subscriptions = _repository->enumerateInstances (nameSpaceNames [i], 
            _CLASS_SUBSCRIPTION);
        }
        catch (CIMException e)
        {
            //
            //  Some namespaces may not include the subscription class
            //  In that case, just continue with the next namespace
            //
            if (e.getCode () == CIM_ERR_INVALID_CLASS)
            {
                continue;
            }
            else
            {
                PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
                throw e;
            }
        }
    
        //
        //  Process each subscription
        //
        for (Uint8 j = 0; j < subscriptions.size (); j++)
        {
            //
            //  Get subscription state
            //
            subscriptionStateValue = 
                subscriptions [j].getInstance ().getProperty
                (subscriptions [j].getInstance ().findProperty 
                (_PROPERTY_STATE)).getValue ();
            subscriptionStateValue.get (subscriptionState);
    
            //
            //  Process each enabled subscription
            //
            if ((subscriptionState == _STATE_ENABLED) ||
                (subscriptionState == _STATE_ENABLEDDEGRADED))
            {
                //
                //  Get filter query
                //
                filterQuery = _getFilterQuery (subscriptions [j].getInstance (),
                    nameSpaceNames [i]);
                selectStatement = _getSelectStatement (filterQuery);
            
                //
                //  Get indication class name from filter query (FROM clause)
                //
                indicationClassName = _getIndicationClassName (selectStatement,
                    nameSpaceNames [i]);
            
                //
                //  Get list of subclass names for indication class
                //
                indicationSubclasses = _repository->enumerateClassNames 
                    (nameSpaceNames [i], indicationClassName, true);
                indicationSubclasses.append (indicationClassName);

                //
                //  Does current subscription include target class?
                //
                if (Contains (indicationSubclasses, targetClass))
                {
                    newMatch = true;
                    formerMatch = true;

                    //
                    //  Get property list from filter query (FROM and WHERE 
                    //  clauses)
                    //
                    if ((selectStatement.getSelectPropertyNameCount () > 0) ||
                        (selectStatement.getWherePropertyNameCount () > 0))
                    {
                        propertyList = _getPropertyList (selectStatement);
                    }

                    //
                    //  If new property list is null (all properties)
                    //  the subscription can be supported
                    //
                    if (!newProperties.isNull ())
                    {
                        //
                        //  If the subscription requires all properties,
                        //  but new property list does not include all 
                        //  properties, the subscription cannot be supported
                        //
                        if (propertyList.isNull ())
                        {
                            newMatch = false;
                        }
                        else
                        {
                            //
                            //  Compare subscription property list
                            //  with new property list
                            //
                            for (Uint8 k = 0; 
                                k < propertyList.getNumProperties (); k++)
                            {
                                if (!Contains 
                                    (newProperties.getPropertyNameArray (), 
                                    propertyList.getPropertyName (k)))
                                {
                                    newMatch = false;
                                }
                            }
                        }
                    }

                    //
                    //  If old property list is null (all properties)
                    //  the subscription previously could be supported
                    //
                    if (!oldProperties.isNull ())
                    {
                        //
                        //  If the subscription requires all properties,
                        //  but old property list does not include all 
                        //  properties, the subscription previously could
                        //  not be supported
                        //
                        if (propertyList.isNull ())
                        {
                            formerMatch = false;
                        }
                        else
                        {
                            //
                            //  Compare subscription property list
                            //  with old property list
                            //
                            for (Uint8 m = 0; 
                                 m < propertyList.getNumProperties (); m++)
                            {
                                if (!Contains 
                                    (oldProperties.getPropertyNameArray (), 
                                    propertyList.getPropertyName (m)))
                                {
                                    formerMatch = false;
                                }
                            }
                        }
                    }

                    //
                    //  Add current subscription ref to appropriate list
                    //
                    if (newMatch && !formerMatch)
                    {
                        current.subscription = subscriptions [j].getInstance ();
                        current.nameSpaceName = nameSpaceNames [i];
                        newSubscriptions.append (current);
                    }
                    else if (!newMatch && formerMatch)
                    {
                        current.subscription = subscriptions [j].getInstance ();
                        current.nameSpaceName = nameSpaceNames [i];
                        formerSubscriptions.append (current);
                    }

                }  // if subscription includes target class
            }  // if subscription is enabled
        }  // for each subscription
    }  // for each namespace

    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
}

Array <SubscriptionRef> IndicationService::_getProviderSubscriptions (
    const String & providerName)
{
    Array <SubscriptionRef> providerSubscriptions;
    Array <String> propertyNames;
    Array <String> nameSpaceNames;
    Array <CIMNamedInstance> providerCapabilities;
    String regProviderName;
    Array <CIMInstance> regProviders;

    const char METHOD_NAME [] = 
        "IndicationService::_getProviderSubscriptions";

    PEG_FUNC_ENTER (TRC_INDICATION_SERVICE, METHOD_NAME);

    //
    //  Construct list of requested properties from Provider Capabilities
    //
    propertyNames.append (_PROPERTY_PROVIDER_NAME);
    propertyNames.append (_PROPERTY_CLASS_NAME);
    propertyNames.append (_PROPERTY_SUPPORTED_PROPERTIES);
    propertyNames.append (_PROPERTY_PROVIDER_TYPE);

    //
    //  Get list of namespaces in repository
    //
    nameSpaceNames = _repository->enumerateNameSpaces ();

    //
    //  Check each namespace in the repository
    //
    for (Uint8 i = 0; i < nameSpaceNames.size (); i++)
    {
        //
        //  Get Provider Capabilities instances from the repository
        //
        try
        {
            providerCapabilities = _repository->enumerateInstances 
                (nameSpaceNames [i], _CLASS_PROVIDER_CAPABILITIES, true, true, 
                 false, false, CIMPropertyList (propertyNames));
        }
        catch (CIMException e)
        {
            //
            //  Some namespaces may not include the capabilities class
            //  In that case, just continue with the next namespace
            //
            if (e.getCode () == CIM_ERR_INVALID_CLASS)
            {
                continue;
            }
            else
            {
                PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
                throw e;
            }
        }
    
        //
        //  Check for Provider Capabilities instances where provider name 
        //  matches name of specified provider
        //
        for (Uint8 j = 0; j < providerCapabilities.size (); j++)
        {
            CIMInstance capabilityInstance =
                providerCapabilities [j].getInstance ();
    
            //
            //  Get Provider Name from instance
            //
            capabilityInstance.getProperty (capabilityInstance.findProperty
                (_PROPERTY_PROVIDER_NAME)).getValue ().get (regProviderName);
    
            //
            //  If registered provider name matches specified provider name, 
            //  add to list
            //
            if (regProviderName == providerName)
            {
                regProviders.append (capabilityInstance);
            }
        }

        //
        //  If there were matching provider capability instances in the current
        //  namespace, check the subscriptions in the current namespace
        //
        if (regProviders.size () > 0)
        {
            Array <CIMNamedInstance> subscriptions;
            CIMValue subscriptionStateValue;
            Uint16 subscriptionState;

            //
            //  Get existing subscriptions in current namespace
            //
            try
            {
            subscriptions = _repository->enumerateInstances 
                (nameSpaceNames [i], _CLASS_SUBSCRIPTION);
            }
            catch (CIMException e)
            {
                //
                //  Some namespaces may not include the subscription class
                //  In that case, just continue with the next namespace
                //
                if (e.getCode () == CIM_ERR_INVALID_CLASS)
                {
                    continue;
                }
                else
                {
                    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
                    throw e;
                }
            }
        
            //
            //  Process each subscription
            //
            for (Uint8 k = 0; k < subscriptions.size (); k++)
            {
                //
                //  Get subscription state
                //
                subscriptionStateValue = 
                    subscriptions [k].getInstance ().getProperty
                    (subscriptions [k].getInstance ().findProperty 
                    (_PROPERTY_STATE)).getValue ();
                subscriptionStateValue.get (subscriptionState);
        
                //
                //  Process each enabled subscription
                //
                if ((subscriptionState == _STATE_ENABLED) ||
                    (subscriptionState == _STATE_ENABLEDDEGRADED))
                {
                    String filterQuery;
                    WQLSelectStatement selectStatement;
                    String indicationClassName;
                    Array <String> indicationSubclasses;
                    CIMPropertyList propertyList;
                    Array <String> requiredProperties;
                    String className;
                    Array <String> supportedProperties;
                    struct SubscriptionRef current;
                    Boolean match;

                    //
                    //  Get filter query
                    //
                    filterQuery = _getFilterQuery 
                        (subscriptions [k].getInstance (), nameSpaceNames [i]);
                    selectStatement = _getSelectStatement (filterQuery);
    
                    //
                    //  Get indication class name from filter query 
                    //  (FROM clause)
                    //
                    indicationClassName = _getIndicationClassName 
                        (selectStatement, nameSpaceNames [i]);
            
                    //
                    //  Get list of subclass names for indication class
                    //
                    indicationSubclasses = _repository->enumerateClassNames
                        (nameSpaceNames [i], indicationClassName, true);
                    indicationSubclasses.append (indicationClassName);

                    //
                    //  Get property list from filter query (FROM and WHERE
                    //  clauses)
                    //
                    if ((selectStatement.getSelectPropertyNameCount () > 0) ||
                        (selectStatement.getWherePropertyNameCount () > 0))
                    {
                        propertyList = _getPropertyList (selectStatement);
                    }

                    requiredProperties = propertyList.getPropertyNameArray ();
            
                    match = false;
                    for (Uint8 m = 0; m < regProviders.size () && !match; m++)
                    {
                        //
                        //  Get class name from provider capability instance
                        //
                        regProviders [m].getProperty
                            (regProviders [m].findProperty
                            (_PROPERTY_CLASS_NAME)).getValue ().get (className);
            
                        //
                        //  If class is a requested class, check supported 
                        //  properties
                        //
                        if (Contains (indicationSubclasses, className))
                        {
                            regProviders [m].getProperty
                                (regProviders [m].findProperty
                                (_PROPERTY_SUPPORTED_PROPERTIES)).getValue ().
                                get (supportedProperties);

                            //
                            //  Compare supported properties with properties 
                            //  required by subscription
                            //
                            match = true;
                            for (Uint8 n = 0;
                                 n < requiredProperties.size () && match;
                                 n++)
                            {
                                if (!Contains (supportedProperties,
                                    requiredProperties [n]))
                                {
                                    match = false;
                                    continue;
                                }
                            }
                        }
                    }

                    //
                    //  Add current subscription to list of subscriptions 
                    //  supported by specified provider
                    //
                    if (match)
                    {
                        current.subscription = subscriptions [k].getInstance ();
                        current.nameSpaceName = nameSpaceNames [i];
                        providerSubscriptions.append (current);
                    }
                }  // if subscription is enabled
            }  // for each subscription
        } // if any provider capabilities for specified provider
    }  // for each namespace

    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
    return (providerSubscriptions);
}

String IndicationService::_getFilterQuery (
    const CIMInstance & subscription,
    const String & nameSpaceName) const
{
    CIMValue filterValue;
    CIMReference filterReference;
    CIMInstance filterInstance;
    String filterQuery;

    const char METHOD_NAME [] = "IndicationService::_getFilterQuery";

    PEG_FUNC_ENTER (TRC_INDICATION_SERVICE, METHOD_NAME);

    filterValue = subscription.getProperty (subscription.findProperty
        (_PROPERTY_FILTER)).getValue ();

    filterValue.get (filterReference);

    filterInstance = _repository->getInstance (nameSpaceName, filterReference);

    filterQuery = filterInstance.getProperty (filterInstance.findProperty 
        (_PROPERTY_QUERY)).getValue ().toString ();

    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
    return (filterQuery);
}

String IndicationService::_getFilterQueryLanguage (
    const CIMInstance & subscription,
    const String & nameSpaceName) const
{
    CIMValue filterValue;
    CIMReference filterReference;
    CIMInstance filterInstance;
    String filterQueryLanguage;

    const char METHOD_NAME [] = 
        "IndicationService::_getFilterQueryLanguage";

    PEG_FUNC_ENTER (TRC_INDICATION_SERVICE, METHOD_NAME);

    filterValue = subscription.getProperty (subscription.findProperty
        (_PROPERTY_FILTER)).getValue ();

    filterValue.get (filterReference);

    filterInstance = _repository->getInstance (nameSpaceName, filterReference);

    filterQueryLanguage = filterInstance.getProperty
        (filterInstance.findProperty (_PROPERTY_QUERYLANGUAGE)).
        getValue ().toString ();

    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
    return (filterQueryLanguage);
}

WQLSelectStatement IndicationService::_getSelectStatement (
    const String & filterQuery) const
{
    WQLSelectStatement selectStatement;
    const char METHOD_NAME [] = "IndicationService::_getSelectStatement";

    PEG_FUNC_ENTER (TRC_INDICATION_SERVICE, METHOD_NAME);

    try
    {
        selectStatement.clear ();
        WQLParser::parse (filterQuery, selectStatement);
    }
    catch (ParseError & pe)
    {
        PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
        throw CIMException (CIM_ERR_INVALID_PARAMETER);
    }
    catch (MissingNullTerminator & mnt)
    {
        PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
        throw CIMException (CIM_ERR_INVALID_PARAMETER);
    }

    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
    return selectStatement;
}

String IndicationService::_getIndicationClassName (
    const WQLSelectStatement & selectStatement,
    const String & nameSpaceName) const
{
    String indicationClassName;
    Array <String> indicationSubclasses;
    const char METHOD_NAME [] = 
        "IndicationService::_getIndicationClassName";

    PEG_FUNC_ENTER (TRC_INDICATION_SERVICE, METHOD_NAME);

    indicationClassName = selectStatement.getClassName ();

    //
    //  Get list of subclass names for indication class
    //
    indicationSubclasses = _repository->enumerateClassNames
        (nameSpaceName, _CLASS_INDICATION, true);
    indicationSubclasses.append (_CLASS_INDICATION);

    //
    //  Validate query FROM clause consists of a single subclass of
    //  the Indication class
    //
    if (!Contains (indicationSubclasses, indicationClassName))
    {
        //cout << "Invalid indication class name in FROM clause of query"
        //     << endl;
        PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
        throw CIMException (CIM_ERR_INVALID_PARAMETER);
    }

    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
    return (indicationClassName);
}

Array <struct ProviderClassList> 
    IndicationService::_getIndicationProviders (
        const String & nameSpace,
        const String & indicationClassName,
        const Array <String> & indicationSubclasses,
        const CIMPropertyList & requiredPropertyList) const
{
    Array <String> propertyNames;
    Uint16 providerType;
    String className;
    Array <String> supportedProperties;
    Array <String> requiredProperties;
    String providerName;
    Array <struct ProviderClassList> indicationProviders;
    struct ProviderClassList provider;
    Boolean duplicate;
    Boolean match;
    const char METHOD_NAME [] = 
        "IndicationService::_getIndicationProviders";

    PEG_FUNC_ENTER (TRC_INDICATION_SERVICE, METHOD_NAME);

    //
    //  Construct list of requested properties from Provider Capabilities
    //
    propertyNames.append (_PROPERTY_PROVIDER_NAME);
    propertyNames.append (_PROPERTY_CLASS_NAME);
    propertyNames.append (_PROPERTY_SUPPORTED_PROPERTIES);
    propertyNames.append (_PROPERTY_PROVIDER_TYPE);

    //
    //  If all properties are required, get list
    //
    if (requiredPropertyList.isNull ())
    {
        CIMClass indicationClass = _repository->getClass 
            (nameSpace, indicationClassName);
        for (Uint32 j = 0; j < indicationClass.getPropertyCount ();
             j++)
        {
            requiredProperties.append (indicationClass.getProperty 
                (j).getName ());
        }
    }
    else
    {
        requiredProperties = 
            requiredPropertyList.getPropertyNameArray ();
    }

    //
    //  Get Provider Capabilities instances from the repository
    //
    Array <CIMNamedInstance> providerCapabilities = 
        _repository->enumerateInstances (nameSpace, 
            _CLASS_PROVIDER_CAPABILITIES, true, true, false, false,
            CIMPropertyList (propertyNames));

    for (Uint8 i = 0; i < providerCapabilities.size (); i++)
    {
        CIMInstance capabilityInstance = 
            providerCapabilities [i].getInstance ();

        //
        //  Get Provider Type from instance
        //
        capabilityInstance.getProperty (capabilityInstance.findProperty 
            (_PROPERTY_PROVIDER_TYPE)).getValue ().get (providerType);

        //
        //  If provider type is Indication, check class name
        //
        if (providerType == _VALUE_INDICATION)
        {
            capabilityInstance.getProperty 
                (capabilityInstance.findProperty 
                (_PROPERTY_CLASS_NAME)).getValue ().get (className);

            //
            //  If class is a requested class, check supported properties
            //
            if (Contains (indicationSubclasses, className))
            {
                Uint32 pos = capabilityInstance.findProperty
                    (_PROPERTY_SUPPORTED_PROPERTIES);

		if (pos != PEG_NOT_FOUND)
		{
		    capabilityInstance.getProperty (pos).getValue ().get 
                        (supportedProperties);
		}

                //
                //  Compare supported properties with properties required by 
                //  subscription
                //
                match = true;
                for (Uint8 k = 0; 
                     k < requiredProperties.size () && match; 
                     k++)
                {
                    if (!Contains (supportedProperties, requiredProperties [k]))
                    {
                        match = false;
                    }
                }

                if (match)
                {
                    provider.providerName = String::EMPTY;
                    provider.classList.clear ();
                    duplicate = false;
    
                    capabilityInstance.getProperty 
                        (capabilityInstance.findProperty 
                        (_PROPERTY_PROVIDER_NAME)).getValue ().get 
                        (providerName);

                    for (Uint8 m = 0; 
                         m < indicationProviders.size () && !duplicate; m++)
                    {
                        if (indicationProviders [m].providerName == 
                            providerName)
                        {
                            //
                            //  Add subclass to list
                            //
                            indicationProviders [m].classList.append 
                                (className);
                            duplicate = true;
                        }
                    }
    
                    //
                    //  Current provider is not yet in list
                    //  Create new list entry
                    //
                    if (!duplicate)
                    {
                        provider.providerName = providerName;
                        provider.classList.append (className);
                        indicationProviders.append (provider);
                    }
                }
            }
        }
    }

    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
    return (indicationProviders);
}


CIMPropertyList IndicationService::_getPropertyList 
    (const WQLSelectStatement & selectStatement) const
{
    Array <String> propertyList;
    String propertyName;

    const char METHOD_NAME [] = "IndicationService::_getPropertyList";

    PEG_FUNC_ENTER (TRC_INDICATION_SERVICE, METHOD_NAME);

    //
    //  Get all the properties referenced in the projection list (SELECT clause)
    //
    Uint32 selectCount = selectStatement.getSelectPropertyNameCount ();
    if (selectCount > 0)
    {
        if (selectStatement.getSelectPropertyName (0) == _QUERY_ALLPROPERTIES)
        {
            //
            //  Return null CIMPropertyList for all properties
            //
            PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
            return CIMPropertyList ();
        }

        for (Uint32 i = 0; i < selectCount; i++)
        {
            propertyName = selectStatement.getSelectPropertyName (i);
            if (!Contains (propertyList, propertyName))
            {
                propertyList.append (propertyName);
            }
        }
    }

    //
    //  Get all the properties referenced in the condition (WHERE clause)
    //
    if (selectStatement.hasWhereClause ())
    {
        Uint32 whereCount = selectStatement.getWherePropertyNameCount ();
        if (whereCount > 0)
        {
            for (Uint32 j = 0; j < whereCount; j++)
            {
                propertyName = selectStatement.getWherePropertyName (j);
                if (!Contains (propertyList, propertyName))
                {
                    propertyList.append (propertyName);
                }
            }
        }
    }

    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
    return (CIMPropertyList (propertyList));
}

String IndicationService::_getCondition 
    (const String & filterQuery) const
{
    String condition = String::EMPTY;
    const char METHOD_NAME [] = "IndicationService::_getCondition";

    PEG_FUNC_ENTER (TRC_INDICATION_SERVICE, METHOD_NAME);

    //
    //  Get condition substring from filter query
    //
    if (filterQuery.find (_QUERY_WHERE) != PEG_NOT_FOUND)
    {
        condition = filterQuery.subString (filterQuery.find 
            (_QUERY_WHERE) + 6);
    }

    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
    return (condition);
}


struct HandlerRef IndicationService::_getHandlerRef (
    const struct SubscriptionRef & subscriptionRef) const
{
    CIMValue handlerValue;
    CIMReference handlerRef;
    CIMInstance handlerInstance;
    struct HandlerRef handler;
    const char METHOD_NAME [] = "IndicationService::_getHandlerRef";

    PEG_FUNC_ENTER (TRC_INDICATION_SERVICE, METHOD_NAME);

    //
    //  Get Handler reference from subscription instance
    //
    handlerValue = subscriptionRef.subscription.getProperty 
        (subscriptionRef.subscription.findProperty
        (_PROPERTY_HANDLER)).getValue ();

    handlerValue.get (handlerRef);

    //
    //  Get Handler instance from the repository
    //
    handlerInstance = _repository->getInstance 
        (subscriptionRef.nameSpaceName, handlerRef);

    handler.handler = handlerInstance;
    handler.nameSpaceName = subscriptionRef.nameSpaceName;

    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
    return (handler);
}

void IndicationService::_sendEnableRequests
    (const Array <struct ProviderClassList> & indicationProviders,
     const String & nameSpace,
     const CIMPropertyList & propertyList,
     const String & condition,
     const String & queryLanguage,
     const CIMInstance & subscription)
{
    CIMValue propValue;
    Uint16 repeatNotificationPolicy;
    String otherRepeatNotificationPolicy;
    CIMDateTime repeatNotificationInterval;
    CIMDateTime repeatNotificationGap;
    Uint16 repeatNotificationCount;
    const char METHOD_NAME [] = "IndicationService::_sendEnableRequests";

    PEG_FUNC_ENTER (TRC_INDICATION_SERVICE, METHOD_NAME);

    //
    //  Get thresholding parameter values from subscription instance
    //

    propValue = subscription.getProperty (subscription.findProperty 
        (_PROPERTY_REPEATNOTIFICATIONPOLICY)).getValue ();
    propValue.get (repeatNotificationPolicy);
    if ((repeatNotificationPolicy != _POLICY_UNKNOWN) &&
        (repeatNotificationPolicy != _POLICY_NONE))
    {
        propValue = subscription.getProperty (subscription.findProperty 
            (_PROPERTY_REPEATNOTIFICATIONINTERVAL)).getValue ();
        propValue.get (repeatNotificationInterval);
        propValue = subscription.getProperty (subscription.findProperty 
            (_PROPERTY_REPEATNOTIFICATIONGAP)).getValue ();
        propValue.get (repeatNotificationGap);
        propValue = subscription.getProperty (subscription.findProperty 
            (_PROPERTY_REPEATNOTIFICATIONCOUNT)).getValue ();
        propValue.get (repeatNotificationCount);

        //
        //  ATTN: Threshold parameter values must go into Operation Context
        //
    }
    if (repeatNotificationPolicy == _POLICY_OTHER)
    {
        propValue = subscription.getProperty (subscription.findProperty 
            (_PROPERTY_OTHERREPEATNOTIFICATIONPOLICY)).getValue ();
        propValue.get (otherRepeatNotificationPolicy);

        //
        //  ATTN: Threshold parameter values must go into Operation Context
        //
    }

    //
    //  ATTN: method of accessing dispatcher queue will be changing
    //  but is not yet finalized
    //
    MessageQueue * queue = MessageQueue::lookup
        ("CIMOpRequestDispatcher");

    if (queue)
    {
        for (Uint8 i = 0; i < indicationProviders.size (); i++)
        {
            CIMEnableIndicationSubscriptionRequestMessage * request =
                new CIMEnableIndicationSubscriptionRequestMessage
                    ("1234",
                    nameSpace,
                    indicationProviders [i].classList,
                    indicationProviders [i].providerName,
                    propertyList.getPropertyNameArray (),
                    repeatNotificationPolicy,
                    otherRepeatNotificationPolicy,
                    repeatNotificationInterval,
                    repeatNotificationGap,
                    repeatNotificationCount,
                    condition,
                    queryLanguage,
                    subscription,
                    QueueIdStack (queue->getQueueId ()));

            AsyncOpNode* op = this->get_op(); 

            AsyncLegacyOperationStart * async_req =
                new AsyncLegacyOperationStart
                    (get_next_xid (),
                    op,
                    _providerManager,
                    request,
                    _queueId);

            AsyncReply * async_reply = SendWait (async_req);

            //
            //  ATTN: Check for return value indicating invalid queue ID
            //  If received, need to find Provider Manager Service queue ID
            //  again
            //

            CIMEnableIndicationSubscriptionResponseMessage * response =
                reinterpret_cast 
                <CIMEnableIndicationSubscriptionResponseMessage *>
                ((static_cast <AsyncLegacyOperationResult *>
                (async_reply))->res);

            delete async_req;
            delete async_reply;
        }
    }
    else
    {
        PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
        throw CIMException (CIM_ERR_FAILED);
    }

    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
}

//
//  NOTE: Currently, _sendModifyRequests is not called, because the only 
//  modification supported is of the Subscription State property of the 
//  Subscription class (which results in an enable or disable request to the 
//  indication provider).  In future releases, modification of other 
//  Subscription properties will be supported, and a modify request will be 
//  used to inform the indication provider of the modification.
//

void IndicationService::_sendModifyRequests
    (const Array <struct ProviderClassList> & indicationProviders,
     const String & nameSpace,
     const CIMPropertyList & propertyList,
     const String & condition,
     const String & queryLanguage,
     const CIMInstance & subscription)
{
    CIMValue propValue;
    Uint16 repeatNotificationPolicy;
    String otherRepeatNotificationPolicy;
    CIMDateTime repeatNotificationInterval;
    CIMDateTime repeatNotificationGap;
    Uint16 repeatNotificationCount;
    const char METHOD_NAME [] = "IndicationService::_sendModifyRequests";

    PEG_FUNC_ENTER (TRC_INDICATION_SERVICE, METHOD_NAME);

    //
    //  Get thresholding parameter values from subscription instance
    //
    propValue = subscription.getProperty (subscription.findProperty
        (_PROPERTY_REPEATNOTIFICATIONPOLICY)).getValue ();
    propValue.get (repeatNotificationPolicy);
    if ((repeatNotificationPolicy != _POLICY_UNKNOWN) &&
        (repeatNotificationPolicy != _POLICY_NONE))
    {
        propValue = subscription.getProperty (subscription.findProperty
            (_PROPERTY_REPEATNOTIFICATIONINTERVAL)).getValue ();
        propValue.get (repeatNotificationInterval);
        propValue = subscription.getProperty (subscription.findProperty
            (_PROPERTY_REPEATNOTIFICATIONGAP)).getValue ();
        propValue.get (repeatNotificationGap);
        propValue = subscription.getProperty (subscription.findProperty
            (_PROPERTY_REPEATNOTIFICATIONCOUNT)).getValue ();
        propValue.get (repeatNotificationCount);

        //
        //  ATTN: Threshold parameter values must go into Operation Context
        //
    }
    if (repeatNotificationPolicy == _POLICY_OTHER)
    {
        propValue = subscription.getProperty (subscription.findProperty
            (_PROPERTY_OTHERREPEATNOTIFICATIONPOLICY)).getValue ();
        propValue.get (otherRepeatNotificationPolicy);

        //
        //  ATTN: Threshold parameter values must go into Operation Context
        //
    }

    //
    //  ATTN: method of accessing dispatcher queue will be changing
    //  but is not yet finalized
    //
    MessageQueue * queue = MessageQueue::lookup
        ("CIMOpRequestDispatcher");

    if (queue)
    {    
        for (Uint8 i = 0; i < indicationProviders.size (); i++)
        {
            CIMModifyIndicationSubscriptionRequestMessage* request =
                new CIMModifyIndicationSubscriptionRequestMessage
                    ("1234",
                    nameSpace,
                    indicationProviders [i].classList,
                    indicationProviders [i].providerName,
                    propertyList.getPropertyNameArray (),
                    repeatNotificationPolicy,
                    otherRepeatNotificationPolicy,
                    repeatNotificationInterval,
                    repeatNotificationGap,
                    repeatNotificationCount,
                    condition,
                    queryLanguage,
                    subscription,
                    QueueIdStack (queue->getQueueId ()));

            AsyncOpNode* op = this->get_op();

            AsyncLegacyOperationStart * async_req =
                new AsyncLegacyOperationStart
                    (get_next_xid (),
                    op,
                    _providerManager,
                    request,
                    _queueId);

            AsyncReply * async_reply = SendWait (async_req);

            //
            //  ATTN: Check for return value indicating invalid queue ID
            //  If received, need to find Provider Manager Service queue ID
            //  again
            //

            CIMModifyIndicationSubscriptionResponseMessage * response =
                reinterpret_cast
                <CIMModifyIndicationSubscriptionResponseMessage *>
                ((static_cast <AsyncLegacyOperationResult *>
                (async_reply))->res);

            delete async_req;
            delete async_reply;
        }
    }
    else
    {
        PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
        throw CIMException (CIM_ERR_FAILED);
    }

    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
}

void IndicationService::_sendDisableRequests
    (const Array <struct ProviderClassList> & indicationProviders,
     const String & nameSpace,
     const CIMInstance & subscription)
{
    const char METHOD_NAME [] = "IndicationService::_sendDisableRequests";

    PEG_FUNC_ENTER (TRC_INDICATION_SERVICE, METHOD_NAME);

    //
    //  ATTN: method of accessing dispatcher queue will be changing
    //  but is not yet finalized
    //
    MessageQueue * queue = MessageQueue::lookup
        ("CIMOpRequestDispatcher");

    if (queue)
    {    
        for (Uint8 i = 0; i < indicationProviders.size (); i++)
        {
            CIMDisableIndicationSubscriptionRequestMessage* request =
                new CIMDisableIndicationSubscriptionRequestMessage
                ("1234",
                nameSpace,
                indicationProviders [i].classList,
                indicationProviders [i].providerName,
                subscription,
                QueueIdStack (queue->getQueueId ()));

            AsyncOpNode* op = this->get_op();

            AsyncLegacyOperationStart * async_req =
                new AsyncLegacyOperationStart
                    (get_next_xid (),
                    op,
                    _providerManager,
                    request,
                    _queueId);

            AsyncReply * async_reply = SendWait (async_req);

            //
            //  ATTN: Check for return value indicating invalid queue ID
            //  If received, need to find Provider Manager Service queue ID
            //  again
            //

            CIMDisableIndicationSubscriptionResponseMessage * response =
                reinterpret_cast
                <CIMDisableIndicationSubscriptionResponseMessage *>
                ((static_cast <AsyncLegacyOperationResult *>
                (async_reply))->res);

            delete async_req;
            delete async_reply;
        }
    }
    else
    {
        PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
        throw CIMException (CIM_ERR_FAILED);
    }

    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
}

CIMInstance IndicationService::_createAlertInstance (
    const String & alertClassName,
    const Array <struct SubscriptionRef> & subscriptionRefs)
{
    const char METHOD_NAME [] = "IndicationService::_createAlertInstance";

    PEG_FUNC_ENTER (TRC_INDICATION_SERVICE, METHOD_NAME);

    CIMInstance indicationInstance (alertClassName);

    // 
    //  Add property values for all required properties of CIM_AlertIndication
    // 
    indicationInstance.addProperty 
        (CIMProperty (_PROPERTY_ALERTTYPE, CIMValue ((Uint16) _TYPE_OTHER)));
    //
    // ATTN: what should Other Alert Type value be??
    //
    indicationInstance.addProperty 
        (CIMProperty (_PROPERTY_OTHERALERTTYPE, alertClassName));

    indicationInstance.addProperty 
        (CIMProperty (_PROPERTY_PERCEIVEDSEVERITY,
                      CIMValue ((Uint16) _SEVERITY_WARNING)));
    //
    // ATTN: what should Probable Cause value be??
    //
    indicationInstance.addProperty 
        (CIMProperty (_PROPERTY_PROBABLECAUSE,
                      CIMValue ((Uint16) _CAUSE_UNKNOWN)));   

    // 
    //  Add properties specific to each alert class
    //  ATTN: update once alert classes have been defined
    //  NB: for _CLASS_NO_PROVIDER_ALERT and _CLASS_PROVIDER_TERMINATED_ALERT,
    //  one of the properties will be a list of affected subscriptions
    //  It is for that reason that subscriptionRefs is passed in as a parameter
    // 
    if (alertClassName == _CLASS_CIMOM_SHUTDOWN_ALERT)
    {
    }
    else if (alertClassName == _CLASS_NO_PROVIDER_ALERT)
    {
    }
    else if (alertClassName == _CLASS_PROVIDER_TERMINATED_ALERT)
    {
    }

    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
    return (indicationInstance);
}

void IndicationService::_sendAlerts (
    const Array <struct SubscriptionRef> & subscriptionRefs,
    /* const */ CIMInstance & alertInstance)
{
    struct HandlerRef current;
    Boolean duplicate;
    Array <struct HandlerRef> handlers;
    const char METHOD_NAME [] = "IndicationService::_sendAlerts";

    PEG_FUNC_ENTER (TRC_INDICATION_SERVICE, METHOD_NAME);

    handlers.clear ();

    //
    //  Get list of unique handler instances for all subscriptions in list
    //
    for (Uint8 i = 0; i < subscriptionRefs.size (); i++)
    {
        //
        //  Get handler instance
        //
        current = _getHandlerRef (subscriptionRefs [i]);

        //
        //  Merge into list of unique handler instances
        //
        duplicate = false;
        for (Uint8 j = 0; j < handlers.size () && !duplicate; j++)
        {
            if ((current.handler == handlers [j].handler) &&
                (current.nameSpaceName == handlers [j].nameSpaceName))
            {
                duplicate = true;
            }
        }

        if (!duplicate)
        {
            handlers.append (current);
        }
    }  // for each subscription in list

    //
    //  ATTN: method of accessing export queue will be changing
    //  but is not yet finalized
    //

    MessageQueue * queue = MessageQueue::lookup
        ("CIMExportRequestDispatcher");

    if (queue)
    {
        for (Uint8 k = 0; k < handlers.size (); k++)
        {
             CIMHandleIndicationRequestMessage * handler_request =
                 new CIMHandleIndicationRequestMessage (
                     "1234",
                     handlers [k].nameSpaceName,
                     handlers [k].handler,
                     alertInstance,
                     QueueIdStack (queue->getQueueId ()));

             AsyncOpNode* op = this->get_op();
             
             AsyncLegacyOperationStart *async_req = 
                 new AsyncLegacyOperationStart(
                     get_next_xid(),
                     op,
                     _handlerService,
                     handler_request,
                     _queueId);

             AsyncReply *async_reply = SendWait(async_req);
             CIMHandleIndicationResponseMessage* response = 
                 reinterpret_cast<CIMHandleIndicationResponseMessage *>
                 ((static_cast<AsyncLegacyOperationResult *>
                 (async_reply))->res);

             delete async_req;
             delete async_reply;
        }
    }
    else
    {
        PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
        throw CIMException (CIM_ERR_FAILED);
    }

    PEG_FUNC_EXIT (TRC_INDICATION_SERVICE, METHOD_NAME);
}

WQLSimplePropertySource IndicationService::_getPropertySourceFromInstance(
    CIMInstance& indicationInstance)
{
    Boolean booleanValue;
    WQLSimplePropertySource source;

    for (Uint8 i=0; i < indicationInstance.getPropertyCount(); i++)
    {
        CIMProperty property = indicationInstance.getProperty(i);
        CIMValue propertyValue = property.getValue();
        CIMType type = property.getType();
        String propertyName = property.getName();

        switch (type)
        {
            case CIMType::UINT8:
                Uint8 propertyValueUint8;
                propertyValue.get(propertyValueUint8);
                source.addValue(propertyName,
                    WQLOperand(propertyValueUint8, WQL_INTEGER_VALUE_TAG));
                break;

            case CIMType::UINT16:
                Uint16 propertyValueUint16;
                propertyValue.get(propertyValueUint16);
                source.addValue(propertyName,
                    WQLOperand(propertyValueUint16, WQL_INTEGER_VALUE_TAG));
                break;

            case CIMType::UINT32:
                Uint32 propertyValueUint32;
                propertyValue.get(propertyValueUint32);
                source.addValue(propertyName,
                    WQLOperand(propertyValueUint32, WQL_INTEGER_VALUE_TAG));
                break;

            case CIMType::UINT64:
                Uint64 propertyValueUint64;
                propertyValue.get(propertyValueUint64);
                source.addValue(propertyName,
                    WQLOperand(propertyValueUint64, WQL_INTEGER_VALUE_TAG));
                break;

            case CIMType::SINT8:
                Sint8 propertyValueSint8;
                propertyValue.get(propertyValueSint8);
                source.addValue(propertyName,
                    WQLOperand(propertyValueSint8, WQL_INTEGER_VALUE_TAG));
                break;

            case CIMType::SINT16:
                Sint16 propertyValueSint16;
                propertyValue.get(propertyValueSint16);
                source.addValue(propertyName,
                    WQLOperand(propertyValueSint16, WQL_INTEGER_VALUE_TAG));
                break;

            case CIMType::SINT32:
                Sint32 propertyValueSint32;
                propertyValue.get(propertyValueSint32);
                source.addValue(propertyName,
                    WQLOperand(propertyValueSint32, WQL_INTEGER_VALUE_TAG));
                break;                break;

            case CIMType::SINT64:
                Sint64 propertyValueSint64;
                propertyValue.get(propertyValueSint64);
                source.addValue(propertyName,
                    WQLOperand(propertyValueSint64, WQL_INTEGER_VALUE_TAG));
                break;

            case CIMType::REAL32:
                Real32 propertyValueReal32;
                propertyValue.get(propertyValueReal32);
                source.addValue(propertyName,
                    WQLOperand(propertyValueReal32, WQL_DOUBLE_VALUE_TAG));
                break;

            case CIMType::REAL64:
                Real64 propertyValueReal64;
                propertyValue.get(propertyValueReal64);
                source.addValue(propertyName,
                    WQLOperand(propertyValueReal64, WQL_DOUBLE_VALUE_TAG));
                break;

            case CIMType::BOOLEAN :
                property.getValue().get(booleanValue);
                source.addValue(propertyName,
                    WQLOperand(booleanValue, WQL_BOOLEAN_VALUE_TAG));
                break;

            case CIMType::CHAR16:
            case CIMType::STRING :
                source.addValue(propertyName,
                    WQLOperand(property.getValue().toString(),
                    WQL_STRING_VALUE_TAG));
                break;

            case CIMType::NONE :
                source.addValue(propertyName,
                    WQLOperand());
                break;
        }
    }

    return (source);
}

PEGASUS_NAMESPACE_END
