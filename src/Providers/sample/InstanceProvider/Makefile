ROOT = ../../../..

DIR = Providers/sample/InstanceProvider

include $(ROOT)/mak/config.mak

LIBRARY = SampleInstanceProvider

LIBRARIES = \
    pegprovider \
    pegclient \
    pegcommon
	
SOURCES = \
	InstanceProviderMain.cpp \
	InstanceProvider.cpp

include $(ROOT)/mak/dynamic-library.mak

tests:
