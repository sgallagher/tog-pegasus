ROOT = $(PEGASUS_ROOT)

PROGRAM_NAME = cimperf
DIR = Clients/$(PROGRAM_NAME)
include $(ROOT)/mak/config.mak
include ../libraries.mak

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY


PROGRAM = $(PROGRAM_NAME)

SOURCES = $(PROGRAM_NAME).cpp

include $(ROOT)/mak/program.mak

tests:

poststarttests:
	$(PROGRAM_NAME)
