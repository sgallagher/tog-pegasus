ROOT = $(PEGASUS_ROOT)
DIR = Executor/tests/LocalAuth

include $(ROOT)/mak/config.mak

PROGRAM = TestExecutorLocalAuth
SOURCES = TestExecutorLocalAuth.c
EXTRA_INCLUDES += -I../../..

LIBRARIES += pegexecutor

include ../common.mak
include $(ROOT)/mak/program.mak

tests:
	$(PROGRAM)

poststarttests:
