ROOT = ../../../..

DIR = Providers/TestProviders/DefaultInstanceProvider

include $(ROOT)/mak/config.mak

LIBRARY = PG_DefaultInstanceProvider

LIBRARIES = \
	pegcommon \
	pegprovider \
	pegclient \
	pegrepository

SOURCES = \
   DefaultInstanceProvider.cpp \
   DefaultInstanceProviderMain.cpp

include $(ROOT)/mak/dynamic-library.mak

tests:
install:
