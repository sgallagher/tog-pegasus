ROOT = ../../../../..
DIR = Pegasus/Common/tests/IPC
include $(ROOT)/mak/config.mak
include ../libraries.mak

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = IPC
SOURCES = IPC.cpp

include $(ROOT)/mak/program.mak

tests:
	$(PROGRAM)

poststarttests:
