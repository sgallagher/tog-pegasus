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

ifdef PEGASUS_ENABLE_REMOTE_CMPI
LOCAL_DEFINES += -DPEGASUS_ENABLE_REMOTE_CMPI
endif

ifdef PEGASUS_ENABLE_OBJECT_NORMALIZATION
LOCAL_DEFINES += -DPEGASUS_ENABLE_OBJECT_NORMALIZATION
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
	pegquerycommon \
	pegwql \
	pegclient \
	pegprm \
	DefaultProviderManager \
	ConfigSettingProvider \
	UserAuthProvider \
	ProviderRegistrationProvider\
	NamespaceProvider

ifdef PEGASUS_ENABLE_SLP
LIBRARIES += \
	InteropProvider
endif

ifndef PEGASUS_DISABLE_PERFINST
LIBRARIES += \
	CIMOMStatDataProvider \
	InteropProvider
endif

ifndef PEGASUS_DISABLE_CQL
LIBRARIES += \
	CIMQueryCapabilitiesProvider
endif

ifdef PEGASUS_HAS_SSL
LIBRARIES += \
	CertificateProvider
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
	reg_table.cpp \
	QuerySupportRouter.cpp \
	WQLOperationRequestDispatcher.cpp

OBJECTS = $(SOURCES:.cpp=$(OBJ))

SYS_LIBS = ws2_32.lib advapi32.lib

include $(ROOT)/mak/library.mak

run:
	Server $(REPOSITORY_ROOT)

include $(ROOT)/mak/install.mak
