ROOT = ../../../../..
DIR = Pegasus/Repository/EnumerateClassNames
include $(ROOT)/mak/config.mak
include ../libraries.mak

PROGRAM = EnumerateClassNames
SOURCES = EnumerateClassNames.cpp

include $(ROOT)/mak/program.mak

tests:
	$(RMDIRHIER) repository
	$(PROGRAM)
	$(RMDIRHIER) repository

poststarttests:

