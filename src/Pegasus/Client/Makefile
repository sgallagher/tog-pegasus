ROOT = ../../..
DIR = Pegasus/Client
include $(ROOT)/mak/config.mak

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_CLIENT_INTERNAL

LIBRARY = pegclient

LIBRARIES = \
    pegcommon \
    pegrepository

PRE_DEPEND_INCLUDES = -I./depends

SOURCES = \
    CIMClient.cpp \
    CIMOperationRequestEncoder.cpp \
    CIMOperationResponseDecoder.cpp \
    ClientAuthenticator.cpp

OBJECTS = $(SOURCES:.cpp=$(OBJ))

include $(ROOT)/mak/library.mak
