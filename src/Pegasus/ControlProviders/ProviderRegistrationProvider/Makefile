# Makes the dynamic load libs for the files named in DIR
ROOT = ../../../..

DIR = Pegasus/ControlProviders/ProviderRegistrationProvider

include $(ROOT)/mak/config.mak

LOCAL_DEFINES = -DPEGASUS_PROVREGPROVIDER_INTERNAL -DPEGASUS_INTERNALONLY

ifdef ENABLE_CMPI_PROVIDER_MANAGER
   LOCAL_DEFINES += -DENABLE_CMPI_PROVIDER_MANAGER
endif

LIBRARY = ProviderRegistrationProvider

LIBRARIES = \
    pegcommon \
    pegprm \
    pegprovider

SOURCES = ProviderRegistrationProvider.cpp

ifeq ($(PEGASUS_SUPPORTS_DYNLIB),yes)
NEEDS_LIBRARIES = -L$(LIB_DIR) -lpegcommon -lpegprm -lpegprovider
endif

include $(ROOT)/mak/library.mak
