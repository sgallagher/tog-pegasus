ROOT = ../../../../..
DIR = Pegasus/CompareRepositories/tests/CompareRepositories
include $(ROOT)/mak/config.mak
include ../libraries.mak

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY
LOCAL_CLEAN_TARGET=local-clean

PROGRAM = CompareRepositories
SOURCES = CompareRepositories.cpp

include $(ROOT)/mak/program.mak

tests:
	$(RMREPOSITORY) $(TMP_DIR)/repository
	$(PROGRAM)
	$(RMREPOSITORY) $(TMP_DIR)/repository

poststarttests:

install:


local-clean:
	rm -rf file1 file2
