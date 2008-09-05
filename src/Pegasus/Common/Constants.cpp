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
//=============================================================================
//
//%////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMNameUnchecked.h>
#include "Constants.h"

PEGASUS_NAMESPACE_BEGIN

const CIMName PEGASUS_CLASSNAME_CONFIGSETTING        =
    CIMNameUnchecked("PG_ConfigSetting");
const CIMName PEGASUS_CLASSNAME_AUTHORIZATION        =
    CIMNameUnchecked("PG_Authorization");
const CIMName PEGASUS_CLASSNAME_USER                 =
    CIMNameUnchecked("PG_User");
const CIMName PEGASUS_CLASSNAME_CERTIFICATE          =
    CIMNameUnchecked("PG_SSLCertificate");
const CIMName PEGASUS_CLASSNAME_CRL                  =
    CIMNameUnchecked("PG_SSLCertificateRevocationList");
const CIMName PEGASUS_CLASSNAME_PROVIDERMODULE       =
    CIMNameUnchecked("PG_ProviderModule");
const CIMName PEGASUS_CLASSNAME_PROVIDER             =
    CIMNameUnchecked("PG_Provider");
const CIMName PEGASUS_CLASSNAME_CAPABILITIESREGISTRATION =
    CIMNameUnchecked("PG_CapabilitiesRegistration");
const CIMName PEGASUS_CLASSNAME_CONSUMERCAPABILITIES =
    CIMNameUnchecked("PG_ConsumerCapabilities");
const CIMName PEGASUS_CLASSNAME_PROVIDERCAPABILITIES =
    CIMNameUnchecked("PG_ProviderCapabilities");
const CIMName PEGASUS_CLASSNAME_INDSUBSCRIPTION      =
    CIMNameUnchecked("CIM_IndicationSubscription");
const CIMName PEGASUS_CLASSNAME_FORMATTEDINDSUBSCRIPTION =
    CIMNameUnchecked("CIM_FormattedIndicationSubscription");
const CIMName PEGASUS_CLASSNAME_INDHANDLER           =
    CIMNameUnchecked("CIM_IndicationHandler");
const CIMName PEGASUS_CLASSNAME_LSTNRDST             =
    CIMNameUnchecked("CIM_ListenerDestination");
const CIMName PEGASUS_CLASSNAME_INDHANDLER_CIMXML    =
    CIMNameUnchecked("CIM_IndicationHandlerCIMXML");
const CIMName PEGASUS_CLASSNAME_LSTNRDST_CIMXML      =
    CIMNameUnchecked("CIM_ListenerDestinationCIMXML");
const CIMName PEGASUS_CLASSNAME_INDHANDLER_SNMP      =
    CIMNameUnchecked("PG_IndicationHandlerSNMPMapper");
const CIMName PEGASUS_CLASSNAME_LSTNRDST_SYSTEM_LOG  =
    CIMNameUnchecked("PG_ListenerDestinationSystemLog");
const CIMName PEGASUS_CLASSNAME_LSTNRDST_EMAIL       =
    CIMNameUnchecked("PG_ListenerDestinationEmail");
const CIMName PEGASUS_CLASSNAME_INDFILTER            =
    CIMNameUnchecked("CIM_IndicationFilter");
const CIMName PEGASUS_CLASSNAME_SHUTDOWN             =
    CIMNameUnchecked("PG_ShutdownService");
const CIMName PEGASUS_CLASSNAME___NAMESPACE          =
    CIMNameUnchecked("__Namespace");

#ifndef PEGASUS_DISABLE_PERFINST
const CIMName PEGASUS_CLASSNAME_CIMOMSTATDATA        =
    CIMNameUnchecked("CIM_CIMOMStatisticalData");
#endif

#ifdef PEGASUS_ENABLE_CQL
const CIMName PEGASUS_CLASSNAME_CIMQUERYCAPABILITIES =
    CIMNameUnchecked("CIM_QueryCapabilities");
#endif

const CIMName PEGASUS_CLASSNAME_PROVIDERINDDATA      =
    CIMNameUnchecked("PG_ProviderIndicationData");
const CIMName PEGASUS_CLASSNAME_SUBSCRIPTIONINDDATA  =
    CIMNameUnchecked("PG_SubscriptionIndicationData");

// Interop Classes Accessed through Interop Control Provider

const CIMName PEGASUS_CLASSNAME_CIMNAMESPACE =
    CIMNameUnchecked("CIM_Namespace");

#if defined PEGASUS_ENABLE_INTEROP_PROVIDER
const CIMName PEGASUS_CLASSNAME_OBJECTMANAGER =
    CIMNameUnchecked("CIM_ObjectManager");
const CIMName PEGASUS_CLASSNAME_PGNAMESPACE =
    CIMNameUnchecked("PG_Namespace");
const CIMName PEGASUS_CLASSNAME_OBJECTMANAGERCOMMUNICATIONMECHANISM =
    CIMNameUnchecked("CIM_ObjectManagerCommunicationMechanism");
const CIMName PEGASUS_CLASSNAME_CIMXMLCOMMUNICATIONMECHANISM =
    CIMNameUnchecked("CIM_CIMXMLCommunicationMechanism");
const CIMName PEGASUS_CLASSNAME_PG_CIMXMLCOMMUNICATIONMECHANISM =
    CIMNameUnchecked("PG_CIMXMLCommunicationMechanism");
const CIMName PEGASUS_CLASSNAME_PROTOCOLADAPTER =
    CIMNameUnchecked("CIM_ProtocolAdapter");
const CIMName PEGASUS_CLASSNAME_NAMESPACEINMANAGER =
    CIMNameUnchecked("CIM_NamespaceInManager");
#endif

// slp Class which operates slp provider. Started by system

#ifdef PEGASUS_ENABLE_SLP
const CIMName PEGASUS_CLASSNAME_WBEMSLPTEMPLATE =
    CIMNameUnchecked("PG_WBEMSLPTEMPLATE");

#endif

// DMTF Indications profile classes
#ifdef PEGASUS_ENABLE_DMTF_INDICATION_PROFILE_SUPPORT
const CIMName PEGASUS_CLASSNAME_CIM_INDICATIONSERVICE =
    CIMNameUnchecked("CIM_IndicationService");

const CIMName PEGASUS_CLASSNAME_CIM_INDICATIONSERVICECAPABILITIES =
    CIMNameUnchecked("CIM_IndicationServiceCapabilities");

const CIMName PEGASUS_CLASSNAME_PG_ELEMENTCAPABILITIES =
    CIMNameUnchecked("PG_ElementCapabilities");

const CIMName PEGASUS_CLASSNAME_PG_HOSTEDINDICATIONSERVICE =
    CIMNameUnchecked("PG_HostedIndicationService");

const CIMName PEGASUS_CLASSNAME_PG_SERVICEAFFECTSELEMENT = 
    CIMNameUnchecked("PG_ServiceAffectsElement");
#endif

//
// Property Names
//

const CIMName PEGASUS_PROPERTYNAME_INDSUB_CREATOR =
    CIMNameUnchecked("Creator");
const CIMName PEGASUS_PROPERTYNAME_INDSUB_ACCEPTLANGS =
    CIMNameUnchecked("AcceptLanguages");
const CIMName PEGASUS_PROPERTYNAME_INDSUB_CONTENTLANGS =
    CIMNameUnchecked("ContentLanguages");
const CIMName PEGASUS_PROPERTYNAME_MODULE_USERCONTEXT =
    CIMNameUnchecked("UserContext");
const CIMName PEGASUS_PROPERTYNAME_MODULE_DESIGNATEDUSER =
    CIMNameUnchecked("DesignatedUserContext");

/**
    The name of the Destination property for CIM XML Indication Handler
    subclass
*/
const CIMName PEGASUS_PROPERTYNAME_LSTNRDST_DESTINATION =
    CIMNameUnchecked("Destination");

/**
    The name of the TargetHost property for SNMP Mapper Indication
    Handler subclass
*/
const CIMName PEGASUS_PROPERTYNAME_LSTNRDST_TARGETHOST =
    CIMNameUnchecked("TargetHost");

/**
    The name of the TextFormat property for Formatted Indication
    Subscription class
*/
const CIMName _PROPERTY_TEXTFORMAT = CIMNameUnchecked("TextFormat");

/**
    The name of the TextFormatParameters property for Formatted
    Indication Subscription class
*/
const CIMName _PROPERTY_TEXTFORMATPARAMETERS =
    CIMNameUnchecked("TextFormatParameters");

/**
    The name of the MailTo property for Email Handler subclass
*/
const CIMName PEGASUS_PROPERTYNAME_LSTNRDST_MAILTO =
    CIMNameUnchecked("MailTo");

/**
    The name of the MailSubject property for Email Handler subclass
*/
const CIMName PEGASUS_PROPERTYNAME_LSTNRDST_MAILSUBJECT =
    CIMNameUnchecked("MailSubject");

/**
    The name of the MailCc  property for Email Handler subclass
*/
const CIMName PEGASUS_PROPERTYNAME_LSTNRDST_MAILCC =
    CIMNameUnchecked("MailCc");

/**
    The name of the Name property for PG_ProviderModule class
*/
const CIMName _PROPERTY_PROVIDERMODULE_NAME =
    CIMNameUnchecked("Name");

/**
    The name of the operational status property
*/
const CIMName _PROPERTY_OPERATIONALSTATUS =
    CIMNameUnchecked("OperationalStatus");

/**
    The name of the Filter reference property for indication subscription class
 */
const CIMName PEGASUS_PROPERTYNAME_FILTER =
    CIMNameUnchecked("Filter");

/**
   The name of the Handler reference property for indication subscription class
 */
const CIMName PEGASUS_PROPERTYNAME_HANDLER =
    CIMNameUnchecked("Handler");

/**
    The name of the Subscription State property for indication subscription
    class
 */
const CIMName PEGASUS_PROPERTYNAME_SUBSCRIPTION_STATE =
    CIMNameUnchecked("SubscriptionState");

/**
    The name of the Query property for indication filter class
 */
const CIMName PEGASUS_PROPERTYNAME_QUERY =
    CIMNameUnchecked("Query");

/**
    The name of the Query Language property for indication filter class
 */
const CIMName PEGASUS_PROPERTYNAME_QUERYLANGUAGE =
    CIMNameUnchecked("QueryLanguage");

/**
    The name of the Name property
 */
const CIMName PEGASUS_PROPERTYNAME_NAME =
    CIMNameUnchecked("Name");

/**
    The name of the Creation Class Name property for indication filter and
    indications handler classes
 */
const CIMName PEGASUS_PROPERTYNAME_CREATIONCLASSNAME =
    CIMNameUnchecked("CreationClassName");

/**
    The name of the Persistence Type property for Indication Handler class
 */
const CIMName PEGASUS_PROPERTYNAME_PERSISTENCETYPE =
    CIMNameUnchecked("PersistenceType");

/**
    The name of the SNMP Version property for SNMP Mapper Indication Handler
    subclass
 */
const CIMName PEGASUS_PROPERTYNAME_SNMPVERSION =
    CIMNameUnchecked("SNMPVersion");

//
// CIM Namespace Names
//

const CIMNamespaceName PEGASUS_NAMESPACENAME_INTEROP  =
    CIMNamespaceName ("root/PG_InterOp");
const CIMNamespaceName PEGASUS_NAMESPACENAME_INTERNAL =
    CIMNamespaceName ("root/PG_Internal");
const CIMNamespaceName PEGASUS_NAMESPACENAME_CIMV2    =
    CIMNamespaceName ("root/cimv2");

const CIMNamespaceName PEGASUS_NAMESPACENAME_AUTHORIZATION =
    PEGASUS_NAMESPACENAME_INTERNAL;
const CIMNamespaceName PEGASUS_NAMESPACENAME_CONFIG        =
    PEGASUS_NAMESPACENAME_INTERNAL;
const CIMNamespaceName PEGASUS_NAMESPACENAME_PROVIDERREG   =
    PEGASUS_NAMESPACENAME_INTEROP;
const CIMNamespaceName PEGASUS_NAMESPACENAME_SHUTDOWN      =
    PEGASUS_NAMESPACENAME_INTERNAL;
const CIMNamespaceName PEGASUS_NAMESPACENAME_USER          =
    PEGASUS_NAMESPACENAME_INTERNAL;
const CIMNamespaceName PEGASUS_NAMESPACENAME_CERTIFICATE   =
    PEGASUS_NAMESPACENAME_INTERNAL;

#ifndef PEGASUS_DISABLE_PERFINST
const CIMNamespaceName PEGASUS_NAMESPACENAME_CIMOMSTATDATA =
    PEGASUS_NAMESPACENAME_CIMV2;
#endif

#ifdef PEGASUS_ENABLE_CQL
const CIMNamespaceName PEGASUS_NAMESPACENAME_CIMQUERYCAPABILITIES  =
    PEGASUS_NAMESPACENAME_CIMV2;
#endif

const CIMNamespaceName PEGASUS_VIRTUAL_TOPLEVEL_NAMESPACE =
    CIMNamespaceName("PG_Reserved");

//
// Server Profile-related class names
//
const CIMName PEGASUS_CLASSNAME_PG_OBJECTMANAGER =
    CIMNameUnchecked("PG_ObjectManager");
const CIMName PEGASUS_CLASSNAME_PG_COMMMECHANISMFORMANAGER =
    CIMNameUnchecked("PG_CommMechanismForManager");
const CIMName PEGASUS_CLASSNAME_PG_NAMESPACEINMANAGER =
    CIMNameUnchecked("PG_NamespaceInManager");
const CIMName PEGASUS_CLASSNAME_PG_REGISTEREDPROFILE =
    CIMNameUnchecked("PG_RegisteredProfile");
const CIMName PEGASUS_CLASSNAME_PG_REGISTEREDSUBPROFILE =
    CIMNameUnchecked("PG_RegisteredSubProfile");
const CIMName PEGASUS_CLASSNAME_PG_REFERENCEDPROFILE =
    CIMNameUnchecked("PG_ReferencedProfile");
const CIMName PEGASUS_CLASSNAME_CIM_ELEMENTCONFORMSTOPROFILE =
    CIMNameUnchecked("CIM_ElementConformsToProfile");
const CIMName PEGASUS_CLASSNAME_PG_ELEMENTCONFORMSTOPROFILE =
    CIMNameUnchecked("PG_ElementConformsToProfile");
const CIMName PEGASUS_CLASSNAME_PG_ELEMENTCONFORMSTOPROFILE_RP_RP =
    CIMNameUnchecked("PG_ElementConformsToProfile_RP_RP");
const CIMName PEGASUS_CLASSNAME_PG_SUBPROFILEREQUIRESPROFILE =
    CIMNameUnchecked("PG_SubProfileRequiresProfile");
const CIMName PEGASUS_CLASSNAME_PG_SOFTWAREIDENTITY =
    CIMNameUnchecked("PG_SoftwareIdentity");
const CIMName PEGASUS_CLASSNAME_PG_ELEMENTSOFTWAREIDENTITY =
    CIMNameUnchecked("PG_ElementSoftwareIdentity");
const CIMName PEGASUS_CLASSNAME_PG_INSTALLEDSOFTWAREIDENTITY =
    CIMNameUnchecked("PG_InstalledSoftwareIdentity");
const CIMName PEGASUS_CLASSNAME_PG_COMPUTERSYSTEM =
    CIMNameUnchecked("PG_ComputerSystem");
const CIMName PEGASUS_CLASSNAME_PG_HOSTEDOBJECTMANAGER =
    CIMNameUnchecked("PG_HostedObjectManager");
const CIMName PEGASUS_CLASSNAME_PG_HOSTEDACCESSPOINT =
    CIMNameUnchecked("PG_HostedAccessPoint");

// Registration classes
const CIMName PEGASUS_CLASSNAME_PG_PROVIDERPROFILECAPABILITIES =
    CIMNameUnchecked("PG_ProviderProfileCapabilities");
const CIMName PEGASUS_CLASSNAME_PG_PROVIDERREFERENCEDPROFILES =
    CIMNameUnchecked("PG_ProviderReferencedProfiles");

/**
    The qualifier name of embedded objects/instances.
*/
const CIMName PEGASUS_QUALIFIERNAME_EMBEDDEDOBJECT = 
    CIMNameUnchecked("EmbeddedObject");
const CIMName PEGASUS_QUALIFIERNAME_EMBEDDEDINSTANCE = 
    CIMNameUnchecked("EmbeddedInstance");


PEGASUS_NAMESPACE_END
