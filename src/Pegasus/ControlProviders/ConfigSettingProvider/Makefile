# Makes the dynamic load libs for the files named in DIR
ROOT = ../../../..

DIR = Pegasus/ControlProviders/ConfigSettingProvider

include $(ROOT)/mak/config.mak

LOCAL_DEFINES = -DPEGASUS_CONFIGSETTINGPROVIDER_INTERNAL -DPEGASUS_INTERNALONLY

LIBRARY = ConfigSettingProvider

LIBRARIES = \
    pegcommon \
    pegconfig \
    pegprovider

SOURCES = ConfigSettingProvider.cpp

ifeq ($(PEGASUS_SUPPORTS_DYNLIB),yes)
NEEDS_LIBRARIES = -L$(LIB_DIR) -lpegcommon -lpegconfig -lpegprovider
endif

include $(ROOT)/mak/library.mak
