//%/////////////////////////////////////////////////////////////////////////////
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
// Author: Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Constants_h
#define Pegasus_Constants_h

#include <Pegasus/Common/Config.h>

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

#define PEGASUS_CLASSNAME_CONFIGSETTING        "PG_ConfigSetting"
#define PEGASUS_CLASSNAME_AUTHORIZATION        "PG_Authorization"
#define PEGASUS_CLASSNAME_USER                 "PG_User"
#define PEGASUS_CLASSNAME_PROVIDERMODULE       "PG_ProviderModule"
#define PEGASUS_CLASSNAME_PROVIDER             "PG_Provider"
#define PEGASUS_CLASSNAME_PROVIDERCAPABILITIES "PG_ProviderCapabilities"
#define PEGASUS_CLASSNAME_INDSUBSCRIPTION      "PG_IndicationSubscription"
#define PEGASUS_CLASSNAME_INDHANDLER           "PG_IndicationHandler"
#define PEGASUS_CLASSNAME_INDHANDLER_CIMXML    "PG_IndicationHandlerCIMXML"
#define PEGASUS_CLASSNAME_INDHANDLER_SNMP      "PG_IndicationHandlerSNMPMapper"
#define PEGASUS_CLASSNAME_INDFILTER            "PG_IndicationFilter"
#define PEGASUS_CLASSNAME_SHUTDOWN             "PG_ShutdownService"
#define PEGASUS_CLASSNAME___NAMESPACE          "__Namespace"
#define PEGASUS_CLASSNAME_NAMESPACE            "CIM_Namespace"



//
// Property Names
//

#define PEGASUS_PROPERTYNAME_INDSUB_CREATOR    "Creator"


//
// CIM Namespace Names
//

#define PEGASUS_NAMESPACENAME_INTEROP  "root/PG_InterOp"
#define PEGASUS_NAMESPACENAME_INTERNAL "root/PG_Internal"
#define PEGASUS_NAMESPACENAME_CIMV2    "root/cimv2"

#define PEGASUS_NAMESPACENAME_AUTHORIZATION PEGASUS_NAMESPACENAME_INTERNAL
#define PEGASUS_NAMESPACENAME_PROVIDERREG   PEGASUS_NAMESPACENAME_INTEROP

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
#define WBEM_HTTP_SERVICE_NAME "wbem-http"
#define WBEM_HTTPS_SERVICE_NAME "wbem-https"

//
// Miscellaneous Constants
//

// Constant defines the maximum number of providers that will be addressed in a single
// enumerate instance request.  This is strictly a performance requirement for different
// systems.  To disable the whole test, simply set this variable to 0
#define MAX_ENUMERATE_BREADTH 30

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Constants_h */
