ROOT = $(PEGASUS_ROOT)

PEGASUS_ZOS_PROGRAM_OBJECT = yes

DIR = Clients/TestClient
include $(ROOT)/mak/config.mak

ifdef PEGASUS_HAS_SSL
   FLAGS += -DPEGASUS_HAS_SSL -DPEGASUS_SSL_RANDOMFILE
   SYS_INCLUDES += -I$(OPENSSL_HOME)/include
endif

include ../libraries.mak

PROGRAM = TestClient
SOURCES = TestClient.cpp


include $(ROOT)/mak/program.mak

tests:

poststarttests:
	$(PROGRAM)
