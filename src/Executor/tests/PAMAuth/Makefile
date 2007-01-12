ROOT = $(PEGASUS_ROOT)
DIR = Executor/tests/PAMAuth

include $(ROOT)/mak/config.mak

PROGRAM = TestExecutorPAMAuth
SOURCES = TestExecutorPAMAuth.c
EXTRA_INCLUDES += -I../../..

LIBRARIES += pegexecutor

include ../common.mak
include $(ROOT)/mak/program.mak

tests:

poststarttests:
