ROOT = ../../..

DIR = Pegasus/ProviderManager2

include $(ROOT)/mak/config.mak

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

include $(ROOT)/mak/library.mak
