ROOT = ../../../..
include ../../libraries.mak

DIR = Providers/sample/HelloWorldProvider

include $(ROOT)/mak/config.mak

LIBRARY = HelloWorldProvider

LIBRARIES = \
	 pegcommon \
	 pegprovider \
	 pegprovidermanager
	
SOURCES = \
   HelloWorldProviderMain.cpp \
   HelloWorldProvider.cpp

include $(ROOT)/mak/library.mak

tests:
