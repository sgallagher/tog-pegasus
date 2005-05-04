ROOT = ../../../../..
DIR = Pegasus/Repository/tests/RepositoryMix
include $(ROOT)/mak/config.mak
include ../libraries.mak

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = RepositoryMix
SOURCES = RepositoryMix.cpp

include $(ROOT)/mak/program.mak


### bug 3011 disabled Binary Repository testing on LINUX_IA64 to allow
### the nightly tests to run

ifeq ($(PEGASUS_PLATFORM),LINUX_IA64_GNU)
tests: testxml 
else
tests: testxml testbin testbinmix testxmlmix
endif
	$(RMREPOSITORY) $(TMP_DIR)/repository
	@ $(ECHO) $(PROGRAM) +++++ passed all tests

testxml:
	$(RMREPOSITORY) $(TMP_DIR)/repository
	$(PROGRAM) XML

testbin:
	$(RMREPOSITORY) $(TMP_DIR)/repository	
	$(PROGRAM) BIN

testbinmix:
	@ $(RMREPOSITORY) $(TMP_DIR)/repository
	$(PROGRAM) BINMIX

testxmlmix:
	@ $(RMREPOSITORY) $(TMP_DIR)/repository
	$(PROGRAM) XMLMIX


poststarttests:

