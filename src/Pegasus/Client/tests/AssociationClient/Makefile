ROOT = $(PEGASUS_ROOT)
DIR = Pegasus/Client/tests/AssociationClient
include $(ROOT)/mak/config.mak
include ../libraries.mak

EXTRA_INCLUDES = $(SYS_INCLUDES)

PROGRAM = AssociationClient

SOURCES = AssociationClient.cpp

include $(ROOT)/mak/program.mak

tests:

poststarttests:	
	$(PROGRAM)

install:
