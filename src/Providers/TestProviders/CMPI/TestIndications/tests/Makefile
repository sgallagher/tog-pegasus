ROOT = ../../../../../../

include $(ROOT)/mak/config.mak
include $(ROOT)/test/config.mak

XMLSCRIPTS =  \
	CreateFilter01 CreateHandler01 CreateSubscription01 
#	DeleteSubscription01 DeleteFilter01 DeleteHandler01

LOGFILES =   SELECTFROMTestCMPIIndicationWHEREPropertyAPropertyAORPropertyBNothingANDss 

removeclasses:
	@wbemexec $(WBEMEXECOPTIONS) DeleteSubscription01.xml
	@wbemexec $(WBEMEXECOPTIONS) DeleteFilter01.xml
	@wbemexec $(WBEMEXECOPTIONS) DeleteHandler01.xml

# I am redefining the poststarttests here.
LOGREQUESTS = $(foreach i, $(LOGFILES), $i.log)
LOGRESPONSES = $(LOGREQUESTS:.log=.loggood)

%.loggood: %.log
	@ $(DIFF) $*.loggood $(TMP_DIR)/$*.log
	@ $(RM) $(TMP_DIR)/$*.log
	@ $(ECHO) +++ $* passed successfully +++

include $(ROOT)/test/configend.mak


poststarttests: $(LOGRESPONSES)
