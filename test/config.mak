include $(ROOT)/mak/test.mak

HOSTNAME =
PORT = -p 5988
HTTPMETHOD = -m M-POST
HTTPVERSION = -v 1.1
USER = -u guest
PASSWORD = -w guest
SSL =

ifeq ($(PEGASUS_PLATFORM), WIN32_IX86_MSVC)
  DIFF = mu compare
else
  DIFF = diff
endif

XMLREQUESTS = $(foreach i, $(XMLSCRIPTS), $i.xml)
XMLRESPONSES = $(XMLREQUESTS:.xml=.rsp)

WBEMEXECOPTIONS = $(HOSTNAME) $(PORT) $(HTTPMETHOD) $(HTTPVERSION) $(USER) $(PASSWORD) $(SSL) 

%.rsp: %.xml
	@ wbemexec $(WBEMEXECOPTIONS) $*.xml > $*.rsp | cd .
	@ $(DIFF) $*rspgood.xml $*.rsp
	@ $(RM) $*.rsp
	@ echo +++ $* passed successfully +++
