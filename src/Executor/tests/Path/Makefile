ROOT = $(PEGASUS_ROOT)
DIR = Executor/tests/Path

include $(ROOT)/mak/config.mak

PROGRAM = TestExecutorPath
SOURCES = TestExecutorPath.c
EXTRA_INCLUDES += -I../../..

LIBRARIES += pegexecutor

include ../common.mak
include $(ROOT)/mak/program.mak

tests:
	$(PROGRAM)

poststarttests:
