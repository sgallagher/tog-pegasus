ROOT = ../../../../..

DIR = Pegasus/ProviderManager2/tests/ProviderManager

include $(ROOT)/mak/config.mak

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_SERVER_INTERNAL -DPEGASUS_INTERNALONLY

LIBRARY = TestProviderManager

LIBRARIES = \
    pegcommon \
	pegprovider \
	pegprovidermanager \
	pegprm

SOURCES = \
	TestProviderManagerMain.cpp \
	TestProviderManager.cpp

include $(ROOT)/mak/library.mak
