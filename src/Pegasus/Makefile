ROOT=$(PEGASUS_ROOT)

include $(ROOT)/mak/config.mak


DIRS = \
    Common \
    Config \
    Repository \
    getoopt \
    Client \
    Compiler \
    Compiler/cmdline \
    ExportClient \
    WQL \
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
    ManagedClient

ifdef PEGASUS_ENABLE_SLP
DIRS += \
    ControlProviders/InteropProvider \
    ControlProviders/InteropProvider/tests
endif

ifdef PEGASUS_USE_23PROVIDER_MANAGER
DIRS += \
	ProviderManager \
	ProviderManager/tests
else
DIRS += \
        ProviderManager2 \
	ProviderManager2/Default \
	ProviderManager2/ProviderAgent \
	ProviderManager2/tests \
	ProviderManager2/Default/tests
endif

ifdef PEGASUS_ENABLE_CMPI_PROVIDER_MANAGER
DIRS += \
        Provider/CMPI \
	ProviderManager2/CMPI
endif


ifdef ENABLE_REMOTE_CMPI
DIRS += \
	ProviderManager2/CMPIR \
	ProviderManager2/CMPIR/tcpcomm \
	ProviderManager2/CMPIR/native \
	ProviderManager2/CMPIR/tcpcommrem \
	ProviderManager2/CMPIR/daemon
endif

ifeq ($(PEGASUS_PLATFORM),LINUX_IX86_GNU)
   ifdef PEGASUS_ENABLE_JMPI_PROVIDER_MANAGER
      DIRS += \
         ProviderManager2/JMPI
   endif
endif

ifdef PEGASUS_USE_PAM_STANDALONE_PROC
DIRS += \
	Security/Cimservera
endif

ifdef PEGASUS_HAS_PERFINST
DIRS += \
	ControlProviders/Statistic \
	ControlProviders/Statistic/test \
	ControlProviders/InteropProvider \
    	ControlProviders/InteropProvider/tests

endif
	
DIRS += \
	Server \
    Handler/CIMxmlIndicationHandler \
    Handler/sendmailIndicationHandler \
    Handler/snmpIndicationHandler \
    Common/tests \
    Config/tests \
    Repository/tests \
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
    msg/CLI

include $(ROOT)/mak/recurse.mak
