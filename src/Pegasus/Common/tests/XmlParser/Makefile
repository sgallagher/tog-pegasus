ROOT = ../../../../..
DIR = Pegasus/Common/tests/XmlParser
include $(ROOT)/mak/config.mak
include ../libraries.mak

PROGRAM = XmlParser
SOURCES = XmlParser.cpp

include $(ROOT)/mak/program.mak

tests:
	@ XmlParser x.xml
	@ echo +++++ passed all tests
