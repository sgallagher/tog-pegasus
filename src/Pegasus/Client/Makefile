ROOT = ../../..
DIR = Pegasus/Client
include $(ROOT)/mak/config.mak

ifdef PEGASUS_HAS_SSL
  FLAGS += -DPEGASUS_HAS_SSL -DPEGASUS_SSL_RANDOMFILE
  SYS_INCLUDES += -I$(OPENSSL_HOME)/include
endif

ifdef PEGASUS_ENABLE_SLP
  FLAGS += -DOPENSLP_SLP_CLIENT_INTERFACE_WRAPPER
ifndef OPENSLP_HOME
  ERROR = openslp_home_undefined
openslp_home_undefined:
	@ echo OPENSLP_HOME environment variable undefined
	@ exit 1
endif
  SYS_INCLUDES += -I$(OPENSLP_HOME)/libslp
endif

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_CLIENT_INTERNAL -DPEGASUS_INTERNALONLY

LIBRARY = pegclient

LIBRARIES = \
    pegcommon

PRE_DEPEND_INCLUDES = -I./depends

SOURCES = \
	CIMClientRep.cpp \
    CIMClient.cpp \
    CIMOperationRequestEncoder.cpp \
    CIMOperationResponseDecoder.cpp \
    ClientAuthenticator.cpp \
    CIMClientException.cpp

SOURCES_SLP = \
    Attribute.cpp \
    CIMServerDescription.cpp \
    CIMServerDiscovery.cpp

ifdef PEGASUS_ENABLE_SLP
  SOURCES += $(SOURCES_SLP)
endif

OBJECTS = $(SOURCES:.cpp=$(OBJ))


ifdef PEGASUS_ENABLE_SLP
ifeq ($(PEGASUS_PLATFORM),WIN32_IX86_MSVC)
SYS_LIBS += $(OPENSLP_HOME)/win32/libslp/Release/openslp.lib
endif
endif

include $(ROOT)/mak/library.mak

