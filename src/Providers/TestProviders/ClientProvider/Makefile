ROOT = ../../../..

DIR = Providers/TestProviders/ClientProvider

include $(ROOT)/mak/config.mak

LIBRARY = TestClientProvider

LIBRARIES = \
    pegprovider \
    pegclient \
    pegcommon
	
SOURCES = \
   ClientProviderMain.cpp \
   ClientProvider.cpp

include $(ROOT)/mak/dynamic-library.mak

tests:
install:
