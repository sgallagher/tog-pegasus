ROOT = ../../..

DIR = Pegasus/ProviderManager2

include $(ROOT)/mak/config.mak

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_PPM_INTERNAL -DPEGASUS_INTERNALONLY -DENABLE_DEFAULT_PROVIDER_MANAGER -DENABLE_CMPI_PROVIDER_MANAGER

LIBRARY = pegprovidermanager

LIBRARIES = \
    pegcommon \
	pegprm \
        pegconfig 

SOURCES = \
	ProviderManagerService.cpp \
	ProviderManagerModule.cpp \
	\
	ProviderManager.cpp \
	ProviderRegistrar.cpp \
	ProviderName.cpp \
	ProviderType.cpp \
	\
	SafeQueue.cpp \
	Lockable.cpp \
	OperationResponseHandler.cpp \
	SimpleResponseHandler.cpp

include $(ROOT)/mak/library.mak
