ROOT = $(PEGASUS_ROOT)

PEGASUS_ZOS_PROGRAM_OBJECT = yes

DIR = Clients/ntpinfo
include $(ROOT)/mak/config.mak
include $(ROOT)/src/Clients/libraries.mak

PROGRAM = ntpinfo
SOURCES = NTPInfo.cpp

include $(ROOT)/mak/program.mak

tests:

poststarttests:
	$(PROGRAM)
