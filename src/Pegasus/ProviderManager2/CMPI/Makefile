include $(PEGASUS_ROOT)/mak/config.mak

DIR = Pegasus/ProviderManager2/CMPI

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_SERVER_INTERNAL -DPEGASUS_INTERNALONLY

LIBRARY = CMPIProviderManager

LIBRARIES = \
    pegcommon \
	pegprovider \
	pegprovidermanager \
	pegprm

SOURCES = \
	CMPIProviderManagerMain.cpp \
	CMPIProviderManager.cpp

OBJECTS = $(SOURCES:.cpp=$(OBJ))

SYS_LIBS = ws2_32.lib advapi32.lib

include $(PEGASUS_ROOT)/mak/library.mak
