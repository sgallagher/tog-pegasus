ROOT = $(PEGASUS_ROOT)

PROGRAM_NAME = CLI

DIR = Clients/CLITestClients/$(PROGRAM_NAME)
include $(ROOT)/mak/config.mak
include ../../libraries.mak

LIBRARIES += pegCLIClientLib

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

EXTRA_INCLUDES = $(SYS_INCLUDES)

PROGRAM = $(PROGRAM_NAME)

SOURCES = $(PROGRAM_NAME).cpp

include $(ROOT)/mak/program.mak

tests:

poststarttests: 


install:
                        