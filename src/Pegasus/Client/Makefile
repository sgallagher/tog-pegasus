ROOT = ../../..
DIR = Pegasus/Client
include $(ROOT)/mak/config.mak

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_CLIENT_INTERNAL

LIBRARY = pegclient

LIBRARIES = \
    $(LIB_DIR)/$(LIB_PREFIX)pegcommon$(LIB_SUFFIX) \
    $(LIB_DIR)/$(LIB_PREFIX)pegrepository$(LIB_SUFFIX)

ifeq ($(PEGASUS_PLATFORM),ZOS_ZSERIES_IBM)
DYNAMIC_LIBRARIES = \
    $(LIB_DIR)/$(LIB_PREFIX)pegcommon.x \
    $(LIB_DIR)/$(LIB_PREFIX)pegrepository.x
else
DYNAMIC_LIBRARIES = \
    -lpegcommon \
    -lpegrepository
endif

PRE_DEPEND_INCLUDES = -I./depends

SOURCES = \
    CIMClient.cpp \
    CIMOperationRequestEncoder.cpp \
    CIMOperationResponseDecoder.cpp \
    ClientAuthenticator.cpp

OBJECTS = $(SOURCES:.cpp=$(OBJ))

include $(ROOT)/mak/library.mak
