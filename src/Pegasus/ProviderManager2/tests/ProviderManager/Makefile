ROOT = ../../../../..

DIR = Pegasus/ProviderManager2/tests/ProviderManager

include $(ROOT)/mak/config.mak

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

LIBRARY = TestProviderManager

LIBRARIES = \
    pegcommon \
	pegprovider \
	pegprovidermanager

SOURCES = \
	TestProviderManagerMain.cpp \
	TestProviderManager.cpp

include $(ROOT)/mak/dynamic-library.mak

install:

