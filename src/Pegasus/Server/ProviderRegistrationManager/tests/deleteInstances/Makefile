ROOT = ../../../../../..
DIR = Pegasus/Server/ProviderRegistrationManager/tests/deleteInstances
include $(ROOT)/mak/config.mak
include ../libraries.mak

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = deleteInstances

SOURCES = deleteInstances.cpp

include $(ROOT)/mak/program.mak

tests:
	@$(MAKE) -f../load/Makefile localrep
	@$(PROGRAM)

poststarttests:
