ROOT = ../../../../..
DIR = Pegasus/Common/tests/AtomicInt
include $(ROOT)/mak/config.mak
include ../libraries.mak

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = TestAtomicInt
SOURCES = AtomicInt.cpp

include $(ROOT)/mak/program.mak

tests:
	$(PROGRAM)

poststarttests:
