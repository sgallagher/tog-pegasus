ROOT = ../../../../..
DIR = Pegasus/Common/tests/XmlDump
include $(ROOT)/mak/config.mak
include ../libraries.mak

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = TestXmlDump

SOURCES = XmlDump.cpp

include $(ROOT)/mak/program.mak

tests:
	$(PROGRAM) x.xml 

poststarttests:

