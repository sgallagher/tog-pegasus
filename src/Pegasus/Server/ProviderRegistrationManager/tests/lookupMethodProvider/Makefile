ROOT = ../../../../../..
DIR = Pegasus/Server/ProviderRegistrationManager/tests/lookupMethodProvider
include $(ROOT)/mak/config.mak
include ../libraries.mak

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = lookupMethodProvider

SOURCES = lookupMethodProvider.cpp 

include $(ROOT)/mak/program.mak

tests:
	@$(MAKE) -f../load/Makefile localrep
	@$(PROGRAM)
	$(RMREPOSITORY) $(TMP_DIR)/repository

poststarttests:
