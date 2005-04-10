ROOT = ../../../../..

DIR = Pegasus/WQL/tests/Parser

LIBRARIES = pegwql pegcommon pegquerycommon

include $(ROOT)/mak/config.mak

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = TestParser
SOURCES = Parser.cpp

include $(ROOT)/mak/program.mak

tests:
	$(PROGRAM)

poststarttests:
