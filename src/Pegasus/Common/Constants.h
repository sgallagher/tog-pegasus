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

#define PEGASUS_SERVICENAME_CIMOPREQDISPATCHER  "CIMOpRequestDispatcher"
#define PEGASUS_SERVICENAME_CIMOPREQDECODER     "CIMOpRequestDecoder"
#define PEGASUS_SERVICENAME_CIMOPREQENCODER     "CIMOpResponseEncoder"
#define PEGASUS_SERVICENAME_CIMOPREQAUTHORIZER  "CIMOpRequestAuthorizer"
#define PEGASUS_SERVICENAME_HTTPAUTHDELEGATOR   "HTTPAuthenticatorDelegator"
#define PEGASUS_SERVICENAME_CONTROLSERVICE      "ControlService"
#define PEGASUS_SERVICENAME_INDICATIONSERVICE   "Server::IndicationService"
#define PEGASUS_SERVICENAME_PROVIDERMANAGER_CPP "Server::ProviderManagerService"
#define PEGASUS_SERVICENAME_HANDLERMANAGER      "IndicationHandlerService"

#define PEGASUS_MODULENAME_CONFIGPROVIDER \
    PEGASUS_SERVICENAME_CONTROLSERVICE "::ConfigProvider"
#define PEGASUS_MODULENAME_USERAUTHPROVIDER \
    PEGASUS_SERVICENAME_CONTROLSERVICE "::UserAuthProvider"
#define PEGASUS_MODULENAME_PROVREGPROVIDER \
    PEGASUS_SERVICENAME_CONTROLSERVICE "::ProviderRegistrationProvider"
#define PEGASUS_MODULENAME_SHUTDOWNPROVIDER \
    PEGASUS_SERVICENAME_CONTROLSERVICE "::ShutdownProvider"

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

#define PEGASUS_NAMESPACENAME_INTEROP  "root/PG_InterOp"
#define PEGASUS_NAMESPACENAME_INTERNAL "root/PG_Internal"
#define PEGASUS_NAMESPACENAME_CIMV2    "root/cimv2"

#define PEGASUS_NAMESPACENAME_AUTHORIZATION PEGASUS_NAMESPACENAME_INTERNAL
#define PEGASUS_NAMESPACENAME_PROVIDERREG   PEGASUS_NAMESPACENAME_INTEROP

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Constants_h */
