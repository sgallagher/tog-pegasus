ROOT = ../../../..
include ../../libraries.mak

DIR = Providers/sample/InstanceProvider

include $(ROOT)/mak/config.mak

LIBRARY = SampleInstanceProvider

LIBRARIES = \
	 pegcommon \
	 pegprovider \
	 pegprovider2
	
SOURCES = \
   InstanceProviderMain.cpp \
   InstanceProvider.cpp

include $(ROOT)/mak/library.mak

tests:
