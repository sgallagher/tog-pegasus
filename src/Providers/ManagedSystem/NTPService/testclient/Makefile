ROOT = $(PEGASUS_ROOT)

PEGASUS_ZOS_PROGRAM_OBJECT = yes

DIR = Providers/ManagedSystem/NTPService/testclient

include $(ROOT)/mak/config.mak
include ../../libraries.mak

PROGRAM = NTPTestClient
SOURCES = NTPTestClient.cpp

include $(ROOT)/mak/program.mak

tests:

poststarttests:
	$(PROGRAM)

