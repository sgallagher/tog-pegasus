ROOT = ../../../../..

DIR = Pegasus/ProviderManager2/tests/ProviderManager

include $(ROOT)/mak/config.mak

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

ifeq ($(OS_TYPE),vms)
 VMS_VECTOR = PegasusCreateProviderManager
endif

LIBRARY = TestProviderManager

LIBRARIES = \
    pegprovider \
    pegprovidermanager \
    pegclient \
    pegconfig \
    pegwql \
    pegcommon

SOURCES = \
	TestProviderManagerMain.cpp \
	TestProviderManager.cpp

include $(ROOT)/mak/dynamic-library.mak

install:
