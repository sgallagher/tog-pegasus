# Makes the dynamic load libs for the files named in DIR
ROOT = $(PEGASUS_ROOT)

MODULE_NAME=InteropProvider

DIR = Pegasus/ControlProviders/$(MODULE_NAME)

include $(ROOT)/mak/config.mak

LOCAL_DEFINES = -DPEGASUS_INTEROPPROVIDER_INTERNAL -DPEGASUS_INTERNALONLY

LIBRARY = $(MODULE_NAME)

LIBRARIES = \
    pegcommon \
    pegconfig \
    pegprovider

SOURCES = $(MODULE_NAME).cpp Guid.cpp

ifeq ($(PEGASUS_SUPPORTS_DYNLIB),yes)
NEEDS_LIBRARIES = -L$(LIB_DIR) -lpegcommon -lpegconfig -lpegprovider
endif

ifeq ($(PEGASUS_PLATFORM),WIN32_IX86_MSVC)
SYS_LIBS = ole32.lib ws2_32.lib advapi32.lib
endif


include $(ROOT)/mak/library.mak
