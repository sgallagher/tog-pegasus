ROOT = $(PEGASUS_ROOT)

DIR = Providers/TestProviders/FamilyProvider

include $(ROOT)/mak/config.mak

LIBRARY = FamilyProvider

LIBRARIES = \
    pegprovider \
    pegclient \
    pegcommon

SOURCES = \
	FamilyProviderMain.cpp \
	FamilyProvider.cpp

include $(ROOT)/mak/dynamic-library.mak

tests:

poststarttests: