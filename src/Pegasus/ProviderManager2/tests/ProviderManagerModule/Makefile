include $(PEGASUS_ROOT)/mak/config.mak

DIR = Pegasus/ProviderManager2/tests/ProviderManagerModule

LIBRARIES = \
	pegcommon \
	pegprovider \
	pegprovidermanager

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = TestProviderManagerModule

SOURCES = ProviderManagerModule.cpp

include $(PEGASUS_ROOT)/mak/program.mak

tests:
	$(PROGRAM)

poststarttests:
