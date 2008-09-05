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

#include <Pegasus/Common/CIMNameUnchecked.h>
#include "IndicationConstants.h"


PEGASUS_NAMESPACE_BEGIN
/**
    The name of the CIMOM Shutdown alert indication class
 */
//
//  ATTN: Update once CimomShutdownAlertIndication has been defined
//
const CIMName _CLASS_CIMOM_SHUTDOWN_ALERT =
    CIMNameUnchecked("CIM_AlertIndication");

/**
    The name of the No Provider alert indication class
 */
//
//  ATTN: Update once NoProviderAlertIndication has been defined
//
const CIMName _CLASS_NO_PROVIDER_ALERT =
    CIMNameUnchecked("CIM_AlertIndication");

/**
    The name of the CIMOM shutdown alert indication class
 */
//
//  ATTN: Update once ProviderTerminatedAlertIndication has been defined
//
const CIMName _CLASS_PROVIDER_TERMINATED_ALERT =
    CIMNameUnchecked("CIM_AlertIndication");


//
//  Property names
//

/**
    The name of the Other Subscription State property for Indication
    Subscription class
 */
const CIMName _PROPERTY_OTHERSTATE = CIMNameUnchecked("OtherSubscriptionState");

/**
    The name of the Repeat Notification Policy property for indication
    subscription class
 */
const CIMName _PROPERTY_REPEATNOTIFICATIONPOLICY =
    CIMNameUnchecked("RepeatNotificationPolicy");

/**
    The name of the Other Repeat Notification Policy property for
    indication subscription class
 */
const CIMName _PROPERTY_OTHERREPEATNOTIFICATIONPOLICY =
    CIMNameUnchecked("OtherRepeatNotificationPolicy");

/**
    The name of the On Fatal Error Policy property for Indication Subscription
    class
 */
const CIMName _PROPERTY_ONFATALERRORPOLICY =
    CIMNameUnchecked("OnFatalErrorPolicy");

/**
    The name of the Other On Fatal Error Policy property for Indication
    Subscription class
 */
const CIMName _PROPERTY_OTHERONFATALERRORPOLICY =
    CIMNameUnchecked("OtherOnFatalErrorPolicy");

/**
    The name of the Failure Trigger Time Interval property for Indication
    Subscription class
 */
const CIMName _PROPERTY_FAILURETRIGGERTIMEINTERVAL =
    CIMNameUnchecked("FailureTriggerTimeInterval");

/**
    The name of the Time Of Last State Change property for Indication
    Subscription class
 */
const CIMName _PROPERTY_LASTCHANGE = CIMNameUnchecked("TimeOfLastStateChange");

/**
    The name of the Subscription Start Time property for Indication
    Subscription class
 */
const CIMName _PROPERTY_STARTTIME = CIMNameUnchecked("SubscriptionStartTime");

/**
    The name of the Subscription Duration property for Indication
    Subscription class
 */
const CIMName _PROPERTY_DURATION = CIMNameUnchecked("SubscriptionDuration");

/**
    The name of the Subscription Time Remaining property for Indication
    Subscription class
 */
const CIMName _PROPERTY_TIMEREMAINING =
    CIMNameUnchecked("SubscriptionTimeRemaining");

/**
    The name of the Repeat Notification Interval property for indication
    subscription class
 */
const CIMName _PROPERTY_REPEATNOTIFICATIONINTERVAL =
    CIMNameUnchecked("RepeatNotificationInterval");

/**
    The name of the Repeat Notification Gap property for indication
    subscription class
 */
const CIMName _PROPERTY_REPEATNOTIFICATIONGAP =
    CIMNameUnchecked("RepeatNotificationGap");

/**
    The name of the Repeat Notification Count property for indication
    subscription class
 */
const CIMName _PROPERTY_REPEATNOTIFICATIONCOUNT =
    CIMNameUnchecked("RepeatNotificationCount");

/**
    The name of the TextFormatOwningEntity property for Formatted Indication
    Subscription class
*/
const CIMName _PROPERTY_TEXTFORMATOWNINGENTITY =
    CIMNameUnchecked("TextFormatOwningEntity");

/**
    The name of the TextFormatID property for Formatted Indication
    Subscription class
*/
const CIMName _PROPERTY_TEXTFORMATID = CIMNameUnchecked("TextFormatID");

/**
    The name of the Caption property for Managed Element class
 */
const CIMName _PROPERTY_CAPTION = CIMNameUnchecked("Caption");

/**
    The name of the Description property for Managed Element class
 */
const CIMName _PROPERTY_DESCRIPTION = CIMNameUnchecked("Description");

/**
    The name of the ElementName property for Managed Element class
 */
const CIMName _PROPERTY_ELEMENTNAME = CIMNameUnchecked("ElementName");

/**
    The name of the Source Namespace property for indication filter class
 */
const CIMName _PROPERTY_SOURCENAMESPACE = CIMNameUnchecked("SourceNamespace");

/**
    The name of the System Name property for indication filter and indications
    handler classes
 */
const CIMName _PROPERTY_SYSTEMNAME = CIMNameUnchecked("SystemName");

/**
    The name of the System Creation Class Name property for indication filter
    and indications handler classes
 */
const CIMName _PROPERTY_SYSTEMCREATIONCLASSNAME =
    CIMNameUnchecked("SystemCreationClassName");

/**
    The name of the Other Persistence Type property for Indication Handler
    class
 */
const CIMName _PROPERTY_OTHERPERSISTENCETYPE =
    CIMNameUnchecked("OtherPersistenceType");

/**
    The name of the Owner property for Indication Handler class
 */
const CIMName _PROPERTY_OWNER = CIMNameUnchecked("Owner");

/**
    The name of the TargetHostFormat property for SNMP Mapper Indication
    Handler subclass
 */
const CIMName _PROPERTY_TARGETHOSTFORMAT = CIMNameUnchecked("TargetHostFormat");

/**
    The name of the OtherTargetHostFormat property for SNMP Mapper Indication
    Handler subclass
 */
const CIMName _PROPERTY_OTHERTARGETHOSTFORMAT =
    CIMNameUnchecked("OtherTargetHostFormat");

/**
    The name of the Port Number property for SNMP Mapper Indication Handler
    subclass
 */
const CIMName _PROPERTY_PORTNUMBER = CIMNameUnchecked("PortNumber");

/**
    The name of the SNMP Security Name property for SNMP Mapper Indication
    Handler subclass
 */
const CIMName _PROPERTY_SNMPSECURITYNAME = CIMNameUnchecked("SNMPSecurityName");

/**
    The name of the SNMP Engine ID property for SNMP Mapper Indication Handler
    subclass
 */
const CIMName _PROPERTY_SNMPENGINEID = CIMNameUnchecked("SNMPEngineID");

/**
    The name of the Alert Type property for Alert Indication class
 */
const CIMName _PROPERTY_ALERTTYPE = CIMNameUnchecked("AlertType");

/**
    The name of the Other Alert Type property for Alert Indication class
 */
const CIMName _PROPERTY_OTHERALERTTYPE = CIMName("OtherAlertType");

/**
    The name of the Perceived Severity property for Alert Indication class
 */
const CIMName _PROPERTY_PERCEIVEDSEVERITY =
    CIMNameUnchecked("PerceivedSeverity");

/**
    The name of the Probable Cause property for Alert Indication class
 */
const CIMName _PROPERTY_PROBABLECAUSE = CIMNameUnchecked("ProbableCause");

/**
    The name of the CreationClassName property of CIM_Service class.
*/
const CIMName _PROPERTY_CREATIONCLASSNAME =
    CIMNameUnchecked("CreationClassName");

/**
    The name of the Name property of CIM_Service class.
*/
const CIMName _PROPERTY_NAME = CIMNameUnchecked("Name");

/**
    The name of the Started property of CIM_Service class.
*/
const CIMName _PROPERTY_STARTED = CIMNameUnchecked("Started");

/**
    The name of the  InstanceID property of CIM_Capabilities
    class.
*/
const CIMName _PROPERTY_INSTANCEID = CIMNameUnchecked("InstanceID");

#ifdef PEGASUS_ENABLE_DMTF_INDICATION_PROFILE_SUPPORT
/**
    The name of the FilterCreationEnabled property of IndicationService class.
*/
const CIMName _PROPERTY_FILTERCREATIONENABLED =
    CIMNameUnchecked("FilterCreationEnabled");

/**
    The name of the SubscriptionRemovalAction property of IndicationService
    class.
*/
const CIMName _PROPERTY_SUBSCRIPTIONREMOVALACTION =
    CIMNameUnchecked("SubscriptionRemovalAction");

/**
    The name of the SubscriptionRemovalTimeInterval property of
    IndicationService class.
*/
const CIMName _PROPERTY_SUBSCRIPTIONREMOVALTIMEINTERVAL =
    CIMNameUnchecked("SubscriptionRemovalTimeInterval");

/**
    The name of the DeliveryRetryAttempts property of
    IndicationService class.
*/
const CIMName _PROPERTY_DELIVERYRETRYATTEMPTS =
    CIMNameUnchecked("DeliveryRetryAttempts");

/**
    The name of the DeliveryRetryInterval property of
    IndicationService class.
*/
const CIMName _PROPERTY_DELIVERYRETRYINTERVAL =
    CIMNameUnchecked("DeliveryRetryInterval");
/**
    The name of the FilterCreationEnabledIsSettable property of
    IndicationServiceCapabilities class.
*/
const CIMName _PROPERTY_FILTERCREATIONENABLEDISSETTABLE =
    CIMNameUnchecked("FilterCreationEnabledIsSettable");

/**
    The name of the DeliveryRetryAttemptsIsSettable property of
    IndicationServiceCapabilities class.
*/
const CIMName _PROPERTY_DELIVERYRETRYATTEMPTSISSETTABLE =
    CIMNameUnchecked("DeliveryRetryAttemptsIsSettable");

/**
    The name of the DeliveryRetryIntervalIsSettable property of
    IndicationServiceCapabilities class.
*/
const CIMName _PROPERTY_DELIVERYRETRYINTERVALISSETTABLE =
    CIMNameUnchecked("DeliveryRetryIntervalIsSettable");

/**
    The name of the SubscriptionRemovalActionIsSettable property of
    IndicationServiceCapabilities class.
*/
const CIMName _PROPERTY_SUBSCRIPTIONREMOVALACTIONISSETTABLE =
    CIMNameUnchecked("SubscriptionRemovalActionIsSettable");

/**
    The name of the SubscriptionRemovalTimeIntervalIsSettable property of
    IndicationServiceCapabilities class.
*/
const CIMName _PROPERTY_SUBSCRIPTIONREMOVALTIMEINTERVALISSETTABLE =
    CIMNameUnchecked("SubscriptionRemovalTimeIntervalIsSettable");

/**
    The name of the MaxListenerDestinations property of
    IndicationServiceCapabilities class.
*/
const CIMName _PROPERTY_MAXLISTENERDESTINATIONS =
    CIMNameUnchecked("MaxListenerDestinations");

/**
    The name of the MaxActiveSubscriptions property of
    IndicationServiceCapabilities class.
*/
const CIMName _PROPERTY_MAXACTIVESUBSCRIPTIONS =
    CIMNameUnchecked("MaxActiveSubscriptions");

/**
    The name of the SubscriptionsPersisted property of
    IndicationServiceCapabilities class.
*/
const CIMName _PROPERTY_SUBSCRIPTIONSPERSISTED =
    CIMNameUnchecked("SubscriptionsPersisted");

#endif

//
//  Qualifier names
//

/**
    The name of the Indication qualifier for classes
 */
const CIMName _QUALIFIER_INDICATION = CIMNameUnchecked("INDICATION");


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
