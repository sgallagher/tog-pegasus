ROOT = ../../../../..
DIR = Pegasus/Common/tests/DQueue
include $(ROOT)/mak/config.mak
include ../libraries.mak

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = DQueue
SOURCES = DQueue.cpp

include $(ROOT)/mak/program.mak

tests:
	$(RM) $(TMP_DIR)/dq_memory_trace
	$(PROGRAM)
	$(RM) $(TMP_DIR)/dq_memory_trace

poststarttests:
