ROOT = ../../../../..
DIR = Pegasus/Repository/tests/InheritanceTree
include $(ROOT)/mak/config.mak
include ../libraries.mak

PROGRAM = InheritanceTree
SOURCES = InheritanceTree.cpp

include $(ROOT)/mak/program.mak

tests:
	$(RM) X.idx
	$(TOUCH) X.idx
	InheritanceTree
	$(RM) X.idx

poststarttests:

