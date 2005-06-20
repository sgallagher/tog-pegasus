ROOT = ../../../../..
DIR = Pegasus/Common/tests/IndicationFormatter
include $(ROOT)/mak/config.mak
include ../libraries.mak

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = TestIndicationFormatter
SOURCES = IndicationFormatter.cpp

include $(ROOT)/mak/program.mak

tests:
	$(PROGRAM)

poststarttests:
