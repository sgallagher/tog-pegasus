//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software, Hewlett-Packard Company, IBM,
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
// Author: Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
// Modified By: Carol Ann Krug Graves, Hewlett-Packard Company
//                (carolann_graves@hp.com)
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
#define PEGASUS_MODULENAME_TEMP \
    PEGASUS_QUEUENAME_CONTROLSERVICE "::temp::do not use this name"
#define PEGASUS_MODULENAME_MONITOR \
    PEGASUS_QUEUENAME_CONTROLSERVICE "::Monitor"

//
// CIM Class Names
//

static const CIMName PEGASUS_CLASSNAME_CONFIGSETTING        = 
    CIMName ("PG_ConfigSetting");
static const CIMName PEGASUS_CLASSNAME_AUTHORIZATION        = 
    CIMName ("PG_Authorization");
static const CIMName PEGASUS_CLASSNAME_USER                 = 
    CIMName ("PG_User");
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
static const CIMName PEGASUS_CLASSNAME_INDHANDLER           = 
    CIMName ("CIM_IndicationHandler");
static const CIMName PEGASUS_CLASSNAME_INDHANDLER_CIMXML    = 
    CIMName ("CIM_IndicationHandlerCIMXML");
static const CIMName PEGASUS_CLASSNAME_INDHANDLER_SNMP      = 
    CIMName ("PG_IndicationHandlerSNMPMapper");
static const CIMName PEGASUS_CLASSNAME_INDFILTER            = 
    CIMName ("CIM_IndicationFilter");
static const CIMName PEGASUS_CLASSNAME_SHUTDOWN             = 
    CIMName ("PG_ShutdownService");
static const CIMName PEGASUS_CLASSNAME___NAMESPACE          = 
    CIMName ("__Namespace");
static const CIMName PEGASUS_CLASSNAME_NAMESPACE            = 
    CIMName ("CIM_Namespace");


//
// Property Names
//

static const CIMName PEGASUS_PROPERTYNAME_INDSUB_CREATOR    = 
    CIMName ("Creator");

// l10n    
static const CIMName PEGASUS_PROPERTYNAME_INDSUB_ACCEPTLANGS = 
    CIMName ("AcceptLanguages"); 
static const CIMName PEGASUS_PROPERTYNAME_INDSUB_CONTENTLANGS = 
    CIMName ("ContentLanguages");  

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


//
// Wbem service names
//


#ifdef PEGASUS_OS_OS400
#pragma convert(37)
#define WBEM_HTTP_SERVICE_NAME "wbem-http"
#define WBEM_HTTPS_SERVICE_NAME "wbem-https"
#pragma convert(0)
#else
#define WBEM_HTTP_SERVICE_NAME "wbem-http"
#define WBEM_HTTPS_SERVICE_NAME "wbem-https"
#endif



//
// File system layout
//

#define PEGASUS_SSLCLIENT_CERTIFICATEFILE "client.pem"
#define PEGASUS_SSLCLIENT_RANDOMFILE      "ssl.rnd"
#define PEGASUS_SSLSERVER_RANDOMFILE      "cimserver.rnd"
#define PEGASUS_LOCAL_AUTH_DIR            "/tmp"
// ATTN: Relocate this relative to PEGASUS_HOME
#define PEGASUS_LOCAL_DOMAIN_SOCKET_PATH  "/tmp/cimxml.socket"

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
# elif defined(PEGASUS_PLATFORM_LINUX_GENERIC_GNU)
#  undef PEGASUS_SSLCLIENT_CERTIFICATEFILE
#  define PEGASUS_SSLCLIENT_CERTIFICATEFILE "/var/cache/pegasus/client.pem"
#  undef PEGASUS_SSLCLIENT_RANDOMFILE
#  define PEGASUS_SSLCLIENT_RANDOMFILE      "/var/cache/pegasus/ssl.rnd"
#  undef PEGASUS_SSLSERVER_RANDOMFILE
#  define PEGASUS_SSLSERVER_RANDOMFILE      "/var/cache/pegasus/cimserver.rnd"
#  undef PEGASUS_LOCAL_AUTH_DIR
#  define PEGASUS_LOCAL_AUTH_DIR            "/var/cache/pegasus/localauth"
#  undef PEGASUS_LOCAL_DOMAIN_SOCKET_PATH
#  define PEGASUS_LOCAL_DOMAIN_SOCKET_PATH  "/var/cache/pegasus/cimxml.socket"
# endif
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

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Constants_h */
