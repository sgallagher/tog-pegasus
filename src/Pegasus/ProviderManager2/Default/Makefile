include $(PEGASUS_ROOT)/mak/config.mak

DIR = Pegasus/ProviderManager2/Default

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

OBJECTS = $(SOURCES:.cpp=$(OBJ))

SYS_LIBS = ws2_32.lib advapi32.lib

include $(PEGASUS_ROOT)/mak/library.mak
