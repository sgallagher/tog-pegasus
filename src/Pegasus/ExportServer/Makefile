ROOT = ../../..
DIR = Pegasus/ExportServer
include $(ROOT)/mak/config.mak

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_EXPORT_SERVER_INTERNAL -DPEGASUS_INTERNALONLY

LIBRARY = pegexportserver

LIBRARIES = \
    pegcommon \
    pegconfig \
    pegprovider


PRE_DEPEND_INCLUDES = -I./depends

SOURCES = \
    CIMExportRequestDecoder.cpp \
    CIMExportResponseEncoder.cpp \
    CIMExportRequestDispatcher.cpp

OBJECTS = $(SOURCES:.cpp=$(OBJ))

include $(ROOT)/mak/library.mak
