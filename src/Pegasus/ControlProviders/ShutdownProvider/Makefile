# Makes the dynamic load libs for the files named in DIR
ROOT = ../../../..

DIR = Pegasus/ControlProviders/ShutdownProvider

include $(ROOT)/mak/config.mak

LOCAL_DEFINES = -DPEGASUS_SHUTDOWNPROVIDER_INTERNAL

LIBRARY = ShutdownProvider

LIBRARIES = \
    pegcommon \
    pegprovider

SOURCES = ShutdownProvider.cpp

ifeq ($(PEGASUS_SUPPORTS_DYNLIB),yes)
NEEDS_LIBRARIES = -L$(LIB_DIR) -lpegcommon -lpegprovider
endif

include $(ROOT)/mak/library.mak
