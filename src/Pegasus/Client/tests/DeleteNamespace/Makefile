ROOT = ../../../../..
DIR = Pegasus/Client/tests/DeleteNamespace
include $(ROOT)/mak/config.mak

ifdef PEGASUS_HAS_SSL
  FLAGS += -DPEGASUS_HAS_SSL -DPEGASUS_SSL_RANDOMFILE
  SYS_INCLUDES += -I$(OPENSSL_HOME)/include
endif

include ../libraries.mak

EXTRA_INCLUDES = $(SYS_INCLUDES)

PROGRAM = DeleteNamespace

SOURCES = DeleteNamespace.cpp

include $(ROOT)/mak/program.mak

tests:

clean:

poststarttests: 
	$(PROGRAM)
