ROOT = ../../../../..
DIR = Pegasus/CompareRepositories/tests/CompareRepositories
include $(ROOT)/mak/config.mak
include ../libraries.mak

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = CompareRepositories
SOURCES = CompareRepositories.cpp

include $(ROOT)/mak/program.mak

tests:
	$(RMREPOSITORY) $(TMP_DIR)/repository
	$(PROGRAM)
	$(RMREPOSITORY) $(TMP_DIR)/repository

poststarttests:

install:
