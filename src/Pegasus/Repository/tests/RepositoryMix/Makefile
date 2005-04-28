ROOT = ../../../../..
DIR = Pegasus/Repository/tests/RepositoryMix
include $(ROOT)/mak/config.mak
include ../libraries.mak

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = RepositoryMix
SOURCES = RepositoryMix.cpp

include $(ROOT)/mak/program.mak

tests:
	@ $(RMREPOSITORY) $(TMP_DIR)/repository
	$(PROGRAM) XML
	@ $(RMREPOSITORY) $(TMP_DIR)/repository
	$(PROGRAM) BIN
	@ $(RMREPOSITORY) $(TMP_DIR)/repository

poststarttests:

