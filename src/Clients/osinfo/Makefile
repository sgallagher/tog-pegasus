ROOT = $(PEGASUS_ROOT)

PEGASUS_ZOS_PROGRAM_OBJECT = yes

DIR = Clients/osinfo
include $(ROOT)/mak/config.mak

LIBRARIES = \
    pegcliutils \
    peggetoopt \
    pegclient \
    pegrepository \
    pegconfig \
    pegcommon

PROGRAM = osinfo
SOURCES = OSInfoException.cpp OSInfo.cpp

include $(ROOT)/mak/program.mak
include $(ROOT)/mak/install.mak

tests:

poststarttests:
	$(PROGRAM)
