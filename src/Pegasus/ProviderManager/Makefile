include $(PEGASUS_ROOT)/mak/config.mak

DIR = Pegasus/ProviderManager

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_SERVER_INTERNAL -DPEGASUS_INTERNALONLY

LIBRARY = pegprovidermanager

LIBRARIES = \
    pegcommon \
    pegprovider \
    pegprm \
	pegconfig

SOURCES = \
	MutexLock.cpp \
	Lockable.cpp \
	SafeQueue.cpp \
	ProviderManagerService.cpp \
	ProviderManager.cpp \
	ProviderAdapter.cpp \
    ProviderModule.cpp \
    ProviderFacade.cpp \
	Provider.cpp \
	OperationResponseHandler.cpp

OBJECTS = $(SOURCES:.cpp=$(OBJ))

SYS_LIBS = ws2_32.lib advapi32.lib

include $(PEGASUS_ROOT)/mak/library.mak
