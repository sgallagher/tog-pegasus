ROOT = ../../../..

DIR = Providers/sample/LocalizedProvider

include $(ROOT)/mak/config.mak

LIBRARY = SampleLocalizedProvider

LIBRARIES = \
    pegprovider \
    pegclient \
    pegcommon
	
SOURCES = \
	LocalizedProviderMain.cpp \
	LocalizedProvider.cpp

include $(ROOT)/mak/dynamic-library.mak

tests:

