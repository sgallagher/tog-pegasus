//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
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
//%/////////////////////////////////////////////////////////////////////////////

#include "IndicationConstants.h"


PEGASUS_NAMESPACE_BEGIN
/**
    The name of the CIMOM Shutdown alert indication class
 */
//
//  ATTN: Update once CimomShutdownAlertIndication has been defined
//
const CIMName _CLASS_CIMOM_SHUTDOWN_ALERT =
    CIMName("CIM_AlertIndication");

/**
    The name of the No Provider alert indication class
 */
//
//  ATTN: Update once NoProviderAlertIndication has been defined
//
const CIMName _CLASS_NO_PROVIDER_ALERT = CIMName("CIM_AlertIndication");

/**
    The name of the CIMOM shutdown alert indication class
 */
//
//  ATTN: Update once ProviderTerminatedAlertIndication has been defined
//
const CIMName _CLASS_PROVIDER_TERMINATED_ALERT =
    CIMName("CIM_AlertIndication");


//
//  Property names
//

/**
    The name of the Other Subscription State property for Indication
    Subscription class
 */
const CIMName _PROPERTY_OTHERSTATE = CIMName("OtherSubscriptionState");

/**
    The name of the Repeat Notification Policy property for indication
    subscription class
 */
const CIMName _PROPERTY_REPEATNOTIFICATIONPOLICY =
    CIMName("RepeatNotificationPolicy");

/**
    The name of the Other Repeat Notification Policy property for
    indication subscription class
 */
const CIMName _PROPERTY_OTHERREPEATNOTIFICATIONPOLICY =
    CIMName("OtherRepeatNotificationPolicy");

/**
    The name of the On Fatal Error Policy property for Indication Subscription
    class
 */
const CIMName _PROPERTY_ONFATALERRORPOLICY =
    CIMName("OnFatalErrorPolicy");

/**
    The name of the Other On Fatal Error Policy property for Indication
    Subscription class
 */
const CIMName _PROPERTY_OTHERONFATALERRORPOLICY =
    CIMName("OtherOnFatalErrorPolicy");

/**
    The name of the Failure Trigger Time Interval property for Indication
    Subscription class
 */
const CIMName _PROPERTY_FAILURETRIGGERTIMEINTERVAL =
    CIMName("FailureTriggerTimeInterval");

/**
    The name of the Time Of Last State Change property for Indication
    Subscription class
 */
const CIMName _PROPERTY_LASTCHANGE = CIMName("TimeOfLastStateChange");

/**
    The name of the Subscription Start Time property for Indication
    Subscription class
 */
const CIMName _PROPERTY_STARTTIME = CIMName("SubscriptionStartTime");

/**
    The name of the Subscription Duration property for Indication
    Subscription class
 */
const CIMName _PROPERTY_DURATION = CIMName("SubscriptionDuration");

/**
    The name of the Subscription Time Remaining property for Indication
    Subscription class
 */
const CIMName _PROPERTY_TIMEREMAINING =
    CIMName("SubscriptionTimeRemaining");

/**
    The name of the Repeat Notification Interval property for indication
    subscription class
 */
const CIMName _PROPERTY_REPEATNOTIFICATIONINTERVAL =
    CIMName("RepeatNotificationInterval");

/**
    The name of the Repeat Notification Gap property for indication
    subscription class
 */
const CIMName _PROPERTY_REPEATNOTIFICATIONGAP =
    CIMName("RepeatNotificationGap");

/**
    The name of the Repeat Notification Count property for indication
    subscription class
 */
const CIMName _PROPERTY_REPEATNOTIFICATIONCOUNT =
    CIMName("RepeatNotificationCount");

/**
    The name of the TextFormatOwningEntity property for Formatted Indication
    Subscription class
*/
const CIMName _PROPERTY_TEXTFORMATOWNINGENTITY =
    CIMName("TextFormatOwningEntity");

/**
    The name of the TextFormatID property for Formatted Indication
    Subscription class
*/
const CIMName _PROPERTY_TEXTFORMATID = CIMName("TextFormatID");

/**
    The name of the Caption property for Managed Element class
 */
const CIMName _PROPERTY_CAPTION = CIMName("Caption");

/**
    The name of the Description property for Managed Element class
 */
const CIMName _PROPERTY_DESCRIPTION = CIMName("Description");

/**
    The name of the ElementName property for Managed Element class
 */
const CIMName _PROPERTY_ELEMENTNAME = CIMName("ElementName");

/**
    The name of the Source Namespace property for indication filter class
 */
const CIMName _PROPERTY_SOURCENAMESPACE = CIMName("SourceNamespace");

/**
    The name of the System Name property for indication filter and indications
    handler classes
 */
const CIMName _PROPERTY_SYSTEMNAME = CIMName("SystemName");

/**
    The name of the System Creation Class Name property for indication filter
    and indications handler classes
 */
const CIMName _PROPERTY_SYSTEMCREATIONCLASSNAME =
    CIMName("SystemCreationClassName");

/**
    The name of the Other Persistence Type property for Indication Handler
    class
 */
const CIMName _PROPERTY_OTHERPERSISTENCETYPE =
    CIMName("OtherPersistenceType");

/**
    The name of the Owner property for Indication Handler class
 */
const CIMName _PROPERTY_OWNER = CIMName("Owner");

/**
    The name of the TargetHostFormat property for SNMP Mapper Indication
    Handler subclass
 */
const CIMName _PROPERTY_TARGETHOSTFORMAT = CIMName("TargetHostFormat");

/**
    The name of the OtherTargetHostFormat property for SNMP Mapper Indication
    Handler subclass
 */
const CIMName _PROPERTY_OTHERTARGETHOSTFORMAT =
    CIMName("OtherTargetHostFormat");

/**
    The name of the Port Number property for SNMP Mapper Indication Handler
    subclass
 */
const CIMName _PROPERTY_PORTNUMBER = CIMName("PortNumber");

/**
    The name of the SNMP Security Name property for SNMP Mapper Indication
    Handler subclass
 */
const CIMName _PROPERTY_SNMPSECURITYNAME = CIMName("SNMPSecurityName");

/**
    The name of the SNMP Engine ID property for SNMP Mapper Indication Handler
    subclass
 */
const CIMName _PROPERTY_SNMPENGINEID = CIMName("SNMPEngineID");

/**
    The name of the Alert Type property for Alert Indication class
 */
const CIMName _PROPERTY_ALERTTYPE = CIMName("AlertType");

/**
    The name of the Other Alert Type property for Alert Indication class
 */
const CIMName _PROPERTY_OTHERALERTTYPE = CIMName("OtherAlertType");

/**
    The name of the Perceived Severity property for Alert Indication class
 */
const CIMName _PROPERTY_PERCEIVEDSEVERITY =
    CIMName("PerceivedSeverity");

/**
    The name of the Probable Cause property for Alert Indication class
 */
const CIMName _PROPERTY_PROBABLECAUSE = CIMName("ProbableCause");

/**
    The name of the CreationClassName property of CIM_Service class.
*/
const CIMName _PROPERTY_CREATIONCLASSNAME =
    CIMName("CreationClassName");

/**
    The name of the Name property of CIM_Service class.
*/
const CIMName _PROPERTY_NAME = CIMName("Name");

/**
    The name of the Started property of CIM_Service class.
*/
const CIMName _PROPERTY_STARTED = CIMName("Started");

/**
    The name of the  InstanceID property of CIM_Capabilities
    class.
*/
const CIMName _PROPERTY_INSTANCEID = CIMName("InstanceID");

#ifdef PEGASUS_ENABLE_DMTF_INDICATION_PROFILE_SUPPORT
/**
    The name of the FilterCreationEnabled property of IndicationService class.
*/
const CIMName _PROPERTY_FILTERCREATIONENABLED =
    CIMName("FilterCreationEnabled");

/**
    The name of the SubscriptionRemovalAction property of IndicationService
    class.
*/
const CIMName _PROPERTY_SUBSCRIPTIONREMOVALACTION =
    CIMName("SubscriptionRemovalAction");

/**
    The name of the SubscriptionRemovalTimeInterval property of
    IndicationService class.
*/
const CIMName _PROPERTY_SUBSCRIPTIONREMOVALTIMEINTERVAL =
    CIMName("SubscriptionRemovalTimeInterval");

/**
    The name of the DeliveryRetryAttempts property of
    IndicationService class.
*/
const CIMName _PROPERTY_DELIVERYRETRYATTEMPTS =
    CIMName("DeliveryRetryAttempts");

/**
    The name of the DeliveryRetryInterval property of
    IndicationService class.
*/
const CIMName _PROPERTY_DELIVERYRETRYINTERVAL =
    CIMName("DeliveryRetryInterval");
/**
    The name of the FilterCreationEnabledIsSettable property of
    IndicationServiceCapabilities class.
*/
const CIMName _PROPERTY_FILTERCREATIONENABLEDISSETTABLE =
    CIMName("FilterCreationEnabledIsSettable");

/**
    The name of the DeliveryRetryAttemptsIsSettable property of
    IndicationServiceCapabilities class.
*/
const CIMName _PROPERTY_DELIVERYRETRYATTEMPTSISSETTABLE =
    CIMName("DeliveryRetryAttemptsIsSettable");

/**
    The name of the DeliveryRetryIntervalIsSettable property of
    IndicationServiceCapabilities class.
*/
const CIMName _PROPERTY_DELIVERYRETRYINTERVALISSETTABLE =
    CIMName("DeliveryRetryIntervalIsSettable");

/**
    The name of the SubscriptionRemovalActionIsSettable property of
    IndicationServiceCapabilities class.
*/
const CIMName _PROPERTY_SUBSCRIPTIONREMOVALACTIONISSETTABLE =
    CIMName("SubscriptionRemovalActionIsSettable");

/**
    The name of the SubscriptionRemovalTimeIntervalIsSettable property of
    IndicationServiceCapabilities class.
*/
const CIMName _PROPERTY_SUBSCRIPTIONREMOVALTIMEINTERVALISSETTABLE =
    CIMName("SubscriptionRemovalTimeIntervalIsSettable");

/**
    The name of the MaxListenerDestinations property of
    IndicationServiceCapabilities class.
*/
const CIMName _PROPERTY_MAXLISTENERDESTINATIONS =
    CIMName("MaxListenerDestinations");

/**
    The name of the MaxActiveSubscriptions property of
    IndicationServiceCapabilities class.
*/
const CIMName _PROPERTY_MAXACTIVESUBSCRIPTIONS =
    CIMName("MaxActiveSubscriptions");

/**
    The name of the SubscriptionsPersisted property of
    IndicationServiceCapabilities class.
*/
const CIMName _PROPERTY_SUBSCRIPTIONSPERSISTED =
    CIMName("SubscriptionsPersisted");

#endif

//
//  Qualifier names
//

/**
    The name of the Indication qualifier for classes
 */
const CIMName _QUALIFIER_INDICATION = CIMName("INDICATION");


//
//  Other literal values
//

/**
    The WHERE keyword in WQL
 */
const char   _QUERY_WHERE []         = "WHERE";

/**
    A zero value CIMDateTime interval
 */
const char _ZERO_INTERVAL_STRING [] = "00000000000000.000000:000";

PEGASUS_NAMESPACE_END
