# Makes the dynamic load libs for the files named in SOURCES
# to create the DLL named in LIBRARY

ROOT = ../../../..

include ../../libraries.mak

DIR = Providers/generic/__NamespaceProvider

include $(ROOT)/mak/config.mak

SOURCES = __NamespaceProvider.cpp

LIBRARY = __NamespaceProvider

include $(ROOT)/mak/library.mak

tests:
