ROOT = ../../../../..
DIR = Pegasus/Compiler/tests/CompAssoc
include $(ROOT)/mak/config.mak
include ../libraries.mak

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = CompAssoc
SOURCES = CompAssoc.cpp

include $(ROOT)/mak/program.mak

MOF_PATH = $(ROOT)/Schemas/CIM25

tests:
	@ $(RMDIRHIER) $(TMP_DIR)/repository
	@ cimmofl -R$(TMP_DIR) -I$(MOF_PATH) -nroot $(MOF_PATH)/CIM_Core25.mof
	@ cimmofl -R$(TMP_DIR) -nroot CompAssoc.mof
	@ $(PROGRAM)
	@ $(RMDIRHIER) $(TMP_DIR)/repository

poststarttests:
