include $(PEGASUS_ROOT)/mak/config.mak

include ../libraries.mak

DIR = Pegasus/Common/tests/Thread

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = TestOperationContext

SOURCES = TestOperationContext.cpp

include $(ROOT)/mak/program.mak

tests:
	$(PROGRAM)

poststarttests:
