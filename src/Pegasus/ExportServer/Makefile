ROOT = ../../..
DIR = Pegasus/ExportClient
include $(ROOT)/mak/config.mak

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_EXPORT_SERVER_INTERNAL

LIBRARY = pegexportserver

LIBRARIES = \
    $(LIB_DIR)/$(LIB_PREFIX)pegcommon$(LIB_SUFFIX)

DYNAMIC_LIBRARIES = -lpegcommon

PRE_DEPEND_INCLUDES = -I./depends

SOURCES = \
    CIMExportRequestDecoder.cpp \
    CIMExportResponseEncoder.cpp \
    CIMExportRequestDispatcher.cpp

OBJECTS = $(SOURCES:.cpp=$(OBJ))

include $(ROOT)/mak/library.mak
