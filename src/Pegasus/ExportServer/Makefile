ROOT = ../../..
DIR = Pegasus/ExportServer
include $(ROOT)/mak/config.mak

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_EXPORT_SERVER_INTERNAL

LIBRARY = pegexportserver

LIBRARIES = \
    pegcommon \
    pegconfig \
    pegprovider \
    pegrepository


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
