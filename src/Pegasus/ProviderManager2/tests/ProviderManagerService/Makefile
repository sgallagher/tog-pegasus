include $(PEGASUS_ROOT)/mak/config.mak

DIR = Pegasus/ProviderManager2/tests/ProviderManagerService

LIBRARIES = \
    pegprovidermanager \
    pegprovider \
    pegconfig \
    pegcommon 

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = TestProviderManagerService

SOURCES = ProviderManagerService.cpp

include $(PEGASUS_ROOT)/mak/program.mak

tests:
	$(PROGRAM)

poststarttests:
install:
