ROOT = $(PEGASUS_ROOT)

PEGASUS_ZOS_PROGRAM_OBJECT = yes

DIR = Providers/ManagedSystem/IP/testclient

include $(ROOT)/mak/config.mak

include ../../libraries.mak

PROGRAM = IPTestClient
SOURCES = IPTestClient.cpp

include $(ROOT)/mak/program.mak

tests:

poststarttests:
	$(PROGRAM)

