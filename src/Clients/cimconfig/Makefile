ROOT = ../../..

PEGASUS_ZOS_PROGRAM_OBJECT = yes

DIR = Clients/cimconfig

include $(ROOT)/mak/config.mak

LIBRARIES = \
    pegcliutils \
    pegclient \
    peggetoopt \
    pegconfig \
    pegcommon \

EXTRA_INCLUDES = $(SYS_INCLUDES)

PROGRAM = cimconfig

SOURCES = CIMConfigCommand.cpp

include $(ROOT)/mak/program.mak

tests:

poststarttests: 

install:
	$(INSTALL_SBIN_PROGRAM)
