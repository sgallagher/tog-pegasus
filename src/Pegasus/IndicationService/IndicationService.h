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
// Author: Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
//
// Modified By:  Carol Ann Krug Graves, Hewlett-Packard Company
//                   (carolann_graves@hp.com)
//               Ben Heilbronn, Hewlett-Packard Company (ben_heilbronn@hp.com)
//               Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//               Yi Zhou, Hewlett-Packard Company (yi.zhou@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_IndicationService_h
#define Pegasus_IndicationService_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/MessageQueueService.h>
#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Common/IPC.h>
#include <Pegasus/Common/AcceptLanguages.h> // l10n
#include <Pegasus/Common/ContentLanguages.h> // l10n
#include <Pegasus/Server/ProviderRegistrationManager/ProviderRegistrationManager.h>
#include <Pegasus/Server/Linkage.h>
#include <Pegasus/Query/QueryExpression/QueryExpression.h>

#include "ProviderClassList.h"
#include "IndicationOperationAggregate.h"

PEGASUS_NAMESPACE_BEGIN

class SubscriptionRepository;
class SubscriptionTable;

/**

    IndicationService class is the service that serves the
    Indication Subscription, Indication Filter, and Indication Handler
    classes, and processes indications.

    @author  Hewlett-Packard Company

 */

class PEGASUS_SERVER_LINKAGE IndicationService : public MessageQueueService
{
public:

    /**
        Constructs an IndicationSubscription instance and initializes instance
        variables.
     */
    IndicationService (
        CIMRepository * repository,
        ProviderRegistrationManager * providerRegManager);

    virtual ~IndicationService(void);

    void handleEnqueue(Message* message);

    virtual void handleEnqueue(void);

    virtual void _handle_async_request(AsyncRequest *req);

    /**
        Gets a String containing the Provider Name, for use in a log message to
        identify the provider.

        @param   provider              provider instance

        @return  String containing the Provider Name
     */
    static String getProviderLogString
        (CIMInstance & provider);

    AtomicInt dienow;

    /**
        Operation types for the NotifyProviderRegistration message
     */
    enum Operation {OP_CREATE = 1, OP_DELETE = 2, OP_MODIFY = 3};

    static Mutex _mutex;

private:

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
        Asynchronous callback function for _handleProcessIndicationRequest.
        The response from the Handler is checked, and if it is not success, the
        subscription's On Fatal Error Policy is implemented.

        @param  operation            shared data structure that controls message
                                         processing
        @param  destination          target queue of completion callback
        @param  userParameter        user parameter for callback processing
     */
    static void _handleIndicationCallBack (
        AsyncOpNode * operation,
        MessageQueue * destination,
        void * userParameter);

    /**
	Notifies the Indication Service that a change in provider registration
	has occurred.  The Indication Service retrieves the subscriptions
	affected by the registration change, sends the appropriate Create,
        Modify, and/or Delete requests to the provider, and sends an alert to
        handler instances of subscriptions that are no longer served by the
        provider.
    */
    void _handleNotifyProviderRegistrationRequest(const Message * message);

    /**
        Notifies the Indication Service that a provider has been disabled.
        The Indication Service retrieves the subscriptions served by the
        disabled provider, and logs a message for each subscription that is no
        longer served by the provider.
     */
    void _handleNotifyProviderTerminationRequest(const Message * message);

    /**
        Notifies the Indication Service that a provider has been enabled.
        The Indication Service retrieves the subscriptions that can be served
        by the enabled provider, sends Create Subscription and Enable
        Indications requests to the provider, and logs a message for each
        subscription that is now served by the provider.
     */
    void _handleNotifyProviderEnableRequest (const Message * message);

    /**
        Determines if it is legal to create an instance.
        Checks for existence of all key and required properties.  Checks that
        properties that MUST NOT exist (based on values of other properties),
        do not exist.  For any property that has a default value, if it does
        not exist, adds the property with the default value.

        @param   instance              instance to be created
        @param   nameSpace             namespace for instance to be created

        @exception   CIM_ERR_INVALID_PARAMETER  if instance is invalid
        @exception   CIM_ERR_NOT_SUPPORTED      if the specified class is not
                                                supported

        @return  True, if the instance can be created;
                 Otherwise throws an exception
     */
    Boolean _canCreate (
        CIMInstance & instance,
        const CIMNamespaceName & nameSpace);

    /**
        Validates the specified required property in the instance.
        If the property does not exist, or has a null value, or is not of the
        expected type, an exception is thrown, using the specified message.

        This function is called by the _canCreate function, and is used to
        validate the  Filter and Handler properties in Subscription instances,
        the Name, Query and Query Language properties in
        Filter instances, the Name and Destination
        properties in CIMXML Handler instances, and the Name,
        Trap Destination, and SNMP Version properties in SNMP Mapper instances.

        @param   instance              instance to be validated
        @param   propertyName          name of property to be validated
        @param   expectedType          expected CIMType of property value
        @param   message               message to be used in exception
	@param   isArray               indicates whether the validated
				       property is array

        @exception   CIM_ERR_INVALID_PARAMETER  if required property is missing
                                                or null
     */
    void _checkRequiredProperty (
        CIMInstance & instance,
        const CIMName & propertyName,
        const CIMType expectedType,
        const String & message,
	const Boolean isArray = false);

    /**
        Validates the specified Uint16 (non-array) property and its
        corresponding String (non-array) Other___ property in the instance.
        If the property does not exist, it is added with the default value.
        If the property exists, but its value is NULL, its value is set to
        the default value.
        If the value of the property is Other, but the corresponding Other___
        property either does not exist, has a value of NULL, or is not of the
        correct type, an exception is thrown.
        If the value of the property is not Other, but the corresponding
        Other___ property exists and has a non-NULL value, an exception is
        thrown.
        If the value of the property is not a supported value, an exception is
        thrown.
        This function is called by the _canCreate function, and is used to
        validate the following pairs of properties in Subscription or Handler
        instances: Subscription State, Other Subscription State, Repeat
        Notification Policy, Other Repeat Notification Policy, On Fatal Error
        Policy, Other On Fatal Error Policy, Persistence Type, Other
        Persistence Type.

        @param   instance              instance to be validated
        @param   propertyName          name of property to be validated
        @param   otherPropertyName     name of Other___ property to be validated
        @param   defaultValue          default value for property
        @param   otherValue            "Other" value for property
        @param   validValues           set of valid values for property
        @param   supportedValues       set of supported values for property

        @exception   CIM_ERR_INVALID_PARAMETER  if value of property or Other___
                                                property is invalid
     */
    void _checkPropertyWithOther (
        CIMInstance & instance,
        const CIMName & propertyName,
        const CIMName & otherPropertyName,
        const Uint16 defaultValue,
        const Uint16 otherValue,
        const Array <Uint16> & validValues,
        const Array <Uint16> & supportedValues);

    /**
        Validates the specified property in the instance.
        If the property does not exist, it is added with the default value.
        If the property exists, but its value is NULL, its value is set to
        the default value.
        This function is called by the _canCreate function, and is used to
        validate the Source Namespace property in Filter instances.
        This function is also called by the _initOrValidateStringProperty
        function to validate the CreationClassName, SystemName, and
        SystemCreationClassName key properties in Filter and Handler instances.

        Note: currently all properties validated by this function are of type
        String.  To use this function in the future with properties of other
        types, a type parameter would need to be added, and the default value
        would need to be passed as a CIMValue instead of a String.

        Note: currently all properties validated by this function are non-array
        properties.  To use this function in the future with both array and
        non-array properties, a Boolean isArray parameter would need to be
        added.

        @param   instance              instance to be validated
        @param   propertyName          name of property to be validated
        @param   defaultValue          default value for property

        @return  the value of the property
     */
    String _checkPropertyWithDefault (
        CIMInstance & instance,
        const CIMName & propertyName,
        const String & defaultValue);

    /**
        Validates the specified property in the instance.
        If the property does not exist, it is added with the default value.
        If the property exists, but its value is NULL, its value is set to
        the default value.
        If the property exists and has a non-NULL value, its value is validated
        against the default (expected) value.  If the value is invalid, an
        exception is thrown.
        This function is called by the _canCreate function, and is used to
        validate the Creation Class Name, System Name and System Creation Class
        Name properties in Filter and Handler instances.

        Note: currently all properties validated by this function are of type
        String.  To use this function in the future with properties of other
        types, a type parameter would need to be added, and the default value
        would need to be passed as a CIMValue instead of a String.

        Note: currently all properties validated by this function are non-array
        properties.  To use this function in the future with both array and
        non-array properties, a Boolean isArray parameter would need to be
        added.

        @param   instance              instance to be validated
        @param   propertyName          name of property to be validated
        @param   defaultValue          default value for property

        @return  the value of the property
     */
    String _initOrValidateStringProperty (
        CIMInstance & instance,
        const CIMName & propertyName,
        const String & defaultValue);

    /**
        Validates the specified property in the instance.
        If the property exists and its value is not NULL, but it is not of
        the correct type, an exception is thrown.
        This function is called by the _canCreate function.  It is used to
        validate the FailureTriggerTimeInterval, TimeOfLastStateChange,
        SubscriptionDuration, SubscriptionStartTime, SubscriptionTimeRemaining,
        RepeatNotificationInterval, RepeatNotificationGap, and
        RepeatNotificationCount properties in Subscription instances, the Owner
        property in Handler instances, and the PortNumber, SNMPSecurityName,
        and SNMPEngineID properties in SNMP Mapper Handler instances.

        Note: currently all properties validated by this function are non-array
        properties.  To use this function in the future with both array and
        non-array properties, a Boolean isArray parameter would need to be
        added.

        @param   instance              instance to be validated
        @param   propertyName          name of property to be validated
        @param   expectedType          expected CIMType for property
        @param   isArray               indicates whether the validated
                                       property is array

        @exception   CIM_ERR_INVALID_PARAMETER  if property exists and is not
                                                null but is not of the correct
                                                type
     */
    void _checkProperty (
        CIMInstance & instance,
        const CIMName & propertyName,
        const CIMType expectedType,
        const Boolean isArray = false);

    /**
        Validates that all properties in the instance are supported properties,
        and throws an exception if an unknown, unsupported property is found.

        @param   instance              instance to be validated

        @exception   CIM_ERR_NOT_SUPPORTED      if instance includes an unknown,
                                                unsupported property
     */
    void _checkSupportedProperties (
        const CIMInstance & instance);

    /**
        Determines if the user is authorized to modify the instance, and if the
        specified modification is supported.  Currently, the only modification
        supported is of the Subscription State property of the Subscription
        class.

        @param   request               modification request
        @param   instance              instance to be modified
        @param   modifiedInstance      modified instance

        @exception   CIM_ERR_NOT_SUPPORTED      if the specified modification is
                                                not supported
        @exception   CIM_ERR_ACCESS_DENIED      if the user is not authorized to
                                                modify the instance
        @exception   CIM_ERR_INVALID_PARAMETER  if the modifiedInstance is
                                                invalid

        @return  True, if the instance can be modified;
                 Otherwise throws an exception
     */
    Boolean _canModify (
        const CIMModifyInstanceRequestMessage * request,
        const CIMObjectPath & instanceReference,
        const CIMInstance & instance,
        CIMInstance & modifiedInstance);

    /**
        Determines if the user is authorized to delete the instance, and if it
        is legal to delete the instance.  If authorized, Subscription instances
        may always be deleted.  Filter and non-transient Handler instances may
        only be deleted if they are not being referenced by any Subscription
        instances. If the instance to be deleted is a transient Handler, any
        referencing Subscriptions are also deleted.

        @param   instanceReference     reference for instance to be deleted
        @param   nameSpace             namespace for instance to be deleted
        @param   currentUser           current user

        @exception   CIM_ERR_ACCESS_DENIED      if the user is not authorized to
                                                delete the instance
        @exception   CIM_ERR_FAILED             if the instance to be deleted is
                                                referenced by a subscription

        @return  True, if the instance can be deleted;
                 Otherwise throws an exception
     */
    Boolean _canDelete (
        const CIMObjectPath & instanceReference,
        const CIMNamespaceName & nameSpace,
        const String & currentUser);

    /**
        Retrieves list of enabled subscription instances in all namespaces,
        where the subscription indication class matches or is a superclass
        of the supported class, and the properties required to process the
        subscription are all contained in the list of supported properties.
        If the checkProvider parameter value is True, a subscription is only
        included in the list returned if the specified provider accepted the
        subscription.  If the checkProvider parameter value is False, the
        provider parameter is not used (ignored).

        @param   supportedClass       the supported class
        @param   nameSpaces           the list of supported namespaces
        @param   supportedProperties  the list of supported properties
        @param   checkProvider        indicates whether provider acceptance is
                                          checked
        @param   provider             the provider (used if checkProvider True)

        @return   list of CIMInstance subscriptions
     */
    Array <CIMInstance> _getMatchingSubscriptions (
        const CIMName & supportedClass,
        const Array <CIMNamespaceName> nameSpaces,
        const CIMPropertyList & supportedProperties,
        const Boolean checkProvider = false,
        const CIMInstance & provider = CIMInstance ());

    /**
        Retrieves lists of enabled subscription instances in all namespaces
        that are either newly supported or previously supported, based on the
        supported class, the supported namespaces before and after modification,
        and the supported properties before and after modification.  For
        subscriptions based on the supported class, the newSubscriptions list
        returned contains the subscriptions for which the properties required
        to process the subscription are all contained in the new list of
        supported properties, but are not all contained in the old list of
        supported properties, and/or the filter source namespace is contained in
        the new list if supported namespaces, but is not contained in the old
        list of supported namespaces.  The formerSubscriptions list returned
        contains the subscriptions for which the properties required to process
        the subscription are not all contained in the new list of supported
        properties, but are all contained in the old list of supported
        properties, and/or the filter source namespace is not contained in the
        new list if supported namespaces, but is contained in the old list of
        supported namespaces.

        @param   supportedClass       the supported class
        @param   newNameSpaces        namespaces supported after modification
        @param   oldNameSpaces        namespaces supported before modification
        @param   newProperties        properties supported after modification
        @param   oldProperties        properties supported before modification
        @param   newSubscriptions     the list of newly supported subscriptions
        @param   formerSubscriptions  the list of previously supported
                                          subscriptions
     */
    void _getModifiedSubscriptions (
        const CIMName & supportedClass,
        const Array <CIMNamespaceName> & newNameSpaces,
        const Array <CIMNamespaceName> & oldNameSpaces,
        const CIMPropertyList & newProperties,
        const CIMPropertyList & oldProperties,
        Array <CIMInstance> & newSubscriptions,
        Array <CIMInstance> & formerSubscriptions);

    /**
        Determines if all of the required properties in the specified list
        are contained in the specified list of supported properties.

        @param   requiredProperties  the required properties
        @param   supportedProperties the supported properties

        @return   true, if all required properties are supported;
                  false otherwise
     */
    Boolean _inPropertyList (
        const CIMPropertyList & requiredProperties,
        const CIMPropertyList & supportedProperties);

    /**
        Builds a QueryExpression from the filter query string,
        the query language name, and the namespace in which the query
        is to be run.

        @param   filterQuery           the filter query string
        @param   queryLanguage         the query language name
        @param   ns                    query namespace

        @return  QueryExpression representing the filter query
     */
    QueryExpression _getQueryExpression (const String& filterQuery,
                                         const String& queryLanguage,
                                         const CIMNamespaceName ns) const;

    /**
        Extracts the indication class name from the specified query expression
        (WQL or CQL), and validates that the name represents a subclass of the
        Indication class.

        @param   queryExpression       the query expression
        @param   nameSpaceName         the namespace

        @return  String containing the indication class name
     */
    CIMName _getIndicationClassName (
        const QueryExpression & queryExpression,
        const CIMNamespaceName & nameSpaceName) const;

    /**
        Retrieves the list of indication providers that serve the specified
        indication subclasses.

        @param   queryExpression       the query expression
        @param   nameSpace             the namespace name
        @param   indicationClassName   the indication class name
        @param   indicationSubclasses  the list of indication subclass names

        @return  list of ProviderClassList structs
     */
    Array <ProviderClassList> _getIndicationProviders (
        const QueryExpression & queryExpression,
        const CIMNamespaceName & nameSpace,
        const CIMName & indicationClassName,
        const Array <CIMName> & indicationSubclasses) const;

    /**
        Retrieves the list of required properties (all the properties
        referenced in the WHERE clause) for the specified filter query
        expression.

        @param   queryExpression       the query expression
        @param   nameSpaceName         the namespace
        @param   indicationClassName   the indication class name

        @return  CIMPropertyList of required properties for the filter query
                 expression
     */
    CIMPropertyList _getPropertyList (
        const QueryExpression & queryExpression,
        const CIMNamespaceName & nameSpaceName,
        const CIMName & indicationClassName) const;

    /**
        Checks if the property list includes all properties in the specified
        class.  If so, a NULL CIMPropertyList is returned.  Otherwise, a
        CIMPropertyList containing the properties is returned.  The list of
        property names in the specified indication class is also returned in
        the indicationClassProperties parameter.

        @param   propertyList                the list of property names
        @param   nameSpaceName               the namespace
        @param   indicationClassName         the indication class name
        @param   indicationClassProperties   the list of property names in the
                                               specified indication class

        @return  CIMPropertyList of properties referenced by the filter query
                 select statement
     */
    CIMPropertyList _checkPropertyList (
        const Array <CIMName> & propertyList,
        const CIMNamespaceName & nameSpaceName,
        const CIMName & indicationClassName,
        Array <CIMName> & indicationClassProperties) const;

    /**
        Extracts the condition (WHERE Clause) from the specified filter query
        string.

        @param   filterQuery       the filter query

        @return  String containing the filter query condition
     */
    String _getCondition (
        const String & filterQuery) const;

    /**
        Deletes subscriptions referencing the specified handler.  All namespaces
        are searched for subscriptions that reference the handler to be deleted.

        @param   nameSpace             the name space of the handler being
                                           deleted
        @param   referenceProperty     the name of the reference property in the
                                           subscription instance
        @param   handler               the handler reference
     */
    void _deleteReferencingSubscriptions (
        const CIMNamespaceName & nameSpace,
        const CIMName & referenceProperty,
        const CIMObjectPath & handler);

    /**
        Determines if specified Subscription has expired

        NOTE: It is assumed that the instance passed to this function is a
        Subscription instance, and that the Subscription Duration and
        Start Time properties exist

        @param   instance              the subscription instance

        @return  True, if the Subscription has expired;
                 False otherwise
     */
    Boolean _isExpired (
        const CIMInstance & instance) const;

    /**
        Deletes specified subscription

        @param   subscription          the subscription reference
     */
    void _deleteExpiredSubscription (
        CIMObjectPath & subscription);

    /**
        Gets the Subscription Time Remaining property

        Calculates time remaining from Subscription Start Time, Subscription
        Duration, and current date time.  If the subscription has a non-null
        Duration, the Time Remaining is set, and True is returned.  If the
        subscription does not have a non-null Duration, it has no expiration
        date, and the time remaining is unlimited.  In this case, the Time
        Remaining is not set and False is returned.

        NOTE: It is assumed that the instance passed to this function is a
        Subscription instance, and that the Start Time property exists and
        has a value

        @param   instance              Input the subscription instance
        @param   timeRemaining         Output the time remaining (seconds)

        @return  True, if the subscription has a non-null Duration;
                 False otherwise
     */
    Boolean _getTimeRemaining (
        const CIMInstance & instance,
        Uint64 & timeRemaining) const;

    /**
        Sets the Subscription Time Remaining property

        Calculates time remaining from Subscription Start Time, Subscription
        Duration, and current date time

        NOTE: It is assumed that the instance passed to this function is a
        Subscription instance, and that the Subscription Duration and
        Start Time properties exist

        @param   instance              the subscription instance
     */
    void _setTimeRemaining (
        CIMInstance & instance);

    /**
        Gets the parameter values required to Create or Modify the subscription
        request.
        If no indication providers are found, condition and queryLanguage are
        set to empty string.

        @param   subscriptionInstance  Input subscription instance
        @param   indicationSubclasses  Output list of subclasses of indication
                                           class in filter query
        @param   indicationProviders   Output list of providers with associated
                                           classes
        @param   propertyList          Output list of properties required by the
                                           subscription
        @param   sourceNameSpace       Output source namespace for filter query
        @param   condition             Output condition part of the filter query
        @param   query                 Output filter query
        @param   queryLanguage         Output query language in which the filter
                                           query is expressed
     */
    void _getCreateParams (
        const CIMInstance & subscriptionInstance,
        Array <CIMName> & indicationSubclasses,
        Array <ProviderClassList> & indicationProviders,
        CIMPropertyList & propertyList,
        CIMNamespaceName & sourceNameSpace,
        String & condition,
        String & query,
        String & queryLanguage);

    /**
        Gets the parameter values required to Create or Modify the subscription
        request.

        @param   subscriptionInstance  Input subscription instance
        @param   indicationSubclasses  Output list of subclasses of indication
                                           class in filter query
        @param   propertyList          Output list of properties required by the
                                           subscription
        @param   sourceNameSpace       Output source namespace for filter query
        @param   condition             Output condition part of the filter query
        @param   query                 Output filter query
        @param   queryLanguage         Output query language in which the filter
                                           query is expressed
     */
    void _getCreateParams (
        const CIMInstance & subscriptionInstance,
        Array <CIMName> & indicationSubclasses,
        CIMPropertyList & propertyList,
        CIMNamespaceName & sourceNameSpace,
        String & condition,
        String & query,
        String & queryLanguage);

    /**
        Gets the parameter values required to Delete the subscription request.

        @param   subscriptionInstance  Input subscription instance
        @param   indicationSubclasses  Output list of subclasses of indication
                                           class in filter query
        @param   sourceNameSpace       Output source namespace for filter query

        @return  List of providers with associated classes to Delete
     */
    Array <ProviderClassList> _getDeleteParams (
        const CIMInstance & subscriptionInstance,
        Array <CIMName> & indicationSubclasses,
        CIMNamespaceName & sourceNameSpace);

    /**
        Sends Create subscription request for the specified subscription
        to each provider in the list.  The requests are sent using SendAsync,
        and the responses are aggregated in the callback methods.  Create
        Subscription requests are sent to the indication providers using
        SendAsync in the following cases: (1) on creation of an enabled
        subscription instance, and (2) on modification of a subscription
        instance, when the state changes to enabled.  In cases (1) and (2),
        there is an original Create Instance or Modify Instance request to
        which the Indication Service must respond.

        @param   indicationProviders   list of providers with associated classes
        @param   nameSpace             the nameSpace name of the resource being
                                           monitored, from the SourceNamespace
                                           property of the CIM_IndicationFilter
                                           instance for the specified
                                           subscription
        @param   propertyList          the properties referenced by the
                                           subscription
        @param   condition             the condition part of the filter query
        @param   query                 the filter query
        @param   queryLanguage         the query language in which the filter
                                           query is expressed
        @param   subscription          the subscription to be created
        @param   acceptLangs           the language of the response, and
                                           future indications
        @param   contentLangs          the language of the subscription
        @param   origRequest           the original request (Create
                                           Instance, Modify Instance)
        @param   indicationSubclasses  the indication subclasses for the
                                           subscription
        @param   userName              the userName for authentication
        @param   authType              the authentication type

     */
    void _sendAsyncCreateRequests (
        const Array <ProviderClassList> & indicationProviders,
        const CIMNamespaceName & nameSpace,
        const CIMPropertyList & propertyList,
        const String & condition,
        const String & query,
        const String & queryLanguage,
        const CIMInstance & subscription,
        const AcceptLanguages & acceptLangs,
        const ContentLanguages & contentLangs,
        const CIMRequestMessage * origRequest,
        const Array <CIMName> & indicationSubclasses,
        const String & userName,
        const String & authType = String::EMPTY);

    /**
        Sends Create subscription request for the specified subscription
        to each provider in the list.  The requests are sent using SendWait,
        so no callback methods are required.  Create Subscription requests are
        sent to the indication providers using SendWait in the following cases:
        (1) on notification of a provider registration change newly enabling
        the provider to serve the subscription, (2) on notification that a
        provider has been enabled and may now serve the subscription, and
        (3) on initialization, for each enabled subscription retrieved from the
        repository.  In cases (1) and (2), there is an original Notify Provider
        Registration or Notify Provider Enable request to which the Indication
        Service must respond.  In case (3), there is no original request and no
        response is required.

        @param   indicationProviders   list of providers with associated classes
        @param   nameSpace             the nameSpace name of the resource being
                                           monitored, from the SourceNamespace
                                           property of the CIM_IndicationFilter
                                           instance for the specified
                                           subscription
        @param   propertyList          the properties referenced by the
                                           subscription
        @param   condition             the condition part of the filter query
        @param   query                 the filter query
        @param   queryLanguage         the query language in which the filter
                                           query is expressed
        @param   subscription          the subscription to be created
        @param   acceptLangs           the language of the response, and
                                           future indications
        @param   contentLangs          the language of the subscription
        @param   userName              the userName for authentication
        @param   authType              the authentication type

        @return  List of providers that accepted subscription
     */
    Array <ProviderClassList> _sendWaitCreateRequests (
        const Array <ProviderClassList> & indicationProviders,
        const CIMNamespaceName & nameSpace,
        const CIMPropertyList & propertyList,
        const String & condition,
        const String & query,
        const String & queryLanguage,
        const CIMInstance & subscription,
        const AcceptLanguages & acceptLangs,
        const ContentLanguages & contentLangs,
        const String & userName,
        const String & authType = String::EMPTY);

    /**
        Sends Modify subscription request for the specified subscription
        to each provider in the list.   The requests are sent using SendWait,
        so no callback methods are required.  Modify Subscription requests must
        be sent to the indication providers on notification of a provider
        registration change, when the provider was formerly serving the
        subscription, and is still serving the subscription, in the following
        cases: (1) the provider is newly serving an additional indication
        subclass specified by the subscription, or (2) the provider is
        no longer serving an indication subclass specified by the subscription
        (but is still serving at least one of the indication subclasses).
        In cases (1) and (2), there is an original Notify Provider Registration
        request to which the Indication Service must respond.

        @param   indicationProviders   list of providers with associated classes
        @param   nameSpace             the nameSpace name of the resource being
                                           monitored, from the SourceNamespace
                                           property of the CIM_IndicationFilter
                                           instance for the specified
                                           subscription
        @param   propertyList          the properties referenced by the
                                           subscription
        @param   condition             the condition part of the filter query
        @param   query                 the filter query
        @param   queryLanguage         the query language in which the filter
                                           query is expressed
        @param   subscription          the subscription to be modified
        @param   acceptLangs           the language of the response, and
                                           future indications
        @param   contentLangs          the language of the subscription
        @param   userName              the userName for authentication
        @param   authType              the authentication type
     */
    void _sendWaitModifyRequests (
        const Array <ProviderClassList> & indicationProviders,
        const CIMNamespaceName & nameSpace,
        const CIMPropertyList & propertyList,
        const String & condition,
        const String & query,
        const String & queryLanguage,
        const CIMInstance & subscription,
        const AcceptLanguages & acceptLangs,
        const ContentLanguages & contentLangs,
        const String & userName,
        const String & authType = String::EMPTY);

    /**
        Sends Delete subscription request for the specified subscription
        to each provider in the list.  The requests are sent using SendAsync,
        and the responses are aggregated in the callback methods.  Delete
        Subscription requests are sent to the indication providers using
        SendAsync in the following cases: (1) on deletion of an enabled
        subscription instance, (2) on modification of a subscription instance,
        when the state changes to disabled, (3) on deletion of an expired
        subscription, and (4) on deletion of a subscription referencing a
        deleted transient handler.  In cases (1) and (2), there is an original
        Delete Instance or Modify Instance request to which the Indication
        Service must respond.  In cases (3) and (4), there is no orginal request
        and no response is required.

        @param   indicationProviders   list of providers with associated classes
        @param   nameSpace             the nameSpace name of the resource being
                                           monitored, from the SourceNamespace
                                           property of the CIM_IndicationFilter
                                           instance for the specified
                                           subscription
        @param   subscription          the subscription to be modified
        @param   acceptLangs           the language of the response
        @param   contentLangs          the language of the subscription
        @param   origRequest           the original request (Delete Instance,
                                           Modify Instance)
        @param   indicationSubclasses  the indication subclasses for the
                                           subscription
        @param   userName              the userName for authentication
        @param   authType              the authentication type
     */
    void _sendAsyncDeleteRequests (
        const Array <ProviderClassList> & indicationProviders,
        const CIMNamespaceName & nameSpace,
        const CIMInstance & subscription,
        const AcceptLanguages & acceptLangs,
        const ContentLanguages & contentLangs,
        const CIMRequestMessage * origRequest,
        const Array <CIMName> & indicationSubclasses,
        const String & userName,
        const String & authType = String::EMPTY);

    /**
        Sends Delete subscription request for the specified subscription
        to each provider in the list.  The requests are sent using SendWait,
        so no callback methods are required.  Delete Subscription requests are
        sent to the indication providers using SendWait in the following case:
        (1) on notification of a provider registration change newly preventing
        the provider from serving the subscription.  In case (1), there is an
        original Notify Provider Registration request to which the Indication
        Service must respond.

        @param   indicationProviders   list of providers with associated classes
        @param   nameSpace             the nameSpace name of the resource being
                                           monitored, from the SourceNamespace
                                           property of the CIM_IndicationFilter
                                           instance for the specified
                                           subscription
        @param   subscription          the subscription to be modified
        @param   acceptLangs           the language of the response
        @param   contentLangs          the language of the subscription
        @param   userName              the userName for authentication
        @param   authType              the authentication type
     */
    void _sendWaitDeleteRequests (
        const Array <ProviderClassList> & indicationProviders,
        const CIMNamespaceName & nameSpace,
        const CIMInstance & subscription,
        const AcceptLanguages & acceptLangs,
        const ContentLanguages & contentLangs,
        const String & userName,
        const String & authType = String::EMPTY);

    /**
        Collects responses from providers for aggregation as they are received,
        and stores them in the IndicationOperationAggregate instance.  Calls
        _handleOperationResponseAggregation to process the responses, once all
        expected responses have been received.

        @param  operation            shared data structure that controls message
                                         processing
        @param  destination          target queue of completion callback
        @param  userParameter        user parameter for callback processing
     */
    static void _aggregationCallBack (
        AsyncOpNode * operation,
        MessageQueue * destination,
        void * userParameter);

    /**
        Calls the appropriate function to processes responses from providers,
        based on the type of request sent to providers, once all responses have
        been received.

        @param   operationAggregate    the operation aggregate instance
     */
    void _handleOperationResponseAggregation (
        IndicationOperationAggregate * operationAggregate);

    /**
        Processes create subscription responses from providers, once all have
        been received.  Takes the appropriate action, based on the type of the
        original request, if any, and the responses received.  Sends the
        response to the original request, if required.

        @param   operationAggregate    the operation aggregate instance
     */
    void _handleCreateResponseAggregation (
        IndicationOperationAggregate * operationAggregate);

    /**
        Processes modify subscription responses from providers, once all have
        been received.  Updates the subscription hash tables.

        @param   operationAggregate    the operation aggregate instance
     */
    void _handleModifyResponseAggregation (
        IndicationOperationAggregate * operationAggregate);

    /**
        Processes delete subscription responses from providers, once all have
        been received.  Updates the subscription hash tables appropriately,
        based on the type of the original request, if any.  Sends the response
        to the original request, if required.

        @param   operationAggregate    the operation aggregate instance
     */
    void _handleDeleteResponseAggregation (
        IndicationOperationAggregate * operationAggregate);

    /**
        Creates an alert instance of the specified class.

        @param   alertClassName        the alert class name
        @param   subscriptions         subscriptions for which alert is to be
                                           created

        @return  the created alert instance
     */
    CIMInstance _createAlertInstance (
        const CIMName & alertClassName,
        const Array <CIMInstance> & subscriptions);

#if 0
     /**
	  Asynchronous completion routine for _sendAlerts
      */

      static void _sendAlertsCallBack(AsyncOpNode *operation,
				      MessageQueue *callback_destination,
				      void *parameter);

    /**
        Sends specified alert to each unique handler instance for the
        specified subscriptions in the list.

        @param   subscriptions         subscriptions for which alert is to be
                                           sent
        @param   alertInstance         the alert to be sent
     */
    void _sendAlerts (
        const Array <CIMInstance> & subscriptions,
        /* const */ CIMInstance & alertInstance);
#endif

    /**
        Sends a Subscription Init Complete request to the Provider
        Manager Service.
     */
    void _sendSubscriptionInitComplete ();

    /**
        Gets the value of the Creator property from the specified Subscription
        instance.  If this function returns False, the value of the creator
        parameter is unchanged.

        @param   instance              subscription instance
        @param   creator               value of Creator property if retrieved

        @return  True, if the value of the Creator property was retrieved;
                 False if Creator property was missing, null, or of an
                       incorrect type
     */
    Boolean _getCreator (
        const CIMInstance & instance,
        String & creator) const;

    /**
        Validates the specified SubscriptionState property value.

        @param   state                 SubscriptionState property value

        @return  True, if the SubscriptionState property value is valid;
                 False otherwise
     */
    Boolean _validateState (
        const Uint16 state) const;

    /**
        This function peforms an authorization test based on the
        value of the enableSubscriptionForNonprivilegedUsers.

        @param   userName                String

        @return  True, if authentication is not enabled or the
                       user is a privileged system user;
                 False, if authentication is enabled and the
                       user is not privileged
     */
    void _checkNonprivilegedAuthorization (
        const String & userName);

    /**
        Updates the propertyList, in preparation for calling the Repository.
        If the propertyList is not null, the Creator property must be added to
        the list.  Also, if the request is for the Subscription class, the
        value of the Time Remaining property need only be calculated if it is
        requested.  In that case, the Subscription Duration and Start Time
        properties must be added to the list if not already there.

        @param   className             class name for the request
        @param   propertyList          list of properties requested
        @param   setTimeRemaining      indicates whether Time Remaining property
                                       was requested
        @param   startTimeAdded        indicates whether Start Time property was
                                       added to the list
        @param   durationAdded         indicates whether Duration property was
                                       added to the list
     */
    void _updatePropertyList
        (CIMName & className,
         CIMPropertyList & propertyList,
         Boolean & setTimeRemaining,
         Boolean & startTimeAdded,
         Boolean & durationAdded);

    /**
        Gets a String containing the comma-separated Subscription Filter Name
        and Handler Name, for use in a log message to identify the subscription.

        @param   subscription          subscription instance

        @return  String containing the comma-separated Subscription Filter Name
                 and Handler Name
     */
    String _getSubscriptionLogString
        (CIMInstance & subscription);

    SubscriptionRepository* _subscriptionRepository;

    SubscriptionTable * _subscriptionTable;

    /**
        Handle to Provider Registration Manager
     */
    ProviderRegistrationManager * _providerRegManager;

    /**
        Pointer to CIMRepository, for use in building QueryExpression.
     */
    CIMRepository* _cimRepository;

    /**
        Integer representing queue ID for accessing Provider Manager Service
     */
    Uint32 _providerManager;

    /**
        Integer representing queue ID for accessing Handler Manager Service
     */
    Uint32 _handlerService;

    /**
        Boolean indicating that the CIM Server has been configured to
        allow non-privileged users read and write access to the
        Subscription classes.
     */
    Boolean _enableSubscriptionsForNonprivilegedUsers;

    /**
	Gets the indication class specified by the subscription filter query.

	@param instance     the subscription instance

	@return CIMClass object for the indication class
    */
    CIMClass _getIndicationClass(
	const CIMInstance & instance);

    /**
        Arrays of valid and supported property values

        Notes:
        Valid values are defined by the CIM Event Schema MOF
        Supported values are a subset of the valid values
        Some valid values, as defined in the MOF, are not currently supported
            by the Pegasus IndicationService

        Supported Values
        SubscriptionState: Enabled, Disabled
        RepeatNotificationPolicy: Unknown, Other, None, Suppress, Delay
        OnFatalErrorPolicy: Ignore, Disable, Remove
        PersistenceType: Permanent, Transient
     */
    Array <Uint16> _validStates;
    Array <Uint16> _validRepeatPolicies;
    Array <Uint16> _validErrorPolicies;
    Array <Uint16> _validPersistenceTypes;
    Array <Uint16> _supportedStates;
    Array <Uint16> _supportedRepeatPolicies;
    Array <Uint16> _supportedErrorPolicies;
    Array <Uint16> _supportedPersistenceTypes;

    /**
        Arrays of names of supported properties for each class
     */
    Array <CIMName> _supportedSubscriptionProperties;
    Array <CIMName> _supportedFormattedSubscriptionProperties;
    Array <CIMName> _supportedFilterProperties;
    Array <CIMName> _supportedCIMXMLHandlerProperties;
    Array <CIMName> _supportedCIMXMLListenerDestinationProperties;
    Array <CIMName> _supportedSNMPHandlerProperties;
    Array <CIMName> _supportedSyslogListenerDestinationProperties;
    Array <CIMName> _supportedEmailListenerDestinationProperties;
};

PEGASUS_NAMESPACE_END

#endif  /* Pegasus_IndicationService_h */
