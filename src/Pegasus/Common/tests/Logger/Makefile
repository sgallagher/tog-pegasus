ROOT = ../../../../..
DIR = Pegasus/Common/tests/Logger
include $(ROOT)/mak/config.mak
include ../libraries.mak

PROGRAM = Logger
SOURCES = Logger.cpp

include $(ROOT)/mak/program.mak

tests:
	$(RM) logs/*.log
	$(PROGRAM)
	$(RM) logs/*.log

# $(RM) logs/*
