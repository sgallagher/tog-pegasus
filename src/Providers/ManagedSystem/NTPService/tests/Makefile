# wbemexec tests for ManagedSystem IP provider

ROOT = ../../../../..
include $(ROOT)/mak/config.mak

HOSTNAME =
PORT = -p 5988
HTTPMETHOD = -m M-POST
HTTPVERSION = -v 1.1
USER = -u guest
PASSWORD = -w guest

ifeq ($(PEGASUS_PLATFORM), WIN32_IX86_MSVC)
   DIFF = mu compare
else
   DIFF = diff
endif

XMLREQUESTS = $(foreach i, $(XMLSCRIPTS), $i.xml)
XMLRESPONSES = $(XMLREQUESTS:.xml=.rsp)

WBEMEXECOPTIONS = $(HOSTNAME) $(PORT) $(HTTPMETHOD) $(HTTPVERSION) $(USER) $(PASSWORD)

%.rsp: %.xml
	@ wbemexec $(WBEMEXECOPTIONS) $*.xml > $(TMP_DIR)/$*.rsp | cd .
	@ $(DIFF) $*rspgood.xml $(TMP_DIR)/$*.rsp
	@ $(RM) $(TMP_DIR)/$*.rsp
	@ echo +++ $* passed successfully +++

XMLCOVREQUESTS = $(foreach i, $(XMLCOVSCRIPTS), $i.xml)
XMLCOVRESPONSES = $(XMLCOVREQUESTS:.xml=.cov)

%.cov: %.xml
	@ wbemexec $(WBEMEXECOPTIONS) $*.xml > $(TMP_DIR)/$*.cov | cd .
	@ $(RM) $(TMP_DIR)/$*.cov
	@ echo +++ $* code coverage exercise complete +++

ifeq ($(OS), HPUX)
XMLSCRIPTS = \
	PG_NTP_Fail1_CreateInstance \
	PG_NTP_Fail2_DeleteInstance \
	PG_NTP_Fail3_ModifyInstance \
	PG_NTP_Fail4_GetInstance_ShortKeys \
	PG_NTP_Fail5_GetInstance_ExtraKeys \
	PG_NTP_Fail6_GetInstance_DupKeys
XMLCOVSCRIPTS = \
	PG_NTP_EnumInst \
	PG_NTP_EnumInstNames
else
ifeq ($(PEGASUS_PLATFORM), WIN32_IX86_MSVC)
XMLSCRIPTS = 
XMLCOVSCRIPTS = 
else
ifeq ($(PEGASUS_PLATFORM), LINUX_IA64_GNU) 
XMLSCRIPTS = 
XMLCOVSCRIPTS = 
else
XMLSCRIPTS = 
XMLCOVSCRIPTS = 
endif
endif
endif

clean:
	$(RM) $(XMLRESPONSES)
	

tests: 

poststarttests: $(XMLRESPONSES) $(XMLCOVRESPONSES)

depend:

general:

strip-license:

prepend-license:
