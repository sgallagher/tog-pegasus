ROOT = ../../..

DIR = Pegasus/Provider

include $(ROOT)/mak/config.mak

INCLUDES = -I..

LOCAL_DEFINES = -DPEGASUS_PROVIDER_INTERNAL

LIBRARIES = $(LIB_DIR)/$(LIB_PREFIX)pegcommon$(LIB_SUFFIX)

ifeq ($(PEGASUS_PLATFORM),ZOS_ZSERIES_IBM)
DYNAMIC_LIBRARIES = $(LIB_DIR)/$(LIB_PREFIX)pegcommon.x
else
 DYNAMIC_LIBRARIES = -lpegcommon
endif

LIBRARY = pegprovider

SOURCES = \
    ProviderHandle.cpp \
    CIMProvider.cpp

include $(ROOT)/mak/library.mak
