include $(PEGASUS_ROOT)/mak/config.mak

DIR = Pegasus/ProviderManager2/Default/tests/ProviderModule

LIBRARIES = \
	pegcommon \
	pegprovider \
	DefaultProviderManager

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = TestProviderModule

SOURCES = ProviderModule.cpp

include $(PEGASUS_ROOT)/mak/program.mak

tests:
	$(PROGRAM)

poststarttests:
