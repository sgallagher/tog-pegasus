ROOT = ../../..

DIR = Pegasus/HandlerService
include $(ROOT)/mak/config.mak

INCLUDES = -I..

LOCAL_DEFINES = -DPEGASUS_HANDLER_SERVICE_INTERNAL -DPEGASUS_INTERNALONLY

LIBRARIES = \
    pegcommon \
    pegconfig \
    pegrepository

LIBRARY = peghandlerservice

SOURCES = \
    IndicationHandlerService.cpp \
    HandlerTable.cpp

include $(ROOT)/mak/library.mak
