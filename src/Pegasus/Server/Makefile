ROOT = ../../..

DIR = Pegasus/Server

include $(ROOT)/mak/config.mak

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_SERVER_INTERNAL

LIBRARY = pegserver

LIBRARIES = \
    pegcommon \
    pegrepository \
    pegprovider \
    pegexportserver \
    pegconfig \
    peguser \
    peglocalauth

PRE_DEPEND_INCLUDES = -I./depends

SOURCES = \
    CIMOperationRequestDecoder.cpp \
    CIMOperationResponseEncoder.cpp \
    CIMOperationRequestDispatcher.cpp \
    CIMOperationRequestAuthorizer.cpp \
    HTTPAuthenticatorDelegator.cpp \
    ProviderManager.cpp \
    ProviderModule.cpp \
	CIMBaseProviderFacade.cpp \
    ServiceCIMOMHandle.cpp \
    ProviderBlockedEntry.cpp \
    ShutdownService.cpp \
    CIMServer.cpp \
    CIMServerState.cpp

OBJECTS = $(SOURCES:.cpp=$(OBJ))

SYS_LIBS = ws2_32.lib advapi32.lib

include $(ROOT)/mak/library.mak

run:
	Server $(REPOSITORY_ROOT)
