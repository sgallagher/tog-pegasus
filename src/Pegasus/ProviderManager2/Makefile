include $(PEGASUS_ROOT)/mak/config.mak

DIR = Pegasus/ProviderManager

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_SERVER_INTERNAL -DPEGASUS_INTERNALONLY -DENABLE_DEFAULT_PROVIDER_MANAGER -DENABLE_CMPI_PROVIDER_MANAGER

LIBRARY = pegprovidermanager

LIBRARIES = \
    pegcommon \
	pegprm

SOURCES = \
	ProviderManagerService.cpp \
	ProviderManagerModule.cpp \
	\
	ProviderManager.cpp \
	ProviderRegistrar.cpp \
	InternalProviderName.cpp \
	\
	SafeQueue.cpp \
	MutexLock.cpp \
	Lockable.cpp

OBJECTS = $(SOURCES:.cpp=$(OBJ))

SYS_LIBS = ws2_32.lib advapi32.lib

include $(PEGASUS_ROOT)/mak/library.mak
