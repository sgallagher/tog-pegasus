ROOT = $(PEGASUS_ROOT)
DIR = Executor/tests/SessionKey

include $(ROOT)/mak/config.mak

PROGRAM = TestExecutorSessionKey
SOURCES = TestExecutorSessionKey.c
EXTRA_INCLUDES += -I../../..

LIBRARIES += pegexecutor

include ../common.mak
include $(ROOT)/mak/program.mak

tests:
	$(PROGRAM)

poststarttests:
