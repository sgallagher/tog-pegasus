ROOT = ../../../../..
DIR = Pegasus/Common/tests/XmlDump
include $(ROOT)/mak/config.mak
include ../libraries.mak

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = TestXmlDump

SOURCES = XmlDump.cpp

include $(ROOT)/mak/program.mak

tests:
ifeq ($(PEGASUS_PLATFORM),ZOS_ZSERIES_IBM)
	@chtag -tc IBM-1047 ./x.xml
endif
	$(PROGRAM) x.xml

poststarttests:
