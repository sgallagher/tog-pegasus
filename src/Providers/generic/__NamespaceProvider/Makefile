# Makes the dynamic load libs for the files named in SOURCES
# to create the DLL named in LIBRARY

ROOT = ../../../..

include ../../libraries.mak

DIR = Providers/generic/__NamespaceProvider

include $(ROOT)/mak/config.mak

SOURCES = __NamespaceProvider.cpp

LIBRARY = __NamespaceProvider

ifeq ($(PEGASUS_SUPPORTS_DYNLIB),yes)
NEEDS_LIBRARIES = -L$(LIB_DIR) -lpegcommon -lpegprovider
endif

include $(ROOT)/mak/library.mak

tests:
