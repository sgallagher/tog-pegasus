ROOT = $(PEGASUS_ROOT)
include $(ROOT)/mak/config.mak
include $(ROOT)/test/config.mak

createSubscription:
	@wbemexec $(WBEMEXECOPTIONS) CreateFilter01.xml
	@wbemexec $(WBEMEXECOPTIONS) CreateHandler01.xml
	@wbemexec $(WBEMEXECOPTIONS) CreateFilter01.xml
	@wbemexec $(WBEMEXECOPTIONS) CreateSubscription01.xml
	@wbemexec $(WBEMEXECOPTIONS) CreateFilter02.xml
	@wbemexec $(WBEMEXECOPTIONS) CreateSubscription02.xml
	@wbemexec $(WBEMEXECOPTIONS) CreateFilter03.xml
	@wbemexec $(WBEMEXECOPTIONS) CreateSubscription03.xml

deleteSubscription:
	@wbemexec $(WBEMEXECOPTIONS) DeleteSubscription01.xml
	@wbemexec $(WBEMEXECOPTIONS) DeleteFilter01.xml
	@wbemexec $(WBEMEXECOPTIONS) DeleteSubscription02.xml
	@wbemexec $(WBEMEXECOPTIONS) DeleteFilter02.xml
	@wbemexec $(WBEMEXECOPTIONS) DeleteSubscription03.xml
	@wbemexec $(WBEMEXECOPTIONS) DeleteFilter03.xml
	@wbemexec $(WBEMEXECOPTIONS) DeleteHandler01.xml

include $(ROOT)/test/configend.mak
