ROOT = ../../../../..
DIR = Pegasus/Repository/tests/SharedNameSpace
include $(ROOT)/mak/config.mak
include ../libraries.mak

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = SharedNameSpace
SOURCES = SharedNameSpace.cpp

include $(ROOT)/mak/program.mak

tests:
	$(RMDIRHIER) $(TMP_DIR)/repository
	$(PROGRAM)
	$(RMDIRHIER) $(TMP_DIR)/repository

poststarttests:
