ROOT = ../../../../..

DIR = Pegasus/Common/tests/Tracer

include $(ROOT)/mak/config.mak

include ../libraries.mak

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = TracerTest

SOURCES = Tracer.cpp

include $(ROOT)/mak/program.mak

tests:
	$(PROGRAM)
	$(RM) *.log
	$(RM) *.trace

poststarttests:

