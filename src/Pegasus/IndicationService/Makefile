ROOT = ../../..

DIR = Pegasus/IndicationService

include $(ROOT)/mak/config.mak

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_SERVER_INTERNAL

LIBRARY = pegindicationservice

LIBRARIES = \
    pegcommon \
    pegprovider \
    pegrepository \
    pegwql

SOURCES = \
    IndicationService.cpp

OBJECTS = $(SOURCES:.cpp=$(OBJ))

SYS_LIBS = ws2_32.lib advapi32.lib

include $(ROOT)/mak/library.mak
