ROOT = ../../../../..
DIR = Pegasus/Common/tests/Connector
include $(ROOT)/mak/config.mak
include ../libraries.mak

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = TestConnector
SOURCES = Connector.cpp

include $(ROOT)/mak/program.mak

tests:
	$(PROGRAM)
