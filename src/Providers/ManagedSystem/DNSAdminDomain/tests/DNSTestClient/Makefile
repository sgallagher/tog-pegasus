ROOT = $(PEGASUS_ROOT)

PEGASUS_ZOS_PROGRAM_OBJECT = yes

LOCAL_DEFINES =

DIR = Providers/ManagedSystem/DNSProvider/tests/DNSTestClient

include $(ROOT)/mak/config.mak
include $(ROOT)/src/Clients/libraries.mak

PROGRAM = DNSTestClient

POSTSTARTTESTS =
ifeq ($(OS),HPUX)
  POSTSTARTTESTS = $(PROGRAM)
endif

SOURCES = DNSTestClient.cpp

include $(ROOT)/mak/program.mak

tests:

poststarttests:
	$(POSTSTARTTESTS)
