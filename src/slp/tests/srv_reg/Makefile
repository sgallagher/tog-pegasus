ROOT = ../../../..
DIR = Pegasus/../slp/tests/srvreg
include $(ROOT)/mak/config.mak
include ../libraries.mak

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = srv_reg
SOURCES = srv_reg.cpp

include $(ROOT)/mak/program.mak

tests:
	$(PROGRAM)

poststarttests:

