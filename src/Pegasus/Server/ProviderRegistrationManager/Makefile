ROOT = ../../../..

DIR = Pegasus/Server/ProviderRegistrationManager

include $(ROOT)/mak/config.mak

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_SERVER_INTERNAL

LIBRARY = pegprm

LIBRARIES = \
    pegcommon \
    pegrepository

SOURCES = \
    ProviderRegistrationTable.cpp \
    ProviderRegistrationManager.cpp

OBJECTS = $(SOURCES:.cpp=$(OBJ))

SYS_LIBS = ws2_32.lib advapi32.lib

include $(ROOT)/mak/library.mak
