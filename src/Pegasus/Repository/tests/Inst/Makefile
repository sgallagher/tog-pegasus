ROOT = ../../../../..
DIR = Pegasus/Repository/Inst
include $(ROOT)/mak/config.mak
include ../libraries.mak

PROGRAM = Inst
SOURCES = Inst.cpp

include $(ROOT)/mak/program.mak

tests:
	$(RM) X.idx
	Inst
	$(RM) X.idx
