ROOT = ../../..

DIR = Pegasus/Server

include $(ROOT)/mak/config.mak

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_SERVER_INTERNAL

LIBRARY = pegserver

LIBRARIES = \
    $(LIB_DIR)/$(LIB_PREFIX)pegcommon$(LIB_SUFFIX) \
    $(LIB_DIR)/$(LIB_PREFIX)pegrepository$(LIB_SUFFIX) \
    $(LIB_DIR)/$(LIB_PREFIX)pegprotocol$(LIB_SUFFIX) \
    $(LIB_DIR)/$(LIB_PREFIX)pegprovider$(LIB_SUFFIX) \
    $(LIB_DIR)/$(LIB_PREFIX)pegprovider2$(LIB_SUFFIX) \
    $(LIB_DIR)/$(LIB_PREFIX)pegexportserver$(LIB_SUFFIX)

ifeq ($(PEGASUS_PLATFORM),ZOS_ZSERIES_IBM)
DYNAMIC_LIBRARIES = \
    $(LIB_DIR)/$(LIB_PREFIX)pegcommon.x \
    $(LIB_DIR)/$(LIB_PREFIX)pegrepository.x \
    $(LIB_DIR)/$(LIB_PREFIX)pegprotocol.x \
    $(LIB_DIR)/$(LIB_PREFIX)pegprovider.x \
    $(LIB_DIR)/$(LIB_PREFIX)pegprovider2.x \
    $(LIB_DIR)/$(LIB_PREFIX)pegexportserver.x
else
DYNAMIC_LIBRARIES = \
    -lpegcommon \
    -lpegrepository \
    -lpegprotocol \
    -lpegprovider \
    -lpegprovider2 \
    -lpegexportserver
endif

PRE_DEPEND_INCLUDES = -I./depends

SOURCES = \
    CIMOperationRequestDecoder.cpp \
    CIMOperationResponseEncoder.cpp \
    CIMOperationRequestDispatcher.cpp \
    ProviderManager.cpp \
    ProviderModule.cpp \
    CIMServer.cpp

OBJECTS = $(SOURCES:.cpp=$(OBJ))

SYS_LIBS = ws2_32.lib advapi32.lib

include $(ROOT)/mak/library.mak

run:
	Server $(REPOSITORY_ROOT)
