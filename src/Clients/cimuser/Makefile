ROOT = ../../..

DIR = Pegasus/Clients/cimconfig

include $(ROOT)/mak/config.mak

LIBRARIES = \
    pegcommon \
    pegconfig \
    peggetoopt \
    pegclient \
    pegcliutils

EXTRA_INCLUDES = $(SYS_INCLUDES)

PROGRAM = cimuser

SOURCES = CIMUserCommand.cpp

include $(ROOT)/mak/program.mak

tests:

install:
