//%2003////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002  BMC Software, Hewlett-Packard Development
// Company, L. P., IBM Corp., The Open Group, Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L. P.;
// IBM Corp.; EMC Corporation, The Open Group.
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
// Author: Carol Ann Krug Graves, Hewlett-Packard Company
//             (carolann_graves@hp.com)
//
// Modified By:  
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_IndicationConstants_h
#define Pegasus_IndicationConstants_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMName.h>

PEGASUS_NAMESPACE_BEGIN

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

//
//  Class names
//

/**
    The name of the CIMOM Shutdown alert indication class
 */
//
//  ATTN: Update once CimomShutdownAlertIndication has been defined
//
static const CIMName _CLASS_CIMOM_SHUTDOWN_ALERT =
    CIMName ("CIM_AlertIndication");

/**
    The name of the No Provider alert indication class
 */
//
//  ATTN: Update once NoProviderAlertIndication has been defined
//
static const CIMName _CLASS_NO_PROVIDER_ALERT = CIMName ("CIM_AlertIndication");

/**
    The name of the CIMOM shutdown alert indication class
 */
//
//  ATTN: Update once ProviderTerminatedAlertIndication has been defined
//
static const CIMName _CLASS_PROVIDER_TERMINATED_ALERT =
    CIMName ("CIM_AlertIndication");


//
//  Property names
//

/**
    The name of the filter reference property for indication subscription class
 */
static const CIMName _PROPERTY_FILTER = CIMName ("Filter");

/**
    The name of the handler reference property for indication subscription class
 */
static const CIMName _PROPERTY_HANDLER = CIMName ("Handler");

/**
    The name of the subscription state property for indication subscription 
    class
 */
static const CIMName _PROPERTY_STATE = CIMName ("SubscriptionState");

/**
    The name of the Other Subscription State property for Indication 
    Subscription class
 */
static const CIMName _PROPERTY_OTHERSTATE = CIMName ("OtherSubscriptionState");

/**
    The name of the repeat notification policy property for indication
    subscription class
 */
static const CIMName _PROPERTY_REPEATNOTIFICATIONPOLICY =
    CIMName ("RepeatNotificationPolicy");

/**
    The name of the other repeat notification policy property for
    indication subscription class
 */
static const CIMName _PROPERTY_OTHERREPEATNOTIFICATIONPOLICY =
    CIMName ("OtherRepeatNotificationPolicy");

/**
    The name of the On Fatal Error Policy property for Indication Subscription 
    class
 */
static const CIMName _PROPERTY_ONFATALERRORPOLICY = 
    CIMName ("OnFatalErrorPolicy");

/**
    The name of the Other On Fatal Error Policy property for Indication 
    Subscription class
 */
static const CIMName _PROPERTY_OTHERONFATALERRORPOLICY = 
    CIMName ("OtherOnFatalErrorPolicy");

/**
    The name of the Time Of Last State Change property for Indication 
    Subscription class
 */
static const CIMName _PROPERTY_LASTCHANGE = CIMName ("TimeOfLastStateChange");

/**
    The name of the Subscription Start Time property for Indication 
    Subscription class
 */
static const CIMName _PROPERTY_STARTTIME = CIMName ("SubscriptionStartTime");

/**
    The name of the Subscription Duration property for Indication 
    Subscription class
 */
static const CIMName _PROPERTY_DURATION = CIMName ("SubscriptionDuration");

/**
    The name of the Subscription Time Remaining property for Indication 
    Subscription class
 */
static const CIMName _PROPERTY_TIMEREMAINING = 
    CIMName ("SubscriptionTimeRemaining");

/**
    The name of the query property for indication filter class
 */
static const CIMName _PROPERTY_QUERY = CIMName ("Query");

/**
    The name of the query language property for indication filter class
 */
static const CIMName _PROPERTY_QUERYLANGUAGE = CIMName ("QueryLanguage");

/**
    The name of the Source Namespace property for indication filter class
 */
static const CIMName _PROPERTY_SOURCENAMESPACE = CIMName ("SourceNamespace");

/**
    The name of the name property for indication filter and indications handler     classes
 */
static const CIMName _PROPERTY_NAME = CIMName ("Name");

/**
    The name of the creation class name property for indication filter and 
    indications handler classes
 */
static const CIMName _PROPERTY_CREATIONCLASSNAME = 
    CIMName ("CreationClassName");

/**
    The name of the system name property for indication filter and indications 
    handler classes
 */
static const CIMName _PROPERTY_SYSTEMNAME = CIMName ("SystemName");

/**
    The name of the system creation class name property for indication filter 
    and indications handler classes
 */
static const CIMName _PROPERTY_SYSTEMCREATIONCLASSNAME = 
    CIMName ("SystemCreationClassName");

/**
    The name of the Persistence Type property for Indication Handler class
 */
static const CIMName _PROPERTY_PERSISTENCETYPE = CIMName ("PersistenceType");

/**
    The name of the Other Persistence Type property for Indication Handler 
    class
 */
static const CIMName _PROPERTY_OTHERPERSISTENCETYPE = 
    CIMName ("OtherPersistenceType");

/**
    The name of the Destination property for CIM XML Indication Handler 
    subclass
 */
static const CIMName _PROPERTY_DESTINATION = CIMName ("Destination");

/**
    The name of the TargetHost property for SNMP Mapper Indication 
    Handler subclass
 */
static const CIMName _PROPERTY_TARGETHOST = CIMName ("TargetHost");

/**
    The name of the TargetHostFormat property for SNMP Indication Handler subclass
 */
static const CIMName _PROPERTY_TARGETHOSTFORMAT = CIMName ("TargetHostFormat");

/**
    The name of the SNMPVersion property for SNMP Indication Handler class
 */
static const CIMName _PROPERTY_SNMPVERSION = CIMName ("SNMPVersion");

/**
    The name of the Alert Type property for Alert Indication class
 */
static const CIMName _PROPERTY_ALERTTYPE = CIMName ("AlertType");

/**
    The name of the Other Alert Type property for Alert Indication class
 */
static const CIMName _PROPERTY_OTHERALERTTYPE = CIMName ("OtherAlertType");

/**
    The name of the Perceived Severity property for Alert Indication class
 */
static const CIMName _PROPERTY_PERCEIVEDSEVERITY = 
    CIMName ("PerceivedSeverity");

/**
    The name of the Probable Cause property for Alert Indication class
 */
static const CIMName _PROPERTY_PROBABLECAUSE = CIMName ("ProbableCause");


//
//  Qualifier names
//

/**
    The name of the Indication qualifier for classes
 */
static const CIMName _QUALIFIER_INDICATION = CIMName ("INDICATION");


//
//  Other literal values
//

/**
    The WHERE keyword in WQL
 */
static const char   _QUERY_WHERE []         = "WHERE";

/**
    A zero value CIMDateTime interval
 */
static const char _ZERO_INTERVAL_STRING [] = "00000000000000.000000:000";

PEGASUS_NAMESPACE_END

#endif  /* Pegasus_IndicationConstants_h */
