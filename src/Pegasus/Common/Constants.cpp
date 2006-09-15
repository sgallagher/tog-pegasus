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
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include "Constants.h"

PEGASUS_NAMESPACE_BEGIN

const CIMName PEGASUS_CLASSNAME_CONFIGSETTING        = 
    CIMName ("PG_ConfigSetting");
const CIMName PEGASUS_CLASSNAME_AUTHORIZATION        = 
    CIMName ("PG_Authorization");
const CIMName PEGASUS_CLASSNAME_USER                 = 
    CIMName ("PG_User");
const CIMName PEGASUS_CLASSNAME_CERTIFICATE          =
    CIMName ("PG_SSLCertificate");
const CIMName PEGASUS_CLASSNAME_CRL                  =
    CIMName ("PG_SSLCertificateRevocationList");
const CIMName PEGASUS_CLASSNAME_PROVIDERMODULE       = 
    CIMName ("PG_ProviderModule");
const CIMName PEGASUS_CLASSNAME_PROVIDER             = 
    CIMName ("PG_Provider");
const CIMName PEGASUS_CLASSNAME_CAPABILITIESREGISTRATION =
    CIMName ("PG_CapabilitiesRegistration");
const CIMName PEGASUS_CLASSNAME_CONSUMERCAPABILITIES =
    CIMName ("PG_ConsumerCapabilities");
const CIMName PEGASUS_CLASSNAME_PROVIDERCAPABILITIES = 
    CIMName ("PG_ProviderCapabilities");
const CIMName PEGASUS_CLASSNAME_INDSUBSCRIPTION      = 
    CIMName ("CIM_IndicationSubscription");
const CIMName PEGASUS_CLASSNAME_FORMATTEDINDSUBSCRIPTION =
    CIMName ("CIM_FormattedIndicationSubscription");
const CIMName PEGASUS_CLASSNAME_INDHANDLER           = 
    CIMName ("CIM_IndicationHandler");
const CIMName PEGASUS_CLASSNAME_LSTNRDST           =
    CIMName ("CIM_ListenerDestination");
const CIMName PEGASUS_CLASSNAME_INDHANDLER_CIMXML    = 
    CIMName ("CIM_IndicationHandlerCIMXML");
const CIMName PEGASUS_CLASSNAME_LSTNRDST_CIMXML    = 
		CIMName ("CIM_ListenerDestinationCIMXML");
const CIMName PEGASUS_CLASSNAME_INDHANDLER_SNMP      = 
    CIMName ("PG_IndicationHandlerSNMPMapper");
const CIMName PEGASUS_CLASSNAME_LSTNRDST_SYSTEM_LOG      =
    CIMName ("PG_ListenerDestinationSystemLog");
const CIMName PEGASUS_CLASSNAME_LSTNRDST_EMAIL      =
    CIMName ("PG_ListenerDestinationEmail");
const CIMName PEGASUS_CLASSNAME_INDFILTER            = 
    CIMName ("CIM_IndicationFilter");
const CIMName PEGASUS_CLASSNAME_SHUTDOWN             = 
    CIMName ("PG_ShutdownService");
const CIMName PEGASUS_CLASSNAME___NAMESPACE          = 
    CIMName ("__Namespace");

#ifndef PEGASUS_DISABLE_PERFINST
const CIMName PEGASUS_CLASSNAME_CIMOMSTATDATA      =
    CIMName ("CIM_CIMOMStatisticalData");
#endif

#ifndef PEGASUS_DISABLE_CQL
const CIMName PEGASUS_CLASSNAME_CIMQUERYCAPABILITIES   =
    CIMName ("CIM_QueryCapabilities");
#endif

// Interop Classes Accessed through Interop Control Provider
#if !defined(PEGASUS_DISABLE_PERFINST) || defined(PEGASUS_ENABLE_SLP)
const CIMName PEGASUS_CLASSNAME_OBJECTMANAGER            = 
    CIMName ("CIM_ObjectManager");
const CIMName PEGASUS_CLASSNAME_PGNAMESPACE            = 
    CIMName ("PG_Namespace");
const CIMName PEGASUS_CLASSNAME_CIMNAMESPACE = 
    CIMName ("CIM_Namespace");
const CIMName PEGASUS_CLASSNAME_OBJECTMANAGERCOMMUNICATIONMECHANISM  = 
    CIMName ("CIM_ObjectManagerCommunicationMechanism");
const CIMName PEGASUS_CLASSNAME_CIMXMLCOMMUNICATIONMECHANISM  = 
    CIMName ("CIM_CIMXMLCommunicationMechanism");
const CIMName PEGASUS_CLASSNAME_PG_CIMXMLCOMMUNICATIONMECHANISM  = 
    CIMName ("PG_CIMXMLCommunicationMechanism");
const CIMName PEGASUS_CLASSNAME_PROTOCOLADAPTER  = 
    CIMName ("CIM_ProtocolAdapter");
const CIMName PEGASUS_CLASSNAME_NAMESPACEINMANAGER  = 
    CIMName ("CIM_NamespaceInManager");

// slp Class which operates slp provider. Started by system

const CIMName PEGASUS_CLASSNAME_WBEMSLPTEMPLATE = 
    CIMName ("PG_WBEMSLPTEMPLATE");
#endif

//
// Property Names
//

const CIMName PEGASUS_PROPERTYNAME_INDSUB_CREATOR    = 
    CIMName ("Creator");
const CIMName PEGASUS_PROPERTYNAME_INDSUB_ACCEPTLANGS = 
    CIMName ("AcceptLanguages"); 
const CIMName PEGASUS_PROPERTYNAME_INDSUB_CONTENTLANGS = 
    CIMName ("ContentLanguages");  
const CIMName PEGASUS_PROPERTYNAME_MODULE_USERCONTEXT =
    CIMName ("UserContext");
const CIMName PEGASUS_PROPERTYNAME_MODULE_DESIGNATEDUSER =
    CIMName ("DesignatedUserContext");

/**
    The name of the Destination property for CIM XML Indication Handler
    subclass
*/
const CIMName PEGASUS_PROPERTYNAME_LSTNRDST_DESTINATION = 
    CIMName ("Destination");

/**
    The name of the TargetHost property for SNMP Mapper Indication
    Handler subclass
*/
const CIMName PEGASUS_PROPERTYNAME_LSTNRDST_TARGETHOST = 
    CIMName ("TargetHost");

/**
    The name of the TextFormat property for Formatted Indication
    Subscription class
*/
    const CIMName _PROPERTY_TEXTFORMAT = CIMName ("TextFormat");

/**
    The name of the TextFormatParameters property for Formatted
    Indication Subscription class
*/
    const CIMName _PROPERTY_TEXTFORMATPARAMETERS =
        CIMName ("TextFormatParameters");

/**
    The name of the MailTo property for Email Handler subclass
*/
    const CIMName PEGASUS_PROPERTYNAME_LSTNRDST_MAILTO =
        CIMName ("MailTo");

/**
    The name of the MailSubject property for Email Handler subclass
*/
    const CIMName PEGASUS_PROPERTYNAME_LSTNRDST_MAILSUBJECT =
        CIMName ("MailSubject");

/**
    The name of the MailCc  property for Email Handler subclass
*/
    const CIMName PEGASUS_PROPERTYNAME_LSTNRDST_MAILCC =
        CIMName ("MailCc");

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

#ifndef PEGASUS_DISABLE_CQL
const CIMNamespaceName PEGASUS_NAMESPACENAME_CIMQUERYCAPABILITIES  =
    PEGASUS_NAMESPACENAME_CIMV2;
#endif

PEGASUS_NAMESPACE_END
