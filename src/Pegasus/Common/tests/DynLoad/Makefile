ROOT = ../../../../..
DIR = Pegasus/Common/tests/DynLoad
include $(ROOT)/mak/config.mak
include ../libraries.mak

PROGRAM = DynLoad
SOURCES = DynLoad.cpp

include $(ROOT)/mak/program.mak

tests:
	$(PROGRAM) $(PEGASUS_HOME)/lib/libDynLib.so
