ROOT = ../../..

DIR = Clients/cimconfig

include $(ROOT)/mak/config.mak

LIBRARIES = \
    pegcommon \
    pegconfig \
    peggetoopt \
    pegclient \
    pegcliutils

EXTRA_INCLUDES = $(SYS_INCLUDES)

PROGRAM = cimconfig

SOURCES = CIMConfigCommand.cpp

include $(ROOT)/mak/program.mak

tests:

install:
