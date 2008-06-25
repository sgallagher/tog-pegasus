#//%2006////////////////////////////////////////////////////////////////////////
#//
#// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
#// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
#// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
#// IBM Corp.; EMC Corporation, The Open Group.
#// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
#// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
#// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
#// EMC Corporation; VERITAS Software Corporation; The Open Group.
#// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
#// EMC Corporation; Symantec Corporation; The Open Group.
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
    Client \
    Config \
    Repository \
    getoopt \
    WQL \
    CQL \
    Query/QueryExpression \
    Provider \
    Compiler \
    Compiler/cmdline

TEST_DIRS = \
    Common/tests \
    Client/tests \
    Config/tests \
    Repository/tests \
    WQL/tests \
    CQL/tests \
    Compiler/tests \
    Query/QueryExpression/tests \
    Query/QueryExpression/tests/Queries \
    CQL/CQLCLI

ifneq ($(OS),HPUX)
    TEST_DIRS += \
        Compiler/cmdline/tests
endif

DIRS += \
    ExportClient \
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
    DynListener \

TEST_DIRS += \
    ExportClient/tests \
    Server/ProviderRegistrationManager/tests \
    ExportServer/tests \
    Listener/tests \
    Security/UserManager/tests \
    Security/Authentication/tests \
    ControlProviders/ProviderRegistrationProvider/tests \
    HandlerService/tests \
    IndicationService/tests \
    DynListener/tests


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
    ProviderManager2/Default

TEST_DIRS += \
    ProviderManager2/tests \
    ProviderManager2/Default/tests

ifeq ($(PEGASUS_ENABLE_CMPI_PROVIDER_MANAGER), true)
    DIRS += \
        Provider/CMPI \
        ProviderManager2/CMPI
endif

ifdef PEGASUS_ENABLE_REMOTE_CMPI

    DIRS += \
        ProviderManager2/CMPIR/native \
        ProviderManager2/CMPIR \
        ProviderManager2/CMPIR/tcpcomm \
        ProviderManager2/CMPIR/tcpcommrem \
        ProviderManager2/CMPIR/daemon \
        ProviderManager2/CMPIR/tools
endif

ifeq ($(PEGASUS_ENABLE_JMPI_PROVIDER_MANAGER), true)
    DIRS += \
        ProviderManager2/JMPI
endif

DIRS += \
    ProviderManagerService \
    ProviderManagerService/ProviderAgent

ifdef PEGASUS_USE_PAM_STANDALONE_PROC
    DIRS += \
        Security/Cimservera
endif

ifndef PEGASUS_DISABLE_PERFINST
    DIRS += ControlProviders/Statistic
    TEST_DIRS += ControlProviders/Statistic/test
endif

ifeq ($(PEGASUS_ENABLE_INTEROP_PROVIDER),true)
    DIRS += ControlProviders/InteropProvider
    TEST_DIRS += \
        ControlProviders/InteropProvider/tests \
        ControlProviders/InteropProvider/ServerProfileTests
endif

ifndef PEGASUS_DISABLE_CQL
    DIRS += \
        ControlProviders/QueryCapabilitiesProvider
    TEST_DIRS += \
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
    msg/Server \
    msg/CLI \
    msg/Listener

TEST_DIRS += \
    Server/tests \
    Handler/CIMxmlIndicationHandler/tests/Destination \
    Handler/snmpIndicationHandler/tests/testclient \
    Handler/snmpIndicationHandler/tests/SnmpHandlerException 
    

include $(ROOT)/mak/recurse.mak
