include $(PEGASUS_ROOT)/mak/config.mak

DIR = Pegasus/ProviderManager2/Default/tests/ProviderManager

LIBRARIES = \
    DefaultProviderManager \
    pegprovider \
    pegprovidermanager \
    pegconfig \
    pegclient \
    pegcommon

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = TestProviderManager

SOURCES = ProviderManager.cpp

include $(PEGASUS_ROOT)/mak/program.mak

tests:
	$(PROGRAM)

poststarttests:
