ROOT = ../../../..

DIR = Pegasus/ProviderManager2/Default

include $(ROOT)/mak/config.mak

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_SERVER_INTERNAL -DPEGASUS_INTERNALONLY

LIBRARY = DefaultProviderManager

LIBRARIES = \
    pegcommon \
	pegprovider \
	pegprovidermanager \
	pegprm

SOURCES = \
	DefaultProviderManagerMain.cpp \
	DefaultProviderManager.cpp \
	LocalProviderManager.cpp \
	Provider.cpp \
	ProviderFacade.cpp \
	ProviderModule.cpp \
	OperationResponseHandler.cpp \
	SimpleResponseHandler.cpp

include $(PEGASUS_ROOT)/mak/library.mak
