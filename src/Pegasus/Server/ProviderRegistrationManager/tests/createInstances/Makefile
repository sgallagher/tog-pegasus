ROOT = ../../../../../..
DIR = Pegasus/Server/ProviderRegistrationManager/tests/createInstances
include $(ROOT)/mak/config.mak
include ../libraries.mak

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = createInstances

SOURCES = createInstances.cpp

include $(ROOT)/mak/program.mak

tests:
	@$(MAKE) -f../load/Makefile localrep
	@$(PROGRAM)
	$(RMREPOSITORY) $(TMP_DIR)/repository

poststarttests:
