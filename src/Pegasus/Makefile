#//%2005////////////////////////////////////////////////////////////////////////
#//
#// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
#// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
#// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
#// IBM Corp.; EMC Corporation, The Open Group.
#// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
#// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
#// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
#// EMC Corporation; VERITAS Software Corporation; The Open Group.
#//
#// Permission is hereby granted, free of charge, to any person obtaining a copy
#// of this software and associated documentation files (the "Software"), to
#// deal in the Software without restriction, including without limitation the
#// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
#// sell copies of the Software, and to permit persons to whom the Software is
#// furnished to do so, subject to the following conditions:
#// 
#// THE ABOVE COPYRIGHT NOTICE AND THIS PERMISSION NOTICE SHALL BE INCLUDED IN
#// ALL COPIES OR SUBSTANTIAL PORTIONS OF THE SOFTWARE. THE SOFTWARE IS PROVIDED
#// "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
#// LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
#// PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
#// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
#// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
#// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#//
#//==============================================================================
ROOT=$(PEGASUS_ROOT)

include $(ROOT)/mak/config.mak


DIRS = \
    Common \
    Query/QueryCommon \
    Config \
    Repository \
    getoopt \
    Client \
    Compiler \
    Compiler/cmdline \
    ExportClient \
    WQL \
    CQL \
    Query/QueryExpression \
    Provider \
    Server/ProviderRegistrationManager \
    ExportServer \
    Listener \
    Security/UserManager \
    Security/Authentication \
    ControlProviders/ConfigSettingProvider \
    ControlProviders/UserAuthProvider \
    ControlProviders/ProviderRegistrationProvider \
    ControlProviders/NamespaceProvider \
    HandlerService \
    IndicationService \
    ManagedClient \
    DynListener


ifdef PEGASUS_ENABLE_SYSTEM_LOG_HANDLER
    DIRS += \
        Handler/SystemLogListenerDestination
endif

ifdef PEGASUS_ENABLE_EMAIL_HANDLER
    DIRS += \
        Handler/EmailListenerDestination
endif

DIRS += \
        ProviderManager2 \
	ProviderManager2/Default \
	ProviderManager2/ProviderAgent \
	ProviderManager2/tests \
	ProviderManager2/Default/tests 

ifdef PEGASUS_ENABLE_CMPI_PROVIDER_MANAGER
DIRS += \
	Provider/CMPI \
	ProviderManager2/CMPI
endif


ifdef PEGASUS_ENABLE_REMOTE_CMPI
  ifeq ($(PEGASUS_PLATFORM), WIN32_IX86_MSVC)
 $(error Error: Remote CMPI is not currently supported on Windows.)
  endif
    DIRS += \
	ProviderManager2/CMPIR \
	ProviderManager2/CMPIR/tcpcomm 

  ifeq ($(PEGASUS_PLATFORM),LINUX_IX86_GNU)
   DIRS += \
	ProviderManager2/CMPIR/native \
	ProviderManager2/CMPIR/tcpcommrem \
	ProviderManager2/CMPIR/daemon
  endif
endif

ifdef PEGASUS_ENABLE_JMPI_PROVIDER_MANAGER
  ifeq ($(PEGASUS_PLATFORM), WIN32_IX86_MSVC)
      DIRS += \
         ProviderManager2/JMPI
  endif
  ifeq ($(PEGASUS_PLATFORM),LINUX_IX86_GNU)
      DIRS += \
         ProviderManager2/JMPI
  endif
  ifeq ($(PEGASUS_PLATFORM),ZOS_ZSERIES_IBM)
      DIRS += \
         ProviderManager2/JMPI
  endif
endif

ifdef PEGASUS_USE_PAM_STANDALONE_PROC
DIRS += \
	Security/Cimservera
endif

ifndef PEGASUS_DISABLE_PERFINST
DIRS += \
	ControlProviders/Statistic \
	ControlProviders/Statistic/test
endif

ifdef PEGASUS_ENABLE_SLP
DIRS += \
    ControlProviders/InteropProvider \
    ControlProviders/InteropProvider/tests
else
    ifndef PEGASUS_DISABLE_PERFINST
    DIRS += \
        ControlProviders/InteropProvider \
        ControlProviders/InteropProvider/tests
	endif
endif

ifndef PEGASUS_DISABLE_CQL
DIRS += \
	ControlProviders/QueryCapabilitiesProvider \
	ControlProviders/QueryCapabilitiesProvider/tests
endif


ifdef PEGASUS_HAS_SSL
DIRS += \
	ControlProviders/CertificateProvider
endif


DIRS += \
	Server \
    Handler/CIMxmlIndicationHandler \
    Handler/snmpIndicationHandler \
    Common/tests \
    Config/tests \
    Repository/tests \
    CQL/tests \
    CQL/CQLCLI \
    Query/QueryExpression/tests \
    Query/QueryExpression/tests/Queries \
    Server/tests \
    Server/ProviderRegistrationManager/tests \
    ExportClient/tests \
    ExportServer/tests \
    Client/tests \
    Compiler/tests \
    Security/Authentication/tests \
    Security/UserManager/tests \
    IndicationService/tests \
    ControlProviders/ProviderRegistrationProvider/tests \
    Listener/tests \
    Consumer/tests \
    msg/Server \
    msg/CLI \
    DynListener/tests \
    WQL/tests \
    msg/Listener

ifneq ($(OS),HPUX)
  DIRS+= \
    Compiler/cmdline/tests
endif

include $(ROOT)/mak/recurse.mak
