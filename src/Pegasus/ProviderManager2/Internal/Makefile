ROOT = ../../../..

DIR = Pegasus/ProviderManager2/Internal

include $(ROOT)/mak/config.mak

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_PPM_INTERNAL -DPEGASUS_INTERNALONLY

ifeq ($(OS_TYPE),vms)
 VMS_VECTOR = PegasusCreateProviderManager
endif

LIBRARY = InternalProviderManager

LIBRARIES = \
    pegprovider \
    pegprovidermanager \
    pegprm \
    pegclient \
    pegconfig \
    pegwql \
    pegcommon

SOURCES = \
	InternalProviderManagerMain.cpp \
	InternalProviderManager.cpp

include $(ROOT)/mak/dynamic-library.mak
