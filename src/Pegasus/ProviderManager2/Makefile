ROOT = ../../..

DIR = Pegasus/ProviderManager2

include $(ROOT)/mak/config.mak

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_PPM_INTERNAL -DPEGASUS_INTERNALONLY -DPEGASUS_ENABLE_DEFAULT_PROVIDER_MANAGER -DPEGASUS_ENABLE_CMPI_PROVIDER_MANAGER

ifdef PEGASUS_ENABLE_JMPI_PROVIDER_MANAGER
LOCAL_DEFINES += -DPEGASUS_ENABLE_JMPI_PROVIDER_MANAGER
endif

LIBRARY = pegprovidermanager

LIBRARIES = \
    pegcommon \
	pegprm \
        pegconfig \
	pegwql

SOURCES = \
	ProviderManagerService.cpp \
	ProviderManagerRouter.cpp \
	BasicProviderManagerRouter.cpp \
        OOPProviderManagerRouter.cpp \
	ProviderManagerModule.cpp \
	\
	ProviderManager.cpp \
	ProviderRegistrar.cpp \
	ProviderName.cpp \
	ProviderType.cpp \
	QueryExpressionFactory.cpp \
	\
	SafeQueue.cpp \
	Lockable.cpp \
	OperationResponseHandler.cpp \
	SimpleResponseHandler.cpp


include $(ROOT)/mak/library.mak
include $(ROOT)/mak/install.mak
