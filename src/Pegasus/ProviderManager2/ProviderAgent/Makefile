ROOT = ../../../..

DIR = Pegasus/ProviderManager2/ProviderAgent

include $(ROOT)/mak/config.mak

LIBRARIES = \
    pegprovidermanager \
    pegprovider \
    pegconfig \
    pegclient \
    pegcommon

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_PROVIDERAGENT_INTERNAL -DPEGASUS_INTERNALONLY

PROGRAM = cimprovagt

SOURCES = \
        ProviderAgent.cpp \
        cimprovagt.cpp

include $(ROOT)/mak/program.mak

tests:

poststarttests:

