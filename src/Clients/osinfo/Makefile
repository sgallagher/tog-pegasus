ROOT = $(PEGASUS_ROOT)

PEGASUS_ZOS_PROGRAM_OBJECT = yes

DIR = Clients/osinfo
include $(ROOT)/mak/config.mak
include $(ROOT)/src/Clients/libraries.mak

PROGRAM = osinfo
SOURCES = OSInfo.cpp
FLAGS = -g

include $(ROOT)/mak/program.mak

tests:

poststarttests:
	$(PROGRAM)
