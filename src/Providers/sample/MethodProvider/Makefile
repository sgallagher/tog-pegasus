ROOT = ../../../..

DIR = Providers/sample/MethodProvider

include $(ROOT)/mak/config.mak

LIBRARY = SampleMethodProvider

LIBRARIES = \
    pegprovider \
    pegclient \
    pegcommon
	
SOURCES = \
   MethodProviderMain.cpp \
   MethodProvider.cpp

include $(ROOT)/mak/dynamic-library.mak

tests:
