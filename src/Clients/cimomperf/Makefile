ROOT = $(PEGASUS_ROOT)

PROGRAM_NAME = cimomperf
DIR = Clients/$(PROGRAM_NAME)
include $(ROOT)/mak/config.mak
include ../libraries.mak


PROGRAM = $(PROGRAM_NAME)

SOURCES = $(PROGRAM_NAME).cpp

include $(ROOT)/mak/program.mak

tests:

poststarttests:
	$(PROGRAM_NAME)
