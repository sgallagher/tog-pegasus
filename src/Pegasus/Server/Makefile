ROOT = ../../..

DIR = Pegasus/Server

include $(ROOT)/mak/config.mak

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_SERVER_INTERNAL -DPEGASUS_INTERNALONLY -DPEGASUS_DEBUG_MEMORY

LIBRARY = pegserver

LIBRARIES = \
    suballoc \
    pegcommon \
    pegrepository \
    pegprovider \
    pegprovidermanager \
    pegexportserver \
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
    ProviderMessageFacade.cpp

OBJECTS = $(SOURCES:.cpp=$(OBJ))

SYS_LIBS = ws2_32.lib advapi32.lib

include $(ROOT)/mak/library.mak

run:
	Server $(REPOSITORY_ROOT)
