ROOT = $(PEGASUS_ROOT)

PEGASUS_ZOS_PROGRAM_OBJECT = yes

DIR = Providers/ManagedSystem/DNSService/tests/testclient

include $(ROOT)/mak/config.mak
include ../../../libraries.mak

PROGRAM = DNSTestClient
SOURCES = DNSTestClient.cpp

include $(ROOT)/mak/program.mak

tests:

poststarttests:
	$(PROGRAM)
