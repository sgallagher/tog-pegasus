ROOT = ../../../../..
DIR = Pegasus/Repository/ClassDeclRep
include $(ROOT)/mak/config.mak
include ../libraries.mak

PROGRAM = ClassDeclRep
SOURCES = ClassDeclRep.cpp
POST_LIBRARIES = -L$(ROOT)/src/PegasusRepository -lPegasusRepository

include $(ROOT)/mak/program.mak

tests:
	$(RMDIRHIER) repository
	$(PROGRAM)
	$(RMDIRHIER) repository

poststarttests:

