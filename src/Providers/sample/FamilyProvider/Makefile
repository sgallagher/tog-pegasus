ROOT = $(PEGASUS_ROOT)
include ../../libraries.mak

DIR = Providers/sample/FamilyProvider

include $(ROOT)/mak/config.mak

LIBRARY = SampleFamilyProvider

LIBRARIES = \
	pegcommon \
	pegprovider
	
SOURCES = \
	SampleFamilyProviderMain.cpp \
	SampleFamilyProvider.cpp

include $(ROOT)/mak/library.mak

tests:
