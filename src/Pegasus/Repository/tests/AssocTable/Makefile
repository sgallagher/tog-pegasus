ROOT = ../../../../..
DIR = Pegasus/Repository/AssocTable
include $(ROOT)/mak/config.mak
include ../libraries.mak

PROGRAM = AssocTable
SOURCES = AssocTable.cpp

include $(ROOT)/mak/program.mak

tests:
	$(RM) ./associations.tbl
	AssocTable
