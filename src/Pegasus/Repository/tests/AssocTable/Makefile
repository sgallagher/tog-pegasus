ROOT = ../../../../..
DIR = Pegasus/Repository/tests/AssocTable
include $(ROOT)/mak/config.mak
include ../libraries.mak

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = AssocTable
SOURCES = AssocTable.cpp

include $(ROOT)/mak/program.mak

tests:
	$(RM) ./associations.tbl
	AssocTable

poststarttests:

