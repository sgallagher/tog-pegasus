ROOT = ../../../../..
DIR = Pegasus/Compiler/tests/CompAssoc
include $(ROOT)/mak/config.mak
include ../libraries.mak

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = CompAssoc
SOURCES = CompAssoc.cpp

include $(ROOT)/mak/program.mak

MOF_PATH = $(ROOT)/Schemas/CIM25
REPOSITORYDIR = $(TMP_DIR)

tests:
	@ $(RMDIRHIER) $(REPOSITORYDIR)/repository
	@ cimmofl "-R$(REPOSITORYDIR)" "-M$(REPOSITORY_MODE)" "-I$(MOF_PATH)" -nroot $(MOF_PATH)/CIM_Core25.mof
	@ cimmofl "-R$(REPOSITORYDIR)" "-M$(REPOSITORY_MODE)" -nroot CompAssoc.mof
	@ $(PROGRAM)
	@ $(RMDIRHIER) $(REPOSITORYDIR)/repository

poststarttests:
