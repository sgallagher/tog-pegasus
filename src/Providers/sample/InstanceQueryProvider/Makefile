ROOT = ../../../..

DIR = Providers/sample/InstanceQueryProvider

include $(ROOT)/mak/config.mak

LIBRARY = SampleInstanceQueryProvider

LIBRARIES = \
	pegcommon \
	pegqueryexpression \
	pegprovider
	
SOURCES = \
	InstanceQueryProviderMain.cpp \
	InstanceQueryProvider.cpp

include $(ROOT)/mak/dynamic-library.mak

tests:
install:
