ROOT = ../../../../..
DIR = Pegasus/Repository/tests/RepositoryMix
include $(ROOT)/mak/config.mak
include ../libraries.mak

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = RepositoryMix
SOURCES = RepositoryMix.cpp

include $(ROOT)/mak/program.mak

tests: testxml testbin testbinmix testxmlmix
	@ $(RMREPOSITORY) $(TMP_DIR)/repository
	@ $ECHO) $(PROGRAM) +++++ all test passed

testxml:
	@ $(RMREPOSITORY) $(TMP_DIR)/repository
	$(PROGRAM) XML

testbin:
	@ $(RMREPOSITORY) $(TMP_DIR)/repository
	$(PROGRAM) BIN

testbinmix:
	@ $(RMREPOSITORY) $(TMP_DIR)/repository
	$(PROGRAM) BINMIX

testxmlmix:
	@ $(RMREPOSITORY) $(TMP_DIR)/repository
	$(PROGRAM) XMLMIX


poststarttests:

