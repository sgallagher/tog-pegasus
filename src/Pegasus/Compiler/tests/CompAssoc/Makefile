ROOT = ../../../../..
DIR = Pegasus/Compiler/tests/CompAssoc
include $(ROOT)/mak/config.mak
include ../libraries.mak

PROGRAM = CompAssoc
SOURCES = CompAssoc.cpp

include $(ROOT)/mak/program.mak

MOF_PATH = $(ROOT)/Schemas/CIM25

tests:
	@ $(RMDIRHIER) ./repository
	@ cimmofl -R. -I$(MOF_PATH) -nroot $(MOF_PATH)/CIM_Core25.mof
	@ cimmofl -R. -nroot CompAssoc.mof
	@ $(PROGRAM)
	@ $(RMDIRHIER) ./repository

poststarttests:
