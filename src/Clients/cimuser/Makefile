ROOT = ../../..

PEGASUS_ZOS_PROGRAM_OBJECT = yes

DIR = Clients/cimuser

include $(ROOT)/mak/config.mak

LIBRARIES = \
    pegcliutils \
    pegclient \
    peggetoopt \
    pegconfig \
    pegcommon

EXTRA_INCLUDES = $(SYS_INCLUDES)

PROGRAM = cimuser

SOURCES = CIMUserCommand.cpp

include $(ROOT)/mak/program.mak

tests:

poststarttests: 


install:
