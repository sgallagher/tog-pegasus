ROOT = ../../../../..
DIR = Pegasus/Common/tests/XmlParser
include $(ROOT)/mak/config.mak
include ../libraries.mak

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = TestXmlParser
SOURCES = XmlParser.cpp

include $(ROOT)/mak/program.mak

tests:
	@ $(PROGRAM) x.xml
	@ $(ECHO) +++++ passed all tests

poststarttests:
install:
