ROOT = ../../../../../..
DIR = Pegasus/Server/ProviderRegistrationManager/tests/createInstances
include $(ROOT)/mak/config.mak
include ../libraries.mak

EXTRA_INCLUDES = $(SYS_INCLUDES)

PROGRAM = createInstances

SOURCES = createInstances.cpp

include $(ROOT)/mak/program.mak

tests:
	@$(MAKE) -f../load/Makefile localrep
	@$(PROGRAM)

poststarttests:
