ROOT = ../../..

DIR = Clients/cliutils

include $(ROOT)/mak/config.mak

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_CLIUTILS_INTERNAL

LIBRARY = pegcliutils

LIBRARIES = \
    $(LIB_DIR)/$(LIB_PREFIX)pegcommon$(LIB_SUFFIX) \

ifeq ($(PEGASUS_PLATFORM),ZOS_ZSERIES_IBM)
DYNAMIC_LIBRARIES = \
    $(LIB_DIR)/$(LIB_PREFIX)pegcommon.x
else
DYNAMIC_LIBRARIES = -lpegcommon
endif

PRE_DEPEND_INCLUDES = -I./depends

SOURCES = \
    Command.cpp \
    CommandException.cpp

OBJECTS = $(SOURCES:.cpp=$(OBJ))

include $(ROOT)/mak/library.mak
