ROOT = $(PEGASUS_ROOT)

PEGASUS_ZOS_PROGRAM_OBJECT = yes

DIR = Clients/osinfo
include $(ROOT)/mak/config.mak

LIBRARIES = \
    pegcommon \
    peggetoopt \
    pegclient \
    pegcliutils 

PROGRAM = osinfo
SOURCES = OSInfoException.cpp OSInfo.cpp

include $(ROOT)/mak/program.mak

tests:

poststarttests:
	$(PROGRAM)
