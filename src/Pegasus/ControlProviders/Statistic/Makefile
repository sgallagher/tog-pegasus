ROOT = $(PEGASUS_ROOT)

MODULE_NAME=CIMOMStatDataProvider

DIR = Pegasus/ControlProviders/Statistic

include $(ROOT)/mak/config.mak

LOCAL_DEFINES = -DPEGASUS_STATISTICPROVIDER_INTERNAL -DPEGASUS_INTERNALONLY

LIBRARY = $(MODULE_NAME)

LIBRARIES = \
	pegcommon \
	pegprovider

	
#	pegclient

SOURCES = $(MODULE_NAME).cpp

ifeq ($(PEGASUS_SUPPORTS_DYNLIB),yes)
NEEDS_LIBRARIES = -L$(LIB_DIR) -lpegcommon -lpeguser -lpegprovider
endif

ifeq ($(PEGASUS_PLATFORM),WIN32_IX86_MSVC)
SYS_LIBS = ole32.lib ws2_32.lib advapi32.lib
endif


include $(ROOT)/mak/library.mak
include $(ROOT)/mak/install.mak
