ROOT = $(PEGASUS_ROOT)

DIR = Providers/TestProviders/EmbeddedInstanceProvider

include $(ROOT)/mak/config.mak

LIBRARY = EmbeddedInstanceProvider

LIBRARIES = \
	pegprovider \
	pegcommon
	
SOURCES = \
	EmbeddedInstanceProviderMain.cpp \
	EmbeddedInstanceProvider.cpp

include $(ROOT)/mak/library.mak

tests:

poststarttests:
