ROOT = $(PEGASUS_ROOT)

PROGRAM_NAME = EnumerateInstances
DIR = Clients/CLITestClients/$(PROGRAM_NAME)
include $(ROOT)/mak/config.mak
include ../../libraries.mak

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = $(PROGRAM_NAME)

SOURCES = $(PROGRAM_NAME).cpp

include $(ROOT)/mak/program.mak

tests:

poststarttests:
