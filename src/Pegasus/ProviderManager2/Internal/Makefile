ROOT = ../../../..

DIR = Pegasus/ProviderManager2/Internal

include $(ROOT)/mak/config.mak

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_SERVER_INTERNAL -DPEGASUS_INTERNALONLY

LIBRARY = InternalProviderManager

LIBRARIES = \
    pegcommon \
	pegprovider \
	pegprovidermanager \
	pegprm

SOURCES = \
	InternalProviderManagerMain.cpp \
	InternalProviderManager.cpp

include $(ROOT)/mak/library.mak
