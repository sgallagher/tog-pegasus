ROOT = ../../../../..
DIR = Pegasus/Repository/tests/Repository
include $(ROOT)/mak/config.mak
include ../libraries.mak

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = Repository
SOURCES = Repository.cpp

include $(ROOT)/mak/program.mak

tests:
	$(RMREPOSITORY) $(TMP_DIR)/repository
	$(PROGRAM)
	$(RMREPOSITORY) $(TMP_DIR)/repository

poststarttests:

