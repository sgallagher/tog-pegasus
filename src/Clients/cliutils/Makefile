ROOT = ../../..

DIR = Clients/cliutils

include $(ROOT)/mak/config.mak

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_CLIUTILS_INTERNAL

LIBRARY = pegcliutils

LIBRARIES = \
    pegcommon \

PRE_DEPEND_INCLUDES = -I./depends

SOURCES = \
    Command.cpp \
    CommandException.cpp

OBJECTS = $(SOURCES:.cpp=$(OBJ))

include $(ROOT)/mak/library.mak
