ROOT = ../../..
DIR = Pegasus/ManagedClient
include $(ROOT)/mak/config.mak

ifdef PEGASUS_HAS_SSL
  FLAGS += -DPEGASUS_HAS_SSL -DPEGASUS_SSL_RANDOMFILE
  SYS_INCLUDES += -I$(OPENSSL_HOME)/include
endif

ifdef PEGASUS_ENABLE_SLP
  FLAGS += -DPEGASUS_SLP_CLIENT_INTERFACE_WRAPPER
endif

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_CLIENT_INTERNAL -DPEGASUS_INTERNALONLY

LIBRARY = pegmanagedclient

LIBRARIES = \
    pegcommon

ifdef PEGASUS_ENABLE_SLP
ifneq ($(PEGASUS_PLATFORM),WIN32_IX86_MSVC)
LIBRARIES += pegslp_client
endif
endif

PRE_DEPEND_INCLUDES = -I./depends

SOURCES = \
	../Client/CIMClientRep.cpp \
    CIMManagedClient.cpp \
    ../Client/CIMOperationRequestEncoder.cpp \
    ../Client/CIMOperationResponseDecoder.cpp \
    ../Client/ClientAuthenticator.cpp \
    ../Client/CIMClientException.cpp \
    CIMClientConnection.cpp \
    CIMDefaultClientConnectionManager.cpp    

SOURCES_SLP = \
    ../Client/Attribute.cpp \
    ../Client/CIMServerDescription.cpp \
    ../Client/CIMServerDiscovery.cpp

ifdef PEGASUS_ENABLE_SLP
  SOURCES += $(SOURCES_SLP)
endif

OBJECTS = $(SOURCES:.cpp=$(OBJ))

include $(ROOT)/mak/library.mak

ifeq ($(PEGASUS_PLATFORM),WIN32_IX86_MSVC)
SYS_LIBS = ws2_32.lib advapi32.lib
endif

ifdef PEGASUS_ENABLE_SLP
$(SOURCES): slp_client
slp_client :
	@ $(MAKE) -SC $(ROOT)/src/slp/slp_client
ifeq ($(PEGASUS_PLATFORM),WIN32_IX86_MSVC)
SYS_LIBS += $(PEGASUS_HOME)/lib/pegslp_client.lib
endif
endif

include $(ROOT)/mak/install.mak
