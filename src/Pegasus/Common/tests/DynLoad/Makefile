ROOT = ../../../../..
DIR = Pegasus/Common/tests/DynLoad
include $(ROOT)/mak/config.mak
include ../libraries.mak

PROGRAM = TestDynLoad
SOURCES = DynLoad.cpp

include $(ROOT)/mak/program.mak

tests:
ifeq ($(OS_TYPE),windows)
	$(PROGRAM) $(HOME_DIR)/bin/DynLib.dll
else
	$(PROGRAM) $(HOME_DIR)/lib/libDynLib.so
endif
