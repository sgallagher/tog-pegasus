ROOT = $(PEGASUS_ROOT)

PEGASUS_ZOS_PROGRAM_OBJECT = yes

LOCAL_DEFINES = -DUSE_CONNECTLOCAL

DIR = Providers/ManagedSystem/DNSService/tests/DNSTestClient

include $(ROOT)/mak/config.mak
include $(ROOT)/src/Clients/libraries.mak

PROGRAM = DNSTestClient
SOURCES = DNSTestClient.cpp

include $(ROOT)/mak/program.mak

tests:

poststarttests:
	$(PROGRAM)
