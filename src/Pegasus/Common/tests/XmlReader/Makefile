ROOT = ../../../../..
DIR = Pegasus/Common/tests/XmlReader
include $(ROOT)/mak/config.mak
include ../libraries.mak

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = TestXmlReader

SOURCES = XmlReader.cpp

include $(ROOT)/mak/program.mak

tests:
	$(RMREPOSITORY) $(TMP_DIR)/repository
	$(PROGRAM)

poststarttests:
