include $(PEGASUS_ROOT)/mak/config.mak

DIR = Pegasus/ProviderManager2/Default/tests/ProviderFacade

LIBRARIES = \
	pegcommon \
	pegprovider \
	DefaultProviderManager

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = TestProviderFacade

SOURCES = ProviderFacade.cpp

include $(PEGASUS_ROOT)/mak/program.mak

tests:
	$(PROGRAM)

poststarttests:
