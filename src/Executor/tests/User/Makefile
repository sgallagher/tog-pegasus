ROOT = $(PEGASUS_ROOT)
DIR = Executor/tests/User

include $(ROOT)/mak/config.mak

PROGRAM = TestExecutorUser
SOURCES = TestExecutorUser.c
EXTRA_INCLUDES += -I../../..

LIBRARIES += pegexecutor

include ../common.mak
include $(ROOT)/mak/program.mak

tests:
	$(PROGRAM)

poststarttests:
