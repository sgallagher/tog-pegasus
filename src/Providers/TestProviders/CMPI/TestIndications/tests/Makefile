ROOT = ../../../../../../

include $(ROOT)/mak/config.mak
include $(ROOT)/test/config.mak

XMLSCRIPTS =  \
	CreateFilter01 CreateHandler01 CreateSubscription01 \
	DeleteSubscription01 DeleteFilter01 DeleteHandler01

LOGFILES =   TestCMPIIndication

removeclasses:
	@wbemexec $(WBEMEXECOPTIONS) DeleteSubscription01.xml
	@wbemexec $(WBEMEXECOPTIONS) DeleteFilter01.xml
	@wbemexec $(WBEMEXECOPTIONS) DeleteHandler01.xml

# I am redefining the poststarttests here.
LOGREQUESTS = $(foreach i, $(LOGFILES), $i.loggood)
LOGRESPONSES = $(LOGREQUESTS:.loggood=.logresp)

%.logresp: %.loggood
	@ $(DIFF) $*.loggood $(TMP_DIR)/$*.log
	@ $(RM) $(TMP_DIR)/$*.log
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

