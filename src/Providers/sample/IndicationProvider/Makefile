ROOT = ../../../..
include ../../libraries.mak

DIR = Providers/sample/IndicationProvider

include $(ROOT)/mak/config.mak

LIBRARY = SampleIndicationProvider

LIBRARIES = \
	 pegcommon \
	 pegprovider \
	 pegprovider2
	
SOURCES = \
   IndicationProviderMain.cpp \
   IndicationProvider.cpp

include $(ROOT)/mak/library.mak

tests:
