ROOT = ../../../..

DIR = Providers/TestProviders/MCCA_TestAssocProvider

include $(ROOT)/mak/config.mak

LIBRARY = MCCATestAssocProvider

LIBRARIES = \
    pegprovider \
    pegclient \
    pegcommon
	
SOURCES = \
   MCCA_TestAssocProviderMain.cpp \
   MCCA_TestAssocProvider.cpp

include $(ROOT)/mak/dynamic-library.mak

tests:
install:
