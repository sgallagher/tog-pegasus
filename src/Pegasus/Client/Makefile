ROOT = ../../..
DIR = Pegasus/Client
include $(ROOT)/mak/config.mak

EXTRA_INCLUDES = $(SYS_INCLUDES) $(ACE_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_CLIENT_INTERNAL

LIBRARY = pegclient

LIBRARIES = \
    $(LIB_DIR)/$(LIB_PREFIX)pegcommon$(LIB_SUFFIX) \
    $(LIB_DIR)/$(LIB_PREFIX)pegrepository$(LIB_SUFFIX) \
    $(LIB_DIR)/$(LIB_PREFIX)pegprotocol$(LIB_SUFFIX) \
    $(ACE_LIB)

PRE_DEPEND_INCLUDES = -I./depends

SOURCES = \
    Client.cpp \
    ClientException.cpp \

OBJECTS = $(SOURCES:.cpp=$(OBJ))

include $(ROOT)/mak/library.mak
