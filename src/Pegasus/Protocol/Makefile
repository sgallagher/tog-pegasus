ROOT=../../..

DIR = Pegasus/Protocol

include $(ROOT)/mak/config.mak

EXTRA_INCLUDES = $(SYS_INCLUDES) $(ACE_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_PROTOCOL_INTERNAL 

LIBRARIES = $(ACE_LIB) $(LIB_DIR)/$(LIB_PREFIX)pegcommon$(LIB_SUFFIX)

LIBRARY = pegprotocol

PRE_DEPEND_INCLUDES = -I./depends

SOURCES = Handler.cpp 

OBJECTS = $(SOURCES:.cpp=$(OBJ))

include $(ROOT)/mak/library.mak
