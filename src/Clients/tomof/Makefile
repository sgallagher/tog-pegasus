ROOT = $(PEGASUS_ROOT)

PROGRAM_NAME = tomof
DIR = Clients/$(PROGRAM_NAME)
include $(ROOT)/mak/config.mak
include ../libraries.mak

PROGRAM = $(PROGRAM_NAME)

SOURCES = $(PROGRAM_NAME).cpp \
	  clientRepositoryInterface.cpp

include $(ROOT)/mak/program.mak

tests:

poststarttests:
	$(PROGRAM)
