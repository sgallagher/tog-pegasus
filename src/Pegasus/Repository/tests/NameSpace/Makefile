ROOT = ../../../../..
DIR = Pegasus/Repository/NameSpace
include $(ROOT)/mak/config.mak
include ../libraries.mak

PROGRAM = NameSpace
SOURCES = NameSpace.cpp

include $(ROOT)/mak/program.mak

tests:
	$(RMDIRHIER) repository
	$(PROGRAM)
	$(RMDIRHIER) repository

poststarttests:

