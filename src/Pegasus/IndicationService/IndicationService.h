//%//////-*-c++-*-//////////////////////////////////////////////////////////////
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

#ifndef Pegasus_IndicationService_h
#define Pegasus_IndicationService_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/MessageQueueService.h>
#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Repository/CIMRepository.h>
#include <Pegasus/WQL/WQLParser.h>
#include <Pegasus/WQL/WQLSelectStatement.h>
#include <Pegasus/WQL/WQLSimplePropertySource.h>

PEGASUS_NAMESPACE_BEGIN

struct ProviderClassList
{
    String providerName;
    Array <String> classList;
};

struct SubscriptionRef
{
    CIMInstance subscription;
    String nameSpaceName;
};

struct HandlerRef
{
    CIMInstance handler;
    String nameSpaceName;
};

/**

    IndicationService class is the provider that serves the
    CIM_IndicationSubscription, CIM_IndicationFilter, and CIM_IndicationHandler
    classes.

    @author  Hewlett-Packard Company

 */

class PEGASUS_SERVER_LINKAGE IndicationService : public MessageQueueService
{
public:

    typedef MessageQueueService Base;

    /**
        Operation types for the NotifyProviderRegistration API
     */
    enum Operation {OP_CREATE = 1, OP_DELETE = 2, OP_MODIFY = 3};

    /**
        Constructs an IndicationSubscription instance and initializes instance
        variables.
     */
    IndicationService(CIMRepository * repository);

    virtual ~IndicationService(void);

    void handleEnqueue(Message* message);

    virtual void handleEnqueue(void); 

    virtual void _handle_async_request(AsyncRequest *req);

    AtomicInt dienow;

private:

    CIMRepository* _repository;

    /**
        Integer representing queue ID for accessing Provider Manager Service
     */
    Uint32 _providerManager;

    /**
        Integer representing queue ID for accessing Repository Service
     */
    //Uint32 _repository;

    /**
        Integer representing queue ID for accessing Handler Manager Service
     */
    Uint32 _handlerService;


    /**
        Values for the Subscription State property of the Subscription class,
        as defined in the CIM Events MOF
     */
    enum SubscriptionState {_STATE_UNKNOWN = 0, _STATE_OTHER = 1, 
         _STATE_ENABLED = 2, _STATE_ENABLEDDEGRADED = 3, _STATE_DISABLED = 4};

    /**
        Values for the Repeat Notification Policy property of the Subscription 
        class, as defined in the CIM Events MOF
     */
    enum RepeatNotificationPolicy {_POLICY_UNKNOWN = 0, _POLICY_OTHER = 1,
         _POLICY_NONE = 2, _POLICY_SUPPRESS = 3, _POLICY_DELAY = 4};

    /**
        Values for the On Fatal Error Policy property of the Subscription 
        class, as defined in the CIM Events MOF
     */
    enum OnFatalErrorPolicy {_ERRORPOLICY_OTHER = 1, _ERRORPOLICY_IGNORE = 2, 
        _ERRORPOLICY_DISABLE = 3, _ERRORPOLICY_REMOVE = 4};

    /**
        Values for the Persistence Type property of the Handler class, 
        as defined in the CIM Events MOF
     */
    enum PersistenceType {_PERSISTENCE_OTHER = 1, _PERSISTENCE_PERMANENT = 2, 
        _PERSISTENCE_TRANSIENT = 3};

    /**
        Values for the Perceived Severity property of the Alert Indication 
        class, as defined in the CIM Events MOF
     */
    enum PerceivedSeverity {_SEVERITY_UNKNOWN = 0, _SEVERITY_OTHER = 1, 
         _SEVERITY_INFORMATION = 2, _SEVERITY_WARNING = 3, _SEVERITY_MINOR = 4,
         _SEVERITY_MAJOR = 5, _SEVERITY_CRITICAL = 6, _SEVERITY_FATAL = 7};

    /**
        Values for the Probable Cause property of the Alert Indication 
        class, as defined in the CIM Events MOF
        Note: not all possible values have been included
     */
    enum ProbableCause {_CAUSE_UNKNOWN = 0, _CAUSE_OTHER = 1};

    /**
        Values for the Alert Type property of the Alert Indication class, 
        as defined in the CIM Events MOF
     */
    enum AlertType {_TYPE_OTHER = 1, _TYPE_COMMUNICATIONS = 2, _TYPE_QOS = 3,
         _TYPE_PROCESSING = 4, _TYPE_DEVICE = 5, _TYPE_ENVIRONMENTAL = 6,
         _TYPE_MODELCHANGE = 7, _TYPE_SECURITY = 8};

    void _initialize (void);

    void _terminate (void);

    void _handleGetInstanceRequest(const Message * message);

    void _handleEnumerateInstancesRequest(const Message * message);

    void _handleEnumerateInstanceNamesRequest(const Message * message);

    void _handleCreateInstanceRequest(const Message * message);

    void _handleModifyInstanceRequest(const Message * message);

    void _handleDeleteInstanceRequest(const Message * message);

    void _handleProcessIndicationRequest(const Message * message);

    /**
	Notifies the Indication Service that a change in provider registration
	has occurred.  The Indication Service retrieves the subscriptions
	affected by the registration change, sends the appropriate enable
	and/or disable requests to the provider, and sends an alert to handler
	instances of subscriptions that are no longer served by the provider.
    */
    void _handleNotifyProviderRegistrationRequest(const Message * message);

    /**
        Notifies the Indication Subscription Service that a provider has
        terminated (either intentionally or abnormally).  The Indication
        Subscription Service retrieves the subscriptions affected by the
        termination, and sends an alert to handler instances of
        subscriptions that are no longer served by the provider.
     */
    void _handleNotifyProviderTerminationRequest(const Message * message);

    /**
        Determines if it is legal to delete an instance. Subscription
        instances may always be deleted. Filter and Handler instances
        may only be deleted if they are not being referenced by any
        Subscription instances.  Authorization checks are NOT performed
        by _canDelete.
     */
    Boolean _canDelete (
        const CIMReference & instanceReference,
        const String & nameSpace);

    /**
        Retrieves list of enabled subscription instances in all namespaces.

        @return   list of SubscriptionRef structs
     */
    Array <struct SubscriptionRef> _getActiveSubscriptions () const;

    /**
        Retrieves list of enabled subscription instances in all namespaces,
        where the subscription indication class matches or is a superclass
        of the target class, and the properties required to process the
        subscription are all contained in the list of target properties.

        @param   targetClass       the target class
        @param   targetProperties  the list of target properties

        @return   list of SubscriptionRef structs
     */
    Array <struct SubscriptionRef> _getMatchingSubscriptions (
        const String & targetClass,
        const CIMPropertyList & targetProperties) const;

    /**
        Retrieves lists of enabled subscription instances in all namespaces
        that are either newly supported or previously supported, based on the
        target class and the old and new property lists.  For subscriptions
        based on the target class, the newSubscriptions list returned contains
        the subscriptions for which the properties required to process the
        subscription are all contained in the list of new properties, but are
        not all contained in the list of old properties.  The
        formerSubscriptions list returned contains the subscriptions for which
        the properties required to process the subscription are not all
        contained in the list of new properties, but are all contained in the
        list of old properties.

        @param   targetClass          the target class
        @param   newProperties        the list of new properties
        @param   oldProperties        the list of old properties
        @param   newSubscriptions     the list of newly supported subscriptions
        @param   formerSubscriptions  the list of previously supported
                                          subscriptions
     */
    void _getModifiedSubscriptions (
        const String & targetClass,
        const CIMPropertyList & newProperties,
        const CIMPropertyList & oldProperties,
        Array <struct SubscriptionRef> & newSubscriptions,
        Array <struct SubscriptionRef> & formerSubscriptions);

    /**
        Retrieves list of enabled subscription instances in all namespaces,
        that are served by the specified provider.

        @param   providerName      the provider name

        @return   list of SubscriptionRef structs
     */
    Array <struct SubscriptionRef> _getProviderSubscriptions (
        const String & providerName);

    /**
        Retrieves the string value of the filter query property
        for the specified subscription instance.

        @param   subscription      the subscription instance
        @param   nameSpaceName     the namespace name

        @return  String containing the filter query
     */
    String _getFilterQuery (
        const CIMInstance & subscription,
        const String & nameSpaceName) const;

    /**
        Retrieves the string value of the filter query language property
        for the specified subscription instance.

        @param   subscription      the subscription instance
        @param   nameSpaceName     the namespace name

        @return  String containing the filter query language
     */
    String _getFilterQueryLanguage (
        const CIMInstance & subscription,
        const String & nameSpaceName) const;

    /**
        Parses the filter query string, and returns the corresponding
        WQLSelectStatement object.

        @param   filterQuery           the filter query string

        @return  WQLSelectStatement representing the filter query
     */
    WQLSelectStatement _getSelectStatement (
        const String & filterQuery) const;

    /**
        Extracts the indication class name from the specified WQL select
        statement, and validates that the name represents a subclass of the
        Indication class.

        @param   selectStatement       the WQL select statement
        @param   nameSpaceName         the namespace

        @return  String containing the indication class name
     */
    String _getIndicationClassName (
        const WQLSelectStatement & selectStatement,
        const String & nameSpaceName) const;

    /**
        Retrieves the list of indication providers that serve the specified
        indication subclasses.

        @param   nameSpaceName         the namespace name
        @param   indicationClassName   the indication class name
        @param   indicationSubclasses  the list of indication subclass names
        @param   requiredPropertyList  the properties required

        @return  list of ProviderClassList structs
     */
    Array <struct ProviderClassList> _getIndicationProviders (
        const String & nameSpace,
        const String & indicationClassName,
        const Array <String> & indicationSubclasses,
        const CIMPropertyList & requiredPropertyList) const;

    /**
        Retrieves the list of properties referenced by the specified
        filter query select statement.

        @param   selectStatement       the WQL select statement

        @return  list of properties referenced by the filter query select
                 statement
     */
    CIMPropertyList _getPropertyList (
        const WQLSelectStatement & selectStatement) const;

    /**
        Extracts the condition (WHERE Clause) from the specified filter query
        string.

        @param   filterQuery       the filter query

        @return  String containing the filter query condition
     */
    String _getCondition (
        const String & filterQuery) const;

    /**
        Retrieves the HandlerRef struct representing the handler of the
        specified subscription.

        @param   subscriptionRef       the subscription

        @return  a HandlerRef struct for the subscription's handler
     */
    struct HandlerRef _getHandlerRef (
        const struct SubscriptionRef& subscriptionRef) const;

    /**
        Sends enable subscription request for the specified subscription
        to each provider in the list.

        @param   indicationProviders   list of providers with associated classes
        @param   nameSpace             the namespace name
        @param   propertyList          the properties referenced by the
                                           subscription
        @param   condition             the condition part of the filter query
        @param   queryLanguage         the query language in which the filter
                                           query is expressed
        @param   subscription          the subscription to be enabled
     */
    void _sendEnableRequests (
        const Array <struct ProviderClassList> & indicationProviders,
        const String & nameSpace,
        const CIMPropertyList & propertyList,
        const String & condition,
        const String & queryLanguage,
        const CIMInstance & subscription);

    /**
        Sends modify subscription request for the specified subscription
        to each provider in the list.

        @param   indicationProviders   list of providers with associated classes
        @param   nameSpace             the namespace name
        @param   propertyList          the properties referenced by the
                                           subscription
        @param   condition             the condition part of the filter query
        @param   queryLanguage         the query language in which the filter
                                           query is expressed
        @param   subscription          the subscription to be modified
     */
    void _sendModifyRequests (
        const Array <struct ProviderClassList> & indicationProviders,
        const String & nameSpace,
        const CIMPropertyList & propertyList,
        const String & condition,
        const String & queryLanguage,
        const CIMInstance & subscription);

    /**
        Sends disable subscription request for the specified subscription
        to each provider in the list.

        @param   indicationProviders   list of providers with associated classes
        @param   nameSpace             the namespace name
        @param   subscription          the subscription to be modified
     */
    void _sendDisableRequests (
        const Array <struct ProviderClassList> & indicationProviders,
        const String & nameSpace,
        const CIMInstance & subscription);

    /**
        Creates an alert instance of the specified class.

        @param   alertClassName        the alert class name
        @param   subscriptionRefs      subscriptions for which alert is to be
                                           created

        @return  the created alert instance
     */
    CIMInstance _createAlertInstance (
        const String & alertClassName,
        const Array <struct SubscriptionRef> & subscriptionRefs);

    /**
        Sends specified alert to each unique handler instance for the
        specified subscriptions in the list.

        @param   subscriptionRefs      subscriptions for which alert is to be
                                           sent
        @param   alertInstance         the alert to be sent
     */
    void _sendAlerts (
        const Array <struct SubscriptionRef> & subscriptionRefs,
        /* const */ CIMInstance & alertInstance);

    WQLSimplePropertySource _getPropertySourceFromInstance(
        CIMInstance & indicationInstance);

    /**
        The name of the indication subscription class
     */
    static const char   _CLASS_SUBSCRIPTION [];

    /**
        The name of the indication filter class
     */
    static const char   _CLASS_FILTER [];

    /**
        The name of the indication handler class
     */
    static const char   _CLASS_HANDLER [];

    /**
        The name of the CIMXML Indication Handler class
     */
    static const char   _CLASS_HANDLERCIMXML [];

    /**
        The name of the SNMP Indication Handler class
     */
    static const char   _CLASS_HANDLERSNMP [];

    /**
        The name of the Indication class
     */
    static const char   _CLASS_INDICATION [];

    /**
        The name of the CIMOM Shutdown alert indication class
     */
    static const char   _CLASS_CIMOM_SHUTDOWN_ALERT [];

    /**
        The name of the No Provider alert indication class
     */
    static const char   _CLASS_NO_PROVIDER_ALERT [];

    /**
        The name of the CIMOM shutdown alert indication class
     */
    static const char   _CLASS_PROVIDER_TERMINATED_ALERT [];

    /**
        The name of the Provider Capabilities class
     */
    static const char   _CLASS_PROVIDER_CAPABILITIES [];

    /**
        The name of the Subscription State property for Indication Subscription
        class
     */
    static const char   _PROPERTY_STATE [];

    /**
        The name of the Other Subscription State property for Indication
        Subscription class
     */
    static const char   _PROPERTY_OTHERSTATE [];

    /**
        The name of the repeat notification policy property for indication
        subscription class
     */
    static const char   _PROPERTY_REPEATNOTIFICATIONPOLICY [];

    /**
        The name of the other repeat notification policy property for
        indication subscription class
     */
    static const char   _PROPERTY_OTHERREPEATNOTIFICATIONPOLICY [];

    /**
        The name of the repeat notification interval property for indication
        subscription class
     */
    static const char   _PROPERTY_REPEATNOTIFICATIONINTERVAL [];

    /**
        The name of the repeat notification gap property for indication
        subscription class
     */
    static const char   _PROPERTY_REPEATNOTIFICATIONGAP [];

    /**
        The name of the repeat notification count property for indication
        subscription class
     */
    static const char   _PROPERTY_REPEATNOTIFICATIONCOUNT [];

    /**
        The name of the On Fatal Error Policy property for Indication 
        Subscription class
     */
    static const char   _PROPERTY_ONFATALERRORPOLICY [];

    /**
        The name of the Other On Fatal Error Policy property for Indication
        Subscription class
     */
    static const char   _PROPERTY_OTHERONFATALERRORPOLICY [];

    /**
        The name of the filter reference property for indication subscription
        class
     */
    static const char   _PROPERTY_FILTER [];

    /**
        The name of the handler reference property for indication subscription
        class
     */
    static const char   _PROPERTY_HANDLER [];

    /**
        The name of the query property for indication filter class
     */
    static const char   _PROPERTY_QUERY [];

    /**
        The name of the query language property for indication filter class
     */
    static const char   _PROPERTY_QUERYLANGUAGE [];

    /**
        The name of the Source Namespace property for indication filter class
     */
    static const char   _PROPERTY_SOURCENAMESPACE [];

    /**
        The name of the name property for indication filter and indication
        handler classes
     */
    static const char   _PROPERTY_NAME [];

    /**
        The name of the creation class name property for indication filter and
        indication handler classes
     */
    static const char   _PROPERTY_CREATIONCLASSNAME [];

    /**
        The name of the system name property for indication filter and
        indication handler classes
     */
    static const char   _PROPERTY_SYSTEMNAME [];

    /**
        The name of the system creation class name property for indication
        filter and indication handler classes
     */
    static const char   _PROPERTY_SYSTEMCREATIONCLASSNAME [];

    /**
        The name of the Persistence Type property for Indication Handler class
     */
    static const char   _PROPERTY_PERSISTENCETYPE [];

    /**
        The name of the Other Persistence Type property for Indication Handler
        class
     */
    static const char   _PROPERTY_OTHERPERSISTENCETYPE [];

    /**
        The name of the Destination property for Indication Handler subclasses
     */
    static const char   _PROPERTY_DESTINATION [];

    /**
        The name of the SNMP Type property for SNMP Indication Handler class
     */
    static const char   _PROPERTY_SNMPTYPE [];

    /**
        The name of the Alert Type property for Alert Indication class
     */
    static const char   _PROPERTY_ALERTTYPE [];

    /**
        The name of the Other Alert Type property for Alert Indication class
     */
    static const char   _PROPERTY_OTHERALERTTYPE [];

    /**
        The name of the Perceived Severity property for Alert Indication class
     */
    static const char   _PROPERTY_PERCEIVEDSEVERITY [];

    /**
        The name of the Probable Cause property for Alert Indication class
     */
    static const char   _PROPERTY_PROBABLECAUSE [];

    /**
        The name of the Creator property for a class
     */
    static const char   _PROPERTY_CREATOR [];

    /**
        The name of the Provider Name property for Provider Capabilities class
     */
    static const char   _PROPERTY_PROVIDER_NAME [];

    /**
        The name of the Class Name property for Provider Capabilities class
     */
    static const char   _PROPERTY_CLASS_NAME [];

    /**
        The name of the Supported Properties property for Provider Capabilities
        class
     */
    static const char   _PROPERTY_SUPPORTED_PROPERTIES [];

    /**
        The name of the Provider Type property for Provider Capabilities class
     */
    static const char   _PROPERTY_PROVIDER_TYPE [];

    /**
        The integer representing the Indication value for the Provider Type
        property of the Provider Capabilities class
     */
    static const Uint16 _VALUE_INDICATION;

    /**
        The WHERE keyword in WQL
     */
    static const char   _QUERY_WHERE [];

    /**
        The string representing the asterisk all properties symbol in WQL
     */
    static const char   _QUERY_ALLPROPERTIES [];

    /**
        The string identifying the service name of the Provider Manager Service
     */
    static const char _SERVICE_PROVIDERMANAGER [];

    /**
        The string identifying the service name of the Repository Service
     */
    static const char _SERVICE_REPOSITORY [];

    /**
        The string identifying the service name of the Handler Manager Service
     */
    static const char _SERVICE_HANDLERMANAGER [];
};

PEGASUS_NAMESPACE_END

#endif
