ROOT = $(PEGASUS_ROOT)

PEGASUS_ZOS_PROGRAM_OBJECT = yes

LOCAL_DEFINES =

DIR = Providers/ManagedSystem/NTPProvider/tests/NTPTestClient

include $(ROOT)/mak/config.mak
include $(ROOT)/src/Clients/libraries.mak

POSTSTARTTESTS =
ifeq ($(OS),HPUX)
  POSTSTARTTESTS = $(PROGRAM)
endif

PROGRAM = NTPTestClient
SOURCES = NTPTestClient.cpp

include $(ROOT)/mak/program.mak

tests:

poststarttests:
	$(POSTSTARTTESTS)
