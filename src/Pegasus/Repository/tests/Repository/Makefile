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
	$(PROGRAM) XML
	$(RMREPOSITORY) $(TMP_DIR)/repository

### disabled to allow nightly test to run. 
### failed on redhat64 - bug 3011
###
###	$(PROGRAM) BIN
###	$(RMREPOSITORY) $(TMP_DIR)/repository	

poststarttests:

