ROOT = ../../..
DIR = Pegasus/ExportServer
include $(ROOT)/mak/config.mak

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_EXPORT_SERVER_INTERNAL

LIBRARY = pegexportserver

LIBRARIES = \
    $(LIB_DIR)/$(LIB_PREFIX)pegcommon$(LIB_SUFFIX) \
    $(LIB_DIR)/$(LIB_PREFIX)pegconfig$(LIB_SUFFIX) \
    $(LIB_DIR)/$(LIB_PREFIX)pegprovider2$(LIB_SUFFIX) \
    $(LIB_DIR)/$(LIB_PREFIX)pegrepository$(LIB_SUFFIX) 


ifeq ($(PEGASUS_PLATFORM),ZOS_ZSERIES_IBM)
DYNAMIC_LIBRARIES = \
    $(LIB_DIR)/$(LIB_PREFIX)pegcommon.x
else
 DYNAMIC_LIBRARIES = -lpegcommon -lpegprovider2 -lpegrepository
endif

PRE_DEPEND_INCLUDES = -I./depends

## Mon Oct  1 16:52:33 2001 mdday 
## added provider 2 linkages below 

SOURCES = \
    CIMExportRequestDecoder.cpp \
    CIMExportResponseEncoder.cpp \
    CIMExportRequestDispatcher.cpp \
    HandlerTable.cpp \
    ConsumerTable.cpp 

OBJECTS = $(SOURCES:.cpp=$(OBJ))

include $(ROOT)/mak/library.mak
