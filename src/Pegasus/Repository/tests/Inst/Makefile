ROOT = ../../../../..
DIR = Pegasus/Repository/tests/Inst
include $(ROOT)/mak/config.mak
include ../libraries.mak

PROGRAM = Inst
SOURCES = Inst.cpp

include $(ROOT)/mak/program.mak

tests:
	$(RM) X.idx
	$(RM) X.instances
	Inst
	$(RM) X.idx
	$(RM) X.instances

poststarttests:

