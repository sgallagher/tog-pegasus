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
// Author: Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
// Modified By: Carol Ann Krug Graves, Hewlett-Packard Company
//                (carolann_graves@hp.com)
//              Yi Zhou, Hewlett-Packard Company (yi.zhou@hp.com)
//				Willis White (whiwill@us.ibm.com) Bug 1596
//              Amit K Arora, IBM (amitarora@in.ibm.com) for PEP 193
//              Sean Keenan, Hewlett-Packard Company (sean.keenan@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Constants_h
#define Pegasus_Constants_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMName.h>

PEGASUS_NAMESPACE_BEGIN

//
// Message Queue Names
//

#define PEGASUS_QUEUENAME_METADISPATCHER      "pegasus meta dispatcher"

#define PEGASUS_QUEUENAME_HTTPACCEPTOR        "HTTPAcceptor"
#define PEGASUS_QUEUENAME_HTTPCONNECTION      "HTTPConnection"
#define PEGASUS_QUEUENAME_HTTPCONNECTOR       "HTTPConnector"
#define PEGASUS_QUEUENAME_HTTPOPTIONS         "HTTPOptions"

#define PEGASUS_QUEUENAME_HTTPAUTHDELEGATOR   "HTTPAuthenticatorDelegator"
#define PEGASUS_QUEUENAME_OPREQDECODER        "CIMOpRequestDecoder"
#define PEGASUS_QUEUENAME_OPREQAUTHORIZER     "CIMOpRequestAuthorizer"
#define PEGASUS_QUEUENAME_OPREQDISPATCHER     "CIMOpRequestDispatcher"
#define PEGASUS_QUEUENAME_OPRESPENCODER       "CIMOpResponseEncoder"

#define PEGASUS_QUEUENAME_CONTROLSERVICE      "ControlService"
#define PEGASUS_QUEUENAME_INDICATIONSERVICE   "Server::IndicationService"
#define PEGASUS_QUEUENAME_PROVIDERMANAGER_CPP "Server::ProviderManagerService"
#define PEGASUS_QUEUENAME_INDHANDLERMANAGER   "IndicationHandlerService"

#define PEGASUS_QUEUENAME_CLIENT              "CIMClient"
#define PEGASUS_QUEUENAME_OPREQENCODER        "CIMOperationRequestEncoder"
#define PEGASUS_QUEUENAME_OPRESPDECODER       "CIMOperationResponseDecoder"

#define PEGASUS_QUEUENAME_EXPORTCLIENT        "CIMExportClient"
#define PEGASUS_QUEUENAME_EXPORTREQENCODER    "CIMExportRequestEncoder"
#define PEGASUS_QUEUENAME_EXPORTRESPDECODER   "CIMExportResponseDecoder"

#define PEGASUS_QUEUENAME_EXPORTREQDECODER    "CIMExportRequestDecoder"
#define PEGASUS_QUEUENAME_EXPORTREQDISPATCHER "CIMExportRequestDispatcher"
#define PEGASUS_QUEUENAME_EXPORTRESPENCODER   "CIMExportResponseEncoder"

#define PEGASUS_QUEUENAME_WBEMEXECCLIENT      "WbemExecClient"
#define PEGASUS_QUEUENAME_INTERNALCLIENT       "InternalClient"
#define PEGASUS_QUEUENAME_BINARY_HANDLER       "BinaryMessageHandler"


//
// ModuleController Module Names
//

#define PEGASUS_MODULENAME_CONFIGPROVIDER \
    PEGASUS_QUEUENAME_CONTROLSERVICE "::ConfigProvider"
#define PEGASUS_MODULENAME_USERAUTHPROVIDER \
    PEGASUS_QUEUENAME_CONTROLSERVICE "::UserAuthProvider"
#define PEGASUS_MODULENAME_PROVREGPROVIDER \
    PEGASUS_QUEUENAME_CONTROLSERVICE "::ProviderRegistrationProvider"
#define PEGASUS_MODULENAME_SHUTDOWNPROVIDER \
    PEGASUS_QUEUENAME_CONTROLSERVICE "::ShutdownProvider"
#define PEGASUS_MODULENAME_NAMESPACEPROVIDER \
    PEGASUS_QUEUENAME_CONTROLSERVICE "::NamespaceProvider"
#define PEGASUS_MODULENAME_CERTIFICATEPROVIDER \
    PEGASUS_QUEUENAME_CONTROLSERVICE "::CertificateProvider"
#define PEGASUS_MODULENAME_TEMP \
    PEGASUS_QUEUENAME_CONTROLSERVICE "::temp::do not use this name"
#define PEGASUS_MODULENAME_MONITOR \
    PEGASUS_QUEUENAME_CONTROLSERVICE "::Monitor"
#define PEGASUS_MODULENAME_INTEROPPROVIDER \
    PEGASUS_QUEUENAME_CONTROLSERVICE "::InteropProvider"

#ifndef PEGASUS_DISABLE_PERFINST
 #define PEGASUS_MODULENAME_CIMOMSTATDATAPROVIDER \
     PEGASUS_QUEUENAME_CONTROLSERVICE "::CIMOMStatDataProvider"
#endif

#ifndef PEGASUS_DISABLE_CQL
 #define PEGASUS_MODULENAME_CIMQUERYCAPPROVIDER \
     PEGASUS_QUEUENAME_CONTROLSERVICE "::CIMQueryCapabilitiesProvider"
#endif

//
// CIM Class Names
//

static const CIMName PEGASUS_CLASSNAME_CONFIGSETTING        = 
    CIMName ("PG_ConfigSetting");
static const CIMName PEGASUS_CLASSNAME_AUTHORIZATION        = 
    CIMName ("PG_Authorization");
static const CIMName PEGASUS_CLASSNAME_USER                 = 
    CIMName ("PG_User");
static const CIMName PEGASUS_CLASSNAME_CERTIFICATE          =
    CIMName ("PG_SSLCertificate");
static const CIMName PEGASUS_CLASSNAME_CRL                  =
    CIMName ("PG_SSLCertificateRevocationList");
static const CIMName PEGASUS_CLASSNAME_PROVIDERMODULE       = 
    CIMName ("PG_ProviderModule");
static const CIMName PEGASUS_CLASSNAME_PROVIDER             = 
    CIMName ("PG_Provider");
static const CIMName PEGASUS_CLASSNAME_CAPABILITIESREGISTRATION =
    CIMName ("PG_CapabilitiesRegistration");
static const CIMName PEGASUS_CLASSNAME_CONSUMERCAPABILITIES =
    CIMName ("PG_ConsumerCapabilities");
static const CIMName PEGASUS_CLASSNAME_PROVIDERCAPABILITIES = 
    CIMName ("PG_ProviderCapabilities");
static const CIMName PEGASUS_CLASSNAME_INDSUBSCRIPTION      = 
    CIMName ("CIM_IndicationSubscription");
static const CIMName PEGASUS_CLASSNAME_FORMATTEDINDSUBSCRIPTION =
    CIMName ("CIM_FormattedIndicationSubscription");
static const CIMName PEGASUS_CLASSNAME_INDHANDLER           = 
    CIMName ("CIM_IndicationHandler");
static const CIMName PEGASUS_CLASSNAME_LSTNRDST           =
    CIMName ("CIM_ListenerDestination");
static const CIMName PEGASUS_CLASSNAME_INDHANDLER_CIMXML    = 
    CIMName ("CIM_IndicationHandlerCIMXML");
static const CIMName PEGASUS_CLASSNAME_LSTNRDST_CIMXML    = 
		CIMName ("CIM_ListenerDestinationCIMXML");
static const CIMName PEGASUS_CLASSNAME_INDHANDLER_SNMP      = 
    CIMName ("PG_IndicationHandlerSNMPMapper");
static const CIMName PEGASUS_CLASSNAME_LSTNRDST_SYSTEM_LOG      =
    CIMName ("PG_ListenerDestinationSystemLog");
static const CIMName PEGASUS_CLASSNAME_LSTNRDST_EMAIL      =
    CIMName ("PG_ListenerDestinationEmail");
static const CIMName PEGASUS_CLASSNAME_INDFILTER            = 
    CIMName ("CIM_IndicationFilter");
static const CIMName PEGASUS_CLASSNAME_SHUTDOWN             = 
    CIMName ("PG_ShutdownService");
static const CIMName PEGASUS_CLASSNAME___NAMESPACE          = 
    CIMName ("__Namespace");

#ifndef PEGASUS_DISABLE_PERFINST
static const CIMName PEGASUS_CLASSNAME_CIMOMSTATDATA      =
    CIMName ("CIM_CIMOMStatisticalData");
#endif

#ifndef PEGASUS_DISABLE_CQL
static const CIMName PEGASUS_CLASSNAME_CIMQUERYCAPABILITIES   =
    CIMName ("CIM_QueryCapabilities");
#endif

// Interop Classes Accessed through Interop Control Provider
#if !defined(PEGASUS_DISABLE_PERFINST) || defined(PEGASUS_ENABLE_SLP)
static const CIMName PEGASUS_CLASSNAME_OBJECTMANAGER            = 
    CIMName ("CIM_ObjectManager");
static const CIMName PEGASUS_CLASSNAME_PGNAMESPACE            = 
    CIMName ("PG_Namespace");
static const CIMName PEGASUS_CLASSNAME_CIMNAMESPACE = 
    CIMName ("CIM_Namespace");
static const CIMName PEGASUS_CLASSNAME_OBJECTMANAGERCOMMUNICATIONMECHANISM  = 
    CIMName ("CIM_ObjectManagerCommunicationMechanism");
static const CIMName PEGASUS_CLASSNAME_CIMXMLCOMMUNICATIONMECHANISM  = 
    CIMName ("CIM_CIMXMLCommunicationMechanism");
static const CIMName PEGASUS_CLASSNAME_PG_CIMXMLCOMMUNICATIONMECHANISM  = 
    CIMName ("PG_CIMXMLCommunicationMechanism");
static const CIMName PEGASUS_CLASSNAME_PROTOCOLADAPTER  = 
    CIMName ("CIM_ProtocolAdapter");
static const CIMName PEGASUS_CLASSNAME_NAMESPACEINMANAGER  = 
    CIMName ("CIM_NamespaceInManager");

// slp Class which operates slp provider. Started by system

static const CIMName PEGASUS_CLASSNAME_WBEMSLPTEMPLATE = 
    CIMName ("PG_WBEMSLPTEMPLATE");
#endif

//
// Property Names
//

static const CIMName PEGASUS_PROPERTYNAME_INDSUB_CREATOR    = 
    CIMName ("Creator");
static const CIMName PEGASUS_PROPERTYNAME_INDSUB_ACCEPTLANGS = 
    CIMName ("AcceptLanguages"); 
static const CIMName PEGASUS_PROPERTYNAME_INDSUB_CONTENTLANGS = 
    CIMName ("ContentLanguages");  
static const CIMName PEGASUS_PROPERTYNAME_MODULE_USERCONTEXT =
    CIMName ("UserContext");
static const CIMName PEGASUS_PROPERTYNAME_MODULE_DESIGNATEDUSER =
    CIMName ("DesignatedUserContext");

/**
    The name of the Destination property for CIM XML Indication Handler
    subclass
*/
static const CIMName PEGASUS_PROPERTYNAME_LSTNRDST_DESTINATION = 
    CIMName ("Destination");

/**
    The name of the TargetHost property for SNMP Mapper Indication
    Handler subclass
*/
static const CIMName PEGASUS_PROPERTYNAME_LSTNRDST_TARGETHOST = 
    CIMName ("TargetHost");

/**
    The name of the TextFormat property for Formatted Indication
    Subscription class
*/
    static const CIMName _PROPERTY_TEXTFORMAT = CIMName ("TextFormat");

/**
    The name of the TextFormatParameters property for Formatted
    Indication Subscription class
*/
    static const CIMName _PROPERTY_TEXTFORMATPARAMETERS =
        CIMName ("TextFormatParameters");

//
// CIM Namespace Names
//

static const CIMNamespaceName PEGASUS_NAMESPACENAME_INTEROP  = 
    CIMNamespaceName ("root/PG_InterOp");
static const CIMNamespaceName PEGASUS_NAMESPACENAME_INTERNAL = 
    CIMNamespaceName ("root/PG_Internal");
static const CIMNamespaceName PEGASUS_NAMESPACENAME_CIMV2    = 
    CIMNamespaceName ("root/cimv2");

static const CIMNamespaceName PEGASUS_NAMESPACENAME_AUTHORIZATION = 
    PEGASUS_NAMESPACENAME_INTERNAL;
static const CIMNamespaceName PEGASUS_NAMESPACENAME_CONFIG        = 
    PEGASUS_NAMESPACENAME_INTERNAL;
static const CIMNamespaceName PEGASUS_NAMESPACENAME_PROVIDERREG   = 
    PEGASUS_NAMESPACENAME_INTEROP;
static const CIMNamespaceName PEGASUS_NAMESPACENAME_SHUTDOWN      = 
    PEGASUS_NAMESPACENAME_INTERNAL;
static const CIMNamespaceName PEGASUS_NAMESPACENAME_USER          = 
    PEGASUS_NAMESPACENAME_INTERNAL;
static const CIMNamespaceName PEGASUS_NAMESPACENAME_CERTIFICATE   =
    PEGASUS_NAMESPACENAME_INTERNAL;

#ifndef PEGASUS_DISABLE_PERFINST
static const CIMNamespaceName PEGASUS_NAMESPACENAME_CIMOMSTATDATA =
    PEGASUS_NAMESPACENAME_CIMV2;
#endif

#ifndef PEGASUS_DISABLE_CQL
static const CIMNamespaceName PEGASUS_NAMESPACENAME_CIMQUERYCAPABILITIES  =
    PEGASUS_NAMESPACENAME_CIMV2;
#endif

//
// HTTP Header Tags, Status Codes, and Reason Phrases
//

#define PEGASUS_HTTPHEADERTAG_ERRORDETAIL "PGErrorDetail"

#define HTTP_STATUSCODE_OK 200
#define HTTP_REASONPHRASE_OK "OK"
#define HTTP_STATUS_OK "200 OK"

#define HTTP_STATUSCODE_BADREQUEST 400
#define HTTP_REASONPHRASE_BADREQUEST "Bad Request"
#define HTTP_STATUS_BADREQUEST "400 Bad Request"

#define HTTP_STATUSCODE_UNAUTHORIZED 401
#define HTTP_REASONPHRASE_UNAUTHORIZED "Unauthorized"
#define HTTP_STATUS_UNAUTHORIZED "401 Unauthorized"

#define HTTP_STATUSCODE_FORBIDDEN    403
#define HTTP_REASONPHRASE_FORBIDDEN    "Forbidden"
#define HTTP_STATUS_FORBIDDEN    "403 Forbidden"

#define HTTP_STATUSCODE_REQUEST_TOO_LARGE 413
#define HTTP_REASONPHRASE_REQUEST_TOO_LARGE "Request Entity Too Large"
#define HTTP_STATUS_REQUEST_TOO_LARGE "413 Request Entity Too Large"

#define HTTP_STATUSCODE_INTERNALSERVERERROR 500
#define HTTP_REASONPHRASE_INTERNALSERVERERROR "Internal Server Error"
#define HTTP_STATUS_INTERNALSERVERERROR "500 Internal Server Error"

#define HTTP_STATUSCODE_NOTIMPLEMENTED 501
#define HTTP_REASONPHRASE_NOTIMPLEMENTED "Not Implemented"
#define HTTP_STATUS_NOTIMPLEMENTED "501 Not Implemented"

#define HTTP_STATUSCODE_SERVICEUNAVAILABLE 503
#define HTTP_REASONPHRASE_SERVICEUNAVAILABLE "Service Unavailable"
#define HTTP_STATUS_SERVICEUNAVAILABLE "503 Service Unavailable"


//
// Default port numbers
//

#define WBEM_DEFAULT_HTTP_PORT 5988
#define WBEM_DEFAULT_HTTPS_PORT 5989

#define SNMP_TRAP_DEFAULT_PORT 162


//
// Wbem service names
//
#define WBEM_HTTP_SERVICE_NAME "wbem-http"
#define WBEM_HTTPS_SERVICE_NAME "wbem-https"
#define WBEM_EXPORT_HTTPS_SERVICE_NAME "wbem-exp-https"



//
// File system layout
//

#define PEGASUS_SSLCLIENT_CERTIFICATEFILE "client.pem"
#define PEGASUS_SSLCLIENT_RANDOMFILE      "ssl.rnd"
#define PEGASUS_SSLSERVER_RANDOMFILE      "cimserver.rnd"
#define PEGASUS_LOCAL_AUTH_DIR            "/tmp"
// ATTN: Relocate this relative to PEGASUS_HOME
#define PEGASUS_LOCAL_DOMAIN_SOCKET_PATH  "/tmp/cimxml.socket"
#define PEGASUS_PAM_STANDALONE_PROC_NAME  "bin/cimservera"
#define PEGASUS_PROVIDER_AGENT_PROC_NAME  "bin/cimprovagt"

#ifdef PEGASUS_USE_RELEASE_DIRS
# if defined(PEGASUS_OS_HPUX)
#  undef PEGASUS_SSLCLIENT_CERTIFICATEFILE
#  define PEGASUS_SSLCLIENT_CERTIFICATEFILE "/etc/opt/hp/sslshare/client.pem"
#  undef PEGASUS_SSLCLIENT_RANDOMFILE
#  define PEGASUS_SSLCLIENT_RANDOMFILE      "/var/opt/wbem/ssl.rnd"
#  undef PEGASUS_SSLSERVER_RANDOMFILE
#  define PEGASUS_SSLSERVER_RANDOMFILE      "/var/opt/wbem/cimserver.rnd"
#  undef PEGASUS_LOCAL_AUTH_DIR
#  define PEGASUS_LOCAL_AUTH_DIR            "/var/opt/wbem/localauth"
#  undef PEGASUS_LOCAL_DOMAIN_SOCKET_PATH
#  define PEGASUS_LOCAL_DOMAIN_SOCKET_PATH  "/var/opt/wbem/socket/cimxml.socket"
#  undef PEGASUS_CORE_DIR
#  define PEGASUS_CORE_DIR                  "/var/opt/wbem"
#  undef PEGASUS_PAM_STANDALONE_PROC_NAME
#  define PEGASUS_PAM_STANDALONE_PROC_NAME  "/opt/wbem/sbin/cimservera"
#  undef PEGASUS_PROVIDER_AGENT_PROC_NAME
#  define PEGASUS_PROVIDER_AGENT_PROC_NAME  "/opt/wbem/sbin/cimprovagt"
# elif defined(PEGASUS_OS_AIX)
#  undef PEGASUS_SSLCLIENT_CERTIFICATEFILE
#  define PEGASUS_SSLCLIENT_CERTIFICATEFILE "/opt/freeware/cimom/pegasus/etc/client.pem"
#  undef PEGASUS_SSLCLIENT_RANDOMFILE
#  define PEGASUS_SSLCLIENT_RANDOMFILE      "/opt/freeware/cimom/pegasus/etc/ssl.rnd"
#  undef PEGASUS_SSLSERVER_RANDOMFILE
#  define PEGASUS_SSLSERVER_RANDOMFILE      "/opt/freeware/cimom/pegasus/etc/cimserver.rnd"
#  undef PEGASUS_LOCAL_DOMAIN_SOCKET_PATH
#  define PEGASUS_LOCAL_DOMAIN_SOCKET_PATH  "/opt/freeware/cimom/pegasus/etc/cimxml.socket"
#  undef PEGASUS_PAM_STANDALONE_PROC_NAME
#  define PEGASUS_PAM_STANDALONE_PROC_NAME  "/opt/freeware/cimom/pegasus/bin/cimservera"
# elif defined(PEGASUS_OS_LINUX) && !defined(PEGASUS_OS_LSB)
#  undef PEGASUS_SSLCLIENT_CERTIFICATEFILE
#  define PEGASUS_SSLCLIENT_CERTIFICATEFILE "/etc/opt/tog-pegasus/client.pem"
#  undef PEGASUS_SSLCLIENT_RANDOMFILE
#  define PEGASUS_SSLCLIENT_RANDOMFILE      "/var/opt/tog-pegasus/ssl.rnd"
#  undef PEGASUS_SSLSERVER_RANDOMFILE
#  define PEGASUS_SSLSERVER_RANDOMFILE      "/var/opt/tog-pegasus/cimserver.rnd"
#  undef PEGASUS_LOCAL_AUTH_DIR
#  define PEGASUS_LOCAL_AUTH_DIR            "/var/opt/tog-pegasus/cache/localauth"
#  undef PEGASUS_LOCAL_DOMAIN_SOCKET_PATH
#  define PEGASUS_LOCAL_DOMAIN_SOCKET_PATH  "/var/run/tog-pegasus/socket/cimxml.socket"
#  undef PEGASUS_CORE_DIR
#  define PEGASUS_CORE_DIR                  "/var/opt/tog-pegasus/cache"
#  undef PEGASUS_PAM_STANDALONE_PROC_NAME
#  define PEGASUS_PAM_STANDALONE_PROC_NAME  "/opt/tog-pegasus/sbin/cimservera"
#  undef PEGASUS_PROVIDER_AGENT_PROC_NAME
#  define PEGASUS_PROVIDER_AGENT_PROC_NAME  "/opt/tog-pegasus/sbin/cimprovagt"
# elif defined(PEGASUS_OS_LSB)
#  define PEGASUS_SSLCLIENT_CERTIFICATEFILE "/etc/opt/lsb-pegasus/client.pem"
#  undef PEGASUS_SSLCLIENT_RANDOMFILE
#  define PEGASUS_SSLCLIENT_RANDOMFILE      "/var/opt/lsb-pegasus/ssl.rnd"
#  undef PEGASUS_SSLSERVER_RANDOMFILE
#  define PEGASUS_SSLSERVER_RANDOMFILE      "/var/opt/lsb-pegasus/cimserver.rnd"
#  undef PEGASUS_LOCAL_AUTH_DIR
#  define PEGASUS_LOCAL_AUTH_DIR            "/var/opt/lsb-pegasus/localauth"
#  undef PEGASUS_LOCAL_DOMAIN_SOCKET_PATH
#  define PEGASUS_LOCAL_DOMAIN_SOCKET_PATH  "/var/opt/lsb-pegasus/socket/cimxml.socket"
#  undef PEGASUS_CORE_DIR
#  define PEGASUS_CORE_DIR                  "/var/opt/lsb-pegasus"
#  undef PEGASUS_PAM_STANDALONE_PROC_NAME
#  define PEGASUS_PAM_STANDALONE_PROC_NAME  "/opt/lsb-pegasus/sbin/cimservera"
#  undef PEGASUS_PROVIDER_AGENT_PROC_NAME
#  define PEGASUS_PROVIDER_AGENT_PROC_NAME  "/opt/lsb-pegasus/sbin/cimprovagt"
# elif defined(PEGASUS_OS_VMS)
#  undef PEGASUS_SSLCLIENT_CERTIFICATEFILE
#  define PEGASUS_SSLCLIENT_CERTIFICATEFILE "/wbem_var/opt/wbem/client.pem"
#  undef PEGASUS_SSLCLIENT_RANDOMFILE
#  define PEGASUS_SSLCLIENT_RANDOMFILE      "/wbem_var/opt/wbem/ssl.rnd"
#  undef PEGASUS_SSLSERVER_RANDOMFILE
#  define PEGASUS_SSLSERVER_RANDOMFILE      "/wbem_var/opt/wbem/cimserver.rnd"
#  undef PEGASUS_LOCAL_AUTH_DIR
#  define PEGASUS_LOCAL_AUTH_DIR            "/wbem_var/opt/wbem/localauth"
#  undef PEGASUS_PAM_STANDALONE_PROC_NAME
#  undef PEGASUS_PROVIDER_AGENT_PROC_NAME
#  define PEGASUS_PROVIDER_AGENT_PROC_NAME  "/wbem_var/opt/wbem/bin/cimprovagt"
# endif
#endif

// Constant defines for path to icu resource bundles
#ifdef PEGASUS_PLATFORM_OS400_ISERIES_IBM
#define PEGASUS_DEFAULT_MESSAGE_SOURCE  "/QIBM/ProdData/OS400/CIM/msg"
#else
#undef PEGASUS_DEFAULT_MESSAGE_SOURCE
#endif

//
// Miscellaneous Constants
//

// Constant defines the maximum number of providers that will be addressed in a single
// enumerate instance request.  This is strictly a performance requirement for different
// systems.  To disable the whole test, simply set this variable to 0
#define MAX_ENUMERATE_BREADTH 30

// Constant defines the maximum timeout to disable a provider
#define PROVIDER_DISABLE_TIMEOUT 15

// Constants defining the operational status of a CIM_ManagedSystemElement
#define CIM_MSE_OPSTATUS_VALUE_OK 2
#define CIM_MSE_OPSTATUS_VALUE_STOPPING 9
#define CIM_MSE_OPSTATUS_VALUE_STOPPED 10

// Constants defining UserContext values in a PG_ProviderModule
#define PG_PROVMODULE_USERCTXT_REQUESTOR 2
#define PG_PROVMODULE_USERCTXT_DESIGNATED 3
#define PG_PROVMODULE_USERCTXT_PRIVILEGED 4
#define PG_PROVMODULE_USERCTXT_CIMSERVER 5

// Standard Pegasus Global Prefix.
// This prefix is used as the basis for pegasus defined classes
// and in identity creation that would require a standard
// Pegasus prefix
#define PEGASUS_INSTANCEID_GLOBAL_PREFIX "PG"

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Constants_h */
