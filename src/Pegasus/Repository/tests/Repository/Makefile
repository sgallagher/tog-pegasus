ROOT = ../../../../..
DIR = Pegasus/Repository/Repository
include $(ROOT)/mak/config.mak
include ../libraries.mak

PROGRAM = Repository
SOURCES = Repository.cpp

include $(ROOT)/mak/program.mak

tests:
	$(RMDIRHIER) repository
	$(PROGRAM)
	$(RMDIRHIER) repository
