ROOT = $(PEGASUS_ROOT)

PEGASUS_ZOS_PROGRAM_OBJECT = yes

DIR = Providers/ManagedSystem/NISServerService/testclient

include $(ROOT)/mak/config.mak
include $(ROOT)/src/Clients/libraries.mak

PROGRAM = NISTestClient
SOURCES = NISTestClient.cpp

include $(ROOT)/mak/program.mak

tests:

poststarttests:
	$(PROGRAM)

