ROOT = ../../../../..
DIR = Pegasus/Repository/QualifierDeclRep
include $(ROOT)/mak/config.mak
include ../libraries.mak

PROGRAM = QualifierDeclRep
SOURCES = QualifierDeclRep.cpp

include $(ROOT)/mak/program.mak

tests:
	$(RMDIRHIER) repository
	$(PROGRAM)
	$(RMDIRHIER) repository

poststarttests:

