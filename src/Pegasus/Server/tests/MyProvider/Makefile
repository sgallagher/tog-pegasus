ROOT = ../../../../..
include ../libraries.mak

DIR = Pegasus/Provider/tests/MyProvider

include $(ROOT)/mak/config.mak

SOURCES = MyProvider.cpp

LIBRARY = MyProvider
ifeq ($(PEGASUS_SUPPORTS_DYNLIB),yes) 
NEEDS_LIBRARIES = -L$(LIB_DIR) -lpegcommon -lpegprovider
endif

include $(ROOT)/mak/library.mak

tests:
