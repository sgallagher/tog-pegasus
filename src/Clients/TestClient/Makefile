ROOT = $(PEGASUS_ROOT)

PEGASUS_ZOS_PROGRAM_OBJECT = yes

DIR = Clients/TestClient
include $(ROOT)/mak/config.mak
include ../libraries.mak

PROGRAM = TestClient
SOURCES = TestClient.cpp


include $(ROOT)/mak/program.mak

tests:

poststarttests:
	$(PROGRAM)
