ROOT = ../../..

PEGASUS_ZOS_PROGRAM_OBJECT = yes

DIR = Clients/cimauth

include $(ROOT)/mak/config.mak

LIBRARIES = \
    pegcliutils \
    pegclient \
    peggetoopt \
    pegconfig \
    pegcommon

EXTRA_INCLUDES = $(SYS_INCLUDES)

PROGRAM = cimauth

SOURCES = CIMAuthCommand.cpp

include $(ROOT)/mak/program.mak

tests:

poststarttests: 

