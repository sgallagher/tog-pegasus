# Makes the dynamic load libs for the files named in DIR
ROOT = ../../../..

DIR = Pegasus/ControlProviders/UserAuthProvider

include $(ROOT)/mak/config.mak

LOCAL_DEFINES = -DPEGASUS_USERAUTHPROVIDER_INTERNAL -DPEGASUS_INTERNALONLY

LIBRARY = UserAuthProvider

LIBRARIES = \
    pegcommon \
    peguser \
    pegprovider

SOURCES = UserAuthProvider.cpp

ifeq ($(PEGASUS_SUPPORTS_DYNLIB),yes)
NEEDS_LIBRARIES = -L$(LIB_DIR) -lpegcommon -lpeguser -lpegprovider
endif

include $(ROOT)/mak/library.mak
