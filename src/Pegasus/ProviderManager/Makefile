ROOT = ../../..

DIR = Pegasus/ProviderManager

include $(ROOT)/mak/config.mak

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_SERVER_INTERNAL

LIBRARY = pegprovidermanager

LIBRARIES = \
    pegcommon \
    pegprovider \
    pegprm \
	pegconfig

SOURCES = \
	Lockable.cpp \
	SafeQueue.cpp \
	ProviderManagerService.cpp \
	ProviderManager.cpp \
    ProviderModule.cpp \
    ProviderFacade.cpp \
	Provider.cpp \
	IndicationResponseHandler.cpp

OBJECTS = $(SOURCES:.cpp=$(OBJ))

SYS_LIBS = ws2_32.lib advapi32.lib

include $(ROOT)/mak/library.mak
