ROOT = ../../../..

DIR = Providers/sample/InstanceQueryProvider

include $(ROOT)/mak/config.mak

LIBRARY = SampleInstanceQueryProvider

LIBRARIES = \
    pegprovider \
    pegqueryexpression \
    pegcql \
    pegquerycommon \
    pegwql \
    pegclient \
    pegconfig \
    pegcommon
	
SOURCES = \
	InstanceQueryProviderMain.cpp \
	InstanceQueryProvider.cpp

include $(ROOT)/mak/dynamic-library.mak

tests:
install:
