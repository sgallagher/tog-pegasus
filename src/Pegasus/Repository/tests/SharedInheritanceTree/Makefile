ROOT = ../../../../..
DIR = Pegasus/Repository/tests/SharedInheritanceTree
include $(ROOT)/mak/config.mak
include ../libraries.mak

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = SharedInheritanceTree
SOURCES = SharedInheritanceTree.cpp

include $(ROOT)/mak/program.mak

tests:
	$(RMDIRHIER) $(TMP_DIR)/repository
	$(PROGRAM)
	$(RMDIRHIER) $(TMP_DIR)/repository

poststarttests:

