ROOT = ../../..

DIR = Pegasus/Server

include $(ROOT)/mak/config.mak

# SSL support
ifdef PEGASUS_HAS_SSL
  FLAGS += -DPEGASUS_HAS_SSL -DPEGASUS_SSL_RANDOMFILE
  SYS_INCLUDES += -I$(OPENSSL_HOME)/include
endif

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_SERVER_INTERNAL -DPEGASUS_INTERNALONLY

ifdef ENABLE_PROVIDER_MANAGER2
LOCAL_DEFINES +=  -DENABLE_PROVIDER_MANAGER2
endif

LIBRARY = pegserver

LIBRARIES = \
    pegcommon \
    pegrepository \
    pegprovider \
	pegexportserver \
    pegprovidermanager \
    pegconfig \
    peguser \
    pegauthentication \
    peghandlerservice \
    pegindicationservice \
    pegwql \
    pegprm \
    ConfigSettingProvider \
    UserAuthProvider \
    ProviderRegistrationProvider\
    NamespaceProvider

ifdef ENABLE_PROVIDER_MANAGER2
LIBRARIES += \
	DefaultProviderManager
endif

PRE_DEPEND_INCLUDES = -I./depends

SOURCES = \
    CIMOperationRequestDecoder.cpp \
    CIMOperationResponseEncoder.cpp \
    CIMOperationRequestDispatcher.cpp \
    CIMOperationRequestAuthorizer.cpp \
    HTTPAuthenticatorDelegator.cpp \
    ShutdownProvider.cpp \
    ShutdownService.cpp \
    CIMServer.cpp \
    CIMServerState.cpp \
    ProviderMessageFacade.cpp \
    BinaryMessageHandler.cpp \
    reg_table.cpp

OBJECTS = $(SOURCES:.cpp=$(OBJ))

SYS_LIBS = ws2_32.lib advapi32.lib

include $(ROOT)/mak/library.mak

run:
	Server $(REPOSITORY_ROOT)
