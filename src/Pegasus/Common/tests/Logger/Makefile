ROOT = ../../../../..
DIR = Pegasus/Common/tests/Logger
include $(ROOT)/mak/config.mak
include ../libraries.mak

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = TestLogger
SOURCES = Logger.cpp

include $(ROOT)/mak/program.mak

tests:
	$(RM) logs/*.log
	$(PROGRAM)
	$(RM) logs/*.log

poststarttests:

