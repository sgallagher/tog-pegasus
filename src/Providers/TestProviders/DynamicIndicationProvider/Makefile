ROOT = ../../../..

DIR = Providers/TestProviders/DynamicIndicationProvider

include $(ROOT)/mak/config.mak

LOCAL_DEFINES = -DPEGASUS_CONSUMER_INTERNAL -DPEGASUS_INTERNALONLY 

LIBRARY = DynamicIndicationProvider

LIBRARIES = \
	 pegcommon \
	 pegprovider
	
SOURCES = \
   DynamicIndicationProviderMain.cpp \
   DynamicIndicationProvider.cpp

include $(ROOT)/mak/library.mak
install:
