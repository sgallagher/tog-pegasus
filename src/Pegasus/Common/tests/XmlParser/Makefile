ROOT = ../../../../..
DIR = Pegasus/Common/tests/XmlParser
include $(ROOT)/mak/config.mak
include ../libraries.mak

PROGRAM = XmlParser
SOURCES = XmlParser.cpp

include $(ROOT)/mak/program.mak

tests:
	@ XmlParser $(ROOT)/doc/CIM_Schema24.xml
	@ echo +++++ passed all tests
