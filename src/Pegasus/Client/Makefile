ROOT = ../../..
DIR = Pegasus/Client
include $(ROOT)/mak/config.mak

ifdef PEGASUS_HAS_SSL
  FLAGS += -DPEGASUS_HAS_SSL -DPEGASUS_SSL_RANDOMFILE
  SYS_INCLUDES += -I$(OPENSSL_HOME)/include
endif

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_CLIENT_INTERNAL -DPEGASUS_INTERNALONLY

LIBRARY = pegclient

LIBRARIES = \
    pegcommon

PRE_DEPEND_INCLUDES = -I./depends

SOURCES = \
    CIMClient.cpp \
    CIMOperationRequestEncoder.cpp \
    CIMOperationResponseDecoder.cpp \
    ClientAuthenticator.cpp \
    CIMClientException.cpp

OBJECTS = $(SOURCES:.cpp=$(OBJ))

include $(ROOT)/mak/library.mak
