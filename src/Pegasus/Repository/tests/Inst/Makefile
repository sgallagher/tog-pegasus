ROOT = ../../../../..
DIR = Pegasus/Repository/tests/Inst
include $(ROOT)/mak/config.mak
include ../libraries.mak

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = Inst
SOURCES = Inst.cpp

include $(ROOT)/mak/program.mak

tests:
	$(RM) $(TMP_DIR)/X.idx
	$(RM) $(TMP_DIR)/X.instances
	Inst
	$(RM) $(TMP_DIR)/X.idx
	$(RM) $(TMP_DIR)/X.instances

poststarttests:

