ROOT = ../../../../..

DIR = Pegasus/Common/tests/Tracer

include $(ROOT)/mak/config.mak

include ../libraries.mak

PROGRAM = TracerTest

SOURCES = Tracer.cpp

include $(ROOT)/mak/program.mak

tests:
	$(PROGRAM)
	$(RM) *.log
	$(RM) *.trace

poststarttests:

