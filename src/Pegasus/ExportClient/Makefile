ROOT = ../../..
DIR = Pegasus/Client
include $(ROOT)/mak/config.mak

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_EXPORTCLIENT_INTERNAL

LIBRARY = pegexportclient

LIBRARIES = \
    $(LIB_DIR)/$(LIB_PREFIX)pegcommon$(LIB_SUFFIX) \
    $(LIB_DIR)/$(LIB_PREFIX)pegrepository$(LIB_SUFFIX) \
    $(LIB_DIR)/$(LIB_PREFIX)pegprotocol$(LIB_SUFFIX) 

DYNAMIC_LIBRARIES = \
    -lpegcommon \
    -lpegrepository \
    -lpegprotocol

PRE_DEPEND_INCLUDES = -I./depends

SOURCES = \
    ExportClientException.cpp \
    CIMExportClient.cpp 

OBJECTS = $(SOURCES:.cpp=$(OBJ))

include $(ROOT)/mak/library.mak
