ROOT = ../../../../..
DIR = Pegasus/Repository/tests/Repository2
include $(ROOT)/mak/config.mak
include ../libraries.mak

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = Repository2
SOURCES = Repository2.cpp

include $(ROOT)/mak/program.mak


### bug 3011 disabled Binary Repository testing on LINUX_IA64 to allow
### the nightly tests to run

ifeq ($(PEGASUS_PLATFORM),LINUX_IA64_GNU) 
tests: testxml
else
tests: testxml testbin
endif
	$(RMREPOSITORY) $(TMP_DIR)/repository
	@ $(ECHO) $(PROGRAM) +++++ passed all tests

testxml:
	$(RMREPOSITORY) $(TMP_DIR)/repository
	$(PROGRAM) XML

testbin:
	$(RMREPOSITORY) $(TMP_DIR)/repository	
	$(PROGRAM) BIN

poststarttests:

