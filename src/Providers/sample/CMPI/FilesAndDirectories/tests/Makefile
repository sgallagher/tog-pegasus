ROOT = $(PEGASUS_ROOT)

include $(ROOT)/mak/config.mak
include $(ROOT)/test/config.mak

XMLSCRIPTS =  \
   EnumerateDirNames10000 \
   EnumerateDir10001

include $(ROOT)/test/configend.mak

poststarttests:

	@echo +++++ CWS_Directory Testing FilesAndDirectories provider

	@wbemexec $(WBEMEXECOPTIONS) EnumerateDirNames10000.xml
	@wbemexec $(WBEMEXECOPTIONS) EnumerateDir10001.xml
	@wbemexec $(WBEMEXECOPTIONS) GetDirInstance10002.xml
	@wbemexec $(WBEMEXECOPTIONS) DeleteDirInstance10003.xml
	@wbemexec $(WBEMEXECOPTIONS) AssocDirNames12001.xml
	@wbemexec $(WBEMEXECOPTIONS) ReferenceDirNames12002.xml

	@wbemexec $(WBEMEXECOPTIONS) PlainFileMethodCall11001.xml

