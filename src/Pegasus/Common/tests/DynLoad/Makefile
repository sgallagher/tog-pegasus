ROOT = ../../../../..
DIR = Pegasus/Common/tests/DynLoad
include $(ROOT)/mak/config.mak
include ../libraries.mak

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = TestDynLoad
SOURCES = DynLoad.cpp

include $(ROOT)/mak/program.mak

tests:
ifeq ($(OS_TYPE),windows)
	$(PROGRAM) $(HOME_DIR)/bin/TestDynLib.dll
else
 ifeq ($(PEGASUS_OS_TYPE),VMS)
	$(PROGRAM) libTestDynLib
 else
	$(PROGRAM) $(HOME_DIR)/lib/libTestDynLib$(LIB_SUFFIX)
 endif
endif

poststarttests:
