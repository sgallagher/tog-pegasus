ROOT = ../../..
DIR = Pegasus/ExportClient
include $(ROOT)/mak/config.mak

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_EXPORT_CLIENT_INTERNAL

LIBRARY = pegexportclient

LIBRARIES = \
    pegcommon \
    pegclient

PRE_DEPEND_INCLUDES = -I./depends

SOURCES = \
    CIMExportClient.cpp \
    CIMExportRequestEncoder.cpp \
    CIMExportResponseDecoder.cpp

OBJECTS = $(SOURCES:.cpp=$(OBJ))

include $(ROOT)/mak/library.mak
