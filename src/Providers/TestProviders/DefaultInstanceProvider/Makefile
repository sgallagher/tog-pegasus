ROOT = ../../../..

DIR = Providers/TestProviders/DefaultInstanceProvider

include $(ROOT)/mak/config.mak

LIBRARY = PG_DefaultInstanceProvider

LIBRARIES = \
    pegprovider \
    pegrepository \
    pegclient \
    pegconfig \
    pegcommon

SOURCES = \
   DefaultInstanceProvider.cpp \
   DefaultInstanceProviderMain.cpp

include $(ROOT)/mak/dynamic-library.mak

tests:
