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
ifeq ($(PEGASUS_PLATFORM),ZOS_ZSERIES_IBM)
	@chtag -tc IBM-1047 ./getErrorElement.xml
	@chtag -tc IBM-1047 ./getInstanceElement.xml
	@chtag -tc IBM-1047 ./getInstanceElement2.xml
endif
	$(PROGRAM)

poststarttests:
