ROOT = ../../..

DIR = Pegasus/IndicationService

include $(ROOT)/mak/config.mak

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_SERVER_INTERNAL -DPEGASUS_INTERNALONLY

LIBRARY = pegindicationservice

LIBRARIES = \
    pegcommon \
    pegprovider \
    pegrepository \
    pegprm \
    pegwql \
    pegconfig

SOURCES = \
    IndicationOperationAggregate.cpp \
    IndicationService.cpp

OBJECTS = $(SOURCES:.cpp=$(OBJ))

SYS_LIBS = ws2_32.lib advapi32.lib

include $(ROOT)/mak/library.mak
