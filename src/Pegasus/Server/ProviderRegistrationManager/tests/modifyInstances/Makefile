ROOT = ../../../../../..
DIR = Pegasus/Server/ProviderRegistrationManager/tests/modifyInstances
include $(ROOT)/mak/config.mak
include ../libraries.mak

EXTRA_INCLUDES = $(SYS_INCLUDES)

PROGRAM = modifyInstances

SOURCES = modifyInstances.cpp

include $(ROOT)/mak/program.mak

tests:
	@$(MAKE) -f../load/Makefile localrep
	@$(PROGRAM)

poststarttests:
