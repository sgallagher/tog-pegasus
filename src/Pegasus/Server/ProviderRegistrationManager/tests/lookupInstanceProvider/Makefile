ROOT = ../../../../../..
DIR = Pegasus/Server/ProviderRegistrationManager/tests/lookupInstanceProvider
include $(ROOT)/mak/config.mak
include ../libraries.mak

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = lookupInstanceProvider

SOURCES = lookupInstanceProvider.cpp 

include $(ROOT)/mak/program.mak

tests:
	@$(MAKE) -f../load/Makefile localrep
	@$(PROGRAM)
	$(RMREPOSITORY) $(TMP_DIR)/repository

poststarttests:
