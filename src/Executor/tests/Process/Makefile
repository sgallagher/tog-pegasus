ROOT = $(PEGASUS_ROOT)
DIR = Executor/tests/Process

include $(ROOT)/mak/config.mak

PROGRAM = TestExProcess
SOURCES = TestExecutorProcess.c
EXTRA_INCLUDES += -I../../..

LIBRARIES += pegexecutor

include ../common.mak
include $(ROOT)/mak/program.mak

tests:
	$(PROGRAM)

poststarttests:
