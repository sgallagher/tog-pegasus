ROOT = ../../../../..
DIR = Pegasus/Repository2/Repository2
include $(ROOT)/mak/config.mak
include ../libraries.mak

PROGRAM = Repository2
SOURCES = Repository2.cpp

include $(ROOT)/mak/program.mak

tests:
	$(RMDIRHIER) repository
	$(PROGRAM)
	$(RMDIRHIER) repository
