ROOT = ../../../../../../

include $(ROOT)/mak/config.mak
include $(ROOT)/test/config.mak

XMLSCRIPTS =  \
	CreateFilter01 CreateHandler01 CreateSubscription01 \
	DeleteSubscription01 DeleteFilter01 DeleteHandler01 \
	CreateFilter02 CreateHandler02 CreateSubscription02 \
	DeleteSubscription02 DeleteFilter02 DeleteHandler02 \
	CreateFilter03 CreateHandler03 CreateSubscription03 \
	DeleteSubscription03 DeleteFilter03 DeleteHandler03 


LOGFILES =   TestCMPIIndication

removeclasses:
	@wbemexec $(WBEMEXECOPTIONS) DeleteSubscription01.xml
	@wbemexec $(WBEMEXECOPTIONS) DeleteFilter01.xml
	@wbemexec $(WBEMEXECOPTIONS) DeleteHandler01.xml
	@wbemexec $(WBEMEXECOPTIONS) DeleteSubscription02.xml
	@wbemexec $(WBEMEXECOPTIONS) DeleteFilter02.xml
	@wbemexec $(WBEMEXECOPTIONS) DeleteHandler02.xml
	@wbemexec $(WBEMEXECOPTIONS) DeleteSubscription03.xml
	@wbemexec $(WBEMEXECOPTIONS) DeleteFilter03.xml
	@wbemexec $(WBEMEXECOPTIONS) DeleteHandler03.xml

# I am redefining the poststarttests here.
LOGREQUESTS = $(foreach i, $(LOGFILES), $i.loggood)
LOGRESPONSES = $(LOGREQUESTS:.loggood=.logresp)

%.logresp: %.loggood
	@ $(MOVE) $*.log $*.logresp
	@ $(DIFF) $*.loggood $(TMP_DIR)/$*.logresp
	@ $(RM) $(TMP_DIR)/$*.logresp
	@ $(ECHO) +++ $* passed successfully +++

clean:
	$(RM) $(XMLRESPONSES)
	$(RM) $(LOGRESPONSES)

depend:

tests:

ifdef PEGASUS_PLATFORM_LINUX_GENERIC_GNU
	@ ./change_hostname
endif
	
poststarttests: $(XMLRESPONSES) $(LOGRESPONSES)

general:

