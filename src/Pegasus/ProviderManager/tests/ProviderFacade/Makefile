include $(PEGASUS_ROOT)/mak/config.mak

DIR = Pegasus/Server/tests/ProviderFacade

LIBRARIES = \
	pegcommon \
	pegprovider \
	pegprovidermanager

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = TestProviderFacade

SOURCES = ProviderFacade.cpp

include $(PEGASUS_ROOT)/mak/program.mak

tests:
	$(PROGRAM)

poststarttests:
