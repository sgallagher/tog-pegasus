ROOT = ../../../../..
DIR = Pegasus/Repository/tests/QualifierDeclRep
include $(ROOT)/mak/config.mak
include ../libraries.mak

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = QualifierDeclRep
SOURCES = QualifierDeclRep.cpp

include $(ROOT)/mak/program.mak

tests:
	$(RMDIRHIER) $(TMP_DIR)/repository
	$(PROGRAM)
	$(RMDIRHIER) $(TMP_DIR)/repository

poststarttests:

