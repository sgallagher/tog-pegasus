ROOT=$(PEGASUS_ROOT)

include $(ROOT)/mak/config.mak


DIRS = \
    Common \
    Config \
    Repository \
    getoopt \
    Client \
    Compiler \
    Compiler/load \
    Compiler/cmdline \
    ExportClient \
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
    WQL \
    HandlerService \
    IndicationService

ifdef ENABLE_PROVIDER_MANAGER2
DIRS += \
    ProviderManager2 \
	ProviderManager2/Default \
	ProviderManager2/CMPI \
	ProviderManager2/tests \
	ProviderManager2/Default/tests
else
DIRS += \
	ProviderManager \
	ProviderManager/tests
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
