ROOT = $(PEGASUS_ROOT)

DIR = Providers/TestProviders/EmbeddedInstanceProvider

include $(ROOT)/mak/config.mak

LIBRARY = EmbeddedInstanceProvider

LIBRARIES = \
	pegprovider \
	pegclient \
	pegcommon
	
SOURCES = \
	EmbeddedInstanceProviderMain.cpp \
	EmbeddedInstanceProvider.cpp

include $(ROOT)/mak/dynamic-library.mak

tests:

poststarttests:
