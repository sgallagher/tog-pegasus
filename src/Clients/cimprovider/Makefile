ROOT = ../../..

PEGASUS_ZOS_PROGRAM_OBJECT = yes

DIR = Clients/cimprovider

include $(ROOT)/mak/config.mak

LIBRARIES = \
    pegcommon \
    pegconfig \
    peggetoopt \
    pegclient \
    pegcliutils

EXTRA_INCLUDES = $(SYS_INCLUDES)

PROGRAM = cimprovider

SOURCES = CIMProviderCommand.cpp

include $(ROOT)/mak/program.mak

tests:

poststarttests:

install:
