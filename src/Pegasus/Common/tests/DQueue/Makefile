ROOT = ../../../../..
DIR = Pegasus/Common/tests/DQueue
include $(ROOT)/mak/config.mak
include ../libraries.mak

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

LIBRARIES = peg_suballoc pegcommon
PROGRAM = DQueue
SOURCES = DQueue.cpp

include $(ROOT)/mak/program.mak

tests:
	$(PROGRAM)

poststarttests:
