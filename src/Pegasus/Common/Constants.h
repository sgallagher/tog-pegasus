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

#ifndef Pegasus_Constants_h
#define Pegasus_Constants_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMName.h>
#include <Pegasus/Common/Linkage.h>

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

/**
    Values for the Subscription State property of the Subscription class,
    as defined in the CIM Events MOF
 */
enum SubscriptionState {STATE_UNKNOWN = 0, STATE_OTHER = 1,
     STATE_ENABLED = 2, STATE_ENABLEDDEGRADED = 3,
     STATE_DISABLED = 4};

/**
    Values for the Persistence Type property of the Handler class,
    as defined in the CIM Events MOF
 */
enum PersistenceType {PERSISTENCE_OTHER = 1, PERSISTENCE_PERMANENT = 2,
    PERSISTENCE_TRANSIENT = 3};

/**
    Values for the SNMPVersion property of the PG_IndicationHandlerSNMPMapper
    class, as defined in the PG Events MOF
 */
enum SnmpVersion {SNMPV1_TRAP = 2, SNMPV2C_TRAP = 3, SNMPV2C_INFORM = 4,
     SNMPV3_TRAP = 5, SNMPV3_INFORM = 6};

//
// CIM Class Names
//

PEGASUS_COMMON_LINKAGE extern const CIMName PEGASUS_CLASSNAME_CONFIGSETTING;
PEGASUS_COMMON_LINKAGE extern const CIMName PEGASUS_CLASSNAME_AUTHORIZATION;
PEGASUS_COMMON_LINKAGE extern const CIMName PEGASUS_CLASSNAME_USER;
PEGASUS_COMMON_LINKAGE extern const CIMName PEGASUS_CLASSNAME_CERTIFICATE;
PEGASUS_COMMON_LINKAGE extern const CIMName PEGASUS_CLASSNAME_CRL;
PEGASUS_COMMON_LINKAGE extern const CIMName PEGASUS_CLASSNAME_PROVIDERMODULE;
PEGASUS_COMMON_LINKAGE extern const CIMName PEGASUS_CLASSNAME_PROVIDER;
PEGASUS_COMMON_LINKAGE
    extern const CIMName PEGASUS_CLASSNAME_CAPABILITIESREGISTRATION;
PEGASUS_COMMON_LINKAGE
    extern const CIMName PEGASUS_CLASSNAME_CONSUMERCAPABILITIES;
PEGASUS_COMMON_LINKAGE
    extern const CIMName PEGASUS_CLASSNAME_PROVIDERCAPABILITIES;
PEGASUS_COMMON_LINKAGE extern const CIMName PEGASUS_CLASSNAME_INDSUBSCRIPTION;
PEGASUS_COMMON_LINKAGE
    extern const CIMName PEGASUS_CLASSNAME_FORMATTEDINDSUBSCRIPTION;
PEGASUS_COMMON_LINKAGE extern const CIMName PEGASUS_CLASSNAME_INDHANDLER;
PEGASUS_COMMON_LINKAGE extern const CIMName PEGASUS_CLASSNAME_LSTNRDST;
PEGASUS_COMMON_LINKAGE extern const CIMName PEGASUS_CLASSNAME_INDHANDLER_CIMXML;
PEGASUS_COMMON_LINKAGE extern const CIMName PEGASUS_CLASSNAME_LSTNRDST_CIMXML;
PEGASUS_COMMON_LINKAGE extern const CIMName PEGASUS_CLASSNAME_INDHANDLER_SNMP;
PEGASUS_COMMON_LINKAGE
    extern const CIMName PEGASUS_CLASSNAME_LSTNRDST_SYSTEM_LOG;
PEGASUS_COMMON_LINKAGE extern const CIMName PEGASUS_CLASSNAME_LSTNRDST_EMAIL;
PEGASUS_COMMON_LINKAGE extern const CIMName PEGASUS_CLASSNAME_INDFILTER;
PEGASUS_COMMON_LINKAGE extern const CIMName PEGASUS_CLASSNAME_SHUTDOWN;
PEGASUS_COMMON_LINKAGE extern const CIMName PEGASUS_CLASSNAME___NAMESPACE;

#ifndef PEGASUS_DISABLE_PERFINST
PEGASUS_COMMON_LINKAGE extern const CIMName PEGASUS_CLASSNAME_CIMOMSTATDATA;
#endif

#ifndef PEGASUS_DISABLE_CQL
PEGASUS_COMMON_LINKAGE
    extern const CIMName PEGASUS_CLASSNAME_CIMQUERYCAPABILITIES;
#endif

PEGASUS_COMMON_LINKAGE extern const CIMName PEGASUS_CLASSNAME_CIMNAMESPACE;
#define PEGASUS_SLP_SERVICE_TYPE   "service:wbem"
// Interop Classes Accessed through Interop Control Provider
#if !defined(PEGASUS_DISABLE_PERFINST) || defined(PEGASUS_ENABLE_SLP)
PEGASUS_COMMON_LINKAGE extern const CIMName PEGASUS_CLASSNAME_OBJECTMANAGER;
PEGASUS_COMMON_LINKAGE extern const CIMName PEGASUS_CLASSNAME_PGNAMESPACE;
PEGASUS_COMMON_LINKAGE
    extern const CIMName PEGASUS_CLASSNAME_OBJECTMANAGERCOMMUNICATIONMECHANISM;
PEGASUS_COMMON_LINKAGE
    extern const CIMName PEGASUS_CLASSNAME_CIMXMLCOMMUNICATIONMECHANISM;
PEGASUS_COMMON_LINKAGE
    extern const CIMName PEGASUS_CLASSNAME_PG_CIMXMLCOMMUNICATIONMECHANISM;
PEGASUS_COMMON_LINKAGE extern const CIMName PEGASUS_CLASSNAME_PROTOCOLADAPTER;
PEGASUS_COMMON_LINKAGE
    extern const CIMName PEGASUS_CLASSNAME_NAMESPACEINMANAGER;

//
// Server Profile-related class names
//
PEGASUS_COMMON_LINKAGE extern const CIMName PEGASUS_CLASSNAME_PG_OBJECTMANAGER;

PEGASUS_COMMON_LINKAGE
    extern const CIMName PEGASUS_CLASSNAME_PG_COMMMECHANISMFORMANAGER;

PEGASUS_COMMON_LINKAGE
    extern const CIMName PEGASUS_CLASSNAME_PG_NAMESPACEINMANAGER;

PEGASUS_COMMON_LINKAGE
    extern const CIMName PEGASUS_CLASSNAME_PG_REGISTEREDPROFILE;
PEGASUS_COMMON_LINKAGE
    extern const CIMName PEGASUS_CLASSNAME_PG_REGISTEREDSUBPROFILE;
PEGASUS_COMMON_LINKAGE
    extern const CIMName PEGASUS_CLASSNAME_PG_REFERENCEDPROFILE;

PEGASUS_COMMON_LINKAGE
    extern const CIMName PEGASUS_CLASSNAME_CIM_ELEMENTCONFORMSTOPROFILE;
PEGASUS_COMMON_LINKAGE
    extern const CIMName PEGASUS_CLASSNAME_PG_ELEMENTCONFORMSTOPROFILE;
PEGASUS_COMMON_LINKAGE
    extern const CIMName PEGASUS_CLASSNAME_PG_SUBPROFILEREQUIRESPROFILE;
PEGASUS_COMMON_LINKAGE
    extern const CIMName PEGASUS_CLASSNAME_PG_SOFTWAREIDENTITY;
PEGASUS_COMMON_LINKAGE
    extern const CIMName PEGASUS_CLASSNAME_PG_ELEMENTSOFTWAREIDENTITY;
PEGASUS_COMMON_LINKAGE
    extern const CIMName PEGASUS_CLASSNAME_PG_INSTALLEDSOFTWAREIDENTITY;

PEGASUS_COMMON_LINKAGE
    extern const CIMName PEGASUS_CLASSNAME_PG_COMPUTERSYSTEM;
PEGASUS_COMMON_LINKAGE
    extern const CIMName PEGASUS_CLASSNAME_PG_HOSTEDOBJECTMANAGER;
PEGASUS_COMMON_LINKAGE
    extern const CIMName PEGASUS_CLASSNAME_PG_HOSTEDACCESSPOINT;

// Registration classes
PEGASUS_COMMON_LINKAGE
    extern const CIMName PEGASUS_CLASSNAME_PG_PROVIDERPROFILECAPABILITIES;
PEGASUS_COMMON_LINKAGE
    extern const CIMName PEGASUS_CLASSNAME_PG_PROVIDERREFERENCEDPROFILES;


// slp Class which operates slp provider. Started by system

PEGASUS_COMMON_LINKAGE extern const CIMName PEGASUS_CLASSNAME_WBEMSLPTEMPLATE;
#endif

//
// Property Names
//

PEGASUS_COMMON_LINKAGE extern const CIMName PEGASUS_PROPERTYNAME_INDSUB_CREATOR;
PEGASUS_COMMON_LINKAGE
    extern const CIMName PEGASUS_PROPERTYNAME_INDSUB_ACCEPTLANGS;
PEGASUS_COMMON_LINKAGE
    extern const CIMName PEGASUS_PROPERTYNAME_INDSUB_CONTENTLANGS;
PEGASUS_COMMON_LINKAGE
    extern const CIMName PEGASUS_PROPERTYNAME_MODULE_USERCONTEXT;
PEGASUS_COMMON_LINKAGE
    extern const CIMName PEGASUS_PROPERTYNAME_MODULE_DESIGNATEDUSER;

/**
    The name of the Destination property for CIM XML Indication Handler
    subclass
*/
PEGASUS_COMMON_LINKAGE
    extern const CIMName PEGASUS_PROPERTYNAME_LSTNRDST_DESTINATION;

/**
    The name of the TargetHost property for SNMP Mapper Indication
    Handler subclass
*/
PEGASUS_COMMON_LINKAGE
    extern const CIMName PEGASUS_PROPERTYNAME_LSTNRDST_TARGETHOST;

/**
    The name of the TextFormat property for Formatted Indication
    Subscription class
*/
    PEGASUS_COMMON_LINKAGE extern const CIMName _PROPERTY_TEXTFORMAT;

/**
    The name of the TextFormatParameters property for Formatted
    Indication Subscription class
*/
    PEGASUS_COMMON_LINKAGE extern const CIMName _PROPERTY_TEXTFORMATPARAMETERS;

/**
    The name of the Name property for PG_ProviderModule class
*/
    PEGASUS_COMMON_LINKAGE extern const CIMName _PROPERTY_PROVIDERMODULE_NAME;

/**
    The name of the operational status property
*/
    PEGASUS_COMMON_LINKAGE extern const CIMName _PROPERTY_OPERATIONALSTATUS;

/**
    The name of the Filter reference property for indication subscription class
 */
PEGASUS_COMMON_LINKAGE extern const CIMName PEGASUS_PROPERTYNAME_FILTER;

/**
    The name of the Handler reference property for indication subscription class
 */
PEGASUS_COMMON_LINKAGE extern const CIMName PEGASUS_PROPERTYNAME_HANDLER;

/**
    The name of the Subscription State property for indication subscription
    class
 */
PEGASUS_COMMON_LINKAGE
    extern const CIMName PEGASUS_PROPERTYNAME_SUBSCRIPTION_STATE;

/**
    The name of the Query property for indication filter class
 */
PEGASUS_COMMON_LINKAGE extern const CIMName PEGASUS_PROPERTYNAME_QUERY;

/**
    The name of the Query Language property for indication filter class
 */
PEGASUS_COMMON_LINKAGE extern const CIMName PEGASUS_PROPERTYNAME_QUERYLANGUAGE;

/**
    The name of the Name property for indication filter and indications handler
    classes
 */
PEGASUS_COMMON_LINKAGE extern const CIMName PEGASUS_PROPERTYNAME_NAME;

/**
    The name of the Creation Class Name property for indication filter and
    indications handler classes
 */
PEGASUS_COMMON_LINKAGE
    extern const CIMName PEGASUS_PROPERTYNAME_CREATIONCLASSNAME;

/**
    The name of the Persistence Type property for Indication Handler class
 */
PEGASUS_COMMON_LINKAGE
    extern const CIMName PEGASUS_PROPERTYNAME_PERSISTENCETYPE;

/**
    The name of the SNMP Version property for SNMP Mapper Indication Handler
    subclass
 */
PEGASUS_COMMON_LINKAGE extern const CIMName PEGASUS_PROPERTYNAME_SNMPVERSION;

//
// CIM Namespace Names
//

PEGASUS_COMMON_LINKAGE
    extern const CIMNamespaceName PEGASUS_NAMESPACENAME_INTEROP;
PEGASUS_COMMON_LINKAGE
    extern const CIMNamespaceName PEGASUS_NAMESPACENAME_INTERNAL;
PEGASUS_COMMON_LINKAGE
    extern const CIMNamespaceName PEGASUS_NAMESPACENAME_CIMV2;

PEGASUS_COMMON_LINKAGE
    extern const CIMNamespaceName PEGASUS_NAMESPACENAME_AUTHORIZATION;
PEGASUS_COMMON_LINKAGE
    extern const CIMNamespaceName PEGASUS_NAMESPACENAME_CONFIG;
PEGASUS_COMMON_LINKAGE
    extern const CIMNamespaceName PEGASUS_NAMESPACENAME_PROVIDERREG;
PEGASUS_COMMON_LINKAGE
    extern const CIMNamespaceName PEGASUS_NAMESPACENAME_SHUTDOWN;
PEGASUS_COMMON_LINKAGE
    extern const CIMNamespaceName PEGASUS_NAMESPACENAME_USER ;
PEGASUS_COMMON_LINKAGE
    extern const CIMNamespaceName PEGASUS_NAMESPACENAME_CERTIFICATE;

#ifndef PEGASUS_DISABLE_PERFINST
PEGASUS_COMMON_LINKAGE
    extern const CIMNamespaceName PEGASUS_NAMESPACENAME_CIMOMSTATDATA;
#endif

#ifndef PEGASUS_DISABLE_CQL
PEGASUS_COMMON_LINKAGE
    extern const CIMNamespaceName PEGASUS_NAMESPACENAME_CIMQUERYCAPABILITIES;
#endif

/**
    An EnumerateInstances operation on the __Namespace class returns the
    "child" namespaces of the namespace in which the operation is requested.
    Since there is no universal top-level namespace, one cannot find all
    namespaces by enumerating the __Namespace class.  To circumvent this
    problem, a virtual top-level namespace is introduced.  An enumeration of
    __Namespace in the virtual top-level namespace returns all namespaces.
*/
PEGASUS_COMMON_LINKAGE
    extern const CIMNamespaceName PEGASUS_VIRTUAL_TOPLEVEL_NAMESPACE;

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


/*
 * Default timeout value for SSL accept pending
 */

#define PEGASUS_SSL_ACCEPT_TIMEOUT_SECONDS 5


//
// Wbem service names
//
#define WBEM_HTTP_SERVICE_NAME "wbem-http"
#define WBEM_HTTPS_SERVICE_NAME "wbem-https"



//
// File system layout
//

#ifdef PEGASUS_USE_RELEASE_DIRS
#ifdef PEGASUS_OVERRIDE_DEFAULT_RELEASE_DIRS
# include <Pegasus/Common/ProductDirectoryStructure.h>
#else
# if defined(PEGASUS_OS_HPUX)
#  undef CIMSERVER_LOCK_FILE
#  define CIMSERVER_LOCK_FILE               "/var/opt/wbem/cimserver_start.lock"
#  undef PEGASUS_REPOSITORY_DIR
#  define PEGASUS_REPOSITORY_DIR            "/var/opt/wbem/repository"
#  undef PEGASUS_CURRENT_CONFIG_FILE_PATH
#  define PEGASUS_CURRENT_CONFIG_FILE_PATH  \
    "/var/opt/wbem/cimserver_current.conf"
#  undef PEGASUS_PLANNED_CONFIG_FILE_PATH
#  define PEGASUS_PLANNED_CONFIG_FILE_PATH  \
    "/var/opt/wbem/cimserver_planned.conf"
#  undef PEGASUS_CIMSERVER_START_FILE
#  define PEGASUS_CIMSERVER_START_FILE      "/etc/opt/wbem/cimserver_start.conf"
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
#  define PEGASUS_PAM_STANDALONE_PROC_NAME  "/opt/wbem/lbin/cimservera"
#  undef PEGASUS_PROVIDER_AGENT_PROC_NAME
#  define PEGASUS_PROVIDER_AGENT_PROC_NAME  "/opt/wbem/lbin/cimprovagt"
# elif defined(PEGASUS_OS_AIX)
#  undef CIMSERVER_LOCK_FILE
#  define CIMSERVER_LOCK_FILE               \
    "/opt/freeware/cimom/pegasus/etc/cimserver_start.lock"
#  undef PEGASUS_REPOSITORY_DIR
#  define PEGASUS_REPOSITORY_DIR            \
    "/opt/freeware/cimom/pegasus/etc/repository"
#  undef PEGASUS_CURRENT_CONFIG_FILE_PATH
#  define PEGASUS_CURRENT_CONFIG_FILE_PATH  \
    "/opt/freeware/cimom/pegasus/etc/cimserver_current.conf"
#  undef PEGASUS_PLANNED_CONFIG_FILE_PATH
#  define PEGASUS_PLANNED_CONFIG_FILE_PATH  \
    "/opt/freeware/cimom/pegasus/etc/cimserver_planned.conf"
#  undef PEGASUS_CIMSERVER_START_FILE
#  define PEGASUS_CIMSERVER_START_FILE      "/tmp/cimserver_start.conf"
#  undef PEGASUS_SSLCLIENT_CERTIFICATEFILE
#  define PEGASUS_SSLCLIENT_CERTIFICATEFILE \
    "/opt/freeware/cimom/pegasus/etc/client.pem"
#  undef PEGASUS_SSLCLIENT_RANDOMFILE
#  define PEGASUS_SSLCLIENT_RANDOMFILE      \
    "/opt/freeware/cimom/pegasus/etc/ssl.rnd"
#  undef PEGASUS_SSLSERVER_RANDOMFILE
#  define PEGASUS_SSLSERVER_RANDOMFILE      \
    "/opt/freeware/cimom/pegasus/etc/cimserver.rnd"
#  undef PEGASUS_LOCAL_DOMAIN_SOCKET_PATH
#  define PEGASUS_LOCAL_DOMAIN_SOCKET_PATH  \
    "/opt/freeware/cimom/pegasus/etc/cimxml.socket"
#  undef PEGASUS_PAM_STANDALONE_PROC_NAME
#  define PEGASUS_PAM_STANDALONE_PROC_NAME  \
    "/opt/freeware/cimom/pegasus/bin/cimservera"
#  undef PEGASUS_PROVIDER_AGENT_PROC_NAME
#  define PEGASUS_PROVIDER_AGENT_PROC_NAME  \
    "/opt/freeware/cimom/pegasus/bin/cimprovagt"
# elif defined(PEGASUS_OS_LINUX) && !defined(PEGASUS_OS_LSB)
#  undef CIMSERVER_LOCK_FILE
#  define CIMSERVER_LOCK_FILE               \
    "/var/opt/tog-pegasus/cimserver_start.lock"
#  undef PEGASUS_REPOSITORY_DIR
#  define PEGASUS_REPOSITORY_DIR            "/var/opt/tog-pegasus/repository"
#  undef PEGASUS_CURRENT_CONFIG_FILE_PATH
#  define PEGASUS_CURRENT_CONFIG_FILE_PATH  \
    "/var/opt/tog-pegasus/cimserver_current.conf"
#  undef PEGASUS_PLANNED_CONFIG_FILE_PATH
#  define PEGASUS_PLANNED_CONFIG_FILE_PATH  \
    "/var/opt/tog-pegasus/cimserver_planned.conf"
#  undef PEGASUS_CIMSERVER_START_FILE
#  define PEGASUS_CIMSERVER_START_FILE      "/var/run/tog-pegasus/cimserver.pid"
#  undef PEGASUS_SSLCLIENT_CERTIFICATEFILE
#  define PEGASUS_SSLCLIENT_CERTIFICATEFILE "/etc/opt/tog-pegasus/client.pem"
#  undef PEGASUS_SSLCLIENT_RANDOMFILE
#  define PEGASUS_SSLCLIENT_RANDOMFILE      "/var/opt/tog-pegasus/ssl.rnd"
#  undef PEGASUS_SSLSERVER_RANDOMFILE
#  define PEGASUS_SSLSERVER_RANDOMFILE      "/var/opt/tog-pegasus/cimserver.rnd"
#  undef PEGASUS_LOCAL_AUTH_DIR
#  define PEGASUS_LOCAL_AUTH_DIR            \
    "/var/opt/tog-pegasus/cache/localauth"
#  undef PEGASUS_LOCAL_DOMAIN_SOCKET_PATH
#  define PEGASUS_LOCAL_DOMAIN_SOCKET_PATH  \
    "/var/run/tog-pegasus/socket/cimxml.socket"
#  undef PEGASUS_CORE_DIR
#  define PEGASUS_CORE_DIR                  "/var/opt/tog-pegasus/cache"
#  undef PEGASUS_PAM_STANDALONE_PROC_NAME
#  define PEGASUS_PAM_STANDALONE_PROC_NAME  "/opt/tog-pegasus/sbin/cimservera"
#  undef PEGASUS_PROVIDER_AGENT_PROC_NAME
#  define PEGASUS_PROVIDER_AGENT_PROC_NAME  "/opt/tog-pegasus/sbin/cimprovagt"
# elif defined(PEGASUS_OS_LSB)
#  undef PEGASUS_REPOSITORY_DIR
#  define PEGASUS_REPOSITORY_DIR            "/var/opt/lsb-pegasus/repository"
#  undef PEGASUS_CURRENT_CONFIG_FILE_PATH
#  define PEGASUS_CURRENT_CONFIG_FILE_PATH  \
    "/etc/opt/lsb-pegasus/cimserver_current.conf"
#  undef PEGASUS_PLANNED_CONFIG_FILE_PATH
#  define PEGASUS_PLANNED_CONFIG_FILE_PATH  \
    "/etc/opt/lsb-pegasus/cimserver_planned.conf"
#  undef PEGASUS_CIMSERVER_START_FILE
#  define PEGASUS_CIMSERVER_START_FILE      "/var/run/cimserver.pid"
#  undef PEGASUS_SSLCLIENT_CERTIFICATEFILE
#  define PEGASUS_SSLCLIENT_CERTIFICATEFILE "/etc/opt/lsb-pegasus/client.pem"
#  undef PEGASUS_SSLCLIENT_RANDOMFILE
#  define PEGASUS_SSLCLIENT_RANDOMFILE      "/var/opt/lsb-pegasus/ssl.rnd"
#  undef PEGASUS_SSLSERVER_RANDOMFILE
#  define PEGASUS_SSLSERVER_RANDOMFILE      "/var/opt/lsb-pegasus/cimserver.rnd"
#  undef PEGASUS_LOCAL_AUTH_DIR
#  define PEGASUS_LOCAL_AUTH_DIR            "/var/opt/lsb-pegasus/localauth"
#  undef PEGASUS_LOCAL_DOMAIN_SOCKET_PATH
#  define PEGASUS_LOCAL_DOMAIN_SOCKET_PATH  \
    "/var/opt/lsb-pegasus/socket/cimxml.socket"
#  undef PEGASUS_CORE_DIR
#  define PEGASUS_CORE_DIR                  "/var/opt/lsb-pegasus"
#  undef PEGASUS_PAM_STANDALONE_PROC_NAME
#  define PEGASUS_PAM_STANDALONE_PROC_NAME  "/opt/lsb-pegasus/sbin/cimservera"
#  undef PEGASUS_PROVIDER_AGENT_PROC_NAME
#  define PEGASUS_PROVIDER_AGENT_PROC_NAME  "/opt/lsb-pegasus/sbin/cimprovagt"
# elif defined(PEGASUS_OS_VMS)
#  undef CIMSERVER_LOCK_FILE
#  define CIMSERVER_LOCK_FILE               \
    "/wbem_var/opt/wbem/cimserver_start.lock"
#  undef PEGASUS_REPOSITORY_DIR
#  define PEGASUS_REPOSITORY_DIR            "/wbem_var/opt/wbem/repository"
#  undef PEGASUS_CURRENT_CONFIG_FILE_PATH
#  define PEGASUS_CURRENT_CONFIG_FILE_PATH  \
    "/wbem_var/opt/wbem/cimserver_current.conf"
#  undef PEGASUS_PLANNED_CONFIG_FILE_PATH
#  define PEGASUS_PLANNED_CONFIG_FILE_PATH  \
    "/wbem_var/opt/wbem/cimserver_planned.conf"
#  undef PEGASUS_CIMSERVER_START_FILE
#  define PEGASUS_CIMSERVER_START_FILE      \
    "/wbem_var/opt/wbem/cimserver_start.conf"
#  undef PEGASUS_SSLCLIENT_CERTIFICATEFILE
#  define PEGASUS_SSLCLIENT_CERTIFICATEFILE \
    "/wbem_etc/opt/hp/sslshare/client.pem"
#  undef PEGASUS_SSLCLIENT_RANDOMFILE
#  define PEGASUS_SSLCLIENT_RANDOMFILE      "/wbem_var/opt/wbem/ssl.rnd"
#  undef PEGASUS_SSLSERVER_RANDOMFILE
#  define PEGASUS_SSLSERVER_RANDOMFILE      "/wbem_var/opt/wbem/cimserver.rnd"
#  undef PEGASUS_LOCAL_AUTH_DIR
#  define PEGASUS_LOCAL_AUTH_DIR            "/wbem_var/opt/wbem/localauth"
#  undef PEGASUS_PAM_STANDALONE_PROC_NAME
#  undef PEGASUS_PROVIDER_AGENT_PROC_NAME
#  define PEGASUS_PROVIDER_AGENT_PROC_NAME  "/wbem_var/opt/wbem/bin/cimprovagt"
# elif defined(PEGASUS_PLATFORM_ZOS_ZSERIES_IBM)
#  undef CIMSERVER_LOCK_FILE
#  define CIMSERVER_LOCK_FILE               "/var/wbem/cimserver_start.lock"
#  undef PEGASUS_REPOSITORY_DIR
#  define PEGASUS_REPOSITORY_DIR            "/var/wbem/repository"
#  undef PEGASUS_CURRENT_CONFIG_FILE_PATH
#  define PEGASUS_CURRENT_CONFIG_FILE_PATH  "/etc/wbem/cimserver_current.conf"
#  undef PEGASUS_PLANNED_CONFIG_FILE_PATH
#  define PEGASUS_PLANNED_CONFIG_FILE_PATH  "/etc/wbem/cimserver_planned.conf"
#  undef PEGASUS_CIMSERVER_START_FILE
#  define PEGASUS_CIMSERVER_START_FILE      "/var/wbem/cimserver.pid"
#  undef PEGASUS_LOCAL_DOMAIN_SOCKET_PATH
#  define PEGASUS_LOCAL_DOMAIN_SOCKET_PATH  "/var/wbem/cimxml.socket"
# elif defined(PEGASUS_OS_DARWIN)
#  undef PEGASUS_CURRENT_CONFIG_FILE_PATH
#  define PEGASUS_CURRENT_CONFIG_FILE_PATH  \
    "/etc/pegasus/cimserver_current.conf"
#  undef PEGASUS_PLANNED_CONFIG_FILE_PATH
#  define PEGASUS_PLANNED_CONFIG_FILE_PATH  \
    "/etc/pegasus/cimserver_planned.conf"
#  undef PEGASUS_CIMSERVER_START_FILE
#  define PEGASUS_CIMSERVER_START_FILE      \
    "/var/cache/pegasus/cimserver_start.conf"
# endif
#endif
#endif

#ifndef CIMSERVER_LOCK_FILE
#define CIMSERVER_LOCK_FILE               "cimserver_start.lock"
#endif
#ifndef PEGASUS_REPOSITORY_DIR
#define PEGASUS_REPOSITORY_DIR            "repository"
#endif
#ifndef PEGASUS_CURRENT_CONFIG_FILE_PATH 
#define PEGASUS_CURRENT_CONFIG_FILE_PATH  "cimserver_current.conf"
#endif
#ifndef PEGASUS_PLANNED_CONFIG_FILE_PATH
#define PEGASUS_PLANNED_CONFIG_FILE_PATH  "cimserver_planned.conf"
#endif
#ifndef PEGASUS_CIMSERVER_START_FILE
#define PEGASUS_CIMSERVER_START_FILE      "/tmp/cimserver_start.conf"
#endif
#ifndef PEGASUS_SSLCLIENT_CERTIFICATEFILE
#define PEGASUS_SSLCLIENT_CERTIFICATEFILE "client.pem"
#endif
#ifndef PEGASUS_SSLCLIENT_RANDOMFILE
#define PEGASUS_SSLCLIENT_RANDOMFILE      "ssl.rnd"
#endif
#ifndef PEGASUS_SSLSERVER_RANDOMFILE
#define PEGASUS_SSLSERVER_RANDOMFILE      "cimserver.rnd"
#endif
#ifndef PEGASUS_LOCAL_AUTH_DIR
#define PEGASUS_LOCAL_AUTH_DIR            "/tmp"
#endif
#ifndef PEGASUS_LOCAL_DOMAIN_SOCKET_PATH
#define PEGASUS_LOCAL_DOMAIN_SOCKET_PATH  "/tmp/cimxml.socket"
#endif
#ifndef PEGASUS_PAM_STANDALONE_PROC_NAME
#define PEGASUS_PAM_STANDALONE_PROC_NAME  "bin/cimservera"
#endif
#ifndef PEGASUS_PROVIDER_AGENT_PROC_NAME
#define PEGASUS_PROVIDER_AGENT_PROC_NAME  "bin/cimprovagt"
#endif

// Use the configuration file as a semaphore for repository access
#define PEGASUS_REPOSITORY_LOCK_FILE PEGASUS_CURRENT_CONFIG_FILE_PATH

// Constant defines for path to icu resource bundles
#if defined(PEGASUS_PLATFORM_OS400_ISERIES_IBM)
# define PEGASUS_DEFAULT_MESSAGE_SOURCE  "/QIBM/ProdData/OS400/CIM/msg"
#elif defined(PEGASUS_PLATFORM_ZOS_ZSERIES_IBM)
# define PEGASUS_DEFAULT_MESSAGE_SOURCE  "/usr/lpp/wbem/msg"
#elif defined(PEGASUS_PLATFORM_AIX_RS_IBMCXX) && \
      !defined(PEGASUS_DEFAULT_MESSAGE_SOURCE)
# define PEGASUS_DEFAULT_MESSAGE_SOURCE  "/opt/freeware/cimom/pegasus/msg"
#else
# undef PEGASUS_DEFAULT_MESSAGE_SOURCE
#endif

//
// Miscellaneous Constants
//

// Constant defines the maximum number of providers that will be addressed in
// a single enumerate instance request.  This is strictly a performance
// requirement for different systems.  To disable the whole test, simply set
// this variable to 0
#define MAX_ENUMERATE_BREADTH 30

// Constant defines the maximum timeout to disable a provider
#define PROVIDER_DISABLE_TIMEOUT 15

// Constants defining the operational status of a CIM_ManagedSystemElement
#define CIM_MSE_OPSTATUS_VALUE_OK 2
#define CIM_MSE_OPSTATUS_VALUE_DEGRADED 3
#define CIM_MSE_OPSTATUS_VALUE_STOPPING 9
#define CIM_MSE_OPSTATUS_VALUE_STOPPED 10

// Constants defining UserContext values in a PG_ProviderModule
#define PG_PROVMODULE_USERCTXT_REQUESTOR 2
#define PG_PROVMODULE_USERCTXT_DESIGNATED 3
#define PG_PROVMODULE_USERCTXT_PRIVILEGED 4
#define PG_PROVMODULE_USERCTXT_CIMSERVER 5

// Constant defining Server truststore type value in a PG_SSLCertificate
#define PG_SSLCERTIFICATE_TSTYPE_VALUE_SERVER Uint16(2)

#ifdef PEGASUS_DEFAULT_USERCTXT_REQUESTOR
#define PEGASUS_DEFAULT_PROV_USERCTXT PG_PROVMODULE_USERCTXT_REQUESTOR
#else
#define PEGASUS_DEFAULT_PROV_USERCTXT PG_PROVMODULE_USERCTXT_PRIVILEGED
#endif

// Standard Pegasus Global Prefix.
// This prefix is used as the basis for pegasus defined classes
// and in identity creation that would require a standard
// Pegasus prefix
#define PEGASUS_INSTANCEID_GLOBAL_PREFIX "PG"

/**
    Values for the Perceived Severity property of the Alert Indication
    class, as defined in the CIM Events MOF
 */
enum PerceivedSeverity {_SEVERITY_UNKNOWN = 0, _SEVERITY_OTHER = 1,
    _SEVERITY_INFORMATION = 2, _SEVERITY_WARNING = 3, _SEVERITY_MINOR = 4,
    _SEVERITY_MAJOR = 5, _SEVERITY_CRITICAL = 6, _SEVERITY_FATAL = 7};

/**
    The name of the MailTo property for Email Handler subclass
*/
PEGASUS_COMMON_LINKAGE extern const CIMName
    PEGASUS_PROPERTYNAME_LSTNRDST_MAILTO;

/**
    The name of the MailSubject property for Email Handler subclass
*/
PEGASUS_COMMON_LINKAGE extern const CIMName
    PEGASUS_PROPERTYNAME_LSTNRDST_MAILSUBJECT;

/**
    The name of the MailCc  property for Email Handler subclass
*/
PEGASUS_COMMON_LINKAGE extern const
    CIMName PEGASUS_PROPERTYNAME_LSTNRDST_MAILCC;

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Constants_h */
