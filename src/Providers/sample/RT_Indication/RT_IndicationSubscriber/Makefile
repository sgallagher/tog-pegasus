ROOT = $(PEGASUS_HOME)/..
include $(ROOT)/mak/config.mak
include $(ROOT)/test/config.mak

createSubscription:
	@wbemexec $(WBEMEXECOPTIONS) CreateFilter01.xml
	@wbemexec $(WBEMEXECOPTIONS) CreateHandler01.xml
	@wbemexec $(WBEMEXECOPTIONS) CreateSubscription01.xml

deleteSubscription:
	@wbemexec $(WBEMEXECOPTIONS) DeleteSubscription01.xml
	@wbemexec $(WBEMEXECOPTIONS) DeleteFilter01.xml
	@wbemexec $(WBEMEXECOPTIONS) DeleteHandler01.xml

include $(ROOT)/test/configend.mak
