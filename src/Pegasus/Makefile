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

ifdef PEGASUS_ENABLE_SLP
DIRS += \
    ControlProviders/InteropProvider \
    ControlProviders/InteropProvider/tests
endif

# temporary add ifeq ($(OS),HPUX) until Bug 2754 is fixed
ifeq ($(OS),HPUX)
  ifdef PEGASUS_ENABLE_SYSTEM_LOG_HANDLER
    DIRS += \
        Handler/SystemLogListenerDestination
  endif
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
 $(error Error: JMPI is currently not supported on Windows.)
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
	ControlProviders/Statistic/test \
	ControlProviders/InteropProvider

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
    Handler/sendmailIndicationHandler \
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
    DynListener/tests

include $(ROOT)/mak/recurse.mak
