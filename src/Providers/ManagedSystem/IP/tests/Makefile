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

ifeq ($(OS), HPUX)
XMLSCRIPTS = 
else
ifeq ($(PEGASUS_PLATFORM), LINUX_GENERIC_GNU) 
XMLSCRIPTS = 
else
ifeq ($(PEGASUS_PLATFORM), LINUX_IA64_GNU) 
XMLSCRIPTS = 
else
XMLSCRIPTS = 
endif
endif
endif

clean:
	$(RM) $(XMLRESPONSES)
	

tests: 

poststarttests: $(XMLRESPONSES)

depend:

general:

strip-license:

prepend-license:
