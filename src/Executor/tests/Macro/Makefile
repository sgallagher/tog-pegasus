ROOT = $(PEGASUS_ROOT)
DIR = Executor/tests/Macro

include $(ROOT)/mak/config.mak

PROGRAM = TestExecutorMacro
SOURCES = TestExecutorMacro.c
EXTRA_INCLUDES += -I../../..

LIBRARIES += pegexecutor

include ../common.mak
include $(ROOT)/mak/program.mak

tests:
	$(PROGRAM)

poststarttests:
