ROOT = $(PEGASUS_ROOT)

DIR = Providers/sample/FamilyProvider

include $(ROOT)/mak/config.mak

LIBRARY = SampleFamilyProvider

LIBRARIES = \
    pegprovider \
    pegclient \
    pegcommon
	
SOURCES = \
	SampleFamilyProviderMain.cpp \
	SampleFamilyProvider.cpp

include $(ROOT)/mak/dynamic-library.mak

tests:
install:
