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
	CIMOMStatDataProvider.cpp 

ifeq ($(PEGASUS_SUPPORTS_DYNLIB),yes)
 NEEDS_LIBRARIES = -L$(LIB_DIR) -lpegcommon -lpeguser -lpegprovider
endif



include $(ROOT)/mak/library.mak
include $(ROOT)/mak/install.mak
