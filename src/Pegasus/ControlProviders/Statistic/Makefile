ROOT = ../../../..

DIR = Pegasus/ControlProviders/statistic

include $(ROOT)/mak/config.mak

LIBRARY = CIM_CIMOMStatDataProvider

ifeq ($(PEGASUS_PLATFORM),WIN32_IX86_MSVC)
SYS_LIBS += ws2_32.lib
endif


LIBRARIES = \
	pegcommon \
	pegprovider \
	pegclient

SOURCES = \
	CIMOMStatDataProvider.cpp \
	CIMOMStatDataProviderMain.cpp

include $(ROOT)/mak/library.mak
