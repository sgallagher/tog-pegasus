ROOT = ../../../../..
DIR = Pegasus/Repository/Inst
include $(ROOT)/mak/config.mak
include ../libraries.mak

PROGRAM = Inst
SOURCES = Inst.cpp

include $(ROOT)/mak/program.mak

tests:
	$(COPY) X.in X.idx
	Inst
