ROOT = ../../../..

DIR = Pegasus/Server/ProviderRegistrationManager

include $(ROOT)/mak/config.mak

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_PRM_INTERNAL -DPEGASUS_INTERNALONLY

LIBRARY = pegprm

LIBRARIES = \
    pegcommon \
    pegprovider \
    pegrepository

SOURCES = \
    ProviderRegistrationTable.cpp \
    ProviderRegistrationManager.cpp

OBJECTS = $(SOURCES:.cpp=$(OBJ))

ifeq ($(PEGASUS_PLATFORM),WIN32_IX86_MSVC)
  SYS_LIBS = ws2_32.lib advapi32.lib
endif

include $(ROOT)/mak/library.mak
