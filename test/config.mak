HOSTNAME =
PORT = -p 5988
HTTPMETHOD = -m M-POST
HTTPVERSION = -v 1.1
USR = -u guest
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
	@ wbemexec $(WBEMEXECOPTIONS) $*.xml > $*.rsp
	@ $(DIFF) $*.rsp $*rspgood.xml
	@ $(RM) $*.rsp
	@ echo +++ $* passed successfully +++
