ROOT = ../../../../../..
DIR = Pegasus/Server/ProviderRegistrationManager/tests/lookupIndicationProvider
include $(ROOT)/mak/config.mak
include ../libraries.mak

EXTRA_INCLUDES = $(SYS_INCLUDES)

PROGRAM = lookupIndicationProvider

SOURCES = lookupIndicationProvider.cpp 

include $(ROOT)/mak/program.mak

tests:
	@$(MAKE) -f../load/Makefile localrep
	@$(PROGRAM)

poststarttests:
