ROOT = ../../../../..
DIR = Pegasus/Repository/tests/ClassDeclRep
include $(ROOT)/mak/config.mak
include ../libraries.mak

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = ClassDeclRep
SOURCES = ClassDeclRep.cpp
POST_LIBRARIES = -L$(ROOT)/src/PegasusRepository -lPegasusRepository

include $(ROOT)/mak/program.mak

tests:
	$(RMDIRHIER) $(TMP_DIR)/repository
	$(PROGRAM)
	$(RMDIRHIER) $(TMP_DIR)/repository

poststarttests:

