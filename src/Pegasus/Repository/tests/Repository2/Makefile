ROOT = ../../../../..
DIR = Pegasus/Repository/tests/Repository2
include $(ROOT)/mak/config.mak
include ../libraries.mak

PROGRAM = Repository2
SOURCES = Repository2.cpp

include $(ROOT)/mak/program.mak

tests:
	$(RMREPOSITORY) repository
	$(PROGRAM)
	$(RMREPOSITORY) repository

poststarttests:

