ROOT = ../../..

DIR = Pegasus/Clients/cimauth

include $(ROOT)/mak/config.mak

LIBRARIES = \
    pegcommon \
    pegconfig \
    peggetoopt \
    pegclient \
    pegcliutils

EXTRA_INCLUDES = $(SYS_INCLUDES)

PROGRAM = cimauth

SOURCES = CIMAuthCommand.cpp

include $(ROOT)/mak/program.mak

tests:

install:
