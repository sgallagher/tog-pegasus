ROOT = ../../../../..
DIR = Pegasus/Common/tests/XmlPrint
include $(ROOT)/mak/config.mak
include ../libraries.mak

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = TestXmlPrint

SOURCES = XmlPrint.cpp

include $(ROOT)/mak/program.mak

tests:
	$(PROGRAM) x.xml

poststarttests:
