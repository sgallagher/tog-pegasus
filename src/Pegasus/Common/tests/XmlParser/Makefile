ROOT = ../../../../..
DIR = Pegasus/Common/tests/XmlParser
include $(ROOT)/mak/config.mak
include ../libraries.mak

PROGRAM = TestXmlParser
SOURCES = XmlParser.cpp

include $(ROOT)/mak/program.mak

tests:
	@ $(PROGRAM) x.xml
	@ echo +++++ passed all tests

poststarttests:

