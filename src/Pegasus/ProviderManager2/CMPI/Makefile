ROOT = ../../../..

DIR = Pegasus/ProviderManager2/CMPI

include $(ROOT)/mak/config.mak

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

include $(ROOT)/mak/library.mak
