ROOT = ../../..

DIR = Pegasus/Server

OBJECT_DIRS = \
	ConfigurationManager

include $(ROOT)/mak/config.mak

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_SERVER_INTERNAL

LIBRARY = pegserver

LIBRARIES = \
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
    UserAuthProvider

PRE_DEPEND_INCLUDES = -I./depends

SOURCES = \
    CIMOperationRequestDecoder.cpp \
    CIMOperationResponseEncoder.cpp \
    CIMOperationRequestDispatcher.cpp \
    CIMOperationRequestAuthorizer.cpp \
    HTTPAuthenticatorDelegator.cpp \
    ShutdownService.cpp \
    CIMServer.cpp \
    CIMServerState.cpp \
    ServiceCIMOMHandle.cpp \
    ProviderMessageFacade.cpp \
    ConfigurationManager/ConfigurationManagerQueue.cpp \
    ConfigurationManager/PG_RegistrationProvider.cpp

OBJECTS = $(SOURCES:.cpp=$(OBJ))

SYS_LIBS = ws2_32.lib advapi32.lib

include $(ROOT)/mak/library.mak

run:
	Server $(REPOSITORY_ROOT)
